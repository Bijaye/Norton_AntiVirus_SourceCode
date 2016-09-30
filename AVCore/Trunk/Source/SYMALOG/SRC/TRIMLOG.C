
// Copyright 1993 Symantec, Peter Norton Product Group
//************************************************************************
//
// $Header:   S:\symalog\vcs\trimlog.c_v   1.12   20 Dec 1994 17:12:04   MARKL  $
//
// Description:
//      Functions to trim the activity size.
//
// Contains:
//      SetLogFileSize()
//
// See Also:
//************************************************************************
// $Log:   S:\symalog\vcs\trimlog.c_v  $
// 
//    Rev 1.12   20 Dec 1994 17:12:04   MARKL
// Fixed where SYMALOG was writing UINTs to the disk.  This doesn't work where
// 16 and 32 bit components have to share the same log file and be able to
// read/write each other's data.  These UINTs were changed to type WORD.
// 
//    Rev 1.11   04 May 1994 00:57:48   BRAD
// Don't use NFileOpen()
// 
//    Rev 1.10   15 Mar 1994 12:05:30   BRUCE
// Changed EXPORT to SYM_EXPORT
// 
//    Rev 1.9   28 Feb 1994 16:20:26   BRAD
// Updated for WIN32.
// 
//    Rev 1.8   10 Feb 1994 14:32:06   MFALLEN
// Fixed some minor compiler warnings that had to do with the SYM_NLM defines.
// 
//    Rev 1.7   10 Feb 1994 14:01:02   MFALLEN
// DDREW added some SYM_NLM code in the Quake branch 2. These changes were moved
// to the Quake trunk. All the changes look correct.
// 
//    Rev 1.6.1.2   13 Dec 1993 13:27:20   DDREW
// cleanup
// 
//    Rev 1.6.1.1   06 Dec 1993 14:51:18   DDREW
// Made code work in SYM_NLM land
// 
//    Rev 1.6.1.0   19 Oct 1993 06:24:00   SCOTTP
// Branch base for version QAK2
// 
//    Rev 1.6   12 Oct 1993 16:46:04   PATRICKM
// Baseline from QAK1 branch
// 
//    Rev 1.5.1.1   18 Sep 1993 19:37:04   MFALLEN
// Changes from Rees's lint utility.
// 
//    Rev 1.5.1.0   03 Sep 1993 21:53:42   SCOTTP
// Branch base for version QAK1
// 
//    Rev 1.5   22 Aug 1993 23:53:20   BARRY
// Added MODULE_NAME for VMM debugging
// 
//    Rev 1.4   26 Jul 1993 16:52:50   MFALLEN
// Hopefully this was the last bug.
// 
//    Rev 1.3   16 Jul 1993 09:15:02   MFALLEN
// Added logic to handle maximum file size of zero bytes.
// 
//    Rev 1.2   15 Jul 1993 08:57:58   MFALLEN
// The way the temporary file name was being created was causing problems
// 
//    Rev 1.1   15 Jul 1993 01:20:42   CAREY
// Use call to FileCreateTemporary() instead of Windows only GetTempFilename().
//************************************************************************


#include "platform.h"
#include "xapi.h"
#include "logio.h"
#include "file.h"

#ifdef SYM_NLM
#include <io.h>                         // open, close, etc.
#include "symserve.h"                   // CreateUniqueFileName()
#include "stdio.h"                      // rename()
#include "nwfile.h"                     // remove()
#endif


MODULE_NAME;

// ************************************************************************ //
// ************             Local Function Prototypes           *********** //
// ************************************************************************ //

DWORD LOCAL BuildFileInfo           (HPSTR hpOldInfo, HPSTR hpNewInfo,
                                     DWORD dwSize);
BOOL LOCAL RenameLogFile            (LPSTR lpLogFilePath, LPSTR lpFromFile);
BOOL LOCAL TrimLogFile              (HFILE hLogFile, DWORD dwMaxSize,
                                     LPSTR lpFile);
BOOL LOCAL CreateNewLogFile         (LPSTR lpFile, HPSTR hpNewInfo,
                                     DWORD dwSize);
BOOL LOCAL FixTempFileRecords       (HFILE hTempFile, DWORD dwMaxSize);
BOOL LOCAL UnwindLogFile            (HFILE hLogFile, HFILE hTempFile,
                                     LPLOGHEADER lpLogHeader);
