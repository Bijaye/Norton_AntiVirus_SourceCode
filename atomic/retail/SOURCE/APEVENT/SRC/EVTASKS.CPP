// Copyright 1994 - 1998 Symantec, Peter Norton Product Group
//***************************************************************************
//
// $Header:   S:/APEVENT/VCS/EVTASKS.CPv   1.11   08 Dec 1998 19:54:32   RStanev  $
//
// Description:
//      This file defines functions for manipulating the task queue
//      in the Event Packets.
//
// Contains:
//      EventPacketInitTaskQueue()
//      EventPacketPostTask()       (local function)
//      EventPacketGetTask()
//      IsFileLength0Fast()         (local inline function)
//      GetFileID()                 (local inline function)
//
// See Also:
//      apevent.h, edextra.cpp, evpacket.cpp
//
// Note:
//      Event Manager module
//
//***************************************************************************
// $Log:   S:/APEVENT/VCS/EVTASKS.CPv  $
// 
//    Rev 1.11   08 Dec 1998 19:54:32   RStanev
// Added support for SE_DEV_READ in NT.
//
//    Rev 1.10   02 Jun 1998 01:09:58   RStanev
// Merged Roswell fixes.
//
//    Rev 1.9   16 Mar 1998 14:18:04   DALLEE
// Match changes to NAVAPCFG struct: has ordered arrays of action responses.
//
//    Rev 1.8   18 Feb 1998 22:16:56   RStanev
// Removed unknown virus detection.
//
//    Rev 1.7   17 Feb 1998 16:55:44   RStanev
// Updated copyright.
//
//    Rev 1.6   02 Jul 1997 19:05:00   RStanev
// Removed the changes from the last revision.
//
//    Rev 1.5   03 Jun 1997 09:36:42   RStanev
// IsThisMoveOperation() now determines whether we should scan the file
// during SE_FS_OBJECT_RENAME.
//
//    Rev 1.4   16 May 1997 05:50:42   RStanev
// Virus sensor and scan for known viruses on create/modify are now
// synchronized much better.
//
//    Rev 1.3   16 May 1997 02:54:44   RStanev
// Oops...  That virus sensor code had a typo in it.
//
//    Rev 1.2   16 May 1997 01:59:12   RStanev
// Enabled virus sensor.
//
//    Rev 1.1   15 May 1997 01:27:54   RStanev
// Too many changes to describe.
//
//    Rev 1.0   26 Feb 1997 04:34:18   RSTANEV
// Initial revision.
//
//***************************************************************************

#include "platform.h"
#include "navap.h"
#include "apevent.h"
#include "monitor.h"
#include "filter.h"

//---------------------------------------------------------------------------
//
// bRebootEventProcessed is a public variable controlling now the reboot
// event is handled by this module.  If this variable is FALSE, then the
// next reboot event given to EventPacketInitTaskQueue() will force the
// event to be processed.  If this variable is set to TRUE, which happens
// as soon as EventPacketInitTaskQueue() gets a reboot event, this function
// will disregard any reboot events.
//
//---------------------------------------------------------------------------
BOOL bRebootEventProcessed = FALSE;

//---------------------------------------------------------------------------
//
// Define local function prototypes
//
//---------------------------------------------------------------------------
VOID EventPacketPostTask (
    HEVENTPACKET prEventPacket,
    TASK         eTask
    );

//---------------------------------------------------------------------------
//
// Define inline functions
//
//---------------------------------------------------------------------------
inline BOOL IsFileLength0Fast (
    LPED  prEventData,
    PUINT puCache
    )
    {
                                        // If cache variable is not yet set,
                                        // then get the file length.

    SYM_ASSERT ( TRUE != -1 );

    if ( *puCache == -1 )
        {
        auto HFILE hFile;

        hFile = FileOpen ( prEventData->SE_ED_lpPathSpec,
                           OF_READ | OF_SHARE_DENY_NONE );

        if ( hFile != HFILE_ERROR )
            {
            *puCache = FileLength ( hFile ) == 0 ? TRUE : FALSE;

            FileClose ( hFile );
            }
        else
            {
            *puCache = FALSE;
            }
        }

                                        // Return to caller.

    return ( (BOOL) *puCache );
    }

