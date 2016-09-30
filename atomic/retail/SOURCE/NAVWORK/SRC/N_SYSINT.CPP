// Copyright 1994 Symantec, Peter Norton Product Group
//************************************************************************
//
// $Header:   S:/NAVWORK/VCS/n_sysint.cpv   1.6   01 Jul 1998 14:11:12   DBuches  $
//
// Description:
//
// Contains:
//
// See Also:
//************************************************************************
// $Log:   S:/NAVWORK/VCS/n_sysint.cpv  $
// 
//    Rev 1.6   01 Jul 1998 14:11:12   DBuches
// Removed How-To help.
// 
//    Rev 1.5   11 Jun 1998 13:29:12   DBuches
// Backed out FAT32 changes.  Moved to SYMINTEG.
// 
//    Rev 1.2   08 Apr 1998 17:15:54   MKEATIN
// Commented out IntegGetSystemNames() in CreateIntegArray() so we only check
// boot/MBR records.  No file innocuating anymore.
// 
//    Rev 1.1   25 Feb 1997 12:39:18   DALLEE
// Port from HYDRA.15:
//            Rev 1.40   09 Oct 1996 12:08:28   CCOTRIL
//         Re-enable network alerting for W32.
// 
//    Rev 1.0   06 Feb 1997 21:02:46   RFULLER
// Initial revision
// 
//    Rev 1.8   23 Jan 1997 13:33:08   JBRENNA
// Use DiskGetBootDrive() instead of DiskGetFirstFixedDisk() when looking for
// the boot drive. On NEC machines, DiskGetFirstFixedDisk() will possibly
// return the wrong drive letter for the boot drive.
// 
//    Rev 1.7   07 Jan 1997 14:54:14   JBRENNA
// Change all IDS_DRIVE_C references to dynamically compute their drive
// letter. This is required because NEC machines have 'A' as the hard drive.
// 
//    Rev 1.6   12 Dec 1996 15:40:14   JBRENNA
// SystemNotInfect(): For NEC, no longer assume C: is the first hard drive. Use
// DiskGetFirstFixedDrive() instead of C:.
// 
//    Rev 1.5   11 Dec 1996 09:43:02   JBRENNA
// Renamed GetSysIntegFile() to GetSysIntegFileOem() because the local function
// was renamed.
// 
//    Rev 1.4   07 Aug 1996 15:36:50   JBRENNA
// LuigiPlus Port: DALLEE: Always set item name for integ changes.
// MBR name is set to new style drive number, no longer "C:" like boot records.
// Fixes problems with activity log.
// 
// For integ change prompt dialog: Forward IDI_CONTINUE to close the dialog
// from Reinoculate or Repair. For some option settings, forwarding
// IDCANCE wouldn't close the dialog.
// 
// 
//    Rev 1.3   11 Jul 1996 08:03:48   TIVANOV
// fix the dialog font initilization
//
//    Rev 1.2   28 Jun 1996 16:05:10   GWESTER
// Ported in fixes from Luigi Plus.
//
//    Rev 1.1   22 Feb 1996 14:08:10   YVANG
// Renamed 'nstdhelp.h' to 'stdhelp.h' to use Quake's StdHelp merged with the
// NAV version.
//
//    Rev 1.0   30 Jan 1996 15:55:44   BARRY
// Initial revision.
//
//    Rev 1.0   30 Jan 1996 14:50:48   BARRY
// Initial revision.
//
//    Rev 1.35   30 Nov 1995 22:42:00   BARRY
// Renamed stdhelp.h to nstdhelp.h to fix trunk and local infodesk problem
//
//    Rev 1.34   15 Nov 1995 21:55:02   DALLEE
// Call S32STAT functions through covers in NAVXUTIL, which dynamically loads
// S32STAT.DLL to get around some path issues on NT.
//
//    Rev 1.33   25 Oct 1995 12:49:54   JWORDEN
// Fix possible GPF during MBR repair
//
//    Rev 1.32   25 Oct 1995 00:59:56   DALLEE
// Move InocPasswordVerify() to N_INOC.CPP.
// This file (n_sysint.cpp) is going to be removed from NAVWNT (Gargoyle)
// builds, since NAVWNT will not use System Inoculation.
//
//    Rev 1.31   24 Oct 1995 19:41:36   JWORDEN
// Separate scan for boot records and MBRs
//
//    Rev 1.30   23 May 1995 16:25:36   KEITH
// Add SYMSTAT.DLL and code to check for locked disks and improved handling
//
//    Rev 1.29   12 May 1995 19:20:08   BARRY
// Fixed defect #35524; Help strings on Continue and Inoculate
// buttons in System Inoculation dialog needed to be switched. (Gary)
//
//
//    Rev 1.28   09 May 1995 10:30:26   KEITH
// Correct some nouns
//
//************************************************************************

#include "platform.h"
#include "ngui.h"
#include "stdhelp.h"
#include "navutil.h"
#include "security.h"
#include "file.h"
#include "nunavdlg.h"
#include "nunavstb.h"
#include "cmbstr.h"
#include "navw_hlp.h"
#include "navalog.h"
#include "ctl3d.h"

#include "syminteg.h"

#include "nonav.h"
#include "virscan.h"

#include "netalert.h"

#ifdef SYM_WIN32
    #include "navstat.h"
    #include "symstat.h"
    #include "navver.h"
    #include "disk.h"
#endif

extern HINSTANCE hInstance;

// ************************************************************************ //
// ****                       Local Prototypes                         **** //
// ************************************************************************ //

VOID LOCAL WriteSysintegLog (
    HWND    hDlg,
    LPNONAV lpNav
    );

VOID LOCAL IntegrityReinoc (
    HWND    hDlg,
    LPNONAV lpNav
    );

VOID LOCAL IntegrityRepair (
    HWND    hDlg,
    LPNONAV lpNav
    );

VOID LOCAL DisplayWhatFailed (
    HWND    hDlg,
    LPNONAV lpNav
    );

BOOL LOCAL StartedFromFloppy (
    LPINTEG lpInteg
    );

