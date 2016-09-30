// Copyright 1996 - 1998 Symantec, Peter Norton Product Group
//***************************************************************************
//
// $Header:   S:/APKERNEL/VCS/TASKSCAN.CPv   1.11   30 Sep 1998 12:52:30   RStanev  $
//
// Description:
//      This file defines Kernel functions for processing known virus
//      scanning tasks.
//
// Contains:
//      TaskScanFile()
//      OverrideDIR2Protection()        (local function)
//      TaskScanBootRecord()
//      TaskScanDevices()
//
// See Also:
//      krnlapi.cpp, frepair.cpp
//
// Note:
//      Kernel module
//
//***************************************************************************
// $Log:   S:/APKERNEL/VCS/TASKSCAN.CPv  $
// 
//    Rev 1.11   30 Sep 1998 12:52:30   RStanev
// UnInitFastCleanCache() should be DeInitFastCleanCache().
//
//    Rev 1.10   29 Sep 1998 20:00:28   RStanev
// Added fast and dirty cache as a patch to slow network access.
//
//    Rev 1.9   27 Jul 1998 16:24:00   RStanev
// Removed changes from previous revision because they had undesirable effects
// when accessing files on write protected media.
//
//    Rev 1.8   11 Jun 1998 18:24:58   CEATON
// Added code to TaskScanFile() to preserve the last-accessed date of files
// being scanned.
//
//    Rev 1.7   08 Jun 1998 21:06:04   RStanev
// Backup files are now created by quarantine.
//
//    Rev 1.6   19 Feb 1998 15:54:28   RStanev
// Added quarantine support.
//
//    Rev 1.5   17 Feb 1998 20:25:12   RStanev
// Updated copyright.
//
//    Rev 1.4   02 Jun 1997 10:12:20   RStanev
// Added support for MESSAGE_BOX_COOKIE.
//
//    Rev 1.3   01 Jun 1997 17:21:26   RStanev
// Updated to match changes in the prototype of VLRepairBootPart().
//
//    Rev 1.2   15 May 1997 02:03:46   RStanev
// Minor changes.
//
//    Rev 1.1   29 Apr 1997 12:04:30   RStanev
// Fixed compilation errors introduced by AVENGE changes.
//
//    Rev 1.0   26 Feb 1997 16:39:48   RSTANEV
// Initial revision.
//
//***************************************************************************

#include "platform.h"
#include "drvcomm.h"
#include "symsync.h"
#include "apcomm.h"
#include "navap.h"
#include "apkernel.h"

//---------------------------------------------------------------------------
//
// Local function prototypes
//
//---------------------------------------------------------------------------
VOID OverrideDIR2Protection (
    HEVENTPACKET hEventPacket,
    HVLVIRUS     hVLVirus
    );

BOOL FastAndDirtyCleanCheck ( PTSTR pszFile );
VOID FastAndDirtyCleanReset ( VOID );

