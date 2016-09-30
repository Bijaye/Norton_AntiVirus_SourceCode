// Copyright 1995 Symantec Corporation
//***************************************************************************
//
// $Header:   S:/NAVWORK/VCS/n_scan2.cpv   1.24   31 Jul 1998 20:45:58   mdunn  $
//
// Description:
//
// Contains:
//
// See Also:
//
//***************************************************************************
// $Log:   S:/NAVWORK/VCS/n_scan2.cpv  $
// 
//    Rev 1.24   31 Jul 1998 20:45:58   mdunn
// Nuked some stray _asm int 3's.
//
//    Rev 1.23   31 Jul 1998 18:35:00   tcashin
// Fixed a !@#$%^&* late night mistake.
//
//    Rev 1.22   31 Jul 1998 02:06:42   tcashin
// More cleanup in DecideWhatToDoAfterProcessingZip().
//
//    Rev 1.21   28 Jul 1998 19:29:54   tcashin
// Cleaned up buggy code in DecideWhatToDoAfterProcessingZip(). We were going
// through the list of infected items all wrong and it get handle later
// anyway.
//
//    Rev 1.20   21 Jul 1998 14:01:06   tcashin
// Bug in DecideWhatToDoAfterProcessingZip(). Need to look at the items
// in the InfArr[] that were added by the decomposer.
//
//    Rev 1.19   16 Jul 1998 17:05:30   tcashin
// Shouldn't increment stats.files.uQuarantined in ...AfterProcessingZip(). It
// get done in the call to MarkCompressedFilesQuarantined().
//
//    Rev 1.18   13 Jul 1998 18:35:58   tcashin
// Removed extra calls to NetAlert().
//
//    Rev 1.17   01 Jul 1998 16:30:02   tcashin
// Fixed a problem where a failed to quarantine compressed file. The quarantine
// operation has to happen after we've returned from the decomposer.
//
//    Rev 1.16   01 Jun 1998 16:11:52   tcashin
// Added support for sending infected compressed files to quarantine.
//
//    Rev 1.15   18 May 1998 22:53:26   tcashin
// Set the notify parameter to FALSE if the scanner action is set to
// Quarantine automatically or Quarantine-if-cant-repair.
//
//    Rev 1.14   12 May 1998 14:18:02   tcashin
// Count cleaned, deleted, and quarantined files separately. Fixed
// some confusion with quarantine if can't repair and quarantine
// automatically.
//
//    Rev 1.13   27 Apr 1998 18:10:00   TCASHIN
// Added quarantine alerts.
//
//    Rev 1.12   20 Apr 1998 23:09:40   tcashin
// Make AutoQuarantine work when AutoRepair fails.
//
//    Rev 1.11   13 Apr 1998 10:48:14   tcashin
// Handle the new quarantine values in uAction.
//
//    Rev 1.10   24 Jul 1997 18:59:32   DBUCHES
// Still more custom response fixes.
//
//    Rev 1.9   17 Jul 1997 16:10:12   DBUCHES
// Fixed problems with Custom response PROMPT.
//
//    Rev 1.8   09 Jul 1997 16:26:04   DBUCHES
// Fixed problems with custom response NOTIFY
//
//    Rev 1.7   03 Jul 1997 14:59:46   TIVANOV
// fix custom notify respomce
//
//    Rev 1.6   24 Jun 1997 16:07:16   TIVANOV
//
//    Rev 1.5   29 May 1997 11:21:58   TIVANOV
// change the logic for finding if the file is doc or executable
//
//    Rev 1.4   22 May 1997 16:21:14   BILL
// No change.
//
//    Rev 1.3   22 May 1997 11:30:58   TIVANOV
// use of the LongCusomMessage
//
//    Rev 1.2   20 May 1997 10:19:16   TIVANOV
// custom responce inmplementation - MakeCustomDecision
//
//    Rev 1.1   12 May 1997 14:47:44   TIVANOV
// new case statment SCAN_MOVE to handle the Quarantine functionality
//
//    Rev 1.0   06 Feb 1997 21:02:56   RFULLER
// Initial revision
//
//    Rev 1.6   07 Aug 1996 15:47:56   JBRENNA
// LuigiPlus Port:
//   MKEATIN: A high ascii filename in a zip file will be displayed
//     correctly int the notify box also.
//
//   MKEATIN: The immediate Notify message now displays high ascii zip
//     file path names correctly.
//
//
//    Rev 1.5   28 Jun 1996 16:21:44   GWESTER
// Ported in fixes from Luigi Plus.
//
//    Rev 1.4   24 Jun 1996 21:33:10   RHAYES
// Add.'l OEM<->Ansi mod.'s (new and from Luigi).
//
//    Rev 1.3   22 May 1996 17:46:00   RHAYES
// Round # 2 of Dave Allee's OEM<->Ansi mod.'s in Luigi.
//
//    Rev 1.2   17 May 1996 14:53:52   RHAYES
// Copy szFullOemPath into repair struct. before calling
// RepairFile. This fixes STS # 53512.
//
//    Rev 1.1   09 May 1996 17:57:50   GWESTER
// Ported in Repair Wizard from Luigi Plus.
//
//    Rev 1.0   30 Jan 1996 15:55:58   BARRY
// Initial revision.
//
//    Rev 1.0   30 Jan 1996 14:51:00   BARRY
// Initial revision.
//
//    Rev 1.14   14 Jan 1996 16:50:06   DALLEE
// Changed #ifdef NAVWNT to NAVSCANW.
//
//    Rev 1.13   04 Nov 1995 12:02:02   DALLEE
// For NT: Removed HALT, Fall through to REPORT instead.
//
//    Rev 1.12   06 Jul 1995 11:30:24   KEITH
// Remove some annoying debug code.
//
//    Rev 1.11   03 Jul 1995 11:58:36   KEITH
// Shorten the .ZIP name and the zip child names so that they
// fit in the existing buffer (part of a global data structure).
// This fix is not valid if we ever try and fix/delete/repair
// files contained in a .ZIP file.
//
//    Rev 1.10   13 Jun 1995 15:54:00   KEITH
// Handle how NAVW continues after a system shutdown is canceled. This would
// be after a memory infection.
// Then...make the shutdown unconditional again anyhow!?
//***************************************************************************

