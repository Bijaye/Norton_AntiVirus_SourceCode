// Copyright 1995-1997 Symantec, Peter Norton Product Group
//***************************************************************************
//
// $Header:   S:/NAVSDK/VCS/navsdk.cpv   1.14   01 Dec 1998 12:03:34   tcashin  $
//
// Description:
//
//      This file is for hiding NAV Engine technology underneath a layer
//      for allowing us to export our scan technology to other products
//      and other companies without giving everything away.
//
// Contains:
//    Exports:
//      VirusLibraryClose()
//      VirusLibraryDeleteFile()
//      VirusLibraryGetLastVirusInfo()
//      VirusLibraryInit()
//      VirusLibraryScanFile()
//      VirusLibraryScanMem()
//      VirusLibraryScanSysArea()
//      VirusLibrarySelfTest()
//
//    NAVSDK internal:
//      AutoProtectOff()
//      AutoProtectOn()
//      ValidOEMContext()
//
//    Local:
//      GetVirusInfo()
//      InitializeLibrary()
//      ScanDrive()
//
//***************************************************************************
// $Log:   S:/NAVSDK/VCS/navsdk.cpv  $
// 
//    Rev 1.14   01 Dec 1998 12:03:34   tcashin
// Fill in the wVirusID in GetVirusInfo().
// 
//    Rev 1.13   31 Aug 1998 18:02:20   jsulton
// Changed SYM_CPU_ALPHA to _ALPHA_ so we build properly.
// 
//    Rev 1.12   26 Aug 1998 14:55:26   jsulton
// Changes made by Thuridion to work on the Alpha platform.
// 
//    Rev 1.11   19 Mar 1998 11:45:02   DALLEE
// Enable use of same scanning handle among multiple threads.
// Now saving per thread copies of oem context information, not
// just a pointer to original info.
// This allows storage of unique last virus found info.
// 
//    Rev 1.10   04 Mar 1998 13:18:24   PVENKAT
// Added GetDefDateFromDir and support for no AppID with Defs Dir.
// 
//    Rev 1.9   23 Dec 1997 11:59:34   DALLEE
// Check error results from FileWipe().
// Delete should now correctly return error for failure due to 
// write protected media, etc.
// 
//    Rev 1.8   18 Dec 1997 13:24:50   DALLEE
// Added memory progress callback.
// 
//    Rev 1.7   18 Dec 1997 02:06:12   DALLEE
// Klugery to combat buggy NAVENG. If the config callback is not implemented
// and there is no .INF engine config file, then heuristic scanning is
// disabled (the engine gets an empty extension list).
// Added engine config NAVSDK.INF (szNavexInf) path to per thread data.
// In callback, if this doesn't exist, manually set scan level to 3.
// 
//    Rev 1.6   11 Dec 1997 12:48:02   DALLEE
// Added VirusLibraryScanSysBuffer() for scanning memory images of
// boot sectors.
// 
//    Rev 1.5   09 Dec 1997 12:11:48   DALLEE
// Added checking for presence of symkrnl vxd before performing memory scan.
// Cheap test so that we don't GPF when we can't get pointers to 
// physical memory.
// 
//    Rev 1.4   08 Dec 1997 16:21:48   DALLEE
// Cleaned possible infinite loop in ScanSysArea().
// Moved VirusLibrarySelfTest() to new file SELFTEST.CPP.
// 
//    Rev 1.3   04 Dec 1997 16:16:32   DALLEE
// 1. Implemented simple/automatic method for managing AppID's.
// 2. Store scanning context information in per thread data, so we
//    can access it in engine callbacks where it is not provided.
// 
//    Rev 1.2   04 Dec 1997 09:52:58   DALLEE
// Added a DllMain() (yikes! How'd we get by w/o one before?).
// Chain through to Symkrnl's DllMain so it gets initialized.
// Added initialization for device list synchronization objects.
// 
//    Rev 1.1   02 Dec 1997 14:43:34   DALLEE
// Changed NAVOEMAPI to NAVSDKAPI for new naming conventions.
// 
//    Rev 1.0   02 Dec 1997 14:28:08   DALLEE
// Initial revision.
//***************************************************************************
// From NEWVIR-NAVOEM navoem.cpv r1.34
//***************************************************************************

//***************************************************************************
// Include Files
//***************************************************************************
#include "platform.h"
#include <stdio.h>
#include "xapi.h"
#include "avapi.h"
#include "navsdk.h"
#include "navsdkr.h"
#include "navdefs.h"
#include "oemapi.h"
#include "device.h"
#include "navutil.h"
#include "symkrnl.h"
#include "header.h"

//***************************************************************************
// Local Function Prototypes
//***************************************************************************

BOOL WINAPI DllMain (
    HINSTANCE   hInstDLL,
    DWORD       dwReason,
    LPVOID      lpvReserved );

static BOOL GetDefDateFromDir ( 
    LPTSTR      lpszDir, 
    LPWORD      pwYear, 
    LPWORD      pwMonth, 
    LPWORD      pwDay, 
    LPDWORD     pdwRev );

static VOID WINAPI GetVirusInfo(
    HVLVIRUS         hVirus,
    LPDEVICEINFOREC  lpDeviceInfo,
    LPVIRUSINFO      lpVirInfo,
    LPTSTR           lpszVirusNameBuf );

static HVCONTEXT WINAPI InitializeEngine (
    LPCTSTR     lpszDefDir,
    LPCTSTR     lpszNavexInf );

static VSTATUS WINAPI ScanDrive(
    HVCONTEXT   hContext,
    BOOL        bPhysDrvFlag,
    BYTE        byDrive,
    LPHVLVIRUS  lphVirus );

//***********************************************************************
// External Data
//***********************************************************************

extern "C" {
extern  AVGLOBALCALLBACKS   g_AVCallbacks;
}


//***********************************************************************
// Global Data
//***********************************************************************
BOOL        gbUseNAVAPDLL = FALSE;
FARPROC     lpfnNAVAPProtect;
FARPROC     lpfnNAVAPUnprotect;
HINSTANCE   ghNAVAP;

HINSTANCE   g_hInstNAVSDK;


//************************************************************************
// DllMain()
//
// Standard stuff, plus chain through to Symkrnl's attach/detach, 
// and initialize synchronization objects for device info list.
// Also allocate per thread (per scan) context storage.
//
// Returns:
//      TRUE    on success.
//      FALSE   otherwise.
//************************************************************************
// 05/27/1997 DALLEE, created.
//************************************************************************
BOOL WINAPI DllMain (
    HINSTANCE   hInstDLL,
    DWORD       dwReason,
    LPVOID      lpvReserved )
{
    auto    LPOEMCONTEXT    lpOEMContext;
    
    switch ( dwReason )
    {
    case DLL_PROCESS_ATTACH:
        // Save HINSTANCE on process attach.

        g_hInstNAVSDK = hInstDLL;

        // Synchronization objects for device info list access.

        InitDeviceListSynchronization();

        // Allocate storage for per thread scan context.

        AllocScanContextStorage();

        // Need to chain through to SYMKRNL's DllMain.

        SymKRNLAttach( hInstDLL, dwReason, lpvReserved );
        break;

    case DLL_PROCESS_DETACH:
        // Synchronization objects for devices info list access.
        
        CloseDeviceListSynchronization();

        // Free per thread context storage.

        FreeScanContextStorage();

        // Chain through to SYMKRNL's DllMain.

        SymKRNLAttach( hInstDLL, dwReason, lpvReserved );
        break;

    case DLL_THREAD_ATTACH:
        // Initialize per thread scan context pointer.

        SetScanContext( NULL );
        break;

    case DLL_THREAD_DETACH:
        // Free this thread's allocated context, if any.
        
        lpOEMContext = GetScanContext();

        if ( NULL != lpOEMContext )
        {
            MemFreePtr( lpOEMContext );
        }
        
        break;
    }

    return ( TRUE );
} // DllMain()


