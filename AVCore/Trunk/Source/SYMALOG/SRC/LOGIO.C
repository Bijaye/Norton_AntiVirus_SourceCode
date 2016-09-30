// Copyright 1993 Symantec, Peter Norton Group
//**********************************************************************
//
// $Header:   S:/SYMALOG/VCS/logio.c_v   1.46   12 Aug 1996 13:36:06   PVENKAT  $
//
// Low-level log file i/o routines
//
// Entry Points:
//    ALogioInit(VOID)
//    ALogOpen()
//    ALogClear()
//    ALogClose()
//    ALogRecWrite()
//    ALogRecRead()
//    ALogRecSeek()
//    ALogSeek()
//    ALogRecTell()
//    ALogTell()
//    ALogGetMaxSize()
//    ALogSetMaxSize()
//    ALogGetMaxEntries()
//    ALogSetMaxEntries()
//    ALogGetMaxEntryAge()
//    ALogSetMaxEntryAge()
//    ALogGetCurrentEntries()
//    ALogConsolidate()
//
// Kedit:Editv setf fext CPP
//
// See Also:
//
// Future Enhancements
//&? use ALogHeader() instead of direct I/O of activity log file header
//**********************************************************************
//
// $Log:   S:/SYMALOG/VCS/logio.c_v  $
// 
//    Rev 1.46   12 Aug 1996 13:36:06   PVENKAT
// Fixed #63851.  If Activitylog file is 0 size, then we display, 'no records'
// msg instead of 'unable to open' msg.
// 
// 
//    Rev 1.45   11 Jul 1996 18:01:02   MZAREMB
// Added another SYM_WIN32 #ifdef.
// 
//    Rev 1.44   11 Jul 1996 15:39:52   PVENKAT
// Added #ifdef SYM_WIN32 for NT-Only changes.
// 
// 
//    Rev 1.43   13 Jun 1996 13:57:56   PVENKAT
// Ignoring ERROR_ACCESS_DENIED error, while trying to update Activity log
// Header as the file may be READ-ONLY.  (Without this the # of logs always
// seems to be 4GB!!!).
// 
// 
//    Rev 1.42   30 Jan 1996 17:59:48   MLELE
// Fixed function ALogRecWrite. If writing first record, initialize the
// first position to NAVLOG_EOF.
// 
//    Rev 1.41   26 Jan 1996 15:01:12   MLELE
// Fixed function ALogRecWrite to work for NLMs.
// 
// 
//    Rev 1.40   12 Jan 1996 12:12:30   JWORDEN
// Port changes from Quake 8
// 
//    Rev 1.38.1.2   13 Dec 1995 22:18:04   YVANG
// Under NT: Modified to check to see if the max file size of log file is zero(0)
// which signifies that the log file size limit is not set (i.e. unlimied or
// the log file size limit checkbox in Activity Log Option dialog is unchecked).
// 
//    Rev 1.38.1.1   08 Dec 1995 15:26:52   YVANG
// 1) For NT: Fixed (hopefully) the problem in which the scanner is generating
//            corrupted activity log.
// 2) Added lots of codes writing debugging information to a file to track 
//    the Activity Log records writing operations.  (The codes are controlled by
//    compilation switch.)  
// 
//    Rev 1.38.1.0   12 Jul 1995 19:00:52   MPYE
// Branch base for version QAK8
// 
//    Rev 1.38   09 Feb 1995 16:45:30   JMILLARD
// 
// open always returns a bad handle on error
// 
//    Rev 1.37   20 Dec 1994 17:12:00   MARKL
// Fixed where SYMALOG was writing UINTs to the disk.  This doesn't work where
// 16 and 32 bit components have to share the same log file and be able to
// read/write each other's data.  These UINTs were changed to type WORD.
// 
//    Rev 1.36   04 May 1994 00:57:50   BRAD
// Don't use NFileOpen()
// 
//    Rev 1.35   15 Mar 1994 12:05:24   BRUCE
// Changed EXPORT to SYM_EXPORT
// 
//    Rev 1.34   09 Mar 1994 20:53:58   BRAD
// Fixed WIN32 warnings/errors
// 
//    Rev 1.33   28 Feb 1994 16:20:06   BRAD
// Updated for WIN32.
// 
//    Rev 1.32   10 Feb 1994 14:08:22   MFALLEN
// JMILLARD made some changes to Quake branch 2. These were moved to the trunk.
// The changes look OK.
// 
//    Rev 1.31.1.3   24 Nov 1993 09:49:56   DDREW
// Remove last change
// 
//    Rev 1.31.1.2   23 Nov 1993 18:21:08   JMILLARD
// ifndef SYM_NLM ALogGetMaxSize
// 
//    Rev 1.31.1.1   22 Oct 1993 14:05:20   JMILLARD
// provide subroutines for FileRead and FileWrite, which do RelinquishControl
// 
//    Rev 1.31.1.0   19 Oct 1993 06:23:54   SCOTTP
// Branch base for version QAK2
// 
//    Rev 1.31   13 Oct 1993 14:41:28   EHU
// Oops, apparently ALogAdd uses GetTrueHandle.
// 
//    Rev 1.30   13 Oct 1993 14:34:22   EHU
// Ifdef'd out GetTrueHandle for SYM_NLM.
// 
//    Rev 1.29   12 Oct 1993 16:46:28   PATRICKM
// Baseline from QAK1 branch
// 
//    Rev 1.26.1.3   06 Oct 1993 16:06:22   EHU
// Ifdef'd out ALogRecReadHdr, ALogRecReadData, LogRecSeek, LogTell,
// LogGetMaxEntries, LogGetMaxEntryAge, LogGetCurrentEntries, PrevRecord,
// NextRecord, MoveToRecordHeader for SYM_NLM.
// 
//    Rev 1.26.1.2   18 Sep 1993 19:37:26   MFALLEN
// Changed from Rees's lint utility.
// 
//    Rev 1.26.1.1   15 Sep 1993 20:41:48   BARRY
// Detect errors when closing files and propogate up to caller
// 
//    Rev 1.26.1.0   03 Sep 1993 21:53:46   SCOTTP
// Branch base for version QAK1
// 
//    Rev 1.26   08 Aug 1993 15:14:46   MFALLEN
// Added GetTrueHandle()
// 
//    Rev 1.25   22 Jul 1993 16:27:48   DALLEE
// Made LOGFILE LogFile STATIC since it's used only in this module, and
// it conflicts with a symbol in the DOS netware library.
// 
//    Rev 1.24   15 Jul 1993 13:24:18   MFALLEN
// The functions ALogRecReadHdr() and ALogRecReadData() take an additional
// parameter that specifies what activity log record number to retrieve
// the information from.
// 
//    Rev 1.23   14 Jul 1993 18:12:30   MFALLEN
// PVCS screwed things up when volume ran out of disk space.
// 
//    Rev 1.22   29 Jun 1993 16:11:22   BLEE
// Added comments describing how to implement the (currently incomplete)
// date setting and consolidation routines.
//
//    Rev 1.21   09 Jun 1993 14:28:50   BLEE
//
//    Rev 1.20   03 Jun 1993 19:54:02   BLEE
// Enabled PrevRecord() NextRecord() ALogHeader() ALogTell() ALogRecSeek()
// functions.
//
//    Rev 1.19   01 Jun 1993 16:42:20   BLEE
// Disable NextRecord() and PrevRecord() [not used]
//
//    Rev 1.18   01 Jun 1993 16:04:36   BLEE
// Eliminated goto's from many functions.
// Consolidated valid handle checking in most functions.
// Correctly update file header when setting Max Size, Max Age, and Max Entries.
// Adjust file for Max Size and Max entries.
// Added entry for ALogConsolidate() [functionality still to come].
// Changed some VOID functions to return int.
//
//    Rev 1.17   27 May 1993 18:18:32   BLEE
// [fixed] misplaced Seek()
//
//    Rev 1.16   27 May 1993 16:18:26   BLEE
// Added ALogClear()
//
//    Rev 1.14   24 May 1993 15:18:44   BLEE
// Changed entry point names from Log...() to ALog...().
// (Preserved old names, for a while, using #defines in the header)
//
//    Rev 1.13   22 May 1993 04:15:12   BLEE
// [enh] reimplemented LogRecWrite().
// [enh] changed to accommodate new file and record headers.
//
//    Rev 1.12   07 May 1993 15:56:02   EHU
// Include io.h for SYM_NLM and stop including errno.h.
//
//    Rev 1.11   21 Apr 1993 16:58:08   EHU
// Added code to set max file size to current size and wrap to BOF when
// we run out of disk space.
//
//    Rev 1.10   19 Apr 1993 21:09:54   EHU
// Some more minor fixes.
//
//    Rev 1.9   19 Apr 1993 18:56:32   EHU
// Fixed typo; &LogRec should be byData in LogRecWrite.  Took out ifndef SYM_NLM
// since SYM_NLM will need those functions for resetting logfile size.
//
//    Rev 1.8   19 Apr 1993 14:17:06   BLEE
// More comment additions.
//
// Changed LogRecWrite
// - No more dynamic allocation.  Separate writes of local header and data
//   passed in.
// - Moved check for size of first record exceeding file size to general
//   case of a record exceeding the bottom bound of the file.
//
//    Rev 1.7   17 Apr 1993 10:25:56   BRUCE
// [FIX] Fixed signed/unsigned compiler warnings on DWORD = -1
//
//    Rev 1.6   17 Apr 1993 00:32:08   BLEE
// Changed references to the size of LOGRECORD to only refer to the
// significant fields (sizeof(LOGRECORD)-1).
//
// Deleted exteraneous FileSeek()'s.
//
// Changed WINAPI to PASCAL on static routines.
//
//    Rev 1.5   16 Apr 1993 16:31:24   EHU
// Added handle validation to exported functions.
//
//    Rev 1.3   13 Apr 1993 16:17:32   EHU
// Added function to init data structures.  When creating a file in
// LogOpen, create, close, then open in specified mode.  Fixed some
// bugs.
//
//    Rev 1.3   02 Apr 1993 14:43:50   EHU
// Changed API to LogRecWrite in logio.c.
//
//    Rev 1.2   01 Apr 1993 18:31:18   EHU
// Don't reset file offset in LogRecRead on error for now.
//
//    Rev 1.1   01 Apr 1993 16:46:12   EHU
// Close log file in LogOpen if everything doesn't succeed.
//**********************************************************************
#ifdef SYM_NLM
#   include <io.h>                      // open, close, etc.
#endif