BOOL LOCAL IntegVerifyFailed (
    HWND    hDlg,
    LPNONAV lpNav
    );

BOOL LOCAL DoIntegCreate (
    HWND    hDlg,
    LPNONAV lpNav,
    BOOL    bPrompt
    );

BOOL LOCAL CreateSysInteg (
    HWND    hDlg,
    LPNONAV lpNav,
    BOOL    bPrompt
    );

VOID LOCAL FreeIntegArray (
    LPNONAV lpNav
    );

VOID LOCAL CreateIntegArray (
    LPNONAV lpNav
    );

BOOL LOCAL SystemNotInfected (
    HWND    hDlg,
    LPNONAV lpNav
    );

VOID LOCAL IntegVerifyFailedStatus (
    LPNONAV lpNav
    );

VOID LOCAL EnableSysIntegButtons (
    HWND    hDlg,
    LPNONAV lpNav
    );

BOOL SYM_EXPORT WINAPI IntegPromptDlgProc (
    HWND    hDlg,
    UINT    uMessage,
    WPARAM  wParam,
    LPARAM  lParam
    );

BOOL SYM_EXPORT WINAPI IntegDlgProc (
    HWND    hDlg,
    UINT    uMessage,
    WPARAM  wParam,
    LPARAM  lParam
    );

VOID LOCAL AlertInocChanged (
    LPNONAV lpNav,
    LPSTR   lpFileName
    );

VOID LOCAL NavNameReturnDrive (
    LPSTR   lpPath,
    LPSTR   lpDrive
    );

#ifdef SYM_WIN32

VOID LOCAL OnContextMenu (              // Respond to WM_CONTEXTMENU message
    HWND    hDlg,                       // [in] dialog that sent the message.
    UINT    uMsg,
    WPARAM  wParam,                     // [in] wParam from dialog
    LPARAM  lParam                      // [in] lParam from dialog
    );

#endif

// ************************************************************************ //

    static INTEG lpIntegArray[] = {
        {0,"MBR80"},
        {0,"BOOT800"},
        {0,"BOOT801"},
        {0,"BOOT802"},
        {0,"BOOT803"},
        {0,""},
        {0,""},
        {0,""},
        {0,""},
        {0,""},
        {0,""},
        {0,""},
        {0,""},
        {0,""},
        {0,""},
        {0,""},
        {0,""},
        {0,""},
        {0,""},
        {0,""},
        {0,""},
        {0,""},
        {0,""},
        {0,""},
        {0,""}
        };

// ************************************************************************ //

BOOL SYM_EXPORT WINAPI Integrity (HWND hDlg, LPNONAV lpNav)
{
    extern       HINSTANCE       hInstance;
    auto         DLGPROC         lpfn;
    auto         int             nStatus;


    lpfn = (DLGPROC) MakeProcInstance((FARPROC)IntegDlgProc, hInstance);
    nStatus = DialogBoxParam(hInstance, MAKEINTRESOURCE (IDD_SYSINTEG),
                                hDlg, lpfn, (LPARAM)lpNav);
    FreeProcInstance((FARPROC) lpfn);
    if (nStatus == -1)
        nStatus = TRUE;

    return((BOOL) nStatus);
}

// ************************************************************************ //

BOOL SYM_EXPORT WINAPI IntegDlgProc (
    HWND    hDlg,
    UINT    uMessage,
    WPARAM  wParam,
    LPARAM  lParam )
{
    static  BOOL        bPaintProcessed = FALSE;
    auto    LPNONAV     lpNav;
    auto    BOOL        bReturn = FALSE;

    switch(uMessage)
        {
        case WM_CTLCOLOR:
            bReturn = (BOOL)Ctl3dCtlColorEx(uMessage, wParam, lParam);
            break;

        case WM_INITDIALOG:
            Ctl3dSubclassDlg(hDlg, CTL3D_ALL);
            NG_SetFontsInDialog (hDlg, 0);
            NG_SetChildSmallFont(hDlg, IDI_SYSINTEG_TEXT);
            NAV_CenterDialog (hDlg);
            bPaintProcessed = FALSE;

            lpNav = (LPNONAV)lParam;
            LP_SET(hDlg, lpNav);

            Static_SetText (GetDlgItem(hDlg, IDI_SYSINTEG_TEXT),
                            String_Ptr(IDS_SYSINTEG_CHECKING));
            bReturn = TRUE;
            break;
                                        // WM_ACTIVATE:  Set help context
                                        // for dialog so F1 works.
        case WM_ACTIVATE:
            if ( GET_WM_ACTIVATE_STATE(wParam,lParam) )
                {
                StdHelpSetContext (HELP_DLG_ASK_ACTION,
                                   HELP_DLG_ASK_ACTION);
                }
            break;

        case WM_DESTROY:
            LP_FREE(hDlg);
            break;

        case WM_PAINT:
            if ( bPaintProcessed == FALSE )
                {
                bPaintProcessed = TRUE;
                PostMessage(hDlg, WM_NAVWORKUSER, 0, 0L);
                }
            break;


        case WM_NAVWORKUSER:
            UpdateWindow(hDlg);
            lpNav = (LPNONAV)LP_GET (hDlg);
            EndDialog (hDlg, DoWholeIntegScenario (hDlg, lpNav, TRUE));
            break;
        }

    return ( bReturn );
}

