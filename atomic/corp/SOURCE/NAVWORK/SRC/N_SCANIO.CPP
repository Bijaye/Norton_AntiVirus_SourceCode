// Copyright 1993 Symantec, Peter Norton Product Group
//************************************************************************
//
// $Header:   S:/NAVWORK/VCS/N_SCANIO.CPv   1.3   08 Jun 1998 12:12:04   DBuches  $
//
// Description:
//      Functions to update the display and statistics during scanning.
//
// Contains:
//      UpdateDir()
//      StatsTimer()
//      UpdateScanStatistics()
//      DrawNavIcon()
//      DrawBitmap()
//      ScanMemBootStat()
//      ScanRes_ScanMemBootStat()
//
// See Also:
//************************************************************************
// $Log:   S:/NAVWORK/VCS/N_SCANIO.CPv  $
// 
//    Rev 1.3   08 Jun 1998 12:12:04   DBuches
// Set main window caption correctly when returning from a scan.
//
//    Rev 1.2   18 May 1998 22:56:50   tcashin
// Fill in the memory-scanned, file quarantined, and files deleted
// controls in the updated dialog box.
//
//    Rev 1.1   13 May 1997 14:37:28   DALLEE
// Adjusted bad assertion on directory length.
// Used to allow only 260-14 (max path - 8.3 filename).
//
//    Rev 1.0   06 Feb 1997 21:02:44   RFULLER
// Initial revision
//
//    Rev 1.10   28 Oct 1996 12:20:32   JBRENNA
// STS 72878 - Cause NameShortenFileName() to add "..." on 38 characters
// rather than 50 characters. This eliminates clipping on Win95 Korean systems.
//
//    Rev 1.9   01 Sep 1996 20:36:32   DALLEE
// DrawNavIcon(): Only load and destroy animated icons if using them.
//   Changed check from IsIconic() to IsVisible() - This makes the icon the
//   doctor bag unless the main window is minimized and visible (the only
//   time we want to show animated icon and % done).
//
//    Rev 1.8   30 Aug 1996 12:50:34   DALLEE
// Fixed up and commented DrawNavIcon().  Merged 95 and NT source.
// Code should work for NT 3.51, NT 4.0, and W95.
//
//    Rev 1.7   19 Aug 1996 14:03:00   GWESTER
// For Kirin, removed the "#ifdef NAVSCANW" statements around download
// and "GloballySaved_..." code, just like in LuigiPlus. This code will
// now get compiled whether you're compiling for Win95 or NT.
//
//    Rev 1.6   18 Jul 1996 10:40:46   TIVANOV
// fix the task bar registration and the icon paining
//
//    Rev 1.5   28 Jun 1996 17:08:42   GWESTER
// Ported in fixes from Luigi Plus.
//
//    Rev 1.4   26 Jun 1996 10:57:42   TIVANOV
// fix the drawicon for the NT version
//
//    Rev 1.3   24 Jun 1996 21:35:40   RHAYES
// Add.'l OEM<->Ansi mod.'s (new and from Luigi).
//
//    Rev 1.2   21 May 1996 15:12:36   RHAYES
// Merged Luigi OEM<->Ansi code for more robust support of
// Non-Ansi directory and filenames.
//
//    Rev 1.1   04 Apr 1996 16:40:22   YVANG
// Ported from Kirin.
//
//    Rev 1.1   03 Apr 1996 14:48:50   YVANG
// Deleted disabled (not used) lines.
//
//    Rev 1.0   30 Jan 1996 15:55:40   BARRY
// Initial revision.
//
//    Rev 1.0   30 Jan 1996 14:50:44   BARRY
// Initial revision.
//
//    Rev 1.37   23 Jan 1996 11:27:58   YVANG
// Removed #ifndef NAVSCANW switch and instead changed to use run-time check
// for non-NT to set text for ID_SC_MEMSCAN and ID_SS_MEMSCAN controls.
//
//    Rev 1.36   18 Jan 1996 15:13:50   YVANG
// Modified to use a different title for iconized main dialog under NACSCANW
// and non-NAVSCANW.
//
//    Rev 1.35   16 Jan 1996 11:55:14   YVANG
// Changed NAVWNT to NAVSCANW.  Changed icon drawing to be done from #ifdef
// switch to run-time check under NT.
//
//    Rev 1.34   21 Nov 1995 12:49:28   GWESTER
// Under NT, fixed problem of GPFs that occur when NAVNT is running and the
// user logs in and logs out.
//
//
//    Rev 1.33   20 Nov 1995 16:39:26   YVANG
// Fixed 1) the scan dialog flickering problem; 2) the problem in which the
// scan dialog keeps trying to pop itself back on top when it finds a virus.
// (These problems are caused by my improper implementation of icon animation
// in the Rev 1.32.)
//
//    Rev 1.32   15 Nov 1995 17:34:32   YVANG
// For NAVWNT: Made icon to animate.  Removed percentage, % field from the
// main dialogbox title.  Modified to hide the main dialogbox during scan when
// the iconized main dialogbox is restored.
//
//    Rev 1.31   06 Nov 1995 13:51:38   DALLEE
// For NAVWNT: removed memory and inoculation text/buttons from dialogs.
//
//    Rev 1.30   02 Nov 1995 16:35:34   JWORDEN
// Convert MasterBootRec elements from booleans to counters
//
//    Rev 1.29   15 Jul 1995 16:20:04   KEITH
// Remove the NameReturnLongName call from UpdateDir since it is
// already long at this point. Fix the calls to NameReturnLongName
// so that the original is restored on failure. Without this, we
// get in big trouble due to changes made to the path when it fails.
//
//    Rev 1.28   07 Jul 1995 11:16:56   MARKL
// Now calls NavHaltSystem()
//
//    Rev 1.27   03 Jul 1995 16:22:14   MARKL
// Cleaned up icons and their painting.
//
//    Rev 1.26   03 Jul 1995 09:37:48   KEITH
// Added an assert and some debug messages.
//
//    Rev 1.25   13 Jun 1995 15:53:54   KEITH
// Handle how NAVW continues after a system shutdown is canceled. This would
// be after a memory infection.
// Then...make the shutdown unconditional again anyhow!?
//
//    Rev 1.24   01 Jun 1995 13:46:10   KEITH
// STS36287 - allow the user to save files in their apps before shutting
// down completely.
//
//    Rev 1.23   24 May 1995 15:50:04   KEITH
// Write info to WININIT.INI to properly delete our saved copy
// of the shutdown .BMPs.
//
//    Rev 1.22   24 May 1995 15:38:04   KEITH
// Add some debug code and change one of the shutdown names.
//
//    Rev 1.21   23 May 1995 16:23:00   KEITH
// Add the option to also replace logow.sys on virus detection reboot.
//
//    Rev 1.20   10 May 1995 17:39:28   KEITH
// Switch the final shutdown screen to one of our own design if we
// shutdown due to a virus infection.
//
//    Rev 1.19   09 May 1995 10:30:24   KEITH
// Correct some nouns
//
//    Rev 1.18   04 May 1995 15:37:32   MFALLEN
// Display paths with casing just as Win95.
//
//    Rev 1.17   30 Apr 1995 17:59:50   DALLEE
// New animations for ET1.
//
//    Rev 1.16   18 Apr 1995 18:26:26   MFALLEN
// Touch ups as docs suggested.
//
//    Rev 1.15   17 Apr 1995 14:46:00   MFALLEN
// SYM_MAX_PATH + 1
//
//    Rev 1.14   27 Mar 1995 18:10:56   AMATIC
// Cosmetic Rework
// n_scanio.cpp -> S:\navwork\vcs\n_scanio.cpv
// Enter description of changes made.
//
//    Rev 1.13   08 Mar 1995 10:16:42   AMATIC
// Rewrote "DrawIcon()"
//
//    Rev 1.12   07 Mar 1995 07:47:48   AMATIC
// Added animation to Scanner dialog, sync. with Minimize/Restore evts, and
// rewrote "manual" Icon/Art - while minimized ...
//
//    Rev 1.11   01 Mar 1995 14:03:34   AMATIC
// Final fix of icon animation ...
//
//    Rev 1.10   01 Mar 1995 13:28:52   AMATIC
//
//    Rev 1.9   28 Feb 1995 18:28:30   AMATIC
// Fixed animation on iconized scan dlg.
//
//    Rev 1.8   25 Jan 1995 16:28:20   MFALLEN
// SYS4_HALT
//
//    Rev 1.7   20 Dec 1994 14:46:38   MFALLEN
// Sami did fixes on CERTLIBN
//
//    Rev 1.6   29 Nov 1994 13:37:14   MFALLEN
// Compiles for W16
//
//    Rev 1.5   22 Nov 1994 13:54:40   MFALLEN
// Made it work once again.
//
//    Rev 1.4   21 Nov 1994 13:07:14   MFALLEN
// Instead of commenting out all the functions that does not work, they have
// been stubbed out instead.
//
//    Rev 1.3   08 Nov 1994 15:41:58   MFALLEN
// Compiles fine under W32.
//
//
//    Rev 1.20   30 Oct 1993 21:14:54   DALLEE
// NavDrawIcon(): Set class hIcon to NULL before calling InvalidateRect() so
// we get a WM_PAINT when minimized.  (Icon wasn't animated).
//************************************************************************

