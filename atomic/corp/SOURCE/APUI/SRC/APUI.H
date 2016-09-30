// Copyright 1996 - 1998 Symantec, Peter Norton Product Group
//***************************************************************************
//
// $Header:   S:/APUI/VCS/APUI.H_v   1.9   20 Feb 1998 19:33:30   RStanev  $
//
// Description:
//      This file defines NAVAP's UI constants, variables, and prototypes
//      all UI functions.  Some of these functions are exported to other
//      NAVAP modules.
//
// Contains:
//      BuildText()
//      BuildButtons()
//      BuildBoxSize()
//      GetDefaultButton()
//      GetBuildTextLength()            (inline function)
//      GetBuildButtonsLength()         (inline function)
//
//      UIFileInfected()
//      UIInfoScanningBoot()            (SYM_VXD only)
//      UIBootInfected()                (SYM_VXD only)
//      UIMemoryInfected()              (SYM_VXD only)
//      UISelfCheckFailed()
//      UIVirusLikeActivity()           (SYM_VXD only)
//
//      UIInitError()
//      UIControlledInitMessage()
//
//      UIOutOfMemoryError()
//
//      UIShutdownError()
//
//      UIShowLongCustomAlert()
//
//      UIGetVirusName()
//      UIGetUserInfo()                 (only if APCOMM.H already included)
//      UIGetCurrentUserSID()           (SYM_NTK only)
//
//      UISendAsyncInformation()
//
//      ScheduleSoundAlert()            (SYM_VXD only, stub otherwise)
//
//      _Begin_Message_Mode()           (SYM_VXD only)
//      _End_Message_Mode()             (SYM_VXD only)
//      _VKD_Get_Kbd_Owner()            (SYM_VXD only)
//      _VKD_Set_Kbd_Owner()            (SYM_VXD only)
//      _VMD_Get_Mouse_Owner()          (SYM_VXD only)
//      _VMD_Set_Mouse_Owner()          (SYM_VXD only)
//      _VDD_Msg_SetCursPos()           (SYM_VXD only)
//      _VDD_Msg_ClrScrn()              (SYM_VXD only)
//      _VDD_Msg_BakColor()             (SYM_VXD only)
//      _VDD_Msg_ForColor()             (SYM_VXD only)
//      _VDD_Msg_TextOut()              (SYM_VXD only)
//      ClearKeyboardQueue()            (SYM_VXD only)
//      GetKeyPressed()                 (SYM_VXD only)
//      SleepDead()                     (SYM_VXD only)
//
//      NAVAPMessage()                  (inline function)
//      NAVAPAsyncMessage()
//
// See Also:
//      -
//
// Note:
//      UI module
//
//***************************************************************************
// $Log:   S:/APUI/VCS/APUI.H_v  $
// 
//    Rev 1.9   20 Feb 1998 19:33:30   RStanev
// BuildBoxSize() now accounts for pszUser.
//
//    Rev 1.8   19 Feb 1998 18:20:22   RStanev
// Changed NAVAPMessage() to automatically resize the box.
//
//    Rev 1.7   19 Feb 1998 16:41:58   RStanev
// Changed the prototype of UISendAsyncInformation().
//
//    Rev 1.6   19 Feb 1998 15:28:12   RStanev
// Removed unknown virus detection.
//
//    Rev 1.5   17 Feb 1998 20:53:42   RStanev
// Updated copyright.
//
//    Rev 1.4   28 May 1997 12:58:48   RStanev
// Prototyped UIShowLongCustomAlert().
//
//    Rev 1.3   21 May 1997 04:29:08   RStanev
// Exported UISelfCheckFailed() for all platforms.
//
//    Rev 1.2   15 May 1997 02:37:28   RStanev
// Cleaned up.
//
//    Rev 1.1   29 Apr 1997 13:19:08   RStanev
// Fixed compilation errors introduced by AVENGE changes.
//
//    Rev 1.0   09 Apr 1997 12:15:48   RStanev
// Initial revision.
//
//***************************************************************************

