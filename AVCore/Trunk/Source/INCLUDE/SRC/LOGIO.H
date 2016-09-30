// Copyright 1993 Symantec, Peter Norton Group
//**********************************************************************
//
// $Header:   S:\include\vcs\logio.h_v   1.1   20 Dec 1994 17:12:26   MARKL  $
//
// Description: Prototypes and data structures for logio.c
//
// Kedit:Editv Setf fext CPP
// See Also:
//  symaadd.c
//**********************************************************************
//
// $Log:   S:\include\vcs\logio.h_v  $
// 
//    Rev 1.1   20 Dec 1994 17:12:26   MARKL
// Fixed where SYMALOG was writing UINTs to the disk.  This doesn't work where
// 16 and 32 bit components have to share the same log file and be able to
// read/write each other's data.  These UINTs were changed to type WORD.
// 
//    Rev 1.0   07 Dec 1994 09:50:16   MFALLEN
// Initial revision.
// 
//    Rev 1.1   22 Aug 1994 16:57:34   JMILLARD
// use new logio.h after all
// 
//    Rev 1.17   26 Apr 1994 16:41:38   BRAD
// Removed PVCS Logs
//**********************************************************************/

#ifndef _LOGIO_H
#define _LOGIO_H


// The following are from scanio.h in VISHNU since we are using the FIL4_*
// functions in scanio.c for VISHNU.  When logio.c is properly quaked
// and uses FileOpen, etc. we can get rid of them.

#define NO_HANDLE   0                   // temporary
#define SEEK_ERROR  0xffffffff          // temporary

/**********************************************************************
** Temporarily for compatibility
**********************************************************************/
#define ALogioInit             LogioInit
#define ALogOpen               LogOpen
#define ALogClose              LogClose
#define ALogRecWrite           LogRecWrite
#define ALogRecRead            LogRecRead
#define ALogRecSeek            LogRecSeek
#define ALogSeek               LogSeek
#define ALogRecTell            LogRecTell
#define ALogTell               LogTell
#define ALogGetMaxEntries      LogGetMaxEntries
#define ALogSetMaxEntries      LogSetMaxEntries
#define ALogGetMaxEntryAge     LogGetMaxEntryAge
#define ALogSetMaxEntryAge     LogSetMaxEntryAge
#define ALogGetCurrentEntries  LogGetCurrentEntries


// error codes

#define NAVLOG_ERR_FILE     1           // open/create error
#define NAVLOG_ERR_HANDLE   2           // bad handle
#define NAVLOG_ERR_SEEK     3           // seek error
#define NAVLOG_ERR_READ     4           // read error
#define NAVLOG_ERR_WRITE    5           // write error
#define NAVLOG_ERR_MEMORY   6           // memory error
#define NAVLOG_ERR_VERSION  7           // version mismatch
#define NAVLOG_ERR_BAD_ARG  8           // bad argument
#define NAVLOG_ERR_NO_SPACE 9           // no disk space

// the logfile header
#define ALOG_VERSION 0x00000100l        // Current header version

typedef struct tagLOGHEADER
    {
    DWORD   Version;                    // ALOG_VERSION
    DWORD   dwFirstEntry;               // file offset of first record
    DWORD   dwLastEntry;                // file offset of last record
                                        // &? Necessary|!!!!!!!!!!!!!!!!!!!!

    DWORD   dwMaxSize;                  // maximum log file size
    WORD    wMaxEntries;                // maximum number of log entries
    WORD    wCurrentEntries;            // current number of log entries
    WORD    wMaxDays;                   // maximum days to keep logfile entries
    } LOGHEADER;

typedef LOGHEADER NEAR  *NPLOGHEADER;
typedef LOGHEADER FAR   *LPLOGHEADER;


// each record has an associated header

typedef struct tagLOGRECORD
    {
    DWORD   dwNextEntry;                // file offset of next record
    DWORD   dwPrevEntry;                // file offset of previous record
    WORD    wType;                      // record type ID (see ALOG_FT_...)
    DWORD   dwKey;                      // field existence identifier flags
                                        //    (or other quick data identifier)
    WORD    wRecSize;                   // size of this record's data
    } LOGRECORD;

