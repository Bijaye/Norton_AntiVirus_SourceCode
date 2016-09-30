////////////////////////
//
// PROPRIETARY / CONFIDENTIAL.
// Use of this product is subject to license terms.
// Copyright © 2006 Symantec Corporation.
// All rights reserved.
//
/////////////////////////////////////////////////////////////////////////////////////////////////

// NAVInfo.h: interface for the CNAVInfo class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_NAVINFO_H__2CAA1156_1D17_4090_A36E_614B70CC184C__INCLUDED_)
#define AFX_NAVINFO_H__2CAA1156_1D17_4090_A36E_614B70CC184C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "tchar.h"
#include "shfolder.h"

/////////////////////////////////////////////////////////////////////////////
// @class CNAVInfo
//
// This class reports common information about NAV's installation.

class CNAVInfo  
{
public:
	const TCHAR* GetAllUsersAppsDir();
	bool MakeNAVUserDir();
	CNAVInfo();
	virtual ~CNAVInfo();

    const TCHAR* GetNAVDir ();
    const TCHAR* GetSymantecCommonDir ();
    const TCHAR* GetNAVCommonDir ();
	const TCHAR* GetNAVOptPath ();
	const TCHAR* GetNAVUserDir ();
    const TCHAR* GetNAVHelpFile ();
	const TCHAR* GetOnlinePlatformClientDir();

    bool IsNAVProfessional ();        // Returns true if this is a NAV Pro install

protected:
	bool InitGetAllUsersAppsDir();
	bool InitGetNAVDir();
	bool InitGetNAVUserDir();
	bool InitGetSymantecCommonDir();
	bool InitGetNAVCommonDir();
	bool InitGetNAVOptPath();
	bool InitGetOnlinePlatformClientDir();

    TCHAR m_szAllUsersAppsDir [MAX_PATH];
    TCHAR m_szNAVDir [MAX_PATH];
    TCHAR m_szSymantecCommonDir [MAX_PATH];
    TCHAR m_szNAVCommonDir [MAX_PATH];
    TCHAR m_szNAVOptPath [MAX_PATH];
    TCHAR m_szNAVUserDir [MAX_PATH];
	TCHAR m_szOPCDir[MAX_PATH];

    // We need to dynamically link to SHFOLDER.DLL otherwise
    // when you compile this on a WinNT machine it uses
    // SHELL32.DLL for the SHGetFolderPath, which isn't in SHELL32.DLL
    // on Win9x machines. Love MS!
    //
    PFNSHGETFOLDERPATH  m_pfnSHGetFolderPath;
	HINSTANCE           m_hSHFOLDER;
    
    bool InitSHGetFolderPath();

};

#endif // !defined(AFX_NAVINFO_H__2CAA1156_1D17_4090_A36E_614B70CC184C__INCLUDED_)
