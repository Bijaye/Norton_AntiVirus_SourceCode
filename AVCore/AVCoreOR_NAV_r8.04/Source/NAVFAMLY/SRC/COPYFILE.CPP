#include "platform.h"
#include "xapi.h"
#include "file.h"
#include "copyfile.h"


// --------------------------------------------------------------------------
//                      LOCAL FUNCTION PROTOTYPES
// --------------------------------------------------------------------------

COPYERR LOCAL InitStruct (              // Initialize our structure
    LPCOPYPRIVATE lpCopyPriv );         // [in] pointer to private struct

COPYERR LOCAL CopyFilesExists (
    LPCOPYPRIVATE lpCopyPriv );         // [in] pointer to private struct

COPYERR LOCAL CheckMedia (
    LPCOPYPRIVATE lpCopyPriv );         // [in] pointer to private struct

COPYERR LOCAL CopyAllFiles (
    LPCOPYPRIVATE lpCopyPriv );         // [in] pointer to private struct

// --------------------------------------------------------------------------
//      (C) Copyright Symantec Corporation, Peter Norton Group  1994
// --------------------------------------------------------------------------
//
// Function: CopyFiles()
//
// Author: Martin Fallenstedt           Date: 07/29/94
//
// Description: This function copies the virus definitions from the specified
//              path to the target path. If callback function is provided it
//              will be called to indicate progress of copy.
//
// Input:   lpCopyStruct,   pointer to copy structure. See copyfile.h for info
//
// Output:  look at the header file copyfile.h for possible COPYERR_ return
//          codes and their description.
//
// --------------------------------------------------------------------------
// Update History:
// MARTIN,  04/29/94 Function created.
// --------------------------------------------------------------------------

COPYERR SYM_EXPORT WINAPI CopyFiles (   // Copy files
    LPCOPYSTRUCT    lpCopy )            // [in] pointer to copy structure
{
    auto    COPYERR     Status = COPYERR_OK;
    auto    COPYPRIVATE CopyPriv = { 0 };

                                        // ----------------------------------
                                        // Check for application errors
                                        // ----------------------------------
    SYM_ASSERT ( lpCopy );
    SYM_ASSERT ( lpCopy->lpszFileArray );
    SYM_ASSERT ( *lpCopy->szCopyTo );
    SYM_ASSERT ( *lpCopy->szCopyFrom );

    CopyPriv.lpCopy = lpCopy;
                                        // ----------------------------------
                                        // Stay in the loop while there are
                                        // tasks to be done and while the
                                        // status indicates success.
                                        // ----------------------------------
    BOOL bDone = FALSE;
    int  nDoStep = 0;

    while ( !bDone && Status == COPYERR_OK )
        {
        switch ( nDoStep )
            {
            case 0:
                                        // ----------------------------------
                                        // Initialize structure
                                        // ----------------------------------
                Status = InitStruct ( &CopyPriv );
                nDoStep++;
                break;

            case 1:
                                        // ----------------------------------
                                        // If files on the target are write
                                        // protected, change their attribute
                                        // and make sure we can write to media
                                        // ----------------------------------
                Status = CheckMedia ( &CopyPriv );
                nDoStep++;
                break;

            case 2:
                                        // ----------------------------------
                                        // Check if the files on the target
                                        // and destination exists. If they
                                        // exists on the target then it's an
                                        // error.
                                        // ----------------------------------
                Status = CopyFilesExists ( &CopyPriv );
                nDoStep++;
                break;

            case 3:
                                        // ----------------------------------
                                        // Copy all the files.
                                        // ----------------------------------
                Status = CopyAllFiles ( &CopyPriv );
                bDone = TRUE;
                break;

            default:
                SYM_ASSERT ( 0 );
                Status = COPYERR_INTERNAL;
                break;
            }
        }

    return ( Status );
}

// --------------------------------------------------------------------------
// --------------------------------------------------------------------------

COPYERR LOCAL InitStruct (              // Initialize our structure
    LPCOPYPRIVATE lpCopyPriv )          // [in] pointer to private struct
{
    auto    COPYERR     Status = COPYERR_OK;
    auto    LPTSTR      lpFile;

    SYM_ASSERT ( lpCopyPriv );
    SYM_ASSERT ( lpCopyPriv->lpCopy->lpszFileArray );

    lpFile = lpCopyPriv->lpCopy->lpszFileArray;

    while ( *lpFile )
        {
        lpCopyPriv->uNumFilesToCopy ++;

        lpFile += ( STRLEN ( lpFile ) + 1 );
        }

    lpCopyPriv->dwTotalBytes = 0;
    lpCopyPriv->dwBytesCopied = 0;
    lpCopyPriv->lpCopy->uFilesCopied = 0;

    return ( Status );
}

