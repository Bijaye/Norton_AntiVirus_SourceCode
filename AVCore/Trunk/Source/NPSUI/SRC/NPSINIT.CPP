/* Copyright 1991-1993 Symantec, Peter Norton Computing Group           */
/************************************************************************/
/*                                                                      */
/* $Header:   S:/NPSUI/VCS/NPSINIT.CPv   1.1   17 Apr 1998 12:17:06   mdunn  $ */
/*                                                                      */
/* Description:                                                         */
/*      This module contains the initialization routines for the        */
/*      Norton Program Scheduler.                                       */
/*                                                                      */
/* Contains:                                                            */
/*                                                                      */
/************************************************************************/
/* $Log:   S:/NPSUI/VCS/NPSINIT.CPv  $ */
// 
//    Rev 1.1   17 Apr 1998 12:17:06   mdunn
// #undef'ed TRACE since it now collides with an MFC function.
// 
//    Rev 1.0   19 May 1997 00:25:08   KSACKIN
// Initial revision.
// 
//    Rev 1.13   30 Sep 1996 11:39:26   JBRENNA
// Discovered that STRNCPY does not always add an EOS when done copying.
// Now add one manually.
// 
//    Rev 1.12   25 Sep 1996 14:25:00   JBRENNA
// 1. Change NpsInitButtonBar() to determine the maximum button width as it
//    is adding the buttons to the button bar.
// 2. Change NpsResetButtonBar() to use either the dynamically calculated Btn
//    width (uButtonMaxWidth) or the hard-coded button width ... whichever
//    is larger. The calc-ed button width is more accurate. However, makes
//    the buttons thinner than they where originally. By using
//    uButtonMaxWidth, the button will grow if any DBCS text is wider than
//    the hard-coded value.
// 3. When NpsResetButtonBar() moves the ButtonBar, I changed it to match
//    the calculate uHeight.
// 4. Added UpdateMaxBtnWidthHeight() to dynamically calc the button width
//    and height.
// 
//    Rev 1.11   18 Sep 1996 10:16:28   JBRENNA
// Add NG_SetFontsInDialog() call when processing WM_INITDIALOG messages.
// This ensures that a DBCS font is used when necessary.
// 
//    Rev 1.10   22 Feb 1996 13:26:14   YVANG
// Renamed 'nstdhelp.h' to 'stdhelp.h' to use Quake's StdHelp merged with the
// NAV version.
// 
//    Rev 1.9   11 Jan 1996 09:49:12   JWORDEN
// Port changes from Quake 8
// 
//    Rev 1.8.1.1   30 Nov 1995 21:03:20   BARRY
// Renamed stdhelp.h to nstdhelp.h to fix trunk and local infodesk problem
// 
//    Rev 1.8   29 Jun 1995 22:21:54   BARRY
// Added missing semicolon.
// 
//    Rev 1.7   28 Jun 1995 18:10:18   KEITH
// Add asserts, and fix an invalidate rect on the desktop!
// 
//    Rev 1.6   26 Jun 1995 15:18:18   MARKL
// Fixed defect 38667 by supplying mask bitmaps.
// 
//    Rev 1.5   19 Jun 1995 15:51:38   GWESTER
// Fixed problem of invisible button bar.
// 
// 
//    Rev 1.4   07 Jun 1995 12:54:46   MARKL
// If there is no Win95 tray, then show our icon.
// 
//    Rev 1.3   03 May 1995 13:19:34   MFALLEN
// Moved stdhelp.h higher up in the include order.
// 
//    Rev 1.2   28 Apr 1995 19:20:20   BRAD
// Try RegisterClassEx() first
// 
//    Rev 1.1   25 Apr 1995 18:33:44   BRAD
// Class registration will now work for NT
// 
//    Rev 1.0   14 Mar 1995 13:01:26   MFALLEN
// Initial revision.
// 
//    Rev 1.21   10 Mar 1995 18:23:00   MFALLEN
// Fixed GPF
// 
//    Rev 1.20   06 Mar 1995 10:11:46   MFALLEN
// Made it work for the Win95 platform and added required enhancements to
// the product to give it a native Win95 look and feel.
// 
//    Rev 1.19   27 Oct 1994 14:51:22   BRAD
// Added DBCS button bar sizes
// 
//    Rev 1.18   26 Mar 1994 21:00:30   BRAD
// Cleaned up Windows messages
// 
//    Rev 1.17   15 Mar 1994 11:58:14   BRUCE
// Changed SYM_EXPORT to SYM_EXPORT
// 
//    Rev 1.16   09 Mar 1994 13:04:48   BRAD
// Cleaned up for SYM_WIN32
// 
//    Rev 1.15   03 Mar 1994 12:09:46   BRAD
// AWK script applied
// 
//    Rev 1.14   10 Feb 1994 12:18:52   PGRAVES
// Add QAK2 changes to trunk.
// 
//    Rev 1.12.1.2   13 Dec 1993 18:42:54   MARKL
// Removed ifdefs for NDW
// 
//    Rev 1.12.1.1   25 Oct 1993 13:47:42   BASIL
// [FIX] fixed problem where destop loses focus at startup when scheduler
// is set to load with windows and hidden when minimized.
// 
//    Rev 1.12.1.0   19 Oct 1993 08:14:44   SCOTTP
// Branch base for version QAK2
// 
//    Rev 1.12   12 Oct 1993 16:07:50   BASIL
// Removed config button for Revolver
// 
//    Rev 1.11   29 Sep 1993 14:53:36   MARKL
// No change.
// 
//    Rev 1.10   11 Aug 1993 16:41:02   MARKL
// Now centers main dialog correctly.  Does not center the icon.
// 
//    Rev 1.9   23 Jul 1993 10:28:18   MARKL
// Help menu now looks like nirvana's.  Removed help/exit buttons.  All
// child dialogs are centered.
// 
//    Rev 1.8   22 Jul 1993 12:02:18   MARKL
// Hooked up quake help system.
// 
//    Rev 1.7   07 Jul 1993 19:01:30   KYECK
// Aligned code in NpsResetButtonBar().
//
//    Rev 1.6   10 Jun 1993 17:09:30   KYECK
// Moved 'exit' button to end of button bar.
//
//    Rev 1.5   10 Jun 1993 09:32:20   KYECK
// Added support for buttonbar.
//
//    Rev 1.4   11 May 1993 20:08:20   MARKL
// Added a new icon such that scheduler no longer displays a working
// clock when iconized.  The application simply shows the icon and
// the current time.
//
//    Rev 1.3   22 Apr 1993 11:29:34   MARKL
// Fixed bugs where parameters to NMsgBox() (a variable argument function)
// were not being typecasted correctly.
//
//    Rev 1.2   21 Apr 1993 22:05:00   MARKL
// Nirvana gives the schedule a chance to take an a radically new look/feel
// in the Add/Edit dialog!!!
//
//    Rev 1.1   18 Mar 1993 20:32:24   MARKL
// Cleaned up unnecessary typecasting of LPSTR.
//
//    Rev 1.0   15 Mar 1993 17:52:04   MARKL
// Initial revision.
//
//    Rev 1.3   26 Jun 1992 17:55:20   MEIJEN
// Get rid of strids.h because NWRES will eventually disappear
//
//    Rev 1.2   19 Jun 1992 10:33:44   MEIJEN
// Decentralize resources from NWRES to NPS(scheduler) for REVOLVER
//
//    Rev 1.1   02 Oct 1991 12:06:44   MARKL
// Made WinStric.H pass.
//
//    Rev 1.0   28 Jul 1991 11:37:42   MARKL
// Initial revision.
//
//    Rev 1.1   06 Jun 1991  1:04:26   MARKL
// Added clock icon.
//
//    Rev 1.0   01 Jun 1991 20:33:38   MARKL
// Initial revision.
/************************************************************************/
#include    "platform.h"
#include    "stdhelp.h"
#include    "xapi.h"
#include    "msgdisp.h"                    /* error message stuff          */
#include    "ngui.h"
#include    "nps.h"
#include    "npsres.h"
#include    "npsstb.h"
#include    "dbcs.h"