#include "platform.h"
#include "xapi.h"
#include "cmbstr.h"
#include "symcfg.h"
#include "navwork.h"
#include "navutil.h"
#include "options.h"
#include "nunavdlg.h"
#include "nunavstb.h"

#ifdef SYM_WIN16
    #include "syminteg.h"
#endif

#include "stub.h"
#include "virscan.h"
#include "nonav.h"

extern   HINSTANCE   hInstance;

VOID LOCAL DoFileError (LPNONAV lpNav);

// ************************************************************************ //
BOOL IsCutomNotify(LPNONAV lpNav)
{
    BOOL rep = lpNav->lpScan->lpNavOptions->customScan.bSelected;
    if (rep)
    {
        if (lpNav->Infected.Notes.wInfo & 0x00f0)
            rep &= lpNav->lpScan->lpNavOptions->customScan.uchBoot == SCAN_REPORT;
        else
        {
            if (lpNav->Infected.Notes.wControl2 & bitINFO_MACRO)
                rep &= (lpNav->lpScan->lpNavOptions->customScan.uchMacro == SCAN_REPORT);
            else
                rep &= (lpNav->lpScan->lpNavOptions->customScan.uchFile  == SCAN_REPORT);
        }
    }

    return (rep);
}

BOOL MakeCustomDecision(LPNONAV lpNav, BYTE& bField)
{
    BOOL bFailed = lpNav->lpScan->lpNavOptions->customScan.bSelected;

    //check if the custom responce is selected
    if (bFailed)
    {
        //check if this file is a doc or executable file
        if (lpNav->Infected.Notes.wControl2 & bitINFO_MACRO)
            bField = lpNav->lpScan->lpNavOptions->customScan.uchMacro;
        else
            bField = lpNav->lpScan->lpNavOptions->customScan.uchFile;
    }
    return (bFailed);
}

