// unixfile.cpp
// Copyright 1999 by Symantec Corporation.  All rights reserved.
// A Unix implementation of the File functions in the Symantec 
// core library (SYMKRNL).


#ifdef SYM_UNIX  // whole file


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>

#include "platform.h"
#include "file.h"


// FileOpen()
// uOpenMode is either READ_ONLY_FILE (0), WRITE_ONLY_FILE (1), 
// or READ_WRITE_FILE (2). 
// These flags are the same as the old Windows OF_xxx (OpenFile) flags.
// Note: NAV uses the alternate names FILE_OPEN_READ_ONLY (0), 
// FILE_OPEN_WRITE_ONLY (1), or FILE_OPEN_READ_WRITE (2).
// WARNING: OF_SHARE_xxx and OF_CREATE flags are not supported in Unix.
// (This could be added using fdopen().)
// Returns file handle, HFILE_ERROR on error.

HFILE FileOpen(LPCTSTR lpName, UINT uOpenMode)
{
    char szMode[10];
    HFILE hFile;

    if ((uOpenMode & 0x3) == OF_READ)
        strcpy(szMode, "rb");
    if ((uOpenMode & 0x3) == OF_WRITE)
        strcpy(szMode, "wb");
    if ((uOpenMode & 0x3) == OF_READWRITE)
        strcpy(szMode, "rb+");

    hFile = fopen((const char *)lpName, szMode);
    if (hFile == NULL)
        return (HFILE_ERROR);

    return (hFile);
}


// FileClose()
// Returns NOERR, or HFILE_ERROR on error.

HFILE FileClose(HFILE hFile)
{
    if (fclose(hFile) != 0)  // error
        return (HFILE_ERROR);

    return (NOERR);
}


// FileCreate()
// uAttr is one of 0 (normal), 1 (read-only), 2 (hidden), or 3 (system).
// Note that this is _not_ a bitwise OR of DOS attribute flags (almost, though).
// WARNING: uAttr values have no effect in Unix.
// Returns file handle, or HFILE_ERROR on error.

HFILE FileCreate(LPCTSTR lpFileName, UINT uAttr)
{
    HFILE hFile;

    hFile = fopen((const char *)lpFileName, "wb");
    if (hFile == NULL)  // error
        return (HFILE_ERROR);

    return (hFile);
}


// FileCreateTemporary()
// lpFileName receives the name of the new file.
// uAttr is a bitwise OR of DOS attribute flags:
// 0 (normal), 1 (read-only), 2 (hidden), and 4 (system).
// WARNING: uAttr values have no effect in Unix.
// Returns file handle, or HFILE_ERROR on error.

HFILE FileCreateTemporary(LPTSTR lpFileName, UINT uAttr)
{
    char* pszName;
    HFILE hFile;

    pszName = tempnam(NULL, "sym");
    if (pszName == NULL)
        return (HFILE_ERROR);

    hFile = fopen(pszName, "wb");

    strcpy(lpFileName, pszName);
    free(pszName);

    if (hFile == NULL)  // error
        return (HFILE_ERROR);

    return (hFile);
}


// FileRead()
// Returns number of bytes actually read, or -1 on error.

UINT FileRead(HFILE hFile, LPVOID lpBuffer, UINT uBytes)
{
    size_t nToRead = uBytes;
    size_t nRead;

    // If reading 0 bytes, don't return an error.
    if (nToRead == 0)
        return (0);

    nRead = fread(lpBuffer, 1, nToRead, hFile);
    if (nRead == 0)  // error
        return ((UINT)-1);

    return ((UINT)nRead);
}


// FileWrite()
// Returns number of bytes actually written, or -1 on error.
         
UINT FileWrite(HFILE hFile, LPVOID lpBuffer, UINT uBytes)
{
    size_t nToWrite = uBytes;
    size_t nWritten;
    int nFile;

    // If writing 0 bytes, truncate the file.
    if (nToWrite == 0)
	{
        nFile = fileno(hFile);

        if (ftruncate(nFile, ftell(hFile)) != 0)  // error
            return((UINT)-1);

        return (0);
	}

    nWritten = fwrite(lpBuffer, 1, nToWrite, hFile);

#if 0
    // Discrepency with Core specification
    if (nToWrite != nWritten)
        return ((UINT)-1);
#endif

    return ((UINT)nWritten);
}


