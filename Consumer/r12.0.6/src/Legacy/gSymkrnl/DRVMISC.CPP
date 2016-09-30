// Copyright 1996 - 1997 Symantec, Peter Norton Product Group
//***************************************************************************
//
// $Header:   S:/SYMKRNL/VCS/drvmisc.cpv   1.46   21 Jun 1998 22:48:10   RStanev  $
//
// Description:
//      This file contains miscellaneous functions for the driver platforms
//      SYM_VXD and SYM_NTK.
//
// Contains:
//      CoreGlobalInit()
//      CoreGlobalDeInit()
//      CoreLocalInit()
//      CoreLocalDeInit()
//
//      GetLastError()
//      SetLastError()
//
//      SYMNTK_KernelModeXfer_CallFunction() (SYM_NTK only)
//      SYMNTK_KernelModeXfer_CheckDIOCTL()  (SYM_NTK only)
//
//      GetSystemTime()
//      Sleep()
//      SleepTimeOutCallback()               (SYM_VXD only, local function)
//
//      CharToOemA()
//      CharToOemW()
//      OemToCharA()
//      OemToCharW()
//      OemToCharBuffW()                     (SYM_NTK only)
//      MultiByteToWideChar()
//      WideCharToMultiByte()
//      VxDGenericStringConvert()            (SYM_VXD only)
//
//      __VxDAssertFailed()                  (SYM_VXD only)
//      NTKAssertMemoryIsLocked()            (SYM_NTK only)
//
//      Zv...() wraps for Zw...() functions  (SYM_NTK only)
//
//      SymVXDCreateResource()               (SYM_VXD only)
//      SymVXDAcquireResourceShared()        (SYM_VXD only)
//      SymVXDAcquireResourceExclusive()     (SYM_VXD only)
//      SymVXDReleaseResource()              (SYM_VXD only)
//      SymVXDDestroyResource()              (SYM_VXD only)
//
// See also:
//      stack.cpp, thdata.cpp
//
//***************************************************************************
// $Log:   S:/SYMKRNL/VCS/drvmisc.cpv  $
//
//    Rev 1.46   21 Jun 1998 22:48:10   RStanev
// Fixed some MBCS<->Unicode conversion functions.
//
//    Rev 1.45   11 Jul 1997 14:35:02   RStanev
// Removed references to KeGetPreviousMode() and ExGetPreviousMode().
//
//    Rev 1.44   06 Jul 1997 03:39:12   RStanev
// Added new Zw...() wraps which support the new stack swapping architecture.
//
//    Rev 1.43   21 May 1997 08:35:34   RStanev
// Implemented GetSystemTime() for SYM_VXD.
//
//    Rev 1.42   14 May 1997 11:19:06   RStanev
// More SYM_ASSERT ( FALSE ) clean up.
//
//    Rev 1.41   14 May 1997 09:41:08   RStanev
// Removed SYM_ASSERT ( FALSE ) from CharToOemA() and OemToCharA() until
// we get closer to cleaning up.
//
//    Rev 1.40   02 May 1997 22:36:26   RStanev
// CoreGlobalDeInit() now frees up its per-thread data area.
//
//    Rev 1.39   25 Apr 1997 11:21:24   LCOHEN
// Corrected SymEvent interface version number for the detection of HwIsNec service.
//
//    Rev 1.38   17 Mar 1997 16:28:28   RStanev
// NEC detection for SYM_VXD now does not tie the full Core functionality
// to newer versions of SYMEvent.
//
//    Rev 1.37   17 Mar 1997 12:02:12   RStanev
// Changed __VxDAssertFailed() to use int 1 as a breakpoint instead of int 3.
// This will allow us to use user-mode debugger with WinIce.
//
//    Rev 1.36   17 Mar 1997 01:59:16   RStanev
// Added support routines for SYM_VXD's version of SymCreateResource(),
// SymAcquireResourceShared(), SymAcquireResourceExclusive(),
// SymReleaseResource(), and SymDestroyResource().  Code compiles, but has
// to be linked into a driver and tested later.
//
//    Rev 1.35   25 Feb 1997 22:43:28   RSTANEV
// Added bVMMIs3x and bHWIsNEC for SYM_VXD.
//
//    Rev 1.34   25 Feb 1997 01:36:32   RSTANEV
// Moved stack swapping and per-thread data management in separate files.
//
//    Rev 1.33   28 Jan 1997 17:12:00   DALLEE
// In VxDGenericStringConvert: Changed INT_MAX for unknown buffer lengths
// to 0x8000 (new local define MAX_VXD_STRING).
//
// Removed SYM_ASSERT() warnings on SYM_VXD implementations of
// WideCharToMultiByte() and MultiByteToWideChar(). Their implementations are
// equivalent to the pre-CORE J VXD calls and appear to work correctly.
//
//    Rev 1.32   06 Jan 1997 12:53:04   RSTANEV
// More comments.
//
//    Rev 1.31   31 Oct 1996 21:13:50   RSTANEV
// Getting closer to merging the SYM_VXD and SYM_NTK architectures.
//
//    Rev 1.30   21 Aug 1996 15:15:40   RSTANEV
// Turned off stack frame optimization for the Zw...() wraps, since they
// call the SwapStack...() functions.
//
//    Rev 1.29   20 Aug 1996 18:46:20   RSTANEV
// Removed a few unnecessary SYM_ASSERT() statements.
//
//    Rev 1.28   06 Aug 1996 01:08:48   RSTANEV
// Removed SYMEvent dependencies.
//
//    Rev 1.27   29 Jul 1996 03:16:16   RSTANEV
// Added per-thread data management functions.
//
//    Rev 1.26   26 Jul 1996 20:18:34   RSTANEV
// Reworked to match the SYMEvent requirements for managing per-thread
// data areas.
//
//    Rev 1.25   23 Jul 1996 13:59:10   RSTANEV
// Avoided stack swapping in the ZvCreateFile() and ZvOpenFile() APIs
// when an oplock on the file was detected.  Also added a comment and
// an assertion in ZvDeleteFile().
//
//    Rev 1.24   23 Jul 1996 11:19:18   RSTANEV
// The wraps on top of ZwCreateFile() and ZwOpenFile() now force the use
// of the FILE_COMPLETE_IF_OPLOCKED flag.  This, together with proper
// correction of the result of the Zw...() file functions, guarantees
// that the wraps will never open oplocked files.
//
//    Rev 1.23   22 Jul 1996 18:10:20   RSTANEV
// Using the NT_SUCCESS() macro.
//
//    Rev 1.22   22 Jul 1996 16:11:38   RSTANEV
// Initializing pvNTSystemProcess to NULL.
//
//    Rev 1.21   22 Jul 1996 16:08:58   RSTANEV
// Added pvNTSystemProcess.
//
//    Rev 1.20   25 Jun 1996 16:29:14   RSTANEV
// Oops... forgot to make CoreSwapStackInTemp() to understand the 0 results
// returned by CoreSwapStackOutTemp().
//
//    Rev 1.19   24 Jun 1996 18:59:54   RSTANEV
// Added stack swapping APIs, but have not been tested.  Also added wraps
// on top of the Zw...() functions, so that we can swap back to the
// internally allocated stack before calling NTOS.
//
//    Rev 1.18   12 Jun 1996 21:58:26   RSTANEV
// Fixed some of the debug code in NTKAssertMemoryIsLocked() to properly
// check the range (spcifically, the last byte) of the PAGELKD section.
//
//    Rev 1.17   23 May 1996 09:11:36   RSTANEV
// Core will service DIOCTL_SYM_INTERNAL_NT_KERNEL_MODE_XFER only if the
// requestor was running in KernelMode.  Otherwise, any user-mode application
// can execute kernel mode code without the security checks.
//
//    Rev 1.16   22 May 1996 22:13:20   RSTANEV
// Added SYMNTK_KernelModeXfer_CallFunction() and
// SYMNTK_KernelModeXfer_CheckDIOCTL(), but haven't run them yet.
//
//    Rev 1.15   17 May 1996 16:42:44   RSTANEV
// Replaced DbgPrint() calls with KdPrint().
//
//    Rev 1.14   17 May 1996 14:17:24   RSTANEV
// Added stubs for GetLastError() and SetLastError().  These functions
// do not currently work properly because we don't have per-thread data.
//
//    Rev 1.13   17 May 1996 09:16:38   jmillar
// delete a struct definition that is now provided by platntk.h
//
//    Rev 1.12   05 May 1996 13:34:42   RSTANEV
// Added NTKAssertMemoryIsLocked().
//
//    Rev 1.11   10 Apr 1996 17:48:28   RSTANEV
// Fixed all the stupid bugs introduced in the previous revision.
//
//    Rev 1.10   09 Apr 1996 14:47:22   RSTANEV
// Added segment init/deinit to DrvGlobalInit() and DrvGlobalDeInit().
//
//    Rev 1.9   09 Apr 1996 08:09:06   RSTANEV
// FastGlobalLockMutex now gets defined in page locked segment.
//
//    Rev 1.8   05 Apr 1996 11:47:02   RSTANEV
// Fixed the delay in Sleep() for SYM_NTK platform.
//
//    Rev 1.7   04 Apr 1996 20:07:34   RSTANEV
// Fixed a whole bunch of problems with the character conversion functions
// in SYM_NTK platform.
//
//    Rev 1.6   31 Mar 1996 18:51:00   RSTANEV
// Implemented Sleep() for SYM_NTK.
//
//    Rev 1.5   31 Mar 1996 17:46:30   RSTANEV
// Added GetSystemTime();
//
//    Rev 1.4   27 Mar 1996 15:24:08   RSTANEV
// Added support for some SYMSYNC APIs, and implemented the DrvGlobalInit()
// and DrvGlobalDeInit() functions.
//
//    Rev 1.3   08 Feb 1996 14:25:04   RSTANEV
// Added OemToCharBuffW() for SYM_NTK platform.
//
//    Rev 1.2   02 Feb 1996 14:09:46   RSTANEV
// Changed VxDGenericStringConvert() to match its prototype.
//
//    Rev 1.1   02 Feb 1996 12:32:10   RSTANEV
// Now, that's a major revision.
//
//    Rev 1.0   30 Jan 1996 18:49:58   RSTANEV
// Initial revision.
//
//***************************************************************************
//
// This file was originally started in Quake, as VXDSTAT.CPP, and it was
// VxD specific.  In Core, it was renamed to DRVMISC.CPP and support for
// both VxD and NT Kernel mode platforms was added.
//
//***************************************************************************

#include "platform.h"
#include "symevent.h"
#include "symsync.h"
#include "drvcomm.h"
#include "xapi.h"
#include "dbcs.h"

//---------------------------------------------------------------------------
//
// Define the FastGlobalLockMutex synchronization variable.  It is used
// by the SymAcquireFastGlobalLock() and SymReleaseFastGlobalLock() APIs.
//
//---------------------------------------------------------------------------
#if defined(SYM_NTK)

#define DRIVER_LOCKED_DATA_SEGMENT
#include "drvseg.h"

FAST_MUTEX FastGlobalLockMutex = {0};

#define DRIVER_DEFAULT_DATA_SEGMENT
#include "drvseg.h"

#elif defined(SYM_VXD)

PVMMMUTEX FastGlobalLockMutex = {0};

#endif

//---------------------------------------------------------------------------
//
// The following variables/functions are used to access addresses in specific
// data and code segments of NT drivers.  These addresses are used by
// CoreGlobalInit() when locking/unlocking segment images.
//
// Notes:
//      There is no need to manually unlock the "PAGE" segment (pageable
//      code) - NT will automatically do this for us.  Remember - this
//      whole mess is necessary only because the C compiler does not like
//      having both code and data segments with the same names.
//      The INIT data and PAGED data should be in the same segment.  That's
//      why we don't define pvID.
//
//---------------------------------------------------------------------------
#if defined(SYM_NTK)

#define DRIVER_LOCKED_CODE_SEGMENT
#define DRIVER_LOCKED_DATA_SEGMENT
#include "drvseg.h"

static PVOID pvLD = NULL;
static PVOID pvLC = NULL;

static void FunctionInLOCKED_CODE ( void ) { ; }

#define DRIVER_DEFAULT_CODE_SEGMENT
#define DRIVER_DEFAULT_DATA_SEGMENT
#include "drvseg.h"

#endif

//---------------------------------------------------------------------------
//
// pvNTSystemProcess is used by CoreIsThisNTSystemProcess().
//
//---------------------------------------------------------------------------
#if defined(SYM_NTK)

PEPROCESS pvNTSystemProcess = NULL;

#endif

//---------------------------------------------------------------------------
//
// bVMMIs3x is SYM_VXD specific and is TRUE if the VxD using Core is running
// on Windows 3.x VMM, FALSE if Windows 95 and later.
//
// bHWIsNEC is also SYM_VXD specific and is TRUE if the VxD using Core is
// running on an NEC machine.
//
//---------------------------------------------------------------------------
#if defined(SYM_VXD)

BOOL bVMMIs3x = FALSE;
BOOL bHWIsNEC = FALSE;

#endif

//---------------------------------------------------------------------------
//
// Define a handle for accessing Core's per-thread data.
//
//---------------------------------------------------------------------------
HOFDATA hCorePerThreadData = NULL;

//---------------------------------------------------------------------------
//
// Define default size of conversion buffers (measured in characters!)
//
//---------------------------------------------------------------------------
#if defined(SYM_VXD)

#define BUFFER_SIZE_CharToOemA  0x10
#define BUFFER_SIZE_OemToCharA  0x10

#else

#define BUFFER_SIZE_CharToOemA  SYM_MAX_PATH
#define BUFFER_SIZE_OemToCharA  SYM_MAX_PATH

#endif

//---------------------------------------------------------------------------
//
// Define local function prototypes.
//
//---------------------------------------------------------------------------
#if defined(SYM_VXD)
static void __cdecl SleepTimeOutCallback ( void );
#endif

//---------------------------------------------------------------------------
//
// BOOL WINAPI CoreGlobalInit ( VOID );
//
// This function initializes Core for use by an NT or Win95 driver.
//
// SYM_NTK: Call this function from DriverEntry() before doing any Core
//          calls.  The implementation for this platform relies on
//          NTOSKRNL calling DriverEntry() under the context of the
//          system process!
//
// SYM_VXD: Call this function from driver's Sys_Critical_Init or
//          SYS_DYNAMIC_DEVICE_INIT, before dispatching the message to
//          other code.
// Entry:
//      -
//
// Exit:
//      TRUE if successful, FALSE if error.
//      Fail driver loading if the function returns FALSE.  Calling any
//      Core APIs before this function completes successfully (unless
//      otherwise stated in API's documentation) may cause unpredictable
//      behavior.
//
// Note:
//      SYMEvent has to be present and locked if Core uses SYMEvent to
//      manage the per-thread data APIs.
//      SYM_VXD implementation will detect NEC machines only if the SYMEvent
//      present on the system supports this functionality.  The client must
//      ensure that the proper version of SYMEvent is installed if NEC
//      detection is required.
//
//---------------------------------------------------------------------------
BOOL WINAPI CoreGlobalInit ( VOID )
    {
#if defined(SYM_NTK)
                                        // Lock the PAGELK and PAGELKD
                                        // segments and get their handles.
                                        // By default all PAGE* segments
                                        // are pageable!

    if ( ! ( pvLD = MmLockPagableDataSection ( &pvLD ) ) )
        {
        SYM_ASSERT ( pvLD );
        return ( FALSE );
        }

    if ( ! ( pvLC = MmLockPagableCodeSection ( &FunctionInLOCKED_CODE ) ) )
        {
        SYM_ASSERT ( pvLC );
        MmUnlockPagableImageSection ( pvLD );
        return ( FALSE );
        }

                                        // Initialize the FastGlobalLock APIs

    ExInitializeFastMutex ( &FastGlobalLockMutex );

                                        // Save the current (system) process
                                        // handle for later use.

    pvNTSystemProcess = PsGetCurrentProcess();

                                        // Initialize the per-thread data
                                        // APIs.

    if ( !CoreInitThreadData() )
        {
        SYM_ASSERT ( FALSE );
        MmUnlockPagableImageSection ( pvLC );
        MmUnlockPagableImageSection ( pvLD );
        return ( FALSE );
        }

                                        // Allocate per-thread data area.

    if ( ! ( hCorePerThreadData = CoreAllocThreadData (
                                      sizeof ( COREPERTHREADDATA ) )
           )
       )
        {
        SYM_ASSERT ( hCorePerThreadData );
        if ( !CoreDeInitThreadData() )
            {
            SYM_ASSERT ( FALSE );
            }
        MmUnlockPagableImageSection ( pvLC );
        MmUnlockPagableImageSection ( pvLD );
        return ( FALSE );
        }

#elif defined(SYM_VXD)

                                        // Initialize bVMMIs3x and bHWIsNEC.
                                        // Note, that version 0x20B of
                                        // SYMEvent is the first to support
                                        // SYMEvent_HWIsNEC().

    bVMMIs3x = Get_VMM_Version() < 0x400;

    if ( SYMEvent_Get_Version() >= 0x20B )
        {
        bHWIsNEC = SYMEvent_HWIsNEC();
        }

                                        // Initialize the FastGlobalLock APIs

    if ( ! ( FastGlobalLockMutex = _CreateMutex ( 0, 0 ) ) )
        {
        SYM_ASSERT ( FastGlobalLockMutex );
        return ( FALSE );
        }

                                        // Initialize the per-thread data
                                        // APIs.

    if ( !CoreInitThreadData() )
        {
        SYM_ASSERT ( FALSE );
        if ( !_DestroyMutex ( FastGlobalLockMutex ) )
            {
            SYM_ASSERT ( FALSE );
            }
        return ( FALSE );
        }

                                        // Allocate per-thread data area.

    if ( ! ( hCorePerThreadData = CoreAllocThreadData (
                                      sizeof ( COREPERTHREADDATA ) )
           )
       )
        {
        SYM_ASSERT ( hCorePerThreadData );
        if ( !CoreDeInitThreadData() )
            {
            SYM_ASSERT ( FALSE );
            }
        if ( !_DestroyMutex ( FastGlobalLockMutex ) )
            {
            SYM_ASSERT ( FALSE );
            }
        return ( FALSE );
        }

#endif

    return ( TRUE );
    }

