////////////////////////////////////////////////////////////
//-----------------------------------------------------------------------------
// $Header:   S:/QUARADD/VCS/quaradd.cpv   1.9   18 Jun 1998 18:12:52   RStanev  $
//-----------------------------------------------------------------------------
// $Log:   S:/QUARADD/VCS/quaradd.cpv  $
// 
//    Rev 1.9   18 Jun 1998 18:12:52   RStanev
// Fixed Unicode to Ansi string conversion.
//
//    Rev 1.8   16 Jun 1998 15:53:34   RStanev
// Fixed a problem with NT file name creation.
//
//    Rev 1.7   08 Jun 1998 19:38:10   RStanev
// Mo changes.
//
//    Rev 1.6   08 Jun 1998 18:57:30   RStanev
// Updated function comments.
//
//    Rev 1.5   08 Jun 1998 18:21:24   RStanev
// Fixed a typo.
//
//    Rev 1.4   08 Jun 1998 18:16:22   RStanev
// VxD string conversions done in the new CreateUniqueFile function.
//
//    Rev 1.3   08 Jun 1998 17:43:04   RStanev
// New AddFileToQuarantine() and AddFileToQuarantineComp().
//
//    Rev 1.2   28 May 1998 13:49:52   jtaylor
// Changed GetLocalTime back to GetSystemTime.
//
//    Rev 1.1   26 May 1998 14:36:22   jtaylor
// Changed from SystemTime to LocalTime when saving the time added to Quarantine
//
//    Rev 1.0   26 May 1998 14:27:42   DBuches
// Initial revision.
//
//    Rev 1.22   20 May 1998 21:57:32   jtaylor
// Enhanced to support long file extensions.
//
//    Rev 1.21   12 May 1998 17:31:08   DALLEE
// Added Status parameter to AddFileToQuarantine().
// Nice to know whether to add item as quarantined or backed up, etc.
//
//    Rev 1.20   28 Apr 1998 18:57:58   jtaylor
// Fixed a bug in CreateUniqueFile. (extension)
//
//    Rev 1.19   28 Apr 1998 02:16:08   DALLEE
// Argh! Added missing RegCloseKey() call in GetQuarantinePath().
// Wonder if this is why I just had to reboot 3 times for
// "Windows has detected a problem with your registry...".
//
//    Rev 1.18   27 Apr 1998 23:29:30   DALLEE
// In AddFileToQuarantine() - save time item found in Quarantine header.
// Fixed compiler error I introduced to SYM_WIN last revision.
//
//    Rev 1.17   26 Apr 1998 21:02:20   DALLEE
// JTAYLOR - corrected some character set conversions. More cleanup.
// DALLEE - Added file times. Even more cleanup.
//
//    Rev 1.16   26 Apr 1998 17:41:14   jtaylor
// Cleaned up the code.
//
//    Rev 1.15   24 Apr 1998 15:41:32   jtaylor
// fixed a typo.
//
//    Rev 1.13   24 Apr 1998 15:32:12   jtaylor
// Added code for converting between codepages in VXD
//
//    Rev 1.12   19 Apr 1998 21:49:52   JTaylor
// Enhanced file already exists error check in CreateUniqueFile.
//
//    Rev 1.11   09 Apr 1998 21:04:22   JTaylor
// Added support for VXD and NTK platforms.  Added a trace function for
// debugging calls from AP.
// Added TCHAR <-> CHAR translation functions
//
//    Rev 1.10   03 Apr 1998 18:56:54   JTaylor
// Added some support for compiling unicode and ascii.  Added ability to compile for kernel mode applications.
//
//    Rev 1.9   11 Mar 1998 18:53:02   JTaylor
// Removed the appid, changed the filestatus to a dword.
//
//    Rev 1.8   10 Mar 1998 18:16:28   SEDWARD
// Fixed indexing bug in CreateUniqueFile().
//
//    Rev 1.7   09 Mar 1998 22:34:08   SEDWARD
// Added 'GetQuarantinePath'.
//
//    Rev 1.6   04 Mar 1998 16:06:46   DBuches
// Populate more fields in the quarantine file header.
//
//    Rev 1.5   03 Mar 1998 00:36:56   JTaylor
// converted _tcs string functions to STR* functions.  Changed the random number generation for filenames.  Added the correct current filename to the header.
//
//    Rev 1.4   02 Mar 1998 18:10:52   JTaylor
// memset the whole header to zero, save the current filename in the header.
//
//    Rev 1.3   27 Feb 1998 01:24:12   jtaylor
// Added function headers, adding a return statement in AddFileToQuarantine, made the quarantine path able to not have a slash at the end.
//
//    Rev 1.2   26 Feb 1998 22:24:22   jtaylor
// Changed AddFileToQuarantine to take 2 arguments, Changed data types in AddFileToQuarantine to not be pointers.
//
//    Rev 1.1   26 Feb 1998 11:21:14   jtaylor
// Moved the function declarations to the h file.
//
//    Rev 1.0   26 Feb 1998 11:03:04   jtaylor
// Main file used to build the .lib for adding files to NAV Quarantine.
//
//-----------------------------------------------------------------------------
#include "platform.h"
#include "xapi.h"
#include "QuarAdd.h"

