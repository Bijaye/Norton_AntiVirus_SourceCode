/* Copyright 1997 Symantec, Peter Norton Computing Group                */
//////////////////////////////////////////////////////////////////////////
//
// $Header:   S:/INCLUDE/VCS/navcomm.h_v   1.5   21 Jul 1997 01:42:42   BGERHAR  $
//
//
//
//////////////////////////////////////////////////////////////////////////
// Created by: DBUCHES   03-26-97
//////////////////////////////////////////////////////////////////////////
/* $Log:   S:/INCLUDE/VCS/navcomm.h_v  $ */
// 
//    Rev 1.5   21 Jul 1997 01:42:42   BGERHAR
// Removed UM_NAVW_GETRESULT message -- not needed any more
// 
//    Rev 1.4   26 Jun 1997 16:42:00   TIVANOV
// new user msg for the command line NORESULTS
//
//    Rev 1.3   13 Jun 1997 08:48:58   TIVANOV
//
//    Rev 1.2   10 Jun 1997 13:45:46   TIVANOV
// new user define messages between navw and navabout dlg
//
//    Rev 1.1   07 Apr 1997 15:43:20   DBUCHES
// Added NAVCOM_NAVW_FINISHED
//
//    Rev 1.0   28 Mar 1997 16:58:36   DBUCHES
// Initial revision.
#ifndef _NAVCOMM_H_
#define _NAVCOMM_H_

//user defined messages for NAVW32
#define UM_NAVW_GETNUMVIRUS     (WM_USER + 333)
#define UM_NAVW_GETDATEDEF      (WM_USER + 334)

//////////////////////////////////////////////////////////////////////////
// This is the message used by NAVW to communicate with the NAV32 UI
// module.
//
// The WParam value will contain the sub-message type, while the LParam
// value will be used to pass an additional DWORD value for use by the
// sub-message handler.
//
//////////////////////////////////////////////////////////////////////////
#define WM_NAVCOMMUNICATE   (WM_USER + 1)

//////////////////////////////////////////////////////////////////////////
// Sub Messages.
//////////////////////////////////////////////////////////////////////////

// Hide the main window
#define NAVCOM_HIDE         0

// Unhide the main window
#define NAVCOM_UNHIDE       1

// Disable the main window, and all child controls
#define NAVCOM_DISABLE      2

// Enable the main window, and all child controls
#define NAVCOM_ENABLE       3

// Shutdown the main window
#define NAVCOM_QUIT         4

// NAV2W has finished running.
#define NAVCOM_NAVW_FINISHED 5


#endif