VOID SYM_EXPORT WINAPI DecideWhatToDo (LPNONAV lpNav)
{
    auto    REPAIRFILESTRUCT    Repair;
    auto    NOTIFYSTRUCT        Notify;
    auto    BOOL                bStatus;
    auto    BOOL                bQuarNotify = TRUE;

    MEMSET (&Notify, '\0', sizeof(Notify));
    lpNav->Infected.wStatus = FILESTATUS_INFECTED;

    if (lpNav->Infected.Notes.wVirID == (WORD)-1)
        {                                // --------------------------
        DoFileError (lpNav);             // Internal DOS Error occured
        }                                // --------------------------
    else if (lpNav->Infected.Notes.wVirID != 0)
        {
        lpNav->Stats.Files.uInfected++;

        BYTE uAction;
        BOOL bCustomDecision = MakeCustomDecision(lpNav, uAction);
        if (bCustomDecision == FALSE)
            uAction = lpNav->Opt.uAction;

        if (lpNav->Opt.bPrestoNotify == TRUE)
            {
            switch (uAction)
                {
                case SCAN_PROMPT:
                    ScanTime(lpNav->hMommyWindow, FALSE);
                    VirusFound (lpNav);
                    ScanTime(lpNav->hMommyWindow, TRUE);
                    break;

                case SCAN_REPAUTO:
                case SCAN_QUAR_CANT_REP:
                    ScanTime(lpNav->hMommyWindow, FALSE);
                                        // -----------------------------------
                                        // DALLEE: Removed special case fail
                                        // for DIR2 since neither DX nor W32
                                        // will repair this, so it's just
                                        // another virus w/ no repair def.
                                        // We don't want to have to in-line
                                        // if a NAVEX repair is ever done.
                                        // -----------------------------------

                    Repair.bUseInoc         = lpNav->Inoc.bUseInoc &&
                                !(NO_GENERIC & lpNav->Infected.Notes.wControl2);

                    Repair.bCreateBackup    = lpNav->Gen.bBackupRep;
                    Repair.uVirusSigIndex   = lpNav->Infected.uVirusSigIndex;
                    STRCPY (Repair.szFullPath, lpNav->Infected.szFullPath);
                    STRCPY (Repair.szFullOemPath, lpNav->Infected.szFullOemPath);
                    STRCPY (Repair.szBackExt, lpNav->Gen.szBackExt);

                    bStatus = RepairFile (&Repair);

                    if (bStatus == TRUE)
                        lpNav->Stats.Files.uCleaned++;

                    NotifyRepairCompletion (lpNav->hMommyWindow, bStatus,
                                            lpNav);

                    ScanTime(lpNav->hMommyWindow, TRUE);

                    // If repair is successful or if we're here for REPAUTO
                    // return, otherwise fall through to quarantine for
                    // the SCAN_QUAR_CANT_REP case.
                    if (bStatus || uAction == SCAN_REPAUTO)
                    {
                        lpNav->Infected.wStatus = UpdateFileStatus (
                                lpNav->Infected.wStatus, PROCESS_REPAIR, bStatus);

                        AddVirusInfo (lpNav);
                        break;
                    }

                case SCAN_MOVE:
                case SCAN_QUARAUTO:
                case SCAN_QUARANTINE:

                    // ************************************************ //
                    // Note that we stop the timer since we will notify //
                    // the user about the completion status of the file //
                    // delete, this is because we don't know the time   //
                    // the user will spend gaizing into the dialog box. //
                    // ************************************************ //

                    ScanTime(lpNav->hMommyWindow, FALSE);

                    if (uAction == SCAN_QUARAUTO || uAction == SCAN_QUAR_CANT_REP)
                        bQuarNotify = FALSE;

                    bStatus = NavMoveFile (lpNav, lpNav->hMommyWindow, bQuarNotify);

                    if (bStatus == TRUE)
                        lpNav->Stats.Files.uQuarantined++;

                    lpNav->Infected.wStatus = UpdateFileStatus (
                            lpNav->Infected.wStatus, PROCESS_QUARANTINE, bStatus);

                    AddVirusInfo (lpNav);

                    ScanTime(lpNav->hMommyWindow, TRUE);
                    break;

                case SCAN_DELAUTO:
                    // ************************************************ //
                    // Note that we stop the timer since we will notify //
                    // the user about the completion status of the file //
                    // delete, this is because we don't know the time   //
                    // the user will spend gaizing into the dialog box. //
                    // ************************************************ //

                    ScanTime(lpNav->hMommyWindow, FALSE);

                                        // -----------------------------------
                                        // Special case for files infected
                                        // with the DIR2 virus is handled
                                        // within NavDeleteFile()
                                        // -----------------------------------
                    bStatus = NavDeleteFile (lpNav, lpNav->hMommyWindow, TRUE);

                    lpNav->Infected.wStatus = UpdateFileStatus (
                            lpNav->Infected.wStatus, PROCESS_DELETE, bStatus);

                    AddVirusInfo (lpNav);

                    ScanTime(lpNav->hMommyWindow, TRUE);
                    break;

                case SCAN_HALTCPU:
#ifndef NAVSCANW      // Fall through to REPORT for NT
                    LongCustomMessage(NULL);
                    ScanTime(lpNav->hMommyWindow, FALSE);
                    HaltCPU (lpNav->hMommyWindow, lpNav);
                    break;
#endif

                case SCAN_REPORT:
                    AddVirusInfo (lpNav);

                    // ************************************************* //
                    // We notify the user that we found an infected file //
                    // ************************************************* //

                    ScanTime(lpNav->hMommyWindow, FALSE);

                    Notify.hParent       = lpNav->hMommyWindow;
                    Notify.uMsgID        = IDS_VIRUSINFILE;
                    Notify.uTitleID      = IDS_NAV;
                    if ( GetCustomVersion() == VERSION_NETSCAPE )
                        {               // Title is "Internet Scanner"; set at runtime.
                    Notify.uTitleID      = IDS_NAV_INTERNET;
                        }
                    Notify.lpFileName    = lpNav->Infected.szFullPath;
                    Notify.lpVirusName   = lpNav->Infected.Notes.lpVirName;
                    Notify.lpAlertMsg    = (lpNav->Alert.bDispAlertMsg) ?
                                            lpNav->Alert.szAlertMsg : NULL;
                    Notify.uRemoveAfter  = (lpNav->Alert.bRemoveAfter) ?
                                            lpNav->Alert.uSeconds : 0;
                    Notify.hInstance     = hInstance;

                    LongCustomMessage(NULL);
                    NotifyUser (&Notify);

                    ScanTime(lpNav->hMommyWindow, TRUE);
                    break;

                default:
                    break;
                }
            }
        else
            {
            switch (uAction)
                {
                case SCAN_REPAUTO:
                case SCAN_QUAR_CANT_REP:
                                        // -----------------------------------
                                        // DALLEE: Removed special case fail
                                        // for DIR2 since neither DX nor W32
                                        // will repair this, so it's just
                                        // another virus w/ no repair def.
                                        // We don't want to have to in-line
                                        // if a NAVEX repair is ever done.
                                        // -----------------------------------
                    Repair.bUseInoc         = lpNav->Inoc.bUseInoc &&
                                !(NO_GENERIC & lpNav->Infected.Notes.wControl2);

                    Repair.bCreateBackup    = lpNav->Gen.bBackupRep;
                    Repair.uVirusSigIndex   = lpNav->Infected.uVirusSigIndex;
                    STRCPY (Repair.szFullPath, lpNav->Infected.szFullPath);
                    STRCPY (Repair.szFullOemPath, lpNav->Infected.szFullOemPath);
                    STRCPY (Repair.szBackExt, lpNav->Gen.szBackExt);

                    bStatus = RepairFile (&Repair);

                    if (bStatus == TRUE)
                        lpNav->Stats.Files.uCleaned++;

                    // If repair is successful or if we're here for REPAUTO
                    // return, otherwise fall through to quarantine for
                    // the SCAN_QUAR_CANT_REP case.
                    if ((bStatus == TRUE) || (uAction == SCAN_REPAUTO))
                    {
                        lpNav->Infected.wStatus = UpdateFileStatus (
                                lpNav->Infected.wStatus, PROCESS_REPAIR, bStatus);

                        AddVirusInfo (lpNav);
                        break;
                    }

                case SCAN_MOVE:
                case SCAN_QUARANTINE:
                case SCAN_QUARAUTO:

                    if (uAction == SCAN_QUARAUTO || uAction == SCAN_QUAR_CANT_REP)
                        bQuarNotify = FALSE;

                    bStatus = NavMoveFile (lpNav, lpNav->hMommyWindow, bQuarNotify);

                    if (bStatus == TRUE)
                        lpNav->Stats.Files.uQuarantined++;

                    lpNav->Infected.wStatus = UpdateFileStatus (
                            lpNav->Infected.wStatus, PROCESS_QUARANTINE, bStatus);

                    AddVirusInfo (lpNav);

                    break;

                case SCAN_DELAUTO:
                                        // -----------------------------------
                                        // Special case for files infected
                                        // with the DIR2 virus is handled
                                        // within NavDeleteFile()
                                        // -----------------------------------
                    bStatus = NavDeleteFile (lpNav, lpNav->hMommyWindow, FALSE);

                    if (bStatus == TRUE)
                        lpNav->Stats.Files.uDeleted++;

                    lpNav->Infected.wStatus = UpdateFileStatus (
                            lpNav->Infected.wStatus, PROCESS_DELETE, bStatus);

                    AddVirusInfo (lpNav);
                    break;

                case SCAN_HALTCPU:
#ifndef NAVSCANW      // Fall through to REPORT for NT.
                    LongCustomMessage(NULL);
                    ScanTime(lpNav->hMommyWindow, FALSE);
                    HaltCPU (lpNav->hMommyWindow, lpNav);
                    break;
#endif
                case SCAN_REPORT:
                    AddVirusInfo (lpNav);
                    break;

                case SCAN_PROMPT:
                    AddVirusInfo (lpNav);
                    break;

                default:
                    AddVirusInfo (lpNav);
                 }
            }
        }
}