/*@API:**********************************************************************
@Declaration:
    static WORD WINAPI InitializeEngine (
        LPCTSTR     lpszDefDir,
        LPCTSTR     lpszNavexInf );

@Description:
    This routine initializes the virus scanning core using the data
    files specified.

@Parameters:
    $lpszDefsDir$   Directory containing definitions files to use.
    $lpszNavexInf$  Location of navex configuration file. May be empty
                        or NULL.

@Returns:
    If successful, the function returns a handle to a virus scanning
    context. On error, it returns NULL.

@See:

@Include:
    navsdk.h

@Compatibility: DOS DX W16 W32
****************************************************************************/
static HVCONTEXT WINAPI InitializeEngine (  // Initialize virus library
    LPCTSTR     lpszDefDir,
    LPCTSTR     lpszNavexInf )
{
    auto    TCHAR           szVirscanDat [ SYM_MAX_PATH ];
    auto    TCHAR           szVirscanInf [ SYM_MAX_PATH ];
    auto    HVCONTEXT       hContext;
    auto    BOOL            bSuccess = FALSE;

    SYM_ASSERT( lpszDefDir && lpszDefDir[0] );

    // Get def file locations.
    // AppID registration and usage will be handled by client of this routine.

    STRCPY( szVirscanDat, lpszDefDir );
    STRCPY( szVirscanInf, lpszDefDir );
    NameAppendFile( szVirscanDat, _T("VIRSCAN.DAT") );
    NameAppendFile( szVirscanInf, _T("VIRSCAN.INF") );

    // Initialize AVAPI.

    // Assert we're aware what engine revision we're using. 
    // If this fires, please verify we're using the new revision of the engine
    // correctly, then update the assert and the revision for VLScanInit().

    SYM_ASSERT( 1 == VL_REVISION );

    hContext = VLScanInit( 1, &g_AVCallbacks, VLINIT_NO_ACCESS_NO_HANDLE );

    if ( hContext )
    {
        if ( VS_OK != VLRegisterVirusDB( hContext,
                                         szVirscanDat,
                                         szVirscanInf,
                                         (LPTSTR)lpszNavexInf ) )
        {
            VLScanClose( hContext );
            hContext = NULL;
        }
    }

    return ( hContext );
} // InitializeEngine()


/***************************************************************************
@Declaration:
    static BOOL GetDefDateFromDir ( 
        LPTSTR      lpszDir, 
        LPWORD      pwYear, 
        LPWORD      pwMonth, 
        LPWORD      pwDay, 
        LPDWORD     pdwRev )

@Description:
    This routine reads the header from VIRSCAN1.DAT and sets the Virus 
    definition dates.

@Parameters:
    $lpszDir$       Directory containing definitions files to use.
    $pwYear$        Year of the current definition
    $pwMonth$       Month of the current definition
    $pwDay$         Day of the current definition
    $pwRev$         Revision of the current definition

@Returns:
    If successful, the function returns TRUE and parameters are set
    otherwise, returns FALSE and the parameters are not disturbed.

@See:

@Include:
    header.h

@Compatibility: DOS DX W16 W32
****************************************************************************/
static BOOL GetDefDateFromDir ( 
    LPTSTR      lpszDir, 
    LPWORD      pwYear, 
    LPWORD      pwMonth, 
    LPWORD      pwDay, 
    LPDWORD     pdwRev )
{
    auto    char    szVirscanPath[_MAX_PATH];
    auto    BOOL    bRetVal = FALSE;
    auto    FILE    *fp;

    auto    DATA_FILE_HEADER_T  rHeader;

    // create the full path to VIRSCAN1.DAT
    strcpy (szVirscanPath, lpszDir);
    NameAppendFile (szVirscanPath, VIRSCAN_FILE) ;

    // get VIRSCAN1.DAT's internal date stamp
    fp = fopen ( szVirscanPath, "rb");

    if (fp)
    {
        // read the header
        if ( fread ( &rHeader, sizeof(rHeader), 1, fp))
        {
            *pwYear = (WORD) (rHeader.byYear + 1900);
            *pwMonth = (WORD) rHeader.byMon;
            *pwDay = (WORD) rHeader.byDay;
            *pdwRev = LOBYTE (rHeader.dwVersionNumber);
            bRetVal = TRUE;
        }
        fclose (fp);
    }
    return bRetVal;
} // GetDefDateFromDir()


