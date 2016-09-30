////////////////////////
//
// PROPRIETARY / CONFIDENTIAL.
// Use of this product is subject to license terms.
// Copyright © 2006 Symantec Corporation.
// All rights reserved.
//
/////////////////////////////////////////////////////////////////////////////////////////////////

// OSInfo.h: interface for the COSInfo class.
//
//////////////////////////////////////////////////////////////////////
#pragma once
#include "windows.h"

class COSInfo  
{
public:
	COSInfo();
	
	enum {
		OS_TYPE_WIN9x = 0,
		OS_TYPE_WINNT,
        OS_TYPE_WINME,
        OS_TYPE_WINXP,
		OS_TYPE_UNKNOWN,
        OS_TYPE_WINVISTA,
	};
	
	static int GetOSType();

	// Any Windows 32 platform
    // 
    static bool IsWin32();

	// UNIX flavor
    //
    static bool IsUnix();
	
    // true for Windows NT, Windows 2000, XP, or Vista ( Windows NT kernel )
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

    // Windows Vista, a.k.a. Longhorn
    static bool IsWinVista();

    // Check for terminal services.
	static bool IsTerminalServicesInstalled();

	// Returns true if called from a process with Administrative rights.  
	// On Win9x systems, this function always returns true;
	static bool IsAdminProcess();

	// Returns true if called from a process with Guest rights.  
	// On Win9x systems, this function always returns false;
	static bool IsGuest();

    // Returns the current session ID.
    static DWORD GetSessionID();
    
private:
	static int m_iOSType;
	static bool m_bInitialized;
	static bool m_bIsTerminalServicesInstalled;
    static DWORD m_dwSessionID;
    
    bool isTerminalServicesInstalled();
    BOOL validateProductSuite (LPSTR lpszSuiteToValidate);
};