UINT SYM_EXPORT WINAPI DoWholeIntegScenario (
    HWND    hDlg,
    LPNONAV lpNav,
    BOOL    bPrompt )
{
    auto    UINT    uResult;
    auto    char    szIntegFile [ SYM_MAX_PATH + 1 ];

    CreateIntegArray (lpNav);

    if (StartedFromFloppy (lpNav->lpInteg))
        uResult = TRUE;
    else
        if ( SystemNotInfected (hDlg, lpNav) == TRUE)
            {
            GetSysIntegFileOem (szIntegFile, sizeof(szIntegFile), &lpNav->Inoc);

            uResult = IntegVerify(szIntegFile, lpNav->lpInteg);

            switch ( uResult )
                {
                case 2:                     // Couldn't open database:  NEW
                    uResult = CreateSysInteg (hDlg, lpNav, bPrompt);
                    break;

                case TRUE:                  // All system inoculation entries matched.
                    uResult = TRUE;         // Continue, everything is OK
                    break;

                case 3:                     // Mem alloc error happened.
                case FALSE:                 // Found a difference
                default:
                    uResult = IntegVerifyFailed (hDlg, lpNav);

                    if ( IntegVerify(szIntegFile, lpNav->lpInteg) != 4 )
                        break;

                    // Fall through if after responding the worst thing
                    // that is left is some missing files.  In all other
                    // cases (particularly verify errors), live with it.

                case 4:                     // Some inoc entries missing
                    // Emulate INOC_AUTO
                    DoIntegCreate (hDlg, lpNav, FALSE);
                    uResult = TRUE;
                    break;
                }
            }
        else
            uResult = lpNav->Inoc.bDispCont;

    FreeIntegArray (lpNav);

    return (uResult);
}

BOOL LOCAL StartedFromFloppy (
    LPINTEG lpInteg )
{
    auto    LPINTEG     lpArray;
    auto    char        cDrive;

    for ( lpArray = &lpInteg[5] ; *lpArray->byFilename ; lpArray ++ )
        {
        cDrive = *lpArray->byFilename;

        if ( cDrive == 'A' || cDrive == 'B' ||
             cDrive == 'a' || cDrive == 'b' )
            {
            return TRUE;
            }
        }

    return FALSE;
}


// The original ignored errors and reported the first virus scanned.
// This has functionality has been retained, but a check for a
// SymStat lock on the drive is made before any scans are attempted.

BOOL LOCAL SystemNotInfected (
    HWND    hDlg,
    LPNONAV lpNav )
{
    const   BYTE            BootDriveLtr      = DiskGetBootDrive ();
    auto    char            szBootDriveLtr[8] = { BootDriveLtr, ':', EOS };
    auto    NOTIFYSTRUCT    Notify;
    auto    BOOL            bStatus = FALSE;
    auto    N30NOTES        N30Notes;
    auto    UINT            i;
    auto    char            szWork[SYM_MAX_PATH];

                                        // Notify.uMsgID is used as a
                                        // virus found flag in this module.
    MEMSET (&Notify, '\0', sizeof(Notify));

                                        // Check the master boot record

    if ( VirusScanMasterBoot (FIRST_HD, &N30Notes, (LPUINT)&i) )
        {
        Notify.uMsgID     = IDS_SYSINTEG_VIRUSINMASTERBOOT;
        Notify.lpVirusName= N30Notes.lpVirName;

        // SPRINTF (szWork, "%d", FIRST_HD - FIRST_HD;
        szWork[0] = '0';
        szWork[1] = EOS;

        Notify.lpFileName = szWork;
        }


#ifdef SYM_WIN32
                                        // If locked, skip boots and continue
    if ( NOERR == NavStatSetDriveState ( BootDriveLtr,
                                         0,
                                         DRIVESTATUS_FOREGROUND,
                                         NAV_APP_CLINIC ) )
#endif
        {
        if ( VirusScanBoot(BootDriveLtr, &N30Notes, (LPUINT)&i) )
            {
            Notify.uMsgID     = IDS_SYSINTEG_VIRUSINBOOT;
            Notify.lpFileName = szBootDriveLtr;
            }
        }

#ifdef SYM_WIN32
    NavStatSetDriveState ( BootDriveLtr,
                           0,
                           DRIVESTATUS_FREE,
                           NAV_APP_CLINIC );
#endif
                                        // If no virus found in boots,
                                        // try the integ list.
    if ( !Notify.uMsgID )
        {
        for (i = 5, bStatus = FALSE; *lpNav->lpInteg[i].byFilename ; i++)
            {
            bStatus = VirusScanFile(lpNav->lpInteg[i].byFilename,
                                    &N30Notes, TRUE);
            if ( bStatus )
                {
                Notify.uMsgID = IDS_SYSINTEG_VIRUSINSYSFILE;
                Notify.lpFileName = lpNav->lpInteg[i].byFilename;
                break;
                }
            }
        }

    if ( Notify.uMsgID )
        {
        bStatus = FALSE;

        Notify.hParent      = hDlg;
        Notify.uTitleID     = IDS_NAV;
        Notify.lpVirusName  = N30Notes.lpVirName;
        Notify.lpAlertMsg   = (lpNav->Alert.bDispAlertMsg) ?
                               lpNav->Alert.szAlertMsg : NULL;
        Notify.uRemoveAfter = (lpNav->Alert.bRemoveAfter) ?
                               lpNav->Alert.uSeconds : 0;
        Notify.hInstance    = hInstance;

        NotifyUser (&Notify);
        }
    else
        bStatus = TRUE;

    return (bStatus);
}


VOID LOCAL CreateIntegArray (
    LPNONAV lpNav )
{
    auto    UINT    i;

    if ((lpNav->lpInteg = (LPINTEG)MemAllocPtr (GHND, sizeof(lpIntegArray))) != NULL)
        {
        MEMCPY (lpNav->lpInteg, lpIntegArray, sizeof(lpIntegArray));


        // In Atomic, we only use SysInteg for MBR/Boot records -
        // so we comment out IntegGetSystemNames();

        //IntegGetSystemNames (&lpNav->lpInteg[5]);

        for (i = 5; *lpNav->lpInteg[i].byFilename != EOS; i++)
            AnsiUpper(lpNav->lpInteg[i].byFilename);
        }
}


VOID LOCAL FreeIntegArray (LPNONAV lpNav)
{
    if ( lpNav->lpInteg )
        MemFreePtr ( lpNav->lpInteg );

    lpNav->lpInteg = NULL;              // NULL so we don't try to re-free
}


