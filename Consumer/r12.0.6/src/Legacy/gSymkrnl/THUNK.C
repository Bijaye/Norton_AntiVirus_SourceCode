/* Copyright 1994 Symantec Corporation                                  */
/************************************************************************
 *                                                                      *
 * $Header:   S:/SYMKRNL/VCS/THUNK.C_v   1.1   26 Aug 1997 07:52:26   gvogel  $ *
 *                                                                      *
 * Description:                                                         *
 *      Contains 16-bit function which is called from Win32s Universal  *
 *      Thunk mechanism.                                                *
 *      Also contains 32-bit function which eventually calls the 16-bit *
 *      function using the UT mechanism.                                *
 *                                                                      *
 * Contains:                                                            *
 *                                                                      *
 * See Also:                                                            *
 *                                                                      *
 ************************************************************************
 * $Log:   S:/SYMKRNL/VCS/THUNK.C_v  $ *
// 
//    Rev 1.1   26 Aug 1997 07:52:26   gvogel
// Added DiskIsSubst to thunks.
// 
//    Rev 1.0   26 Jan 1996 20:21:46   JREARDON
// Initial revision.
// 
//    Rev 1.43   30 May 1995 18:10:02   REESE
// Added HWIsPCI function.
// 
//    Rev 1.42   10 Apr 1995 13:26:06   BRUCE
// Added debug info - but only when SYM_DEBUG is defined.  Also fixed freelibrary
// 
//    Rev 1.41   19 Mar 1995 19:15:42   TORSTEN
// Moved the typedef for SYMSYSHEAPINFO into THUNK.H
// 
//    Rev 1.40   19 Mar 1995 19:11:24   TORSTEN
// Made this thing compilable for WIN16...
// 
//    Rev 1.39   18 Mar 1995 22:25:08   DAVID
// Do not #include toolhelp.h on the W32 platform.  It defines a MAX_PATH
// of 255 which conflicts with one defined by the Win95 SDK headers.
// 
//    Rev 1.38   10 Mar 1995 01:17:56   BRUCE
// 
// Changed the lpbSwaped parm to _DiskIsDoubleSpace() to be a pointer to a
// WORD instead of a ptr to a BOOL because BOOL changes size between 32 and 16
// sides (and the thunk compiler doesn't do well with that).
// 
//    Rev 1.37   08 Mar 1995 19:42:58   BRIANF
// Macro'd thunking DLLs for Quake Custom usage.
//
//    Rev 1.36   26 Jan 1995 17:01:10   HENRI
// Updated to use quake 7
//
//    Rev 1.35   04 Jan 1995 16:43:58   RGLENN
// Added StackerFlushDrive()
//
//    Rev 1.34   27 Dec 1994 16:25:34   BRUCE
// Rewrote NameReturnLongName() so that no thunking is needed
//
//    Rev 1.33   21 Dec 1994 16:08:00   BRAD
// Make long/short name functions consistent with Quake APIs.
//
//    Rev 1.32   13 Dec 1994 13:42:08   BILL
//
//
//    Rev 1.31   13 Dec 1994 13:30:58   BILL
// Renamed SystemHeapInfo
//
//    Rev 1.30   09 Dec 1994 17:13:18   BILL
// Fixed file so it compiles
//
//    Rev 1.29   09 Dec 1994 16:42:50   BILL
// Removed FAR keyword
//
//
//    Rev 1.28   09 Dec 1994 15:12:46   HENRI
// Added SystemHeapInfo and GetSystemResources thunks
//
//    Rev 1.27   21 Nov 1994 19:05:22   BRUCE
// Added NameGetLong/ShortName() functions and thunks to get to them
//
//    Rev 1.26   16 Nov 1994 18:20:10   BRAD
// No longer thunk for MemCopyPhysical()
//
//    Rev 1.25   02 Nov 1994 20:11:36   RGLENN
// ore Stacker STuff
//
//    Rev 1.24   01 Nov 1994 11:58:26   DBUCHES
// Added StackerDiskBlockDevice thunk
//
//    Rev 1.23   27 Oct 1994 15:34:10   RGLENN
// Added thunks for StackerGet/PutUnitData
//
//    Rev 1.22   04 Oct 1994 16:13:10   DBUCHES
// Added thunk for DiskGetDriverTable
//
//    Rev 1.21   03 Oct 1994 17:00:34   DBUCHES
// Added thunk for DiskGetDASDType()
//
//    Rev 1.20   28 Sep 1994 18:27:02   DBUCHES
// Added thunk for DiskGetLockedTable
//
//    Rev 1.19   21 Sep 1994 16:01:30   DAVID
// More workarounds for thunk-induced exception violations under the
// debugger.  All pointers fields in structures are now declared as DWORDs
// to the thunk compiler, and an extra function argument is used to translate
// the field value separately.  The translated value is stuffed back into the
// structure before the 16-bit code is executed, and the original field value
// is restored before returning control back to the 32-bit caller.
//
//    Rev 1.18   01 Sep 1994 21:10:44   MARKK
// Commented out OutputDebugString since it was causing a crash under B169
//
//    Rev 1.17   15 Aug 1994 14:34:24   SCOTTP
// changed load library to the proper 6 ending
//
//    Rev 1.16   10 Aug 1994 16:06:06   Basil
// Fixed IsEISA and IsMicroChannel recursion problem (REESE/BASIL)
//
//    Rev 1.15   27 Jul 1994 16:17:22   DBUCHES
// Added thunk for DiskRecToFloppyAbsDiskRec
//
//    Rev 1.14   25 Jul 1994 13:33:10   DBUCHES
// Added _DiskIsDoubleSpace()
//
//    Rev 1.13   12 Jul 1994 16:04:50   HENRI
// Fixed DiskGetInfo32 thunk
//
//    Rev 1.12   07 Jul 1994 16:04:12   DBUCHES
// Added thunks for DiskIsStacker, DiskIsBlockDevice and DiskIsDriveSpace
//
//    Rev 1.11   06 Jul 1994 17:08:16   DBUCHES
// Added thunks for DiskGetPhysicalType,DiskIsCompressed,
// and DiskIsDoubleSpace.
//
//    Rev 1.10   29 Jun 1994 18:32:34   DBUCHES
// Added thunk for DiskGetHDParams
//
//    Rev 1.9   23 Jun 1994 17:58:28   HENRI
// Thunk DLL initialization.
//
//    Rev 1.8   29 Apr 1994 13:04:58   BRAD
// Added UTTRANSLATIONPROC
//
//    Rev 1.7   29 Apr 1994 12:40:46   BRAD
// Added run-time linking to Win32s 16-bit functions
//
//    Rev 1.6   27 Apr 1994 11:58:08   BRAD
// Added DiskReset() and DiskResetDOS() to Win32s thunks
//
//    Rev 1.5   26 Apr 1994 15:44:44   BRAD
// Added MemCopyPhysical() as Win32s thunk
//
//    Rev 1.4   26 Apr 1994 12:06:20   BRAD
// Added DiskMapLogToPhyParams() and DiskGetBootDrive() to Win32s thunks
//
//    Rev 1.3   25 Apr 1994 17:45:00   BRAD
// Added DiskBlockDevice() and DiskGetInfo() to WIN32s thunk
//
//    Rev 1.2   25 Apr 1994 12:14:18   BRAD
// Was using linear address, rather than segmented
//
//    Rev 1.1   22 Apr 1994 18:12:22   BRAD
// Added Win32s thunks for DiskAbsOperation() and DiskGetPhysicalInfo()
//
//    Rev 1.0   30 Mar 1994 15:56:58   BRAD
// Initial revision.
 ************************************************************************/

