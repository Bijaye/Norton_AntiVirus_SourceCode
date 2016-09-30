//*********************************************************************
// Copyright 1993 Symantec, Peter Norton Group
//
// $Header:   S:/SYMALOG/VCS/symalog.c_v   1.52   12 Jun 1998 19:21:02   DBuches  $
//
// This file contains the "core" routines for generating managing
// an Activity Log for Anti-Virus reporting.
//
// Entry Points:
//    LOCAL DOSLoadString()   - Mimics Windows' LoadString for Dos
//    ALogStart()             - ALog Component initialization
//    ALogDone()              - ALog Component termination
//    ALogBuildFile()         - Create viewable log file
//    LOCAL BuildLogRecord()  - Build a single record from log data
//    SearchForField()        - Search for field type in log data
//    LOCAL KeyWord ()        - Expand token to record data content
//    LOCAL FilterLog()       - Determine whether to include log record
//
// See Also:
//    SYMAPRT.C SYMARC.H SYMALOG.H
//
// cc: "CL -c -P -W3 -Alfw  -Gs2D -Zpe -NT _SYMALOG -Zi -Od %s"
//Kedit:Tabs Incr 3
//Kedit:EditV SetF FExt CPP
//
// Future Enhancements:
// - Clean up this ugly interface!!
//*********************************************************************
// $Log:   S:/SYMALOG/VCS/symalog.c_v  $
// 
//    Rev 1.52   12 Jun 1998 19:21:02   DBuches
// Use correct value for field length
// 
//    Rev 1.51   08 Jun 1998 13:00:58   DBuches
// For Win32 platforms, allways display time in 24 hour format.
//
//    Rev 1.50   28 May 1998 16:13:48   DBuches
// Use Win32 function for formatting time strings.
//
//    Rev 1.49   24 Feb 1998 14:32:26   CEATON
// Exported functions FilterLog() and KeyWord() for READLOG project.
//
//
//    Rev 1.48   12 Sep 1997 11:39:38   KSACKIN
// Ported changes from QAKG.
//
//    Rev 1.47.2.1   22 Aug 1997 12:43:10   DBUCHES
// Fixed problem with displaying international dates.
//
//    Rev 1.47.2.0   12 Aug 1997 22:46:44   DALVIDR
// Branch base for version QAKG
//
//    Rev 1.47   12 Aug 1996 13:37:02   PVENKAT
// Fixed #63851.  Always we open the log file with READ_WRITE access instead
// of READ_ONLY.
//
//
//    Rev 1.46   05 Aug 1996 15:11:20   JBRENNA
// LuigiPlus Port:
//   JBRENNA:
//     STS 61316 - The NAnsiToOem function capitalizes all strings. This
//     resulted in the format string changing. The format character for the date
//     and time was capitalized. This resulted in a failure to process the
//     format character.
//
//     Changed to simply use AnsiToOem on SYM_WIN platforms and nothing on
//     non-SYM_WIN platforms.
//
//   MKEATIN:
//     Changed AnsiToOem() to NAnsiToOem() in order to compile on all platforms.
//
//   YVANG:
//     Added 'AnsiToOem()' to convert loaded resource strings which are in
//     ANSI format into OEM format. This is done so that .TXT file of activity
//     log now is in OEM format. The file will be converted later into ANSI
//     before display.
//
//    Rev 1.45   25 Jul 1996 11:20:10   MZAREMB
// Changed two functions from being static (LOCAL) to being public. This
// facilitated the capability of accessing activity log files from outside of
// a Norton program.
//
//    Rev 1.44   11 Jul 1996 15:39:00   PVENKAT
// Enclosed NT-only code within #ifdef SYM_WIN32.
//
//
//    Rev 1.43   26 Jun 1996 16:45:00   YVANG
// Ported from Luigi-Plus.
//
//    Rev 1.42   10 Jun 1996 16:06:42   PVENKAT
// Modified the read mode to READ_ONLY for Log file.
//
//    Rev 1.41   08 Mar 1996 14:10:20   YVANG
// Enabled DBCS.
//
//    Rev 1.40   12 Jan 1996 12:13:00   JWORDEN
// Port changes from Quake 8
//
//    Rev 1.39   09 Nov 1995 13:17:14   GDZIECI
// Roll over Keith's changes from QUAKE 6.
//
//    Rev 1.38   20 Dec 1994 17:10:20   MARKL
// Fixed where SYMALOG was writing UINTs to the disk.  This doesn't work where
// 16 and 32 bit components have to share the same log file and be able to
// read/write each other's data.  These UINTs were changed to type WORD.
//
//    Rev 1.37   15 Mar 1994 12:05:14   BRUCE
// Changed EXPORT to SYM_EXPORT
//
//    Rev 1.36   09 Mar 1994 20:55:32   BRAD
// Fixed WIN32 warnings/errors
//
//    Rev 1.35   28 Feb 1994 16:20:32   BRAD
// Updated for WIN32.
//
//    Rev 1.34   08 Nov 1993 14:18:36   MFALLEN
// Fixed typo.
//
//    Rev 1.33   12 Oct 1993 16:46:10   PATRICKM
// Baseline from QAK1 branch
//
//    Rev 1.31.1.16   30 Sep 1993 14:05:36   keith
// Use IsBadxPtr() functions now supported in Mirrors.DLL 3.01
//
//    Rev 1.31.1.15   19 Sep 1993 22:13:14   SGULLEY
// fixed users filter
//
//    Rev 1.31.1.14   19 Sep 1993 10:40:56   MFALLEN
// Problems on segment boundaries.
//
//    Rev 1.31.1.13   16 Sep 1993 23:41:34   MFALLEN
// In the activity log file translation function from binary to text was opening
// the log file in RO mode. When closing it down, an attempt to write to the
// header was being made with an error as result. The file is now opened RW.
//
//    Rev 1.31.1.12   15 Sep 1993 20:41:50   BARRY
// Detect errors when closing files and propogate up to caller
//
//    Rev 1.31.1.11   15 Sep 1993 16:04:58   keith
// Avoid unsupported Mirrors calls.
//
//    Rev 1.31.1.10   15 Sep 1993 12:37:48   MFALLEN
// Added pointer validation functions to make sure that SYMALOG doesn't crashes
// with an invalid activity log file.
//
//    Rev 1.31.1.9   14 Sep 1993 01:04:28   SGULLEY
// MARTIN fixed users code
//
//    Rev 1.31.1.8   13 Sep 1993 22:58:20   MFALLEN
// The code that was checking if a specified user name is present in an
// activity log record was using a wrong pointer.
//
//    Rev 1.31.1.7   10 Sep 1993 21:29:02   EHU
// Fixed sense of b24hour in KeyWord().
//
//    Rev 1.31.1.6   09 Sep 1993 15:54:48   DALLEE
//
//    Rev 1.31.1.5   09 Sep 1993 15:46:48   DALLEE
// #ifdef around Windows API call (GetProfileInt()).
//
//    Rev 1.31.1.4   09 Sep 1993 13:09:14   SGULLEY
// backed out change. left martin a note as to where it GP's when the log
// is larger than its specified limit
//
//    Rev 1.31.1.3   09 Sep 1993 12:19:54   SGULLEY
// placed temp mem check on lpLogRecord
//
//    Rev 1.31.1.2   09 Sep 1993 10:01:44   MFALLEN
// Exit gracefully if a bad activity log file is encountered.
//
//    Rev 1.31.1.1   07 Sep 1993 17:27:38   SGULLEY
// added 24hr support
//
//    Rev 1.31.1.0   03 Sep 1993 21:53:42   SCOTTP
// Branch base for version QAK1
//
//    Rev 1.31   02 Sep 1993 22:33:24   MFALLEN
// Added progress bar when loading activity log file.
//
//    Rev 1.30   02 Sep 1993 18:28:42   BARRY
// Guarantee termination in DOSLoadString
//
//    Rev 1.29   23 Aug 1993 20:11:26   BARRY
// Fix definition of 'T' type
//
//    Rev 1.28   22 Aug 1993 23:53:18   BARRY
// Added MODULE_NAME for VMM debugging
//
//    Rev 1.27   19 Aug 1993 11:55:26   MFALLEN
// Fixed bug with Between and Not Between.
//
//    Rev 1.26   16 Aug 1993 12:55:42   MFALLEN
// Added fix that can't be tested because quake is busted. Will check shortly.
//
//    Rev 1.25   14 Aug 1993 16:15:00   MARKL
// Synchronized with qak0
//
//    Rev 1.23.1.3   12 Aug 1993 18:20:04   BRAD
// Removed warning.
//
//    Rev 1.23.1.2   08 Aug 1993 15:01:28   MFALLEN
// Adjusted buffer size.
//
//    Rev 1.23.1.1   08 Aug 1993 14:45:52   MFALLEN
// Modified creation of viewable activity log file to be faster if there
// is memory to read in the entire log file into memory.
//
//
//    Rev 1.23.1.0   02 Aug 1993 10:07:22   SCOTTP
// Branch base for version QAK0
//
//    Rev 1.23   15 Jul 1993 13:25:42   MFALLEN
// The functions ALogRecReadHdr() and ALogRecReadData() take an additional
// parameter that specifies what activity log record number to retrieve
// the information from.
//
//    Rev 1.22   14 Jul 1993 18:11:12   MFALLEN
// Open log file is now called with the size of the file it should be.
//
//    Rev 1.21   02 Jul 1993 15:18:42   BRAD
// Fixed for C8.
//
//    Rev 1.20   01 Jul 1993 12:39:56   DALLEE
// extern'd new DOS string.  Commented out LPDOMAIN member of
// struct ALOG.  Was blowing up DOS and WINDOWS compiles and didn't
// appear to be used.
// 04/20/93 WRL is William R. Lee II (BLEE)
//            - Moved to Quake from NIRVANA:ACTILOG
// 06/28/93 WRL Allow the log to specify that a local output template is
//              to be used for that record.
//*********************************************************************
#include "symalogi.h"                   // Project specific dcls
#include "symintl.h"                    // NIntlInit()
                                        // NGetIntlDate()