BOOL LOCAL ReadWriteLogHeader       (HFILE hLogFile, HFILE hTempFile,
                                     LPLOGHEADER lpLogHeader);
BOOL LOCAL BuildNoMemFileInfo       (HFILE hLogFile, DWORD dwMaxSize,
                                     LPSTR lpTempFile);

// ************************************************************************ //

// ********************************************************************
// ***      Copyright Symantec Corp. Peter Norton Group 1993        ***
// ********************************************************************
//
// Function Name:
//                  SetLogFileSize()
//
// Description:
//
// This function unwinds and truncates the specified activity log file.
// This function will attempt to perform the required task two different
// ways. First the fast way, by allocating huge chunks of memory and 
// rebuilding to log file in memory. If memory allocation fails a second
// method is used that doesn't allocate huge chunks of memory, but only
// the minimum required to perform the required task.
//
// Parameters:
//      LPSTR   lpLogFile,              Full path to the log file
//      DWORD   dwNewSize,              The size the log file should be
//                                      truncated to.
// Returns:
//      BOOL                            TRUE == Success, FALSE == dudu
// ********************************************************************
// 7/14/93 Martin, Function created.
// 7/15/93 DALLEE, change call to GetTempFilename to FileCreateTemporary()
//          so we can compile and link under DOS.
// ********************************************************************

BOOL SYM_EXPORT WINAPI SetLogFileSize (LPSTR lpLogFile, DWORD dwNewSize)
{
    auto    DWORD       dwFileSize;
    auto    HFILE       hLogFile;
#ifndef SYM_NLM
    auto    HFILE       hTempFile;
#else
    auto    BOOL        bErr;
#endif
    auto    LOGHEADER   LogHeader;
    auto    char        szTempFile [ SYM_MAX_PATH ];
    auto    BOOL        bStatus = FALSE;


    if ((hLogFile = FileOpen (lpLogFile, READ_WRITE_FILE)) != HFILE_ERROR)
        {
                                        // ---------------------------------
                                        // Read in log file header so we can
                                        // access the rest of the log file.
                                        // ---------------------------------
        if (FileRead (hLogFile, &LogHeader, sizeof(LogHeader)) ==
                      sizeof(LogHeader))
            {
            dwFileSize = FileLength (hLogFile);

                                        // ---------------------------------
                                        // If the new size is zero (do not
                                        // limit the size of the log file)
                                        // or if the new size is larger than
                                        // the size specified in the file
                                        // header, just set the new value
                                        // in the header, and write the
                                        // updated header. The rest the
                                        // logio functions will deal with it
                                        // ---------------------------------
            if (dwNewSize == 0 || dwNewSize > dwFileSize)
                {
                LogHeader.dwMaxSize = dwNewSize;
                FileSeek (hLogFile, 0L, SEEK_SET);

                if (FileWrite (hLogFile, &LogHeader, sizeof(LogHeader)) ==
                                sizeof(LogHeader))
                    {
                    bStatus = TRUE;
                    }
                }
            else if ((dwNewSize < dwFileSize))
                {                       // ---------------------------------
                                        // The new size is smaller than the
                                        // current size. This indicates that
                                        // we must trim the log file.
                                        // ---------------------------------

#ifdef SYM_NLM
                bErr = (BOOL)CreateUniqueFileName( szTempFile,
                                                   lpLogFile );
                if( bErr )
                    goto errexit;

#else
                *szTempFile = EOS;
                hTempFile = FileCreateTemporary(szTempFile, FA_NORMAL);
                FileClose(hTempFile);
#endif
 
                bStatus = TrimLogFile (hLogFile, dwNewSize, szTempFile);

                FileClose (hLogFile);
                hLogFile = HFILE_ERROR;

                if (bStatus == TRUE)
                    bStatus = RenameLogFile (lpLogFile, szTempFile);
                }
            }
        }

#ifdef SYM_NLM
errexit:
#endif

    if (hLogFile != HFILE_ERROR)
        FileClose (hLogFile);

    return (bStatus);
}

// ********************************************************************
// ***      Copyright Symantec Corp. Peter Norton Group 1993        ***
// ********************************************************************
//
// Function Name:
//                  TrimLogFile()
//
// Description:
//
// This function will branch the activity log file processing depending
// on if sufficient memory is available.
//
// Parameters:
//      HFILE   hLogFile,               Handle to the opened log file
//      DWORD   dwMaxSize,              The size the log file should be
//                                      truncated to.
//      LPSTR   lpTmpFile,              Temporary filename to use
// Returns:
//      BOOL                            TRUE == Success, FALSE == dudu
// ********************************************************************
// 7/14/93 Martin, Function created.
// ********************************************************************