#ifdef SYM_WIN
    #include <stdio.h>
    #include <stdlib.h>
    #include <time.h>
    #include <string.h>
#else SYM_WIN
    #include "file.h"
    #include "symcfg.h"
#endif // SYM_WIN

static PSTR TranslateTcharToAnsiChar( PTSTR pszSourceAnsi );

#ifndef SYM_WIN

//-----------------------------------------------------------------------------
//
// void CreateUniqueFileName (
//     PTSTR pszFileName,
//     PTSTR pszExtension
//     );
//
// This function create a unique file given a file name and extension.
//
// Entry:
//      pszFileName  - base name of the file to be created.
//      pszExtension - extension of the file to be created.
//
// Exit:
//      pszFileName contains the name of the newly created file.  If this
//      function runs out of options, the filename that will be placed in
//      pszFileName may be the name of an already existing file.  This is a
//      huge limitation, but with the current architecture this is the best
//      that can be done.
//
//-----------------------------------------------------------------------------
void CreateUniqueFileName (
    PTSTR pszFileName,
    PTSTR pszExtension
    )
{
    auto int   iAppendNumber;
    auto int   iCounter;
    auto HFILE hFile;

    iAppendNumber = STRLEN ( pszFileName );

    iCounter = 0;

    do
    {
        _ConvertWordToString ( iCounter, &pszFileName[iAppendNumber], 10, 0 );

        if ( STRLEN ( pszExtension ) )
            {
            NameAppendExtension ( pszFileName, pszExtension );
            }

#if defined(SYM_VXD)

        auto PTSTR pszVxDConvert;

        if ( pszVxDConvert = (PTSTR) MemAllocPtr ( GHND, ( STRLEN ( pszFileName ) + 1 ) * sizeof(WCHAR) ) )
        {
            VxDGenericStringConvert ( pszFileName, CHARSET_OEM, pszVxDConvert, CHARSET_CURRENT );

            hFile = FileCreate ( pszVxDConvert, 0x80000000 | FILE_ATTRIBUTE_NORMAL );

            MemFreePtr ( pszVxDConvert );
        }
        else
        {
            hFile = HFILE_ERROR;

            iCounter = 1000;
        }

#elif defined(SYM_NTK)

        hFile = FileCreate ( pszFileName, 0x80000000 | FILE_ATTRIBUTE_NORMAL );

#else

        SYM_ASSERT ( FALSE ); // put your own create here.

#endif

    } while ( hFile == HFILE_ERROR && ( iCounter++ < 1000 ) );

    if ( hFile != HFILE_ERROR )
        FileClose ( hFile );
}

