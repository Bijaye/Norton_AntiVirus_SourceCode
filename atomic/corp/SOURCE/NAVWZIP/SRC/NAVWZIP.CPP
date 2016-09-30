//-----------------------------------------------------------------
//-----------------------------------------------------------------
//  ZipInit()
//  ZipProcessFile()
//  ZipCleanup()
//
//-----------------------------------------------------------------
// $Header:   S:/NAVWZIP/VCS/navwzip.cpv   1.3   11 Jun 1998 18:59:00   CEATON  $
//-----------------------------------------------------------------
// $Log:   S:/NAVWZIP/VCS/navwzip.cpv  $
// 
//    Rev 1.3   11 Jun 1998 18:59:00   CEATON
// Dynamically appends the extension of the extracted filename to the temporary
// filename used for scanning.
// 
//    Rev 1.2   11 Aug 1997 00:50:00   KSACKIN
// Removed the exception handler around the ZipProcessFile call.  We are now
// wrapping this one step higher in NavWork.  If this fails, we are going to
// completely unload the NavZip DLL, and reload it.  This is so we can have a
// little more confidence in the fact that we are going to be solid in the
// rest of our scanning.
//
//    Rev 1.0   06 Feb 1997 21:04:46   RFULLER
// Initial revision
//
//    Rev 1.1   11 Jul 1996 14:43:12   RHAYES
// Partial fix for STS # 58334. Now when we get an error extracting a file
// that is password protected, we pass a different status code so that
// scanning of non-protected files can continue.
//
//    Rev 1.0   30 Jan 1996 15:56:04   BARRY
// Initial revision.
//
//    Rev 1.0   30 Jan 1996 14:51:06   BARRY
// Initial revision.
//
//    Rev 1.9   28 May 1995 20:35:56   BARRY
// Don't extract directories and set file attribs to normal
//
//    Rev 1.8   28 May 1995 19:46:44   BARRY
// Added IsCompressedExtension (from N_SCAN.CPP in NAVWORK)
//
//    Rev 1.7   26 May 1995 00:52:30   BARRY
// Fixed temp file handling and other minor problems in the basic ZIP flow
//
//    Rev 1.6   18 May 1995 12:57:52   SZIADEH
// fixed scanning subdirs within compressed files.
//
//    Rev 1.5   17 May 1995 16:23:08   BARRY
// Fixed bogus assertion (value being compared wasn't initialized)
//
//    Rev 1.4   13 Mar 1995 22:02:44   SZIADEH
// reactivated zip stuff by uncommenting mratin's pile of valvita cheese :-)
//
//-----------------------------------------------------------------

#include "platform.h"
#include "xapi.h"
#include "file.h"
#include "wnfsv.h"
#include "wnzip.h"
#include "navwzip.h"

#ifdef SYM_WIN
HINSTANCE hInstance;
#endif  // #ifdef SYM_WIN

//-----------------------------------------------------------------
//                          LOCAL PROTOTYPES
//-----------------------------------------------------------------

ZIP_STATUS ZipRecordFill (
   LPZIP_FILE_INFO lpZipFileInfo,
   LPZIPRECORD lpZipRecord );

//-----------------------------------------------------------------
//-----------------------------------------------------------------

/*@API:*************************************************************************
@Declaration:

    ZIP_STATUS SYM_EXPORT WINAPI ZipProcessFile(
                                 LPUNZIP_STRUCT lpUnZipStruct );
@Description:
    This function attempts to process an Archived file by first
    opening the file, then obtaining the files in the archive,
    and processing each by calling the callback function provided
    in lpUnZipStruct.  This function communicates with the callback
    via action messages.

@Parameters:

    LPUNZIP_STRUCT lpUnZipStruct: has zip Archive file info
@Returns:
   ZIP_OK          on success
   enum ZIP_STATUS otherwise
*******************************************************************************/

