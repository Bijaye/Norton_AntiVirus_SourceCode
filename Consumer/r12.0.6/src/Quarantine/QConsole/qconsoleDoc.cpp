//////////////////////////////////////////////////////////////////////////////
// PROPRIETARY / CONFIDENTIAL.
// Use of this product is subject to license terms.
// Copyright © 2005 Symantec Corporation.
// All rights reserved.
//////////////////////////////////////////////////////////////////////////////
//
// qconsoleDoc.cpp : implementation of the CQconsoleDoc class
//
/////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "qconsole.h"

#define INITGUID
#include "qconsoleDoc.h"
#include "const.h"

#include "defutilsinterface.h"  // Defutils syminterface object
#include "NAVDefutilsLoader.h"

#include "qsfields.h"
#include "QItemEZ.h"
#include <math.h>
#include "AVRESBranding.h"

#include "NAVSettingsHelperEx.h"
#include "OptNames.h"
#include "navopt32.h"
#include "navinfo.h"

#include "MainFrm.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CString CQconsoleDoc::m_sProductName;
CString CQconsoleDoc::m_sAppName;


/////////////////////////////////////////////////////////////////////////////
// CQconsoleDoc

IMPLEMENT_DYNCREATE(CQconsoleDoc, CDocument)

BEGIN_MESSAGE_MAP(CQconsoleDoc, CDocument)
	//{{AFX_MSG_MAP(CQconsoleDoc)
	// NOTE - the ClassWizard will add and remove mapping macros here.
	//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CQconsoleDoc construction/destruction

////////////////////////////////////////////////////////////////////////////////
// 2/19/98 - DBUCHES function created / header added
////////////////////////////////////////////////////////////////////////////////
CQconsoleDoc::CQconsoleDoc()
{
	CCTRACEI(_T("%s - Start"), __FUNCTION__);

	// Get app object
	CWinApp* pApp = AfxGetApp();

	// Get list view style.
	m_dwListViewStyle = LVS_REPORT;

	LoadExpandedThreatShowSetting();
	if(GetExpandedThreatShowSetting())
		m_iViewMode = VIEW_MODE_EXPANDED_THREATS;
	else
		m_iViewMode = VIEW_MODE_VIRAL_THREATS;

	// Set defaults.
	m_pQuarantine = NULL;
	SetSortCol(MapColumnIDToIndex(COLUMN_QDATE));
	m_pOpts = NULL;
	m_bStartupScanNeeded = FALSE;
	ZeroMemory(&m_stDefsDate, sizeof(SYSTEMTIME));
	ZeroMemory(&m_stCurDefsDate, sizeof(SYSTEMTIME));
	ZeroMemory(m_szCurDefsDate, sizeof(m_szCurDefsDate));

	// Q: What OS are we running on ?
	OSVERSIONINFO os;
	os.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
	GetVersionEx(&os);
	m_bNT = (BOOL)(os.dwPlatformId == VER_PLATFORM_WIN32_NT);
	if(m_bNT)
	{
		if(os.dwMajorVersion > 5)
			m_bWinXP = TRUE;
		else  
			m_bWinXP = (os.dwMajorVersion == 5 && os.dwMinorVersion >= 1);
	}
	else
	{
		m_bWinXP = FALSE;
	}

	// Get valid trialware state.
	m_bQConsoleEnabled = ((CQconsoleApp*)AfxGetApp())->IsQConsoleEnabled();

	// Set up our arrays
	ResetArrays();

	if(m_sProductName.IsEmpty())
	{
		CBrandingRes BrandRes;
		m_sProductName = BrandRes.ProductName();
		m_sAppName = BrandRes.GetString(IDS_QUARANTINE_APP_NAME);
	}

	CCTRACEI(_T("%s - End"), __FUNCTION__);
}


////////////////////////////////////////////////////////////////////////////////
// 2/26/98 - DBUCHES function created / header added
////////////////////////////////////////////////////////////////////////////////
CQconsoleDoc::~CQconsoleDoc()
{
	// Cleanup.
	ResetArrays();

	//
	// Release objects

	if(NULL != m_pQuarantine)
	{
		m_pQuarantine->Release();
		m_pQuarantine = NULL;
	}

	if(NULL != m_pOpts)
	{
		m_pOpts->Release();
		m_pOpts = NULL;
	}
}


/////////////////////////////////////////////////////////////////////////////
// CQconsoleDoc diagnostics

#ifdef _DEBUG
void CQconsoleDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CQconsoleDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG


/////////////////////////////////////////////////////////////////////////////
// CQconsoleDoc commands

////////////////////////////////////////////////////////////////////////////////
// Description	    : Saves any document-centric data to the registry
// 2/19/98 - DBUCHES function created / header added
////////////////////////////////////////////////////////////////////////////////
void CQconsoleDoc::OnCloseDocument() 
{
	CDocument::OnCloseDocument();
}


