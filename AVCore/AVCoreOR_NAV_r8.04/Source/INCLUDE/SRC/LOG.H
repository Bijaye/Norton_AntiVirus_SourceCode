// Copyright 1993 Symantec, Peter Norton Group
//**********************************************************************
//
// $Header:   S:\include\vcs\log.h_v   1.1   20 Dec 1994 17:12:24   MARKL  $
//
// Description: Prototypes for log.c
//
// See Also:
//
//**********************************************************************
// $Log:   S:\include\vcs\log.h_v  $
// 
//    Rev 1.1   20 Dec 1994 17:12:24   MARKL
// Fixed where SYMALOG was writing UINTs to the disk.  This doesn't work where
// 16 and 32 bit components have to share the same log file and be able to
// read/write each other's data.  These UINTs were changed to type WORD.
// 
//    Rev 1.0   07 Dec 1994 09:50:14   MFALLEN
// Initial revision.
// 
//    Rev 1.1   22 Aug 1994 16:56:50   JMILLARD
// change to UINT file handles for new Win32 style stuff
// 
//    Rev 1.0   15 Aug 1994 16:14:28   GDZIECI
// Baseline Mario from Vishnu with Win32 awk changes applied
// 
//    Rev 1.17   18 Apr 1994 14:02:48   KEITH
// Perform CVT1 for Win32 update.
// 
//    Rev 1.16   28 Feb 1994 19:28:58   BRAD
// Updated for SYM_WIN32
// 
//    Rev 1.15   25 Feb 1994 17:11:42   BRAD
// Applied new AWK defines
// 
//    Rev 1.14   10 Sep 1993 17:48:14   EHU
// Added prototypes for LogfileLock and LogfileUnlock.
// 
//    Rev 1.13   21 Aug 1993 17:46:04   EHU
// Renamed LogError to NAVLogError.  #define LogError NAVLogError until we
// can fix all the instances.
// 
//    Rev 1.12   20 Aug 1993 14:53:44   JMILLARD
// Add LogString
// 
//    Rev 1.11   28 Jun 1993 21:41:46   EHU
// Added protos for LogfileOpen and LogfileClose.
// 
//    Rev 1.10   28 Jun 1993 18:22:06   EHU
// Added prototype for LogError.
// 
//    Rev 1.9   14 Jun 1993 20:27:36   EHU
// Added some function pointer types for pager SYM_NLM.
// 
//    Rev 1.8   25 May 1993 09:31:02   MARKL
// Temporarily hav log.h include symalog.h so we can do a build.
// 
//    Rev 1.7   24 May 1993 23:27:04   EHU
// Changed to use updated logfile routines.
// 
//    Rev 1.6   19 May 1993 22:33:18   EHU
// Updated prototypes.
// 
//    Rev 1.5   22 Apr 1993 12:19:50   EHU
// Added prototype for MsgErrLog.
// 
//    Rev 1.4   16 Apr 1993 13:35:30   JMILLARD
// move DEFAULT_COLOR from monitor.h
// 
//    Rev 1.3   15 Apr 1993 17:39:32   EHU
// Added prototype for Log and took out ones for wLogError, etc.
// 
//    Rev 1.2   15 Apr 1993 17:35:26   DDREW
// No change.
// 
//    Rev 1.1   22 Mar 1993 21:42:14   EHU
// Added some defines and prototypes.
// 
//    Rev 1.0   12 Mar 1993 21:45:36   EHU
// Initial revision.
/************************************************************************/

#ifndef _LOG_H
#define _LOG_H

//  .h files required to include this .h file

#include "symalog.h"


// temp fix until we rename all instances of LogError
#define LogError        NAVLogError


// log record types

#define ERROR_LOG       ALOG_FI_ERROR
#define STATUS_LOG      ALOG_FI_STATUS
#define LOAD_LOG        ALOG_FI_LOADUNLOAD
#define SCAN_LOG        ALOG_FI_STARTEND
#define VIRLIST_LOG     ALOG_FI_VIRLISTCHANGE
#define KNOWNV_LOG      ALOG_FI_KNOWNVIRUS
#define UNKNOWNV_LOG    ALOG_FI_UNKNOWNVIRUS


