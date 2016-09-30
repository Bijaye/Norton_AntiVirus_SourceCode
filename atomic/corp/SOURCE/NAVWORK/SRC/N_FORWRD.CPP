// Copyright 1998 Symantec, Peter Norton Product Group
//************************************************************************
//
// $Header:   S:/NAVWORK/VCS/n_forwrd.cpv   1.1   17 Jun 1998 23:48:16   tcashin  $
//
// Description:
//
// Contains:
//
// See Also:
//************************************************************************
// $Log:   S:/NAVWORK/VCS/n_forwrd.cpv  $
// 
//    Rev 1.1   17 Jun 1998 23:48:16   tcashin
// Improvements to the forward-to-quarantine thread and dialog.
// 
//    Rev 1.0   03 Jun 1998 18:50:22   tcashin
// Initial revision.
// 
//************************************************************************

#include "platform.h"
#include <windowsx.h>
#include <process.h>
#include "nonav.h"
#include "xapi.h"
#include "ngui.h"
#include "stdhelp.h"
#include "ctl3d.h"

#ifdef SYM_WIN16
    #include "syminteg.h"
#endif

#include "navgraph.hpp"
#include "stack.hpp"

#include "stub.h"
#include "virscan.h"
#include "symcfg.h"
#include "navutil.h"
#include "options.h"
#include "inoc.h"
#include "navw_hlp.h"
#include "cmbstr.h"
#include "nunavdlg.h"
#include "nunavstb.h"
#include "navwork.h"
#include "iquaran.h"

extern  HINSTANCE       hInstance;

// ************************************************************************ //
// ****                      Local Prototypes                          **** //
// ************************************************************************ //


BOOL CALLBACK WINAPI ForwardDlgProc (
    HWND        hDlg,
    UINT        message,
    WPARAM      wParam,
    LPARAM      lParam
    );

VOID LOCAL DoInitDialog (
    LPHANDLE    phDlg,
    LPARAM      lParam,
    LPHANDLE    phThread
    );

VOID LOCAL PaintMyIcon (
    HWND        hDlg
    );

DWORD WINAPI ForwardFilesToQuarantineServer ( LPVOID lpvThreadParam );

BOOL WINAPI ForwardFilesProgressCallback (LPDWORD lpSent, LPDWORD lpTotalToSend);


// ************************************************************************ //
// ****                      Local Data                                **** //
// ************************************************************************ //
static  HANDLE          hThread;
static  HANDLE          hStopForwardingEvent;
static  HWND            hForwardDlg;
static  HWND            hFillbar;
static  IQuarantineDLL* pQuarDLL;
static  DWORD           dwThreadId;
static  BOOL            bStopforwarding;


//************************************************************************
// ForwardFiles()
//
// This routine handle the process of forwarding files to a quarantine server.
//
// Parameters:
//      hWnd            [in] Window handle of main dialog window
//      lpNav           [in] 
//
// Returns:
//
//************************************************************************
// 6/6/98 TCashin Function created.
//************************************************************************

UINT SYM_EXPORT WINAPI ForwardFiles(HWND hDlg, LPNONAV lpNav)
{
    auto    HRESULT         hr;    
    auto    BOOL            bNeedToForwardFiles = FALSE;
    auto    BOOL            bApisWereAnsi;

#ifdef SYM_WIN32
    bApisWereAnsi = AreFileApisANSI ();

    if (!bApisWereAnsi)
       SetFileApisToANSI ();
#endif

    CoInitialize(NULL);

    hr = CoCreateInstance( CLSID_QuarantineDLL,
                           NULL,
                           CLSCTX_INPROC_SERVER,
                           IID_QuarantineDLL,
                           (LPVOID*)&pQuarDLL);

    if( SUCCEEDED( hr ) )
    {
        hr = pQuarDLL->Initialize();

        if ( SUCCEEDED( hr ) )
        {
            
            hr = pQuarDLL->NeedToForwardFiles(&bNeedToForwardFiles);
          
            pQuarDLL->Release();
        }
    }

    CoUninitialize();

    if ( bNeedToForwardFiles )
    {
        EnableWindow(hDlg, FALSE);

        DialogBoxParam (hInstance, MAKEINTRESOURCE(IDD_FORWARDFILES),
                        hDlg, ForwardDlgProc, (LPARAM)lpNav);

        EnableWindow(hDlg, TRUE);
    }

#ifdef SYM_WIN32
    if (!bApisWereAnsi)
        SetFileApisToOEM ();
#endif

    return (0);
}