#ifndef _APUI_H_
#define _APUI_H_

#include "uistrs.h"

//---------------------------------------------------------------------------
//
// Prototype general functions for building messages.
//
//---------------------------------------------------------------------------
UINT BuildText (
    void  **ppvTextTable,
    void  **ppvParameterTable,
    PTSTR pszText
    );

UINT BuildButtons (
    PTSTR pszButtonLine,
    UINT  uButtonsAllowed
    );

VOID BuildBoxSize (
    LPRECT lprBoxTemplate,
    PTSTR  pszText,
    PTSTR  pszUser,
    PTSTR  pszButtons
    );

UINT GetDefaultButton (
    PTSTR pszButtonLine
    );

inline UINT GetBuildTextLength (
    void** t,
    void** p
    )
    {
    return ( BuildText ( t, p, NULL ) );
    }

inline UINT GetBuildButtonsLength (
    UINT a
    )
    {
    return ( BuildButtons ( NULL, a ) );
    }

//---------------------------------------------------------------------------
//
// Prototype main UI functions.
//
//---------------------------------------------------------------------------
AVACTION UIFileInfected (
    HEVENTPACKET  hEventPacket,
    HVLVIRUS      hVLVirus,
    AVACTION      uLastAVAction,
    BOOL          bLastActionCompleted,
    PAVACTIONDATA prAVActionData
    );

#if defined(SYM_VXD)

VOID UIInfoScanningBoot (
    HEVENTPACKET hEventPacket,
    BOOL         bShowOrHide
    );

AVACTION UIBootInfected (
    HEVENTPACKET  hEventPacket,
    HVLVIRUS      hVLVirus,
    AVACTION      uLastAVAction,
    BOOL          bLastActionCompleted,
    PAVACTIONDATA prAVActionData
    );

AVACTION UIMemoryInfected (
    HEVENTPACKET  hEventPacket,
    HVLVIRUS      hVLVirus,
    AVACTION      uLastAVAction,
    BOOL          bLastActionCompleted,
    PAVACTIONDATA prAVActionData
    );

#endif // #if defined(SYM_VXD)

AVACTION UISelfCheckFailed (
    HEVENTPACKET  hEventPacket,
    UINT          uReason,
    AVACTION      uLastAVAction,
    BOOL          bLastActionCompleted,
    PAVACTIONDATA prAVActionData
    );

#if defined(SYM_VXD)

AVACTION UIVirusLikeActivity (
    HEVENTPACKET  hEventPacket,
    AVCONTEXT     uAVContext,
    AVACTION      uLastAVAction,
    BOOL          bLastActionCompleted,
    PAVACTIONDATA prAVActionData
    );

#endif // #if defined(SYM_VXD)

//---------------------------------------------------------------------------
//
// Prototype initialization UI functions.
//
//---------------------------------------------------------------------------
VOID UIInitError (
    UIINITERROR eError
    );

VOID UIControlledInitMessage (
    UICONTROLLEDINITMESSAGE eMessage
    );

//---------------------------------------------------------------------------
//
// Prototype helper UI functions.
//
//---------------------------------------------------------------------------
VOID UIOutOfMemoryError (
    VOID
    );

VOID UIShutdownError (
    UISHUTDOWNERROR eError
    );

VOID UIShowLongCustomAlert (
    HEVENTPACKET hEventPacket,
    SE_AMB_COLOR eColor
    );

VOID UIGetVirusName (
    HVLVIRUS hVLVirus,
    PTSTR    pszVirusName
    );

#if defined(_APCOMM_H_)

BOOL UIGetUserInfo (
    LPED                     prEventData,
    PNAVAP_USERINFOSUBPACKET prUserInfo
    );

#endif // #if defined(_APCOMM_H_)

#if defined(SYM_NTK)

DWORD UIGetCurrentUserSID (
    PSID  pSID,
    DWORD cbSID
    );