#include "platform.h"
#include <windowsx.h>
#include "xapi.h"
#include "ngui.h"
#include "symcfg.h"

#include "navutil.h"
#include "options.h"
#include "cmbstr.h"

#ifdef SYM_WIN16
    #include "syminteg.h"
#endif

#include "stub.h"
#include "nonav.h"
#include "nunavdlg.h"
#include "nunavstb.h"
#include "navwork.h"

extern HINSTANCE hInstance;

#ifdef SYM_DEBUG
    void OutputDebugString2 ( LPSTR s, LPSTR t=NULL, LPSTR u=NULL );
    #define OUTPUTDEBUGSTRINGS(s, t, u) OutputDebugString2(s,t,u)
#else
    #define OUTPUTDEBUGSTRINGS(s, t, u)
#endif


// ************************************************************************ //
// ****        (C) Copyright Symantec Corporation 19xx - 1993          **** //
// ************************************************************************ //
// ****                                                                **** //
// ****  Procedure:                                                    **** //
// ****                                                                **** //
// ****  Author: Martin Fallenstedt          Date: January 8, 1993     **** //
// ****                                                                **** //
// ****  Functionality:                                                **** //
// ****     Display the path currently being scanned                   **** //
// ****                                                                **** //
// ****  Input:                                                        **** //
// ****                                                                **** //
// ****                                                                **** //
// ****  Output:                                                       **** //
// ****     hDlg            Scan Dialog Handle                         **** //
// ****     lpPath          Path to be displayed                       **** //
// ****     bConvertToAnsi  TRUE - lpPath is OEM                       **** //
// ****                                                                **** //
// ****  Update History:                                               **** //
// ****                                                                **** //
// ****                                                                **** //
// ************************************************************************ //