#undef TRACE
#define TRACE   ERR_TRACE_ID (_NPSINIT_C)



/************************************************************************/
/* Local component prototypes                                           */
/************************************************************************/

/************************************************************************/
/* NPSINIT routines                                                     */
/************************************************************************/

static void UpdateMaxBtnWidthHeight (const HWND         hWnd,
                                     const LPBUTTONDATA lpButtonData,
                                     LPUINT             lpuCurMaxWidth,
                                     LPUINT             lpuCurMaxHeight = NULL);


//****************************************************************************
// DlgUnitsToPixelsX
//
// Converts Dialog Units to Pixels for the X axis.
//****************************************************************************
inline UINT DlgUnitsToPixelsX (const UINT uDlgUnits)
{
    return (uDlgUnits * LOWORD (GetDialogBaseUnits ())) / 4;
}

//****************************************************************************
// DlgUnitsToPixelsY
//
// Converts Dialog Units to Pixels for the Y axis.
//****************************************************************************
inline UINT DlgUnitsToPixelsY (const UINT uDlgUnits)
{
    return (uDlgUnits * HIWORD (GetDialogBaseUnits ())) / 8;
}

/************************************************************************/
/* Description:                                                         */
/*      This routine registers the window class for the Norton          */
/*      Program Scheduler (NPS) application.                            */
/*                                                                      */
/* Return values:                                                       */
/*      NOERR                           - Everything went fine.         */
/*      NPS_REGISTER_CANT               - Error registering class.      */
/*                                                                      */
/************************************************************************/
/* 02/11/91 MSL Function created.                                       */
/* 02/28/91 MSL added NPS3D define to toggle 3D effect on and off.      */
/* 06/06/91 MSL When minimized, use clock icon.                         */
/* 05/11/93 MSL When minimized, use scheduler icon.                     */
/************************************************************************/
WORD PASCAL NpsRegisterClass (          /* Registers the NPS wnd class  */
    VOID)                               /* No parameters.               */
{
    auto    WORD        wRet = NOERR;
    auto    BOOL        bRegistered = FALSE;
#ifdef SYM_WIN32
    auto    WNDCLASSEX  wcEx;
#endif
    auto    WNDCLASS    wc;
    extern  HINSTANCE   ghInst;
    extern  char        szAppName[];


#if SYM_WIN32
    MEMSET (&wcEx, EOS, sizeof(wcEx));
                                    /* Set up our class structure   */
    wcEx.style            = CS_HREDRAW | CS_VREDRAW;
    wcEx.lpfnWndProc      = (WNDPROC)MainWndProc;
    wcEx.cbClsExtra       = 0;
    wcEx.cbWndExtra       = DLGWINDOWEXTRA;
    wcEx.hInstance        = ghInst;
    wcEx.hIcon            = LoadIcon (ghInst,
                                MAKEINTRESOURCE (IDI_NPS_ICON));
    wcEx.hCursor          = LoadCursor (NULL, IDC_ARROW);
    wcEx.hbrBackground    = (HBRUSH)(COLOR_WINDOW + 1);
    wcEx.lpszMenuName     = NULL;
    wcEx.lpszClassName    = szAppName;

                                    // -----------------------------------
                                    // Added elements for Windows 95:
                                    // -----------------------------------
    wcEx.cbSize       = sizeof(wcEx);
    wcEx.hIconSm      = (HICON) LoadImage (
                                    ghInst, 
                                    MAKEINTRESOURCE (IDI_NPS_ICON),
                                    IMAGE_ICON,
                                    16,
                                    16,
                                    0
                                    );
    if (RegisterClassEx (&wcEx))
        bRegistered = TRUE;
    else
#endif
        {
        MEMSET ( &wc, EOS, sizeof(wc) );
                                        /* Set up our class structure   */
        wc.style            = CS_HREDRAW | CS_VREDRAW;
        wc.lpfnWndProc      = (WNDPROC)MainWndProc;
        wc.cbClsExtra       = 0;
        wc.cbWndExtra       = DLGWINDOWEXTRA;
        wc.hInstance        = ghInst;
        wc.hIcon            = LoadIcon (ghInst,
                                    MAKEINTRESOURCE (IDI_NPS_ICON));
        wc.hCursor          = LoadCursor (NULL, IDC_ARROW);
        wc.hbrBackground    = (HBRUSH)(COLOR_WINDOW + 1);
        wc.lpszMenuName     = NULL;
        wc.lpszClassName    = szAppName;

        if (RegisterClass (&wc))
            bRegistered = TRUE;
        }

    if (!bRegistered)
        {
        wRet = NPS_REGISTER_CANT;
        NMsgBox (ghInst, NULL, wRet, TRACE, NULL, -1);
        }

                                        /* Return to caller             */
    return (wRet);
} /* NpsRegisterClass() */