BOOL LOCAL TrimLogFile (HFILE hLogFile, DWORD dwMaxSize, LPSTR lpTmpFile)
{
    auto    BOOL        bStatus     = FALSE;
    auto    BOOL        bNoMemory   = FALSE;
    auto    DWORD       dwSize      = 0;
    auto    HPSTR       hpOldInfo   = NULL;
    auto    HPSTR       hpNewInfo   = NULL;

    if ((dwSize = FileLength (hLogFile)) != HFILE_ERROR)
        {
        if (((hpOldInfo = MemAllocPtr (GHND, dwSize)) != NULL) &&
            ((hpNewInfo = MemAllocPtr (GHND, dwSize)) != NULL))
            {
            FileSeek (hLogFile, 0L, SEEK_SET);

#ifdef SYM_NLM

            if (FileRead (hLogFile, hpOldInfo, dwSize) == dwSize)

#else

            if (FileHugeRead (hLogFile, hpOldInfo, dwSize) == dwSize)

#endif                
                dwSize = BuildFileInfo (hpOldInfo, hpNewInfo, dwMaxSize);

            if (dwSize)
                bStatus = CreateNewLogFile (lpTmpFile, hpNewInfo, dwSize);
            }
        else
            bNoMemory = TRUE;

        if (hpOldInfo)
            MemFreePtr (hpOldInfo);
        if (hpNewInfo)
            MemFreePtr (hpNewInfo);

        if (bNoMemory == TRUE)
            bStatus = BuildNoMemFileInfo (hLogFile, dwMaxSize, lpTmpFile);
        }

    return (bStatus);
}

// ********************************************************************
// ***      Copyright Symantec Corp. Peter Norton Group 1993        ***
// ********************************************************************
//
// Function Name:
//                  BuildFileInfo()
//
// Description:
//
// This function unwinds the file using the fast memory method. After
// this is done it calculates the number of bytes to remove from the
// file to obtain the user specified size, and finally the new information
// is reorganized in memory so the buffer can just be flushed to disk.
//
// Parameters:
//      HPSTR   hpOldInfo,              huge pointer to where the unprocessed
//                                      information is located.
//      HPSTR   hpNewInfo,              huge pointer to where the processed
//                                      information is placed.
//      DWORD   dwSize,                 The size the log file should be
//                                      truncated to.
// Returns:
//      DWORD   Size of the processed information. 0 == error happened
// ********************************************************************
// 7/14/93 Martin, Function created.
// ********************************************************************