/*@API:**********************************************************************
@Declaration:
    NAVSDKAPI WORD WINAPI VirusLibraryInit (
        LPVIRUSLIBINIT lpVirLibInit );

@Description:
    This routine initializes the virus scanning core
    using the data provided in the init structure,

@Parameters:
    $HVirLibInit$ Pointer to an init structure

@Returns:
    VIRLIB_OK on success.
    VIRLIB_ERROR otherwise.

@See:

@Include:
    navsdk.h

@Compatibility: W32
****************************************************************************/
NAVSDKAPI WORD WINAPI VirusLibraryInit (// Initialize virus library
    LPVIRUSLIBINIT lpVirLibInit )
{
    auto    LPOEMCONTEXT    lpOEMContext;
    auto    LPOEMCONTEXT    lpCopyOfOEMContext;
    auto    HVIRUSDEF       hDefUtils;
    auto    TCHAR           szAppID [ VIRLIB_MAX_APP_ID_SIZE + 1 ];
    auto    BOOL            bDefsSuccess;
    auto    BOOL            bNoAppID = FALSE;

    // Ensure NULL returned for scanner handle on error (even with
    // VIRLIB_ERROR result).

    lpVirLibInit->hScannerHandle = NULL;

    // Get AppID to use for definitions.
    // Simple method will automatically use newest on product's AppID,
    // then create temporary ID to track usage for this scan.
    // Advanced method will use the AppID passed by the client w/o
    // any calls to VirusDefUseNewest().

    if ( ( EOS == lpVirLibInit->szAppID[0] ) && 
         ( EOS == lpVirLibInit->szVirusDefDir[0] ) )
        return ( VIRLIB_ERROR );

    if ( EOS == lpVirLibInit->szAppID[0] )
        bNoAppID = TRUE;

    if ( !bNoAppID )
    {
        STRCPY( szAppID, lpVirLibInit->szAppID );

        if ( lpVirLibInit->bManageVirusDefs )
        {
            // Update product's AppID automatically.

            if ( VirusDefInit( &hDefUtils ) )
            {
                if ( VirusDefInitWindowsApp( hDefUtils, szAppID ) )
                     VirusDefUseNewest( hDefUtils, lpVirLibInit->szVirusDefDir, sizeof( lpVirLibInit->szVirusDefDir ), NULL );

                VirusDefClose( hDefUtils );
            }

            // Get temp ID and register it with the newest definitions.

            bDefsSuccess = FALSE;

            if ( VirusDefInit( &hDefUtils ) )
            {
                if ( VirusDefGetUniqueAppID( hDefUtils, NULL, szAppID, sizeof( szAppID ) ) &&
                     VirusDefInitWindowsApp( hDefUtils, szAppID ) &&
                     VirusDefUseNewest( hDefUtils, lpVirLibInit->szVirusDefDir, sizeof( lpVirLibInit->szVirusDefDir ), NULL ) )
                {
                    bDefsSuccess = TRUE;
                }

                VirusDefClose( hDefUtils );
            }

            if ( FALSE == bDefsSuccess )
                return ( VIRLIB_ERROR );
        }        

        // Get definitions location.

        bDefsSuccess = FALSE;

        if ( VirusDefInit( &hDefUtils ) )
        {
            if ( VirusDefInitWindowsApp( hDefUtils, szAppID ) &&
                 VirusDefGetCurrent( hDefUtils, lpVirLibInit->szVirusDefDir, sizeof( lpVirLibInit->szVirusDefDir ) ) )
            {
                bDefsSuccess = TRUE;
            }

            VirusDefGetCurrentDate( hDefUtils,
                                    &lpVirLibInit->wVirusDateYear,
                                    &lpVirLibInit->wVirusDateMonth,
                                    &lpVirLibInit->wVirusDateDay,
                                    &lpVirLibInit->dwVirusDefRev );

            VirusDefClose( hDefUtils );
            hDefUtils = NULL;
        }

        if ( FALSE == bDefsSuccess )
            return ( VIRLIB_ERROR );
    }
    else    // If no app id then get the Virus Def Date directly from dir
    {
        GetDefDateFromDir( lpVirLibInit->szVirusDefDir,
                           &lpVirLibInit->wVirusDateYear,
                           &lpVirLibInit->wVirusDateMonth,
                           &lpVirLibInit->wVirusDateDay,
                           &lpVirLibInit->dwVirusDefRev );
    }

    // Set what functions are enabled.

    // Version 3 is a custom interface version originally for BlackHawk - 
    // NAV for Lotus Notes.
    // For version 3, the VIRLIBINIT.FuncEnable member is treated as input.
    // Clients MUST set flags for all functionality desired.
    // This allows client apps to prevent drive pinging by CreateDeviceList()
    // by not selecting any FuncEnable.SysAreaFlags settings.

    if ( 3 != lpVirLibInit->dwInterfaceVersion )
    {
        lpVirLibInit->FuncEnable.MemFlags     = VIRLIB_SCANMEMORY;
        lpVirLibInit->FuncEnable.FileFlags    = VIRLIB_SCANFILES | VIRLIB_DELFILES;
        lpVirLibInit->FuncEnable.SysAreaFlags = VIRLIB_SCANSYSAREA | VIRLIB_LOGICALDRV;
    }            

    // NT? No memory scan, and no logical drive scanning.

    if ( SYM_SYSTEM_WIN_NT == SystemGetWindowsType() )
    {
        lpVirLibInit->FuncEnable.MemFlags = 0;
        lpVirLibInit->FuncEnable.SysAreaFlags &= VIRLIB_SCANSYSAREA;
    }            

    // NEC hardware? Don't allow SysArea scanning

    if ( HWIsNEC() )
    {
        lpVirLibInit->FuncEnable.SysAreaFlags = 0;
    }

    //--------------------------------------------------------------------
    // Allocate and save per thread (per scan) context information so this
    // is available in callbacks which don't return user data.
    // For the duration of this function, we'll be operating on the 
    // original context information.
    // On exit, we will create a copy and store a pointer in the per thread
    // scan context storage. All other functions will work on the copied
    // information - this is to allow storage of per thread last virus info.
    //--------------------------------------------------------------------

    lpOEMContext = (LPOEMCONTEXT)MemAllocPtr( GHND, sizeof( OEMVIRUSCONTEXT ) );
    lpCopyOfOEMContext = (LPOEMCONTEXT)MemAllocPtr( GHND, sizeof( OEMVIRUSCONTEXT ) );

    if ( ( NULL == lpOEMContext ) || ( NULL == lpCopyOfOEMContext ) )
    {
        if ( lpOEMContext )
            MemFreePtr( lpOEMContext );
    
        if ( lpCopyOfOEMContext )
            MemFreePtr( lpCopyOfOEMContext );                        
        
        // Free temp App ID, if in auto definitions handling mode.
        
        if ( !bNoAppID &&
             lpVirLibInit->bManageVirusDefs &&
             VirusDefInit( &hDefUtils ) )
        {
            if ( VirusDefInitWindowsApp( hDefUtils, szAppID ) )
                VirusDefStopUsingDefs( hDefUtils );

            VirusDefClose( hDefUtils );
        }

        return ( VIRLIB_ERROR );
    }

    lpOEMContext->dwOEMSignature          = OEM_SIGNATURE;
    lpOEMContext->wHeuristicScanningLevel = lpVirLibInit->wHeuristicScanningLevel;
    lpOEMContext->bDisableNAVAPBySession  = lpVirLibInit->bDisableNAVAPBySession;
    lpOEMContext->bManageVirusDefs        = lpVirLibInit->bManageVirusDefs && !bNoAppID;
    lpOEMContext->lpfMemProgress          = lpVirLibInit->Progress;
    STRCPY( lpOEMContext->szVirusDefDir, lpVirLibInit->szVirusDefDir );
    STRCPY( lpOEMContext->szAppID, szAppID );
    MEMCPY( &lpOEMContext->FuncEnable,
            &lpVirLibInit->FuncEnable,
            sizeof( VIRLIBFUNCFLAGS ) );

    // Get NAVSDK.INF location.

    GetModuleFileName( g_hInstNAVSDK, lpOEMContext->szNavexInf, sizeof( lpOEMContext->szNavexInf ) - 1 );
    NameStripFile( lpOEMContext->szNavexInf );
    NameAppendFile( lpOEMContext->szNavexInf, _T("NAVSDK.INF") );

    // Save per thread context information.

    SetScanContext( lpOEMContext );

    //-------------------------
    // Initialize virus library
    //-------------------------

    lpOEMContext->hContext = InitializeEngine( lpOEMContext->szVirusDefDir, 
                                               lpOEMContext->szNavexInf );

    // Return if init failed.

    if ( NULL == lpOEMContext->hContext )
    {
        if ( !bNoAppID &&
             lpVirLibInit->bManageVirusDefs &&
             VirusDefInit( &hDefUtils ) )
        {
            if ( VirusDefInitWindowsApp( hDefUtils, szAppID ) )
                VirusDefStopUsingDefs( hDefUtils );

            VirusDefClose( hDefUtils );
        }

        MemFreePtr( lpOEMContext );
        MemFreePtr( lpCopyOfOEMContext );

        SetScanContext( NULL );
        return ( VIRLIB_ERROR );
    }

    // Create list of devices.

    if ( 0 != lpVirLibInit->FuncEnable.SysAreaFlags )
    {
        CreateDeviceList();
    }

    // Deal with AutoProtect issues ...

    gbUseNAVAPDLL = CheckForNewVxDMethod( &ghNAVAP );

    if ( TRUE == lpVirLibInit->bDisableNAVAPBySession )
    {
        AutoProtectOff();
    }

    // Get # of viruses we claim to detect.

    lpVirLibInit->dwVirusDBCount = VLNumClaimedVirusEntries( lpOEMContext->hContext );

    // Return scanner handle (undefined scanner handle is actually
    // OEM context).

    lpVirLibInit->hScannerHandle = (HVIRUSSCANNER)lpOEMContext;

    // Switch stored per thread context info to the copy of the original.
    // We need to keep separate copies per thread so last virus found info
    // is unique per thread using the same context.

    MEMCPY( lpCopyOfOEMContext, lpOEMContext, sizeof( *lpOEMContext ) );
    SetScanContext( lpCopyOfOEMContext );

    return ( VIRLIB_OK );
} // VirusLibraryInit()


