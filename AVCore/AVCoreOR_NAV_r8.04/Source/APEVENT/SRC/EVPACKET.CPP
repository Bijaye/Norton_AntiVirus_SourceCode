// Copyright 1994 - 1998 Symantec, Peter Norton Product Group
//***************************************************************************
//
// $Header:   S:/APEVENT/VCS/EVPACKET.CPv   1.2   17 Feb 1998 16:55:34   RStanev  $
//
// Description:
//      This file defines functions for creating, destroying and working
//      with event packets.
//
// Contains:
//      EventPacketCreate()
//      EventPacketDestroy()
//      EventPacketProcess()
//      EventPacketGetResult()
//      EventPacketSetResult()
//      GetDefaultEventResult()
//
// See Also:
//      apevent.h, edextra.cpp, evtasks.cpp
//
// Note:
//      Event Manager module
//
//***************************************************************************
// $Log:   S:/APEVENT/VCS/EVPACKET.CPv  $
// 
//    Rev 1.2   17 Feb 1998 16:55:34   RStanev
// Updated copyright.
//
//    Rev 1.1   15 May 1997 01:12:36   RStanev
// Minor cleanup work.
//
//    Rev 1.0   26 Feb 1997 04:34:16   RSTANEV
// Initial revision.
//
//***************************************************************************

#include "platform.h"
#include "navap.h"
#include "apevent.h"

//---------------------------------------------------------------------------
//
// BOOL EventPacketCreate (
//      PHEVENTPACKET phEventPacket,
//      PEDEXTRA      prEDExtra
//      );
//
// This function allocates memory for an event packet, initializes
// the EVENTPACKET structure and calls a function which posts the
// appropriate tasks to the task queue.
//
// Entry:
//      phEventPacket - pointer to a variable of type HEVENTPACKET where
//                      the event packet handle should be stored.
//      prEDExtra     - pointer to a structure with information about the
//                      event and the task which generated it.
//
// Exit:
//      TRUE  if event packet created
//          *phEventPacket contains a handle to the packet.  After this
//          handle is created, the *prEDExtra structure should not be
//          destroyed.  *prEDExtra can be released only after the event
//          packet handle has been destroyed.
//      FALSE if error
//
//---------------------------------------------------------------------------
BOOL EventPacketCreate (
    PHEVENTPACKET phEventPacket,
    PEDEXTRA      prEDExtra
    )
    {
    auto PEVENTPACKET prLocalEP;

                                        // Validate input parameters.

    SYM_ASSERT ( phEventPacket );
    SYM_ASSERT ( prEDExtra );

                                        // Allocate memory for EVENTPACKET
                                        // structure.

    prLocalEP = (PEVENTPACKET) MemAllocPtr ( GHND, sizeof(EVENTPACKET) );

                                        // Fatal error if allocation failed.

    if ( !prLocalEP )
        {
        UIOutOfMemoryError();
        return ( FALSE );
        }

                                        // Initialize the Event Packet.

                                        // Note: uCurrentTask and uNextTask
                                        //       should be set to 0, but
                                        //       since MemAllocPtr cleared
                                        //       the memory, there's no need
                                        //       of the extra overhead.

    prLocalEP->prEDExtra       = prEDExtra;
    prLocalEP->bCallbackResult = TRUE;

                                        // Post appropriate tasks.

    if ( !EventPacketInitTaskQueue ( prLocalEP ) )
        {
        SYM_ASSERT ( FALSE );

        MemFreePtr ( prLocalEP );

        return ( FALSE );
        }

                                        // Set the handle (use the pointer
                                        // to EVENTPACKET) and return.

    *phEventPacket = prLocalEP;

    return ( TRUE );
    }

//---------------------------------------------------------------------------
//
// BOOL EventPacketDestroy (
//      PHEVENTPACKET phEventPacket
//      );
//
// This function deletes an event packet.  It frees the memory allocated
// for the handle.
//
// Entry:
//      phEventPacket - pointer to a variable containing the event handle
//                      to delete
//
// Exit:
//      This function returns always TRUE, and *phEventPacket set to NULL
//
//---------------------------------------------------------------------------
BOOL EventPacketDestroy (
    PHEVENTPACKET phEventPacket
    )
    {
                                        // We shouldn't get invalid handles.

    SYM_ASSERT ( phEventPacket );
    SYM_ASSERT ( *phEventPacket );

                                        // Free the memory for the Event
                                        // Packet structure.

    MemFreePtr ( *phEventPacket );

                                        // Invalidate the handle.

    *phEventPacket = NULL;

    return ( TRUE );
    }

