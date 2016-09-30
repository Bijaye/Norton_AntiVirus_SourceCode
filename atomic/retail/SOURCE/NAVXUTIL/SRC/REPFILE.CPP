// Copyright 1993 Symantec, Peter Norton Product Group
//************************************************************************
//
// $Header:   S:/NAVXUTIL/VCS/repfile.cpv   1.13   24 Jun 1998 12:36:06   jtaylor  $
//
// Description:
//      Repair file (either Inoc or Virus repair) and create backups.
//
// Contains:
//      RepairFile()
//      RestoreFile()
//
// See Also:
//************************************************************************
// $Log:   S:/NAVXUTIL/VCS/repfile.cpv  $
// 
//    Rev 1.13   24 Jun 1998 12:36:06   jtaylor
// Moved the personality dll include file to the Windows only section.
//
//    Rev 1.12   24 Jun 1998 12:32:30   jtaylor
// Removed the n32behav.h file for dos platforms.
//
//    Rev 1.11   23 Jun 1998 21:03:30   jtaylor
// Changed OEM versions to use .vir files.
//
//    Rev 1.10   18 Jun 1998 09:39:42   mdunn
// Bracketed Chuck's changes with #ifdef SYM_WIN32's.
//
//    Rev 1.9   11 Jun 1998 18:17:30   CEATON
// Added code to turn off file sharing before and after file repair to get around
// slowdown when repairing files on Netware servers.
//
//    Rev 1.8   19 May 1998 22:52:14   tcashin
// Ifdef the quarantine COM stuff for Win32. Changed a repair file data
// structure element to the right thing for the DVX platform.
//
//    Rev 1.7   18 May 1998 21:34:46   tcashin
// Changed RestoreFile() to pass a pointer to a REPAIFILESTRUCT. Added the code
// to get the backup file out of quarantine. Fixed some ANSI/OEM problems.
//
//    Rev 1.6   26 Aug 1997 20:21:36   DALLEE
// In RestoreFile() now using local function to zero and truncate the
// failed repair file w/o deleting as in FileWipe().
// My testing was bad for the last fix -- last rev's code had the scan
// the same file multiple times w/ AutoRepair and repair failed problem.
//
//    Rev 1.5   25 Aug 1997 13:45:22   DALLEE
// In RestoreFile() added FileWipe() of original before FileCopy() restore
// of backup. Norton Unerase was catching the hidden delete that happened
// in FileCopy()'s call to CreateFile( CREATE_ALWAYS ) and infected files
// showed up in the recycle bin.
//
//    Rev 1.4   12 Aug 1997 17:38:34   MDUNN
// Forget that last rev... found out that there's a FileWipe() in Quake
// that does the trick.
//
//    Rev 1.3   11 Aug 1997 12:58:40   MDUNN
// Now wiping the backup file after a failed repair so that Smartcan doesn't
// save the file (which is infected).
//
//    Rev 1.2   07 Jul 1997 18:19:06   DBUCHES
// Ifdefd out inoculation code for NT platforms
//
//    Rev 1.1   31 May 1997 18:53:22   MKEATIN
// Passed NULL as the lpvCookie parameter VirusScanRepairFile()
//
//    Rev 1.0   06 Feb 1997 21:05:26   RFULLER
// Initial revision
//
//    Rev 1.2   07 Aug 1996 13:51:32   YVANG
// Ported changes from LuigiPlus.
//
//    Rev 1.1   28 Jun 1996 12:03:04   MKEATIN
// Ported Changes from LuigiPlus
//
//    Rev 1.0   30 Jan 1996 15:56:40   BARRY
// Initial revision.
//
//    Rev 1.0   30 Jan 1996 14:51:42   BARRY
// Initial revision.
//
//    Rev 1.5   11 Dec 1995 20:48:50   DALLEE
// Try to preserve same directory entry in RestoreFile().
// Will now to a FileCopy() back to the original name -- removed
// the fairly bogus use of CreateBackup() to do the restore.
//
//    Rev 1.4   03 Feb 1995 16:58:36   MFALLEN
// ANother
//
//    Rev 1.3   03 Feb 1995 16:56:32   MFALLEN
// SmartInocOpen() API change.
//
//    Rev 1.2   16 Dec 1994 13:51:18   MFALLEN
// RestoreFile is exported.
//
//    Rev 1.1   16 Dec 1994 11:00:08   MFALLEN
// Temporarely commented out TSR_
//
//    Rev 1.0   16 Dec 1994 10:37:00   MFALLEN
// Initial revision.
//
//    Rev 1.6   15 Dec 1994 20:14:04   MFALLEN
// From NIRVANA
//
//    Rev 1.5   20 Dec 1993 11:11:50   MFALLEN
// Reopen inoculation file in RW mode if that's how the file was opened when
// we entered into RepairFile().
//
//    Rev 1.4   07 Sep 1993 14:04:42   MFALLEN
// REmoved previously added SmartInocClose()
//
//    Rev 1.3   07 Sep 1993 10:34:32   MFALLEN
// Call to SmartInocClose()
//
//    Rev 1.2   07 Sep 1993 01:49:22   DALLEE
// After inoc repair, check if the file is still infected.
// Check existance and write-protect status of the file before repairing.
//************************************************************************