////////////////////////////////////////////////////////////////////////////
// Description	    : Handles creation of a new document
// 2/25/98 DBUCHES - Function created / Header added.
////////////////////////////////////////////////////////////////////////////
BOOL CQconsoleDoc::OnNewDocument()
{
	CCTRACEI(_T("%s - Start"), __FUNCTION__);

	HRESULT hr = S_OK;
	CWnd* pActiveWnd = AfxGetApp()->m_pActiveWnd;

	// Create the quarantine server object.
	if(SYM_FAILED(m_QuarDllLoader.CreateObject(&m_pQuarantine)) ||
		m_pQuarantine == NULL)
	{
		CCTRACEE(_T("%s - SYM_FAILED(QuarDLL_Loader::CreateObject()). Exiting."), __FUNCTION__);
		CString sMessage;
		sMessage.Format(IDS_ERROR_CREATING_QUARANTINE_OBJECT, m_sProductName);
		pActiveWnd->MessageBox(sMessage, m_sAppName, MB_ICONSTOP | MB_OK);
		return FALSE;
	}

	// Initialize the IQuarantineDLL.
	hr = m_pQuarantine->Initialize();
	if(FAILED(hr))
	{
		if(hr == E_UNABLE_TO_INITIALIZE_DEFUTILS)
		{
			CCTRACEE(_T("%s - Failed to initialize IQuarantineDLL. hr=E_UNABLE_TO_INITIALIZE_DEFUTILS. Exiting."), __FUNCTION__);

			CString sMessage((LPCTSTR) IDS_UNABLE_TO_INITIALIZE_DEFUTILS);
			pActiveWnd->MessageBox(sMessage, m_sAppName, MB_ICONSTOP | MB_OK);
		}
		else
		{
			CCTRACEE(_T("%s - Failed to initialize IQuarantineDLL. hr=0x%08X. Exiting."), __FUNCTION__, hr);

			CString sMessage;
			sMessage.Format(IDS_ERROR_CREATING_QUARANTINE_OBJECT, m_sProductName);
			pActiveWnd->MessageBox(sMessage, m_sAppName, MB_ICONSTOP | MB_OK);
		}

		return FALSE;
	}

	// Get location of quarantine folder
	m_pQuarantine->GetQuarantineFolder(m_sQuarantineFolder.GetBuffer(MAX_PATH), MAX_PATH);
	m_sQuarantineFolder.ReleaseBuffer();

	// Get scanning context
	if(FALSE == InitVirusDefs())
	{
		CCTRACEE(_T("%s - FALSE == InitVirusDefs()"), __FUNCTION__);

		CString sMsg;
		sMsg.LoadString(IDS_ERROR_PROBLEM_LOADING_VIRUS_DEFS);
		((CMainFrame*)AfxGetMainWnd())->DisplayNAVError(IDS_ERROR_PROBLEM_LOADING_VIRUS_DEFS, sMsg);

		// Should we return FALSE?
	}

	// Create quarantine options object
	if(SYM_FAILED(m_QuarOptcLoader.CreateObject(&m_pOpts)) ||
		m_pOpts == NULL)
	{
		CCTRACEE(_T("%s - SYM_FAILED(QuarOPTS_Loader::CreateObject()) || m_pOpts == NULL). Exiting."), __FUNCTION__);

		CString sMessage;
		sMessage.Format(IDS_ERROR_CREATING_QUARANTINE_OBJECT, m_sProductName);
		pActiveWnd->MessageBox(sMessage, m_sAppName, MB_ICONSTOP | MB_OK);
		return FALSE;
	}

	// Compute path to options file
	TCHAR szFileName[MAX_PATH];
	TCHAR* p;
	GetModuleFileName(NULL, szFileName, MAX_PATH);

	// Search for our module name in string, and replace it with 
	// options file name.
	_tcsupr(szFileName);
	p = strstr(szFileName, _T("QCONSOLE.EXE"));
	if(p == NULL)
	{
		CCTRACEE(_T("%s - Error generating options file name. Exiting."), __FUNCTION__);

		CString sMessage;
		sMessage.Format(IDS_ERROR_CREATING_QUARANTINE_OBJECT, m_sProductName);
		pActiveWnd->MessageBox(sMessage, m_sAppName, MB_ICONSTOP | MB_OK);
		return FALSE;
	}

	// Add options file name to path
	*p = NULL;
	lstrcat(szFileName, _T("QUAROPTS.DAT"));

	BOOL bCreate = FALSE;
	if(!::PathFileExists(szFileName))
		bCreate = TRUE;

	// Initialize object.
	hr = m_pOpts->Initialize(szFileName, bCreate);
	if(FAILED(hr))
	{
		CCTRACEE(_T("%s - Error initializing options object. Exiting."), __FUNCTION__);

		CString sMessage;
		sMessage.Format(IDS_ERROR_READING_OPTIONS, m_sProductName);
		pActiveWnd->MessageBox(sMessage, m_sAppName, MB_ICONSTOP | MB_OK);
		return FALSE;
	}

	// Get last defs date from registry.
	HKEY hKey;
	LONG lRet = RegOpenKeyEx(HKEY_LOCAL_MACHINE,
							 REGKEY_QUARANTINE,
							 0,
							 KEY_READ | KEY_WRITE,
							 &hKey);
	if(ERROR_SUCCESS == lRet)
	{
		// Get date of last defs used by quarantine console
		DWORD dwSize = sizeof(SYSTEMTIME);
		DWORD dwType = REG_BINARY;
		lRet = RegQueryValueEx(hKey,
							   REGKEY_VALUE_DEFSDATE,
							   0,
							   &dwType,
							   (LPBYTE) &m_stDefsDate,
							   &dwSize);
		if(ERROR_SUCCESS == lRet)
		{
			// Compare the times.  If the defs being used by quarantine are newer
			// fire off a scan.
			CCTRACEI("%s - Old defs = %d%02d%02d.%03d | New Defs = %d%02d%02d.%03d", __FUNCTION__, m_stDefsDate.wYear, m_stDefsDate.wMonth, m_stDefsDate.wDay, m_stDefsDate.wHour, m_stCurDefsDate.wYear, m_stCurDefsDate.wMonth, m_stCurDefsDate.wDay, m_stCurDefsDate.wHour);
			CTime t1(m_stDefsDate);
			CTime t2(m_stCurDefsDate);

			if(t2 > t1)
				m_bStartupScanNeeded = TRUE;
		}
		else
		{
			CCTRCTXE1(_T("Could not read previous defs date from registry. lReturn=%d"), lRet);
		}

		// Save off new defs date.
		dwSize = sizeof(SYSTEMTIME);
		lRet = RegSetValueEx(hKey, 
							 REGKEY_VALUE_DEFSDATE, 
							 0, 
							 REG_BINARY, 
							 (LPBYTE)&m_stCurDefsDate, 
							 dwSize);
		if(ERROR_SUCCESS != lRet)
		{
			CCTRCTXE1(_T("Could not save new defs date to registry. lReturn=%d"), lRet);
			
			//CString sMessage;
			//sMessage.Format(IDS_ERROR_CREATING_QUARANTINE_OBJECT, m_sProductName);
			//pActiveWnd->MessageBox(sMessage, m_sAppName, MB_ICONSTOP | MB_OK);
			//RegCloseKey(hKey);
			//return FALSE;
		}

		// Cleanup
		RegCloseKey(hKey);
	}
	else
	{
		CCTRCTXE1(_T("Could not open quarantine registry key. lReturn=%d"), lRet);
	}

	CCTRACEI(_T("%s - Finished custom portion of OnNewDocument."), __FUNCTION__);

	return CDocument::OnNewDocument();
}


