// ************************************************************************
// Copyright 1992-1993 Symantec, Peter Norton Product Group
// ************************************************************************
//                                                                
// $Header:   S:/NAVXUTIL/VCS/numbytes.cpv   1.1   23 Sep 1997 09:06:20   TSmith  $
//                                                                
// Description:                                                   
//      Figures out number of bytes to scan.
//                                                                
// Contains (as exports): 
//                                                                
// See Also:                                    
// ************************************************************************
// $Log:   S:/NAVXUTIL/VCS/numbytes.cpv  $
// 
//    Rev 1.1   23 Sep 1997 09:06:20   TSmith
// Fixed 'Walmart' bag: Modified 'GetBytesToScan' to use 'GetDiskFreeSpaceEx'
// when available rather than 'DiskFreeAvailGet'.
// 
//    Rev 1.0   06 Feb 1997 21:05:10   RFULLER
// Initial revision
// 
//    Rev 1.3   07 Nov 1996 12:13:16   MKEATIN
// Added DBCS code
// 
//    Rev 1.2   17 Apr 1996 11:30:30   YVANG
// Added #include "dbcs.h".
// 
//    Rev 1.1   04 Apr 1996 17:28:30   YVANG
// Ported from Kirin.
// 
//    Rev 1.1   03 Apr 1996 15:25:28   YVANG
// DBCS enable.
// 
//    Rev 1.0   30 Jan 1996 15:56:24   BARRY
// Initial revision.
// 
//    Rev 1.0   30 Jan 1996 14:51:26   BARRY
// Initial revision.
// 
//    Rev 1.5   17 Apr 1995 15:47:14   MFALLEN
// sym_max_path+1
// 
//    Rev 1.4   05 Apr 1995 11:19:20   MFALLEN
// Moved IsUNC to validate.cpp
// 
//    Rev 1.3   30 Mar 1995 14:00:46   MFALLEN
// Replaced NameGetIllegalChars() with NameReturnIllegalChars()
// 
//    Rev 1.2   20 Dec 1994 17:21:22   MFALLEN
// symineg.h was removed from nonav.h include file and had to be included here.
// 
//    Rev 1.1   16 Dec 1994 13:04:32   MFALLEN
// Dos build was complaining.
// ************************************************************************

#include "platform.h"                   // Platform independence
#include "symcfg.h"
#include "symnet.h"

#ifdef CANT_PORT_THIS
    #include "dosnet.h"
#endif

#include "file.h"
#include "disk.h"
#include "xapi.h"
#include "stddos.h"
#include "tsr.h"
#include "tsrcomm.h"
#include "navutil.h"
#include "syminteg.h"
#include "nonav.h"
#include "dbcs.h"

// ************************************************************************
//                      LOCAL FUNCTION PROTOTYPES
// ************************************************************************

DWORD LOCAL DetermineDirContents(
    LPSTR   lpPathPtr,
    DWORD   dwClusterSize,
    BOOL    bOnlyCountDirs);


DWORD LOCAL MyGetVolumeInformation (    // Gets volume info for GetBytesToScan()
    LPSTR   lpVolume,                   // [in] pointer to volume info
    LPDWORD lpdwNetSecPerBlock,         // [out] sectors per block
    LPDWORD lpdwNetTotalBlocks,         // [out] total number of blocks
    LPDWORD lpdwNetAvailBlocks );       // [out] available blocks

// ************************************************************************
// RUNTIME IMPORT API PROTOTYPES
// ************************************************************************

#ifdef SYM_WIN32

#ifdef UNICODE
    #define SZGETDISKFREESPACEEX TEXT( "GetDiskFreeSpaceExW" )
#else
    #define SZGETDISKFREESPACEEX TEXT( "GetDiskFreeSpaceExA" )
#endif

BOOL ( STDAPICALLTYPE* lpfnGetDiskFreeSpaceEx )(
    LPCTSTR         lpDirectoryName,
    PULARGE_INTEGER lpFreeBytesAvailableToCaller,
    PULARGE_INTEGER lpTotalNumberOfBytes,
    PULARGE_INTEGER lpTotalNumberOfFreeBytes
    );

#endif


// ************************************************************************
// DOS EXTERNS
// ************************************************************************

#if defined(SYM_DOS) || defined(SYM_VXD)