#include "platform.h"
#include "file.h"
#include "xapi.h"
#include "navutil.h"
#include "inoc.h"
#include "virscan.h"
#include "tsrcomm.h"

#ifdef SYM_WIN32
    #include "iquaran.h"
    #include "quar32.h"
    #include "n32behav.h" // For personality DLL
#endif

extern   HINSTANCE   hInstance;

//************************************************************************
// Local prototypes
//************************************************************************

static BOOL ZeroAndTruncateFile( LPTSTR lpszFile );

#ifdef SYM_WIN32

#define W95_ROOT_KEY   "SOFTWARE\\Symantec\\Norton Antivirus\\Auto-Protect\\InternalSettings"
#define NT_ROOT_KEY     "SOFTWARE\\Symantec\\Norton AntiVirus NT\\Auto-Protect\\InternalSettings"
#define FILE_SHARING     "FileSharing"

BOOL IsAutoProtectFileSharing()

{
    HKEY hKey;
    LPSTR pKey;
    DWORD dwFileSharingEnabled,dwSize = sizeof( DWORD );

    if ( SYM_SYSTEM_WIN_95 == SystemGetWindowsType() )
        pKey = W95_ROOT_KEY;
    else
        pKey = NT_ROOT_KEY;

    if ( RegOpenKeyEx( HKEY_LOCAL_MACHINE,pKey,0,KEY_QUERY_VALUE,&hKey ) != ERROR_SUCCESS )
        return( FALSE );

    if ( RegQueryValueEx( hKey,FILE_SHARING,NULL,NULL,( LPBYTE ) &dwFileSharingEnabled,&dwSize ) != ERROR_SUCCESS )
        return( FALSE );

    if ( dwFileSharingEnabled == 0 )
        return( TRUE );

    return( FALSE );
}

#endif  // def SYM_WIN32

//************************************************************************
// RepairFile()
//
// This routine backs up (if specified) and attempts to repair a file.
// The file is restored if possible to the original state.
//
// Parameters:
//      LPREPAIRFILESTRUCT  lpRepair    Info needed to repair file.
//
// Returns:
//      TRUE                            Repair succeeded.
//      FALSE                           Repair failed.
//************************************************************************
// 9/07/93 DALLEE, Check for infection after inoc repairs.
//************************************************************************

