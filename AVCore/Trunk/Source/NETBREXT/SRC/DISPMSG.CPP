/////////////////////////////////////////////////////////////////////
// DISPMSG.CPP - Created - 11/06/96
//
// Copyright 1992-1996 Symantec, Peter Norton Product Group
/////////////////////////////////////////////////////////////////////
// $Header:   S:/NETBREXT/VCS/dispmsg.cpv   1.0   21 Nov 1997 11:51:58   KSACKIN  $
//
// Description:
//    This file will contain the code to display various types of
//    messages based on the parameters that you send it.  One method
//    would be to send a string ID, while another could be a string
//    and a dword to substitute.  This will load the standard
//    error title for each type of message display.
//
/////////////////////////////////////////////////////////////////////
//
// $Log:   S:/NETBREXT/VCS/dispmsg.cpv  $
// 
//    Rev 1.0   21 Nov 1997 11:51:58   KSACKIN
// Initial revision.
// 
//    Rev 1.0   18 Apr 1997 14:02:54   DSACKIN
// Ported from Tesla
// 
//    Rev 1.0   13 Mar 1997 15:16:30   RFULLER
// Initial revision
// 
//    Rev 1.0   20 Nov 1996 01:49:10   ksackin
// Initial revision.
//
/////////////////////////////////////////////////////////////////////

#include "stdafx.h"


/////////////////////////////////////////////////////////////////////
// DisplayErrorMessage
//
// Purpose:
//    Displays a custom error message using a string ID for the
//    message text, as well as for the message box caption.  This
//    function was designed to make it easier to add custom messages
//    without having to format your own strings and load the string
//    resources.
//
// Parameters:
//    HWND  hParent               - [IN] Owner of this message box.
//    DWORD dwMessageID           - [IN] Message ID for string resouce.
//    DWORD dwMessageBoxCaptionID - [IN] Message Caption ID for string
//                                       resource (IDS_STD_ERROR_MESSAGE_CAPTION
//                                       is default).
//
// Returns:
//    None.
//
// See Also:
//
/////////////////////////////////////////////////////////////////////
// 11/05/96 - KSACKIN - Function Created.
/////////////////////////////////////////////////////////////////////
void DisplayErrorMessage(
    HWND  hParent,
    DWORD dwMessageID,
    DWORD dwMessageBoxCaptionID   // = IDS_STD_ERROR_MESSAGE_CAPTION
    )
{
CString cszMessageCaption;
CString cszMessageText;

                                        // Load string for the Message Caption.
    if ( cszMessageCaption.LoadString( dwMessageBoxCaptionID ) )
        {
                                        // Load string for the Message Text.
        if ( cszMessageText.LoadString( dwMessageID ) )
            {
                                        // Display the custom message box.
            MessageBox( hParent, cszMessageText, cszMessageCaption, MB_OK | MB_ICONEXCLAMATION );
            }
        else
            {
                                        // Assert if we could not load
                                        // the message box string.
            ASSERT( 0 );
            }
        }
    else
        {
                                        // Assert if we could not load
                                        // the message box string.
        ASSERT( 0 );
        }
}
