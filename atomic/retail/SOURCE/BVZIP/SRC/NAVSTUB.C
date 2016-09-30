//-----------------------------------------------------------------------
// the stubs in this file were added to eliminate the need for
// SYMEVENT when building the SYMFSV DLL/LIB for NAV.  NAV utilizes
// only the UNZIP file services of SYMFSV.
//
// contact SAMI at (310) 449 4364
//-----------------------------------------------------------------------
// $Header:   S:/bvzip/VCS/navstub.c_v   1.0   06 Feb 1997 20:49:38   RFULLER  $
//-----------------------------------------------------------------------
// $Log:   S:/bvzip/VCS/navstub.c_v  $
// 
//    Rev 1.0   06 Feb 1997 20:49:38   RFULLER
// Initial revision
// 
//    Rev 1.1   04 Jun 1996 16:05:26   RHAYES
// Removed STRUPR() call from FsvIO_GetDirEntry() since it causes
// problems with directory/file names containing high-ASCII
// chars. Also circumvent FsvUtil_ValidateFullFileName() so that
// zip files containing file/directory names will be scanned.
// 
//    Rev 1.0   30 Jan 1996 15:48:10   BARRY
// Initial revision.
// 
//    Rev 1.0   30 Jan 1996 09:54:50   BARRY
// Initial revision.
// 
//    Rev 1.7   28 May 1995 20:09:30   BARRY
// Fixed wildcard match wrapper (paths always end in backslash)
// 
//    Rev 1.6   28 May 1995 20:02:46   BARRY
// Only have ZIP LZH and LHA as zip extensions now
// 
//    Rev 1.5   23 May 1995 13:16:18   BARRY
// Properly initialize BufIO sub-system (some macros weren't working before)
// 
//    Rev 1.4   22 May 1995 21:06:30   BARRY
// Fixed CharNext bug (not moving pointer)
//-----------------------------------------------------------------------
#ifdef NAV95
#include "fsv_pch.h"
#include "wnfsv.h"
#include "fsv.p"
#include "wnzip.h"
#include "platform.h"                   // Quake stuff
#include "file.h"

LONG WINAPI FsvBufIO_AllocInitFBufInfo( HFILE, WORD, LPLPFSVBUFIO_FBUF_INFO, ERROR_CALLBACK, DWORD );

//==========================================================================
//
// LONG FsvBufIO_fCreat( lpszName, nAttr, wBufSiz, lplpFBuf, lpfnCritErrFunc,
//                       dwCritErrData )
//
//    lpszName        - The name of the file to open.
//    nAttr           - The file attribute(s) for the file to create (hidden,
//                      system, etc.).
//    wBufSiz         - The size of the buffer to allocate for buffered file
//                      i/o.  A value of zero implies that the i/o will NOT
//                      be buffered.
//    lplpFBuf        - Pointer to the pointer that will point to the file
//                      buffer info structure in the "open" is successful.
//    lpfnCritErrFunc - Pointer to the function to call if a critical error
//                      is detected.  This pointer can be NULL if the caller
//                      wants to ignore critical errors.
//    dwCritErrData   - The data that is to be passed to the critical error
//                      routine (usually a structure pointer).
//
// This function tries to create the file in question and then allocate the
// file buffer info structure and an affiliate i/o buffer.
//
// Returns:
//    FSV_SUCCESS          - The operation was successful.
//    FSVERR_OUT_OF_MEMORY - Not enough memory for the dynamic data structures.
//    FSVERR_...           - The appropriate error code returned by File
//                           Services.
//
// 05/17/95 Barry       Lifted from RAPTOR\SYMFSV\SRC\FSVBUFIO.C
//==========================================================================

LONG WINAPI FsvBufIO_fCreat(
   LPCSTR                 lpszName,
   int                    nAttr,
   WORD                   wBufSiz,
   LPLPFSVBUFIO_FBUF_INFO lplpFBuf,
   ERROR_CALLBACK         lpfnCritErrFunc,
   DWORD                  dwCritErrData )
{
    HFILE hFile;
    LONG  lErr = FSV_ERROR;

    if ( lpszName == NULL || lplpFBuf == NULL )
        return (FSV_ERROR);

    *lplpFBuf = NULL;

    if ( (hFile = FileCreate(lpszName, nAttr)) != HFILE_ERROR )
        {
        lErr = FsvBufIO_AllocInitFBufInfo( hFile, wBufSiz, lplpFBuf, 
                                           lpfnCritErrFunc, dwCritErrData );

        if ( lErr != FSV_SUCCESS )
            {
            FileClose( hFile );
            FsvIO_DeleteFile( lpszName, FALSE, NULL, 0 );
            }
        }

    return (lErr);
}

//==========================================================================
//
// LONG FsvBufIO_fOpen( lpszName, nMode, wBufSiz, lplpFBuf, lpfnCritErrFunc,
//                      dwCritErrData )
//
//    lpszName        - The name of the file to open.
//    nMode           - The access privledges to be to given to the opened
//                      file (allow others to read, deny other write access,
//                      etc.).  The values passed in are the OF_... and the
//                      OF_SHARE_... constants that are used with the
//                      Windows _lopen() function.
//    wBufSiz         - The size of the buffer to allocate for buffered file
//                      i/o.  A value of zero implies that the i/o will NOT
//                      be buffered.
//    lplpFBuf        - Pointer to the pointer that will point to the file
//                      buffer info structure in the "open" is successful.
//    lpfnCritErrFunc - Pointer to the function to call if a critical error
//                      is detected.  This pointer can be NULL if the caller
//                      wants to ignore critical errors.
//    dwCritErrData   - The data that is to be passed to the critical error
//                      routine (usually a structure pointer).
//
// This function tries to open the file in question and then allocate the
// file buffer info structure and an affiliate i/o buffer.
//
// Returns:
//    FSV_SUCCESS          - The operation was successful.
//    FSVERR_OUT_OF_MEMORY - Not enough memory for the dynamic data structures.
//    FSVERR_...           - The appropriate error code returned by File
//                           Services.
//
//==========================================================================

LONG WINAPI FsvBufIO_fOpen (
   LPCSTR                 lpszName,
   int                    nMode,
   WORD                   wBufSiz,
   LPLPFSVBUFIO_FBUF_INFO lplpFBuf,
   ERROR_CALLBACK         lpfnCritErrFunc,
   DWORD                  dwCritErrData )
{
    HFILE hFile;
    LONG  lErr = FSV_ERROR;

    if ( lpszName == NULL || lplpFBuf == NULL )
        return( FSV_ERROR );

    *lplpFBuf = NULL;

    if ( (hFile = FileOpen(lpszName, nMode)) != HFILE_ERROR )
        {
        lErr = FsvBufIO_AllocInitFBufInfo( hFile, wBufSiz, lplpFBuf, 
                                           lpfnCritErrFunc, dwCritErrData );

        if ( lErr != FSV_SUCCESS )
            FileClose( hFile );
        }

    return (lErr);
}

