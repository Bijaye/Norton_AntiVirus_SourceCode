// Copyright 1996 - 1998 Symantec, Peter Norton Product Group
//***************************************************************************
//
// $Header:   S:/APUI/VCS/vlact.cpv   1.5   16 Mar 1998 14:23:58   DALLEE  $
//
// Description:
//      This file defines main UI functions which deal with virus-like
//      activities.
//
// Contains:
//      UIVirusLikeActivity()
//
// See Also:
//      apui.h
//
// Notes:
//      UI module
//
//      Supports SYM_VXD platform only.
//
//***************************************************************************
// $Log:   S:/APUI/VCS/vlact.cpv  $
// 
//    Rev 1.5   16 Mar 1998 14:23:58   DALLEE
// Changes for sequences of actions:
// Call NAVGetMainAction() with ordered list of actions to take.
// 
//    Rev 1.4   19 Feb 1998 16:50:46   RStanev
// Changed the prototype of UISendAsyncInformation().
//
//    Rev 1.3   17 Feb 1998 20:54:02   RStanev
// Updated copyright.
//
//    Rev 1.2   28 May 1997 13:59:04   RStanev
// Added support for long custom alerts.
//
//    Rev 1.1   15 May 1997 02:32:34   RStanev
// Cleaned up.
//
//    Rev 1.0   09 Apr 1997 12:16:06   RStanev
// Initial revision.
//
//***************************************************************************

#if !defined(SYM_VXD)
#error This file supports only SYM_VXD!
#endif

#include "platform.h"
#include "navap.h"
#include "apui.h"