DWORD LOCAL BuildFileInfo (HPSTR hpOldInfo, HPSTR hpNewInfo, DWORD dwSize)
{
    auto    LPLOGHEADER     lpFileHeader    = NULL;
    auto    LOGRECORD HUGE *hpOldRecHeader  = NULL;
    auto    LOGRECORD HUGE *hpNewRecHeader  = NULL;
    auto    DWORD           dwCurrPos       = 0;
    auto    DWORD           dwNextEntry     = 0;
    auto    DWORD           dwPrevEntry     = 0;
    auto    DWORD           dwBytesToChop   = 0;
    auto    WORD            wThisRecordSize = 0;
    auto    BOOL            bDone           = FALSE;

    MEMCPY (hpNewInfo, hpOldInfo, sizeof(LOGHEADER));
    lpFileHeader = (LPLOGHEADER)hpNewInfo;
    dwCurrPos = sizeof(LOGHEADER);
                                        // -----------------------------------
                                        // Since the log file is a circular
                                        // file, first we lay it out flat
                                        // and then compute how many record
                                        // shoult be choped off from the
                                        // beginning. This first loop lays the
                                        // file out flat in memory.
                                        // -----------------------------------

    hpNewRecHeader = (LOGRECORD HUGE *)(hpNewInfo + dwCurrPos);
    hpOldRecHeader = (LOGRECORD HUGE *)(hpOldInfo + lpFileHeader->dwFirstEntry);

    lpFileHeader->dwFirstEntry = dwCurrPos;
    lpFileHeader->dwMaxSize    = dwSize;

    while (!bDone)
        {
        wThisRecordSize = (sizeof(LOGRECORD) + hpOldRecHeader->wRecSize);
        MEMCPY (hpNewRecHeader, hpOldRecHeader, wThisRecordSize);

        hpNewRecHeader->dwNextEntry = (DWORD)(dwCurrPos + wThisRecordSize);
        hpNewRecHeader->dwPrevEntry = dwPrevEntry;

        dwPrevEntry = dwCurrPos;
        dwCurrPos  += wThisRecordSize;

        hpNewRecHeader = (LOGRECORD HUGE *)(hpNewInfo + dwCurrPos);
        hpOldRecHeader = (LOGRECORD HUGE *)(hpOldInfo + hpOldRecHeader->dwNextEntry);

        if (hpOldRecHeader->dwNextEntry == NAVLOG_EOF)
            {
            wThisRecordSize = (sizeof(LOGRECORD) + hpOldRecHeader->wRecSize);
            MEMCPY (hpNewRecHeader, hpOldRecHeader, wThisRecordSize);

            hpNewRecHeader->dwPrevEntry = dwPrevEntry;
            dwCurrPos  += wThisRecordSize;

            bDone = TRUE;
            }
        }
                                        // -----------------------------------
                                        // The log file is layed out flat so
                                        // we compute if any records should
                                        // be choped off from the beginning.
                                        // -----------------------------------
    if (dwCurrPos > dwSize)
        {
        dwBytesToChop = dwCurrPos - dwSize;

        dwCurrPos = lpFileHeader->dwFirstEntry;
        hpNewRecHeader = (LOGRECORD HUGE *)(hpNewInfo + dwCurrPos);
                                        // ---------------------------------
                                        // Now we compute the offset of the
                                        // first record that should now be
                                        // choped off
                                        // ---------------------------------
        while (dwBytesToChop)
            {
            dwCurrPos = hpNewRecHeader->wRecSize + sizeof(LOGRECORD);

            if (dwBytesToChop > dwCurrPos)
                dwBytesToChop -= dwCurrPos;
            else
                dwBytesToChop = 0;

            if (hpNewRecHeader->dwNextEntry == NAVLOG_EOF)
                dwBytesToChop = 0;
            else 
                hpNewRecHeader = (LOGRECORD HUGE *)(hpNewInfo + 
                                  hpNewRecHeader->dwNextEntry);
            }
                                        // ----------------------------------
                                        // By now hpNewRecHeader points to
                                        // the first valid activity log that
                                        // will be written to the log file
                                        // ----------------------------------
        bDone           = FALSE;
        dwCurrPos       = sizeof(LOGHEADER);
        dwPrevEntry     = 0;
        hpOldRecHeader  = hpNewRecHeader;
        hpNewRecHeader  = (LOGRECORD HUGE *)(hpNewInfo + dwCurrPos);
        lpFileHeader->wCurrentEntries = 0;

        while (!bDone)
            {
            dwNextEntry = hpOldRecHeader->dwNextEntry;
            wThisRecordSize = (sizeof(LOGRECORD) + hpOldRecHeader->wRecSize);
            MEMCPY (hpNewRecHeader, hpOldRecHeader, wThisRecordSize);

            hpNewRecHeader->dwNextEntry = (DWORD)(dwCurrPos + wThisRecordSize);
            hpNewRecHeader->dwPrevEntry = dwPrevEntry;

            dwPrevEntry = dwCurrPos;
            dwCurrPos  += (wThisRecordSize);

            hpNewRecHeader = (LOGRECORD HUGE *)(hpNewInfo + dwCurrPos);
            hpOldRecHeader = (LOGRECORD HUGE *)(hpNewInfo + dwNextEntry);

            if (hpOldRecHeader->dwNextEntry == NAVLOG_EOF)
                {
                dwNextEntry = hpOldRecHeader->dwNextEntry;

                wThisRecordSize = (sizeof(LOGRECORD) + hpOldRecHeader->wRecSize);
                MEMCPY (hpNewRecHeader, hpOldRecHeader, wThisRecordSize);

                hpNewRecHeader->dwPrevEntry = dwPrevEntry;

                lpFileHeader->wCurrentEntries ++ ;
                lpFileHeader->dwLastEntry = dwCurrPos;
                dwCurrPos += (wThisRecordSize + 1);

                bDone = TRUE;
                }
        
            lpFileHeader->wCurrentEntries ++ ;
            }
        }
    else


#ifdef SYM_NLM
        
        MEMCPY(hpNewInfo, hpOldInfo, dwCurrPos);

#else
        
        MemoryHugeCopy (hpNewInfo, hpOldInfo, dwCurrPos);

#endif


    return (dwCurrPos);
}

