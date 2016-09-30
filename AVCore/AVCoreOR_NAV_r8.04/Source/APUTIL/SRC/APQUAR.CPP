#include "platform.h"
#include "xapi.h"
#include "apquar.h"
#include "file.h"

static PSTR TranslateTcharToAnsiChar( PTSTR pszSourceAnsi );

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

    auto PWSTR pszTest;

    pszTest = (PWSTR) MemAllocPtr ( GHND, uLength );

    if ( !pszTest )
    {
        MemFreePtr ( pszResult );
        return NULL;
    }

    if ( WideCharToMultiByte ( CP_ACP,
                               0,
                               pszSourceTchar,
                               -1,
                               pszResult,
                               uLength,
                               NULL,
                               NULL ) == 0 )
        {
        MemFreePtr ( pszTest );
        MemFreePtr ( pszResult );
        return ( NULL );
        }

    if ( MultiByteToWideChar ( CP_ACP,
                               0,
                               pszResult,
                               -1,
                               pszTest,
                               uLength ) == 0 )
        {
        MemFreePtr ( pszTest );
        MemFreePtr ( pszResult );
        return ( NULL );
        }

    if ( wcscmp ( pszTest, pszSourceTchar ) )
        {
        MemFreePtr ( pszTest );
        MemFreePtr ( pszResult );
        return ( NULL );
        }

    MemFreePtr ( pszTest );

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