/*@API:**********************************************************************
@Declaration:
    NAVSDKAPI VOID WINAPI VirusLibraryClose (
        HVIRUSSCANNER hScannerHandle )

@Description:
    This function unloads the virus scanning core.

@Parameters:
    $DWORD$ The handle to the virus context to close.

@Returns:
    Nothing.

@See:

@Include:
    navsdk.h

@Compatibility: W32
****************************************************************************/
NAVSDKAPI VOID WINAPI VirusLibraryClose (   // Unload virus library
    HVIRUSSCANNER   hScannerHandle )    // [in] Scanner Handle
{
    auto    HVIRUSDEF       hDefUtils;
    auto    LPOEMCONTEXT    lpOEMContext;

    if ( ValidOEMContext( hScannerHandle, &lpOEMContext ) &&
         SynchronizeScanContext( lpOEMContext ) )
    {
        VLScanClose ( lpOEMContext->hContext );

        if ( lpOEMContext->bManageVirusDefs )
        {
            if ( VirusDefInit( &hDefUtils ) )
            {
                if ( VirusDefInitWindowsApp( hDefUtils, lpOEMContext->szAppID ) )
                    VirusDefStopUsingDefs( hDefUtils );

                VirusDefClose( hDefUtils );
            }
        }

        if ( 0 != lpOEMContext->FuncEnable.SysAreaFlags )
        {
            DestroyDeviceList();
        }

        if ( lpOEMContext->bDisableNAVAPBySession )
        {
            AutoProtectOn();
        }

        MemFreePtr ( (LPVOID)lpOEMContext );

        // Free per thread copy of scan context.

        lpOEMContext = GetScanContext();

        if ( NULL != lpOEMContext )
        {
            MemFreePtr( lpOEMContext );
        }

        SetScanContext( NULL );
    }
} // VirusLibraryClose()


/*@API:**********************************************************************
@Declaration:
    NAVSDKAPI WORD WINAPI VirusLibraryDeleteFile (
        HVIRUSSCANNER   hScannerHandle,
        LPCSTR          lpszFile )

@Description:
    This function deletes and wipes the specified file if it is infected.

@Parameters:
    $DWORD$  The handle to the virus context.
    $LPCSTR$ Name of the file to delete

@Returns:
    VIRLIB_OK       if deleted
    VIRLIB_INFECTED if infected or should not be deleted
    VIRLIB_ERROR    if an error is returned during scan or delete.

@See:

@Include:
    navsdk.h

@Compatibility: W32
****************************************************************************/
NAVSDKAPI WORD WINAPI VirusLibraryDeleteFile (  // Delete File
    HVIRUSSCANNER   hScannerHandle,     // [in] Scanner Handle
    LPCSTR          lpszFile)           // [in] File to delete
{
    auto    HVLVIRUS     hVirus;
    auto    LPOEMCONTEXT lpOEMContext;
    auto    VSTATUS      nStatus;
    auto    WORD         wRet = VIRLIB_ERROR;

    if ( !ValidOEMContext( hScannerHandle, &lpOEMContext) ||
         !SynchronizeScanContext( lpOEMContext ) )
        return ( wRet );

    // See if the function is enabled.

    if ( 0 == ( lpOEMContext->FuncEnable.FileFlags & VIRLIB_DELFILES ) )
    {
        wRet = VIRLIB_NOT_ENAB;
        return ( wRet );
    }

    AutoProtectOff();

    nStatus = VLScanFile( lpOEMContext->hContext,
                          (LPSTR)lpszFile,
                          &hVirus );

    if ( ( VS_OK == nStatus ) && ( NULL != hVirus ) )
    {
        if ( VLCanDeleteItem( hVirus ) )
        {
            // Delete the file.
            // Note: Delete function should overwrite the file data for safety.
            //  Should also truncate to 0 bytes before delete so it doesn't 
            //  wind up in NU's protected recycle bin.
            
            if ( NOERR == FileWipe( lpszFile, 10 ) )
            {    
                wRet = VIRLIB_OK;
            }
        }
        else
        {
            wRet = VIRLIB_INFECTED;
        }

        VLReleaseVirusHandle (hVirus);
    }
    else                                // If the scanner returned an error,
    {                                   // Check for a zero-byte file.
        HFILE           fhandle;

        fhandle = FileOpen( lpszFile, OF_READ );

        if ( fhandle != HFILE_ERROR )
        {
            if ( 0 == FileLength( fhandle ) )
            {
                // Delete the file.

                FileClose( fhandle );
                if ( NOERR == FileWipe( lpszFile, 10 ) )
                {    
                    wRet = VIRLIB_OK;   // Zero-byte files are really ok to delete.
                }                       // This really should be checked in the engine.
            }                           
            else
            {
                FileClose( fhandle );
            }
        }
    }

    lpOEMContext->vStatus = nStatus;

    AutoProtectOn();

    return ( wRet );
} // VirusLibraryDeleteFile()



/*@API:**********************************************************************
@Declaration:
    NAVSDKAPI WORD WINAPI VirusLibraryGetLastVirusInfo (
        HVIRUSSCANNER hScannerHandle,
        LPVIRUSINFO   lpVirInfo )

@Description:
    This function provides extended information about the last
    virus found.

@Parameters:
    $hScannerHandle$    The handle to the virus context.
    $lpVirInfo$         Pointer to a structure to get virus information

@Returns:
    VIRLIB_OK    if successful
    VIRLIB_ERROR if the scanner handle or info structure pointer are NULL.

@See:

@Include:
    navsdk.h

@Compatibility: W32
****************************************************************************/
NAVSDKAPI WORD WINAPI VirusLibraryGetLastVirusInfo (    // Get more virus info
    HVIRUSSCANNER hScannerHandle,       // [in] Scanner Handle
    LPVIRUSINFO   lpVirInfo )           // [out] Buffer to get virus info
{
    auto    LPOEMCONTEXT lpOEMContext;
    auto    WORD         wRet = VIRLIB_ERROR;

    if ( ValidOEMContext( hScannerHandle, &lpOEMContext) &&
         ( NULL != ( lpOEMContext = GetScanContext() ) ) &&
         ( lpOEMContext->hContext == ((LPOEMCONTEXT)hScannerHandle)->hContext ) )
    {
         MEMCPY( lpVirInfo,
                 &lpOEMContext->VirInfo,
                 sizeof( VIRUSINFO ) );

         wRet = VIRLIB_OK;
    }

    return ( wRet );
} // VirusLibraryGetLastVirusInfo()


/*@API:**********************************************************************
@Declaration:
    NAVSDKAPI WORD WINAPI VirusLibraryScanFile (
        HVIRUSSCANNER   hScannerHandle,
        LPCSTR          lpszFile,
        LPSTR           lpszVirusNameBuf )

@Description:
    This function scans the specified file to see if it is infected.  If it is
    infected the function returns TRUE and sets the name of the virus in
    lpszVirusNameBuf.

@Parameters:
    $hScannerHandle$    The handle to the virus context.
    $lpszFile$          Name of the file to scan
    $lpszVirusNameBuf$  Buffer to receive virus name if the file is infected.

@Returns:
    VIRLIB_OK       if not infected
    VIRLIB_INFECTED if infected
    VIRLIB_ERROR    if an error is returned during the scan.

@See:

@Include:
    navsdk.h

@Compatibility: W32
****************************************************************************/
NAVSDKAPI WORD WINAPI VirusLibraryScanFile (    // Scans File for viruses
    HVIRUSSCANNER   hScannerHandle,     // [in] Scanner Handle
    LPCSTR          lpszFile,           // [in] File to scan
    LPSTR           lpszVirusNameBuf )  // [out] Buffer to receive virus name
{
    auto    HVLVIRUS     hVirus = NULL;
    auto    LPOEMCONTEXT lpOEMContext;
    auto    VSTATUS      nStatus;
    auto    WORD         wRet = VIRLIB_ERROR;

    AutoProtectOff();

    // Verify valid OEM context.

    if ( !ValidOEMContext( hScannerHandle, &lpOEMContext) ||
         !SynchronizeScanContext( lpOEMContext ) ||
         ( NULL == ( lpOEMContext = GetScanContext() ) ) )
        return ( wRet );


    // If file scanning isn't enabled return error.

    if ( 0 == lpOEMContext->FuncEnable.FileFlags )
    {
        wRet = VIRLIB_NOT_ENAB;
        return ( wRet );
    }

    // Scan the specified file to see if it is infected.

    nStatus = VLScanFile( lpOEMContext->hContext,
                          (LPSTR)lpszFile,
                          &hVirus );

    if ( VS_OK == nStatus )
    {
        if ( NULL != hVirus )
        {
            // We have a virus!!!

            wRet = VIRLIB_INFECTED;

            GetVirusInfo( hVirus,
                          NULL,
                          &lpOEMContext->VirInfo,
                          lpszVirusNameBuf );

            MEMCPY( &((LPOEMCONTEXT)hScannerHandle)->VirInfo,
                    &lpOEMContext->VirInfo,
                    sizeof( VIRUSINFO ) );

            // Release the virus handle since we have the info we need about
            // the virus found in this file.

            VLReleaseVirusHandle( hVirus );
        }
        else
        {
            // The file was clean of viruses.

            wRet = VIRLIB_CLEAN;
        }
    }
    else                                // If the scanner returned an error,
    {                                   // Check for a zero-byte file.
        auto    HFILE   fhandle;

        fhandle = FileOpen( lpszFile, OF_READ );

        if ( HFILE_ERROR != fhandle )
        {
            if ( 0 == FileLength( fhandle ) )
            {
                wRet = VIRLIB_CLEAN;    // Zero-byte files are really ok. This
            }                           // really should be checked in the engine.

            FileClose(fhandle);
        }
    }

    lpOEMContext->vStatus = nStatus;

    AutoProtectOn();

    return ( wRet );
} // VirusLibraryScanFile()