#include "platform.h"
#include "xapi.h"
#include "disk.h"

#if defined(SYM_WIN16)
    #include <toolhelp.h>
#endif

#include "thunk.h"

#include "quakever.h"
#include "cdisk.h"

#if defined(SYM_DEBUG)
	#ifndef SYM_OUTPUT_DEBUG
		#define SYM_OUTPUT_DEBUG(szOut) OutputDebugString(QMODULE_SYMKRNLTHUNK": "szOut)
	#endif
#else
	#ifndef SYM_OUTPUT_DEBUG
		#define SYM_OUTPUT_DEBUG(szOut) 
	#endif
#endif	


#ifdef SYM_WIN32

BOOL _stdcall thk_ThunkConnect32(LPSTR          pszDll16,
                                 LPSTR          pszDll32,
                                 HINSTANCE      hInst,
                                 DWORD          dwReason);


HINSTANCE          hInst_Thunk        = NULL;




/**********************************************************************/
//
//   DllMain()
//
// Description:
//
//   This is the equivalent of LibMain() for Win32 dlls.
//
//
// See Also:
//
//
/**********************************************************************/
//  2/ 1/94 BEM Function created.
/**********************************************************************/
BOOL WINAPI DllMain(HINSTANCE hInstance, DWORD dwReason, LPVOID lpReserved)
{
    SYM_OUTPUT_DEBUG( "DLLMain\n" );
#ifdef SYM_COMPILEDTHUNK
    if (!(thk_ThunkConnect32(QMODULE_SYMKRNLTHUNK_16 ".DLL",
                             QMODULE_SYMKRNLTHUNK_32 ".DLL",
                             hInstance,
                             dwReason)))
        {
        SYM_OUTPUT_DEBUG( "thk_ThunkConnect32 failed.\n" );
        return FALSE;
        }
    else
        SYM_OUTPUT_DEBUG( "thk_ThunkConnect32 succeeded!\n" );

#endif

    switch(dwReason)
        {
        case DLL_PROCESS_ATTACH:
        	SYM_OUTPUT_DEBUG("DllMain::DLL_PROCESS_ATTACH\n");
            hInst_Thunk = hInstance;
            break;

        case DLL_PROCESS_DETACH:
        	SYM_OUTPUT_DEBUG("DllMain::DLL_PROCESS_DETACH\n");
            break;

        case DLL_THREAD_ATTACH:
        	SYM_OUTPUT_DEBUG("DllMain::DLL_THREAD_ATTACH\n");
            break;
            
        case DLL_THREAD_DETACH:
        	SYM_OUTPUT_DEBUG("DllMain::DLL_THREAD_DETACH\n");
            break;
        }

    return TRUE;
}




