/* Copyright 1992 Symantec Corporation                                  */
/************************************************************************
 *                                                                      *
 * $Header:   S:/SYMKRNL/VCS/SYMKRNL.C_v   1.5   23 Apr 1998 10:45:58   TSmith  $ *
 *                                                                      *
 * Description:                                                         *
 *                                                                      *
 * Contains:                                                            *
 *                                                                      *
 * See Also:                                                            *
 *                                                                      *
 ************************************************************************
 * $Log:   S:/SYMKRNL/VCS/SYMKRNL.C_v  $ *
// 
//    Rev 1.5   23 Apr 1998 10:45:58   TSmith
// Removed call to obsoleted function 'DBCSFreeFontFaceMemory'.
// 
//    Rev 1.4   07 Nov 1997 10:07:52   TSMITH
// Added call to 'DBCSFreeFontFaceMemory' to SymKRNLAttach DLL_PROCESS_DETACH case.
// This releases memory allocated by calls to 'DBCSGetFontFace'.
// 
//    Rev 1.3   10 Aug 1997 15:23:26   bsobel
// Changed to dynamically load thunk dll on win32 only when accessed for the first time.
// 
//    Rev 1.2   19 Mar 1997 21:38:00   BILL
// Modified files to build properly under Alpha platform
// 
//    Rev 1.1   04 Dec 1996 15:05:14   RCHINTA
// Added hImm32Dll and changed SYMKRNLAttach() to fix problems
// in the process of porting changes related to IME from Quake 9
// to CORE (in SYMINTL.C).
// 
//    Rev 1.0   26 Jan 1996 20:22:54   JREARDON
// Initial revision.
// 
//    Rev 1.59   16 Nov 1995 21:32:40   GVOGEL
// 
//    Rev 1.58   30 May 1995 18:10:06   REESE
// Added HWIsPCI function.
// 
//    Rev 1.57   28 Apr 1995 18:10:16   BRAD
// Don't load thunks for NT
// 
//    Rev 1.56   27 Apr 1995 20:36:26   MFALLEN
// Fixed typo.
// 
//    Rev 1.55   27 Apr 1995 16:38:30   BRAD
// Added SystemWin95ShellPresent()
// 
//    Rev 1.54   20 Apr 1995 21:49:10   MBARNES
// Extended SystemGetWindowsType() to work in DOS boxes
// 
//    Rev 1.53   19 Apr 1995 20:09:34   DAVID
// Cleaned up the management of the Symkrnl VxD
// 
//    Rev 1.52   10 Apr 1995 13:26:04   BRUCE
// Added debug info - but only when SYM_DEBUG is defined.  Also fixed freelibrary
// 
//    Rev 1.51   21 Mar 1995 14:41:58   MFALLEN
// Created a SymKRNLAttach() and a SymKRNLDetach() function for the 16-bit
// platform also.
// 
//    Rev 1.50   08 Mar 1995 19:48:22   BRIANF
// Added QUAKEVER.H
//
//    Rev 1.49   08 Mar 1995 19:42:56   BRIANF
// Macro'd thunking DLLs for Quake Custom usage.
//
//    Rev 1.48   02 Mar 1995 11:44:50   BRIANF
// Made changes that are necessary to build custom Quake DLLs.
//
//    Rev 1.47   13 Feb 1995 12:13:36   MFALLEN
// Free memory for command line processing when DllMain() receives a detach.
//
//    Rev 1.46   07 Feb 1995 22:31:10   MFALLEN
// Debugged for W32 and DOS
//
//    Rev 1.44   07 Feb 1995 18:18:34   MFALLEN
// Code was assuming that command line parameters can never be larger than
// SYM_MAX_PATH. This is not valid for W32 so buffer must be allocated at run-time.
// This also forces us to use a new function called ConfigProcessCmdLineEnd() to
// free memory that was allocated for command line processing. Applications that
// call ConfigProcessCmdLineEx() must call ConfigProcessCmdLineEnd().
//
//
//    Rev 1.43   02 Feb 1995 17:56:20   BRAD
// Added SYMKRNL VxD changes from QAK6
//
//    Rev 1.42   26 Jan 1995 17:23:10   HENRI
// Updated to use quake 7
//
//    Rev 1.41   13 Dec 1994 13:30:52   BILL
// Renamed SystemHeapInfo
//
//    Rev 1.40   09 Dec 1994 20:10:22   BILL
// Removed platwin (already included)
//
//    Rev 1.39   09 Dec 1994 20:03:18   BILL
// Added include of platwin
//
//    Rev 1.38   09 Dec 1994 19:08:38   BILL
// Added WIN32 functions GetFreeSystemResources and SystemHeapInfo
//
//    Rev 1.37   09 Dec 1994 15:55:48   BRAD
// Removed SystemIsWin...() and replaced with SystemGetWindowsType()
//
//    Rev 1.36   22 Nov 1994 16:27:54   BRAD
// Added WinNT abs. disk read/write
//
//    Rev 1.35   16 Nov 1994 18:20:00   BRAD
// No longer thunk for MemCopyPhysical()
//
//    Rev 1.34   25 Oct 1994 19:34:24   HENRI
// Now using the common IOCtlVWin32 for all WIN32 IOCTL functions
//
//    Rev 1.33   21 Oct 1994 11:58:18   BRAD
// Changed name of routines for initing DBCS tables
//
//    Rev 1.32   20 Oct 1994 18:28:08   BRAD
// Init DBCS tables, when starting up
//
//    Rev 1.31   01 Sep 1994 21:07:36   MARKK
// Commented out OutputDebugString since it was causing a crash under B169
//
//    Rev 1.30   15 Aug 1994 14:27:44   SCOTTP
// fixed load library to use proper 6 ending
//
//    Rev 1.29   06 Jul 1994 11:50:48   HENRI
// Turn optimizations off on LibMain and WEP for thunking to work properly
//
//    Rev 1.28   23 Jun 1994 17:26:14   HENRI
// Changed names of thunking DLL's
//
//    Rev 1.27   23 Jun 1994 16:12:02   HENRI
// Removed Chicago style thunk initialization
//
//    Rev 1.26   22 Jun 1994 12:26:02   HENRI
// Added thunking layers
//
//    Rev 1.25   29 Apr 1994 12:40:48   BRAD
// Added run-time linking to Win32s 16-bit functions
//
//    Rev 1.24   05 Apr 1994 18:09:02   BRAD
// Upon detach, do a DiskCacheTerminate()
//
//    Rev 1.23   30 Mar 1994 16:09:54   BRAD
// Added Thunk stuff.
//
//    Rev 1.22   21 Mar 1994 00:20:22   BRAD
// Cleaned up for WIN32
//
//    Rev 1.21   15 Mar 1994 12:35:02   BRUCE
// Changed EXPORT to SYM_EXPORT
//
//    Rev 1.20   11 Mar 1994 13:33:04   PGRAVES
// Added SYMKRNL16ThunkProc (stub).
//
//    Rev 1.19   02 Mar 1994 17:58:46   PGRAVES
// SYM_SYM_WIN32 -> SYM_WIN32.
//
//    Rev 1.18   25 Feb 1994 15:03:24   BRUCE
// Reinstated FP_SEG, FP_OFF, MAKELP
//
//    Rev 1.17   25 Feb 1994 12:23:36   BRUCE
// Ran scripts to change to SYM_ constants and get ready for Win32
//
//    Rev 1.16   14 Feb 1994 14:05:40   JOHN
// Added DiskGetUnderSymEvent().
//
//    Rev 1.15   09 Feb 1994 17:58:58   PGRAVES
// Win32.
//
//    Rev 1.14   01 Feb 1994 17:29:54   BRUCE
// Added win32 platform code
//
//    Rev 1.13   14 Dec 1993 13:43:18   TONY
// OS/2 support through #ifndefs.  May need to add chunks back later.
//
//
//    Rev 1.12   12 Oct 1993 17:46:22   EHU
// Ifdef'd out code not used in SYM_NLM.
//
//    Rev 1.11   27 Aug 1993 14:52:36   HENRI
// Added call to DiskCacheTerminate in WEP function
//
//    Rev 1.10   04 Aug 1993 19:49:44   HENRI
// Check if hInst_SHELL is NULL before loading SHELL.DLL library
//
//    Rev 1.9   30 Jul 1993 15:11:24   KEITH
// Shell.DLL does not exist in OS/2 and the undocumented calls made are
// not supported in Mirrors.DLL. SymKrnlInitDll() was changed such that
// the unsupported function pointers are initialized to point to stubs.
//
//
//    Rev 1.8   22 Jul 1993 18:34:12   JOHN
// Added code to WEP() to free selector allocated by DOSIntlStringCompare().
//
//    Rev 1.7   09 Jul 1993 10:03:18   KEITH
// Modified LibMain for OS/2 Mirrors only to eliminate the version get.
// This is not necessary in this environment and caused problems.
//
// &? The ASM code in LibMain should be further checked and the unused and
// &? redundant operations removed.
//
//    Rev 1.6   02 Jul 1993 09:51:58   ENRIQUE
// Set _dosmajor and _dosminor in the Libmain of symkrnl.
//
//    Rev 1.5   22 Jun 1993 19:55:20   DLEVITON
// Include quakever.h instead of quake.ver.
//
//    Rev 1.4   21 Jun 1993 16:08:22   DLEVITON
// Eliminated reference to obsolete version.h
//
//    Rev 1.3   18 Jun 1993 13:23:40   EHU
// Don't include shellapi.h and leave out DoEnvironmentSubst stuff for SYM_NLM.
//
//    Rev 1.2   17 Jun 1993 19:56:14   MARKL
// SHELL.DLL is now dynamically loaded by SymKrnl.  The rouitines RegOpenKey(),
// RegCloseKey(), RegQueryValue(), and DoEnvironmentSubst() are now called
// through function pointers.  In the case of DOS, a function pointer has
// been established for DoEnvironmentSubst() to allow consistant code in both
// the Windows and DOS platforms.
//
//    Rev 1.1   16 Jun 1993 07:58:36   ED
// [Enh] Added SYMKRNLGetBuild for doing a really simple external sanity check.
// This function is exported, but it is declared NONAME so it will not show up
// when exports are dumped.
//
//
//    Rev 1.0   12 Feb 1993 04:26:50   ENRIQUE
// Initial revision.
//
//    Rev 1.2   08 Jan 1993 13:33:46   HENRI
// Added hInst_SYMPRIM to save the hinstance of symprim.
//
//    Rev 1.1   09 Nov 1992 08:44:42   ED
// Added the GetVer function
//
//    Rev 1.0   09 Nov 1992 08:43:18   ED
// Initial revision.
//
//    Rev 1.0   26 Aug 1992 17:00:46   ED
// Initial revision.
//
//    Rev 1.0   20 Jul 1992 14:21:20   ED
// Initial revision.
 ************************************************************************/