/************************************************************************/
/* Description:                                                         */
/*      This routine activates the previous instance of this            */
/*      application for the user.  If the app is iconized it is         */
/*      restored to its normal size.  The previous instance is made the */
/*      active window.                                                  */
/*                                                                      */
/* Return values:                                                       */
/*      NOERR                           - Everything went fine.         */
/*                                                                      */
/************************************************************************/
/* 02/11/91 MSL Function created.                                       */
/* 03/06/91 MSL FindWindow() only goes after the caption since the      */
/*              caption changes on the fly.                             */
/************************************************************************/
WORD PASCAL NpsPrevInstActivate (       /* Registers the NPS wnd class  */
    VOID)                               /* No parameters.               */
{
    HWND        hWndPrevInst;           /* Handle to previous instance  */
    extern char szAppName[];
                                        /* Activate our previous        */
                                        /* instance.                    */
    hWndPrevInst = FindWindow (szAppName, NULL);

    if ( hWndPrevInst )
        {
        if ( IsIconic ( hWndPrevInst ) )
            {
            FORWARD_WM_COMMAND
                   ( hWndPrevInst, ID_TRAY_MENU_RESTORE, 0, 0, PostMessage );
            }
        else
            {
            ShowWindow ( hWndPrevInst, SW_SHOWNORMAL );
            }

#ifdef SYM_WIN16
        SetActiveWindow ( hWndPrevInst );
#else
        SetForegroundWindow ( hWndPrevInst );
#endif
        }

    return (NOERR);
} /* NpsPrevInstActivate() */