#include "xapi.h"                       // ConvertWordToString()
                                        // ConvertDateToEncoded()
#include "file.h"                       // FileCreate() FileClose()
                                        // FA_NORMAL READ_ONLY_FILE
#include "logio.h"                      // LogOpen() LogRead() LogClose()
                                        // LogGetCurrentEntries()
#include "dbcs.h"                       // DBCS

#if defined(SYM_WIN) || defined(SYM_NLM)
#  include "ngui.h"                     // NG_Init()  NG_Done()
#endif

                                        // --------------------------------
                                        // This variable is used internally
                                        // to indicate if we detected that
                                        // the log file is corrupted or not
                                        // --------------------------------
static BOOL gbLogFileCorrupted;

MODULE_NAME;

#if !defined(SYM_WIN) && !defined(SYM_NLM)    // Dos string support

extern char    IDS_VIRLISTCHANGE [];
extern char    IDS_ERROR         [];
extern char    IDS_INOCHANGE     [];
extern char    IDS_KNOWNVIRUS    [];
extern char    IDS_UNKNOWNVIRUS  [];
extern char    IDS_LOADUNLOAD    [];
extern char    IDS_STARTEND      [];
extern char    IDS_STATUS        [];
extern char    IDS_VIRUSLIKE     [];
extern char    IDS_PT_NLM        [];
extern char    IDS_PT_TSR        [];
extern char    IDS_PT_WINDOWS    [];
extern char    IDS_INGOING       [];
extern char    IDS_OUTGOING      [];

extern char    IDS_MSGACTIONTMPL [];

extern char    IDS_DEFPRINTFILE  [];
extern char    IDS_COMMDLG_TITLE [];

extern char    IDS_ZAPFILE       [];

#define LoadString(h,s,d,n) DOSLoadString(s,d,n)

//********************************************************************
// DOSLoadString()
//
// Funny business to cover LoadString() calls.  In DOS compiles,
// all IDS_XXX's are character arrays in SYMALOGS.STR, not string ID's.
//
// Parameters:
//      LPCSTR  lpSource                Source string.
//      LPSTR   lpDest                  Destination buffer.
//      WORD    wLength                 Buffer size (including EOS)
//
// Returns:
//      WORD                            length of string copied.
//********************************************************************
// 5/31/93 DALLEE, Function created.
//********************************************************************
WORD LOCAL PASCAL DOSLoadString (LPCSTR lpSource, LPSTR lpDest, WORD wLength)
{
    STRNCPY(lpDest, lpSource, wLength);
    lpDest[wLength-1]=EOS ;             // Guarantee terminated

    return(STRLEN(lpDest));
} // End DOSLoadString()

//&? End of DOS string kludges.
#endif


//*********************************************************************
// - Local Function Prototypes
//*********************************************************************

UINT LOCAL BuildMemoryPlenty (
    UINT hLog,                         // In: Log file handle
    HFILE hDispFile,                    // In: Display file handle
    LPALOGFILTER lpFilter,              // In: Filters conditions
    LPALOG lpALog,                      // In: Convert to something useful
    LPSTR FAR *szDescriptions,          // In: Array of file descrip
    WORD FAR *wEntries,
    LOGCALLBACK lpfn,
    DWORD        dwUserData);

UINT LOCAL BuildMemoryStarved (
    UINT hLog,                         // In: Log file handle
    HFILE hDispFile,                    // In: Display file handle
    LPALOGFILTER lpFilter,              // In: Filters conditions
    LPALOG lpALog,                      // In: Convert to something useful
    LPSTR FAR *szDescriptions,          // In: Array of file descrip
    WORD  FAR *wEntries,
    LOGCALLBACK lpfn,
    DWORD        dwUserData);

BOOL SYM_EXPORT WINAPI MemRecReadData(      // Read record data
    HPSTR hpMem,
    WORD  wLength,                      // Amount of data to read
    LPSTR lpData,                       // Buffer to fill
    WORD  wRecNo);