// --------------------------------------------------------------------------
// --------------------------------------------------------------------------

COPYERR LOCAL CopyFilesExists (
    LPCOPYPRIVATE lpCopyPriv )          // [in] pointer to private struct
{
    auto    COPYERR     Status = COPYERR_OK;
    auto    LPTSTR      lpFile;

    SYM_ASSERT ( lpCopyPriv );
    SYM_ASSERT ( lpCopyPriv->lpCopy->lpszFileArray );

    lpFile = lpCopyPriv->lpCopy->lpszFileArray;

    if ( NameIsDirectory ( lpCopyPriv->lpCopy->szCopyTo ) == FALSE )
        {
        Status = COPYERR_PARAMS;
        }

    while ( *lpFile && Status == COPYERR_OK )
        {
        TCHAR   szFullPath [ SYM_MAX_PATH + 1] = { 0 };
        TCHAR   szFileName [ SYM_MAX_PATH + 1] = { 0 };

        STRCPY ( szFullPath, lpCopyPriv->lpCopy->szCopyFrom );
        NameReturnFileEx ( lpFile, szFileName );
        SYM_ASSERT ( *szFileName );
        NameAppendFile ( szFullPath, szFileName );

        HFILE hFile = FileOpen ( szFullPath, READ_ONLY_FILE );

                                        // ------------------------------------
                                        // Find out how many bytes to copy
                                        // between all the files.
                                        // ------------------------------------
        if ( hFile == HFILE_ERROR )
            Status = COPYERR_FILEOPEN;
        else
            {
            lpCopyPriv->dwTotalBytes += FileLength ( hFile );

            FileClose ( hFile );
            }

        lpFile += ( STRLEN ( lpFile ) + 1 );
        }


    return ( Status );
}

// --------------------------------------------------------------------------
// --------------------------------------------------------------------------

COPYERR LOCAL CheckMedia (
    LPCOPYPRIVATE lpCopyPriv )          // [in] pointer to private struct
{
    auto    COPYERR     Status = COPYERR_OK;
    auto    TCHAR       szFullPath [ SYM_MAX_PATH + 1] = { 0 };

    SYM_ASSERT ( lpCopyPriv );
    SYM_ASSERT ( lpCopyPriv->lpCopy->lpszFileArray );

    STRCPY ( szFullPath, _T("X:\\") );
    *szFullPath = *lpCopyPriv->lpCopy->szCopyTo;

#ifdef SYM_WIN32
    int nMagic = GetTempFileName ( szFullPath, "", 0, szFullPath );
#else
    int nMagic = GetTempFileName ( *szFullPath, "", 0, szFullPath );
#endif

    if ( nMagic )
        {
        HFILE hFile = FileCreate ( szFullPath, 0 );

        if ( hFile )
            {
            FileClose ( hFile );
            FileDelete ( szFullPath );

            if ( !(lpCopyPriv->lpCopy->wFlags & COPYFLAG_OVERWRITEIFEXISTS) )
                {
                                        // ----------------------------------
                                        // Make sure that none of the files
                                        // the user would like to copy exists
                                        // on the target drive.
                                        // ----------------------------------

                for ( LPTSTR lpFile = lpCopyPriv->lpCopy->lpszFileArray;
                      *lpFile && Status == COPYERR_OK;
                      lpFile += ( STRLEN ( lpFile ) + 1 ) )
                    {
                    TCHAR   szFileName [ SYM_MAX_PATH + 1] = { 0 };

                    STRCPY ( szFullPath, lpCopyPriv->lpCopy->szCopyTo );
                    NameReturnFileEx ( lpFile, szFileName );
                    SYM_ASSERT ( *szFileName );
                    NameAppendFile ( szFullPath, szFileName );

                    if ( FileExists ( szFullPath ) )
                        {
                        Status = COPYERR_EXISTS;
                        }
                    }
                }
             }
        else
            {
            Status = COPYERR_RO;        // Target media is read only
            }
        }
    else
        Status = COPYERR_INTERNAL;

    return ( Status );
}

// --------------------------------------------------------------------------
// --------------------------------------------------------------------------

