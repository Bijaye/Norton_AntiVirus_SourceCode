// See Also:
//************************************************************************
// $Log:   S:/NAVWORK/VCS/N_MEMSCN.CPv  $
// 
//    Rev 1.13   01 Feb 1999 13:53:36   MBROWN
// Backed out of large sector fix and nec floppy hack removal
//
//    Rev 1.10   27 Jul 1998 10:09:58   tcashin
// In ScanMasterBootRecord(), make sure the bIsExtInt13 field in the ABSDISK
// structure is set to ISINT13X_UNKNOWN (defect #119072).
//
//    Rev 1.9   22 Jul 1998 19:15:22   DBuches
// Added kludgey NEC boot record support.
//
//    Rev 1.8   16 Mar 1998 17:14:18   MKEATIN
// Made changed to match those in the new disk.h
//
//    Rev 1.7   03 Mar 1998 16:37:58   MKEATIN
// Initialized lpszDrives and lpszCurrDrive to NULL in EnumFixedDrives
//
//    Rev 1.6   18 Feb 1998 16:22:12   mdunn
// Ported NEC code from the ROSNEC area.
//
//    Rev 1.5   21 Jul 1997 01:39:46   BGERHAR
// Added support for PROGRAM_AUTOMATED mode of uProgramMode
//
//    Rev 1.4   15 Jul 1997 14:36:54   DBUCHES
// Added Ctl3d support for DEC Alpha
//
//    Rev 1.3   03 Jul 1997 15:01:42   TIVANOV
// fix custom notify responce
//
//    Rev 1.2   22 May 1997 16:20:54   BILL
// Alpha changes (removed ctl3d)
//
//    Rev 1.1   24 Apr 1997 11:09:52   JBRENNA
// Remove the IDD_MEMSCANDLG_MEMONLY dialog.
// Tesla Port: KSACKIN r1.1:
//  Implemented fix for command line scanning when logged in as a user.
//  Previously, if you were a simple user, boot record scans would fail becuase
//  you have no access to the boot record.  Now, if we are running in command
//  line mode, we are not going to put up and error and exit our scanning if
//  we are unable to scan the boot sector.  Also, if you are doing a command
//  line scan, you are not even asking to scan the boot sector, so putting up a
//  nag message saying we cannot scan it is annoying.
//
//    Rev 1.0   06 Feb 1997 21:02:52   RFULLER
// Initial revision
//
//    Rev 1.7   27 Jan 1997 11:29:04   JBRENNA
// For MemScanDlg(), use IDD_MEMSCANDLG_MEMONLY when we are not scanning
// MBRs or BRs. This is especially useful on NEC where we always do not scan
// MBRs or BRs.
//
//    Rev 1.6   11 Jul 1996 14:33:40   RHAYES
// Changes to GetPhysicalDrive() to correctly handle SUBST'ed drives
// under NT 3.5X.
//
//
//    Rev 1.5   11 Jul 1996 08:03:46   TIVANOV
// fix the dialog font initilization
//
//    Rev 1.4   01 Jul 1996 12:55:42   GWESTER
// Ported in fixes from Luigi Plus.
//
//    Rev 1.3   28 Jun 1996 17:39:08   GWESTER
// Ported in fixes from Luigi Plus.
//
//    Rev 1.2   28 Jun 1996 17:26:20   GWESTER
// Ported in changes from Luigi Plus.
//
//    Rev 1.1   14 Jun 1996 13:44:34   RHAYES
// Changes to detect a SUBST'd drive and use the target
// drive letter when scanning the boot record. Added
// new function GetPhysicalDriveLetter().
//
//    Rev 1.0   30 Jan 1996 15:55:34   BARRY
// Initial revision.
//
//    Rev 1.0   30 Jan 1996 14:50:36   BARRY
// Initial revision.
//
//    Rev 1.33   23 Jan 1996 11:48:22   YVANG
// 1) Added #ifndef NAVSCANW #else switch around DialogBoxParam(,IDD_MEMSCANDLG,,,).
//    Also for #ifdef NAVSCANW case, changed to use different dialog templates
//    under Win95 and NT.
// 2) Replaced #ifndef NAVSCANW switch with run-time check for NT around
//    the block of codes for 'VirusScanMemory()'.
//
//    Rev 1.32   17 Jan 1996 22:41:58   YVANG
// For NAVSCANW: Modified to hide controls of IDI_MS_BOX1 and IDI_MS_BOX2 for
// 3-D frame with run-time check under Win95.
//
//    Rev 1.31   16 Jan 1996 11:54:16   YVANG
// Changed NAVWNT to NAVSCANW.  Added NT run-time check for 3-D frame.
//
//    Rev 1.30   05 Dec 1995 01:31:50   MKEATIN
// skipps scanning boot records on subst drives for NT
//
//    Rev 1.29   21 Nov 1995 16:44:44   YVANG
// For NAVWNT: Added a call, NG_SetGroupBoxSunken(), to make the progress bars
// of the Memory and Boot Scan diaglog to be displayed in 3-D frame.
//
//
//    Rev 1.28   21 Nov 1995 12:51:02   GWESTER
// Under NT, fixed problem of GPFs that occur when NAVNT is running and the
// user logs in and logs out.
//
//
//    Rev 1.27   15 Nov 1995 21:55:46   DALLEE
// Call S32STAT functions through covers in NAVXUTIL, which dynamically loads
// S32STAT.DLL to get around some path issues on NT.
//
//    Rev 1.26   06 Nov 1995 18:08:34   DALLEE
// Added delay to mem and boot fillbars.
// For NAVWNT: removed memory scan.
//
//    Rev 1.25   02 Nov 1995 16:34:44   JWORDEN
// Convert MasterBootRec elements from booleans to counters
//
//    Rev 1.24   24 Oct 1995 19:27:26   JWORDEN
// Make modifications required to scan MBR's on multiple hard drives
//
//    Rev 1.23   13 Jun 1995 15:54:02   KEITH
// Handle how NAVW continues after a system shutdown is canceled. This would
// be after a memory infection.
// Then...make the shutdown unconditional again anyhow!?
//
//    Rev 1.22   24 May 1995 12:05:16   KEITH
// Add messages for MBR and BOOT locked
//
//    Rev 1.21   23 May 1995 16:25:34   KEITH
// Add SYMSTAT.DLL and code to check for locked disks and improved handling
//
//    Rev 1.20   09 May 1995 10:30:50   KEITH
// Correct some nouns
//
//************************************************************************