//---------------------------------------------------------------------------
//
// BOOL TaskScanFile (
//      HEVENTPACKET hEventPacket
//      );
//
// This function services the TASK_SCAN_FILE task.  It scans a file for
// known viruses and if the file is infected, it queries the UI for
// an action to take.
//
// Entry:
//      hEventPacket - Event Packet handle
//
// Exit:
//      TRUE  if no virus was found
//      FALSE if a known virus was found, and an action against it was
//            taken.  In this case, hEventPacket members set so that the
//            event callback knows what steps to take from now on.
//
//---------------------------------------------------------------------------
BOOL TaskScanFile (
    HEVENTPACKET hEventPacket
    )
    {
    auto BOOL         bResult;
    auto HVLVIRUS     hVLVirus;
    auto AVACTION     uAVAction;
    auto AVACTIONDATA rAVActionData;
    auto BOOL         bActionCompleted = FALSE;
    auto VSTATUS      uVStatus;

                                        // Validate input parameters.

    SYM_ASSERT ( hEventPacket );
    SYM_ASSERT ( hEventPacket->prEDExtra );
    SYM_ASSERT ( hEventPacket->prEDExtra->hVGlobalContext );

                                        // Check if we had already scanned
                                        // the file and if it is clean.

    if ( FastAndDirtyCleanCheck ( EDXpszOEMFile ( hEventPacket->prEDExtra ) ) )
        return TRUE;

                                        // Initialize local variables.

    bResult = TRUE;

    hVLVirus = NULL;

                                        // Scan file.

    uVStatus = VLScanFile ( hEventPacket->prEDExtra->hVGlobalContext,
                            hEventPacket->prEDExtra->pszFile,
                            &hVLVirus );

                                        // If out of memory, report error.

    if ( uVStatus == VS_MEMALLOC )
        {
        UIOutOfMemoryError();
        }
    else if ( uVStatus == VS_OK )
        {
                                        // If scanning ok then check if
                                        // virus found and file not
                                        // excluded.

        uAVAction = ( hVLVirus &&
                      !NAVAPIsFileExcluded (
                          hEventPacket,
                          EDXpszOEMFile(hEventPacket->prEDExtra),
                          excVIRUS_FOUND )
                    ) ? AVACTION_FIRST : AVACTION_LAST;

        while ( uAVAction != AVACTION_LAST )
            {
                                        // Override AVAPI's DIR2 protection.

            OverrideDIR2Protection ( hEventPacket, hVLVirus );

                                        // Get an action to take.

            uAVAction = UIFileInfected ( hEventPacket,
                                         hVLVirus,
                                         uAVAction,
                                         bActionCompleted,
                                         &rAVActionData );

                                        // Assume the action will complete.

            bActionCompleted = TRUE;

                                        // Take the action.

            switch ( uAVAction )
                {
                case AVACTION_STOP:

                                        // If stop action was selected, and
                                        // we have found a virus on create,
                                        // and we need to delete the newly
                                        // created file, then do so.

                    if ( hEventPacket->prEDExtra->bScanOnClose &&
                         prAPCfg(hEventPacket)->bDeleteInfectedOnCreate )
                        {
                        VLDeleteFile ( hVLVirus );
                        }

                    EventPacketSetResult ( hEventPacket, FALSE );

                    break;

                case AVACTION_CONTINUE:

                    EventPacketSetResult ( hEventPacket, TRUE );

                    break;

                case AVACTION_REPAIR:

                                        // First, create a backup if
                                        // required.

                    BackupFileBeforeRepair ( hEventPacket );

                                        // Then repair file.

                    if ( RepairInfectedFile ( hEventPacket, &hVLVirus ) )
                        {
                        ProcessBackupFileAfterRepair ( hEventPacket, TRUE );

                        EventPacketSetResult ( hEventPacket, TRUE );
                        }
                    else
                        {
                        bActionCompleted = FALSE;

                        ProcessBackupFileAfterRepair ( hEventPacket, FALSE );
                        }

                    break;

                case AVACTION_DELETE:

                    if ( VLDeleteFile ( hVLVirus ) == VS_OK )
                        {
                        EventPacketSetResult ( hEventPacket, FALSE );
                        }
                    else
                        {
                        bActionCompleted = FALSE;
                        }

                    break;

                case AVACTION_EXCLUDE:

                    if ( NAVAPExcludeFile (
                             hEventPacket,
                             EDXpszOEMFile(hEventPacket->prEDExtra),
                             excVIRUS_FOUND ) )
                        {
                        EventPacketSetResult ( hEventPacket, TRUE );
                        }
                    else
                        {
                        bActionCompleted = FALSE;
                        }

                    break;

                case AVACTION_QUARANTINE:

                    if ( QuarantineInfectedFile ( hEventPacket, &hVLVirus ) )
                        {
                        EventPacketSetResult ( hEventPacket, FALSE );
                        }
                    else
                        {
                        bActionCompleted = FALSE;
                        }

                    break;

                case AVACTION_MOVE:

                    if ( MoveInfectedFile ( hEventPacket, &hVLVirus ) )
                        {
                        EventPacketSetResult ( hEventPacket, FALSE );
                        }
                    else
                        {
                        bActionCompleted = FALSE;
                        }

                    break;

                case AVACTION_RENAME:

                    if ( RenameInfectedFile ( hEventPacket, &hVLVirus ) )
                        {
                        EventPacketSetResult ( hEventPacket, FALSE );
                        }
                    else
                        {
                        bActionCompleted = FALSE;
                        }

                    break;

                case AVACTION_SHUTDOWN:

                    EventPacketSetResult ( hEventPacket, FALSE );

                    InitiateShutdown();

                    break;

                case AVACTION_FIRST:
                case AVACTION_LAST:

                    break;

                default:

                    SYM_ASSERT ( FALSE );

                    bActionCompleted = FALSE;
                }

                                        // No need to process more tasks.

            bResult = FALSE;
            }

                                        // Cleanup if action completed.

        if ( hVLVirus )
            {
            if ( VLReleaseVirusHandle ( hVLVirus ) == VS_OK  )
                {
                hVLVirus = NULL;
                }
            }
        else
            {
                                        // Once we have determined that a
                                        // file is clean, add it to the clean
                                        // file cache.  Ignore the error code
                                        // returned by this function, because
                                        // there are many legitimate reasons
                                        // for it to fail.  For example, if
                                        // we are under the context of a
                                        // close event generated on a newly
                                        // created file, this function will
                                        // fail because it will not be able
                                        // to acquire the file ID.

            AddFileToCleanCache ( hEventPacket->prEDExtra->prSEEventData );
            }

        SYM_ASSERT ( !hVLVirus );
        }

                                        // If we have detected infection,
                                        // then clear the fast and dirty
                                        // cache.

    if ( !bResult )
        FastAndDirtyCleanReset();

    return ( bResult );
    }