COPYERR LOCAL CopyAllFiles (
    LPCOPYPRIVATE lpCopyPriv )          // [in] pointer to private struct
{
    auto    COPYERR     Status = COPYERR_OK;
    auto    LPTSTR      lpBuffer;
    const   UINT        uSizeBuffer = 10240;

    SYM_ASSERT ( lpCopyPriv );
    SYM_ASSERT ( lpCopyPriv->lpCopy->lpszFileArray );

    lpBuffer = (LPTSTR)MemAllocPtr ( GHND, uSizeBuffer );

    if ( lpBuffer )
        {
        LPTSTR lpFile = lpCopyPriv->lpCopy->lpszFileArray;
                                        // ----------------------------------
                                        // Copy each file.
                                        // ----------------------------------
        for ( UINT uIndex = 0;
              *lpFile && Status == COPYERR_OK;
              lpFile += ( STRLEN ( lpFile ) + 1 ), uIndex++ )
            {
            TCHAR   szTarget    [ SYM_MAX_PATH + 1] = { 0 };
            TCHAR   szSource    [ SYM_MAX_PATH + 1] = { 0 };
            TCHAR   szFileName  [ SYM_MAX_PATH + 1] = { 0 };

                                        // ------------------------------------
                                        // Create target file.
                                        // ------------------------------------
            STRCPY ( szTarget, lpCopyPriv->lpCopy->szCopyTo );
            NameReturnFileEx ( lpFile, szFileName );
            SYM_ASSERT ( *szFileName );
            NameAppendFile ( szTarget, szFileName );

            HFILE hTarget = FileCreate ( szTarget, 0 );

                                        // ------------------------------------
                                        // Open source file for read only.
                                        // ------------------------------------

            STRCPY ( szSource, lpCopyPriv->lpCopy->szCopyFrom );
            NameReturnFileEx ( lpFile, szFileName );
            SYM_ASSERT ( *szFileName );
            NameAppendFile ( szSource, szFileName );

            HFILE hSource = FileOpen ( szSource, READ_ONLY_FILE );

                                        // ------------------------------------
                                        // By this point we should have changed
                                        // the attribute and made sure the media
                                        // isn't read-only.
                                        // ------------------------------------
            if ( hTarget == HFILE_ERROR || hSource == HFILE_ERROR )
                Status = COPYERR_FILEOPEN;
            else
                {
                DWORD   dwBytes;
                DWORD   dwFileSize = FileLength ( hSource );
                DWORD   dwBytesLeft = dwFileSize;

                                        // ------------------------------------
                                        // Stay in loop while there are bytes
                                        // to copy and status is OK.
                                        // ------------------------------------
                while ( dwBytesLeft && Status == COPYERR_OK )
                    {
                    if ( dwBytesLeft > uSizeBuffer )
                        dwBytes = uSizeBuffer;
                    else
                        dwBytes = dwBytesLeft;

                    if ( FileRead ( hSource, lpBuffer, (UINT)dwBytes ) == dwBytes )
                        {
                        if ( FileWrite ( hTarget, lpBuffer, (UINT)dwBytes ) == dwBytes )
                            {
                            dwBytesLeft -= dwBytes;
                            lpCopyPriv->dwBytesCopied += dwBytes;

                            if ( lpCopyPriv->lpCopy->lpfnDefCallback )
                                {
                                DWORD dwFilePercent = ((dwFileSize - dwBytesLeft)
                                                        * 100 ) / dwFileSize;
                                SYM_ASSERT ( dwFilePercent <= 100 );

                                DWORD dwTotalPercent = (lpCopyPriv->dwBytesCopied
                                                        * 100 ) / 
                                                        lpCopyPriv->dwTotalBytes;
                                SYM_ASSERT ( dwTotalPercent <= 100 );

                                BOOL bContinue = lpCopyPriv->lpCopy->lpfnDefCallback (
                                                   uIndex,
                                                   lpCopyPriv->uNumFilesToCopy,
                                                   szFileName,
                                                   (UINT)dwFilePercent,
                                                   (UINT)dwTotalPercent,
                                                   lpCopyPriv->lpCopy->dwCookie
                                                   );

                                if ( bContinue == FALSE )
                                    Status = COPYERR_HALTED;
                                }
                            }
                        else
                            Status = COPYERR_WRITE;
                        }
                    else
                        Status = COPYERR_READ;
                    }
                }

            if ( hTarget )
                {
                FileClose ( hTarget );
                hTarget = NULL;
                }
            
            if ( hSource )
                {
                FileClose ( hSource );
                hSource = NULL;
                }

            if ( Status != COPYERR_OK )
                {
                FileDelete ( szTarget );
                }
            else
                {
                lpCopyPriv->lpCopy->uFilesCopied ++;
                }
            }

        MemFreePtr ( lpBuffer );
        }
    else
        {
        Status = COPYERR_MEMALLOC;
        }

    return ( Status );
}


