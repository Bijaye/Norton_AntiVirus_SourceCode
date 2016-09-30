// Copyright 1996 Symantec, Peter Norton Product Group
//************************************************************************
//
// $Header:   S:/AVENGE/VCS/ENGCOPYF.CPv   1.0   18 Apr 1997 13:38:00   MKEATIN  $
//
// Description:
//  File copy function, using new callbacks.
//
// Contains:
//  EngCopyFile()
//
// See Also:
//
//************************************************************************
// $Log:   S:/AVENGE/VCS/ENGCOPYF.CPv  $
// 
//    Rev 1.0   18 Apr 1997 13:38:00   MKEATIN
// Initial revision.
// 
//    Rev 1.3   04 Jun 1996 17:41:06   DCHI
// Changes to conform with new header files in global include.
// 
//    Rev 1.2   31 May 1996 16:11:34   DCHI
// Switched date/time parameters in DateTime file callbacks.
// 
//    Rev 1.1   29 May 1996 18:08:00   DCHI
// Changed lpvFileInfo to lpvInfo for file objects.
// 
//    Rev 1.0   20 May 1996 14:41:20   DCHI
// Initial revision.
// 
//************************************************************************

#include "avengel.h"


//********************************************************************
//
// Function:
//  EngCopyFile()
//
// Parameters:
//  lpSourceObject          Object to the source file
//  lpDestObject            Object to the dest file
//  lpbyWorkBuffer          Buffer to copy bytes in
//  wBufferSize             Size of the buffer
//
//
// Description:
//  This function opens the source file and creates (or opens for write
//  only and truncates) the destination file.  It then copies the entire
//  contents of the source file to the destination file.  The date
//  and time of the destination are set to be the same as the date
//  and time of the source.
//
// Returns:
//  ENGSTATUS_OK            On success
//  ENGSTATUS_ERROR         On failure
//
//********************************************************************

ENGSTATUS   EngCopyFile
(
    LPFILEOBJECT        lpSourceObject,
    LPFILEOBJECT        lpDestObject,
    LPBYTE              lpbyWorkBuffer,
    WORD                wBufferSize
)
{
    DWORD       dwSourceFileSize, dwRead, dwLeft, dwActualRead, dwWrite,
                dwActualWrite;
    CBSTATUS    cb1, cb2;
    WORD        wDate, wTime;

    // first open the source file

    if (lpSourceObject->lpCallBacks->
            FileOpen(lpSourceObject->lpvInfo,
                     FILE_OPEN_READ_ONLY) != CBSTATUS_OK)
        return(ENGSTATUS_ERROR);

    // Get its date and time

    if (lpSourceObject->lpCallBacks->
            FileGetDateTime(lpSourceObject->lpvInfo,
                            0,
                            &wDate,
                            &wTime) == CBSTATUS_ERROR)
    {
        // error: close the source file

        lpSourceObject->lpCallBacks->FileClose(lpSourceObject->lpvInfo);

        return(ENGSTATUS_ERROR);
    }

    // then open the dest file

    if (lpDestObject->lpCallBacks->
            FileOpen(lpDestObject->lpvInfo,
                     FILE_OPEN_WRITE_ONLY) != CBSTATUS_OK)
    {
        // error: close the source file

        lpSourceObject->lpCallBacks->FileClose(lpSourceObject->lpvInfo);

        return(ENGSTATUS_ERROR);
    }

    // find out how many bytes we want to copy...

    if (lpSourceObject->lpCallBacks->
            FileLength(lpSourceObject->lpvInfo,
                       &dwSourceFileSize) != CBSTATUS_OK)
    {
        // error: close both files

        lpSourceObject->lpCallBacks->FileClose(lpSourceObject->lpvInfo);
        lpDestObject->lpCallBacks->FileClose(lpDestObject->lpvInfo);

        // delete our dest file

        lpDestObject->lpCallBacks->FileDelete(lpDestObject->lpvInfo);

        return(ENGSTATUS_ERROR);
    }

    // reset how many bytes we have to go

    dwLeft = dwSourceFileSize;

    while (dwLeft > 0)
    {
        // determine how many bytes to read

        if (dwLeft > wBufferSize)
            dwRead = wBufferSize;
        else
            dwRead = dwLeft;

        // read those bytes

        if (lpSourceObject->lpCallBacks->
                FileRead(lpSourceObject->lpvInfo,
                         lpbyWorkBuffer,
                         dwRead,
                         &dwActualRead) != CBSTATUS_OK ||
            dwRead != dwActualRead)
        {
            // error: close both files

            lpSourceObject->lpCallBacks->FileClose(lpSourceObject->lpvInfo);
            lpDestObject->lpCallBacks->FileClose(lpDestObject->lpvInfo);

            // delete our new file

            lpDestObject->lpCallBacks->FileDelete(lpDestObject->lpvInfo);

            // remove those virus signature traces

            MEMSET(lpbyWorkBuffer,0,wBufferSize);

            return(ENGSTATUS_ERROR);
        }

        // write those bytes

        dwWrite = dwRead;

        if (lpDestObject->lpCallBacks->
                FileWrite(lpDestObject->lpvInfo,
                          lpbyWorkBuffer,
                          dwWrite,
                          &dwActualWrite) != CBSTATUS_OK ||
            dwWrite != dwActualWrite)
        {
            // error: close both files

            lpSourceObject->lpCallBacks->FileClose(lpSourceObject->lpvInfo);
            lpDestObject->lpCallBacks->FileClose(lpDestObject->lpvInfo);

            // delete our new file

            lpDestObject->lpCallBacks->FileDelete(lpDestObject->lpvInfo);

            // remove those virus signature traces

            MEMSET(lpbyWorkBuffer,0,wBufferSize);

            return(ENGSTATUS_ERROR);
        }
        
        dwLeft -= dwRead;
    }

    cb1 = lpSourceObject->lpCallBacks->FileClose(lpSourceObject->lpvInfo);

    // Set the destination's date and time the same as the source

    if (lpDestObject->lpCallBacks->
            FileSetDateTime(lpDestObject->lpvInfo,
                            0,
                            wDate,
                            wTime) == CBSTATUS_ERROR)
    {
        // error: close the destination file

        lpDestObject->lpCallBacks->FileClose(lpDestObject->lpvInfo);

        // delete the destination file

        lpDestObject->lpCallBacks->FileDelete(lpDestObject->lpvInfo);

        return(ENGSTATUS_ERROR);
    }

    cb2 = lpDestObject->lpCallBacks->FileClose(lpDestObject->lpvInfo);

    if (cb1 != CBSTATUS_OK || cb2 != CBSTATUS_OK)
    {
        lpDestObject->lpCallBacks->FileDelete(lpDestObject->lpvInfo);

        // remove those virus signature traces

        MEMSET(lpbyWorkBuffer,0,wBufferSize);

        return(ENGSTATUS_ERROR);
    }

    // remove those virus signature traces

    MEMSET(lpbyWorkBuffer,0,wBufferSize);

    return(ENGSTATUS_OK);
}