// PostString defines

#define DEFAULT_COLOR       0

#if 0
// field positions in log record

#define LOGFLD_FIRST        0
#define LOGFLD_DATE         1
#define LOGFLD_TIME         2
#define LOGFLD_MESSAGE      3
#define LOGFLD_SERVER       4
#define LOGFLD_USER         5
#define LOGFLD_NETADDR      6
#define LOGFLD_FTYPE        7
#define LOGFLD_PATH         8
#define LOGFLD_FSIZE        9
#define LOGFLD_VIRUSNAME    10
#define LOGFLD_DIRECTION    11
#define LOGFLD_ACTION       12
#define LOGFLD_LAST         (LOGFLD_ACTION+1)


// codes for fields in log record

#define LOGCODE_FIRST       ":I"        // who & what
#define LOGCODE_DATE        ":D"        // date
#define LOGCODE_TIME        ":T"        // time
#define LOGCODE_MESSAGE     ":M"        // message
#define LOGCODE_SERVER      ":S"        // server
#define LOGCODE_USER        ":U"        // user
#define LOGCODE_NETADDR     ":N"        // net address
#define LOGCODE_FTYPE       ":n"        // name space
#define LOGCODE_PATH        ":P"        // path
#define LOGCODE_FSIZE       ":Z"        // file size
#define LOGCODE_VIRUSNAME   ":V"        // virus name
#define LOGCODE_DIRECTION   ":d"        // direction
#define LOGCODE_ACTION      ":A"        // action


typedef struct
    {
    LPSTR   lpszCode;                   // format code
    LPSTR   lpszText;                   // associated text
    } LOGFIELD, FAR * LPLOGFIELD;
#endif



//**********************************************************************
//
// Prototypes
//
//**********************************************************************

VOID LogEventThread(                    // this function is the thread
    VOID  *pJunk);                      // the event being created

WORD WakenLogEventThread(VOID);

WORD SYM_EXPORT WINAPI WriteLog(
    BYTE        byType,                 // record type
    DWORD       dwFileSize,             // file size, 0 if none
    LPALOGADD   lpLogAdd);              // record to pass to ALogAdd

WORD SYM_EXPORT Log(
    BYTE    byType,                     // log type
    DWORD   dwColor,                    // screen colors
    LPSTR   lpszFormat,                 // FastStringPrint type format
    ...);

WORD SYM_EXPORT LogString(
    BYTE    byType,                     // log type
    DWORD   dwColor,                    // screen colors
    WORD    wStringTableId,             // string table id
    WORD    wStringIndex,               // string id
    ...);

typedef WORD (FAR *LOGFUNC)(
    BYTE    byType,                     // log type
    DWORD   dwColor,                    // screen colors
    LPSTR   lpszFormat,                 // FastStringPrint type format
    ...);

WORD SYM_EXPORT MsgErrLog(
    WORD    wArgCount,                  // argument count
    ...);                               // text strings

WORD SYM_EXPORT NAVLogError(
    HSYMSTR hStr[],                     // master string table list
    UINT    uErrCode,                   // error code
    DWORD   dwTraceId,                  // file/line trace id
    ...);

WORD SYM_EXPORT WINAPI LogfileOpen(
    LPSTR   szLogfile,                  // name of logfile
    WORD    wMode,                      // open mode
    LPUINT  pwHandle,                   // handle returned
    BOOL    bCreate);                   // Try to create if doesn't exist

WORD SYM_EXPORT WINAPI LogfileClose(
    UINT    wHand);                     // file handle

VOID SYM_EXPORT WINAPI LogfileLock(VOID);
VOID SYM_EXPORT WINAPI LogfileUnlock(VOID);

LPSTR SYM_EXPORT WINAPI lpFileSpaceName(UINT);


#endif  // _LOG_H