BOOL LOCAL CreateSysInteg (
    HWND    hDlg,
    LPNONAV lpNav,
    BOOL    bPrompt )
{
    const   BYTE            BootDriveLtr      = DiskGetBootDrive ();
    auto    char            szBootDriveLtr[8] = { BootDriveLtr, ':', EOS };
    auto    NOTIFYSTRUCT    Notify;
    auto    YESNOSTRUCT     YesNo;

    switch ( lpNav->Inoc.uActionNewInoc )
        {
        case INOC_PROMPT:
            {
            static DWORD    dwWhatsThisSystemInoc[] =
                {
                IDOK,       IDH_NAVW_WHATHIS_SYSTEM_INOC_DLG_INOC_BTN,
                IDCANCEL,	IDH_NAVW_WHATHIS_SYSTEM_INOC_DLG_CONTINUE_BTN,
                0,          0
                };

            MEMSET (&YesNo, EOS, sizeof(YesNo));

            YesNo.hParent       =   hDlg;
            YesNo.uQuestionID   =   IDS_CREATESYSINTEG;
            YesNo.uYesButtonID  =   IDS_BUTTON_INOCULATE;
            YesNo.uNoButtonID   =   IDS_BUTTON_CONTINUE;
            YesNo.uTitleID      =   IDS_TITLE_SYSINOC;
            YesNo.hInstance     =   hInstance;
            YesNo.bYesDisabled  =   !lpNav->Inoc.bDispInoc;
            YesNo.bNoDisabled   =   !lpNav->Inoc.bDispCont;
            YesNo.lpdwWhatsThisArray = dwWhatsThisSystemInoc;
            YesNo.uHelpBtnID    =   IDH_NAVW_SYSTEM_INOC_DLG_HELP_BTN;

            if ( AskYesNo (&YesNo) == TRUE )
                DoIntegCreate (hDlg, lpNav, bPrompt);
            }

            break;

        case INOC_AUTO:
            DoIntegCreate (hDlg, lpNav, bPrompt);

            break;

        case INOC_REPORT:
        case INOC_DENY:
            if ( bPrompt )
                {
                MEMSET (&Notify, '\0', sizeof(Notify));
                Notify.hParent      = hDlg;
                Notify.uMsgID       = IDS_SYSINTEG_INTEGNOTEXISTS;
                Notify.uTitleID     = IDS_NAV;
                Notify.lpFileName   = szBootDriveLtr;
                Notify.lpAlertMsg   = (lpNav->Alert.bDispAlertMsg) ?
                                       lpNav->Alert.szAlertMsg : NULL;
                Notify.uRemoveAfter = (lpNav->Alert.bRemoveAfter) ?
                                       lpNav->Alert.uSeconds : NULL;
                Notify.hInstance    = hInstance;

                NotifyUser (&Notify);
                }

            lpNav->Infected.wStatus = FILESTATUS_INTEG_NOTEXIST;

            if (lpNav->Act.bInocChange == TRUE)
                WriteSysintegLog (hDlg, lpNav);
            break;
        }

    return (TRUE);
}

BOOL LOCAL DoIntegCreate (
    HWND    hDlg,
    LPNONAV lpNav,
    BOOL    bPrompt )
{
    const   BYTE            BootDriveLtr      = DiskGetBootDrive ();
    auto    char            szBootDriveLtr[8] = { BootDriveLtr, ':', EOS };
    auto    NOTIFYSTRUCT    Notify;
    auto    BOOL            bStatus = FALSE;
    auto    char            szIntegFile [ SYM_MAX_PATH+1 ];
    auto    char            szMessage   [ SYM_MAX_PATH+1 ];

    if (InocPasswordVerify(hDlg, lpNav))
        {
        if ( bPrompt )
            {
            String_Get (IDS_SYSINTEG_CREATING, szIntegFile, sizeof(szIntegFile));
            SPRINTF (szMessage, szIntegFile, (LPSTR)szBootDriveLtr);
            Static_SetText (GetDlgItem(hDlg, IDI_SYSINTEG_TEXT), szMessage);
            }

        GetSysIntegFileOem (szIntegFile, sizeof(szIntegFile), &lpNav->Inoc);

        if ( IntegCreate (szIntegFile, lpNav->lpInteg) == TRUE )
            bStatus = TRUE;
        else
            bStatus = FALSE;

        if ( bPrompt && !bStatus )
            {
            MEMSET (&Notify, '\0', sizeof(Notify));
            Notify.hParent      = hDlg;
            Notify.uMsgID       = IDS_SYSINTEG_CREATEFAILED;
            Notify.uTitleID     = IDS_NAV;
            Notify.lpFileName   = szBootDriveLtr;
            Notify.lpAlertMsg   = (lpNav->Alert.bDispAlertMsg) ?
                                   lpNav->Alert.szAlertMsg : NULL;
            Notify.uRemoveAfter = (lpNav->Alert.bRemoveAfter) ?
                                   lpNav->Alert.uSeconds : NULL;
            Notify.hInstance    = hInstance;

            NotifyUser (&Notify);
            }

        if (lpNav->Act.bInocChange == TRUE)
            {
            lpNav->Infected.wStatus = (bStatus) ? FILESTATUS_INTEG_CREATED :
                                      FILESTATUS_INTEG_CREATEFAILED;
            WriteSysintegLog (hDlg, lpNav);
            }
        }

    return (bStatus);
}


