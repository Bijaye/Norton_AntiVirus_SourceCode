// Copyright 1995-1997 Symantec, Peter Norton Product Group
//************************************************************************
//
// $Header:   S:/INCLUDE/VCS/msgreg.h_v   1.2   13 Mar 1998 15:32:00   TCASHIN  $
//
// Description:
//      This header contain defines and function prototypes for
//      registering and translating shared custom Windows messages.
//
// Contains:
//
// See Also:
//      msgreg.cpp in NAVW project.
//************************************************************************
// $Log:   S:/INCLUDE/VCS/msgreg.h_v  $
// 
//    Rev 1.2   13 Mar 1998 15:32:00   TCASHIN
// Added the UWM_DISPLAY_OPTIONS message, sent from AP to NAVW to
// put up the options dialog.
// 
//    Rev 1.1   27 Mar 1997 15:49:26   DALLEE
// Removed old NDW drag drop messages.
//************************************************************************

#define WM_STRINGMESSAGE 0xC000
#define TRANSMSG(x) if(x >= WM_STRINGMESSAGE) x=TranslateRegisteredMessage(x);
#define MAKEREG(x) MessageToRegistered(x)

WORD SYM_EXPORT WINAPI TranslateRegisteredMessage(WORD msg);
WORD SYM_EXPORT WINAPI MessageToRegistered(WORD msg);


/*
    The following are the messages registered by this code.
    They have been broken out of their include files in order
    to BREAK the code using them so we can track them all down.
*/

#define UWM_NOP                 (WM_USER + 0x6000)

#define TOTAL_REG_MESSAGES 5

#define UWM_AGENT_CLOSE         (WM_USER + 0x6011)
#define UWM_AGENT_SAVE          (WM_USER + 0x6012)

#define UWM_INI_CHANGE          (WM_USER + 0x6013)

#define UWM_NAVAP_ACTIVATING    (WM_USER + 0x6014)

#define UWM_DISPLAY_OPTIONS     (WM_USER + 0x6015)

