// Copyright 1994 Symantec, Peter Norton Product Group
//************************************************************************
//
// $Header:   S:/NAVWORK/VCS/N_REPORT.CPv   1.6   17 Aug 1997 17:59:32   DBUCHES  $
//
// Description:
//
// Contains:
//
// See Also:
//************************************************************************
// $Log:   S:/NAVWORK/VCS/N_REPORT.CPv  $
// 
//    Rev 1.6   17 Aug 1997 17:59:32   DBUCHES
// Fixed yet another custom response bug.  Now works correctly for boot record virus reporting.
//
//    Rev 1.5   17 Jul 1997 16:10:16   DBUCHES
// Fixed problems with Custom response PROMPT.
//
//    Rev 1.4   09 Jul 1997 16:26:04   DBUCHES
// Fixed problems with custom response NOTIFY
//
//    Rev 1.3   03 Jul 1997 15:01:24   TIVANOV
// fix custom notify responce
//
//    Rev 1.2   20 May 1997 10:20:06   TIVANOV
// implement custom responce
//
//    Rev 1.1   25 Feb 1997 12:25:06   DALLEE
// Port from HYDRA.15:
//
//            Rev 1.15   09 Oct 1996 12:04:08   CCOTRIL
//         Re-enable network alerting for W32.
//
//    Rev 1.0   06 Feb 1997 21:02:36   RFULLER
// Initial revision
//
//    Rev 1.1   28 Jun 1996 16:53:02   GWESTER
// Ported in fixes from Luigi Plus.
//
//    Rev 1.0   30 Jan 1996 15:55:50   BARRY
// Initial revision.
//
//    Rev 1.0   30 Jan 1996 14:50:54   BARRY
// Initial revision.
//
//    Rev 1.12   14 Jan 1996 16:50:04   DALLEE
// Changed #ifdef NAVWNT to NAVSCANW.
//
//    Rev 1.11   04 Nov 1995 12:01:36   DALLEE
// For NT: Removed HALT, Fall through to REPORT instead.
//
//    Rev 1.10   02 Nov 1995 16:35:50   JWORDEN
// Convert MasterBootRec elements from booleans to counters
//
//    Rev 1.9   24 Oct 1995 19:30:52   JWORDEN
// Process file name for MBR's as a single byte drive number
//
//    Rev 1.8   24 Jun 1995 13:26:50   BARRY
// Use MessageBeep() instead of MUS4_BEEP (MUS4_BEEP conflicts with DOS)
//
//    Rev 1.7   09 May 1995 10:30:58   KEITH
// Correct some nouns
//
//************************************************************************

#include "platform.h"
#include "file.h"
#include "symcfg.h"
#include "navutil.h"
#include "options.h"
#include "nunavdlg.h"
#include "nunavstb.h"
#include "netobj.h"

#include "stub.h"
#include "nonav.h"
#include "virscan.h"

#include "netalert.h"
#include "navwork.h"

extern HINSTANCE hInstance;


VOID SYM_EXPORT WINAPI ReportVirusInMem (HWND hDlg, LPNONAV lpNav)
{
    auto    NETALERT    Alert;

    MEMSET (&Alert, '\0', sizeof(Alert));

    lpNav->hMommyWindow = hDlg;

    Alert.lpNet          = lpNav->lpNetUserItem;
    Alert.lpInfected     = &lpNav->Infected;
    Alert.lpAlertOpt     = &lpNav->Alert;
    Alert.uTypeOfEvent   = EV_ALERT_KNOWN_VIRUS;
    NetAlert (&Alert);

    // ******************************************************** //
    // Since a virus in memory is serious stuff, we care a huut //
    // about if PrestoNotify is on or off.                      //
    // ******************************************************** //

    lpNav->Infected.wStatus = FILESTATUS_MEMORYINFECTED;
    HaltCPU (hDlg, lpNav);
}