#include "platform.h"                   // Symantec global stuff
#include "symserve.h"   
#include "file.h"                       // Symantec I/O
#include "xapi.h"                       // for SystemGetWindowsType() &
                                        //     SYM_SYSTEM_WIN_NT
#include "logio.h"                      // External declarations

#ifdef VDEBUG
static char szDebugFile[] = "dbg.txt";
HFILE hDbgFile;
char szBuf [1024];
#endif

//**********************************************************************
//
//  Local component prototypes
//
//**********************************************************************
WORD LOCAL PASCAL ReadRecHdr(           // Read record header @ or after
                                        //    dwOffset
    HFILE       hFile,                  // File handle
    DWORD       dwOffset,               // File offset
    LPLOGRECORD lpLogRec);

WORD LOCAL PASCAL WriteRecHdr(
    HFILE       hFile,                  // File handle
    DWORD       dwOffset,
    LPLOGRECORD lpLogRec);

WORD LOCAL PASCAL NextRecord(LPLOGFILE lpFile); // logfile struct
WORD LOCAL PASCAL PrevRecord(LPLOGFILE lpFile); // logfile struct

VOID LOCAL MoveToRecordHeader (
    HFILE       hFile,                  // File handle
    WORD        wRecNo);                // Record number

int LOCAL PASCAL ALogConsolidate(UINT hLog);

//*********************************************************************
//  Local static data
//*********************************************************************
#define MAX_LOG 2                       // Max number of open log files
STATIC LOGFILE LogFile[MAX_LOG];        // Array of log file state blocks


//** ALogioInit() ******************************************************
//
// Initialize data structures.  Must be called before
//               using the logfile i/o package.
//
//**********************************************************************
//  13apr93 efh - New
//  20may93 wrl - Ensure that array does not get overwritten if called
//                more than once.
//**********************************************************************
void SYM_EXPORT WINAPI ALogioInit(void)
{
    static BOOL bCalled=FALSE;
    int i;

    if (bCalled)                        // Already initialized
        return;

    bCalled = TRUE;                     // Let this happen once only

    for (i = 0; i < MAX_LOG; i++)       // Init file handles to "unused"
        {
        LogFile[i].wHandle = HFILE_ERROR;
        }

#ifdef VDEBUG
    if (FileExists(szDebugFile))
        FileDelete(szDebugFile);
    if ((hDbgFile = FileCreate(szDebugFile, FA_NORMAL)) == HFILE_ERROR)
        {
        MessageBox(NULL,"ALogioInit():Debug output file create error.\n",NULL,MB_OK);
        return;
        }
    FileClose(hDbgFile);
#endif


} // ALogioInit()


//** ALogOpen() ********************************************************
//
// Open log file
//
//**********************************************************************
//  26mar93 efh - New
//  20may93 wrl - deleted use of lpLhdr pointer (for performance)
//**********************************************************************
WORD SYM_EXPORT WINAPI ALogOpen(
    LPSTR   szLogfile,                  // name of logfile
    WORD    wMode,                      // open mode
    LPUINT  puHandle,                   // "handle" returned
    BOOL    bCreate,                    // Create if doesn't exist
    DWORD   dwMaxSize)                  // max size to use if create
{
    WORD    i;
    HFILE   wHandle = HFILE_ERROR;
    WORD    wRead;                      // bytes read
    WORD    wErr = NOERR;               // Result status
    DWORD   dwFileLength;               // Length of Activity.Log file.

    *puHandle = (UINT) HFILE_ERROR;     // flag bad handle just in case

    for (i = 0; i < MAX_LOG; i++)       // Search for unused file block
        {
        if (LogFile[i].wHandle == HFILE_ERROR)
            break;
        }

    if (LogFile[i].wHandle != HFILE_ERROR) // No file state blocks left
        {
        wErr = NAVLOG_ERR_HANDLE;
        goto done;
        }

    if (wMode == WRITE_ONLY_FILE)
        wMode = READ_WRITE_FILE;        // need to read file header

    // ********************************************************** //
    // If we are unable to open with the specified flags, and the //
    // user specified that the file should be created if open     //
    // fails, then try to create the file.                        //
    // ********************************************************** //

    if ((wHandle = FileOpen(szLogfile, wMode)) == HFILE_ERROR &&
         wMode != READ_ONLY_FILE && bCreate == TRUE)
        {
        if ((wHandle = FileCreate(szLogfile, FA_NORMAL)) != HFILE_ERROR)
            {
            FileClose(wHandle);                    // Reopen new file
            wHandle = FileOpen(szLogfile, wMode); // with specifed flags
            }
        }
#ifdef  SYM_WIN32
    else
        {
		if (wHandle == HFILE_ERROR)
			{
			DWORD   dwAttr = GetFileAttributes (szLogfile);

			// File should exist here, may be 0 size, or Read Only.
			// To handle the Log file with Read-Only set, we also 
			// open only read-only purpose (even though it could 
			// be 0 sized where we normally write the header).

			if (wMode = READ_WRITE_FILE && 
				( (dwAttr & FILE_ATTRIBUTE_READONLY) == FILE_ATTRIBUTE_READONLY))
				wMode = READ_ONLY_FILE;
			wHandle = FileOpen(szLogfile, wMode); // with specifed flags
			}
        }
#endif

    if (wHandle == HFILE_ERROR)         // Still cant open or cant open
        {                               // with specified flags.
        wErr = NAVLOG_ERR_FILE;
        goto done;
        }

    dwFileLength = FileLength (wHandle);
                                        // Read file header
    wRead = FileRead(wHandle, (LPSTR)&LogFile[i].Loghdr, sizeof(LOGHEADER));
    if (wRead == (WORD) 0 && wMode != (WORD) READ_ONLY_FILE ) 
        {                                      // Create file if empty
                                               // Initialize file header
        LogFile[i].Loghdr.Version         = ALOG_VERSION;
        LogFile[i].Loghdr.dwFirstEntry    = NAVLOG_EOF;
        LogFile[i].Loghdr.dwLastEntry     = 0;
        LogFile[i].Loghdr.dwMaxSize       = dwMaxSize;
        LogFile[i].Loghdr.wMaxEntries     = 0;
        LogFile[i].Loghdr.wCurrentEntries = 0;
        LogFile[i].Loghdr.wMaxDays        = 0;


#ifdef VDEBUG
        if ((hDbgFile = FileOpen(szDebugFile, READ_WRITE_FILE)) == HFILE_ERROR)
            {
            MessageBox(NULL,"ALogRecWrite(1):File Open Error.\n",NULL,MB_OK);
            goto done;
            }

        wsprintf(szBuf,"ALogOpen():\n\r"
        "   loghdr.dwFirstEntry=%ld\n\r"
        "         .dwLastEntry=%ld\n\r"
        "         .dwMaxSize=%ld\n\r"
        "         .wMaxEntries=%d\n\r"
        "         .wCurrentEntries=%d\n\r",
            LogFile[i].Loghdr.dwFirstEntry,
            LogFile[i].Loghdr.dwLastEntry,
            LogFile[i].Loghdr.dwMaxSize,
            LogFile[i].Loghdr.wMaxEntries,
            LogFile[i].Loghdr.wCurrentEntries);
        FileSeek(hDbgFile, FileLength(hDbgFile), SEEK_SET);
        if (FileWrite(hDbgFile, (LPSTR)szBuf, strlen(szBuf)) != strlen(szBuf))
            {
            MessageBox(NULL, "ALogOpen(): Debug File Write Error.\n",NULL,MB_OK);
            goto done;
            }
#endif
                                        // Write file header
        if (FileWrite(wHandle, (LPSTR)&LogFile[i].Loghdr, sizeof(LOGHEADER)) !=
                      sizeof(LOGHEADER) )
            {
            wErr = NAVLOG_ERR_WRITE;
            goto done;
            }
        }
    else if (wRead != sizeof(LOGHEADER))// Could not read whole file hdr
        {
        wErr = NAVLOG_ERR_READ;
        goto done;
        }
    else if (LogFile[i].Loghdr.Version != ALOG_VERSION) // Bad data read
        {                                     // from header
        wErr = NAVLOG_ERR_VERSION;            //&? in the future, this
        goto done;                            // may need to support
        }                                     // different version IDs

    LogFile[i].wHandle  = wHandle;      // Save file handle
    LogFile[i].dwCurOff = 0;            // Current position at start
    LogFile[i].wCurRecNum = 0;          // First or non-existent record
    LogFile[i].LastRec = FALSE;         // Last record being read.

    *puHandle = i;                      // Return index as "handle"

done:
    if (wErr && wHandle != HFILE_ERROR)
        FileClose(wHandle);

#ifdef VDEBUG
    if (hDbgFile)
        FileClose(hDbgFile);
#endif

    return(wErr);
} // ALogOpen()