//---------------------------------------------------------------------------
//
// BOOL WINAPI CoreGlobalDeInit ( VOID );
//
// This function un-does whatever CoreGlobalInit() did.
//
// Call CoreGlobalDeInit() before unloading the driver, but only if
// CoreGlobalInit() has completed successfully.  Allow all calls to Core
// to complete before calling this function, and don't call any Core APIs,
// including CoreGlobalInit(), once this function is called.
//
// Entry:
//      -
//
// Exit:
//      CoreGlobalDeInit() returns TRUE if successful; FALSE otherwise.
//      The current implementation returns always TRUE, but the client
//      should still check the error code, and at least fail an assertion
//      should this function start returning FALSE.
//
//---------------------------------------------------------------------------
BOOL WINAPI CoreGlobalDeInit ( VOID )
    {
                                        // Free the per-thread data area
                                        // allocated for Core's usage.

    SYM_ASSERT ( hCorePerThreadData );

    if ( !CoreFreeThreadData ( hCorePerThreadData ) )
        {
                                        // ~~~TBD~~~ Save the handle in
                                        // Core's persistent data.  Note,
                                        // that this doesn't need to be
                                        // implemented as long as Core's
                                        // per-thread data management does
                                        // not use SymEvent.

        SYM_ASSERT ( FALSE );
        }

                                        // Deinit the per-thread data APIs.

    if ( !CoreDeInitThreadData() )
        {
        SYM_ASSERT ( FALSE );
        }

#if defined(SYM_NTK)

                                        // Invalidate Core's global lock

    MEMSET ( &FastGlobalLockMutex, 0, sizeof(FastGlobalLockMutex ) );

                                        // Unlock the PAGELK and PAGELKD
                                        // segments.

    SYM_ASSERT ( pvLC );
    SYM_ASSERT ( pvLD );

    MmUnlockPagableImageSection ( pvLC );
    MmUnlockPagableImageSection ( pvLD );

#elif defined(SYM_VXD)

                                        // Invalidate Core's global lock

    SYM_ASSERT ( !_GetMutexOwner ( FastGlobalLockMutex ) );

    if ( !_DestroyMutex ( FastGlobalLockMutex ) )
        {
        SYM_ASSERT ( FALSE );
        }

#endif

    return ( TRUE );
    }

//---------------------------------------------------------------------------
//
// PVOID WINAPI CoreLocalInit ( VOID );
//
// This function initializes Core for use by the current thread.  Call this
// function only after Core has been globally initialized with
// CoreGlobalInit().
//
// Entry:
//      -
//
// Exit:
//      Non-NULL if successful, NULL if error.  Upon successful completion,
//      the returned value should be saved and then passed to
//      CoreLocalDeInit() when the client is done with Core.
//
// Synchronization:
//      This function is reentrant from different threads.  It may also be
//      called recursively (multiple times) from the same thread, but is
//      non-reentrant from the same thread.  Recursive calls from the same
//      thread must be matched with the same number of calls to
//      CoreLocalDeInit() from the same thread.  Calling CoreGlobalDeInit()
//      with an unmatched number of CoreLocalInit()/CoreLocalDeInit() pairs
//      will cause problems.
//
//---------------------------------------------------------------------------
PVOID WINAPI CoreLocalInit ( VOID )
    {
    auto PVOID pvResult;

                                        // Lock the per-thread data for this
                                        // thread.

    SYM_ASSERT ( hCorePerThreadData );

    pvResult = _CoreLockThreadDataPtr ( hCorePerThreadData );

    if ( pvResult )
        {
        ((PCOREPERTHREADDATA)pvResult)->iLockCount++;
        }

    return ( pvResult );
    }

//---------------------------------------------------------------------------
//
// VOID WINAPI CoreLocalDeInit ( PVOID pvHandle );
//
// This function un-does whatever CoreLocalInit() did.  Call this function
// for every successful call to CoreLocalInit().  For client's convenience,
// this function will not do anything bad if it is also called to match
// all failed CoreLocalInit() calls (pvHandle == NULL).
//
// Entry:
//      pvHandle - the value returned by CoreLocalInit().
//
// Exit:
//      -
//
//---------------------------------------------------------------------------
VOID WINAPI CoreLocalDeInit ( PVOID pvHandle )
    {
    SYM_ASSERT ( hCorePerThreadData );

                                        // Unlock the per-thread data for
                                        // this thread.

    if ( pvHandle )
        {
        if ( ! --((PCOREPERTHREADDATA)pvHandle)->iLockCount )
            {
            MEMSET ( pvHandle, 0, sizeof(COREPERTHREADDATA) );
            }

        _CoreUnLockThreadDataPtr ( hCorePerThreadData, pvHandle );
        }
    }

//---------------------------------------------------------------------------
//
// WINBASEAPI DWORD WINAPI GetLastError (
//      VOID
//      );
//
// This function works just like the Windows API.
//
// Note, that if the current per-thread data was not locked at the time of
// setting the last error, or if the data was unlocked between set and get
// of last error, the result of this function will be undefined!
//
//---------------------------------------------------------------------------
WINBASEAPI DWORD WINAPI GetLastError (
    VOID
    )
    {
    auto DWORD              dwResult;
    auto PCOREPERTHREADDATA prPerThreadData;

                                        // Lock the per-thread data for this
                                        // thread (just in case).  This will
                                        // complete really fast if the data
                                        // was already locked, as it
                                        // should've been.

    SYM_ASSERT ( hCorePerThreadData );

    prPerThreadData = (PCOREPERTHREADDATA) _CoreLockThreadDataPtr (
                                           hCorePerThreadData );

    SYM_ASSERT ( prPerThreadData );

                                        // Get information if data was locked
                                        // or return 0 if no data.

    if ( prPerThreadData )
        {
        dwResult = prPerThreadData->dwLastError;

        _CoreUnLockThreadDataPtr ( hCorePerThreadData, prPerThreadData );
        }
    else
        {
        dwResult = 0;
        }

    return ( dwResult );
    }

//---------------------------------------------------------------------------
//
// WINBASEAPI VOID WINAPI SetLastError (
//      DWORD dwErrCode
//      );
//
// This function works just like the Windows API.
//
// Note, that the per-thread data should be locked by the client before
// calling this function, and should be kept locked as long as the client
// wants to get the proper result by calling GetLastError().  If the data
// was not locked prior to calling SetLastError(), the value will be lost.
//
//---------------------------------------------------------------------------
WINBASEAPI VOID WINAPI SetLastError (
    DWORD dwErrCode
    )
    {
    auto PCOREPERTHREADDATA prPerThreadData;

                                        // Lock the per-thread data for this
                                        // thread (just in case).  This will
                                        // complete really fast if the data
                                        // was already locked, as it
                                        // should've been.

    SYM_ASSERT ( hCorePerThreadData );

    prPerThreadData = (PCOREPERTHREADDATA) _CoreLockThreadDataPtr (
                                           hCorePerThreadData );

    SYM_ASSERT ( prPerThreadData );

                                        // Set the last error code if data
                                        // was locked.

    if ( prPerThreadData )
        {
        prPerThreadData->dwLastError = dwErrCode;

        _CoreUnLockThreadDataPtr ( hCorePerThreadData, prPerThreadData );
        }
    }

//---------------------------------------------------------------------------
//
// BOOL WINAPI SYMNTK_KernelModeXfer_CallFunction (
//      SYMNTK_KernelModeXfer_Function FunctionToCall,
//      PVOID                          pvParams
//      );
//
// This function reenters the Windows NT kernel, thus setting the previous
// mode to KernelMode.  After this operation, NT will bypass the security
// checks on all Nt...() calls.
//
// Before using this service, the client must set pszCoreKernelModeXferDevice
// to point to a string containing the fully qualified name of the device
// which will service the DIOCTL request issued by this function.  The macro
// SYMNTK_KernelModeXfer_SetDevice() can be used to do this.  Also, the
// device function servicing IRP_MJ_DEVICE_CONTROL should call
// SYMNTK_KernelModeXfer_CheckDIOCTL() to allow Core to complete the request.
//
// Note: This is a really, I mean REALLY slow function and should be avoided
//       unless its usage is absolutely necessary.  NTOSKRNL usually exports
//       most of the Nt...() APIs as Zw...() APIs.  The Zw guys do the same,
//       except that they do it the right (and unfortunately undocumented)
//       way.  Also, if executing code in last mode == KernelMode can be
//       done in context other than the current context, then it is better
//       for the client to schedule it as a work item for the system process.
//
// Entry:
//      FunctionToCall - pointer to a function to call once the last mode
//                       is set to KernelMode.
//      pvParams       - value to be passed to FunctionToCall.
//
// Exit:
//      TRUE  if last mode was successfully set to KernelMode and
//            FunctionToCall was executed.
//      FALSE if error - FunctionToCall was never executed.
//
//---------------------------------------------------------------------------
#if defined(SYM_NTK)

PWSTR pszCoreKernelModeXferDevice = NULL;

typedef struct
    {
    SYMNTK_KernelModeXfer_Function FunctionToCall;
    PVOID                          pvParams;
    } KERNELMODEXFERDIOCTL;

BOOL WINAPI SYMNTK_KernelModeXfer_CallFunction (
    SYMNTK_KernelModeXfer_Function FunctionToCall,
    PVOID                          pvParams
    )
    {
    auto HANDLE               hDevice;
    auto UNICODE_STRING       TargetDevice;
    auto OBJECT_ATTRIBUTES    Object;
    auto IO_STATUS_BLOCK      IoStatus;
    auto KERNELMODEXFERDIOCTL Packet;

                                        // If we don't have device, then
                                        // forget it.

    if ( !pszCoreKernelModeXferDevice )
        {
        return ( FALSE );
        }
                                        // Last mode was UserMode, so we need
                                        // to reenter the NTOS kernel.

    RtlInitUnicodeString ( &TargetDevice, pszCoreKernelModeXferDevice );

    InitializeObjectAttributes ( &Object,
                                 &TargetDevice,
                                 OBJ_CASE_INSENSITIVE,
                                 NULL,
                                 NULL );

    if ( !NT_SUCCESS ( ZwCreateFile ( &hDevice,
                                      GENERIC_READ,
                                      &Object,
                                      &IoStatus,
                                      NULL,
                                      0,
                                      FILE_SHARE_READ | FILE_SHARE_WRITE,
                                      FILE_OPEN,
                                      FILE_NON_DIRECTORY_FILE | FILE_RANDOM_ACCESS,
                                      NULL,
                                      0 ) ) )
        {
        return ( FALSE );
        }

    Packet.FunctionToCall = FunctionToCall;
    Packet.pvParams       = pvParams;

    if ( !NT_SUCCESS ( ZwDeviceIoControlFile ( hDevice,
                                               NULL,
                                               NULL,
                                               NULL,
                                               &IoStatus,
                                               DIOCTL_SYM_INTERNAL_NT_KERNEL_MODE_XFER,
                                               &Packet,
                                               sizeof(Packet),
                                               NULL,
                                               0 ) ) )
        {
        ZwClose ( hDevice );
        return ( FALSE );
        }

    ZwClose ( hDevice );

    return ( TRUE );
    }

//---------------------------------------------------------------------------
//
// BOOL WINAPI SYMNTK_KernelModeXfer_CheckDIOCTL (
//      IN PDEVICE_OBJECT DeviceObject,
//      IN PIRP Irp
//      );
//
// This function should be called by the IRP_MJ_DEVICE_CONTROL service
// routine.  It examines the Irp, and services it if it is issued by
// SYMNTK_KernelModeXfer_CallFunction().  In this case, the return value
// will be TRUE.  If the Irp is not for KernelMode transfer, then the return
// value is FALSE.  If the function returns TRUE, the Irp status is set to
// complete, so the client has to only return STATUS_SUCCESS from the
// IRP_MJ_DEVICE_CONTROL service routine.
//
//---------------------------------------------------------------------------
BOOL WINAPI SYMNTK_KernelModeXfer_CheckDIOCTL (
    IN PDEVICE_OBJECT DeviceObject,
    IN PIRP Irp
    )
    {
    auto PIO_STACK_LOCATION   pIrpStack;
    auto KERNELMODEXFERDIOCTL *pPacket;

                                        // We don't really want to give
                                        // user mode clients full control
                                        // of the system...

    if ( Irp->RequestorMode != KernelMode )
        {
        return ( FALSE );
        }

                                        // Get the IRP stack location, so
                                        // that we can access the DIOC call
                                        // information.

    pIrpStack = IoGetCurrentIrpStackLocation ( Irp );

                                        // Check the DIOCTL code.

    if ( pIrpStack->Parameters.DeviceIoControl.IoControlCode !=
         DIOCTL_SYM_INTERNAL_NT_KERNEL_MODE_XFER )
        {
        return ( FALSE );
        }

                                        // It is our DIOCTL - make sure the
                                        // in/out buffers are set up the
                                        // same way as the function above
                                        // sets them.

    if ( !pIrpStack->Parameters.DeviceIoControl.Type3InputBuffer ||
         pIrpStack->Parameters.DeviceIoControl.InputBufferLength != sizeof(KERNELMODEXFERDIOCTL) ||
         Irp->UserBuffer ||
         pIrpStack->Parameters.DeviceIoControl.OutputBufferLength )
        {
        return ( FALSE );
        }

                                        // They are, call the routine.

    pPacket = (KERNELMODEXFERDIOCTL*)pIrpStack->Parameters.DeviceIoControl.Type3InputBuffer;

    pPacket->FunctionToCall(pPacket->pvParams);

                                        // Complete the Irp.

    Irp->IoStatus.Status = STATUS_SUCCESS;
    Irp->IoStatus.Information = 0;

    IoCompleteRequest ( Irp, IO_NO_INCREMENT );

    return ( TRUE );
    }

#endif

//---------------------------------------------------------------------------
//
// VOID WINAPI GetSystemTime ( LPSYSTEMTIME lpSystemTime );
//
// This function works just like the one documented in the Windows API.
//
// Note:
//      SYM_VXD:
//
//---------------------------------------------------------------------------
VOID WINAPI GetSystemTime (
    LPSYSTEMTIME lpSystemTime
    )
    {
#if defined(SYM_NTK)

    auto LARGE_INTEGER CurrentTime;
    auto TIME_FIELDS   TimeFields;

    KeQuerySystemTime ( &CurrentTime );

    RtlTimeToTimeFields ( &CurrentTime, &TimeFields );

    lpSystemTime->wYear         = TimeFields.Year;
    lpSystemTime->wMonth        = TimeFields.Month;
    lpSystemTime->wDayOfWeek    = TimeFields.Weekday;
    lpSystemTime->wDay          = TimeFields.Day;
    lpSystemTime->wHour         = TimeFields.Hour;
    lpSystemTime->wMinute       = TimeFields.Minute;
    lpSystemTime->wSecond       = TimeFields.Second;
    lpSystemTime->wMilliseconds = TimeFields.Milliseconds;

#elif defined(SYM_VXD)

    auto UINT uYear, uMonth, uDay, uDayC, uWeekDay, uHour, uMin, uSec, uHund;

    do
        {
        DOSGetDate ( &uMonth, &uDay, &uYear );

        DOSGetTime ( &uHour, &uMin, &uSec, &uHund );

        uWeekDay = DOSGetDate ( &uMonth, &uDayC, &uYear );

        } while ( uDay != uDayC );

    lpSystemTime->wYear         = uYear;
    lpSystemTime->wMonth        = uMonth;
    lpSystemTime->wDayOfWeek    = uWeekDay;
    lpSystemTime->wDay          = uDay;
    lpSystemTime->wHour         = uHour;
    lpSystemTime->wMinute       = uMin;
    lpSystemTime->wSecond       = uSec;
    lpSystemTime->wMilliseconds = uHund;

#else

    SYM_ASSERT ( FALSE );

    memset ( lpSystemTime, 0, sizeof(*lpSystemTime) );

#endif
    }

