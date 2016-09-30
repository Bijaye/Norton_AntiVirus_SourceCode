// Copyright 1996 - 1998 Symantec, Peter Norton Product Group
//***************************************************************************
//
// $Header:   S:/APUI/VCS/ASYNCMSG.CPv   1.2   17 Feb 1998 20:53:48   RStanev  $
//
// Description:
//      This file defines a function to replace AsyncMsgBox() for
//      platforms where SymEvent does not implement this API.
//
// Contains:
//      NAVAPAsyncMessage()
//
// See Also:
//      apui.h
//
// Note:
//      UI module
//
//***************************************************************************
// $Log:   S:/APUI/VCS/ASYNCMSG.CPv  $
// 
//    Rev 1.2   17 Feb 1998 20:53:48   RStanev
// Updated copyright.
//
//    Rev 1.1   15 May 1997 02:18:30   RStanev
// Cosmetic changes.
//
//    Rev 1.0   09 Apr 1997 12:15:54   RStanev
// Initial revision.
//
//***************************************************************************

#include "platform.h"
#include "navap.h"
#include "apui.h"

//---------------------------------------------------------------------------
//
// UINT NAVAPAsyncMessage (
//      UINT  uTimeOut,
//      UINT  uDefaultResult,
//      PTSTR pszTitle,
//      PTSTR pszText,
//      PTSTR pszUser,
//      PTSTR pszButtons
//      );
//
// This function is a !SYM_VXD stub for NAVAPMessage().  I haven't determined
// yet how this function will work.  Possibly, it will send packets to user
// mode.  Currently, it only displays debug messages.
//
// Entry:
//      uTimeOut       - same as in AsyncMsgBox()
//      uDefaultResult - same as in AsyncMsgBox()
//      pszTitle       - same as in AsyncMsgBox()
//      pszText        - same as in AsyncMsgBox()
//      pszUser        - same as in AsyncMsgBox()
//      pszButtons     - same as in AsyncMsgBox()
//
// Exit:
//      Returns the default result.
//
//---------------------------------------------------------------------------
UINT NAVAPAsyncMessage (
    UINT  uTimeOut,
    UINT  uDefaultResult,
    PTSTR pszTitle,
    PTSTR pszText,
    PTSTR pszUser,
    PTSTR pszButtons
    )
    {
#if defined(SYM_NTK)

    auto UNICODE_STRING UnicodeString;
    auto OEM_STRING OemString;

    DbgPrint ( "NAVAPAsyncMsg() called with:\n" );

    RtlInitUnicodeString ( &UnicodeString, pszTitle ? pszTitle : _T("- no pszTitle -") );
    if ( NT_SUCCESS ( RtlUnicodeStringToOemString ( &OemString, &UnicodeString, TRUE ) ) )
        {
        DbgPrint ( OemString.Buffer );
        RtlFreeOemString ( &OemString );
        }
    else
        {
        DbgPrint ( "Unable to convert pszTitle." );
        }
    DbgPrint ( "\n" );

    RtlInitUnicodeString ( &UnicodeString, pszText ? pszText : _T("- no pszText -") );
    if ( NT_SUCCESS ( RtlUnicodeStringToOemString ( &OemString, &UnicodeString, TRUE ) ) )
        {
        DbgPrint ( OemString.Buffer );
        RtlFreeOemString ( &OemString );
        }
    else
        {
        DbgPrint ( "Unable to convert pszText." );
        }
    DbgPrint ( "\n" );

    RtlInitUnicodeString ( &UnicodeString, pszUser ? pszUser : _T("- no pszUser -") );
    if ( NT_SUCCESS ( RtlUnicodeStringToOemString ( &OemString, &UnicodeString, TRUE ) ) )
        {
        DbgPrint ( OemString.Buffer );
        RtlFreeOemString ( &OemString );
        }
    else
        {
        DbgPrint ( "Unable to convert pszUser." );
        }
    DbgPrint ( "\n" );

    RtlInitUnicodeString ( &UnicodeString, pszButtons ? pszButtons : _T("- no pszButtons -") );
    if ( NT_SUCCESS ( RtlUnicodeStringToOemString ( &OemString, &UnicodeString, TRUE ) ) )
        {
        DbgPrint ( OemString.Buffer );
        RtlFreeOemString ( &OemString );
        }
    else
        {
        DbgPrint ( "Unable to convert pszButtons." );
        }
    DbgPrint ( "\n" );

    DbgPrint ( "Returning default result.\n" );

    return ( uDefaultResult );

#else

                                        // No specific implementation for
                                        // other platforms.

    SYM_ASSERT ( FALSE );

    return ( uDefaultResult );

#endif
    }