BYTE SYM_EXPORT WINAPI DiskGetPhysicalInfo32(LPABSDISKREC lpAbsDisk)
{
#if defined(SYM_COMPILEDTHUNK)
    return(DiskGetPhysicalInfo16(lpAbsDisk));
#elif defined(SYM_RUNTIMETHUNK)
    return((BYTE) ThunkProc(lpAbsDisk, THUNK_DISKGETPHYSICALINFO, NULL));
#endif
}


BYTE SYM_EXPORT WINAPI DiskAbsOperation32 (BYTE command, LPABSDISKREC lpAbsDisk)
{
#if defined(SYM_COMPILEDTHUNK)
    LPVOID origBuffer = lpAbsDisk->buffer;
    BYTE byRet = DiskAbsOperation16(command, lpAbsDisk, lpAbsDisk->buffer);
    lpAbsDisk->buffer = origBuffer;
    return byRet;
#elif defined(SYM_RUNTIMETHUNK)
    TP_DISKABSOPERATION         tpDiskAbsOperation;

    tpDiskAbsOperation.command   = command;
    tpDiskAbsOperation.lpAbsDisk = lpAbsDisk;
    return((BYTE) ThunkProc(&tpDiskAbsOperation, THUNK_DISKABSOPERATION, NULL));
#endif
}

BOOL SYM_EXPORT WINAPI StackerGetUnitData32(BYTE drive, LPPERUNIT lpPerUnit)
{
#if defined(SYM_COMPILEDTHUNK)
    return(StackerGetUnitData16(drive, lpPerUnit));
#endif
}

BOOL SYM_EXPORT WINAPI StackerPutUnitData32(BYTE drive, LPPERUNIT lpPerUnit)
{
#if defined(SYM_COMPILEDTHUNK)
    return(StackerPutUnitData16(drive, lpPerUnit));
#endif
}


BOOL SYM_EXPORT WINAPI StackerFlushDrive32(BYTE drive)
{
#if defined(SYM_COMPILEDTHUNK)
    return (StackerFlushDrive16(drive));
#endif
}

BOOL SYM_EXPORT WINAPI DiskGetDriverTable32 (BYTE dl, LPDISKREC lpDiskPtr)
{
#if defined(SYM_COMPILEDTHUNK)
    return(DiskGetDriverTable16(dl, lpDiskPtr));
#endif
}

BOOL SYM_EXPORT WINAPI DiskGetHDParams32 (LPDISKREC     diskPtr,
                                 LPABSDISKREC   absDiskPtr,
                                 LPBPBREC       BPBPtr)
{
#if defined(SYM_COMPILEDTHUNK)
    return(DiskGetHDParams16(diskPtr, absDiskPtr, BPBPtr));
#endif
}

