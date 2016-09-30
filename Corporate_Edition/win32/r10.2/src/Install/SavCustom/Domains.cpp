// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright (c) 2005 Symantec Corporation. All rights reserved.
/********************************************************
	Author: Randy Templeton




	Copywrite (C) 1997 Intel Corporation
******************************************************/

#include "stdafx.h"
#include "domains.h"
#include "resource.h"
#include "Vpsetup.h"
//#include "InitSetup.h"
#include "RegKey.h"

#include <afxmt.h>
#include <time.h>

CSemaphore	g_mutex;				// thread sinc object
CPtrList* g_pDomains	= 0;	// domain list
CStringArray* g_pServers= 0;    // server list
HWND g_hList		= 0;
HWND g_hSrvList	= 0;	// stores all LDVP servers
BOOL g_TransmanComplete	= 0;	// set when Thread is complete
extern LPTM g_pTrans;
extern DWORD g_dwAbort;

// thread func
UINT DomainThread( LPVOID pParam )
{
	// Send a single ping to each server in AddressCache
	ASSERT (g_pTrans); // TMInit(), must be called first.
					   // TMInit() will initialize transman for us.
	AddAddressCacheInfo(NULL);

	g_pTrans->FindAllComputers(false);
	Relinquish();
	g_TransmanComplete = TRUE;

	return 0;
}



void StartDomains()
{
	if (g_pDomains) return;

	try
	{
		g_pDomains = new CPtrList;
		g_pServers = new CStringArray;
	}
	catch (std::bad_alloc &) {}
	DomainThread(NULL);
}

// release all memory allocated by domain list

void CleanupDomains()
{
	CSingleLock cs(&g_mutex,TRUE);
	if ( NULL != g_pDomains )
	{
		while (!g_pDomains->IsEmpty())
			delete (CDomainInfo*)g_pDomains->RemoveHead();

		delete g_pDomains;
		g_pDomains = NULL;
	}
	if ( NULL != g_pServers )
	{
		g_pServers->RemoveAll();
		delete g_pServers;
		g_pServers = NULL;
	}
}


DWORD ReturnPong(const char *ComputerName,const char *DomainName,const char *Mom,PONGDATA *pong)
{
	if (!g_pDomains || !g_pServers)
		return 0;

	LV_ITEM lvi;
	CDomainInfo *p = NULL;
	CString sComputer(ComputerName);
	CString sName(DomainName);
	CString sMom(Mom);
	int idx,found;

	sComputer.MakeUpper();
	if (sMom.IsEmpty())
		sMom = sComputer;
	else
		sMom.MakeUpper();

	CSingleLock cs(&g_mutex,TRUE);

	// add server name to the list of LDVP servers if unique
	found = FALSE;
	for (idx = g_pServers->GetSize()-1; idx>=0; idx--)
		if (sComputer == g_pServers->GetAt(idx))
			found = TRUE;

	// add new unique server to the server list
	if (!found)
	{
		if ( NULL != g_pServers )
			g_pServers->Add(sComputer);

		lvi.iItem = 32767;
		lvi.iSubItem = 0;
		lvi.mask = LVIF_TEXT | LVIF_IMAGE;
		lvi.iImage = 9;
		lvi.pszText = (LPTSTR)(LPCTSTR)sComputer;

		if ( NULL != g_hSrvList)
		{
			LV_ITEM *pLvi = NULL;
			try
			{
				pLvi = new LV_ITEM(lvi);
				pLvi->pszText = new char[MAX_PATH];
			}
			catch (std::bad_alloc &) {}
			_tcscpy(pLvi->pszText,lvi.pszText);
			ListView_InsertItem(g_hSrvList, pLvi);
		}

	}

	// search domain list for duplicate
	for( POSITION pos = g_pDomains->GetHeadPosition(); pos != NULL; )
	{
    	p = (CDomainInfo*)g_pDomains->GetNext(pos);
		if ((sName == p->m_sName) && (sMom == p->m_sParent))
			return 0;
	}

	try
	{
		// add new unique domain to the domain list
		p = new CDomainInfo(sName,sMom);
	}
	catch (std::bad_alloc &) {}

	if ( NULL != p )
		g_pDomains->AddTail(p);

	lvi.iItem = 32767;
	lvi.iSubItem = 0;
	lvi.mask = LVIF_TEXT | LVIF_IMAGE | LVIF_PARAM;
	lvi.pszText = LPSTR_TEXTCALLBACK;
	lvi.iImage = 1;
	lvi.lParam = (LPARAM)p;

	if ( NULL != g_hList)
		ListView_InsertItem(g_hList,&lvi);

	return 0;
}


