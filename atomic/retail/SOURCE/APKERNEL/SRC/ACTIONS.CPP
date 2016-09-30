// Copyright 1994 - 1998 Symantec, Peter Norton Product Group
//***************************************************************************
//
// $Header:   S:/APKERNEL/VCS/Actions.cpv   1.5   16 Mar 1998 14:26:04   DALLEE  $
//
// Description:
//      This file contains functions for the logic of NAVAP actions.
//
// Contains:
//      NAVGetMainAction()
//      NAVGetPromptActions()
//
// See Also:
//      actions.h
//
// Note:
//      Kernel module
//
//***************************************************************************
// $Log:   S:/APKERNEL/VCS/Actions.cpv  $
// 
//    Rev 1.5   16 Mar 1998 14:26:04   DALLEE
// NAVGetMainAction() now takes pointer to sequential list of actions
// to take in response to events.
// 
//    Rev 1.4   19 Feb 1998 15:48:22   RStanev
// Added quarantine support.
//
//    Rev 1.3   18 Feb 1998 21:44:34   RStanev
// Removed unknown virus detection.
//
//    Rev 1.2   17 Feb 1998 20:25:02   RStanev
// Updated copyright.
//
//    Rev 1.1   15 May 1997 01:50:50   RStanev
// Minor changes.
//
//    Rev 1.0   26 Feb 1997 16:39:38   RSTANEV
// Initial revision.
//
//***************************************************************************

#include "platform.h"
#include "navap.h"

//---------------------------------------------------------------------------
//
// MAINACTION NAVGetMainAction (
//      AVCONTEXT     uAVContext,
//      PMAINACTION   puMainActions,
//      AVACTION      uLastAVAction,
//      PAVACTIONDATA prAVActionData
//      );
//
// This function examines the current NAVAP status, which is passed as
// parameters to the function, and returns a main action to be taken by
// the caller.  This function should not be called if this is not the first
// action, or the last action had not failed.
//
// puMainActions is an ordered list of actions to take in response to events. 
// If an action fails, the following action will be attempted. 
// Exception: if MAINACTION_PROMPT is selected, the user will be prompted 
// until the event is resolved.
// Lists must be terminated by MAINACTION_LAST. If MAINACTION_LAST is reached,
// an appropriate final action (continue, stop, deny access) will be 
// selected based on the context of the current event.
// 
// MAX_RESPONSE_ACTIONS defines the maximum number of sequential actions which
// may be listed, including the terminating non-action MAINACTION_LAST.
//
// Entry:
//      uAVContext     - current NAVAP context
//      puMainActions  - sequence of actions which NAVAP is configured to take
//      uLastAVAction  - contains the failed action, or AVACTION_FIRST if
//                       first call to the function.
//      prAVActionData - pointer to internally maintained structure to keep
//                       track of the status of the actions.
//
// Exit:
//      Returns an action to take.
//
// Notes:
//      This function assumes that the following actions will never fail:
//          MAINACTION_CONTINUE
//          MAINACTION_CONTINUE_NOTIFY
//          MAINACTION_DENYACCESS
//          MAINACTION_DENYACCESS_NOTIFY
//          MAINACTION_SHUTDOWN_NOTIFY
//          MAINACTION_ALLOW
//          MAINACTION_DONOTALLOW_NOTIFY
//
//---------------------------------------------------------------------------
MAINACTION NAVGetMainAction (
    AVCONTEXT     uAVContext,
    PMAINACTION   puMainActions,
    AVACTION      uLastAVAction,
    PAVACTIONDATA prAVActionData
    )
    {
    auto MAINACTION uResult;
    auto int        i;

                                        // Validate input parameters.

    SYM_ASSERT ( prAVActionData );

                                        // Get main action.

    if ( uLastAVAction == AVACTION_FIRST )
        {
                                        // If first call to the function,
                                        // result should be the first 
                                        // uMainAction. Must be valid action.

        SYM_ASSERT ( MAINACTION_LAST != *puMainActions );

        uResult = *puMainActions;
        }
    else if ( prAVActionData->uLastMainAction == MAINACTION_PROMPT )
        {
        uResult = MAINACTION_PROMPT;
        }
    else
        {
                                        // Find index of last action. Looking at
                                        // MAX_RESPONSE_ACTIONS - 1 actions since
                                        // last action must be MAINACTION_LAST.

        for ( i = 0; 
              ( i < MAX_RESPONSE_ACTIONS - 1 ) && ( MAINACTION_LAST != puMainActions[i] );
              i++ )
            {
            if ( prAVActionData->uLastMainAction == puMainActions[i] )
                break;
            }

        SYM_ASSERT ( prAVActionData->uLastMainAction == puMainActions[i] );

                                        // Get next action.
        uResult = puMainActions[i + 1];
        }

    if ( MAINACTION_LAST == uResult )
        {
                                        // Get closing action to take based
                                        // on current context.

        switch ( uAVContext )
            {
            case AVCONTEXT_SCANBOOT:
            case AVCONTEXT_SCANMBR:

                uResult = MAINACTION_CONTINUE_NOTIFY;

                break;

            case AVCONTEXT_HDFORMAT:
            case AVCONTEXT_HDMBRWRITE:
            case AVCONTEXT_HDBOOTWRITE:
            case AVCONTEXT_FDBOOTWRITE:
            case AVCONTEXT_WRITETOFILE:
            case AVCONTEXT_FILEROATTRCHANGE:

                uResult = MAINACTION_DONOTALLOW_NOTIFY;

                break;

            case AVCONTEXT_SCANFILE:

                uResult = MAINACTION_DENYACCESS;

                break;

            default:
                
                SYM_ASSERT ( FALSE );

                uResult = MAINACTION_PROMPT;

                break;
            }
        }

#if defined ( SYM_DEBUG ) || defined ( SYM_PRERELEASE )
    switch ( uResult )
        {
        case MAINACTION_REPAIR:

            SYM_ASSERT ( uAVContext == AVCONTEXT_SCANFILE ||
                         uAVContext == AVCONTEXT_SCANBOOT ||
                         uAVContext == AVCONTEXT_SCANMBR );

            break;

        case MAINACTION_DELETE:
        case MAINACTION_MOVE:
        case MAINACTION_MOVE_NOTIFY:
        case MAINACTION_QUARANTINE:
        case MAINACTION_RENAME:
        case MAINACTION_RENAME_NOTIFY:

            SYM_ASSERT ( uAVContext == AVCONTEXT_SCANFILE );

            break;

        case MAINACTION_ALLOW:
        case MAINACTION_DONOTALLOW_NOTIFY:

            SYM_ASSERT ( uAVContext == AVCONTEXT_HDFORMAT    || 
                         uAVContext == AVCONTEXT_HDMBRWRITE  || 
                         uAVContext == AVCONTEXT_HDBOOTWRITE || 
                         uAVContext == AVCONTEXT_FDBOOTWRITE || 
                         uAVContext == AVCONTEXT_WRITETOFILE ||
                         uAVContext == AVCONTEXT_FILEROATTRCHANGE );

            break;
        }
#endif

    prAVActionData->uLastMainAction = uResult;

    return ( uResult );
    }