int SYM_EXPORT WINAPI DiskGetPhysicalType32(LPDISKREC diskPtr, LPABSDISKREC absDiskPtr)
{
#if defined(SYM_COMPILEDTHUNK)
    return(DiskGetPhysicalType16(diskPtr, absDiskPtr));
#endif
}

BOOL SYM_EXPORT WINAPI DiskRecToFloppyAbsDiskRec32 (LPCDISKREC diskPtr, LPABSDISKREC lpAbsDisk)
{
#if defined(SYM_COMPILEDTHUNK)
    return(DiskRecToFloppyAbsDiskRec16(diskPtr, lpAbsDisk));
#endif
}


BOOL SYM_EXPORT WINAPI DiskGetDASDType32(BYTE drive, LPBYTE DASDTypePtr)
{
#if defined(SYM_COMPILEDTHUNK)
    return(DiskGetDASDType16(drive, DASDTypePtr));
#endif
}


BOOL SYM_EXPORT WINAPI   DiskIsCompressed32(BYTE byDriveLetter)
{
#if defined(SYM_COMPILEDTHUNK)
    return(DiskIsCompressed16(byDriveLetter));
#endif
}

BOOL SYM_EXPORT WINAPI DiskIsDoubleSpace32(BYTE dl)
{
#if defined(SYM_COMPILEDTHUNK)
    return(DiskIsDoubleSpace16(dl));
#endif
}

BOOL SYM_EXPORT WINAPI DiskIsDriveSpace32(BYTE dl)
{
#if defined(SYM_COMPILEDTHUNK)
    return(DiskIsDriveSpace16(dl));
#endif
}

BOOL SYM_EXPORT WINAPI   DiskIsStacker32(BYTE byDriveLetter)
{
#if defined(SYM_COMPILEDTHUNK)
    return(DiskIsStacker16(byDriveLetter));
#endif
}

BOOL SYM_EXPORT WINAPI _DiskIsDoubleSpace32(BYTE drive, LPBYTE lpbyHostDriveLetter,
                                   LPWORD lpbSwapped, LPBYTE lpbySequence)
{
#if defined(SYM_COMPILEDTHUNK)
    return(_DiskIsDoubleSpace16(drive, lpbyHostDriveLetter, lpbSwapped, lpbySequence));
#endif
}


BOOL SYM_EXPORT WINAPI DiskIsBlockDevice32 (BYTE dl)
{
#if defined(SYM_COMPILEDTHUNK)
    return(DiskIsBlockDevice16(dl));
#endif
}


BOOL SYM_EXPORT WINAPI DiskIsSubst32(BYTE dl)
{
#if defined(SYM_COMPILEDTHUNK)
    return(DiskIsSubst16(dl));
#endif
}

UINT SYM_EXPORT WINAPI DiskBlockDevice32 (UINT uCommand, LPDISKREC lpDisk)
{
#if defined(SYM_COMPILEDTHUNK)
    LPVOID origBuffer = lpDisk->buffer;
    UINT wRet = DiskBlockDevice16 (uCommand, lpDisk, lpDisk->buffer);
    lpDisk->buffer = origBuffer;
    return wRet;
#elif defined(SYM_RUNTIMETHUNK)
    TP_DISKBLOCKDEVICE  tpDiskBlockDevice;

    tpDiskBlockDevice.wCommand = (WORD) uCommand;
    tpDiskBlockDevice.lpDisk   = lpDisk;
    return((UINT) ThunkProc(&tpDiskBlockDevice, THUNK_DISKBLOCKDEVICE, NULL));
#endif
}



VOID SYM_EXPORT WINAPI DiskMapLogToPhyParams32 (DWORD dwSector, BYTE dl,
                                   LPINT13REC lpInt13Package)
{
#if defined(SYM_COMPILEDTHUNK)
    DiskMapLogToPhyParams16 (dwSector, dl, lpInt13Package);
#elif defined(SYM_RUNTIMETHUNK)

    TP_DISKMAPLOGTOPHYPARAMS    tpDiskMapLogToPhyParams;

    tpDiskMapLogToPhyParams.dwSector       = dwSector;
    tpDiskMapLogToPhyParams.dl             = dl;
    tpDiskMapLogToPhyParams.lpInt13Package = lpInt13Package;
    ThunkProc(&tpDiskMapLogToPhyParams, THUNK_DISKMAPLOGTOPHYPARAMS, NULL);
#endif
}