#include "platform.h"
#include "disk.h"
#include "xapi.h"
#include "symkrnl.h"
#include "dbcs.h"
#include "thunk.h"
#include "symcfg.h"                     // ConfigProcessCmdLineEnd()
#include "quakever.h"

#if defined(SYM_WIN)                    // Only valid for WIN

HINSTANCE  hInst_SYMKRNL = NULL;

#if defined(SYM_DEBUG)
    #ifndef SYM_OUTPUT_DEBUG
        #define SYM_OUTPUT_DEBUG(szOut) OutputDebugString(QMODULE_SYMKRNL": "szOut)
    #endif
#else
    #ifndef SYM_OUTPUT_DEBUG
        #define SYM_OUTPUT_DEBUG(szOut) 
    #endif
#endif  


#if defined(SYM_WIN32)                  // Win32 specific stuff

#ifndef _M_ALPHA
HINSTANCE  hInstThunkDll = NULL;
#endif
HINSTANCE  hImm32Dll = NULL;

////////////////////////////////////////////////////////////////////////
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
////////////////////////////////////////////////////////////////////////
//  2/ 1/94 BEM Function created.
////////////////////////////////////////////////////////////////////////
#ifndef QCUST // Ignore DLLMAin in case of  custom quake
BOOL WINAPI DllMain(HINSTANCE hInstance, DWORD dwReason, LPVOID lpReserved)
{
    return(SymKRNLAttach(hInstance, dwReason, lpReserved));
}
#endif