// ********************************************************************
// ***      Copyright Symantec Corp. Peter Norton Group 1993        ***
// ********************************************************************
//
// Function Name:
//                  CreateNewLogFile()
//
// Description:
//
// This function creates and flushes the provided huge pointer to the provided
// file name.
//
// Parameters:
//      LPSTR   lpFile,                 Name of the file to create
//      HPSTR   hpNewInfo,              huge pointer to where the processed
//                                      information is.
//      DWORD   dwSize,                 The size the information to write.
// Returns:
//      BOOL                            TRUE == Success, FALSE == dudu
// ********************************************************************
// 7/14/93 Martin, Function created.
// ********************************************************************

BOOL LOCAL CreateNewLogFile (LPSTR lpFile, HPSTR hpNewInfo, DWORD dwSize)
{
    auto    BOOL    bStatus = FALSE;
    auto    HFILE   hFile;

    if ((hFile = FileCreate (lpFile, FA_NORMAL)) != HFILE_ERROR)
        {

#ifdef SYM_NLM        
        if( FileWrite(hFile, hpNewInfo, dwSize) == dwSize )
            bStatus = TRUE;

        FileClose (hFile);
        
        if (bStatus == FALSE)
            remove(lpFile);

#else
        if (FileHugeWrite (hFile, hpNewInfo, dwSize) == dwSize)
            bStatus = TRUE;
        
        FileClose (hFile);

        if (bStatus == FALSE)
            FileDelete (lpFile);
#endif

        }

    return (bStatus);
}


// ********************************************************************
// ***      Copyright Symantec Corp. Peter Norton Group 1993        ***
// ********************************************************************
//
// Function Name:
//                  RenameLogFile()
//
// Description:
//
// This function copies/create the log file from source to destination
//
// Parameters:
//      LPSTR   lpLogFilePath,          File to create
//      LPSTR   lpFromFile,             from this file.
// Returns:
//      BOOL                            TRUE == Success, FALSE == dudu
// ********************************************************************
// 7/14/93 Martin, Function created.
// ********************************************************************

BOOL LOCAL RenameLogFile (LPSTR lpLogFilePath, LPSTR lpFromFile)
    {
    auto    int     nReturn;


#ifdef SYM_NLM

    // SYM_NLM rename function
    remove( lpLogFilePath );            // kill original file so can use name
    nReturn = rename( lpFromFile, lpLogFilePath ); 

#else

    // for WinDos copy is neccessary because TMP dir is used to hold new file
    nReturn = FileCopy (lpFromFile, lpLogFilePath, NULL);
    FileDelete (lpFromFile);

#endif


    return ((nReturn == NOERR) ? TRUE : FALSE);
    }


// ********************************************************************
// ***      Copyright Symantec Corp. Peter Norton Group 1993        ***
// ********************************************************************
//
// Function Name:
//                  BuildNoMemFileInfo()
//
// Description:
//
// This function unwinds/compacts/slices and dices the log file when
// there isn't much memory to work with.
//
// Parameters:
//      HFILE   hLogFile,               log file handle to slice'n dice
//      DWORD   dwMaxSize,              size to truncate file to
//      LPSTR   lpTempFile,             name of the temprary file to use
// Returns:
//      BOOL                            TRUE == Success, FALSE == dudu
// ********************************************************************
// 7/14/93 Martin, Function created.
// ********************************************************************

BOOL LOCAL BuildNoMemFileInfo (HFILE hLogFile, DWORD dwMaxSize,
                               LPSTR lpTempFile)
{
    auto    LOGHEADER   LogHeader;
    auto    BOOL        bStatus     =   FALSE;
    auto    HFILE       hTempFile   =   HFILE_ERROR;

    FileSeek (hLogFile, 0L, SEEK_SET);

    if ((hTempFile = FileCreate(lpTempFile, FA_NORMAL)) != HFILE_ERROR)
        {
        if (ReadWriteLogHeader (hLogFile, hTempFile, &LogHeader) == TRUE)
            {
            if (UnwindLogFile (hLogFile, hTempFile, &LogHeader) == TRUE)
                bStatus = FixTempFileRecords (hTempFile, dwMaxSize);
            }

        FileClose (hTempFile);
        }

    return (bStatus);
}