//==========================================================================
//
// LONG FsvBufIO_fReadByts( lpFBuf, lpBuf, wBytsToRead, lpfnCritErrFunc,
//                          dwCritErrData )
//
//    lpFBuf          - Pointer to the structure used to buffer file i/o.
//    lpBuf           - Pointer to the buffer that will contain the data
//                      copied from the i/o buffer and/or disk file.
//    wBytsToRead     - The number of bytes to read.
//    lpfnCritErrFunc - Pointer to the function to call if a critical error
//                      is detected.  This pointer can be NULL if the caller
//                      wants to ignore critical errors.
//    dwCritErrData   - The data that is to be passed to the critical error
//                      routine (usually a structure pointer).
//
// This function calls FsvBufIO_fRead() to read the data in question.  This
// function will return an error if not all of the requested bytes were
// read.
//
// Returns:
//    FSV_SUCCESS - The operation was successful.
//    FSV_ERROR   - The end of file was prematurely detected which means
//                  the archive file is in an unstable state.
//    FSVERR...   - The appropriate error code returned by File Services
//                  ( i.e. file not found, disk i/o error, etc.)
//
//==========================================================================

LONG WINAPI FsvBufIO_fReadByts(
   LPFSVBUFIO_FBUF_INFO lpFBuf,
   LPVOID               lpBuf,
   WORD                 wBytsToRead,
   ERROR_CALLBACK       lpfnCritErrFunc,
   DWORD                dwCritErrData )
{
    LONG lErr;

    if ( lpFBuf == NULL || lpBuf == NULL )
        return( FSV_ERROR );

    if ( ( lErr = FsvBufIO_fRead( lpFBuf, lpBuf, wBytsToRead, lpfnCritErrFunc,
                                  dwCritErrData ) ) != ( LONG ) wBytsToRead )
    // Bads news, unable to read the file or all of the data.
        {
        if ( lErr >= 0 )                // The end of file was detected.  
            lErr = FSVERR_EOF;
        }
    else                                // Clear the error return. 
        lErr = FSV_SUCCESS;

    return( lErr );
}

//==========================================================================
//
// LONG FsvBufIO_fRead( lpFBuf, lpBuf, wBytsToRead, lpfnCritErrFunc,
//                      dwCritErrData )
//
//    lpFBuf          - Pointer to the structure used to buffer file i/o.
//    lpBuf           - Pointer to the buffer that will contain the data
//                      copied from the i/o buffer and/or disk file.
//    wBytsToRead     - The number of bytes to read.
//    lpfnCritErrFunc - Pointer to the function to call if a critical error
//                      is detected.  This pointer can be NULL if the caller
//                      wants to ignore critical errors.
//    dwCritErrData   - The data that is to be passed to the critical error
//                      routine (usually a structure pointer).
//
// This function will copy the number of bytes requested from the i/o buffer
// and/or the disk file.
//
// This function will return the number of bytes read from the i/o buffer
// and/or the disk file.  If the number returned is not equal to the number
// requested but greater than or equal to zero then the end of file was
// detected.  If the value returned is less than zero, it contains the
// File Services error code of the error that was encountered.
//
//==========================================================================

LONG WINAPI FsvBufIO_fRead(
   LPFSVBUFIO_FBUF_INFO  lpFBuf,
   LPVOID                lpBuf,
   WORD                  wBytsToRead,
   ERROR_CALLBACK        lpfnCritErrFunc,
   DWORD                 dwCritErrData )
{
    if ( lpFBuf == NULL || lpBuf == NULL )
        return( FSV_ERROR );

    return (LONG) FileRead ( lpFBuf->hFile, lpBuf, wBytsToRead );

}

//==========================================================================
//
// LONG FsvBufIO_fWriteByts( lpFBuf, lpBuf, wBytsToWrt, lpfnCritErrFunc,
//                           dwCritErrData )
//
//    lpFBuf          - Pointer to the structure used to buffer file i/o.
//    lpBuf           - Pointer to the buffer that contains the data to copy
//                      to the i/o buffer and/or disk file.
//    wBytsToWrt      - The number of bytes to write.
//    lpfnCritErrFunc - Pointer to the function to call if a critical error
//                      is detected.  This pointer can be NULL if the caller
//                      wants to ignore critical errors.
//    dwCritErrData   - The data that is to be passed to the critical error
//                      routine (usually a structure pointer).
//
// This function calls FsvBufIO_fWrite() to write the data in question.  This
// function will return an error if not all of the requested bytes were
// written.
//
//==========================================================================

LONG WINAPI FsvBufIO_fWriteByts(
   LPFSVBUFIO_FBUF_INFO lpFBuf,
   LPCVOID              lpBuf,
   WORD                 wBytsToWrt,
   ERROR_CALLBACK       lpfnCritErrFunc,
   DWORD                dwCritErrData )
{
    LONG lErr;

    if ( lpFBuf == NULL || lpBuf == NULL )
        return( FSV_ERROR );

    if ( ( lErr = 
           FsvBufIO_fWrite( lpFBuf, lpBuf, wBytsToWrt, 
                            lpfnCritErrFunc, dwCritErrData ) ) != ( LONG ) wBytsToWrt )
                                        // Bads news, unable to write to disk.
        {
        if ( lErr >= 0 )                // Insufficient disk space.
            lErr = FSVERR_DISKFULL;
        }
    else                                // Clear the error return.
        lErr = FSV_SUCCESS;

   return( lErr );
}

//==========================================================================
//
// LONG FsvBufIO_fWrite( lpFBuf, lpBuf, wBytsToWrt, lpfnCritErrFunc,
//                       dwCritErrData )
//
//    lpFBuf          - Pointer to the structure used to buffer file i/o.
//    lpBuf           - Pointer to the buffer that contains the data to copy
//                      to the i/o buffer and/or disk file.
//    wBytsToWrt      - The number of bytes to write.
//    lpfnCritErrFunc - Pointer to the function to call if a critical error
//                      is detected.  This pointer can be NULL if the caller
//                      wants to ignore critical errors.
//    dwCritErrData   - The data that is to be passed to the critical error
//                      routine (usually a structure pointer).
//
// This function will write the specified bytes to the i/o buffer and/or the
// disk file.
//
// This function will return the number of bytes written to the i/o buffer
// and/or the disk file.  If the value returned is less than zero, it contains
// the File Services error code of the error that was encountered.
//
//==========================================================================

LONG WINAPI FsvBufIO_fWrite(
   LPFSVBUFIO_FBUF_INFO lpFBuf,
   LPCVOID              lpBuf,
   WORD                 wBytsToWrt,
   ERROR_CALLBACK       lpfnCritErrFunc,
   DWORD                dwCritErrData )
{
    LONG lBytesWritten;
    LONG lTell;

    if ( lpFBuf == NULL || lpBuf == NULL )
        return( FSV_ERROR );

    lBytesWritten = FileWrite ( lpFBuf->hFile, lpBuf, wBytsToWrt );
    lTell = FileSeek( lpFBuf->hFile, 0L, SEEK_CUR );

    lpFBuf->lEOFPos = max(lpFBuf->lEOFPos, lTell);
    return lBytesWritten;
}

//==========================================================================
//
// LONG FsvBufIO_fTell( lpFBuf )
//
//    lpFBuf - Pointer to the structure used to buffer file i/o.
//
// This function returns the logical file position.
//
//==========================================================================

LONG WINAPI FsvBufIO_fTell( LPCFSVBUFIO_FBUF_INFO lpFBuf )
{
    if ( lpFBuf == NULL )
        return( FSV_ERROR );
    
    return FileSeek (lpFBuf->hFile, 0L, SEEK_CUR);
}