/*-----------------03-01-95 04:52pm-----------------
 SymKRNLAttach initializes any global data and performs
 other initialization routines.  This is here so that
 Custom Quake DLLs can access SymKRNL's Initialization
 items without having to manually parse KRNL's LibMain.
--------------------------------------------------*/

BOOL WINAPI SymKRNLAttach(HINSTANCE hInstance, DWORD dwReason, LPVOID lpReserved)
{
#ifndef _M_ALPHA
    extern HANDLE hVWin32;              // located is DSK_ACCS.C
    extern HANDLE hSymkrnlVxD;          // located in DSK_ACCS.C
    UINT    uType;
#endif    


    hInst_SYMKRNL = hInstance;

    switch(dwReason)
        {
        case DLL_PROCESS_ATTACH:
            SYM_OUTPUT_DEBUG("DllMain, DLL_PROCESS_ATTACH\n");
#ifndef _M_ALPHA
            uType = SystemGetWindowsType();
            if (uType == SYM_SYSTEM_WIN_32S)
                {
                SYM_OUTPUT_DEBUG("on Win32s\n");
                SYM_OUTPUT_DEBUG("Loading TKKES327.DLL\n");
                hInstThunkDll = LoadLibrary( "TKKES327.DLL" );
                }
            else if (uType == SYM_SYSTEM_WIN_95)
                {
                // WES 08/10/97 Defered loading until used the first time.
                SYM_OUTPUT_DEBUG("on Windows 9x\n");
                //SYM_OUTPUT_DEBUG("Loading " QMODULE_SYMKRNLTHUNK ".DLL\n");
                //hInstThunkDll = LoadLibrary( QMODULE_SYMKRNLTHUNK ".DLL" );
                }
#endif                
            DBCSSetInternalTable();
            break;

        case DLL_PROCESS_DETACH:
            SYM_OUTPUT_DEBUG("DLLMain, DLL_PROCESS_DETACH\n");
#ifndef _M_ALPHA      
            if (hInstThunkDll > (HINSTANCE)HINSTANCE_ERROR)
                {
                SYM_OUTPUT_DEBUG("DllMain, about to FreeLibrary(" QMODULE_SYMKRNLTHUNK ".DLL)\n");
                FreeLibrary( hInstThunkDll );
                }
#endif
                                if(hImm32Dll)
                                {
                                        FreeLibrary(hImm32Dll);
                                }

                                        // ----------------------------------
                                        // Free up memory that was used for
                                        // command line processing.
                                        // ----------------------------------
            ConfigProcessCmdLineEnd ( NULL );

#ifndef _M_ALPHA      
            if (hVWin32)
                CloseHandle((HANDLE)hVWin32);

            if (hSymkrnlVxD)
                VxDServicesTerminate();
#endif                
            break;

        case DLL_THREAD_ATTACH:
            SYM_OUTPUT_DEBUG("DLLMain, DLL_THREAD_ATTACH\n");
            break;

        case DLL_THREAD_DETACH:
            SYM_OUTPUT_DEBUG("DLLMain, DLL_THREAD_DETACH\n");
            break;
        }

    return (TRUE);
}


