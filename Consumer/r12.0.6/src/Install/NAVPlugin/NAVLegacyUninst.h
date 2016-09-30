// NAVLegacyUninst.h: interface for the CNAVLegacyUninst class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_NAVLEGACYUNINST_H__7205B4ED_06F6_4A74_86D6_D5A0C3EC9363__INCLUDED_)
#define AFX_NAVLEGACYUNINST_H__7205B4ED_06F6_4A74_86D6_D5A0C3EC9363__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "stdafx.h"

class CNAVLegacyUninst  
{
public:
	BOOL Uninstall(LPCTSTR szDirectory = NULL);
	CNAVLegacyUninst();
	virtual ~CNAVLegacyUninst();

	BOOL Initialize(LPCTSTR szDirectory = NULL);
	BOOL GetLegacyVersionNum(int* iVersion);
	BOOL ShutdownAPUI();
	BOOL RemoveNAVServices();
	BOOL RemoveScheduledTasks();
	BOOL ShutdownPoproxy();
	BOOL RemoveScriptBlocking();
	BOOL UnregisterWithLU();
	BOOL UninstallVirusDefs();
	BOOL RemoveNAVDXStartUp();
	BOOL RemoveNetscapePlugin();
	BOOL UnregisterWithSymEvent();
	BOOL UnregisterCOMObjects();
	BOOL UnregisterNMain();
	BOOL RemoveAdvProtection();
	BOOL UnregisterSAP();
	BOOL RemoveRescue();
	BOOL RemoveVBoxWrapper();
	BOOL RemoveRegkeys();
	BOOL RemoveFiles();
	BOOL RemoveShortcuts();

private:
	BOOL RemoveTaskFiles();
	BOOL UnregisterCOMDll(TCHAR* pszFileName, TCHAR* pszDirectory);
	BOOL RemoveService(const TCHAR* pszServiceName);
	LONG DeleteRegKeyName(HKEY hRootKey, LPCTSTR lpszSubKey, LPCTSTR lpszName);
	VOID RemoveBackLogKey();
	
	TCHAR m_szNAVInstDir[MAX_PATH];
	BOOL m_bInitialized;
	int m_iVersionNum;
	BOOL m_bNAVInstalled;
	BOOL m_bNAVPro;
	BOOL m_bFORCE;
};

#endif // !defined(AFX_NAVLEGACYUNINST_H__7205B4ED_06F6_4A74_86D6_D5A0C3EC9363__INCLUDED_)