VOID SYM_EXPORT WINAPI UpdateDir (
    HWND    hDlg,
    LPSTR   lpPath,
    BOOL    bConvertToAnsi
    )
{
    auto    char    szFullPath [ SYM_MAX_PATH + 1 ];

    SYM_ASSERT ( STRLEN( lpPath ) < SYM_MAX_PATH );

#ifdef SYM_DEBUG
    OutputDebugString ( lpPath );
    OutputDebugString ( "\n" );
#endif

    STRCPY ( szFullPath, lpPath );
    NameShortenFileName ( lpPath, szFullPath, 38 );
    if (bConvertToAnsi)
        NavOemToAnsi (szFullPath, szFullPath);
    SetDlgItemText ( hDlg, ID_SC_DIR, szFullPath );
}

// ************************************************************************ //
// ****        (C) Copyright Symantec Corporation 19xx - 1993          **** //
// ************************************************************************ //
// ****                                                                **** //
// ****  Procedure:                                                    **** //
// ****                                                                **** //
// ****  Author: Martin Fallenstedt          Date: January 8, 1993     **** //
// ****                                                                **** //
// ****  Functionality:                                                **** //
// ****                                                                **** //
// ****                                                                **** //
// ****  Input:                                                        **** //
// ****                                                                **** //
// ****                                                                **** //
// ****  Output:                                                       **** //
// ****                                                                **** //
// ****                                                                **** //
// ****  Update History:                                               **** //
// ****                                                                **** //
// ****                                                                **** //
// ************************************************************************ //