/************************************************************************/
/* Description:                                                         */
/*      This routine creates the main dialog box for the Norton Program */
/*      Scheduler.  Note that this dialog box serves as the main        */
/*      window for our application as opposed to an overlapped window.  */
/*      We also make sure that the scheduler comes up centered on the   */
/*      desktop.                                                        */
/*                                                                      */
/* Parameters:                                                          */
/*      LPHWND          lphDlg          - Buffer to return the handle   */
/*                                        of the dialog box created.    */
/*      int             nCmdShow        - How to initially display      */
/*                                        the dialog box.               */
/* Return values:                                                       */
/*      NOERR                           - Everything went fine.         */
/*      NPS_CREATE_DIALOG_CANT          - Error creating dialog box.    */
/*                                                                      */
/************************************************************************/
/* 02/12/91 MSL Function created.                                       */
/* 03/05/91 MSL Resources moved to NWRES.                               */
/************************************************************************/
WORD PASCAL NpsDialogCreate (           /* Creates Nps Dialog Box       */
    PHWND        phDlg,                 /* For returning handle to box  */
    int          nCmdShow)              /* How to initially display     */
{
    WORD                        wRet;
    extern      char            szAppName[];
    extern      HINSTANCE       ghInst;

                                        /* Initialization               */
    wRet = NOERR;

                                        /* Create the dialog box        */
    if (!(*phDlg = CreateDialog (ghInst,
                                 szAppName,
                                 0,
                                 NULL)))
        {
        wRet = NPS_CREATE_DIALOG_CANT;
        NMsgBox(ghInst,NULL, wRet, TRACE, NULL, -1, (LPSTR)szAppName);
        goto NpsDialogCreateExit;
        }

    NG_CenterDialog ( *phDlg, GetDesktopWindow() );

                                        // Setup the Fonts. Finds the correct
                                        // DBCS font if DBCSInstalled().
    NG_SetFontsInDialog (*phDlg, 0);

                                        /* Make the dialog box visible  */
                                        /* or hidden, as specified in   */
                                        /* the norton.ini file.         */

    if ((nCmdShow & SW_SHOWMINIMIZED) &&
        GetPrivateProfileInt (szTitle,
                              szHide,
                              FALSE,
                              szNortonIni))
        {
//&? MSL:  SETINTERNALWINDOWPOS WAS IN NDW.   WHAT SHOULD WE DO HERE
//&?    IN QUAKE?
//&?    pt.x = pt.y = SIWP_TWIGHLIGHTZONE;
//&?    SetInternalWindowPos (*phDlg, SW_MINIMIZE, NULL, &pt);

/*----------------------------------------------------------------------*
 * By showing the window using nCmdShow then hiding it, the focus will  *
 * be correctly returned to the destop.  This fixes the problem where   *
 * load with windows + Hide when iconized used to cause loss of focus   *
 * at startup. --BG 10/25/93                                            *
 *----------------------------------------------------------------------*/

        if (FindWindow ("Shell_TrayWnd", NULL) == NULL)
            {
            ShowWindow (*phDlg, nCmdShow);
            }
        else
            {
                                        // Hide if this is Win95 and the
                                        // tray is there.
            ShowWindow (*phDlg, SW_HIDE);
            }
        }
    else
        {
        ShowWindow ( *phDlg, nCmdShow );
        }
                                        /* Return to caller             */
NpsDialogCreateExit:
    return (wRet);
} /* NpsDialogCreate() */




//**********************************************************************
// Description:
//      This routine initializes the button bar control for the
//      main (dialog) window of the Norton Program Scheduler (NPS)
//      application.
//
// Return values:
//      NOERR                           - Everything went fine.
//
//**********************************************************************
// 06/03/93 KDY Function created.
// 09/24/96 JBRENNA - Call UpdateMaxBtnWidthHeight() to calculate the maximum
//          button width based on the text and bitmap sizes.
//**********************************************************************

