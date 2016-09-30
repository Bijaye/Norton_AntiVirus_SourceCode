// Copyright 1995 Symantec, Peter Norton Product Group
//***************************************************************************
//
// $Header:   S:/NAVFAMLY/VCS/static.cpv   1.6   28 Apr 1997 13:51:20   MKEATIN  $
//
// This file defines functions for communicating with external AV functions.
// This file has to be statically linked to AVAPI.
//
// Contains:
//      EXTInitVxD()
//      EXTCloseVxD()
//
//***************************************************************************
// $Log:   S:/NAVFAMLY/VCS/static.cpv  $
// 
//    Rev 1.6   28 Apr 1997 13:51:20   MKEATIN
// EXTInitDX() should be initialized once and never closed. EXTCloseDX() simply
// returns EXTSTATUS_OK.
// 
//    Rev 1.5   28 Apr 1997 13:46:44   MKEATIN
// Added DX support
// 
//    Rev 1.4   25 Jul 1996 19:00:04   DALLEE
// Added EXTScanWin() and EXTCloseWin() for dynamically loading 
// NAVEX DLL for each HVCONTEXT.
// 
//    Rev 1.3   01 Mar 1996 15:31:52   JWORDEN
// DBCS enable
// 
//    Rev 1.2   07 Feb 1996 09:12:58   RSTANEV
// Cleaned up.
//
//    Rev 1.1   11 Jan 1996 07:51:56   JWORDEN
// Move Quake 8 to trunk (new file in quake 8)
//
//    Rev 1.0.1.2   17 Nov 1995 10:18:30   CNACHEN
// Changed VAVEX to NAVEX...
//
//    Rev 1.0.1.1   16 Oct 1995 18:25:10   DALLEE
// AVEX.H to NAVEX.H
//
//    Rev 1.0.1.0   11 Aug 1995 14:48:10   RSTANEV
// Branch base for version QAK8
//
//    Rev 1.0   11 Aug 1995 14:44:26   RSTANEV
// Initial revision.
//
//***************************************************************************

#include "platform.h"
#include <stdio.h>
#include "xapi.h"
#include "avapi.h"
#include "navex.h"

#if defined (SYM_VXD)

#define VAVEX_GET_EXPORT_TABLE 2

static TCHAR szExportFileName[]   = _T("NAVEX.VXD");
static TCHAR szExportModuleName[] = _T("NAVEX");

