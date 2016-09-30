// Copyright 1993 Symantec, Peter Norton Product Group
// ************************************************************************
//
// $Header:   S:/NAVWORK/VCS/n_dowork.cpv   1.26   23 Jul 1998 13:17:28   jtaylor  $
//
// Description:
//
// Contains:
//      RepairBoot()
//      DeleteFile()
//      InocThisFile()
//      InocDeleteFile()
//      ExcludeAddFile()
//      NotifyRepairCompletion()
//
// See Also:
// ************************************************************************
// $Log:   S:/NAVWORK/VCS/n_dowork.cpv  $
// 
//    Rev 1.26   23 Jul 1998 13:17:28   jtaylor
// Updated the Quarantine process to change the file attributes on the source file to not be Read Only.(really fixed this time....  ;)
//
//    Rev 1.25   23 Jul 1998 12:24:14   jtaylor
// Updated the Quarantine process to change the file attributes on the source file to not be Read Only.
//
//    Rev 1.24   21 Jul 1998 13:52:16   tcashin
// Added NoveMoveFileInf().
//
//    Rev 1.23   09 Jul 1998 12:07:28   DBuches
// Removed DebugBreak() call.
//
//    Rev 1.22   09 Jun 1998 13:12:58   tcashin
// fixed a bug I introduced in the last change in NavMoveFile().
//
//    Rev 1.21   03 Jun 1998 18:28:30   tcashin
// Change to quarantine: if a file has an undeletable virus or cannot be
// deleted (read only, etc.) the take it out of quarantine and return an
// error.
//
//    Rev 1.20   03 Jun 1998 13:01:56   DBuches
// Fixed problem with building NAVWNT target.
//
//    Rev 1.19   02 Jun 1998 18:38:10   DBuches
// Now, we attempt to preserve the FAT32 root SCN when restoring boot records.
//
//    Rev 1.18   01 Jun 1998 16:05:06   tcashin
// Added support to quarantine compressed files.
//
//    Rev 1.17   27 May 1998 18:58:04   TCASHIN
// Allow non-deletable files to be quarantined. #116485.
//
//    Rev 1.16   18 May 1998 22:38:46   tcashin
// Fixed ANSI/OEM problems in NavMoveFile() and NavDeleteFile().
//
//    Rev 1.15   13 May 1998 09:57:52   tcashin
// Test for VirusScanCanDeleteFile() was bass-ackwards causing Quarantine
// to fail. !@#$%^&*
//
//    Rev 1.14   12 May 1998 14:10:50   tcashin
// A couple of fixes: count the deleted files as deleted, not cleaned.
// In NavMoveFile and NavDeleteFile call the new VirusScanCanDeleteFile()
// instead of testing the Dir2 bit. It didn't agree with AVAPI.
//
//    Rev 1.13   19 Apr 1998 00:39:18   JTaylor
// Added another argument to AddFileToQuarantine call.
//
//    Rev 1.12   13 Apr 1998 22:42:38   tcashin
// Fixed a typo in a #ifdef.
//
//    Rev 1.11   13 Apr 1998 10:54:50   tcashin
// Use new QFILE* values in quar32.h
//
//    Rev 1.10   03 Apr 1998 11:58:02   tcashin
// Added some of the new quarantine code.
//
//    Rev 1.9   18 Mar 1998 15:48:50   DALLEE
// Some pruning: Removed calls to old UpdateQuarantineFile() left from
// first aborted implementation of quarantine-- this created a text file
// in NAV's dir with a single line entry for each quarantine event.
//
//    Rev 1.8   18 Feb 1998 16:22:10   mdunn
// Ported NEC code from the ROSNEC area.
//
//    Rev 1.7   31 May 1997 18:38:28   MKEATIN
// Passed NULL as the lpvCookie Parameter in VirusScanRepairBoot
//
//    Rev 1.6   22 May 1997 11:30:56   TIVANOV
// use of the LongCusomMessage
//
//    Rev 1.5   19 May 1997 15:43:48   TIVANOV
// move the quarantine log function to navxutil proj
//
//    Rev 1.4   13 May 1997 13:53:16   TIVANOV
// add quarantine functionality when inoculate files
//
//    Rev 1.3   09 May 1997 13:29:12   TIVANOV
// new function UpdateQuarantineFile - creates and saves information about the
// quarantine files in a text file.
//
//    Rev 1.2   05 May 1997 15:18:08   TIVANOV
// change NavMoveFile to pass an extra param to VirusScanMoveFile
//
//    Rev 1.1   02 May 1997 11:45:54   TIVANOV
// new function - NavMoveFile and some new helpting functions to support NavMove
//
//    Rev 1.0   06 Feb 1997 21:02:36   RFULLER
// Initial revision
//
//    Rev 1.9   23 Jan 1997 13:32:10   JBRENNA
// Use DiskGetBootDrive() instead of DiskGetFirstFixedDisk() when looking for
// the boot drive. On NEC machines, DiskGetFirstFixedDisk() will possibly
// return the wrong drive letter for the boot drive.
//
//    Rev 1.8   18 Dec 1996 15:15:54   JBRENNA
// No longer hard code 'C' drive as the first hard disk in RepairBoot().
//
//    Rev 1.7   11 Dec 1996 09:39:38   JBRENNA
// For some reason NavWork.Dll had its own version of GetSysIntegFile(). This
// version added the NavAnsiToOem() call. In order to remove the duplicate
// code, I added the "Oem" on the end of this file's GetSysIntegFile name. Now
// we can directly call the real GetSysIntegFile() in NavXUtil.
//
//    Rev 1.6   28 Jun 1996 15:46:26   GWESTER
// Ported in fixes from Luigi Plus.
//
//    Rev 1.5   24 Jun 1996 21:22:40   RHAYES
// Add.'l OEM<->Ansi mod.'s (new and from Luigi).
//
//    Rev 1.4   14 Jun 1996 13:46:30   RHAYES
// Support to correctly repair a SUBST'd drive
// by passing the target drive derived using the
// new function GetPhysicalDriveLetter() located
// in n_memscn.cpp.
//
//    Rev 1.3   21 May 1996 15:04:30   RHAYES
// Merged Luigi OEM<->Ansi code for more robust support of
// Non-Ansi directory and filenames.
//
//    Rev 1.2   09 May 1996 18:00:34   GWESTER
// Ported in Repair Wizard from Luigi Plus.
//
//    Rev 1.1   14 Feb 1996 16:58:56   JWORDEN
// Process internal file names in OEM mode to allow scan, and repair, and
// innoculation of file names containing certain non-ANSI translatable
// characters.
//
//    Rev 1.0   30 Jan 1996 15:55:54   BARRY
// Initial revision.
//
//    Rev 1.0   30 Jan 1996 14:50:56   BARRY
// Initial revision.
//
//    Rev 1.26   24 Jan 1996 14:59:22   GWESTER
// Changed wording on confirmation dialogs that appear in download mode.
// (Repaired, Deleted, Could Not Repair, etc.)
//
//    Rev 1.25   14 Jan 1996 16:50:12   DALLEE
// Changed #ifdef NAVWNT to NAVSCANW.
//
//    Rev 1.24   15 Nov 1995 21:55:44   DALLEE
// Call S32STAT functions through covers in NAVXUTIL, which dynamically loads
// S32STAT.DLL to get around some path issues on NT.
//
//    Rev 1.23   30 Oct 1995 16:46:38   JWORDEN
// Fix RepairBoot () to detect physical drive number when testing for
// MBR integ repair
//
//    Rev 1.22   30 Oct 1995 10:22:04   JWORDEN
// Different notify message for boot repaired and master boot repaired
//
//    Rev 1.21   25 Oct 1995 00:58:30   DALLEE
// Removed System Inoculation from NAVWNT (Gargoyle) builds.
//
//    Rev 1.20   24 Oct 1995 19:26:12   JWORDEN
// Make modifications to allow for scanning multiple MBR's
//
//    Rev 1.19   01 Jun 1995 14:15:18   SZIADEH
// repair using SYSINTEG first when repairing boot sectors.
//
//    Rev 1.18   23 May 1995 16:25:32   KEITH
// Add SYMSTAT.DLL and code to check for locked disks and improved handling
//
//    Rev 1.17   09 May 1995 10:31:06   KEITH
// Correct some nouns
//
//    Rev 1.16   04 May 1995 15:37:30   MFALLEN
// Display paths with casing just as Win95.
//
//    Rev 1.15   21 Apr 1995 18:05:16   MFALLEN
// NotifyInocDeleteCompletion()
//
//    Rev 1.14   19 Apr 1995 11:16:10   MFALLEN
// Added inoc specific repair feedback message.
//
//    Rev 1.13   18 Apr 1995 18:26:22   MFALLEN
// Touch ups as docs suggested.
//
//    Rev 1.12   17 Apr 1995 15:06:30   MFALLEN
// Special virus handling.
//
//    Rev 1.11   07 Feb 1995 16:44:32   DALLEE
// Removed extraneous call to VirusScanBoot() in RepairBoot().
// Wasn't even checking return code.
//
//
//    Rev 1.10   03 Feb 1995 16:43:44   MFALLEN
// API for SmartInocOpen() changed.
//
//    Rev 1.9   20 Dec 1994 14:46:10   MFALLEN
// Sami did fixes on CERTLIBN
//
//    Rev 1.8   16 Dec 1994 14:19:56   MFALLEN
// Back using the old navxutil
//
//    Rev 1.7   29 Nov 1994 13:36:48   MFALLEN
// Compiles for W16
//
//    Rev 1.6   21 Nov 1994 13:06:52   MFALLEN
// Instead of commenting out all the functions that does not work, they have
// been stubbed out instead.
//
//    Rev 1.5   14 Nov 1994 12:51:28   MFALLEN
// Commented out all functions that deal with passwords, TSR enabeling and network
// alerts.
//
//    Rev 1.4   08 Nov 1994 15:41:56   MFALLEN
// Compiles fine under W32.
//
//
//    Rev 1.35   24 Oct 1994 13:33:56   DALLEE
// Check vs. NO_GENERIC flag before using sys integ to repair a boot virus.
//
//    Rev 1.34   05 Oct 1994 20:03:30   DALLEE
// Boot repair stats should be set outside of RepairBoot().
//   Removed code setting MBR as repaired.
//
//    Rev 1.33   04 Mar 1994 17:50:28   DALLEE
// Set bits to excREG_CHANGE when adding registration exclusion during scanning.
//
//    Rev 1.32   01 Mar 1994 11:47:52   DALLEE
// Use new registration strings for registration items.
//
//    Rev 1.31   24 Feb 1994 14:26:16   DALLEE
// File registration support.  Logs event as inoculation - fix next rev.
//
//    Rev 1.30   12 Dec 1993 16:17:04   DALLEE
// If available, use system inoculation repairs if known boot repair fails.
//
//    Rev 1.29   07 Dec 1993 20:46:34   DALLEE
//
// Set bExcludeDirty flag when exclusions change.
//
//    Rev 1.28   15 Oct 1993 12:29:22   MFALLEN
// Added ComputeKilobytesScanned()
//
//    Rev 1.27   14 Oct 1993 17:42:28   MFALLEN
// Make sure that we don't prompt the user about successfull repairs and
// deletes since it's anoying.
//
//    Rev 1.26   07 Oct 1993 17:51:02   MFALLEN
// Removed AnsiToOem() conversion for FIL4_FILL() since the CERTLIB4 library
// has been fixed.
//
//    Rev 1.25   18 Sep 1993 20:42:32   MFALLEN
// Only display the customized alert message if the process failed. On success
// do not display customized alert.
//
//    Rev 1.24   16 Sep 1993 03:50:30   DALLEE
// Added uRepairType flag (MBR_REPAIR or BOOT_REPAIR) to VirusScanRepairBoot()
// calls.
// ************************************************************************