#else

BYTE _dosmajor;
BYTE _dosminor;

HINSTANCE hWin32s16;
HINSTANCE hInst_SYMEVENT;


/****************************************************************************
   FUNCTION: LibMain(HANDLE, WORD, WORD, LPSTR)

   PURPOSE:  Is called by LibEntry.  LibEntry is called by Windows when
             the DLL is loaded.  The LibEntry routine is provided in
             the LIBENTRY.OBJ in the SDK Link Libraries disk.  (The
             source LIBENTRY.ASM is also provided.)
             the initialization is successful.

*******************************************************************************/
#pragma optimize("", off)       // required for thunking to work

#ifndef QCUST                           // Ignore LibMain when custom quake

int WINAPI LibMain (
    HINSTANCE   hModule,
    UINT        wDataSeg,
    UINT        cbHeapSize,
    LPCSTR      lpszCmdLine
    )
{
    SymKRNLAttach ( hModule );

    if (cbHeapSize != 0)
        UnlockData(0);

    return (1);
}

#endif // #ifdef QCUST

/****************************************************************************
    FUNCTION:   SymKRNLAttach(int)

    PURPOSE:    SymKRNLAttach initializes any global data and performs
                other initialization routines.  This is here so that
                Custom Quake DLLs can access SymKRNL's Initialization
                items without having to manually parse KRNL's LibMain.
                This function is for 16-bit only. There's a 32-bit version
                with the same name.

*******************************************************************************/

