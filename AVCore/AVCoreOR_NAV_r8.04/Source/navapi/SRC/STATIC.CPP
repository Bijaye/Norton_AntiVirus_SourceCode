// Copyright 1995 Symantec, Peter Norton Product Group
//***************************************************************************
//
// $Header:   S:/NAVAPI/VCS/static.cpv   1.7   20 Aug 1998 15:50:22   MKEATIN  $
//
// This file defines functions for communicating with external AV functions.
// This file has to be statically linked to AVAPI.
//
// Contains:
//      EXTInitVxD()
//      EXTCloseVxD()
//
//***************************************************************************
// $Log:   S:/NAVAPI/VCS/static.cpv  $
//
//    Rev 1.7   20 Aug 1998 15:50:22   MKEATIN
// Fixed some handle closing logic pointed out by MMENDOC (Thanks Melissa).
//
//    Rev 1.6   19 Aug 1998 18:07:10   MKEATIN
// Doing another check on hEngineLib before freeing it.
//
//    Rev 1.5   19 Aug 1998 17:52:08   MKEATIN
// If Navex fails to load, we now check to see if the naveng DLL succeeded
// and we free it, if so, before returning error.
//
//    Rev 1.4   04 Aug 1998 17:06:32   MKEATIN
// EXTInitWin now compiles for Win16.
//
//    Rev 1.3   29 Jul 1998 18:33:58   DHERTEL
// DX and NLM changes
//
//    Rev 1.2   29 Jul 1998 14:39:48   MKEATIN
// For Win16, look for NAVEX16a.DLL and NAVENG16.DLL.
//
//    Rev 1.1   21 May 1998 20:30:14   MKEATIN
// Changed avapi.h to avapi_l.h
//
//    Rev 1.0   21 May 1998 19:53:26   MKEATIN
// Initial revision.
//
//    Rev 1.37   14 May 1998 16:42:58   DDREW
// Fix to clear pointers in NLM ExtShutdownNLM
//
//    Rev 1.36   12 Sep 1997 14:17:20   MKEATIN
// Ported changes from branch G.
//
//    Rev 1.33.1.1   13 Aug 1997 14:18:24   MKEATIN
// Made changes so NAVEX15.INF will come from the app directory.
//
//    Rev 1.33.1.0   12 Aug 1997 22:40:18   DALVIDR
// Branch base for version QAKG
//
//    Rev 1.33   07 Aug 1997 09:33:40   DDREW
// More changes for NLM - only - so don't worry!
//
//    Rev 1.32   31 Jul 1997 11:24:26   DDREW
// Work in progress - EXTInitNLM() and EXTCloseNLM
//
//    Rev 1.31   18 Jul 1997 14:20:48   DBUCHES
// Removed previous revision.  This change was not neccessary after all.
//
//    Rev 1.30   16 Jul 1997 17:14:46   CNACHEN
// Updated NKT platform for alpha to support proper filenames for NAVEXDA.SYS
// and NAVENGDA.SYS.
//
//    Rev 1.29   10 Jul 1997 23:02:26   RStanev
// Restored back the communication names for NAVEX15 and NAVENG on
// Alpha builds.  These names are not affected by changes in the executable
// image.
//
//    Rev 1.28   27 Jun 1997 19:00:48   DBUCHES
// Fixed up reference to an un-initialized variable in EXTWinInit().
//
//    Rev 1.27   09 Jun 1997 03:18:10   BGERHAR
// AreWildDefs as C function
//
//    Rev 1.26   09 Jun 1997 02:48:36   BGERHAR
// Support wild def set for DOS platform
//
//    Rev 1.25   04 Jun 1997 17:57:32   DCHI
//
//    Rev 1.24   04 Jun 1997 17:53:50   DCHI
//
//    Rev 1.23   04 Jun 1997 17:48:26   DCHI
// Added #ifdef's for DEC Alpha NAVEX names.
//
//    Rev 1.22   28 May 1997 11:33:06   MKEATIN
// Call EXTShutdown() in EXTCloseVxD() and EXTCloseNTK().  Also, handled some
// error conditions in EXTInitWin() and EXTInitDX().
//
//    Rev 1.21   27 May 1997 19:00:36   MKEATIN
// Added the NTK platform to NAVEX 1.5 technology.
//
//    Rev 1.20   27 May 1997 12:26:10   RStanev
// Fixed one more bug in EXTInitVxD().
//
//    Rev 1.19   25 May 1997 01:26:38   RStanev
// Fixed a couple of bugs in EXTInitVxD().
//
//    Rev 1.18   24 May 1997 15:40:56   MKEATIN
// Made our buffer allocation a deallocation cleaner and added some
// error checking.
//
//    Rev 1.17   23 May 1997 18:15:18   MKEATIN
// Fixed various bugs int EXTInitVxD and EXTCloseVXD.
//
//    Rev 1.16   21 May 1997 16:05:14   MKEATIN
// Enabled EXTInitVXD() for NAVEX 1.5
//
//    Rev 1.15   14 May 1997 15:54:30   MKEATIN
// Now passing pathnames (instead of NULL) to parameters in EXTStartUp()
//
//    Rev 1.13   12 May 1997 17:38:02   MKEATIN
// Added DX 1.5 cleanup functionality
//
//    Rev 1.12   09 May 1997 17:23:50   MKEATIN
// Temporary build issue fixes
//
//    Rev 1.11   09 May 1997 17:09:26   MKEATIN
// Fixed build issues for NAVAP
//
//    Rev 1.10   09 May 1997 16:42:14   MKEATIN
// Fixed QueryInterface calls in the DX platform.
//
//    Rev 1.9   08 May 1997 18:24:56   MKEATIN
// Changed NAVEX.EXP to NAVEX15.EXP
//
//    Rev 1.8   08 May 1997 18:23:00   MKEATIN
// Changed NAVEX32.DLL to NAVEX32a.DLL (the new 1.5 dll).
//
//    Rev 1.7   08 May 1997 15:49:26   MKEATIN
// Remove the dreaded lpgCallBack and upgraded to CALLBACKREV2 and NAVEX 1.5
// support
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
#include "avapi_l.h"
#if defined(SYM_NLM)
#undef AVAPI_INCLUDE
#endif
#include "navex15.h"

#ifdef SYM_WIN32
#include "navexisi.h"
#endif  // #ifdef SYM_WIN32

                                        // This is a function pointer to
                                        // EXTQueryInterface() in NAVEX

typedef EXTSTATUS (WINAPI *LPFUNC_QUERYINTERFACE)(DWORD,LPLPVOID);

#if defined (SYM_VXD)

#define NAVEX_GET_QUERY_INTERFACE 2

static TCHAR szExportModule[] = _T("NAVEX15");
static TCHAR szEngineModule[] = _T("NAVENG");