/*@API:**********************************************************************
@Declaration:
    NAVSDKAPI WORD WINAPI VirusLibraryScanMem (
        HVIRUSSCANNER   hScannerHandle,
        WORD            wMemFlags,
        LPVOID          lpvUserData,
        LPSTR           lpszVirusNameBuf )

@Description:
    This function scans memory to see if it is infected.  If it is
    infected the function returns VIRLIB_INFECTED and sets the name of
    the virus in lpszVirusNameBuf.

@Parameters:
    $hScannerHandle$    The handle to the virus context.
    $wMemFlags$         Flags specifying what part of memory to scan.
    $lpvUserData$       User data for optional progress callback.
    $lpszVirusNameBuf$  Buffer to receive virus name if memory is infected.

@Returns:
    VIRLIB_OK       if memory is not infected
    VIRLIB_INFECTED if memory infected
    VIRLIB_ERROR    if an error is returned during the scan.

@See:
    VirusLibraryInit()
    VIRLIBINIT.Progress

@Include:
    navsdk.h

@Compatibility: W32
****************************************************************************/
NAVSDKAPI WORD WINAPI VirusLibraryScanMem ( // Scans Memory for viruses
    HVIRUSSCANNER   hScannerHandle,     // [in] Scanner Handle
    WORD            wMemFlags,          // [in] Memory scanning flags
    LPVOID          lpvUserData,        // [in] User data for optional progress callback.
    LPSTR           lpszVirusNameBuf )  // [out] Buffer to receive virus name
{
    auto    HVLVIRUS     hVirus;
    auto    LPOEMCONTEXT lpOEMContext;
    auto    VSTATUS      nStatus;
    auto    WORD         wRet = VIRLIB_ERROR;


    if ( !ValidOEMContext( hScannerHandle, &lpOEMContext) ||
         !SynchronizeScanContext( lpOEMContext ) ||
         ( NULL == ( lpOEMContext = GetScanContext() ) ) )
        return ( wRet );

    // See if the function is enabled

    if ( 0 == lpOEMContext->FuncEnable.MemFlags )
    {
        return ( VIRLIB_NOT_ENAB );
    }

#ifdef SYM_WIN32
#ifndef _ALPHA_

                                        // Startup the VxD (NAVSDK.VXD) on Win95.
                                        // Must be present to do mem scanning.
                                        // Actually, it only gets initialized
                                        // once, but this call lets us check
                                        // success or failure of that initialization.

    if ( ( SYM_SYSTEM_WIN_95 == SystemGetWindowsType() ) &&
         ( FALSE == VxDServicesInit() ) )
    {
        return ( VIRLIB_ERROR );
    }
#endif //SYM_CPU_ALPHA
#endif

    AutoProtectOff();

    // Preparation for memory callback. 
    // Need to save user data in our klugey per thread data so our callback 
    // can get it. Also need to set flag indicating we're scanning memory and 
    // should pass progress message through to the client's progress function
    // (we don't show progress for any other operations).

    lpOEMContext->bScanningMem   = TRUE;
    lpOEMContext->lpvMemUserData = lpvUserData;

    //
    // Scan memory
    // 

    nStatus = VLScanMemory( lpOEMContext->hContext,
                            wMemFlags,
                            &hVirus );

    lpOEMContext->bScanningMem = FALSE;

    if ( VS_OK == nStatus )
    {
        if ( NULL != hVirus )
        {
            // We have a virus!!!

            wRet = VIRLIB_INFECTED;

            GetVirusInfo( hVirus,
                          NULL,
                          &lpOEMContext->VirInfo,
                          lpszVirusNameBuf );

            MEMCPY( &((LPOEMCONTEXT)hScannerHandle)->VirInfo,
                    &lpOEMContext->VirInfo,
                    sizeof( VIRUSINFO ) );

            VLReleaseVirusHandle( hVirus );
        }
        else
        {
            // The file was clean of viruses.

            wRet = VIRLIB_CLEAN;
        }
    }

    lpOEMContext->vStatus = nStatus;

    AutoProtectOn();

    // Return to caller

    return ( wRet );
} // VirusLibraryScanMem()