WORD WINAPI NpsInitButtonBar (          // Initializes the button bar
    HWND        hDlg,                   // [in] Handle to dialog box
    HWND        hWndButtonBar,          // [in] handle to button bar
    UINT        uButtonStyle,           // [in] button bar style to use
    BOOL        bMoveTop                // [in] move button bar to top
    )
{
    UINT        uButtonMaxWidth  = 0;      // Button width for button bar
    BUTTONDATA  ButtonData;             // Button bar data structure
    HBITMAP     hBitmap;                // Handle to bitmap
    HBITMAP     hBitmapMask;            // Handle to bitmap mask
    BYTE        szText[SSIZE];          // Button bar text

                                        // -----------------------------------
                                        // If this is the first time,
                                        // set up the button bar such
                                        // the the following CONFIG are
                                        // supported:  Exit, Add, Edit,
                                        // Copy, Delete, Config, Help.
                                        // -----------------------------------

    ButtonBar_ResetContent ( hWndButtonBar );

                                        // -----------------------------------
                                        // Add 'Add' button.
                                        // -----------------------------------
    MEMSET ( &ButtonData, EOS, sizeof(ButtonData) );

    hBitmap     = LoadBitmap (ghInst, MAKEINTRESOURCE (IDB_NPS_BBAR_ADD));
    hBitmapMask = LoadBitmap (ghInst, MAKEINTRESOURCE (IDB_NPS_BBAR_ADD_MASK));
    ButtonData.wID           = IDC_NPS_ADD;
    ButtonData.wFlags        = BBFLAG_BITMAPS | BBFLAG_TEXT;
    ButtonData.hNormal       = hBitmap;
    ButtonData.hSelected     = hBitmap;
    ButtonData.hNormalMask   = hBitmapMask;
    ButtonData.hSelectedMask = hBitmapMask;
    LoadString ( ghInst, IDS_NPS_BBAR_ADD, (LPSTR)szText, sizeof (szText)-1 );
    ButtonData.lpszText      = (LPSTR)szText;
    ButtonBar_AddButton (hWndButtonBar, &ButtonData);
    UpdateMaxBtnWidthHeight (hWndButtonBar, &ButtonData, &uButtonMaxWidth);

                                        // -----------------------------------
                                        // Add 'Edit' button.
                                        // -----------------------------------
    MEMSET ( &ButtonData, EOS, sizeof(ButtonData) );

    hBitmap     = LoadBitmap (ghInst, MAKEINTRESOURCE (IDB_NPS_BBAR_EDIT));
    hBitmapMask = LoadBitmap (ghInst, MAKEINTRESOURCE (IDB_NPS_BBAR_EDIT_MASK));
    ButtonData.wID           = IDC_NPS_EDIT;
    ButtonData.wFlags        = BBFLAG_BITMAPS | BBFLAG_TEXT;
    ButtonData.hNormal       = hBitmap;
    ButtonData.hSelected     = hBitmap;
    ButtonData.hNormalMask   = hBitmapMask;
    ButtonData.hSelectedMask = hBitmapMask;
    LoadString ( ghInst, IDS_NPS_BBAR_EDIT, (LPSTR)szText,sizeof (szText)-1 );
    ButtonData.lpszText      = (LPSTR)szText;
    ButtonBar_AddButton (hWndButtonBar, &ButtonData);
    UpdateMaxBtnWidthHeight (hWndButtonBar, &ButtonData, &uButtonMaxWidth);

                                        // -----------------------------------
                                        // Add 'Copy' button.
                                        // -----------------------------------
    MEMSET ( &ButtonData, EOS, sizeof(ButtonData) );

    hBitmap     = LoadBitmap (ghInst, MAKEINTRESOURCE (IDB_NPS_BBAR_COPY));
    hBitmapMask = LoadBitmap (ghInst, MAKEINTRESOURCE (IDB_NPS_BBAR_COPY_MASK));
    ButtonData.wID           = IDC_NPS_COPY;
    ButtonData.wFlags        = BBFLAG_BITMAPS | BBFLAG_TEXT;
    ButtonData.hNormal       = hBitmap;
    ButtonData.hSelected     = hBitmap;
    ButtonData.hNormalMask   = hBitmapMask;
    ButtonData.hSelectedMask = hBitmapMask;
    LoadString ( ghInst, IDS_NPS_BBAR_COPY, (LPSTR)szText, sizeof (szText)-1 );
    ButtonData.lpszText      = (LPSTR)szText;
    ButtonBar_AddButton (hWndButtonBar, &ButtonData);
    UpdateMaxBtnWidthHeight (hWndButtonBar, &ButtonData, &uButtonMaxWidth);

                                        // -----------------------------------
                                        // Add 'Delete' button.
                                        // -----------------------------------
    MEMSET ( &ButtonData, EOS, sizeof(ButtonData) );

    hBitmap     = LoadBitmap (ghInst, MAKEINTRESOURCE (IDB_NPS_BBAR_REMOVE));
    hBitmapMask = LoadBitmap (ghInst, MAKEINTRESOURCE (IDB_NPS_BBAR_REMOVE_MASK));
    ButtonData.wID           = IDC_NPS_REMOVE;
    ButtonData.wFlags        = BBFLAG_BITMAPS | BBFLAG_TEXT;
    ButtonData.hNormal       = hBitmap;
    ButtonData.hSelected     = hBitmap;
    ButtonData.hNormalMask   = hBitmapMask;
    ButtonData.hSelectedMask = hBitmapMask;
    LoadString ( ghInst, IDS_NPS_BBAR_REMOVE, (LPSTR)szText, sizeof (szText)-1 );
    ButtonData.lpszText      = (LPSTR)szText;
    ButtonBar_AddButton (hWndButtonBar, &ButtonData);
    UpdateMaxBtnWidthHeight (hWndButtonBar, &ButtonData, &uButtonMaxWidth);

                                        // -----------------------------------
                                        // HACK. Quake button bar has some
                                        // problems. If all buttons do not fit
                                        // within child control area they do
                                        // not get trunkated just painted funny.
                                        // This if() tells that the button bar
                                        // is not for the main screen and if
                                        // only texts should be displayed, add
                                        // only 4 buttons. ( Sample button bar
                                        // on options screen. )
                                        // -----------------------------------

    if ( bMoveTop == TRUE || NPS_DISPLAY_BBAR_BITMAPS ( uButtonStyle ) )
        {
                                        // -----------------------------------
                                        // Add 'Help' button.
                                        // -----------------------------------
    
        MEMSET ( &ButtonData, EOS, sizeof(ButtonData) );

        hBitmap     = LoadBitmap (ghInst, MAKEINTRESOURCE (IDB_NPS_BBAR_HELP));
        hBitmapMask = LoadBitmap (ghInst, MAKEINTRESOURCE (IDB_NPS_BBAR_HELP_MASK));
        ButtonData.wID           = IDC_NPS_HELP;
        ButtonData.wFlags        = BBFLAG_BITMAPS | BBFLAG_TEXT;
        ButtonData.hNormal       = hBitmap;
        ButtonData.hSelected     = hBitmap;
        ButtonData.hNormalMask   = hBitmapMask;
        ButtonData.hSelectedMask = hBitmapMask;
        LoadString ( ghInst, IDS_NPS_BBAR_HELP, (LPSTR)szText,sizeof (szText)-1 );
        ButtonData.lpszText      = (LPSTR)szText;
        ButtonBar_AddButton (hWndButtonBar, &ButtonData);
        UpdateMaxBtnWidthHeight (hWndButtonBar, &ButtonData, &uButtonMaxWidth);

                                        // -----------------------------------
                                        // Add 'Exit' button.
                                        // -----------------------------------
        MEMSET ( &ButtonData, EOS, sizeof(ButtonData) );

        hBitmap     = LoadBitmap (ghInst, MAKEINTRESOURCE (IDB_NPS_BBAR_CLOSE));
        hBitmapMask = LoadBitmap (ghInst, MAKEINTRESOURCE (IDB_NPS_BBAR_CLOSE_MASK));
        ButtonData.wID           = IDC_NPS_CLOSE;
        ButtonData.wFlags        = BBFLAG_BITMAPS | BBFLAG_TEXT;
        ButtonData.hNormal       = hBitmap;
        ButtonData.hSelected     = hBitmap;
        ButtonData.hNormalMask   = hBitmapMask;
        ButtonData.hSelectedMask = hBitmapMask;
        LoadString ( ghInst, IDS_NPS_BBAR_CLOSE, (LPSTR)szText,sizeof (szText)-1 );
        ButtonData.lpszText      = (LPSTR)szText;
        ButtonBar_AddButton (hWndButtonBar, &ButtonData);
        UpdateMaxBtnWidthHeight (hWndButtonBar, &ButtonData, &uButtonMaxWidth);
        }

                                        // -----------------------------------
                                        // Set the base id.
                                        // -----------------------------------
    ButtonBar_SetBase(hWndButtonBar, ID_NPS_RANGE_FIRST);

                                        // -----------------------------------
                                        // Call the reset function to
                                        // make any adjustments based
                                        // on the current style.
                                        // -----------------------------------
    NpsResetButtonBar (
                    hDlg, hWndButtonBar,
                    uButtonStyle,
                    uButtonMaxWidth,
                    TRUE, bMoveTop
                    );

    return (NOERR);
} // NpsInitButtonBar()