BOOL WINAPI SymKRNLAttach (
    HINSTANCE   hInstance
    )
{
#ifndef SYM_MIRRORS
    auto    BYTE                byFlags;
    auto    BYTE                byOEMCode;
    auto    BYTE                byUser1;
    auto    WORD                wUser2;
#endif

    hInst_SYMKRNL = hInstance;

                                        // get the DOS version
#ifndef SYM_MIRRORS
    _asm {
        push    si
        push    di
        push    es
        push    ds
        push    bp

        mov     ax, 03000h
        int     21h

        xchg    ah, al                  ; returns minor in AH, major in AL

        mov     [_dosmajor], ah
        mov     [_dosminor], al
        mov     [byOEMCode], bh
        mov     [byUser1], bl
        mov     [wUser2], cx

        pop     bp
        pop     ds
        pop     es
        pop     di
        pop     si
        };
                                        // get the real DOS 5.00+ version
    _asm {
        push    si
        push    di
        push    es
        push    ds
        push    bp

        mov     bl, [_dosmajor]
        mov     bh, [_dosminor]
        xor     dx, dx

        mov     ax, 03306h
        int     21h

        mov     [_dosmajor], bl
        mov     [_dosminor], bh
        mov     [byFlags], dh

        pop     bp
        pop     ds
        pop     es
        pop     di
        pop     si
        };
#endif

    DBCSSetInternalTable();

    return ( TRUE );
}

/****************************************************************************
    FUNCTION:  WEP(int)

    PURPOSE:  Performs cleanup tasks when the DLL is unloaded.  WEP() is
              called automatically by Windows when the DLL is unloaded (no
              remaining tasks still have the DLL loaded).  It is strongly
              recommended that a DLL have a WEP() function, even if it does
              nothing but returns success (1), as in this example.

*******************************************************************************/

#ifndef QCUST                           // Ignore LibMain when custom quake

int SYM_EXPORT WINAPI WEP (
    int     bSystemExit
    )
{
    SymKRNLDetach();
    return(1);
}

#endif                                  // #ifndef QCUST

/****************************************************************************
    FUNCTION:  SymKRNLDetach(VOID)

    PURPOSE:  Performs cleanup tasks when the DLL is unloaded.  WEP() is
              called automatically by Windows when the DLL is unloaded (no
              remaining tasks still have the DLL loaded).  It is strongly
              recommended that a DLL have a WEP() function, even if it does
              nothing but returns success (1), as in this example.

*******************************************************************************/

BOOL WINAPI SymKRNLDetach (
    VOID
    )
{
    extern HINSTANCE    hInst_SYMEVENT;
    extern HINSTANCE    hWin32s16;
    extern WORD         gwCollatingTableSelector; // From PRM_STR.C

    DiskCacheTerminate();

    if (hInst_SYMEVENT > HINSTANCE_ERROR)
        FreeLibrary(hInst_SYMEVENT);

    if (hWin32s16 > HINSTANCE_ERROR)
        FreeLibrary(hWin32s16);

    if (gwCollatingTableSelector)
        SelectorFree(gwCollatingTableSelector);

                                        // ----------------------------------
                                        // Free up memory that was used for
                                        // command line processing.
                                        // ----------------------------------
    ConfigProcessCmdLineEnd ( NULL );

    return ( TRUE );
}

