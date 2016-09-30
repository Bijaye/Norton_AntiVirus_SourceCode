//************************************************************************
//
// $Header:   S:/navzip/VCS/navzip.cpv   1.0   06 Feb 1997 21:06:00   RFULLER  $
//
// Description:
//      Provides DX platform compatible zip interface to NAV using
//      Greenleaf libraries.  Replaces SYMZIP.
//
// Contains:
//      ZipProcessFile
//      ZipCleanup
//      ZipInit
//      extract_zip
//
//************************************************************************
// $Log:   S:/navzip/VCS/navzip.cpv  $
// 
//    Rev 1.0   06 Feb 1997 21:06:00   RFULLER
// Initial revision
// 
//    Rev 1.4   31 Jul 1996 15:31:36   JALLEE
// Clear attributes of temp files.
// 
//    Rev 1.3   17 Jul 1996 11:07:12   JALLEE
// Zip engine sends to scanner only filename, not full path of objects inside zip.
// 
//    Rev 1.2   01 Jul 1996 15:12:22   JALLEE
// Check for user requested abort during zip scanning.
// 
//    Rev 1.1   25 Jun 1996 19:30:08   MZAREMB
// Forced the name buffer to be upper case so that our "Program Files Extensions"
// list would match properly.
// 
//    Rev 1.0   24 Jun 1996 14:04:20   JALLEE
// Initial revision.
//************************************************************************
#include "platform.h"
#include "xapi.h"
#include "stddos.h"
#include "file.h"
#include "disk.h"
#include <direct.h>
#include "arclib.h"
#include "pkarc.h"
#include "symzip.h"
#include "mntr.h"


//************************************************************************
// LOCAL PROTOTYPES
//************************************************************************

void extract_zip(LPSTR lpszFileName, LPUNZIP_STRUCT lpUnZipStruct);

//************************************************************************
// DATA
//************************************************************************

char gszFileBuffer[SYM_MAX_PATH];

//************************************************************************
// ZipProcessFile()
//
// This routine handles scanning ( and excluding ) files within a zip
// archive.
//
// Parameters:
//      LPUNZIP_STRUCT lpUnzipStruct
//
// Returns:
//      NOERR
//      ERR
//************************************************************************
// 6/18/96 JALLEE created.
//************************************************************************

UINT SYM_EXPORT WINAPI ZipProcessFile (LPUNZIP_STRUCT lpUnZipStruct)
{
    FINDDATA            findData;
    HFIND               hFind;
    ZIPRECORD           ZipRecord;

    NameConvertFwdSlashes((LPSTR) (lpUnZipStruct->lpszZipFileName));

    hFind = FileFindFirstAttr(lpUnZipStruct->lpszZipFileName,
                              FA_NORMAL|FA_RDONLY|FA_HIDDEN,
                              &findData);

    if ( hFind == INVALID_HANDLE_VALUE )
    {
                                        // Specified zip file was not found!?
        ZipRecord.lpszBuffer = lpUnZipStruct->lpszZipFileName;

        if ( lpUnZipStruct->lpCallback )
            (lpUnZipStruct->lpCallback)(ZIP_ERROR, ENOT_FOUND, &ZipRecord,
                                             lpUnZipStruct->dwUserData);

        return (ERR);
    }

    extract_zip((LPSTR)findData.szFileName,
                lpUnZipStruct);

    FileFindClose(hFind);

    if ( lpUnZipStruct->lpCallback )
        (lpUnZipStruct->lpCallback)(ZIP_DONE, 0, NULL,
                                    lpUnZipStruct->dwUserData);

    return (NOERR);
}          

//************************************************************************
// ZipCleanup()
//
// This routine provided to maintain SYMZIP compatibility in SCAND.
//
// Parameters:
//      None
//
// Returns:
//      NOERR
//      ERR
//************************************************************************
// 6/18/96 JALLEE created.
//************************************************************************
UINT SYM_EXPORT WINAPI ZipCleanup(VOID)
{
    return(NOERR);
}

//************************************************************************
// ZipCleanup()
//
// This routine provided to maintain SYMZIP compatibility in SCAND.
//
// Parameters:
//      None
//
// Returns:
//      NOERR
//      ERR
//************************************************************************
// 6/18/96 JALLEE created.
//************************************************************************
UINT SYM_EXPORT WINAPI ZipInit (void)
{
    return(NOERR);
}

//************************************************************************
// extract_zip()
//
// Greenleaf specific code to extract, rename, scan, delete files in
// zip archives.  Instantiating the MyMonitor class provides progress
// callbacks.
//
// Parameters:
//      LPSTR          lpszFileName
//      LPUNZIP_STRUCT lpUnzipStruct
//
// Returns:
//      Nothing
//************************************************************************
// 6/18/96 JALLEE created.
//************************************************************************


void extract_zip(LPSTR lpszFileName, LPUNZIP_STRUCT lpUnZipStruct)
{
    HFILE       hFile;
    UINT        uRet;
    ZIPRECORD   ZipRecord;

    MEMSET(&ZipRecord,0,sizeof(ZIPRECORD));

    ALPkArchive archive( lpszFileName );
    MyMonitor monitor(lpUnZipStruct);

                                        // Declare list, use monitor as
                                        // callback and pkzip toolset.
    ALEntryList list( &monitor, PkDecompressTools() );

                                        // Read zip directory into list.
    archive.ReadDirectory( list );

    ALEntry *entry = list.GetFirstEntry();
    monitor.bAbort = FALSE;

                                        // Process all files in zip.
    while ( entry )
    {
                                        // All files selected by default
                                        // clear all, process one by one.
        list.ClearMarks(0);
        STRNCPY((LPSTR)gszFileBuffer, entry->mpStorageObject->mName.StripPath(), SYM_MAX_PATH);
        STRUPR((LPSTR)gszFileBuffer);
        ZipRecord.lpszBuffer = (LPSTR)gszFileBuffer;

                                        // Check for exclusions / program file.
                                        // Also set up dialogs and save the
                                        // real name of file inside the zip.
        if ( lpUnZipStruct->lpCallback )
            uRet = (lpUnZipStruct->lpCallback)(ZIP_FILESPEC,
                                               0,
                                               &ZipRecord,
                                               lpUnZipStruct->dwUserData);
        else
        {
            uRet = ZIP_EXTRACT;
        }

        if (uRet == ZIP_EXTRACT)
        {
                                        // Give the record a temp name
            *gszFileBuffer = EOS;

// &! Error checking recommended.
            hFile = FileCreateTemporary (gszFileBuffer, 0);
            if (hFile != HFILE_ERROR)
            {
                                        // We have the file open exclusively
                FileClose(hFile);
                                        // Apply the temp name
                entry->mpStorageObject->mName = gszFileBuffer;
                entry->mpStorageObject->mAttributes.SetFromPackedAttributes(0);
                entry->SetMark();
                                        // Extract the file.
                archive.Extract ( list );
                                        // The monitor will call ZipCallback()
                                        // in scanutil.c to perform the scan
                                        // when the file is extracted.
                                        // Time to delete the temp file.
                FileDelete(gszFileBuffer);
            }
        }
        else if (uRet == ZIP_ABORT)
        {
            break;
        }
                                        // Check if monitor has called for
                                        // an abort, (probably user generated.)
        if (monitor.bAbort)
            // Abort scanning this zip file.
            break;
        else
            // Go to the next entry in the zip file.
            entry = entry->GetNextEntry();
    }
}