//**********************************************************************
// Description:
//      This routine resets the button bar control based on the value of
//      uButtonStyle for the main (dialog) window of the Norton Program
//      Scheduler (NPS) application.
//
// Return values:
//      NOERR                           - Everything went fine.
//
//**********************************************************************
// 06/03/93 KDY Function created.
// 09/24/96 JBRENNA - Add the uButtonMaxWidth parameter. Modify the
//          routine to use uButtonMaxWidth when that value is larger than
//          the hard-coded width. This enables the buttons to grow if the
//          text (Japanese translated) is too big for the hard-coded width.
//**********************************************************************

WORD WINAPI NpsResetButtonBar (         // Resets the button bar
    HWND        hDlg,                   // [in] Handle to dialog box
    HWND        hWndButtonBar,          // [in] handle to button bar
    UINT        uButtonStyle,           // [in] button bar style to use
    UINT        uButtonMaxWidth,        // [in] Calculated Max Button Width
    BOOL        bInitDialog,            // [in] TRUE if initing dlg box
    BOOL        bMoveTop                // [in[ TRUE to move to top of dialog
    )
{
    auto    UINT            uWidth, uHeight;
    auto    RECT            crWnd;

    SYM_ASSERT ( hDlg );
    SYM_ASSERT ( hWndButtonBar );
                                        // -----------------------------------
                                        // See if we need to do anything.
                                        // -----------------------------------

    if ( (UINT)ButtonBar_GetStyle (hWndButtonBar) == uButtonStyle &&
          !bInitDialog )
        {
        return (NOERR);
        }
                                        // -----------------------------------
                                        // Update the button bar style.
                                        // -----------------------------------
    ButtonBar_SetStyle ( hWndButtonBar, uButtonStyle );

                                        // -----------------------------------
                                        // Bitmaps with text.
                                        // -----------------------------------

    if ( NPS_DISPLAY_BBAR_BITMAPS ( uButtonStyle ) &&
         NPS_DISPLAY_BBAR_TEXT    ( uButtonStyle ) )
        {
        if (DBCSInstalled())
            {
            uWidth  = max (uButtonMaxWidth, 
                           DlgUnitsToPixelsX (NPS_BUTTON_WIDTH_BITMAPNTEXT_DBCS));
            uHeight = DlgUnitsToPixelsY (NPS_BUTTON_HEIGHT_BITMAPNTEXT_DBCS);
            }
        else
            {                    
            uWidth  = max (uButtonMaxWidth, 
                           DlgUnitsToPixelsX (NPS_BUTTON_WIDTH_BITMAPNTEXT));
            uHeight = DlgUnitsToPixelsY (NPS_BUTTON_HEIGHT_BITMAPNTEXT);
            }
        }
                                        // -----------------------------------
                                        // Bitmaps only.
                                        // -----------------------------------

    else if ( NPS_DISPLAY_BBAR_BITMAPS ( uButtonStyle ) &&
             !NPS_DISPLAY_BBAR_TEXT    ( uButtonStyle ))
        {
        uWidth  = max (uButtonMaxWidth, 
                       DlgUnitsToPixelsX (NPS_BUTTON_WIDTH_BITMAP));

        uHeight = DlgUnitsToPixelsY (DBCSInstalled()?
                                        NPS_BUTTON_HEIGHT_BITMAP_DBCS :
                                        NPS_BUTTON_HEIGHT_BITMAP);
        }
                                        // -----------------------------------
                                        // Text only.
                                        // -----------------------------------

    else if ( NPS_DISPLAY_BBAR_TEXT    ( uButtonStyle ) &&
             !NPS_DISPLAY_BBAR_BITMAPS ( uButtonStyle ))
        {
        uWidth  = max (uButtonMaxWidth, 
                       DlgUnitsToPixelsX (DBCSInstalled()?
                                            NPS_BUTTON_WIDTH_TEXT_DBCS : 
                                            NPS_BUTTON_WIDTH_TEXT));
        uHeight = DlgUnitsToPixelsY (NPS_BUTTON_HEIGHT_TEXT);
        }
    else
        {
        uWidth = 0;
        uHeight = 0;
        }

                                        // -----------------------------------
                                        // Set the size of the buttons.
                                        // -----------------------------------

    ButtonBar_SetButtonSize ( hWndButtonBar, uWidth, uHeight );

                                        // -----------------------------------
                                        // Move the button bar.
                                        // -----------------------------------
    if ( bMoveTop == TRUE )
        {
        GetClientRect ( hDlg, &crWnd );
        crWnd.left      = 0;
        crWnd.top       = 0;
        crWnd.bottom    = uHeight + 1;
        MoveWindow ( hWndButtonBar, crWnd.left, crWnd.top, crWnd.right,
                     crWnd.bottom, FALSE );
        }
    else
        {
        GetClientRect ( hWndButtonBar, &crWnd );

        MapWindowPoints(
                hWndButtonBar,	        // handle of window to be mapped from 
                hDlg,	                // handle of window to be mapped to 
                (LPPOINT)&crWnd,	    // address of structure array with points to map 
                2                       // number of structures in array 
                );	

                                        // Clear the old Button Bar area. This
                                        // is to cleanup when the BB decreases
                                        // its height.
        SYM_ASSERT ( hWndButtonBar && GetDesktopWindow() != hWndButtonBar );
        InvalidateRect (GetParent (hWndButtonBar), &crWnd, TRUE);

        MoveWindow ( hWndButtonBar, crWnd.left, crWnd.top,
                     crWnd.right  - crWnd.left,
                     uHeight + 1, FALSE );
        }

    return ( NOERR );
} // NpsResetButtonBar()