#ifdef __cplusplus
extern "C" {
#endif

    extern  WORD        trapping_enabled;

#ifdef __cplusplus
}
#endif

#endif  // SYM_DOS


//************************************************************************
// GetBytesToScan()
//
// This routine reads the scan list from the NONAV structure, estimates
// the size of the files to scan, and writes the value to the NONAV
// structure.
//
// Parameters:
//      LPNONAV     lpNoNav             Pointer to info for this scan.
//
// Returns:
//      DWORD                           Size of files in scan list.
//************************************************************************
// 5/28/93 DALLEE, lifted from Martin's BldProgressValue in N_SCNSTR.C
// 9/3/93  CAREY,  did some work on it.
// 9/3/93  MARTIN, modified it so both DOS and Windows can use it.
//************************************************************************

DWORD SYM_EXPORT WINAPI GetBytesToScan (
    LPSTR           lpScanList,         // [in] list of items to scan
    BOOL            bScanSubs,          // [in] scan sub directories
    LPFNBADENTRY    lpfnSkipDrive,      // function to call to skip a drive
    LPFNDRIVEERROR  lpfnDriveError,     // function to notify if drive error
    DWORD           dwUserData)         // data to pass on to callbacks
{
    auto    DWORD           dwTotalKBytes = 0;
    auto    DWORD           dwUsedBytes  = 0;
    auto    DWORD           dwAvailBytes = 0;
    auto    HFILE           hFile;
    auto    BOOL            bStripEntry ;
    auto    DWORD           dwClusterSize;
    auto    DWORD           dwClusters;
    auto    DWORD           dwClusterMod;
    auto    DWORD           dwFileLen;
    auto    BOOL            bAcessedNetworkDrive = FALSE;
    auto    DWORD           dwNetTotalBlocks;
    auto    DWORD           dwNetAvailBlocks;

#ifdef SYM_WIN32
    auto    ULARGE_INTEGER  liTotalFreeBytes;
    auto    ULARGE_INTEGER  liCallerFreeBytes;
    auto    ULARGE_INTEGER  liTotalBytes;
    auto    ULARGE_INTEGER  liInUseBytes;
    auto    HINSTANCE       hInstance;
    auto    BOOL            bResult;
#endif

#ifndef SYM_DOS //&? Comment out 8/16/94.  Symnet still no-go for .DX
    auto    DWORD       dwNetSecPerBlock;
#endif

#ifdef SYM_DOS
    auto    WORD        wOldErrorTrapState;
#endif

    SYM_ASSERT ( lpScanList );

// MARTIN    TSR_OFF;
    while (*lpScanList != EOS)
        {
#ifdef SYM_WIN
                                        // ------------------------------------
                                        // If the user has previously indicated
                                        // that this drive should be skipped.
                                        // ------------------------------------
        if (lpScanList[1] == '#')
            {
            lpScanList = SzzFindNext ( lpScanList ) ;
            continue;
            }
#endif
        bStripEntry = FALSE ;
        dwUsedBytes = 0 ;

                                        // ------------------------------------
                                        // Determine cluster size
                                        // ------------------------------------

        dwClusterSize = 0 ;             // Start with 0....

                                        // ------------------------------------
                                        // If the disk is a network disk, 
                                        // try Netware call...
                                        // ------------------------------------

        if ( DiskIsNetwork(*lpScanList) || IsUNCPath(lpScanList) == TRUE )
            {
#ifndef SYM_DOS //&? Comment out 8/16/94.  Symnet still no-go for .DX

            dwClusterSize = MyGetVolumeInformation (
                                        lpScanList,
                                        &dwNetSecPerBlock,
                                        &dwNetTotalBlocks,
                                        &dwNetAvailBlocks
                                        );
            if ( dwClusterSize )
                bAcessedNetworkDrive = TRUE;

#endif  // !SYM_DOS
            }
                                        // -----------------------------------
                                        // If not a network disk or network 
                                        // call failed, try DOS call (it works
                                        // on some networks and at least gives
                                        // a decent default).
                                        //
                                        // If this little $^@ is a UNC path then
                                        // in todays primitive world we have
                                        // a handfull of problems.
                                        //
                                        // -----------------------------------
        if ( IsUNCPath ( lpScanList ) == FALSE )
            {
            if ( dwClusterSize == 0 )
                {
                                        // -----------------------------------
                                        // Check for error the first time we 
                                        // hit the disk and let the user Retry
                                        // or Skip the disk. The assumption here
                                        // is that if the first access fails, 
                                        // the disk is probably off-line and 
                                        // shouldn't be scanned.  If we get 
                                        // past here, we assume the other 
                                        // accesses will be OK. disable critical
                                        // error (we handle message ourselves)
                                        // -----------------------------------
#ifndef SYM_WIN
                _ClearErrorFar ( ) ;    // Clear any pending so we see this one
                wOldErrorTrapState = trapping_enabled ;
                trapping_enabled = FALSE ;
#endif

                while ( (dwClusterSize = DiskGetClusterSize(*lpScanList)) == -1 )
                    {
                                        // -----------------------------------
                                        // Unless they want to skip, 
                                        // force a retry.
                                        // -----------------------------------

                    if ( lpfnDriveError &&  
                         (*lpfnDriveError)(*lpScanList,dwUserData) != DRIVEERROR_SKIP )
                        continue ;          // Try again...
                    else                    // If no callback or they skipped, remove entry
                        {
                        bStripEntry = TRUE ;
                        break ;
                        }
                    }
                                        // -----------------------------------
                                        // Upon exit, dwClusterSize == -1 and 
                                        // bStripEntry is set if error
                                        // -----------------------------------
#ifndef SYM_WIN
                                        // -----------------------------------
                                        // Re-enable critical errors
                                        // -----------------------------------
                trapping_enabled = wOldErrorTrapState ;
                _ClearErrorFar ( ) ;
#endif
                }                       // end determine cluster size

                                        // -----------------------------------
                                        // Give up if dwClusterSize still = 0 
                                        // (or -1 )
                                        // -----------------------------------

                                        // -----------------------------------
                                        // Determine dwUsedBytes
                                        // -----------------------------------

                                        // -----------------------------------
                                        // Case 0: Can't access drive
                                        // -----------------------------------
            if ( dwClusterSize == -1 || dwClusterSize == 0 )
                {
                dwUsedBytes = 0 ;
                }
                                        // -----------------------------------
                                        // Case 1: Scanning a whole drive
                                        // -----------------------------------
            else if (STRLEN(lpScanList) == 3 && bScanSubs )
                {
                if ( bAcessedNetworkDrive == TRUE )
                    {
                    dwUsedBytes = dwNetTotalBlocks - dwNetAvailBlocks;
                    dwUsedBytes *= dwClusterSize;
                    }
                else
                    {
#ifdef SYM_WIN32
                    liTotalFreeBytes.QuadPart = 0;
                    liCallerFreeBytes.QuadPart = 0;
                    liTotalBytes.QuadPart = 0;
                    liInUseBytes.QuadPart = 0;

                                        // -----------------------------------
                                        // Use the 'GetDiskFreeSpaceEx' call 
                                        // directly when available (i.e. on
                                        // WinNT >= 4.0 and Win95 >= OSR2).
                                        // On Win95 the 'GetDiskFreeSpace'
                                        // call (which is called by
                                        // 'DiskFreeAvailGet') returns bogus
                                        // values for drives > 2GB as stated
                                        // in MS documentation.
                                        // -----------------------------------
                    hInstance = LoadLibrary( TEXT( "KERNEL32" ) );

                    ( FARPROC& )lpfnGetDiskFreeSpaceEx = GetProcAddress( hInstance, SZGETDISKFREESPACEEX );

                                        // -----------------------------------
                                        // If the address to the function was 
                                        // successfully obtained, go ahead and
                                        // make the call.
                                        // -----------------------------------
                    if ( NULL != (FARPROC&)lpfnGetDiskFreeSpaceEx )
                        {
                        bResult = lpfnGetDiskFreeSpaceEx( lpScanList,
                                                          &liCallerFreeBytes,
                                                          &liTotalBytes,
                                                          &liTotalFreeBytes );
                        FreeLibrary( hInstance );

                        if ( bResult )
                            {
                            liInUseBytes.QuadPart = liTotalBytes.QuadPart - liTotalFreeBytes.QuadPart;
                            dwUsedBytes = ( liInUseBytes.HighPart > 0 ) ? MAXDWORD : liInUseBytes.LowPart;
                            }
                        else
                            {
                            dwUsedBytes = 0;
                            }
                        }
                    else
                        {
#endif // SYM_WIN32
                                        // -----------------------------------
                                        // Fall back to the older method...
                                        // -----------------------------------
                        if ( NOERR != DiskFreeAvailGet( *lpScanList,
                                                        &dwAvailBytes,
                                                        &dwUsedBytes ) )
                            {
                            dwUsedBytes = 0;
                            }
#ifdef SYM_WIN32
                        }
#endif // SYM_WIN32
                    }
                }
                                        // -----------------------------------
                                        // Case 2: Scanning a single file
                                        // (ignoring bScanSubs case)
                                        // -----------------------------------

            else if ((hFile = FileOpen(lpScanList, READ_ONLY_FILE)) != HFILE_ERROR)
                {
                if ( dwClusterSize != 0 )   // Avoid divide by 0
                    {
                    dwFileLen = FileLength(hFile);

                    dwClusterMod = (DWORD)(dwFileLen % dwClusterSize);
                    dwClusters = (DWORD)(dwFileLen / dwClusterSize);
            
                    if (dwClusterMod != 0)
                        dwClusters++;

                    dwUsedBytes = (DWORD)(dwClusters * dwClusterSize) ;
                    }

                FileClose (hFile);
                }
            else
                {
                                        // ------------------------------------
                                        // Case 3: Scanning a directory
                                        // (wildcards also fall through)
                                        // Check if we're doing subdirs or
                                        // just files.
                                        // If bScanSubs is TRUE we add kludge
                                        // for each subdir in the specified
                                        // directory, else we count the total
                                        // file sizes.
                                        // ------------------------------------

                dwUsedBytes += DetermineDirContents (
                                                lpScanList,dwClusterSize,
                                                bScanSubs
                                                );

                }
            }
        else
            {
                                        // ------------------------------------
                                        // The darn thing is a UNC path.
                                        // At present time there isn't any way
                                        // of obtaining the size of a UNC path
                                        // so we invent a large number.
                                        // ------------------------------------
            dwUsedBytes = (DWORD)-1;
            }
                                        // ------------------------------------
                                        // When dealing with multiple network
                                        // drives the number of bytes to scan
                                        // can be really huge, so in order to
                                        // prevent variables to wrap around
                                        // we count number of K scanned.
                                        // ------------------------------------
        dwUsedBytes /= 1024;
                                        // ------------------------------------
                                        // Add in the bytes for entry,but don't
                                        // wrap to 0 
                                        // ------------------------------------

        if ((dwTotalKBytes + dwUsedBytes) < dwTotalKBytes)
            {
            dwTotalKBytes = (DWORD) -1L;
            }
        else
            {
            dwTotalKBytes += dwUsedBytes;
            }
                                        // ------------------------------------
                                        // Advance to next entry
                                        // ------------------------------------

        if ( bStripEntry && lpfnSkipDrive )
            (*lpfnSkipDrive)(&lpScanList, dwUserData);
        else
            lpScanList = SzzFindNext ( lpScanList ) ;
        }
// MARTIN   TSR_ON;

   return ( dwTotalKBytes );

} // End GetBytesToScan()