//** ALogClear() ******************************************************
// Empty the log file.
//
//*********************************************************************
// 05/27/93 WRL
//*********************************************************************
WORD SYM_EXPORT WINAPI ALogClear(           // Name of log file
    UINT    wHand,                      // Handle or NULL
    LPSTR   szLogFile)                  // Name of file if wHand == 0
{

    HFILE hFile;
    LOGHEADER LogHeader;
    WORD wErr = NOERR;

    if (!wHand)                         // Use file name
        {
        hFile = FileOpen(szLogFile, READ_WRITE_FILE);
        }
    else if (wHand == HFILE_ERROR || LogFile[wHand].wHandle == HFILE_ERROR)
        {
        wErr = NAVLOG_ERR_HANDLE;
        } // else if
    else
        {
        hFile = LogFile[wHand].wHandle;
        FileSeek(hFile, 0, SEEK_SET);   // Ensure that we're at to top
        }

    if (!wErr && hFile == HFILE_ERROR)
        {
        wErr = NAVLOG_ERR_HANDLE;
        } /* then */

    if (!wErr)
        {
                                        // Read file header
        if (FileRead(hFile, &LogHeader, sizeof(LogHeader)) != HFILE_ERROR)
            {
            LogHeader.dwFirstEntry  = NAVLOG_EOF;
            LogHeader.dwLastEntry   = 0;
            LogHeader.wCurrentEntries = 0;

            FileSeek(hFile, 0, SEEK_SET); // Move back to top of file
                                          // Write header
            if (FileWrite(hFile, &LogHeader, sizeof(LogHeader)) != HFILE_ERROR)
                FileSetSize(hFile, sizeof(LOGHEADER)); // Resize file
            }
        }

    if (!wErr && wHand && wHand != HFILE_ERROR)  // If handle defined,
        {                               // reset memory values
        LogFile[wHand].dwCurOff = 0;    //    Current position at start
        LogFile[wHand].wCurRecNum = 0;  //    First or non-existent record
        LogFile[wHand].Loghdr = LogHeader; // Update memory version of hdr
        } /* then */
    else if (!wErr && hFile != HFILE_ERROR)
        FileClose(hFile);

    return wErr;

} // ALogClear()


//** ALogClose() *******************************************************
//
// Flush and close log file
//
//**********************************************************************
//  26mar93 efh - New
//**********************************************************************
WORD SYM_EXPORT WINAPI ALogClose(           // Close the log file
    UINT   wHand)                       // Log file "handle"
{
    WORD    wErr;                       // Return value
    DWORD   dwErr;

                                        // Validate "handle"
    if (wHand == HFILE_ERROR || LogFile[wHand].wHandle == HFILE_ERROR)
        {
        wErr = NAVLOG_ERR_HANDLE;
        }
    else
        {                               // Update file header
         wErr = ALogHeader(wHand, TRUE, &LogFile[wHand].Loghdr);

#ifdef   SYM_WIN32   // only on NT
         // May be the log file is locked, lets just ignore this case.
         if ( (dwErr = GetLastError ()) == ERROR_ACCESS_DENIED)
            wErr = 0;
#endif

                                        // close file
         if ( FileClose(LogFile[wHand].wHandle) != NOERR && !wErr)
         {
             wErr = NAVLOG_ERR_WRITE ;  // Caches wait to report WRITE errors
#ifdef   SYM_WIN32   // only on NT
             dwErr = GetLastError ();
#endif
         }

         LogFile[wHand].wHandle = HFILE_ERROR; // Reset file state block
                                               //    "unused"
        }

    return(wErr);
} // ALogClose()