#include "platform.h"
#include <fcntl.h>
#include <io.h>
#include <time.h>
#include <sys\types.h>
#include <sys\stat.h>
#include "nonav.h"
#include "xapi.h"
#include "symcfg.h"
#include "file.h"
#include "navutil.h"
#include "navwork.h"
#include "options.h"
#include "nunavstb.h"

#ifdef SYM_WIN16
    #include "syminteg.h"
#endif

#include "stub.h"
#include "inoc.h"
#include "virscan.h"

#ifdef SYM_WIN32
    #include "navstat.h"
    #include "symstat.h"
    #include "navver.h"
    #include "disk.h"
    #include "msgdisp.h"
    #include "iquaran.h"
    #include "quar32.h"
#endif

extern  HINSTANCE   hInstance;


// ************************************************************************
//                      LOCAL FUNCTION PROTOTYPES
// ************************************************************************
BOOL DoIntegRepair( char chBootDrive, LPSTR szIntegFile, INTEG* pInteg );



// ************************************************************************
// RepairBoot()
//
// Attempt to repair a boot record from a known virus.
// Record results in NONAV struct.
//
// Parameters:
//      LPNONAV lpNav                   This scan's info.
//      HWND    hDlg
//      BOOL    bNotify
//
// Returns:
//      TRUE                            Successful repair.
//      FALSE                           Repair failed.
// ************************************************************************
// 9/16/93 DALLEE, Added UINT uRepairType parameter to VirusScanRepairBoot.
//              to differentiate MBR and Boot record repairs.
//              Necessary for verification of repair and placement of
//              generic boot sector if known repair fails.
// ************************************************************************