/*@API:**********************************************************************
@Declaration:
    NAVSDKAPI WORD WINAPI VirusLibraryScanSysArea (
        HVIRUSSCANNER   hScannerHandle,
        WORD            wDriveFlags,
        LPBYTE          lpDrive,
        LPSTR           lpszVirusNameBuf )

@Description:
    This function scans the system area of a drive to see if it is
    infected.  If it is infected the function returns VIRLIB_INFECTED
    and sets the name of the virus in lpszVirusNameBuf.

    The system area currently is defined as the MBR and boot sectors of
    all partitions on a drive.

    A single drive can be scanned by pointing lpDrive to a physical
    drive number and setting the VIRLIB_SCAN_DRIVE_FLAG in wDriveFlags.

    All hard drives can be scanned by setting the VIRLIB_FIXED_FLAG. All
    floppy drives can be scanned by setting the VIRLIB_REMOVABLE_FLAG. All
    drives on a system can be scanned by setting both flags.

    If a virus is found when one or both of these flags are set, the drive
    where the virus was found is returned in lpDrive. The partition number
    or MBR info is available by calling VirusLibraryGetLastVirusInfo.

    Logical or physical drive designators are allowed. Logical drives are the
    default. If physical drive numbers are to be used, then VIRLIB_PHYSICAL_DRIVES
    should be set in wDriveFlags. Logical drives are not allowed in NT.

@Parameters:
    $hScannerHandle$    The handle to the virus context.
    $wDriveFlags$       Drive scanning flags.
    $lpDrive$           Drive designator
    $lpszVirusNameBuf$  Buffer to receive virus name if system area infected.

@Returns:
    VIRLIB_OK              if not infected
    VIRLIB_INFECTED        if infected
    VIRLIB_ERROR           if an error is returned during the scan.
    VIRLIB_NO_LOGICAL      if logical drive mapping is not supported
    VIRLIB_NOT_BOOT_DEVICE if logical drive is not part of a physical device.

@See:

@Include:
    navsdk.h

@Compatibility: W32
****************************************************************************/
NAVSDKAPI WORD WINAPI VirusLibraryScanSysArea ( // Scans system areas for viruses
    HVIRUSSCANNER   hScannerHandle,     // [in] Scanner Handle
    WORD            wDriveFlags,        // [in] Drive scanning flags
    LPBYTE          lpDrive,            // [in|out] Drive number designator
    LPSTR           lpszVirusNameBuf )  // [out] Buffer to receive virus name
{
    auto    HVLVIRUS        hVirus = NULL;
    auto    LPOEMCONTEXT    lpOEMContext;
    auto    VSTATUS         nStatus;
    auto    LPDEVICEINFOREC lpDeviceInfo;
    auto    WORD            wRet = VIRLIB_CLEAN;
    auto    BOOL            bPhysDrvFlag;

    if ( !ValidOEMContext( hScannerHandle, &lpOEMContext) ||
         !SynchronizeScanContext( lpOEMContext ) ||
         ( NULL == ( lpOEMContext = GetScanContext() ) ) )
    {
        wRet = VIRLIB_ERROR;
        goto Ret;
    }

    // See if the function is enabled.

    if ( 0 == lpOEMContext->FuncEnable.SysAreaFlags )
    {
        wRet = VIRLIB_NOT_ENAB;
        goto Ret;
    }

    // Check whether we're to scan logical or physical drives.

    if ( VIRLIB_PHYSICAL_DRIVES & wDriveFlags )
    {
        bPhysDrvFlag = TRUE;
    }
    else 
    {
        if ( 0 == ( VIRLIB_LOGICALDRV & lpOEMContext->FuncEnable.SysAreaFlags ) )
        {
            wRet = VIRLIB_NO_LOGICAL;
            goto Ret;
        }

        bPhysDrvFlag = FALSE;
    }

    // Turn autoprotect off while we scan.

    AutoProtectOff();

    //
    // Go through all the drive flags.
    //

    // Scan a specific drive?

    if ( VIRLIB_SCAN_DRIVE_FLAG & wDriveFlags )
    {
        // Must point somewhere, or it's an error.

        if ( lpDrive == NULL )
        {
            wRet = VIRLIB_ERROR;
            goto Ret;
        }

        // Do the scan.

        nStatus = ScanDrive( lpOEMContext->hContext,
                                bPhysDrvFlag,
                                *lpDrive,
                                &hVirus );

        // Check results. We don't continue. They asked for
        // a single drive.

        if ( VS_OK == nStatus )
        {
            if ( hVirus != NULL )
            {
                lpDeviceInfo = bPhysDrvFlag ? GetPhysicalDevice( *lpDrive ) 
                                            : GetLogicalDrive( *lpDrive );
                wRet = VIRLIB_INFECTED;
            }
        }
        else
        {
            if ( VIRLIB_NOT_BOOT_DEVICE == nStatus )
            {
                wRet = VIRLIB_NOT_BOOT_DEVICE;
            }
        }
        goto Ret;
    }

    if ( VIRLIB_FIXED_FLAG & wDriveFlags )
    {
        // Scan all fixed drives.

        auto    BOOL    bDriveScanned[256] = { FALSE };

        lpDeviceInfo = GetDeviceListHead();

        while ( NULL != lpDeviceInfo )
        {
            if ( !bDriveScanned[ lpDeviceInfo->uPhysicalDevice ] )
            {
                nStatus = ScanDrive( lpOEMContext->hContext,
                                     TRUE,
                                     (BYTE)lpDeviceInfo->uPhysicalDevice,
                                     &hVirus );

                bDriveScanned[ lpDeviceInfo->uPhysicalDevice ] = TRUE;

                if ( nStatus == VS_OK )
                {
                    if ( hVirus != NULL )
                    {
                        wRet = VIRLIB_INFECTED;
                        goto Ret;
                    }
                }
                else
                {
                    if ( VS_NO_ACCESS == nStatus )
                        wRet = VIRLIB_NOT_BOOT_DEVICE;

                    goto Ret;
                }
            }

            lpDeviceInfo = GetNextDevice( lpDeviceInfo );
        }
    }

    if ( wDriveFlags & VIRLIB_REMOVABLE_FLAG )
    {
        // Scan all FLOPPY drives.

        lpDeviceInfo = GetPhysicalDevice( 0x00 );

        while ( NULL != lpDeviceInfo )
        {
            nStatus = ScanDrive( lpOEMContext->hContext,
                                 TRUE,
                                 (BYTE)lpDeviceInfo->uPhysicalDevice,
                                 &hVirus );

            if ( VS_OK == nStatus )
            {
                if ( NULL != hVirus )
                {
                    wRet = VIRLIB_INFECTED;
                    goto Ret;
                }
            }
            else
            {
                wRet = VIRLIB_ERROR;
                goto Ret;
            }

            lpDeviceInfo = GetNextRemovableDevice( lpDeviceInfo );
        }
    }    

Ret:
    if ( VIRLIB_INFECTED == wRet )
    {        
        // Get virus information and put it in our structure.

        GetVirusInfo ( hVirus,
                       lpDeviceInfo,
                       &lpOEMContext->VirInfo,
                       lpszVirusNameBuf );

        MEMCPY( &((LPOEMCONTEXT)hScannerHandle)->VirInfo,
                &lpOEMContext->VirInfo,
                sizeof( VIRUSINFO ) );

        // Return the infected drive.

        if ( bPhysDrvFlag == TRUE )
            *lpDrive = lpOEMContext->VirInfo.byInfectedPhysicalDrive;
        else
            *lpDrive = lpOEMContext->VirInfo.byInfectedLogicalDrive;

        // Release the handle.

        VLReleaseVirusHandle( hVirus );
    }

    // Turn autoprotect back on.

    AutoProtectOn();

    lpOEMContext->vStatus = nStatus;

    return ( wRet );
} // VirusLibraryScanSysArea()


/*@API:**********************************************************************
@Declaration:
    NAVSDKAPI WORD WINAPI VirusLibraryScanSysBuffer (
        HVIRUSSCANNER   hScannerHandle,
        WORD            wBufferFlags,
        LPBYTE          lpSysBuffer,
        LPSTR           lpszVirusNameBuf )

@Description:
    This routine scans a memory image of a boot sector for viruses.
    If wBufferFlags is set to VIRLIB_BOOT_RECORD_BUFFER, the image in 
    lpszSysBuffer is scanned for boot sector viruses, otherwise, the
    buffer is scanned for master boot record viruses.

@Parameters:
    $hScannerHandle$    The handle to the virus context.
    $wDriveFlags$       Buffer content flags. 
                            VIRLIB_BOOT_RECORD_BUFFER or 
                            VIRLIB_MASTER_BOOT_RECORD_BUFFER
    $lpSysBuffer$       Buffer with boot record image. Must be size
                            VIRLIB_BOOT_BUFFER_SIZE
    $lpszVirusNameBuf$  Buffer to receive virus name if system area infected.

@Returns:
    VIRLIB_OK              if not infected
    VIRLIB_INFECTED        if infected
    VIRLIB_ERROR           if an error is returned during the scan.

@See:

@Include:
    navsdk.h

@Compatibility: W32
****************************************************************************/
NAVSDKAPI WORD WINAPI VirusLibraryScanSysBuffer (   // Scans a buffer for boot viruses
    HVIRUSSCANNER   hScannerHandle,         // [in] Scanner Handle
    WORD            wBufferFlags,           // [in] Flags describing buffer content
    LPBYTE          lpSysBuffer,            // [in] Buffer with boot sector image
    LPSTR           lpszVirusNameBuf)       // [out] Buffer to receive virus name
{
    auto    HVLVIRUS        hVirus = NULL;
    auto    LPOEMCONTEXT    lpOEMContext;
    auto    VSTATUS         nStatus;
    auto    WORD            wRet;

    if ( !ValidOEMContext( hScannerHandle, &lpOEMContext) ||
         !SynchronizeScanContext( lpOEMContext ) ||
         ( NULL == ( lpOEMContext = GetScanContext() ) ) )
        return ( VIRLIB_ERROR );

    // This functionality is available for all platforms.
    //
    //if ( 0 == lpOEMContext->FuncEnable.SysAreaFlags )
    //{
    //    wRet = VIRLIB_NOT_ENAB;
    //    goto Ret;
    //}

    // Turn autoprotect off while we scan.

    AutoProtectOff();

    // Scan now for either boot sector or master boot record viruses.

    if ( wBufferFlags & VIRLIB_BOOT_RECORD_BUFFER )
    {
        nStatus = VLScanBoot( lpOEMContext->hContext,
                              0,
                              0,
                              lpSysBuffer,
                              &hVirus );
    }
    else
    {
        nStatus = VLScanPart( lpOEMContext->hContext,
                              0,
                              lpSysBuffer,
                              &hVirus );
    }

    if ( VS_OK != nStatus )
    {
        // Error scanning
        
        wRet = VIRLIB_ERROR;
    }        
    else if ( NULL != hVirus )
    {
        // Buffer is infected 
        
        wRet = VIRLIB_INFECTED;
        
        // Get virus information and put it in our structure.

        GetVirusInfo( hVirus,
                      NULL,
                      &lpOEMContext->VirInfo,
                      lpszVirusNameBuf );

        MEMCPY( &((LPOEMCONTEXT)hScannerHandle)->VirInfo,
                &lpOEMContext->VirInfo,
                sizeof( VIRUSINFO ) );

        // Release the handle.

        VLReleaseVirusHandle( hVirus );
    }
    else 
    {
        //  Buffer is clean
        
        wRet = VIRLIB_CLEAN;
    }

    // Turn autoprotect back on.

    AutoProtectOn();

    lpOEMContext->vStatus = nStatus;

    return ( wRet );
} // VirusLibraryScanSysArea()