VOID SYM_EXPORT CALLBACK StatsTimer(HWND hDlg, UINT message, UINT wParam, DWORD lParam)
{
   auto     LPNONAV  lpNav;
   auto     double   dbl = 0;
   auto     double   dblHuge;

#ifndef NAVSCANW
    if ((lpNav = (LPNONAV)LP_GET(hDlg)) != 0)
#else                                   // Verify lpNav if this is NT
    lpNav = (LPNONAV)LP_GET(hDlg);
    if (!lpNav)
        {LP_SET (hDlg, GloballySaved_lpNav);
         lpNav = (LPNONAV)LP_GET(hDlg);
         /*SYM_ASSERT (lpNav);*/}
    if (lpNav)
#endif
      {
                                        // -----------------------------------
                                        // Now we calculate the new value for
                                        // progress bar, and update it
                                        // -----------------------------------
      if (lpNav->dwTotalKBytes)
         {
         dblHuge = lpNav->dwScannedKBytes;
         dbl = dblHuge * 100;

                                        // -----------------------------------
                                        // Make sure we never divide if dbl = 0
                                        // -----------------------------------
         if (dbl > 0)
            {
            dbl = dbl / lpNav->dwTotalKBytes;

                                        // -----------------------------------
                                        // Make sure we never set the progress
                                        // bar to 100%
                                        // -----------------------------------
            if ( dbl >= 100 )
                dbl = 99;

            Fillbar_SetLevel(GetDlgItem(hDlg, ID_SC_FILLBAR), (UINT) dbl);
            }
         }

      UpdateScanStatistics(hDlg);

      DrawNavIcon (hDlg, (UINT)dbl);
      }
}

// ************************************************************************ //
// ****        (C) Copyright Symantec Corporation 19xx - 1993          **** //
// ************************************************************************ //
// ****                                                                **** //
// ****  Procedure:                                                    **** //
// ****                                                                **** //
// ****  Author: Martin Fallenstedt          Date: January 8, 1993     **** //
// ****                                                                **** //
// ****  Functionality:                                                **** //
// ****                                                                **** //
// ****                                                                **** //
// ****  Input:                                                        **** //
// ****                                                                **** //
// ****                                                                **** //
// ****  Output:                                                       **** //
// ****                                                                **** //
// ****                                                                **** //
// ****  Update History:                                               **** //
// ****                                                                **** //
// ****                                                                **** //
// ************************************************************************ //

VOID UpdateScanStatistics(HWND hDlg)
{
    auto     char     lpWork[SIZE_WORK + 1];
    auto     LPNONAV  lpNav;

#ifndef NAVSCANW
    if ((lpNav = (LPNONAV)LP_GET(hDlg)) != 0)
#else                                   // Verify lpNav if this is NT
    lpNav = (LPNONAV)LP_GET(hDlg);
    if (!lpNav)
        {LP_SET (hDlg, GloballySaved_lpNav);
         lpNav = (LPNONAV)LP_GET(hDlg);
         /*SYM_ASSERT (lpNav);*/}
    if (lpNav)
#endif
        {
        // ************************************** //
        // Show how many files has been processed //
        // ************************************** //

        ConvertLongToString (lpNav->Stats.Files.dwScanned, lpWork);
        Static_SetText(GetDlgItem(hDlg, ID_SC_FILSCAN), lpWork);

        ConvertWordToString ((WORD)lpNav->Stats.Files.uInfected, lpWork);
        Static_SetText(GetDlgItem(hDlg, ID_SC_FILINF), lpWork);

        ConvertWordToString ((WORD)lpNav->Stats.Files.uCleaned, lpWork);
        Static_SetText(GetDlgItem(hDlg, ID_SC_FILCLN), lpWork);

        ConvertWordToString ((WORD)lpNav->Stats.Files.uQuarantined, lpWork);
        Static_SetText(GetDlgItem(hDlg, ID_SC_FILQUAR), lpWork);

        ConvertWordToString ((WORD)lpNav->Stats.Files.uDeleted, lpWork);
        Static_SetText(GetDlgItem(hDlg, ID_SC_FILDEL), lpWork);

        }
}