// -------------------------------------------------------------------------- //
// -------------------------------------------------------------------------- //
VOID SYM_EXPORT WINAPI DecideWhatToDoAfterProcessingZip (LPNONAV lpNav, LPINFARR lpInfArr)
{
    auto    BOOL            bStatus = FALSE;

    if (lpNav->Infected.Notes.wVirID == (WORD)-1)
    {                                // --------------------------
        DoFileError (lpNav);             // Internal DOS Error occured
    }                                // --------------------------
    else
    {
        auto    LPINFARR        lpBlockList;
        auto    LPINFECTEDFILE  lpInfected;
        auto    LPINFECTEDFILE  lpInfectedZipItem = NULL;
        auto    UINT            i;
        auto    BOOL            bFoundInfData = FALSE;
        auto    BOOL            bFoundBlock = FALSE;

        // Position to the first INFECTEDFILE item where the infected item contains
        // the name of the current compressed file. Get to it by first finding the
        // right INFARR block, then look at the items with that block.

        for ( lpBlockList = lpNav->lpInfArr;
              lpBlockList != NULL && !bFoundInfData;
              lpBlockList = ( LPINFARR ) lpBlockList->lpNext )
        {
            if ( lpInfArr && !bFoundBlock )
            {
                if ( lpInfArr != lpBlockList )
                    continue;
                else
                    bFoundBlock = TRUE;
            }

            lpInfected = ( LPINFECTEDFILE ) lpBlockList->hInf;

            if ( lpInfected )
            {
                // Look through the items in this block.
                for ( i = 0;
                      i < lpBlockList->uNumUsed && !bFoundInfData;
                      i++, lpInfected++ )
                {
                    if ( strstr(lpInfected->szFullPath,lpNav->Zip.lpZipFileName) ||
                         strstr(lpInfected->szFullOemPath,lpNav->Zip.lpZipFileName) )
                    {
                        if ( lpInfected->wStatus = FILESTATUS_ZIPINFECTED )
                        {

                            // Found one.

                            bFoundInfData = TRUE;
                            lpInfectedZipItem = lpInfected;
                        }
                    }
                }
            }
        }

        // If we found an infected item that is contained within the current
        // compressed file, then process it.

        if (  lpInfectedZipItem )
        {
            switch (lpNav->Opt.uAction)
            {

                case SCAN_PROMPT:
                case SCAN_REPORT:
                case SCAN_MOVE:
                case SCAN_QUARANTINE:
                case SCAN_DELAUTO:
                case SCAN_REPAUTO:

                    break;

                case SCAN_QUARAUTO:
                case SCAN_QUAR_CANT_REP:

                    if ( lpInfectedZipItem->wStatus = FILESTATUS_ZIPINFECTED )
                    {
                        bStatus = NavMoveFileInf (lpNav, lpInfectedZipItem, lpNav->hMommyWindow, FALSE);

                        if (bStatus == TRUE)
                        {
                            MarkCompressedFilesQuarantined(lpNav, lpInfectedZipItem, lpInfArr, NULL, NULL);
                        }
                    }

                    break;

                default:

                    break;
            }
        }
    }
}