//** ALogRecWrite() ****************************************************
//
// Write log file record
//
// This routine appends a new record to the activity log file.  It must
// check for constraint conditions specified in the file header.
//
// If the disk runs out of space, the maxsize is set to the current file
// size.
//
// Future Enhancements:
// - Dont update wCurRecNum (or any lpFile fields) until we are sure
//   that everything worked okay.
//
// Note that the record pointers get inconsistent when a write fails
// after the file has already been wrapped.  I'm not quite sure how to
// deal with this, but if this happens, we're already hosed big time
// anyway. [4/21/93 efh]
//**********************************************************************
//  26mar93 efh - New
//  19apr93 wrl - No dynamic allocation.
//  21apr93 efh - Set max size and wrap writes when no disk space.
//  22may93 efh - Reorgianzed and cleanup entire function.
// 01/26/96 MLELE  Fixed to check if the first record position needs to
//                 be advanced BAFORE & AFTER the new record wraps.
//**********************************************************************
WORD SYM_EXPORT WINAPI ALogRecWrite(        // Append a record to log file
    UINT    wHand,                      // Log file handle
    WORD    wType,                      // Record type
    DWORD   dwKey,                      // Data key
    WORD    wSize,                      // data size
    LPBYTE  byData)                     // log record data
{
    LPLOGFILE   lpFile;                 // Local log file state block
    LOGRECORD   FirstRecord,            // Header of existing first record
                LastRecord;             // Header of existing last record
    LOGRECORD   LogRec;                 // Header of new record

    DWORD       dwFirstPos;             // first record offset
    DWORD       dwNewPos;               // new last record offset
    DWORD       dwEndPos;               // end of new last record offset
    DWORD       dwWCount;               // size of new record
    WORD        wErr = NOERR;
    BOOL        bNoSpace = FALSE;       // did we run out of disk space?

#ifdef VDEBUG
    char        szData1[65];
    char        szData2[65];
    char        szData3[65];
#endif

                                        // Validate "handle"
    if (wHand == HFILE_ERROR || LogFile[wHand].wHandle == HFILE_ERROR)
        {
        wErr = NAVLOG_ERR_HANDLE;
        goto done;
        }

    lpFile = &LogFile[wHand];           // Localize file state block
    lpFile->wCurRecNum = lpFile->Loghdr.wCurrentEntries;

    LogRec.dwPrevEntry  = lpFile->Loghdr.dwLastEntry; // Set back pointer
    LogRec.dwNextEntry  = NAVLOG_EOF;   // This is the last entry (no other)
    LogRec.wType        = wType;            // Record type
    LogRec.dwKey        = dwKey;            // Save field flags
    LogRec.wRecSize     = wSize;            // Size of data
    dwWCount = sizeof(LOGRECORD) + wSize;   // Set total size of new record



#ifdef VDEBUG
    if ((hDbgFile = FileOpen(szDebugFile, READ_WRITE_FILE)) == HFILE_ERROR)
        {
        MessageBox(NULL,"ALogRecWrite(1):File Open Error.\n",NULL,MB_OK);
        goto done;
        }
    wsprintf(szBuf,"\n\rALogRecWrite(1): Initialize LogRec\n\r"
        "   wSize=%d\n\r"
        "   LogRec.dwPrevEntry=lpFile->Loghdr.dwLastEntry=%ld\n\r"
        "         .dwNextEntry=%ld\n\r"
        "         .wType=%d\n\r"
        "         .dwKey=%ld\n\r"
        "         .wRecSize=%d\n\r"
        "   lpFile->wCurRecNum=%d\n\r"
        "   lpFile->Loghdr.dwMaxSize=%ld\n\r"
        "   dwWcount=sizeof(LOGRECORD)+wSize=%d+%d=%ld\n\r",
       wSize,
       LogRec.dwPrevEntry,
       LogRec.dwNextEntry,
       LogRec.wType,
       LogRec.dwKey,
       LogRec.wRecSize,
       lpFile->wCurRecNum,
       lpFile->Loghdr.dwMaxSize,
       sizeof(LOGRECORD),wSize,dwWCount);
    FileSeek(hDbgFile, FileLength(hDbgFile), SEEK_SET);
    if (FileWrite(hDbgFile, (LPSTR)szBuf, strlen(szBuf)) != strlen(szBuf))
        {
        MessageBox(NULL,"ALogRecWrite(1):Debug File Write Error.\n",NULL,MB_OK);
        goto done;
        }
#endif  // .. VDEBUG



    //
    //  Is the new record too big for file? Is max file size == 0?
    //  Check this before proceeding further.
    //
    if ( (0 != lpFile->Loghdr.dwMaxSize) &&
            ((sizeof(LOGHEADER) + dwWCount) > lpFile->Loghdr.dwMaxSize) )
        {
        wErr = NAVLOG_ERR_WRITE;
        goto done;
        }



tryagain:                               // "recursive" call
    //*****************************************************************
    //
    //  Set dwNewPos, dwEndPos & dwFirstPos
    //
    //*****************************************************************

    if (lpFile->Loghdr.dwLastEntry == 0)
        {

        //
        //  This is the first record in the file.
        //
        dwNewPos   = sizeof(LOGHEADER);
        dwEndPos   = sizeof(LOGHEADER) + dwWCount;

        //
        // IMPORTANT:
        //
        //  DO NOT set it to sizeof(LOGHEADER). It will be done
        //  later. NAVLOG_EOF indicates that this is the first record.
        //
        dwFirstPos = NAVLOG_EOF;


#ifdef VDEBUG
        wsprintf(szBuf,"\n\rALogRecWrite(2): Position as first record in file\n\r"
         "    dwNewPos=%ld\n\r"
         "    dwEndPos=%ld\n\r"
         "    dwFirstPos=lpFile->loghdr.dwFirstEntry=%ld\n\r",
           dwNewPos,
           dwEndPos,
           dwFirstPos);
        FileSeek(hDbgFile, FileLength(hDbgFile), SEEK_SET);
        if (FileWrite(hDbgFile, (LPSTR)szBuf, strlen(szBuf)) != strlen(szBuf))
            {
            MessageBox(NULL,"ALogRecWrite(2):Debug File Write Error.\n",NULL,MB_OK);
            goto done;
            }
#endif  // .. VDEBUG


        }
    else
        {

        //
        //  So, this is NOT first record in file.
        //  Find end of log file.
        //
        if (wErr = ReadRecHdr(lpFile->wHandle, lpFile->Loghdr.dwLastEntry,
                              &LastRecord) )
            goto done;



#ifdef VDEBUG
        wsprintf(szBuf,"\n\rALogRecWrite(3): Not first record in file, Find end of log file\n\r"
          "LastRecord read at lpFile->Loghdr.dwLastEntry=%ld\n\r"
          "LastRecord.dwPrevEntry=%ld\n\r"
          "          .dwNextEntry=%ld\n\r"
          "          .wType=%d\n\r"
          "          .wRecSize=%d\n\r",
          lpFile->Loghdr.dwLastEntry,
          LastRecord.dwPrevEntry,
          LastRecord.dwNextEntry,
          LastRecord.wType,
          LastRecord.wRecSize);
        FileSeek(hDbgFile, FileLength(hDbgFile), SEEK_SET);
        if (FileWrite(hDbgFile, (LPSTR)szBuf, strlen(szBuf)) != strlen(szBuf))
            {
            MessageBox(NULL,"ALogRecWrite(3):Debug File Write Error.\n",NULL,MB_OK);
            goto done;
            }
#endif  // .. VDEBUG

        
        //
        //  Calculate beginning and end offset of new record.
        //  Set existing file pointers before appending.
        //
        dwNewPos = lpFile->Loghdr.dwLastEntry + sizeof(LastRecord)
                   + LastRecord.wRecSize;
        dwEndPos = dwNewPos + dwWCount;



#ifdef VDEBUG
        wsprintf(szBuf,"\n\rALogRecWrite(4): Calcualte beginning and end offset of new record\n\r"
          "                    Set existing file pointers before appending\n\r"
          "    dwNewPos=%ld\n\r"
          "    dwEndPos=dwNewPos+dwWCount=%ld+%ld=%ld\n\r",
           dwNewPos,
           dwNewPos,dwWCount,dwEndPos);
        FileSeek(hDbgFile, FileLength(hDbgFile), SEEK_SET);
        if (FileWrite(hDbgFile, (LPSTR)szBuf, strlen(szBuf)) != strlen(szBuf))
            {
            MessageBox(NULL,"ALogRecWrite(4):Debug File Write Error.\n",NULL,MB_OK);
            goto done;
            }
#endif  // .. VDEBUG



        //
        //  If this is a "wrapped" file, the last record would be BEFORE
        //  the first record. The new record may overlap our first few
        //  records. Remedy: Advance the first record position until after
        //  the end of this new record.
        //
        //  NOTE:
        //      This may wrap the first position. So we'll have to do
        //      this again if the new position wraps.
        //
        dwFirstPos = lpFile->Loghdr.dwFirstEntry;
        FirstRecord.dwNextEntry = 0;        // Flag as non-read



#ifdef VDEBUG
    wsprintf(szBuf,"\n\rALogRecWrite(4.a): \n\r"
        "                     The new record position may cause the record to overlap...\n\r"
        "    dwFirstPos=lpFile->Loghdr.dwFirstEntry=%ld\n\r"
        "    FirstRecord.dwNextEntry=0\n\r",
        dwFirstPos);
    FileSeek(hDbgFile, FileLength(hDbgFile), SEEK_SET);
    if (FileWrite(hDbgFile, (LPSTR)szBuf, strlen(szBuf)) != strlen(szBuf))
        {
        MessageBox(NULL,"ALogRecWrite(4):Debug File Write Error.\n",NULL,MB_OK);
        goto done;
        }
#endif  // .. VDEBUG


        //
        //  Continue as long as dwFirstPos lies between
        //  dwNewPos and dwEndPos.
        //  
        while (dwNewPos <= dwFirstPos && dwFirstPos < dwEndPos)
            {
            if ((wErr = ReadRecHdr(lpFile->wHandle, dwFirstPos, &FirstRecord))
                != NOERR)
                goto done;


#ifdef VDEBUG
            wsprintf(szBuf,"\n\rALogRecWrite(5): dwNewPos <= dwFirstPos < dwEndPos \n\r"
                "   FirstREcord read at dwFirstPos=%ld\n\r"
                "   FirstRecord.dwPrevEntry=%ld\n\r"
                "              .dwNextEntry=%ld\n\r"
                "              .wType=%d\n\r"
                "              .wRecSize=%d\n\r"
                "   lpFile->Loghdr.dwFirstEntry=%ld\n\r",
                dwFirstPos,
                FirstRecord.dwPrevEntry,
                FirstRecord.dwNextEntry,
                FirstRecord.wType,
                FirstRecord.wRecSize,
                lpFile->Loghdr.dwFirstEntry);
            FileSeek(hDbgFile, FileLength(hDbgFile), SEEK_SET);
            if (FileWrite(hDbgFile, (LPSTR)szBuf, strlen(szBuf)) != strlen(szBuf))
                {
                MessageBox(NULL,"ALogRecWrite(5):Debug File Write Error.\n",NULL,MB_OK);
                goto done;
                }
#endif  // .. VDEBUG



            //
            //  Advance the first record and decrement the record count
            //
            dwFirstPos = FirstRecord.dwNextEntry;
            lpFile->wCurRecNum--;



#ifdef VDEBUG
            wsprintf(szBuf,"\n\rALogRecWrite(5.a): check next rec position\n\r"
                "   ===>dwFirstPos=FirstRecord.dwNextEntry=%ld\n\r"
                "   lpFile->wCurRecNum=%d\n\r",
                dwFirstPos,
                lpFile->wCurRecNum);
            FileSeek(hDbgFile, FileLength(hDbgFile), SEEK_SET);
            if (FileWrite(hDbgFile, (LPSTR)szBuf, strlen(szBuf)) != strlen(szBuf))
                {
                MessageBox(NULL,"ALogRecWrite(5.a):Debug File Write Error.\n",NULL,MB_OK);
                goto done;
                }
#endif


            } /* while dwNewPos <= dwFirstPos < dwEndPos */


        //
        //  We've made sure that the new record doesn't trample the first
        //  few records. We can write it, but if the end of the new record
        //  is falling beyond the set file size limit, we need to "wrap"
        //  the file and position new record at top of the file AFTER THE
        //  FILE HEADER.
        //
        if (lpFile->Loghdr.dwMaxSize && dwEndPos > lpFile->Loghdr.dwMaxSize)
            {
            dwNewPos = sizeof(LOGHEADER);
            dwEndPos = sizeof(LOGHEADER) + dwWCount;


#ifdef VDEBUG
            wsprintf(szBuf,"\n\rALogRecWrite(5.b): If record exceeds file size, position..\n\r"
                "   dwNewPos=sizeof(LOGHEADER)=%ld\n\r"
                "   dwEndPos=sizeof(LOGHEADER)+dwWCount=%d+%ld=%ld\n\r",
                dwNewPos,
                sizeof(LOGHEADER),dwWCount,dwEndPos);
            FileSeek(hDbgFile, FileLength(hDbgFile), SEEK_SET);
            if (FileWrite(hDbgFile, (LPSTR)szBuf, strlen(szBuf)) != strlen(szBuf))
                {
                MessageBox(NULL,"ALogRecWrite(5.b):Debug File Write Error.\n",NULL,MB_OK);
                goto done;
                }
#endif  // .. VDEBUG


            }


        //
        //  New position wrapped. This may again result in the new record
        //  trampling our first few records. Advance the first record
        //  position (set above) until after the end of this new
        //  record position.
        //
        while (dwNewPos <= dwFirstPos && dwFirstPos < dwEndPos)
            {
            if ( (wErr = ReadRecHdr ( lpFile->wHandle,
                                      dwFirstPos,
                                      &FirstRecord ) ) != NOERR)
                goto done;



#ifdef VDEBUG
            wsprintf(szBuf,"\n\rALogRecWrite(7): New position wrapped.  \n\r"
                "                      dwNewPos <= dwFirstPos < dwEndPos\n\r"
                "   FirstRecord read at dwFirstPos=%ld\n\r"
                "   FirstRecord.dwPrevEntry=%ld\n\r"
                "              .dwNextEntry=%ld\n\r"
                "              .wType=%d\n\r"
                "              .wRecSize=%d\n\r"
                "   lpFile->Loghdr.dwFirstEntry=%ld\n\r",
                dwFirstPos,
                FirstRecord.dwPrevEntry,
                FirstRecord.dwNextEntry,
                FirstRecord.wType,
                FirstRecord.wRecSize,
                lpFile->Loghdr.dwFirstEntry);
            FileSeek(hDbgFile, FileLength(hDbgFile), SEEK_SET);
            if (FileWrite(hDbgFile, (LPSTR)szBuf, strlen(szBuf)) != strlen(szBuf))
                {
                MessageBox(NULL,"ALogRecWrite(7):Debug File Write Error.\n",NULL,MB_OK);
                goto done;
                }
#endif  // .. VDEBUG



            //
            //  Advance the first record and decrement the record count
            //
            dwFirstPos = FirstRecord.dwNextEntry;
            lpFile->wCurRecNum--;



#ifdef VDEBUG
            wsprintf(szBuf,"\n\rALogRecWrite(8): Check next rec position\n\r"
                "   ===>dwFirstPos=FirstReocrd.dwNextEntry=%ld\n\r"
                "   lpFile->wCurRecNum=%d\n\r",
                dwFirstPos,
                lpFile->wCurRecNum);
            FileSeek(hDbgFile, FileLength(hDbgFile), SEEK_SET);
            if (FileWrite(hDbgFile, (LPSTR)szBuf, strlen(szBuf)) != strlen(szBuf))
                {
                MessageBox(NULL,"ALogRecWrite(8):Debug File Write Error.\n",NULL,MB_OK);
                goto done;
                }
#endif  // .. VDEBUG



            } /* while dwNewPos <= dwFirstPos < dwEndPos */

        } // end else (dwLastEntry == 0)




    if (dwFirstPos == NAVLOG_EOF)       // All records have been dumped or
        {                               // there is no first record.
        dwFirstPos = dwNewPos;          // Save new firstpos for later
        lpFile->wCurRecNum = 0;         // All records non-existent or
        LogRec.dwPrevEntry = 0;         //    cleared; no previous entry



#ifdef VDEBUG
        wsprintf(szBuf,"\n\rALogRecWrite(10): dwFirstPos == NAVLOG_EOF\n\r"
          "    dwFirstPos=dwNewPos=%ld\n\r"
          "    lpFile->wCurREcNum=0\n\r"
          "    LogRec.dwPrevEntry=0\n\r",
          dwFirstPos);
        FileSeek(hDbgFile, FileLength(hDbgFile), SEEK_SET);
        if (FileWrite(hDbgFile, (LPSTR)szBuf, strlen(szBuf)) != strlen(szBuf))
            {
            MessageBox(NULL,"ALogRecWrite(10):Debug File Write Error.\n",NULL,MB_OK);
            goto done;
            }
#endif  // .. VDEBUG


        }
    else if (FirstRecord.dwNextEntry != 0) // If first record was read,
        {                                  // then set its back pointers


#ifdef VDEBUG
        wsprintf(szBuf,"\n\rALogRecWrite(10.a): If first record was read, \n\r"
          "   FirstRecord.dwNextEntry=%ld\n\r",
              FirstRecord.dwNextEntry);
        FileSeek(hDbgFile, FileLength(hDbgFile), SEEK_SET);
        if (FileWrite(hDbgFile, (LPSTR)szBuf, strlen(szBuf)) != strlen(szBuf))
            {
            MessageBox(NULL,"ALogRecWrite(10.a):Debug File Write Error.\n",NULL,MB_OK);
            goto done;
            }
#endif  // .. VDEBUG



        if ((wErr = ReadRecHdr(lpFile->wHandle, dwFirstPos, &FirstRecord))
            != NOERR)
            goto done;



#ifdef VDEBUG
        wsprintf(szBuf,"\n\rALogRecWrite(11): ReadRecHdr(, at %ld, &FirstRecord) \n\r"
          "   FirstRecord.dwPrevEntry=%ld\n\r"
          "              .dwNextEntry=%ld\n\r"
          "              .wType=%d\n\r"
          "              .wRecSize=%d\n\r",
          dwFirstPos,
          FirstRecord.dwPrevEntry,
          FirstRecord.dwNextEntry,
          FirstRecord.wType,
          FirstRecord.wRecSize);
        FileSeek(hDbgFile, FileLength(hDbgFile), SEEK_SET);
        if (FileWrite(hDbgFile, (LPSTR)szBuf, strlen(szBuf)) != strlen(szBuf))
            {
            MessageBox(NULL,"ALogRecWrite(11):Debug File Write Error.\n",NULL,MB_OK);
            goto done;
            }
#endif  // .. VDEBUG



        FirstRecord.dwPrevEntry = 0;



#ifdef VDEBUG
        wsprintf(szBuf,"\n\rALogRecWrite(11.a): WriteRecHdr(, at %ld, &FirstRecord) \n\r"
          "   FirstRecord.dwPrevEntry=%ld\n\r"
          "              .dwNextEntry=%ld\n\r"
          "              .wType=%d\n\r"
          "              .wRecSize=%d\n\r",
          dwFirstPos,
          FirstRecord.dwPrevEntry,
          FirstRecord.dwNextEntry,
          FirstRecord.wType,
          FirstRecord.wRecSize);
        FileSeek(hDbgFile, FileLength(hDbgFile), SEEK_SET);
        if (FileWrite(hDbgFile, (LPSTR)szBuf, strlen(szBuf)) != strlen(szBuf))
            {
            MessageBox(NULL,"ALogRecWrite(11.a):Debug File Write Error.\n",NULL,MB_OK);
            goto done;
            }
#endif  // .. VDEBUG



        if ((wErr = WriteRecHdr(lpFile->wHandle, dwFirstPos, &FirstRecord))
            != NOERR)
            goto done;
        }



#ifdef VDEBUG
        STRNCPY(szData1,byData,64);
        szData1[64]='\0';
        STRNCPY(szData2,&byData[64],64);
        szData2[64]='\0';
        STRNCPY(szData3,&byData[128],64);
        szData3[64]='\0';
        wsprintf(szBuf,"\n\rALogRecWrite(11.b): write new last record(, at %ld, &LogRec) \n\r"
          "  LogRec.dwPrevEntry=%ld\n\r"
          "        .dwNextEntry=%ld\n\r"
          "        .wType=%d\n\r"
          "        .wRecSize=%d\n\r"
          "  Data=%s\n\r"
          "       %s\n\r"
          "       %s\n\r",
          dwNewPos,
          LogRec.dwPrevEntry,
          LogRec.dwNextEntry,
          LogRec.wType,
          LogRec.wRecSize,
          (LPSTR)szData1,
          (LPSTR)szData2,
          (LPSTR)szData3);
        FileSeek(hDbgFile, FileLength(hDbgFile), SEEK_SET);
        if (FileWrite(hDbgFile, (LPSTR)szBuf, strlen(szBuf)) != strlen(szBuf))
            {
            MessageBox(NULL,"ALogRecWrite(11.b):Debug File Write Error.\n",NULL,MB_OK);
            goto done;
            }
#endif  // .. VDEBUG


    //
    //  Write new last record
    //
    if (WriteRecHdr(lpFile->wHandle, dwNewPos, &LogRec)
        || FileWrite(lpFile->wHandle, (LPSTR)byData, wSize) != wSize)
        {
                                        // if !wrapped & no space,
                                        // we're in big trouble!
        if (!bNoSpace && dwNewPos > lpFile->Loghdr.dwLastEntry)
            {

            bNoSpace = TRUE;            // Signal a warning.
            lpFile->Loghdr.dwMaxSize = FileLength(lpFile->wHandle);


#ifdef VDEBUG
            wsprintf(szBuf,"\n\rALogRecWrite(16): if !wrapped & no space\n\r"
              "   lpFile->Loghdr.dwMaxSize=%ld\n\r",
              lpFile->Loghdr.dwMaxSize);
            FileSeek(hDbgFile, FileLength(hDbgFile), SEEK_SET);
            if (FileWrite(hDbgFile, (LPSTR)szBuf, strlen(szBuf)) != strlen(szBuf))
                {
                MessageBox(NULL,"ALogRecWrite(16):Debug File Write Error.\n",NULL,MB_OK);
                goto done;
                }
#endif  // .. VDEBUG


            if (lpFile->Loghdr.dwMaxSize) // if filesize > 0, wrap file



#ifdef VDEBUG
                {
                STRCPY(szBuf,"\n\rALogRecWrite(17): if filesize > 0, wrap file\n\r"
                  "   goto tryagain\n\r");
                FileSeek(hDbgFile, FileLength(hDbgFile), SEEK_SET);
                if (FileWrite(hDbgFile, (LPSTR)szBuf, strlen(szBuf)) != strlen(szBuf))
                    {
                    MessageBox(NULL,"ALogRecWrite(17):Debug File Write Error.\n",NULL,MB_OK);
                    goto done;
                    }
#endif  // .. VDEBUG


                goto tryagain;            //    and try writing again


#ifdef VDEBUG
                }
#endif

            }

        wErr = NAVLOG_ERR_WRITE;
        goto done;
        }

    lpFile->wCurRecNum++;               // Notice new record written.



#ifdef VDEBUG
    wsprintf(szBuf,"\n\rALogRecWrite(18): Notice new record written\n\r"
      "   lpFile->wCurRecNum=%d\n\r",lpFile->wCurRecNum);
    FileSeek(hDbgFile, FileLength(hDbgFile), SEEK_SET);
    if (FileWrite(hDbgFile, (LPSTR)szBuf, strlen(szBuf)) != strlen(szBuf))
        {
        MessageBox(NULL,"ALogRecWrite(18):Debug File Write Error.\n",NULL,MB_OK);
        goto done;
        }
#endif  // .. VDEBUG



    //*****************************************************************
    // Update previous last record to refer to new record. If there's
    // no overlap with old last record, update it
    //*****************************************************************
    if (lpFile->Loghdr.dwLastEntry != 0 &&
        (lpFile->Loghdr.dwLastEntry > dwEndPos
         || lpFile->Loghdr.dwLastEntry < dwNewPos))
        {
        LastRecord.dwNextEntry = dwNewPos;



#ifdef VDEBUG
        wsprintf(szBuf,"\n\rALogRecWrite(19): Update previous last record to refer to new record\n\r"
          "   lpFile->Loghdr.dwLastEntry=%ld\n\r"
          "   dwNewPos=%ld\n\r"
          "   dwEndPos=%ld\n\r"
          "   LastRecord.dwNextEntry=dwNewPos=%ld\n\r",
          lpFile->Loghdr.dwLastEntry,
          dwNewPos,
          dwEndPos,
          LastRecord.dwNextEntry);
        FileSeek(hDbgFile, FileLength(hDbgFile), SEEK_SET);
        if (FileWrite(hDbgFile, (LPSTR)szBuf, strlen(szBuf)) != strlen(szBuf))
            {
            MessageBox(NULL,"ALogRecWrite(19):Debug File Write Error.\n",NULL,MB_OK);
            goto done;
            }

        wsprintf(szBuf,"\n\rALogRecWrite(19.a): WriteRecHdr(,at %ld, &LastRecord)\n\r"
          "LastRecord.dwPrevEntry=%ld\n\r"
          "          .dwNextEntry=%ld\n\r"
          "          .wType=%d\n\r"
          "          .wRecSize=%d\n\r",
          lpFile->Loghdr.dwLastEntry,
          LastRecord.dwPrevEntry,
          LastRecord.dwNextEntry,
          LastRecord.wType,
          LastRecord.wRecSize);
        FileSeek(hDbgFile, FileLength(hDbgFile), SEEK_SET);
        if (FileWrite(hDbgFile, (LPSTR)szBuf, strlen(szBuf)) != strlen(szBuf))
            {
            MessageBox(NULL,"ALogRecWrite(19.a):Debug File Write Error.\n",NULL,MB_OK);
            goto done;
            }
#endif  // .. VDEBUG



        if ((wErr = WriteRecHdr(lpFile->wHandle, lpFile->Loghdr.dwLastEntry,
                                &LastRecord)) != NOERR)
            goto done;
        }

                                        // update file header
    lpFile->Loghdr.dwFirstEntry = dwFirstPos; // new(?) first record
    lpFile->Loghdr.dwLastEntry  = dwNewPos;   // append to file
    lpFile->Loghdr.wCurrentEntries = lpFile->wCurRecNum;
    lpFile->dwCurOff = dwEndPos;



#ifdef VDEBUG
    wsprintf(szBuf,"\n\rALogRecWrite(20): update file header\n\r"
      "   lpFile->Loghdr.dwFirstEntry=dwFirstPos=%ld\n\r"
      "                 .dwLastEntry=dwNewPos=%ld\n\r"
      "                 .wCurrentEntries=lpFile->wCurRecNum=%d\n\r"
      "         ->dwCurOff=%ld\n\r",
      lpFile->Loghdr.dwFirstEntry,
      lpFile->Loghdr.dwLastEntry,
      lpFile->Loghdr.wCurrentEntries,
      lpFile->dwCurOff);
    FileSeek(hDbgFile, FileLength(hDbgFile), SEEK_SET);
    if (FileWrite(hDbgFile, (LPSTR)szBuf, strlen(szBuf)) != strlen(szBuf))
        {
        MessageBox(NULL,"ALogRecWrite(20):Debug File Write Error.\n",NULL,MB_OK);
        goto done;
        }
#endif  // .. VDEBUG



done:
    if (bNoSpace == TRUE && !wErr)
        wErr = NAVLOG_ERR_NO_SPACE;

    return(wErr);
} // ALogRecWrite()