//************************************************************************
// DrawNavIcon()
//
// VOID SYM_EXPORT CALLBACK DrawNavIcon (HWND hDlg, UINT uPercent)
//
// This routine updates the main window's title text and icon to
// display current scan status. (The main window is the iconized goody
// that the user actually sees while scanning)
//
// Parameters:
//      HWND    hDlg            Handle of scan dlg - owner is main window.
//      UINT    uPercent        Percentage of scan completed.
//
// Returns:
//      Nothing.
//************************************************************************
// 08/30/1996 DALLEE - Added comment block and merged 95 and NT
//      functionality. Good for NT3.51 NT4.0 and W95.
//************************************************************************

VOID SYM_EXPORT CALLBACK DrawNavIcon (HWND hDlg, UINT uPercent)
{
    const   int     SIZE_TITLE=60;
    auto    CHAR    szTitle [ SIZE_TITLE+1 ];

    auto    HICON   hIcon;
    auto    HICON   hIconSmall;

    static  UINT    uIcon      = 0;
    static  BOOL    bWasIconic = FALSE;

    static  HWND hWndOwner = (HWND)GetWindow(hDlg, GW_OWNER);
    SYM_ASSERT(hWndOwner);

    static  WORD ClockID[] =
        {
        ID_ICON_NAV1,
        ID_ICON_NAV2,
        ID_ICON_NAV3,
        ID_ICON_NAV4,
        ID_ICON_NAV5,
        ID_ICON_NAV6,
        ID_ICON_NAV7,
        ID_ICON_NAV8,
        0
        };


                                        // Get information on the current
                                        // icon used by the window.
    static HICON hOriginalIcon   = (HICON)GetClassLong (hWndOwner, GCL_HICON);
    static HICON hOriginalIconSM = (HICON)GetClassLong (hWndOwner, GCL_HICONSM);

                                        // Go on to our next icon in the
                                        // animation sequence, wrapping
                                        // around if we are at the end of
                                        // the list.
    if ( ! ClockID[ ++ uIcon] )
        uIcon = 0;

                                        // If main window is not visible, then
                                        // we want just the regular icon
                                        // and title to show in the task list.
    if ( !IsWindowVisible(hWndOwner) )
        {
        if ( bWasIconic )               // -----------------------------------
            {                           // restore original title and icon ...
            // Set icon to NULL first and invalidate rect so NT 3.51 will
            // repaint the icon in the main window.
            SetClassLong(hWndOwner, GCL_HICON, NULL);
            InvalidateRect(hWndOwner, NULL, TRUE);

            if ( GetCustomVersion() == VERSION_NETSCAPE )
                {                       // Title is "Internet Scanner"; set at runtime.
                SetWindowText (hWndOwner, String_Ptr(IDS_NAV_INTERNET));
                SetWindowText (hDlg,      String_Ptr(IDS_NAV_INTERNET));
                }
            else
                {
                SetWindowText (hWndOwner, String_Ptr(IDS_MAINWND_CAPTION));
                SetWindowText (hDlg,      String_Ptr(IDS_NAV));
                }
            SetClassLong (hWndOwner, GCL_HICON,   (LONG)hOriginalIcon);
            SetClassLong (hWndOwner, GCL_HICONSM, (LONG)hOriginalIconSM);

            UpdateWindow(hWndOwner);
            bWasIconic = FALSE;
            }
        }
                                        // Else if main window is visible and
                                        // minimized during scan, then show
                                        // animated icon and the percent done.
    else
        {
        // Set icon to NULL first and invalidate rect so NT 3.51 will
        // repaint the icon in the minimized main window.
        SetClassLong(hWndOwner, GCL_HICON, NULL);
        InvalidateRect(hWndOwner, NULL, TRUE);

                                        // Change a title and
        if ( uPercent <= 99 )           // Animate icons of OWNER (main wnd) ...
            {
                                        // Load the icon we wish to animate
                                        // with.
            hIcon      = LoadIcon(hInstance, MAKEINTRESOURCE(ClockID[uIcon]));
            hIconSmall = LoadIcon(hInstance, MAKEINTRESOURCE(ClockID[uIcon]));

            if ( GetCustomVersion() == VERSION_NETSCAPE )
                {                       // Title is "Internet Scanner"; set at runtime.
                SPRINTF (szTitle, String_Ptr(IDS_NAV_PERCENT_W95_INTERNET), uPercent);
                }
            else
                {
                SPRINTF (szTitle, String_Ptr(IDS_NAV_PERCENT_W95), uPercent);
                }

            SetWindowText (hWndOwner, szTitle);
            SetClassLong (hWndOwner, GCL_HICON,   (LONG)hIcon);
            SetClassLong (hWndOwner, GCL_HICONSM, (LONG)hIconSmall);

            DestroyIcon (hIcon);
            DestroyIcon (hIconSmall);
            }
        else                            // Last percentage - restore orig. look
            {
            if ( GetCustomVersion() == VERSION_NETSCAPE )
                {                       // Title is "Internet Scanner"; set at runtime.
                SetWindowText (hWndOwner, String_Ptr(IDS_NAV_INTERNET));
                SetWindowText (hDlg,      String_Ptr(IDS_NAV_INTERNET));
                }
            else
                {
                SetWindowText (hWndOwner, String_Ptr(IDS_MAINWND_CAPTION));
                SetWindowText (hDlg,      String_Ptr(IDS_NAV));
                }
            SetClassLong (hWndOwner, GCL_HICON,   (LONG)hOriginalIcon);
            SetClassLong (hWndOwner, GCL_HICONSM, (LONG)hOriginalIconSM);
            }                           // ----------------------------------
        UpdateWindow   (hWndOwner);
        bWasIconic = TRUE;
        }

} // DrawNavIcon()