VOID SYM_EXPORT WINAPI DiskGetLockedTable32(LPBYTE lockedTablePtr)
{
#if defined(SYM_COMPILEDTHUNK)
    DiskGetLockedTable16(lockedTablePtr);
#endif
}


BOOL SYM_EXPORT WINAPI DiskGetInfo32 (BYTE dl, LPDISKREC lpDisk)
{
#if defined(SYM_COMPILEDTHUNK)
    return( DiskGetInfo16(dl, lpDisk));
#elif defined(SYM_RUNTIMETHUNK)
    TP_DISKGETINFO              tpDiskGetInfo;

    tpDiskGetInfo.dl     = dl;
    tpDiskGetInfo.lpDisk = lpDisk;
    return((BYTE) ThunkProc(&tpDiskGetInfo, THUNK_DISKGETINFO, NULL));
#endif
}


BYTE SYM_EXPORT WINAPI DiskGetBootDrive32(VOID)
{
#if defined(SYM_COMPILEDTHUNK)
    return( DiskGetBootDrive16());
#elif defined(SYM_RUNTIMETHUNK)
    return ((BYTE)ThunkProc(NULL, THUNK_DISKGETBOOTDRIVE, NULL));
#endif
}

VOID SYM_EXPORT WINAPI DiskResetDOS32 (BYTE dl)
{
#if defined(SYM_COMPILEDTHUNK)
    DiskResetDOS16(dl);
#elif defined(SYM_RUNTIMETHUNK)
    ThunkProc(&dl, THUNK_DISKRESETDOS, NULL);
#endif
}

VOID SYM_EXPORT WINAPI DiskReset32(VOID)
{
#if defined(SYM_COMPILEDTHUNK)
    DiskReset16();
#elif defined(SYM_RUNTIMETHUNK)
    ThunkProc(NULL, THUNK_DISKRESET, NULL);
#endif
}

UINT SYM_EXPORT WINAPI   STK20DiskBlockDeviceThunk32(UINT command, LPDISKREC diskPtr)
{
#if defined(SYM_COMPILEDTHUNK)
    LPVOID origBuffer = diskPtr->buffer;
    UINT wRet = STK20DiskBlockDeviceThunk16(command, diskPtr, diskPtr->buffer);
    diskPtr->buffer = origBuffer;
    return(wRet);
#endif
}

BYTE SYM_EXPORT WINAPI EISACMOSRead32(BYTE     slot,
                                    BYTE     funct,
                                    LPBYTE   address)
{
#if defined(SYM_COMPILEDTHUNK)
    return( EISACMOSRead16(slot, funct, address));
#elif defined(SYM_RUNTIMETHUNK)
    TP_EISACMOSREAD         tpEISACMOSRead;

    tpEISACMOSRead.slot   = slot;
    tpEISACMOSRead.funct  =  funct;
    tpEISACMOSRead.address = address;
    return((BYTE) ThunkProc(&tpEISACMOSRead, THUNK_EISACMOSREAD, NULL));
#endif
}


BYTE SYM_EXPORT WINAPI EISACMOSWrite32(WORD     length,
                                     LPBYTE   address)
{
#if defined(SYM_COMPILEDTHUNK)
    return(EISACMOSWrite16(length, address));
#elif defined(SYM_RUNTIMETHUNK)
    TP_EISACMOSWRITE         tpEISACMOSWrite;

    tpEISACMOSWrite.length = length;
    tpEISACMOSWrite.address = address;
    return((BYTE) ThunkProc(&tpEISACMOSWrite, THUNK_EISACMOSWRITE, NULL));

#endif
}

BYTE SYM_EXPORT WINAPI CMOSRead32(BYTE address)
{
#if defined(SYM_COMPILEDTHUNK)
    return(CMOSRead16(address));
#elif defined(SYM_RUNTIMETHUNK)
     return ((BYTE) ThunkProc(&address, THUNK_CMOSREAD, NULL));
#endif
}

VOID SYM_EXPORT WINAPI CMOSWrite32(BYTE address, BYTE CMOSdata)
{
#if defined(SYM_COMPILEDTHUNK)
    CMOSWrite16(address, CMOSdata);
#elif defined(SYM_RUNTIMETHUNK)
    DWORD       dwPacked;

    dwPacked = MAKELONG(address, CMOSdata);
    ThunkProc(&dwPacked, THUNK_CMOSWRITE, NULL);
#endif
}