BOOL SYM_EXPORT WINAPI RepairBoot (LPNONAV lpNav, HWND hDlg, BOOL bNotify)
{
    auto    BOOL            bStatus = FALSE;
    auto    NOTIFYSTRUCT    Notify;
    auto    UINT            uRepairType;
#ifndef NAVSCANW
    const   BYTE            cBootDriveLtr = DiskGetBootDrive();
    auto    UINT            uScanStatus;
    auto    INTEG           rInteg;
    auto    N30NOTES        rNotes;
    auto    char            szIntegFile [SYM_MAX_PATH + 1];
#endif
    auto    UINT            uStatus = 0; // non zero is error

#ifdef SYM_WIN32
    auto    DRIVELIST       DriveList [ NUM_DRIVES ];
    auto    LPDRIVELIST     lpCurrDrive;
    auto    LPDRIVELIST     lpScanDrive;
    auto    DRIVEPROBLEMSTRUCT  DriveAccess;
    auto    BOOL            bRetry = TRUE;
#endif

    uRepairType = (lpNav->Infected.wStatus == FILESTATUS_MASTERBOOTINFECTED ?
                        MBR_REPAIR : BOOT_REPAIR);

    TSR_OFF

#ifdef SYM_WIN32

    if (uRepairType != MBR_REPAIR)
        {

                                            // Find the physical drive number for
                                            // each of the logical drives

        MEMSET (DriveList, 0, sizeof (DriveList));

        EnumFixedDrives (DriveList);

                                        // -----------------------------------
                                        // The Saga of the SymStat Lock
                                        //
                                        // In order to make sure that we have
                                        // really gotten the SymStat lock on
                                        // this drive we must take the lock
                                        // on all logical drives that share
                                        // this physical drive. If any of them
                                        // cannot be locked we will report this
                                        // drive as in use.
                                        // -----------------------------------
        while (bRetry)
            {
            bRetry = FALSE;

            for (lpScanDrive = DriveList;
                 lpScanDrive->cDriveLetter;
                 lpScanDrive++)
                {
                if (lpScanDrive->cDriveLetter == lpNav->Infected.szFullPath[0])
                    break;
                }

            for (lpCurrDrive = DriveList;
                 lpCurrDrive->cDriveLetter;
                 lpCurrDrive++)
                {
                if (lpCurrDrive->byPhysicalDrive == lpScanDrive->byPhysicalDrive)
                    uStatus = NavStatSetDriveState (lpCurrDrive->cDriveLetter,
                                                    0,
                                                    DRIVESTATUS_FOREGROUND,
                                                    NAV_APP_CLINIC);
                    if (uStatus != NOERR)
                    {
                        if (bNotify == TRUE)
                        {
                            MEMSET (&DriveAccess, EOS, sizeof(DriveAccess));
                            DriveAccess.hParent         = hDlg;
                            DriveAccess.byDrive         = lpNav->Infected.szFullPath[0];
                            DriveAccess.bHideContinue   = TRUE;

                            DriveAccess.uInfoMsgID = DRIVEREPAIR_MSG_DISKLOCKED_SYMSTAT_BOOT;
                            NavStatGetDriveState ( (BYTE)lpCurrDrive->cDriveLetter,
                                                   0,
                                                   DriveAccess.szWho,
                                                   sizeof (DriveAccess.szWho) );
                            switch ( DriveAccessProblem ( &DriveAccess ) )
                                {
                                case DRIVEERROR_RETRY:
                                    bRetry = TRUE;
                                    for (lpCurrDrive = DriveList;
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
                                    break;

                                case DRIVEERROR_ERROR:
                                case DRIVEERROR_CONTINUE:
                                case DRIVEERROR_SKIP:
                                    break;
                                }
                        }

                        break;
                    }
                    else
                        lpCurrDrive->bDriveLocked = TRUE;

                }

            }
        }

    if (!uStatus)
#endif
        {
#ifndef NAVSCANW
                                           // try SYSINTEG first
        if ( !HWIsNEC() &&
             (
               (uRepairType == MBR_REPAIR &&
                lpNav->Infected.szFullPath[0] == 0x00)
               ||
               (uRepairType != MBR_REPAIR &&
                lpNav->Infected.szFullPath[0] == cBootDriveLtr)
             )
           )
            {
            if (!(NO_GENERIC & lpNav->Infected.Notes.wControl2)
            &&  lpNav->Inoc.bUseSystemInoc )
                {
                rInteg.wStatus = INTEG_FAIL_VERIFY;
                STRCPY(rInteg.byFilename, (uRepairType == MBR_REPAIR ? "MBR80"
                                                                     : "BOOT800"));
                GetSysIntegFile(szIntegFile, SYM_MAX_PATH, &lpNav->Inoc);

                if (IntegVerifyDatabaseMatch(szIntegFile))
                    {
                    DoIntegRepair( cBootDriveLtr, szIntegFile, &rInteg );
                    }

                                                // Verify the repair.
                bStatus = (uRepairType == MBR_REPAIR
                           ? !VirusScanMasterBoot ((UINT)(lpNav->Infected.szFullPath[0] + 0x80),
                                                   &rNotes,
                                                   &uScanStatus)
                           : !VirusScanBoot(GetPhysicalDriveLetter( lpNav->Infected.szFullPath[0] ),
                                            &rNotes,
                                            &uScanStatus) );
                if (NOERR != uScanStatus)       // Make sure the scan was valid.
                    {
                    bStatus = FALSE;
                    }
                }
            }
#endif
                                            // If SYSINTEG didn't repair
                                            // do it the old fashion way!
        if ( bStatus == FALSE )
            {
                if (uRepairType != MBR_REPAIR)
                {
                bStatus = VirusScanRepairBoot (GetPhysicalDriveLetter( *lpNav->Infected.szFullPath ),
                                                lpNav->Infected.uVirusSigIndex,
                                                uRepairType,
                                                NULL);
                }
            else
                {
                bStatus = VirusScanRepairBoot ((UINT)(*lpNav->Infected.szFullPath) + 0x80,
                                                lpNav->Infected.uVirusSigIndex,
                                                uRepairType,
                                                NULL);
                }
            }
        }

#ifdef SYM_WIN32
    if (uRepairType != MBR_REPAIR)
        {
        for (lpCurrDrive = DriveList;
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
        }
#endif

    TSR_ON

    if (bNotify == TRUE)
        {
        char szPath[SYM_MAX_PATH + 1];

        MEMSET (&Notify, EOS, sizeof(Notify));
        Notify.hParent       = hDlg;

        if (uRepairType == MBR_REPAIR)
            {
            SPRINTF (szPath, "%d", (UINT)lpNav->Infected.szFullPath[0]);
            if (bStatus == TRUE)
                Notify.uMsgID        = IDS_MASTERBOOTREPAIRED;
            else
                Notify.uMsgID        = IDS_MASTERBOOTNOTREPAIRED;
            }
        else
            {
            STRCPY (szPath, lpNav->Infected.szFullPath);
            if (bStatus == TRUE)
                Notify.uMsgID        = IDS_BOOTREPAIRED;
            else
                Notify.uMsgID        = IDS_BOOTNOTREPAIRED;
            }
        Notify.uTitleID      = IDS_NAV;
        if ( GetCustomVersion() == VERSION_NETSCAPE )
            {                           // Title is "Internet Scanner"; set at runtime.
            Notify.uTitleID      = IDS_NAV_INTERNET;
            }
        Notify.lpFileName    = szPath;
        Notify.lpVirusName   = lpNav->Infected.Notes.lpVirName;
        Notify.uRemoveAfter  = (lpNav->Alert.bRemoveAfter) ?
                                lpNav->Alert.uSeconds : NULL;
        Notify.hInstance     = hInstance;

        if ( bStatus == FALSE )
            {
            Notify.lpAlertMsg = (lpNav->Alert.bDispAlertMsg) ?
                                lpNav->Alert.szAlertMsg : NULL;
            }

        LongCustomMessage(hDlg);

        NotifyUser (&Notify);
        }

    return (bStatus);
}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

BOOL DoIntegRepair( char chBootDrive, LPSTR szIntegFile, INTEG* pInteg )
{
#ifndef NAVSCANW
    auto    DISKREC         diskRec;
    auto    CLUSTER         clRootSCN;

    ZeroMemory( &diskRec, sizeof( DISKREC ) );

    // Gather information about this disk.
    if( DiskGetInfo( chBootDrive, &diskRec ) == FALSE )
        return FALSE;

    // If this is not a FAT32 drive, just do the normal repair
    if( !IsFAT32( diskRec ) )
        {
        IntegRepair(szIntegFile, pInteg);
        return TRUE;
        }

    // Get Root SCN from OS.
    if( clRootSCN = DiskGetRootSCN( &diskRec ) == 0 )
        return FALSE;

    // Do the integ repair.
    IntegRepair(szIntegFile, pInteg);

    // Now, set the root SCN.
    if( DiskSetRootSCN( &diskRec, clRootSCN ) == FALSE )
        return FALSE;
#endif
    return TRUE;
}


// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

BOOL SYM_EXPORT WINAPI NavDeleteFile (
    LPNONAV     lpNav,
    HWND        hDlg,
    BOOL        bNotify
    )
{

    auto    BOOL            bStatus;

#ifdef SYM_WIN32
    auto    BOOL    bFileApisAnsi;
#endif

    TSR_OFF;

    // The ANSI/OEM mess continues. Make sure file APIs are OEM
    // before calling the Virscan functions

#ifdef SYM_WIN32
    bFileApisAnsi = AreFileApisANSI();

    if (bFileApisAnsi)
        SetFileApisToOEM ();
#endif

    if (!VirusScanCanDeleteFile(lpNav->Infected.szFullOemPath))
        {
        bStatus = FALSE;
        }
    else
        {
        bStatus = VirusScanDeleteFile (lpNav->Infected.szFullOemPath, 1);
        }

    // Set the file APIs to back to ANSI when done with Virscan.

#ifdef SYM_WIN32
    if (bFileApisAnsi)
        SetFileApisToANSI ();
#endif

    TSR_ON;

    if ( bNotify == TRUE )
        NotifyDeleteCompletion (hDlg, bStatus, lpNav);

    return (bStatus);
}

BOOL SYM_EXPORT WINAPI NavMoveFile (LPNONAV lpNav, HWND hDlg, BOOL bNotify)
{
    auto    BOOL                bFileApisAnsi = TRUE;
    auto    BOOL                bStatus = FALSE;
    auto    IQuarantineDLL*     p;
    auto    IQuarantineItem*    pQuarBackupItem = NULL;
    auto    HRESULT             hr;
    auto    int                 q_type = QFILE_TYPE_REPAIRABLE;
    auto    char                szFileName  [ SYM_MAX_PATH + 1 ] = {0};
    auto    char                szPath[ SYM_MAX_PATH + 1 ] = {0};
    auto    char                szFile[ SYM_MAX_PATH + 1 ] = {0};

    // The Horror! File APIs have to be OEM for Virscan and
    // ANSI for Quarantine.

#ifdef SYM_WIN32
    bFileApisAnsi = AreFileApisANSI();

    if (!bFileApisAnsi)
    {
        SetFileApisToANSI ();
    }
#endif

    CoInitialize(NULL);

    hr = CoCreateInstance( CLSID_QuarantineDLL,
                           NULL,
                           CLSCTX_INPROC_SERVER,
                           IID_QuarantineDLL,
                           (LPVOID*)&p);

    if( FAILED( hr ) )
        bStatus = FALSE;
    else
    {

        hr = p->Initialize();

        if( FAILED( hr ) )
            bStatus = FALSE;
        else
        {
            MEMCPY(szFileName, lpNav->Infected.szFullPath, sizeof(lpNav->Infected.szFullPath));

            if (lpNav->Infected.wStatus == FILESTATUS_ZIPINFECTED)
            {
                RemoveZipInfected (szFileName);
                q_type = QFILE_TYPE_COMPRESSED;
            }

            hr = p->AddFileToQuarantine(szFileName,
                                        q_type,
                                        QFILE_STATUS_QUARANTINED,
                                        &pQuarBackupItem);

            if( FAILED( hr ) )
                bStatus = FALSE;
            else
            {
                bStatus = TRUE;

                if (hr != S_UNDELETABLE_VIRUS_FOUND )
                {
                    // Change the file attributes to not be read-only
                    DWORD dwFileAttr = GetFileAttributes( szFileName );
                    if( 0xFFFFFFFF != dwFileAttr )
                        {
                        if( ( dwFileAttr & FILE_ATTRIBUTE_READONLY ) == FILE_ATTRIBUTE_READONLY )
                            {
                            dwFileAttr -= FILE_ATTRIBUTE_READONLY;
                            }

                        bStatus = SetFileAttributes( szFileName, dwFileAttr );
                        }

                    // Delete the source file.
                    if ( FileDelete (szFileName) == NOERR )
                        bStatus = TRUE;
                    else
                        bStatus = FALSE;
                }

                // If the file delete failed or the quarantined item contains a
                // non-deletable Dir II-type virus, then delete the item from
                // quarantine and tell the caller we failed.

                if ( (bStatus == FALSE) || (hr == S_UNDELETABLE_VIRUS_FOUND) )
                {
                    pQuarBackupItem->DeleteItem();
                    pQuarBackupItem->Release();

                    bStatus = FALSE;
                }
            }
            p->Release();
        }

    }

    CoUninitialize();

    // Make sure file APIs are set back to ANSI.

#ifdef SYM_WIN32
    if (!bFileApisAnsi)
        SetFileApisToOEM ();
#endif

    if ( bNotify == TRUE )
        NotifyMoveCompletion(hDlg, bStatus, lpNav);

    return (bStatus);
}


// ---------------------------------------------------------------------------
// NavMoveFileInf
//
// Same as NavMoveFile() but has an additional parameter to an LPINFECTEDFILE
// and uses that to get the file info instead of getting it from LPNAV->Infected.
// ---------------------------------------------------------------------------
BOOL SYM_EXPORT WINAPI NavMoveFileInf (LPNONAV lpNav, LPINFECTEDFILE lpInfected, HWND hDlg, BOOL bNotify)
{
    auto    BOOL                bFileApisAnsi = TRUE;
    auto    BOOL                bStatus = FALSE;
    auto    IQuarantineDLL*     p;
    auto    IQuarantineItem*    pQuarBackupItem = NULL;
    auto    HRESULT             hr;
    auto    int                 q_type = QFILE_TYPE_REPAIRABLE;
    auto    char                szFileName  [ SYM_MAX_PATH + 1 ] = {0};
    auto    char                szPath[ SYM_MAX_PATH + 1 ] = {0};
    auto    char                szFile[ SYM_MAX_PATH + 1 ] = {0};

    // The Horror! File APIs have to be OEM for Virscan and
    // ANSI for Quarantine.

#ifdef SYM_WIN32
    bFileApisAnsi = AreFileApisANSI();

    if (!bFileApisAnsi)
    {
        SetFileApisToANSI ();
    }
#endif

    CoInitialize(NULL);

    hr = CoCreateInstance( CLSID_QuarantineDLL,
                           NULL,
                           CLSCTX_INPROC_SERVER,
                           IID_QuarantineDLL,
                           (LPVOID*)&p);

    if( FAILED( hr ) )
        bStatus = FALSE;
    else
    {

        hr = p->Initialize();

        if( FAILED( hr ) )
            bStatus = FALSE;
        else
        {
            MEMCPY(szFileName, lpInfected->szFullPath, sizeof(lpInfected->szFullPath));

            if (lpInfected->wStatus == FILESTATUS_ZIPINFECTED)
            {
                RemoveZipInfected (szFileName);
                q_type = QFILE_TYPE_COMPRESSED;
            }

            hr = p->AddFileToQuarantine(szFileName,
                                        q_type,
                                        QFILE_STATUS_QUARANTINED,
                                        &pQuarBackupItem);

            if( FAILED( hr ) )
                bStatus = FALSE;
            else
            {
                bStatus = TRUE;

                if (hr != S_UNDELETABLE_VIRUS_FOUND )
                {
                    // Change the file attributes to not be read-only
                    DWORD dwFileAttr = GetFileAttributes( szFileName );
                    if( 0xFFFFFFFF != dwFileAttr )
                        {
                        if( ( dwFileAttr & FILE_ATTRIBUTE_READONLY ) == FILE_ATTRIBUTE_READONLY )
                            {
                            dwFileAttr -= FILE_ATTRIBUTE_READONLY;
                            }

                        bStatus = SetFileAttributes( szFileName, dwFileAttr );
                        }

                    // Delete the source file.
                    if ( FileDelete (szFileName) == NOERR )
                        bStatus = TRUE;
                    else
                        bStatus = FALSE;
                }

                // If the file delete failed or the quarantined item contains a
                // non-deletable Dir II-type virus, then delete the item from
                // quarantine and tell the caller we failed.

                if ( (bStatus == FALSE) || (hr == S_UNDELETABLE_VIRUS_FOUND) )
                {
                    pQuarBackupItem->DeleteItem();
                    pQuarBackupItem->Release();

                    bStatus = FALSE;
                }
            }
            p->Release();
        }

    }

    CoUninitialize();

    // Make sure file APIs are set back to ANSI.

#ifdef SYM_WIN32
    if (!bFileApisAnsi)
        SetFileApisToOEM ();
#endif

    if ( bNotify == TRUE )
        NotifyMoveCompletion(hDlg, bStatus, lpNav);

    return (bStatus);
}



// ---------------------------------------------------------------------------
// InocThisFile()
//
// (ANSI/OEM mess: Works internally in ANSI)
// ---------------------------------------------------------------------------

BOOL SYM_EXPORT WINAPI InocThisFile (
    LPNONAV     lpNav,
    HWND        hDlg,
    BOOL        bNotify
    )
{
    auto    BOOL            bStatus = FALSE;
    auto    NOTIFYSTRUCT    Notify;
    auto    UINT            uOrigInocState = lpNav->uInocOpenOK;
    auto    char            szFileName [ SYM_MAX_PATH + 1 ] = { 0 };
#ifdef SYM_WIN32
    auto    BOOL            bFileApisAnsi;
#endif

    SYM_ASSERT ( lpNav );

#ifdef SYM_WIN32
    bFileApisAnsi = AreFileApisANSI();

    if (!bFileApisAnsi)
        SetFileApisToANSI();
#endif
                                        // -----------------------------------
                                        // Work with the copy so case is pre-
                                        // served.
                                        // -----------------------------------

    STRNCPY ( szFileName, lpNav->Infected.szFullPath, sizeof(szFileName) );

    TSR_OFF;

    if (lpNav->Infected.wStatus == FILESTATUS_INOCULATEDCHANGED)
        InocDelFile ( szFileName );

    if (lpNav->uInocOpenOK != SMARTOPEN_RW)
        {
        lpNav->uInocOpenOK = SmartInocOpen ( szFileName, TRUE );
        }

    if (lpNav->uInocOpenOK != SMARTOPEN_ERROR)
        bStatus = InocFile ( szFileName );
    else
        bStatus = FALSE;

    TSR_ON;

    if (bNotify == TRUE)
        {
        MEMSET (&Notify, EOS, sizeof(Notify));
        Notify.hParent       = hDlg;

        if (lpNav->Infected.wStatus == FILESTATUS_NOTINOCULATED)
            {
            Notify.uMsgID = (bStatus == TRUE) ? IDS_FILEINOCULATED :
                            IDS_FILENOTINOCULATED;
            }
        else
            {
            Notify.uMsgID = (bStatus == TRUE) ? IDS_FILEREINOCULATED :
                            IDS_FILENOTINOCULATED;
            }

        Notify.uTitleID      = IDS_NAV;
        if ( GetCustomVersion() == VERSION_NETSCAPE )
            {                           // Title is "Internet Scanner"; set at runtime.
            Notify.uTitleID      = IDS_NAV_INTERNET;
            }
        Notify.lpFileName    = szFileName;
        Notify.lpVirusName   = lpNav->Infected.Notes.lpVirName;
        Notify.uRemoveAfter  = (lpNav->Alert.bRemoveAfter) ?
                                lpNav->Alert.uSeconds : 0;
        Notify.hInstance     = hInstance;

        if ( bStatus == FALSE )
            {
            Notify.lpAlertMsg = (lpNav->Alert.bDispAlertMsg) ?
                                lpNav->Alert.szAlertMsg : NULL;
            }

        NotifyUser (&Notify);
        }

    if (uOrigInocState != SMARTOPEN_RW)
        {
        lpNav->uInocOpenOK = SmartInocOpen ( szFileName, FALSE );
        }

#ifdef SYM_WIN32
    if (!bFileApisAnsi)
        SetFileApisToOEM();
#endif

    return (bStatus);
}


/*@API:**********************************************************************
@Declaration:
BOOL SYM_EXPORT WINAPI InocDeleteFile (
    LPNONAV     lpNav,
    HWND        hDlg,
    BOOL        bNotify
    )

@Description:
This routine deletes a file which has inoculation data. Presumably one
which has changed since being inoculated. :-)
Inoculation data for the file is also removed from the inoc database.
The inoc database must be initialized and open before calling this routine.

06/20/1996 DALLEE: Errors in removing inoc information are not reported,
only file delete results are returned.
(T/F return code doesn't allow specific errors so it's best to just give
a correct result for the primary function -- delete).

@Parameters:
$lpNav$     Pointer to this scan's info
$hDlg$      Parent HWND if need to display message
$bNotify$   TRUE to display error messages

@Returns:   TRUE if file deleted, FALSE otherwise.
@See:
@Include:   NONAV.H
@Compatibility: W32
****************************************************************************/

BOOL SYM_EXPORT WINAPI InocDeleteFile (
    LPNONAV     lpNav,
    HWND        hDlg,
    BOOL        bNotify
    )
{
    auto    BOOL            bStatus = FALSE;
    auto    NOTIFYSTRUCT    Notify;
    auto    char            szFileName [ SYM_MAX_PATH + 1 ];
#ifdef SYM_WIN32
    auto    BOOL            bFileApisAnsi;
#endif

                                        // -----------------------------------
                                        // Work with the copy of the file so
                                        // case is preserved.
                                        // -----------------------------------

    TSR_OFF;
#ifdef SYM_WIN32
                                        // Nuke inoc data for this file (ANSI)
    bFileApisAnsi = AreFileApisANSI();
    if (!bFileApisAnsi)
        SetFileApisToANSI();
#endif

    STRCPY(szFileName, lpNav->Infected.szFullPath);
    InocDelFile(szFileName);

#ifdef SYM_WIN32
                                        // Nuke the file (OEM)
    SetFileApisToOEM();
    STRCPY(szFileName, lpNav->Infected.szFullOemPath);
#endif

    bStatus = (NOERR == FileDelete(szFileName));

#ifdef SYM_WIN32
    if (bFileApisAnsi)
        SetFileApisToANSI();
#endif

    TSR_ON;

    if (bNotify == TRUE)
        {
        MEMSET (&Notify, EOS, sizeof(Notify));
        Notify.hParent       = hDlg;
        Notify.uMsgID        = (bStatus == TRUE) ? IDS_FILEINOCDELETED :
                                IDS_FILEINOCNOTDELETED;
        Notify.uTitleID      = IDS_NAV;
        if ( GetCustomVersion() == VERSION_NETSCAPE )
            {                           // Title is "Internet Scanner"; set at runtime.
            Notify.uTitleID      = IDS_NAV_INTERNET;
            }
        Notify.lpFileName    = szFileName;
        Notify.lpVirusName   = lpNav->Infected.Notes.lpVirName;
        Notify.uRemoveAfter  = (lpNav->Alert.bRemoveAfter) ?
                               lpNav->Alert.uSeconds : 0;
        Notify.hInstance     = hInstance;

        if ( bStatus == FALSE )
            {
            Notify.lpAlertMsg = (lpNav->Alert.bDispAlertMsg) ?
                                lpNav->Alert.szAlertMsg : NULL;
            }

        NotifyUser (&Notify);
        }

    return (bStatus);
}


//************************************************************************
// ExcludeAddFile()
//
// This routine adds the current file to the list of exclusions.
// (ANSI/OEM mess: Internal work here is in ANSI)
//
// Parameters:
//      LPNONAV lpNav                   Info on current scan and file.
//      HWND    hDlg                    Handle to current prompt dialog.
//      BOOL    bNotify                 Pop-up completion status?
//
// Returns:
//      TRUE                            File added to exclusions.
//      FALSE                           File not added.
//************************************************************************
//  3/04/94 DALLEE, added support for registration exclusions (BANKERS only)
//************************************************************************

BOOL SYM_EXPORT WINAPI ExcludeAddFile (
    LPNONAV lpNav,
    HWND    hDlg,
    BOOL    bNotify
    )
{
    auto    BOOL            bStatus = FALSE;
    auto    EXCLUDEITEM     ExcludeItem;
    auto    NOTIFYSTRUCT    Notify;

    STRCPY (ExcludeItem.szText, lpNav->Infected.szFullOemPath);
    NavOemStrUpr (ExcludeItem.szText);
    ExcludeItem.bSubDirectory = FALSE;

    if (lpNav->Infected.wStatus == FILESTATUS_INOCULATEDCHANGED ||
        lpNav->Infected.wStatus == FILESTATUS_INOCULATIONFAILED ||
        lpNav->Infected.wStatus == FILESTATUS_REINOCULATIONFAILED ||
        lpNav->Infected.wStatus == FILESTATUS_NOTINOCULATED)
        ExcludeItem.wBits = excINOC_CHANGE;
    else
        ExcludeItem.wBits = excVIRUS_FOUND;

    if (ExcludeAdd (lpNav->lpExclude, &ExcludeItem))
        bStatus = TRUE;

    lpNav->bExcludeDirty = TRUE;

    if (bNotify == TRUE)
        {
        MEMSET (&Notify, EOS, sizeof(Notify));
        Notify.hParent = hDlg;

        if (ExcludeItem.wBits == excINOC_CHANGE)
            {
            Notify.uMsgID = (bStatus == TRUE) ? IDS_FILEEXCLUDED_INOC :
                             IDS_FILENOTEXCLUDED;
            }
        else
            {
            Notify.uMsgID = (bStatus == TRUE) ? IDS_FILEEXCLUDED :
                             IDS_FILENOTEXCLUDED;
            }

        Notify.uTitleID      = IDS_NAV;
        if ( GetCustomVersion() == VERSION_NETSCAPE )
            {                           // Title is "Internet Scanner"; set at runtime.
            Notify.uTitleID      = IDS_NAV_INTERNET;
            }
        Notify.lpFileName    = lpNav->Infected.szFullPath;
        Notify.lpVirusName   = lpNav->Infected.Notes.lpVirName;
        Notify.uRemoveAfter  = (lpNav->Alert.bRemoveAfter) ?
                                lpNav->Alert.uSeconds : 0;
        Notify.hInstance     = hInstance;

        if ( bStatus == FALSE )
            {
            Notify.lpAlertMsg = (lpNav->Alert.bDispAlertMsg) ?
                                lpNav->Alert.szAlertMsg : NULL;
            }

        NotifyUser (&Notify);
        }

    return (bStatus);
}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

VOID WINAPI NotifyRepairCompletion (
    HWND    hDlg,
    BOOL    bStatus,
    LPNONAV lpNav
    )
{
    auto    NOTIFYSTRUCT    Notify;

    MEMSET (&Notify, EOS, sizeof(Notify));

    Notify.hParent = hDlg;

    if (bStatus == TRUE)
        {
        if (lpNav->lpScan->uProgramMode == PROGRAM_DOWNLOAD)
            Notify.uMsgID = IDS_DOWNLOADREPAIRED;
        else
            Notify.uMsgID = IDS_FILEREPAIRED;
        }
    else
        {

        if (lpNav->lpScan->uProgramMode == PROGRAM_DOWNLOAD)
            Notify.uMsgID = IDS_DOWNLOADNOTREPAIRED;
        else
            Notify.uMsgID = IDS_FILENOTREPAIRED;
        }

    Notify.uTitleID      = IDS_NAV;
    if ( GetCustomVersion() == VERSION_NETSCAPE )
        {                               // Title is "Internet Scanner"; set at runtime.
        Notify.uTitleID      = IDS_NAV_INTERNET;
        }
                                        // If we're downloading, don't
                                        // refer to the file by using it's
                                        // full path. Use only the
                                        // file name:
    char szTemp[ SYM_MAX_PATH + 1 ];

    if (lpNav->lpScan->uProgramMode == PROGRAM_DOWNLOAD)
        {
        Notify.lpFileName    = lpNav->Infected.szFullPath;
        if ((NameReturnFile((lpNav->Infected.szFullPath), szTemp)) == 0)
            Notify.lpFileName    = szTemp;
        }
    else
        {
        Notify.lpFileName    = lpNav->Infected.szFullPath;
        }

    Notify.lpVirusName   = lpNav->Infected.Notes.lpVirName;
    Notify.uRemoveAfter  = (lpNav->Alert.bRemoveAfter) ?
                            lpNav->Alert.uSeconds : NULL;
    Notify.hInstance     = hInstance;

    if ( bStatus == FALSE )
        {
        Notify.lpAlertMsg = (lpNav->Alert.bDispAlertMsg) ?
                            lpNav->Alert.szAlertMsg : NULL;
        }

    LongCustomMessage(hDlg);
    NotifyUser (&Notify);
}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

VOID WINAPI NotifyInocRepairCompletion (
    HWND    hDlg,
    BOOL    bStatus,
    LPNONAV lpNav
    )
{
    auto    NOTIFYSTRUCT    Notify;

    MEMSET (&Notify, EOS, sizeof(Notify));

    Notify.hParent = hDlg;

    if (bStatus == TRUE)
        Notify.uMsgID = IDS_FILEINOCREPAIRED;
    else
        Notify.uMsgID = IDS_FILEINOCNOTREPAIRED;

    Notify.uTitleID      = IDS_NAV;
    if ( GetCustomVersion() == VERSION_NETSCAPE )
        {                           // Title is "Internet Scanner"; set at runtime.
        Notify.uTitleID      = IDS_NAV_INTERNET;
        }
    Notify.lpFileName    = lpNav->Infected.szFullPath;
    Notify.uRemoveAfter  = (lpNav->Alert.bRemoveAfter) ?
                            lpNav->Alert.uSeconds : NULL;
    Notify.hInstance     = hInstance;

    if ( bStatus == FALSE )
        {
        Notify.lpAlertMsg = (lpNav->Alert.bDispAlertMsg) ?
                            lpNav->Alert.szAlertMsg : NULL;
        }

    NotifyUser (&Notify);
}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

VOID WINAPI NotifyDeleteCompletion (
    HWND    hDlg,
    BOOL    bStatus,
    LPNONAV lpNav
    )
{
    auto    NOTIFYSTRUCT    Notify;

    MEMSET (&Notify, EOS, sizeof(Notify));

    Notify.hParent = hDlg;

    if (bStatus == TRUE)
        {
        if (lpNav->lpScan->uProgramMode == PROGRAM_DOWNLOAD)
            Notify.uMsgID = IDS_DOWNLOADDELETED;
        else
            Notify.uMsgID = IDS_FILEDELETED;
        }
    else
        {
        if (lpNav->lpScan->uProgramMode == PROGRAM_DOWNLOAD)
            Notify.uMsgID = IDS_DOWNLOADNOTDELETED;
        else
            Notify.uMsgID = IDS_FILENOTDELETED;
        }
    Notify.uTitleID      = IDS_NAV;
    if ( GetCustomVersion() == VERSION_NETSCAPE )
        {                           // Title is "Internet Scanner"; set at runtime.
        Notify.uTitleID      = IDS_NAV_INTERNET;
        }
                                        // If we're downloading, don't
                                        // refer to the file by using it's
                                        // full path. Use only the
                                        // file name:
    char szTemp[ SYM_MAX_PATH + 1 ];

    if (lpNav->lpScan->uProgramMode == PROGRAM_DOWNLOAD)
        {
        Notify.lpFileName    = lpNav->Infected.szFullPath;
        if ((NameReturnFile((lpNav->Infected.szFullPath), szTemp)) == 0)
            Notify.lpFileName    = szTemp;
        }
    else
        {
        Notify.lpFileName    = lpNav->Infected.szFullPath;
        }

    Notify.lpVirusName   = lpNav->Infected.Notes.lpVirName;
    Notify.uRemoveAfter  = (lpNav->Alert.bRemoveAfter) ?
                            lpNav->Alert.uSeconds : NULL;
    Notify.hInstance     = hInstance;

    if ( bStatus == FALSE )
        {
        Notify.lpAlertMsg = (lpNav->Alert.bDispAlertMsg) ?
                            lpNav->Alert.szAlertMsg : NULL;
        }

    LongCustomMessage(hDlg);
    NotifyUser (&Notify);
}

VOID WINAPI NotifyMoveCompletion (HWND hDlg, BOOL bStatus, LPNONAV lpNav)
{
    auto    NOTIFYSTRUCT    Notify;

    MEMSET (&Notify, EOS, sizeof(Notify));

    Notify.hParent = hDlg;

    if (bStatus == TRUE)
    {
        if (lpNav->lpScan->uProgramMode == PROGRAM_DOWNLOAD)
            Notify.uMsgID = IDS_DOWNLOADMOVED;
        else
            Notify.uMsgID = IDS_FILEMOVED;
    }
    else
    {
        if (lpNav->lpScan->uProgramMode == PROGRAM_DOWNLOAD)
            Notify.uMsgID = IDS_DOWNLOADNOTMOVED;
        else
            Notify.uMsgID = IDS_FILENOTMOVED;
    }
    Notify.uTitleID      = IDS_NAV;
    if ( GetCustomVersion() == VERSION_NETSCAPE )
    {                           // Title is "Internet Scanner"; set at runtime.
        Notify.uTitleID      = IDS_NAV_INTERNET;
    }
                                        // If we're downloading, don't
                                        // refer to the file by using it's
                                        // full path. Use only the
                                        // file name:
    char szTemp[ SYM_MAX_PATH + 1 ];

    if (lpNav->lpScan->uProgramMode == PROGRAM_DOWNLOAD)
    {
        Notify.lpFileName    = lpNav->Infected.szFullPath;
        if ((NameReturnFile((lpNav->Infected.szFullPath), szTemp)) == 0)
            Notify.lpFileName = szTemp;
    }
    else
    {
        if (lpNav->Infected.wStatus == FILESTATUS_QUARANTINE_INFECTED_ZIP ||
            lpNav->Infected.wStatus == FILESTATUS_ZIPINFECTED )
        {
            MEMCPY(szTemp, lpNav->Infected.szFullPath, sizeof(lpNav->Infected.szFullPath));
            RemoveZipInfected (szTemp);
            Notify.lpFileName = szTemp;
        }
        else
            Notify.lpFileName = lpNav->Infected.szFullPath;
    }

    Notify.lpVirusName   = lpNav->Infected.Notes.lpVirName;
    Notify.uRemoveAfter  = (lpNav->Alert.bRemoveAfter) ? lpNav->Alert.uSeconds : NULL;
    Notify.hInstance     = hInstance;

    if ( bStatus == FALSE )
    {
        Notify.lpAlertMsg = (lpNav->Alert.bDispAlertMsg) ? lpNav->Alert.szAlertMsg : NULL;
    }

    LongCustomMessage(hDlg);
    NotifyUser (&Notify);
}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

VOID WINAPI NotifyInocDeleteCompletion (
    HWND    hDlg,
    BOOL    bStatus,
    LPNONAV lpNav
    )
{
    auto    NOTIFYSTRUCT    Notify;

    MEMSET (&Notify, EOS, sizeof(Notify));

    Notify.hParent = hDlg;

    if (bStatus == TRUE)
        Notify.uMsgID = IDS_FILEINOCDELETED;
    else
        Notify.uMsgID = IDS_FILEINOCNOTDELETED;

    Notify.uTitleID      = IDS_NAV;
    if ( GetCustomVersion() == VERSION_NETSCAPE )
        {                           // Title is "Internet Scanner"; set at runtime.
        Notify.uTitleID      = IDS_NAV_INTERNET;
        }
    Notify.lpFileName    = lpNav->Infected.szFullPath;
    Notify.uRemoveAfter  = (lpNav->Alert.bRemoveAfter) ?
                            lpNav->Alert.uSeconds : NULL;
    Notify.hInstance     = hInstance;

    if ( bStatus == FALSE )
        {
        Notify.lpAlertMsg = (lpNav->Alert.bDispAlertMsg) ?
                            lpNav->Alert.szAlertMsg : NULL;
        }

    NotifyUser (&Notify);
}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

VOID WINAPI GetSysIntegFileOem (
    LPSTR       lpPath,
    UINT        uSize,
    LPINOCOPT   lpInoc
    )
{
    GetSysIntegFile (lpPath, uSize, lpInoc);
    NavAnsiToOem(lpPath, lpPath);
}