//-----------------------------------------------------------------------------
//
// BOOL AddFileToQuarantine (
//     PTSTR  pszSourceFileName,
//     DWORD  dwStatus,
//     PTSTR  pszQuarantinePath,
//     PTSTR  pszQuarFileName,
//     PHFILE phQuarFile
//     );
//
// This function creates a transition quarantine file.  AddFileToQuarantine()
// is only the first step in the process of creating a transition quarantine
// item.  This two-step process allows a client to create transition quarantine
// file, regain back control (which is when this function returns), do pretty
// much anything his/her heart desires, and then either finalize the quarantine
// item or cancel the quarantine process.  Note, that for SYM_VXD all strings
// are in OEM.
//
// Entry:
//      pszSourceFileName - name of the file to be quarantined.
//      dwStatus          - type of quarantine to be performed:
//                              QFILE_STATUS_QUARANTINED
//                              QFILE_STATUS_BACKUP_FILE
//      pszQuarantinePath - directory where the transition quarantine file is
//                          to be created.
//      pszQuarFileName   - pointer to a buffer to receive the name of the
//                          transition file created; the length of the buffer
//                          should be MAX_QUARANTINE_FILENAME_LEN.
//      phQuarFile        - pointer to an HFILE variable to receive the handle
//                          of the transition file.
//
// Exit:
//      TRUE if the function succeeded, FALSE if failed.  If this function
//      succeeded, the variables at pszQuarFileName and phQuarFile will contain
//      information needed by AddFileToQuarantineComp() to successfully
//      complete the quarantine process.  These two items should not be
//      used by the client!  Whenever AddFileToQuarantine() returns TRUE,
//      the client must call AddFileToQuarantineComp() to complete the process.
//      If this function returns FALSE, then the quarantine could not complete
//      and the client should not call AddFileToQuarantineComp().
//
//-----------------------------------------------------------------------------
BOOL AddFileToQuarantine (
    PTSTR  pszSourceFileName,
    DWORD  dwStatus,
    PTSTR  pszQuarantinePath,
    PTSTR  pszQuarFileName,
    PHFILE phQuarFile
    )
{
    auto BOOL                   bResult;
    auto DWORD                  dwHeaderLength;
    auto QFILE_AP_HEADER_STRUCT *prHeader;

    // Verify path input

    SYM_ASSERT ( pszSourceFileName );
    SYM_ASSERT ( *pszSourceFileName );
    SYM_ASSERT ( pszQuarantinePath );
    SYM_ASSERT ( *pszQuarantinePath );
    SYM_ASSERT ( pszQuarFileName );
    SYM_ASSERT ( phQuarFile );

    // These are the two allowable statuses currently. Please update assertion if more
    // legitimate AP quarantine statuses are added and this fires.

    SYM_ASSERT ( ( QFILE_STATUS_QUARANTINED == dwStatus ) ||
                 ( QFILE_STATUS_BACKUP_FILE == dwStatus ) );

    // Set default result as fail.  Upon successful completion, we will overwrite its value.

    bResult = FALSE;

    // Determine the size and allocate memory for the quarantine file header.

    dwHeaderLength = max ( sizeof(*prHeader), DEFAULT_HEADER_BUFFER_SIZE );

    if ( prHeader = (QFILE_AP_HEADER_STRUCT*) MemAllocPtr ( GHND, dwHeaderLength ) )
    {
        auto PSTR pszAnsiName;

        // Save the signature, version, and type information.

        prHeader->dwSignature = QUARANTINE_SIGNATURE;
        prHeader->dwVersion = QUARANTINE_AP_VERSION;
        prHeader->dwHeaderBytes = dwHeaderLength;
        prHeader->FileInfo.dwFileStatus = dwStatus;

        // Convert the source filename to ANSI and store it in the quarantine header.

        if ( pszAnsiName = TranslateTcharToAnsiChar ( pszSourceFileName ) )
        {
            auto PVOID pvBuffer;

            strcpy ( prHeader->FileInfo.szOriginalAnsiFilename, pszAnsiName );

            MemFreePtr ( pszAnsiName );

            // Build a unique filename "AP.[ext]" where [ext] is the file extension of
            // the file to be quarantined.

            if ( pvBuffer = MemAllocPtr ( GHND, max ( MAX_QUARANTINE_EXTENSION_LENGTH,
                                                      MAX_QUARANTINE_FILENAME_LEN ) * sizeof(WCHAR) )
               )
            {
                if ( NameReturnExtensionEx ( pszSourceFileName,
                                             (PTSTR) pvBuffer,
                                             MAX_QUARANTINE_EXTENSION_LENGTH ) == NOERR )
                {
                    auto HFILE hQuarFile;

                    STRCPY ( pszQuarFileName, pszQuarantinePath );
                    NameAppendFile ( pszQuarFileName, _T("AP") );

                    CreateUniqueFileName ( pszQuarFileName, (PTSTR) pvBuffer );

                    // Create a new transition quarantine file and open it exclusively.
#if defined(SYM_VXD)
                    VxDGenericStringConvert ( pszQuarFileName, CHARSET_OEM, pvBuffer, CHARSET_CURRENT );

                    hQuarFile = FileCreate ( (PTSTR) pvBuffer, FILE_ATTRIBUTE_NORMAL );

                    if ( hQuarFile != HFILE_ERROR )
                        {
                        FileClose ( hQuarFile );

                        hQuarFile = FileOpen ( (PTSTR) pvBuffer, OF_READWRITE | OF_SHARE_EXCLUSIVE );
                        }
#else
                    hQuarFile = FileCreate ( pszQuarFileName, FILE_ATTRIBUTE_NORMAL );

                    if ( hQuarFile != HFILE_ERROR )
                        {
                        FileClose ( hQuarFile );

                        hQuarFile = FileOpen ( pszQuarFileName, OF_READWRITE | OF_SHARE_EXCLUSIVE );
                        }
#endif
                    if ( hQuarFile != HFILE_ERROR )
                    {
                        // Convert the transition quarantine filename to ANSI and store it in
                        // the quarantine header.

                        if ( NameReturnFile ( pszQuarFileName, (PTSTR) pvBuffer ) == NOERR )
                        {
                            if ( pszAnsiName = TranslateTcharToAnsiChar ( (PTSTR) pvBuffer ) )
                            {
                                auto HFILE hSource;

                                strcpy ( prHeader->FileInfo.szCurrentFilename, pszAnsiName );

                                MemFreePtr ( pszAnsiName );

                                // Open the file to be quarantined.
#if defined(SYM_VXD)
                                VxDGenericStringConvert ( pszSourceFileName, CHARSET_OEM, pvBuffer, CHARSET_CURRENT );

                                hSource = FileOpen ( (PTSTR) pvBuffer, OF_READ );
#else
                                hSource = FileOpen ( pszSourceFileName, OF_READ );
#endif
                                if ( hSource != HFILE_ERROR )
                                {
                                    // Save off original file size.

                                    prHeader->FileInfo.dwOriginalFileSize = FileLength( hSource );

                                    // Save filetimes.

                                    auto FILETIME stFileTime;

                                    if ( -1 != FileGetTime ( hSource, TIME_CREATION, &stFileTime ) )
                                        prHeader->FileInfo.ftOriginalFileDateCreated = stFileTime;

                                    if ( -1 != FileGetTime ( hSource, TIME_LASTACCESS, &stFileTime ) )
                                        prHeader->FileInfo.ftOriginalFileDateAccessed = stFileTime;

                                    if ( -1 != FileGetTime ( hSource, TIME_LASTWRITE, &stFileTime ) )
                                        prHeader->FileInfo.ftOriginalFileDateWritten = stFileTime;

                                    // Save time item quarantined.

                                    GetSystemTime ( &prHeader->FileInfo.stDateQuarantined );

                                    // Write the header to the transition quarantine file.

                                    if ( FileWrite ( hQuarFile, prHeader, prHeader->dwHeaderBytes ) == prHeader->dwHeaderBytes )
                                    {
                                        auto PVOID pvXfrBuffer;

                                        // Copy the contents of the file to be quarantined into the transition file.

                                        if ( pvXfrBuffer = MemAllocPtr ( GHND, QUARANTINE_TRANSFER_BUFFER_SIZE ) )
                                        {
                                            auto UINT uBytesXfrd;

                                            while ( ( uBytesXfrd = FileRead ( hSource, pvXfrBuffer, QUARANTINE_TRANSFER_BUFFER_SIZE ) ) &&
                                                    uBytesXfrd == FileWrite ( hQuarFile, pvXfrBuffer, uBytesXfrd ) );

                                            // If the whole file was transferred, then we have completed everything as planned.

                                            bResult = FileLength ( hQuarFile ) == prHeader->dwHeaderBytes + prHeader->FileInfo.dwOriginalFileSize;

                                            MemFreePtr ( pvXfrBuffer );
                                        }
                                    }

                                    FileClose ( hSource );
                                }
                            }
                        }

                        if ( bResult )
                        {
                            *phQuarFile = hQuarFile;
                        }
                        else
                        {
                            FileClose ( hQuarFile );
#if defined(SYM_VXD)
                            VxDGenericStringConvert ( pszQuarFileName, CHARSET_OEM, pvBuffer, CHARSET_CURRENT );

                            FileDelete ( (PTSTR) pvBuffer );
#else
                            FileDelete ( pszQuarFileName );
#endif
                        }
                    }
                }

                MemFreePtr ( pvBuffer );
            }
        }

        MemFreePtr ( prHeader );
    }

    return bResult;
}

