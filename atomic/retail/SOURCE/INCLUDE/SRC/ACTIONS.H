// Copyright 1994 - 1998 Symantec, Peter Norton Product Group
//***************************************************************************
//
// $Header:   S:/INCLUDE/VCS/actions.h_v   1.4   16 Mar 1998 14:07:36   DALLEE  $
//
// Description:
//      This file declares NAVAP's action enumerators and prototypes
//      related functions.
//
// Contains:
//      NAVGetMainAction()
//      NAVGetPromptActions()
//
// See Also:
//      APKERNEL\actions.cpp
//
//***************************************************************************
// $Log:   S:/INCLUDE/VCS/actions.h_v  $
// 
//    Rev 1.4   16 Mar 1998 14:07:36   DALLEE
// Changes for sequences of actions to take:
// Added MAINACTION_LAST and NAVGetMainAction() now takes MAINACTION *.
// 
//    Rev 1.3   19 Feb 1998 14:54:44   RStanev
// Removed unknown virus detection.  Added quarantine support.
//
//    Rev 1.2   18 Feb 1998 22:29:56   RStanev
// Updated copyright.
//
//    Rev 1.1   15 May 1997 00:28:36   RStanev
// Reordered some AVACTION members and updated action function prototypes.
//
//    Rev 1.0   09 Apr 1997 12:29:44   RStanev
// Initial revision.
//
//***************************************************************************

#ifndef _ACTIONS_H_
#define _ACTIONS_H_

//---------------------------------------------------------------------------
//
// AVCONTEXT enumerates all NAVAP contexts.
//
// These contexts are used for determining what actions to take in
// special cases.
//
// Note:
//      This table has to be kept in sync with the functions in actions.cpp.
//
//---------------------------------------------------------------------------
typedef enum
    {
    AVCONTEXT_SCANFILE,
    AVCONTEXT_HDFORMAT,
    AVCONTEXT_HDMBRWRITE,
    AVCONTEXT_HDBOOTWRITE,
    AVCONTEXT_FDBOOTWRITE,
    AVCONTEXT_WRITETOFILE,
    AVCONTEXT_FILEROATTRCHANGE,
    AVCONTEXT_SCANBOOT,
    AVCONTEXT_SCANMBR,
    AVCONTEXT_SCANMEMSTARTUP
    } AVCONTEXT, *PAVCONTEXT;

//---------------------------------------------------------------------------
//
// AVACTION enumerates all NAVAP actions.
//
// The positions of AVACTION_FIRST, AVACTION_LAST should not be changed.
// They can be used in some action schemes for locating and tracking
// actions that have failed.
//
// When adding/deleting/moving AVACTION members, please update the
// string tables which are used to convert AVACTIONs to strings.
//
//---------------------------------------------------------------------------
typedef enum
    {
    AVACTION_FIRST = 0,
    AVACTION_OK,
    AVACTION_STOP,
    AVACTION_CONTINUE,
    AVACTION_REPAIR,
    AVACTION_DELETE,
    AVACTION_EXCLUDE,
    AVACTION_QUARANTINE,
    AVACTION_MOVE,
    AVACTION_RENAME,
    AVACTION_SHUTDOWN,
    AVACTION_LAST
    } AVACTION, *PAVACTION;

//---------------------------------------------------------------------------
//
// fPROMPTACTION_ are used to represent each one of AVACTION's
// members as a bit field.  These flags are used when prompting
// user for action.
//
//---------------------------------------------------------------------------
#define fPROMPTACTION_OK            ( 1 << AVACTION_OK )
#define fPROMPTACTION_STOP          ( 1 << AVACTION_STOP )
#define fPROMPTACTION_CONTINUE      ( 1 << AVACTION_CONTINUE )
#define fPROMPTACTION_REPAIR        ( 1 << AVACTION_REPAIR )
#define fPROMPTACTION_DELETE        ( 1 << AVACTION_DELETE )
#define fPROMPTACTION_EXCLUDE       ( 1 << AVACTION_EXCLUDE )
#define fPROMPTACTION_QUARANTINE    ( 1 << AVACTION_QUARANTINE )
#define fPROMPTACTION_MOVE          ( 1 << AVACTION_MOVE )
#define fPROMPTACTION_RENAME        ( 1 << AVACTION_RENAME )

//---------------------------------------------------------------------------
//
// MAINACTION enumerates NAVAP main actions.  These are all actions which
// NAV(W)'s configuration screens refer to as "Action to take if..."
//
// The position of MAINACTION_LAST should not be changed. It is used to 
// terminate ordered lists of actions to take in response to events.
//
//---------------------------------------------------------------------------
typedef enum
    {
    MAINACTION_PROMPT = 1,
    MAINACTION_CONTINUE,
    MAINACTION_CONTINUE_NOTIFY,
    MAINACTION_DENYACCESS,
    MAINACTION_DENYACCESS_NOTIFY,
    MAINACTION_REPAIR,
    MAINACTION_DELETE,
    MAINACTION_SHUTDOWN_NOTIFY,
    MAINACTION_ALLOW,
    MAINACTION_DONOTALLOW_NOTIFY,
    MAINACTION_QUARANTINE,
    MAINACTION_MOVE,
    MAINACTION_MOVE_NOTIFY,
    MAINACTION_RENAME,
    MAINACTION_RENAME_NOTIFY,
    MAINACTION_LAST
    } MAINACTION, *PMAINACTION;

//---------------------------------------------------------------------------
//
// AVACTIONDATA is a structure allocated by a client, but it is internally
// maintained by the AV-action API.
//
//---------------------------------------------------------------------------
typedef struct
    {
    MAINACTION uLastMainAction;
    UINT       uPromptActionsFailed;
    } AVACTIONDATA, *PAVACTIONDATA;

//---------------------------------------------------------------------------
//
// Function prototypes.
//
//---------------------------------------------------------------------------
MAINACTION NAVGetMainAction (
    AVCONTEXT     uAVContext,
    PMAINACTION   puMainAction,
    AVACTION      uLastAVAction,
    PAVACTIONDATA prAVActionData
    );

UINT NAVGetPromptActions (
    AVCONTEXT     uAVContext,
    UINT          uActionsAllowed,
    AVACTION      uLastAVAction,
    PAVACTIONDATA prAVActionData
    );

#endif // _ACTIONS_H_