inline BOOL GetFileID (
    PTSTR   pszFileName,
    __int64 *pi64FileID
    )
    {
#if defined(SYM_NTK)

    auto BOOL                       bResult;
    auto HFILE                      hFile;
    auto NTSTATUS                   Status;
    auto FILE_FS_DEVICE_INFORMATION DevInfo;
    auto FILE_INTERNAL_INFORMATION  FileInfo;
    auto IO_STATUS_BLOCK            IoStatus;

                                        // Validate input parameters.

    SYM_ASSERT ( pszFileName );
    SYM_VERIFY_STRING ( pszFileName );
    SYM_ASSERT ( pi64FileID );

                                        // Open the file in a mode to query
                                        // its file ID.

    hFile = FileOpen ( pszFileName, OF_SHARE_DENY_NONE );

    if ( hFile != HFILE_ERROR )
        {
                                        // Filter out devices that we do not
                                        // want to get unique file IDs from.

        Status = ZvQueryVolumeInformationFile ( (HANDLE)hFile,
                                                &IoStatus,
                                                &DevInfo,
                                                sizeof(DevInfo),
                                                FileFsDeviceInformation );

        if ( NT_SUCCESS(Status) &&
             IoStatus.Information == sizeof(DevInfo) &&
             !( DevInfo.Characteristics & rNAVAPInternalCfg.ulFileCacheFilterFlags )
           )
            {
                                        // Query the file ID.

            Status = ZvQueryInformationFile ( (HANDLE)hFile,
                                              &IoStatus,
                                              &FileInfo,
                                              sizeof(FileInfo),
                                              FileInternalInformation );

                                        // If successful, then save it at
                                        // the return buffer.

            if ( NT_SUCCESS(Status) )
                {
                bResult = ( *pi64FileID = FileInfo.IndexNumber.QuadPart ) ?
                          TRUE : FALSE;
                }
            else
                {
                bResult = FALSE;
                }
            }
        else
            {
            bResult = FALSE;
            }

        FileClose ( hFile );
        }
    else
        {
        bResult = FALSE;
        }

    return ( bResult );

#else

    return ( FALSE );

#endif
    }