//************************************************************************
// ForwardDlgProc
//
// This is the dialog proc Forward Files dialog.
//
// Parameters:
//      hDlg            [in] Standard Windows Callback Stuff
//      message         [in] 
//      wParam          [in]
//      lParam          [in]
//
// Returns:
//
//************************************************************************
// 6/6/98 TCashin Function created.
//************************************************************************
BOOL CALLBACK WINAPI ForwardDlgProc (
    HWND    hDlg,
    UINT    message,
    WPARAM  wParam,
    LPARAM  lParam
    )
{
    static  BOOL        bPaintProcessed;
    auto    LPNONAV     lpNav;
    auto    UINT        i = 0;
    auto    BOOL        bReturn = FALSE;

    switch(message)
        {
        case WM_CTLCOLOR:
            bReturn = (BOOL)Ctl3dCtlColorEx(message, wParam, lParam);
            break;

        case WM_INITDIALOG:
            NG_SetFontsInDialog (hDlg, 0);
            lpNav = (LPNONAV)lParam;

            hForwardDlg = hDlg;
            bPaintProcessed     = FALSE;
            DoInitDialog((LPHANDLE)&hForwardDlg, lParam, &hThread);

#ifdef NAVSCANW
            if (SYM_SYSTEM_WIN_NT == SystemGetWindowsType())
                NG_SetGroupBoxSunken(hDlg, ID_WRITELOG_BOX);
            else
                ShowWindow(GetDlgItem(hDlg, ID_WRITELOG_BOX), SW_HIDE);
#endif
            bReturn = TRUE;
            break;

        case WM_PAINT:
            if ( bPaintProcessed == FALSE )
                {
                bPaintProcessed = TRUE;
                PostMessage(hDlg, WM_NAVWORKUSER, 0, 0L);
                }

            if (IsIconic(hDlg))
                PaintMyIcon(hDlg);
            break;

        case WM_NAVWORKUSER:
            UpdateWindow(hDlg);

            break;

        case WM_DESTROY:
        case WM_CLOSE:

            Fillbar_Close(GetDlgItem(hDlg, ID_FORWARDFILE_FILLBAR));

            EndDialog(hDlg, 0);
            LP_FREE(hDlg);

            break;

        case WM_COMMAND:

            switch ( GET_WM_COMMAND_ID(wParam,lParam) )
                {
                case IDCANCEL:
                    
                    bStopforwarding = TRUE;

                    Fillbar_Close(GetDlgItem(hDlg, ID_FORWARDFILE_FILLBAR));

                    EndDialog(hDlg, 0);

                    hForwardDlg = NULL;

                    break;
                }
            break;
        }

   return ( bReturn );
}

//************************************************************************
// PaintMyIcon
//
// Paint NAVs icon.
//
// Parameters:
//      hDlg              [in] Window handle
//
// Returns:
//      None
//************************************************************************
// 6/6/98 TCashin Function created.
//************************************************************************
VOID LOCAL PaintMyIcon (HWND hDlg)
{
    auto    PAINTSTRUCT     ps;
    auto    HICON           hIcon;

    BeginPaint    (hDlg, &ps);

    hIcon = LoadIcon (hInstance, MAKEINTRESOURCE(ID_ICON_NAV0));

    DrawIcon (ps.hdc, 0, 0, hIcon);

    if (hIcon)
        DestroyIcon (hIcon);

    EndPaint (hDlg, &ps);
}