ZIP_STATUS SYM_EXPORT WINAPI ZipProcessFile (
       LPUNZIP_STRUCT lpUnZipStruct )
{
    auto HGLOBAL hMem = NULL;
    auto HZIP   hArchvFile=NULL;
    auto UINT   uFileCount=0;
    auto UINT   uFileIndex;
    auto UINT   uAction;
    auto UINT   uResult;
    auto WORD   wBytesRead;
    auto ZIP_STATUS zReturn = ZIP_OK;

    auto LPZIP_FILE_INFO lpZipFileInfo;
                                        // Just path of temp file
    auto CHAR            szDestDir[SYM_MAX_PATH];
    auto CHAR            szDestFile[SYM_MAX_PATH];
    auto CHAR            szOriginalTempFile[SYM_MAX_PATH];
    auto ZIPRECORD       ZipRecord;
    auto CHAR            szName[SYM_MAX_PATH];
    auto int             nNameLen;
    auto LPSTR           lpszVirtualExt;

    *szDestDir = EOS;

    //-----------------------------------------------------------------
    // make sure a call back does exist
    //-----------------------------------------------------------------
    SYM_ASSERT (lpUnZipStruct->lpCallback);

    //-----------------------------------------------------------------
    // allocate work buffer
    //-----------------------------------------------------------------
    if ((hMem = MemAlloc(GHND | GMEM_SHARE, SIZE_BUFFER)) == NULL)
        return ( ZIP_MEMORYALLOC_ERROR );

    lpZipFileInfo = ( LPZIP_FILE_INFO ) MemLock( hMem );

    //-----------------------------------------------------------------
    // open zip file and build the internal list structure
    //-----------------------------------------------------------------
    if ( ZIP_NO_ERROR != ZipOpen( lpUnZipStruct->lpszZipFileName,
                                  &hArchvFile,
                                  ZIP_READONLY,
                                  NULL, NULL, NULL, NULL, NULL, NULL)
       )
        {
        hArchvFile = NULL;
        zReturn = ZIP_FILEOPEN_ERROR;
        goto go_cleanup;
        }

                                        // Only using one temp filename for
                                        // entire session
                                        // (Borrow szDestFile)
                                        // NOTE: Only works on W32 (sorry)!
    if ( GetTempPath( sizeof(szDestFile), szDestFile ) == 0 ||
         GetTempFileName( szDestFile, "NAV", 0, szDestDir ) == 0 ) // ##IPE
        {
        *szDestFile = EOS;
        zReturn = ZIP_FILEOPEN_ERROR;   //&? Should create new enum...
        goto go_cleanup;
        }

    NameReturnFile( szDestDir, szName );
    NameStripFile( szDestDir );

    STRCPY( szOriginalTempFile,szName );

    //-----------------------------------------------------------------
    // get number of files in the archive
    //-----------------------------------------------------------------
    uResult = ZipGetInfo ( hArchvFile, ZIP_FILECOUNT, (LPBYTE) &uFileCount,
                                           sizeof(UINT), &wBytesRead );

    if ( ZIP_NO_ERROR != uResult || wBytesRead <= 0 )
        {
        zReturn = ZIP_FILEREAD_ERROR;
        goto go_cleanup;
        }

    //-----------------------------------------------------------------
    // process files in the archive
    //-----------------------------------------------------------------
    for ( uFileIndex = 0;
          (uFileIndex < uFileCount) && ( uAction != (UINT) ZIP_ABORT );
          uFileIndex++ )
        {

        //-----------------------------------------------------------------
        // get compressed file information - all of it
        //-----------------------------------------------------------------
        uResult = ZipGetFileInfo ( hArchvFile, NULL, uFileIndex, ZIP_ALLINFO,
                                    lpZipFileInfo, SIZE_BUFFER, &wBytesRead );

        // MOREWORK:  IF dir, skip over it
        if (ZIP_NO_ERROR == uResult)
           {
           if ( lpZipFileInfo->wFileAttrs & FSV_ATTRIB_DIRECTORY )
               continue;                // Skip directories

           uAction = ZIP_FILESPEC;
           }
        else
           uAction = ZIP_ERROR;

        ZipRecordFill ( lpZipFileInfo, &ZipRecord );


                                        // In this case, must point to the
                                        // name of the compressed filename
                                        // so exclusions, etc. can be checked.
        NameAppendFile( szDestDir, lpZipFileInfo->szName );
        ZipRecord.lpszBuffer = szDestDir;

        //-----------------------------------------------------------------
        // let the callback function know we have a file name
        //-----------------------------------------------------------------
        uAction = (lpUnZipStruct->lpCallback) ( uAction, uFileIndex, &ZipRecord,
                                                lpUnZipStruct->dwUserData);

        NameStripFile( szDestDir );

        // Copy the actual temporary file name without the extension

        NameStripExtension( szName );

        // Get the length of the name so we know where to put the
        //  extension and where to remove it from

        nNameLen = strlen(szName);

        // Get the extension of the virtual file name

        lpszVirtualExt = strrchr(lpZipFileInfo->szName,'.');

        // Append the extension, if any, to the actual temporary file name
        //  Don't add the extension if the name length will be exceeded

        if (lpszVirtualExt != NULL &&
            (nNameLen + strlen(lpszVirtualExt)) < SYM_MAX_PATH)
        {
            // Append the virtual extention to the actual temporary file name

            strcpy(szName+nNameLen,lpszVirtualExt);

        }

        // Set the new name

        strcpy( szDestFile,szName );

        switch ( uAction )
            {
            case ZIP_EXTRACT:
                uResult = ZipExtractFile( hArchvFile,
                                          NULL, //lpZipFileInfo->szName,
                                          uFileIndex,
                                          szDestDir,
                                          szDestFile,
                                          FALSE,
                                          NULL );
                if ( uResult == ZIP_NO_ERROR )
                    {
                    uAction = ZIP_EXTRACTED;
                    }
                else
                    {
                                        // If file is password protected
                                        // (encrypted), then report but
                                        // allow scanning to continue.
                    if( uResult == ZIP_EN_FILE_NO_KEY )
                        uAction = ZIP_PASSWORD;
                    else
                        uAction = ZIP_ERROR;
                    }

                                        // In this case, must point to the
                                        // actual extracted filename so we
                                        // can scan it (i.e., the temp
                                        // filename)
                NameAppendFile(szDestDir, szDestFile);
                ZipRecord.lpszBuffer = szDestDir;

                                        // Always create a normal file
                FileSetAttr( ZipRecord.lpszBuffer, FA_NORMAL );

                uAction = (lpUnZipStruct->lpCallback) ( uAction, uFileIndex, &ZipRecord,
                                                        lpUnZipStruct->dwUserData);



                FileDelete( ZipRecord.lpszBuffer );

                NameStripFile( szDestDir );
                break;

            case ZIP_ABORT:
            case ZIP_SKIP:
                continue;

            default:
                uAction = (UINT) ZIP_ABORT;
                break;
            }
        } // end for loop

go_cleanup:
    if ( *szDestDir )                   // Clean-up temp file
        {
        NameAppendFile( szDestDir, szOriginalTempFile );
        FileDelete( szDestDir );
        }

    if ( hArchvFile )                   // Close archive file
        {
        if ( ZIP_NO_ERROR != ZipClose( hArchvFile ) && zReturn == ZIP_OK )
            zReturn = ZIP_FILECLOSE_ERROR;
        }

    if ( hMem )                         // Free memory
        MemFree ( hMem );

                                        // Say we're leaving...
    (lpUnZipStruct->lpCallback) ( ZIP_DONE, 0, NULL,
                                  lpUnZipStruct->dwUserData);

    return zReturn;
}