BOOL LOCAL IntegVerifyFailed (
    HWND    hDlg,
    LPNONAV lpNav )
{
    const   BYTE            BootDriveLtr      = DiskGetBootDrive ();
    auto    char            szBootDriveLtr[8] = { BootDriveLtr, ':', EOS };
    auto    char            szIntegFile [ SYM_MAX_PATH+1 ];
    auto    BOOL            bContinue = TRUE;
    auto    NOTIFYSTRUCT    Notify;
    auto    BOOL            bNotified = FALSE;

    GetSysIntegFileOem (szIntegFile, sizeof(szIntegFile), &lpNav->Inoc);

    if (IntegVerifyDatabaseMatch (szIntegFile) == TRUE)
        {
        for (lpNav->bDoScan = 0;
            *lpNav->lpInteg[lpNav->bDoScan].byFilename && bContinue == TRUE;
             lpNav->bDoScan++ )
            {
            if (lpNav->lpInteg[lpNav->bDoScan].wStatus == INTEG_FAIL_VERIFY)
                {
                AlertInocChanged(lpNav,
                                (LPSTR)&lpNav->lpInteg[lpNav->bDoScan].byFilename);

                IntegVerifyFailedStatus (lpNav);

                switch (lpNav->Inoc.uActionInocChg)
                    {
                    case INOC_PROMPT:
                    case INOC_DENY:
                        bContinue = SysIntegPrompt (hDlg, lpNav);
                        break;

                    case INOC_REPORT:
                                        // -----------------------------------
                                        // Check if we have already notified
                                        // the user about the event. This avoids
                                        // multiple notifications.
                                        // -----------------------------------

                        if ( bNotified == FALSE )
                            {
                            bNotified = TRUE;

                            MEMSET (&Notify, '\0', sizeof(Notify));
                            Notify.hParent      = hDlg;
                            Notify.uMsgID       = IDS_SYSINTEG_CHANGED;
                            Notify.uTitleID     = IDS_NAV;
                            Notify.lpFileName   = szBootDriveLtr;
                            Notify.lpAlertMsg   = (lpNav->Alert.bDispAlertMsg) ?
                                                   lpNav->Alert.szAlertMsg : NULL;
                            Notify.uRemoveAfter = (lpNav->Alert.bRemoveAfter) ?
                                                   lpNav->Alert.uSeconds : NULL;
                            Notify.hInstance    = hInstance;

                            NotifyUser (&Notify);

                            if (lpNav->Act.bInocChange == TRUE)
                                WriteSysintegLog (hDlg, lpNav);

                            bContinue = lpNav->Inoc.bDispCont;
                            }
                        break;
                    }
                }
            }
        }
    else
        {
        DoIntegCreate (hDlg, lpNav, FALSE);
        }

    return ( bContinue );
}


VOID LOCAL AlertInocChanged (
    LPNONAV lpNav,
    LPSTR   lpFileName )
{
    auto    NETALERT        Alert;
    auto    INFECTEDFILE    Infected;
    auto    UINT            n;

    MEMSET (&Alert,    EOS, sizeof(Alert));
    MEMSET (&Infected, EOS, sizeof(Infected));

                                        // ----------------------------------
                                        // Check if it's the master boot rec.
                                        // ----------------------------------

    if (!STRCMP(lpFileName, lpIntegArray[0].byFilename))
        Alert.uTypeOfEvent = EV_ALERT_SYSTEMINOC_MBOOT_CHANGE;
    else
        {
                                        // ----------------------------------
                                        // Check if it's the boot record.
                                        // ----------------------------------
        for (n = 1; n < 5; n++)
            {
            if (!STRCMP(lpFileName, lpIntegArray[n].byFilename))
                {
                Alert.uTypeOfEvent = EV_ALERT_SYSTEMINOC_BOOT_CHANGE;
                break;
                }
            }
        }
                                        // ----------------------------------
                                        // If it wasn't a boot record change
                                        // treat it as a regular file inoc.
                                        // change.
                                        // ----------------------------------
    if (!Alert.uTypeOfEvent)
        {
        Alert.uTypeOfEvent = EV_ALERT_INOC_CHANGE;
        STRCPY (Infected.szFullPath, lpFileName);
        }

    Alert.lpNet          = lpNav->lpNetUserItem;
    Alert.lpInfected     = &Infected;
    Alert.lpAlertOpt     = &lpNav->Alert;
    NetAlert (&Alert);
}


//************************************************************************
// IntegVerifyFailedStatus()
//
// This routine fills out the LPNONAV->Infected struct with the name of
// the changed system inoc item and the correct status.
//
// Parameters:
//  LPNONAV lpNav                       Info for this scan.
//
// Returns:
//  Nothing.
//************************************************************************
// 07/18/1996 DALLEE, initialize item name as well as status here.
//************************************************************************

VOID LOCAL IntegVerifyFailedStatus (LPNONAV lpNav)
{
    switch ((UINT)lpNav->bDoScan)
        {
        case 0:
                                        // Fill in "name" for MBR of
                                        // physical drive #0
            lpNav->Infected.szFullPath[0] = 0;
            lpNav->Infected.szFullPath[1] = ':';
            lpNav->Infected.wStatus = FILESTATUS_INTEG_MBCHANGED;
            break;

        case 1:
        case 2:
        case 3:
        case 4:
            {
            const   BYTE  BootDriveLtr      = DiskGetBootDrive ();
            auto    char  szBootDriveLtr[8] = { BootDriveLtr, ':', EOS };

            STRCPY(lpNav->Infected.szFullPath, szBootDriveLtr);
            lpNav->Infected.wStatus = FILESTATUS_INTEG_BOOTCHANGED;
            break;
            }

        default:
            lpNav->Infected.wStatus = FILESTATUS_INTEG_SYSFILECHANGED;
            STRCPY (lpNav->Infected.szFullPath,
                    lpNav->lpInteg[(UINT)lpNav->bDoScan].byFilename);
            break;
        }
}


BOOL SYM_EXPORT WINAPI SysIntegPrompt (HWND hDlg, LPNONAV lpNav)
{
    extern       HINSTANCE       hInstance;
    auto         DLGPROC         lpfn;
    auto         int             nStatus;


    lpfn = (DLGPROC) MakeProcInstance(
                                (FARPROC)IntegPromptDlgProc,
                                hInstance
                                );
    SYM_ASSERT ( lpfn );

    nStatus = DialogBoxParam (
                            hInstance,
                            MAKEINTRESOURCE (IDD_SYSINTEG_PROMPT),
                            hDlg,
                            lpfn,
                            (LPARAM)lpNav
                            );

    FreeProcInstance((FARPROC) lpfn);

    SYM_ASSERT ( nStatus != -1 );

    if ( nStatus == -1 )
        nStatus = TRUE;

    return ( (BOOL) nStatus );
}

// ************************************************************************ //