//---------------------------------------------------------------------------
//
// VOID OverrideDIR2Protection (
//      HEVENTPACKET hEventPacket,
//      HVLVIRUS     hVLVirus
//      );
//
// This function overrides the DIR2 repair and delete protections if the
// current event allows repair/delete of this virus.
//
// Entry:
//      hEventPacket - Event Packet handle
//      hVLVirus     - Virus handle
//
// Exit:
//      -
//
// Note:
//      You can safely pass a NULL for hVLVirus.
//
//---------------------------------------------------------------------------
VOID OverrideDIR2Protection (
    HEVENTPACKET hEventPacket,
    HVLVIRUS     hVLVirus
    )
    {
                                        // Validate input parameters.

    SYM_ASSERT ( hEventPacket );
    SYM_ASSERT ( hEventPacket->prEDExtra );

                                        // If the scanning was happening
                                        // during file close (which is,
                                        // after create), then we can
                                        // override the DIR2 protections.

    if ( hVLVirus && hEventPacket->prEDExtra->bScanOnClose )
        {
        VLOverrideDIR2RepairProtection ( hVLVirus );
        VLOverrideDIR2DeleteProtection ( hVLVirus );
        }
    }

//---------------------------------------------------------------------------
//
// BOOL TaskScanBootRecord (
//      HEVENTPACKET hEventPacket
//      );
//
// This function services the TASK_SCAN_BOOT task.
//
// Entry:
//      hEventPacket - Event Packet handle
//
// Exit:
//      always TRUE
//
//---------------------------------------------------------------------------
BOOL TaskScanBootRecord (
    HEVENTPACKET hEventPacket
    )
    {
#if defined(SYM_NTK)

    auto NAVAP_SCANMOUNTEDDEVICEPACKET rPacket;
    auto SYSTEMTIME                    rSystemTime;

                                        // Validate input parameters.

    SYM_ASSERT ( hEventPacket );
    SYM_ASSERT ( hEventPacket->prEDExtra );
    SYM_ASSERT ( !hEventPacket->prEDExtra->bMBRAccess );

                                        // Build a scan notification packet.

    memset ( &rPacket, 0, sizeof(rPacket) );

    GetSystemTime ( &rSystemTime );

    rPacket.rSystemTime.wYear         = rSystemTime.wYear;
    rPacket.rSystemTime.wMonth        = rSystemTime.wMonth;
    rPacket.rSystemTime.wDayOfWeek    = rSystemTime.wDayOfWeek;
    rPacket.rSystemTime.wDay          = rSystemTime.wDay;
    rPacket.rSystemTime.wHour         = rSystemTime.wHour;
    rPacket.rSystemTime.wMinute       = rSystemTime.wMinute;
    rPacket.rSystemTime.wSecond       = rSystemTime.wSecond;
    rPacket.rSystemTime.wMilliseconds = rSystemTime.wMilliseconds;

    rPacket.szDevice[0] = hEventPacket->prEDExtra->chDeviceID;
    rPacket.szDevice[1] = ':';

    UIGetUserInfo ( NULL, &rPacket.rUser );

                                        // Send packet.

    if ( DriverCommAsyncSend ( NAVAP_COMM_SCANMOUNTEDDEVICE,
                               &rPacket,
                               sizeof ( rPacket ) ) != ERROR_DRVCOMM_SUCCESS )
        {
        SYM_ASSERT ( FALSE );
        }

    return ( TRUE );

#else

    auto BOOL               bResult;
    auto VSTATUS            vStatus;
    auto HVLVIRUS           hVLVirus;
    auto AVACTION           uAVAction;
    auto AVACTIONDATA       rAVActionData;
    auto BOOL               bActionCompleted = FALSE;
    auto PEDEXTRA           prEDExtra;
    auto MESSAGE_BOX_COOKIE rCBMsgBox;

                                        // Validate input parameters.

    SYM_ASSERT ( hEventPacket );
    SYM_ASSERT ( hEventPacket->prEDExtra );
    SYM_ASSERT ( hEventPacket->prEDExtra->hVGlobalContext );
    SYM_ASSERT ( !hEventPacket->prEDExtra->bMBRAccess );

                                        // Initialize local variables.

    bResult = TRUE;

    prEDExtra = hEventPacket->prEDExtra;

    hVLVirus = NULL;

                                        // Make sure that we get the correct
                                        // string for device ID.  This is
                                        // so that we could reuse the same
                                        // EDEXTRA when scanning more than
                                        // one device.

    EDXInvalidatepszOEMDevice ( prEDExtra );

                                        // Scan boot record.

    UIInfoScanningBoot ( hEventPacket, TRUE );

    vStatus = VLScanBoot ( hEventPacket->prEDExtra->hVGlobalContext,
                           prEDExtra->uPhysicalDevice,
                           prEDExtra->uSectorLocation,
                           NULL, &hVLVirus );

    UIInfoScanningBoot ( hEventPacket, FALSE );

                                        // Assume the item is not infected.

    prEDExtra->bItemIsInfected = FALSE;

    if ( vStatus == VS_OK )
        {
                                        // If scanning ok then check if
                                        // virus found, otherwise return to
                                        // caller.

        uAVAction = hVLVirus ? AVACTION_FIRST : AVACTION_LAST;

        while ( uAVAction != AVACTION_LAST )
            {
                                        // Get an action to take.

            uAVAction = UIBootInfected ( hEventPacket,
                                         hVLVirus,
                                         uAVAction,
                                         bActionCompleted,
                                         &rAVActionData );

                                        // Assume the action will complete.

            bActionCompleted = TRUE;

                                        // Take the action.
            switch ( uAVAction )
                {
                case AVACTION_CONTINUE:

                    prEDExtra->bItemIsInfected = TRUE;

                    EventPacketSetResult ( hEventPacket, TRUE );

                    break;

                case AVACTION_REPAIR:

                                        // Then repair file.

                    rCBMsgBox.bUserInterventionAllowed =
                        hEventPacket->prEDExtra->bUserIntervention;

                    rCBMsgBox.uTimeOut = prAPCfg(prEDExtra)->uAlertTimeOut;

                    if ( VLRepairBootPart ( hVLVirus, &rCBMsgBox ) == VS_OK )
                        {
                        EventPacketSetResult ( hEventPacket, TRUE );
                        }
                    else
                        {
                        bActionCompleted = FALSE;
                        }

                    break;

                case AVACTION_SHUTDOWN:

                    prEDExtra->bItemIsInfected = TRUE;

                    EventPacketSetResult ( hEventPacket, FALSE );

                    InitiateShutdown();

                    break;

                case AVACTION_FIRST:
                case AVACTION_LAST:

                    break;

                default:

                    SYM_ASSERT ( FALSE );

                    bActionCompleted = FALSE;
                }

                                        // No need to process more tasks.

            bResult = FALSE;
            }

                                        // Cleanup if action completed.

        if ( hVLVirus && VLReleaseVirusHandle ( hVLVirus ) == VS_OK )
            {
            hVLVirus = NULL;
            }

        SYM_ASSERT ( !hVLVirus );
        }

    return ( bResult );

#endif
    }