// ********************************************************************
// ***      Copyright Symantec Corp. Peter Norton Group 1993        ***
// ********************************************************************
//
// Function Name:
//                  ReadWriteLogHeader()
//
// Description:
//
// This function read the log file header and writes it to the temp file
//
// Parameters:
//      HFILE   hLogFile,               log file handle to read from
//      HFILE   hTempFile,              temp file handle to write to
//      LPLOGHEADER lpLogHeader,        use this buffer so we return
//                                      the info to the caller
// Returns:
//      BOOL                            TRUE == Success, FALSE == dudu
// ********************************************************************
// 7/14/93 Martin, Function created.
// ********************************************************************

BOOL LOCAL ReadWriteLogHeader (HFILE hLogFile, HFILE hTempFile,
                               LPLOGHEADER lpLogHeader)
{
    auto    BOOL    bStatus = FALSE;

    if (FileRead (hLogFile, lpLogHeader, sizeof(LOGHEADER))
                                                        == sizeof(LOGHEADER))
        {
        if (FileWrite (hTempFile, lpLogHeader, sizeof(LOGHEADER))
                                                        == sizeof(LOGHEADER))
            bStatus = TRUE;
        }
    return (bStatus);
}

// ********************************************************************
// ***      Copyright Symantec Corp. Peter Norton Group 1993        ***
// ********************************************************************
//
// Function Name:
//                  UnwindLogFile()
//
// Description:
//
// This function unwinds the log file using the memory starved way.
//
// Parameters:
//      HFILE   hLogFile,               log file handle to read from
//      HFILE   hTempFile,              temp file handle to write to
//      LPLOGHEADER lpLogHeader,        use this buffer so we return
//                                      the info to the caller
// Returns:
//      BOOL                            TRUE == Success, FALSE == dudu
// ********************************************************************
// 7/14/93 Martin, Function created.
// ********************************************************************

BOOL LOCAL UnwindLogFile (HFILE hLogFile, HFILE hTempFile,
                          LPLOGHEADER lpLogHeader)
{
    auto    BOOL        bStatus = TRUE;
    auto    BOOL        bDone   = FALSE;
    auto    LOGRECORD   LogRecord;
    auto    DWORD       dwPrevEntry = 0;
    auto    DWORD       dwNextLogEntry;
    auto    LPSTR       lpMem;
    auto    WORD        wTotalSize;

    dwNextLogEntry = lpLogHeader->dwFirstEntry;

    do
        {
        FileSeek (hLogFile, dwNextLogEntry, SEEK_SET);

        if (FileRead (hLogFile, &LogRecord, sizeof(LogRecord))
                                                            == sizeof(LogRecord))
            {
            if (LogRecord.dwNextEntry != NAVLOG_EOF)
                {
                dwNextLogEntry = LogRecord.dwNextEntry;

                LogRecord.dwNextEntry = FileSeek (hTempFile, 0L, SEEK_CUR);
                LogRecord.dwNextEntry += LogRecord.wRecSize;
                }
            else
                bDone = TRUE;

            LogRecord.dwPrevEntry = dwPrevEntry;
            dwPrevEntry = FileSeek (hTempFile, 0L, SEEK_CUR);

            wTotalSize = LogRecord.wRecSize + sizeof(LogRecord);

            if ((lpMem = MemAllocPtr (GHND, wTotalSize)) != NULL)
                {
                MEMCPY (lpMem, &LogRecord, sizeof(LogRecord));

                if (FileRead (hLogFile, &lpMem[sizeof(LogRecord)],
                              LogRecord.wRecSize) != LogRecord.wRecSize)
                    {
                    bStatus = FALSE;
                    }
                else
                    {
                    if (FileWrite (hTempFile, lpMem, wTotalSize) != wTotalSize)
                        bStatus = FALSE;
                    }

                MemFreePtr (lpMem);
                }
            else
                bStatus = FALSE;
            }
        else
            bStatus = FALSE;
        }
    while (bStatus == TRUE && bDone == FALSE);

    return (bStatus);
}


// ********************************************************************
// ***      Copyright Symantec Corp. Peter Norton Group 1993        ***
// ********************************************************************
//
// Function Name:
//                  FixTempFileRecords()
//
// Description:
//
// This function compacts the temp file that is already filled with info
//
// Parameters:
//      HFILE   hTempFile,              temp file handle to compact
//      DWORD   dwMaxSize,              Size to compact file to
// Returns:
//      BOOL                            TRUE == Success, FALSE == dudu
// ********************************************************************
// 7/14/93 Martin, Function created.
// ********************************************************************