////////////////////////////////////////////////////////////////////////////////
// Description	    : Sets up our item arrays.
// 2/26/98 - DBUCHES function created / header added
////////////////////////////////////////////////////////////////////////////////
void CQconsoleDoc::ResetArrays()
{
	// Set up allocation sizes for arrays
	POSITION pos;
	IQuarantineItem* p1;
	IQuarantineItem* p2;

	// Release all elements.
	for(pos = m_aMapAllItems.GetStartPosition(); pos != NULL;)
	{
		m_aMapAllItems.GetNextAssoc(pos, p1, p2);
		p2->Release();
	}

	// Remove references from lists
	m_aMapAllItems.RemoveAll();
	m_aMapCurrentSubset.RemoveAll();
	
	// Reset counters.
	m_iTotalItems = 0;    
	m_iQuarantineItems = 0;
	m_iBackupItems = 0;   
	//m_iSubmittedItems = 0;
	//m_iThreatBackupItems = 0;
	m_iExpandedThreatItems = 0;
	m_iViralItems = 0;
}


////////////////////////////////////////////////////////////////////////////////
// Description	    : This routine will enumerate all items in Quarantine, and
//                    add them to the appropriate lists.
// 2/26/98 - DBUCHES function created / header added
////////////////////////////////////////////////////////////////////////////////
void CQconsoleDoc::RebuildItemArrays()
{
	// Remove contents of arrays.
	ResetArrays();    

	// Set up enumeration.
	IEnumQuarantineItems* pEnum;
	if(FAILED(m_pQuarantine->Enum(&pEnum)))
	{
		ASSERT(FALSE);
		return;
	}

	// Fetch items and insert them into the proper array
	ULONG uFetched;
	DWORD dwStatus;
	IQuarantineItem* pQuarItem;
	while(pEnum->Next(1, &pQuarItem, &uFetched) == NOERROR)
	{
		// Classify this item
		if(FAILED(pQuarItem->GetFileStatus(&dwStatus)))
		{
			CCTRACEE(_T("%s - Failed to get the file status for this item. Assuming corrupt item and removing it"), __FUNCTION__);
			pQuarItem->DeleteItem();
			continue;
		}

		// Add this item to our list of items.
		m_aMapAllItems.SetAt(pQuarItem, pQuarItem);

		// Bump count
		m_iTotalItems++;

		if(dwStatus & QFILE_STATUS_QUARANTINED)
		{
			m_iQuarantineItems++;
			m_iViralItems++; 
		}
		else if(dwStatus & QFILE_STATUS_BACKUP_FILE)
		{
			m_iBackupItems++;
			m_iViralItems++;
		}
		else if(dwStatus & QFILE_STATUS_THREAT_BACKUP)
		{
			m_iExpandedThreatItems++;
		}
		else if(dwStatus & QFILE_STATUS_SUBMITTED)
		{
			// If we got to here, the status is ONLY submitted. Submitted should no 
			// longer exist as a solo status.  An item should also have a 
			// viral (quarantined or backup) or expanded threat status.
			if(QItemEZ::THREAT_VIRAL == QItemEZ::IsViral(pQuarItem))
			{
				dwStatus = dwStatus | QFILE_STATUS_QUARANTINED;
				m_iQuarantineItems++;
			}
			else
			{
				dwStatus = dwStatus | QFILE_STATUS_THREAT_BACKUP;
				m_iExpandedThreatItems++;
			}

			HRESULT hRes = pQuarItem->SetFileStatus(dwStatus);
			if(SUCCEEDED(hRes))
			{
				hRes = pQuarItem->SaveItem();
				if(FAILED(hRes))
					CCTRACEE(_T("%s - FAILED(pQuarItem->SetFileStatus(0x%08X)), 0x%08X"), __FUNCTION__, dwStatus, hRes);
			}
			else
			{
				CCTRACEE(_T("%s - FAILED(pQuarItem->SaveItem()), 0x%08X"), __FUNCTION__, hRes);
			}
		}
	}

	// Release enumeration object.
	pEnum->Release();
}


////////////////////////////////////////////////////////////////////////////////
// 2/26/98 - DBUCHES function created / header added
////////////////////////////////////////////////////////////////////////////////
CMapPtrToPtr* CQconsoleDoc::GetItemList(int iMode /* = -1 */)
{
	// Return the correct list    
	if(iMode == -1)
		iMode = m_iViewMode;

	switch(iMode)
	{
	case VIEW_MODE_ALL:
		return &m_aMapAllItems;

	case VIEW_MODE_EXPANDED_THREATS:
	case VIEW_MODE_VIRAL_THREATS:
		BuildSubsetList(iMode);
		return &m_aMapCurrentSubset;

	default:
		ASSERT(FALSE);
		return NULL;
	}

	return NULL;
}


