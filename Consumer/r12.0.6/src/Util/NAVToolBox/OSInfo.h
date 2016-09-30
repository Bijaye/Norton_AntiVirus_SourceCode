// /////////////////////////////////////////////////////////////////////////////////////////////////
// Symantec copyright header start
// //////////////////////
//
// PROPRIETARY / CONFIDENTIAL.
// Use of this product is subject to license terms.
// Copyright © 2004 Symantec Corporation.
// All rights reserved.
//
// /////////////////////////////////////////////////////////////////////////////////////////////////
// Symantec copyright header stop
// //////////////////////
// OSInfo.h: interface for the COSInfo class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_OSINFO_H__73C0B1AD_AAEC_443D_B45B_C50DCEA3F0E8__INCLUDED_)
#define AFX_OSINFO_H__73C0B1AD_AAEC_443D_B45B_C50DCEA3F0E8__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "windows.h"

class COSInfo  
{
public:
	COSInfo();
	
	enum {
		OS_TYPE_WIN9x = 0,
		OS_TYPE_WINNT,
		OS_TYPE_SOLARIS,
		OS_TYPE_LINUX,
        OS_TYPE_WINME,
        OS_TYPE_WINXP,
		OS_TYPE_UNKNOWN
	};
	
	static int GetOSType();

	// Any Windows 32 platform
    // 
    static bool IsWin32();

	// UNIX flavor
    //
    static bool IsUnix();
	
    // The dreaded NEC PC98 hardware
    //
    static bool IsNECPC98();

    // Windows NT or Windows 2000 or Whistler ( Windows NT kernel )
    //
    static bool IsWinNT();

    // Check if we're on a DBCS language system
    //
    static bool IsDBCS ();

    // Windows ME a.k.a. Windows Millenium
    //
    static bool IsWinME();

    // Windows XP, a.k.a. Whistler
    //
    static bool IsWinXP();

    // Check for terminal services.
	static bool IsTerminalServicesInstalled();

	// Returns true if called from a process with Administrative rights.  
	// On Win9x systems, this function always returns true;
	static bool IsAdminProcess();

	// Returns true if called from a processed started in the currently 
	// active terminal services session running on the console.  On Win9x systems,
	// this function always returns true.
	static bool IsActiveSession();
	static bool IsActiveSession(DWORD& dwError);

    // Returns the current session ID.
    static DWORD GetSessionID();
    
private:
	static int m_iOSType;
	static bool m_bInitialized;
	static bool m_bIsNEC;
	static bool m_bIsTerminalServicesInstalled;
    static DWORD m_dwSessionID;
    
    bool isTerminalServicesInstalled();
    BOOL validateProductSuite (LPSTR lpszSuiteToValidate);
};

#endif // !defined(AFX_OSINFO_H__73C0B1AD_AAEC_443D_B45B_C50DCEA3F0E8__INCLUDED_)