// ************************************************************************ //
// ****        (C) Copyright Symantec Corporation 19xx - 1993          **** //
// ************************************************************************ //
// ****                                                                **** //
// ****  Procedure:                                                    **** //
// ****                                                                **** //
// ****  Author: Martin Fallenstedt          Date: January 8, 1993     **** //
// ****                                                                **** //
// ****  Functionality:                                                **** //
// ****                                                                **** //
// ****                                                                **** //
// ****  Input:                                                        **** //
// ****                                                                **** //
// ****                                                                **** //
// ****  Output:                                                       **** //
// ****                                                                **** //
// ****                                                                **** //
// ****  Update History:                                               **** //
// ****                                                                **** //
// ****                                                                **** //
// ************************************************************************ //

VOID SYM_EXPORT WINAPI DrawBitmap (HDC hDC, HBITMAP hBitmap)
{
   HDC     hDCMem;
   HBITMAP hOldBitmap;

   hDCMem = CreateCompatibleDC (hDC);

   // ******************************** //
   // Select that bitmap into our      //
   // memory DC and set the mapping    //
   // mode identical to the DC we will //
   // be drawing to.                   //
   // ******************************** //

   hOldBitmap = (HBITMAP)SelectObject (hDCMem, hBitmap);

   // ***************************************************************** //
   // Transfer our bitmap from our memory DC to our screen bit-for-bit. //
   // ***************************************************************** //
   BitBlt (hDC, 5, 5, 64, 64,
           hDCMem, 0, 0, SRCCOPY);

   SelectObject (hDCMem, hOldBitmap);
   DeleteDC (hDCMem);
}