#ifndef SYM_NLM

//** ALogRecRead() *****************************************************
//
// Read log file record from current position.
//
//**********************************************************************
//  26mar93 efh - New
// 06/08/93 WRL function created from ALogRecRead():  no dynamic
//              allocation for each read now.
//**********************************************************************
int SYM_EXPORT WINAPI ALogRecReadHdr(       // Read a record
    UINT    hLog,                       // In: file handle
    LPWORD  wType,                      // Out: Record type
    LPDWORD dwKey,                      // Out: Data key
    LPWORD  wSize,                      // Out: size of record
    WORD    wRecNo)                     // Record Number to read
{
    LOGRECORD   LogRecord;
    LPLOGFILE   lpFile;                 // Local state pointer
    DWORD       dwCurrPos = 0;          // current file pointer
    WORD        wRead;                  // Read size
    WORD        rc = NOERR;             // Return value
                                        // Validate "handle"
    if (hLog == HFILE_ERROR || LogFile[hLog].wHandle == HFILE_ERROR)
        {
        rc = NAVLOG_ERR_HANDLE;
        goto badhandle;
        }

    lpFile = &LogFile[hLog];

    // ------------------------------------------------------------------ //
    // This function call will position the file pointer to the header of //
    // the required record number. After the read took place the original //
    // file pointer is restored to not to goof up any other project.      //
    // ------------------------------------------------------------------ //

    if ( wRecNo )
        {
        dwCurrPos = FileSeek (lpFile->wHandle, 0L, SEEK_CUR);
        MoveToRecordHeader (lpFile->wHandle, wRecNo);
        }

    // ------------------------------------------------------------------ //

                                        // Get rec hdr
    wRead = FileRead(lpFile->wHandle, (LPSTR)&LogRecord, sizeof(LogRecord));
    if (wRead != sizeof(LogRecord))
        {
        rc = NAVLOG_ERR_READ;
        }
    else                                // No error
        {
        *wType  = LogRecord.wType;
        *dwKey  = LogRecord.dwKey;
        *wSize  = LogRecord.wRecSize;
        }
                                        // ------------------------------
                                        // If the file pointer has been
                                        // moved then restore it.
                                        // ------------------------------
    if ( dwCurrPos )
        FileSeek (lpFile->wHandle, (dwCurrPos + wRead), SEEK_SET);

    if (LogRecord.dwNextEntry == NAVLOG_EOF) // last record.
        {
        LogFile[hLog].LastRec = TRUE;
        } /* then */

    lpFile->dwCurOff += wRead;

badhandle:
    return(rc);
} // ALogRecRead()