//---------------------------------------------------------------------------
//
// EXTSTATUS WINAPI EXTInitVxD (
//      HVCONTEXT hContext
//      );
//
// This function initializes hContext such that the external AV APIs can
// be called.
//
//---------------------------------------------------------------------------
EXTSTATUS WINAPI EXTInitVxD (
    HVCONTEXT hContext
    )
    {
    auto PEXPORT_TABLE_TYPE prExportTable;
    auto TCHAR              szExportFileFull[SYM_MAX_PATH * sizeof(WCHAR)];
    auto TCHAR              szExportFileShort[SYM_MAX_PATH * sizeof(WCHAR)];
    auto PVMMDDB            prDDB;
    auto DeviceInfo         *prDeviceInfo;
    auto DIOCPARAMETERS     rDIOCParams;
    auto PVOID              pfnControlProc;
    auto BOOL               bExportTableInvalid;
    auto UINT               uBytesReturned;

                                        // Validate input parameters

    SYM_ASSERT ( hContext != NULL );

                                        // Assume there is no external
                                        // file

    hContext->prExportTable = NULL;

                                        // If VXDLDR is not present, then
                                        // return to caller, but report
                                        // success.  Not using external APIs
                                        // is not a critical error (correct?)

    if ( VXDLDR_GetVersion() == 0 )
        {
        return ( EXTSTATUS_OK );
        }

                                        // Allocate memory for callback
                                        // structure.

    prExportTable = (PEXPORT_TABLE_TYPE) VLPermMemoryAlloc (
                                             sizeof(*prExportTable) );

    if ( prExportTable == NULL )
        {
        return ( EXTSTATUS_MEM_ERROR );
        }

                                        // Create name of external file.
                                        // Note: I am assuming that VXDLDR
                                        // takes names of VxDs in the current
                                        // character set.  If this assumption
                                        // is wrong, we have to fix the code!

    STRCPY ( szExportFileShort, hContext->szVirScanDataFile );

    NameStripFile ( szExportFileShort );

    NameAppendFile ( szExportFileShort, szExportFileName );

    VxDGenericStringConvert ( szExportFileShort,
                              CHARSET_OEM,
                              szExportFileFull,
                              CHARSET_CURRENT );

    if ( NameReturnShortName ( szExportFileFull,
                               szExportFileShort ) == ERR )
        {
        STRCPY ( szExportFileFull, hContext->szVirScanDataFile );

        NameStripFile ( szExportFileFull );

        NameAppendFile ( szExportFileFull, szExportFileName );

        VxDGenericStringConvert ( szExportFileFull,
                                  CHARSET_OEM,
                                  szExportFileShort,
                                  CHARSET_CURRENT );
        }

                                        // Load external file and get address
                                        // of control procedure.

    if ( VXDLDR_LoadDevice ( &prDDB,
                             &prDeviceInfo,
                             szExportFileShort,
                             TRUE ) != 0 )
        {
        VLPermMemoryFree ( prExportTable );

        return ( EXTSTATUS_OK );
        }

    pfnControlProc = (PVOID)prDDB->DDB_Control_Proc;

                                        // If file loaded, then get structure
                                        // with exported functions.

    MEMSET ( &rDIOCParams, 0, sizeof(rDIOCParams) );

    rDIOCParams.dwIoControlCode   = VAVEX_GET_EXPORT_TABLE;
    rDIOCParams.lpvOutBuffer      = (DWORD)prExportTable;
    rDIOCParams.cbOutBuffer       = sizeof(*prExportTable);
    rDIOCParams.lpcbBytesReturned = (DWORD)&uBytesReturned;

    _asm {
        pushad
        mov     eax,W32_DEVICEIOCONTROL
        lea     esi,[rDIOCParams]
        cld
        call    [pfnControlProc]
        setc    al
        movzx   eax,al
        mov     bExportTableInvalid,eax
        popad
        }

    if ( bExportTableInvalid != FALSE ||
         uBytesReturned != sizeof(*prExportTable) )
        {
        VLPermMemoryFree ( prExportTable );

        if ( VXDLDR_UnloadDevice ( NULL, szExportModuleName ) != 0 )
            {
            SYM_ASSERT ( FALSE );
            }

        return ( EXTSTATUS_OK );
        }

                                        // Update the pointers in hContext.

    hContext->prExportTable = prExportTable;

                                        // Return to caller.

    return ( EXTSTATUS_OK );
    }

//---------------------------------------------------------------------------
//
// EXTSTATUS WINAPI EXTCloseVxD (
//      HVCONTEXT hContext
//      );
//
// This function closes the external APIs associated with hContext.
//
//---------------------------------------------------------------------------
EXTSTATUS WINAPI EXTCloseVxD (
    HVCONTEXT hContext
    )
    {
                                        // Validate input parameters.

    SYM_ASSERT ( hContext != NULL );

                                        // If there is no external APIs, then
                                        // just return to caller.

    if ( hContext->prExportTable == NULL )
        {
        return ( EXTSTATUS_OK );
        }

                                        // If there are external APIs, then
                                        // release the memory allocated for
                                        // them.

    VLPermMemoryFree ( hContext->prExportTable );

                                        // Invalidate pointer.

    hContext->prExportTable = NULL;

                                        // Unload VxD.

    if ( VXDLDR_UnloadDevice ( NULL, szExportModuleName ) != 0 )
        {
        SYM_ASSERT ( FALSE );
        }

                                        // Return to caller.

    return ( EXTSTATUS_OK );
    }

#elif defined(SYM_WIN)

// Statics for loading NAVEX32.DLL and functions

#if defined(SYM_WIN32)
  static char szExportFileName[]   = "NAVEX32.DLL";
#else
  static char szExportFileName[]   = "NAVEX16.DLL";
#endif

/*@API:**********************************************************************
@Declaration:
EXTSTATUS WINAPI EXTInitWin ( HVCONTEXT hContext )

@Description:
This routine dynamically loads NAVEX32.DLL and sets up hContext's
NAVEX function pointers.

EXTCloseWin() should be called to free the library.

@Parameters:
None.

@Returns:
EXTSTATUS_OK            if successful.
EXTSTATUS_INIT_ERROR    if failed loading library or function pointers.
EXTSTATUS_MEM_ERROR     if unable to allocate mem for export table.

@See:
@Include:   navex.h
@Compatibility: W32
****************************************************************************/