////////////////////////////////////////////////////////////////////////////////
// Description	    : Builds the current subset list.
// 3/12/98 - DBUCHES function created / header added
////////////////////////////////////////////////////////////////////////////////
void CQconsoleDoc::BuildSubsetList(int iMode)
{
	BOOL bAdd;
	POSITION pos;
	DWORD dwStatus;
	IQuarantineItem* p;
	IQuarantineItem* p1;

	// Clean up list and map.
	m_aMapCurrentSubset.RemoveAll();

	// Classify all elements
	for(pos = m_aMapAllItems.GetStartPosition(); pos != NULL;)
	{
		m_aMapAllItems.GetNextAssoc(pos, p, p1);
		ASSERT(p);

		if(FAILED(p->GetFileStatus(&dwStatus)))
			return;

		bAdd = FALSE;
		if(dwStatus & QFILE_STATUS_QUARANTINED)
		{
			if(iMode == VIEW_MODE_VIRAL_THREATS)
				bAdd = TRUE;
		}
		else if(dwStatus & QFILE_STATUS_BACKUP_FILE)
		{
			if(iMode == VIEW_MODE_VIRAL_THREATS)
				bAdd = TRUE;
		}
		else if(dwStatus & QFILE_STATUS_THREAT_BACKUP)
		{
			if(iMode == VIEW_MODE_EXPANDED_THREATS)
				bAdd = TRUE;
		}
		//else if(dwStatus & QFILE_STATUS_SUBMITTED)
		//{
		//	if(iMode == VIEW_MODE_SUBMITTED)
		//		bAdd = TRUE;
		//}

		if(bAdd)
		{
			// Add this item to our list of items.
			m_aMapCurrentSubset.SetAt(p, p);
		}
	}
}


////////////////////////////////////////////////////////////////////////////////
// Description	    : Recomputes the item type counts
// 3/12/98 - DBUCHES function created / header added
////////////////////////////////////////////////////////////////////////////////
void CQconsoleDoc::RecalcItemTypes()
{
	POSITION pos;
	DWORD dwStatus;
	IQuarantineItem* pItem;
	IQuarantineItem* p;

	// Reset counters.
	m_iQuarantineItems = 0;
	//m_iSubmittedItems = 0;
	m_iBackupItems = 0;
	//m_iThreatBackupItems = 0;
	m_iExpandedThreatItems = 0;
	m_iViralItems = 0;

	// Classify all elements
	for(pos = m_aMapAllItems.GetStartPosition(); pos != NULL;)
	{
		m_aMapAllItems.GetNextAssoc(pos, p, pItem);
		ASSERT(pItem);

		// Classify this item
		if(FAILED(pItem->GetFileStatus(&dwStatus)))
			continue;

		// adjust counters
		if(dwStatus & QFILE_STATUS_QUARANTINED)
		{
			m_iQuarantineItems++;
			m_iViralItems++; 
		}
		else if(dwStatus & QFILE_STATUS_BACKUP_FILE)
		{
			m_iBackupItems++;
			m_iViralItems++;
		}
		else if(dwStatus & QFILE_STATUS_THREAT_BACKUP)
		{
			m_iExpandedThreatItems++;
		}
		//else if(dwStatus & QFILE_STATUS_SUBMITTED)
		//{
		//	m_iSubmittedItems++;
		//}
	}
}


////////////////////////////////////////////////////////////////////////////
// Description	    : Removes a quarantine item from the master list, as well
//                    as from any classification lists
// Return type		: BOOL - TRUE on success
// Argument         : IQuarantineItem* pItem - item to remove
// 3/6/98 DBUCHES - Function created / Header added.
////////////////////////////////////////////////////////////////////////////
BOOL CQconsoleDoc::DeleteItemFromLists(IQuarantineItem* pItem, BOOL bAll /* = TRUE */)
{
	DWORD dwStatus;

	// Remove from current subset list    
	m_aMapCurrentSubset.RemoveKey(pItem);
        
	// Remove from main list.
	if(bAll)
	{
		// Remove element.
		m_aMapAllItems.RemoveKey(pItem);

		// Adjust counters
		m_iTotalItems --;
		if(SUCCEEDED(pItem->GetFileStatus(&dwStatus)))
		{
			if(dwStatus & QFILE_STATUS_QUARANTINED)
			{
				m_iQuarantineItems--;
				m_iViralItems--;
			}
			else if(dwStatus & QFILE_STATUS_BACKUP_FILE)
			{
				m_iBackupItems--;
				m_iViralItems--;
			}
			else if(dwStatus & QFILE_STATUS_THREAT_BACKUP)
			{
				m_iExpandedThreatItems--;
			}
			//else if(dwStatus & QFILE_STATUS_SUBMITTED)
			//{
			//	m_iSubmittedItems--;
			//}
		}

		// Release object
		pItem->Release();
	}

	// Success!
	return TRUE;
}


