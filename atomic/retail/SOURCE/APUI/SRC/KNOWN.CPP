// Copyright 1996 - 1998 Symantec, Peter Norton Product Group
//***************************************************************************
//
// $Header:   S:/APUI/VCS/known.cpv   1.9   16 Mar 1998 14:22:56   DALLEE  $
//
// Description:
//      This file defines main UI functions which deal with known virus
//      infections.
//
// Contains:
//      UIFileInfected()
//      UIInfoScanningBoot()            (SYM_VXD only)
//      UIBootInfected()                (SYM_VXD only)
//      UIMemoryInfected()              (SYM_VXD only)
//
// See Also:
//      apui.h
//
// Note:
//      UI module
//
//***************************************************************************
// $Log:   S:/APUI/VCS/known.cpv  $
// 
//    Rev 1.9   16 Mar 1998 14:22:56   DALLEE
// Changes for sequences of actions:
// Call NAVGetMainAction() with ordered list of actions to take.
// 
//    Rev 1.8   02 Mar 1998 12:05:22   DALLEE
// Added success messages for Delete and Quarantine.
// 
//    Rev 1.7   19 Feb 1998 16:50:22   RStanev
// Changed the prototype of UISendAsyncInformation().
//
//    Rev 1.6   19 Feb 1998 14:58:42   RStanev
// Added quarantine support.  Added successful action confirmation.
//
//    Rev 1.5   17 Feb 1998 20:53:54   RStanev
// Updated copyright.
//
//    Rev 1.4   01 Jun 1997 18:11:16   RStanev
// UIFileInfected() and UIBootInfected() now update the bUserIntervention
// field in hEventPacket.
//
//    Rev 1.3   28 May 1997 13:50:30   RStanev
// Added support for long custom alerts.  Added message to support failed
// AVACTION_MOVE after user was prompted by UIFileInfected().  Split
// uScanFileAction into uScanFileExeAction and uScanFileMacroAction.
//
//    Rev 1.2   15 May 1997 02:25:02   RStanev
// Added UIInfoScanningBoot().
//
//    Rev 1.1   29 Apr 1997 13:18:30   RStanev
// Fixed compilation errors introduced by AVENGE changes.
//
//    Rev 1.0   09 Apr 1997 12:16:00   RStanev
// Initial revision.
//
//***************************************************************************

#include "platform.h"
#include "navap.h"
#include "apui.h"