BOOL SYM_EXPORT WINAPI HWIsPCI32(VOID)
{
#if defined(SYM_COMPILEDTHUNK)
    return(HWIsPCI16());
#elif defined(SYM_RUNTIMETHUNK)
    return((BOOL) ThunkProc(NULL, THUNK_HWISPCI, NULL));
#endif
}

BOOL SYM_EXPORT WINAPI HWIsEISA32(VOID)
{
#if defined(SYM_COMPILEDTHUNK)
    return(HWIsEISA16());
#elif defined(SYM_RUNTIMETHUNK)
    return((BOOL) ThunkProc(NULL, THUNK_HWISEISA, NULL));
#endif
}


BOOL SYM_EXPORT WINAPI HWIsMicroChannel32(VOID)
{
#if defined(SYM_COMPILEDTHUNK)
    return(HWIsMicroChannel16());
#elif defined(SYM_RUNTIMETHUNK)
    return((BOOL) ThunkProc(NULL, THUNK_HWISMICROCHANNEL, NULL));
#endif
}


BOOL SYM_EXPORT WINAPI SystemHeapInfo32(SYMSYSHEAPINFO * lpSysHeap)
{
#if defined(SYM_COMPILEDTHUNK)
    return (SystemHeapInfo16(lpSysHeap));
#endif
}


WORD SYM_EXPORT GetFreeSystemResources32(WORD fuSysResource)
{
#if defined(SYM_COMPILEDTHUNK)
    return( GetFreeSystemResources16(fuSysResource) );
#endif
}

#endif                                  // #if defined(SYM_WIN32)

#if defined(SYM_RUNTIMETHUNK)
//************************************************************************
// Point all thunked routines to this entry.  That way, we have one
// procedure which can decide whether to use Win32s thunking or
// Chicago method.
//************************************************************************
DWORD SYM_EXPORT WINAPI ThunkProc(
        LPVOID  lpData,
        DWORD   dwFunction,
        LPVOID  *lpTranslationList)
{
    static      BOOL            bFirstTime = TRUE;
    static      UT32PROC        pfnThunkProc16 = NULL;

                                        // Only do a UTRegister(), if running
                                        // under Win32s.  Also, we only need
                                        // to register once.
                                        // Note: We do it here, rather than
                                        // in DllMain().  That way, programs
                                        // which don't need to use any of the
                                        // thunking stuff don't need to ship
                                        // with the 16-bit DLLs.
                                        // Win32s will execute UTUnRegister()
                                        // when this DLL is deattached, so we
                                        // won't worry about that.
     if ( bFirstTime )
        {
        extern  HINSTANCE       hInst_Thunk;
        auto    BOOL            bResult = TRUE;
        auto    UINT            uOldErr;
        auto    BOOL            bWin32s;
        auto    PUTREGISTER     pfnUTRegister;
        auto    HINSTANCE       hKernel32;


        bFirstTime = FALSE;
        bWin32s = (BOOL) (GetVersion() & 0x80000000);
        if ( !bWin32s )
            return(NOT_SUPPORTED);      // Not on Win32s - no further initialization needed

        uOldErr = SetErrorMode(SEM_NOOPENFILEERRORBOX | SEM_FAILCRITICALERRORS);
                                        // Get Handle to Kernel32.Dll
        hKernel32 = LoadLibrary("KERNEL32.DLL");
        pfnUTRegister = (PUTREGISTER) GetProcAddress( hKernel32, "UTRegister" );
        if ( !pfnUTRegister )
            bResult = FALSE;        // Error - On Win32s, can't find UTRegister/UTUnRegister

        if ( bResult )
            bResult = (*pfnUTRegister)(
                    hInst_Thunk,
                                        // Name of 16-bit DLL
                    QMODULE_SYMKRNL_16 ".DLL",
                    NULL,               // No initialization
                    "ThunkProc16",      // 16-bit Thunk function
                    &pfnThunkProc16,    // Ptr to 16-bit Thunk function
                    NULL,               // No 16-to-32 callback routine
                    NULL);              // No initialization buffer

        FreeLibrary( hKernel32 );       // KERNEL32 is always in there
        SetErrorMode(uOldErr);
        if (!bResult)                   // If error, make sure its NULL
            pfnThunkProc16 = NULL;
        }

     return((pfnThunkProc16) ? (*pfnThunkProc16)(lpData, dwFunction, lpTranslationList) : NOT_SUPPORTED);
}