BOOL SYM_EXPORT WINAPI MemRecReadHdr(       // Read a record
    HPSTR   hpMem,                      // In:
    LPWORD  lpwType,                    // Out: Record type
    LPDWORD dwKey,                      // Out: Data key
    LPWORD  lpwSize,                    // Out: size of record
    WORD    wRecNo);                    // Record Number to read

UINT LOCAL BuildLogRecord(              // Build individual record
   LPALOG hALog,                        // In: Handle for this component
   HFILE  hFile,                        // In: Output file
   WORD   wRecType,                     // In: Record type
   DWORD  FieldFlags,                   // In:
   LPSTR  Description,                  // In: Log file description
   LPSTR  lpRecord,                     // In: Raw log record
   int    TypeIdx,                      // In: Index into typecode array
   WORD   wSize);                       // IN: SIZE OF lpRecord

//** ALogStart() ******************************************************
//
// This routine initializes the component.  A ALOGINIT structure of
// "application global" info is passed.  The number of types is the
// number of record types that will be screened by the filter.
// lpFormats is an array (nTypes long) of structures describing the
// types.  Each element of the structure contains the record type and
// its associated format string ID (this string ID is referenced from
// the instance of the caller).
//
// Each template contains '%' delimited values along with constant
// text.  When generating a displayable log file, the '%' delimited
// values are replaced by corresponding record content.
//
// Future Enhancements:
// - Chain multiple Alog blocks for multiple calls.
// - Check errors in memory allocation
//*********************************************************************
// 05/06/93 WRL
//*********************************************************************
LPVOID SYM_EXPORT WINAPI ALogStart(         // Component initialization
   HINSTANCE  hCaller,                  // Instance of caller
   LPALOGINIT InitStruct)               // Initialization parameters
{
   LPALOG hALog;

#  if defined(SYM_WIN) || defined(SYM_NLM)
      NG_Init();                        // Initialize SymGui
#  endif
   NIntlInit("Intl");                   // Initialize international settings

   hALog = (LPALOG)MemAllocPtr(GHND,sizeof(ALOG));

   hALog->hInst= hCaller;
   hALog->Init = *InitStruct;

   if (InitStruct->lpFormats) {         // Copy format array
      hALog->Init.lpFormats = MemAllocPtr(GHND,
                              sizeof(ALOGFORMAT)*InitStruct->nTypes);
      MEMCPY(hALog->Init.lpFormats,InitStruct->lpFormats,
             sizeof(ALOGFORMAT) * InitStruct->nTypes);
   } /* then */

   return (LPVOID)hALog;
} // ALogInit()

//** ALogDone() *******************************************************
// This is just for symetry with the Init call.
//
// Future Enhancements:
//*********************************************************************
// 05/06/93 WRL
//*********************************************************************
UINT SYM_EXPORT WINAPI ALogDone(            // Component termination
   LPVOID hDummy)                       // handle returned by init
{
   LPALOG hALog=(LPALOG)hDummy;         // Make into something useful

#  if defined(SYM_WIN) || defined(SYM_NLM)
      NG_Done();                        // Terminate  SymGui
#  endif
   if (hALog->Init.lpFormats)
      {
      MemFreePtr(hALog->Init.lpFormats);
      }

   MemFreePtr(hALog);

   return NOERR;
} // ALogDone()

//** ALogBuildFile() **************************************************
// Extract information from Log file for display.
//
// This call creates a temporary file that contains the formatted,
// filtered records from the specifed activity logs.  An array of
// pointers to log files are passed.  An array of pointers to
// descriptions for each file are also passed.  These descriptions
// may be incorporated into the output based on the %Description%
// variable usage in the formatting strings passed to symALogInit().
//
// A structure containing filter information is also passed.  This
// structure contains the structure passed to the symALogAdd function
// so that those same fields may be used to filter the output records.
// Note that an array of booleans is passed (why isnt this a bit
// bit vector?!), which specifies which corresponding record types are
// to be included in the output.  The order of the values in this
// array correspond to the order of ALOGFORMAT array passed to
// the Init function.
//
// Future Enhancements:
// - Add return code
// - Check size of filter struct
// - Sort records across all files by date/time
// - Sort by user.
// - If the TypeIncl pointer in the filter array is NULL, include all
// - Must sort records for all log files
//*********************************************************************
// 04/20/93 WRL
//*********************************************************************
int SYM_EXPORT WINAPI ALogBuildFile(        // Create viewable log file
   HALOG hDummy,                        // In: Dummy project handle
   WORD nLogs,                          // In: Number of log files
   LPSTR FAR *szLogName,                // In: Array of file names
   LPSTR FAR *szDescriptions,           // In: Array of file descrip
   LPALOGFILTER pFilter,                // In: Filters conditions
   LPSTR szFile,                        // I/O: Temp file name
   DWORD dwMaxSize,                     // max size of log file
   LOGCALLBACK lpfn,
   DWORD        dwUserData)
{
    auto    HFILE   hFile;              // Display file
    auto    WORD    wEntries;           // Number of records displayed
    auto    LPSTR FAR *pLogArry;        // Pointer to array of log names
    auto    LPALOG  lpALog = (LPALOG)hDummy;  // Convert to something useful
    auto    UINT    hLog;               // Log file "handle"
    auto    WORD    CurSize=256;        // Buffer size
    auto    int     nRet = ALOGERR_NOERR;

    if (!szFile[0])                      // If not defined,
        hFile=FileCreateTemporary(szFile,FA_NORMAL);
    else
        hFile=FileCreate(szFile, FA_NORMAL);

    gbLogFileCorrupted = FALSE;

    if (hFile == HFILE_ERROR)
        nRet = ALOGERR_DISPFILE;
                                        // ------------------------
                                        // Loop for each log file
                                        // ------------------------
    for (wEntries=0, pLogArry=szLogName;
         nRet==ALOGERR_NOERR, pLogArry < szLogName+nLogs;
         pLogArry++)
        {
        if (LogOpen(*pLogArry, READ_WRITE_FILE, &hLog, FALSE, dwMaxSize) != NOERR)
            nRet = ALOGERR_OPENLOG;         // Error !!!

        if (BuildMemoryPlenty (hLog, hFile, pFilter, lpALog, szDescriptions,
                               &wEntries, lpfn, dwUserData))
            {
            if (BuildMemoryStarved (hLog, hFile, pFilter, lpALog, szDescriptions,
                                &wEntries, lpfn, dwUserData))
                nRet = ALOGERR_MEMORY;
            }

        if ( nRet != ALOGERR_OPENLOG )
            if ( LogClose(hLog) != NOERR )
                nRet = ALOGERR_CLOSELOG;

        if (szDescriptions)             // If descriptions given,
            szDescriptions++;           //    use next one.
        }

    if (hFile != HFILE_ERROR)
        {
        FileWrite(hFile," ",1);         // Ensure that something is
        FileClose(hFile);               // written out.
        }

    if ( nRet == ALOGERR_NOERR )
        nRet = (gbLogFileCorrupted == TRUE) ? ALOGERR_BADLOGFILE : wEntries;

    return ( nRet );

} // BuildLogInfo()