//==========================================================================
//
// LONG FsvBufIO_fSeek( lpFBuf, lOffset, nSeekOp, lpfnCritErrFunc,
//                      dwCritErrData )
//
//    lpFBuf          - Pointer to the structure used to buffer file i/o.
//    lOffset         - The offset to add to the beggining of file, current
//                      file position, or the end of the file.
//    nSeekOp         - The type of seek operation to perform (seek from
//                      beginning of file, seek from current file position,
//                      seek to the end of the file).
//    lpfnCritErrFunc - Pointer to the function to call if a critical error
//                      is detected.  This pointer can be NULL if the caller
//                      wants to ignore critical errors.
//    dwCritErrData   - The data that is to be passed to the critical error
//                      routine (usually a structure pointer).
//
// This function will update the logical file position.
//
// This function will return the new logical file position if successful,
// or the appropriate File Services error code if it failed.
//
//==========================================================================

LONG WINAPI FsvBufIO_fSeek(
   LPFSVBUFIO_FBUF_INFO lpFBuf,
   LONG                 lOffset,
   int                  nSeekOp,
   ERROR_CALLBACK       lpfnCritErrFunc,
   DWORD                dwCritErrData )
{
    if ( lpFBuf == NULL )
        return( FSV_ERROR );

    return FileSeek (lpFBuf->hFile, lOffset, nSeekOp);
}

//==========================================================================
//
// BOOL FsvBufIO_fSeekCausesFlush( lpFBuf, lOffset, nSeekOp )
//
//    lpFBuf  - Pointer to the structure used to buffer file i/o.
//    lOffset - The offset to add to the beggining of file, current file
//              position, or the end of the file.
//    nSeekOp - The type of seek operation to perform (seek from beginning
//              of file, seek from current file position, seek to the end
//              of the file).
//
// This function checks to see if the "seek" will cause the i/o buffer to
// be flushed.
//
// Returns:
//    FALSE - I/O buffer will NOT be flushed.
//    TRUE  - I/O buffer WILL be flushed.
//
//==========================================================================

BOOL WINAPI FsvBufIO_fSeekCausesFlush(
   LPFSVBUFIO_FBUF_INFO lpFBuf,
   LONG                 lOffset,
   int                  nSeekOp )
{
    return ( FALSE );
}

//==========================================================================
//
// LONG FsvBufIO_fSetEOF( lpFBuf, lEOFPostn, lpfnCritErrFunc, dwCritErrData )
//
//    lpFBuf          - Pointer to the structure used to buffer file i/o.
//    lEOFPostn       - The zero-based file position to make the End of File.
//    lpfnCritErrFunc - Pointer to the function to call if a critical error
//                      is detected.  This pointer can be NULL if the caller
//                      wants to ignore critical errors.
//    dwCritErrData   - The data that is to be passed to the critical error
//                      routine (usually a structure pointer).
//
// This function will try to set the end of file to the position specified
// (an index position of 5 will make the file 5 bytes long).  It will only
// update the current file position to the new end of file position, if the
// new end of file position is less the old current file position.
//
// Returns:
//    FSV_SUCCESS - The operation was successful.
//    FSVERR_...  - The appropriate error code returned by File Services.
//
//==========================================================================