//**********************************************************************
// Description:
//      This routine converts the rect struct from screen coordinates
//              to client coordinates of hWnd.
//
// Return values:
//      None
//
//**********************************************************************
// 06/03/93 KDY Function created.
//**********************************************************************
WORD PASCAL NpsScreenToClient(                                  // Convert rect
        HWND    hWnd,                                                           // [in] Window handle
        LPRECT  lpcrWnd)                                                        // [in/out] Window rect
{
        POINT   pt1, pt2;
        pt1.x = lpcrWnd->left;
        pt1.y = lpcrWnd->top;
        pt2.x = lpcrWnd->right;
        pt2.y = lpcrWnd->bottom;
        ScreenToClient(hWnd, &pt1);
        ScreenToClient(hWnd, &pt2);
        lpcrWnd->left = pt1.x;
        lpcrWnd->top = pt1.y;
        lpcrWnd->right = pt2.x;
        lpcrWnd->bottom = pt2.y;

        return(NOERR);
} // NpsScreenToClient()


//****************************************************************************
// UpdateMaxBtnWidthHeight
//
// Based on the ButtonData provided, this routine updates the current maximum
// width and height values. The ButtonData provides the bitmap and text needed
// to calculate the size of this button. That size information is then used
// to update the current maximum settings.
//
// Note that this routine looks at the ButtonData->wFlags to determine if
// only the bitmaps, only the text, or both text and bitmaps make up the
// button size.
//
// Parameters:
//   hWnd               [in]  Window to fetch font information from
//   lpButtonData       [in]  Privides the button information
//   lpuCurMaxWidth     [i/o] The current maximum width. If this is NULL, the
//                            variable is never updated.
//   lpuCurMaxHeight    [i/o] The current maximum height. If this is NULL, the
//                            variable is never updated.
//
//****************************************************************************
// 09/24/96 JBRENNA - created.
//****************************************************************************