VOID SYM_EXPORT WINAPI ScanMemBootStat (HWND hDlg, LPNONAV lpNav)
{
    auto    char    lpWork[SIZE_WORK + 1];

    if (SYM_SYSTEM_WIN_NT != SystemGetWindowsType())
        {
        if (lpNav->Opt.bMemory)
            Static_SetText(GetDlgItem(hDlg, ID_SC_MEMSCAN), String_Ptr(IDS_MEMSCANNED));
        else
            Static_SetText(GetDlgItem(hDlg, ID_SC_MEMSCAN), String_Ptr(IDS_MEMNOTSCANNED));
        }

    ConvertWordToString ((WORD)lpNav->Stats.MasterBootRec.uScanned, lpWork);
    Static_SetText(GetDlgItem(hDlg, ID_SC_MBSCAN), lpWork);

    ConvertWordToString ((WORD)lpNav->Stats.MasterBootRec.uInfected, lpWork);
    Static_SetText(GetDlgItem(hDlg, ID_SC_MBINF), lpWork);

    ConvertWordToString ((WORD)lpNav->Stats.MasterBootRec.uCleaned, lpWork);
    Static_SetText(GetDlgItem(hDlg, ID_SC_MBCLN), lpWork);

    ConvertWordToString ((WORD)lpNav->Stats.BootRecs.uScanned, lpWork);
    Static_SetText(GetDlgItem(hDlg, ID_SC_BRSCAN), lpWork);

    ConvertWordToString ((WORD)lpNav->Stats.BootRecs.uInfected, lpWork);
    Static_SetText(GetDlgItem(hDlg, ID_SC_BRINF), lpWork);

    ConvertWordToString ((WORD)lpNav->Stats.BootRecs.uCleaned, lpWork);
    Static_SetText(GetDlgItem(hDlg, ID_SC_BRCLN), lpWork);
}

VOID SYM_EXPORT WINAPI ScanRes_ScanMemBootStat (HWND hDlg, LPNONAV lpNav)
{
    auto    char    lpWork[SIZE_WORK + 1];

    if (SYM_SYSTEM_WIN_NT != SystemGetWindowsType())
        {
        if (lpNav->Opt.bMemory)
            Static_SetText(GetDlgItem(hDlg, ID_SC_MEMSCAN), String_Ptr(IDS_MEMSCANNED));
        else
            Static_SetText(GetDlgItem(hDlg, ID_SC_MEMSCAN), String_Ptr(IDS_MEMNOTSCANNED));
        }

    ConvertWordToString ((WORD)lpNav->Stats.MasterBootRec.uScanned, lpWork);
    Static_SetText(GetDlgItem(hDlg, ID_SS_MBSCAN), lpWork);

    ConvertWordToString ((WORD)lpNav->Stats.MasterBootRec.uInfected, lpWork);
    Static_SetText(GetDlgItem(hDlg, ID_SS_MBINF), lpWork);

    if (lpNav->Stats.MasterBootRec.uCleaned)
        {
        ConvertWordToString ((WORD)lpNav->Stats.MasterBootRec.uCleaned, lpWork);
        Static_SetText(GetDlgItem(hDlg, ID_SS_MBCLN), lpWork);
        }

    ConvertWordToString ((WORD)lpNav->Stats.BootRecs.uScanned, lpWork);
    Static_SetText(GetDlgItem(hDlg, ID_SS_BRSCAN), lpWork);

    ConvertWordToString ((WORD)lpNav->Stats.BootRecs.uInfected, lpWork);
    Static_SetText(GetDlgItem(hDlg, ID_SS_BRINF), lpWork);

    if ( lpNav->Stats.BootRecs.uCleaned )
        {
        ConvertWordToString ((WORD)lpNav->Stats.BootRecs.uCleaned, lpWork);
        Static_SetText(GetDlgItem(hDlg, ID_SS_BRCLN), lpWork);
        }
}


#ifdef SYM_DEBUG
void OutputDebugString2 ( LPSTR s, LPSTR t, LPSTR u )
{
    if ( s != NULL )
        OutputDebugString ( s );
    if ( t != NULL )
        OutputDebugString ( t );
    if ( u != NULL )
        OutputDebugString ( u );

    OutputDebugString ( "\n" );
}
#endif