#pragma optimize("", on)



//************************************************************************
// Make sure you update THUNK.H, when adding a new function.
//************************************************************************

DWORD SYM_EXPORT FAR PASCAL ThunkProc16(
        LPVOID  lpData,
        DWORD   dwFunction)
{
    static      BOOL               bFirstTime = TRUE;
    static      UTTRANSLATIONPROC  lpfnUTSelectorOffsetToLinear, lpfnUTLinearToSelectorOffset;

                                        // Make sure we have the translation
                                        // routines
    if ( bFirstTime )
        {
        UINT    uOldErr;

        bFirstTime = FALSE;
        uOldErr = SetErrorMode(SEM_NOOPENFILEERRORBOX | SEM_FAILCRITICALERRORS);
                                    // Get Handle to Kernel32.Dll
        hWin32s16 = LoadLibrary("WIN32S16.DLL");

        lpfnUTSelectorOffsetToLinear = (UTTRANSLATIONPROC)
                        GetProcAddress(hWin32s16, "UTSelectorOffsetToLinear");
        lpfnUTLinearToSelectorOffset = (UTTRANSLATIONPROC)
                        GetProcAddress(hWin32s16, "UTLinearToSelectorOffset");

        SetErrorMode(uOldErr);
        }

    if ( lpfnUTSelectorOffsetToLinear == NULL )
        return (NOT_SUPPORTED);

    switch(dwFunction)
        {
        case THUNK_CMOSREAD:                         // Read CMOS
            return (CMOSRead(*((LPBYTE) lpData)));

        case THUNK_CMOSWRITE:                        // Write CMOS
            {
            DWORD       dwPacked;

            dwPacked = *((LPDWORD) lpData);
            CMOSWrite((BYTE) LOWORD(dwPacked), (BYTE) HIWORD(dwPacked));
            return(0);
            }

        case THUNK_HWISPCI:
            return (HWIsPCI());

        case THUNK_HWISEISA:
            return (HWIsEISA());

        case THUNK_HWISMICROCHANNEL:
            return (HWIsMicroChannel());

        case THUNK_DISKABSOPERATION:
            {
            LPTP_DISKABSOPERATION lptpDiskAbsOperation =
                                        (LPTP_DISKABSOPERATION) lpData;
            ABSDISKREC          absDisk;
            LPABSDISKREC        lpAbsDisk;
            BYTE                byRet;


            lpAbsDisk = (LPABSDISKREC) (*lpfnUTLinearToSelectorOffset)((LPBYTE) lptpDiskAbsOperation->lpAbsDisk);
            absDisk = *lpAbsDisk;
            absDisk.buffer = (LPVOID) (*lpfnUTLinearToSelectorOffset)(
                                       (LPBYTE) lpAbsDisk->buffer);
            byRet = DiskAbsOperation(lptpDiskAbsOperation->command, &absDisk);
            lpAbsDisk->sectorsProcessed = absDisk.sectorsProcessed;
            return(byRet);
            }

        case THUNK_DISKGETPHYSICALINFO:
            {
            LPABSDISKREC        lpAbsDisk = (LPABSDISKREC) lpData;
            BYTE                byRet;

            byRet = DiskGetPhysicalInfo(lpAbsDisk);
            lpAbsDisk->buffer = (*lpfnUTSelectorOffsetToLinear)(lpAbsDisk->buffer);
            return(byRet);
            }

        case THUNK_DISKBLOCKDEVICE:
            {
            LPTP_DISKBLOCKDEVICE lptpDiskBlockDevice =
                                        (LPTP_DISKBLOCKDEVICE) lpData;
            DISKREC             disk;
            LPDISKREC           lpDisk;


            lpDisk = (LPDISKREC) (*lpfnUTLinearToSelectorOffset)((LPBYTE) lptpDiskBlockDevice->lpDisk);
            disk = *lpDisk;
            disk.buffer = (LPVOID) (*lpfnUTLinearToSelectorOffset)(
                                       (LPBYTE) lpDisk->buffer);
            return (DiskBlockDevice(lptpDiskBlockDevice->wCommand, &disk));
            }

        case THUNK_DISKGETINFO:
            {
            LPTP_DISKGETINFO lptpDiskGetInfo =
                                        (LPTP_DISKGETINFO) lpData;
            LPDISKREC           lpDisk;


            lpDisk = (LPDISKREC) (*lpfnUTLinearToSelectorOffset)((LPBYTE) lptpDiskGetInfo->lpDisk);
            return (DiskGetInfo(lptpDiskGetInfo->dl, lpDisk));
            }

        case THUNK_DISKMAPLOGTOPHYPARAMS:
            {
            LPTP_DISKMAPLOGTOPHYPARAMS lptpDiskMapLogToPhyParams =
                                        (LPTP_DISKMAPLOGTOPHYPARAMS) lpData;
            LPINT13REC          lpInt13Package;


            lpInt13Package = (LPINT13REC) (*lpfnUTLinearToSelectorOffset)((LPBYTE) lptpDiskMapLogToPhyParams->lpInt13Package);
            DiskMapLogToPhyParams(lptpDiskMapLogToPhyParams->dwSector,
                                  lptpDiskMapLogToPhyParams->dl,
                                  lpInt13Package);
            return(0);
            }

        case THUNK_DISKGETBOOTDRIVE:
            return (DiskGetBootDrive());

        case THUNK_DISKRESETDOS:
            {
            DiskResetDOS(*((LPBYTE) lpData));
            return (0);
            }

        case THUNK_DISKRESET:
            {
            DiskReset();
            return (0);
            }
        }

    return(0);
}