UINT LOCAL BuildMemoryPlenty (
    UINT hFakeLog,                     // In: Log file handle
    HFILE hDispFile,                    // In: Display file handle
    LPALOGFILTER lpFilter,              // In: Filters conditions
    LPALOG lpALog,                      // In: Convert to something useful
    LPSTR FAR *szDescriptions,          // In: Array of file descrip
    WORD  FAR *wEntries,
    LOGCALLBACK lpfn,
    DWORD   dwUserData)
{
    auto    HFILE   hLog;
    auto    UINT    uStatus = 0;
    auto    HPSTR   hpMem = NULL;
    auto    LPSTR   lpMem = NULL;
    auto    DWORD   dwOrgOffset;
    auto    DWORD   dwFileSize;
    auto    DWORD   FieldFlags;
    auto    DWORD   dwReadSize = 0;
    auto    int     TypeIdx;
    auto    WORD    wNumEntries;
    auto    WORD    wType;
    auto    WORD    wSize;
    auto    WORD    wBufferSize = 0;
    auto    WORD    i;
    auto    LPLOGHEADER     lpLogHeader;

    hLog = GetTrueHandle (hFakeLog);
    dwOrgOffset = FileSeek (hLog, 0L, SEEK_CUR);

    dwFileSize = FileLength (hLog);

    if (dwFileSize && (hpMem = MemAllocPtr (GHND, dwFileSize)) != NULL)
        {
        wNumEntries = LogGetCurrentEntries (hFakeLog);

        FileSeek (hLog, 0L, SEEK_SET);

        if (FileHugeRead (hLog, hpMem, dwFileSize) == dwFileSize)
            {
            for ( i=0 ; i < wNumEntries; i++)
                {
                if (MemRecReadHdr (hpMem, &wType, &FieldFlags, &wSize, i))
                    break;

                // This next code segment is used to check and see if the
                //  activity log is corrupted or truncated. At a minimum,
                //  it attempts to prevent a GPF.
                dwReadSize += (DWORD)wSize;         // Keep track of processing
                if ( dwReadSize > dwFileSize )      // Bad size or truncated
                {
                    lpLogHeader = (LPLOGHEADER)hpMem;
                    wSize = (WORD)(dwReadSize - dwFileSize );
                    if ( wSize > (WORD)lpLogHeader->dwFirstEntry )
                        wSize -= (WORD)lpLogHeader->dwFirstEntry;
                    wNumEntries = i;                // Stop the iterations
                }
                if (wBufferSize < wSize)
                    {
                    if ( lpMem )
                        MemFreePtr (lpMem);

                    if ((lpMem = MemAllocPtr(GHND, wSize + sizeof(LOGRECORD))) == NULL)
                        break;

                    wBufferSize = wSize;
                    }
                else
                    MEMSET (lpMem, EOS, wBufferSize);

                if (MemRecReadData(hpMem, wSize, lpMem, i))
                    break;
                                        // If record is w/in date
                                        // range, then output record.
                if (FilterLog(lpALog, wType, FieldFlags, lpMem, lpFilter,&TypeIdx, wSize))
                    {
                    (*wEntries)++;
                    BuildLogRecord (lpALog, hDispFile, wType, FieldFlags,
                                    (szDescriptions ? *szDescriptions : NULL),
                                    lpMem,TypeIdx, wSize);
                    }

                                        // ---------------------------------
                                        // Notify callback function about
                                        // our great progress.
                                        // ---------------------------------
                if (lpfn && (*lpfn)(i, wNumEntries, dwUserData))
                    break;
                }
            }

        if (lpMem)
            MemFreePtr (lpMem);

        MemFreePtr (hpMem);
        }
    else
        uStatus = NAVLOG_ERR_MEMORY;

    FileSeek (hLog, dwOrgOffset, SEEK_SET);
    return (uStatus);
}

UINT LOCAL BuildMemoryStarved (
    UINT hLog,                         // In: Log file handle
    HFILE hDispFile,                    // In: Display file handle
    LPALOGFILTER lpFilter,              // In: Filters conditions
    LPALOG lpALog,                      // In: Convert to something useful
    LPSTR FAR *szDescriptions,          // In: Array of file descrip
    WORD FAR *wEntries,
    LOGCALLBACK lpfn,
    DWORD   dwUserData)
{
    #define SMALLBUFFER 256
    auto    DWORD   FieldFlags;
    auto    int     TypeIdx;
    auto    UINT    uStatus = 0;
    auto    WORD    wNumEntries;
    auto    WORD    wType;
    auto    WORD    wSize;
    auto    WORD    i;
    auto    WORD    wCurSize;
    auto    LPSTR   lpMem;

    wNumEntries = LogGetCurrentEntries (hLog);

    if ((lpMem = MemAllocPtr(GHND, SMALLBUFFER)) != NULL)
        {
        wCurSize = SMALLBUFFER;

        for ( i=0 ; i < wNumEntries; i++)
            {
                                        // Read log record
            if (ALogRecReadHdr(hLog, &wType, &FieldFlags, &wSize, (WORD)(i+1)) != NOERR)
                break;                  // If error, Quit loop

            if (wSize > SMALLBUFFER)
                {
                MemFreePtr(lpMem);

                if ((lpMem = MemAllocPtr(GHND, wCurSize=wSize)) == NULL)
                    break;
                } /* then */

            if (ALogRecReadData(hLog, wSize, lpMem, (WORD)(i + 1)))
                break;
                                        // If record is w/in date
                                        // range, then output record.
            if (FilterLog(lpALog, wType, FieldFlags, lpMem, lpFilter,&TypeIdx, wSize))
                {
                (*wEntries)++;
                BuildLogRecord (lpALog, hDispFile, wType, FieldFlags,
                                (szDescriptions ? *szDescriptions : NULL),
                                lpMem,TypeIdx, wSize);
                }   // if
                                        // ---------------------------------
                                        // Notify callback function about
                                        // our great progress.
                                        // ---------------------------------
            if (lpfn && (*lpfn)(i, wNumEntries, dwUserData))
                break;
            }

        MemFreePtr(lpMem);              // Free mem allocated by Read
        }
    else
        uStatus = NAVLOG_ERR_MEMORY;

    return (uStatus);
}

