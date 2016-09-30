/////////////////////////////////////////////////////////////////////
// DISPMSG.H - Created - 11/06/96
//
// Copyright 1992-1996 Symantec, Peter Norton Product Group
/////////////////////////////////////////////////////////////////////
// $Header:   S:/NETBREXT/VCS/dispmsg.h_v   1.0   21 Nov 1997 11:52:02   KSACKIN  $
//
// Description:
//    This file will define the API's for displaying messages based
//    upon various types of parameters.  This will make displaying
//    messages much more simple, as well as more uniform.  Such
//    things as message box titles and string ID's will be loaded
//    here instead of in the application.
//
/////////////////////////////////////////////////////////////////////
//
// $Log:   S:/NETBREXT/VCS/dispmsg.h_v  $
// 
//    Rev 1.0   21 Nov 1997 11:52:02   KSACKIN
// Initial revision.
// 
//    Rev 1.0   18 Apr 1997 14:03:06   DSACKIN
// Ported from Tesla
// 
//    Rev 1.0   13 Mar 1997 15:16:32   RFULLER
// Initial revision
// 
//    Rev 1.0   20 Nov 1996 02:12:32   ksackin
// Initial revision.
//
/////////////////////////////////////////////////////////////////////

#if !defined(__DISPMSG_H)
#define __DISPMSG_H


/////////////////////////////////////////////////////////////////////
// Function Prototypes.

                                        // Display a standard error
                                        // message box.
void DisplayErrorMessage(
    HWND  hParent,
    DWORD dwMessageID,
    DWORD dwMessageBoxCaptionID = IDS_STD_ERROR_MESSAGE_CAPTION
    );



#endif // if !defined(__DISPMSG_H)
