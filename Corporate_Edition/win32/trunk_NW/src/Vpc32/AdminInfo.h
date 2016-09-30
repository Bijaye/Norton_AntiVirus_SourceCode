// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright (c) 2005 Symantec Corporation. All rights reserved.
// AdminInfo.h: interface for the CAdminInfo class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ADMININFO_H__77E939BB_CBAF_4000_8A08_5356915B4B60__INCLUDED_)
#define AFX_ADMININFO_H__77E939BB_CBAF_4000_8A08_5356915B4B60__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

//////////////////////////////////////////////////////////////////////
// Local defines
//////////////////////////////////////////////////////////////////////
#define NAVNTUTL_DLL_NAME			_T( "NAVNTUTL.DLL" )
#define PROCESSISNORMALUSER         _T("ProcessIsNormalUser")
#define POLICYCHECKHIDEDRIVES       _T("PolicyCheckHideDrives")
#define POLICYCHECKRUNONLY          _T("PolicyCheckRunOnly")
#define PROCESSCANUPDATEREGKEY      _T("ProcessCanUpdateRegKey")

//////////////////////////////////////////////////////////////////////
// Local typedefs
//////////////////////////////////////////////////////////////////////
typedef ULONG (*PFNProcessIsNormalUser)(ULONG * lpbNormalUser);
typedef ULONG (*PFNPolicyCheckHideDrives)( ULONG * lpdwDrives );
typedef ULONG (*PFNPolicyCheckRunOnly)( LPTSTR lpszAppName, ULONG * lpbAllowed );
typedef ULONG (*PFNProcessCanUpdateRegKey)( HANDLE hProcessToken, HANDLE hMainKey, LPCTSTR lpszRegKey, ULONG * lpbCanUpdate);

class CAdminInfo  
{
public:
	CAdminInfo();
	virtual ~CAdminInfo();

	DWORD AppIsAllowedToRun(LPTSTR lpszAppName, LPBOOL lpbAllowed);
	DWORD IsProcessNormalUser(LPBOOL lpbNormalUser);
	DWORD GetHiddenDriveList(LPDWORD lpdwHiddenDrives);
	BOOL  CanProcessUpdateRegKey(HANDLE hProcessToken, HANDLE hMainKey, LPCTSTR lpszRegKey);

protected:
	DWORD m_dwPlatformId;
	HINSTANCE m_hNAVNTUTL;
	DWORD m_dwHiddenDrives;

    PFNProcessIsNormalUser      m_pfnProcessIsNormalUser;
    PFNPolicyCheckHideDrives    m_pfnPolicyCheckHideDrives;
    PFNPolicyCheckRunOnly       m_pfnPolicyCheckRunOnly;
    PFNProcessCanUpdateRegKey   m_pfnProcessCanUpdateRegKey;

};

#endif // !defined(AFX_ADMININFO_H__77E939BB_CBAF_4000_8A08_5356915B4B60__INCLUDED_)