//---------------------------------------------------------------------------
//
// BOOL TaskScanDevices (
//      HEVENTPACKET hEventPacket
//      );
//
// This function services the TASK_SCAN_DEVICES task, which is posted
// during a system reboot (CTRL-ALT-DEL) or system shutdown.
//
// Entry:
//      hEventPacket - Event Packet handle
//
// Exit:
//      always TRUE
//
//---------------------------------------------------------------------------
BOOL TaskScanDevices (
    HEVENTPACKET hEventPacket
    )
    {
    auto PEDEXTRA       prEDExtra;
    auto PDEVICEINFOREC prDeviceInfo, prFirstRemovableDevice;

                                        // Validate input parameters.

    SYM_ASSERT ( hEventPacket );
    SYM_ASSERT ( hEventPacket->prEDExtra );

                                        // Initialize local variables.

    prEDExtra = hEventPacket->prEDExtra;

                                        // Make sure that we display message
                                        // for each device being scanned.

    prEDExtra->bDisplayInfo = TRUE;

                                        // Call TaskScanBootRecord() for
                                        // each removable device on the
                                        // system, unless the configuration
                                        // wants the scan to be done only
                                        // on the bootable device.

    prDeviceInfo = prFirstRemovableDevice = NULL;

    while ( prDeviceInfo = GetNextRemovableDevice ( prDeviceInfo ) )
        {
                                        // Remember the first removable
                                        // device on the list.

        if ( !prFirstRemovableDevice ||
             ( prDeviceInfo->uPhysicalDevice <
               prFirstRemovableDevice->uPhysicalDevice )
           )
            {
            prFirstRemovableDevice = prDeviceInfo;
            }

                                        // Initialize the information
                                        // in the EDEXTRA structure, which
                                        // would be needed when calling
                                        // TaskScanBootRecord().

        prEDExtra->uPhysicalDevice = prDeviceInfo->uPhysicalDevice;
        prEDExtra->uSectorLocation = 0;
        prEDExtra->bMBRAccess      = FALSE;
        prEDExtra->chDeviceID      = prDeviceInfo->chLogicalID;

                                        // If all devices should be scanned,
                                        // then scan now...

        if ( prAPCfg(prEDExtra)->bScanNonBootDevicesUponBoot )
            {
            do
                {
                TaskScanBootRecord ( hEventPacket );
                } while ( hEventPacket->prEDExtra->bItemIsInfected );
            }
        }

                                        // If only the bootable device
                                        // should be scanned, then scan it
                                        // now...

                                        // The bootable device which we will
                                        // scan is the one with the lowest
                                        // uPhysicalDeviceID.  If one was
                                        // found, then go for it.

    if ( !(prAPCfg(prEDExtra)->bScanNonBootDevicesUponBoot) &&
         prFirstRemovableDevice )
        {
                                        // Initialize the information
                                        // in the EDEXTRA structure, which
                                        // would be needed, and scan device.

        prEDExtra->uPhysicalDevice = prFirstRemovableDevice->uPhysicalDevice;
        prEDExtra->uSectorLocation = 0;
        prEDExtra->bMBRAccess      = FALSE;
        prEDExtra->chDeviceID      = prFirstRemovableDevice->chLogicalID;

        do
            {
            TaskScanBootRecord ( hEventPacket );
            } while ( hEventPacket->prEDExtra->bItemIsInfected );
        }

#if defined(SYM_VXD)
                                        // Give all devices some time to
                                        // stop working.  This is to solve
                                        // a problem where, after shutting
                                        // down Windows 95, or exiting to
                                        // DOS, the floppies don't stop
                                        // spinning.  This is because
                                        // Windows switches to real mode
                                        // before the PM timeout functions
                                        // for stopping the motors get
                                        // executed.

    SleepDead ( rNAVAPInternalCfg.dwDeviceShutdownDelayTime );

#endif // #if defined(SYM_VXD)

                                        // Return to caller.

    return ( TRUE );
    }