BOOL LOCAL FixTempFileRecords (HFILE hTempFile, DWORD dwMaxSize)
{
    auto    LOGHEADER   LogHeader;
    auto    LOGRECORD   LogRecord;
    auto    DWORD       dwBytesToChop;
    auto    DWORD       dwPrevEntry;
    auto    DWORD       dwWriteToOffset;
    auto    DWORD       dwReadFromOffset;
    auto    WORD        wTotalSize;
    auto    BOOL        bStatus = TRUE;
    auto    BOOL        bDone   = FALSE;
    auto    LPSTR       lpMem;

    FileSeek (hTempFile, 0L, SEEK_SET);

    dwBytesToChop = FileLength (hTempFile);

    if (dwBytesToChop > dwMaxSize)
        {
        FileRead (hTempFile, &LogHeader, sizeof(LogHeader));
        dwBytesToChop -= dwMaxSize;
        }
    else
        dwBytesToChop = 0;

    while (dwBytesToChop)
        {
        if (FileRead(hTempFile, &LogRecord, sizeof(LogRecord)) == sizeof(LogRecord))
            {
            wTotalSize = LogRecord.wRecSize + sizeof(LogRecord);

            if (dwBytesToChop > wTotalSize)
                dwBytesToChop -= wTotalSize;
            else
                dwBytesToChop = 0;

            FileSeek (hTempFile, LogRecord.wRecSize, SEEK_CUR);
            }
        else
            bStatus = FALSE;
        }

    if ( bStatus == TRUE )
        {
        dwPrevEntry      = 0;
        dwWriteToOffset  = sizeof(LogHeader);
        dwReadFromOffset = FileSeek (hTempFile, 0L, SEEK_CUR);

        LogHeader.dwFirstEntry = sizeof(LogHeader);
        LogHeader.dwMaxSize    = dwMaxSize;
        LogHeader.wCurrentEntries = 0;

        do
            {
            FileSeek (hTempFile, dwReadFromOffset, SEEK_SET);
            wTotalSize = FileRead (hTempFile, &LogRecord, sizeof(LogRecord));

            if (wTotalSize == sizeof(LogRecord))
                {
                wTotalSize += LogRecord.wRecSize;

                if ((lpMem = MemAllocPtr (GHND, wTotalSize)) != NULL)
                    {
                    if (LogRecord.dwNextEntry != NAVLOG_EOF)
                        LogRecord.dwNextEntry = dwWriteToOffset + wTotalSize;
                    else
                        {
                        LogHeader.dwLastEntry = dwWriteToOffset;
                        bDone = TRUE;
                        }

                    LogRecord.dwPrevEntry = dwPrevEntry;
                    dwPrevEntry = dwWriteToOffset;

                    MEMCPY (lpMem, &LogRecord, sizeof(LogRecord));

                    if (FileRead (hTempFile, &lpMem[sizeof(LogRecord)],
                                  LogRecord.wRecSize) == LogRecord.wRecSize)
                        {
                        dwReadFromOffset = FileSeek (hTempFile, 0L, SEEK_CUR);
                        FileSeek (hTempFile, dwWriteToOffset, SEEK_SET);

                        if (FileWrite (hTempFile, lpMem, wTotalSize) == wTotalSize)
                            {
                            LogHeader.wCurrentEntries ++ ;
                            dwWriteToOffset += wTotalSize;
                            }
                        else
                            bStatus = FALSE;
                        }
                    else
                        bStatus = FALSE;

                    MemFreePtr (lpMem);
                    }
                else
                    bStatus = FALSE;
                }
            else
                bStatus = FALSE;
            }
        while ( bStatus == TRUE && bDone == FALSE );

        if (bStatus == TRUE)
            {
                                        // ---------------------
                                        // Truncate the file
                                        // ---------------------
            FileSeek (hTempFile, dwWriteToOffset, SEEK_SET);
            FileWrite (hTempFile, "", 0);

                                        // ------------------------------
                                        // Write the updated file header
                                        // ------------------------------
            FileSeek (hTempFile, 0L, SEEK_SET);
            FileWrite(hTempFile, &LogHeader, sizeof(LogHeader));
            }
        }

    return (bStatus);
}