/***************************************************************************
@Declaration:
    VOID WINAPI AutoProtectOff (VOID)
    VOID WINAPI AutoProtectOn  (VOID)

@Description:
    The functions invoke the TSR_OFF and TSR_ON macros. These
    call functions to toggle autoprotect.

@Parameters:
    None.

@Returns:
    None.

@See:

@Include:   OEMAPI.H

@Compatibility: W32
****************************************************************************/
VOID WINAPI AutoProtectOff ( VOID )
{
    //&? Must accept scanning instance handle for setting of
    // disable per instance flag.
    // Does AP already track number of enable disables? if so we're set.
    // otherwise, we should add synchronized counts here for MT stuff.
    //&? Why are these here and not in some util thingum?

#ifdef SYM_WIN32
    TalkToVxD( VXDACTION_DISABLE );
#endif
} // AutoProtectOff()

VOID WINAPI AutoProtectOn ( VOID )
{
#ifdef SYM_WIN32
    TalkToVxD( VXDACTION_ENABLE );
#endif
} // AutoProtectOn()


/***************************************************************************
@Declaration:
    static VOID WINAPI GetVirusInfo (
        HVLVIRUS    hVirus,
        LPVIRUSINFO lpVirInfo,
        LPSTR       lpszVirusNameBuf )

@Description:
    This function gets virus info from the engine for the specififed
    hVirus and fills in various fields & flags in a user VirusInfo buffer.

@Parameters:
    $HVLVIRUS$    The handle to the virus context.
    $LPVIRUSINFO$ User buffer for extended virus info
    $LPSTR$       Buffer to receive the name of the virus if the file is
                  infected.

@Returns:
    None

@See:

@Include:

@Compatibility: W32
****************************************************************************/
static VOID WINAPI GetVirusInfo (       // Get virus information
    HVLVIRUS         hVirus,            // [in] Pointer to engine virus buffer
    LPDEVICEINFOREC  lpDeviceInfo,      // [in] Device Info pointer
    LPVIRUSINFO      lpVirInfo,         // [out] Pointer to a user info buffer
    LPSTR            lpszVirusNameBuf ) // [out] Pointer to a virus name buffer.
{
    auto    char    MiscBuffer[ 20 ];
    auto    UINT    MiscVal;

    MEMSET ( MiscBuffer, 0, 20 );
    MEMSET ( lpVirInfo, 0 , sizeof( VIRUSINFO ) );

    // Get all the flags ...

    if ( VLVirusInfo( hVirus, 0, VI_BOOL_INFECT_FILES, NULL ) )
        lpVirInfo->dwVirusFlags |= VIRINFO_INFECT_FILES;

    if ( VLVirusInfo( hVirus, 0, VI_BOOL_INFECT_EXE,   NULL ) )
        lpVirInfo->dwVirusFlags |= VIRINFO_INFECT_EXE;

    if ( VLVirusInfo( hVirus, 0, VI_BOOL_INFECT_COM,   NULL ) )
        lpVirInfo->dwVirusFlags |= VIRINFO_INFECT_COM;

    if ( VLVirusInfo( hVirus, 0, VI_BOOL_INFECT_SYS,   NULL ) )
        lpVirInfo->dwVirusFlags |= VIRINFO_INFECT_SYS;

    if ( VLVirusInfo( hVirus, 0, VI_BOOL_INFECT_MBR,   NULL ) )
        lpVirInfo->dwVirusFlags |= VIRINFO_INFECT_MBR;

    if ( VLVirusInfo( hVirus, 0, VI_BOOL_INFECT_FLOPPY_BOOT, NULL ) )
        lpVirInfo->dwVirusFlags |= VIRINFO_INFECT_FLOPPY;

    if ( VLVirusInfo( hVirus, 0, VI_BOOL_INFECT_HD_BOOT, NULL ) )
        lpVirInfo->dwVirusFlags |= VIRINFO_INFECT_HD_BOOT;

    if ( VLVirusInfo( hVirus, 0, VI_BOOL_INFECT_WILD, NULL ) )
        lpVirInfo->dwVirusFlags |= VIRINFO_INFECT_WILD;

    if ( VLVirusInfo( hVirus, 0, VI_BOOL_MEMORY_RES, NULL ) )
        lpVirInfo->dwVirusFlags |= VIRINFO_INFECT_MEM_RES;

    if ( VLVirusInfo( hVirus, 0, VI_BOOL_SIZE_STEALTH, NULL ) )
        lpVirInfo->dwVirusFlags |= VIRINFO_INFECT_SIZE_ST;

    if ( VLVirusInfo( hVirus, 0, VI_BOOL_FULL_STEALTH, NULL ) )
        lpVirInfo->dwVirusFlags |= VIRINFO_INFECT_FULL_ST;

    if ( VLVirusInfo( hVirus, 0, VI_BOOL_TRIGGER, NULL ) )
        lpVirInfo->dwVirusFlags |= VIRINFO_INFECT_TRIGGER;

    if ( VLVirusInfo( hVirus, 0, VI_BOOL_ENCRYPTING, NULL ) )
        lpVirInfo->dwVirusFlags |= VIRINFO_INFECT_ENCRYPT;

    if ( VLVirusInfo( hVirus, 0, VI_BOOL_POLYMORPHIC, NULL ) )
        lpVirInfo->dwVirusFlags |= VIRINFO_INFECT_POLYMOR;

    if ( VLVirusInfo( hVirus, 0, VI_BOOL_MULTIPART, NULL ) )
        lpVirInfo->dwVirusFlags |= VIRINFO_INFECT_MULTIPRT;

    if ( VLVirusInfo( hVirus, 0, VI_BOOL_REPAIRABLE, NULL ) )
        lpVirInfo->dwVirusFlags |= VIRINFO_CAN_REPAIR;

    if ( VLVirusInfo( hVirus, 0, VI_BOOL_DELETABLE, NULL ) )
        lpVirInfo->dwVirusFlags |= VIRINFO_CAN_DELETE;

    if ( VLVirusInfo( hVirus, 0, VI_BOOL_MACRO_VIRUS, NULL ) )
        lpVirInfo->dwVirusFlags |= VIRINFO_MACRO;

    if ( VLVirusInfo( hVirus, 0, VI_BOOL_WINDOWS_VIRUS, NULL ) )
        lpVirInfo->dwVirusFlags |= VIRINFO_WINDOWS;

    // Get the drive number string and change it back to a number.

    VLVirusInfo( hVirus,
                 0,
                 VI_PHYSICAL_DRIVE_NUM,
                 MiscBuffer );

    ConvertStringToWord( &MiscVal, MiscBuffer );
    lpVirInfo->byInfectedPhysicalDrive = (BYTE)MiscVal;

    // Is it a MBR virus? If so make our partition number be 0xFF.

    if ( VLVirusInfo( hVirus,
                      0,
                      VI_BOOL_PART_ITEM,
                      NULL ) )
    {
        lpVirInfo->byInfectedPartitionNumber = 0xFF;
    }
    else                            
    {                               
        // Else get the partition number string and change it back to a number.

        VLVirusInfo( hVirus,
                     0,
                     VI_PARTITION_NUM,
                     MiscBuffer );

        ConvertStringToWord( &MiscVal, MiscBuffer );
        lpVirInfo->byInfectedPartitionNumber = (BYTE)MiscVal;
    }

    // Get the logical drive if not disembodied memory image scan. 
    // If we have a device info pointer, use it.

    if ( VLVirusInfo( hVirus,
                      0,
                      VI_BOOL_DISEMBODIED,
                      NULL ) )
    {
        lpVirInfo->byInfectedLogicalDrive    = 0xFF;
        lpVirInfo->byInfectedPartitionNumber = 0xFF;
        lpVirInfo->byInfectedPhysicalDrive   = 0xFF;
    }    
    else if ( NULL != lpDeviceInfo )
    {
        lpVirInfo->byInfectedLogicalDrive = lpDeviceInfo->chPhysicalID;
    }
    else
    {
        // Else get it based on device/partition number.

        lpVirInfo->byInfectedLogicalDrive = \
                GetLogDrvFromPhysDev( lpVirInfo->byInfectedPhysicalDrive,
                                      lpVirInfo->byInfectedPartitionNumber );
    }

    // Get the virus size.

    VLVirusInfo( hVirus,
                 0,
                 VI_VIRUS_SIZE,
                 lpVirInfo->szVirusSize );

    // The the infected file name.

    VLVirusInfo( hVirus,
                 0,
                 VI_FILENAME,
                 lpVirInfo->szInfectedFileName );

    // And the virus name. Make sure the string is NULL if there isn't one.

    if ( FALSE == VLVirusInfo( hVirus,
                               0,
                               VI_VIRUS_NAME,
                               lpVirInfo->szVirusName ) )
    {
        lpVirInfo->szVirusName[0] = EOS;
    }
    else
    {
        STRNCPY( lpszVirusNameBuf,
                 lpVirInfo->szVirusName,
                 VIRLIB_VIRUS_NAME_SIZE );

        lpszVirusNameBuf [VIRLIB_VIRUS_NAME_SIZE - 1] = EOS;
    }

    // Get the virus ID.
    lpVirInfo->wVirusID = hVirus->stVirusInfoBlock.wVirID;

    // Last, get the description.

    VLVirusInfo( hVirus,
                 0,
                 VI_VIRUS_INFO,
                 lpVirInfo->szVirusDescription );
} // GetVirusInfo()