//*********************************************************************
// Read record data
//
// Note:  No error checking of handle
//*********************************************************************
// 06/08/93 WRL function created from ALogRecRead()
//*********************************************************************
int SYM_EXPORT WINAPI ALogRecReadData(      // Read record data
    UINT  hLog,                         // Log file "handle"
    WORD  Length,                       // Amount of data to read
    LPSTR lpData,                       // Buffer to fill
    WORD  wRecNo)
{
    auto    WORD    wBytesRead = 0;
    auto    DWORD   dwCurrPos  = 0;

    int rc=NOERR;                       // Return value

    if (lpData == NULL)                 // bad pointer
        {
        rc = NAVLOG_ERR_BAD_ARG;
        goto done;
        }

    // ------------------------------------------------------------------ //
    // This function call will position the file pointer to the header of //
    // the required record number. After the read took place the original //
    // file pointer is restored to not to goof up any other project.      //
    // ------------------------------------------------------------------ //

    if ( wRecNo )
        {
        dwCurrPos = FileSeek (LogFile[hLog].wHandle, 0L, SEEK_CUR);
        MoveToRecordHeader (LogFile[hLog].wHandle, wRecNo);
        FileSeek (LogFile[hLog].wHandle, sizeof(LOGRECORD), SEEK_CUR);
        }

    // ------------------------------------------------------------------ //
                                        // Get user data
    wBytesRead = FileRead(LogFile[hLog].wHandle, lpData, Length);

                                        // ------------------------------
                                        // If the file pointer has been
                                        // moved then restore it.
                                        // ------------------------------
    if ( dwCurrPos )
        FileSeek (LogFile[hLog].wHandle, (dwCurrPos + wBytesRead), SEEK_SET);

    if (wBytesRead != Length)
        {
        rc = NAVLOG_ERR_READ;
        goto done;
        }

    if (LogFile[hLog].LastRec)          // Force error;
        {                               // make next read return 0 bytes
        LogFile[hLog].dwCurOff = FileSeek(LogFile[hLog].wHandle, 0L, SEEK_END);
        LogFile[hLog].LastRec = FALSE;
        }

    LogFile[hLog].dwCurOff += Length;

    LogFile[hLog].wCurRecNum++;         // Done reading record

done:
    return rc;
} // ALogRedReadData()


//** ALogRecSeek() *****************************************************
//
// Seek to a specific record in log file
//
// Future Enhancements:
// - Use Prev and Next on from the nearest of known positions: 1st record
//   in file, first record of log, last record in log, and current record.
//**********************************************************************
//  26mar93 efh - New
//  08jun93 wrl - Consolidated HFILE_ERROR checking
//**********************************************************************
WORD SYM_EXPORT WINAPI ALogRecSeek(
    UINT    wHand,                      // file handle
    DWORD   dwRecNum)                   // record number to seek to (1-(n-1))
{
    LPLOGFILE   lpFile;
    WORD        wErr = NOERR;

    if (wHand == HFILE_ERROR || LogFile[wHand].wHandle == HFILE_ERROR)
        {
        wErr = NAVLOG_ERR_HANDLE;
        goto done;
        }

    lpFile = &LogFile[wHand];

    if (dwRecNum >= lpFile->Loghdr.wCurrentEntries) // argument out of range
        {
        wErr = NAVLOG_ERR_BAD_ARG;
        goto done;
        }

    while (dwRecNum < lpFile->wCurRecNum)
        {
        if ((wErr = PrevRecord(lpFile)) != NOERR)
            goto done;
        }

    while (dwRecNum > lpFile->wCurRecNum)
        {
        if ((wErr = NextRecord(lpFile)) != NOERR)
            goto done;
        }

done:
    return(wErr);
} // ALogRecSeek()



#if 0 // Not used.
//** ALogSeek() ********************************************************
//
// Seek in log file.  Set file pointer to beginning of
//  record that is at or after specified offset.
//
// Future Enhancements:
// - Accommodate log files that wrap
//**********************************************************************
//  26mar93 efh - New
//  01jun93 wrl - Consolidated HFILE_ERROR checking
//**********************************************************************
WORD SYM_EXPORT WINAPI ALogSeek(
    UINT    wHand,                      // file handle
    DWORD   dwOff)                      // file offset
{
    LPLOGFILE   lpFile;
    WORD        wErr = NOERR;

    if (wHand != HFILE_ERROR && LogFile[wHand].wHandle != HFILE_ERROR)
        {
        lpFile = &LogFile[wHand];
        if (dwOff < lpFile->Loghdr.dwFirstEntry
            || dwOff > lpFile->Loghdr.dwLastEntry)
            {
            wErr = NAVLOG_ERR_BAD_ARG;
            goto done;
            }

        while (dwOff < lpFile->dwCurOff)
            {
            if ((wErr = PrevRecord(lpFile)) != NOERR)
                goto done;
            }

        while (dwOff > lpFile->dwCurOff)
            {
            if ((wErr = NextRecord(lpFile)) != NOERR)
                goto done;
            }
        }
    else
        wErr = NAVLOG_ERR_HANDLE;

done:
    return(wErr);
} // ALogSeek()
#endif // Not used.



#if 0 // Not used.
//** ALogRecTell() *****************************************************
//
// Get current record number
//
//**********************************************************************
//  26mar93 efh - New
//  01jun93 wrl - Consolidated HFILE_ERROR checking and eliminated goto
//**********************************************************************
DWORD SYM_EXPORT WINAPI ALogRecTell(UINT wHand) // file handle
{
    DWORD   dwRet = (DWORD)-1;

    if (wHand != HFILE_ERROR && LogFile[wHand].wHandle != HFILE_ERROR)
        dwRet = LogFile[wHand].wCurRecNum;

    return(dwRet);
} // ALogRecTell()
#endif // Not used.


//** ALogTell() ********************************************************
//
// Get current file position
//
//**********************************************************************
//  26mar93 efh - New
//  01jun93 wrl - Consolidated HFILE_ERROR checking and eliminated goto
//**********************************************************************
DWORD SYM_EXPORT WINAPI ALogTell(
    UINT wHand)                         // file handle
{
    DWORD   dwRet=(DWORD)-1;

    if (wHand != HFILE_ERROR && LogFile[wHand].wHandle != HFILE_ERROR)
        dwRet = LogFile[wHand].dwCurOff;

    return(dwRet);
} // ALogTell()