// -------------------------------------------------------------------------- //
// -------------------------------------------------------------------------- //
VOID SYM_EXPORT WINAPI DecideWhatToDoWithZip (LPNONAV lpNav)
{
    auto    NOTIFYSTRUCT    Notify = {0};
    auto    char            szFileName [ SYM_MAX_PATH+1 ] = {0};
    auto    BOOL            bQuarNotify = TRUE;
    auto    BOOL            bStatus;

                                        // ----------------------------
                                        // Construct display version of
                                        // zip filename.
                                        // And fill in lpNav->Infected
                                        // ----------------------------
    STRCPY (lpNav->Infected.szFullOemPath, lpNav->szTheDirectory);
    NameReturnFile (lpNav->Zip.lpZipFileName, szFileName);
    NameAppendFile (lpNav->Infected.szFullOemPath, szFileName);

    ZipNameAppendChildToParent ( lpNav->Infected.szFullOemPath,
                                 lpNav->Zip.szZipChildFile,
                                 sizeof ( lpNav->Infected.szFullOemPath ) );

    NavOemToAnsi(lpNav->Infected.szFullOemPath, lpNav->Infected.szFullPath);

    lpNav->Infected.wStatus = FILESTATUS_ZIPINFECTED;

    if (lpNav->Infected.Notes.wVirID == (WORD)-1)
        {                                // --------------------------
        DoFileError (lpNav);             // Internal DOS Error occured
        }                                // --------------------------
    else if (lpNav->Infected.Notes.wVirID != 0)
        {
        lpNav->Stats.Files.uInfected++;

        if (lpNav->Opt.bPrestoNotify == TRUE)
            {
            switch (lpNav->Opt.uAction)
                {
                case SCAN_PROMPT:
                    ScanTime(lpNav->hMommyWindow, FALSE);
                    VirusFound (lpNav);
                    ScanTime(lpNav->hMommyWindow, TRUE);
                    break;

                case SCAN_HALTCPU:
#ifndef NAVSCANW      // Fall through to REPORT for NT.
                    LongCustomMessage(NULL);
                    ScanTime(lpNav->hMommyWindow, FALSE);
                    HaltCPU (lpNav->hMommyWindow, lpNav);
                    break;
#endif

                case SCAN_DELAUTO:
                case SCAN_REPAUTO:
                case SCAN_REPORT:
                    // ************************************************* //
                    // We notify the user that we found an infected file //
                    // ************************************************* //

                    ScanTime(lpNav->hMommyWindow, FALSE);

                    Notify.hParent       = lpNav->hMommyWindow;
                    Notify.uMsgID        = IDS_VIRUSINZIPFILE;
                    Notify.uTitleID      = IDS_NAV;
                    if ( GetCustomVersion() == VERSION_NETSCAPE )
                        {               // Title is "Internet Scanner"; set at runtime.
                        Notify.uTitleID      = IDS_NAV_INTERNET;
                        }

                    // Notify.lpZipFileName = lpNav->Zip.lpZipFileName;
                    char szTemp[ SYM_MAX_PATH + 1 ];
                    ParseZipFileName (lpNav->Infected.szFullPath, szTemp, szFileName);
                    Notify.lpZipFileName = szTemp;
                    Notify.lpFileName    = szFileName;

                    Notify.lpVirusName   = lpNav->Infected.Notes.lpVirName;
                    Notify.lpAlertMsg    = (lpNav->Alert.bDispAlertMsg) ?
                                            lpNav->Alert.szAlertMsg : NULL;
                    Notify.uRemoveAfter  = (lpNav->Alert.bRemoveAfter) ?
                                            lpNav->Alert.uSeconds : 0;
                    Notify.hInstance     = hInstance;

                    LongCustomMessage(NULL);
                    NotifyUser (&Notify);
                                        // ----------------------------------
                                        // Add the name of the zip file, and
                                        // the name of the compressed file to
                                        // our list of infected files.
                                        // ----------------------------------
                    lpNav->Infected.wStatus = FILESTATUS_ZIPINFECTED;
                    AddVirusInfo (lpNav);

                    ScanTime(lpNav->hMommyWindow, TRUE);

                    break;

                case SCAN_MOVE:
                case SCAN_QUARANTINE:
                case SCAN_QUARAUTO:
                case SCAN_QUAR_CANT_REP:

                    if (lpNav->Opt.uAction == SCAN_QUARAUTO || lpNav->Opt.uAction == SCAN_QUAR_CANT_REP)
                        bQuarNotify = FALSE;

                    bStatus = NavMoveFile (lpNav, lpNav->hMommyWindow, bQuarNotify);

                    if (bStatus == TRUE)
                        lpNav->Stats.Files.uQuarantined++;

                    lpNav->Infected.wStatus = UpdateFileStatus (
                            lpNav->Infected.wStatus, PROCESS_QUARANTINE, bStatus);

                    AddVirusInfo (lpNav);

                    break;

                default:
                    break;
                }
            }
        else
            {
            switch (lpNav->Opt.uAction)
                {
                case SCAN_HALTCPU:
#ifndef NAVSCANW      // Fall through to REPORT for NT
                    LongCustomMessage(NULL);
                    ScanTime(lpNav->hMommyWindow, FALSE);
                    HaltCPU (lpNav->hMommyWindow, lpNav);
                    break;
#endif

                default:
                                        // ----------------------------------
                                        // Add the name of the zip file, and
                                        // the name of the compressed file to
                                        // our list of infected files.
                                        // ----------------------------------
                    AddVirusInfo (lpNav);
                    break;
                }
            }
        }
}