#define DRIVER_LOCKED_CODE_SEGMENT
#define DRIVER_LOCKED_DATA_SEGMENT
#include "drvseg.h"

#if defined(SYM_VXD)

static HTIMEOUT hFastCacheTimeout = NULL;

VOID __cdecl VXDCacheTimeout ( VOID )
    {
    hFastCacheTimeout = NULL;
    }

BOOL SetCacheTimeout ( VOID )
    {
    SYM_ASSERT ( !hFastCacheTimeout );

    hFastCacheTimeout = Set_Global_Time_Out ( VXDCacheTimeout,
                                              rNAVAPInternalCfg.dwFastCacheResetTimeout,
                                              NULL );

    return hFastCacheTimeout;
    }

BOOL CancelCacheTimeout ( VOID )
    {
    if ( hFastCacheTimeout )
        {
        Cancel_Time_Out ( hFastCacheTimeout );

        hFastCacheTimeout = NULL;

        return TRUE;
        }

    return FALSE;
    }

#elif defined(SYM_NTK)

static BOOL          bCacheTimerInitialized = FALSE;
static KTIMER        kCacheTimer = {0};
static LARGE_INTEGER iLarge0 = {0};

BOOL SetCacheTimeout ( VOID )
    {
    auto LARGE_INTEGER iTimeout;

    SYM_ASSERT ( !bCacheTimerInitialized );

    KeInitializeTimer ( &kCacheTimer );

    iTimeout.QuadPart = - (__int64)rNAVAPInternalCfg.dwFastCacheResetTimeout * 1000 * 1000 / 100;

    KeSetTimer ( &kCacheTimer, iTimeout, NULL );

    return ( bCacheTimerInitialized = TRUE );
    }