#elif defined(SYM_WIN16)


HANDLE ghDLLInst;

// -----------------------------------------------------------------
//
// Function: LibMain
//
// Purpose : This is the DLL's entry point.  It is analogous to WinMain
//           for applications.
//
// Params  : hInstance   ==  The handle to the DLL's instance.
//           wDataSeg    ==  Basically it is a pointer to the DLL's
//                           data segment.
//           wHeapSize   ==  Size of the DLL's heap in bytes.
//           lpszCmdLine ==  The command line passed to the DLL
//                           by Windows.  This is rarely used.
//
// Returns : 1 indicating DLL initialization is successful.
//
// Comments: LibMain is called by Windows.  Do not call it in your
//           application!
// -----------------------------------------------------------------

int FAR PASCAL LibMain (HANDLE hInstance,
                        WORD   wDataSeg,
                        WORD   wHeapSize,
                        LPSTR  lpszCmdLine)
{
    ghDLLInst = hInstance;

    if (wHeapSize != 0)   // If DLL data seg is MOVEABLE
        UnlockData (0);

    return (1);
}


BOOL FAR PASCAL thk_ThunkConnect16(LPSTR pszDll16,
                                            LPSTR pszDll32,
                                            WORD  hInst,
                                            DWORD dwReason);

BOOL FAR PASCAL __export DllEntryPoint(DWORD dwReason,
                                       WORD  hInst,
                                       WORD  wDS,
                                       WORD  wHeapSize,
                                       DWORD dwReserved1,
                                       WORD  wReserved2)
{
    if (!(thk_ThunkConnect16(QMODULE_SYMKRNLTHUNK_16 ".DLL", QMODULE_SYMKRNLTHUNK_32 ".DLL", hInst, dwReason)))
        {
        SYM_OUTPUT_DEBUG("In SYMKRNL 16it DllEntryPoint: thkThunkConnect16 ret FALSE\n");
        return FALSE;
        }
    SYM_OUTPUT_DEBUG("In SYMKRNL 16bit DllEntryPoint: thkThunkConnect16 ret TRUE\n");
    return TRUE;
}


BYTE SYM_EXPORT WINAPI DiskGetPhysicalInfo16(LPABSDISKREC lpAbsDisk)
{
    return(DiskGetPhysicalInfo(lpAbsDisk));
}


BYTE SYM_EXPORT WINAPI DiskAbsOperation16 (BYTE command, LPABSDISKREC lpAbsDisk, LPVOID buffer)
{
    lpAbsDisk->buffer = buffer;
    return(DiskAbsOperation(command, lpAbsDisk));
}


UINT SYM_EXPORT WINAPI DiskBlockDevice16 (UINT uCommand, LPDISKREC lpDisk, LPVOID buffer)
{
    lpDisk->buffer = buffer;
    return(DiskBlockDevice (uCommand, lpDisk));
}

VOID SYM_EXPORT WINAPI DiskMapLogToPhyParams16 (DWORD dwSector, BYTE dl,
                                   LPINT13REC lpInt13Package)
{
    DiskMapLogToPhyParams (dwSector, dl, lpInt13Package);
}

BOOL SYM_EXPORT WINAPI DiskGetHDParams16 (LPDISKREC diskPtr, LPABSDISKREC absDiskPtr,
                                        LPBPBREC BPBPtr)
{
    return(DiskGetHDParams(diskPtr, absDiskPtr, BPBPtr));
}

VOID SYM_EXPORT WINAPI DiskGetLockedTable16(LPBYTE lockedTablePtr)
{
    DiskGetLockedTable(lockedTablePtr);
}

int SYM_EXPORT WINAPI DiskGetPhysicalType16(LPDISKREC diskPtr, LPABSDISKREC absDiskPtr)
{
    return(DiskGetPhysicalType(diskPtr,absDiskPtr));
}

BOOL SYM_EXPORT WINAPI   DiskIsCompressed16(BYTE byDriveLetter)
{
    return(DiskIsCompressed(byDriveLetter));
}

BOOL SYM_EXPORT WINAPI DiskIsDoubleSpace16(BYTE dl)
{
    return(DiskIsDoubleSpace(dl));
}

BOOL SYM_EXPORT WINAPI DiskIsBlockDevice16(BYTE dl)
{
    return(DiskIsBlockDevice(dl));
}

