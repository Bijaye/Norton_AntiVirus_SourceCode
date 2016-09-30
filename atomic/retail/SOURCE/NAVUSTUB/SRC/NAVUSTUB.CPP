/////////////////////////////////////////////////////////////////////////////
//  Copyright (c)1998 - Symantec, Peter Norton Computing Group
/////////////////////////////////////////////////////////////////////////////
//
//  $Header:   S:/NAVUSTUB/VCS/Navustub.cpv   1.3   15 Jan 1999 18:05:40   mdunn  $
//
//  NavUStub.CPP -  Stub to record uninstall command line switches and
//                  then launch UninstallShield's EXE.
//
/////////////////////////////////////////////////////////////////////////////
//  $Log:   S:/NAVUSTUB/VCS/Navustub.cpv  $
// 
//    Rev 1.3   15 Jan 1999 18:05:40   mdunn
// Change for NSW - we now wait on the UninstallShield process, so that NSW
// knows when our uninstall procedure is done.
// 
//    Rev 1.2   15 Oct 1998 21:27:48   mdunn
// NT uses a different reg key.... Added that.
//
//    Rev 1.1   15 Oct 1998 18:02:36   mdunn
// Just thought of something - add in code to handle the case where we have
// no command line (like when the program is double-clicked from Explorer).
//
//    Rev 1.0   15 Oct 1998 17:53:48   mdunn
// Initial revision.
/////////////////////////////////////////////////////////////////////////////

#include <windows.h>
#include <windowsx.h>
#include <tchar.h>
#include "resource.h"

#define NAV9X_KEY _T("Software\\Symantec\\Norton AntiVirus")
#define NAVNT_KEY _T("Software\\Symantec\\Norton AntiVirus NT")

int WINAPI WinMain ( HINSTANCE hInst, HINSTANCE hPrevInst,
                     LPSTR lpCmdLine, int nCmdShow )
{
DWORD  dwSilent = 0;
BOOL   bOSIsNT;
HKEY   hkey;
DWORD  dwDisp;                          // disposition - not used by us
TCHAR  szWinDir [MAX_PATH];
STARTUPINFO         rSI;
PROCESS_INFORMATION rPI;

    // Check if we were launched with no cmd line, as when a curious
    // user double-clicks the program.

    if ( '\0' == *lpCmdLine )
        {
        return 0;                       // silently quit
        }

    ZeroMemory ( &rSI, sizeof(rSI) );
    ZeroMemory ( &rPI, sizeof(rPI) );
    rSI.cb = sizeof(rSI);

    GetWindowsDirectory ( szWinDir, MAX_PATH );

    // Check what OS we're on.
    bOSIsNT = (GetVersion() & 0x80000000UL) ? FALSE : TRUE;

    // We consider uninstall silent with the -y switch (don't ask "are you
    // sure?" before beginning) and -a switch (don't delete any shared files
    // w/usage count 0) both present.

    if ( NULL != _tcsstr ( lpCmdLine, _T(" -a") ) &&
         NULL != _tcsstr ( lpCmdLine, _T(" -y") )    )
        {
        dwSilent = 1;
        }


    // Write the silent uninstall flag to the registry.  This will be read
    // later on by the custom install DLL.

    if ( ERROR_SUCCESS ==
             RegCreateKeyEx ( HKEY_LOCAL_MACHINE,
                              bOSIsNT ? NAVNT_KEY : NAV9X_KEY,
                              0, _T(""), REG_OPTION_NON_VOLATILE,
                              KEY_SET_VALUE, NULL, &hkey, &dwDisp ) )
        {
        RegSetValueEx ( hkey, "Silent Uninstall", 0, REG_DWORD,
                        (const BYTE*) &dwSilent, sizeof(DWORD) );

        RegCloseKey ( hkey );
        }


    // Now launch the real uninstaller.

    if ( CreateProcess ( NULL, lpCmdLine, NULL, NULL, FALSE,
                         NORMAL_PRIORITY_CLASS, NULL,
                         szWinDir, &rSI, &rPI ) )
        {
        // Wait on the process we just launched.  This is here for SystemWorks.
        // NSW's uninstaller launches us and waits on our process, so 
        // the stub can't terminate until the uninstall procedure is done.

        WaitForSingleObject ( rPI.hProcess, INFINITE );

        CloseHandle ( rPI.hThread );
        CloseHandle ( rPI.hProcess );
        }
    else
        {
        TCHAR szCaption [128];
        TCHAR szMsg [1024];

        LoadString ( hInst, IDS_CAPTION, szCaption, 128 );
        LoadString ( hInst, IDS_FAILED_LAUNCH, szMsg, 1024 );

        MessageBox ( GetDesktopWindow(), szMsg, szCaption, MB_ICONSTOP );
        }

    return 0;
}