/***********************************************************************
@Declaration:
    static VSTATUS WINAPI ScanDrive (
        HVCONTEXT   hContext,
        BOOL        bPhysDrvFlag,
        BYTE        byDrive,
        LPHVLVIRUS  lphVirus)

@Description:
    This function scans the system areas of a specified physical
    drive to see if it is infected. It scans the MBR then the boot
    record of each partition on the drive. If an infection is found, the
    function returns without continuing.

    It is assumed AutoProtect is off when entering the function.

@Parameters:
    $hContext$      Engine handle.
    $bPhysDrvFlag$  TRUE byDrive indicates physical drive, FALSE for logical.
    $bPhysDrv$      Physical drive to scan.
    $lphVirus$      Buffer to receive virus information

@Returns:
    VSTATUS

@See:
    avapi.cpp or documentation
    device.cpp

@Include:
    avapi.h
    device.h

@Compatibility: W32
****************************************************************************/
static VSTATUS WINAPI ScanDrive (       // Scans a physical drive for viruses
    HVCONTEXT   hContext,               // [in] Scanner Context
    BOOL        bPhysDrvFlag,           // [in] Set if drives are physical
    BYTE        byDrive,                // [in] Drive designator
    LPHVLVIRUS  lphVirus)               // [out] Buffer to receive virus info
{
    auto    VSTATUS         nStatus;
    auto    LPDEVICEINFOREC lpDeviceInfo;

    // Get device info.

    if ( bPhysDrvFlag )
        lpDeviceInfo = GetPhysicalDevice( byDrive );
    else
        lpDeviceInfo = GetLogicalDrive( byDrive );

    if ( NULL == lpDeviceInfo )
        return ( VIRLIB_NOT_BOOT_DEVICE );

    // Is the device/partition visible in the current mode?

    if ( lpDeviceInfo->bVisible &&
         ( lpDeviceInfo->uPhysicalDevice >= 0x80 ) )
    {
        // Yep. Scan the MBR.

        nStatus = VLScanPart( hContext,
                              lpDeviceInfo->uPhysicalDevice,
                              NULL,
                              lphVirus );

        // If there was a scanning error, or a virus was found, then return.

        if ( ( VS_OK != nStatus ) ||
             ( NULL != *lphVirus ) )
        {
            return ( nStatus );
        }
    }

    // Scan all partitions for this physical device.

    while ( NULL != lpDeviceInfo )
    {
        // Is the device/partition visible in the current mode, or is it a
        // floppy drive?

        if ( lpDeviceInfo->bVisible ||
             ( lpDeviceInfo->uPhysicalDevice <= 0x80 ) )
        {
            // Yep. Scan.

            nStatus = VLScanBoot( hContext,
                                  lpDeviceInfo->uPhysicalDevice,
                                  (BYTE)lpDeviceInfo->uPartition,
                                  NULL,
                                  lphVirus );

            // If there was a scanning error, or a virus was found, then return.

            if ( ( VS_OK != nStatus ) ||
                 ( NULL != *lphVirus ) )
            {
                return ( nStatus );
            }
        }

        if ( TRUE == bPhysDrvFlag )
            lpDeviceInfo = GetNextPartition( lpDeviceInfo );
        else
            goto ret;
    }

ret:
    return ( nStatus );
} // ScanDrive()


/**************************************************************************
@Declaration:
    BOOL WINAPI ValidOEMContext(
        HVIRUSSCANNER       hScannerHandle,
        LPOEMCONTEXT FAR *  lppOEMContext )

@Description:
    This function verifies that a scanner handle is valid then converts
    the handle to a pointer to a OEM context structure.

@Parameters:
    $hScannerHandle$         NAVSDK scanning handle.
    $lppOEMContext$          Pointer to buffer to receive pointer to OEMContext

@Returns:
    TRUE if the handle is valid, otherwise FALSE.

@See:

@Include:
    oemapi.h

@Compatibility: W32
****************************************************************************/
BOOL WINAPI ValidOEMContext(
    HVIRUSSCANNER       hScannerHandle, // [in] a valid HVCONTEXT handle
    LPOEMCONTEXT FAR *  lpOEMContext)   // [out] receives LPOEMCONTEXT if valid.
{
    // Must...
    //  Have valid address
    //  Have valid signature
    //  Have valid AVAPI context
    
    if ( ( NULL == hScannerHandle ) ||
         ( OEM_SIGNATURE != ((LPOEMCONTEXT)hScannerHandle)->dwOEMSignature ) ||
         ( !VLValidContext( ((LPOEMCONTEXT)hScannerHandle)->hContext) ) )
        return( FALSE );

    *lpOEMContext = (LPOEMCONTEXT)hScannerHandle;

    return( TRUE );
} // ValidOEMContext()