//** BuildLogRecord() *************************************************
// Construct an output record from the raw log data, parsing the
// data according to a template in the parent as necessary for display.
//
// For each log record, read each template and append to the file.
// After all templates are processed, output a "newline".
//
// For each template perform variable substitution.
//
// Note:
// This implementation must load the format template from the string
// table each time since it gets obliterated when it is parsed (by
// STRTOK()).  Also, notice that there is a bug(?) in STRTOK() that
// returns NULL when %% occurs (at least at the end of a string).  So
// %% cannot be used to indicate a literal % without extra processing.
//
// Future Enhancements:
//*********************************************************************
// 04/20/93 WRL
//*********************************************************************
UINT LOCAL BuildLogRecord(              // Construct output record
   LPALOG hALog,                        // In: Handle for this component
   HFILE  hFile,                        // In: Output file
   WORD   RecType,                      // In: Record type
   DWORD  FieldFlags,                   // In:
   LPSTR  Description,                  // In: Log file description
   LPSTR  lpRecord,                     // In: Raw log record
   int    TypeIdx,                      // In: Index into typecode array
   WORD   wSize)
{
   LPSTR    lpMem       = NULL;         // Working output buffer
#  define   TMPMEMSIZE  1000            // Working output buffer size
#  define   LINEBRK     "\r\n"          // Working output buffer size
#  define   SUBSTCHAR   '#'             // Substitution delimiter char
   WORD     wTotalBytes = 0;            // Return: total bytes output

   if (lpMem = MemAllocPtr(GHND, TMPMEMSIZE)) {
      LPSTR    pOutput;                 // Ptr to output buffer
#     define   MAXFORMATLEN  255        // Max length of format string
                                        // &? YVANG Enough for DBCS?
      char     Format[MAXFORMATLEN+1];  // Format buffer
      LPSTR    sTempl, eTempl;          // Ptrs to format string

      pOutput=lpMem;                    // Reset output buffer
                                        // Load string from caller
      if ((FieldFlags&ALOG_FTB_LOCALTMPL) // Use local template.
                                        // Ensure that MESSAGE/ACTION
                                        // fields are included.
          && (FieldFlags & (ALOG_FTB_MESSAGE|ALOG_FTB_ACTION))
                         == (ALOG_FTB_MESSAGE|ALOG_FTB_ACTION))
      {
         LoadString(hDLLInst,IDS_MSGACTIONTMPL,Format,MAXFORMATLEN);
      } /* then */
      else
      {
          char     FormatAnsi[MAXFORMATLEN+1];  // Format buffer for Ansi

         LoadString(hALog->hInst,
                    hALog->Init.lpFormats[TypeIdx].SID,
                    FormatAnsi, MAXFORMATLEN);

                                        // When running windows, cover the string to OEM.
                                        // We cannot call NAnsiToOem because it captializes
                                        // the format string. Thus, changing the format characters
                                        // such that the are no longer parsed correctly.
#ifdef SYM_WIN
         AnsiToOem (FormatAnsi, Format);
#else
         STRNCPY (Format, FormatAnsi, MAXFORMATLEN);
#endif
      }

      for (sTempl=Format; eTempl=SYMstrchr(sTempl,SUBSTCHAR); sTempl=eTempl+1)
      {
         STRNCPY(pOutput,sTempl,eTempl-sTempl); // Copy "static" part
         pOutput += eTempl-sTempl;

         eTempl++;                      // Skip delimiter

         if (!eTempl)                   // No more string left
            break;                      //   so skip on out of here
         else {                         // Process variable name
            WORD Len;
            KeyWord(lpRecord, *eTempl, RecType, FieldFlags,
                    Description, (WORD)(TMPMEMSIZE+lpMem-pOutput),
                    pOutput, &Len,
                    wSize);
            pOutput += Len;             // Move ptr ahead
         } /* else */
      } // for

      if (*sTempl) {                    // Copy remaining
         STRCPY(pOutput, sTempl);
         pOutput += STRLEN(sTempl);
      } /* then */

      //************************************************************
      // We have processed this template. If our temporary buffer
      // contains information, write this record to file.
      //************************************************************
      STRCPY(pOutput, LINEBRK);         // Terminate line
      FileWrite(hFile, lpMem, pOutput-lpMem+STRLEN(LINEBRK));
      wTotalBytes += pOutput-lpMem;     // Keep track of total bytes

      MemFreePtr(lpMem);
   } // if

   return (wTotalBytes);
} // BuildLogRecord()

//*********************************************************************
// Search record data for requested field. Return pointer to field
// data and the data's length.
//
//&? Future Enhancements:
// - Once we know that log files are stable between the DOSTSR generated
//   logfile and the Windows/SYM_NLM versions, we can take the cast to
//   char out from the comparison statement.
//*********************************************************************
LPSTR SearchForField(
   ALOGFIELD FieldID,
   LPSTR Data,                          // Data
   FIELDLEN FAR *Len,
   WORD wSize)
{
    auto    LPSTR   lpRecordData;
    auto    int     nRecordLength = wSize;


    lpRecordData = Data;  // Start at beginning of field.

    while (*(FIELDLEN FAR *)lpRecordData)  // While there are fields left
        {
                                        // If field ID matches requsted
        if ((char)*((FIELDLEN FAR *)lpRecordData+1) == (char)FieldID)
            {
            if (Len)                       // Return length to caller
                *Len = *(FIELDLEN FAR *)lpRecordData - sizeof(FIELDLEN) -
                        sizeof(ALOGFIELD);

            return (lpRecordData + sizeof(FIELDLEN) + sizeof(ALOGFIELD));
            }

        nRecordLength -= *(FIELDLEN FAR *)lpRecordData;

#ifdef SYM_WIN
        if (nRecordLength <= 0 || IsBadReadPtr(lpRecordData, nRecordLength))
#else
        if (nRecordLength <= 0)
#endif
            {
            gbLogFileCorrupted = TRUE;
            break;
            }

        lpRecordData += *(FIELDLEN FAR *)lpRecordData;
        }

    if (Len)
        *Len = 0;

    return ( NULL );
}