//************************************************************************
// MyGetVolumeInformation()
//
// This routine obtains the volume information from the specified volume
//
// Parameters:
//      lpVolume,           pointer to volume to obtain info from
//      lpdwNetSecPerBlock  returns sectors per block
//      lpdwNetTotalBlocks  returns total number of blocks
//      lpdwNetAvailBlocks  returns available blocks
//
// Returns:
//      DWORD   Size of a cluster on the specified volume or zero if error.
//************************************************************************
// 8/25/94  MARTIN, function created
//************************************************************************

DWORD LOCAL MyGetVolumeInformation (    // Gets volume info for GetBytesToScan()
    LPSTR   lpVolume,                   // [in] pointer to volume info
    LPDWORD lpdwNetSecPerBlock,         // [out] sectors per block
    LPDWORD lpdwNetTotalBlocks,         // [out] total number of blocks
    LPDWORD lpdwNetAvailBlocks          // [out] available blocks
    )
{
    auto    DWORD   dwClusterSize   = 0;

#ifdef SYM_WIN32
    auto    DWORD   dwSectorsPerCluster = 0;
    auto    DWORD   dwBytesPerSector    = 0;
    auto    DWORD   dwFreeClusters      = 0;
    auto    DWORD   dwTotalNumClusters  = 0;

    BOOL bStatus = GetDiskFreeSpace (
                                    lpVolume,
                                    &dwSectorsPerCluster,
                                    &dwBytesPerSector,
                                    &dwFreeClusters,
                                    &dwTotalNumClusters
                                    );
    if ( bStatus == TRUE )
        {
        *lpdwNetSecPerBlock = dwSectorsPerCluster;
        *lpdwNetTotalBlocks = dwTotalNumClusters;
        *lpdwNetAvailBlocks = dwFreeClusters;

        dwClusterSize = dwSectorsPerCluster * 512;
        }
    #ifdef SYM_DEBUG
    else
        {
        dwTotalNumClusters = GetLastError();
        }
    #endif // #ifdef SYM_DEBUG
#else

    auto    int                 nNWError        = NETERR_NONETWORK;
    auto    NETVOLENUMSTRUCT    VolInfo         = { 0 };
    auto    char                szBuffer [SYM_MAX_PATH+1];

    WORD wBufferSize = sizeof(szBuffer);
    NetGetConnection (NULL, lpVolume, szBuffer, &wBufferSize);

    // ---------------------------------------------------- //
    // NOTE:  This assumes a NETWARE name: SERVER/VOL:path  //
    //        It will fail on UNC names like \\SERVER\SHARE //
    // ---------------------------------------------------- //

    for ( UINT i = 0; szBuffer[i] && szBuffer[i] != '/';)
        {
        if ( DBCSIsLeadByte(szBuffer[i]) )
            i++;
        i++;
        }

    if ( szBuffer [ i ] )       // Avoid going past end of string
        {
        szBuffer[i] = EOS;

        for ( i++; szBuffer[i] && szBuffer[i] != ':'; )
            {
            if ( DBCSIsLeadByte(szBuffer[i]) )
                i++;
            i++;
            }

        szBuffer[i] = EOS;
        }

    MEMSET ( & VolInfo , 0 , sizeof ( VolInfo ) ) ;
    VolInfo.dwStructSize = sizeof(VolInfo);

    nNWError = NetVolGetInfo (
                             NULL,
                             szBuffer,
                             &szBuffer[STRLEN(szBuffer) + 1],
                             &VolInfo
                             );

    *lpdwNetSecPerBlock = VolInfo.dwSectorsPerBlock;
    *lpdwNetTotalBlocks = VolInfo.dwTotalBlocks;
    *lpdwNetAvailBlocks = VolInfo.dwAvailableBlocks;

                                        // ----------------------------------
                                        // Calculate network cluster size =
                                        // sectors per block * sectorsize
                                        // ----------------------------------

    if ( nNWError == NOERR )
        dwClusterSize = *lpdwNetSecPerBlock * 512;

#endif

    return ( dwClusterSize );
}