////////////////////////////////////////////////////////////////////////////
// Description	    : Sets up our virus definitions
// 3/2/98 DBUCHES - Function created / Header added.
////////////////////////////////////////////////////////////////////////////
BOOL CQconsoleDoc::InitVirusDefs()
{
	DefUtilsLoader DefUtilsLoader;

	{ // Scope the object
		IDefUtilsPtr pIDefUtils;
		if(SYM_FAILED(DefUtilsLoader.CreateObject(pIDefUtils.m_p)))
		{
			ASSERT(FALSE);
			CCTRACEE("%s - Failed to create the defutils loader object", __FUNCTION__);
			return FALSE;
		}

		// Initialize defutils
		if(pIDefUtils->InitWindowsApp(_T(QUARANTINE_APP_ID)) == false)
		{
			ASSERT(FALSE);
			CCTRACEE("%s - Failed to initialize defutils", __FUNCTION__);
			return FALSE;
		}

		// Get virus def location
		if(pIDefUtils->GetCurrentDefs(m_sDefDir.GetBuffer(MAX_PATH), MAX_PATH) == false)
		{
			m_sDefDir.ReleaseBuffer();
			CCTRACEE("%s - Failed to get the current defs folder", __FUNCTION__);
			ASSERT(FALSE);
			return FALSE;
		}
		m_sDefDir.ReleaseBuffer();

		// Get current defs being used by quarantine
		DWORD dwTmpRev = 0;
		if(pIDefUtils->GetCurrentDefsDate(&m_stCurDefsDate.wYear, &m_stCurDefsDate.wMonth, &m_stCurDefsDate.wDay, &dwTmpRev) == false)
		{
			CCTRACEE("%s - Failed to get the definitions date", __FUNCTION__);
			return FALSE;
		}

		// Set the revision
		m_stCurDefsDate.wHour = (WORD)dwTmpRev;
	} // End scope

	// YYYYMMDDRRRR = 12 chars
	//TCHAR szDefsRevision [13] = {0};
	wsprintf(m_szCurDefsDate, _T("%04u%02u%02u%04u"), m_stCurDefsDate.wYear, m_stCurDefsDate.wMonth, m_stCurDefsDate.wDay, m_stCurDefsDate.wHour);

	// All good.
	return TRUE;    
}


bool CQconsoleDoc::GetDefsDate(SYSTEMTIME& stDefsDate)
{
	stDefsDate = m_stCurDefsDate;
	if(0 == stDefsDate.wYear)
		return false;
	else
        return true;
}


bool CQconsoleDoc::GetDefsDate(LPTSTR szDefsDate)
{
	_tcscpy(szDefsDate, m_szCurDefsDate);
	if(_tcslen(szDefsDate) <= 0)
		return false;
	else
		return true;
}


BOOL CQconsoleDoc::GetFullANSIFilePath(IQuarantineItem* pQItem, CString& sOutPath, DWORD dwRemediationIndex)
{
	return QItemEZ::GetFullANSIFilePath(pQItem, sOutPath, dwRemediationIndex);
}


BOOL CQconsoleDoc::GetANSIFileNameOnly(IQuarantineItem* pQItem, CString& sOutPath, DWORD dwRemediationIndex)
{
	return QItemEZ::GetANSIFileNameOnly(pQItem, sOutPath, dwRemediationIndex);
}


BOOL CQconsoleDoc::GetANSIFilePathOnly(IQuarantineItem* pQItem, CString& sOutPath, DWORD dwRemediationIndex)
{
	return QItemEZ::GetANSIFilePathOnly(pQItem, sOutPath, dwRemediationIndex);
}


BOOL CQconsoleDoc::GetThreatName(IQuarantineItem* pQItem, CString& cszThreatName)
{
	if(!QItemEZ::GetThreatName(pQItem, cszThreatName) || cszThreatName.IsEmpty())
	{
		// If no Virus, display "Unknown"
		CString s, cszDefault, cszFileName;
		cszDefault.LoadString(IDS_UNKNOWN_THREAT_NAME);
		QItemEZ::GetANSIFileNameOnly(pQItem, cszFileName);

		s.FormatMessage(cszDefault, cszFileName);

		// Is this file from WarChild?
		DWORD dwFileType;
		pQItem->GetFileType (&dwFileType);

		if(QFILE_TYPE_WARCHILD & dwFileType)
			s.LoadString (IDS_WARCHILD_SOURCE);

		if(QFILE_TYPE_OEH & dwFileType)
			s.LoadString (IDS_OEH_SOURCE);

		cszThreatName = s;
	}

	return FALSE == cszThreatName.IsEmpty();
}


DWORD CQconsoleDoc::GetRiskLevelNumber(IQuarantineItem* pQuarItem, DWORD dwCategory)
{
	DWORD dwValue = -1;
	HRESULT hResult = pQuarItem->GetAnomalyDamageFlag(dwCategory, &dwValue);
	if(FAILED(hResult))
		dwValue = GetDefaultDamageFlagValue(pQuarItem);

	return dwValue;
}

bool CQconsoleDoc::GetRiskLevelText(DWORD dwRiskLevelNumber, CString& sRiskLevel)
{
	BOOL bSucceeded = FALSE;

	switch(dwRiskLevelNumber)
	{
	case 0:
	case 1:
	case 2:
		bSucceeded = sRiskLevel.LoadString(IDS_THREAT_MATRIX_LOW);
		break;
	case 3:
		bSucceeded = sRiskLevel.LoadString(IDS_THREAT_MATRIX_MED);
		break;
	case 4:
	case 5:
		bSucceeded = sRiskLevel.LoadString(IDS_THREAT_MATRIX_HIGH);
		break;
	default:
		break;
	}

	return (bSucceeded ? true : false);
}


// Based on ThreatMatrixDlg ToHighMedLow()
// In-synch as of 4/28/2005
RiskLevel CQconsoleDoc::GetRiskLevel(DWORD dwRiskLevelNumber)
{
	RiskLevel rLevel;
	switch(dwRiskLevelNumber)
	{
	case 0:
	case 1:
	case 2:
		rLevel = RiskLevel_Low;
		break;
	case 3:
		rLevel = RiskLevel_Medium;
		break;
	case 4:
	case 5:
		rLevel = RiskLevel_High;
		break;
	default:
		rLevel = RiskLevel_NotApplicable;
		break;
	}

	return rLevel;
}