//** KeyWord() ********************************************************
// Output corresponding text corresponding to the specified keyword.
//
//&? Future Enhancements:
// - Once we know that log files are stable between the DOSTSR generated
//   logfile and the Windows/SYM_NLM versions, we can take the casts to
//   char out from the case statements and change lpKey to ALOGFIELD.
//*********************************************************************
// 04/20/93 WRL
//*********************************************************************
UINT SYM_EXPORT WINAPI KeyWord (
   LPSTR lpRecord,                      // In: Raw log record
   char  lpKey,                         // In: Key name to check
   WORD  RecType,                       // In: Record type
   DWORD FieldFlags,                    // In:
   LPSTR szDesc,                        // In: Description string
   WORD  FieldLen,                      // In: Size of buffer size
   LPSTR szField,                       // Out: Return field replacement
   LPWORD Len,                          // Out: Len of return data
   WORD  wSize)                         // IN: LENGTH OF lpRecord
{
   BOOL b24Hour;

#ifdef SYM_WIN32
   SYSTEMTIME st;
#endif


#ifdef SYM_WIN
   b24Hour = !(BOOL)GetProfileInt ("INTL", "iTime", 0);
#else
   b24Hour = FALSE;
#endif

   *Len = 0;                            // Default to "no changes"
   switch (lpKey)
   {
   case ALOG_FT_ACTION:
      if (FieldFlags & ALOG_FTB_ACTION) {
         LPSTR    Data;
         Data = SearchForField(ALOG_FT_ACTION, lpRecord, Len, wSize);
         STRNCPY(szField,Data,min(FieldLen, *Len));
      } /* then */
      break;

   case ALOG_FT_DATE:
      if (FieldFlags & ALOG_FTB_DATE) {
         UINT Month, Day, Year;
         DWORD dwEncoded;
         dwEncoded = *(WORD FAR *)SearchForField(ALOG_FT_DATE, lpRecord, NULL, wSize);

         ConvertEncodedDateToNormal((UINT)dwEncoded, &Month, &Day, &Year);

#ifdef SYM_WIN32
         // Use correct date formatting function for Win32.
         ZeroMemory(&st, sizeof(SYSTEMTIME));

         st.wMonth = (WORD) Month;
         st.wDay = (WORD) Day;
         st.wYear = (WORD) Year;

         GetDateFormat( LOCALE_SYSTEM_DEFAULT,
                        DATE_SHORTDATE,
                        &st,
                        NULL,
                        szField,
                        FieldLen );
#else
         NSetIntlDate(szField, Month, Day, Year);
#endif
         *Len = STRLEN(szField);
      } /* then */
      break;

   case ALOG_FT_DIRECTION:
      if (FieldFlags & ALOG_FTB_DIRECTION) {
         WORD Direction;
         Direction = *(WORD FAR *)SearchForField(ALOG_FT_DIRECTION, lpRecord, NULL, wSize);

         if (Direction == ALOG_DIR_IN)
            *Len = LoadString(hDLLInst, IDS_INGOING, szField, FieldLen);
         else
            *Len = LoadString(hDLLInst, IDS_OUTGOING, szField, FieldLen);
      } /* then */
      break;

   case ALOG_FT_FILESIZE:
      if (FieldFlags & ALOG_FTB_FILESIZE) {
         DWORD Size;
         Size = *(DWORD FAR *)SearchForField(ALOG_FT_FILESIZE, lpRecord, NULL, wSize);

         ConvertLongToString(Size, szField);
         *Len = STRLEN(szField);
      } /* then */
      break;

   case ALOG_FT_MESSAGE:
      if (FieldFlags & ALOG_FTB_MESSAGE) {
         LPSTR    Data;
         Data = SearchForField(ALOG_FT_MESSAGE, lpRecord, Len, wSize);
         STRNCPY(szField,Data,min(FieldLen, *Len));
      } /* then */
      break;

   case ALOG_FT_NETWORK:
      if (FieldFlags & ALOG_FTB_NETWORK) {
         LPSTR    Data;
         Data = SearchForField(ALOG_FT_NETWORK, lpRecord, Len, wSize);
         STRNCPY(szField,Data,min(FieldLen, *Len));
      } /* then */
      break;

   case ALOG_FT_OS:
      if (FieldFlags & ALOG_FTB_OS) {
         LPSTR    Data;
         Data = SearchForField(ALOG_FT_OS, lpRecord, Len, wSize);
         STRNCPY(szField,Data,min(FieldLen, *Len));
      } /* then */
      break;

   case ALOG_FT_PATH:
      if (FieldFlags & ALOG_FTB_PATH) {
         LPSTR    Data;
         Data = SearchForField(ALOG_FT_PATH, lpRecord, Len, wSize);
         STRNCPY(szField,Data,min(FieldLen, *Len));
      } /* then */
      break;

   case ALOG_FT_PROGTYPE:
      if (FieldFlags & ALOG_FTB_PROGTYPE) {
         STRINGREF StrID;
         BYTE ProgType;
         ProgType = *SearchForField(ALOG_FT_DIRECTION, lpRecord, NULL, wSize);

         switch (ProgType) {
         case ALOG_PT_NLM:
            StrID = IDS_PT_NLM;
            break;
         case ALOG_PT_TSR:
            StrID = IDS_PT_TSR;
            break;
         case ALOG_PT_WINDOWS:
            StrID = IDS_PT_WINDOWS;
            break;
         default:
            StrID = IDS_ERROR;
         } /* switch */
         *Len = LoadString(hDLLInst, StrID, szField, FieldLen);
      } /* then */
      break;

   case ALOG_FT_SERVER:
      if (FieldFlags & ALOG_FTB_SERVER) {
         LPSTR    Data;
         Data = SearchForField(ALOG_FT_SERVER, lpRecord, Len, wSize);
         STRNCPY(szField,Data,min(FieldLen, *Len));
      } /* then */
      break;

   case ALOG_FT_TIME:
      if (FieldFlags & ALOG_FTB_TIME) {
         UINT Hour, Min, Sec;
         DWORD dwEncoded;
         dwEncoded = *(WORD FAR *)SearchForField(ALOG_FT_TIME, lpRecord, NULL, wSize);

         ConvertEncodedTimeToNormal((UINT)dwEncoded, &Hour, &Min, &Sec);

#ifdef SYM_WIN32
         // Use correct time formatting function for Win32.
         ZeroMemory(&st, sizeof(SYSTEMTIME));

         st.wHour = (WORD) Hour;
         st.wMinute = (WORD) Min;
         st.wSecond = (WORD) Sec;

         GetTimeFormat( LOCALE_SYSTEM_DEFAULT,
                         TIME_FORCE24HOURFORMAT | TIME_NOTIMEMARKER,
                         &st,
                         NULL,
                         szField,
                         FieldLen );

#else
         NSetIntlTime(szField, Hour, Min, Sec, TRUE, b24Hour);
#endif

         *Len = STRLEN(szField);
      } /* then */
      break;

   case ALOG_FT_USER:
      if (FieldFlags & ALOG_FTB_USER) {
         LPSTR    Data;
         Data = SearchForField(ALOG_FT_USER, lpRecord, Len, wSize);
         STRNCPY(szField,Data,min(FieldLen, *Len));
      } /* then */
      break;

   case ALOG_FT_VIRUSNAME:
      if (FieldFlags & ALOG_FTB_VIRUSNAME) {
         LPSTR    Data;
         Data = SearchForField(ALOG_FT_VIRUSNAME, lpRecord, Len, wSize);
         STRNCPY(szField,Data,min(FieldLen, *Len));
      } /* then */
      break;

   case ALOG_FT_RECTYPE:
      {
      STRINGREF StrID;

      switch (RecType) {
      case ALOG_FI_VIRLISTCHANGE:
         StrID = IDS_VIRLISTCHANGE;
         break;

      case ALOG_FI_ERROR:
         StrID = IDS_ERROR;
         break;

      case ALOG_FI_INOCHANGE:
         StrID = IDS_INOCHANGE;
         break;

      case ALOG_FI_KNOWNVIRUS:
         StrID = IDS_KNOWNVIRUS;
         break;

      case ALOG_FI_UNKNOWNVIRUS:
         StrID = IDS_UNKNOWNVIRUS;
         break;

      case ALOG_FI_LOADUNLOAD:
         StrID = IDS_LOADUNLOAD;
         break;

      case ALOG_FI_STARTEND:
         StrID = IDS_STARTEND;
         break;

      case ALOG_FI_STATUS:
         StrID = IDS_STATUS;
         break;

      case ALOG_FI_VIRUSLIKE:
         StrID = IDS_VIRUSLIKE;
         break;

      default:
         StrID = IDS_ERROR;
      } // switch (RecType)

      *Len = LoadString(hDLLInst, StrID, szField, FieldLen);
   }  break; // ALOG_FT_RECTYPE

   case ALOG_FT_DESC:
      if (szDesc) {
         LPSTR    Data;
         Data = SearchForField(ALOG_FT_USER, lpRecord, Len, wSize);
         STRNCPY(szField,Data,min(FieldLen, *Len));
      } /* then */
      break;

   case SUBSTCHAR:
      *szField = SUBSTCHAR;
      *Len = 1;
      break;

   default:
      ;
   } /* switch */

   return (NOERR);
} // KeyWord()