EXTSTATUS WINAPI EXTInitWin ( HVCONTEXT hContext )
{
    auto PEXPORT_TABLE_TYPE prExportTable;
    auto HINSTANCE          hExportLib;
    auto TCHAR              szExportFile[SYM_MAX_PATH + 1];

    SYM_ASSERT(NULL != hContext);

                                        // Don't load lib more than once.
    if ( NULL != hContext->prExportTable )
        {
        return (EXTSTATUS_OK);
        }
                                        // Allocate memory for callback
                                        // structure.
    prExportTable = (PEXPORT_TABLE_TYPE) VLPermMemoryAlloc (
                                             sizeof(*prExportTable) );

    if ( NULL == prExportTable )
        {
        return ( EXTSTATUS_MEM_ERROR );
        }

    STRCPY ( szExportFile, hContext->szVirScanDataFile );
    NameStripFile ( szExportFile );
    NameAppendFile ( szExportFile, szExportFileName );

                                        // Load NAVEX DLL.
    hExportLib = (HINSTANCE)LoadLibrary(szExportFile);

    if ((HINSTANCE)HINSTANCE_ERROR > hExportLib)
        {
        VLPermMemoryFree ( prExportTable );
        return ( EXTSTATUS_INIT_ERROR );
        }

    // Get function pointers.

    prExportTable->EXTScanFile   =
        (EXTSTATUS (FAR WINAPI *)(LPCALLBACKREV1,LPTSTR,HFILE,LPBYTE,LPBYTE,WORD,LPWORD))
        GetProcAddress(hExportLib, "EXTScanFile");

    prExportTable->EXTScanBoot   =
        (EXTSTATUS (FAR WINAPI *)(LPCALLBACKREV1,BYTE,BYTE,WORD,WORD,WORD,LPBYTE,LPBYTE,WORD,LPWORD))
        GetProcAddress(hExportLib, "EXTScanBoot");

    prExportTable->EXTScanPart   =
        (EXTSTATUS (FAR WINAPI *)(LPCALLBACKREV1,BYTE,LPBYTE,LPBYTE,WORD,LPWORD))
        GetProcAddress(hExportLib, "EXTScanPart");

    prExportTable->EXTScanMemory =
        (EXTSTATUS (FAR WINAPI *)(LPCALLBACKREV1,WORD,BOOL,LPWORD))
        GetProcAddress(hExportLib, "EXTScanMemory");

    prExportTable->EXTRepairFile =
        (EXTSTATUS (FAR WINAPI *)(LPCALLBACKREV1,WORD,LPN30,LPTSTR,LPBYTE))
        GetProcAddress(hExportLib, "EXTRepairFile");

    prExportTable->EXTRepairBoot =
        (EXTSTATUS (FAR WINAPI *)(LPCALLBACKREV1,WORD,LPN30,BYTE,BYTE,WORD,WORD,WORD,LPBYTE))
        GetProcAddress(hExportLib, "EXTRepairBoot");

    prExportTable->EXTRepairPart =
        (EXTSTATUS (FAR WINAPI *)(LPCALLBACKREV1,WORD,LPN30,BYTE,LPBYTE))
        GetProcAddress(hExportLib, "EXTRepairPart");

    if ( (NULL == prExportTable->EXTScanFile)   ||
         (NULL == prExportTable->EXTScanBoot)   ||
         (NULL == prExportTable->EXTScanPart)   ||
         (NULL == prExportTable->EXTScanMemory) ||
         (NULL == prExportTable->EXTRepairFile) ||
         (NULL == prExportTable->EXTRepairBoot) ||
         (NULL == prExportTable->EXTRepairPart) )
        {
#if defined(SYM_WIN16)                  // Not necessary for Win32
        if (NULL != prExportTable->EXTScanFile)
            FreeProcInstance((FARPROC)prExportTable->EXTScanFile);
        if (NULL != prExportTable->EXTScanBoot)
            FreeProcInstance((FARPROC)prExportTable->EXTScanBoot);
        if (NULL != prExportTable->EXTScanPart)
            FreeProcInstance((FARPROC)prExportTable->EXTScanPart);
        if (NULL != prExportTable->EXTScanMemory)
            FreeProcInstance((FARPROC)prExportTable->EXTScanMemory);
        if (NULL != prExportTable->EXTRepairFile)
            FreeProcInstance((FARPROC)prExportTable->EXTRepairFile);
        if (NULL != prExportTable->EXTRepairBoot)
            FreeProcInstance((FARPROC)prExportTable->EXTRepairBoot);
        if (NULL != prExportTable->EXTRepairPart)
            FreeProcInstance((FARPROC)prExportTable->EXTRepairPart);
#endif

        VLPermMemoryFree ( prExportTable );
        FreeLibrary ( hExportLib );
        return ( EXTSTATUS_INIT_ERROR );
        }

                                        // Save info in hContext.
    hContext->prExportTable = prExportTable;
    hContext->hExportLib = hExportLib;

    return (EXTSTATUS_OK);
} // EXTInitWin()