static void UpdateMaxBtnWidthHeight (const HWND         hWnd,
                                     const LPBUTTONDATA lpButtonData,
                                     LPUINT             lpuCurMaxWidth,
                                     LPUINT             lpuCurMaxHeight)
{
    const  UINT uPadWidth      = 4 /* buffer */ + 4 /* shadow */;
    const  UINT uPadHeight     = 1;
    const  UINT uBothPadHeight = 4;     // Padding if both TEXT & BITMAPS being displayed.

    auto   UINT uBitmapWidth  = 0;
    auto   UINT uBitmapHeight = 0;
    auto   UINT uTextWidth    = 0;
    auto   UINT uTextHeight   = 0;
    auto   UINT uPadding;

    if (lpuCurMaxWidth == NULL && lpuCurMaxHeight == NULL)
        return;                         // nothing to do

    // If the wFlags indicate that TEXT is included in the display, calculate
    // the space consumed by the string. Unfortunately, '&' chars (which 
    // provide the visual underline) in the string must be stripped. If they
    // are not stripped, they add space to the calculation. Also, for some
    // reason the currently selected font is not the one that the buttons use
    // to display. So, we have to temporarily select it into the DC to get the
    // correct measurements.
    if (lpButtonData->wFlags & BBFLAG_TEXT)
        {
        const HDC    hDC   = GetDC (hWnd);
        auto  LPTSTR lpAmp = STRCHR (lpButtonData->lpszText, '&');
        auto  HANDLE hOldFont;
        auto  HFONT  hNGFont;
        auto  SIZE   textSize;
        auto  TCHAR  szBuffer[SYM_MAX_PATH];
        auto  LPTSTR lpszText;

                                    // Get the NG font that is used to display
                                    // the buttons and select it into the DC.
        NG_FontGet (hWnd, &hNGFont, FW_NORMAL, 0);
        hOldFont = SelectObject (hDC, hNGFont);

                                    // Remove the '&' from the string if there
                                    // is one.
        if (!lpAmp)
            lpszText = lpButtonData->lpszText;
        else
            {
                                    // Remove the '&'. This should be fine with
                                    // DBCS strings because the the STRCHR up
                                    // above found a SBC '&'.
            lpszText = szBuffer;
            STRNCPY (szBuffer, lpButtonData->lpszText, lpAmp - lpButtonData->lpszText);
            szBuffer[lpAmp - lpButtonData->lpszText] = EOS;
            STRCAT  (szBuffer, lpAmp + 1);

                                    // Ensure we did not overrun the buffer.
            SYM_ASSERT (STRLEN (szBuffer) < SYM_MAX_PATH);
            }

                                    // Get the width and height of the string.
        GetTextExtentPoint(hDC, lpszText, STRLEN (lpszText), &textSize);

        uTextWidth  = textSize.cx;
        uTextHeight = textSize.cy;

        SelectObject (hDC, hOldFont);
        ReleaseDC (hWnd, hDC);
        }

    if (lpButtonData->wFlags & BBFLAG_BITMAPS)
        {
        BITMAP bm;
        BOOL result;

        result = GetObject (lpButtonData->hNormal, sizeof (bm), &bm);

        uBitmapWidth  = bm.bmWidth;
        uBitmapHeight = bm.bmHeight;

        result = GetLastError ();
        result = 0;
        }

    if (lpuCurMaxWidth)
        {
        *lpuCurMaxWidth = max (*lpuCurMaxWidth, uTextWidth   + uPadWidth);
        *lpuCurMaxWidth = max (*lpuCurMaxWidth, uBitmapWidth + uPadWidth);
        }

    if (lpuCurMaxHeight)
        {
                                    // Add more height padding if both
                                    // text and bitmaps are on. This
                                    // covers the space between them.
        uPadding = uPadHeight;
        if ((lpButtonData->wFlags & (BBFLAG_TEXT | BBFLAG_BITMAPS)) ==
            (BBFLAG_TEXT | BBFLAG_BITMAPS))
            uPadding += uBothPadHeight;

        *lpuCurMaxHeight = max (*lpuCurMaxHeight,
                                uTextHeight   + uBitmapHeight + uPadding);
        }
}