//** ALogGetMaxSize() **************************************************
//
// Get max file size
//
//**********************************************************************
//  02apr93 efh - New
//  01jun93 wrl - Consolidated HFILE_ERROR checking and eliminated goto
//**********************************************************************
DWORD SYM_EXPORT WINAPI ALogGetMaxSize(
    UINT wHand)                         // file handle
{
    DWORD dwRet=0;

    if (wHand != HFILE_ERROR && LogFile[wHand].wHandle != HFILE_ERROR)
        dwRet = LogFile[wHand].Loghdr.dwMaxSize;

    return(dwRet);
} // ALogGetMaxSize()

#endif  // ifndef SYM_NLM


//** ALogSetMaxSize() **************************************************
//
// Set max file size
//
// Implementation:
//  Determine the starting size of the log file (excluding any unused
//  portions) depending on whether the records wrap or not.
//
//  If wrapped:
//      StartSize =  LastRec.offset + LastRec.RecSize
//                   + TopRec.PrevRec.offset + TopRec.PrevRec.RecSize
//                   - FirstRec.offset
//  Else not wrapped:
//      StartSize =  Header.Size + LastRec.offset + LastRec.RecSize
//                   - FirstRec.offset
//
//  while StartSize > MaxSize
//      StartSize -= FirstRec.RecSize
//      FirstRec++
//
//
// Future Enhancements:
//**********************************************************************
//  02apr93 efh - New
//  01jun93 wrl - Consolidated HFILE_ERROR checking
//              - Delete old records if file is too big.
//**********************************************************************
int SYM_EXPORT WINAPI ALogSetMaxSize(
    UINT    hLog,                       // file handle
    DWORD   dwMaxSize)                  // new max size
{
    int rc;                             // Return value

    if (hLog == HFILE_ERROR || LogFile[hLog].wHandle == HFILE_ERROR)
        return NAVLOG_ERR_HANDLE;

    if (dwMaxSize && dwMaxSize > LogFile[hLog].Loghdr.dwMaxSize)
    {
        DWORD     ValidSize;
        LOGRECORD FirstRec, LastRec;
        DWORD     FirstRecOffset;
                                        // save first record offset for later
        FirstRecOffset = LogFile[hLog].Loghdr.dwFirstEntry;

        rc = ReadRecHdr(LogFile[hLog].wHandle, FirstRecOffset, &FirstRec);
        if (rc) goto done;

        rc = ReadRecHdr(LogFile[hLog].wHandle,
                        LogFile[hLog].Loghdr.dwLastEntry, &LastRec);
        if (rc) goto done;

        if (FirstRecOffset < LogFile[hLog].Loghdr.dwLastEntry)
        {                               // File is not not wrapped
            ValidSize = sizeof(LOGHEADER) + LogFile[hLog].Loghdr.dwLastEntry
                        + sizeof(LOGRECORD) + LastRec.wRecSize
                        - FirstRecOffset;
        } /* then */
        else                            // File is wrapped
        {
            LOGRECORD BottomRec;
            ReadRecHdr(LogFile[hLog].wHandle, sizeof(LOGHEADER),
                       &BottomRec);
            ReadRecHdr(LogFile[hLog].wHandle, BottomRec.dwPrevEntry,
                       &BottomRec);

            ValidSize = LogFile[hLog].Loghdr.dwLastEntry
                        + sizeof(LOGRECORD) + LastRec.wRecSize
                        + BottomRec.dwPrevEntry
                          + sizeof(LOGRECORD) + BottomRec.wRecSize
                        - FirstRecOffset;
        } /* else */

        if (ValidSize > dwMaxSize) {
            WORD Entries = LogFile[hLog].Loghdr.wCurrentEntries;
            do {
                                        // Calc new file size
                ValidSize -= sizeof(LOGRECORD) + FirstRec.wRecSize;
                Entries--;              // Decrement number of records
                FirstRecOffset = FirstRec.dwNextEntry; // Save new first rec.

                rc = ReadRecHdr(LogFile[hLog].wHandle, FirstRecOffset,
                                &FirstRec);
                if (rc) goto done;
            }
            while (ValidSize > dwMaxSize
                   && FirstRec.dwNextEntry != NAVLOG_EOF);

            if (ValidSize > dwMaxSize)  // Still too large?
                rc = ALogClear(hLog, NULL);
            else
            {
                FirstRec.dwPrevEntry = 0;   // First record
                rc = WriteRecHdr(LogFile[hLog].wHandle, FirstRecOffset,
                                 &FirstRec);
                if (rc) goto done;

                LogFile[hLog].Loghdr.wCurrentEntries = Entries;
                LogFile[hLog].Loghdr.dwFirstEntry = FirstRecOffset;
            }
        } /* then */

    }
    else
        rc = NAVLOG_ERR_VERSION;
                                        // Write new size to file header
    LogFile[hLog].Loghdr.dwMaxSize = dwMaxSize;
    LogFile[hLog].dwCurOff = sizeof(LOGHEADER);
    FileSeek(LogFile[hLog].wHandle, 0, SEEK_SET);
    FileWrite(LogFile[hLog].wHandle, &LogFile[hLog].Loghdr,
              sizeof(LOGHEADER));
    LogFile[hLog].dwCurOff = sizeof(LOGHEADER);

    ALogConsolidate(hLog);

done:
    return rc;
} // ALogSetMaxSize()


#ifndef SYM_NLM
//** ALogGetMaxEntries() ***********************************************
//
// Get max log entries
//
//**********************************************************************
//  02apr93 efh - New
//  01jun93 wrl - Consolidated HFILE_ERROR checking and eliminated goto
//**********************************************************************
DWORD SYM_EXPORT WINAPI ALogGetMaxEntries(
    UINT wHand)                         // file handle
{
    DWORD    dwRet = 0;

    if (wHand != HFILE_ERROR && LogFile[wHand].wHandle != HFILE_ERROR)
        dwRet = LogFile[wHand].Loghdr.wMaxEntries;

    return(dwRet);
} // ALogGetMaxEntries()
#endif


//** ALogSetMaxEntries() ***********************************************
//
// Set max log entries
//
// Implementation:
//
//  Dump each leading record while the number of records in the file
//  exceeds the requested maximum number.  Then, consolidate the
//  file.
//
// Future Enhancements:
// - Truncate file if new number of entries is less.
//**********************************************************************
//  02apr93 efh - New
//  01jun93 wrl - Consolidated HFILE_ERROR checking and eliminated goto
//**********************************************************************
int SYM_EXPORT WINAPI ALogSetMaxEntries(
    UINT    hLog,                       // file handle
    DWORD   dwMaxEntries)                // new max entries
{
    int rc=NOERR;                       // Return code
    WORD Entries;

    if (hLog == HFILE_ERROR || LogFile[hLog].wHandle == HFILE_ERROR)
        return NAVLOG_ERR_HANDLE;

    if (dwMaxEntries && dwMaxEntries < LogFile[hLog].Loghdr.wCurrentEntries)
    {
        LOGRECORD FirstRec;
        DWORD     FirstRecOffset;

        Entries = LogFile[hLog].Loghdr.wCurrentEntries;
        FirstRecOffset = LogFile[hLog].Loghdr.dwFirstEntry;

        while (dwMaxEntries < Entries && FirstRecOffset != NAVLOG_EOF)
        {
            rc = ReadRecHdr(LogFile[hLog].wHandle, FirstRecOffset, &FirstRec);
            if (rc) goto done;

            Entries--;
            FirstRecOffset = FirstRec.dwNextEntry; // Save new first rec.
        }

        rc = ReadRecHdr(LogFile[hLog].wHandle, FirstRecOffset, &FirstRec);
        if (rc) goto done;

        FirstRec.dwPrevEntry = 0;   // First record
        rc = WriteRecHdr(LogFile[hLog].wHandle, FirstRecOffset,
                         &FirstRec);
        if (rc) goto done;

        LogFile[hLog].Loghdr.dwFirstEntry = FirstRecOffset;

        LogFile[hLog].Loghdr.wCurrentEntries = Entries;

    } /* then */
                                        // Write new size to file header
    LogFile[hLog].Loghdr.wMaxEntries = (WORD) dwMaxEntries;
    LogFile[hLog].dwCurOff = sizeof(LOGHEADER);
    FileSeek(LogFile[hLog].wHandle, 0, SEEK_SET);
    FileWrite(LogFile[hLog].wHandle, &LogFile[hLog].Loghdr,
              sizeof(LOGHEADER));
    LogFile[hLog].dwCurOff = sizeof(LOGHEADER);

    ALogConsolidate(hLog);

done:
    return rc;
} // ALogSetMaxEntries()


#ifndef SYM_NLM
//** ALogGetMaxEntryAge() **********************************************
//
// Get max age of entries
//
//**********************************************************************
//  02apr93 efh - New
//  01jun93 wrl - Consolidated HFILE_ERROR checking and eliminated goto
//**********************************************************************
DWORD SYM_EXPORT WINAPI ALogGetMaxEntryAge(
    UINT wHand)                         // file handle
{
    DWORD    dwRet=0;

    if (wHand != HFILE_ERROR && LogFile[wHand].wHandle != HFILE_ERROR)
        dwRet = LogFile[wHand].Loghdr.wMaxDays;

    return(dwRet);
} // ALogGetMaxEntryAge()
#endif