static BOOL bEngineModuleLoaded = FALSE;

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
EXTSTATUS WINAPI EXTInitVxD
(
    HVCONTEXT hContext
)
{
    PEXPORT15_TABLE_TYPE  prExportTable, prEngineTable;
    LPFUNC_QUERYINTERFACE fpQueryNavexInterface  = NULL;
    LPFUNC_QUERYINTERFACE fpQueryEngineInterface = NULL;
    auto  PVMMDDB         prDDB;
    auto  DeviceInfo      *prDeviceInfo;
    auto  DIOCPARAMETERS  rDIOCParams;
    auto  PVOID           pfnControlProc;
    const TCHAR           szExportFileName[]   = _T("NAVEX15.VXD");
    const TCHAR           szEngineFileName[]   = _T("NAVENG.VXD" );
    auto  TCHAR           szNavexDataDir[SYM_MAX_PATH + 1];
    auto  LPTSTR          lpszExportFileFull, lpszExportFileShort,
                          lpszEngineFileFull, lpszEngineFileShort;

    SYM_ASSERT (hContext);

                                        // if VXDLDR is not present, then
                                        // return error.
    if ( VXDLDR_GetVersion() == 0 )
    {
        return (EXTSTATUS_INIT_ERROR);
    }
                                        // dynamically allocate these 4
                                        // pathnames to save stack
    lpszExportFileFull = (LPSTR)
                         MemAllocPtr(GHND, 4 * SYM_MAX_PATH * sizeof(WCHAR));

    if (!lpszExportFileFull)
    {
        return (EXTSTATUS_INIT_ERROR);
    }

    lpszExportFileShort = &lpszExportFileFull [SYM_MAX_PATH];
    lpszEngineFileFull  = &lpszExportFileShort[SYM_MAX_PATH];
    lpszEngineFileShort = &lpszEngineFileFull [SYM_MAX_PATH];


//------------------------------------------------------------------------
//
//  Get the pathname's for NAVEX15.VXD and NAVENG.VXD so they are
//  acceptable to VXDLDER.  Also, set up Inf and Data Dir strings.
//
//------------------------------------------------------------------------

    STRCPY(lpszExportFileShort, hContext->szVirScanDataFile);
    NameStripFile(lpszExportFileShort);

    STRCPY(lpszEngineFileShort, lpszExportFileShort);
    STRCPY(szNavexDataDir,      lpszExportFileShort);

    NameAppendFile(lpszExportFileShort, szExportFileName  );
    NameAppendFile(lpszEngineFileShort, szEngineFileName  );

    VxDGenericStringConvert(lpszExportFileShort,
                            CHARSET_OEM,
                            lpszExportFileFull,
                            CHARSET_CURRENT);

    if (NameReturnShortName(lpszExportFileFull,
                            lpszExportFileShort) == ERR)
    {
        memcpy(lpszExportFileShort,
               lpszExportFileFull,
               SYM_MAX_PATH * sizeof(WCHAR));
    }

    VxDGenericStringConvert(lpszEngineFileShort,
                            CHARSET_OEM,
                            lpszEngineFileFull,
                            CHARSET_CURRENT);

    if (NameReturnShortName(lpszEngineFileFull,
                            lpszEngineFileShort) == ERR)
    {
        memcpy(lpszEngineFileShort,
               lpszEngineFileFull,
               SYM_MAX_PATH * sizeof(WCHAR));
    }


//------------------------------------------------------------------------
//
//  Load NAVEX15.VXD and execute it's EXTQueryInterface().
//
//------------------------------------------------------------------------

    if (VXDLDR_LoadDevice (&prDDB,
                           &prDeviceInfo,
                           lpszExportFileShort,
                           TRUE) != 0)
    {
        MemFreePtr(lpszExportFileFull);

        return (EXTSTATUS_INIT_ERROR);
    }

    pfnControlProc = (PVOID)prDDB->DDB_Control_Proc;

                                        // get the function pointer to
                                        // NAVEX15.VXD's EXTQueryInterface()
    MEMSET (&rDIOCParams, 0, sizeof(rDIOCParams));

    rDIOCParams.dwIoControlCode = NAVEX_GET_QUERY_INTERFACE;
    rDIOCParams.lpvOutBuffer    = (DWORD)&fpQueryNavexInterface;

    _asm
    {                                   // exucute the Contol Proc to
        pushad                          // get fpQueryNavexInterface
        mov     eax,W32_DEVICEIOCONTROL
        lea     esi,[rDIOCParams]
        call    [pfnControlProc]
        popad
    }                                   // fpQueryNavexInterface will be
                                        // NULL if the Control Proc failed.
                                        // If not, execute it here.
    if (fpQueryNavexInterface &&
       (*fpQueryNavexInterface)(NAVEX15_INTERFACE,
                               (LPLPVOID)&prExportTable) == EXTSTATUS_OK)
    {


//------------------------------------------------------------------------
//
//  NAVEX15.VXD Success!
//
//  Now, load NAVENG.VXD (if present) and execute it's EXTQueryInterface()
//
//------------------------------------------------------------------------

        if (VXDLDR_LoadDevice (&prDDB,
                               &prDeviceInfo,
                               lpszEngineFileShort,
                               TRUE) == 0)
        {
            pfnControlProc = (PVOID)prDDB->DDB_Control_Proc;

                                        // Get function pointer to
                                        // NAVENG.VXD's EXTQueryInterface()
            MEMSET ( &rDIOCParams, 0, sizeof(rDIOCParams) );

            rDIOCParams.dwIoControlCode   = NAVEX_GET_QUERY_INTERFACE;
            rDIOCParams.lpvOutBuffer      = (DWORD)&fpQueryEngineInterface;

            _asm
            {                           // Exucute the Contol Proc
                pushad                  // getting fpQueryEngineInterface
                mov     eax,W32_DEVICEIOCONTROL
                lea     esi,[rDIOCParams]
                call    [pfnControlProc]
                popad
            }
                                        // fpQueryEngineInterface will be
                                        // NULL if the Control Proc failed
            if (fpQueryEngineInterface &&
               (*fpQueryEngineInterface)(NAVEX15_INTERFACE,
                                        (LPLPVOID)&prEngineTable) == EXTSTATUS_OK)
            {
                                        // NAVENG.VXD success!
                                        // Execute Navex's EXTStartUp()
                if (prExportTable->EXTStartUp(hContext->lpstCallBack,
                                              0,
                                              prEngineTable,
                                              szNavexDataDir,
                                              hContext->szNavexInfFile,
                                              NULL) == EXTSTATUS_OK)
                {
                    hContext->prExportTable = prExportTable;
                    hContext->prEngineTable = prEngineTable;
                    bEngineModuleLoaded = TRUE;

                    MemFreePtr(lpszExportFileFull);

                    return (EXTSTATUS_OK);
                }
                else                    // EXTStartUp failure
                {
                    if (VXDLDR_UnloadDevice(NULL, szEngineModule) != 0)
                    {
                        SYM_ASSERT (FALSE);
                    }

                    if (VXDLDR_UnloadDevice(NULL, szExportModule) != 0)
                    {
                        SYM_ASSERT (FALSE);
                    }

                    MemFreePtr(lpszExportFileFull);

                    return (EXTSTATUS_INIT_ERROR);
                }
            }

            if (VXDLDR_UnloadDevice(NULL, szEngineModule) != 0)
            {
                SYM_ASSERT (FALSE);
            }
        }                               // No Engine Export Table
                                        // Execute EXTStartUp()
        if (prExportTable->EXTStartUp(hContext->lpstCallBack,
                                      0,
                                      NULL,
                                      szNavexDataDir,
                                      hContext->szNavexInfFile,
                                      NULL) == EXTSTATUS_OK)
        {
            hContext->prExportTable = prExportTable;

            MemFreePtr(lpszExportFileFull);

            return (EXTSTATUS_OK);
        }
    }
                                        // Failure
    if (VXDLDR_UnloadDevice(NULL, szExportModule) != 0)
    {
        SYM_ASSERT (FALSE);
    }

    MemFreePtr(lpszExportFileFull);

    return (EXTSTATUS_INIT_ERROR);
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
EXTSTATUS WINAPI EXTCloseVxD
(
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
                                        // Call EXTShutDown();
    EXTShutDown(hContext);
                                        // Invalided export table pointers
    hContext->prExportTable = NULL;
    hContext->prEngineTable = NULL;
                                        // Unload NAVENG.VXD if loaded
    if (bEngineModuleLoaded &&
       (VXDLDR_UnloadDevice(NULL, szEngineModule) != 0))
    {
        SYM_ASSERT ( FALSE );
    }                                   // Unload NAVEX15.VXD

    if (VXDLDR_UnloadDevice (NULL, szExportModule) != 0 )
    {
        SYM_ASSERT ( FALSE );
    }

    bEngineModuleLoaded = FALSE;

    return ( EXTSTATUS_OK );
}

#elif defined(SYM_NTK)

#include "drvcomm.h"

//---------------------------------------------------------------------------
//
// NAVEXFileObject and EngineFileObject are used to lock the device
// objects in memory.
//
//---------------------------------------------------------------------------

static PFILE_OBJECT NAVEXFileObject  = NULL;
static PFILE_OBJECT EngineFileObject = NULL;

//---------------------------------------------------------------------------
//
// EXTSTATUS WINAPI EXTInitNTK (
//      HVCONTEXT hContext
//      );
//
// This function initializes the NAVEX engine, and makes it available to
// hContext.  No other hContext can be using NAVEX when EXTInitNTK() is
// called.  This means, that once EXTInitNTK() was successfully called,
// you can only call EXTCloseNTK().
//
// Entry:
//      hContext - AVAPI context handle.
//
// Exit:
//      EXTSTATUS_OK if successful, otherwise error.
//
// Note:
//      Currently this function returns only EXTSTATUS_OK.
//
//---------------------------------------------------------------------------
EXTSTATUS WINAPI EXTInitNTK
(
    HVCONTEXT hContext
)
{
    auto   PIRP                 Irp;
    auto   UNICODE_STRING       ObjectName;
    auto   PDEVICE_OBJECT       DeviceObject;
    auto   KEVENT               event;
    auto   IO_STATUS_BLOCK      IoStatusBlock;
    auto   PEXPORT15_TABLE_TYPE prExportTable;
    auto   PEXPORT15_TABLE_TYPE prEngineTable;

    const  TCHAR szNAVEXDeviceName[]  = _T("\\Device\\NAVEX15");
    const  TCHAR szEngineDeviceName[] = _T("\\Device\\NAVENG");

    LPFUNC_QUERYINTERFACE fpQueryNavexInterface  = NULL;
    LPFUNC_QUERYINTERFACE fpQueryEngineInterface = NULL;
    auto   TCHAR          szNavexDataDir[SYM_MAX_PATH + 1];

                                        // Validate input parameters.

    SYM_ASSERT (hContext);

                                        // Setup paths for EXTStartup()

    STRCPY(szNavexDataDir, hContext->szVirScanDataFile);
    NameStripFile(szNavexDataDir);


//------------------------------------------------------------------------
//
//  Load NAVEX15.VXD and execute it's EXTQueryInterface().
//
//------------------------------------------------------------------------

    RtlInitUnicodeString(&ObjectName, szNAVEXDeviceName);

    if (!NT_SUCCESS(IoGetDeviceObjectPointer(&ObjectName,
                                             FILE_ALL_ACCESS,
                                             &NAVEXFileObject,
                                             &DeviceObject)) ||
        !DeviceObject)
    {
        goto Cleanup;
    }
                                        // Allocate Irp for DIOCTL

    Irp = IoBuildDeviceIoControlRequest(DIOCTL_SYM_USER01,
                                        DeviceObject,
                                        NULL,
                                        0,
                                        &fpQueryNavexInterface,
                                        sizeof(DWORD),
                                        FALSE,
                                        &event,
                                        &IoStatusBlock);
    if (!Irp)
    {
        goto Cleanup;
    }
                                        // Init an event object to wait on.

    KeInitializeEvent(&event, NotificationEvent, FALSE);

                                        // go get EXTQueryInterface()

    switch (IoCallDriver(DeviceObject, Irp))
    {
        case STATUS_PENDING:

            KeWaitForSingleObject(&event,
                                  Suspended,
                                  KernelMode,
                                  FALSE,
                                  NULL);

            if (!NT_SUCCESS(IoStatusBlock.Status))
            {
                goto Cleanup;
            }

        case STATUS_SUCCESS:

            if (!NT_SUCCESS(IoStatusBlock.Status) ||
                IoStatusBlock.Information != sizeof(DWORD))
            {
                goto Cleanup;
            }
                                        // Success - break!
            break;

        default:

            goto Cleanup;
    }
                                        // Now fill in prExportTable
    if (!fpQueryNavexInterface ||
       (*fpQueryNavexInterface)(NAVEX15_INTERFACE,
                               (LPLPVOID)&prExportTable) != EXTSTATUS_OK)
    {
        goto Cleanup;
    }


//------------------------------------------------------------------------
//
//  NAVEX15.SYS Success!
//
//  Now, load NAVENG.SYS (if present) and execute it's EXTQueryInterface()
//
//------------------------------------------------------------------------

    RtlInitUnicodeString(&ObjectName, szEngineDeviceName);

    if (NT_SUCCESS(IoGetDeviceObjectPointer(&ObjectName,
                                            FILE_ALL_ACCESS,
                                            &EngineFileObject,
                                            &DeviceObject)) &&
        DeviceObject)
    {
                                        // Allocate Irp for DIOCTL

        Irp = IoBuildDeviceIoControlRequest(DIOCTL_SYM_USER01,
                                            DeviceObject,
                                            NULL,
                                            0,
                                            &fpQueryEngineInterface,
                                            sizeof(DWORD),
                                            FALSE,
                                            &event,
                                            &IoStatusBlock );

        if (Irp)
        {
                                        // Init an event object to wait on.

            KeInitializeEvent(&event, NotificationEvent, FALSE);

                                        // go get EXTQueryInterface()

            switch (IoCallDriver(DeviceObject, Irp))
            {
                case STATUS_PENDING:

                    KeWaitForSingleObject(&event,
                                          Suspended,
                                          KernelMode,
                                          FALSE,
                                          NULL);

                    if (!NT_SUCCESS(IoStatusBlock.Status))
                    {
                        break;
                    }

                case STATUS_SUCCESS:

                    if (!NT_SUCCESS(IoStatusBlock.Status) ||
                        IoStatusBlock.Information != sizeof(DWORD))
                    {
                        break;
                    }
                                        // Success! Now fill in prEngineTable
                                        // and execute EXTSTartup()

                    if (fpQueryEngineInterface &&
                        ((*fpQueryEngineInterface)(NAVEX15_INTERFACE,
                                                  (LPLPVOID)&prEngineTable) == EXTSTATUS_OK) &&
                        (prExportTable->EXTStartUp(hContext->lpstCallBack,
                                                   0,
                                                   prEngineTable,
                                                   szNavexDataDir,
                                                   hContext->szNavexInfFile,
                                                   NULL) == EXTSTATUS_OK))
                    {
                        hContext->prExportTable = prExportTable;
                        hContext->prEngineTable = prEngineTable;

                        return (EXTSTATUS_OK);
                    }
                    else
                    {
                        goto Cleanup;
                    }

                default:

                    break;
            }
        }
    }


//------------------------------------------------------------------------
//
//  NAVENG.SYS Failed to load and initialize. Just load NAVEX.SYS.
//
//------------------------------------------------------------------------

    if (EngineFileObject)
    {
        ObDereferenceObject(EngineFileObject);
        EngineFileObject = NULL;
    }

    if (prExportTable->EXTStartUp(hContext->lpstCallBack,
                                  0,
                                  NULL,
                                  szNavexDataDir,
                                  hContext->szNavexInfFile,
                                  NULL) == EXTSTATUS_OK)
    {
        hContext->prExportTable = prExportTable;

        return (EXTSTATUS_OK);
    }


Cleanup:

    if (NAVEXFileObject)
    {
        ObDereferenceObject(NAVEXFileObject);
        NAVEXFileObject = NULL;
    }

    if (EngineFileObject)
    {
        ObDereferenceObject(EngineFileObject);
        EngineFileObject = NULL;
    }

    return (EXTSTATUS_INIT_ERROR);
}

//---------------------------------------------------------------------------
//
// EXTSTATUS WINAPI EXTCloseNTK (
//      HVCONTEXT hContext
//      );
//
// This function deinitializes the NAVEX engine for hContext.
//
// Entry:
//      hContext - AVAPI context handle, to which EXTInitNTK() has
//                 successfully added NAVEX support.
//
// Exit:
//      EXTSTATUS_OK
//
// Synchronization:
//      This function is non-reentrant.
//
//---------------------------------------------------------------------------
EXTSTATUS WINAPI EXTCloseNTK
(
    HVCONTEXT hContext
)
{
    if (hContext->prExportTable == NULL)
    {
        return (EXTSTATUS_OK);
    }
                                        // Call EXTShutDown();
    EXTShutDown(hContext);

                                        // Dereference the handles to the file
                                        // objects and invalidate the exports
                                        // in hContext.

    if (NAVEXFileObject)
    {
        ObDereferenceObject(NAVEXFileObject);
        NAVEXFileObject = NULL;
    }

    if (EngineFileObject)
    {
        ObDereferenceObject(EngineFileObject);
        EngineFileObject = NULL;
    }

    return ( EXTSTATUS_OK );
}


#elif defined(SYM_WIN)

/*@API:**********************************************************************
@Declaration:
EXTSTATUS WINAPI EXTInitWin (HVCONTEXT hContext)

@Description:
This routine dynamically loads NAVEX32.DLL and sets up hContext's
NAVEX function pointers.

This function has been NAVEX 1.5 enabled.

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

EXTSTATUS WINAPI EXTInitWin
(
    HVCONTEXT hContext
)
{
    PEXPORT15_TABLE_TYPE  prExportTable, prEngineTable;
    HINSTANCE             hExportLib, hEngineLib;
    TCHAR                 szExportFile[SYM_MAX_PATH + 1];
    TCHAR                 szEngineFile[SYM_MAX_PATH + 1];
    TCHAR                 szNavexDataDir[SYM_MAX_PATH + 1];
    LPFUNC_QUERYINTERFACE fpQueryNavexInterface, fpQueryEngineInterface;

#ifdef _ALPHA_
    TCHAR szExportFileName[]   = _T("NAVEXDA.DLL");
    TCHAR szEngineFileName[]   = _T("NAVENGDA.DLL");
#elif SYM_WIN32
    TCHAR szExportFileName[]   = _T("NAVEX32a.DLL");
    TCHAR szEngineFileName[]   = _T("NAVENG32.DLL");
#else
    char szExportFileName[]  = "NAVEX16a.DLL";
    char szEngineFileName[]  = "NAVENG16.DLL";
#endif

    SYM_ASSERT (NULL != hContext);
                                        // Don't load lib more than once.
    if (hContext->prExportTable != NULL)
    {
        return (EXTSTATUS_OK);
    }
                                        // Get the full Paths for:
                                        //   NAVEX??.DLL, NAVENG??.DLL
                                        //   NAVEX15.INF, and data dir.
    STRCPY(szExportFile, hContext->szVirScanDataFile);
    NameStripFile(szExportFile);
    STRCPY(szEngineFile,    szExportFile);
    STRCPY(szNavexDataDir,  szExportFile);
    NameAppendFile(szExportFile, szExportFileName);
    NameAppendFile(szEngineFile, szEngineFileName);

                                        // Load NAVEX??.DLL.
    hExportLib = (HINSTANCE)LoadLibrary(szExportFile);

                                        // Load Engine??.DLL.
    hEngineLib = (HINSTANCE)LoadLibrary(szEngineFile);

#ifdef SYM_WIN16
    // On Win16, LoadLibrary doesn't always return NULL on failure.  So,
    // we set it to NULL on failure here.

    hExportLib = hExportLib <= HINSTANCE_ERROR ? NULL : hExportLib;
    hEngineLib = hEngineLib <= HINSTANCE_ERROR ? NULL : hEngineLib;
#endif

    if (hExportLib == NULL)
    {
        if (hEngineLib)
        {
            FreeLibrary(hEngineLib);
        }
        return (EXTSTATUS_INIT_ERROR);
    }
                                        // Get function pointer to
                                        // NAVEX??.DLL's EXTQueryInterface()
    fpQueryNavexInterface = (LPFUNC_QUERYINTERFACE)
        (GetProcAddress(hExportLib, "EXTQueryInterface"));

    if (fpQueryNavexInterface &&
       (*fpQueryNavexInterface)(NAVEX15_INTERFACE,
                               (LPLPVOID)&prExportTable) == EXTSTATUS_OK)
    {
                                        // NAVEX??.DLL success!
                                        // Now try NAVENG32.DLL
        if ( hEngineLib != NULL )
        {
            fpQueryEngineInterface = (LPFUNC_QUERYINTERFACE)
                (GetProcAddress(hEngineLib, "EXTQueryInterface"));

            if (fpQueryEngineInterface &&
               (*fpQueryEngineInterface)(NAVEX15_INTERFACE,
                                        (LPLPVOID)&prEngineTable) == EXTSTATUS_OK)
            {
                                        // NAVENG??.DLL success!
                                        // Execute EXTStartUp()
                if (prExportTable->EXTStartUp(hContext->lpstCallBack,
                                              0,
                                              prEngineTable,
                                              szNavexDataDir,
                                              hContext->szNavexInfFile,
                                              NULL) == EXTSTATUS_OK)
                {
                    hContext->prExportTable = prExportTable;
                    hContext->hExportLib    = hExportLib;
                    hContext->prEngineTable = prEngineTable;
                    hContext->hEngineLib    = hEngineLib;

                    return (EXTSTATUS_OK);
                }
                else                    // EXTStartUp failure
                {
                    FreeLibrary(hExportLib);
                    FreeLibrary(hEngineLib);

                    return(EXTSTATUS_INIT_ERROR);
                }
            }
            else                       // QueryInterface() failed for NAVENG
            {
                FreeLibrary(hEngineLib);
            }
        }
                                        // No Engine Export Table
                                        // Execute EXTStartUp()
        if (prExportTable->EXTStartUp(hContext->lpstCallBack,
                                      0,
                                      NULL,
                                      szNavexDataDir,
                                      hContext->szNavexInfFile,
                                      NULL) == EXTSTATUS_OK)
        {
            hContext->prExportTable = prExportTable;
            hContext->hExportLib    = hExportLib;
            return (EXTSTATUS_OK);
        }
    }
                                        // Failure
    FreeLibrary(hExportLib);

    if (hEngineLib)
    {
        FreeLibrary(hEngineLib);
    }

    return(EXTSTATUS_INIT_ERROR);
}


/*@API:**********************************************************************
@Declaration:
EXTSTATUS WINAPI EXTCloseWin (HVCONTEXT hContext)

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

EXTSTATUS WINAPI EXTCloseWin
(
    HVCONTEXT hContext
)
{
    SYM_ASSERT (hContext);

    if (hContext->prExportTable == NULL)
    {
        return (EXTSTATUS_OK);
    }
                                        // Call EXTShutDown
    EXTShutDown(hContext);

    FreeLibrary (hContext->hExportLib);
    FreeLibrary (hContext->hEngineLib);

    hContext->prExportTable = NULL;
    hContext->hExportLib    = NULL;
    hContext->prEngineTable = NULL;
    hContext->hEngineLib    = NULL;

    return (EXTSTATUS_OK);
}

#if defined(SYM_WIN32) && !defined(_ALPHA_)

/*@API:**********************************************************************
@Declaration:
EXTSTATUS WINAPI EXTInitIsInfectable (HVCONTEXT hContext)

@Description:
This routine sets up hContext's NAVEX IsInfectable function pointers.

This function has been NAVEX 1.5 enabled.

EXTCloseIsInfectable() should be called to free the library.

@Parameters:
hContext

@Returns:
EXTSTATUS_OK            if successful.
EXTSTATUS_INIT_ERROR    if failed loading library or function pointers.
EXTSTATUS_MEM_ERROR     if unable to allocate mem for export table.

@See:
@Include:   navex.h
@Compatibility: W32
****************************************************************************/

EXTSTATUS WINAPI EXTInitIsInfectable
(
    HVCONTEXT hContext
)
{
    PEXPORT15_ISI_TABLE_TYPE    prExportIsInfectableTable;
    LPFUNC_QUERYINTERFACE       fpQueryNavexInterface = NULL;
    TCHAR                       szNavexDataDir[SYM_MAX_PATH + 1] = {0};
    EXTSTATUS                   extStatus = EXTSTATUS_OK;

    SYM_ASSERT(hContext);
    SYM_ASSERT(hContext->hExportLib);

    fpQueryNavexInterface = (LPFUNC_QUERYINTERFACE)
        (GetProcAddress(hContext->hExportLib, "EXTQueryInterface"));

    if (fpQueryNavexInterface &&
       (*fpQueryNavexInterface)(NAVEX15_ISI_INTERFACE,
                               (LPLPVOID)&prExportIsInfectableTable) == EXTSTATUS_OK)
    {
        STRCPY(szNavexDataDir, hContext->szVirScanDataFile);
        NameStripFile(szNavexDataDir);
        extStatus = prExportIsInfectableTable->EXTStartUpIsInfectable(hContext->lpstCallBack,
                                                                      szNavexDataDir,
                                                                      hContext->szNavexInfFile);

        if (extStatus == EXTSTATUS_OK)
            hContext->prExportIsInfectableTable = prExportIsInfectableTable;
        else
            extStatus = EXTSTATUS_INIT_ERROR;
    }
    else
        extStatus = EXTSTATUS_INIT_ERROR;

    return extStatus;
}

/*@API:**********************************************************************
@Declaration:
EXTSTATUS WINAPI EXTCloseIsInfectable (HVCONTEXT hContext)

@Description:
This routine zeros hContext's NAVEX IsInfectable function pointers.

@Parameters:
hContext

@Returns:
EXTSTATUS_OK    if successful or NavStat never initialized.

@See:
@Include:   navex.h
@Compatibility: W32
****************************************************************************/

EXTSTATUS WINAPI EXTCloseIsInfectable
(
    HVCONTEXT hContext
)
{
    SYM_ASSERT (hContext);

    if (hContext->prExportIsInfectableTable == NULL)
        return EXTSTATUS_OK;
                                        // Call EXTShutDown
    EXTShutDownIsInfectable(hContext);
    hContext->prExportIsInfectableTable = NULL;

    return EXTSTATUS_OK;
}
#endif // #if defined(SYM_WIN32) && !defined(_ALPHA_)

#elif defined(SYM_DOSX)

/*@API:**********************************************************************
@Declaration:
EXTSTATUS WINAPI EXTInitDX ( HVCONTEXT hContext )

@Description: This function initializes NAVEX15.EXP for the DX platform

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

                                        // Kludge to do wild set
extern "C" BOOL PASCAL    AreDefsWild(VOID);

EXTSTATUS WINAPI EXTInitDX (HVCONTEXT hContext)
{
    PEXPORT15_TABLE_TYPE  prExportTable, prEngineTable;
    char                  szExportFile[SYM_MAX_PATH + 1];
    char                  szEngineFile[SYM_MAX_PATH + 1];
    char                  szNavexAddress[9], szEngineAddress[9];
    char                  szNavexDataDir[SYM_MAX_PATH + 1];
    static TSF            Navex_tsf, Engine_tsf;
    HFILE                 hNavexFile, hEngineFile;
    LPFUNC_QUERYINTERFACE fpQueryNavexInterface  = NULL;
    LPFUNC_QUERYINTERFACE fpQueryEngineInterface = NULL;
    LPSTR                 lpstNavexMemoryHandle  = NULL;
    LPSTR                 lpstEngineMemoryHandle = NULL;

    // This routine does not use the VLFileOpen() function
    // because we need a real OS HFILE for the d16LoadEXP() call.


    if (hContext->lpstNavexMemoryHandle)// Already initialized
    {
        return (EXTSTATUS_OK);
    }

    SPRINTF(szNavexAddress,  "%08lX", &fpQueryNavexInterface);
    SPRINTF(szEngineAddress, "%08lX", &fpQueryEngineInterface);

                                        // Get paths for NAVEX15.EXP and
                                        // NAVENG.EXP
    STRCPY(szExportFile, hContext->szVirScanDataFile);
    NameStripFile(szExportFile);
    STRCPY(szEngineFile,   szExportFile);
    STRCPY(szNavexDataDir, szExportFile);
    NameAppendFile(szExportFile, "NAVEX15.EXP");

    hNavexFile = FileOpen(szExportFile, READ_ONLY_FILE);

                                        // If using wild set, try full NAVEX
                                        // If that doesn't work, use WildEx
    if ((hNavexFile == -1) && AreDefsWild())
    {
        NameStripFile(szExportFile);
        NameAppendFile(szExportFile, "WILDEX15.EXP");
        NameAppendFile(szEngineFile, "WILDENG.EXP");

        hNavexFile = FileOpen(szExportFile, READ_ONLY_FILE);

    }
    else
    {
        NameAppendFile(szEngineFile,   "NAVENG.EXP");
    }

    if (hNavexFile == -1)
    {
        return (EXTSTATUS_INIT_ERROR);
    }
                                        // load NAVEX.EXP
    lpstNavexMemoryHandle = d16LoadEXP((FDORNAME)hNavexFile, &Navex_tsf);

    if(!lpstNavexMemoryHandle)
    {
        FileClose(hNavexFile);
        return (EXTSTATUS_INIT_ERROR);
    }
                                        // run NAVEX.EXP passing it the
                                        // address of fpQueryNavexInterface
    d16CallEXP(&Navex_tsf, szNavexAddress);

    if (fpQueryNavexInterface &&
       (*fpQueryNavexInterface)(NAVEX15_INTERFACE,
                               (LPLPVOID)&prExportTable) == EXTSTATUS_OK)
    {
                                        // NAVEX.EXP success!
                                        // Now try NAVENG.EXP
        hEngineFile = FileOpen(szEngineFile, READ_ONLY_FILE);

        if (hEngineFile != -1)
        {
            lpstEngineMemoryHandle = d16LoadEXP((FDORNAME)hEngineFile, &Engine_tsf);
        }

        if (lpstEngineMemoryHandle)
        {
                                        // run NAVENG.EXP passing it the
                                        // address of fpQueryEngineInterface
            d16CallEXP(&Engine_tsf, szEngineAddress);

            if (fpQueryEngineInterface &&
               (*fpQueryEngineInterface)(NAVEX15_INTERFACE,
                                        (LPLPVOID)&prEngineTable) == EXTSTATUS_OK)
            {
                                        // NAVENG.EXP success!
                                        // Execute EXTStartUp()
                if (prExportTable->EXTStartUp(hContext->lpstCallBack,
                                              0,
                                              prEngineTable,
                                              szNavexDataDir,
                                              hContext->szNavexInfFile,
                                              NULL) == EXTSTATUS_OK)
                {
                    hContext->prExportTable = prExportTable;
                    hContext->prEngineTable = prEngineTable;
                    hContext->lpstNavexMemoryHandle = lpstNavexMemoryHandle;
                    hContext->lpstEngineMemoryHandle = lpstEngineMemoryHandle;

                    return (EXTSTATUS_OK);
                }
                else                    // EXTStartUp failure
                {
                    d16FreeEXP(lpstNavexMemoryHandle);
                    d16FreeEXP(lpstEngineMemoryHandle);
                    FileClose(hNavexFile);
                    FileClose(hEngineFile);

                    return(EXTSTATUS_INIT_ERROR);
                }
            }
            else                        // QueryInterface failed in NAVENG
            {
                d16FreeEXP(lpstEngineMemoryHandle);
                FileClose(hEngineFile);
            }
        }
        else
        {
            FileClose(hEngineFile);
        }

                                        // No Engine Export Table
                                        // Execute EXTStartUp()
        if (prExportTable->EXTStartUp(hContext->lpstCallBack,
                                      0,
                                      NULL,
                                      szNavexDataDir,
                                      hContext->szNavexInfFile,
                                      NULL) == EXTSTATUS_OK)
        {
            hContext->prExportTable = prExportTable;
            hContext->lpstNavexMemoryHandle = lpstNavexMemoryHandle;
            return (EXTSTATUS_OK);
        }
    }
                                        // Failure
    d16FreeEXP(lpstNavexMemoryHandle);
    FileClose(hNavexFile);
    return(EXTSTATUS_INIT_ERROR);
}

EXTSTATUS WINAPI EXTCloseDX (HVCONTEXT hContext)
{
    EXTShutDown(hContext);

    if(hContext->lpstNavexMemoryHandle) // Free NAVEX15.EXP
        d16FreeEXP(hContext->lpstNavexMemoryHandle);

    if(hContext->lpstEngineMemoryHandle)// Free NAVENG.EXP
        d16FreeEXP(hContext->lpstEngineMemoryHandle);

    return (EXTSTATUS_OK);
}

#elif defined(SYM_NLM)

#include <symserve.h>

/*@API:**********************************************************************
@Declaration:
EXTSTATUS WINAPI EXTInitNLM (HVCONTEXT hContext)

@Description:
This routine dynamically loads NAVEX15.NLM and sets up hContext's
NAVEX15 function pointers.

This function has been NAVEX 1.5 enabled.

EXTCloseNLM() should be called to free the library.

@Parameters:
None.

@Returns:
EXTSTATUS_OK            if successful.
EXTSTATUS_INIT_ERROR    if failed loading library or function pointers.
EXTSTATUS_MEM_ERROR     if unable to allocate mem for export table.

@See:
@Include:   symserve.h
@Compatibility: NLM SDK 12
****************************************************************************/

EXTSTATUS WINAPI EXTInitNLM
(
    HVCONTEXT hContext
)
{
    PEXPORT15_TABLE_TYPE  prExportTable, prEngineTable;
    HINSTANCE             hExportLib, hEngineLib;
    TCHAR                 szExportFile[SYM_MAX_PATH + 1];
    TCHAR                 szEngineFile[SYM_MAX_PATH + 1];
    TCHAR                 szNavexDataDir[SYM_MAX_PATH + 1];
    LPFUNC_QUERYINTERFACE fpQueryNavexInterface, fpQueryEngineInterface;

    TCHAR szExportFileName[]   = "NAVEX15.NLM";
    TCHAR szEngineFileName[]   = "NAVENG.NLM";


    SYM_ASSERT (hContext);
                                        // Don't load lib more than once.
    if (hContext->prExportTable != NULL)
    {
        return (EXTSTATUS_OK);
    }

//------------------------------------------------------------------------
//
//  Get the pathname's for NAVEX15.NLM and NAVENG.NLM
//
//------------------------------------------------------------------------


                                        // Get the full Paths for:
                                        //   NAVEX15.NLM, NAVENG.NLM
                                        //   NAVEX15.INF, and data dir.
    STRCPY(szExportFile, hContext->szVirScanDataFile);
    NameStripFile(szExportFile);
    STRCPY(szEngineFile,    szExportFile);
    STRCPY(szNavexDataDir,  szExportFile);
    NameAppendFile(szExportFile, szExportFileName);
    NameAppendFile(szEngineFile, szEngineFileName);

//------------------------------------------------------------------------
//
//  Load NAVEX15.NLM and execute it's EXTQueryInterface().
//
//------------------------------------------------------------------------
                                        // Load NAVEX15.NLM.
    hExportLib = (HINSTANCE)LoadLibraryNLM(szExportFile);

                                        // Load Engine.NLM.
    hEngineLib = (HINSTANCE)LoadLibraryNLM(szEngineFile);

    if (hExportLib == NULL)
    {
        if (hEngineLib)
        {
            FreeLibraryNLM(hEngineLib);
        }

        return (EXTSTATUS_INIT_ERROR);
    }
                                        // Get function pointer to
                                        // NAVEX15.NLM's EXTQueryInterface()
    fpQueryNavexInterface = (LPFUNC_QUERYINTERFACE)
        (GetProcAddressNLM(hExportLib, "EXTQUERYINTERFACE"));

    if (fpQueryNavexInterface &&
       (*fpQueryNavexInterface)(NAVEX15_INTERFACE,
                               (LPLPVOID)&prExportTable) == EXTSTATUS_OK)
    {

//------------------------------------------------------------------------
//
//  NAVEX15.VXD Success!
//
//  Now, load NAVENG.VXD (if present) and execute it's EXTQueryInterface()
//
//------------------------------------------------------------------------

        if ( hEngineLib != NULL )
        {
            fpQueryEngineInterface = (LPFUNC_QUERYINTERFACE)
                (GetProcAddressNLM(hEngineLib, "EXTQUERYINTERFACEENG"));

            if (fpQueryEngineInterface &&
               (*fpQueryEngineInterface)(NAVEX15_INTERFACE,
                                        (LPLPVOID)&prEngineTable) == EXTSTATUS_OK)
            {
                                        // NAVENG32.DLL success!
                                        // Execute EXTStartUp()
                if (prExportTable->EXTStartUp(hContext->lpstCallBack,
                                              0,
                                              prEngineTable,
                                              szNavexDataDir,
                                              hContext->szNavexInfFile,
                                              NULL) == EXTSTATUS_OK)
                {
                    hContext->prExportTable = prExportTable;
                    hContext->hExportLib    = hExportLib;
                    hContext->prEngineTable = prEngineTable;
                    hContext->hEngineLib    = hEngineLib;

                    return (EXTSTATUS_OK);
                }
                else                    // EXTStartUp failure
                {
                    FreeLibraryNLM(hExportLib);
                    FreeLibraryNLM(hEngineLib);

                    return(EXTSTATUS_INIT_ERROR);
                }
            }
            else                       // QueryInterface() failed for NAVENG
            {
                FreeLibraryNLM(hEngineLib);
            }
        }
                                        // No Engine Export Table
                                        // Execute EXTStartUp()
        if (prExportTable->EXTStartUp(hContext->lpstCallBack,
                                      0,
                                      NULL,
                                      szNavexDataDir,
                                      hContext->szNavexInfFile,
                                      NULL) == EXTSTATUS_OK)
        {
            hContext->prExportTable = prExportTable;
            hContext->hExportLib    = hExportLib;
            return (EXTSTATUS_OK);
        }
    }
                                        // Failure
    FreeLibraryNLM(hExportLib);

    if (hEngineLib)
    {
        FreeLibraryNLM(hEngineLib);
    }

    return(EXTSTATUS_INIT_ERROR);
}


/*@API:**********************************************************************
@Declaration:
EXTSTATUS WINAPI EXTCloseNLM (HVCONTEXT hContext)

@Description:
This routine unloads then NAVEX15 NLM and zeros hContext's NAVEX
function pointers.

@Parameters:
None.

@Returns:
EXTSTATUS_OK    if successful or NavStat never initialized.

@See:
@Include:   navex.h
@Compatibility: NLM SDK 12
****************************************************************************/

EXTSTATUS WINAPI EXTCloseNLM
(
    HVCONTEXT hContext
)
{
    SYM_ASSERT (hContext);

    if (hContext->prExportTable == NULL)
    {
        return (EXTSTATUS_OK);
    }

/*                                       // Call EXTShutDown
    These changes made because we encountered some server abends with garbage
    in these (hContext->prExportTable) pointers. The original code follows.
    The release order was changed and the NULLing is done immediatly so no
    other thread can try to use the pointers. ==ddrew 5/1998

    EXTShutDown(hContext);

    FreeLibraryNLM(hContext->hExportLib);
    FreeLibraryNLM(hContext->hEngineLib);
*/

    EXTShutDown(hContext);

/*
    this considered too dangerous as this is a static filled in structure

    if (hContext->prExportTable != NULL)
        {
        MEMSET( hContext->prExportTable, 0, sizeof(EXPORT15_TABLE_TYPE));
        }
    if (hContext->prEngineTable != NULL)
        {
        MEMSET( hContext->prEngineTable, 0, sizeof(EXPORT15_TABLE_TYPE));
        }
*/

    hContext->prEngineTable = NULL;
    hContext->prExportTable = NULL;

    FreeLibraryNLM(hContext->hEngineLib);
    hContext->hEngineLib    = NULL;

    FreeLibraryNLM(hContext->hExportLib);
    hContext->hExportLib    = NULL;


    return (EXTSTATUS_OK);
}

#elif defined(SYM_UNIX)

#include <dlfcn.h>

/*@API:**********************************************************************
@Declaration:
EXTSTATUS WINAPI EXTInitUnix (HVCONTEXT hContext)

@Description:
This routine dynamically loads libnavex.so and sets up hContext's
NAVEX function pointers.

This function has been NAVEX 1.5 enabled.

EXTCloseUnix() should be called to free the library.

@Parameters:
None.

@Returns:
EXTSTATUS_OK            if successful.
EXTSTATUS_INIT_ERROR    if failed loading library or function pointers.
EXTSTATUS_MEM_ERROR     if unable to allocate mem for export table.

@See:
@Include:   navex.h
@Compatibility: Unix
****************************************************************************/

EXTSTATUS WINAPI EXTInitUnix
(
    HVCONTEXT hContext
)
{
    PEXPORT15_TABLE_TYPE  prExportTable, prEngineTable;
    void*                 hExportLib;
    void*                 hEngineLib;
    TCHAR                 szExportFile[SYM_MAX_PATH + 1];
    TCHAR                 szEngineFile[SYM_MAX_PATH + 1];
    TCHAR                 szNavexDataDir[SYM_MAX_PATH + 1];
    LPFUNC_QUERYINTERFACE fpQueryExportInterface, fpQueryEngineInterface;

    char szExportFileName[]  = "navexsol.so";    // NAVEX file name
    char szEngineFileName[]  = "navensol.so";   // NAVENGE file name

                                        // Validate the function arguments.
    SYM_ASSERT(NULL != hContext);

                                        // If NAVEX is already initialized,
                                        // just return success.
    if (hContext->prExportTable != NULL)
        return (EXTSTATUS_OK);

                                        // Get the full pathnames for:
                                        //   NAVEX (szExportFile)
                                        //   NAVENG (szEngineFile)
                                        //   NAVEX dir (szNavexDataDir)
    STRCPY(szExportFile, hContext->szVirScanDataFile);
    NameStripFile(szExportFile);
    STRCPY(szEngineFile,    szExportFile);
    STRCPY(szNavexDataDir,  szExportFile);
    NameAppendFile(szExportFile, szExportFileName);
    NameAppendFile(szEngineFile, szEngineFileName);


                                        // Try to load NAVEX and NAVENG.
    hExportLib = dlopen(szExportFile, RTLD_NOW);

#if defined(SYM_DEBUG)
    if (hExportLib == NULL)
        fprintf(stderr, "EXTInitUnix: dlopen(%s, RTLD_NOW) failed: %s\n", szExportFile, dlerror());
#endif

    hEngineLib = dlopen(szEngineFile, RTLD_NOW);

#if defined(SYM_DEBUG)
    if (hEngineLib == NULL)
        fprintf(stderr, "EXTInitUnix: dlopen(%s, RTLD_NOW) failed: %s\n", szEngineFile, dlerror());
#endif

                                        // If NAVEX is not available,
                                        // unload NAVENG and return failure.
    if (hExportLib == NULL)
    {
        if (hEngineLib != NULL)
            dlclose(hEngineLib);
        return (EXTSTATUS_INIT_ERROR);
    }
                                        // If we can find and call the NAVEX
                                        // EXTQueryInterface() function...
    fpQueryExportInterface = (LPFUNC_QUERYINTERFACE)
        (dlsym(hExportLib, "EXTQueryInterface"));

#if defined(SYM_DEBUG)
    if (fpQueryExportInterface == NULL)
        fprintf(stderr, "EXTInitUnix: dlsym(hExportLib, \"EXTQueryInterface\") failed: %s\n", dlerror());
#endif

    if (fpQueryExportInterface &&
        (*fpQueryExportInterface)(NAVEX15_INTERFACE,
                                  (LPLPVOID)&prExportTable) == EXTSTATUS_OK)
    {
                                        // If NAVENG is available...
        if (hEngineLib != NULL)
        {
                                        // If we can find and call the NAVENG
                                        // EXTQueryInterface() function...
            fpQueryEngineInterface = (LPFUNC_QUERYINTERFACE)
                (dlsym(hEngineLib, "EXTQueryInterface"));

#if defined(SYM_DEBUG)
            if (fpQueryEngineInterface == NULL)
                fprintf(stderr, "EXTInitUnix: dlsym(hEngineLib, \"EXTQueryInterface\") failed: %s\n", dlerror());
#endif

            if (fpQueryEngineInterface &&
                (*fpQueryEngineInterface)(NAVEX15_INTERFACE,
                                          (LPLPVOID)&prEngineTable) == EXTSTATUS_OK)
            {
                                        // Both libraries are linked & queried.
                                        // Call NAVEX EXTStartUp()
                                        // with the NAVENG table.
                                        // If successful, set up the context.
                if (prExportTable->EXTStartUp(hContext->lpstCallBack,
                                              0,
                                              prEngineTable,
                                              szNavexDataDir,
                                              hContext->szNavexInfFile,
                                              NULL) == EXTSTATUS_OK)
                {
                    hContext->prExportTable = prExportTable;
                    hContext->hExportLib    = hExportLib;
                    hContext->prEngineTable = prEngineTable;
                    hContext->hEngineLib    = hEngineLib;
                    return (EXTSTATUS_OK);
                }
                                        // Else NAVEX/NAVENG EXTStartUp failed.
                                        // Unload both and return failure.
                else
                {
                    dlclose(hExportLib);
                    dlclose(hEngineLib);
                    return (EXTSTATUS_INIT_ERROR);
                }
            }
                                       // Else NAVENG QueryInterface() failed.
                                       // Unload NAVENG and proceed with NAVEX.
            else
                dlclose(hEngineLib);
        }
                                        // NAVENG is not available.
                                        // Execute the NAVEX EXTStartUp()
                                        // without the NAVENG table.
                                        // If successful, set up the context.
        if (prExportTable->EXTStartUp(hContext->lpstCallBack,
                                      0,
                                      NULL,
                                      szNavexDataDir,
                                      hContext->szNavexInfFile,
                                      NULL) == EXTSTATUS_OK)
        {
            hContext->prExportTable = prExportTable;
            hContext->hExportLib    = hExportLib;
            return (EXTSTATUS_OK);
        }
    }

                                        // NAVEX QueryInterface() failed, or
                                        // NAVEX EXTStartUp() failed.
                                        // Unload both NAVEX and NAVENG and
                                        // return failure.
    dlclose(hExportLib);
    if (hEngineLib != NULL)
        dlclose(hEngineLib);
    return (EXTSTATUS_INIT_ERROR);
}


/*@API:**********************************************************************
@Declaration:
EXTSTATUS WINAPI EXTCloseUnix (HVCONTEXT hContext)

@Description:
This routine unloads libnavex.so and zeros hContext's NAVEX
function pointers.

@Parameters:
None.

@Returns:
EXTSTATUS_OK    if successful.

@See:
@Include:   navex.h
@Compatibility: Unix
****************************************************************************/

EXTSTATUS WINAPI EXTCloseUnix
(
    HVCONTEXT hContext
)
{
    SYM_ASSERT(hContext);

                                        // If NAVEX is initialized,
                                        // call NAVEX EXTShutDown().
    if (hContext->prExportTable != NULL)
        EXTShutDown(hContext);

                                        // If NAVEX is loaded, unload it
                                        // and zero the NAVEX context info.
    if (hContext->hExportLib != NULL)
    {
        dlclose(hContext->hExportLib);
        hContext->hExportLib = NULL;
        hContext->prExportTable = NULL;
    }

                                        // If NAVENG is loaded, unload it
                                        // and zero the NAVENG context info.
    if (hContext->hEngineLib != NULL)
    {
        dlclose(hContext->hEngineLib);
        hContext->hEngineLib = NULL;
        hContext->prEngineTable = NULL;
    }

    return (EXTSTATUS_OK);
}


#endif
