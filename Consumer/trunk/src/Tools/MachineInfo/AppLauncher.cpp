////////////////////////
//
// PROPRIETARY / CONFIDENTIAL.
// Use of this product is subject to license terms.
// Copyright © 2006 Symantec Corporation.
// All rights reserved.
//
/////////////////////////////////////////////////////////////////////////////////////////////////

// AppLauncher.cpp: implementation of the CAppLauncher class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "MachineInfo.h"
#include "AppLauncher.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CAppLauncher::CAppLauncher()
{

}

CAppLauncher::~CAppLauncher()
{

}

bool CAppLauncher::LaunchAppAndWait ( LPCTSTR lpcszFileName, LPCTSTR lpcszParameters )
{
    HINSTANCE hInst = 0;
    bool bReturn = false;

	// Register the service
	SHELLEXECUTEINFO Info = {0};
	Info.fMask = SEE_MASK_NOCLOSEPROCESS;
	Info.cbSize = sizeof(Info);
	Info.hwnd = NULL;
	Info.lpFile = lpcszFileName;
	Info.lpParameters = lpcszParameters;
	Info.nShow = SW_NORMAL;

	if ( ShellExecuteEx(&Info))
    {
        bReturn = true;

        // Wait for it to finish

	    while (WaitForSingleObject(Info.hProcess, 100) == WAIT_TIMEOUT)
	    {
            
		    MSG msg = {0};
		    while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		    {
			    TranslateMessage(&msg);
			    DispatchMessage(&msg);
		    };
            
	    }
    }

    CloseHandle(Info.hProcess);
    return bReturn;
}