//************************************************************************
// DoInitDialog
//
// Initialize the Forward Files dialog. Starts the thread that does the 
// forwarding.
//
// Parameters:
//      phDlg           [in]  Window handle
//      lParam          [in]  lpNav
//      phThread        [out] Thread handle
//
// Returns:
//      None
//************************************************************************
// 6/6/98 TCashin Function created.
//************************************************************************
VOID LOCAL DoInitDialog (LPHANDLE phDlg, LPARAM lParam, LPHANDLE phThread)
{
    auto    FILLBARSTYLE    fbStyle;
    auto    LPNONAV         lpNav;

    Ctl3dSubclassDlg((HWND)*phDlg, CTL3D_ALL);

    NAV_CenterDialog ((HWND)*phDlg);

    lpNav = (LPNONAV)lParam;

    hFillbar = GetDlgItem((HWND)*phDlg, ID_FORWARDFILES_FILLBAR);

    fbStyle.nStruclen         = sizeof(fbStyle);
    fbStyle.b3D               = TRUE;
    fbStyle.nLeftTopWidth     = FB_USEDEFAULT;
    fbStyle.nRightBottomWidth = FB_USEDEFAULT;
    fbStyle.crColor           = RGB(255, 0, 0);

    Fillbar_SetRange(hFillbar, 0, 100);
    Fillbar_Show(hFillbar, &fbStyle);

    // ForwardFilesToQuarantineServer() no longer runs in a thread.  The
    // NAVWORK DLL was completing before the thread finished, causing the
    // thread to abnormally terminate.
    /*
    *phThread = CreateThread( NULL, 
                              0, 
                              (LPTHREAD_START_ROUTINE)ForwardFilesToQuarantineServer, 
                              NULL, 
                              0, 
                              &dwThreadId);
    */

    ForwardFilesToQuarantineServer(NULL);

    if (IsIconic(GetParent((HWND)*phDlg)))
        ShowWindow((HWND)*phDlg, SW_SHOWMINIMIZED);
}


//************************************************************************
// ForwardFilesToQuarantineServer
//
// Call ForwardAllFiles. When it returns,post a message to close the Forward 
// Files dialog and close down the thread.
//
// Parameters:
//
//      lpvThreadParam  [in] Window handle of main dialog window
//
// Returns:
//
//************************************************************************
// 6/6/98 TCashin Function created.
//************************************************************************
DWORD WINAPI ForwardFilesToQuarantineServer ( LPVOID lpvThreadParam )
{
    auto    HRESULT         hr;    
    auto    IQuarantineDLL* p;

    CoInitialize(NULL);

    hr = CoCreateInstance( CLSID_QuarantineDLL,
                           NULL,
                           CLSCTX_INPROC_SERVER,
                           IID_QuarantineDLL,
                           (LPVOID*)&p);

    if( SUCCEEDED( hr ) )
    {
        hr = p->Initialize();

        if ( SUCCEEDED( hr ) )
        {            
            bStopforwarding = FALSE;

            p->ForwardAllFiles(&ForwardFilesProgressCallback);          
            p->Release();
        }
    }

    CoUninitialize();

    PostMessage(hForwardDlg, WM_COMMAND, IDCANCEL, 0L);    

    hThread = NULL;

    return (0);
}


//************************************************************************
// ForwardFilesProgressCallback
//
// Parameters:
//
// Returns:
//          TRUE:   Continue forwarding files
//          FALSE:  Stop forwarding
//
//************************************************************************
// 6/8/98 TCashin Function created.
//************************************************************************
BOOL WINAPI ForwardFilesProgressCallback (LPDWORD lpSent, LPDWORD lpTotalToSend)
{
    auto    DWORD   dwPercent = 0;

    dwPercent = ((DWORD)*lpSent * 100) / *lpTotalToSend;

    if ( hForwardDlg )
    {
        Fillbar_SetLevel (GetDlgItem(hForwardDlg, ID_FORWARDFILES_FILLBAR),
                          (UINT)dwPercent);

        PostMessage(hForwardDlg, WM_NAVWORKUSER, 0, 0L);
    }

    if ( bStopforwarding )
        return (FALSE);
    else
        return (TRUE);
}