BOOL SYM_EXPORT WINAPI ReportVirusInBoot (HWND hDlg, BYTE byDrive, LPNONAV lpNav)
{
    auto    NETALERT       Alert;
    auto    NOTIFYSTRUCT   Notify;
    auto    BOOL           bStatus;
    auto    char           szNotifyName [SYM_MAX_PATH + 1];

    MEMSET (&Alert, '\0', sizeof(Alert));

    // ************************************************************ //
    // Note that for Master Boot Record Viruses the drive passed in //
    // byDrive is the physical drive number (80h, 81h, etc.) and    //
    // NOT a drive letter                                           //
    // ************************************************************ //
    lpNav->hMommyWindow = hDlg;
    if (lpNav->Infected.wStatus == FILESTATUS_MASTERBOOTINFECTED)
        lpNav->Infected.szFullPath[0] = (char)byDrive - 0x80;
    else
        lpNav->Infected.szFullPath[0] = byDrive;
    lpNav->Infected.szFullPath[1] = ':';

    Alert.lpNet          = lpNav->lpNetUserItem;
    Alert.lpInfected     = &lpNav->Infected;
    Alert.lpAlertOpt     = &lpNav->Alert;
    Alert.uTypeOfEvent   = EV_ALERT_KNOWN_VIRUS;
    NetAlert (&Alert);

    BYTE uAction;
    BOOL bCustomDecision = lpNav->lpScan->lpNavOptions->customScan.bSelected;
    if (bCustomDecision)
        uAction = lpNav->lpScan->lpNavOptions->customScan.uchBoot;
    else
        uAction = lpNav->Opt.uAction;

    if ( lpNav->Alert.bBeep && lpNav->Opt.bPrestoNotify )
        MessageBeep ( MB_ICONASTERISK );

    // ************************************************************ //
    // If immediate notification is turned ON, then take appropiate //
    // action, otherwise just log this information for later.       //
    // ************************************************************ //

    if (lpNav->Opt.bPrestoNotify)
        {
        switch (uAction)
            {
            case SCAN_PROMPT:
                bStatus = VirusFound (lpNav);
                break;

            case SCAN_REPAUTO:
                // *********************************************** //
                // Notify the user about the status of the repair. //
                // *********************************************** //

                bStatus = RepairBoot (lpNav, hDlg, TRUE);

                if (bStatus == TRUE)
                    {
                    if (lpNav->Infected.wStatus == FILESTATUS_BOOTINFECTED)
                        lpNav->Stats.BootRecs.uCleaned++;
                    else
                        lpNav->Stats.MasterBootRec.uCleaned++;
                    }

                lpNav->Infected.wStatus = UpdateFileStatus (
                            lpNav->Infected.wStatus, PROCESS_REPAIR, bStatus);

                AddVirusInfo (lpNav);

                bStatus = FALSE;    // Continue Scanning
                break;

            case SCAN_HALTCPU:
#ifndef NAVSCANW  // Fall through to SCAN_REPORT for NT.
                HaltCPU (hDlg, lpNav);
                break;
#endif

            case SCAN_REPORT:
            case SCAN_DELAUTO:
            default:
                // *************************************************** //
                // For any of these options just notify the user about //
                // the virus in the boot.                              //
                // *************************************************** //

                AddVirusInfo (lpNav);

                MEMSET (&Notify, '\0', sizeof(Notify));
                Notify.hParent       = hDlg;
                if (lpNav->Infected.wStatus == FILESTATUS_MASTERBOOTINFECTED)
                    {
                    Notify.uMsgID        = IDS_VIRUSINMASTERBOOT;
                    SPRINTF(szNotifyName, "%d", lpNav->Infected.szFullPath[0]);
                    }
                else
                    {
                    Notify.uMsgID        = IDS_VIRUSINBOOT;
                    STRCPY(szNotifyName, lpNav->Infected.szFullPath);
                    }
                Notify.lpFileName    = szNotifyName;

                if ( GetCustomVersion() == VERSION_NETSCAPE )
                    {                   // Title is "Internet Scanner"; set at runtime.
                    Notify.uTitleID      = IDS_NAV_INTERNET;
                    }
                else
                    {
                    Notify.uTitleID      = IDS_NAV;
                    }

                Notify.lpVirusName   = lpNav->Infected.Notes.lpVirName;
                Notify.lpAlertMsg    = (lpNav->Alert.bDispAlertMsg) ?
                                        lpNav->Alert.szAlertMsg : NULL;
                Notify.uRemoveAfter  = (lpNav->Alert.bRemoveAfter) ?
                                        lpNav->Alert.uSeconds : 0;
                Notify.hInstance     = hInstance;

                NotifyUser (&Notify);
                break;
            }
        }
    else
        {
        switch (uAction)
            {
            case SCAN_REPAUTO:
                bStatus = RepairBoot (lpNav, hDlg, FALSE);

                if (bStatus == TRUE)
                    {
                    if (lpNav->Infected.wStatus == FILESTATUS_BOOTINFECTED)
                        lpNav->Stats.BootRecs.uCleaned++;
                    else
                        lpNav->Stats.MasterBootRec.uCleaned++;
                     }

                lpNav->Infected.wStatus = UpdateFileStatus (
                            lpNav->Infected.wStatus, PROCESS_REPAIR, bStatus);

                AddVirusInfo (lpNav);

                bStatus = FALSE;    // Continue Scanning
                break;

            case SCAN_HALTCPU:
#ifndef NAVSCANW  // Fall through to REPORT for NT
                HaltCPU (hDlg, lpNav);
                break;
#endif

            case SCAN_REPORT:
                AddVirusInfo (lpNav);
                bStatus = FALSE;
                break;

            case SCAN_PROMPT:
                AddVirusInfo (lpNav);
                bStatus = FALSE;
                break;


            case SCAN_DELAUTO:
            default:
                // *************************************************** //
                // For any of these options just notify the user about //
                // the virus in the boot.                              //
                // *************************************************** //

                AddVirusInfo (lpNav);

                bStatus = FALSE;
                break;
            }
        }

    return (bStatus);
}