#endif     //  #if !defined(SYM_WIN32)


#if defined(SYM_WIN32)
BOOL __declspec(dllexport) WINAPI SymSystemHeapInfo(SYMSYSHEAPINFO FAR * lpSysHeap)
{
#ifdef _M_ALPHA
        SYM_ASSERT(FALSE);
        return FALSE;
#else   
    static FARPROC      lpfnSystemHeapInfo;
    extern HINSTANCE    hInstThunkDll;

    if (lpfnSystemHeapInfo == NULL)
        {
            if ((hInstThunkDll == NULL) && (SystemGetWindowsType() == SYM_SYSTEM_WIN_95))
                    {
                SYM_OUTPUT_DEBUG("Loading " QMODULE_SYMKRNLTHUNK ".DLL\n");
                hInstThunkDll = LoadLibrary( QMODULE_SYMKRNLTHUNK ".DLL" );
                }
        if (hInstThunkDll > (HINSTANCE)HINSTANCE_ERROR)
            {
            lpfnSystemHeapInfo = GetProcAddress(hInstThunkDll,"SystemHeapInfo32" );
            if (lpfnSystemHeapInfo == NULL)
                return FALSE;
            }
        else
            return FALSE;
        }

    return( lpfnSystemHeapInfo(lpSysHeap));
#endif    
}

WORD __declspec(dllexport) WINAPI SymGetFreeSystemResources(WORD fuSysResource)
{
#ifdef _M_ALPHA
        SYM_ASSERT(FALSE);
        return 0;
#else   
    static FARPROC      lpfnGetFreeSystemResources;
    extern HINSTANCE    hInstThunkDll;

    if (lpfnGetFreeSystemResources == NULL)
        {
            if ((hInstThunkDll == NULL) && (SystemGetWindowsType() == SYM_SYSTEM_WIN_95))
                    {
                SYM_OUTPUT_DEBUG("Loading " QMODULE_SYMKRNLTHUNK ".DLL\n");
                hInstThunkDll = LoadLibrary( QMODULE_SYMKRNLTHUNK ".DLL" );
                }
        if (hInstThunkDll > (HINSTANCE)HINSTANCE_ERROR)
            {
            lpfnGetFreeSystemResources = GetProcAddress(hInstThunkDll,"GetFreeSystemResources32" );
            if (lpfnGetFreeSystemResources == NULL)
                return 0;
            }
        else
            return 0;
        }

    return( lpfnGetFreeSystemResources(fuSysResource));
#endif    
}
#endif