//** FilterLog() ******************************************************
// This function checks filter information to determine whether the
// record should be included.
//
// Return Values:
//    TRUE  include record
//    FALSE ignore record
//
// Future Enhancements:
// - Catch invalid filter values.
// - Generalize handling filter flags more generally.
//*********************************************************************
// 04/20/93 WRL
//*********************************************************************
#define CompareDate( StartDate, EndDate)/* Compare encoded dates     */ \
   ((StartDate) < (EndDate)             /*    returning -1, 0, 1 to  */ \
    ? -1                                /*    indicate that Start is */ \
    : (StartDate) == (EndDate)          /*    less than, equal, or   */ \
      ? 0                               /*    greater than End       */ \
      : 1)

BOOL SYM_EXPORT WINAPI FilterLog(                   // Returns TRUE if included
   LPALOG lpALog,                       // In:
   WORD   RecType,
   DWORD  FieldFlags,
   LPSTR  lpRecord,                     // In: Log record data
   LPALOGFILTER pFilter,                // In: Filter applied to record
   int FAR *TypeIdx,                    // Out:Index into typecode array
   WORD wSize)                          // IN: SIZE OF lpRecord
{
   int   i;

   //******************************************************************
   // Detemine whether this record is an accepted type.  Check the
   // array of types supported.  If a type occurs that is not in the
   // filter array, fail!
   //******************************************************************
   for (i = 0; i < lpALog->Init.nTypes; i++) {
      if (lpALog->Init.lpFormats[i].TypeCode == RecType)
      {
                                  // Check inclusion array (if defined)
         if (pFilter && !pFilter->pTypeIncl[i])
            return FALSE;         // Type not to be included.

         *TypeIdx = i;            // Return type index
         break;                   // Leave for loop
      } // if
   } // for
   if (i >= lpALog->Init.nTypes)        // Didnt find type so caller wont
      return FALSE;                     //   know what to do with it.

   //******************************************************************
   // Determine whether this record is within the specified date range.
   //******************************************************************
   if (pFilter && pFilter->DateLogic && pFilter->StartDate && pFilter->EndDate
       && (FieldFlags & ALOG_FTB_DATE))
   {                                    // Date found and logic spec'd
      int   i;                          // Within date range
      WORD RecDate;

      RecDate = *(WORD FAR *)SearchForField(ALOG_FT_DATE, lpRecord,  NULL, wSize);

      i = CompareDate(RecDate, pFilter->StartDate);

      switch (pFilter->DateLogic) {
      case ALOG_EQ_START:               // Date must be equal
         if (i)                         // So if compare is not ==
            return FALSE;               //    then reject
         break;

      case ALOG_LT_START|ALOG_GT_START: // Date must not be equal
         if (!i)                        // So if compare is ==
            return FALSE;               //    then reject
         break;

      case ALOG_LT_START:               // Date must be earlier
         if (i != -1)                   // So if compare is not earlier
            return FALSE;               //    then reject
         break;

      case ALOG_LT_START|ALOG_EQ_START: // Date must be equal or earlier
         if (i > 0)                     // So if compare is later
            return FALSE;               //    then reject
         break;

      case ALOG_GT_START:               // Date must be later
         if (i <= 0)                    // So if compare is == or earlier
            return FALSE;               //    then reject
         break;

      case ALOG_EQ_START|ALOG_GT_START: // Date must be equal or later
         if (i < 0)                     // So if compare is earlier
            return FALSE;               //    then reject
         break;

                                        // "Between including start, end"
      case ALOG_BETWEEN:                // Date is between start and end
                                        // So if compare is earlier than
                                        //    start or later than end
                                        //    (i.e., outside date range)
         if (i < 0 || CompareDate(RecDate, pFilter->EndDate) > 0)
            return FALSE;               //    then reject
         break;
                                        // "Between excluding start, end"
      case ALOG_GT_START|ALOG_LT_END:   // Date is between start and end
                                        // but not including start and end.
         if (i <= 0 || CompareDate(RecDate, pFilter->EndDate) >= 0 )
            return FALSE;               //    then reject
         break;
                                        // "Not between"
      case ALOG_EQ_START|ALOG_LT_START| // Date is not between start and end
           ALOG_EQ_END|ALOG_GT_END:     // So if compare is later than
                                        //    start or earlier than end
                                        //    (i.e., inside date range)
         if (i >= 0 && CompareDate(RecDate, pFilter->EndDate) <= 0)
            return FALSE;               //    then reject
         break;
                                        // "Outside"
      case ALOG_LT_START|ALOG_GT_END:   // Date is not between start and end
                                        // So if compare is later than
                                        //    start or earlier than end
         if (i >= 0 && CompareDate(RecDate, pFilter->EndDate) <= 0)
            return FALSE;               //    then reject
         break;
      } // switch
   } // if

   //******************************************************************
   // Determine if user is a match
   //******************************************************************

   if ( pFilter && pFilter->FldStruct.lpUser )
       {
       if (FieldFlags & ALOG_FTB_USER && pFilter)
          {
          LPSTR RecUser;
          LPSTR pUser;
          WORD Len;

          RecUser = SearchForField(ALOG_FT_USER, lpRecord, &Len, wSize);

          for (pUser=pFilter->FldStruct.lpUser; // Start at beginning of lst
              *pUser;                          // While double EOS not found
              pUser+=STRLEN(pUser)+1)          // Check next name in list
              {
              Len = STRLEN(pUser);

              if (Len + 1 == (WORD)STRLEN(RecUser))
                  if (!STRNICMP(pUser, RecUser, Len) && !pUser[Len])
                      break;                      // Found, so stop searching
              } /* for */

          if (!*pUser)                      // If pUser -> to EOS
             return FALSE;                  //    then user not found: rtn false
          }
       else
          return ( FALSE );
       }

   return TRUE;
} // FilterLog()