//---------------------------------------------------------------------------
//
// VOID WINAPI Sleep ( DWORD dwTimeOut );
//
// This function is the same as Windows' Sleep() function.
//
// Entry:
//      Refer to Windows docs
//
// Exit:
//      Refer to Windows docs
//
// Note (since I keep forgetting):
//      1s  = 1000 ms
//      1ms = 1000 æs
//      1æs = 1000 ns
//      NT kernel mode measures time in 100 nanosecond intervals.
//
//---------------------------------------------------------------------------
VOID WINAPI Sleep ( DWORD dwTimeOut )
    {
#if defined(SYM_NTK)

    auto LARGE_INTEGER TimeOut;

    TimeOut.QuadPart = - (__int64)dwTimeOut * 1000 * 1000 / 100;

    KeDelayExecutionThread ( KernelMode, FALSE, &TimeOut );

#elif defined(SYM_VXD)

    auto UINT  hSemaphore;
    auto DWORD dwEndTime;

                                        // If dwTimeOut is 0, return

    if ( dwTimeOut == 0 )
        {
        return;
        }
                                        // Create semaphore for us to wait on

    hSemaphore = Create_Semaphore ( 0 );

    if ( hSemaphore == NULL )
        {
        return;
        }

                                        // Calculate when we should time out.

    dwEndTime = Get_System_Time() + dwTimeOut;

                                        // Wait until until timeout expires.

    while ( Get_System_Time() < dwEndTime )
        {
        Set_Global_Time_Out ( &SleepTimeOutCallback, 100, hSemaphore );

        Wait_Semaphore ( hSemaphore, BLOCK_THREAD_IDLE );
        }

                                        // Destroy the semaphore

    Destroy_Semaphore ( hSemaphore );

#else

    SYM_ASSERT ( FALSE );

#endif
    }

//---------------------------------------------------------------------------
//
// static void __cdecl SleepTimeOutCallback ( void );
//
// This is a standard VMM callback.  It is used by Sleep() to
// signal the semaphore, on which Sleep() is waiting.
//
// Entry:
//      edx - handle to a semaphore to signal
//
// Exit:
//      -
//
//---------------------------------------------------------------------------
#if defined(SYM_VXD)

static void __cdecl SleepTimeOutCallback ( void )
    {
    auto UINT hSemaphore;

    __asm   mov     hSemaphore, edx
    Signal_Semaphore ( hSemaphore );
    }

#endif


//---------------------------------------------------------------------------
//
// BOOL WINAPI CharToOemA ( PCSTR pszSource, PSTR pszDestination );
//
// This function translates the ANSI string at pszSource into its OEM
// equivalent.  The result is stored at pszDestination.
//
// Returns TRUE if successful, FALSE if error.  Note that the Win32 specs
// document this function as always returning TRUE.
//
// If CharToOemA() returns FALSE, then the string at pszDestination is
// a partially converted pszSource.
//
// Notes:
//      pszSource and pszDestination may point to the same string.
//
//      SYM_VXD: This function splits the source string into several strings
//      of BUFFER_SIZE_CharToOemA bytes max, converts each one of them to
//      Unicode into a local buffer, then converts the Unicode string
//      to OEM and adds it to pszDestination.
//
//      SYM_NTK: If possible, avoid calling this function.  Use
//      WideCharToMultiByte() if the Unicode version of pszSource is
//      available.
//
//      SYM_NTK: This function allocates memory for storage of temporary
//      Unicode string if the local buffer of size BUFFER_SIZE_CharToOemA
//      characters is not big enough to handle a temporary Unicode string.
//
//---------------------------------------------------------------------------
BOOL WINAPI CharToOemA ( PCSTR pszSource, PSTR pszDestination )
    {
                                        // ~~~TBD~~~ Remove this function
                                        // after switching all clients to
                                        // alternative methods of string
                                        // conversion.
//    SYM_ASSERT ( FALSE );

    SYM_ASSERT ( pszSource );
    SYM_VERIFY_STRING ( pszSource );
    SYM_ASSERT ( pszDestination );

#if defined(SYM_NTK)

    auto union {
         ANSI_STRING    AnsiString;
         OEM_STRING     OemString;
         };
    auto UNICODE_STRING UnicodeString;
    auto BOOLEAN        NTbAllocateBuffer;
    auto WCHAR          szBuffer[BUFFER_SIZE_CharToOemA];

    RtlInitAnsiString ( &AnsiString, pszSource );

    if ( RtlAnsiStringToUnicodeSize ( &AnsiString ) <= sizeof(szBuffer) )
        {
        NTbAllocateBuffer = FALSE;
        UnicodeString.MaximumLength = sizeof(szBuffer);
        UnicodeString.Buffer = szBuffer;
        }
    else
        {
        NTbAllocateBuffer = TRUE;
        }

    if ( NT_SUCCESS ( RtlAnsiStringToUnicodeString ( &UnicodeString,
                                                     &AnsiString,
                                                     NTbAllocateBuffer ) ) )
        {
        OemString.MaximumLength = USHORT(-1);
        OemString.Buffer = pszDestination;

        SYM_VERIFY_BUFFER ( pszDestination,
                            RtlUnicodeStringToOemSize ( &UnicodeString ) );

        if ( NT_SUCCESS ( RtlUnicodeStringToOemString ( &OemString,
                                                        &UnicodeString,
                                                        FALSE ) ) )
            {
            SYM_ASSERT ( OemString.Length );
            pszDestination[OemString.Length - 1] = '\0';
            if ( NTbAllocateBuffer ) RtlFreeUnicodeString ( &UnicodeString );
            return ( TRUE );
            }

        if ( NTbAllocateBuffer ) RtlFreeUnicodeString ( &UnicodeString );
        }

    *pszDestination = '\0';
    SYM_ASSERT ( FALSE );
    return ( FALSE );

#elif defined(SYM_VXD)

    auto size_t         uCurrentSize,   // Bytes in current ANSI buffer
                        uConvertedSize, // Bytes in converted OEM buffer
                        uCurrentChars,  // Chars in current ANSI buffer
                        uBytesLeft,     // Bytes left to convert
                        uCharSize;      // Size of char to add to buffer

    auto unsigned short szUnicodeBuffer[BUFFER_SIZE_CharToOemA];

    uBytesLeft = STRLEN(pszSource);

    while ( uBytesLeft != 0 )
        {
                                        // Get largest string to fit in
                                        // conversion buffer

        uCurrentSize  = 0;              // Bytes in string
        uCurrentChars = 0;              // Chars in string

        while ( uCurrentSize < uBytesLeft )
            {
            uCharSize = DBCSIsLeadByte ( pszSource[uCurrentSize] ) ? 2 : 1;

            if ( uCurrentSize + uCharSize <= BUFFER_SIZE_CharToOemA )
                {
                uCurrentSize += uCharSize;
                uCurrentChars++;
                }
            else
                {
                break;
                }
            }

        if ( uCurrentSize > uBytesLeft )
            {
            uCurrentSize = uBytesLeft;
            SYM_ASSERT ( FALSE );       // Ill-formed DBCS string
            }

        if ( BCSToUni ( szUnicodeBuffer,
                        (unsigned char*)pszSource,
                        uCurrentSize,
                        BCS_WANSI ) !=
             ( sizeof(*szUnicodeBuffer) * uCurrentChars ) )
            {
            SYM_ASSERT ( FALSE );
            }


        uConvertedSize = UniToBCS ( (unsigned char*)pszDestination,
                                    szUnicodeBuffer,
                                    sizeof(*szUnicodeBuffer) * uCurrentChars,
                                    uCurrentSize,
                                    BCS_OEM );

                                        // This can fire in real world cases,
                                        // but indicates invalid characters.
        SYM_ASSERT ( uConvertedSize == uCurrentSize );

        pszSource      += uCurrentSize;
        pszDestination += uConvertedSize;
        uBytesLeft     -= uCurrentSize;
        }

    *pszDestination = '\0';

    return (TRUE);

#else

#error Not implemented.

#endif
    }

//---------------------------------------------------------------------------
//
// BOOL WINAPI CharToOemW ( PCWSTR pszSource, PSTR pszDestination );
//
// This function translates the Unicode string at pszSource into its OEM
// equivalent.  The result is stored at pszDestination.
//
// Returns TRUE if successful, FALSE if error.  Note that the Win32 specs
// document this function as always returning TRUE.
//
// If CharToOemW() returns FALSE, then the string at pszDestination is
// a partially converted pszSource.
//
// Notes:
//      SYM_VXD: This function may not be DBCS enabled; please check
//               the UniToBCS() IFSMgr service documentation.
//
//      pszSource and pszDestination should not point to the same string.
//
//---------------------------------------------------------------------------
BOOL WINAPI CharToOemW ( PCWSTR pszSource, PSTR pszDestination )
    {
    SYM_ASSERT ( pszSource );
    SYM_VERIFY_STRING ( pszSource );
    SYM_ASSERT ( pszDestination );
    SYM_ASSERT ( pszSource != (PCWSTR)pszDestination );

#if defined(SYM_NTK)

    auto UNICODE_STRING UnicodeString;
    auto OEM_STRING     OemString;

    RtlInitUnicodeString ( &UnicodeString, pszSource );

    OemString.MaximumLength = USHORT(-1);
    OemString.Buffer = pszDestination;

    SYM_VERIFY_BUFFER ( pszDestination,
                        RtlUnicodeStringToOemSize ( &UnicodeString ) );

    if ( NT_SUCCESS ( RtlUnicodeStringToOemString ( &OemString,
                                                    &UnicodeString,
                                                    FALSE ) ) )
        {
        SYM_ASSERT ( OemString.Length );
        pszDestination[OemString.Length - 1] = '\0';
        return ( TRUE );
        }

    *pszDestination = '\0';
    SYM_ASSERT ( FALSE );
    return ( FALSE );

#elif defined(SYM_VXD)

    auto size_t uUniLength;

    if ( uUniLength = wcslen(pszSource) )
        {
        uUniLength++;

        uUniLength *= sizeof(unsigned short);

        UniToBCS ( (unsigned char*)pszDestination,
                   (unsigned short*)pszSource,
                   uUniLength,
                   uUniLength,
                   BCS_OEM );
        }
    else
        {
        *pszDestination = '\0';
        }

    return ( TRUE );

#else

#error Not implemented.

#endif
    }

//---------------------------------------------------------------------------
//
// BOOL WINAPI OemToCharA ( PCSTR pszSource, PSTR pszDestination );
//
// This function translates the OEM string at pszSource into its ANSI
// equivalent.  The result is stored at pszDestination.
//
// Returns TRUE if successful, FALSE if error.  Note that the Win32 specs
// document this function as always returning TRUE.
//
// If OemToCharA() returns FALSE, then the string at pszDestination is
// a partially converted pszSource.
//
// Notes:
//      pszSource and pszDestination may point to the same string.
//
//      SYM_VXD: This function splits the source string into several strings
//      of BUFFER_SIZE_OemToCharA bytes max, converts each one of them to
//      Unicode into a local buffer, then converts the Unicode string
//      to OEM and adds it to pszDestination.
//
//      SYM_NTK: If possible, avoid calling this function.  Use
//      WideCharToMultiByte() if the Unicode version of pszSource is
//      available.
//
//      SYM_NTK: This function allocates memory for storage of temporary
//      Unicode string if the local buffer of size BUFFER_SIZE_OemToCharA
//      characters is not big enough to handle a temporary Unicode string.
//
//---------------------------------------------------------------------------
BOOL WINAPI OemToCharA ( PCSTR pszSource, PSTR pszDestination )
    {
                                        // ~~~TBD~~~ Remove this function
                                        // after switching all clients to
                                        // alternative methods of string
                                        // conversion.
//    SYM_ASSERT ( FALSE );

    SYM_ASSERT ( pszSource );
    SYM_VERIFY_STRING ( pszSource );
    SYM_ASSERT ( pszDestination );

#if defined(SYM_NTK)

    auto union {
         OEM_STRING     OemString;
         ANSI_STRING    AnsiString;
         };
    auto UNICODE_STRING UnicodeString;
    auto BOOLEAN        NTbAllocateBuffer;
    auto WCHAR          szBuffer[BUFFER_SIZE_OemToCharA];

    RtlInitString ( &OemString, pszSource );

    if ( RtlOemStringToUnicodeSize ( &OemString ) <= sizeof(szBuffer) )
        {
        NTbAllocateBuffer = FALSE;
        UnicodeString.MaximumLength = sizeof(szBuffer);
        UnicodeString.Buffer = szBuffer;
        }
    else
        {
        NTbAllocateBuffer = TRUE;
        }

    if ( NT_SUCCESS ( RtlOemStringToUnicodeString ( &UnicodeString,
                                                    &OemString,
                                                    NTbAllocateBuffer ) ) )
        {
        AnsiString.MaximumLength = USHORT(-1);
        AnsiString.Buffer = pszDestination;

        SYM_VERIFY_BUFFER ( pszDestination,
                            RtlUnicodeStringToAnsiSize ( &UnicodeString ) );

        if ( NT_SUCCESS ( RtlUnicodeStringToAnsiString ( &AnsiString,
                                                         &UnicodeString,
                                                         FALSE ) ) )
            {
            SYM_ASSERT ( AnsiString.Length );
            pszDestination[AnsiString.Length - 1] = '\0';
            if ( NTbAllocateBuffer ) RtlFreeUnicodeString ( &UnicodeString );
            return ( TRUE );
            }

        if ( NTbAllocateBuffer ) RtlFreeUnicodeString ( &UnicodeString );
        }

    *pszDestination = '\0';
    SYM_ASSERT ( FALSE );
    return ( FALSE );

#elif defined(SYM_VXD)

    auto size_t         uCurrentSize,   // Bytes in current OEM buffer
                        uConvertedSize, // Bytes in converted ANSI buffer
                        uCurrentChars,  // Chars in current OEM buffer
                        uBytesLeft,     // Bytes left to convert
                        uCharSize;      // Size of char to add to buffer

    auto unsigned short szUnicodeBuffer[BUFFER_SIZE_OemToCharA];

    uBytesLeft = STRLEN(pszSource);

    while ( uBytesLeft != 0 )
        {
                                        // Get largest string to fit in
                                        // conversion buffer

        uCurrentSize  = 0;              // Bytes in string
        uCurrentChars = 0;              // Chars in string

        while ( uCurrentSize < uBytesLeft )
            {
                                        // Lead bytes for OEM same as ANSI

            uCharSize = DBCSIsLeadByte ( pszSource[uCurrentSize] ) ? 2 : 1;

            if ( uCurrentSize + uCharSize <= BUFFER_SIZE_OemToCharA )
                {
                uCurrentSize += uCharSize;
                uCurrentChars++;
                }
            else
                {
                break;
                }
            }

        if ( uCurrentSize > uBytesLeft )
            {
            uCurrentSize = uBytesLeft;
            SYM_ASSERT ( FALSE );       // Ill-formed DBCS string
            }

        if ( BCSToUni ( szUnicodeBuffer,
                        (unsigned char*)pszSource,
                        uCurrentSize,
                        BCS_OEM ) !=
             ( sizeof(*szUnicodeBuffer) * uCurrentChars ) )
            {
            SYM_ASSERT ( FALSE );
            }

        uConvertedSize = UniToBCS ( (unsigned char*)pszDestination,
                                    szUnicodeBuffer,
                                    sizeof(*szUnicodeBuffer) * uCurrentChars,
                                    uCurrentSize,
                                    BCS_WANSI );

                                        // This can fire in real world cases,
                                        // but indicates invalid characters.
        SYM_ASSERT ( uConvertedSize == uCurrentSize );

        pszSource      += uCurrentSize;
        pszDestination += uConvertedSize;
        uBytesLeft     -= uCurrentSize;
        }

    *pszDestination = '\0';

    return (TRUE);

#else

#error Not implemented.

#endif
    }

//---------------------------------------------------------------------------
//
// BOOL WINAPI OemToCharW ( PCSTR pszSource, PWSTR pszDestination );
//
// This function translates the OEM string at pszSource into its Unicode
// equivalent.  The result is stored at pszDestination.
//
// Returns TRUE if successful, FALSE if error.  Note that the Win32 specs
// document this function as always returning TRUE.
//
// If OemToCharW() returns FALSE, then the string at pszDestination is
// a partially converted pszSource.
//
// Notes:
//      SYM_VXD: This function may not be DBCS enabled; please check
//               the BCSToUni() IFSMgr service documentation.
//
//      pszSource and pszDestination should not point to the same string.
//
//---------------------------------------------------------------------------
BOOL WINAPI OemToCharW ( PCSTR pszSource, PWSTR pszDestination )
    {
    SYM_ASSERT ( pszSource );
    SYM_VERIFY_STRING ( pszSource );
    SYM_ASSERT ( pszDestination );
    SYM_ASSERT ( pszSource != (PCSTR) pszDestination );

#if defined(SYM_NTK)

    auto OEM_STRING     OemString;
    auto UNICODE_STRING UnicodeString;

    RtlInitString ( &OemString, pszSource );

    UnicodeString.MaximumLength = USHORT(-1);
    UnicodeString.Buffer = pszDestination;

    SYM_VERIFY_BUFFER ( pszDestination,
                        RtlOemStringToUnicodeSize ( &OemString ) );

    if ( NT_SUCCESS ( RtlOemStringToUnicodeString ( &UnicodeString,
                                                    &OemString,
                                                    FALSE ) ) )
        {
        SYM_ASSERT ( UnicodeString.Length / sizeof(WCHAR) );
        pszDestination[UnicodeString.Length / sizeof(WCHAR) - 1] = '\0';
        return ( TRUE );
        }

    *pszDestination = '\0';
    SYM_ASSERT ( FALSE );
    return ( FALSE );

#elif defined(SYM_VXD)

    auto size_t uSrcLength;

    if ( uSrcLength = strlen(pszSource) )
        {
        BCSToUni ( (string_t)pszDestination,
                   (unsigned char*)pszSource,
                   uSrcLength + 1,
                   BCS_OEM );
        }
    else
        {
        *pszDestination = '\0';
        }

    return ( TRUE );

#else

#error Not implemented.

#endif
    }

