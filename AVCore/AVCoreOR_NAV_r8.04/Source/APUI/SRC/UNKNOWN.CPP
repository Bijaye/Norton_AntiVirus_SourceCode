// Copyright 1996 - 1998 Symantec, Peter Norton Product Group
//***************************************************************************
//
// $Header:   S:/APUI/VCS/UNKNOWN.CPv   1.7   19 Feb 1998 16:50:44   RStanev  $
//
// Description:
//      This file defines main UI functions which deal with unknown virus
//      problems.
//
// Contains:
//      UISelfCheckFailed()
//
// See Also:
//      apui.h
//
// Note:
//      UI module
//
//***************************************************************************
// $Log:   S:/APUI/VCS/UNKNOWN.CPv  $
// 
//    Rev 1.7   19 Feb 1998 16:50:44   RStanev
// Changed the prototype of UISendAsyncInformation().
//
//    Rev 1.6   19 Feb 1998 15:15:40   RStanev
// Removed unknown virus detection.
//
//    Rev 1.5   17 Feb 1998 20:54:00   RStanev
// Updated copyright.
//
//    Rev 1.4   28 May 1997 13:56:32   RStanev
// Added support for long custom alerts.  Added support for MAINACTION_MOVE
// to UIInocFile().  Added message to support failed AVACTION_MOVE after user
// was prompted by UIInocFile().
//
//    Rev 1.3   21 May 1997 04:31:12   RStanev
// Implemented UISelfCheckFailed() for SYM_NTK.
//
//    Rev 1.2   15 May 1997 02:31:40   RStanev
// Minor changes.
//
//    Rev 1.1   01 May 1997 14:22:54   RStanev
// Removed one-half specific handling from within UISelfCheckFailed().
//
//    Rev 1.0   09 Apr 1997 12:16:04   RStanev
// Initial revision.
//
//***************************************************************************

#include "platform.h"
#include "navap.h"
#include "apui.h"

