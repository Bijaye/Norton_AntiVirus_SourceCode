// Copyright 1994 Symantec, Peter Norton Product Group
//************************************************************************
//
// $Header:   S:/NAVWORK/VCS/n_dll.cpv   1.5   18 May 1998 22:15:42   tcashin  $
//
// Description:
//
// Contains:
//
// See Also:
//************************************************************************
// $Log:   S:/NAVWORK/VCS/n_dll.cpv  $
// 
//    Rev 1.5   18 May 1998 22:15:42   tcashin
// Added quarantine-related defines/includes for the NAVWORK DLL.
// 
//    Rev 1.4   09 Mar 1998 23:41:24   DALLEE
// Removed DllMain.
// NAVWORK is now using MFC's for the new RepairWizard.
// For old DllMain-type functionality, please plug in to CNavworkApp.
// 
// 
//    Rev 1.3   15 Jul 1997 14:36:42   DBUCHES
// Added Ctl3d support for DEC Alpha
// 
//    Rev 1.2   22 May 1997 16:20:40   BILL
// Alpha changes (removed ctl3d)
// 
//    Rev 1.1   24 Apr 1997 11:13:08   JBRENNA
// Tesla port: DSACKIN r1.1:
//  Initialize a critical section that will be used in our RPC connections.
// 
//    Rev 1.0   06 Feb 1997 21:02:42   RFULLER
// Initial revision
// 
//    Rev 1.0   30 Jan 1996 15:55:34   BARRY
// Initial revision.
// 
//    Rev 1.0   30 Jan 1996 14:50:38   BARRY
// Initial revision.
// 
//    Rev 1.17   03 Jul 1995 16:22:26   MARKL
// Cleaned up icons and their painting.
// 
//    Rev 1.16   09 May 1995 10:30:36   KEITH
// Correct some nouns
// 
//************************************************************************

#include "platform.h"
#include "ngui.h"
#include "file.h"
#include "disk.h"
#include "ctl3d.h"

#ifdef SYM_WIN16
    #include "syminteg.h"
#endif

#ifdef SYM_WIN32
    #ifndef INITGUID
        #define INITGUID
    #endif

    #include "objbase.h"
    #include "iquaran.h"
#endif

#include "stub.h"
#include "cmbstr.h"
#include "symcfg.h"
#include "navutil.h"
#include "options.h"
#include "nonav.h"
#include "nunavdlg.h"
#include "nunavstb.h"

HINSTANCE       hInstance;
CALLBACKREV1    Init;

static UINT uNumUsers = 0;

// --------------------------------------------------------------------------
//                      LOCAL FUNCTION PROTOTYPES
// --------------------------------------------------------------------------

WORD LOCAL ScanWinRegister ( VOID );

// -------------------------------------------------------------------------
// -------------------------------------------------------------------------

#ifdef SYM_WIN32

/****************************************************************************
   FUNCTION: DllMain

   PURPOSE:  Called when a new process is either using or not using
             this DLL anymore.

*******************************************************************************/

// No longer have DllMain - MFC will do this for us. If necessary,
// plug in to CNavworkApp::InitInstance() and CNavworkApp::ExitInstance()

#endif

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

VOID SYM_EXPORT WINAPI NavworkInit (
    VOID
    )
{
#ifdef SYM_WIN32
extern CRITICAL_SECTION rRpcCritical;
#endif

    if ( uNumUsers == 0 )
        {
        // Init string library bs
            
        String_Init(hInstance, NULL);

        // Register scan dll window class.

        ScanWinRegister();

        // Initialize certlib that this dll is linked with.

#ifdef _X86_
        InitCertlib ( &Init, INIT_CERTLIB_USE_DEFAULT );
#endif        

        // We must initialize our critical section before any functionality
        // is used.

        InitializeCriticalSection(&rRpcCritical);
        }

    uNumUsers++;
}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

VOID SYM_EXPORT WINAPI NavworkEnd (
    VOID
    )
{
    if ( uNumUsers )
        uNumUsers --;

    // If number of users hit zero free up all the junk that we collected.

    if ( uNumUsers == 0 )
        {
        String_End();
        }
}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

WORD LOCAL ScanWinRegister ( VOID )
{
#ifdef SYM_WIN32
   WNDCLASSEX     WndClass      = {0};
#else
   WNDCLASS       WndClass      = {0};
#endif

   WndClass.style              = CS_HREDRAW | CS_VREDRAW;
   WndClass.lpfnWndProc        = (WNDPROC)ScanDlgProc;
   WndClass.cbWndExtra         = DLGWINDOWEXTRA;
   WndClass.hInstance          = hInstance;
   WndClass.hCursor            = NULL;
   WndClass.hbrBackground      = (HBRUSH)(COLOR_BTNFACE + 1);
   WndClass.lpszMenuName       = NULL;
   WndClass.lpszClassName      = NAV_SCANWINDOW_CLASS;

#ifdef SYM_WIN32
    WndClass.cbSize             = sizeof(WNDCLASSEX);
    WndClass.hIconSm            = (HICON)LoadImage (
                                        hInstance,
                                        MAKEINTRESOURCE(ID_ICON_NAV0),
                                        IMAGE_ICON,
                                        16, 16, 0);
    WndClass.hIcon              = (HICON)LoadImage (
                                        hInstance,
                                        MAKEINTRESOURCE(ID_ICON_NAV0),
                                        IMAGE_ICON,
                                        32, 32, 0);
   return (RegisterClassEx (&WndClass));
#else
   WndClass.hIcon              = LoadIcon (hInstance, 
                                           MAKEINTRESOURCE (ID_ICON_NAV0));
   return (RegisterClass (&WndClass));
#endif
}