//---------------------------------------------------------------------------
//
// BOOL WINAPI OemToCharBuffW (
//      PCSTR pszSource,
//      PWSTR pszDestination,
//      DWORD cchDstLength );
//
// This function translates the OEM string at pszSource into its Unicode
// equivalent.  The result is stored at pszDestination.  The length (in
// characters) of pszDestination is specified by cchDstLength.
//
// Returns TRUE if successful, FALSE if error.  Note that the Win32 specs
// document this function as always returning TRUE.
//
// If OemToCharBuffW() returns FALSE, then the string at pszDestination is
// a partially converted pszSource.
//
// Notes:
//      pszSource and pszDestination should not point to the same string.
//
//---------------------------------------------------------------------------
#if defined(SYM_NTK)

BOOL WINAPI OemToCharBuffW (
    PCSTR pszSource,
    PWSTR pszDestination,
    DWORD cchDstLength )
    {
    SYM_ASSERT ( pszSource );
    SYM_VERIFY_STRING ( pszSource );
    SYM_ASSERT ( pszDestination );
    SYM_ASSERT ( int(cchDstLength) > 0 );
    SYM_VERIFY_BUFFER ( pszDestination, cchDstLength * sizeof(WCHAR) );
    SYM_ASSERT ( pszSource != (PCSTR) pszDestination );

    auto OEM_STRING     OemString;
    auto UNICODE_STRING UnicodeString;

    RtlInitString ( &OemString, pszSource );

    UnicodeString.MaximumLength = int(cchDstLength) * sizeof(WCHAR);
    UnicodeString.Buffer = pszDestination;

    if ( NT_SUCCESS ( RtlOemStringToUnicodeString ( &UnicodeString,
                                                    &OemString,
                                                    FALSE ) ) )
        {
        return ( TRUE );
        }

    *pszDestination = '\0';
    SYM_ASSERT ( FALSE );
    return ( FALSE );
    }

#endif

//---------------------------------------------------------------------------
//
// int WINAPI MultiByteToWideChar (
//      UINT    CodePage,
//      DWORD   dwFlags,
//      LPCSTR  lpMultiByteStr,
//      int     cchMultiByte,
//      LPWSTR  lpWideCharStr,
//      int     cchWideChar );
//
// To find out more how this function works, read the Win32 API specs, and
// the following notes:
//
//    - The only valid values for CodePage are CP_ACP and CP_OEMCP.
//    - dwFlags should always be 0.
//    - cchWideChar should always be > 0.  Therefore, lpWideCharStr
//      should always be != 0.
//    - Most of the parameter checking in this function is done with
//      SYM_ASSERTs, so invalid parameters can cause unpredictable
//      behavior.  The only run-time error checking that is done, and
//      is handled properly is after the usage of FS APIs.
//
//---------------------------------------------------------------------------
int WINAPI MultiByteToWideChar (
    UINT    CodePage,
    DWORD   dwFlags,
    LPCSTR  lpMultiByteStr,
    int     cchMultiByte,
    LPWSTR  lpWideCharStr,
    int     cchWideChar )
    {
    SYM_ASSERT ( CodePage == CP_ACP || CodePage == CP_OEMCP );
    SYM_ASSERT ( !dwFlags );
    SYM_ASSERT ( lpMultiByteStr );
    if ( cchMultiByte == -1 )
        {
        SYM_VERIFY_STRING ( lpMultiByteStr );
        }
    else
        {
        SYM_VERIFY_BUFFER ( lpMultiByteStr, cchMultiByte );
        }
    SYM_ASSERT ( cchWideChar > 0 );
    SYM_ASSERT ( lpWideCharStr );
    SYM_ASSERT ( lpMultiByteStr != (LPCSTR)lpWideCharStr );

#if defined(SYM_NTK)

    auto union {
         OEM_STRING     OemString;
         ANSI_STRING    AnsiString;
         };
    auto UNICODE_STRING UnicodeString;

    OemString.Buffer = (LPSTR)lpMultiByteStr;
    if ( cchMultiByte == -1 )
        {
        OemString.Length = strlen ( lpMultiByteStr );
        OemString.MaximumLength = OemString.Length + sizeof(char);
        }
    else
        {
        OemString.Length = \
        OemString.MaximumLength = cchMultiByte;
        }

    SYM_ASSERT ( sizeof(OemString) == sizeof(AnsiString) );

    if ( cchWideChar < ( CodePage == CP_ACP ?
                            RtlAnsiStringToUnicodeSize ( &AnsiString ) :
                            RtlOemStringToUnicodeSize ( &OemString ) ) / sizeof(WCHAR) )
        {
        SYM_ASSERT ( FALSE );
        return ( 0 );
        }

    SYM_VERIFY_BUFFER ( lpWideCharStr,
                        CodePage == CP_ACP ?
                            RtlAnsiStringToUnicodeSize ( &AnsiString ) :
                            RtlOemStringToUnicodeSize ( &OemString ) );

    UnicodeString.MaximumLength = cchWideChar * sizeof(WCHAR);
    UnicodeString.Buffer = lpWideCharStr;

    if ( NT_SUCCESS ( CodePage == CP_ACP ?
                          RtlAnsiStringToUnicodeString ( &UnicodeString,
                                                         &AnsiString,
                                                         FALSE ) :
                          RtlOemStringToUnicodeString ( &UnicodeString,
                                                        &OemString,
                                                        FALSE ) ) )
        {
        return ( UnicodeString.Length / sizeof(WCHAR) +
                 ( cchMultiByte == - 1 ? 1 : 0 ) );
        }

    SYM_ASSERT ( FALSE );
    return ( 0 );

#elif defined(SYM_VXD)

    auto int result;

    result = BCSToUni ( (string_t)lpWideCharStr,
                        (unsigned char*)lpMultiByteStr,
                        cchMultiByte == -1 ? strlen ( lpMultiByteStr ) :
                                             cchMultiByte,
                        CodePage == CP_ACP ? BCS_WANSI : BCS_OEM );

    if ( cchMultiByte == -1 )
        {
        result /= sizeof(WCHAR);
        lpWideCharStr[result] = '\0';
        return ( (result + 1) * sizeof(WCHAR) );
        }

    return result;

#else

#error Not implemented.

#endif
    }

//---------------------------------------------------------------------------
//
// int WINAPI WideCharToMultiByte (
//      UINT    CodePage,
//      DWORD   dwFlags,
//      LPCWSTR lpWideCharStr,
//      int     cchWideChar,
//      LPSTR   lpMultiByteStr,
//      int     cchMultiByte,
//      LPCSTR  lpDefaultChar,
//      LPBOOL  lpUsedDefaultChar );
//
// To find out more how this function works, read the Win32 API specs, and
// the following notes:
//
//    - The only valid values for CodePage are CP_ACP and CP_OEMCP.
//    - dwFlags should always be 0.
//    - cchMultiByte should always be > 0.  Therefore, lpMultiByteStr
//      should always be != 0.
//    - Most of the parameter checking in this function is done with
//      SYM_ASSERTs, so invalid parameters can cause unpredictable
//      behavior.  The only run-time error checking that is done, and
//      is handled properly is after the usage of FS APIs.
//    - lpDefaultChar and lpUsedDefaultChar should be NULL.
//
//---------------------------------------------------------------------------
int WINAPI WideCharToMultiByte (
    UINT    CodePage,
    DWORD   dwFlags,
    LPCWSTR lpWideCharStr,
    int     cchWideChar,
    LPSTR   lpMultiByteStr,
    int     cchMultiByte,
    LPCSTR  lpDefaultChar,
    LPBOOL  lpUsedDefaultChar )
    {
    SYM_ASSERT ( CodePage == CP_ACP || CodePage == CP_OEMCP );
    SYM_ASSERT ( !dwFlags );
    SYM_ASSERT ( lpWideCharStr );
    if ( cchWideChar == -1 )
        {
        SYM_VERIFY_STRING ( lpWideCharStr );
        }
    else
        {
        SYM_VERIFY_BUFFER ( lpWideCharStr, cchWideChar * sizeof(WCHAR) );
        }
    SYM_ASSERT ( cchMultiByte > 0 );
    SYM_ASSERT ( lpMultiByteStr );
    SYM_ASSERT ( lpWideCharStr != (LPCWSTR)lpMultiByteStr );
    SYM_ASSERT ( !lpDefaultChar );
    SYM_ASSERT ( !lpUsedDefaultChar );

#if defined(SYM_NTK)

    auto union {
         OEM_STRING     OemString;
         ANSI_STRING    AnsiString;
         };
    auto UNICODE_STRING UnicodeString;

    UnicodeString.Buffer = (LPWSTR)lpWideCharStr;
    if ( cchWideChar == -1 )
        {
        UnicodeString.Length = sizeof(WCHAR) * wcslen ( lpWideCharStr );
        UnicodeString.MaximumLength = UnicodeString.Length + sizeof(WCHAR);
        }
    else
        {
        UnicodeString.MaximumLength =
        UnicodeString.Length = sizeof(WCHAR) * cchWideChar;
        }

    if ( cchMultiByte < ( CodePage == CP_ACP ?
                            RtlUnicodeStringToAnsiSize ( &UnicodeString ) :
                            RtlUnicodeStringToOemSize ( &UnicodeString ) ) )
        {
        SYM_ASSERT ( FALSE );
        return ( 0 );
        }

    SYM_VERIFY_BUFFER ( lpMultiByteStr,
                        CodePage == CP_ACP ?
                            RtlUnicodeStringToAnsiSize ( &UnicodeString ) :
                            RtlUnicodeStringToOemSize ( &UnicodeString ) );

    OemString.MaximumLength = cchMultiByte;
    OemString.Buffer = lpMultiByteStr;

    SYM_ASSERT ( sizeof(OemString) == sizeof(AnsiString) );

    if ( NT_SUCCESS ( CodePage == CP_ACP ?
                          RtlUnicodeStringToAnsiString ( &AnsiString,
                                                         &UnicodeString,
                                                         FALSE ) :
                          RtlUnicodeStringToOemString ( &OemString,
                                                        &UnicodeString,
                                                        FALSE ) ) )
        {
        return ( OemString.Length + ( cchWideChar == - 1 ? 1 : 0 ) );
        }

    SYM_ASSERT ( FALSE );
    return ( 0 );

#elif defined(SYM_VXD)

    auto int result;
    auto int length;

    length = cchWideChar == -1 ? wcslen ( lpWideCharStr ) * sizeof(WCHAR) :
                                 cchWideChar;

    result = UniToBCS ( (unsigned char*)lpMultiByteStr,
                        (string_t)lpWideCharStr,
                        length,
                        cchMultiByte - ( cchWideChar == -1 ? 1 : 0 ),
                        CodePage == CP_ACP ? BCS_WANSI : BCS_OEM );

    if ( cchWideChar == -1 )
        {
        lpMultiByteStr[result] = '\0';
        return ( result + 1);
        }

    return result;

#else

#error Not implemented.

#endif
    }

//---------------------------------------------------------------------------
//
// BOOL WINAPI VxDGenericStringConvert (
//      PVOID pszSource,
//      int   SourceCharSet,
//      PVOID pszDestination,
//      int   DestinationCharSet
//      );
//
// This function converts the string at pszSource in uSourceCharSet to
// a string in uDestinationCharSet and places it at pszDestination.
//
// Entry:
//      pszSource          - a pointer to source string
//      SourceCharSet      - character set of the source string
//      pszDestination     - a pointer to destination buffer
//      DestinationCharSet - character set for the destination string
//
//      uSourceCharSet and uDestinationCharSet can be one of the following:
//          CHARSET_UNICODE
//          CHARSET_ANSI
//          CHARSET_OEM
//          CHARSET_CURRENT
//
// Exit:
//      TRUE if successful
//      FALSE if error
//          pszDestination may be destroyed
//
//---------------------------------------------------------------------------
#if defined(SYM_VXD)

BOOL WINAPI VxDGenericStringConvert (
    PVOID pszSource,
    int   SourceCharSet,
    PVOID pszDestination,
    int   DestinationCharSet
    )
    {
    #define MAX_VXD_STRING  0x8000

                                        // Validate input parameters
    SYM_ASSERT ( pszSource );
    SYM_ASSERT ( pszDestination );

                                        // Fix up character sets.

    if ( SourceCharSet == CHARSET_CURRENT )
        {
        SourceCharSet = VxDGetCurrentCHARSET();
        }

    if ( DestinationCharSet == CHARSET_CURRENT )
        {
        DestinationCharSet = VxDGetCurrentCHARSET();
        }

                                        // Convert string

    switch ( SourceCharSet )
        {
        case CHARSET_ANSI:
            switch ( DestinationCharSet )
                {
                case CHARSET_ANSI:
                    strcpy ( (char *)pszDestination, (const char*)pszSource );
                    return ( TRUE );

                case CHARSET_OEM:
                    return ( CharToOemA ( (PCSTR)pszSource, (PSTR)pszDestination ) );

                case CHARSET_UNICODE:
                    return ( MultiByteToWideChar ( CP_ACP,
                                                   0,
                                                   (LPCSTR)pszSource,
                                                   -1,
                                                   (LPWSTR)pszDestination,
                                                   MAX_VXD_STRING ) != 0 ? TRUE : FALSE );

                default:
                    SYM_ASSERT ( FALSE );
                    return ( FALSE );
                }
            break;

        case CHARSET_OEM:
            switch ( DestinationCharSet )
                {
                case CHARSET_ANSI:
                    return ( OemToCharA ( (PCSTR)pszSource, (PSTR)pszDestination ) );

                case CHARSET_OEM:
                    strcpy ( (char *)pszDestination, (const char *)pszSource );
                    return ( TRUE );

                case CHARSET_UNICODE:
                    return ( MultiByteToWideChar ( CP_OEMCP,
                                                   0,
                                                   (LPCSTR)pszSource,
                                                   -1,
                                                   (LPWSTR)pszDestination,
                                                   MAX_VXD_STRING ) != 0 ? TRUE : FALSE );

                default:
                    SYM_ASSERT ( FALSE );
                    return ( FALSE );
                }
            break;

        case CHARSET_UNICODE:
            switch ( DestinationCharSet )
                {
                case CHARSET_ANSI:
                    return ( WideCharToMultiByte ( CP_ACP,
                                                   0,
                                                   (LPCWSTR)pszSource,
                                                   -1,
                                                   (LPSTR)pszDestination,
                                                   MAX_VXD_STRING,
                                                   NULL,
                                                   NULL ) != 0 ? TRUE : FALSE );

                case CHARSET_OEM:
                    return ( WideCharToMultiByte ( CP_OEMCP,
                                                   0,
                                                   (LPCWSTR)pszSource,
                                                   -1,
                                                   (LPSTR)pszDestination,
                                                   MAX_VXD_STRING,
                                                   NULL,
                                                   NULL ) != 0 ? TRUE : FALSE );

                case CHARSET_UNICODE:
                    wcscpy ( (wchar_t *)pszDestination, (const wchar_t *)pszSource );
                    return ( TRUE );

                default:
                    SYM_ASSERT ( FALSE );
                    return ( FALSE );
                }
            break;

        default:
            SYM_ASSERT ( FALSE );
        }

    return ( FALSE );
    }

#endif


//---------------------------------------------------------------------------
//
// VOID __VxDAssertFailed ( LPSTR lpszFile, int iLine );
//
// This function services assertion failures.
//
// Entry:
//      lpszFile - pointer to a file where assertion had failed.
//      iLine    - line number if the file where assertion had failed.
//
// Exit:
//      -
//
// Notes:
//      The text strings in this function should not be translated.
//
//---------------------------------------------------------------------------
#if defined(SYM_VXD)

VOID __VxDAssertFailed ( LPSTR lpszFile, int iLine )
    {
    auto char szLineNum[] = "00000\n\r";
    auto int  iDigit      = 5;

    do
        {
        szLineNum[--iDigit] = (char)((iLine % 10) + '0');
        iLine /= 10;
        } while ( iLine > 0 && iDigit > 0 );

    Out_Debug_String("SYM_ASSERT: Assertion failed ");
    Out_Debug_String(lpszFile);
    Out_Debug_String(", line ");
    Out_Debug_String(&szLineNum[iDigit]);

    _asm int 1
    }

#endif