//************************************************************************
// DetermineDirContents
//
// If bOnlyCountDirs == FALSE, this routine determines the total size of all
// *files* in the specified directory at the TOP level.  If it is TRUE, the
// number of directories at the top level is counted and 1 is added for each
// directory found.
//
// Parameters:
//      LPSTR   lpPathPtr
//      BOOL    bOnlyCountDirs;
//
// Returns:
//      DWORD                           Size of files in specified directory
//************************************************************************
// 9/2/93 CAREY created
//************************************************************************

DWORD LOCAL DetermineDirContents (
    LPSTR   lpPathPtr,
    DWORD   dwClusterSize,
    BOOL    bOnlyCountDirs
    )
{
    #define DIR_CONST_VALUE     524288L

    auto    char                szTempPath[SYM_MAX_PATH + 1];
    auto    DWORD               dwCount = 0;
    auto    DWORD               dwClusters;
    auto    WORD                wLen;
    auto    FINDDATA            rFFInfo;
    auto    HFIND               hFind = NULL;

                                        // ------------------------------------
                                        // Now determine if we're dealing with
                                        // a directory name or a file name
                                        // with wild cards.  If we were dealing
                                        // with a valid file, we woulh have
                                        // already determined the size of that
                                        // file earlier.
                                        // ------------------------------------

    STRCPY(szTempPath,lpPathPtr);

    if (!SYMstrchr(szTempPath,'*') && !SYMstrchr(szTempPath,'?'))
        {
                                        // ------------------------------------
                                        // this is the case where there are no 
                                        // wildcard characters. Append a
                                        // *.* on the end of the path.
                                        // ------------------------------------

        if ((szTempPath[(wLen = STRLEN(szTempPath))-1] != '\\') ||
            (DBCSGetByteType(&szTempPath[0], 
                             &szTempPath[wLen-1]) == _MBC_TRAIL))
            {
            szTempPath[wLen] = '\\';        // add backslash before *.*
            szTempPath[wLen+1] = 0;
            }

        STRCAT(szTempPath,"*.*");           // X:\YYY\*.*
        }
    else if (bOnlyCountDirs)
        {
                                        // ------------------------------------
                                        // this is the case where there is a 
                                        // wildcard in the pathname.  Check
                                        // if we're scanning subdirs.  If so,
                                        // we remove the wildcard filename
                                        // from the end of the path and scan 
                                        // for all files (*.*), only looking
                                        // for directories.
                                        // ------------------------------------

        NameReturnPath(lpPathPtr,szTempPath);


        if ((szTempPath[(wLen = STRLEN(szTempPath))-1] != '\\') ||
            (DBCSGetByteType(&szTempPath[0], 
                             &szTempPath[wLen-1]) == _MBC_TRAIL))
            {
            szTempPath[wLen] = '\\';        // add backslash before *.*
            szTempPath[wLen+1] = 0;
            }

        STRCAT(szTempPath,"*.*");           // X:\YYY\*.*

        }

    hFind = FileFindFirst(szTempPath, &rFFInfo);

                                        // ------------------------------
                                        // Can't work around this. The
                                        // compiler is very unhappy.
                                        // ------------------------------
#ifdef SYM_WIN32
    while ( hFind != INVALID_HANDLE_VALUE )
#else
    while ( (int)hFind != -1 )
#endif
        {
        if (bOnlyCountDirs == TRUE)
            {
                                        // ------------------------------
                                        // kludge if we have directories 
                                        // (other than . and ..)
                                        // ------------------------------

            if ((rFFInfo.dwFileAttributes & FA_DIR ) && 
                 rFFInfo.szFileName[0] != '.')
                {
                dwCount += DIR_CONST_VALUE;
                }
            }
                                        // ------------------------------
                                        // Note that I'm only using the
                                        // low order DWORD for the file size
                                        // assuming that a file will not
                                        // be larger than 4GB.
                                        // ------------------------------

        dwClusters = ((DWORD)rFFInfo.dwFileSizeLow / dwClusterSize);

        if ( (DWORD) rFFInfo.dwFileSizeLow % dwClusterSize )
            dwClusters++;

        dwCount += (DWORD)(dwClusters * dwClusterSize);

        if ( FileFindNext( hFind, &rFFInfo) == FALSE )
            break;
        }

    if ( hFind )
        FileFindClose ( hFind );

    return ( dwCount );
}


//************************************************************************
// ComputeKilobytesScanned ()
//
//  This function computes the number of kilobytes that this file occupies.
//
// Parameters:
//
//  DTA_ FAR *lpDtal    Pointer to DTA structure
//  DWORD dwClusterSize The size of one cluster on this disk.
//
// Returns:     
//  Number of kilobytes this file occupies.
//
//************************************************************************
// 04/01/91 Martin   Function created
//************************************************************************

DWORD SYM_EXPORT WINAPI ComputeKilobytesScanned (
    DWORD   dwFileSize,
    DWORD   dwClusterSize
    )
{
    auto    DWORD   dwClusters;
                                        // ----------------------------------
                                        // Prevent an accidental divide by 0
                                        // ----------------------------------

    if ( 0 == dwClusterSize || -1 == dwClusterSize )
        dwClusterSize = 4096;

    dwClusters = (dwFileSize / dwClusterSize);

    if ( dwFileSize % dwClusterSize )
        dwClusters++;

    dwFileSize = (dwClusters * dwClusterSize) / 1024;

    return (dwFileSize);
}