LONG WINAPI FsvBufIO_fSetEOF(
   LPFSVBUFIO_FBUF_INFO  lpFBuf,
   LONG                  lEOFPostn,
   ERROR_CALLBACK        lpfnCritErrFunc,
   DWORD                 dwCritErrData )
{
    LONG lErr = FSV_ERROR;
    LONG lOrigPos;
    BYTE bWrite = 0;

    if ( lpFBuf == NULL )
        return( FSV_ERROR );

                                        // Save current position
    lOrigPos = FileSeek ( lpFBuf->hFile, 0L, SEEK_CUR );

    if ( (lErr = FileSeek (lpFBuf->hFile, 0L, SEEK_END)) < 0L )
        return ( lErr );

#ifndef SYM_WIN32
    if ( lEOFPostn > lErr )             // Are we expanding file?
        {
        bWrite = 1;
        lEOFPostn -= 1L;                // In DOS and expanding file, need
                                        // to go one less and write a byte
        }
#endif

                                        // Position to new EOF
    if ( (lErr = FileSeek ( lpFBuf->hFile, lEOFPostn, SEEK_SET )) == lEOFPostn )
        {
#ifdef SYM_WIN32
        if ( SetEndOfFile ( (HANDLE)lpFBuf->hFile ) )
#else
                                        // Writing 0 bytes informs DOS to 
                                        // truncate file when shortening
                                        // Writing 1 byte is needed when
                                        // expanding to catch disk full.
        if ( (lErr = FileWrite ( lpFBuf->hFile, &bWrite, bWrite ) == bWrite )
#endif
            lpFBuf->lEOFPos = lEOFPostn + bWrite;
            lErr = FSV_SUCCESS;
        }
                                        // Restore original position
    if ( lOrigPos > 0L )
        FileSeek ( lpFBuf->hFile, lOrigPos, SEEK_SET );

    return ( lErr );
}

//==========================================================================
//
// LONG FsvBufIO_fFlush( lpFBuf, lpfnCritErrFunc, dwCritErrData )
//
//    lpFBuf          - Pointer to the structure used to buffer file i/o.
//                      This pointer will be NULL if the ...
//    lpfnCritErrFunc - Pointer to the function to call if a critical error
//                      is detected.  This pointer can be NULL if the caller
//                      wants to ignore critical errors.
//    dwCritErrData   - The data that is to be passed to the critical error
//                      routine (usually a structure pointer).
//
// This function will write out the i/o buffer if it has been "dirtied" and
// it will move the DOS file position to the 1st byte after the end of the
// current buffer.
//
// Returns:
//    FSV_SUCCESS - The operation was successful.
//    FSVERR_...  - The appropriate error code returned by File Services.
//
//==========================================================================

LONG WINAPI FsvBufIO_fFlush(
   LPFSVBUFIO_FBUF_INFO lpFBuf,
   ERROR_CALLBACK       lpfnCritErrFunc,
   DWORD                dwCritErrData )
{
    return FSV_SUCCESS;                 // Do nothing (no buffering)
}

//==========================================================================
//
// VOID FsvBufIO_fClose( lpFBuf, bFlushChk, lpfnCritErrFunc, dwCritErrData )
//
//    lpFBuf          - Pointer to the structure used to buffer file i/o.
//    bFlushChk       - If this flag is TRUE then the buffer affiliated with
//                      the file will be written if it has been modified.
//    lpfnCritErrFunc - Pointer to the function to call if a critical error
//                      is detected.  This pointer can be NULL if the caller
//                      wants to ignore critical errors.
//    dwCritErrData   - The data that is to be passed to the critical error
//                      routine (usually a structure pointer).
//
// This function tries to write out any "dirty" data in the i/o buffer
// and close the file in question.  It will also free the i/o buffer and
// the file buffer info structure.
//
// Returns:
//    FSV_SUCCESS - The close was successful.
//    FSVERR_...  - The appropriate error code returned by File Services
//                  (not good).
//
//==========================================================================

LONG WINAPI FsvBufIO_fClose(
   LPFSVBUFIO_FBUF_INFO lpFBuf,
   BOOL                 bFlushChk,
   ERROR_CALLBACK       lpfnCritErrFunc,
   DWORD                dwCritErrData )
{ 
    HFILE       hErr;

    if ( lpFBuf == NULL )
        return( FSV_ERROR );

    hErr = FileClose (lpFBuf->hFile);

    MemFreePtr ( lpFBuf );

    return ( (LONG) hErr );
}

//===========================================================================
//
// FsvIO_DeleteFile( lpFilename, fRecycle, lpCrit, dwCrit ) : long;
//
// Deletes the specified file.
//
// Input:
//    lpFilename     specifies the file to be deleted
//    fRecycle       TRUE = move to Recycle Bin, FALSE = normal delete.
//    lpCrit         Critical error handler.
//    dwCrit         dwparam to pass to critical error handler.
//
// Returns: FSV_SUCCESS if successful, FSV_ERROR if not
//
//==========================================================================

long WINAPI FsvIO_DeleteFile(
    LPCSTR lpFilename, 
    BOOL fRecycle, 
    ERROR_CALLBACK lpCrit, 
    DWORD dwCrit )
{
                                        // Use the arguments
    fRecycle == fRecycle; lpCrit == lpCrit; dwCrit == dwCrit;

    if ( FileDelete (lpFilename) == NOERR )
        return FSV_SUCCESS;
    else
        return FSV_ERROR;
}

//============================================================================
// FsvStr_GetByteCount: LONG
//
// Returns the total count of bytes used by the string (as opposed to the
// string length which is normally a count of characters used by the string).
// The count does NOT include the terminating NULL character.
//
//============================================================================

LONG WINAPI FsvStr_GetByteCount( LPCTSTR  lpszIn )
{
    return ( (LONG) STRLEN (lpszIn) );
}

//============================================================================
// FsvStr_GetCharCount: LONG
//
// Returns the total count of characters used by the string (length of the
// string rather than the count of bytes used by the string).  The count does
// NOT include the terminating NULL character.
//
LONG WINAPI FsvStr_GetCharCount( LPCTSTR lpszIn )
{
    LONG     lRtn = 0;

    if (lpszIn)
        {
        while (*lpszIn)
            {
            lRtn++;
            lpszIn = CharNext(lpszIn);
            }
        }

   return lRtn;
}

//============================================================================
// FsvStr_RevStr: LPTSTR
//
// Returns a pointer to the n-th character from the end of the string.  If
// n is 0, the pointer is to the terminating NULL of the string.  If n is
// greater than the length of the string, the return value is NULL.
//
LPTSTR WINAPI FsvStr_RevStr( LPCTSTR  lpszIn, int nthChar )
{
   LPTSTR   lpszRtn = NULL;
   LPCTSTR  lpszWork = lpszIn;
   LONG     lStrLen = FsvStr_GetCharCount( lpszWork );

   if ( lpszIn && nthChar >= 0 && nthChar <= lStrLen )
   {
         while ( (lStrLen - nthChar) > 0)
         {
            lStrLen--;
            lpszWork = CharNext(lpszWork);
         }
         lpszRtn = (LPTSTR)lpszWork;
   }

   return lpszRtn;
}

//==========================================================================
//
// BOOL FsvUtil_ValidateFileName( lpszFileMask, f83 )
//
//    lpszFileName - Pointer to the "file name" to validate.
//    f83 - Validate as 8.3 filename
//
// This function validates the "file name" (cannot have a path) entered by
// the user.  It checks for multiple extension separators, bad characters,
// too many characters, and wild card characters.
//
// This function returns "TRUE" if the file name is valid and "FALSE" if it
// is bogus.
//
//==========================================================================

BOOL WINAPI FsvUtil_ValidateFileName( LPCTSTR lpszFileName, BOOL f83 )
{
    f83 == f83;                         // Use arguments

                                        // MOREWORK: ZIP is only using 8.3
                                        // names and is asking for validation
                                        // of these types of files. Quake
                                        // doesn't support this option.
    return NameValidatePath((LPTSTR)lpszFileName);
}

//==========================================================================
//
// BOOL FsvUtil_ValidateFullFileName( lpszFullMask, f83 )
//
//    lpszFullName - Pointer to the "file name" to validate.
//    f83 - Validate as a 8.3 filename
//
// This function validates the "file name" (it can have a path) entered by
// the user.  It checks for multiple extension separators, bad characters,
// too many characters, wild card characters, and anything else that is
// unacceptable.
//
// This function returns "TRUE" if the file name is valid and "FALSE" if it
// is bogus.
//
//==========================================================================

BOOL WINAPI FsvUtil_ValidateFullFileName( LPCTSTR lpszFullName, BOOL f83 )
{
    f83 == f83;                         // Use arguments
#ifdef SYM_WIN32
                                        // RLH - NameValidatePath fails if
                                        // the file name contains high ASCII
                                        // chars. Since the file names were
                                        // OK to get added originally AND
                                        // NAV is a extract-only operation,
                                        // we skip this test...
    return (TRUE);
#endif
                                        // MOREWORK: ZIP is only using 8.3
                                        // names and is asking for validation
                                        // of these types of files. Quake
                                        // doesn't support this option.
    return NameValidatePath((LPTSTR)lpszFullName);
}

//==========================================================================
//
// VOID FsvUtil_GetPathMask( lpszFullName, lpszPath, lpszMask, f83 )
//
//  lpszName - Pointer to a fully qualified drive/path with an optional
//             file mask.
//  lpszPath - Pointer to the string that will contain the drive and the
//             path.  This pointer can be NULL.
//  lpszMask - Pointer to the string that will contain the optional mask.
//             This pointer can be NULL.
//  f83      - Treat names as 8.3 filenames?
//
// This function extracts the drive/path and the optional path from the
// fully qualified path name.
//
//==========================================================================

VOID WINAPI FsvUtil_GetPathMask( LPCSTR lpszFullName,LPSTR lpszPath,LPSTR lpszMask, BOOL f83 )
{
    CHAR  szTempFile [ SYM_MAX_PATH ];
    CHAR  szTempPath [ SYM_MAX_PATH ];
    LPSTR lpszTempPtr;
    BOOL  bHasMask=TRUE;

                                        // Extablish baseline path & file
    STRCPY( szTempPath, lpszFullName ); 
    NameReturnFile( szTempPath, szTempFile );
    NameStripFile( szTempPath );
    FsvUtil_AddBackSlashChk( szTempPath );
      
    // Handle special cases (as was done in original Fsv function):
    // 1) File portion is only dots (indicating it's a directory)
    // 2) UNC case of \\server or \\server\vol

                                        // Only have Dots in the name?
    if ( *szTempFile && STRSPN( szTempFile, "." ) == STRLEN ( szTempFile ) )
        bHasMask = FALSE;

    if ( lpszFullName[0]=='\\' && lpszFullName[1]=='\\' )
        {
                                        // Check for \\Server only
        lpszTempPtr = STRCHR ( &lpszFullName[2], '\\' );

        if ( lpszTempPtr == NULL )
            bHasMask = FALSE;
                                        // Check for \\Server\\Volume and
                                        //           \\Server\\Volume: only
        else if ( STRCHR ( &lpszTempPtr[1], '\\' ) == NULL &&
                  STRCHR ( &lpszTempPtr[1], ':' ) == NULL )
            bHasMask = FALSE;
        }

    if ( lpszPath )
        STRCPY( lpszPath, szTempPath );

    if ( lpszMask )
        {
        if ( bHasMask )
            STRCPY ( lpszMask, szTempFile );
        else
            *lpszMask = EOS;
        }
}

//==========================================================================
//
// VOID FsvUtil_AddBackSlashChk( lpszPath )
//
//  lpszPath - Pointer to the path name.
//
// This function adds a trailing "\" to the path name if one doesn't
// exist.
//
//==========================================================================

VOID WINAPI FsvUtil_AddBackSlashChk( LPSTR lpszPath )
{
    LPSTR lpszTemp ;

    lpszTemp = FsvStr_RevStr ( lpszPath, 1 );
    
    if ( lpszPath && *lpszPath && *lpszTemp != '\\' )
        {
        lpszTemp = CharNext ( lpszTemp );
        *lpszTemp++ = '\\';
        *lpszTemp = EOS;
        }
}

//==========================================================================
//
// VOID FsvUtil_RemoveBackSlashChk( lpszPath )
//
//  lpszPath - Pointer to the path name.
//
// This function removes the trailing "\" to the path name if it's present
//
//==========================================================================

VOID WINAPI FsvUtil_RemoveBackSlashChk( LPSTR lpszPath )
{
    LPSTR lpszTemp ;

    lpszTemp = FsvStr_RevStr ( lpszPath, 1 );
    
    if ( lpszPath && *lpszPath && *lpszTemp == '\\' )
        *lpszTemp = EOS;
}

//==========================================================================
//
// FsvUtil_CreateMultipleDirs( lpszDirName, lpCrit, dwCrit ) : LONG
//
//    lpszDirName - Pointer to the fully qualified source name for the file
//                  operation.
//    lpCrit      - Callback pointer to the critical error handler.  Can be
//                  NULL.
//    dwCrit      - Pointer to the critical error structure.  Can be NULL.
//
// This function is called when it has been determined that a path
// specified by the operator doesn't exist.  This function will determine
// which portion of the path does exist and then create all of the subsequent
// sub-directories that do not exist.
//
// Notes:  1) It is assumed that the directory contains a drive and a
//            "path".
//
//==========================================================================

LONG WINAPI FsvUtil_CreateMultipleDirs( LPSTR lpszDirName, ERROR_CALLBACK lpCrit, DWORD dwCrit )
{
    if ( DirMakePath(lpszDirName) == NOERR )
        return FSV_SUCCESS;
    else
        return FSV_ERROR;
}

//============================================================================
// FsvUtil_GetTempFileName( bDrive, lpszPrefix, uUnique, lpszBuffer, dwEstSize) : long
//
//    bDrive         suggested drive ('A','B', etc)
//    lpszPrefix     prefix to use for filename (may be NULL)
//    uUnique        number to use as prefix
//    lpszBuffer     output buffer for filename
//    dwEstSize      estimated size of the file
//
// This function will return a temp file name. It is identical to the Windows
// API call GetTempFileName, with the exception of the estimated size field.
//
//============================================================================

#ifdef COMMENT_OUT      // REMOVE IF THIS WORKS
long WINAPI FsvUtil_GetTempFileName(BYTE bDrive,LPCSTR lpszPrefix,UINT uUnique,LPSTR lpszBuffer,DWORD dwEstSize)
{
    long lSuccess = FSVERR_DISKFULL;

                                        // Use the arguments
    dwEstSize == dwEstSize;

                                        // Degraded from BV version:
                                        // No check for disk space.
    if ( lpszBuffer )
        {
        TCHAR szPath [SYM_MAX_PATH];

        /* Construct a temporary file using the specified prefix. */
        GetTempPath( sizeof(szPath), szPath );
        if ( GetTempFileName( szPath, lpszPrefix, uUnique, lpszBuffer ) != 0 )
            lSuccess = FSV_SUCCESS;            
        }

   return lSuccess;
}
#endif

//===========================================================================
//
// FsvIO_SetFileAttributes( lpFilename, dwAttributes, lpCrit, dwCrit ) : long;
//
// Sets the attributes WORD for the specified file.
//
// Input:
//      lpFilename     far ptr to path\filename
//      dwAttributes   new attributes DWORD
//      lpCrit         Critical error handler.
//      dwCrit         dwparam to pass to critical error handler.
//
// Returns: FSV_SUCCESS if successful, FSVERR_??? if not
//
//===========================================================================

long WINAPI FsvIO_SetFileAttributes (LPCSTR lpFilename,DWORD dwAttributes, ERROR_CALLBACK lpCrit, DWORD dwCrit )
{
    LONG     lRtn = FSV_ERROR;
    if (lpFilename)
        {
        if (FileSetAttr( lpFilename, (DWORD)dwAttributes ) != ERR)
            lRtn = FSV_SUCCESS;
        }
   return lRtn;
}

//===========================================================================
//
// FsvIO_GetDriveLabel( bDrive, lpszLabel, wBytes, lpCrit, dwCrit ) : long;
//
//    bDrive      the drive number  (1=A, 2=B, etc)
//    lpszLabel   buffer to place the label in
//    wBytes      size of the buffer (including the null terminator).
//      lpCrit      Critical error handler.
//      dwCrit      dwparam to pass to critical error handler.
//
// Retrieves the label for the specified drive.
//
// Returns: FSV_ERROR if an error occurs, FSV_SUCCESS if a label returned;
//
//===========================================================================

long WINAPI FsvIO_GetDriveLabel (BYTE bDrive,LPSTR lpszLabel,WORD wBytes, ERROR_CALLBACK lpCrit, DWORD dwCrit )
{
    CHAR cDriveLetter = bDrive + 'A' - 1;
    
    if ( VolumeLabelGet( cDriveLetter, lpszLabel ) == NOERR )
        return FSV_SUCCESS;
    else
        return FSV_ERROR;
}

//============================================================================
//
// FsvIO_GetDirEntry( lpFullName, lpDirEntry, lpLFN, lpCrit, dwCrit ) : long
//
//      lpFullName     Full Name of the file or directory
//      lpDirEntry     Far pointer to an FSVIO_DIRENTRYSTRUCT
//      lpLFN          Far pointer to buffer to be filled with LFN (may be NULL)
//                     This pointer will be assigned to lpDirEntry->lpszLongDesc.
//      lpCrit         Critical error handler.
//      dwCrit         dwparam to pass to critical error handler.
//
// Fills in a DirEntry structure for the given file/directory. If the name ends
// in a trailing backslash, it will assume a directory. If it does not end in a
// backslash, it will try to find a matching file, then a matching directory.
//
// Returns:    FSV_SUCCESS if successful, FSV_ERROR if unsuccessful
//
//============================================================================

long WINAPI FsvIO_GetDirEntry (
   LPCTSTR                 lpFullName,
   LPFSVIO_DIRENTRYSTRUCT  lpDirEntry,
   LPTSTR                  lpLFN,
   ERROR_CALLBACK          lpCrit,
   DWORD                   dwCrit )
{
    long               lResult = FSV_ERROR ;

    if ( lpFullName && lpDirEntry && *lpFullName )
        {
        HANDLE hFind;
        WIN32_FIND_DATA ffData;
        TCHAR szOemFullName[FSV_LFILENAMEBUF_LEN];

        STRNCPY( szOemFullName, lpFullName, sizeof(szOemFullName) );
#ifndef SYM_WIN32
                              // RLH - Do not convert to upper case since
                              // some OEM chars. do not convert correctly to
                              // upper case.
        STRUPR( szOemFullName );
#endif
        FsvUtil_RemoveBackSlashChk( szOemFullName );

                                        // NOTE: W32 call (not Quake)
        hFind = FindFirstFile( szOemFullName, &ffData );

        if ( hFind != INVALID_HANDLE_VALUE )
            {
            FindClose ( hFind );        // Only needed ffData...

            FileTimeToDosTime( &(ffData.ftLastWriteTime), 
                               &(lpDirEntry->wDate), &(lpDirEntry->wTime));
            lpDirEntry->dwFsvAttrib  = ffData.dwFileAttributes;
            lpDirEntry->dwDosAttrib  = ffData.dwFileAttributes;
            lpDirEntry->dwTag        = 0L;
            lpDirEntry->lpFsvNet     = NULL;
            lpDirEntry->lpUndInfo    = NULL;
            lpDirEntry->lpNetInfo    = NULL;
            lpDirEntry->lpszLongDesc = lpLFN;

            if ( lpLFN )
                {
                if ( *ffData.cFileName != EOS )
                    STRNCPY( lpLFN, ffData.cFileName, FSV_LFILENAMEBUF_LEN );
                else
                    STRNCPY( lpLFN, ffData.cAlternateFileName, FSV_LFILENAMEBUF_LEN );

                lpLFN[FSV_LFILENAMEBUF_LEN - 1] = EOS;
                }

            lpDirEntry->bLevel            = 0;
            lpDirEntry->ftCreationTime    = ffData.ftCreationTime;
            lpDirEntry->ftLastAccessTime  = ffData.ftLastAccessTime;
            lpDirEntry->ftLastWriteTime   = ffData.ftLastWriteTime;

            if ( *ffData.cAlternateFileName != EOS )
                STRNCPY( lpDirEntry->szName, ffData.cAlternateFileName, sizeof(lpDirEntry->szName) );
            else
                STRNCPY( lpDirEntry->szName, ffData.cFileName, sizeof(lpDirEntry->szName) );
            
            lpDirEntry->szName[sizeof(lpDirEntry->szName)-1] = EOS;

            if ( !(ffData.dwFileAttributes & FSV_ATTRIB_DIRECTORY) )
                {
                lpDirEntry->dwSize = ffData.nFileSizeLow;
                lpDirEntry->dwSizeHigh = ffData.nFileSizeHigh;
                }
            else
                {
                lpDirEntry->dwSize = 0;
                lpDirEntry->dwSizeHigh = 0;
                }

            lResult = FSV_SUCCESS;
            }
        else
            {
                                        // Assume this is the reason we failed
            lResult = FSV_ERROR;        // Wanted: FSV_FILE_NOT_FOUND;                                
            }
        }
    return lResult;
}

//==========================================================================
//
// BOOL FsvUtil_ValidateFileMask( lpszFileMask, f83 )
//
//    lpszFileMask - Pointer to the "file mask" to validate.
//
// This function validates the "file mask" (cannot have a path) entered by
// the user.  It checks for multiple extension separators, bad characters,
// too many characters, and illegal use of wild card characters.
//
// This function returns "TRUE" if the mask is valid and "FALSE" if it
// is bogus.
//
// Note: When this function encounters an asterisk it removes any unnecessary
//       characters that follow it (i.e. "**.*NC*" ==> "*.*").  This means
//       that the input string could possibly be MODIFIED.
//

BOOL WINAPI FsvUtil_ValidateFileMask(
   LPTSTR lpszFileMask,
   BOOL  f83 )
{
   BOOL bMaskValid;
   WORD wAsteriskCount;
   WORD wExtSepCount;
   WORD wSubCount;
   WORD wMaxChs;
   LPTSTR lpszFileMaskBase = lpszFileMask;

   if ( lpszFileMask == NULL || lpszFileMask[ 0 ] == '\0' )
   // Bogus parameters.
   {
      return( FALSE );
   }

   if (f83)
   {
      bMaskValid     = TRUE;
      wAsteriskCount = 0;
      wExtSepCount   = 0;
      wSubCount      = 0;
      wMaxChs        = FSV_FILEPREFIX_LEN;

      /* Parse the file mask for validity. */
      for ( ; bMaskValid && *lpszFileMask != '\0'; lpszFileMask = CharNext(lpszFileMask) )
      {
         switch ( *lpszFileMask )
         {
            // Don't allow multiple extension separators.
            case '.':
               wExtSepCount++;
               bMaskValid     = ( wExtSepCount == 1 && ( wSubCount > 0 || wAsteriskCount > 0 ) );
               wAsteriskCount = 0;
               wSubCount      = 0;
               wMaxChs        = FSV_FILEEXTENSION_LEN;
            break;

            case '*':
            {
               int   nCount;
               LPSTR lpszTmp;

               wAsteriskCount++;
   //          bMaskValid = ( wAsteriskCount == 1 );

               if ( wSubCount == wMaxChs )
               /* The asterisk is superfluous, so remove it. */
               {
                  lpszFileMask = CharPrev( lpszFileMaskBase, lpszFileMask );
               }

               /* Scan for any unnecessary characters that follow the */
               /* asterisk.                                           */
               for ( nCount = 0, lpszTmp = CharNext( lpszFileMask );
                     lpszTmp[ 0 ] != '.' && lpszTmp[ 0 ] != '\0';
                     lpszTmp = CharNext(lpszTmp), nCount++ );

               if ( nCount > 0 )
               /* Remove unnecessary characters that followed the asterisk. */
               {
                  _fmemmove( CharNext(lpszFileMask), lpszTmp,
                             FsvStr_GetByteCount( lpszTmp ) + 1 );
               }
            }
            break;

            /* These characters be bogus. */
            case ' ':
            case '/':
            case '|':
            case ',':
            case ';':
            case '+':
            case '=':
            case '[':
            case ']':
            case '<':
            case '>':
            case '"':
            case ':':
            case '\\':
               bMaskValid = FALSE;
            break;

            default:
               /* Account for the character. */
               wSubCount++;
               if (IsDBCSLeadByte( *lpszFileMask ))
                  wSubCount++;

               if ( wSubCount > wMaxChs )
               /* Too many characters for the file portion or */
               /* the extension portion.                      */
               {
                  bMaskValid = FALSE;
               }
            break;
         }
      }
   }
   else // Validate long filename
   {
      bMaskValid     = TRUE;
      //This is done elsewhere now, can except the exotic masks
   }

   return( bMaskValid );
}

//==========================================================================
//
// BOOL FsvUtil_ValidatePathFileMask( lpszFileMask, bAllwSubstMask, f83  )
//
//    lpszFileMask    - Pointer to the "file mask" to validate.
//    bAllowSubstMask - If this flag is TRUE then allow the path to contain
//                      the "substition mask" (i.e. "*\").
//    f83 - Validate as 8.3 name
//
// This function validates the "file mask" entered by the user.  It checks
// for multiple extension separators, bad characters and other stuff like
// that.  If the mask contains a path then the path will also be validated.
//
// This function returns "TRUE" if the mask is valid and "FALSE" if it
// is bogus.
//
// Note: When this function encounters an asterisk it removes any unnecessary
//       characters that follow it (i.e. "**.*NC*" ==> "*.*").  This means
//       that the input string could possibly be MODIFIED.
//
//==========================================================================

BOOL WINAPI FsvUtil_ValidatePathFileMask( LPTSTR lpszFileMask, BOOL bAllowSubstMask, BOOL f83 )
{
   BOOL  bMaskValid;
   WORD  wLen;
   LPSTR lpszNode;

   if ( lpszFileMask == NULL || lpszFileMask[ 0 ] == '\0' )
   // Bogus parameters.
   {
      return( FALSE );
   }

   if (f83)
   {
      // Init locals.
      bMaskValid = TRUE;
      wLen       = (WORD)FsvStr_GetCharCount( lpszFileMask );
      lpszNode   = FsvStr_RevStr( lpszFileMask, 1 );

      /* Does the mask contain a path? */
      for ( ;
            wLen > 0 && *lpszNode != '\\' && *lpszNode != ':';
            wLen--, lpszNode = CharPrev(lpszFileMask,lpszNode) );

#ifdef NOT_IMPLEMENTED
      if ( !FsvUtil_IsPathLenValid( FsvStr_GetByteCount(lpszFileMask) ) )
      /* The "path" is "too" big.  Must have room for a file name. */
      {
         bMaskValid = FALSE;
      }
      else
#endif
      if ( wLen > 0 )
      /* Validate the path. */
      {
         BOOL  bHasExtSprtr = FALSE;
         LPSTR lpszCh       = lpszFileMask;
         WORD  wChCount     = 0;
         WORD  wColonCount  = 0;
         WORD  wAstrskCnt   = 0;

         for ( ; bMaskValid && wLen > 0 ; wLen--, lpszCh = CharNext(lpszCh) )
         {
            switch ( *lpszCh )
            {
               /* Don't allow multiple extension separators. */
               case '.':
                  bMaskValid   = !bHasExtSprtr;
                  bHasExtSprtr = TRUE;
                  wChCount     = 0;
               break;

               case ':':
                  bMaskValid = ( wColonCount == 0 &&
                                 lpszFileMask[ 1 ] == ':' &&
                                 CharIsAlpha( lpszFileMask[ 0 ] ) );
                  wColonCount++;
                  wChCount = 0;
               break;

               case '*':
                  if ( bAllowSubstMask && wAstrskCnt == 0 )
                  {
                     wAstrskCnt++;
                     continue;
                  }
               break;

               /* These characters be bogus. */
               case ' ':
               case '/':
               case '|':
               case ',':
               case ';':
               case '+':
               case '=':
               case '[':
               case ']':
               case '<':
               case '>':
               case '"':
               case '?':
                  bMaskValid = FALSE;
               break;

               case '\\':
                  bMaskValid   = ( wChCount > 0 || lpszCh == lpszFileMask ||
                                   lpszCh[ -1 ] == ':' || wAstrskCnt > 0 );
                  bHasExtSprtr = FALSE;
                  wChCount     = 0;
                  wAstrskCnt   = 0;
               break;

               default:
                  wChCount++;
                  bMaskValid = ( wAstrskCnt == 0 &&
                                 ( ( !bHasExtSprtr && wChCount <= FSV_FILEPREFIX_LEN ) ||
                                   ( bHasExtSprtr  && wChCount <= FSV_FILEEXTENSION_LEN ) ) );
               break;
            }
         }
      }

      if ( bMaskValid && *CharNext(lpszNode) != '\0' )
      // Call routine to validate the file mask portion.
      {
         bMaskValid = FsvUtil_ValidateFileMask( ++lpszNode, f83 );
      }
   }
   else // Validate LFN
   {
      LPTSTR   lpszTemp1,lpszFilePart;
      BOOL     fPath = FALSE;

      bMaskValid = FALSE;

      // Does it begin with *\ ?
      // Does it begin with \\ ?
      // Does it begin with d: ?
      // Does it begin with \  ?
      // Does it contain \     ?
      lpszTemp1 = CharNext(lpszFileMask);
      if ( strncmp(lpszFileMask,"*\\",2) == 0 ||
           strncmp(lpszFileMask,"\\\\",2) == 0 )
      {
         bMaskValid = fPath = TRUE;
         lpszFileMask = CharNext(lpszFileMask);
         lpszFileMask = CharNext(lpszFileMask);
      }
      else if ( strncmp(lpszTemp1,":",1) == 0 &&
                CharIsAlpha(*lpszFileMask) )
      {
         bMaskValid = fPath = TRUE;
         lpszFileMask = CharNext(lpszTemp1);
         if (strncmp(lpszFileMask,"\\",1) == 0)
            lpszFileMask = CharNext( lpszFileMask );
      }
      else if ( strncmp(lpszFileMask,"\\",1) == 0 &&
                strncmp(lpszTemp1,"\\",1) != 0)
      {
         bMaskValid = fPath = TRUE;
         lpszFileMask = CharNext(lpszTemp1);
      }
      else if ( strchr(lpszFileMask,'\\') )
      {
         bMaskValid = fPath = TRUE;
      }
      else
      {
         lpszFilePart = lpszFileMask;
      }

      // lpszFileMask now points past the initial path header.

      if (bMaskValid && fPath)
      {
         // Validate rest of path up to filename
         lpszFilePart = strrchr( lpszFileMask, '\\' );
         if (!lpszFilePart)
         {
            // No path.
            lpszFilePart = lpszFileMask;
         }
         else
         {
            if (lpszFilePart == lpszFileMask)
            {
               // Empty path.  Error.
               bMaskValid = FALSE;
            }
            else
            {
               lpszFilePart = CharNext( lpszFilePart );

               while (bMaskValid && lpszFileMask < lpszFilePart)
               {
                  switch (*lpszFileMask)
                  {
                     case ':':
                     case '*':
                     case '/':
                     case '|':
                     case '<':
                     case '>':
                     case '"':
                     case '?':
                        bMaskValid = FALSE;
                     break;
                  }
                  lpszFileMask = CharNext(lpszFileMask);
               }
            }
         }
      }

      if (bMaskValid || !fPath)
      {
         // Validate filename

         // Could be just a path.
         if (lstrlen(lpszFilePart) > 0)
         {
            bMaskValid = FsvUtil_ValidateFileMask( lpszFilePart, f83 );
         }
      }
   }

   return( bMaskValid );
}

//==========================================================================
//
// BYTE FsvUtil_ConvrtDrvLtr2ID( cDrvLtr )
//
//  cDrvLtr - The "drive letter" for a disk drive.
//
// This functions converts the specified character to a
// one-based "disk drive ID" (i.e. 1 = A:).
//
// This function returns a one-based disk drive ID.
//
//==========================================================================

BYTE  WINAPI FsvUtil_ConvrtDrvLtr2ID(char cDrvLtr )
{
   return( ( BYTE ) ( CharToUpper ( cDrvLtr ) - 'A' + 1 ) );
}

//===========================================================================
//
// FsvUtil_IsZipFile( lpszName, lpwType ) : BOOL
//
//    lpszName     Incoming file name (may include path)
//    lpwType      Will be filled with the archive file type.
//                 ZIP_??? - defined in wnzip.h
//
// This routine will check to see if the file name matches that of
// a known archive file.
//
// Returns: return TRUE if an archive file (zip/arc/pak), FALSE if not.
//
//===========================================================================

BOOL WINAPI FsvUtil_IsZipFile(LPCSTR lpszName, LPWORD lpwType)
{
    STATIC LPCSTR   lpszExtensions = "zip lzh lha"; // ##IPE

    LPSTR    lpFind  = NULL;
    INT      iExtLen = 0;
    WORD     wType   = ZIP_FORMAT_NONE;
    CHAR     szExt [ 4 ];

                                        // Get 3 character extension
    lpFind = STRRCHR( lpszName , '.' );

    if ( lpFind )
        {
        lpFind = CharNext( lpFind );    // Move past dot

        for ( iExtLen = 0 ; *lpFind && iExtLen < sizeof(szExt)-1 ; )
            {
            if ( IsDBCSLeadByte( *lpFind ) )
                {
                                        // Room to add 2nd char?
                if ( iExtLen >= sizeof(szExt) - 2 )     
                    break;
                else
                    szExt[iExtLen++] = *lpFind++;
                }

            szExt[iExtLen++] = *lpFind++;
            }

        szExt[iExtLen] = EOS;
        STRLWR( szExt );

        // MOREWORK: Use registry here and in NAVW32 to determine extensions to use
        //nBytes = Reg_GetProfileString( HKEY_CURRENT_USER, REGISTRY_FILE_SERVICES, "Archives", "zip arc pak lzh lha", lpBuffer, 4000);

                                        // Get list of extensions
        lpFind = STRSTR( lpszExtensions, szExt );
        }

                                        // Rule out bad finds
    if ( lpFind )
        {
                                        // Is it aligned at start of extension?
        if ( lpFind != lpszExtensions && 
             *CharPrev( lpszExtensions, lpFind ) != ' ' )
            lpFind = NULL;
                                        // Is it an "exact" match?
        else if ( lpFind[iExtLen] != ' ' && lpFind[iExtLen] != EOS )
            lpFind = NULL;
        }

                                        // Cheap parsing to get an idea
                                        // of archive type.
    if ( lpFind && lpwType )
        {
         // Default to ZIP format.
         wType = ZIP_FORMAT_ZIP;

         // Do quick and dirty check on the first letter, then check the
         // other letters.
         switch ( szExt[0] )
             {
             case 'a':
                 if ( szExt[1] == 'r' && szExt[2] == 'c' )
                     wType = ZIP_FORMAT_ARC;
                 break;
             case 'l':
                 if ( ( szExt[1] == 'z' && szExt[2] == 'h') ||
                      ( szExt[1] == 'h' && szExt[2] == 'a') )
                     wType = ZIP_FORMAT_LHA;
                 break;
             case 'p':
                 if ( szExt[1] == 'a' && szExt[2] == 'k' )
                     wType = ZIP_FORMAT_PAK;
                 break;
             case 'z':
             default:
                 wType = ZIP_FORMAT_ZIP;
                 break;
             }
         }

    if (lpwType)
        *lpwType = wType;

    return (lpFind != NULL);
}

//========================================================================
//
//  WORD WINAPI DOSWrap_5701_SetDateAndTime (
//      int  iFileHandle
//      DWORD   dwDateTime
//      );
//
//  Returns 0 if successfull, 06 if invalid file handle, 01 if no
//  access (file sharing).
//
LONG WINAPI DOSWrap_5701_SetDateAndTime (int iFileHandle,DWORD dwDateTime)
{
    FILETIME ftData;

    DosTimeToFileTime( HIWORD(dwDateTime), LOWORD(dwDateTime) , &ftData);

    if ( FileSetTime( iFileHandle, TIME_LASTWRITE , &ftData ) == NOERR &&
         FileSetTime( iFileHandle, TIME_LASTACCESS, &ftData ) == NOERR &&
         FileSetTime( iFileHandle, TIME_CREATION  , &ftData ) == NOERR )
        return FSV_SUCCESS;
    else 
        return -1L;                     // Assume no access
}

//==========================================================================
//
// LPFSVBUFIO_FBUF_INFO FsvBufIO_AllocInitFBufInfo( hFile, lplpFBuf, wBufSiz,
//                                                  lpfnCritErrFunc,
//                                                  dwCritErrData )
//
//    hFile           - The handle of the file that was opened or created.
//    wBufSiz         - The size of the buffer to allocate for buffered file
//                      i/o.  A value of zero implies that the i/o will NOT
//                      be buffered.
//    lplpFBuf        - Pointer to the pointer that will point to the file
//                      buffer info structure if the allocations were
//                      successful.
//    lpfnCritErrFunc - Pointer to the function to call if a critical error
//                      is detected.  This pointer can be NULL if the caller
//                      wants to ignore critical errors.
//    dwCritErrData   - The data that is to be passed to the critical error
//                      routine (usually a structure pointer).
//
// This function tries to allocate the file buffer info structure and an
// affiliate i/o buffer.  If it is successful then it will initialize
// the state info in the file buffer structure to point to the first
// byte in the file.
//
// Returns:
//    FSV_SUCCESS          - The operation was successful.
//    FSVERR_OUT_OF_MEMORY - Not enough memory for the dynamic data structures.
//
//==========================================================================

LONG WINAPI FsvBufIO_AllocInitFBufInfo(
   HFILE                  hFile,
   WORD                   wBufSiz,
   LPLPFSVBUFIO_FBUF_INFO lplpFBuf,
   ERROR_CALLBACK         lpfnCritErrFunc,
   DWORD                  dwCritErrData
)

{
    LPFSVBUFIO_FBUF_INFO lpFBuf;
    LONG                 lEOF;

    *lplpFBuf = NULL;

    /* Try to determine the size of the file. */
    if ( ( lEOF = FileSeek( hFile, 0L, SEEK_END ) ) >= 0 &&
         FileSeek( hFile, 0L, SEEK_SET ) == 0 )
        {
        if ( ( lpFBuf = 
               (LPFSVBUFIO_FBUF_INFO) MemAllocPtr( GHND, sizeof(FSVBUFIO_FBUF_INFO) ) ) == NULL )
            {
            lEOF = FSV_ERROR;           // Want to return FSV_OUT_OF_MEMORY   
            }
        else
            {
            lpFBuf->lpBuf = NULL;       // Not using a buffer for wrapper   

            /* Initialize affiliate state info. */
            lpFBuf->cDirty        = FALSE;
            lpFBuf->hFile         = hFile;
            lpFBuf->lStrtBufPos   = 0;
            lpFBuf->wBufSiz       = 0;
            lpFBuf->wBytsUsed     = 0;
            lpFBuf->wBufOffset    = 0;
            lpFBuf->lEOFPos       = lEOF;
            lpFBuf->lEOFCharPos   = lEOF;

            *lplpFBuf = lpFBuf;
            lEOF      = FSV_SUCCESS;
            }
        }

   return( lEOF );
} /* FsvBufIO_AllocInitFBufInfo */

#endif          // [Keep at end of file] This file is only used for NAV95