//***************************************************************************
// ZipNameAppendChildToParent (
//    LPSTR lpParent,
//    LPSTR lpChild,
//    int   ncDest )                    // Max chars for destination.
//
// Description: Append a zipfile child name to a zipfile name using
//              the ZIP_SEPARATOR (currently '|').
//
//              Shorten such that that they will fit within a
//              SYM_MAX_PATH string. This is temporary. The target buffer
//              should be made handle all the names needed.
//
//              Child size is more significant than that of the parent.
//              Shorten the parent so that all of the child fits, but
//              don't force the parent be less than ncMinShortParent.
//              Then, shorten the child to fit into what remains.
//
// Parameters:
//
// Return Value:
//
// See Also:
//
//***************************************************************************
// 07/02/1995 KEITH Function Created.
//***************************************************************************

void ZipNameAppendChildToParent (
    LPSTR lpParent,
    LPSTR lpChild,
    int   ncDest )
{
    auto    char            szShort [ SYM_MAX_PATH+1 ] = {0};

                                        // ----------------------------------
                                        // Determine the smallest parent we
                                        // will enforce, the lesser of
                                        // 40 or half of the destination.
                                        // ----------------------------------
    auto    int             ncMinShortParent = min((ncDest-1)/2,40);

                                        // ----------------------------------
                                        // Shorten the name leaving
                                        // room for the null and the
                                        // ZIP_SEPARATOR.
                                        // ----------------------------------
    NameShortenFileName ( lpParent,
                          szShort,
                          max( ncMinShortParent, (ncDest-STRLEN(lpChild))-2 ) );
    STRCPY ( lpParent, szShort );
                                        // ----------------------------------
                                        // Shorten the child to the largest
                                        // we can fit now that the parent
                                        // was crunched to make room.
                                        // ----------------------------------
    NameShortenFileName ( lpChild,
                          szShort,
                          (ncDest-STRLEN(lpParent))-2 );

                                        // ----------------------------------
                                        // Add the separator and the child
                                        // ----------------------------------
    int ncLen = STRLEN(lpParent);

    lpParent[ ncLen ]   = ZIP_SEPARATOR;
    lpParent[ ncLen+1 ] = EOS;

    STRCAT ( lpParent,
             szShort );
}