// FileLength()
// Returns length of file, or -1 on error.

DWORD FileLength(HFILE hFile)
{
    long lOrigOffset, lNewOffset;

    lOrigOffset = ftell(hFile);

    // Seek to end of file.
    if (fseek(hFile, 0, SEEK_END) == -1)  // error
        return ((DWORD)-1);

    // Postion at end equals file size.
    lNewOffset = ftell(hFile);

    // Go back to where we were.
    if (fseek(hFile, lOrigOffset, SEEK_SET) == -1)  // error
        return ((DWORD)-1);

    return (lNewOffset);
}


// FileSeek()
// Returns new file position, or -1 on error.

DWORD FileSeek(HFILE hFile, LONG dwOffset, int nFlag)
{
    if (fseek(hFile, dwOffset, nFlag) == -1)  // error
        return ((DWORD)-1);

    return ((DWORD)ftell(hFile));
}


// FileGetAttr()
// *uAttr is a bitwise OR of DOS attribute flags:
// 0 (normal), 1 (read-only), 2 (hidden), and 4 (system).
// WARNING: On Unix, this function returns an attribute of
// 1 if the program does not have write permission, and 0 otherwise.
// Returns NOERR on success, ERR on error.

UINT FileGetAttr(LPCTSTR lpFileName, UINT* uAttr)
{
    // Assume write permission is granted
    *uAttr = 0;

    // If there is no write permission...
    if (access(lpFileName, W_OK) != 0)  // error or no permission
    {
        // Check for a "real" error
        if (errno != EACCES)
            return (ERR);

        // Set the read-only bit
        *uAttr = 1;
    }

    return (NOERR);
}


// FileSetAttr()
// uAttr is a bitwise OR of DOS attribute flags:
// 0 (normal), 1 (read-only), 2 (hidden), and 4 (system).
// WARNING: On Unix, this function simulates setting normal
// and read-only attubutes by attempting to set write permissions
// for the user, group, and others.  Otherwise it does nothing.
// Returns NOERR on success, ERR on error.

UINT FileSetAttr(LPCTSTR lpFileName, UINT uAttr)
{
    struct stat s;
    int nMask = S_IWUSR | S_IWGRP | S_IWOTH;

    // Get current file mode
    if (stat(lpFileName, &s) == -1)  // error
        return (ERR);

    // If requesting normal attribute (not read-only)...
    if ((uAttr & 1) == 0)
    {
        // Add write permissions for user, group, other
        if (chmod(lpFileName, s.st_mode | nMask) == -1)  // error
            return (ERR);
    }
    // Else if requesting read-only attribute...
    else if ((uAttr & 1) == 1)
    {
        // Remove write permissions for user, group, other
        if (chmod(lpFileName, s.st_mode & ~nMask) == -1)  // error
            return (ERR);
    }

    return (NOERR);
}


// FileGetTime()
// uType is TIME_LASTWRITE (0), TIME_LASTACCESS (4), or TIME_CREATION (6).
// WARNING: NAV callback functions expect the date/time as two WORDs 
// and return a BOOL.
// WARNING: This function always fails in Unix.
// Returns ???

UINT FileGetTime(HFILE hFile, UINT uType, LPFILETIME lpFileTime)
{
    // TODO: Code required
    return (ERR);
}


// FileSetTime()
// uType is TIME_LASTWRITE (0), TIME_LASTACCESS (4), or TIME_CREATION (6).
// WARNING: NAV callback functions expect the date/time as two WORDs
// and return a BOOL.
// WARNING: This function always fails in Unix.
// Returns ???

UINT FileSetTime(HFILE hFile, UINT uType, LPFILETIME lpFileTime)
{
    // TODO: Code required
    return (ERR);
}


// FileExists()
// Returns TRUE if file exists, FALSE otherwise.

BOOL FileExists(LPCTSTR lpFileName)
{
    // Check for "file existence" access.
    if (access(lpFileName, F_OK) != 0)  // error or no "existence" access
        return (FALSE);
    return (TRUE);
}


// FileDelete()
// Returns NOERR on success, or ERR on error.
// WARNING: NAV callback functions are expected to return 1 for success, 
// or 0 on error.

UINT FileDelete(LPCTSTR lpFileName)
{
    if (remove(lpFileName) != 0)  // error
        return (ERR);

    return (NOERR);
}