//---------------------------------------------------------------------------
//
// BOOL EventPacketInitTaskQueue (
//      HEVENTPACKET prEventPacket
//      );
//
// This function examines the event at prEventPacket->prEDExtra and posts
// appropriate tasks to the task queue of the event packet.
//
// Entry:
//      prEventPacket - a pointer (handle) to an Event Packet.
//
// Exit:
//      TRUE  if function completed successfully.
//      FALSE if error
//
//---------------------------------------------------------------------------
BOOL EventPacketInitTaskQueue (
    HEVENTPACKET prEventPacket
    )
    {
    auto PEDEXTRA  prEDExtra;
    auto PNAVAPCFG prCfg;
#if defined(SYM_VXD)
    auto UINT      uFileAttributes;
#endif // #if defined(SYM_VXD)
    auto UINT      uFileMonitorAttributes;
    auto UINT      uIsFileLength0Cache;
    auto __int64   i64FileID;

                                        // Validate input parameters.

    SYM_ASSERT ( prEventPacket );
    SYM_ASSERT ( prEventPacket->prEDExtra );

                                        // Initialize local data for faster
                                        // processing.

    prEDExtra = prEventPacket->prEDExtra;
    prCfg     = prAPCfg ( prEDExtra );

                                        // Post tasks.

    switch ( prEDExtra->uEventType )
        {

#if defined(SYM_VXD)

        case SE_LDR_APPLICATION_EXECUTE:

                                        // Post TASK_SCAN_FILE if we are
                                        // configured to scan on execute,

            if ( prCfg->bScanFileOnExecute &&

                                        // ... and we are either scanning all
                                        // files, or the file that is being
                                        // executed matches an extension in
                                        // our list of executable extensions,

                 ( prCfg->bScanAllFiles ||
                   EDXbFileIsExecutable ( prEDExtra ) ) &&

                                        // ... and we have locked an AVAPI
                                        // context.

                 prEDExtra->hVGlobalContext
               )
                {
                EventPacketPostTask ( prEventPacket, TASK_SCAN_FILE );
                }

            break;

#endif // #if defined(SYM_VXD)

        case SE_FS_OBJECT_RENAME:

                                        // Post TASK_SCAN_FILE if we are
                                        // configured to scan on either open
                                        // or create,

            if ( ( prCfg->bScanFileOnOpen ||
                   prCfg->bScanFileOnCreate ) &&

                                        // ... and we are either scanning all
                                        // files,

                 ( prCfg->bScanAllFiles ||

                                        // ... or the source or the target
                                        // filename matches an extension in
                                        // the list of executable extensions,

                   EDXbFileIsExecutable ( prEDExtra ) ||
                   EDXbAltFileIsExecutable ( prEDExtra ) ) &&

                                        // ... and the source filename is
                                        // not in the cache with clean files,

                 ( !GetFileID ( prEDExtra->pszFile, &i64FileID ) ||
                   !IsFileInCleanCache ( i64FileID ) ) &&

                                        // ... and we have locked an AVAPI
                                        // context.

                 prEDExtra->hVGlobalContext
               )
                {
                EventPacketPostTask ( prEventPacket, TASK_SCAN_FILE );
                }

            break;

        case SE_FS_FILE_OPEN:

#if defined(SYM_NTK)

                                        // Save the file ID in the open-file
                                        // data area so that it can be
                                        // removed from the cache as soon as
                                        // we see a write to that file.

            if ( GetFileID ( prEDExtra->pszFile, &i64FileID ) )
                {
                SYM_ASSERT ( i64FileID );

                FileMonitorSetFileID ( prEDExtra->prSEEventData, i64FileID );
                }

#endif // #if defined(SYM_NTK)

                                        // Post TASK_SCAN_FILE if we are
                                        // configured to scan files on open,

            if ( prCfg->bScanFileOnOpen &&

                                        // ... and we are either scanning all
                                        // files, or the current file is with
                                        // a program file extension,

                 ( prCfg->bScanAllFiles ||
                   EDXbFileIsExecutable ( prEDExtra ) ) &&

                                        // ... and the file has not been
                                        // added to the clean-file cache,

                 !IsFileInCleanCache ( prEDExtra->prSEEventData ) &&

                                        // ... and we have locked AVAPI
                                        // handle.

                 prEDExtra->hVGlobalContext
               )
                {
                EventPacketPostTask ( prEventPacket, TASK_SCAN_FILE );
                }

                                        // Check filters for doing anything
                                        // with the file upon close.

            uFileMonitorAttributes = FILEMONITOR_BABYSITTING;

            uIsFileLength0Cache = (UINT)-1;

                                        // Schedule a scan on close if we are
                                        // configured to scan on create or
                                        // modify,

            if ( prCfg->bScanFileOnCreate &&

                                        // ... and if we are supposed to scan
                                        // all files, or if the current
                                        // file's extension matches the list
                                        // of program file extensions.

                 ( prCfg->bScanAllFiles ||
                   EDXbFileIsExecutable ( prEDExtra ) )
               )
                {
                uFileMonitorAttributes |= FILEMONITOR_CHECKONCLOSE;

                                        // During the processing of the close
                                        // event, it would be helpful if we
                                        // knew the initial file length.

                if ( IsFileLength0Fast ( prEDExtra->prSEEventData,
                                         &uIsFileLength0Cache ) )
                    {
                    uFileMonitorAttributes |= FILEMONITOR_SIZE0ATOPEN;
                    }

#if defined(SYM_NTK)

                                        // Save the user SID when a scan on
                                        // close is scheduled.

                FileMonitorSaveCurrentUserSID ( prEDExtra->prSEEventData );

#endif // #if defined(SYM_NTK)

                }

#if defined(SYM_VXD)

                                        // Start monitor for write to program
                                        // files if the configuration tells
                                        // us to do so,

            if ( prCfg->auWriteToProgramFilesActions[0] != MAINACTION_ALLOW &&

                                        // ... and if we have a match of
                                        // file's extension in our program
                                        // file extensions list,

                 EDXbFileIsExecutable ( prEDExtra ) &&

                                        // ... and if the file length is not
                                        // 0 (allow writing to new files).

                 !IsFileLength0Fast ( prEDExtra->prSEEventData,
                                      &uIsFileLength0Cache )
               )
                {
                uFileMonitorAttributes |= FILEMONITOR_CONFIRMWRITE;
                }

#endif // #if defined(SYM_VXD)

                                        // Set the file monitor attributes.

            FileMonitorSetAttributes ( prEventPacket,
                                       uFileMonitorAttributes );

            break;

        case SE_FS_FILE_CREATE:

                                        // Schedule a scan on close if we are
                                        // configured to scan on create or
                                        // modify,

            if ( prCfg->bScanFileOnCreate &&

                                        // ... and if we are supposed to scan
                                        // all files, or if the current
                                        // file's extension matches the list
                                        // of program file extensions.

                 ( prCfg->bScanAllFiles ||
                   EDXbFileIsExecutable ( prEDExtra ) )
               )
                {
                uFileMonitorAttributes = FILEMONITOR_BABYSITTING |
                                         FILEMONITOR_CHECKONCLOSE |

                                        // During the processing of the close
                                        // event, it would be helpful if we
                                        // knew the file was created.

                                         FILEMONITOR_CREATED;

#if defined(SYM_NTK)

                                        // Save the user SID when a scan on
                                        // close is scheduled.

                FileMonitorSaveCurrentUserSID ( prEDExtra->prSEEventData );

#endif // #if defined(SYM_NTK)

                }
            else
                {
                uFileMonitorAttributes = FILEMONITOR_BABYSITTING;
                }

                                        // Set the file monitor attributes.

            FileMonitorSetAttributes ( prEventPacket,
                                       uFileMonitorAttributes );

            break;

        case SE_FS_FILE_CLOSE:

                                        // Get the monitor flags for this
                                        // file.

            if ( FileMonitorGetAttributes ( prEventPacket,
                                            &uFileMonitorAttributes ) )
                {
                SYM_ASSERT ( uFileMonitorAttributes & FILEMONITOR_BABYSITTING );

                                        // If this file was modified, then
                                        // we should remove it from the
                                        // clean file cache.  Note, that if
                                        // we are not enabled, we will not
                                        // remove the file from the cache.
                                        // That's because the cache is
                                        // cleaned up globally during enable/
                                        // disable.

                                        // Note, that the file gets removed
                                        // removed from the clean cache as
                                        // as soon as we see the first write
                                        // write coming in.  But still, due
                                        // to NT's caching, we may get the
                                        // FILEMONITOR_MODIFIED flag set
                                        // without ever seeing a file write
                                        // event!

                if ( bNAVAPIsEnabled &&
                     ( uFileMonitorAttributes & FILEMONITOR_MODIFIED )
                   )
                    {
                    RemoveFileFromCleanCache ( prEDExtra->prSEEventData );
                    }

                                        // Post TASK_SCAN_FILE if we are
                                        // enabled,

                if ( bNAVAPIsEnabled &&

                                        // ... and there was a scan on
                                        // close scheduled,

                     ( uFileMonitorAttributes & FILEMONITOR_CHECKONCLOSE ) &&

                                        // and the file was either just
                                        // created, or 0 bytes in length
                                        // when opened, or modified (SYM_VXD
                                        // only),

#if defined(SYM_VXD)

                     ( uFileMonitorAttributes & ( FILEMONITOR_CREATED |
                                                  FILEMONITOR_SIZE0ATOPEN |
                                                  FILEMONITOR_MODIFIED ) ) &&

#endif // #if defined(SYM_VXD)

                                        // ... and we have locked a valid
                                        // AVAPI handle.

                     prEDExtra->hVGlobalContext
                   )
                    {
                                        // If the file was created (or it was
                                        // empty when opened), then give the
                                        // "deny access" action the special
                                        // privilege of deleting that file.

                    if ( uFileMonitorAttributes & ( FILEMONITOR_CREATED |
                                                    FILEMONITOR_SIZE0ATOPEN )
                       )
                        {
                        prEDExtra->bScanOnClose = TRUE;
                        }

                    EventPacketPostTask ( prEventPacket, TASK_SCAN_FILE );
                    }
                }

            break;

#if defined(SYM_VXD)

        case SE_FS_FILE_WRITE:

                                        // Make sure we mark this file as
                                        // written to, even if write failed
                                        // or was denied access to.

            FileMonitorSetAttributes ( prEventPacket, FILEMONITOR_MODIFIED );

                                        // Get flags which are set to monitor
                                        // this file for.

            FileMonitorGetAttributes ( prEventPacket,
                                       &uFileMonitorAttributes );

                                        // Check filters for notifying that
                                        // a file is being written to.

            if ( uFileMonitorAttributes & FILEMONITOR_CONFIRMWRITE )
                {

                                        // Post TASK_NOTIFY_FILE_WRITE.

                EventPacketPostTask ( prEventPacket, TASK_NOTIFY_FILE_WRITE );
                }

            break;

        case SE_FS_OBJECT_SETATTRIBUTES:

                                        // Check filters for RO attribute
                                        // check, then see if the RO
                                        // attribute is being cleared.
                                        // SYM_VXD: post task only if change
                                        // is done by a V86 mode application.

            if ( prCfg->auROAttributeClearActions[0] != MAINACTION_ALLOW &&
                 Get_Cur_VM_Handle() != Get_Sys_VM_Handle() &&
                 FileGetAttr ( prEDExtra->pszFile, &uFileAttributes ) == NOERR &&
                 (uFileAttributes & FA_READ_ONLY) == FA_READ_ONLY &&
                 (prEDExtra->uFileAttributes & FA_READ_ONLY) != FA_READ_ONLY
               )
                {
                                        // Post TASK_NOTIFY_FILEATTR_CHANGE.

                EventPacketPostTask ( prEventPacket, TASK_NOTIFY_FILEATTR_CHANGE );
                }

            break;

        case SE_DEV_WRITE:

                                        // Check filters for notifying
                                        // of writing to removable device.

            if ( !prEDExtra->bPartitionable &&
                 prCfg->auWriteToFDBootRecordActions[0] != MAINACTION_ALLOW )
                {
                                        // Post TASK_NOTIFY_FDBOOT_WRITE.

                EventPacketPostTask ( prEventPacket, TASK_NOTIFY_FDBOOT_WRITE );
                }

                                        // Check filters for notifying
                                        // of writing to fixed device.

            if ( prEDExtra->bPartitionable &&
                 prCfg->auWriteToHDBootRecordsActions[0] != MAINACTION_ALLOW )
                {
                if ( prEDExtra->bMBRAccess )
                    {
                                        // Post TASK_NOTIFY_HDMBR_WRITE.

                    EventPacketPostTask ( prEventPacket, TASK_NOTIFY_HDMBR_WRITE );
                    }
                else
                    {
                                        // Post TASK_NOTIFY_HDBOOT_WRITE.

                    EventPacketPostTask ( prEventPacket, TASK_NOTIFY_HDBOOT_WRITE );
                    }
                }

            break;

        case SE_DEV_FORMAT:

                                        // Check filters for notifying
                                        // of formatting a fixed device.

            if ( prCfg->auFormatHardDiskActions[0] != MAINACTION_ALLOW )
                {
                                        // Post TASK_NOTIFY_HDFORMAT.

                EventPacketPostTask ( prEventPacket, TASK_NOTIFY_HDFORMAT );
                }

            break;

        case SE_DEV_READ:

                                        // Check filters for scanning device
                                        // upon read of sector 1.

            if ( prCfg->bScanDeviceUponAccess &&
                 !prEDExtra->bPartitionable )
                {
                                        // Post TASK_SCAN_BOOT.

                EventPacketPostTask ( prEventPacket, TASK_SCAN_BOOT );
                }

            break;

        case SE_MSC_REBOOT:

                                        // Check filters for scanning devices
                                        // upon reboot / shutdown.

            if ( prCfg->bScanBootDeviceUponBoot &&
                 !bRebootEventProcessed &&
                 !bNAVAPIsInShutdownMode )
                {
                                        // Once we enter here, we should
                                        // never again process this event,
                                        // unless someone else explicitly
                                        // tells us that it's ok to do so.

                bRebootEventProcessed = TRUE;

                                        // Post TASK_SCAN_DEVICES.

                EventPacketPostTask ( prEventPacket, TASK_SCAN_DEVICES );
                }

            break;

#endif // #if defined(SYM_VXD)

        case SE_DEV_MOUNT:

                                        // Post TASK_SCAN_BOOT if we are
                                        // configured to scan devices upon
                                        // mount.

            if ( prCfg->bScanDeviceUponAccess )
                {
                                        // Post TASK_SCAN_BOOT.

                EventPacketPostTask ( prEventPacket, TASK_SCAN_BOOT );
                }

            break;

        default:

            SYM_ASSERT ( FALSE );

            return ( FALSE );
        }

    return ( TRUE );
    }