//-----------------------------------------------------------------------------
//
// static UINT FileSetTimeValid (
//      HFILE     hFile,
//      UINT      uType,
//      PFILETIME pFileTime
//      );
//
// This function calls FileSetTime() with exactly the same parameters that are
// given to it, but only if pFileTime is valid.  The structure is considered
// valid if at least one of its members is not 0.  If FileSetTime() was called,
// then its return value is passed back to the caller.  If FileSetTime() was
// not called, the return value is 0 (success).
//
//-----------------------------------------------------------------------------
static UINT FileSetTimeValid (
    HFILE     hFile,
    UINT      uType,
    PFILETIME pFileTime
    )
{
    auto PBYTE pbyScan;
    auto int iCount;

    for ( iCount = sizeof(*pFileTime), pbyScan = (PBYTE) pFileTime;
          iCount && !*pbyScan;
          iCount--, pbyScan++ );

    if ( iCount )
        return ( FileSetTime ( hFile, uType, pFileTime ) );

    return ( 0 );
}

//-----------------------------------------------------------------------------
//
// BOOL AddFileToQuarantineComp (
//     PTSTR pszSourceFileName,
//     PTSTR pszQuarFileName,
//     HFILE hQuarFile,
//     BOOL  bCancel
//     );
//
// This function completes the functionality of AddFileToQuarantine().
// Depending on the status of bCancel, the quarantine will either be processed
// or cancelled.  This function should be called only if AddFileToQuarantine()
// returned TRUE.  Note, that in SYM_VXD all string parameters are in OEM.
//
// Entry:
//      pszSourceFileName - name of the file to be quarantined; same as the
//                          pszSourceFileName parameter to
//                          AddFileToQuarantine().
//      pszQuarFileName   - same as the as pszQuarFileName parameter to
//                          AddFileToQuarantine().
//      hQuarFile         - the variable written to the phQuarFile parameter
//                          by AddFileToQuarantine().
//      bCancel           - FALSE if the quarantine process started by
//                          AddFileToQuarantine() should be completed.  TRUE
//                          if the process should be canceled, in which case
//                          the quarantined file is restored back to its
//                          original state as when AddFileToQuarantine() was
//                          called and the quarantine transition file is
//                          deleted.
//
// Exit:
//      TRUE if the function succeeded, FALSE if failed.  If this function
//      failed, the quarantine item created by AddFileToQuarantine() will be
//      removed, but the source file is not guaranteed to have been restored
//      successfully.
//
//-----------------------------------------------------------------------------
BOOL AddFileToQuarantineComp (
    PTSTR pszSourceFileName,
    PTSTR pszQuarFileName,
    HFILE hQuarFile,
    BOOL  bCancel
    )
{
    auto BOOL bResult;

    // Validate input parameters.

    SYM_ASSERT ( pszSourceFileName );
    SYM_VERIFY_STRING ( pszSourceFileName );
    SYM_ASSERT ( pszQuarFileName );
    SYM_VERIFY_STRING ( pszQuarFileName );
    SYM_ASSERT ( hQuarFile != HFILE_ERROR );

    // If the user has requested to cancel the quarantine, we will set the result
    // to TRUE only after we process a lot of the code below.  Otherwise, there
    // is no chance we can ever fail.

    bResult = !bCancel;

    // Check if the client has requested that we cancel the quarantine operation.
    // If so, then enter this big block of code to restore the original filename
    // and remove the quarantine transition file from the quarantine directory.

    if ( bCancel )
    {
        auto QFILE_AP_HEADER_STRUCT *prHeader;

        // Allocate memory for the quarantine file header.

        if ( prHeader = (QFILE_AP_HEADER_STRUCT*) MemAllocPtr ( GHND, sizeof(*prHeader) ) )
        {
            auto PVOID pvXfrBuffer;

            // Read the quarantine file header, validate it, and allocate memory for the
            // transfer of the quarantine transition file back to its original location.

            if ( FileSeek ( hQuarFile, 0L, SEEK_SET ) == 0L &&
                 FileRead ( hQuarFile, prHeader, sizeof(*prHeader) ) == sizeof(*prHeader) &&
                 FileLength ( hQuarFile ) == ( prHeader->dwHeaderBytes + prHeader->FileInfo.dwOriginalFileSize ) &&
                 FileSeek ( hQuarFile, prHeader->dwHeaderBytes, SEEK_SET ) == prHeader->dwHeaderBytes &&
                 ( pvXfrBuffer = MemAllocPtr ( GHND, max ( QUARANTINE_TRANSFER_BUFFER_SIZE,
                                                           MAX_QUARANTINE_FILENAME_LEN * sizeof(WCHAR) ) ) )
               )
            {
                auto HFILE hSource;

                // Create the file that we had initially quarantined.  If the file is still
                // there, we must only open it so that the ownership is preserved on volumes
                // with persistent ACLs.

#if defined(SYM_VXD)
                VxDGenericStringConvert ( pszSourceFileName, CHARSET_OEM, pvXfrBuffer, CHARSET_CURRENT );

                hSource = FileOpen ( (PTSTR)pvXfrBuffer, OF_WRITE | OF_SHARE_DENY_NONE );

                if ( hSource == HFILE_ERROR )
                    hSource = FileCreate ( (PTSTR)pvXfrBuffer, FILE_ATTRIBUTE_NORMAL );
#else
                hSource = FileOpen ( pszSourceFileName, OF_WRITE | OF_SHARE_DENY_NONE );

                if ( hSource == HFILE_ERROR )
                    hSource = FileCreate ( pszSourceFileName, FILE_ATTRIBUTE_NORMAL );
#endif
                if ( hSource != HFILE_ERROR )
                {
                    auto UINT uBytesXfrd;

                    // Restore the original file contents.

                    while ( ( uBytesXfrd = FileRead ( hQuarFile, pvXfrBuffer, QUARANTINE_TRANSFER_BUFFER_SIZE ) ) &&
                            uBytesXfrd == FileWrite ( hSource, pvXfrBuffer, uBytesXfrd ) );

                    // If the whole file was restored, truncate it at its current position,
                    // and restore its attributes.

                    if ( FileSeek ( hSource, 0, SEEK_CUR ) == prHeader->FileInfo.dwOriginalFileSize &&
                         FileWrite ( hSource, pvXfrBuffer, 0 ) == 0 &&
                         FileLength ( hSource ) == prHeader->FileInfo.dwOriginalFileSize &&
                         FileSetTimeValid ( hSource, TIME_CREATION, &prHeader->FileInfo.ftOriginalFileDateCreated ) != -1 &&
                         FileSetTimeValid ( hSource, TIME_LASTACCESS, &prHeader->FileInfo.ftOriginalFileDateAccessed ) != -1 &&
                         FileSetTimeValid ( hSource, TIME_LASTWRITE, &prHeader->FileInfo.ftOriginalFileDateWritten ) != -1
                       )
                    {
                        // We have done it all.

                        bResult = TRUE;
                    }

                        // Close the restored file.

                    FileClose ( hSource );
                }

                // Regardless of whether the code up there succeeded, delete the quarantine file.

                FileClose ( hQuarFile );

                hQuarFile = HFILE_ERROR;

#if defined(SYM_VXD)
                VxDGenericStringConvert ( pszQuarFileName, CHARSET_OEM, pvXfrBuffer, CHARSET_CURRENT );

                FileDelete ( (PTSTR)pvXfrBuffer );
#else
                FileDelete ( pszQuarFileName );
#endif
                MemFreePtr ( pvXfrBuffer );
            }

            MemFreePtr ( prHeader );
        }
    }

    // Close any outstanding open files.

    if ( hQuarFile != HFILE_ERROR )
        FileClose ( hQuarFile );

    return ( bResult );
}