/*@API:*************************************************************************
@Declaration:
@Description:
@Parameters:
@Returns:
*******************************************************************************/
ZIP_STATUS SYM_EXPORT WINAPI ZipInit()
{
    return (ZIP_OK );
}

/*@API:*************************************************************************
@Declaration:
@Description:
@Parameters:
@Returns:
*******************************************************************************/
ZIP_STATUS SYM_EXPORT WINAPI ZipCleanup ()
{
    return ( ZIP_OK );
}

/*@API:*************************************************************************
@Declaration:
BOOL SYM_EXPORT WINAPI IsCompressedExtension ( // Check if extension is a compressed
    LPSTR   lpszFilename                      // [in] filename
    );

@Description:
This function checks if the provided filename has an extension that
corresponds to a valid compressed file extension.

@Parameters:
$lpszExt$ pointer to filename to check

@Returns:
TRUE if the file is one of the supported program file types.

@Compatibility: Win16 and Win32
*******************************************************************************/

BOOL SYM_EXPORT WINAPI IsCompressedExtension ( LPSTR lpszFilename )
{
    return FsvUtil_IsZipFile( lpszFilename, NULL );
}

///////////////////////////////////////////////////////////////////
//                     LOCAL FUNCTIONS
///////////////////////////////////////////////////////////////////