/*@API:**********************************************************************
@Declaration:
EXTSTATUS WINAPI EXTCloseWin ( HVCONTEXT hContext )

@Description:
This routine unloads then NAVEX DLL and zeros hContext's NAVEX
function pointers.

@Parameters:
None.

@Returns:
EXTSTATUS_OK    if successful or NavStat never initialized.

@See:
@Include:   navex.h
@Compatibility: W32
****************************************************************************/

EXTSTATUS WINAPI EXTCloseWin ( HVCONTEXT hContext )
{
    SYM_ASSERT ( NULL != hContext );

    if ( NULL == hContext->prExportTable )
        {
        return ( EXTSTATUS_OK );
        }

#if defined(SYM_WIN16)                  // Not necessary for Win32
    FreeProcInstance((FARPROC) ((PEXPORT_TABLE_TYPE)hContext->prExportTable)->EXTScanFile );
    FreeProcInstance((FARPROC) ((PEXPORT_TABLE_TYPE)hContext->prExportTable)->EXTScanBoot );
    FreeProcInstance((FARPROC) ((PEXPORT_TABLE_TYPE)hContext->prExportTable)->EXTScanPart );
    FreeProcInstance((FARPROC) ((PEXPORT_TABLE_TYPE)hContext->prExportTable)->EXTScanMemory );
    FreeProcInstance((FARPROC) ((PEXPORT_TABLE_TYPE)hContext->prExportTable)->EXTRepairFile );
    FreeProcInstance((FARPROC) ((PEXPORT_TABLE_TYPE)hContext->prExportTable)->EXTRepairBoot );
    FreeProcInstance((FARPROC) ((PEXPORT_TABLE_TYPE)hContext->prExportTable)->EXTRepairPart );
#endif

    VLPermMemoryFree ( hContext->prExportTable );
    FreeLibrary ( hContext->hExportLib );

    hContext->prExportTable = NULL;
    hContext->hExportLib = NULL;

    return ( EXTSTATUS_OK );
} // EXTCloseWin()

#elif defined(SYM_DOSX)

/*@API:**********************************************************************
@Declaration:
EXTSTATUS WINAPI EXTInitDX ( HVCONTEXT hContext )

@Description: This function initializes NAVEX.EXP for the DX platform

EXTCloseDX () should be called to free the export table buffer.

@Parameters:
   hContext

@Returns:
EXTSTATUS_OK            if successful.
EXTSTATUS_INIT_ERROR    if failed loading library or function pointers.
EXTSTATUS_MEM_ERROR     if unable to allocate mem for export table.

@See:
@Include:   navex.h
@Compatibility: DX
****************************************************************************/


EXTSTATUS WINAPI EXTInitDX (HVCONTEXT hContext)
{
    PEXPORT_TABLE_TYPE prExportTable;
    char               szExportFile[SYM_MAX_PATH + 1];
    char               szAddress[9];
    static TSF         tsf;
    HFILE              hFile;
    BOOL               bError = FALSE;
    static BOOL        bInitialized = FALSE;

    if (bInitialized)                   // only initialize once
    {
        return (EXTSTATUS_OK);
    }

    prExportTable = (PEXPORT_TABLE_TYPE) VLPermMemoryAlloc (
                                             sizeof(*prExportTable));
    if ( NULL == prExportTable )
    {
        return (EXTSTATUS_MEM_ERROR);
    }
                                        // convert prExportTable to a string
    SPRINTF(szAddress, "%08lX", &prExportTable);  

    STRCPY (szExportFile, hContext->szVirScanDataFile);
    NameStripFile (szExportFile);
    NameAppendFile (szExportFile, "NAVEX.EXP");

    hFile = VLFileOpen(szExportFile, READ_ONLY_FILE); 

    if (hFile == -1)
    {
        return (EXTSTATUS_INIT_ERROR);
    }   
                                        // load NAVEX.EXP
    if(!(d16LoadEXP ((FDORNAME)hFile, &tsf)))   
    {
        return (EXTSTATUS_INIT_ERROR);
    }
                                        // run NAVEX.EXP passing it the
    d16CallEXP(&tsf, szAddress);        // address of prExporTable                      

    hContext->prExportTable = prExportTable;

    bInitialized = TRUE;

    return (EXTSTATUS_OK);
}

EXTSTATUS WINAPI EXTCloseDX (HVCONTEXT hContext)
{
    return (EXTSTATUS_OK);
}

#endif