//---------------------------------------------------------------------------
//
// VOID NTKAssertMemoryIsLocked (
//      PVOID pvMemory,
//      DWORD dwSize
//      );
//
// This function ensures that every byte in the buffer pointed to by
// pvMemory with length dwSize bytes is pagelocked.  If the memory is not
// pagelocked, NTKAssertMemoryIsLocked() will SYM_ASSERT ( FALSE ).  This
// function will be able to validate addresses only if they are located in
// pagelocked driver data segment defined by including DRVSEG.H.  Code
// can also be added to search ahead of the address for tags placed by
// MemAllocPtr(), but currently MemAllocPtr() does not place any tags
// specifying exactly what kind of memory was allocated.  If implementing
// such check becomes necessary, then we will have to modify MemAllocPtr()
// as well.  Note: This function currently relies on MmLockPagableDataSection
// to return a pointer to the image section header of the particular section.
// If this changes, or if the image section header format changes, this
// function will crash!
//
// NTKAssertMemoryIsLockd() will be defined only if compiling for SYM_DEBUG
// or PRERELEASE with SYM_PARANOID set.
//
// No strings in this function should be translated.
//
// Entry:
//      pvMemory - va of block
//      dwSize   - length of block (in bytes)
//
// Exit:
//      -
//
//---------------------------------------------------------------------------
#if defined(SYM_NTK) && defined(SYM_PARANOID) && (defined(SYM_DEBUG) || defined(PRERELEASE))

VOID NTKAssertMemoryIsLocked (
    PVOID pvMemory,
    DWORD dwSize
    )
    {
    auto PIMAGE_SECTION_HEADER Section;

                                        // The image section handle is just
                                        // a pointer to an image section
                                        // (at least in NT 3.51).  So we get
                                        // the handle that was returned to
                                        // us when we locked our pagelocked
                                        // data segment in CoreGlobalInit().

    if ( ! ( Section = (PIMAGE_SECTION_HEADER)pvLD ) )
        {
        KdPrint (( "NTKAssertMemoryIsLocked(): invalid image section handle\n" ));
        SYM_ASSERT ( FALSE );
        return;
        }

                                        // Now, just in case, make sure that
                                        // the image section contains our
                                        // name.  It should be the name
                                        // defined by DRVSEG.H, which is
                                        // "PAGELKD".  This operation may
                                        // crash if NT changes the format
                                        // of the image section handle so
                                        // that it does not point to
                                        // IMAGE_SECTION_HEADER.

    if ( Section->Name[0] != 'P' ||
         Section->Name[1] != 'A' ||
         Section->Name[2] != 'G' ||
         Section->Name[3] != 'E' ||
         Section->Name[4] != 'L' ||
         Section->Name[5] != 'K' ||
         Section->Name[6] != 'D' )
        {
        KdPrint (( "NTKAssertMemoryIsLocked(): PAGELKD image section handle does not point to valid header\n" ));
        SYM_ASSERT ( FALSE );
        return;
        }

                                        // OK, now we can be about 4.83% sure
                                        // that the pointer we have is
                                        // actually really pointing to
                                        // IMAGE_SECTION_HEADER.  But we
                                        // have the other 95.17% where we can
                                        // crash by doing the things below.

                                        // The PointerToLinenumbers member
                                        // of the header contains a pointer
                                        // to the starting va of segment.

    if ( !Section->PointerToLinenumbers )
        {
        KdPrint (( "NTKAssertMemoryIsLocked(): PAGELKD image section header contains null pointer in PointerToLinenumbers\n" ));
        SYM_ASSERT ( FALSE );
        }

                                        // The Misc.VirtualSize member of
                                        // the header contains the size of
                                        // the segment.

    if ( !Section->Misc.VirtualSize )
        {
        KdPrint (( "NTKAssertMemoryIsLocked(): PAGELKD image section header contains 0 in Misc.VirtualSize\n" ));
        SYM_ASSERT ( FALSE );
        return;
        }

                                        // The NumberOfLinenumbers member of
                                        // the header contains the lock count
                                        // of the segment.  If the count is 0
                                        // no locks are applied to the
                                        // segment.  1 means that the segment
                                        // is in process of being locked.  2
                                        // and above starts counting the
                                        // number of the locks.

    if ( Section->NumberOfLinenumbers < 2 )
        {
        KdPrint (( "NTKAssertMemoryIsLocked(): PAGELKD image section is not locked\n" ));
        SYM_ASSERT ( FALSE );
        return;
        }

                                        // Now that we have the start of the
                                        // segment and its length, see if the
                                        // supplied buffer is within the
                                        // given range.

    if ( ( (DWORD)pvMemory < Section->PointerToLinenumbers ) ||
         ( (DWORD)pvMemory + dwSize > Section->PointerToLinenumbers + Section->Misc.VirtualSize )
       )
        {
        KdPrint (( "NTKAssertMemoryIsLocked(): Supplied address is not within driver's PAGELKD segment\n" ));
        SYM_ASSERT ( FALSE );
        return;
        }
    }

#endif


//---------------------------------------------------------------------------
//
// Define Zv...() wraps for Zw...() functions.
//
//---------------------------------------------------------------------------
#if defined(SYM_NTK)

#if defined(ZwDEBUGMONITOR)

#define DRIVER_LOCKED_DATA_SEGMENT
#include "drvseg.h"

LONG counterOpenHandles = 0L;
LONG counterZwOpenKey = 0L;
LONG counterZwClose = 0L;
LONG counterZwCreateKey = 0L;
LONG counterZwQueryValueKey = 0L;
LONG counterZwSetValueKey = 0L;
LONG counterZwCreateFile = 0L;
LONG counterZwDeviceIoControlFile = 0L;
LONG counterZwQueryVolumeInformationFile = 0L;
LONG counterZwQueryInformationFile = 0L;
LONG counterZwSetInformationFile = 0L;
LONG counterZwReadFile = 0L;
LONG counterZwWriteFile = 0L;
LONG counterZwDeleteFile = 0L;
LONG counterZwOpenFile = 0L;
LONG counterNtSetSecurityObject = 0L;
LONG counterZwQuerySecurityObject = 0L;
LONG counterZwQueryDirectoryFile = 0L;

LONG counterExAllocatePoolWithTag = 0L;
LONG counterExFreePool = 0L;

#define DRIVER_DEFAULT_DATA_SEGMENT
#include "drvseg.h"

#endif // #if defined(ZwDEBUGMONITOR)

#if defined(USE_NEW_STACK_SWAPPING)

typedef struct {
    NTSTATUS x;
    DWORD p1;
    DWORD p2;
    DWORD p3;
    DWORD p4;
    DWORD p5;
    DWORD p6;
    DWORD p7;
    DWORD p8;
    DWORD p9;
    DWORD pA;
    DWORD pB;
    } USERSTACKPARAMS;

#define pparams ((USERSTACKPARAMS*)pvparams)

VOID __cdecl KernelStackZwOpenKey ( PVOID pvparams )
    {
    pparams->x = ZwOpenKey ( (PHANDLE) pparams->p1,
                             (ACCESS_MASK) pparams->p2,
                             (POBJECT_ATTRIBUTES) pparams->p3 );
    }

NTSTATUS
NTAPI
ZvOpenKey(
    PHANDLE h,
    ACCESS_MASK a,
    POBJECT_ATTRIBUTES o
    )
    {
    auto USERSTACKPARAMS params;

#if defined(ZwDEBUGMONITOR)

    SymInterlockedIncrement(&counterZwOpenKey);

#endif

    params.p1 = (DWORD) h;
    params.p2 = (DWORD) a;
    params.p3 = (DWORD) o;

    CallOnKernelStack ( KernelStackZwOpenKey, &params );

#if defined(ZwDEBUGMONITOR)

    SymInterlockedDecrement(&counterZwOpenKey);

    if ( NT_SUCCESS(params.x) )
        {
        KdPrint ( ( "(%08X)(CORE) ZwOpenKey() succeeded han=%08X len=%04X str=%ls\n", KeGetCurrentThread(), (DWORD)*h, o->ObjectName->Length, o->ObjectName->Buffer ) );
        SymInterlockedIncrement(&counterOpenHandles);
        }
    else if ( NT_SUCCESS(params.x) )
        {
        KdPrint ( ( "(%08X)(CORE) ZwOpenKey() returned %08X\n", KeGetCurrentThread(), params.x ) );
        }

#endif

    return(params.x);
    }

VOID __cdecl KernelStackZwClose ( PVOID pvparams )
    {
    pparams->x = ZwClose ( (HANDLE) pparams->p1 );
    }

NTSTATUS
NTAPI
ZvClose(
    HANDLE h
    )
    {
    auto USERSTACKPARAMS params;

#if defined(ZwDEBUGMONITOR)

    SymInterlockedIncrement(&counterZwClose);

#endif

    params.p1 = (DWORD) h;

    CallOnKernelStack ( KernelStackZwClose, &params );

#if defined(ZwDEBUGMONITOR)

    SymInterlockedDecrement(&counterZwClose);

    if ( NT_SUCCESS(params.x) )
        {
        KdPrint ( ( "(%08X)(CORE) ZwClose() succeeded han=%08X\n", KeGetCurrentThread(), h ) );
        SymInterlockedDecrement(&counterOpenHandles);
        }
    else if ( NT_SUCCESS(params.x) )
        {
        KdPrint ( ( "(%08X)(CORE) ZwClose() returned %08X\n", KeGetCurrentThread(), params.x ) );
        }

#endif

    return(params.x);
    }

VOID __cdecl KernelStackZwCreateKey ( PVOID pvparams )
    {
    pparams->x = ZwCreateKey ( (PHANDLE) pparams->p1,
                               (ACCESS_MASK) pparams->p2,
                               (POBJECT_ATTRIBUTES) pparams->p3,
                               (ULONG) pparams->p4,
                               (PUNICODE_STRING) pparams->p5,
                               (ULONG) pparams->p6,
                               (PULONG) pparams->p7 );
    }

NTSTATUS
NTAPI
ZvCreateKey(
    PHANDLE h,
    ACCESS_MASK a,
    POBJECT_ATTRIBUTES o,
    ULONG t,
    PUNICODE_STRING c,
    ULONG u,
    PULONG d
    )
    {
    auto USERSTACKPARAMS params;

#if defined(ZwDEBUGMONITOR)

    SymInterlockedIncrement(&counterZwCreateKey);

#endif

    params.p1 = (DWORD) h;
    params.p2 = (DWORD) a;
    params.p3 = (DWORD) o;
    params.p4 = (DWORD) t;
    params.p5 = (DWORD) c;
    params.p6 = (DWORD) u;
    params.p7 = (DWORD) d;

    CallOnKernelStack ( KernelStackZwCreateKey, &params );

#if defined(ZwDEBUGMONITOR)

    SymInterlockedDecrement(&counterZwCreateKey);

    if ( NT_SUCCESS(params.x) )
        {
        KdPrint ( ( "(%08X)(CORE) ZwCreateKey() succeeded han=%08X len=%04X str=%ls\n", KeGetCurrentThread(), (DWORD)*h, o->ObjectName->Length, o->ObjectName->Buffer ) );
        SymInterlockedIncrement(&counterOpenHandles);
        }
    else if ( NT_SUCCESS(params.x) )
        {
        KdPrint ( ( "(%08X)(CORE) ZwCreateKey() returned %08X\n", KeGetCurrentThread(), params.x ) );
        }

#endif

    return(params.x);
    }

VOID __cdecl KernelStackZwQueryValueKey ( PVOID pvparams )
    {
    pparams->x = ZwQueryValueKey ( (HANDLE) pparams->p1,
                                   (PUNICODE_STRING) pparams->p2,
                                   (KEY_VALUE_INFORMATION_CLASS) pparams->p3,
                                   (PVOID) pparams->p4,
                                   (ULONG) pparams->p5,
                                   (PULONG) pparams->p6 );
    }

NTSTATUS
NTAPI
ZvQueryValueKey(
    HANDLE h,
    PUNICODE_STRING v,
    KEY_VALUE_INFORMATION_CLASS c,
    PVOID i,
    ULONG l,
    PULONG r
    )
    {
    auto USERSTACKPARAMS params;

#if defined(ZwDEBUGMONITOR)

    SymInterlockedIncrement(&counterZwQueryValueKey);

#endif

    params.p1 = (DWORD) h;
    params.p2 = (DWORD) v;
    params.p3 = (DWORD) c;
    params.p4 = (DWORD) i;
    params.p5 = (DWORD) l;
    params.p6 = (DWORD) r;

    CallOnKernelStack ( KernelStackZwQueryValueKey, &params );

#if defined(ZwDEBUGMONITOR)

    SymInterlockedDecrement(&counterZwQueryValueKey);

#endif

    return(params.x);
    }

VOID __cdecl KernelStackZwSetValueKey ( PVOID pvparams )
    {
    pparams->x = ZwSetValueKey ( (HANDLE) pparams->p1,
                                 (PUNICODE_STRING) pparams->p2,
                                 (ULONG) pparams->p3,
                                 (ULONG) pparams->p4,
                                 (PVOID) pparams->p5,
                                 (ULONG) pparams->p6 );
    }

NTSTATUS
NTAPI
ZvSetValueKey(
    HANDLE h,
    PUNICODE_STRING v,
    ULONG i,
    ULONG t,
    PVOID d,
    ULONG s
    )
    {
    auto USERSTACKPARAMS params;

#if defined(ZwDEBUGMONITOR)

    SymInterlockedIncrement(&counterZwSetValueKey);

#endif

    params.p1 = (DWORD) h;
    params.p2 = (DWORD) v;
    params.p3 = (DWORD) i;
    params.p4 = (DWORD) t;
    params.p5 = (DWORD) d;
    params.p6 = (DWORD) s;

    CallOnKernelStack ( KernelStackZwSetValueKey, &params );

#if defined(ZwDEBUGMONITOR)

    SymInterlockedDecrement(&counterZwSetValueKey);

#endif

    return(params.x);
    }

VOID __cdecl KernelStackZwCreateFile ( PVOID pvparams )
    {
    pparams->x = ZwCreateFile ( (PHANDLE) pparams->p1,
                                (ACCESS_MASK) pparams->p2,
                                (POBJECT_ATTRIBUTES) pparams->p3,
                                (PIO_STATUS_BLOCK) pparams->p4,
                                (PLARGE_INTEGER) pparams->p5,
                                (ULONG) pparams->p6,
                                (ULONG) pparams->p7,
                                (ULONG) pparams->p8,
                                (ULONG) pparams->p9,
                                (PVOID) pparams->pA,
                                (ULONG) pparams->pB );
    }

NTSTATUS
NTAPI
ZvCreateFile(
    PHANDLE h,
    ACCESS_MASK a,
    POBJECT_ATTRIBUTES o,
    PIO_STATUS_BLOCK s,
    PLARGE_INTEGER l,
    ULONG at,
    ULONG ac,
    ULONG cd,
    ULONG co,
    PVOID eab,
    ULONG eal
    )
    {
    auto USERSTACKPARAMS params;

#if defined(ZwDEBUGMONITOR)

    SymInterlockedIncrement(&counterZwCreateFile);

#endif

    co |= FILE_COMPLETE_IF_OPLOCKED;

    params.p1 = (DWORD) h;
    params.p2 = (DWORD) a;
    params.p3 = (DWORD) o;
    params.p4 = (DWORD) s;
    params.p5 = (DWORD) l;
    params.p6 = (DWORD) at;
    params.p7 = (DWORD) ac;
    params.p8 = (DWORD) cd;
    params.p9 = (DWORD) co;
    params.pA = (DWORD) eab;
    params.pB = (DWORD) eal;

    CallOnKernelStack ( KernelStackZwCreateFile, &params );

    if ( NT_SUCCESS(params.x) && params.x != STATUS_SUCCESS )
        {
        params.x = ZwClose ( *h );

        SYM_ASSERT ( NT_SUCCESS(params.x) );

        params.x = STATUS_UNSUCCESSFUL;
        }

#if defined(ZwDEBUGMONITOR)

    SymInterlockedDecrement(&counterZwCreateFile);

    if ( NT_SUCCESS(params.x) )
        {
        KdPrint ( ( "(%08X)(CORE) ZwCreateFile() succeeded han=%08X len=%04X str=%ls\n", KeGetCurrentThread(), (DWORD)*h, o->ObjectName->Length, o->ObjectName->Buffer ) );
        SymInterlockedIncrement(&counterOpenHandles);
        }
    else if ( NT_SUCCESS(params.x) )
        {
        KdPrint ( ( "(%08X)(CORE) ZwCreateFile() returned %08X\n", KeGetCurrentThread(), params.x ) );
        }

#endif

    return(params.x);
    }

VOID __cdecl KernelStackZwDeviceIoControlFile ( PVOID pvparams )
    {
    pparams->x = ZwDeviceIoControlFile ( (HANDLE) pparams->p1,
                                         (HANDLE) pparams->p2,
                                         (PIO_APC_ROUTINE) pparams->p3,
                                         (PVOID) pparams->p4,
                                         (PIO_STATUS_BLOCK) pparams->p5,
                                         (ULONG) pparams->p6,
                                         (PVOID) pparams->p7,
                                         (ULONG) pparams->p8,
                                         (PVOID) pparams->p9,
                                         (ULONG) pparams->pA );
    }