//---------------------------------------------------------------------------
//
// AVACTION UIVirusLikeActivity (
//      HEVENTPACKET  hEventPacket,
//      AVCONTEXT     uAVContext,
//      AVACTION      uLastAVAction,
//      BOOL          bLastActionCompleted,
//      PAVACTIONDATA prAVActionData
//      );
//
// This function is called when a virus-like activity has been detected
// and the Kernel needs to query the UI for an action to be taken.  It first
// sends a packet to user mode with the status of the previous action taken,
// then depending on that status it either returns to the caller with
// AVACTION_LAST or returns a new action determined by the current
// configuration or user input.
//
// The process of calling UIVirusLikeActivity() is the same as in
// UIFileInfected().
//
// Entry:
//      hEventPacket         - handle to current event packet.
//      uAVContext           - context of the virus-like activity.
//      uLastAVAction        - last AVACTION returned by this function,
//                             or AVACTION_FIRST if first call to this
//                             function.
//      bLastActionCompleted - TRUE if last action completed successfully,
//                             FALSE if last action failed.
//      prAVActionData       - pointer to a structure where this function
//                             can keep track of previously taken actions
//                             and their status.
//
// Exit:
//      Returns an AVACTION member, depending on current configuration
//      and/or user's response.
//
//---------------------------------------------------------------------------
AVACTION UIVirusLikeActivity (
    HEVENTPACKET  hEventPacket,
    AVCONTEXT     uAVContext,
    AVACTION      uLastAVAction,
    BOOL          bLastActionCompleted,
    PAVACTIONDATA prAVActionData
    )
    {
    auto AVACTION       uResult;
    auto BOOL           bThisIsLastAction;
    auto MAINACTION     uMainAction;
    auto UINT           uPromptActions;
    auto BOOL           bNotifyOnly;
    auto void           **ppvVirusLikeActivity;
    auto void           *apvParameters[_UI_PARAMETER_TABLE_SIZE];
    auto PTSTR          pszText, pszButtons;

                                        // UI data for the current
                                        // virus-like activity.

    auto PMAINACTION    puContextMainActions;
    auto void           **ppvFirstMessage;
    auto void           **ppvExcludeFailedMessage;
    auto void           **ppvNotifyMessage;

                                        // Validate input parameters.

    SYM_ASSERT ( hEventPacket );
    SYM_ASSERT ( hEventPacket->prEDExtra );
    SYM_ASSERT ( prAVActionData );

                                        // If the last action was successful,
                                        // then no more processing should be
                                        // done.

    bThisIsLastAction = uLastAVAction != AVACTION_FIRST &&
                        bLastActionCompleted;

                                        // Notify the user mode counterpart
                                        // of what's going on.

    UISendAsyncInformation ( hEventPacket,
                             uAVContext,
                             uLastAVAction,
                             bLastActionCompleted,
                             bThisIsLastAction,
                             NULL );

                                        // Check if we should end the action
                                        // loop.

    if ( bThisIsLastAction )
        {
        return ( AVACTION_LAST );
        }

                                        // Check if in shutdown mode.

    if ( bNAVAPIsInShutdownMode )
        {
        return ( AVACTION_STOP );
        }

                                        // Get parameters for the current
                                        // AV context.

    switch ( uAVContext )
        {
        case AVCONTEXT_HDFORMAT:

            puContextMainActions = prAPCfg(hEventPacket)->auFormatHardDiskActions;

            ppvFirstMessage         = apvHDFormat;
            ppvExcludeFailedMessage = apvHDFormatExcludeFailed;
            ppvNotifyMessage        = apvHDFormatStopNotify;

            break;

        case AVCONTEXT_HDMBRWRITE:

            puContextMainActions = prAPCfg(hEventPacket)->auWriteToHDBootRecordsActions;

            ppvFirstMessage         = apvHDMBRWrite;
            ppvExcludeFailedMessage = apvHDMBRWriteExcludeFailed;
            ppvNotifyMessage        = apvHDMBRWriteStopNotify;

            break;

        case AVCONTEXT_HDBOOTWRITE:

            puContextMainActions = prAPCfg(hEventPacket)->auWriteToHDBootRecordsActions;

            ppvFirstMessage         = apvHDBootWrite;
            ppvExcludeFailedMessage = apvHDBootWriteExcludeFailed;
            ppvNotifyMessage        = apvHDBootWriteStopNotify;

            break;

        case AVCONTEXT_FDBOOTWRITE:

            puContextMainActions = prAPCfg(hEventPacket)->auWriteToFDBootRecordActions;

            ppvFirstMessage         = apvFDBootWrite;
            ppvExcludeFailedMessage = apvFDBootWriteExcludeFailed;
            ppvNotifyMessage        = apvFDBootWriteStopNotify;

            break;

        case AVCONTEXT_WRITETOFILE:

            puContextMainActions = prAPCfg(hEventPacket)->auWriteToProgramFilesActions;

            ppvFirstMessage         = apvWriteToFile;
            ppvExcludeFailedMessage = apvWriteToFileExcludeFailed;
            ppvNotifyMessage        = apvWriteToFileStopNotify;

            break;

        case AVCONTEXT_FILEROATTRCHANGE:

            puContextMainActions = prAPCfg(hEventPacket)->auROAttributeClearActions;

            ppvFirstMessage         = apvFileROAttrChange;
            ppvExcludeFailedMessage = apvFileROAttrChangeExcludeFailed;
            ppvNotifyMessage        = apvFileROAttrChangeStopNotify;

            break;

        default:

            SYM_ASSERT ( FALSE );

            return ( AVACTION_LAST );
        }

                                        // Get main action to take.

    uMainAction = NAVGetMainAction ( uAVContext,
                                     puContextMainActions,
                                     uLastAVAction,
                                     prAVActionData
                                     );

    switch ( uMainAction )
        {
        case MAINACTION_ALLOW:

            return ( AVACTION_CONTINUE );

        case MAINACTION_DONOTALLOW_NOTIFY:

            bNotifyOnly = TRUE;

            break;

        case MAINACTION_PROMPT:

            bNotifyOnly = FALSE;

            break;

        default:

            SYM_ASSERT ( FALSE );

            return ( AVACTION_STOP );
        }

    if ( !bNotifyOnly )
        {
                                        // Select a message to display when
                                        // prompting user for action.

        switch ( uLastAVAction )
            {
            case AVACTION_FIRST:

                ppvVirusLikeActivity = ppvFirstMessage;

                break;

            case AVACTION_EXCLUDE:

                ppvVirusLikeActivity = ppvExcludeFailedMessage;

                break;

            default:

                SYM_ASSERT ( FALSE );

                ppvVirusLikeActivity = ppvFirstMessage;
            }

                                        // Get prompt actions allowed.

        uPromptActions = NAVGetPromptActions (
                            uAVContext,
                            prAPCfg(hEventPacket)->uBehaviorBlockPrompts,
                            uLastAVAction,
                            prAVActionData );

        if ( !hEventPacket->prEDExtra->bOwnerIsKnown )
            {
            uPromptActions &= ~fPROMPTACTION_EXCLUDE;
            }
        }
    else
        {
                                        // Notify only, no actions allowed.

        ppvVirusLikeActivity = ppvNotifyMessage;

        uPromptActions = fPROMPTACTION_OK;
        }

                                        // Create parameter table.

    apvParameters[_UI_FILE_CAUSING_TROUBLE] = EDXpszOEMOwner (
                                                  hEventPacket->prEDExtra
                                                  );

    apvParameters[_UI_FILE_IN_TROUBLE] = EDXpszOEMFile (
                                             hEventPacket->prEDExtra
                                             );

    apvParameters[_UI_DRIVE_IN_TROUBLE] = EDXpszOEMDevice (
                                              hEventPacket->prEDExtra
                                              );

                                        // Allocate memory for message and
                                        // buttons.

    pszText = (PTSTR) MemAllocPtr ( GHND,
                                    GetBuildTextLength ( ppvVirusLikeActivity,
                                                         apvParameters ) );

    pszButtons = (PTSTR) MemAllocPtr ( GHND,
                                       GetBuildButtonsLength ( uPromptActions ) );

    if ( prAPCfg(hEventPacket)->bAudibleAlert )
        {
        ScheduleSoundAlert ( SOUND_VIRUS_LIKE_ACTIVITY,
                             prAPCfg(hEventPacket)->uAudibleAlertDelay );
        }

    if ( pszText && pszButtons )
        {
                                        // If we display a dialog, and this
                                        // is the first time we are doing
                                        // it, then show the long custom
                                        // alert message first.

        if ( uLastAVAction == AVACTION_FIRST )
            {
            UIShowLongCustomAlert ( hEventPacket, AMB_COLOR_CYAN );
            }

                                        // Build message.

        BuildText ( ppvVirusLikeActivity, apvParameters, pszText );

                                        // Build buttons.

        BuildButtons ( pszButtons, uPromptActions );

                                        // Display warning message.

        uResult = (AVACTION) NAVAPMessage (
                                &rVirusLikeActivityMessageBoxRect,
                                prAPCfg(hEventPacket)->uAlertTimeOut,
                                GetDefaultButton(pszButtons),
                                AMB_COLOR_CYAN,
                                szAutoProtectTitle,
                                pszText,
                                ( prAPCfg(hEventPacket)->bDisplayAlert ) ?
                                prAPCfg(hEventPacket)->pszAlertText : NULL,
                                pszButtons
                                );
        }
    else
        {
                                        // If the prompt cannot be displayed,
                                        // then return STOP.

        UIOutOfMemoryError();

        uResult = AVACTION_STOP;
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

                                        // Return an AVACTION.

    if ( bNotifyOnly )
        {
        return ( AVACTION_STOP );
        }

    return ( uResult );
    }