// FileKill()
// Like FileDelete() but attempts to delete even read-only files.
// Returns NOERR on success, or ERR on error.

UINT FileKill(LPCTSTR lpFileName)
{
    struct stat s;

    if (remove(lpFileName) != 0)  // error
    {
        if (errno == EPERM || errno == EACCES)
        {
            if (stat(lpFileName, &s) == -1)
                return (ERR);

            // Add write permission for user
            if (chmod(lpFileName, s.st_mode | S_IWUSR) == -1)  // error
                return (ERR);

            if (remove(lpFileName) != 0)  // error
                return (ERR);
        }
    }

    return (NOERR);
}


// FileWipe()
// Like FileDelete() but overwrites the data with zeros first.
// Returns: NOERR on success, or ERR on error.

UINT FileWipe(LPCTSTR  lpFilename, UINT uTimesToWipe)
{
    auto  HFILE  hFile            = HFILE_ERROR;
    auto  LPVOID lpvBuffer        = NULL;
    const UINT   uMaxBytesToWrite = 0xFC00;
    auto  UINT   uBytesToWrite;
    auto  DWORD  dwFileSize;
    auto  DWORD  dwBytesLeft;

    // Make sure we can write to the file
    if (FileSetAttr(lpFilename, FA_NORMAL) != NOERR)
        goto Cleanup;

    // Open the file with write access
    if ((hFile = FileOpen(lpFilename, OF_WRITE)) == HFILE_ERROR)
        goto Cleanup;

    // Get the file's size
    if ((dwFileSize = FileLength(hFile)) == (DWORD)-1)
        goto Cleanup;

    // Allocate a data buffer
    if ((lpvBuffer = MemAllocPtr(GHND, uMaxBytesToWrite)) == NULL)
        goto Cleanup;

    // Set the buffer to zeros
    MEMSET(lpvBuffer, 0, uMaxBytesToWrite);

    // Overwrite the file as many times as requested
    while (uTimesToWipe)
    {
        dwBytesLeft = dwFileSize;
        
        // Seek to the beginning of the file
        if (FileSeek(hFile, 0, SEEK_SET) == (DWORD)-1)
            goto Cleanup;

        // While their are bytes left to be overwritten
        while (dwBytesLeft)
        {
            // We cannot write more than the size of the data buffer at once
            uBytesToWrite = (dwBytesLeft > uMaxBytesToWrite) ?
                            uMaxBytesToWrite :
                            (UINT) dwBytesLeft;

            // Write over the next portion of the file
            if (FileWrite(hFile, lpvBuffer, uBytesToWrite) != uBytesToWrite)
                goto Cleanup;

            dwBytesLeft -= uBytesToWrite;
        }

        uTimesToWipe--;
    }
    
    // Truncate the file
    if (FileSeek(hFile, 0, SEEK_SET) == (DWORD)-1)
        goto Cleanup;
    if (FileWrite(hFile, lpvBuffer, 0) != 0)
        goto Cleanup;

    // Free the buffer and close the file
    MemFreePtr(lpvBuffer);
    FileClose(hFile);

    // Delete the file
    return (FileDelete(lpFilename));

Cleanup:
                                       
    // Something went wrong if we're here
    if (hFile != HFILE_ERROR)
        FileClose(hFile);
    if (lpvBuffer)
        MemFreePtr(lpvBuffer);
    return (ERR);
}


// FileCopy()
// Returns one of:
//   COPY_SUCCESSFUL         The copy was successful
//   COPY_BREAK_PRESSED      The breakCheck function returned TRUE to abort the copy
//   COPY_MEMORY_ERROR       Insufficient memory
//   COPY_READ_ERROR         A read error occured
//   COPY_CRITICAL_ERROR     A critical disk error occurred
//   COPY_DISK_FULL          A write error occurred
//   > 0                     The specified DOS error occurred