BOOL SYM_EXPORT WINAPI RepairFile ( LPREPAIRFILESTRUCT lpRepair )
{
    auto    BOOL        bStatus = FALSE;
    auto    char        szExt [ SYM_MAX_PATH+1 ];
    auto    UINT        uOrigAttr;
#if 0
#ifdef SYM_WIN
    TSR_OFF;
#endif
#endif

#ifdef SYM_WIN32
    if ( IsAutoProtectFileSharing() == TRUE )
        FileEnableFileSharing( FALSE );
#endif

    if ( (NOERR == FileGetAttr(lpRepair->szFullPath, &uOrigAttr)) &&
         (NOERR == FileSetAttr(lpRepair->szFullPath, uOrigAttr)) )
        {
        bStatus = FALSE;

#if !defined(NAVWNT) && !defined(NAVSCANW)
    	N30NOTES    N30Notes;
        if ( lpRepair->bUseInoc == TRUE )
            {
            uOrigAttr = SmartInocGetStatus();
            SmartInocOpen ( lpRepair->szFullPath, FALSE );

            if (IsInoculated(lpRepair->szFullPath) == INOC_CHANGED)
                {
                STRCPY (szExt, lpRepair->szBackExt);
                if ( lpRepair->bCreateBackup == TRUE )
                    CreateBackup (lpRepair);

                if (InocRepairFile(lpRepair->szFullPath))
                    {
                    bStatus = TRUE;

                    lpRepair->uVirusSigIndex = VirusScanFile(
                                                        lpRepair->szFullPath,
                                                        &N30Notes, TRUE);
                    bStatus = !lpRepair->uVirusSigIndex;
                    }

                if ((TRUE == lpRepair->bCreateBackup) && (FALSE == bStatus))
                    {
                    RestoreFile (lpRepair);
                    }
                }

            if ( uOrigAttr == SMARTOPEN_RW )
                SmartInocOpen ( lpRepair->szFullPath, TRUE );
            }
#endif
        if ( bStatus == FALSE )
            {
                                            // -------------------------------
                                            // Restore contents since it could
                                            // be modified.
                                            // -------------------------------
            STRCPY (szExt, lpRepair->szBackExt);

            if ( lpRepair->bCreateBackup == TRUE)
                CreateBackup (lpRepair);

            if ( VirusScanRepairFile (lpRepair->szFullPath,
                                     lpRepair->uVirusSigIndex,
                                     NULL) )
                {
                bStatus = TRUE;
                }
            }

        if ( bStatus == FALSE )
            {
            if ( lpRepair->bCreateBackup == TRUE )
                RestoreFile (lpRepair);
            }
        }

#ifdef SYM_WIN32
    if ( IsAutoProtectFileSharing() == TRUE )
        FileEnableFileSharing( TRUE );
#endif

#if 0
#ifdef SYM_WIN
    TSR_ON;
#endif
#endif

    return (bStatus);
}


//************************************************************************
// RestoreFile()
//
// Given the original filename, and the extension used to make a backup,
// restores a file's original contents.
// Attempts to preserve the orignal directory entry so we don't scan a
// file twice when repairing immediately.
//
// Parameters:
//      LPSTR   lpOemFileName           Name of original file. (OEM)
//      LPSTR   lpBackExt               Backup extension used.
//
// Returns:
//      Nothing.
//************************************************************************
// 12/11/95 DALLEE, removed crazy use of CreateBackup() function.
//      Use FileCopy() in order to preserve the orig dir entry. FileCopy()
//      was used when creating the backup, so it should be as legit here.
// 8/11/97 Mike Dunn -- Now wiping the backup file before deleting it, to
//                      keep Smartcan from saving the file.
// 6/23/98 JTAYLOR   -- Changed to now use ".vir" files for dos or OEM components
//                      and Quarantine backups for Windows components.
//************************************************************************

VOID SYM_EXPORT WINAPI RestoreFile (
    LPREPAIRFILESTRUCT lpRepair)
{
    auto    char    szFile [ SYM_MAX_PATH+1 ],
                    szPath [ SYM_MAX_PATH+1 ];

// do not attempt to use Quarantine from DOS.
#ifndef SYM_DOS
    auto    BOOL    bOEM = NAV_IS_SCANONLY();

    // Find out from the personality DLL if the install is the OEM version
    // If not OEM then use Quarantine for a backup mechanism.
    if( !bOEM )
        {
        auto    BOOL                bFileApisAnsi = AreFileApisANSI ();

        if (lpRepair->pQuarBackupItem != NULL)
            {
            if (!bFileApisAnsi)
                SetFileApisToANSI ();

            NameReturnPath(lpRepair->szFullPath, szPath);
            NameReturnFile(lpRepair->szFullPath, szFile);

            lpRepair->pQuarBackupItem->RestoreItem(szPath, szFile, TRUE);
            lpRepair->pQuarBackupItem->DeleteItem();
            lpRepair->pQuarBackupItem->Release();

            lpRepair->pQuarBackupItem = NULL;

            if (bFileApisAnsi)
                SetFileApisToANSI ();
            else
                SetFileApisToOEM ();
            }
        }
    else
#endif // SYM_DOS

        // If we are in DOS mode, or are an OEM product then restore the ".vir" file
        {
        auto    char    szBackExtOem [ SYM_MAX_PATH + 1];

        auto    UINT    uOrigAttr;
                                            // Set file attributes to normal
                                            // so we can modify it.
        if (NOERR == FileGetAttr(lpRepair->szFullOemPath, &uOrigAttr))
            {
            if (NOERR == FileSetAttr(lpRepair->szFullOemPath, FA_NORMAL))
                {
                                            // Construct the name of the
                                            // backup file in szFile.
                STRCPY(szFile, lpRepair->szFullOemPath);
                NameStripExtension(szFile);
                NavAnsiToOem (lpRepair->szBackExt, szBackExtOem);
                NameAppendExtension(szFile, szBackExtOem);

                // 1) Code below could be written as
                //        FileWipe( lpOemFileName, 1 );
                //        FileRename( szFile, lpOemFileName );
                //    but isn't because we'd like to restore to the original
                //    directory entry so we won't rescan the file if this happens
                //    in an immediate response.
                //
                // 2) ZeroAndTruncateFile() called before FileCopy() so
                //    Norton Protected Recycled Bin won't catch the delete
                //    caused by FileCopy()'s CreateFile( CREATE_ALWAYS ).
                //    Can't use FileWipe() since if we delete now then we may
                //    move the file to a later directory listing and get the
                //    rescan problem.

                ZeroAndTruncateFile( lpRepair->szFullOemPath );
                if ( COPY_SUCCESSFUL == FileCopy( szFile, lpRepair->szFullOemPath, NULL ) )
                    {
                    // Wipe and delete backup.

                    FileWipe ( szFile, 1 );
                    }
                else
                    {
                    // Had error -- Can only try renaming the backup to the
                    // original at this point...

                    FileRename( szFile, lpRepair->szFullOemPath );
                    }
                }

            FileSetAttr(lpRepair->szFullOemPath, uOrigAttr);
            }
        }
} // RestoreFile()