BOOL CancelCacheTimeout ( VOID )
    {
    if ( bCacheTimerInitialized )
        {
        auto BOOL bResult;

        switch ( KeWaitForSingleObject ( &kCacheTimer, Executive, KernelMode, FALSE, &iLarge0 ) )
            {
            case STATUS_SUCCESS:
                bResult = FALSE;
                break;
            case STATUS_TIMEOUT:
                bResult = TRUE;
                break;
            default:
                bResult = TRUE;
                SYM_ASSERT ( FALSE );
            }

        KeCancelTimer ( &kCacheTimer );

        bCacheTimerInitialized = FALSE;

        return bResult;
        }

    return FALSE;
    }

#endif

TCHAR szFastAndDirtyClean[SYM_MAX_PATH] = {0};

BOOL FastAndDirtyCleanCheck ( PTSTR pszFile )
    {
    if ( rNAVAPInternalCfg.dwFastCacheResetTimeout )
        {
        auto BOOL  bResult;
        auto int   iLen;
        auto TCHAR szFastAndDirtyCleanCopy[SYM_MAX_PATH];

        iLen = STRLEN ( pszFile );

        SymAcquireFastGlobalLock();

        if ( CancelCacheTimeout() )
            {
            STRCPY ( szFastAndDirtyCleanCopy, szFastAndDirtyClean );

            SymReleaseFastGlobalLock();

            bResult = ( STRICMP ( szFastAndDirtyCleanCopy, pszFile ) == 0 );

            SymAcquireFastGlobalLock();
            }
        else
            bResult = FALSE;

        if ( iLen < SYM_MAX_PATH )
            {
            STRCPY ( szFastAndDirtyClean, pszFile );

            if ( !SetCacheTimeout() )
                UIOutOfMemoryError();
            }

        SymReleaseFastGlobalLock();

        return bResult;
        }

    return FALSE;
    }

VOID FastAndDirtyCleanReset ( VOID )
    {
    if ( rNAVAPInternalCfg.dwFastCacheResetTimeout )
        {
        SymAcquireFastGlobalLock();

        CancelCacheTimeout();

        SymReleaseFastGlobalLock();
        }
    }

VOID InitFastCleanCache ( VOID )
    {
    }

VOID DeInitFastCleanCache ( VOID )
    {
    if ( rNAVAPInternalCfg.dwFastCacheResetTimeout )
        {
        SymAcquireFastGlobalLock();

        CancelCacheTimeout();

        SymReleaseFastGlobalLock();
        }
    }

#define DRIVER_DEFAULT_CODE_SEGMENT
#define DRIVER_DEFAULT_DATA_SEGMENT
#include "drvseg.h"