//---------------------------------------------------------------------------
//
// VOID EventPacketPostTask (
//      HEVENTPACKET prEventPacket,
//      TASK         eTask
//      );
//
// This function adds a task to the task queue of prEventPacket.
//
// Entry:
//      prEventPacket - a pointer (handle) to an Event Packet.
//      eTask         - task to be posted
//
// Exit:
//      -
//
// Note:
//      There should be a free record in the Task Queue before calling this
//      function.
//
//---------------------------------------------------------------------------
VOID EventPacketPostTask (
    HEVENTPACKET prEventPacket,
    TASK         eTask
    )
    {
                                        // Validate input parameters.

    SYM_ASSERT ( prEventPacket );

                                        // Make sure we have enough space
                                        // in the task queue.

    SYM_ASSERT ( prEventPacket->uNextTask < TASK_QUEUE_SIZE );

                                        // Post task.

    prEventPacket->aeTaskQueue[prEventPacket->uNextTask] = eTask;

    prEventPacket->uNextTask++;
    }

//---------------------------------------------------------------------------
//
// BOOL EventPacketGetTask (
//      HEVENTPACKET prEventPacket,
//      PTASK        peTask
//      );
//
// This function retrieves a task from the task queue of prEventPacket.
//
// Entry:
//      prEventPacket - a pointer (handle) to an Event Packet.
//      peTask        - pointer to a task enumarator to store the result
//
// Exit:
//      TRUE  if task retrieved successfully
//      FALSE if no more tasks
//
//---------------------------------------------------------------------------
BOOL EventPacketGetTask (
    HEVENTPACKET prEventPacket,
    PTASK        peTask
    )
    {
                                        // Validate input parameters.

    SYM_ASSERT ( prEventPacket );
    SYM_ASSERT ( peTask );

                                        // Get task.

    if ( prEventPacket->uCurrentTask < prEventPacket->uNextTask )
        {
        *peTask =
            prEventPacket->aeTaskQueue[prEventPacket->uCurrentTask];

        prEventPacket->uCurrentTask++;

        return ( TRUE );
        }

    return ( FALSE );
    }