// Based on CEZAnomaly::GetDamageFlagAverage()
// In-synch as of 4/28/2005
DWORD CQconsoleDoc::GetRiskLevelAverage(IQuarantineItem* pQuarItem)
{
	float fTotal = 0;
	DWORD dwValue = 0;
	float fAvg = 0;
	float fValueCount = 0;

	dwValue = GetRiskLevelNumber(pQuarItem, ccEraser::IAnomaly::Performance);
	fValueCount++;
	fTotal += dwValue;

	dwValue = GetRiskLevelNumber(pQuarItem, ccEraser::IAnomaly::Privacy);
	fValueCount++;
	fTotal += dwValue;

	dwValue = GetRiskLevelNumber(pQuarItem, ccEraser::IAnomaly::Removal);
	fValueCount++;
	fTotal += dwValue;

	dwValue = GetRiskLevelNumber(pQuarItem, ccEraser::IAnomaly::Stealth);
	fValueCount++;
	fTotal += dwValue;

	if(0 == fValueCount)
	{
		// This should never happen since we always fall back to the default, but adding it just
		// to uber-avoid dividing by 0.
		DWORD dwDefault = GetDefaultDamageFlagValue(pQuarItem);
		CCTRACEW(_T("%s - There were no damage flags available. Using default of %d"), __FUNCTION__, dwDefault);
		return dwDefault;
	}

	fAvg = fTotal/fValueCount;

	// 0 - 2.49 = Low
	if(fAvg < 2.5)
		dwValue = 1;
	// 2.5 - 3.49 = Medium
	else if(fAvg < 3.5)
		dwValue = 3;
	// 3.5 or greater = High
	else
		dwValue = 5;

	// Find out if we want to return the floor or ceiling of this guy by checking the
	// fractional part of the value and rounding up or down
	//float fIntegralPart = 0.0;
	//float fFractionalPart = modf(fAvg, &fIntegralPart);

	//if(fFractionalPart >= 0.5)
	//    dwValue = static_cast<DWORD>(ceil(fAvg));
	//else
	//    dwValue = static_cast<DWORD>(floor(fAvg));

	return dwValue;
}


bool CQconsoleDoc::GetRiskLevelCategoryText(RiskCategory riskCategory, DWORD dwRiskLevelNumber, CString& sHeader, CString& sDescription)
{
	BOOL bSucceeded = FALSE;

	RiskLevel rLevel = GetRiskLevel(dwRiskLevelNumber);

	switch(riskCategory)
	{
	case RiskCategory_Overall:
		{
			bSucceeded = sHeader.LoadString(IDS_OVERALL);

			switch(rLevel)
			{
			case RiskLevel_Low:
				bSucceeded = sDescription.LoadString(IDS_THREAT_MATRIX_DESC_OVERALL_LOW);
				break;
			case RiskLevel_Medium:
				bSucceeded = sDescription.LoadString(IDS_THREAT_MATRIX_DESC_OVERALL_MED);
				break;
			case RiskLevel_High:
				bSucceeded = sDescription.LoadString(IDS_THREAT_MATRIX_DESC_OVERALL_HIGH);
				break;
			}
			break;
		}

	case (ccEraser::IAnomaly::Performance):
		{
			bSucceeded = sHeader.LoadString(IDS_PERFORMANCE);

			switch(rLevel)
			{				
			case RiskLevel_Low:
				bSucceeded = sDescription.LoadString(IDS_THREAT_MATRIX_DESC_PERF_LOW);
				break;
			case RiskLevel_Medium:
				bSucceeded = sDescription.LoadString(IDS_THREAT_MATRIX_DESC_PERF_MED);
				break;
			case RiskLevel_High:
				bSucceeded = sDescription.LoadString(IDS_THREAT_MATRIX_DESC_PERF_HIGH);
				break;
			}
			break;
		}

	case (ccEraser::IAnomaly::Privacy):
		{
			bSucceeded = sHeader.LoadString(IDS_PRIVACY);

			switch(rLevel)
			{
			case RiskLevel_Low:
				bSucceeded = sDescription.LoadString(IDS_THREAT_MATRIX_DESC_PRIV_LOW);
				break;
			case RiskLevel_Medium:
				bSucceeded = sDescription.LoadString(IDS_THREAT_MATRIX_DESC_PRIV_MED);
				break;
			case RiskLevel_High:
				bSucceeded = sDescription.LoadString(IDS_THREAT_MATRIX_DESC_PRIV_HIGH);
				break;
			}
			break;
		}

	case (ccEraser::IAnomaly::Removal):
		{
			bSucceeded = sHeader.LoadString(IDS_REMOVAL);

			switch(rLevel)
			{
			case RiskLevel_Low:
				bSucceeded = sDescription.LoadString(IDS_THREAT_MATRIX_DESC_REMOVE_LOW);
				break;
			case RiskLevel_Medium:
				bSucceeded = sDescription.LoadString(IDS_THREAT_MATRIX_DESC_REMOVE_MED);
				break;
			case RiskLevel_High:
				bSucceeded = sDescription.LoadString(IDS_THREAT_MATRIX_DESC_REMOVE_HIGH);
				break;
			}
			break;
		}

	case (ccEraser::IAnomaly::Stealth):
		{
			bSucceeded = sHeader.LoadString(IDS_STEALTH);

			switch(rLevel)
			{
			case RiskLevel_Low:
				bSucceeded = sDescription.LoadString(IDS_THREAT_MATRIX_DESC_STEALTH_LOW);
				break;
			case RiskLevel_Medium:
				bSucceeded = sDescription.LoadString(IDS_THREAT_MATRIX_DESC_STEALTH_MED);
				break;
			case RiskLevel_High:
				bSucceeded = sDescription.LoadString(IDS_THREAT_MATRIX_DESC_STEALTH_HIGH);
				break;
			}
			break;
		}
	}

	return (bSucceeded ? true : false);
}