//************************************************************************
// ZeroAndTruncateFile( LPTSTR lpszFile )
//
// Special use function for RestoreFile() above. Will overwrite a failed
// repair file w/ zeros and truncate to 0 bytes. That way we can still
// use FileCopy() to restore, retain the same directory entry so we don't
// rescan, and NProtect won't catch an infected file deletion.
//
// Parameters:
//      LPTSTR  lpszFile            File to make look harmless to NProtect.
//
// Returns:
//      TRUE    on success
//      FALSE   otherwise.
//************************************************************************
// 08/26/1997 DALLEE, ripped guts from Quake's FileWipe().
//************************************************************************

static BOOL ZeroAndTruncateFile( LPTSTR lpszFile )
{
    #define ZERO_BUFFER_SIZE    ( 16 * 1024 ) // Less than 64K for DOS

    auto    HFILE       hFile;
    auto    UINT        uOrigAttr;
    auto    BOOL        bSuccess;
    auto    DWORD       dwFileSize;
    auto    DWORD       dwBytesLeft;
    auto    DWORD       dwBytesToWrite;
    auto    LPBYTE      lpbZeros;

    // Initialize locals.

    uOrigAttr = (UINT) -1;
    bSuccess  = FALSE;
    hFile     = HFILE_ERROR;
    lpbZeros  = NULL;


    if ( NOERR != FileGetAttr( lpszFile, &uOrigAttr ) )
        goto BailOut;

    if ( NOERR != FileSetAttr( lpszFile, FA_NORMAL ) )
        goto BailOut;

    lpbZeros = (LPBYTE) MemAllocPtr( GHND, ZERO_BUFFER_SIZE );
    if ( NULL == lpbZeros )
        goto BailOut;

    hFile = FileOpen( lpszFile, OF_READWRITE | OF_SHARE_DENY_WRITE );
    if ( HFILE_ERROR == hFile )
        goto BailOut;

    dwFileSize = FileLength( hFile );
    if ( HFILE_ERROR == dwFileSize )
        goto BailOut;

    // Write zeros to file.

    for ( dwBytesLeft = dwFileSize;
          dwBytesLeft;
          dwBytesLeft -= dwBytesToWrite )
    {
        dwBytesToWrite = min( dwBytesLeft, ZERO_BUFFER_SIZE );

        if ( dwBytesToWrite != FileWrite( hFile, lpbZeros, (UINT) dwBytesToWrite ) )
            break;
    }

    // Truncate file.

    FileSeek( hFile, 0, SEEK_SET );

#ifdef SYM_WIN32
    if ( SetEndOfFile( (HANDLE) hFile ) )
        bSuccess = TRUE;
#else
    if ( HFILE_ERROR != FileWrite( hFile, lpbZeros, 0 ) )
        bSuccess = TRUE;
#endif

BailOut:
    if ( (UINT) -1 != uOrigAttr )
        FileSetAttr( lpszFile, uOrigAttr );

    if ( HFILE_ERROR != hFile )
        FileClose( hFile );

    if ( NULL != lpbZeros )
        MemFreePtr( lpbZeros );

    return ( bSuccess );
} // ZeroAndTruncateFile()


