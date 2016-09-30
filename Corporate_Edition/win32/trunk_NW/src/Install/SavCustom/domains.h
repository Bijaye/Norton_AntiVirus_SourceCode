// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright (c) 2005 Symantec Corporation. All rights reserved.
/********************************************************
	Author: Randy Templeton




	Copywrite (C) 1997 Intel Corporation
******************************************************/
#ifndef _DOMAINS_H
#define _DOMAINS_H

#include "TransmanClass.h"

#define ID_INSERTSERVER			WM_USER+20


class CDomainInfo
{
public:
	CString m_sName;
	CString m_sParent;
	CDomainInfo(CString name,CString parent)
		: m_sName(name), m_sParent(parent) {};
};


void StartDomains();
//void GetDomains(CListCtrl* pList);
void CleanupDomains();
//BOOL ServerInPongList(LPSTR pComputer);
void GetServers(CListCtrl* pList, BOOL bCltSupport = FALSE, BOOL* bAdminOnly = NULL);
BOOL AlreadyInDomains(CString*,CString*);
BOOL VerifyServerEngine(CString sDomain, HWND hList = NULL);
//void ModifyServerState(int selection,CString sDomain,HWND hList);
//void PromptDomainSelection(HWND hList,UINT msgID,HWND hList2=NULL);
//BOOL ServersFound();
//BOOL ServerClientSupport();
void AddAddressCacheInfo(CString*);
DWORD ReturnPong(const char *ComputerName,const char *DomainName,const char *Mom,PONGDATA *pong);
//DWORD AbortDiscovery();

#endif