BOOL SYM_EXPORT WINAPI IntegPromptDlgProc (
    HWND    hDlg,
    UINT    uMessage,
    WPARAM  wParam,
    LPARAM  lParam
    )
{
    auto    BOOL        bReturn = FALSE;
    auto    LPNONAV     lpNav;

    switch ( uMessage )
        {
        case WM_CTLCOLOR:
            bReturn = (BOOL)Ctl3dCtlColorEx(uMessage, wParam, lParam);
            break;

        case WM_INITDIALOG:
            Ctl3dSubclassDlg(hDlg, CTL3D_ALL);
            NG_SetFontsInDialog (hDlg, 0);
            NG_SetChildSmallFont(hDlg, IDI_SYSINTEG_TEXT);
            NG_SetChildSmallFont(hDlg, IDI_SYSINTEG_TEXT2);
            NAV_CenterDialog (hDlg);

            lpNav = (LPNONAV)lParam;
            LP_SET(hDlg, lpNav);

            if ( lpNav->Alert.bDispAlertMsg )
                SetDlgItemText (hDlg, IDI_SYSINTEG_ALERT,
                                lpNav->Alert.szAlertMsg);

            DisplayWhatFailed (hDlg, lpNav);

            EnableSysIntegButtons(hDlg, lpNav);

            bReturn = TRUE;
            break;
                                        // WM_ACTIVATE:  Set help context
                                        // for dialog so F1 works.
        case WM_ACTIVATE:
            if ( GET_WM_ACTIVATE_STATE(wParam,lParam) )
                {
                StdHelpSetContext (HELP_DLG_ASK_ACTION,
                                   HELP_DLG_ASK_ACTION);
                }
            break;

#ifdef SYM_WIN32
        case WM_HELP:
        case WM_CONTEXTMENU:
                                        // ----------------------------------
                                        // Bring up right click help menu.
                                        // ----------------------------------
            OnContextMenu ( hDlg, uMessage, wParam, lParam );
            break;
#endif

        case WM_DESTROY:
            LP_FREE(hDlg);
            break;

        case WM_COMMAND:
            lpNav = (LPNONAV)LP_GET (hDlg);

            switch ( GET_WM_COMMAND_ID(wParam,lParam) )
                {
                case IDCANCEL:
                    if (lpNav->Inoc.bDispCont == TRUE)
                        {
                        FORWARD_WM_COMMAND ( hDlg, IDI_SYSINTEG_CONT,
                                    GetDlgItem ( hDlg, IDI_SYSINTEG_CONT ),
                                    BN_CLICKED, PostMessage );
                        }
                    else if (lpNav->Inoc.bDispStop == TRUE)
                        {
                        FORWARD_WM_COMMAND ( hDlg, IDI_SYSINTEG_STOP,
                                    GetDlgItem ( hDlg, IDI_SYSINTEG_STOP ),
                                    BN_CLICKED, PostMessage );
                        }
                    break;

                case IDI_SYSINTEG_REPAIR:
                    IntegrityRepair (hDlg, lpNav);
                    Button_Enable (GetDlgItem (hDlg, IDI_SYSINTEG_REPAIR),
                                   FALSE);

                    FORWARD_WM_COMMAND ( hDlg, IDI_SYSINTEG_CONT,
                                GetDlgItem ( hDlg, IDI_SYSINTEG_CONT ),
                                BN_CLICKED, PostMessage );
                    break;

                case IDI_SYSINTEG_INOC:
                    IntegrityReinoc (hDlg, lpNav);
                    Button_Enable (GetDlgItem (hDlg, IDI_SYSINTEG_INOC),
                                   FALSE);
                    FORWARD_WM_COMMAND ( hDlg, IDI_SYSINTEG_CONT,
                                GetDlgItem ( hDlg, IDI_SYSINTEG_CONT ),
                                BN_CLICKED, PostMessage );
                    break;

                case IDI_SYSINTEG_CONT:
                    if (lpNav->Act.bInocChange == TRUE)
                        WriteSysintegLog (hDlg, lpNav);

                    EndDialog (hDlg, TRUE);
                    break;

                case IDI_SYSINTEG_STOP:
                    if (lpNav->Act.bInocChange == TRUE)
                        WriteSysintegLog (hDlg, lpNav);
                    EndDialog (hDlg, FALSE);
                    break;

                case IDHELP:
                    StdHelpFromDialog (
                                    hDlg,
                                    IDH_NAVW_SYSTEM_INTEGRITY_DLG_HELP_BTN
                                    );
                    break;
                }
            break;
        }

    return ( bReturn );
}

VOID LOCAL DisplayWhatFailed (HWND hDlg, LPNONAV lpNav)
{
    auto    char    szMessage [ SIZE_WORK ];
    auto    char    szTemp    [ SIZE_WORK ];

    MEMSET (szMessage, EOS, sizeof(szMessage));
    MEMSET (szTemp, EOS, sizeof(szTemp));

    switch ((UINT)lpNav->bDoScan)
        {
        case 0:
            lpNav->Infected.wStatus = FILESTATUS_INTEG_MBCHANGED;
            String_Get (IDS_MASTERBOOTRECORD, szTemp, sizeof(szTemp));
            break;

        case 1:
        case 2:
        case 3:
        case 4:
            lpNav->Infected.wStatus = FILESTATUS_INTEG_BOOTCHANGED;
            String_Get (IDS_BOOTRECORD, szTemp, sizeof(szTemp));
            break;

        default:
            lpNav->Infected.wStatus = FILESTATUS_INTEG_SYSFILECHANGED;
            STRCPY (szTemp, lpNav->lpInteg[(UINT)lpNav->bDoScan].byFilename);
            break;
        }

    if (*szTemp)
        {
        SPRINTF (szMessage, String_Ptr(IDS_SYSINTEG_DOESNOTMATCH), (LPSTR)szTemp);
        Static_SetText (GetDlgItem (hDlg, IDI_SYSINTEG_TEXT), szMessage);
        }
}