//** ALogSetMaxEntryAge() **********************************************
//
// Set max age of entries.
//
// Implementation:
//
//  while LastRec.Date - FirstRec.Date > MaxDays
//      FirstRec++
//
//  That is, get the date of the first and last record of the log.  Keep
//  following along the chain, dumping the leading records, until the
//  difference between the (new) first record is within MaxDays of the
//  last record.  Finally, consolidate the file.
//
// Future Enhancements:
// - Truncate file if new age is shorter
//**********************************************************************
//  02apr93 efh - New
//  01jun93 wrl - Consolidated HFILE_ERROR checking and eliminated goto
//              - Trim old days.
//**********************************************************************
int SYM_EXPORT WINAPI ALogSetMaxEntryAge(
    UINT    hLog,                       // file handle
    DWORD   dwMaxDays)                   // new max days
{
//  int rc;                             // Return code
//  LOGRECORD FirstRec, LastRec;


    if (hLog == HFILE_ERROR || LogFile[hLog].wHandle == HFILE_ERROR)
        return NAVLOG_ERR_HANDLE;

                                        // Write new size to file header
    LogFile[hLog].Loghdr.wMaxDays = (WORD) dwMaxDays;
    LogFile[hLog].dwCurOff = sizeof(LOGHEADER);
    FileSeek(LogFile[hLog].wHandle, 0, SEEK_SET);
    FileWrite(LogFile[hLog].wHandle, &LogFile[hLog].Loghdr,
              sizeof(LOGHEADER));
    LogFile[hLog].dwCurOff = sizeof(LOGHEADER);

    ALogConsolidate(hLog);

    return NOERR;
} // ALogSetMaxEntryAge()


#ifndef SYM_NLM
//** ALogGetCurrentEntries() *******************************************
//
// Get current number of file entries
//
//**********************************************************************
//  02apr93 efh - New
//  01jun93 wrl - Consolidated HFILE_ERROR checking and eliminated goto
//**********************************************************************
WORD SYM_EXPORT WINAPI ALogGetCurrentEntries(
    UINT wHand)                         // file handle
{
    WORD    wRet = 0;

    if (wHand != HFILE_ERROR && LogFile[wHand].wHandle != HFILE_ERROR)
        wRet = LogFile[wHand].Loghdr.wCurrentEntries;

    return(wRet);
} // ALogGetCurrentEntries()


//** PrevRecord() ******************************************************
//
// Seek to previous record
//
//**********************************************************************
//
//  26mar93 efh - New
//
//**********************************************************************
WORD LOCAL PASCAL PrevRecord(
    LPLOGFILE   lpFile)                 // logfile struct
{
    LOGRECORD   LogRecord;
    WORD        wErr = NOERR;

    if (lpFile->dwCurOff > lpFile->Loghdr.dwFirstEntry)
        {
        if (FileRead(lpFile->wHandle, (LPSTR)&LogRecord, sizeof(LogRecord))
            != sizeof(LogRecord))
            {
            wErr = NAVLOG_ERR_READ;
            goto done;
            }

        lpFile->dwCurOff = FileSeek(lpFile->wHandle, LogRecord.dwPrevEntry, SEEK_SET);
        if (lpFile->dwCurOff != LogRecord.dwPrevEntry)
            {
            wErr = NAVLOG_ERR_SEEK;
            goto done;
            }

        lpFile->wCurRecNum--;
        }

done:
    return(wErr);
} // PrevRecord()


//** NextRecord() ******************************************************
//
// Seek to next record
//
//**********************************************************************
//
//  26mar93 efh - New
//
//**********************************************************************
WORD LOCAL PASCAL NextRecord(
    LPLOGFILE lpFile)                   // logfile struct
{
    LOGRECORD   LogRecord;
    WORD        wErr = NOERR;

    if (lpFile->dwCurOff < lpFile->Loghdr.dwLastEntry)
        {
        if (FileRead(lpFile->wHandle, (LPSTR)&LogRecord, sizeof(LogRecord))
            != sizeof(LogRecord))
            {
            wErr = NAVLOG_ERR_READ;
            goto done;
            }

        lpFile->dwCurOff = FileSeek(lpFile->wHandle, LogRecord.dwNextEntry, SEEK_SET);
        if (lpFile->dwCurOff != LogRecord.dwNextEntry
            || lpFile->dwCurOff == NAVLOG_EOF)
            {
            wErr = NAVLOG_ERR_SEEK;
            goto done;
            }

        lpFile->wCurRecNum++;
        }

done:
    return(wErr);
} // NextRecord()
#endif


//** ReadRecHdr() *****************************************************
//
// Read specified record header
//
//**********************************************************************
//
//  26mar93 efh - New
//
//**********************************************************************
WORD LOCAL PASCAL ReadRecHdr(
    HFILE       hFile,                  // File handle
    DWORD       dwOffset,
    LPLOGRECORD lpLogRec)
{
    WORD    wRead;
    WORD    wErr;

    if (FileSeek(hFile, dwOffset, SEEK_SET) != (LONG)dwOffset)
        {
        wErr = NAVLOG_ERR_SEEK;
        goto done;
        }

    wRead = FileRead(hFile, (LPSTR)lpLogRec, sizeof(*lpLogRec));
    if (wRead != sizeof(*lpLogRec))
        {
        wErr = NAVLOG_ERR_READ;
        goto done;
        }

    wErr = NOERR;

done:
    return(wErr);
} // ReadRecHdr()


//** WriteRecHdr() ******************************************************
//
// Write specified record header
//
//**********************************************************************
//
//  26mar93 efh - New
//
//**********************************************************************
WORD LOCAL PASCAL WriteRecHdr(
    HFILE       hFile,                  // File handle
    DWORD       dwOffset,
    LPLOGRECORD lpLogRec)
{
    WORD    wErr;

    if (FileSeek(hFile, dwOffset, SEEK_SET) != (LONG)dwOffset)
        {
        wErr = NAVLOG_ERR_SEEK;
        goto done;
        }

    if (FileWrite(hFile, (LPSTR)lpLogRec, sizeof(*lpLogRec))
        != sizeof(*lpLogRec))
        {
        wErr = NAVLOG_ERR_WRITE;
        goto done;
        }

    wErr = NOERR;

done:
    return(wErr);
} // WriteRecHdr()

//** ALogHeader() *****************************************************
// Read or write log file header.
//*********************************************************************
int SYM_EXPORT WINAPI ALogHeader(           // Read or write log header
    UINT        hLog,                   // File handle
    BOOL        bWrite,                 // TRUE: write; FALSE: read
    LPLOGHEADER LogHdr)                 // I/O:  structure to fill
{
    int rc=NOERR;

    FileSeek(LogFile[hLog].wHandle, 0, SEEK_SET); // Go to top
    if (bWrite)
    {
        if (FileWrite(LogFile[hLog].wHandle, LogHdr, sizeof(*LogHdr))
            != sizeof(*LogHdr))
           rc = NAVLOG_ERR_WRITE;
    }
    else
        if (FileRead(LogFile[hLog].wHandle, LogHdr, sizeof(*LogHdr))
            != sizeof(*LogHdr))
           rc = NAVLOG_ERR_READ;

    LogFile[hLog].dwCurOff = sizeof(*LogHdr);
    return rc;
} // ALogHeader()

//** ALogConsolidate() ************************************************
// This routine "compacts" the log file, moving the first record to the
// top and "unwrapping" the records (if the file is wrapped).
//
// For wrapped files, there are two methods this could be implemented:
//
// 1)  Write the log file to a new log file by starting with the first
//     record of the log and writing the records out to the new file
//     in record order.  This will automatically unwrap the file.
//     This method can take, potentially, twice as much disk space as
//     as the size of the log file itself.
//
// 2)  This method will take less disk space than the first method,
//     but is more complex.
//
//     a) Copy the first record of the file (not necessarily the first
//        record of the log) through the last record of the log to a
//        temporary log.
//     b) Move first record of the log through the last record of the
//        file to the top.
//     c) Copy the records from the temporary log to the end of the
//        modified log (from the previous step).
//
// If you really want to get fancy, you can do method (1) when the size
// of the first record of the log through the last record of the file
// is less than the size of the first record in the file through the
// last record in the log.
//
// Otherwise, choose the second method.
//
//*********************************************************************
// 06/01/93 WRL Consolidate log file
//*********************************************************************
int LOCAL PASCAL ALogConsolidate(UINT hLog)
{
    if (hLog != HFILE_ERROR && LogFile[hLog].wHandle != HFILE_ERROR)
    {

    }
    return NOERR;
} // ALogConsolidate()


#ifndef SYM_NLM
VOID LOCAL MoveToRecordHeader (HFILE hFile, WORD wRecNo)
{
    auto    LOGHEADER   LogHeader;
    auto    LOGRECORD   LogRecord;
    auto    DWORD       dwGotoOffset;
    auto    WORD        i;

    FileSeek (hFile, 0L, SEEK_SET);

    if (FileRead (hFile, &LogHeader, sizeof(LogHeader)) == sizeof(LogHeader))
        {
        dwGotoOffset = LogHeader.dwFirstEntry;

        for ( i = 0; i < wRecNo && dwGotoOffset != NAVLOG_EOF; i++ )
            {
            FileSeek (hFile, dwGotoOffset, SEEK_SET);

            if (FileRead (hFile, &LogRecord, sizeof(LogRecord)) ==
                                             sizeof(LogRecord))
                {
                dwGotoOffset = LogRecord.dwNextEntry;
                }
            else
                break;
            }

        dwGotoOffset = FileSeek (hFile, 0L, SEEK_CUR);
        dwGotoOffset -= sizeof(LogRecord);
        FileSeek (hFile, dwGotoOffset, SEEK_SET);
        }
}
#endif  // ifndef SYM_NLM

HFILE SYM_EXPORT WINAPI GetTrueHandle (UINT hLog)
{
    auto    HFILE       hTrueHandle = 0;
    auto    LPLOGFILE   lpFile;

    if (hLog != HFILE_ERROR && LogFile[hLog].wHandle != HFILE_ERROR)
        {
        lpFile = &LogFile[hLog];
        hTrueHandle = lpFile->wHandle;
        }

    return (hTrueHandle);
}


#ifdef SYM_NLM


LONG FileRead( int hHandle, void *cBuffer, LONG lLen )
{
    RelinquishControl( );

    return (read( hHandle, cBuffer, lLen ));
}

LONG FileWrite( int hHandle, void *cBuffer, LONG lLen )
{
    RelinquishControl( );

    return (write( hHandle, cBuffer, lLen ));
}

#endif