#endif // !SYM_WIN


#ifdef SYM_WIN
////////////////////////////////////////////////////////////////////////////
// Function name    : CreateUniqueFile
//
// Description      : Creates a new file with a unique filename and the
//                    extension passed in.  The file is created in the
//                    TargetPath and the name is assigned to lpszFileName
//                    and a handle to the open file is returned.
//
// Return type      : HANDLE -- Handle of newly opened file.
//                      This value will be INVALID_HANDLE_VALUE on failure.
//
// Argument         :  LPTSTR lpszTargetPath -- Full path for the new file
//                     LPTSTR lpszExtension  -- Extension of new file 3 characters or less
//                     LPTSTR lpszFileName   -- [out] full path a and filename of the new file.
//
////////////////////////////////////////////////////////////////////////////
// 2/26/98 JTAYLOR - Function created / Header added.
// 4/19/98 JTAYLOR - Enhanced file already exists error check.
////////////////////////////////////////////////////////////////////////////
HANDLE CreateUniqueFile(LPTSTR          lpszTargetPath,
                        LPTSTR          lpszExtension,
                        LPTSTR          lpszFileName)
    {
    auto int            nPathLength;
    auto HANDLE         hFile = INVALID_HANDLE_VALUE;
    auto TCHAR          szGeneratedFileName[MAX_QUARANTINE_FILENAME_LEN];

    // Copy the destination path and make sure there is a slash on the end.
    nPathLength = STRLEN(lpszTargetPath);

    // seed the random number generator
    srand( (unsigned)time( NULL ) );

    while (INVALID_HANDLE_VALUE == hFile)
        {
        //Genereate a random number for the filename
        DWORD dwNumber = MAKELONG(rand(),rand());
        // Make sure that a DWORD stays defined as 32 bits.
        SPRINTF( szGeneratedFileName, _T("%.8X."), dwNumber & 0xFFFFFFFF );

        // Make sure the extension is will fit in the filename
        SYM_ASSERT( _tcslen( lpszExtension ) <= MAX_QUARANTINE_FILENAME_LEN - 9 );
        if( _tcslen( lpszExtension ) > MAX_QUARANTINE_FILENAME_LEN - 9 )
            {
            goto Exit_Function;
            }

        // Append the extension to the filename after the '.'
        STRCPY( &szGeneratedFileName[9], lpszExtension );

        // Copy the path to the beginning of the output buffer
        STRCPY(lpszFileName, lpszTargetPath);

        // Append the filename to the path.
        NameAppendFile( lpszFileName, szGeneratedFileName );

        // Attempt to create the new file.
        hFile = CreateFile(lpszFileName,
                           GENERIC_WRITE | GENERIC_READ,
                           0,
                           NULL,
                           CREATE_NEW,
                           FILE_ATTRIBUTE_NORMAL,
                           NULL);

        // If we were unable to create the file, loop only if the file could not
        // be created due to a filename collision.
        if( INVALID_HANDLE_VALUE == hFile )
            {
            // Find out why the file could not be created.
            DWORD dwLastError = GetLastError();

            // Loop again if the error was that the file already exists.
            // ERROR_ALREADY_EXISTS is what the API claimed is returned if the
            // file already exists, ERROR_FILE_EXISTS is what is actually
            // returned on my computer.  I have included them both, in case one
            // is pre IE 4.0.
            if( ( ERROR_ALREADY_EXISTS != dwLastError ) &&
                ( ERROR_FILE_EXISTS != dwLastError ) )
                {
                goto Exit_Function;
                }
            }
        else
            {
            return (hFile);
            }
        }

Exit_Function:
    return INVALID_HANDLE_VALUE;
    }