//---------------------------------------------------------------------------
//
// AVACTION UISelfCheckFailed (
//      HEVENTPACKET  hEventPacket,
//      UINT          uReason,
//      AVACTION      uLastAVAction,
//      BOOL          bLastActionCompleted,
//      PAVACTIONDATA prAVActionData
//      );
//
// This function is called when NAVAP's self check had failed and either
// the Kernel needs to query the UI for an action to be taken or the user
// has to be notified that NAVAP will not load and system will shut down.
// It first sends to user mode the status of the previous action taken, then
// depending on that status it either returns to caller with AVACTION_LAST or
// returns a new action determined by the current configuration or user
// input.
//
// Entry:
//      hEventPacket         - handle to current event packet, or NULL
//                             if called during startup.
//      uReason              - the value returned by FileSelfTest().
//      uLastAVAction        - last AVACTION returned by this function, or
//                             AVACTION_SHUTDOWN if called during startup.
//      bLastActionCompleted - TRUE if last action completed successfully,
//                             FALSE if last action failed.  TRUE if called
//                             during startup.
//      prAVActionData       - pointer to a structure where this function
//                             can keep track of previously taken actions
//                             and their status.  NULL if called during
//                             startup.
//
// Exit:
//      Returns an AVACTION member, depending on current configuration
//      and/or user's response.  Returns AVACTION_SHUTDOWN if called
//      during startup.
//
//---------------------------------------------------------------------------
AVACTION UISelfCheckFailed (
    HEVENTPACKET  hEventPacket,
    UINT          uReason,
    AVACTION      uLastAVAction,
    BOOL          bLastActionCompleted,
    PAVACTIONDATA prAVActionData
    )
    {
#if defined(SYM_VXD)

    auto void        **ppvMessage;
    auto PTSTR       pszText, pszButtons;
    auto BOOL        bLocalEventPacket;
    auto EVENTPACKET rEventPacket;
    auto EDEXTRA     rEDExtra;

                                        // Validate input parameters.

    SYM_ASSERT ( uLastAVAction == AVACTION_SHUTDOWN );
    SYM_ASSERT ( bLastActionCompleted );

                                        // If hEventPacket is NULL, then
                                        // create a fake one, using the
                                        // current configuration.

    if ( !hEventPacket )
        {
        hEventPacket           = &rEventPacket;
        rEventPacket.prEDExtra = &rEDExtra;
        rEDExtra.prNAVAPCfg    = LockNAVAPConfiguration();

        SYM_ASSERT ( rEDExtra.prNAVAPCfg );

        bLocalEventPacket      = TRUE;
        }
    else
        {
        bLocalEventPacket = FALSE;
        }

                                        // Notify the user mode counterpart
                                        // of what's going on.

                                        // Note: Currently the user mode
                                        // agent does not have a clue of
                                        // how to handle this situation, so
                                        // we will not send anything.

//    UISendAsyncInformation ( hEventPacket,
//                             AVCONTEXT_???,
//                             uLastAVAction,
//                             bLastActionCompleted,
//                             TRUE,
//                             NULL );

                                        // Determine what message to
                                        // display.

    switch ( uReason )
        {
        case SELF_TEST_NO_CRC:

            ppvMessage = apvSelfCheckFailedNoCRC;

            break;

        case SELF_TEST_FAIL:

            ppvMessage = apvSelfCheckFailedCRCChanged;

            break;

        case SELF_TEST_FILE_ERROR:

            ppvMessage = apvSelfCheckFailedIOError;

            break;

        case SELF_TEST_SUCCESS:

                                        // This value will be passed if
                                        // the caller could not set up
                                        // internal variables to call
                                        // FileSelfTest().

            ppvMessage = apvSelfCheckFailedInternal;

            break;

        default:

            SYM_ASSERT ( FALSE );

        }

                                        // Allocate memory for message and
                                        // buttons.

    pszText = (PTSTR) MemAllocPtr ( GHND,
                                    GetBuildTextLength ( ppvMessage, NULL ) );

    pszButtons = (PTSTR) MemAllocPtr ( GHND,
                                       GetBuildButtonsLength ( fPROMPTACTION_OK ) );

    if ( prAPCfg(hEventPacket)->bAudibleAlert )
        {
        ScheduleSoundAlert ( SOUND_UNKNOWN_VIRUS,
                             prAPCfg(hEventPacket)->uAudibleAlertDelay );
        }

    if ( pszText && pszButtons )
        {
                                        // Show the long custom alert
                                        // message.

        UIShowLongCustomAlert ( hEventPacket, AMB_COLOR_BLUE );

                                        // Build message to notify that
                                        // the system will be shut down.

        BuildText ( ppvMessage, NULL, pszText );

                                        // Build buttons.

        BuildButtons ( pszButtons, fPROMPTACTION_OK );

                                        // Display warning message.

        NAVAPMessage ( &rSelfCheckFailedMessageBoxRect,
                       prAPCfg(hEventPacket)->uAlertTimeOut,
                       GetDefaultButton(pszButtons),
                       AMB_COLOR_BLUE,
                       szAutoProtectTitle,
                       pszText,
                       ( prAPCfg(hEventPacket)->bDisplayAlert ) ?
                       prAPCfg(hEventPacket)->pszAlertText : NULL,
                       pszButtons
                       );
        }
    else
        {
                                        // The prompt cannot be displayed.

        UIOutOfMemoryError();
        }

                                        // Free up allocated memory.

    if ( pszText )
        {
        MemFreePtr ( pszText );
        }

    if ( pszButtons )
        {
        MemFreePtr ( pszButtons );
        }

                                        // Unlock locked configuration.

    if ( bLocalEventPacket )
        {
        UnlockNAVAPConfiguration ( rEDExtra.prNAVAPCfg );
        }

#elif defined(SYM_NTK)

                                        // Validate input parameters.

    SYM_ASSERT ( uLastAVAction == AVACTION_SHUTDOWN );
    SYM_ASSERT ( bLastActionCompleted );

                                        // Determine what message to report.

    switch ( uReason )
        {
        case SELF_TEST_NO_CRC:

            NAVAPNTWriteStandardEventLog ( aUISELFCHECKERRORTranslator[UISELFCHECKERROR_NO_CRC] );

            break;

        case SELF_TEST_FAIL:

            NAVAPNTWriteStandardEventLog ( aUISELFCHECKERRORTranslator[UISELFCHECKERROR_CRC_CHANGED] );

            break;

        case SELF_TEST_FILE_ERROR:

            NAVAPNTWriteStandardEventLog ( aUISELFCHECKERRORTranslator[UISELFCHECKERROR_FILE_IO] );

            break;

        case SELF_TEST_SUCCESS:

                                        // This value will be passed if
                                        // the caller could not set up
                                        // internal variables to call
                                        // FileSelfTest().

            NAVAPNTWriteStandardEventLog ( aUISELFCHECKERRORTranslator[UISELFCHECKERROR_INTERNAL] );

            break;

        default:

            SYM_ASSERT ( FALSE );

        }

#endif

                                        // Return an AVACTION of
                                        // SHUTDOWN.

    return ( AVACTION_SHUTDOWN );
    }