//---------------------------------------------------------------------------
//
// AVACTION UIFileInfected (
//      HEVENTPACKET  hEventPacket,
//      HVLVIRUS      hVLVirus,
//      AVACTION      uLastAVAction,
//      BOOL          bLastActionCompleted,
//      PAVACTIONDATA prAVActionData
//      );
//
// This function is called when an infected file is found and the Kernel
// needs to query the UI for an action to be taken.  It notifies NAVAP's
// user mode counterpart of the status of the previous action taken, then
// depending on that status it either returns to caller with AVACTION_LAST
// or returns a new action determined by the current configuration or user
// input.
//
// UIFileInfected() should be called multiple times per task.  The first
// time this function is called, the uLastAVAction parameter should be
// set to AVACTION_FIRST.  bLastActionCompleted is not used during the
// first call to this function.  This function will return an AVACTION to
// be taken by the caller.  After the caller performs the requested action,
// this function should be called again, but this time the
// bLastActionCompleted parameter should be set.  UIFileInfected() will
// then return with another action to be taken.  This process should be
// repeated until UIFileInfected() returns AVACTION_LAST.
//
// Entry:
//      hEventPacket         - handle to current event packet
//      hVLVirus             - handle to virus found
//      uLastAVAction        - last AVACTION returned by this function
//      bLastActionCompleted - TRUE if last action completed successfully,
//                             FALSE if last action failed
//      prAVActionData       - pointer to a structure where this function
//                             can keep track of previously taken actions
//                             and their status
//
// Exit:
//      Returns an AVACTION member, depending on current configuration
//      and/or user's response.
//
//---------------------------------------------------------------------------
AVACTION UIFileInfected (
    HEVENTPACKET  hEventPacket,
    HVLVIRUS      hVLVirus,
    AVACTION      uLastAVAction,
    BOOL          bLastActionCompleted,
    PAVACTIONDATA prAVActionData
    )
    {
    auto AVACTION       uResult;
    auto BOOL           bThisIsLastAction;
    auto PMAINACTION    puMainActions;
    auto MAINACTION     uMainAction;
    auto TCHAR          szVirusName[SIZE_VIRUSNAME + 1];
    auto UINT           uPromptActions;
    auto void           **ppvFileInfectedMessage;
    auto void           *apvParameters[_UI_PARAMETER_TABLE_SIZE];
    auto PTSTR          pszText, pszButtons;

                                        // Validate input parameters.

    SYM_ASSERT ( hEventPacket );
    SYM_ASSERT ( hEventPacket->prEDExtra );
    SYM_ASSERT ( hVLVirus );
    SYM_ASSERT ( prAVActionData );

                                        // If the last action was successful,
                                        // then no more processing should be
                                        // done.

    bThisIsLastAction = uLastAVAction != AVACTION_FIRST &&
                        bLastActionCompleted;

                                        // Notify the user mode counterpart
                                        // of what's going on.

    UISendAsyncInformation ( hEventPacket,
                             AVCONTEXT_SCANFILE,
                             uLastAVAction,
                             bLastActionCompleted,
                             bThisIsLastAction,
                             hVLVirus );

                                        // Confirm successful action if user
                                        // intervention was required.

    if ( bThisIsLastAction &&
         hEventPacket->prEDExtra->bUserIntervention )
        {
                                        // Determine what message to display
                                        // based on the successful action.

        switch ( uLastAVAction )
            {
            case AVACTION_REPAIR:

                ppvFileInfectedMessage = apvInfectedFileRepairSuccessful;

                break;

            case AVACTION_DELETE:

                ppvFileInfectedMessage = apvInfectedFileDeleteSuccessful;

                break;

            case AVACTION_QUARANTINE:

                ppvFileInfectedMessage = apvInfectedFileQuarantineSuccessful;

                break;

            default:

                ppvFileInfectedMessage = NULL;

            }

        if ( ppvFileInfectedMessage )
            {
                                        // Get virus name.

            UIGetVirusName ( hVLVirus, szVirusName );

                                        // Create parameter table.

            apvParameters[_UI_VIRUS_NAME]      = szVirusName;
            apvParameters[_UI_FILE_IN_TROUBLE] = EDXpszOEMFile (
                                                     hEventPacket->prEDExtra
                                                     );

                                        // Allocate memory for message and
                                        // buttons.

            pszText = (PTSTR) MemAllocPtr ( GHND,
                                            GetBuildTextLength (
                                                ppvFileInfectedMessage,
                                                apvParameters ) );

            pszButtons = (PTSTR) MemAllocPtr (
                                         GHND,
                                         GetBuildButtonsLength (
                                             fPROMPTACTION_CONTINUE ) );

            if ( pszText && pszButtons )
                {
                                        // Build message.

                BuildText ( ppvFileInfectedMessage, apvParameters, pszText );

                                        // Build buttons.

                BuildButtons ( pszButtons, fPROMPTACTION_CONTINUE );

                                        // Display confirmation message.

                NAVAPMessage ( &rFileInfectedMessageBoxRect,
                               prAPCfg(hEventPacket)->uAlertTimeOut,
                               GetDefaultButton(pszButtons),
                               AMB_COLOR_RED,
                               szAutoProtectTitle,
                               pszText,
                               ( prAPCfg(hEventPacket)->bDisplayAlert ) ?
                               prAPCfg(hEventPacket)->pszAlertText : NULL,
                               pszButtons
                               );
                }
            else
                {
                                        // If the confirmation cannot be
                                        // displayed, warn the user.

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
            }
        }

                                        // From now on, assume that the
                                        // last call to this function did
                                        // not require user intervention.

    hEventPacket->prEDExtra->bUserIntervention = FALSE;

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

                                        // Otherwise, get main action to
                                        // take.

    if ( VLVirusInfo ( hVLVirus, 0, VI_BOOL_MACRO_VIRUS, NULL ) )
        {
        puMainActions = prAPCfg(hEventPacket)->auScanFileMacroActions;
        }
    else
        {
        puMainActions = prAPCfg(hEventPacket)->auScanFileExeActions;
        }

    uMainAction = NAVGetMainAction ( AVCONTEXT_SCANFILE,
                                     puMainActions,
                                     uLastAVAction,
                                     prAVActionData
                                     );

    switch ( uMainAction )
        {
        case MAINACTION_REPAIR:

            return ( AVACTION_REPAIR );

        case MAINACTION_DELETE:

            return ( AVACTION_DELETE );

        case MAINACTION_DENYACCESS:

            return ( AVACTION_STOP );

        case MAINACTION_DENYACCESS_NOTIFY:

                                        // Notify that access will be denied,
                                        // no actions allowed.

            ppvFileInfectedMessage = apvInfectedFileFoundStopNotify;

            uPromptActions = fPROMPTACTION_OK;

            break;

        case MAINACTION_SHUTDOWN_NOTIFY:

                                        // Notify that the system will be
                                        // shut down.

            ppvFileInfectedMessage = apvInfectedFileFoundShutdownNotify;

            uPromptActions = fPROMPTACTION_OK;

            break;

        case MAINACTION_QUARANTINE:

            return ( AVACTION_QUARANTINE );

        case MAINACTION_MOVE:

            return ( AVACTION_MOVE );

        case MAINACTION_RENAME:

            return ( AVACTION_RENAME );

        case MAINACTION_PROMPT:

                                        // Select a message to display when
                                        // prompting user for action.

            switch ( uLastAVAction )
                {
                case AVACTION_FIRST:

                    ppvFileInfectedMessage = apvInfectedFileFound;

                    break;

                case AVACTION_REPAIR:

                    ppvFileInfectedMessage = apvInfectedFileRepairFailed;

                    break;

                case AVACTION_DELETE:

                    ppvFileInfectedMessage = apvInfectedFileDeleteFailed;

                    break;

                case AVACTION_EXCLUDE:

                    ppvFileInfectedMessage = apvInfectedFileExcludeFailed;

                    break;

                case AVACTION_QUARANTINE:

                    ppvFileInfectedMessage = apvInfectedFileQuarantineFailed;

                    break;

                case AVACTION_MOVE:

                    ppvFileInfectedMessage = apvInfectedFileMoveFailed;

                    break;

                case AVACTION_RENAME:

                    ppvFileInfectedMessage = apvInfectedFileRenameFailed;

                    break;

                default:

                    SYM_ASSERT ( FALSE );

                    ppvFileInfectedMessage = apvInfectedFileFound;
                }

                                        // Get prompt actions allowed.

            uPromptActions = NAVGetPromptActions (
                                 AVCONTEXT_SCANFILE,
                                 prAPCfg(hEventPacket)->uScanFilePrompts,
                                 uLastAVAction,
                                 prAVActionData
                                 );

            break;

        default:

            SYM_ASSERT ( FALSE );

            return ( AVACTION_STOP );
        }

                                        // If we are here, then user
                                        // intervention is required.

    hEventPacket->prEDExtra->bUserIntervention = TRUE;

                                        // Get virus name.

    UIGetVirusName ( hVLVirus, szVirusName );

                                        // Create parameter table.

    apvParameters[_UI_VIRUS_NAME]      = szVirusName;
    apvParameters[_UI_FILE_IN_TROUBLE] = EDXpszOEMFile (
                                             hEventPacket->prEDExtra
                                             );

                                        // Allocate memory for message and
                                        // buttons.

    pszText = (PTSTR) MemAllocPtr ( GHND,
                                    GetBuildTextLength (
                                        ppvFileInfectedMessage,
                                        apvParameters ) );

    pszButtons = (PTSTR) MemAllocPtr ( GHND,
                                       GetBuildButtonsLength (
                                           uPromptActions ) );

    if ( prAPCfg(hEventPacket)->bAudibleAlert )
        {
        ScheduleSoundAlert ( SOUND_KNOWN_VIRUS,
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
            UIShowLongCustomAlert ( hEventPacket, AMB_COLOR_RED );
            }

                                        // Build message.

        BuildText ( ppvFileInfectedMessage, apvParameters, pszText );

                                        // Build buttons.

        BuildButtons ( pszButtons, uPromptActions );

                                        // Display warning message.

        uResult = (AVACTION) NAVAPMessage (
                                &rFileInfectedMessageBoxRect,
                                prAPCfg(hEventPacket)->uAlertTimeOut,
                                GetDefaultButton(pszButtons),
                                AMB_COLOR_RED,
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

    if ( uMainAction == MAINACTION_DENYACCESS_NOTIFY )
        {
        uResult = AVACTION_STOP;
        }
    else if ( uMainAction == MAINACTION_SHUTDOWN_NOTIFY )
        {
        uResult = AVACTION_SHUTDOWN;
        }

    return ( uResult );
    }

//---------------------------------------------------------------------------
//
// VOID UIInfoScanningBoot (
//      HEVENTPACKET   hEventPacket,
//      BOOL           bShowOrHide
//      );
//
// This function shows or hides the scanning boot sector information dialog.
//
// Entry:
//      hEventPacket - event packet handle, used to derive the device name
//      bShowOrHide  - TRUE to show the information dialog, FALSE to hide.
//
// Exit:
//      -
//
//---------------------------------------------------------------------------
#if defined(SYM_VXD)

VOID UIInfoScanningBoot (
    HEVENTPACKET hEventPacket,
    BOOL         bShowOrHide
    )
    {
    auto   PEDEXTRA prEDExtra;
    auto   PTSTR    pszText;
    auto   void     *apvParameters[_UI_PARAMETER_TABLE_SIZE];
    auto   RECT     rRect;
    static int      iDisplayCount = 0;

                                        // Validate input parameters.

    SYM_ASSERT ( hEventPacket );

                                        // Initialize local variables.

    prEDExtra = hEventPacket->prEDExtra;

                                        // Make sure we are asked to display
                                        // info and check if the
                                        // configuration allows us to do so.

    if ( rNAVAPInternalCfg.bShowUIWhileScanningBoot &&
         prEDExtra->bDisplayInfo )
        {
                                        // Show or hide the dialog.

        if ( bShowOrHide )
            {
                                        // Show it.

                                        // Prepare parameter table for
                                        // building the info message.

            apvParameters[_UI_DRIVE_TO_SCAN] = EDXpszOEMDevice ( prEDExtra );

                                        // Allocate memory for generating
                                        // the message.

            pszText = (PTSTR) MemAllocPtr ( GHND,
                                            GetBuildTextLength (
                                                apvScanningBootMessage,
                                                apvParameters ) );

            if ( pszText )
                {
                                        // Build message.

                BuildText ( apvScanningBootMessage,
                            apvParameters,
                            pszText );

                                        // Display message.

                NAVAPMessage ( &rScanningBootMessageBoxRect,
                               0,
                               0,
                               AMB_COLOR_BLUE,
                               szAutoProtectTitle,
                               pszText,
                               NULL,
                               NULL
                               );

                                        // Release allocated resources.

                MemFreePtr ( pszText );

                                        // Remember that we are now in info
                                        // mode.

                iDisplayCount++;
                }
            else
                {
                UIOutOfMemoryError();
                }
            }
        else
            {
                                        // Hide it.

                                        // Make sure that we have displayed
                                        // the info.

            if ( iDisplayCount )
                {
                                        // Initialize a special RECT
                                        // structure, which we will pass to
                                        // NAVAPMessage() and tell it to
                                        // close the current dialog.

                memset ( &rRect, 0, sizeof ( rRect ) );

                                        // Close information dialog.

                NAVAPMessage ( &rRect,
                               0,
                               0,
                               AMB_COLOR_BLUE,
                               szAutoProtectTitle,
                               pszText,
                               NULL,
                               NULL
                               );

                                        // Remember that we have closed this
                                        // information box.

                iDisplayCount--;
                }
            }
        }
    }

#endif // #if defined(SYM_VXD)

//---------------------------------------------------------------------------
//
// AVACTION UIBootInfected (
//      HEVENTPACKET  hEventPacket,
//      HVLVIRUS      hVLVirus,
//      AVACTION      uLastAVAction,
//      BOOL          bLastActionCompleted,
//      PAVACTIONDATA prAVActionData
//      );
//
// This function is called when an infected boot is found and the Kernel
// needs to query the UI for an action to be taken.  It first sends to user
// mode the current status of the taken action, then depending on that
// status it either returns to caller with AVACTION_LAST or returns
// a new action determined by the current configuration or user input.
//
// It is just like UIFileInfected().
//
// Entry:
//      hEventPacket         - handle to current event packet
//      hVLVirus             - handle to virus found
//      uLastAVAction        - last AVACTION returned by this function
//      bLastActionCompleted - TRUE if last action completed successfully,
//                             FALSE if last action failed
//      prAVActionData       - pointer to a structure where this function
//                             can keep track of previously taken actions
//                             and their status
//
// Exit:
//      Returns an AVACTION member, depending on current configuration
//      and/or user's response.
//
//---------------------------------------------------------------------------
#if defined(SYM_VXD)

AVACTION UIBootInfected (
    HEVENTPACKET  hEventPacket,
    HVLVIRUS      hVLVirus,
    AVACTION      uLastAVAction,
    BOOL          bLastActionCompleted,
    PAVACTIONDATA prAVActionData
    )
    {
    auto AVACTION   uResult;
    auto BOOL       bThisIsLastAction;
    auto BOOL       bWeAreShuttingDown;
    auto MAINACTION uMainAction;
    auto TCHAR      szVirusName[SIZE_VIRUSNAME + 1];
    auto UINT       uPromptActions;
    auto void       **ppvBootInfectedMessage;
    auto void       *apvParameters[_UI_PARAMETER_TABLE_SIZE];
    auto PTSTR      pszText, pszButtons;

                                        // Validate input parameters.

    SYM_ASSERT ( hEventPacket );
    SYM_ASSERT ( hEventPacket->prEDExtra );
    SYM_ASSERT ( hVLVirus );
    SYM_ASSERT ( prAVActionData );

                                        // If the last action was successful,
                                        // then no more processing should be
                                        // done.

    bThisIsLastAction = uLastAVAction != AVACTION_FIRST &&
                        bLastActionCompleted;

                                        // Notify the user mode counterpart
                                        // of what's going on.

    UISendAsyncInformation ( hEventPacket,
                             AVCONTEXT_SCANBOOT,
                             uLastAVAction,
                             bLastActionCompleted,
                             bThisIsLastAction,
                             hVLVirus );

                                        // Confirm successful action if user
                                        // intervention was required.

    if ( bThisIsLastAction &&
         hEventPacket->prEDExtra->bUserIntervention )
        {
                                        // Determine what message to display
                                        // based on the successful action.

        switch ( uLastAVAction )
            {
            case AVACTION_REPAIR:

                ppvBootInfectedMessage = apvInfectedBootRepairSuccessful;

                break;

            default:

                ppvBootInfectedMessage = NULL;

            }

        if ( ppvBootInfectedMessage )
            {
                                        // Get virus name.

            UIGetVirusName ( hVLVirus, szVirusName );

                                        // Create parameter table.

            apvParameters[_UI_VIRUS_NAME]       = szVirusName;
            apvParameters[_UI_DRIVE_IN_TROUBLE] = EDXpszOEMDevice (
                                                     hEventPacket->prEDExtra
                                                     );

                                        // Allocate memory for message and
                                        // buttons.

            pszText = (PTSTR) MemAllocPtr ( GHND,
                                            GetBuildTextLength (
                                                ppvBootInfectedMessage,
                                                apvParameters ) );

            pszButtons = (PTSTR) MemAllocPtr (
                                         GHND,
                                         GetBuildButtonsLength (
                                             fPROMPTACTION_CONTINUE ) );

            if ( pszText && pszButtons )
                {
                                        // Build message.

                BuildText ( ppvBootInfectedMessage, apvParameters, pszText );

                                        // Build buttons.

                BuildButtons ( pszButtons, fPROMPTACTION_CONTINUE );

                                        // Display confirmation message.

                NAVAPMessage ( &rBootInfectedMessageBoxRect,
                               prAPCfg(hEventPacket)->uAlertTimeOut,
                               GetDefaultButton(pszButtons),
                               AMB_COLOR_RED,
                               szAutoProtectTitle,
                               pszText,
                               ( prAPCfg(hEventPacket)->bDisplayAlert ) ?
                               prAPCfg(hEventPacket)->pszAlertText : NULL,
                               pszButtons
                               );
                }
            else
                {
                                        // If the confirmation cannot be
                                        // displayed, warn the user.

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
            }
        }

                                        // From now on, assume that the
                                        // last call to this function did
                                        // not require user intervention.

    hEventPacket->prEDExtra->bUserIntervention = FALSE;

                                        // Check if we should end the action
                                        // loop.

    if ( bThisIsLastAction )
        {
        return ( AVACTION_LAST );
        }

                                        // Initialize local variables.

    bWeAreShuttingDown =
        hEventPacket->prEDExtra->uEventType == SE_MSC_REBOOT ? TRUE :
                                                               FALSE;

                                        // Check if in shutdown mode.

    if ( bNAVAPIsInShutdownMode )
        {
        return ( AVACTION_CONTINUE );
        }

                                        // Get main action to take.

    uMainAction = NAVGetMainAction ( AVCONTEXT_SCANBOOT,
                                     bWeAreShuttingDown ?
                                         prAPCfg(hEventPacket)->auScanDeviceUponBootActions :
                                         prAPCfg(hEventPacket)->auScanDeviceUponAccessActions,
                                     uLastAVAction,
                                     prAVActionData
                                     );

    switch ( uMainAction )
        {
        case MAINACTION_REPAIR:

            return ( AVACTION_REPAIR );

        case MAINACTION_CONTINUE:

            return ( AVACTION_CONTINUE );

        case MAINACTION_CONTINUE_NOTIFY:

                                        // Only notify that boot is infected,
                                        // but don't allow any actions.

            if ( uLastAVAction == AVACTION_FIRST )
                {
                ppvBootInfectedMessage = bWeAreShuttingDown ?
                    apvInfectedBootFoundNotifyRemoveDisk :
                    apvInfectedBootFoundNotify;
                }
            else if ( uLastAVAction == AVACTION_REPAIR )
                {
                ppvBootInfectedMessage = bWeAreShuttingDown ?
                    apvInfectedBootRepairFailedRemoveDisk :
                    apvInfectedBootRepairFailed;
                }
            else
                SYM_ASSERT ( FALSE );

            uPromptActions = fPROMPTACTION_CONTINUE;

            break;

        case MAINACTION_SHUTDOWN_NOTIFY:

                                        // Notify that the system will be
                                        // shut down.

            ppvBootInfectedMessage = apvInfectedBootFoundShutdownNotify;

            uPromptActions = fPROMPTACTION_OK;

            break;

        case MAINACTION_PROMPT:

                                        // Select a message to display when
                                        // prompting user for action.

            switch ( uLastAVAction )
                {
                case AVACTION_FIRST:

                    ppvBootInfectedMessage = bWeAreShuttingDown ?
                        apvInfectedBootFoundRemoveDisk :
                        apvInfectedBootFound;

                    break;

                case AVACTION_REPAIR:

                    ppvBootInfectedMessage = bWeAreShuttingDown ?
                        apvInfectedBootRepairFailedRemoveDisk :
                        apvInfectedBootRepairFailed;

                    break;

                default:

                    SYM_ASSERT ( FALSE );

                    ppvBootInfectedMessage = apvInfectedBootFound;
                }

                                        // Get prompt actions allowed.

            uPromptActions = NAVGetPromptActions (
                                 AVCONTEXT_SCANBOOT,
                                 prAPCfg(hEventPacket)->uScanDevicePrompts,
                                 uLastAVAction,
                                 prAVActionData
                                 );

            break;

        default:

            SYM_ASSERT ( FALSE );

            return ( AVACTION_STOP );
        }

                                        // If we are here, then user
                                        // intervention is required.

    hEventPacket->prEDExtra->bUserIntervention = TRUE;

                                        // Get virus name.

    UIGetVirusName ( hVLVirus, szVirusName );

                                        // Create parameter table.

    apvParameters[_UI_VIRUS_NAME]      = szVirusName;
    apvParameters[_UI_DRIVE_IN_TROUBLE] = EDXpszOEMDevice (
                                             hEventPacket->prEDExtra
                                             );

                                        // Allocate memory for message and
                                        // buttons.

    pszText = (PTSTR) MemAllocPtr ( GHND,
                                    GetBuildTextLength (
                                        ppvBootInfectedMessage,
                                        apvParameters ) );

    pszButtons = (PTSTR) MemAllocPtr ( GHND,
                                       GetBuildButtonsLength (
                                           uPromptActions ) );

    if ( prAPCfg(hEventPacket)->bAudibleAlert )
        {
        ScheduleSoundAlert ( SOUND_KNOWN_VIRUS,
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
            UIShowLongCustomAlert ( hEventPacket, AMB_COLOR_RED );
            }

                                        // Build message.

        BuildText ( ppvBootInfectedMessage, apvParameters, pszText );

                                        // Build buttons.

        BuildButtons ( pszButtons, uPromptActions );

                                        // Display warning message.

        uResult = (AVACTION) NAVAPMessage (
                                &rBootInfectedMessageBoxRect,
                                prAPCfg(hEventPacket)->uAlertTimeOut,
                                GetDefaultButton(pszButtons),
                                AMB_COLOR_RED,
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

    if ( uMainAction == MAINACTION_SHUTDOWN_NOTIFY )
        {
        uResult = AVACTION_SHUTDOWN;
        }

    return ( uResult );
    }

#endif // #if defined(SYM_VXD)

//---------------------------------------------------------------------------
//
// AVACTION UIMemoryInfected (
//      HEVENTPACKET  hEventPacket,
//      HVLVIRUS      hVLVirus,
//      AVACTION      uLastAVAction,
//      BOOL          bLastActionCompleted,
//      PAVACTIONDATA prAVActionData
//      );
//
// This function is called when memory is found to be infected and either
// the Kernel needs to query the UI for an action to be taken or the user
// has to be notified that NAVAP will not load and system will shut down.
// It first sends a packet to user mode with the status of the previous
// action taken, then depending on that status it either returns to caller
// with AVACTION_LAST or returns a new action determined by the current
// configuration or user input.
//
// Entry:
//      hEventPacket         - handle to current event packet, or NULL
//                             if called during startup.
//      hVLVirus             - handle to virus found
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
#if defined(SYM_VXD)

AVACTION UIMemoryInfected (
    HEVENTPACKET  hEventPacket,
    HVLVIRUS      hVLVirus,
    AVACTION      uLastAVAction,
    BOOL          bLastActionCompleted,
    PAVACTIONDATA prAVActionData
    )
    {
    auto TCHAR       szVirusName[SIZE_VIRUSNAME + 1];
    auto void        *apvParameters[_UI_PARAMETER_TABLE_SIZE];
    auto PTSTR       pszText, pszButtons;
    auto BOOL        bLocalEventPacket;
    auto EVENTPACKET rEventPacket;
    auto EDEXTRA     rEDExtra;

                                        // Validate input parameters.

    SYM_ASSERT ( hVLVirus );
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

    UISendAsyncInformation ( hEventPacket,
                             AVCONTEXT_SCANMEMSTARTUP,
                             uLastAVAction,
                             bLastActionCompleted,
                             TRUE,
                             hVLVirus );

                                        // Get virus name.

    UIGetVirusName ( hVLVirus, szVirusName );

                                        // Create parameter table.

    apvParameters[_UI_VIRUS_NAME] = szVirusName;

                                        // Allocate memory for message and
                                        // buttons.

    pszText = (PTSTR) MemAllocPtr ( GHND,
                                    GetBuildTextLength ( apvInfectedMemoryFoundStartupNotify,
                                                         apvParameters ) );

    pszButtons = (PTSTR) MemAllocPtr ( GHND,
                                       GetBuildButtonsLength ( fPROMPTACTION_OK ) );

    if ( prAPCfg(hEventPacket)->bAudibleAlert )
        {
        ScheduleSoundAlert ( SOUND_KNOWN_VIRUS,
                             prAPCfg(hEventPacket)->uAudibleAlertDelay );
        }

    if ( pszText && pszButtons )
        {
                                        // Show the long custom alert
                                        // message.

        UIShowLongCustomAlert ( hEventPacket, AMB_COLOR_RED );

                                        // Build message to notify that
                                        // the system will be shut down.

        BuildText ( apvInfectedMemoryFoundStartupNotify, apvParameters, pszText );

                                        // Build buttons.

        BuildButtons ( pszButtons, fPROMPTACTION_OK );

                                        // Display warning message.

        NAVAPMessage ( &rMemoryInfectedMessageBoxRect,
                       prAPCfg(hEventPacket)->uAlertTimeOut,
                       GetDefaultButton(pszButtons),
                       AMB_COLOR_RED,
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

                                        // Return an AVACTION of SHUTDOWN.

    return ( AVACTION_SHUTDOWN );
    }

#endif // #if defined(SYM_VXD)