BOOL SYM_EXPORT WINAPI DiskIsSubst16(BYTE dl)
{
    return(DiskIsSubst(dl));
}

BOOL SYM_EXPORT WINAPI DiskIsDriveSpace16(BYTE dl)
{
    return(DiskIsDriveSpace(dl));
}

BOOL SYM_EXPORT WINAPI   DiskIsStacker16(BYTE byDriveLetter)
{
    return(DiskIsStacker(byDriveLetter));
}


BOOL SYM_EXPORT WINAPI _DiskIsDoubleSpace16(BYTE drive, LPBYTE lpbyHostDriveLetter,
                                   LPWORD lpbSwapped, LPBYTE lpbySequence)
{
    return(_DiskIsDoubleSpace(drive, lpbyHostDriveLetter, lpbSwapped, lpbySequence));
}

BOOL SYM_EXPORT WINAPI DiskGetInfo16 (BYTE dl, LPDISKREC lpDisk)
{
    return(DiskGetInfo(dl, lpDisk));
}

BOOL SYM_EXPORT WINAPI DiskRecToFloppyAbsDiskRec16 (LPCDISKREC diskPtr, LPABSDISKREC lpAbsDisk)
{
    return(DiskRecToFloppyAbsDiskRec(diskPtr, lpAbsDisk));
}

BYTE SYM_EXPORT WINAPI DiskGetBootDrive16(VOID)
{
    return(DiskGetBootDrive());
}

VOID SYM_EXPORT WINAPI DiskResetDOS16 (BYTE dl)
{
    DiskResetDOS(dl);
}

BOOL SYM_EXPORT WINAPI DiskGetDriverTable16 (BYTE dl, LPDISKREC lpDiskPtr)
{
    return(DiskGetDriverTable(dl, lpDiskPtr));
}


VOID SYM_EXPORT WINAPI DiskReset16(VOID)
{
    DiskReset();
}

BOOL SYM_EXPORT WINAPI DiskGetDASDType16(BYTE drive, LPBYTE DASDTypePtr)
{
    return(DiskGetDASDType(drive, DASDTypePtr));
}


BOOL SYM_EXPORT WINAPI StackerGetUnitData16(BYTE drive, LPPERUNIT lpPerUnit)
{
    return(StackerGetUnitData(drive, lpPerUnit));
}

BOOL SYM_EXPORT WINAPI StackerPutUnitData16(BYTE drive, LPPERUNIT lpPerUnit)
{
    return(StackerPutUnitData(drive, lpPerUnit));
}

UINT SYM_EXPORT WINAPI   STK20DiskBlockDeviceThunk16(UINT command, LPDISKREC diskPtr, LPVOID buffer)
{
    diskPtr->buffer = buffer;
    return(STK20DiskBlockDeviceThunk( NULL, command, diskPtr));
}

BOOL SYM_EXPORT WINAPI StackerFlushDrive16(BYTE drive)
{
    return (StackerFlushDrive(drive));
}


BYTE SYM_EXPORT WINAPI EISACMOSRead16(BYTE     slot,
                                    BYTE     funct,
                                    LPBYTE   address)
{
    return(EISACMOSRead(slot, funct, address));
}


BYTE SYM_EXPORT WINAPI EISACMOSWrite16(WORD     length,
                                     LPBYTE   address)
{
    return(EISACMOSWrite(length,address));
}

BYTE SYM_EXPORT WINAPI CMOSRead16(BYTE address)
{
    return(CMOSRead(address));
}

VOID SYM_EXPORT WINAPI CMOSWrite16(BYTE address, BYTE CMOSdata)
{
    CMOSWrite(address, CMOSdata);
}

BOOL SYM_EXPORT WINAPI HWIsPCI16(VOID)
{
    return(HWIsPCI());
}

BOOL SYM_EXPORT WINAPI HWIsEISA16(VOID)
{
    return(HWIsEISA());
}

BOOL SYM_EXPORT WINAPI HWIsMicroChannel16(VOID)
{
    return(HWIsMicroChannel());
}

BOOL SYM_EXPORT WINAPI SystemHeapInfo16(SYMSYSHEAPINFO FAR * lpSysHeap)
{
    return (SystemHeapInfo((SYMSYSHEAPINFO FAR * )lpSysHeap));
}

WORD SYM_EXPORT WINAPI GetFreeSystemResources16(WORD fuSysResource)
{
    return( GetFreeSystemResources(fuSysResource) );
}

#endif



