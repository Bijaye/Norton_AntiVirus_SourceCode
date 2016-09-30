// Copyright 1995 Symantec Corporation                                  
//***************************************************************************
// 
// $Header:   S:\certlibn\vcs\fstatc.cpv   1.5   29 Apr 1995 17:08:10   DALLEE  $ 
// 
// Description: c version of old certlib asm file status function
//              returns selected items from the file stat function
// 
// Returns: TRUE if an error occurred.
//
// Contains:                                                            
// 
// See Also:                                                            
// 
//***************************************************************************
// $Log:   S:\certlibn\vcs\fstatc.cpv  $
// 
//    Rev 1.5   29 Apr 1995 17:08:10   DALLEE
// Added LOADDS for 16-bit "C" functions since CERTLIBN is naughty with DS.
//
//    Rev 1.4   25 Jan 1995 13:28:34   JMILLARD
// 
// add standard file header
// merge NLM fix from quake 6
// 
//    Rev 1.3.1.1   25 Jan 1995 13:26:32   JMILLARD
// fix bug with return value for NLM
// use single function declaration, change bRet to bErr
//***************************************************************************


#include "platform.h"
#include "xapi.h"
#include "file.h"
#include "ctsn.h"

#if !defined(SYM_TSR)

BOOL WINAPI CTSN_LOADDS FIL4_STAT(LPSTR lpszFile, LPSTR lpbyStorage, LPSTR lpBuffer)

#ifdef SYM_NLM

    {
    struct  stat    pStat;

    int     nRet = 0;
    BOOL    bErr = TRUE;


    (void) lpBuffer;
    
    // get file / dir statistics from NetWare

    nRet = stat( lpszFile, &pStat );
    
    if (nRet == INVALID_HANDLE_VALUE)
        {
        goto BailOut;
        }

    bErr = FALSE;               // clear error return

//  Attr(DB), Time(DW), Date(DW), Size(DD)

    *lpbyStorage              = (BYTE)pStat.st_attr;  // file attributes                          */
    *(DWORD *)(&lpbyStorage[ 1 ]) = (DWORD)pStat.st_mtime;// last modify date/time
    *(DWORD *)(&lpbyStorage[ 5 ]) = (DWORD)pStat.st_size; // file size

BailOut:

    return (bErr);
    }

#else

{
    BOOL        bErr = TRUE;
    HFIND       hFind;
    FINDDATA    findData;
    WORD        wDate, wTime;


    hFind = FileFindFirst(lpszFile, &findData);
    if (hFind != INVALID_HANDLE_VALUE)
        {
        FileFindClose(hFind);
        bErr = FALSE;

        FileTimeToDosTime(&findData.ftLastWriteTime, &wDate, &wTime);
        
        // Fill in buffer the way its supposed to be filled in.
        *lpbyStorage++ = (BYTE) findData.dwFileAttributes;
        *((LPWORD) lpbyStorage) = wTime;
        lpbyStorage += sizeof(WORD);
        *((LPWORD) lpbyStorage) = wDate;
        lpbyStorage += sizeof(WORD);
        *((LPDWORD) lpbyStorage) = findData.dwFileSizeLow;
        }
    return(bErr);
}

#endif

#endif