#endif // SYM_WIN

////////////////////////////////////////////////////////////////////////////
// Function name    : GetQuarantinePath
//
// Description      : This function retrieves the Quarantine paths from the
//                    registry.  In VXD mode, the only path that is retrievable
//                    is the Incoming path.
//
// Return type      : BOOL  TRUE  for success
//                          FALSE for failure
//
// Arguments        : LPTSTR lpszOutBuf - This is the buffer in which the
//                                        path will be stored. [OUT]
//                    DWORD  nBufSize   - This is the size of lpszOutBuf in
//                                        bytes.
//                    int    nTargetPathInfo - This is the number of the path
//                                             that should be retrieved.
//                                             This argument is ignored in VXD/NTK
//
////////////////////////////////////////////////////////////////////////////
// 3/09/98 SEDWARD - Function created / Header added.
// 4/26/98 JTAYLOR - Header updated.
////////////////////////////////////////////////////////////////////////////
BOOL GetQuarantinePath(LPTSTR          lpszOutBuf,
                       DWORD           nBufSize,
                       int             nTargetPathInfo)
    {
#ifdef SYM_WIN

    auto BOOL        bRetVal = TRUE;
    auto HKEY        hKey = NULL;
    auto long        lResult;
    auto LPTSTR      lpszValueName = NULL;

    // open the Quarantine path registry key
    lResult = RegOpenKeyEx( HKEY_LOCAL_MACHINE, REGKEY_QUARANTINE_PATH, 0, KEY_READ, &hKey);
    if (ERROR_SUCCESS != lResult)
        {
        bRetVal = FALSE;
        goto  Bail_Function;
        }

    // get the appropriate Quarantine path
    switch (nTargetPathInfo)
        {
        case   TARGET_QUARANTINE_PATH:
            lpszValueName = REGVAL_QUARANTINE_PATH_STR;
            break;

        case   TARGET_QUARANTINE_INCOMING_PATH:
            lpszValueName = REGVAL_QUARANTINE_INCOMING_PATH_STR;
            break;

        case   TARGET_QUARANTINE_TEMP_PATH:
            lpszValueName = REGVAL_QUARANTINE_TEMP_PATH_STR;
            break;

        default:
            lpszValueName = NULL;
            break;
        }

    if (NULL == *lpszValueName)
        {
        bRetVal = FALSE;
        goto  Bail_Function;
        }

    // get the desired path value
    auto    DWORD       regValueType;
    lResult = RegQueryValueEx( hKey, lpszValueName, 0, &regValueType, (LPBYTE)lpszOutBuf, &nBufSize );
    if ((ERROR_SUCCESS != lResult)  ||  (REG_SZ != regValueType))
        {
        bRetVal = FALSE;
        }

Bail_Function:
        if ( hKey )
            {
            RegCloseKey( hKey );
            }

        return (bRetVal);

#else // Not Defined SYM_WIN

    auto BOOL      bRetVal = FALSE;
    auto DWORD     dwRegKeyType;

    // setup a registry key to point to the Quarantine Incoming directory
    auto CFGREGKEY rRegKey = {
                             HKEY_LOCAL_MACHINE,
                             REGVAL_QUARANTINE_PRODUCT_NAME,
                             NULL,
                             REGVAL_QUARANTINE_PATHKEY,
                             NULL,
                             NULL,
                             REGVAL_QUARANTINE_INCOMING_PATH_STR,
                             NULL
                             };

    // read in the Quarantine Incoming directory
    if ( ConfigRegQueryValue ( &rRegKey,
                               NULL,
                               &dwRegKeyType,
                               (VOID *)lpszOutBuf,
                               &nBufSize ) == ERROR_SUCCESS )
        {

        if ( dwRegKeyType == REG_SZ )
            {
            bRetVal = TRUE;
            }
        }

    return bRetVal;
#endif // SYMWIN

    }  // end of "GetQuarantinePath"