NTSTATUS
NTAPI
ZvDeviceIoControlFile(
    HANDLE h,
    HANDLE e,
    PIO_APC_ROUTINE r,
    PVOID c,
    PIO_STATUS_BLOCK s,
    ULONG cc,
    PVOID in,
    ULONG inl,
    PVOID out,
    ULONG outl
    )
    {
    auto USERSTACKPARAMS params;

#if defined(ZwDEBUGMONITOR)

    SymInterlockedIncrement(&counterZwDeviceIoControlFile);

#endif

    params.p1 = (DWORD) h;
    params.p2 = (DWORD) e;
    params.p3 = (DWORD) r;
    params.p4 = (DWORD) c;
    params.p5 = (DWORD) s;
    params.p6 = (DWORD) cc;
    params.p7 = (DWORD) in;
    params.p8 = (DWORD) inl;
    params.p9 = (DWORD) out;
    params.pA = (DWORD) outl;

    CallOnKernelStack ( KernelStackZwDeviceIoControlFile, &params );

#if defined(ZwDEBUGMONITOR)

    SymInterlockedDecrement(&counterZwDeviceIoControlFile);

#endif

    return(params.x);
    }

VOID __cdecl KernelStackZwQueryVolumeInformationFile ( PVOID pvparams )
    {
    pparams->x = ZwQueryVolumeInformationFile ( (HANDLE) pparams->p1,
                                                (PIO_STATUS_BLOCK) pparams->p2,
                                                (PVOID) pparams->p3,
                                                (ULONG) pparams->p4,
                                                (FS_INFORMATION_CLASS) pparams->p5 );
    }

NTSTATUS
NTAPI
ZvQueryVolumeInformationFile(
    HANDLE h,
    PIO_STATUS_BLOCK s,
    PVOID i,
    ULONG l,
    FS_INFORMATION_CLASS c
    )
    {
    auto USERSTACKPARAMS params;

#if defined(ZwDEBUGMONITOR)

    SymInterlockedIncrement(&counterZwQueryVolumeInformationFile);

#endif

    params.p1 = (DWORD) h;
    params.p2 = (DWORD) s;
    params.p3 = (DWORD) i;
    params.p4 = (DWORD) l;
    params.p5 = (DWORD) c;

    CallOnKernelStack ( KernelStackZwQueryVolumeInformationFile, &params );

#if defined(ZwDEBUGMONITOR)

    SymInterlockedDecrement(&counterZwQueryVolumeInformationFile);

#endif

    return(params.x);
    }

VOID __cdecl KernelStackZwQueryInformationFile ( PVOID pvparams )
    {
    pparams->x = ZwQueryInformationFile ( (HANDLE) pparams->p1,
                                          (PIO_STATUS_BLOCK) pparams->p2,
                                          (PVOID) pparams->p3,
                                          (ULONG) pparams->p4,
                                          (FILE_INFORMATION_CLASS) pparams->p5 );
    }

NTSTATUS
NTAPI
ZvQueryInformationFile(
    HANDLE h,
    PIO_STATUS_BLOCK s,
    PVOID i,
    ULONG l,
    FILE_INFORMATION_CLASS c
    )
    {
    auto USERSTACKPARAMS params;

#if defined(ZwDEBUGMONITOR)

    SymInterlockedIncrement(&counterZwQueryInformationFile);

#endif

    params.p1 = (DWORD) h;
    params.p2 = (DWORD) s;
    params.p3 = (DWORD) i;
    params.p4 = (DWORD) l;
    params.p5 = (DWORD) c;

    CallOnKernelStack ( KernelStackZwQueryInformationFile, &params );

#if defined(ZwDEBUGMONITOR)

    SymInterlockedDecrement(&counterZwQueryInformationFile);

#endif

    return(params.x);
    }

VOID __cdecl KernelStackZwSetInformationFile ( PVOID pvparams )
    {
    pparams->x = ZwSetInformationFile ( (HANDLE) pparams->p1,
                                        (PIO_STATUS_BLOCK) pparams->p2,
                                        (PVOID) pparams->p3,
                                        (ULONG) pparams->p4,
                                        (FILE_INFORMATION_CLASS) pparams->p5 );
    }

NTSTATUS
NTAPI
ZvSetInformationFile(
    HANDLE h,
    PIO_STATUS_BLOCK s,
    PVOID i,
    ULONG l,
    FILE_INFORMATION_CLASS c
    )
    {
    auto USERSTACKPARAMS params;

#if defined(ZwDEBUGMONITOR)

    SymInterlockedIncrement(&counterZwSetInformationFile);

#endif

    params.p1 = (DWORD) h;
    params.p2 = (DWORD) s;
    params.p3 = (DWORD) i;
    params.p4 = (DWORD) l;
    params.p5 = (DWORD) c;

    CallOnKernelStack ( KernelStackZwSetInformationFile, &params );

#if defined(ZwDEBUGMONITOR)

    SymInterlockedDecrement(&counterZwSetInformationFile);

#endif

    return(params.x);
    }

VOID __cdecl KernelStackZwReadFile ( PVOID pvparams )
    {
    pparams->x = ZwReadFile ( (HANDLE) pparams->p1,
                              (HANDLE) pparams->p2,
                              (PIO_APC_ROUTINE) pparams->p3,
                              (PIO_APC_ROUTINE) pparams->p4,
                              (PIO_STATUS_BLOCK) pparams->p5,
                              (PVOID) pparams->p6,
                              (ULONG) pparams->p7,
                              (PLARGE_INTEGER) pparams->p8,
                              (PULONG) pparams->p9 );
    }

NTSTATUS
NTAPI
ZvReadFile(
    HANDLE h,
    HANDLE e,
    PIO_APC_ROUTINE r,
    PIO_APC_ROUTINE c,
    PIO_STATUS_BLOCK s,
    PVOID b,
    ULONG l,
    PLARGE_INTEGER o,
    PULONG k
    )
    {
    auto USERSTACKPARAMS params;

#if defined(ZwDEBUGMONITOR)

    SymInterlockedIncrement(&counterZwReadFile);

#endif

    params.p1 = (DWORD) h;
    params.p2 = (DWORD) e;
    params.p3 = (DWORD) r;
    params.p4 = (DWORD) c;
    params.p5 = (DWORD) s;
    params.p6 = (DWORD) b;
    params.p7 = (DWORD) l;
    params.p8 = (DWORD) o;
    params.p9 = (DWORD) k;

    CallOnKernelStack ( KernelStackZwReadFile, &params );

#if defined(ZwDEBUGMONITOR)

    SymInterlockedDecrement(&counterZwReadFile);

#endif

    return(params.x);
    }

VOID __cdecl KernelStackZwWriteFile ( PVOID pvparams )
    {
    pparams->x = ZwWriteFile ( (HANDLE) pparams->p1,
                               (HANDLE) pparams->p2,
                               (PIO_APC_ROUTINE) pparams->p3,
                               (PVOID) pparams->p4,
                               (PIO_STATUS_BLOCK) pparams->p5,
                               (PVOID) pparams->p6,
                               (ULONG) pparams->p7,
                               (PLARGE_INTEGER) pparams->p8,
                               (PULONG) pparams->p9 );
    }

NTSTATUS
NTAPI
ZvWriteFile(
    HANDLE h,
    HANDLE e,
    PIO_APC_ROUTINE a,
    PVOID c,
    PIO_STATUS_BLOCK s,
    PVOID b,
    ULONG l,
    PLARGE_INTEGER o,
    PULONG k
    )
    {
    auto USERSTACKPARAMS params;

#if defined(ZwDEBUGMONITOR)

    SymInterlockedIncrement(&counterZwWriteFile);

#endif

    params.p1 = (DWORD) h;
    params.p2 = (DWORD) e;
    params.p3 = (DWORD) a;
    params.p4 = (DWORD) c;
    params.p5 = (DWORD) s;
    params.p6 = (DWORD) b;
    params.p7 = (DWORD) l;
    params.p8 = (DWORD) o;
    params.p9 = (DWORD) k;

    CallOnKernelStack ( KernelStackZwWriteFile, &params );

#if defined(ZwDEBUGMONITOR)

    SymInterlockedDecrement(&counterZwWriteFile);

#endif

    return(params.x);
    }

VOID __cdecl KernelStackZwOpenFile ( PVOID pvparams )
    {
    pparams->x = ZwOpenFile ( (PHANDLE) pparams->p1,
                              (ACCESS_MASK) pparams->p2,
                              (POBJECT_ATTRIBUTES) pparams->p3,
                              (PIO_STATUS_BLOCK) pparams->p4,
                              (ULONG) pparams->p5,
                              (ULONG) pparams->p6 );
    }

NTSTATUS
NTAPI
ZvOpenFile(
    PHANDLE h,
    ACCESS_MASK a,
    POBJECT_ATTRIBUTES o,
    PIO_STATUS_BLOCK s,
    ULONG ac,
    ULONG oo
    )
    {
    auto USERSTACKPARAMS params;

#if defined(ZwDEBUGMONITOR)

    SymInterlockedIncrement(&counterZwOpenFile);

#endif

    oo |= FILE_COMPLETE_IF_OPLOCKED;

    params.p1 = (DWORD) h;
    params.p2 = (DWORD) a;
    params.p3 = (DWORD) o;
    params.p4 = (DWORD) s;
    params.p5 = (DWORD) ac;
    params.p6 = (DWORD) oo;

    CallOnKernelStack ( KernelStackZwOpenFile, &params );

    if ( NT_SUCCESS(params.x) && params.x != STATUS_SUCCESS )
        {
        params.x = ZwClose ( *h );

        SYM_ASSERT ( NT_SUCCESS(params.x) );

        params.x = STATUS_UNSUCCESSFUL;
        }

#if defined(ZwDEBUGMONITOR)

    SymInterlockedDecrement(&counterZwOpenFile);

    if ( NT_SUCCESS(params.x) )
        {
        KdPrint ( ( "(%08X)(CORE) ZwOpenFile() succeeded han=%08X len=%04X str=%ls\n", KeGetCurrentThread(), (DWORD)*h, o->ObjectName->Length, o->ObjectName->Buffer ) );
        SymInterlockedIncrement(&counterOpenHandles);
        }
    else if ( NT_SUCCESS(params.x) )
        {
        KdPrint ( ( "(%08X)(CORE) ZwOpenFile() returned %08X\n", KeGetCurrentThread(), params.x ) );
        }

#endif

    return(params.x);
    }

VOID __cdecl KernelStackZwSetSecurityObject ( PVOID pvparams )
    {
    pparams->x = NtSetSecurityObject ( (HANDLE) pparams->p1,
                                       (SECURITY_INFORMATION) pparams->p2,
                                       (PSECURITY_DESCRIPTOR) pparams->p3 );
    }

NTSTATUS
NTAPI
ZvSetSecurityObject(
    HANDLE h,
    SECURITY_INFORMATION i,
    PSECURITY_DESCRIPTOR d
    )
    {
    auto USERSTACKPARAMS params;

#if defined(ZwDEBUGMONITOR)

    SymInterlockedIncrement(&counterNtSetSecurityObject);

#endif

    params.p1 = (DWORD) h;
    params.p2 = (DWORD) i;
    params.p3 = (DWORD) d;

    CallOnKernelStack ( KernelStackZwSetSecurityObject, &params );

#if defined(ZwDEBUGMONITOR)

    SymInterlockedDecrement(&counterNtSetSecurityObject);

#endif

    return(params.x);
    }

VOID __cdecl KernelStackZwQuerySecurityObject ( PVOID pvparams )
    {
    pparams->x = ZwQuerySecurityObject ( (HANDLE) pparams->p1,
                                         (SECURITY_INFORMATION) pparams->p2,
                                         (PSECURITY_DESCRIPTOR) pparams->p3,
                                         (ULONG) pparams->p4,
                                         (PULONG) pparams->p5 );
    }

NTSTATUS
NTAPI
ZvQuerySecurityObject(
    HANDLE h,
    SECURITY_INFORMATION i,
    PSECURITY_DESCRIPTOR d,
    ULONG l,
    PULONG n
    )
    {
    auto USERSTACKPARAMS params;

#if defined(ZwDEBUGMONITOR)

    SymInterlockedIncrement(&counterZwQuerySecurityObject);

#endif

    params.p1 = (DWORD) h;
    params.p2 = (DWORD) i;
    params.p3 = (DWORD) d;
    params.p4 = (DWORD) l;
    params.p5 = (DWORD) n;

    CallOnKernelStack ( KernelStackZwQuerySecurityObject, &params );

#if defined(ZwDEBUGMONITOR)

    SymInterlockedDecrement(&counterZwQuerySecurityObject);

#endif

    return(params.x);
    }

VOID __cdecl KernelStackZwQueryDirectoryFile ( PVOID pvparams )
    {
    pparams->x = ZwQueryDirectoryFile ( (HANDLE) pparams->p1,
                                        (HANDLE) pparams->p2,
                                        (PIO_APC_ROUTINE) pparams->p3,
                                        (PVOID) pparams->p4,
                                        (PIO_STATUS_BLOCK) pparams->p5,
                                        (PVOID) pparams->p6,
                                        (ULONG) pparams->p7,
                                        (FILE_INFORMATION_CLASS) pparams->p8,
                                        (BOOLEAN) pparams->p9,
                                        (PUNICODE_STRING) pparams->pA,
                                        (BOOLEAN) pparams->pB );
    }

NTSTATUS
NTAPI
ZvQueryDirectoryFile(
    HANDLE h,
    HANDLE e,
    PIO_APC_ROUTINE r,
    PVOID c,
    PIO_STATUS_BLOCK s,
    PVOID i,
    ULONG l,
    FILE_INFORMATION_CLASS cl,
    BOOLEAN y,
    PUNICODE_STRING n,
    BOOLEAN rs
    )
    {
    auto USERSTACKPARAMS params;

#if defined(ZwDEBUGMONITOR)

    SymInterlockedIncrement(&counterZwQueryDirectoryFile);

#endif

    params.p1 = (DWORD) h;
    params.p2 = (DWORD) e;
    params.p3 = (DWORD) r;
    params.p4 = (DWORD) c;
    params.p5 = (DWORD) s;
    params.p6 = (DWORD) i;
    params.p7 = (DWORD) l;
    params.p8 = (DWORD) cl;
    params.p9 = (DWORD) y;
    params.pA = (DWORD) n;
    params.pB = (DWORD) rs;

    CallOnKernelStack ( KernelStackZwQueryDirectoryFile, &params );

#if defined(ZwDEBUGMONITOR)

    SymInterlockedDecrement(&counterZwQueryDirectoryFile);

#endif

    return(params.x);
    }



VOID __cdecl KernelStackExAllocatePoolWithTag ( PVOID pvparams )
    {
    pparams->x = ExAllocatePoolWithTag( (POOL_TYPE) pparams->p1,
                                        (DWORD) pparams->p2,
                                        (ULONG) pparams->p3 );
    }

PVOID
NTAPI
EvAllocatePoolWithTag(
    POOL_TYPE pt,
    DWORD dw,
    ULONG tag
    )
    {
    auto USERSTACKPARAMS params;

#if defined(ZwDEBUGMONITOR)

    SymInterlockedIncrement(&counterExAllocatePoolWithTag);

#endif

    params.p1 = (DWORD) pt;
    params.p2 = (DWORD) dw;
    params.p3 = (DWORD) tag;

    CallOnKernelStack ( KernelStackExAllocatePoolWithTag, &params );

#if defined(ZwDEBUGMONITOR)

    SymInterlockedDecrement(&counterExAllocatePoolWithTag);

#endif

    return(params.x);
    }

VOID __cdecl KernelStackExFreePool ( PVOID pvparams )
    {
        ExFreePool ( (PVOID) pparams->p1 );
    }

VOID
NTAPI
EvFreePool(
    PVOID pv
    )
    {
    auto USERSTACKPARAMS params;

#if defined(ZwDEBUGMONITOR)

    SymInterlockedIncrement(&counterExFreePool);

#endif

    params.p1 = (DWORD) pt;

    CallOnKernelStack ( KernelStackExFreePool, &params );

#if defined(ZwDEBUGMONITOR)

    SymInterlockedDecrement(&counterExFreePool);

#endif

    }

#else

#pragma optimize("y",off)

NTSTATUS
NTAPI
ZvOpenKey(
    PHANDLE h,
    ACCESS_MASK a,
    POBJECT_ATTRIBUTES o
    )
    {
    auto NTSTATUS x;
    auto void*    oldstack;

#if defined(ZwDEBUGMONITOR)

    SymInterlockedIncrement(&counterZwOpenKey);

#endif

    oldstack = CoreSwapStackOutTemp();

    // SYM_ASSERT ( IsSPInKernelStack() );

    x = ZwOpenKey(h,a,o);

    CoreSwapStackInTemp ( oldstack );

#if defined(ZwDEBUGMONITOR)

    SymInterlockedDecrement(&counterZwOpenKey);

    if ( NT_SUCCESS(x) )
        {
        KdPrint ( ( "(%08X)(CORE) ZwOpenKey() succeeded han=%08X len=%04X str=%ls\n", KeGetCurrentThread(), (DWORD)*h, o->ObjectName->Length, o->ObjectName->Buffer ) );
        SymInterlockedIncrement(&counterOpenHandles);
        }
    else if ( NT_SUCCESS(x) )
        {
        KdPrint ( ( "(%08X)(CORE) ZwOpenKey() returned %08X\n", KeGetCurrentThread(), x ) );
        }

#endif

    return(x);
    }