ZIP_STATUS  ZipRecordFill (
   LPZIP_FILE_INFO lpZipFileInfo,
   LPZIPRECORD lpZipRecord )
{
    //DWORD  dwDate;
    //DWORD  dwTime;

    // ZIP_DATE_TYPE diDate The date and time of associated
    // ZIP_TIME_TYPE tiTime  with the "original" file.

    // dwDate = (DWORD) lpZipFileInfo->diDate;

    lpZipRecord->wPathTag     = 0;
    lpZipRecord->dwSig        = 0;
    lpZipRecord->wCreat_Ver   = 0;
    lpZipRecord->wVer_2_ext   = 0;
    lpZipRecord->wflag_word   = 0;

    lpZipRecord->wComp_Type   = (WORD)  lpZipFileInfo->ePckTyp;
    lpZipRecord->dwDate       = 0;   // don't know the format
    lpZipRecord->dwCrc        = (DWORD) lpZipFileInfo->lCRC;
    lpZipRecord->dwComp_Size  = (DWORD) lpZipFileInfo->lComprsdSiz;
    lpZipRecord->dwUncomp_Size= (DWORD) lpZipFileInfo->lOrigSiz;
    lpZipRecord->wFnlen       = 0;
    lpZipRecord->wExtra_len   = 0;
    lpZipRecord->wComment_len = 0;
    lpZipRecord->wStart_disk  = 0;
    lpZipRecord->wI_attr      = (WORD)   lpZipFileInfo->wFileAttrs;
    lpZipRecord->dwE_attr     = 0;
    lpZipRecord->dwLocal_off  = (WORD)  lpZipFileInfo->lCompDataOffset;

    // STRCPY ( lpZipRecord->szZipFileName, lpZipFileInfo->szName );
    // STRCPY ( lpZipRecord->szPath, lpZipFileInfo->szPath );

    return ( ZIP_OK );
}

//------------------------------------------------------------------------
// WINDOWS ROUTINES
//------------------------------------------------------------------------

#if defined(SYM_WIN32)

/****************************************************************************
   FUNCTION: DllMain

   PURPOSE:  Called when a new process is either using or not using
             this DLL anymore.

*******************************************************************************/
BOOL WINAPI DllMain(HINSTANCE hInstDLL, DWORD dwReason, LPVOID lpvReserved)
{
    hInstance = hInstDLL;
    return(TRUE);
}

#elif defined(SYM_WIN)

//************************************************************************
// LibMain()
//
// Parameters:
//
// Returns:
//
//************************************************************************
// 3/\93 Martin created.
//************************************************************************

int WINAPI LibMain (HINSTANCE hInst, WORD wDataSeg, WORD wHeapSize,
                    LPSTR lpszCmdLine)
{
    if (wHeapSize != 0)
        UnlockData(0);

    hInstance = hInst;

    return (1);
}

//************************************************************************
// WEP()
//
// Parameters:
//
// Returns:
//
//************************************************************************
// 3/\93 Martin created.
//************************************************************************

int SYM_EXPORT WINAPI WEP (int bSystemExit)
{
    return(1);
}

#endif