////////////////////////////////////////////////////////////////////////////
// Function name    : TranslateTcharToAnsiChar
//
// Description      : This function translates from a platform's native string
//                    format into ANSI chars.  Any unmappable characters will
//                    be converted into '_'.
//                    The platforms native string formats are:
//                      VXD = OEM, NTK = UNICODE, W32 = ANSI
//
// Return type      : PSTR -  This is a char * that will be used to allocate
//                            a buffer (using MemAllocPtr) for the converted
//                            string in char format in the ANSI codepage.
//                            This memory needs to be deallocated by the
//                            calling function.  If the memory allocation or
//                            string conversion failed, the return value will
//                            be NULL.
//
// Arguments        : PTSTR pszSourceUnicode - This is the TCHAR string to
//                                             convert to char.
//
////////////////////////////////////////////////////////////////////////////
// 4/26/98 JTAYLOR - Header updated.
////////////////////////////////////////////////////////////////////////////
static PSTR TranslateTcharToAnsiChar (
    PTSTR pszSourceTchar
    )
    {
    auto    PSTR    pszResult;
    auto    UINT    uLength;

    // Validate input parameters.

    SYM_ASSERT ( pszSourceTchar );
    SYM_VERIFY_STRING ( pszSourceTchar );

    // Allocate memory for the converted string.

    uLength = ( STRLEN(pszSourceTchar) + 1 ) * sizeof(WCHAR);

    pszResult = (PSTR) MemAllocPtr ( GHND, uLength );

    if( NULL == pszResult )
        {
        return NULL;
        }

    // Convert the string to ANSI
#if defined(UNICODE)

    if ( WideCharToMultiByte ( CP_ACP,
                               0,
                               pszSourceTchar,
                               -1,
                               pszResult,
                               uLength,
                               NULL,
                               NULL ) == 0 )
        {
        MemFreePtr ( pszResult );
        return ( NULL );
        }

#elif defined(SYM_VXD)

    VxDGenericStringConvert ( pszSourceTchar,
                              CHARSET_OEM,
                              pszResult,
                              CHARSET_ANSI );

#else
    STRCPY( pszResult, pszSourceTchar );
#endif

    return ( pszResult );
    }