void GetServers(CListCtrl* pList, BOOL bCltSupport, BOOL* pAdminOnly)
{
	CSingleLock cs(&g_mutex,TRUE);

	if (!pList || !pList->m_hWnd)
	{
		g_hSrvList=NULL;
		return;
	}
	g_hSrvList = pList->m_hWnd;

	CString sTemp, sComputer(GetComputerName());


	DWORD dwFlag;
	LV_ITEM lvi;
	lvi.iItem = 32767;
	lvi.iSubItem = 0;
	lvi.mask = LVIF_TEXT | LVIF_IMAGE;
	lvi.iImage = 9;

	// add all items in the domain list to the list control

	if ( NULL != g_hSrvList )
		ListView_DeleteAllItems(g_hSrvList);

	for (int idx = g_pServers->GetSize()-1; idx>=0; idx--)
	{
		sTemp = g_pServers->GetAt(idx);
		if (bCltSupport)
		{
			CString sPath(REGHEADER);
			sPath += "\\AddressCache\\" + sTemp;

			if (!RegKeyGetValue(HKLM,sPath,"Flags",(LPBYTE)&dwFlag,4,NULL) &&  dwFlag & PF_SUPPORT_CLIENTS)
				lvi.pszText = (LPTSTR)(LPCTSTR)sTemp;
			else
				continue;
		}
		else
			lvi.pszText = (LPTSTR)(LPCTSTR)sTemp;

		if ( NULL != g_hSrvList )
			ListView_InsertItem(g_hSrvList,&lvi);

		// Check for host in list and reset admin flag if found
		if (pAdminOnly && !sComputer.CompareNoCase(sTemp))
			*pAdminOnly = FALSE;
	}

	// add administrator to list
	if (pAdminOnly && *pAdminOnly)
	{
		sComputer.MakeUpper();
		lvi.pszText = (LPTSTR)(LPCTSTR)sComputer;
		if (*pAdminOnly & SERVER_INSTALLED)
		{
			lvi.iImage = 9;
			*pAdminOnly = FALSE;
		}
		else
			lvi.iImage = 10;

		if ( NULL != g_hSrvList )
			ListView_InsertItem(g_hSrvList,&lvi);
	}
}


void AddAddressCacheInfo(CString *pDomInfo)
{
	if (!g_pServers) return;

	TCHAR sKey[MAX_PATHS];
	CString sTemp(REGHEADER "\\AddressCache");
	HKEY regKey;
	ULONG size=MAX_PATHS;
	FILETIME ft;
	DWORD index=0;

	if (!RegOpenKeyEx(HKEY_LOCAL_MACHINE,(LPCTSTR)sTemp,0,KEY_QUERY_VALUE | KEY_ENUMERATE_SUB_KEYS,&regKey))
	{
		while (!RegEnumKeyEx(regKey,index,sKey,&size,0,0,0,&ft))
		{
			if (!pDomInfo)
			{
				SendSinglePing(sKey);
			}
			size=MAX_PATHS;
			index++;
		}
		RegCloseKey(regKey);
	}
}

// Checks to see if the selected Domain (Domain Page) is already in use
BOOL AlreadyInDomains(CString *sDomain, CString *sParent)
{
	CSingleLock cs(&g_mutex,TRUE);
	CDomainInfo *pDomain = NULL;

	if (!g_pDomains) return FALSE;

	for( POSITION pos = g_pDomains->GetHeadPosition(); pos != NULL; )
	{
    	pDomain = (CDomainInfo*)g_pDomains->GetNext(pos);
		if (!sDomain->CompareNoCase(pDomain->m_sName))
		{
			// This will ensure that we do not have the
			// same domain name is mixed case floating around.
			// eg., if we have a domain called TEST and the
			// administrator decides to create a new domain "test"
			// the name domain name will be set to "TEST"
			*sDomain = pDomain->m_sName;

			if (sParent->IsEmpty())
				*sParent = pDomain->m_sParent;
			return TRUE;
		}
	}
	return FALSE;
}

BOOL VerifyServerEngine(CString sDomain, HWND hList)
{
	CSingleLock cs(&g_mutex,TRUE);

	if (!g_pServers) return TRUE;

	CString sPath, sEngine;
	char sVendor[MAX_PATH];

	// Read Legacy File to determine which SEV Engine to use
	GetPrivateProfileString("ScanEngineOptions","Engine","NAV",sEngine.GetBuffer(SBUFFER),SBUFFER,GetModulePath() + BSLASH "Legacy.inf");
	sEngine.ReleaseBuffer();

	for (int idx = g_pServers->GetSize()-1; idx>=0; idx--)
	{
		if (hList)
		{
			LV_FINDINFO lvFind;
			lvFind.flags = LVFI_STRING;
			lvFind.psz = (LPCSTR)g_pServers->GetAt(idx);
			int item = ListView_FindItem(hList,-1,&lvFind);
			if (item != -1 && ListView_GetItemState(hList,item,LVIS_SELECTED) & LVIS_SELECTED)
				continue;
		}

		sPath = "~\\AddressCache\\" + g_pServers->GetAt(idx);
		if (!RegKeyGetValue(HKLM,sPath,"Domain",(LPBYTE)sVendor,MAX_PATH,NULL) && !sDomain.CompareNoCase(sVendor))
		{
			memset(sVendor,0,MAX_PATH);
			RegKeyGetValue(HKLM,sPath,"ScanEngineVendor",(LPBYTE)sVendor,MAX_PATH,NULL);
			if (!strlen(sVendor))
				_tcscpy(sVendor,"TRD");
			if (_tcsicmp(sVendor,sEngine))
				return FALSE;
		}
	}
	return TRUE;
}