//---------------------------------------------------------------------------
//
// VOID EventPacketProcess (
//      HEVENTPACKET hEventPacket
//      );
//
// This function notifies the Kernel that there is an Event Packet to
// be processed, activates the Kernel and waits for it to process the
// tasks.
//
// Entry:
//      hEventPacket - handle (pointer) to an Event Packet
//
// Exit:
//      -
//
// Note:
//      Currently this procedure does not create a thread and does not
//      use multitasking features.  The Kernel processes Event Packets
//      during the notification.
//
//---------------------------------------------------------------------------
VOID EventPacketProcess (
    HEVENTPACKET hEventPacket
    )
    {
                                        // Check for invalid parameters.

    SYM_ASSERT ( hEventPacket );

                                        // Notify Kernel of Event Packet.

    KernelNotify ( hEventPacket );
    }

//---------------------------------------------------------------------------
//
// BOOL EventPacketGetResult (
//      HEVENTPACKET prEventPacket
//      );
//
// This function returns the status of an Event Packet after it has been
// processed by the Kernel.  The result is used after this by SYMEvent
// to determine whether to continue or to stop the event.
//
// Entry:
//      prEventPacket - handle (pointer) to an Event Packet
//
// Exit:
//      TRUE  if SYMEvent should continue with the event
//      FALSE if SYMEvent should stop the event
//
//---------------------------------------------------------------------------
BOOL EventPacketGetResult (
    HEVENTPACKET prEventPacket
    )
    {
                                        // Check for invalid parameters.

    SYM_ASSERT ( prEventPacket );

                                        // Get result.

    return ( prEventPacket->bCallbackResult );
    }

//---------------------------------------------------------------------------
//
// VOID EventPacketSetResult (
//      HEVENTPACKET prEventPacket,
//      BOOL         bResult
//      );
//
// This function sets the result of an Event Packet.  The result
// is used later by SYMEvent to determine whether to continue or
// to stop the event.
//
// Entry:
//      prEventPacket - handle (pointer) to an Event Packet
//      bResult       - TRUE/FALSE result for the event packet.
//
// Exit:
//      Event Packet result set.  The new value should be read by calling
//      EventPacketGetResult().
//
//---------------------------------------------------------------------------

#define ERROR_CODE_STOP_FS_EVENT    5

#if defined(SYM_VXD)
#define ERROR_CODE_STOP_DEV_EVENT   IORS_WRITE_PROTECT
#else
#define ERROR_CODE_STOP_DEV_EVENT   (WORD)STATUS_MEDIA_WRITE_PROTECTED
#endif

VOID EventPacketSetResult (
    HEVENTPACKET prEventPacket,
    BOOL         bResult
    )
    {
                                        // Check for invalid parameters.

    SYM_ASSERT ( prEventPacket );
    SYM_ASSERT ( prEventPacket->prEDExtra );
    SYM_ASSERT ( prEventPacket->prEDExtra->prSEEventData );

                                        // Set result.

    prEventPacket->bCallbackResult = bResult;

                                        // If result is FALSE, set error
                                        // code for SYMEvent.

    if ( !bResult )
        {
        switch ( prEventPacket->prEDExtra->prSEEventData->SE_ED_etEventType )
            {
            case SE_LDR_APPLICATION_EXECUTE:
            case SE_FS_FILE_OPEN:
            case SE_FS_OBJECT_RENAME:
            case SE_FS_FILE_CREATE:
            case SE_FS_FILE_CLOSE:
            case SE_FS_FILE_WRITE:
            case SE_FS_OBJECT_SETATTRIBUTES:

                prEventPacket->prEDExtra->prSEEventData->SE_ED_wRtnCode =
                    ERROR_CODE_STOP_FS_EVENT;

                break;

            case SE_DEV_WRITE:
            case SE_DEV_FORMAT:

                prEventPacket->prEDExtra->prSEEventData->SE_ED_wRtnCode =
                    ERROR_CODE_STOP_DEV_EVENT;

                break;

            default:
                SYM_ASSERT ( FALSE );
            }
        }
    }

//---------------------------------------------------------------------------
//
// BOOL GetDefaultEventResult (
//      LPED     prED,
//      PEDEXTRA prEDExtra
//      );
//
// This function is called if NAVAP was not able to create or to process
// an event packet.  The return value tells SYMEvent what to do with
// the event.
//
// Entry:
//      prED      - pointer to SYMEvent's event structure
//      prEDExtra - pointer to EDEXTRA structure containing information
//                  about the event.
//
// Exit:
//      TRUE  if SYMEvent should continue with the event
//      FALSE if SYMEvent should stop the event
//
// Note:
//      Only one of the parameters to the function can be NULL.  If both
//      parameters are valid pointers, the function will use the data
//      in prEDExtra.
//
//---------------------------------------------------------------------------
BOOL GetDefaultEventResult (
    LPED     prED,
    PEDEXTRA prEDExtra
    )
    {
                                        // Check for invalid parameters.

    SYM_ASSERT ( prEDExtra || prED );

                                        // All actions will be allowed.

    return ( TRUE );
    }
