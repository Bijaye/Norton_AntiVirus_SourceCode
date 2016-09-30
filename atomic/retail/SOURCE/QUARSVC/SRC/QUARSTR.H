// Copyright 1998 Symantec Corporation.
//*************************************************************************
// QuarStr.h - created 3/4/98 3:43:32 PM
//
// $Header:   S:/QUARSVC/VCS/QuarStr.h_v   1.1   29 Jun 1998 20:05:32   sedward  $
//
// Description: String resource and error ID include file for QuarSvc.
//
//*************************************************************************
// $Log:   S:/QUARSVC/VCS/QuarStr.h_v  $
// 
//    Rev 1.1   29 Jun 1998 20:05:32   sedward
// Added IDS_RPC_SERVICE.
//
//    Rev 1.0   23 Mar 1998 13:25:08   SEDWARD
// Initial revision.
//
//    Rev 1.0   05 Mar 1998 13:02:16   JALLEE
// Initial revision.
//*************************************************************************

#ifndef __QUARSTR_H__
#define __QUARSTR_H__

#if !defined(__SERVICE_H)
#  include "service.h"
#endif


// This project is not performing event logging, but let's leave all
// the comments in place just in case we change our minds. jallee
// #include "QuarMsg.h"                     // compiled from QuarMsg.mc

// Error Strings and Error ID's
//
// Special handling is needed for String Table vs. MC Resource
//
// I'm representing these in HEX for easier conversion to the MC ID
//   MC ID = ERROR_ID | ID_ERR_MCBASE (0x8000)
//
// Example:
//
// String ID for IDS_REGOPEN_FAILED:   0x202
// MC ID for IDS_REGOPEN_FAILED:      0x8202
//
// CService::ErrorHandler() expects this convention.  We have a duplicate
// or at least analogous set of strings, one used to log to the event log,
// the other used for message boxes.
//

// For this project, string IDs in the range 0x200 - 0x300 are error messages
// which may be both displayed and written to the event log.


// This project is not performing event logging.
// #define IDS_ERR_TITLE (IDM_ERR_TITLE & ~ID_ERR_MCBASE)  //( 0x200 ) 512

/////////////////////////////////////////////////////////////////////
// Error Handling IDs
//
// For each possible error we also need to define an error ID that
// will determine how the error is handled.
// From service.h
// CService::ErrorHandler() provides the following functionality:
// The base message ID will only be the low word of the error ID.
// the high word can be used to set flags as to how the message should
// be displayed or reported.
// #define ID_ERR_LOGEVENT   (0x10000) // Event will be added to event log
// #define ID_ERR_MESSAGEBOX (0x20000) // Will display a message box notification
// #define ID_ERR_FATAL      (0x40000) // Will abort our service


// This project is not performing event logging.
// #define     ID_ERR_TITLE    (IDS_ERR_TITLE | ID_ERR_ALL)

/////////////////////////////////////////////////////////////////////
// Generic strings

#define     IDS_QUAR_SERVICE_NAME                   (0x300)

#define     IDS_ERR_REGISTERSERVICECTRLR_FAILED     (0x350)
#define     IDS_RPC_SERVICE                         (0x351)



#endif //__QUARSTR_H__