// Based on CEZAnomaly::GetDefaultDamageFlagValue()
DWORD CQconsoleDoc::GetDefaultDamageFlagValue(IQuarantineItem* pQuarItem)
{
	DWORD DEFAULT_HIGH_RISK = 5;
	DWORD DEFAULT_MEDIUM_RISK = 3;

	if(QItemEZ::THREAT_VIRAL == QItemEZ::IsViral(pQuarItem))
		return DEFAULT_HIGH_RISK;


	// If there are any "high" level categories return a high value otherwise
	// return a medium value

	bool bScanInfectionCategories = false;
	CString sAnomalyCategories;
	QItemEZ::GetAnomalyCategories(pQuarItem, sAnomalyCategories);

	sAnomalyCategories.Trim();
	if(sAnomalyCategories.IsEmpty())
	{
		// Try to get info from viral categories
		QItemEZ::GetScanInfectionCategories(pQuarItem, sAnomalyCategories);
		sAnomalyCategories.Trim();
		if(sAnomalyCategories.IsEmpty())
			return DEFAULT_MEDIUM_RISK;

		bScanInfectionCategories = true;
	}

	TCHAR* szTokens = _T(" ,");
	int iCurPos= 0;
	DWORD dwCategory = -1;
	CString sTokenized = sAnomalyCategories.Tokenize(szTokens, iCurPos);
	while(sTokenized != "")
	{
		DWORD dwCategory = atol(sTokenized);

		if(!bScanInfectionCategories &&
			(ccEraser::IAnomaly::SecurityRisk == dwCategory ||
			ccEraser::IAnomaly::Hacktool == dwCategory ||
			ccEraser::IAnomaly::SpyWare == dwCategory ||
			ccEraser::IAnomaly::Dialer == dwCategory ||
			ccEraser::IAnomaly::RemoteAccess == dwCategory ||
			ccEraser::IAnomaly::Adware))
		{
			return DEFAULT_HIGH_RISK;
		}

		if(bScanInfectionCategories &&
			(SCAN_THREAT_TYPE_SECURITY_RISK == dwCategory ||
			SCAN_THREAT_TYPE_HACKTOOL == dwCategory ||
			SCAN_THREAT_TYPE_SPYWARE == dwCategory ||
			SCAN_THREAT_TYPE_DIALER == dwCategory ||
			SCAN_THREAT_TYPE_REMOTE_ACCESS == dwCategory ||
			SCAN_THREAT_TYPE_ADWARE))
		{
			return DEFAULT_HIGH_RISK;
		}

		sTokenized = sAnomalyCategories.Tokenize(szTokens, iCurPos);
	}

	// This was not any of our high non-viral categories, return medium value
	return DEFAULT_MEDIUM_RISK;
}


int CQconsoleDoc::MapColumnIDToIndex(int iColumnID)
{
	int iViewMode = GetViewMode();
	bool bViralView = (VIEW_MODE_VIRAL_THREATS == iViewMode);

	switch(iColumnID)
	{
	case COLUMN_THREAT_NAME:
		return (bViralView ? ColumnMap_ViralView_RiskName : ColumnMap_ExpandedThreatView_RiskName);
	case COLUMN_QDATE:
		return (bViralView ? ColumnMap_ViralView_DateAdded : ColumnMap_ExpandedThreatView_DateAdded);
	case COLUMN_DEPENDENCIES:
		return (bViralView ? -1 : ColumnMap_ExpandedThreatView_Dependencies);
	case COLUMN_RISK_LEVEL:
		return (bViralView ? ColumnMap_ViralView_RiskLevel : ColumnMap_ExpandedThreatView_RiskLevel);
	//case COLUMN_FILE_NAME:
	//	return ColumnMap_FileName;
	//case COLUMN_LOCATION:
	//	return ColumnMap_Location;
	case COLUMN_SDATE:
		return (bViralView ? ColumnMap_ViralView_SubmittedDate : ColumnMap_ExpandedThreatView_SubmittedDate);
	case COLUMN_STATUS:
		return (bViralView ? ColumnMap_ViralView_Status : ColumnMap_ExpandedThreatView_Status);
	case COLUMN_CATEGORY:
		return (bViralView ? ColumnMap_ViralView_Category : ColumnMap_ExpandedThreatView_Category);
	}

	CCTRACEE(_T("%s - Invalid column ID for view %s"),
		__FUNCTION__, 
		((VIEW_MODE_VIRAL_THREATS == iViewMode) ? _T("VIEW_MODE_VIRAL_THREATS") : _T("VIEW_MODE_EXPANDED_THREATS")));
	return -1;
}

int CQconsoleDoc::MapColumnIndexToID(int iColumnIndex)
{
	int iViewMode = GetViewMode();

	switch(iColumnIndex)
	{
	case 0:
	case 1:
	case 2:
	case 3:
		return iColumnIndex;

	case 4:
		switch(iViewMode)
		{
		case VIEW_MODE_EXPANDED_THREATS:
			return COLUMN_DEPENDENCIES;
		case VIEW_MODE_VIRAL_THREATS:
			return COLUMN_SDATE;
		}

	case 5:
		switch(iViewMode)
		{
		case VIEW_MODE_EXPANDED_THREATS:
			return COLUMN_SDATE;
		case VIEW_MODE_VIRAL_THREATS:
			return COLUMN_STATUS;
		}

	case 6:
		if(VIEW_MODE_EXPANDED_THREATS == iViewMode)
			return COLUMN_STATUS;
	}

	CCTRACEE(_T("%s - Invalid column index for view %s"), 
		__FUNCTION__, 
		((VIEW_MODE_VIRAL_THREATS == iViewMode) ? _T("VIEW_MODE_VIRAL_THREATS") : _T("VIEW_MODE_EXPANDED_THREATS")));
	return -1;
}