VOID LOCAL DoFileError (LPNONAV lpNav)
{
    auto    NOTIFYSTRUCT   Notify   ;
    auto    BOOL            bStatus;

    MEMSET (&Notify, '\0', sizeof(Notify));

    STRCPY(lpNav->Infected.Notes.lpVirName, String_Ptr(IDS_DOSERROR));
    lpNav->Infected.wStatus = FILESTATUS_ERROR;

    if (lpNav->Opt.bPrestoNotify == TRUE)
        {
        switch (lpNav->Opt.uAction)
            {
            case SCAN_PROMPT:
                ScanTime(lpNav->hMommyWindow, FALSE);
                VirusFound (lpNav);
                ScanTime(lpNav->hMommyWindow, TRUE);
                break;

            case SCAN_DELAUTO:
                ScanTime(lpNav->hMommyWindow, FALSE);

                bStatus = NavDeleteFile (lpNav, lpNav->hMommyWindow, TRUE);

                lpNav->Infected.wStatus = UpdateFileStatus (
                            lpNav->Infected.wStatus, PROCESS_DELETE, bStatus);

                AddVirusInfo (lpNav);

                ScanTime(lpNav->hMommyWindow, TRUE);
                break;

            case SCAN_HALTCPU:
#ifndef NAVSCANW  // Fall through to REPORT for NT.
                ScanTime(lpNav->hMommyWindow, FALSE);
                HaltCPU (lpNav->hMommyWindow, lpNav);
                break;
#endif

            case SCAN_REPAUTO:
            case SCAN_REPORT:
                ScanTime(lpNav->hMommyWindow, FALSE);

                AddVirusInfo (lpNav);

                Notify.hParent       = lpNav->hMommyWindow;
                Notify.uMsgID        = IDS_DOS_ERROR;
                Notify.uTitleID      = IDS_NAV;
                if ( GetCustomVersion() == VERSION_NETSCAPE )
                    {               // Title is "Internet Scanner"; set at runtime.
                Notify.uTitleID      = IDS_NAV_INTERNET;
                    }
                Notify.lpFileName    = lpNav->Infected.szFullPath;
                Notify.lpVirusName   = NULL;
                Notify.lpAlertMsg    = (lpNav->Alert.bDispAlertMsg) ?
                                        lpNav->Alert.szAlertMsg : NULL;
                Notify.uRemoveAfter  = (lpNav->Alert.bRemoveAfter) ?
                                        lpNav->Alert.uSeconds : 0;
                Notify.hInstance     = hInstance;

                NotifyUser (&Notify);

                ScanTime(lpNav->hMommyWindow, TRUE);
                break;

            default:
                break;
            }
        }
    else
        {
        switch (lpNav->Opt.uAction)
            {
            case SCAN_DELAUTO:
                bStatus = NavDeleteFile (lpNav, lpNav->hMommyWindow, FALSE);

                lpNav->Infected.wStatus = UpdateFileStatus (
                            lpNav->Infected.wStatus, PROCESS_DELETE, bStatus);

                AddVirusInfo (lpNav);
                break;

            case SCAN_HALTCPU:
#ifndef NAVSCANW  // Fall through to REPORT for NT.
                ScanTime(lpNav->hMommyWindow, FALSE);
                HaltCPU (lpNav->hMommyWindow, lpNav);
                break;
#endif

            case SCAN_REPORT:
            default:
                AddVirusInfo (lpNav);
                break;
            }
        }
}