BOOL SYM_EXPORT WINAPI MemRecReadHdr(       // Read a record
    HPSTR   hpMem,                      // In:
    LPWORD  wType,                      // Out: Record type
    LPDWORD dwKey,                      // Out: Data key
    LPWORD  wSize,                      // Out: size of record
    WORD    wRecNo)                     // Record Number to read
{
    auto    BOOL            bExitSoonerThanExpected = FALSE;
    auto    LPLOGHEADER     lpLogHeader;
    auto    LOGRECORD       LogRecord;
    auto    DWORD           dwGoto;
    auto    WORD            i;
    auto    HPSTR           hpMemWalk = hpMem;

#ifdef VDEBUG
    static  char            szDebugFile[] = "dbg.txt";
    auto    HFILE           hDbgFile;
    auto    char            szBuf [1024];
    auto    char            szData1[65];
    auto    char            szData2[65];
    auto    char            szData3[65];
#endif

    lpLogHeader = (LPLOGHEADER)hpMem;


#ifdef VDEBUG
    if ((hDbgFile = FileOpen(szDebugFile, READ_WRITE_FILE)) == HFILE_ERROR)
        {
        MessageBox(NULL,"MemRecReadHdr(1):File Open Error.\n",NULL,MB_OK);
        return TRUE;
        }
    wsprintf(szBuf,"\n\rMemRecReadHdr(1): \n\r"
      "LogHeader.dwFirstEntry=%ld\n\r"
      "         .dwLastEntry=%ld\n\r"
      "         .wCurrentEntries=%d\n\r"
      "         .dwMaxSize=%ld\n\r",
      lpLogHeader->dwFirstEntry,
      lpLogHeader->dwLastEntry,
      lpLogHeader->wCurrentEntries,
      lpLogHeader->dwMaxSize);
    FileSeek(hDbgFile, FileLength(hDbgFile), SEEK_SET);
    if (FileWrite(hDbgFile, (LPSTR)szBuf, strlen(szBuf)) != strlen(szBuf))
        {
        MessageBox(NULL,"MemRecReadHdr(1):Debug File Write Error.\n",NULL,MB_OK);
        return TRUE;
        }
    FileClose(hDbgFile);
#endif


    dwGoto = lpLogHeader->dwFirstEntry;


    for ( i = 0; i < wRecNo; i++ )
        {
        hpMemWalk = hpMem + dwGoto;
        MEMCPY(&LogRecord, hpMemWalk, sizeof(LOGRECORD));


        if ( LogRecord.dwNextEntry == NAVLOG_EOF )
            {
            gbLogFileCorrupted = TRUE;
            bExitSoonerThanExpected = TRUE;

#ifdef VDEBUG
            wsprintf(szBuf,"\n\rMemRecReadHdr(1): \n\r"
              "LogHeader.dwFirstEntry=%ld\n\r"
              "         .dwLastEntry=%ld\n\r"
              "         .wCurrentEntries=%d\n\r"
              "         .dwMaxSize=%ld\n\r",
              lpLogHeader->dwFirstEntry,
              lpLogHeader->dwLastEntry,
              lpLogHeader->wCurrentEntries,
              lpLogHeader->dwMaxSize);
            FileSeek(hDbgFile, FileLength(hDbgFile), SEEK_SET);
            if (FileWrite(hDbgFile, (LPSTR)szBuf, strlen(szBuf)) != strlen(szBuf))
                {
                MessageBox(NULL,"MemRecReadHdr(1):Debug File Write Error.\n",NULL,MB_OK);
                return TRUE;
                }
            if ((hDbgFile = FileOpen(szDebugFile, READ_WRITE_FILE)) == HFILE_ERROR)
                {
                MessageBox(NULL,"MemRecReadHdr(2):File Open Error.\n",NULL,MB_OK);
                return TRUE;
                }
            STRNCPY(szData1,(LPSTR)(hpMemWalk+sizeof(LOGRECORD)),64);
            szData1[64]='\0';
            STRNCPY(szData2,(LPSTR)(hpMemWalk+sizeof(LOGRECORD)+64),64);
            szData2[64]='\0';
            STRNCPY(szData3,(LPSTR)(hpMemWalk+sizeof(LOGRECORD)+128),64);
            szData3[64]='\0';
            wsprintf(szBuf,"\n\rMemRecReadHdr(2): LogRecord.dwNextEntry == NAVLOG_EOF \n\r"
              "LogRecord.dwPrevEntry=%ld\n\r"
              "         .dwNextEntry=%ld\n\r"
              "         .wType=%d\n\r"
              "         .wRecSize=%d\n\r"
              "  Data=%s\n\r"
              "       %s\n\r"
              "       %s\n\r",
              LogRecord.dwPrevEntry,
              LogRecord.dwNextEntry,
              LogRecord.wType,
              LogRecord.wRecSize,
              (LPSTR)szData1,
              (LPSTR)szData2,
              (LPSTR)szData3);
            FileSeek(hDbgFile, FileLength(hDbgFile), SEEK_SET);
            if (FileWrite(hDbgFile, (LPSTR)szBuf, strlen(szBuf)) != strlen(szBuf))
                 {
                 MessageBox(NULL,"MemRecReadHdr(2):Debug File Write Error.\n",NULL,MB_OK);
                 return TRUE;
                 }
            FileClose(hDbgFile);
#endif

            break;
            }

        dwGoto = LogRecord.dwNextEntry;
        }

    if ( bExitSoonerThanExpected == FALSE )
        {
        hpMemWalk = hpMem + dwGoto;
        MEMCPY(&LogRecord, hpMemWalk, sizeof(LOGRECORD));

        *wType  = LogRecord.wType;
        *dwKey  = LogRecord.dwKey;
        *wSize  = LogRecord.wRecSize;
        }

    return ( bExitSoonerThanExpected );
}

BOOL SYM_EXPORT WINAPI MemRecReadData(      // Read record data
    HPSTR hpMem,
    WORD  wLength,                      // Amount of data to read
    LPSTR lpData,                       // Buffer to fill
    WORD  wRecNo)
{
    auto    BOOL            bExitSoonerThanExpected = FALSE;
    auto    LPLOGHEADER     lpLogHeader;
    auto    LOGRECORD       LogRecord;
    auto    DWORD           dwGoto;
    auto    WORD            i;
    auto    HPSTR           hpMemWalk = hpMem;

    if (lpData != NULL)
        {
        lpLogHeader = (LPLOGHEADER)hpMem;
        dwGoto = lpLogHeader->dwFirstEntry;

        for ( i = 0; i < wRecNo; i++ )
            {
            hpMemWalk = hpMem + dwGoto;
            MEMCPY(&LogRecord, hpMemWalk, sizeof(LOGRECORD));

            dwGoto = LogRecord.dwNextEntry;
            }

        hpMemWalk = (HPSTR)&hpMem[dwGoto] + sizeof(LOGRECORD);

#ifdef SYM_WIN
#ifndef SYM_MIRRORS
        if (IsBadWritePtr(lpData, wLength) ||
            IsBadHugeReadPtr (hpMemWalk,  wLength))
#else
        if (IsBadWritePtr(lpData, uLength))
#endif
            {
            bExitSoonerThanExpected = TRUE;
            gbLogFileCorrupted = TRUE;
            }
        else
#endif
            MEMCPY (lpData, hpMemWalk, wLength);
        }

    return ( bExitSoonerThanExpected );
}