VOID LOCAL IntegrityRepair (HWND hDlg, LPNONAV lpNav)
{
    auto    NOTIFYSTRUCT    Notify;
    auto    char            szIntegFile [ SYM_MAX_PATH + 1 ];
    auto    BOOL            bShowCustomAlert = TRUE;
    auto    BOOL            bStatus;

    MEMSET (&Notify, '\0', sizeof(Notify));

    GetSysIntegFileOem (szIntegFile, sizeof(szIntegFile), &lpNav->Inoc);

    bStatus = IntegRepair (szIntegFile,
                           &lpNav->lpInteg[(UINT)lpNav->bDoScan]);

    switch ((UINT)lpNav->bDoScan)
        {
        case 0:
            if (bStatus == TRUE)
                {
                Notify.uMsgID = IDS_SYSINTEG_MBREPAIRED;
                lpNav->Infected.wStatus = FILESTATUS_INTEG_MBREPAIRED;
                bShowCustomAlert = FALSE;
                }
            else
                {
                Notify.uMsgID = IDS_SYSINTEG_MBREPAIRFAILED;
                lpNav->Infected.wStatus = FILESTATUS_INTEG_MBREPAIRFAILED;
                }
            break;

        case 1:
        case 2:
        case 3:
        case 4:
            if (bStatus == TRUE)
                {
                Notify.uMsgID = IDS_SYSINTEG_BOOTREPAIRED;
                lpNav->Infected.wStatus = FILESTATUS_INTEG_BOOTREPAIRED;
                bShowCustomAlert = FALSE;
                }
            else
                {
                Notify.uMsgID = IDS_SYSINTEG_BOOTREPAIRFAILED;
                lpNav->Infected.wStatus = FILESTATUS_INTEG_BOOTREPAIRFAILED;
                }
            break;

        default:
            if (bStatus == TRUE)
                {
                Notify.uMsgID = IDS_SYSINTEG_SYSFILEREPAIRED;
                lpNav->Infected.wStatus = FILESTATUS_INTEG_SYSFILEREPAIRED;
                bShowCustomAlert = FALSE;
                }
            else
                {
                Notify.uMsgID = IDS_SYSINTEG_SYSFILEREPAIRFAILED;
                lpNav->Infected.wStatus = FILESTATUS_INTEG_SYSFILEREPAIRFAILED;
                }
            break;
        }

    Notify.hParent       = hDlg;
    Notify.uTitleID      = IDS_NAV;
    Notify.uRemoveAfter  = (lpNav->Alert.bRemoveAfter) ?
                            lpNav->Alert.uSeconds : NULL;
    Notify.hInstance     = hInstance;

    if ( bShowCustomAlert == TRUE )
        {
        Notify.lpAlertMsg = (lpNav->Alert.bDispAlertMsg) ?
                            lpNav->Alert.szAlertMsg : NULL;
        }

    NotifyUser (&Notify);
}

VOID LOCAL IntegrityReinoc (HWND hDlg, LPNONAV lpNav)
{
    auto    NOTIFYSTRUCT    Notify;
    auto    char            szIntegFile [ SYM_MAX_PATH + 1 ];
    auto    BOOL            bStatus = FALSE;

    GetSysIntegFileOem (szIntegFile, sizeof(szIntegFile), &lpNav->Inoc);

    FileDelete (szIntegFile);

    FreeIntegArray (lpNav);
    CreateIntegArray (lpNav);

    bStatus = IntegCreate (szIntegFile, lpNav->lpInteg);

    lpNav->Infected.wStatus = (bStatus) ? FILESTATUS_INTEG_REINOCULATED :
                                          FILESTATUS_INTEG_REINOCULATEFAILED;

    if ( !bStatus )
        {
        const   BYTE  BootDriveLtr      = DiskGetBootDrive ();
        auto    char  szBootDriveLtr[8] = { BootDriveLtr, ':', EOS };

        MEMSET (&Notify, '\0', sizeof(Notify));
        Notify.hParent      = hDlg;
        Notify.uMsgID       = IDS_SYSINTEG_REINOCULATEFAILED;
        Notify.uTitleID     = IDS_NAV;
        Notify.lpFileName   = szBootDriveLtr;
        Notify.lpAlertMsg   = (lpNav->Alert.bDispAlertMsg) ?
                                lpNav->Alert.szAlertMsg : NULL;
        Notify.uRemoveAfter = (lpNav->Alert.bRemoveAfter) ?
                                lpNav->Alert.uSeconds : NULL;
        Notify.hInstance    = hInstance;

        NotifyUser (&Notify);
        }
}