int FileCopy(LPCTSTR lpFromFile, LPCTSTR lpToFile, BREAKFUNC fnBreakCheck)
{
    HFILE   hFromFile = HFILE_ERROR;
    HFILE   hToFile = HFILE_ERROR;
    UINT    uMaxBytesToWrite = 0xFC00;
    UINT    uBytesToRead = 0;
    UINT    uBytesRead = 0;
    UINT    uBytesToWrite = 0;
    UINT    uBytesWritten = 0;
    DWORD   dwFileSize = 0;
    UINT    uBufferSize = 0;
    LPVOID  lpvBuffer = NULL;
    DWORD   dwBytesLeft = 0;
    FILETIME fileTime;
    int     nReturn = COPY_SUCCESSFUL;

    // Open the source file.
    hFromFile = FileOpen(lpFromFile, READ_ONLY_FILE | OF_SHARE_DENY_NONE);
    if (hFromFile == HFILE_ERROR)
    {
        nReturn = COPY_READ_ERROR;
        goto Cleanup;
    }

    // Create/open the destination file.
    hToFile = FileCreate(lpToFile, FA_NORMAL);

    // If can't open the file, try removing read-only status.
    if (hToFile == HFILE_ERROR && FileExists(lpToFile))
    {
        // Make sure we can write to the destination file.
        if (FileSetAttr(lpToFile, FA_NORMAL) != NOERR)
        {
            nReturn = COPY_CRITICAL_ERROR;
            goto Cleanup;
        }
        hToFile = FileCreate(lpToFile, FA_NORMAL);
    }

    if (hToFile == HFILE_ERROR)
    {
        nReturn = COPY_CRITICAL_ERROR;
        goto Cleanup;
    }

    // Get the source file size
    if ((dwFileSize = FileLength(hFromFile)) == (DWORD)-1)
    {
        nReturn = COPY_READ_ERROR;
        goto Cleanup;
    }

    // Special case for zero size files
    if (dwFileSize == 0)
        goto Cleanup;

    // Allocate a data buffer
    if (dwFileSize < uMaxBytesToWrite)
        uBufferSize = (UINT) dwFileSize;
    else
        uBufferSize = uMaxBytesToWrite;
    if ((lpvBuffer = MemAllocPtr(GHND, uBufferSize)) == NULL)
    {
        nReturn = COPY_MEMORY_ERROR;
        goto Cleanup;
    }

    // Copy from the source file to the destination in chunks
    dwBytesLeft = dwFileSize;
    while (dwBytesLeft != 0)
    {
        // If the callback function returns TRUE, stop.
        if (fnBreakCheck != NULL && fnBreakCheck())
        {
            nReturn = COPY_BREAK_PRESSED;
            goto Cleanup;
        }

        // Get the size of the next chunk
        if (dwBytesLeft < uBufferSize)
            uBytesToRead = (UINT) dwBytesLeft;
        else
            uBytesToRead = uBufferSize;

        // Read from the source file
        uBytesRead = FileRead(hFromFile, lpvBuffer, uBytesToRead);
        if (uBytesRead != uBytesToRead)
        {
            nReturn = COPY_READ_ERROR;
            break;
        }

        // Write to the destination file
        uBytesWritten = FileWrite(hToFile, lpvBuffer, uBytesRead);
        if (uBytesWritten != uBytesRead)
        {
            nReturn = COPY_DISK_IS_FULL;
            goto Cleanup;
        }

        // Done with this chunk
        dwBytesLeft -= uBytesWritten;
    }

Cleanup:

    if (hFromFile != HFILE_ERROR && hToFile != HFILE_ERROR)
    {
        // Transfer the date/time of the source file to the destination file.
        FileGetTime(hFromFile, TIME_LASTWRITE, &fileTime);
        FileSetTime(hToFile, TIME_LASTWRITE, &fileTime);
        FileGetTime(hFromFile, TIME_LASTACCESS, &fileTime);
        FileSetTime(hToFile, TIME_LASTACCESS, &fileTime);
    }
                                       
    if (hFromFile != HFILE_ERROR)
        FileClose(hFromFile);

    if (hToFile != HFILE_ERROR)
        FileClose(hToFile);

    if (lpvBuffer != NULL)
        MemFreePtr(lpvBuffer);

    if (nReturn != COPY_SUCCESSFUL)
        FileDelete(lpToFile);

    return (nReturn);
}


// FileRename()
// Returns NOERR on success, or ERR on error.

UINT FileRename(LPCTSTR lpOldName, LPCTSTR lpNewName)
{
    if (rename(lpOldName, lpNewName) != 0)  // error
        return (ERR);
    return (NOERR);
}


#endif // SYM_UNIX