bool CQconsoleDoc::GetIntroEnabled()
{
	if(!GetExpandedThreatShowSetting())
	{
		CCTRACEI(_T("%s - !GetExpandedThreatShowSetting(). Exit."), __FUNCTION__);
		return false;
	}

	bool bShowIntro = true; // Default to showing the intro dialog

	HKEY hKey = NULL;
    CString sKeyName;
	sKeyName.Format(_T("%s\\%s"), REGKEY_QUARANTINE_UI, REGKEY_UI_SETTINGS);
    
    if(ERROR_SUCCESS == RegOpenKeyEx(HKEY_CURRENT_USER, sKeyName, 0, KEY_READ, &hKey))
	{
		DWORD dwValue = 0;
		DWORD dwType = 0;
		DWORD dwSize = sizeof(DWORD);

		if(ERROR_SUCCESS == RegQueryValueEx(hKey, 
			REGKEY_VALUE_SHOW_INTRO,
			0,
			&dwType,
			(LPBYTE)&dwValue,
			&dwSize))
		{
			if(REG_DWORD == dwType)
				bShowIntro = ((dwValue == 0) ? false : true);
			else
				CCTRACEE(_T("%s - Wrong reg value type"), __FUNCTION__);
		}

		if(ERROR_SUCCESS != RegCloseKey(hKey))
			CCTRACEE(_T("%s - Failed RegCloseKey()"), __FUNCTION__);
	}
	
	CCTRACEI(_T("%s - Return %s"), __FUNCTION__, (bShowIntro ? _T("true") : _T("false")));

	return bShowIntro;
}


bool CQconsoleDoc::SetIntroEnabled(bool bShowIntro)
{
	HKEY hKey = NULL;
 
    CString sKeyName;
	sKeyName.Format(_T("%s\\%s"), REGKEY_QUARANTINE_UI, REGKEY_UI_SETTINGS);

	bool bRet = false;
    if(ERROR_SUCCESS == RegOpenKeyEx(HKEY_CURRENT_USER, sKeyName, 0, KEY_WRITE, &hKey))
	{
		DWORD dwValue = (bShowIntro ? 1 : 0);
		DWORD dwSize = sizeof(DWORD);

		if(ERROR_SUCCESS != RegSetValueEx(hKey, 
			REGKEY_VALUE_SHOW_INTRO,
			0,
			REG_DWORD,
			(LPBYTE)&dwValue,
			dwSize))
		{
			CCTRACEE(_T("%s - Failed to set ShowIntro value"), __FUNCTION__);
		}
		else
		{
			bRet = true;
		}

		RegCloseKey(hKey);
	}

	return bRet;
}


HRESULT CQconsoleDoc::LoadExpandedThreatShowSetting()
{
	// Check NAV options
	DWORD dwValue;
	m_bShowExpandedThreats = true; // Default to showing expanded threats
	HRESULT hResult = E_FAIL;

	bool bIsSafeMode = false;
	if(0 != GetSystemMetrics(SM_CLEANBOOT))
	{
		bIsSafeMode = true;
		CCTRACEI(_T("%s - Running in safe mode."), __FUNCTION__);
	}

	if(!bIsSafeMode)
	{
		CNAVOptSettingsEx NavOpts;
		if(!NavOpts.Init())
		{
			CCTRACEE("%s - Unable to initialize the options library.", __FUNCTION__);
			return E_FAIL;
		}

		// Default to showing expanded threats
		hResult = NavOpts.GetValue(THREAT_NoThreatCat, dwValue, 0);
		if(SUCCEEDED(hResult))
			m_bShowExpandedThreats = ((0 == dwValue) ? true : false);
	}
	else
	{
		TCHAR szNAVOpts[MAX_PATH] = {0};
		HNAVOPTS32 hOptions = NULL;
		DWORD dwSize = MAX_PATH;
		DWORD dwValue = 0;
		HKEY hKey = NULL;

		CNAVInfo NAVInfo;
		_tcscpy(szNAVOpts, NAVInfo.GetNAVOptPath());

		NAVOPTS32_STATUS Status = NavOpts32_Allocate(&hOptions);
		if(NAVOPTS32_OK == Status)
		{
			// initialize the NAVOpts.dat option file.
			Status = NavOpts32_Load(szNAVOpts, hOptions, true);
			if(NAVOPTS32_OK == Status)
			{
				Status = NavOpts32_GetDwordValue(hOptions, THREAT_NoThreatCat, &dwValue, 0);
				if(NAVOPTS32_OK == Status)
					m_bShowExpandedThreats = ((0 == dwValue) ? true : false);
			}
			else
			{
				hResult = E_FAIL;
				CCTRACEE(_T("%s - Unable to load the NAVOpts.dat option file."), __FUNCTION__);
			}
		}
		else
		{
			hResult = E_FAIL;
			CCTRACEE(_T("%s - Unable to initialize options library."), __FUNCTION__);
		}

		// Clean up
		if(hOptions)
			NavOpts32_Free(hOptions);
	}

	return hResult;
}


bool CQconsoleDoc::GetExpandedThreatShowSetting()
{
	return m_bShowExpandedThreats;
}

bool CQconsoleDoc::IsFileCompressed(LPCTSTR szFileName)
{
    LPCTSTR szCompressedExtList[] = { 
        _T("ARC"), 
        _T("ARJ"), 
        _T("BZ2"), 
        _T("CAB"), 
        _T("GZ"), 
        _T("LHA"), 
        _T("LZH"),
        _T("MIM"), 
        _T("MME"),
        _T("RAR"),
        _T("TAR"), 
        _T("TAZ"), 
        _T("TGZ"), 
        _T("UUE"),
        _T("Z"), 
        _T("ZIP") };

    LPCTSTR szExt = NULL;

    szExt = PathFindExtension(szFileName);

    if(!szExt)
        return false;

    if(_T('.') == *szExt)
        szExt ++;

    if(!*szExt)
        return false;

    INT iCount = CCDIMOF(szCompressedExtList);
    for(INT iIndex = 0; iIndex < iCount; iIndex++)
    {
        if(0 == _tcsicmp(szExt, szCompressedExtList[iIndex]))
            return true;

        LPCTSTR szLastChar = CharPrev(szExt, szExt + _tcslen(szExt));
        if(szLastChar && _T('_') == *szLastChar)
            return true;
    }

    return false;
}