////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////
BOOL SYM_EXPORT WINAPI SystemWin95ShellPresent(VOID)
{
#if 0 //**Left for reference. This was implementation in Qak8, 9, & A.
    static  BOOL        bFirstTime = TRUE;
    static  BOOL        bHasWin95Shell;
    auto    HINSTANCE   hLibraryShell32;


    if (bFirstTime)
        {
        hLibraryShell32 = LoadLibrary("SHELL32.DLL");
        if (hLibraryShell32 >= (HINSTANCE) HINSTANCE_ERROR)
            {
            if (GetProcAddress (hLibraryShell32, "SHGetDesktopFolder"))
                bHasWin95Shell = TRUE;   
            FreeLibrary(hLibraryShell32);
            }

        bFirstTime = FALSE;
        }
    return(bHasWin95Shell);
#endif
        return( FindWindow( "Shell_TrayWnd", NULL ) != NULL );
}

#endif     //  #ifdef SYM_WIN



////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////
UINT SYM_EXPORT WINAPI SystemGetWindowsType(VOID)
{
#if defined(SYM_WIN32)

    DWORD   dwVersion = GetVersion();


    if (dwVersion < 0x80000000)
        {
        return(SYM_SYSTEM_WIN_NT);
        }
    else if (LOBYTE(LOWORD(dwVersion)) < 4)
        {
        return(SYM_SYSTEM_WIN_32S);
        }
    else
        return(SYM_SYSTEM_WIN_95);

#elif defined(SYM_WIN)

#define WF_WINNT    (0x4000)            // New flag for GetWinFlags()

    DWORD   dwVersion;
    WORD    wWinVer;

    dwVersion = GetVersion();
    wWinVer = LOWORD(dwVersion);
    wWinVer = wWinVer << 8 | wWinVer >> 8;  // Swap order
    if (wWinVer == 0x035F)
        {
        return(SYM_SYSTEM_WIN_95);
        }
    else if (GetWinFlags() & WF_WINNT)
        {
        return(SYM_SYSTEM_WIN_NT);
        }
    else
        return(SYM_SYSTEM_WIN_31);

#elif defined(SYM_DOS) || defined(SYM_X86)

    WORD    wWindowsType;

    wWindowsType = 0;
    _asm
    {
        push    si
        push    di
        push    es
        push    ds
        push    bp

        mov     ax, 01600h              ; test for enhanced mode
        int     2Fh

        and     ax, 07Fh
        jz      WIN_REAL

        cmp     al, 4                   ; distinguish between NT and Chicago
        jge     WIN_NT

        mov     [wWindowsType], SYM_SYSTEM_WIN_31
        jmp     WIN_DONE

    WIN_REAL:
        mov     ax, 04680h              ; test for real or standard mode
        int     2Fh

        cmp     ax, 0
        jne     WIN_NT

        mov     [wWindowsType], SYM_SYSTEM_WIN_31
        jmp     WIN_DONE

    WIN_NT:
        mov     ax, 03306h
        xor     bx, bx
        int     21h

        cmp     bx, 03205h              ; check for Windows NT
        jne     WIN_CHICAGO

        mov     [wWindowsType], SYM_SYSTEM_WIN_NT
        jmp     WIN_DONE

    WIN_CHICAGO:                        ; check for Chicago
        cmp     bl, 07h
        jne     WIN_DONE

        mov     [wWindowsType], SYM_SYSTEM_WIN_95
        jmp     WIN_DONE

    WIN_DONE:

        pop     bp
        pop     ds
        pop     es
        pop     di
        pop     si
    }

    return(wWindowsType);
    
#endif
}