#endif // #if defined(SYM_NTK)

//---------------------------------------------------------------------------
//
// Prototype UI functions for sending asynchronous information packets to
// user mode.
//
//---------------------------------------------------------------------------
VOID UISendAsyncInformation (
    HEVENTPACKET hEventPacket,
    AVCONTEXT    uAVContext,
    AVACTION     uLastAVAction,
    BOOL         bLastActionCompleted,
    BOOL         bFinalActionInSequence,
    HVLVIRUS     hVLVirus
    );

//---------------------------------------------------------------------------
//
// Prototype sound APIs.
//
//---------------------------------------------------------------------------
#if defined(SYM_VXD)

#define SOUND_UNDEFINED_ERROR           20
#define SOUND_KNOWN_VIRUS               5
#define SOUND_UNKNOWN_VIRUS             20
#define SOUND_VIRUS_LIKE_ACTIVITY       20

extern "C" VOID WINAPI ScheduleSoundAlert (
    UINT uSoundStyle,
    UINT uTimeOut
    );

#else

#define ScheduleSoundAlert(s,t) ( (void)0 )

#endif

//---------------------------------------------------------------------------
//
// Declare VxD specific UI functions.
//
//---------------------------------------------------------------------------
#if defined(SYM_VXD)

extern "C" {

VOID WINAPI _Begin_Message_Mode (
    VOID
    );

VOID WINAPI _End_Message_Mode (
    VOID
    );

UINT WINAPI _VKD_Get_Kbd_Owner (
    VOID
    );

VOID WINAPI _VKD_Set_Kbd_Owner (
    UINT uVM
    );

UINT WINAPI _VMD_Get_Mouse_Owner (
    VOID
    );

VOID WINAPI _VMD_Set_Mouse_Owner (
    UINT uVM
    );

VOID WINAPI _VDD_Msg_SetCursPos (
    UINT uPosX,
    UINT uPosY
    );

VOID WINAPI _VDD_Msg_ClrScrn (
    UINT  uScreenAttributes,
    PUINT puSizeX,
    PUINT puSizeY
    );

VOID WINAPI _VDD_Msg_BakColor (
    UINT uBackgroundColor
    );

VOID WINAPI _VDD_Msg_ForColor (
    UINT uForegroundColor
    );

VOID WINAPI _VDD_Msg_TextOut (
    PSTR pszText,
    UINT uTextLen,
    UINT uPosX,
    UINT uPosY
    );

VOID WINAPI ClearKeyboardQueue (
    VOID
    );

BOOL WINAPI GetKeyPressed (
    PWORD pwKeyCode
    );

VOID WINAPI SleepDead (
    UINT uMilliseconds
    );

}

#endif // #if defined(SYM_VXD)

//---------------------------------------------------------------------------
//
// Define NAVAPMessage().
//
// SYM_VXD: This function adjusts the size of the message box, and then
// calls SymEvent's AsyncMsgBox().  In other platforms, it translates into
// a call to the local to NAVAP function NAVAPAsyncMessage().
//
//---------------------------------------------------------------------------
UINT NAVAPAsyncMessage (
    UINT  uTimeOut,
    UINT  uDefaultResult,
    PTSTR pszTitle,
    PTSTR pszText,
    PTSTR pszUser,
    PTSTR pszButtons
    );

inline UINT NAVAPMessage (
    LPRECT a,
    UINT b,
    UINT c,
    SE_AMB_COLOR d,
    LPTSTR e,
    LPTSTR f,
    LPTSTR g,
    LPTSTR h
    )
    {
#if defined(SYM_VXD)

    auto RECT resize;

    resize = *a;

    BuildBoxSize (&resize, f, g, h);

    return ( AsyncMsgBox(&resize,b,c,d,e,f,g,h) );

#else

    return ( NAVAPAsyncMessage(b,c,e,f,g,h) );

#endif
    }

#endif // #ifndef _APUI_H_