#include "platform.h"
#include "xapi.h"
#include "windowsx.h"
	#include "ctl3d.h"

#ifdef SYM_WIN16
    #include "syminteg.h"
#endif

#include "ngui.h"
#include "disk.h"
#include "symcfg.h"
#include "navutil.h"
#include "navwork.h"
#include "options.h"
#include "nunavdlg.h"
#include "nunavstb.h"

#include "stub.h"
#include "virscan.h"
#include "nonav.h"

#include "avapi.h"
#ifdef SYM_WIN32
    #include "navstat.h"
    #include "symstat.h"
    #include "navver.h"
    #include "disk.h"
    #include "winioctl.h"
#endif

#define MEMSCAN_MEM     1
#define BAD_DRIVE       '#'

extern HINSTANCE hInstance;
BOOL    bMemoryScanned = FALSE;

#ifdef SYM_WIN32
                                        // See VIRSCAN.H for ERR_VIRSCAN_*
                                        // and keep this different than all.
#define ERR_DISKLOCKED_SYMSTAT      100
#define ERR_DISKLOCKED_SYMSTAT_MBR  101
#define ERR_DISKLOCKED_SYMSTAT_BOOT 102

#endif

// ************************************************************************ //
// ****                       Local Prototypes                         **** //
// ************************************************************************ //

BOOL SYM_EXPORT WINAPI MemScanDlgProc (
    HWND,
    UINT,
    WPARAM,
    LPARAM
    );

BOOL LOCAL ScanTheBootRecord (
    LPSTR       lpDrives,
    LPSTR       lpScanList,
    LPSTR       lpWalkList,
    LPDRIVELIST lpDriveList,
    HWND        hFillBoot,
    LPNONAV     lpNav,
    HWND        hDlg
    );


UINT LOCAL ScanNEC12Floppy( char chDriveLetter, LPNONAV lpNav ); 
BOOL LOCAL IsNEC12MBFloppy( char chDriveLetter );

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