typedef LOGRECORD NEAR  *NPLOGRECORD;
typedef LOGRECORD FAR   *LPLOGRECORD;


// dwNextEntry is set to NAVLOG_EOF in last record

#define NAVLOG_EOF  ((DWORD)-1)


// in-core structure for every open log file

typedef struct tagLOGFILE
    {
    HFILE       wHandle;                // file handle
    WORD        wCurRecNum;             // current record number
    DWORD       dwCurOff;               // current file offset
    BOOL        LastRec;                // Last record to be read
    LOGHEADER   Loghdr;                 // header for this file
    } LOGFILE;

typedef LOGFILE NEAR    *NPLOGFILE;
typedef LOGFILE FAR     *LPLOGFILE;



#ifdef __cplusplus
extern "C" {
#endif

//
// prototypes
//

BOOL SYM_EXPORT WINAPI SetLogFileSize (LPSTR lpLogFileName,
                                   DWORD dwNewSize);

VOID SYM_EXPORT WINAPI ALogioInit(VOID);     // initialize log i/o package

WORD SYM_EXPORT WINAPI ALogOpen(
    LPSTR   szLogfile,                  // name of logfile
    WORD    wMode,                      // open mode
    LPUINT  pwHandle,                   // handle returned
    BOOL    bCreate,                    // Try to create if doesn't exist
    DWORD   dwMaxSize);                 // max size to use if create

WORD SYM_EXPORT WINAPI ALogClose(
    UINT    wHand);                     // file handle

WORD SYM_EXPORT WINAPI ALogRecWrite(        // Append a record to log file
    UINT    wHand,                      // Log file handle
    WORD    wType,                      // Record type
    DWORD   dwKey,                      // Data key
    WORD    wSize,                      // data size
    LPBYTE  byData);                    // log record data

int SYM_EXPORT WINAPI ALogRecReadHdr(       // Read a record
    UINT    wHand,                      // In: file handle
    LPWORD  wType,                      // Out: Record type
    LPDWORD dwKey,                      // Out: Data key
    LPWORD  wSize,                      // Out: size of record
    WORD    wRecNo);                    // Record # to read. 0 to disable

int SYM_EXPORT WINAPI ALogRecReadData(      // Read record data
    UINT  hLog,                         // Log file "handle"
    WORD  Length,                       // Amount of data to read
    LPSTR lpData,                       // Buffer to fill
    WORD  wRecNo);                      // Record # to read. 0 to disable

WORD SYM_EXPORT WINAPI ALogRecSeek(
    UINT    wHand,                      // file handle
    DWORD   dwRecNum);                  // record number to seek to

WORD SYM_EXPORT WINAPI ALogSeek(
    UINT    wHand,                      // file handle
    DWORD   dwOff);                     // file offset

DWORD SYM_EXPORT WINAPI ALogRecTell(
    UINT    wHand);                     // file handle

DWORD SYM_EXPORT WINAPI ALogTell(
    UINT    wHand);                     // file handle


DWORD SYM_EXPORT WINAPI ALogGetMaxEntries(
    UINT    wHand);                     // file handle

int SYM_EXPORT WINAPI ALogSetMaxEntries(
    UINT    wHand,                      // file handle
    DWORD   dwMaxEntries);               // new max entries

DWORD SYM_EXPORT WINAPI ALogGetMaxEntryAge(
    UINT    wHand);                     // file handle

int SYM_EXPORT WINAPI ALogSetMaxEntryAge(
    UINT    wHand,                      // file handle
    DWORD   dwMaxDays);                  // new max days

WORD SYM_EXPORT WINAPI ALogGetCurrentEntries(
    UINT    wHand);                     // file handle

int SYM_EXPORT WINAPI ALogHeader(           // Read or write log header
    UINT        wHand,                  // File handle
    BOOL        bWrite,                 // TRUE: write; FALSE: read
    LPLOGHEADER LogHdr);                // I/O:  structure to fill

HFILE SYM_EXPORT WINAPI GetTrueHandle (UINT hLog);

#ifdef __cplusplus
}
#endif

#endif  // _LOGIO_H