//---------------------------------------------------------------------------
//
// UINT NAVGetPromptActions (
//      AVCONTEXT     uAVContext,
//      UINT          uActionsAllowed,
//      AVACTION      uLastAVAction,
//      PAVACTIONDATA prAVActionData
//      );
//
// This function examines the current NAVAP status, which is passed as
// parameters to the function, and returns a list of available prompt
// actions to the caller.  This function should not be called if the
// last action had not failed and this is not the first action.
//
// Entry:
//      uAVContext      - context under which the action is taken.
//      uActionsAllowed - fPROMPTACTION_ bits of this variable are set to
//                        actions which NAVAP is configured to take.
//      uLastAVAction   - contains the failed action, or AVACTION_FIRST if
//                        first call to the function.
//      prAVActionData  - pointer to internally maintained structure to keep
//                        track of the status of the actions.
//
// Exit:
//      Returns a variable with fPROMPTACTION_ bits set to actions
//      which the user is allowed to take.
//
// Notes:
//      Currently uAVContext is not used.
//      This function assumes that the following actions will never fail:
//          PROMPTACTION_CONTINUE
//          PROMPTACTION_STOP
//
//---------------------------------------------------------------------------
UINT NAVGetPromptActions (
    AVCONTEXT     uAVContext,
    UINT          uActionsAllowed,
    AVACTION      uLastAVAction,
    PAVACTIONDATA prAVActionData
    )
    {
    auto UINT uResult;

                                        // Validate input parameters.

    SYM_ASSERT ( prAVActionData );

                                        // Get a set of actions.

    if ( uLastAVAction == AVACTION_FIRST )
        {
        prAVActionData->uPromptActionsFailed = 0;
        }
    else
        {
        prAVActionData->uPromptActionsFailed |= (1 << uLastAVAction);
        }

    uResult = uActionsAllowed & (~prAVActionData->uPromptActionsFailed);

    switch ( uAVContext )
        {
        case AVCONTEXT_SCANFILE:

                                        // If for some reason there are no
                                        // actions left, then allow stop
                                        // only.

            if ( uResult == 0 )
                {
                uResult = fPROMPTACTION_STOP;
                }

            break;

        case AVCONTEXT_SCANBOOT:
        case AVCONTEXT_SCANMBR:

                                        // If for some reason there are no
                                        // actions left, then allow continue
                                        // only.

            if ( uResult == 0 )
                {
                uResult = fPROMPTACTION_CONTINUE;
                }

            break;

        case AVCONTEXT_HDFORMAT:
        case AVCONTEXT_HDMBRWRITE:
        case AVCONTEXT_HDBOOTWRITE:
        case AVCONTEXT_FDBOOTWRITE:
        case AVCONTEXT_WRITETOFILE:
        case AVCONTEXT_FILEROATTRCHANGE:

                                        // If exclude failed, allow only
                                        // continue.  No other action from
                                        // virus-like activities can fail,
                                        // thus we will never be without
                                        // buttons.

            if ( uLastAVAction == AVACTION_EXCLUDE )
                {
                uResult = fPROMPTACTION_CONTINUE;
                }

            break;

        default:

            SYM_ASSERT ( FALSE );

            uResult = fPROMPTACTION_STOP;

        }

    return ( uResult );
    }