NTSTATUS
NTAPI
ZvClose(
    HANDLE h
    )
    {
    auto NTSTATUS x;
    auto void*    oldstack;

#if defined(ZwDEBUGMONITOR)

    SymInterlockedIncrement(&counterZwClose);

#endif

    oldstack = CoreSwapStackOutTemp();

    // SYM_ASSERT ( IsSPInKernelStack() );

    x = ZwClose(h);

    CoreSwapStackInTemp ( oldstack );

#if defined(ZwDEBUGMONITOR)

    SymInterlockedDecrement(&counterZwClose);

    if ( NT_SUCCESS(x) )
        {
        KdPrint ( ( "(%08X)(CORE) ZwClose() succeeded han=%08X\n", KeGetCurrentThread(), h ) );
        SymInterlockedDecrement(&counterOpenHandles);
        }
    else if ( NT_SUCCESS(x) )
        {
        KdPrint ( ( "(%08X)(CORE) ZwClose() returned %08X\n", KeGetCurrentThread(), x ) );
        }

#endif

    return(x);
    }

NTSTATUS
NTAPI
ZvCreateKey(
    PHANDLE h,
    ACCESS_MASK a,
    POBJECT_ATTRIBUTES o,
    ULONG t,
    PUNICODE_STRING c,
    ULONG u,
    PULONG d
    )
    {
    auto NTSTATUS x;
    auto void*    oldstack;

#if defined(ZwDEBUGMONITOR)

    SymInterlockedIncrement(&counterZwCreateKey);

#endif

    oldstack = CoreSwapStackOutTemp();

    // SYM_ASSERT ( IsSPInKernelStack() );

    x = ZwCreateKey(h,a,o,t,c,u,d);

    CoreSwapStackInTemp ( oldstack );

#if defined(ZwDEBUGMONITOR)

    SymInterlockedDecrement(&counterZwCreateKey);

    if ( NT_SUCCESS(x) )
        {
        KdPrint ( ( "(%08X)(CORE) ZwCreateKey() succeeded han=%08X len=%04X str=%ls\n", KeGetCurrentThread(), (DWORD)*h, o->ObjectName->Length, o->ObjectName->Buffer ) );
        SymInterlockedIncrement(&counterOpenHandles);
        }
    else if ( NT_SUCCESS(x) )
        {
        KdPrint ( ( "(%08X)(CORE) ZwCreateKey() returned %08X\n", KeGetCurrentThread(), x ) );
        }

#endif

    return(x);
    }

NTSTATUS
NTAPI
ZvQueryValueKey(
    HANDLE h,
    PUNICODE_STRING v,
    KEY_VALUE_INFORMATION_CLASS c,
    PVOID i,
    ULONG l,
    PULONG r
    )
    {
    auto NTSTATUS x;
    auto void*    oldstack;

#if defined(ZwDEBUGMONITOR)

    SymInterlockedIncrement(&counterZwQueryValueKey);

#endif

    oldstack = CoreSwapStackOutTemp();

    // SYM_ASSERT ( IsSPInKernelStack() );

    x = ZwQueryValueKey(h,v,c,i,l,r);

    CoreSwapStackInTemp ( oldstack );

#if defined(ZwDEBUGMONITOR)

    SymInterlockedDecrement(&counterZwQueryValueKey);

#endif

    return(x);
    }

NTSTATUS
NTAPI
ZvSetValueKey(
    HANDLE h,
    PUNICODE_STRING v,
    ULONG i,
    ULONG t,
    PVOID d,
    ULONG s
    )
    {
    auto NTSTATUS x;
    auto void*    oldstack;

#if defined(ZwDEBUGMONITOR)

    SymInterlockedIncrement(&counterZwSetValueKey);

#endif

    oldstack = CoreSwapStackOutTemp();

    // SYM_ASSERT ( IsSPInKernelStack() );

    x = ZwSetValueKey(h,v,i,t,d,s);

    CoreSwapStackInTemp ( oldstack );

#if defined(ZwDEBUGMONITOR)

    SymInterlockedDecrement(&counterZwSetValueKey);

#endif

    return(x);
    }

NTSTATUS
NTAPI
ZvCreateFile(
    PHANDLE h,
    ACCESS_MASK a,
    POBJECT_ATTRIBUTES o,
    PIO_STATUS_BLOCK s,
    PLARGE_INTEGER l,
    ULONG at,
    ULONG ac,
    ULONG cd,
    ULONG co,
    PVOID eab,
    ULONG eal
    )
    {
    auto NTSTATUS x;
    auto void*    oldstack;

#if defined(ZwDEBUGMONITOR)

    SymInterlockedIncrement(&counterZwCreateFile);

#endif

    co |= FILE_COMPLETE_IF_OPLOCKED;

    oldstack = CoreSwapStackOutTemp();

    // SYM_ASSERT ( IsSPInKernelStack() );

    x = ZwCreateFile(h,a,o,s,l,at,ac,cd,co,eab,eal);

    if ( NT_SUCCESS(x) && x != STATUS_SUCCESS )
        {
        x = ZwClose ( *h );

        SYM_ASSERT ( NT_SUCCESS(x) );

        x = STATUS_UNSUCCESSFUL;
        }

    CoreSwapStackInTemp ( oldstack );

#if defined(ZwDEBUGMONITOR)

    SymInterlockedDecrement(&counterZwCreateFile);

    if ( NT_SUCCESS(x) )
        {
        KdPrint ( ( "(%08X)(CORE) ZwCreateFile() succeeded han=%08X len=%04X str=%ls\n", KeGetCurrentThread(), (DWORD)*h, o->ObjectName->Length, o->ObjectName->Buffer ) );
        SymInterlockedIncrement(&counterOpenHandles);
        }
    else if ( NT_SUCCESS(x) )
        {
        KdPrint ( ( "(%08X)(CORE) ZwCreateFile() returned %08X\n", KeGetCurrentThread(), x ) );
        }

#endif

    return(x);
    }

NTSTATUS
NTAPI
ZvDeviceIoControlFile(
    HANDLE h,
    HANDLE e,
    PIO_APC_ROUTINE r,
    PVOID c,
    PIO_STATUS_BLOCK s,
    ULONG cc,
    PVOID in,
    ULONG inl,
    PVOID out,
    ULONG outl
    )
    {
    auto NTSTATUS x;
    auto void*    oldstack;

#if defined(ZwDEBUGMONITOR)

    SymInterlockedIncrement(&counterZwDeviceIoControlFile);

#endif

    oldstack = CoreSwapStackOutTemp();

    // SYM_ASSERT ( IsSPInKernelStack() );

    x = ZwDeviceIoControlFile(h,e,r,c,s,cc,in,inl,out,outl);

    CoreSwapStackInTemp ( oldstack );

#if defined(ZwDEBUGMONITOR)

    SymInterlockedDecrement(&counterZwDeviceIoControlFile);

#endif

    return(x);
    }

NTSTATUS
NTAPI
ZvQueryVolumeInformationFile(
    HANDLE h,
    PIO_STATUS_BLOCK s,
    PVOID i,
    ULONG l,
    FS_INFORMATION_CLASS c
    )
    {
    auto NTSTATUS x;
    auto void*    oldstack;

#if defined(ZwDEBUGMONITOR)

    SymInterlockedIncrement(&counterZwQueryVolumeInformationFile);

#endif

    oldstack = CoreSwapStackOutTemp();

    // SYM_ASSERT ( IsSPInKernelStack() );

    x = ZwQueryVolumeInformationFile(h,s,i,l,c);

    CoreSwapStackInTemp ( oldstack );

#if defined(ZwDEBUGMONITOR)

    SymInterlockedDecrement(&counterZwQueryVolumeInformationFile);

#endif

    return(x);
    }

NTSTATUS
NTAPI
ZvQueryInformationFile(
    HANDLE h,
    PIO_STATUS_BLOCK s,
    PVOID i,
    ULONG l,
    FILE_INFORMATION_CLASS c
    )
    {
    auto NTSTATUS x;
    auto void*    oldstack;

#if defined(ZwDEBUGMONITOR)

    SymInterlockedIncrement(&counterZwQueryInformationFile);

#endif

    oldstack = CoreSwapStackOutTemp();

    // SYM_ASSERT ( IsSPInKernelStack() );

    x = ZwQueryInformationFile(h,s,i,l,c);

    CoreSwapStackInTemp ( oldstack );

#if defined(ZwDEBUGMONITOR)

    SymInterlockedDecrement(&counterZwQueryInformationFile);

#endif

    return(x);
    }

NTSTATUS
NTAPI
ZvSetInformationFile(
    HANDLE h,
    PIO_STATUS_BLOCK s,
    PVOID i,
    ULONG l,
    FILE_INFORMATION_CLASS c
    )
    {
    auto NTSTATUS x;
    auto void*    oldstack;

#if defined(ZwDEBUGMONITOR)

    SymInterlockedIncrement(&counterZwSetInformationFile);

#endif

    oldstack = CoreSwapStackOutTemp();

    // SYM_ASSERT ( IsSPInKernelStack() );

    x = ZwSetInformationFile(h,s,i,l,c);

    CoreSwapStackInTemp ( oldstack );

#if defined(ZwDEBUGMONITOR)

    SymInterlockedDecrement(&counterZwSetInformationFile);

#endif

    return(x);
    }

NTSTATUS
NTAPI
ZvReadFile(
    HANDLE h,
    HANDLE e,
    PIO_APC_ROUTINE r,
    PIO_APC_ROUTINE c,
    PIO_STATUS_BLOCK s,
    PVOID b,
    ULONG l,
    PLARGE_INTEGER o,
    PULONG k
    )
    {
    auto NTSTATUS x;
    auto void*    oldstack;

#if defined(ZwDEBUGMONITOR)

    SymInterlockedIncrement(&counterZwReadFile);

#endif

    oldstack = CoreSwapStackOutTemp();

    // SYM_ASSERT ( IsSPInKernelStack() );

    x = ZwReadFile(h,e,r,c,s,b,l,o,k);

    CoreSwapStackInTemp ( oldstack );

#if defined(ZwDEBUGMONITOR)

    SymInterlockedDecrement(&counterZwReadFile);

#endif

    return(x);
    }

NTSTATUS
NTAPI
ZvWriteFile(
    HANDLE h,
    HANDLE e,
    PIO_APC_ROUTINE a,
    PVOID c,
    PIO_STATUS_BLOCK s,
    PVOID b,
    ULONG l,
    PLARGE_INTEGER o,
    PULONG k
    )
    {
    auto NTSTATUS x;
    auto void*    oldstack;

#if defined(ZwDEBUGMONITOR)

    SymInterlockedIncrement(&counterZwWriteFile);

#endif

    oldstack = CoreSwapStackOutTemp();

    // SYM_ASSERT ( IsSPInKernelStack() );

    x = ZwWriteFile(h,e,a,c,s,b,l,o,k);

    CoreSwapStackInTemp ( oldstack );

#if defined(ZwDEBUGMONITOR)

    SymInterlockedDecrement(&counterZwWriteFile);

#endif

    return(x);
    }

NTSTATUS
NTAPI
ZvDeleteFile(
    POBJECT_ATTRIBUTES o
    )
    {
    auto NTSTATUS x;
    auto void*    oldstack;

    // Warning: This function is not oplock safe if operating under the
    // context of SRV!  The problem is that NtDeleteFile() (called by
    // ZwDeleteFile()) opens the file with DELETE flag only.  The flag
    // FILE_COMPLETE_IF_OPLOCKED is not given, so if there is an oplock on
    // the file, the NtDeleteFile() function will block until the oplock is
    // broken.  Instead of using ZvDeleteFile(), issue an NtCreateFile(), and
    // mark the file with pending delete.

    SYM_ASSERT ( FALSE );

#if defined(ZwDEBUGMONITOR)

    SymInterlockedIncrement(&counterZwDeleteFile);

#endif

    oldstack = CoreSwapStackOutTemp();

    // SYM_ASSERT ( IsSPInKernelStack() );

    x = ZwDeleteFile(o);

    CoreSwapStackInTemp ( oldstack );

#if defined(ZwDEBUGMONITOR)

    SymInterlockedDecrement(&counterZwDeleteFile);

#endif

    return(x);
    }

NTSTATUS
NTAPI
ZvOpenFile(
    PHANDLE h,
    ACCESS_MASK a,
    POBJECT_ATTRIBUTES o,
    PIO_STATUS_BLOCK s,
    ULONG ac,
    ULONG oo
    )
    {
    auto NTSTATUS x;
    auto void*    oldstack;

#if defined(ZwDEBUGMONITOR)

    SymInterlockedIncrement(&counterZwOpenFile);

#endif

    oo |= FILE_COMPLETE_IF_OPLOCKED;

    oldstack = CoreSwapStackOutTemp();

    // SYM_ASSERT ( IsSPInKernelStack() );

    x = ZwOpenFile(h,a,o,s,ac,oo);

    if ( NT_SUCCESS(x) && x != STATUS_SUCCESS )
        {
        x = ZwClose ( *h );

        SYM_ASSERT ( NT_SUCCESS(x) );

        x = STATUS_UNSUCCESSFUL;
        }

    CoreSwapStackInTemp ( oldstack );

#if defined(ZwDEBUGMONITOR)

    SymInterlockedDecrement(&counterZwOpenFile);

    if ( NT_SUCCESS(x) )
        {
        KdPrint ( ( "(%08X)(CORE) ZwOpenFile() succeeded han=%08X len=%04X str=%ls\n", KeGetCurrentThread(), (DWORD)*h, o->ObjectName->Length, o->ObjectName->Buffer ) );
        SymInterlockedIncrement(&counterOpenHandles);
        }
    else if ( NT_SUCCESS(x) )
        {
        KdPrint ( ( "(%08X)(CORE) ZwOpenFile() returned %08X\n", KeGetCurrentThread(), x ) );
        }

#endif

    return(x);
    }

NTSTATUS
NTAPI
ZvSetSecurityObject(
    HANDLE h,
    SECURITY_INFORMATION i,
    PSECURITY_DESCRIPTOR d
    )
    {
    auto NTSTATUS x;
    auto void*    oldstack;

#if defined(ZwDEBUGMONITOR)

    SymInterlockedIncrement(&counterNtSetSecurityObject);

#endif

    oldstack = CoreSwapStackOutTemp();

    // SYM_ASSERT ( IsSPInKernelStack() );

    x = NtSetSecurityObject(h,i,d);

    CoreSwapStackInTemp ( oldstack );

#if defined(ZwDEBUGMONITOR)

    SymInterlockedDecrement(&counterNtSetSecurityObject);

#endif

    return(x);
    }

NTSTATUS
NTAPI
ZvQuerySecurityObject(
    HANDLE h,
    SECURITY_INFORMATION i,
    PSECURITY_DESCRIPTOR d,
    ULONG l,
    PULONG n
    )
    {
    auto NTSTATUS x;
    auto void*    oldstack;

#if defined(ZwDEBUGMONITOR)

    SymInterlockedIncrement(&counterZwQuerySecurityObject);

#endif

    oldstack = CoreSwapStackOutTemp();

    // SYM_ASSERT ( IsSPInKernelStack() );

    x = ZwQuerySecurityObject(h,i,d,l,n);

    CoreSwapStackInTemp ( oldstack );

#if defined(ZwDEBUGMONITOR)

    SymInterlockedDecrement(&counterZwQuerySecurityObject);

#endif

    return(x);
    }

NTSTATUS
NTAPI
ZvQueryDirectoryFile(
    HANDLE h,
    HANDLE e,
    PIO_APC_ROUTINE r,
    PVOID c,
    PIO_STATUS_BLOCK s,
    PVOID i,
    ULONG l,
    FILE_INFORMATION_CLASS cl,
    BOOLEAN y,
    PUNICODE_STRING n,
    BOOLEAN rs
    )
    {
    auto NTSTATUS x;
    auto void*    oldstack;

#if defined(ZwDEBUGMONITOR)

    SymInterlockedIncrement(&counterZwQueryDirectoryFile);

#endif

    oldstack = CoreSwapStackOutTemp();

    // SYM_ASSERT ( IsSPInKernelStack() );

    x = ZwQueryDirectoryFile(h,e,r,c,s,i,l,cl,y,n,rs);

    CoreSwapStackInTemp ( oldstack );

#if defined(ZwDEBUGMONITOR)

    SymInterlockedDecrement(&counterZwQueryDirectoryFile);

#endif

    return(x);
    }


PVOID
NTAPI
EvAllocatePoolWithTag(
    POOL_TYPE pt,
    DWORD dw,
    ULONG tag
    )
    {
    auto PVOID pv;
    auto void*    oldstack;

#if defined(ZwDEBUGMONITOR)

    SymInterlockedIncrement(&counterExAllocatePoolWithTag);

#endif

    oldstack = CoreSwapStackOutTemp();

    // SYM_ASSERT ( IsSPInKernelStack() );

    pv = ExAllocatePoolWithTag(pt,dw,tag);

    CoreSwapStackInTemp ( oldstack );

#if defined(ZwDEBUGMONITOR)

    SymInterlockedDecrement(&counterExAllocatePoolWithTag);

#endif

    return(pv);
    }