BOOL SYM_EXPORT WINAPI MemScanDlg (
    HWND        hDlg,
    LPNONAV     lpNav
    )
{
    extern       HINSTANCE       hInstance;
    auto         DLGPROC         lpfn;
    auto         int             nStatus;
    auto         LPTSTR          lpDialogId;

    lpfn = (DLGPROC) MakeProcInstance (
                                    (FARPROC)MemScanDlgProc,
                                    hInstance
                                    );
    SYM_ASSERT ( lpfn );

    if (SYM_SYSTEM_WIN_NT == SystemGetWindowsType())
        {
        lpDialogId = HWIsNEC() ? MAKEINTRESOURCE (IDD_MEMSCANDLG_NT_NEC)
                               : MAKEINTRESOURCE (IDD_MEMSCANDLG_NT);
        }
    else
        {
        lpDialogId = HWIsNEC() ? MAKEINTRESOURCE (IDD_MEMSCANDLG_MEMANDBR)
                               : MAKEINTRESOURCE (IDD_MEMSCANDLG);
        }

    nStatus = DialogBoxParam (hInstance,
                              lpDialogId,
                              hDlg,
                              lpfn,
                              (LPARAM)lpNav);

    FreeProcInstance ( (FARPROC)lpfn );

    SYM_ASSERT ( nStatus != -1 );

    if (nStatus == -1)
        nStatus = TRUE;

    return ( (BOOL)nStatus );
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

BOOL SYM_EXPORT WINAPI MemScanDlgProc (
    HWND    hDlg,
    UINT    message,
    WPARAM  wParam,
    LPARAM  lParam
    )
{
    static  BOOL        bPaintProcessed = FALSE;
    static  HWND        hFillMem        = NULL;
    static  HWND        hFillMBoot;
    static  HWND        hFillBoot;
    static  BOOL        bEndScan;
    auto    BOOL        bReturn = FALSE;
    auto    FILLBARSTYLE fbStyle;
    auto    LPNONAV     lpNav;

    switch(message)
        {

        case WM_CTLCOLOR:
            bReturn = ((BOOL)Ctl3dCtlColorEx(message, wParam, lParam));
            break;

        case WM_INITDIALOG:
            Ctl3dSubclassDlg(hDlg, CTL3D_ALL);
            NG_SetFontsInDialog (hDlg, 0);
            NG_SetGroupBoxSunken(hDlg, IDI_MS_BOX);

#ifdef NAVSCANW
            if (SYM_SYSTEM_WIN_NT == SystemGetWindowsType())
                {
                NG_SetGroupBoxSunken(hDlg, IDI_MS_BOX1);

                if ( !HWIsNEC() )
                    {
                                        // This box is around the MBR progress
                                        // bar, which isn't present on NECs.
                    NG_SetGroupBoxSunken(hDlg, IDI_MS_BOX2);
                    }
                }
            else
                {
                ShowWindow(GetDlgItem(hDlg, IDI_MS_BOX1), SW_HIDE);
                ShowWindow(GetDlgItem(hDlg, IDI_MS_BOX2), SW_HIDE);
                }
#endif
            NAV_CenterDialog (hDlg);
            bEndScan = FALSE;

            lpNav = (LPNONAV)lParam;
            LP_SET(hDlg, lpNav);
            bPaintProcessed = FALSE;

            // ********************************************************* //
            // Initialize fillbar with all the currently required values //
            // ********************************************************* //
                                         // No mem scan in NT.
            if (SYM_SYSTEM_WIN_NT != SystemGetWindowsType())
                {
                hFillMem = GetDlgItem(hDlg, IDI_MS_FILLMEM);

                fbStyle.nStruclen         = sizeof(FILLBARSTYLE);
                fbStyle.b3D               = TRUE;
                fbStyle.nLeftTopWidth     = FB_USEDEFAULT;
                fbStyle.nRightBottomWidth = FB_USEDEFAULT;
                fbStyle.crColor           = RGB(255, 0, 0);

                Fillbar_SetRange(hFillMem, 0, 100);
                Fillbar_Show(hFillMem, &fbStyle);
                }
            // ********************************************************* //

                                        // Init the MBR progress bar, but
                                        // only on ATs. On NECs, a different
                                        // dlg w/o an MBR progress bar is used.
            if ( !HWIsNEC() )
                {
                hFillMBoot = GetDlgItem(hDlg, IDI_MS_FILLMBOOT);

                if (hFillMBoot)
                    {
                    fbStyle.nStruclen         = sizeof(FILLBARSTYLE);
                    fbStyle.b3D               = TRUE;
                    fbStyle.nLeftTopWidth     = FB_USEDEFAULT;
                    fbStyle.nRightBottomWidth = FB_USEDEFAULT;
                    fbStyle.crColor           = RGB(0, 255, 0);

                    Fillbar_SetRange(hFillMBoot, 0, 100);
                    Fillbar_Show(hFillMBoot, &fbStyle);
                    }
                }

            // ********************************************************* //

            hFillBoot = GetDlgItem(hDlg, IDI_MS_FILLBOOT);

            if (hFillBoot)
            {
                fbStyle.nStruclen         = sizeof(FILLBARSTYLE);
                fbStyle.b3D               = TRUE;
                fbStyle.nLeftTopWidth     = FB_USEDEFAULT;
                fbStyle.nRightBottomWidth = FB_USEDEFAULT;
                fbStyle.crColor           = RGB(0, 0, 255);

                Fillbar_SetRange(hFillBoot, 0, 100);
                Fillbar_Show(hFillBoot, &fbStyle);
            }

            bReturn = TRUE;
            break;

        case WM_DESTROY:
            LP_FREE(hDlg);
            bPaintProcessed = FALSE;
            break;

        case WM_PAINT:
            if ( bPaintProcessed == FALSE )
                {
                bPaintProcessed = TRUE;
                FORWARD_WM_COMMAND ( hDlg, MEMSCAN_MEM, 0, 0, PostMessage );
                }
            break;

        case WM_COMMAND:
            lpNav = (LPNONAV)LP_GET(hDlg);
#ifdef NAVSCANW                           // Verify lpNav if this is NT
            if (!lpNav)
                {LP_SET (hDlg, GloballySaved_lpNav);
                 lpNav = (LPNONAV)LP_GET(hDlg);
                 SYM_ASSERT (lpNav);}
#endif

            switch ( GET_WM_COMMAND_ID ( wParam, lParam) )
                {
                case MEMSCAN_MEM:
                                        // -----------------------------------
                                        // Scan Memory If Required
                                        // -----------------------------------

                    UpdateWindow(hDlg);

                                        // No mem scan in NT.
                    if (SYM_SYSTEM_WIN_NT != SystemGetWindowsType())
                        {
                        if (lpNav->Opt.bMemory == TRUE)
                            {
                            if ( bMemoryScanned == FALSE )
                                {
                                auto    UINT        i;

                                bMemoryScanned = TRUE;

                                TSR_OFF

                                lpNav->Infected.uVirusSigIndex = VirusScanMemory (
                                        &lpNav->Infected.Notes, lpNav->Gen.bScanHighMemory);

                                TSR_ON

                                for (i=0; i < 101; i += 20)
                                    {
                                    auto    DWORD   dwTimeInMs;

                                    dwTimeInMs = GetTickCount();
                                    while (55 > GetTickCount() - dwTimeInMs)
                                        {
                                        }

                                    Fillbar_SetLevel(hFillMem, i);
                                    }

                                if (lpNav->Infected.uVirusSigIndex)
                                    {
                                    ReportVirusInMem (hDlg, lpNav);
                                    bEndScan = TRUE;
                                    }
                                }
                            else
                                Fillbar_SetLevel(hFillMem, 100);
                            }
                        } // if (SYM_SYSTEM_WIN_NT != ...)
                    else
                        lpNav->Infected.uVirusSigIndex = 0;

                                        // -----------------------------------
                                        // Scan Master Boot Record If Required
                                        // (but only on ATs!)
                                        // -----------------------------------
                    if ( !HWIsNEC() )
                        {
                        if ( !bEndScan )
                            bEndScan = ScanMasterBootRecord ( hDlg,
                                                              lpNav,
                                                              hFillMBoot);
                        }

                                        // -----------------------------------
                                        // Scan Boot Records If Required
                                        // -----------------------------------

                    if ( !bEndScan )
                        bEndScan = ScanBootRecord (hDlg, lpNav, hFillBoot);

                    Fillbar_Close(hFillMem);
                    Fillbar_Close(hFillBoot);

                    if ( !HWIsNEC() )
                        {
                        Fillbar_Close(hFillMBoot);
                        }

                    EndDialog(hDlg, bEndScan);
                    break;
                }
            break;
        }

    return ( bReturn );
}

BOOL SYM_EXPORT WINAPI ScanMasterBootRecord (
    HWND    hDlg,
    LPNONAV lpNav,
    HWND    hFillMBoot
    )
{
    auto    DRIVEPROBLEMSTRUCT  DriveAccess;
    auto    BOOL                bEndScan = FALSE;
    auto    UINT                uStatus = 0; // non zero is error
#ifdef TRANSLATE_LOGICAL_PHYSICAL
    auto    LPSTR               lpScanList;
#endif
    auto    UINT                i;
    auto    BYTE                byDrive;
    auto    ABSDISKREC          rAbsDisk;
    auto    LPBYTE              lpbyBuffer;

    if (lpNav->Opt.bMasterBootRec == TRUE)
        {
        TSR_OFF

                                        // -----------------------------------
                                        // Create a temporary buffer to hold
                                        // the disk sector during our test read
                                        //below
                                        // -----------------------------------

        lpbyBuffer = (LPBYTE)MemAllocPtr (GHND, PHYSICAL_SECTOR_SIZE);

                                        // -----------------------------------
                                        // Process each physical drive in turn
                                        // We determine that a physical drive
                                        // exists by attempting an absolute
                                        // read of sector 1. If it works the
                                        // Drive exists and must be scanned.
                                        // If it doesn't work we quit
                                        // -----------------------------------

        for (byDrive = 0x80; byDrive < 256; byDrive++)
        {
                                        // -----------------------------------
                                        // Is this a local hard drive?
                                        // If not, get out
                                        // -----------------------------------
            rAbsDisk.dn = byDrive;
            rAbsDisk.dwHead = 0;
            rAbsDisk.dwTrack = 0;
            rAbsDisk.dwSector = 1;
            rAbsDisk.numSectors = 1;
            rAbsDisk.buffer = lpbyBuffer;
            rAbsDisk.bIsExtInt13 = ISINT13X_UNKNOWN;

            if (DiskAbsOperation (READ_COMMAND, &rAbsDisk))
                break;

                                        // -----------------------------------
                                        // Since we were able to read, this is
                                        // real drive.
                                        // -----------------------------------

            do
                {
                uStatus = 0;

                lpNav->Infected.uVirusSigIndex =
                           VirusScanMasterBoot ((UINT)byDrive,
                                                &lpNav->Infected.Notes,
                                                &uStatus);
                if ( uStatus )
                    {
                                            // -----------------------------------
                                            // We should notify if scanning full
                                            // screen OR when we're running mini-
                                            // mized AND immediate notify flag is
                                            // turned on.
                                            // -----------------------------------

                    if ( (!hFillMBoot && (lpNav->Opt.bPrestoNotify || IsCutomNotify(lpNav)))
                    ||   hFillMBoot )
                        {
                        lpNav->hMommyWindow = hDlg;

                        MEMSET (&DriveAccess, EOS, sizeof(DriveAccess));

                        DriveAccess.hParent         = hDlg;
                        DriveAccess.byDrive         = (BYTE)(byDrive - 0x80);
                        DriveAccess.bHideContinue   = FALSE;

                        switch ( uStatus )
                            {
                            case ERR_VIRSCAN_DISKLOCKED:
                                DriveAccess.uInfoMsgID = DRIVEACCESS_MSG_PHYS_DISKLOCKED;
                                break;
#ifdef SYM_WIN32
                            case ERR_DISKLOCKED_SYMSTAT_MBR:
                                DriveAccess.uInfoMsgID = DRIVEACCESS_MSG_PHYS_DISKLOCKED;
                                NavStatGetDriveState ( DriveAccess.byDrive,
                                                       0,
                                                       DriveAccess.szWho,
                                                       sizeof (DriveAccess.szWho) );
                                break;
#endif

                            default:
                                DriveAccess.uInfoMsgID = DRIVEACCESS_MSG_PHYS_BOOTRECORD;
                                break;
                            }

                        if ( lpNav->Alert.bRemoveAfter )
                            DriveAccess.uTimeOutSeconds = lpNav->Alert.uSeconds;

                        i = DriveAccessProblem (&DriveAccess);

#ifdef TRANSLATE_LOGICAL_PHYSICAL
                        if (i == DRIVEERROR_SKIP)
                            {
                                            // -----------------------------------
                                            // If we ever figure out how to
                                            // translate from physical to
                                            // logical drives we need to put
                                            // this back
                                            // -----------------------------------
                            lpScanList = (LPSTR)MemLock (lpNav->lpScan->hScanList);
                            RemoveDriveFromList (lpScanList, 'C');
                            MemUnlock (lpNav->lpScan->hScanList);
                            break;
                            }
                         else
#endif
                             if (i == DRIVEERROR_RETRY)
                                 continue;
                             else
                                 if (i == DRIVEERROR_CONTINUE
                                 ||  i == DRIVEERROR_ERROR)
                                    break;
                        }
                    else
                        {
#ifdef TRANSLATE_LOGICAL_PHYSICAL
                        lpScanList = (LPSTR)MemLock (lpNav->lpScan->hScanList);
                        RemoveDriveFromList (lpScanList, 'C');
                        MemUnlock (lpNav->lpScan->hScanList);
                        break;
#endif
                        }

                    }
                else
                    {
                                        // -------------------------------------
                                        // One more MBR scanned
                                        // -------------------------------------

                     lpNav->Stats.MasterBootRec.uScanned++;

                                        // -------------------------------------
                                        // Take care of this while it's still
                                        // fresh on our minds. This will report
                                        // AND offer to fix the problem
                                        // -------------------------------------

                    if (lpNav->Infected.uVirusSigIndex)
                        {
                        lpNav->Stats.MasterBootRec.uInfected++;
                        lpNav->Infected.wStatus = FILESTATUS_MASTERBOOTINFECTED;

                        if (ReportVirusInBoot (hDlg, byDrive, lpNav) == TRUE)
                            {
                            bEndScan = TRUE;
                            break;
                            }
                        }
                    }
                }
            while ( uStatus );

        }

        MemFreePtr (lpbyBuffer);

        TSR_ON
                                        // --------------------------------
                                        // If no error occured, move the
                                        // progress bar to make user happy
                                        // --------------------------------
        if (!uStatus && hFillMBoot)
            {
            for (i=0; i < 101; i += 20)
                {
                auto    DWORD   dwTimeInMs;

                dwTimeInMs = GetTickCount();
                while (55 > GetTickCount() - dwTimeInMs)
                    {
                    }

                Fillbar_SetLevel(hFillMBoot, i);
                }
            }
        }

    return ( bEndScan );
}

BOOL SYM_EXPORT WINAPI ScanBootRecord (
    HWND    hDlg,
    LPNONAV lpNav,
    HWND    hFillBoot
    )
{
    auto    char    szDrives [ NUM_DRIVES + 2 ];

    DRIVELIST DriveList [ NUM_DRIVES ];

    auto    BOOL    bEndScan = FALSE;
    auto    LPSTR   lpScanList;
    auto    LPSTR   lpWalkList;
    auto    BOOL    bHasBoot;


    if (lpNav->Opt.bBootRecs == TRUE)
        {
                        // -----------------------------------
                        // Find the physical drive number for
                        // each of the logical drives
                        // -----------------------------------

        MEMSET (DriveList, 0, sizeof (DriveList));

        EnumFixedDrives (DriveList);

                        // -----------------------------------
                        // Stay in this loop while there are
                        // drives to be scanned
                        // -----------------------------------

        lpScanList = (LPSTR)MemLock (lpNav->lpScan->hScanList);

        lpWalkList = lpScanList;

                        // -----------------------------------
                        // szDrives contains all the drives
                        // that we've scanned. This assures us
                        // that the same drive is not scanned
                        // several times.
                        // -----------------------------------

        MEMSET (szDrives, '\0', NUM_DRIVES);

                        // ----------------------------------
                        // Note that the TSR is being deacti-
                        // vated inside ScanTheBootRecord()
                        // ----------------------------------
        while (*lpWalkList && bEndScan == FALSE)
            {
                                        // Only scan the boot, if this drive
                                        // has an INT 25h accessible boot
            TSR_OFF;

            bHasBoot = DiskHasBootRecord( GetPhysicalDriveLetter( *lpWalkList ) );

            if ( bHasBoot )
                {
                bEndScan = ScanTheBootRecord (szDrives,
                                              lpScanList,
                                              lpWalkList,
                                              DriveList,
                                              hFillBoot,
                                              lpNav,
                                              hDlg);
                }

            TSR_ON;

            lpWalkList += (STRLEN(lpWalkList) + 1);
            }

        MemUnlock (lpNav->lpScan->hScanList);
        }

    return (bEndScan);
}

BOOL LOCAL ScanTheBootRecord (
    LPSTR       lpDrives,
    LPSTR       lpScanList,
    LPSTR       lpWalkList,
    LPDRIVELIST lpDriveList,
    HWND        hFillBoot,
    LPNONAV     lpNav,
    HWND        hDlg
    )
{
    auto    DRIVEPROBLEMSTRUCT  DriveAccess;
    auto    UINT                uError = 0;
    auto    UINT                i;
    auto    UINT                wMayor;
    auto    UINT                wMinor;
    auto    BOOL                bEndScan = FALSE;
    auto    BOOL                bOkToScan = FALSE;
    auto    BOOL                bRetryScan = TRUE;
    auto    BOOL                bSkipDrive = FALSE;
    auto    char                bDrive;
    auto    LPDRIVELIST         lpCurrDrive;
    auto    LPDRIVELIST         lpScanDrive;
    auto    BYTE                byLockedDrive = 0;

                                        // -----------------------------------
                                        // Scan only the boot track of this
                                        // drive if it has not been scanned
                                        // before, and if its available.
                                        // -----------------------------------

    if (!DriveScanned(lpDrives, *lpWalkList) && lpWalkList[1] != BAD_DRIVE)
        {
        while ( bRetryScan == TRUE )
            {
            TSR_OFF

            bDrive = GetPhysicalDriveLetter( *lpWalkList );
            uError = DiskGetType( bDrive, &wMayor, &wMinor);

            if ((wMayor == TYPE_ENCRYPTED && wMinor == TYPE_CLOSE) || uError == ERR)
                {
                uError = TRUE;
                }
            else if ( HWIsNEC() && wMayor != TYPE_REMOVEABLE )
                {
                                        // On NECs, scan boot records only on
                                        // removable disks (floppies).
                bSkipDrive = TRUE;
                }
            else
                {
#ifdef SYM_WIN32
                                        // -----------------------------------
                                        // The Long Sad Story
                                        //
                                        // Locking the drive to be scanned
                                        // prevents problems with certain HDD
                                        // controllers. In order to make sure
                                        // that we have really gotten the
                                        // SymStat lock on this drive we must
                                        // take the lock on all logical drives
                                        // that share this physical drive.
                                        // If any of them cannot be locked we
                                        // will report this drive as in use.
                                        // -----------------------------------
                for (lpScanDrive = lpDriveList;
                     lpScanDrive->cDriveLetter;
                     lpScanDrive++)
                {
                    if (lpScanDrive->cDriveLetter == *lpWalkList)
                        break;
                }
                for (lpCurrDrive = lpDriveList;
                     lpCurrDrive->cDriveLetter;
                     lpCurrDrive++)
                {
                    if (lpCurrDrive->byPhysicalDrive == lpScanDrive->byPhysicalDrive)
                        if (NOERR != NavStatSetDriveState (lpCurrDrive->cDriveLetter,
                                                           0,
                                                           DRIVESTATUS_FOREGROUND,
                                                           NAV_APP_CLINIC))
                        {
                            uError = ERR_DISKLOCKED_SYMSTAT_BOOT;
                            byLockedDrive = (BYTE)lpCurrDrive->cDriveLetter;
                            break;
                        }
                        else
                        {
                            lpCurrDrive->bDriveLocked = TRUE;
                        }
                }
#endif
                //
                // Kludge applied under much durress.  Hard code detection of 2 specific
                // viruses which only occur on NEC 1.2Mb 3.5" floppy disks.  This is because
                // it is too late ( day before GM1 build! ) to apply the correct fix.
                //
                if (uError == 0)
                {
                    if( HWIsNEC() && IsNEC12MBFloppy( bDrive ) )
                        {
                        uError = ScanNEC12Floppy( bDrive, lpNav );
                        }
                    else
                        {
                        lpNav->Infected.uVirusSigIndex = VirusScanBoot((UINT)bDrive,
                                                                  &lpNav->Infected.Notes, &uError);
                        }
                }

                                        // Check to see if we are running in
                                        // command line mode and an error
                                        // occurred...
                if( (lpNav->lpScan->uProgramMode == PROGRAM_COMMANDLINE ||
                     lpNav->lpScan->uProgramMode == PROGRAM_AUTOMATED) &&
                    (FALSE != uError) )
                {
                    uError = FALSE;
                }

#ifdef SYM_WIN32
                for (lpCurrDrive = lpDriveList;
                     lpCurrDrive->cDriveLetter;
                     lpCurrDrive++)
                    {
                    if (lpCurrDrive->bDriveLocked)
                        {
                        NavStatSetDriveState ( lpCurrDrive->cDriveLetter,
                                               0,
                                               DRIVESTATUS_FREE,
                                               NAV_APP_CLINIC );
                        lpCurrDrive->bDriveLocked = FALSE;
                        }
                    }
#endif
                }

            TSR_ON

            if ( uError )
                {
                                        // -----------------------------------
                                        // We should notify if scanning full
                                        // screen OR when we're running mini-
                                        // mized AND immediate notify flag is
                                        // turned on.
                                        // -----------------------------------

                if ( (!hFillBoot && (lpNav->Opt.bPrestoNotify || IsCutomNotify(lpNav))) || hFillBoot )
                    {
                    MEMSET (&DriveAccess, EOS, sizeof(DriveAccess));
                    DriveAccess.hParent         = hDlg;
                    DriveAccess.byDrive         = *lpWalkList;
                    DriveAccess.bHideContinue   = FALSE;

                    switch ( uError )
                        {
                        case ERR_VIRSCAN_DISKLOCKED:
                            DriveAccess.uInfoMsgID = DRIVEACCESS_MSG_DISKLOCKED;
                            break;

#ifdef SYM_WIN32
                        case ERR_DISKLOCKED_SYMSTAT_BOOT:
                            DriveAccess.uInfoMsgID = DRIVEACCESS_MSG_DISKLOCKED_SYMSTAT_BOOT;
                            NavStatGetDriveState ( byLockedDrive,
                                                   0,
                                                   DriveAccess.szWho,
                                                   sizeof (DriveAccess.szWho) );
                            break;
#endif

                        default:
                            DriveAccess.uInfoMsgID = DRIVEACCESS_MSG_BOOTRECORD;
                            break;
                        }

                    if ( lpNav->Alert.bRemoveAfter )
                        DriveAccess.uTimeOutSeconds = lpNav->Alert.uSeconds;

                    switch ( DriveAccessProblem ( &DriveAccess ) )
                        {
                        case DRIVEERROR_RETRY:
                            break;

                        case DRIVEERROR_ERROR:
                        case DRIVEERROR_CONTINUE:
                            bRetryScan = FALSE;
                            break;

                        case DRIVEERROR_SKIP:
                            RemoveDriveFromList (lpScanList, *lpWalkList);
                            bRetryScan = FALSE;
                            break;
                        }
                    }
                else
                    {
                    RemoveDriveFromList (lpScanList, *lpWalkList);
                    bRetryScan = FALSE;
                    }
                }
            else if ( bSkipDrive )
                {
                bRetryScan = FALSE;
                }
            else
                {
                                    // -----------------------------------
                                    // Make user happy with progress bar
                                    // -----------------------------------
                if ( hFillBoot )
                    {
                    for (i=0; i < 101; i += 20)
                        {
                        auto    DWORD   dwTimeInMs;

                        dwTimeInMs = GetTickCount();
                        while (55 > GetTickCount() - dwTimeInMs)
                            {
                            }

                        Fillbar_SetLevel(hFillBoot, i);
                        }
                    }

                lpNav->Stats.BootRecs.uScanned++;

                bRetryScan = FALSE;

                if (lpNav->Infected.uVirusSigIndex && !uError)
                    {
                    lpNav->Infected.wStatus = FILESTATUS_BOOTINFECTED;
                    lpNav->Stats.BootRecs.uInfected++;

                    if (ReportVirusInBoot (hDlg, (BYTE)*lpWalkList,
                                  lpNav) == TRUE)
                        {
                        bEndScan = TRUE;
                        }
                    }
                }
            }
        }

    return ( bEndScan );
}

VOID SYM_EXPORT WINAPI RemoveDriveFromList (
    LPSTR   lpScanList,
    BYTE    byDriveToRemove
    )
{
    while (*lpScanList)
        {
        if (*lpScanList == byDriveToRemove)
            lpScanList[1] = BAD_DRIVE;

        lpScanList += (STRLEN(lpScanList) + 1);
        }
}


BOOL SYM_EXPORT WINAPI DriveScanned(LPSTR lpDriveList, BYTE byDrive)
{
    auto    BOOL    bStatus = FALSE;
    auto    UINT    i;

    for (i = 0; i < NUM_DRIVES && lpDriveList[i] ; i++)
        if ((BYTE)lpDriveList[i] == byDrive)
            {
            bStatus = TRUE;
            break;
            }

    if (bStatus == FALSE)
        lpDriveList[i] = byDrive;

    return (bStatus);
}

VOID SYM_EXPORT WINAPI NAV_CenterDialog (HWND hDlg)
{
    auto    HWND    hParent;

    hParent = GetParent(hDlg);

    if (IsIconic(hParent))
        NG_CenterDialog (hDlg, NULL);
    else
        NG_CenterDialog (hDlg, hParent);
}



/****************************************************************************
* Function   : GetPhysicalDriveLetter
*
* Description: If the specified drive is substituted, this function will
*              return the target physical drive, else this returns the same
*              drive letter.
*
* Inputs     :
*
* Returns    :
*
****************************************************************************/
BYTE GetPhysicalDriveLetter( char bDrive )
{
#ifdef SYM_WIN32
   static char szDrive[] = "X:";
   static char szSubstDrive351Prefix[] = "\\DosDevices\\";
   static char szSubstDrive4Prefix[]  = "\\??\\";
   auto char szDeviceBuffer[512];

   szDrive[0] = bDrive;
   if( QueryDosDevice( szDrive, szDeviceBuffer, sizeof(szDeviceBuffer) ) )
      {
                        // -------------------------------------------------
                        // Substituted drives under NT 3.51 have the following
                        // syntax:
                        //    \DosDevices\X:\Path
                        // Where X is the actual target physical drive
                        // -------------------------------------------------
      if( !STRNICMP( szDeviceBuffer, szSubstDrive351Prefix,
                    sizeof(szSubstDrive351Prefix) - 1 ) )
          bDrive = szDeviceBuffer[sizeof(szSubstDrive351Prefix) - 1];
                        // -------------------------------------------------
                        // Substituted drives under NT 4.XX have the following
                        // syntax:
                        //    \??\X:\Path
                        // Where X is the actual target physical drive
                        // -------------------------------------------------
      if( !STRNCMP( szDeviceBuffer, szSubstDrive4Prefix,
                    sizeof(szSubstDrive4Prefix) - 1 ) )
          bDrive = szDeviceBuffer[sizeof(szSubstDrive4Prefix) - 1];
      }
#endif
   return( bDrive );
}


//=======================================================================
//
//   Function: EnumFixedDrives
//
//   Purpose:  Loads a character array with a list of all hard drives
//
//   Input:    lpDriveList              // [out] List of hard drives
//
//   Return:   FALSE - Success
//             TRUE - Failed
//
//=======================================================================

BOOL EnumFixedDrives
(
    LPDRIVELIST lpDriveList             // [out] List of hard drives
)
{
    LPSTR lpszDrives    = NULL;
    LPSTR lpszCurrDrive = NULL;

    WORD wType;

    DWORD dwDriveListLength;

                                        // -----------------------------------
                                        // Create a temporary buffer to hold
                                        // the disk drive list we are about
                                        // to request
                                        // -----------------------------------

    dwDriveListLength = GetLogicalDriveStrings (0, lpszDrives);
    dwDriveListLength += 2;

    lpszDrives = (LPSTR)MemAllocPtr (GHND, dwDriveListLength);

    GetLogicalDriveStrings (dwDriveListLength, lpszDrives);

                                        // If there are no drives, how the heck
                                        // did you run this?? But just in case
                                        // someone invents direct insertion of
                                        // programs by mental telepathy...

    if (dwDriveListLength == LB_ERR)
        return (TRUE);

                                        // For each hard drive in the afore
                                        // mentioned list box, make an entry
                                        // in the drive list.

    for (lpszCurrDrive = lpszDrives;
         *lpszCurrDrive;
         lpszCurrDrive += STRLEN (lpszCurrDrive) + 1)
    {
                                        // Force the drive letter to upper case

        STRUPR (lpszCurrDrive);

        // ---------- Get the drive type

        wType = GetDriveType (lpszCurrDrive);

        if (wType == DRIVE_FIXED)
        {
            lpDriveList->cDriveLetter = *lpszCurrDrive;
            lpDriveList->byPhysicalDrive = VirusScanMapLogicalToPhysical (lpDriveList->cDriveLetter);

            lpDriveList++;
        }
    }

                                        // Destroy the evidence

    MemFreePtr (lpszDrives);

    return (FALSE);

}   // ---------- End of EnumFixedDrives ().



///////////////////////////////////////////////////////////////////////////
// NEC boot code/signatures below
///////////////////////////////////////////////////////////////////////////


#define ENC(x) ((x)^0xFF)

typedef struct
{
    WORD            wVirusID;
    BYTE            byLength;
    BYTE            abySig[32];     // ^ 0xFF encrypted
} NEC_BOOT_SIGNATURE;

NEC_BOOT_SIGNATURE gastNECSigs[] =
{
    {0x7AFC, 28,    // JapHal
         { ENC(0xF3), ENC(0xA5), ENC(0x1F), ENC(0xBE),
           ENC(0x4C), ENC(0x00), ENC(0xBF), ENC(0x00),
           ENC(0x02), ENC(0xA5), ENC(0xA5), ENC(0xB8),
           ENC(0x80), ENC(0xFD), ENC(0x89), ENC(0x44),
           ENC(0xFE), ENC(0xC7), ENC(0x44), ENC(0xFC),
           ENC(0x68), ENC(0x33), ENC(0xBE), ENC(0x6C),
           ENC(0x00), ENC(0xBF), ENC(0x04), ENC(0x02)
         }
    },
    {0x7AFD, 29,
         { ENC(0xB8), ENC(0x90), ENC(0x5D), ENC(0xBB),
           ENC(0x04), ENC(0x00), ENC(0xB9), ENC(0x00),
           ENC(0x03), ENC(0xBA), ENC(0x00), ENC(0x01),
           ENC(0x33), ENC(0xED), ENC(0xCD), ENC(0x1B),
           ENC(0x0E), ENC(0x07), ENC(0xB8), ENC(0x91),
           ENC(0x55), ENC(0xBB), ENC(0x00), ENC(0x04),
           ENC(0xBA), ENC(0x01), ENC(0x00), ENC(0xCD),
           ENC(0x1B),
         }
    }
};


#define NUM_NEC_SIGS (sizeof(gastNECSigs) / sizeof(NEC_BOOT_SIGNATURE))


//*************************************************************************
// ScanNEC12Floppy()
//
// ScanNEC12Floppy(
//       BYTE byDrive - Drive letter of drive to scan.
//      LPNONAV lpNav - NONAV stucture containing infection infomration.
//                )
//
// Description: This routine will scan the specified drive for NEC 1.2MB
//              specific boot viruses.
//
// Returns: UINT LOCAL  - zero on success, non-zero otherwise.
//
//*************************************************************************
// 7/22/98 DBUCHES, created - header added.
//*************************************************************************

UINT LOCAL ScanNEC12Floppy( char chDriveLetter, LPNONAV lpNav )
{
    int                                 i, j, k;
    WORD                                wFromBuffer;
    LPBYTE                              lpbyWorkBuffer;
    UINT                                uRet = 0xFFFFFFFF;

    
    __try
        {
        //
        // Clear virus index
        //
        lpNav->Infected.uVirusSigIndex = 0;

        
        //
        // Make sure input is upper case.
        //
        chDriveLetter = toupper( chDriveLetter );

        //
        // Allocate our work buffer
        //
       
        lpbyWorkBuffer = new BYTE[2048];
        if( lpbyWorkBuffer == NULL )
            return 0xFFFFFFFF;
    
        //
        // Read in first sector of this drive.
        //
        ABSDISKREC absDisk;
        ZeroMemory( &absDisk, sizeof( ABSDISKREC ) );

        absDisk.dn = (BYTE) ( chDriveLetter - 'A' );
        absDisk.numSectors = 1;
        absDisk.dwSector = 1;
        absDisk.buffer = lpbyWorkBuffer;
        absDisk.sectorLenCode = 3;
        
        if( DiskAbsOperation( READ_COMMAND, &absDisk ) )
            {
            __leave;
            }

        //
        //  Can't fail after this point
        //
        uRet = 0;

        // encrypt
        for (i=0;i<1024;i++)
            lpbyWorkBuffer[i] = ENC(lpbyWorkBuffer[i]);

        // now scan using a quickie loop

        for (i=0;i<1023;i++)
            {
            wFromBuffer = *(LPWORD)(lpbyWorkBuffer+i);

            for (j=0;j<NUM_NEC_SIGS;j++)
                {   
                // match first word of each sig before verifying the rest.

                if (*(LPWORD)gastNECSigs[j].abySig == wFromBuffer)
                    {
                    for (k=0;k<gastNECSigs[j].byLength;k++)
                        {
                        if (i+k >= 1024 ||
                            gastNECSigs[j].abySig[k] != lpbyWorkBuffer[i+k])
                            break;
                        }

                    // Found a match
                    if (k == gastNECSigs[j].byLength)
                        {
                        WORD wVirusIndex;
                        
                        lpNav->Infected.uVirusSigIndex = gastNECSigs[j].wVirusID;

                        if( VS_OK == VLDetermineVirusIndex( (HVCONTEXT)VirusScanGetScanContext(),
                                                            lpNav->Infected.uVirusSigIndex,
                                                            &wVirusIndex ) )
                            {
                            N30 n30;
                            if( VLLoadN30( (HVCONTEXT)VirusScanGetScanContext(), wVirusIndex, &n30 ) )
                                {

                                lpNav->Infected.Notes.wVirID   = n30.wVirID;
                                lpNav->Infected.Notes.wType    = n30.wType;
                                lpNav->Infected.Notes.wControl2= n30.wControl2;
                                lpNav->Infected.Notes.wInfo    = n30.wInfo;
                                lpNav->Infected.Notes.wSize    = n30.wInfSize;
                                lstrcpyn( lpNav->Infected.Notes.lpVirName, n30.lpVName, N30_SIZE_VIRUSNAME );
                                }
                            else
                                uRet = 0xFFFFFFFF;
                            }
                        
                        __leave;
                        }
                    }
                }
            }
        }
    __finally
        {
        // Cleanup
        if( lpbyWorkBuffer )
            delete [] lpbyWorkBuffer;
        }

    return uRet;    

}   // End of EXTScanBootNEC



#define		NEC_125MB_CYLINDERS		77
#define		NEC_125MB_SECTORS_PER_TRACK 8
#define		NEC_125MB_TRACKS_PER_CYLINDER 2

//*************************************************************************
// IsNEC12MBFloppy()
//
// IsNEC12MBFloppy(
//      char chDriveLetter )
//
// Description:
//
// Returns: BOOL LOCAL  TRUE if the drive letter in question is a 1.2MB floppy
//                      FALSE otherwise.
//
//*************************************************************************
// 7/22/98 DBUCHES, created - header added.
//*************************************************************************

BOOL LOCAL IsNEC12MBFloppy( char chDriveLetter )
{
    BOOL			  bResult = FALSE;

    if (SystemGetWindowsType() == SYM_SYSTEM_WIN_NT)
	    {
        char              szDrive[SYM_MAX_PATH];
        HANDLE            hFile;
        DWORD             dwBytesReturned;
	    DISK_GEOMETRY	  dg;

        wsprintf(szDrive, "\\\\.\\%c:", chDriveLetter );

        hFile = CreateFile(szDrive, GENERIC_READ | GENERIC_WRITE,
                                    FILE_SHARE_READ | FILE_SHARE_WRITE,
                                    NULL,
                                    OPEN_EXISTING,
                                    0,
                                    NULL);

		if (hFile == INVALID_HANDLE_VALUE)
			return bResult;

		if ( DeviceIoControl(hFile, IOCTL_DISK_GET_DRIVE_GEOMETRY,
				NULL, 0, &dg, sizeof(DISK_GEOMETRY), &dwBytesReturned, NULL) )
			{
			if(	(dg.Cylinders.LowPart == NEC_125MB_CYLINDERS) &&
				(dg.TracksPerCylinder == NEC_125MB_TRACKS_PER_CYLINDER)  &&
				(dg.SectorsPerTrack == NEC_125MB_SECTORS_PER_TRACK)  &&
				(dg.BytesPerSector == PHYSICAL_SECTOR_SIZE98) )
				{
				bResult = TRUE;
				}
			}

		CloseHandle(hFile);
        return bResult;
		}
    else
        {
        //
        // On Win9x Systems, just get the bytes per sector count.   If it is 1024,
        // we're most likely looking at a 1.2mb floppy.
        //
        char szDrive[5];
        wsprintf( szDrive, "%c:\\", chDriveLetter );
        DWORD dwSectorsPerCluster, dwBytesPerSector, dwNumberOfFreeClusters, dwTotalNumberOfClusters;
        if( FALSE == GetDiskFreeSpace( szDrive, 
                          &dwSectorsPerCluster, 
                          &dwBytesPerSector, 
                          &dwNumberOfFreeClusters, 
                          &dwTotalNumberOfClusters ) )
            {
            return FALSE;
            }

        if( dwBytesPerSector == PHYSICAL_SECTOR_SIZE98 ) 
            {
            return TRUE;
            }
        else
            return FALSE;

        }
	
    return bResult;
}