VOID LOCAL WriteSysintegLog (HWND hDlg, LPNONAV lpNav)
{
    const   BYTE            BootDriveLtr      = DiskGetBootDrive ();
    auto    char            szBootDriveLtr[8] = { BootDriveLtr, ':', EOS };
    auto    LOGSTRUCT       Log;
    auto    UINT            uLogAction;
    auto    LPSTR           lpIntegChanged = NULL;
    auto    char            szTemp [ SYM_MAX_PATH + 1 ] = { 0 };

    MEMSET (&Log, EOS, sizeof(Log));

    switch (lpNav->Infected.wStatus)
        {
        case FILESTATUS_INTEG_CREATED:
            break;

        case FILESTATUS_INTEG_CREATEFAILED:
            uLogAction = LOGEV_INTEG_CREATEFAILED;
            break;

        case FILESTATUS_INTEG_REINOCULATED:
            uLogAction = LOGEV_INTEG_REINOCULATED;

            NavNameReturnDrive ( lpNav->Infected.szFullPath, szTemp );
            lpIntegChanged = szTemp;
            break;

        case FILESTATUS_INTEG_REINOCULATEFAILED:
            uLogAction = LOGEV_INTEG_REINOCULATEFAILED;

            NavNameReturnDrive ( lpNav->Infected.szFullPath, szTemp );
            lpIntegChanged = szTemp;
            break;

        case FILESTATUS_INTEG_MBREPAIRED:
            uLogAction = LOGEV_INTEG_MBREPAIRED;
            break;

        case FILESTATUS_INTEG_MBREPAIRFAILED:
            uLogAction = LOGEV_INTEG_MBREPAIRFAILED;
            break;

        case FILESTATUS_INTEG_BOOTREPAIRED:
            uLogAction = LOGEV_INTEG_BOOTREPAIRED;
            break;

        case FILESTATUS_INTEG_BOOTREPAIRFAILED:
            uLogAction = LOGEV_INTEG_BOOTREPAIRFAILED;
            break;

        case FILESTATUS_INTEG_SYSFILEREPAIRED:
            uLogAction = LOGEV_INTEG_SYSFILEREPAIRED;
            lpIntegChanged = lpNav->Infected.szFullPath;
            break;

        case FILESTATUS_INTEG_SYSFILEREPAIRFAILED:
            uLogAction = LOGEV_INTEG_SYSFILEREPAIRFAILED;
            lpIntegChanged = lpNav->Infected.szFullPath;
            break;

        case FILESTATUS_INTEG_MBCHANGED:
            uLogAction = LOGEV_INTEG_MBCHANGED;
            break;

        case FILESTATUS_INTEG_BOOTCHANGED:
            uLogAction = LOGEV_INTEG_BOOTCHANGED;
            break;

        case FILESTATUS_INTEG_SYSFILECHANGED:
            uLogAction = LOGEV_INTEG_SYSFILECHANGED;
            lpIntegChanged = lpNav->Infected.szFullPath;
            break;

        case FILESTATUS_INTEG_NOTEXIST:
            uLogAction = LOGEV_INTEG_NOTEXIST;
            break;

        default:
            uLogAction = LOGEV_INTEG_SYSFILECHANGED;
            break;
        }

    if ( lpNav->Infected.wStatus != FILESTATUS_INTEG_CREATED )
        {
        if ( lpNav->Infected.wStatus == FILESTATUS_INTEG_REINOCULATED ||
             lpNav->Infected.wStatus == FILESTATUS_INTEG_REINOCULATEFAILED)
            {
            lpIntegChanged = szBootDriveLtr;
            }
        else
            {
            lpIntegChanged = lpNav->Infected.szFullPath;
            }

        Log.uEvent       = uLogAction;
        Log.lpFileName   = lpIntegChanged;
        Log.lpAct        = &lpNav->Act;

        if (LogEvent (&Log))
            NotifyLogWriteError (hDlg, lpNav);
        }
}

//***************************************************************************
// EnableSysIntegButton()
//
// Description:
//      Disable the requested button and check if we are out of buttons.
//      If so, enable the continue button.
//      NOTE:  Always set the Continue button last when you call this so
//             it doesn't accidentally get turned on while setting things.
//
//***************************************************************************
// 09/20/1993 BARRY Function Created.
//***************************************************************************

VOID LOCAL EnableSysIntegButtons ( HWND hDlg, LPNONAV lpNav )
{
    EnableWindow (GetDlgItem(hDlg, IDI_SYSINTEG_REPAIR),
                  lpNav->Inoc.bDispRepair);
    EnableWindow (GetDlgItem(hDlg, IDI_SYSINTEG_INOC),
                  lpNav->Inoc.bDispInoc);
    EnableWindow (GetDlgItem(hDlg, IDI_SYSINTEG_CONT),
                  lpNav->Inoc.bDispCont);
    EnableWindow (GetDlgItem(hDlg, IDI_SYSINTEG_STOP),
                  lpNav->Opt.bAllowScanStop);

    if (!lpNav->Inoc.bDispRepair &&
        !lpNav->Inoc.bDispInoc   &&
        !lpNav->Opt.bAllowScanStop)
        {
        EnableWindow(GetDlgItem(hDlg, IDI_SYSINTEG_CONT), TRUE);
        }
}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

VOID LOCAL NavNameReturnDrive (
    LPSTR   lpPath,
    LPSTR   lpDrive
    )
{
    SYM_ASSERT ( lpPath );
    SYM_ASSERT ( lpDrive );

    *lpDrive = EOS;

    if ( lpPath && *lpPath )
        {
        if ( lpPath[1] == ':' )
            STRNCPY ( lpDrive, lpPath, 2 );
        else
            {
            SYM_ASSERT ( 0 );           // Should never happen
            STRCPY ( lpDrive, "C:" );
            }
        }
}

#ifdef SYM_WIN32

/* **************************************************************************
@Name: VOID LOCAL OnContextMenu (
            HWND    hDlg,
            WPARAM  wParam,
            LPARAM  lParam
            )

@Description:
This function is called when the dialog box receives a WM_CONTEXTMENU windows
message. The purpose of this function is to call the standard symantec help
routines that display an appropiate floating help menu.

@Parameters:
$hDlg$  handle of the dialog box where the message was intercepted.

$wParam$    the wParam that was passed to WM_CONTEXTMENU message.

$lParam$    the lParam that was passed to WM_CONTEXTMENU message.

@Include: stdhelp.h
************************************************************************** */

VOID LOCAL OnContextMenu (              // Respond to WM_CONTEXTMENU message
    HWND    hDlg,                       // [in] dialog that sent the message.
    UINT    uMsg,
    WPARAM  wParam,                     // [in] wParam from dialog
    LPARAM  lParam )                    // [in] lParam from dialog
{
                                        // -----------------------------------
                                        // This array is a cross reference
                                        // between a control id number and a
                                        // help id number that should be
                                        // displayed when the user selects the
                                        // 'What's This' menu entry.
                                        // -----------------------------------

    static  DWORD       dwWhatsThisMap[] =
        {
        IDI_SYSINTEG_REPAIR,IDH_NAVW_WHATHIS_REPAIR_BTN,
        IDI_SYSINTEG_CONT,  IDH_NAVW_WHATHIS_CONTINUE_BTN,
        IDI_SYSINTEG_STOP,  IDH_NAVW_WHATHIS_STOP_BTN,
        IDI_SYSINTEG_INOC,  IDH_NAVW_WHATHIS_INOCULATE_BTN,
        0,                  0
        };

    NavDlgOnContextHelp(                /* Invoke Common Context Help Handling*/
                        hDlg,
                        uMsg,
                        wParam,
                        lParam,
                        dwWhatsThisMap,
                        NULL, 
                        NULL,           /* This dlg. does not have any LBoxs */
                        NULL            /* that's why these nulls            */
                       );

}
#endif