VOID
NTAPI
EvFreePool(
    PVOID pv
    )
    {
    auto void*    oldstack;

#if defined(ZwDEBUGMONITOR)

    SymInterlockedIncrement(&counterExFreePool);

#endif

    oldstack = CoreSwapStackOutTemp();

    // SYM_ASSERT ( IsSPInKernelStack() );

    ExFreePool(pv);

    CoreSwapStackInTemp ( oldstack );

#if defined(ZwDEBUGMONITOR)

    SymInterlockedDecrement(&counterExFreePool);

#endif
    }

#pragma optimize("y",on)

#endif

#endif // #if defined(SYM_NTK) - the big block with Zw wraps.


//---------------------------------------------------------------------------
//
// Define support routines for SYM_VXD's version of SymCreateResource(),
// SymAcquireResourceShared(), SymAcquireResourceExclusive(),
// SymReleaseResource(), and SymDestroyResource().
//
// The functions below are documented in symsync.h.
//
//---------------------------------------------------------------------------
#if defined(SYM_VXD)

#define DRIVER_LOCKED_CODE_SEGMENT
#include "drvseg.h"

typedef struct tagVXDRESOURCEOWNER
    {
    tagVXDRESOURCEOWNER *prNext;
    DWORD               dwOwnerID;
    } *PVXDRESOURCEOWNER;

BOOL SymVXDCreateResource (
    PSYMRESOURCE prResource
    )
    {
    SYM_ASSERT ( prResource );
    SYM_ASSERT ( !prResource->bResourceInitialized );

                                        // Allocate semaphore to block shared
                                        // waiters.

    if ( prResource->dwSharedSemaphore = Create_Semaphore ( 0 ) )
        {
                                        // Allocate semaphore to block
                                        // exclusive waiters.

        if ( prResource->dwExclusiveSemaphore = Create_Semaphore ( 0 ) )
            {
                                        // Make sure the owner/wait lists
                                        // are empty.

            prResource->pvExclusiveOwner = NULL;
            prResource->pvBlockedExclusiveRequests = NULL;
            prResource->pvSharedOwners = NULL;
            prResource->pvBlockedSharedRequests = NULL;

                                        // Return success.

            return ( prResource->bResourceInitialized = TRUE );
            }

        Destroy_Semaphore ( prResource->dwSharedSemaphore );
        }

    return ( prResource->bResourceInitialized = FALSE );
    }

BOOL SymVXDAcquireResourceShared (
    PSYMRESOURCE prResource,
    BOOL         bBlock,
    int          iTimeOut
    )
    {
    auto PVXDRESOURCEOWNER prOwnerEntry;

    SYM_ASSERT ( prResource );
    SYM_ASSERT ( prResource->bResourceInitialized );
    SYM_ASSERT ( !iTimeOut );

                                        // Before we do anything, make sure
                                        // we have a resource ownership
                                        // entry allocated.  It is very
                                        // important this entry to be in
                                        // locked memory, so that we can work
                                        // with the resource without
                                        // acquiring access locks!

    prOwnerEntry = (PVXDRESOURCEOWNER) MemAllocPtr ( GMEM_FIXED, sizeof(*prOwnerEntry) );

    if ( !prOwnerEntry )
        {
        return ( FALSE );
        }

                                        // Generate current owner ID.

    prOwnerEntry->dwOwnerID = bVMMIs3x ? (DWORD)Get_Cur_VM_Handle() :
                                         (DWORD)Get_Cur_Thread_Handle();

#if defined(SYM_DEBUG) || defined(PRERELEASE)

    auto PVXDRESOURCEOWNER prDbgList;

                                        // Make sure the current owner does
                                        // NOT already own the resource
                                        // exclusively.

    SYM_ASSERT ( !prResource->pvExclusiveOwner ||
                 ((PVXDRESOURCEOWNER)prResource->pvExclusiveOwner)->dwOwnerID != prOwnerEntry->dwOwnerID );

                                        // Make sure the current owner does
                                        // NOT appear in the list with the
                                        // exclusive waiters.

    for ( prDbgList = (PVXDRESOURCEOWNER)prResource->pvBlockedExclusiveRequests;
          prDbgList;
          prDbgList = prDbgList->prNext )
        {
        SYM_ASSERT ( prDbgList->dwOwnerID != prOwnerEntry->dwOwnerID );
        }

                                        // Make sure the current owner does
                                        // NOT appear in the list of shared
                                        // owners.

    for ( prDbgList = (PVXDRESOURCEOWNER)prResource->pvSharedOwners;
          prDbgList;
          prDbgList = prDbgList->prNext )
        {
        SYM_ASSERT ( prDbgList->dwOwnerID != prOwnerEntry->dwOwnerID );
        }

                                        // Make sure the current owner does
                                        // NOT appear in the list of shared
                                        // waiters.

    for ( prDbgList = (PVXDRESOURCEOWNER)prResource->pvBlockedSharedRequests;
          prDbgList;
          prDbgList = prDbgList->prNext )
        {
        SYM_ASSERT ( prDbgList->dwOwnerID != prOwnerEntry->dwOwnerID );
        }

#endif

                                        // Check if there is exclusive owner
                                        // or waiters, and if we can't
                                        // acquire the resource right now,
                                        // do something about it.

    if ( prResource->pvExclusiveOwner ||
         prResource->pvBlockedExclusiveRequests )
        {
                                        // If blocking is allowed, then
                                        // add us to the end of the list of
                                        // shared waiters and block until
                                        // shared access is granted.  Whoever
                                        // signals us will update the
                                        // ownerships in prResource.

        if ( bBlock )
            {
            prOwnerEntry->prNext = NULL;

                                        // Note, that currently the order of
                                        // blocking for shared access doesn't
                                        // matter - we will release all
                                        // waiters together, in one loop.
                                        // Because of this, the code for
                                        // adding at the end of the list
                                        // is commented out, and the entries
                                        // are being added at the front of
                                        // the list.

            prOwnerEntry->prNext = (PVXDRESOURCEOWNER)prResource->pvBlockedSharedRequests;

            prResource->pvBlockedSharedRequests = prOwnerEntry;

//            if ( prResource->pvBlockedSharedRequests )
//                {
//                auto PVXDRESOURCEOWNER prList;
//
//                for ( prList = (PVXDRESOURCEOWNER)prResource->pvBlockedSharedRequests;
//                      prList->prNext;
//                      prList = prList->prNext );
//
//                prList->prNext = prOwnerEntry;
//                }
//            else
//                {
//                prResource->pvBlockedSharedRequests = prOwnerEntry;
//                }

            Wait_Semaphore ( prResource->dwSharedSemaphore, BLOCK_THREAD_IDLE );

            return ( TRUE );
            }
        else
            {
                                        // If blocking is not allowed, then
                                        // fail to acquire the resource.

            MemFreePtr ( prOwnerEntry );

            return ( FALSE );
            }
        }

                                        // No exclusive access is seen on the
                                        // horizon, so grant us the shared
                                        // access right away.

    prOwnerEntry->prNext = (PVXDRESOURCEOWNER)prResource->pvSharedOwners;

    prResource->pvSharedOwners = prOwnerEntry;

    return ( TRUE );
    }

BOOL SymVXDAcquireResourceExclusive (
    PSYMRESOURCE prResource,
    BOOL         bBlock,
    int          iTimeOut
    )
    {
    auto PVXDRESOURCEOWNER prOwnerEntry;

    SYM_ASSERT ( prResource );
    SYM_ASSERT ( prResource->bResourceInitialized );
    SYM_ASSERT ( !iTimeOut );

                                        // Before we do anything, make sure
                                        // we have a resource ownership
                                        // entry allocated.  It is very
                                        // important this entry to be in
                                        // locked memory, so that we can work
                                        // with the resource without
                                        // acquiring access locks!

    prOwnerEntry = (PVXDRESOURCEOWNER)  MemAllocPtr ( GMEM_FIXED, sizeof(*prOwnerEntry) );

    if ( !prOwnerEntry )
        {
        return ( FALSE );
        }

                                        // Generate current owner ID.

    prOwnerEntry->dwOwnerID = bVMMIs3x ? (DWORD)Get_Cur_VM_Handle() :
                                         (DWORD)Get_Cur_Thread_Handle();

#if defined(SYM_DEBUG) || defined(PRERELEASE)

    auto PVXDRESOURCEOWNER prDbgList;

                                        // Make sure the current owner does
                                        // NOT already own the resource
                                        // exclusively.

    SYM_ASSERT ( !prResource->pvExclusiveOwner ||
                 ((PVXDRESOURCEOWNER)prResource->pvExclusiveOwner)->dwOwnerID != prOwnerEntry->dwOwnerID );

                                        // Make sure the current owner does
                                        // NOT appear in the list with the
                                        // exclusive waiters.

    for ( prDbgList = (PVXDRESOURCEOWNER)prResource->pvBlockedExclusiveRequests;
          prDbgList;
          prDbgList = prDbgList->prNext )
        {
        SYM_ASSERT ( prDbgList->dwOwnerID != prOwnerEntry->dwOwnerID );
        }

                                        // Make sure the current owner does
                                        // NOT appear in the list of shared
                                        // owners.

    for ( prDbgList = (PVXDRESOURCEOWNER)prResource->pvSharedOwners;
          prDbgList;
          prDbgList = prDbgList->prNext )
        {
        SYM_ASSERT ( prDbgList->dwOwnerID != prOwnerEntry->dwOwnerID );
        }

                                        // Make sure the current owner does
                                        // NOT appear in the list of shared
                                        // waiters.

    for ( prDbgList = (PVXDRESOURCEOWNER)prResource->pvBlockedSharedRequests;
          prDbgList;
          prDbgList = prDbgList->prNext )
        {
        SYM_ASSERT ( prDbgList->dwOwnerID != prOwnerEntry->dwOwnerID );
        }

#endif

                                        // Check if there are any owners or
                                        // waiters, and if we can't acquire
                                        // the resource right now, do
                                        // something about it.

    if ( prResource->pvExclusiveOwner ||
         prResource->pvBlockedExclusiveRequests ||
         prResource->pvSharedOwners
       )
        {
                                        // If blocking is allowed, then
                                        // add us to the end of the list of
                                        // exclusive waiters and block until
                                        // exclusive access is granted.
                                        // Whoever signals us will update the
                                        // ownerships in prResource.

        if ( bBlock )
            {
            prOwnerEntry->prNext = NULL;

            if ( prResource->pvBlockedExclusiveRequests )
                {
                auto PVXDRESOURCEOWNER prList;

                for ( prList = (PVXDRESOURCEOWNER)prResource->pvBlockedExclusiveRequests;
                      prList->prNext;
                      prList = prList->prNext );

                prList->prNext = prOwnerEntry;
                }
            else
                {
                prResource->pvBlockedExclusiveRequests = prOwnerEntry;
                }

            Wait_Semaphore ( prResource->dwExclusiveSemaphore, BLOCK_THREAD_IDLE );

                                        // Make sure the VMM releases threads
                                        // in the same order in which they
                                        // were blocked.  If the SYM_ASSERT()
                                        // below fails, we will have to
                                        // implement some kind of ordering
                                        // scheme where the threads get
                                        // released in the proper order.

            SYM_ASSERT ( prResource->pvExclusiveOwner == prOwnerEntry );

            return ( TRUE );
            }
        else
            {
                                        // If blocking is not allowed, then
                                        // fail to acquire the resource.

            MemFreePtr ( prOwnerEntry );

            return ( FALSE );
            }
        }

                                        // No access to the resource is given
                                        // to anyone, so grant us the
                                        // exclusive access.

    prResource->pvExclusiveOwner = prOwnerEntry;

    return ( TRUE );
    }

VOID SymVXDReleaseResource (
    PSYMRESOURCE prResource
    )
    {
    auto DWORD             dwOwnerID;
    auto PVXDRESOURCEOWNER prReleaseMe, prTemp;

    SYM_ASSERT ( prResource );
    SYM_ASSERT ( prResource->bResourceInitialized );

                                        // Generate current owner ID.

    dwOwnerID = bVMMIs3x ? (DWORD)Get_Cur_VM_Handle() :
                           (DWORD)Get_Cur_Thread_Handle();

                                        // Check the type of lock that we are
                                        // releasing.

    if ( prResource->pvExclusiveOwner )
        {
                                        // Releasing exclusive lock.

                                        // Validate resource integrity.

        SYM_ASSERT ( ((PVXDRESOURCEOWNER)prResource->pvExclusiveOwner)->dwOwnerID == dwOwnerID );
        SYM_ASSERT ( !prResource->pvSharedOwners );

                                        // Invalidate the exclusive owner
                                        // entry.

        prReleaseMe = (PVXDRESOURCEOWNER)prResource->pvExclusiveOwner;

        prResource->pvExclusiveOwner = NULL;
        }
    else
        {
                                        // Releasing shared lock.

                                        // Validate resource integrity.

        SYM_ASSERT ( !prResource->pvExclusiveOwner );
        SYM_ASSERT ( prResource->pvSharedOwners );

                                        // Find the entry in the list of
                                        // shared owners.

        prTemp = NULL;

        for ( prReleaseMe = (PVXDRESOURCEOWNER)prResource->pvSharedOwners;
              prReleaseMe->dwOwnerID != dwOwnerID;
              prTemp = prReleaseMe, prReleaseMe = prReleaseMe->prNext )
            {
                                        // If the entry is missing, then we
                                        // are in a bad, bad situation.  NT
                                        // coredumps the machine, and so will
                                        // we because of accessing data
                                        // through NULL pointers.

            SYM_ASSERT ( prReleaseMe->prNext );
            }

                                        // Invalidate the shared owner entry.

        if ( prTemp )
            {
            prTemp->prNext = prReleaseMe->prNext;
            }
        else
            {
            prResource->pvSharedOwners = prReleaseMe->prNext;
            }
        }

                                        // Now that we have removed the
                                        // current owner entry, see who gets
                                        // access next.

                                        // Just for the sake of it, make
                                        // sure we are without exclusive
                                        // owner - because we shouldn't have
                                        // one down here.

    SYM_ASSERT ( !prResource->pvExclusiveOwner );

                                        // If there are shared owners, then
                                        // we must have just released another
                                        // shared owner.  In this case there
                                        // is nothing else to do - if there
                                        // is a waiting exclusive request, it
                                        // should keep waiting; if there are
                                        // waiting shared requests, then they
                                        // must be waiting for the blocked
                                        // exclusive request.

    if ( prResource->pvSharedOwners )
        {
        SYM_ASSERT ( !prResource->pvBlockedSharedRequests ||
                     prResource->pvBlockedExclusiveRequests );

                                        // Free the old owner entry.

        MemFreePtr ( prReleaseMe );

        return;
        }

                                        // If noone is owning the resource
                                        // at this point, and there is an
                                        // exclusive waiter, then grant it
                                        // access.

    if ( prResource->pvBlockedExclusiveRequests )
        {
        prResource->pvExclusiveOwner = prResource->pvBlockedExclusiveRequests;

        prResource->pvBlockedExclusiveRequests = ((PVXDRESOURCEOWNER)prResource->pvBlockedExclusiveRequests)->prNext;

        Signal_Semaphore ( prResource->dwExclusiveSemaphore );

                                        // Free the old owner entry.

        MemFreePtr ( prReleaseMe );

        return;
        }

                                        // If noone is owning the resource
                                        // at this point, and there are no
                                        // exclusive waiters, and there are
                                        // shared waiters, then grant access
                                        // to all of 'em.

    if ( prResource->pvBlockedSharedRequests )
        {
        prResource->pvSharedOwners = prResource->pvBlockedSharedRequests;

        prResource->pvBlockedSharedRequests = NULL;

                                        // Signal the shared semaphore as
                                        // many times as the number of
                                        // blocked shared requests.  Don't
                                        // signal as we count, because the
                                        // list may change due to thread
                                        // swapping forced by
                                        // Signal_Semaphore()!

        auto int iCount;

        for ( iCount = 1, prTemp = (PVXDRESOURCEOWNER)prResource->pvSharedOwners;
              prTemp = prTemp->prNext;
              iCount++ );

        for ( ;
              iCount--;
              Signal_Semaphore ( prResource->dwSharedSemaphore ) );

                                        // Free the old owner entry.

        MemFreePtr ( prReleaseMe );

        return;
        }

                                        // Well, the resoure is not used
                                        // right now...  Free the old
                                        // owner entry.

    MemFreePtr ( prReleaseMe );
    }

VOID SymVXDDestroyResource (
    PSYMRESOURCE prResource
    )
    {
    SYM_ASSERT ( prResource );
    SYM_ASSERT ( prResource->bResourceInitialized );

                                        // Make sure noone owns the resource.

    SYM_ASSERT ( !prResource->pvExclusiveOwner );
    SYM_ASSERT ( !prResource->pvBlockedExclusiveRequests );
    SYM_ASSERT ( !prResource->pvSharedOwners );
    SYM_ASSERT ( !prResource->pvBlockedSharedRequests );

                                        // Destory exclusive owner semaphore.

    Destroy_Semaphore ( prResource->dwExclusiveSemaphore );

                                        // Destory shared owner semaphore.

    Destroy_Semaphore ( prResource->dwSharedSemaphore );

                                        // Uninitialize prResource.

    prResource->bResourceInitialized = FALSE;
    }

#endif // #if defined(SYM_VXD) - the big block with VxD resource functions.
