// Copyright 1994 - 1998 Symantec, Peter Norton Product Group
//***************************************************************************
//
// $Header:   S:/APKERNEL/VCS/KRNLAPI.CPv   1.3   18 Feb 1998 21:13:06   RStanev  $
//
// Description:
//      This file defines the main Kernel functions.
//
// Contains:
//      KernelNotify()
//
// See Also:
//      taskscan.cpp, taskvl.cpp
//
// Note:
//      Kernel module
//
//***************************************************************************
// $Log:   S:/APKERNEL/VCS/KRNLAPI.CPv  $
// 
//    Rev 1.3   18 Feb 1998 21:13:06   RStanev
// Removed unknown virus detection.
//
//    Rev 1.2   17 Feb 1998 20:25:10   RStanev
// Updated copyright.
//
//    Rev 1.1   15 May 1997 01:55:48   RStanev
// Minor changes.
//
//    Rev 1.0   26 Feb 1997 16:39:44   RSTANEV
// Initial revision.
//
//***************************************************************************

#include "platform.h"
#include "navap.h"
#include "apkernel.h"

//---------------------------------------------------------------------------
//
// VOID KernelNotify (
//      HEVENTPACKET hEventPacket
//      );
//
// This function sets up the Kernel to process an Event Packet.
//
// Entry:
//      hEventPacket - Event Packet handle
//
// Exit:
//      -
//
// Note:
//      Currently this function does all the work the Kernel is
//      supposed to do.  It reads tasks from the task queue and dispatches
//      them to the appropriate servicing function.  When and how this
//      function returns to the caller is determined by the context of the
//      current event and the status of the executed tasks.
//
//---------------------------------------------------------------------------
VOID KernelNotify (
    HEVENTPACKET hEventPacket
    )
    {
    auto TASK eTask;

                                        // Validate input parameters.

    SYM_ASSERT ( hEventPacket );

                                        // Dispatch tasks in the task queue.

    while ( EventPacketGetTask ( hEventPacket, &eTask ) )
        {
        switch ( eTask )
            {
            case TASK_SCAN_FILE:

                TaskScanFile ( hEventPacket );

                break;

            case TASK_SCAN_BOOT:

                TaskScanBootRecord ( hEventPacket );

                break;

#if defined(SYM_VXD)

            case TASK_SCAN_DEVICES:

                TaskScanDevices ( hEventPacket );

                break;

            case TASK_NOTIFY_FILEATTR_CHANGE:

                TaskNotifyFileAttrChange ( hEventPacket );

                break;

            case TASK_NOTIFY_FILE_WRITE:

                TaskNotifyFileWrite ( hEventPacket );

                break;

            case TASK_NOTIFY_HDFORMAT:

                TaskNotifyHDFormat ( hEventPacket );

                break;

            case TASK_NOTIFY_HDMBR_WRITE:

                TaskNotifyHDMBRWrite ( hEventPacket );

                break;

            case TASK_NOTIFY_HDBOOT_WRITE:

                TaskNotifyHDBootWrite ( hEventPacket );

                break;

            case TASK_NOTIFY_FDBOOT_WRITE:

                TaskNotifyFDBootWrite ( hEventPacket );

                break;

#endif // #if defined(SYM_VXD)

            default:

                SYM_ASSERT ( FALSE );

            }
        }
    }
