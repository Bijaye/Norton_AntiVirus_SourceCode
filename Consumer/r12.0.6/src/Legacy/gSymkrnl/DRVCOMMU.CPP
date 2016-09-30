// Copyright 1996 - 1997 Symantec, Peter Norton Product Group
//***************************************************************************
//
// $Header:   S:/SYMKRNL/VCS/DRVCOMMU.CPv   1.8   11 Sep 1997 21:34:42   RStanev  $
//
// Description:
//      This file contains user mode routines for driver communication.
//
// Contains:
//      DesignatedThreads                       (local class)
//
//      DriverComm::DriverComm()
//      DriverComm::~DriverComm()
//      DriverComm::SetVxDLocation()
//      DriverComm::LoadAndOpen()
//      DriverComm::Open()
//      DriverComm::DesignateServiceThreads()
//      DriverComm::RegisterServiceCallback()
//      DriverComm::Send()
//      DriverComm::SendIO()
//
//      MainServiceThread()                     (local function)
//
//      DesignatedThreads::DesignatedThreads()  (local function)
//      DesignatedThreads::~DesignatedThreads() (local function)
//      DesignatedThreads::Create()             (local function)
//      DesignatedThreads::Link()               (inline function)
//      DesignatedThreads::Next()               (inline function)
//      DesignatedThreads::ServicePacket()      (local function)
//
//      DesignatedServiceThread()               (local function)
//
//      ImmediateServicePacket()                (local function)
//      DrvCommGetWindowsType()                 (local function)
//
// See Also:
//      DRVCOMM.H, DRVCOMMK.CPP
//
// Platforms supported:
//      SYM_WIN32
//
//***************************************************************************
// $Log:   S:/SYMKRNL/VCS/DRVCOMMU.CPv  $
// 
//    Rev 1.8   11 Sep 1997 21:34:42   RStanev
// Using platform specific flags and options when calling CreateFile().
//
//    Rev 1.7   13 May 1997 21:10:00   RStanev
// Fixed the error handling in DriverComm::LoadAndOpen() when a driver
// refuses to load.
//
//    Rev 1.6   07 Apr 1997 18:53:52   RStanev
// Removed test code that was failing assertions in DRVCOMMK.CPP.
//
//    Rev 1.5   06 Apr 1997 15:52:36   RStanev
// Unified communications with NTK and VxD platforms.  Added callbacks
// for cancelled packet requests.  Prepared module for synchronous
// communication.
//
//    Rev 1.4   21 Jan 1997 18:38:30   RSTANEV
// Added Windows 95 support to DriverComm.
// Fixed some incorrect GetLastError() reports due to MemAllocPtr()
// and MemFreePtr() updating the status of the last error.
// Replaced all references to Core's STR* functions with references to RTL's
// _tcs* functions.
//
//    Rev 1.3   01 May 1996 20:00:40   RSTANEV
// DriverComm::Open() now locks the driver in memory until the destructor
// of the class is called.
//
//    Rev 1.2   25 Mar 1996 12:04:22   RSTANEV
// Fixed the flags passed to MultiByteToWideChar() and WideCharToMultiByte().
//
//    Rev 1.1   22 Mar 1996 16:04:46   RSTANEV
// Overloaded DriverComm::Open() such that the function can be called with
// pszDriver in both multi-byte and Unicode character set.
//
//    Rev 1.0   18 Mar 1996 11:11:00   RSTANEV
// Initial revision.
//
//***************************************************************************

#if !defined(SYM_WIN32)
#error This file supports only Win32 target.
#endif

#include "platform.h"
#include "process.h"
#include "drvcomm.h"

//---------------------------------------------------------------------------
//
// Local definitions.
//
//---------------------------------------------------------------------------
#define DRVCOMM_WIN_NT      1
#define DRVCOMM_WIN_32S     2
#define DRVCOMM_WIN_95      3

//---------------------------------------------------------------------------
//
// Local function prototypes.
//
//---------------------------------------------------------------------------
static unsigned __stdcall MainServiceThread ( void *pvArgList );
static unsigned __stdcall DesignatedServiceThread ( void *pvArgList );

static int DrvCommGetWindowsType (
    VOID
    );

static void ImmediateServicePacket (
    PDRVCOMMCALLBACK     pfnCallback,
    PDRVCOMMDIOCTLPACKET prPacket
    );

//---------------------------------------------------------------------------
//
// DesignatedThreads is a class that keeps track of designated service
// threads.  This class is used only by DriverComm.
//
//---------------------------------------------------------------------------
class DesignatedThreads
    {
    public:

    DesignatedThreads();
    ~DesignatedThreads();

    BOOL Create (
        PDWORD           pdwServiceCodeArray,
        int              iNumberOfThreads,
        PDRVCOMMCALLBACK pfnDesignatedServiceCallback
        );

    DesignatedThreads* Link (
        DesignatedThreads* pListStart
        )
        {
        pNext = pListStart;
        return ( this );
        }

    DesignatedThreads* Next (
        void
        )
        {
        return ( pNext );
        }

    BOOL ServicePacket (
        PDRVCOMMDIOCTLPACKET prPacket
        );

    friend unsigned __stdcall DesignatedServiceThread ( void *pvArgList );

    private:

    DesignatedThreads *pNext;

    int iCodes;
    PDWORD pdwCodes;

    int iThreads;
    HANDLE *phThreads;

    HANDLE hSemaphore;

    PDRVCOMMCALLBACK pfnCallback;

    CRITICAL_SECTION rPacketListSync;
    PDRVCOMMDIOCTLPACKET prPacketListHead;
    PDRVCOMMDIOCTLPACKET prPacketListTail;
    };

//---------------------------------------------------------------------------
//
// DriverComm::DriverComm()
//
// This is the constructor for DriverComm class.  It sets all members of
// the newly allocated class to uninitialized status.
//
//---------------------------------------------------------------------------
DriverComm::DriverComm()
    {
    memset ( this, 0, sizeof(*this) );

    hDriverLock = INVALID_HANDLE_VALUE;
    }

//---------------------------------------------------------------------------
//
// DriverComm::~DriverComm()
//
// This is the destructor for DriverComm class.  If there was a main service
// thread created, or any designated service threads, they will be
// terminated.
//
// Note:
//      The client must ensure that no other class member functions are
//      called at the time a DriverComm object is destroyed.  If for some
//      reason a member function is executed while in ~DriverComm(), there
//      are no guarantees on how well the class will behave from now on.
//
//---------------------------------------------------------------------------
DriverComm::~DriverComm()
    {
                                        // See if there is a main service
                                        // thread created.

    if ( hMainServiceThread )
        {
                                        // Signal hMainServiceThread that it
                                        // is time to abandon everything and
                                        // terminate.  The little game played
                                        // here with the two BOOL members of
                                        // the class and the DIOCTL call is
                                        // a very delicate synchronization
                                        // scheme.  Please be careful when
                                        // changing it!

        bTerminateMainServiceThread = TRUE;

        if ( bMainServiceThreadResumed )
            {
                                        // The thread may have passed the
                                        // point of checking whether it
                                        // should be terminated or not, so
                                        // chances are it is already blocked
                                        // by the driver.  We have to tell
                                        // the driver to release it.  Note,
                                        // that call for releasing the
                                        // service thread is absolutely
                                        // harmless, even if the main service
                                        // thread is not blocked, and/or
                                        // there are no queued packets for
                                        // servicing.  In the worst case, we
                                        // would end up signaling the
                                        // semaphore which contains a counter
                                        // of how many service request
                                        // packets are in the queue more
                                        // times than the number of queued
                                        // packets.  This should not harm us,
                                        // since the code (both user and
                                        // kernel mode) is implemented such
                                        // that if a case like this is
                                        // detected, the thread will loop
                                        // until the semaphore counter drops
                                        // down to 0 and the thread is
                                        // blocked again.

            if ( !SendIO ( DIOCTL_SYM_INTERNAL_RELEASE_SERVICES,
                           0,
                           NULL,
                           0,
                           NULL,
                           0,
                           NULL ) )
                {
                SYM_ASSERT ( GetLastError() == ERROR_SUCCESS );
                }
            }
        else
            {
                                        // The thread has not yet checked
                                        // bTerminateMainServiceThread.  This
                                        // means that it is just about to do
                                        // this, or it is suspended.  Either
                                        // way, try to resume it so that it
                                        // eventually takes a look at
                                        // bTerminateMainServiceThread.

            ResumeThread ( hMainServiceThread );
            }

                                        // Wait for the thread to terminate.

        while ( WaitForSingleObject ( hMainServiceThread, INFINITE ) !=
                WAIT_OBJECT_0 )
            {
            SYM_ASSERT ( FALSE );
            }

                                        // Close main service thread handle.

        if ( !CloseHandle ( hMainServiceThread ) )
            {
            SYM_ASSERT ( FALSE );
            }
        }

                                        // Release the resources for all
                                        // designated service threads.

    auto DesignatedThreads *pDeleteMe;

    while ( pDeleteMe = (DesignatedThreads *)pvDesignatedThreads )
        {
        pvDesignatedThreads = pDeleteMe->Next();

        delete pDeleteMe;
        }

                                        // If we ever sent a
                                        // DIOCTL_SYM_INTERNAL_INIT_SERVICES
                                        // request to the driver, then send
                                        // a deinit request now.  This will
                                        // tell the driver to allow another
                                        // DriverComm class to start
                                        // servicing requests.

    if ( bServiceCommEstablished )
        {
        while ( !SendIO ( DIOCTL_SYM_INTERNAL_DEINIT_SERVICES,
                          0,
                          NULL,
                          0,
                          NULL,
                          0,
                          NULL ) )
            {
            if ( GetLastError() != ERROR_DRVCOMM_UNSAFE )
                {
                SYM_ASSERT ( GetLastError() == ERROR_SUCCESS );
                break;
                }
            }
        }

                                        // Release the memory allocated for
                                        // the main service thread's buffer.

    if ( prPacket )
        {
        delete [](PBYTE)prPacket;
        }

                                        // Free the memory allocated for
                                        // driver's name.

    if ( pszDriverName )
        {
        delete []pszDriverName;
        }

                                        // Close hDriverLock, thus unlocking
                                        // the driver and allowing the OS
                                        // to unload it.

    if ( hDriverLock != INVALID_HANDLE_VALUE &&
         !CloseHandle ( hDriverLock ) )
        {
        SYM_ASSERT ( FALSE );
        }

                                        // Release pszLocation, if allocated.

    if ( pszLocation )
        {
        delete []pszLocation;
        }
    }

//---------------------------------------------------------------------------
//
// BOOL DriverComm::SetVxDLocation (
//      PCTSTR pszPath
//      );
//
// This member of DriverComm affects the way DriverComm::LoadAndOpen() works
// in a Windows 95 environment.  If DriverComm::LoadAndOpen() is called
// without setting the Windows 95 driver location, it will assume that the
// driver is located in the SYSTEM directory of the OS.  If the driver
// resides in a different location, then this function should be used to
// override the default settings of DriverComm::LoadAndOpen().  Call
// DriverComm::SetVxDLocation() only before DriverComm::LoadAndOpen().
//
// Entry:
//      pszPath - Points to a NULL-terminated string containing the directory
//                where the driver is located.
//
// Exit:
//      This function returns TRUE if successful, or FALSE if error.  Use
//      GetLastError() to get more information if the function failed.
//
// Note:
//      Call this function only once per DriverComm object instance.
//      The client can call this function even if running under Windows NT.
//      In this case, the function will succeed, but
//      DriverComm::LoadAndOpen() will not be affected.
//
//---------------------------------------------------------------------------
BOOL DriverComm::SetVxDLocation (
    PCTSTR pszPath
    )
    {
    auto DWORD  dwPathLength, dwShortPathLength, dwLastError;
    auto PTSTR  pszShortPath;
    auto PCTSTR pszCurrent;

                                        // Validate parameters.

    SYM_ASSERT ( pszPath );
    SYM_ASSERT ( pszPath[0] );
    SYM_VERIFY_STRING ( pszPath );

    if ( !pszPath || !pszPath[0] )
        {
        SetLastError ( ERROR_DRVCOMM_BAD_ARGUMENT );

        return ( FALSE );
        }

    SYM_ASSERT ( !pszLocation );

                                        // Make sure we are running under
                                        // Windows 95.

    if ( DrvCommGetWindowsType() != DRVCOMM_WIN_95 )
        {
        return ( TRUE );
        }

                                        // Convert pszPath to SFN.  This is
                                        // required by VXDLDR.

    dwPathLength = sizeof(TCHAR) * ( _tcslen ( pszPath ) + 1 );

    pszShortPath = (PTSTR) new BYTE[dwPathLength];

    if ( !pszShortPath )
        {
        return ( FALSE );
        }

    dwShortPathLength = GetShortPathName ( pszPath,
                                           pszShortPath,
                                           dwPathLength );

    if ( !dwShortPathLength )
        {
        dwLastError = GetLastError();

        delete []pszShortPath;

        SetLastError ( dwLastError );

        return ( FALSE );
        }

    if ( dwShortPathLength > dwPathLength )
        {
        delete []pszShortPath;

        pszShortPath = (PTSTR) new BYTE[dwShortPathLength];

        if ( !pszShortPath )
            {
            return ( FALSE );
            }

        dwPathLength = GetShortPathName ( pszPath,
                                          pszShortPath,
                                          dwShortPathLength );

        if ( !dwPathLength || dwPathLength > dwShortPathLength )
            {
            dwLastError = GetLastError();

            delete []pszShortPath;

            SetLastError ( dwLastError );

            return ( FALSE );
            }
        }

                                        // Determine if we need an extra
                                        // backslash at the end of the
                                        // path.

    for ( pszCurrent = pszShortPath;
          * ( CharNext ( pszCurrent ) ) != _T('\0');
          pszCurrent = CharNext ( pszCurrent ) );

    pszCurrent = ( *pszCurrent == _T('\\') || *pszCurrent == _T('/') ) ?
                     _T("") : _T("\\");

                                        // Allocate buffer for the location,
                                        // private to our class.

    pszLocation = new TCHAR[_tcslen ( pszShortPath ) +
                            _tcslen ( pszCurrent ) + 1];

    if ( !pszLocation )
        {
        dwLastError = GetLastError();

        delete []pszShortPath ;

        SetLastError ( dwLastError );

        return ( FALSE );
        }

                                        // Form a complete path into the
                                        // newly allocated buffer.

    _tcscpy ( pszLocation, pszShortPath );
    _tcscat ( pszLocation, pszCurrent );

                                        // At least in MBCS mode, we seem to
                                        // have to convert the string to OEM,

#if !defined(UNICODE)

    CharToOem ( pszLocation, pszLocation );

#endif

                                        // Release allocated memory and
                                        // return to caller.

    delete []pszShortPath;

    return ( TRUE );
    }

//---------------------------------------------------------------------------
//
// BOOL DriverComm::SetVxDLocation (
//      PCSTR pszPath
//      );
//
// This function overloads the default DriverComm::SetVxDLocation() so that
// the client can call the SetVxDLocation() member of the class, passing any
// character set as pszPath.
//
// Entry:
//      Same as the first DriverComm::SetVxDLocation().
//
// Exit:
//      Same as the first DriverComm::SetVxDLocation().
//
// Note:
//      This function is available only when TCHAR is Unicode.
//
//---------------------------------------------------------------------------
#if defined(UNICODE)

BOOL DriverComm::SetVxDLocation (
    PCSTR pszPath
    )
    {
    auto int   iLength;
    auto PWSTR pszPathW;
    auto BOOL  bResult;
    auto DWORD dwLastError;

                                        // Validate parameters.

    SYM_ASSERT ( pszPath );
    SYM_VERIFY_STRING ( pszPath );

    if ( !pszPath )
        {
        SetLastError ( ERROR_DRVCOMM_BAD_ARGUMENT );

        return ( FALSE );
        }

                                        // Calculate the length of a buffer
                                        // where we will convert the name of
                                        // the file before we call the
                                        // fully functional
                                        // DriverComm::SetVxDLocation().

    iLength = MultiByteToWideChar ( CP_ACP,
                                    MB_PRECOMPOSED,
                                    pszPath,
                                    -1,
                                    NULL,
                                    0 );

    if ( !iLength )
        {
        return ( FALSE );
        }

                                        // Allocate buffer for conversion.

    pszPathW = new WCHAR[iLength];

    if ( !pszPathW )
        {
        return ( FALSE );
        }

                                        // Convert the string.

    bResult = MultiByteToWideChar ( CP_ACP,
                                    MB_PRECOMPOSED,
                                    pszPath,
                                    -1,
                                    pszPathW,
                                    iLength ) == 0 ? FALSE : TRUE;

                                        // Set the location.

    if ( bResult )
        {
        bResult = SetVxDLocation ( pszPathW );
        }

                                        // Release allocated memory.

    if ( !bResult )
        {
        dwLastError = GetLastError();
        }

    delete []pszPathW;

    if ( !bResult )
        {
        SetLastError ( dwLastError );
        }

                                        // Return status.

    return ( bResult );
    }

#endif // #if defined(UNICODE)

//---------------------------------------------------------------------------
//
// BOOL DriverComm::SetVxDLocation (
//      PCWSTR pszPath
//      );
//
// This function overloads the default DriverComm::SetVxDLocation() so that
// the client can call the SetVxDLocation() member of the class, passing any
// character set as pszPath.
//
// Entry:
//      Same as the first DriverComm::SetVxDLocation().
//
// Exit:
//      Same as the first DriverComm::SetVxDLocation().
//
// Note:
//      This function is available only when TCHAR is multi-byte.
//
//---------------------------------------------------------------------------
#if !defined(UNICODE)

BOOL DriverComm::SetVxDLocation (
    PCWSTR pszPath
    )
    {
    auto int   iLength;
    auto PSTR  pszPathA;
    auto BOOL  bResult;
    auto DWORD dwLastError;

                                        // Validate parameters.

    SYM_ASSERT ( pszPath );
    SYM_VERIFY_STRING ( pszPath );

    if ( !pszPath )
        {
        SetLastError ( ERROR_DRVCOMM_BAD_ARGUMENT );

        return ( FALSE );
        }

                                        // Calculate the length of a buffer
                                        // where we will convert the name of
                                        // the file before we call the
                                        // fully functional
                                        // DriverComm::SetVxDLocation().

    iLength = WideCharToMultiByte ( CP_ACP,
                                    WC_COMPOSITECHECK,
                                    pszPath,
                                    -1,
                                    NULL,
                                    0,
                                    NULL,
                                    NULL );

    if ( !iLength )
        {
        return ( FALSE );
        }

                                        // Allocate buffer for conversion.

    pszPathA = new char[iLength];

    if ( !pszPathA )
        {
        return ( FALSE );
        }

                                        // Convert the string.

    bResult = WideCharToMultiByte ( CP_ACP,
                                    WC_COMPOSITECHECK,
                                    pszPath,
                                    -1,
                                    pszPathA,
                                    iLength,
                                    NULL,
                                    NULL ) == 0 ? FALSE : TRUE;

                                        // Set the location.

    if ( bResult )
        {
        bResult = SetVxDLocation ( pszPathA );
        }

                                        // Release allocated memory.

    if ( !bResult )
        {
        dwLastError = GetLastError();
        }

    delete []pszPathA;

    if ( !bResult )
        {
        SetLastError ( dwLastError );
        }

                                        // Return status.

    return ( bResult );
    }

#endif // #if !defined(UNICODE)

//---------------------------------------------------------------------------
//
// BOOL DriverComm::LoadAndOpen (
//      PCTSTR pszDriver,
//      BOOL   bUnload
//      );
//
// This member of DriverComm initializes the class, loads pszDriver if it
// is not loaded, prepares the class for sending packets to the driver, and
// allows the client to register service callback(s).
//
// Entry:
//      pszDriver - Points to a NULL-terminated string containing the name
//                  of the driver to load and establish communication with.
//                  The string should not be formatted like it would normally
//                  be formatted for CreateFile() when opening a device,
//                  such as _T("\\\\.\\SAMPLE").  Instead, it should point
//                  to something like _T("SAMPLE").
//      bUnload   - TRUE if the driver should be unloaded when the DriverComm
//                  class is deleted, or FALSE if the driver should be left
//                  running.
//
// Exit:
//      This function returns TRUE if initialization was successful, or
//      FALSE if it failed.  Use GetLastError() to get more information
//      if the function failed.
//
// Note:
//      Call this function only once per DriverComm object instance.  Calling
//      DriverComm::LoadAndOpen() more than once for the same DriverComm
//      object is invalid, and the results of it are unpredictable.
//      Once DriverComm::LoadAndOpen() is called, you can not use
//      DriverComm::Open() from the same DriverComm class.
//
//---------------------------------------------------------------------------
BOOL DriverComm::LoadAndOpen (
    PCTSTR pszDriver,
    BOOL   bUnload
    )
    {
    auto PCTSTR pszPrefix;
    auto PCTSTR pszSuffix;
    auto PTSTR  pszFullDriverName;
    auto DWORD  dwLastError;
    auto BOOL   bResult;

                                        // Validate parameters.

    SYM_ASSERT ( pszDriver );
    SYM_VERIFY_STRING ( pszDriver );
    SYM_ASSERT ( !_tcschr ( pszDriver, _T('\\') ) );
    SYM_ASSERT ( !_tcschr ( pszDriver, _T('/') ) );

    if ( !pszDriver ||
         _tcschr ( pszDriver, _T('\\') ) ||
         _tcschr ( pszDriver, _T('/') )
       )
        {
        SetLastError ( ERROR_DRVCOMM_BAD_ARGUMENT );

        return ( FALSE );
        }

                                        // Create the name of the driver to
                                        // open.  Note, that currently only
                                        // Windows 95 VxDs loading is
                                        // supported.

    switch ( DrvCommGetWindowsType() )
        {
        case DRVCOMM_WIN_NT:

            SetLastError ( ERROR_DRVCOMM_NOT_IMPLEMENTED );

            return ( FALSE );

        case DRVCOMM_WIN_95:

            pszPrefix = _T("\\\\.\\");
            pszSuffix = _T(".VXD");

            break;

        default:

            SetLastError ( ERROR_DRVCOMM_NOT_IMPLEMENTED );

            return ( FALSE );
        }

    pszFullDriverName = new TCHAR[_tcslen(pszPrefix) +
                                  _tcslen(pszSuffix) +
                                  ( pszLocation ? _tcslen(pszLocation) : 0 ) +
                                  _tcslen(pszDriver) + 1];

    if ( !pszFullDriverName )
        {
        return ( FALSE );
        }

    _tcscpy ( pszFullDriverName, pszPrefix );

    if ( pszLocation )
        {
        _tcscat ( pszFullDriverName, pszLocation );
        }

    _tcscat ( pszFullDriverName, pszDriver );
    _tcscat ( pszFullDriverName, pszSuffix );

                                        // Open a handle to the driver, and
                                        // leave it open throughout the life
                                        // of the class.  This will lock the
                                        // driver in memory, until the handle
                                        // is released (which happens in the
                                        // destructor of DriverComm).

    hDriverLock = CreateFile ( pszFullDriverName,
                               0,
                               0,
                               NULL,
                               0,
                               bUnload ? FILE_FLAG_DELETE_ON_CLOSE : 0,
                               NULL );

    if ( hDriverLock != INVALID_HANDLE_VALUE )
        {
                                        // Open communication with the
                                        // driver.

        bResult = Open ( pszDriver );
        }
    else
        {
                                        // We failed to load the driver.

        bResult = FALSE;
        }

                                        // Release the memory

    dwLastError = GetLastError();

    delete []pszFullDriverName;

    SetLastError ( dwLastError );

    return ( bResult );
    }

//---------------------------------------------------------------------------
//
// BOOL DriverComm::LoadAndOpen (
//      PCSTR pszDriver,
//      BOOL  bUnload
//      );
//
// This function overloads the default DriverComm::LoadAndOpen() so that the
// client can call the LoadAndOpen() member of the class, passing any
// character set as pszDriver.
//
// Entry:
//      Same as the first DriverComm::LoadAndOpen().
//
// Exit:
//      Same as the first DriverComm::LoadAndOpen().
//
// Note:
//      This function is available only when TCHAR is Unicode.
//
//---------------------------------------------------------------------------
#if defined(UNICODE)

BOOL DriverComm::LoadAndOpen (
    PCSTR pszDriver,
    BOOL  bUnload
    )
    {
    auto int   iLength;
    auto PWSTR pszDriverW;
    auto BOOL  bResult;
    auto DWORD dwLastError;

                                        // Validate parameters.

    SYM_ASSERT ( pszDriver );
    SYM_VERIFY_STRING ( pszDriver );

    if ( !pszDriver )
        {
        SetLastError ( ERROR_DRVCOMM_BAD_ARGUMENT );

        return ( FALSE );
        }

                                        // Calculate the length of a buffer
                                        // where we will convert the name of
                                        // the file before we call the
                                        // fully functional
                                        // DriverComm::LoadAndOpen().

    iLength = MultiByteToWideChar ( CP_ACP,
                                    MB_PRECOMPOSED,
                                    pszDriver,
                                    -1,
                                    NULL,
                                    0 );

    if ( !iLength )
        {
        return ( FALSE );
        }

                                        // Allocate buffer for conversion.

    pszDriverW = new WCHAR[iLength];

    if ( !pszDriverW )
        {
        return ( FALSE );
        }

                                        // Convert the string.

    bResult = MultiByteToWideChar ( CP_ACP,
                                    MB_PRECOMPOSED,
                                    pszDriver,
                                    -1,
                                    pszDriverW,
                                    iLength ) == 0 ? FALSE : TRUE;

                                        // Open communication.

    if ( bResult )
        {
        bResult = LoadAndOpen ( pszDriverW, bUnload );
        }

                                        // Release allocated memory.

    if ( !bResult )
        {
        dwLastError = GetLastError();
        }

    delete []pszDriverW;

    if ( !bResult )
        {
        SetLastError ( dwLastError );
        }

                                        // Return status.

    return ( bResult );
    }

#endif // #if defined(UNICODE)

//---------------------------------------------------------------------------
//
// BOOL DriverComm::LoadAndOpen (
//      PCWSTR pszDriver,
//      BOOL   bUnload
//      );
//
// This function overloads the default DriverComm::LoadAndOpen() so that the
// client can call the LoadAndOpen() member of the class, passing any
// character set as pszDriver.
//
// Entry:
//      Same as the first DriverComm::LoadAndOpen().
//
// Exit:
//      Same as the first DriverComm::LoadAndOpen().
//
// Note:
//      This function is available only when TCHAR is multi-byte.
//
//---------------------------------------------------------------------------
#if !defined(UNICODE)

BOOL DriverComm::LoadAndOpen (
    PCWSTR pszDriver,
    BOOL   bUnload
    )
    {
    auto int   iLength;
    auto PSTR  pszDriverA;
    auto BOOL  bResult;
    auto DWORD dwLastError;

                                        // Validate parameters.

    SYM_ASSERT ( pszDriver );
    SYM_VERIFY_STRING ( pszDriver );

    if ( !pszDriver )
        {
        SetLastError ( ERROR_DRVCOMM_BAD_ARGUMENT );

        return ( FALSE );
        }

                                        // Calculate the length of a buffer
                                        // where we will convert the name of
                                        // the file before we call the
                                        // fully functional
                                        // DriverComm::LoadAndOpen().

    iLength = WideCharToMultiByte ( CP_ACP,
                                    WC_COMPOSITECHECK,
                                    pszDriver,
                                    -1,
                                    NULL,
                                    0,
                                    NULL,
                                    NULL );

    if ( !iLength )
        {
        return ( FALSE );
        }

                                        // Allocate buffer for conversion.

    pszDriverA = new char[iLength];

    if ( !pszDriverA )
        {
        return ( FALSE );
        }

                                        // Convert the string.

    bResult = WideCharToMultiByte ( CP_ACP,
                                    WC_COMPOSITECHECK,
                                    pszDriver,
                                    -1,
                                    pszDriverA,
                                    iLength,
                                    NULL,
                                    NULL ) == 0 ? FALSE : TRUE;

                                        // Open communication.

    if ( bResult )
        {
        bResult = LoadAndOpen ( pszDriverA, bUnload );
        }

                                        // Release allocated memory.

    if ( !bResult )
        {
        dwLastError = GetLastError();
        }

    delete []pszDriverA;

    if ( !bResult )
        {
        SetLastError ( dwLastError );
        }

                                        // Return status.

    return ( bResult );
    }

#endif // #if !defined(UNICODE)

//---------------------------------------------------------------------------
//
// BOOL DriverComm::Open (
//      PCTSTR pszDriver
//      );
//
// This member of DriverComm initializes the class, prepares it for sending
// packets to pszDriver, and allows the client to register service
// callback(s).
//
// Entry:
//      pszDriver - Points to a NULL-terminated string containing the name
//                  of the driver to open and establish communication with.
//                  The string should not be formatted like it would normally
//                  be formatted for CreateFile() when opening a device,
//                  such as _T("\\\\.\\SAMPLE").  Instead, it should point
//                  to something like _T("SAMPLE").
//
// Exit:
//      This function returns TRUE if initialization was successful, or
//      FALSE if it failed.  Use GetLastError() to get more information
//      if the function failed.
//
// Note:
//      Call this function only once per DriverComm object instance.  Calling
//      DriverComm::Open() more than once for the same DriverComm object is
//      invalid, and the results of it are unpredictable.
//      Once DriverComm::Open() is called, you can not use
//      DriverComm::LoadAndOpen() from the same DriverComm class.
//
//---------------------------------------------------------------------------
BOOL DriverComm::Open (
    PCTSTR pszDriver
    )
    {
    const PTSTR pszPrefix = _T("\\\\.\\");

                                        // Validate parameters.

    SYM_ASSERT ( pszDriver );
    SYM_VERIFY_STRING ( pszDriver );
    SYM_ASSERT ( !_tcschr ( pszDriver, _T('\\') ) );
    SYM_ASSERT ( !_tcschr ( pszDriver, _T('/') ) );

    if ( !pszDriver ||
         _tcschr ( pszDriver, _T('\\') ) ||
         _tcschr ( pszDriver, _T('/') )
       )
        {
        SetLastError ( ERROR_DRVCOMM_BAD_ARGUMENT );

        return ( FALSE );
        }

    SYM_ASSERT ( !pszDriverName );

                                        // Create the name of the driver to
                                        // open.

    pszDriverName = new TCHAR[_tcslen(pszPrefix) + _tcslen(pszDriver) + 1];

    if ( !pszDriverName )
        {
        return ( FALSE );
        }

    _tcscpy ( pszDriverName, pszPrefix );
    _tcscat ( pszDriverName, pszDriver );

                                        // Open a handle to the driver, and
                                        // leave it open throughout the life
                                        // of the class.  This will lock the
                                        // driver in memory, until the handle
                                        // is released (which happens in the
                                        // destructor of DriverComm).

    if ( hDriverLock == INVALID_HANDLE_VALUE )
        {
        switch ( DrvCommGetWindowsType() )
            {
            case DRVCOMM_WIN_95:

                hDriverLock = CreateFile ( pszDriverName,
                                           0,
                                           0,
                                           NULL,
                                           0,
                                           0,
                                           NULL );

                break;

            case DRVCOMM_WIN_NT:

                hDriverLock = CreateFile ( pszDriverName,
                                           GENERIC_READ | GENERIC_WRITE,
                                           FILE_SHARE_READ | FILE_SHARE_WRITE,
                                           NULL,
                                           OPEN_EXISTING,
                                           0,
                                           NULL );

                break;

            default:

                SetLastError ( ERROR_DRVCOMM_NOT_IMPLEMENTED );
            }
        }

                                        // Return to client.

    return ( hDriverLock == INVALID_HANDLE_VALUE ? FALSE : TRUE );
    }

//---------------------------------------------------------------------------
//
// BOOL DriverComm::Open (
//      PCSTR pszDriver
//      );
//
// This function overloads the default DriverComm::Open() so that the client
// can call the Open() member of the class, passing any character set as
// pszDriver.
//
// Entry:
//      Same as the first DriverComm::Open().
//
// Exit:
//      Same as the first DriverComm::Open().
//
// Note:
//      This function is available only when TCHAR is Unicode.
//
//---------------------------------------------------------------------------
#if defined(UNICODE)

BOOL DriverComm::Open (
    PCSTR pszDriver
    )
    {
    auto int   iLength;
    auto PWSTR pszDriverW;
    auto BOOL  bResult;
    auto DWORD dwLastError;

                                        // Validate parameters.

    SYM_ASSERT ( pszDriver );
    SYM_VERIFY_STRING ( pszDriver );

    if ( !pszDriver )
        {
        SetLastError ( ERROR_DRVCOMM_BAD_ARGUMENT );

        return ( FALSE );
        }

                                        // Calculate the length of a buffer
                                        // where we will convert the name of
                                        // the file before we call the
                                        // fully functional
                                        // DriverComm::Open().

    iLength = MultiByteToWideChar ( CP_ACP,
                                    MB_PRECOMPOSED,
                                    pszDriver,
                                    -1,
                                    NULL,
                                    0 );

    if ( !iLength )
        {
        return ( FALSE );
        }

                                        // Allocate buffer for conversion.

    pszDriverW = new WCHAR[iLength];

    if ( !pszDriverW )
        {
        return ( FALSE );
        }

                                        // Convert the string.

    bResult = MultiByteToWideChar ( CP_ACP,
                                    MB_PRECOMPOSED,
                                    pszDriver,
                                    -1,
                                    pszDriverW,
                                    iLength ) == 0 ? FALSE : TRUE;

                                        // Open communication.

    if ( bResult )
        {
        bResult = Open ( pszDriverW );
        }

                                        // Release allocated memory.

    if ( !bResult )
        {
        dwLastError = GetLastError();
        }

    delete []pszDriverW;

    if ( !bResult )
        {
        SetLastError ( dwLastError );
        }

                                        // Return status.

    return ( bResult );
    }

#endif // #if defined(UNICODE)

//---------------------------------------------------------------------------
//
// BOOL DriverComm::Open (
//      PCWSTR pszDriver
//      );
//
// This function overloads the default DriverComm::Open() so that the client
// can call the Open() member of the class, passing any character set as
// pszDriver.
//
// Entry:
//      Same as the first DriverComm::Open().
//
// Exit:
//      Same as the first DriverComm::Open().
//
// Note:
//      This function is available only when TCHAR is multi-byte.
//
//---------------------------------------------------------------------------
#if !defined(UNICODE)

BOOL DriverComm::Open (
    PCWSTR pszDriver
    )
    {
    auto int   iLength;
    auto PSTR  pszDriverA;
    auto BOOL  bResult;
    auto DWORD dwLastError;

                                        // Validate parameters.

    SYM_ASSERT ( pszDriver );
    SYM_VERIFY_STRING ( pszDriver );

    if ( !pszDriver )
        {
        SetLastError ( ERROR_DRVCOMM_BAD_ARGUMENT );

        return ( FALSE );
        }

                                        // Calculate the length of a buffer
                                        // where we will convert the name of
                                        // the file before we call the
                                        // fully functional
                                        // DriverComm::Open().

    iLength = WideCharToMultiByte ( CP_ACP,
                                    WC_COMPOSITECHECK,
                                    pszDriver,
                                    -1,
                                    NULL,
                                    0,
                                    NULL,
                                    NULL );

    if ( !iLength )
        {
        return ( FALSE );
        }

                                        // Allocate buffer for conversion.

    pszDriverA = new char[iLength];

    if ( !pszDriverA )
        {
        return ( FALSE );
        }

                                        // Convert the string.

    bResult = WideCharToMultiByte ( CP_ACP,
                                    WC_COMPOSITECHECK,
                                    pszDriver,
                                    -1,
                                    pszDriverA,
                                    iLength,
                                    NULL,
                                    NULL ) == 0 ? FALSE : TRUE;

                                        // Open communication.

    if ( bResult )
        {
        bResult = Open ( pszDriverA );
        }

                                        // Release allocated memory.

    if ( !bResult )
        {
        dwLastError = GetLastError();
        }

    delete []pszDriverA;

    if ( !bResult )
        {
        SetLastError ( dwLastError );
        }

                                        // Return status.

    return ( bResult );
    }

#endif // #if !defined(UNICODE)

//---------------------------------------------------------------------------
//
// BOOL DriverComm::DesignateServiceThreads (
//      PDWORD           pdwServiceCodeArray,
//      int              iNumberOfThreads,
//      PDRVCOMMCALLBACK pfnDesignatedServiceCallback
//      );
//
// This function designates extra service threads for processing packets
// with specific service codes.  This function can be called multiple times
// for each DriverComm class instance, but only before registering the main
// service callback with DriverComm::RegisterServiceCallback().  Calling
// DriverComm::DesignateServiceThreads() multiple times with matching
// service code array members should be avoided.
//
// Entry:
//      pdwServiceCodeArray          - a 0-terminated array of user-defined
//                                     service codes which will be serviced
//                                     by the designated threads.
//      iNumberOfThreads             - number of threads to designate for
//                                     servicing the requests specified in
//                                     pdwServiceCodeArray.
//      pfnDesignatedServiceCallback - pointer to a designated service
//                                     callback function to process the
//                                     requests.
//
// Exit:
//      TRUE if designated service threads successfully created, or FALSE
//      if error.  If this function failed, use GetLastError() to retrieve
//      error information.
//
// Note:
//      Keep the value for iNumberOfThreads low.  Theoretically, it should
//      not exceed the largest int - 1 or - 2 (don't have the exact count -
//      it is limited by semaphores, etc.).  But in real life,
//      iNumberOfThreads should be kept in the single, maybe double digit
//      values...
//
//---------------------------------------------------------------------------
BOOL DriverComm::DesignateServiceThreads (
    PDWORD           pdwServiceCodeArray,
    int              iNumberOfThreads,
    PDRVCOMMCALLBACK pfnDesignatedServiceCallback
    )
    {
    auto DesignatedThreads *pThreads;
    auto DWORD             dwLastError;

                                        // First, create a new instance of
                                        // the DesignatedThreads class.

    if ( pThreads = new DesignatedThreads )
        {
                                        // Create designated threads for the
                                        // new class instance.

        if ( pThreads->Create ( pdwServiceCodeArray,
                                iNumberOfThreads,
                                pfnDesignatedServiceCallback ) )
            {
                                        // Make the current DriverComm class
                                        // aware of the new designated
                                        // service threads.

            pvDesignatedThreads =
                pThreads->Link ( (DesignatedThreads *)pvDesignatedThreads );

            return ( TRUE );
            }

                                        // If we failed to initialize, but
                                        // we still have the class, release
                                        // it while preserving the last
                                        // error code.

        dwLastError = GetLastError();

        delete pThreads;

        SetLastError ( dwLastError );
        }
    else
        {
        SetLastError ( ERROR_DRVCOMM_OUT_OF_MEMORY );
        }

                                        // Fail the call.

    return ( FALSE );
    }

//---------------------------------------------------------------------------
//
// BOOL DriverComm::RegisterServiceCallback (
//      PDRVCOMMCALLBACK pfnServiceCallback
//      );
//
// This function registers service request callback function.  The service
// callback function creates a separate thread that "listens" for requests
// from the driver.  The driver issues requests by calling DriverCommSend()
// and DriverCommAsyncSend().  When such request comes in, this function
// calls pfnServiceCallback with the parameters given by the driver.  Every
// time the function is called, it will be executing under the context of
// the thread that was waiting for the request.  Therefore, DriverComm will
// serialize all driver requests to be serviced in the same thread.  You can
// call DriverComm::DesignateServiceThreads() to designate extra service
// threads for servicing certain driver requests simultaneously.  Call this
// function only once per DriverComm instance.
//
// Entry:
//      pfnServiceCallback - pointer to a function to service the requests
//                           sent by the driver.
//
// Exit:
//      TRUE if service callback was successfully installed, FALSE if error.
//      If this function failed, use GetLastError() to retrieve error
//      information.
//
//---------------------------------------------------------------------------
BOOL DriverComm::RegisterServiceCallback (
    PDRVCOMMCALLBACK pfnServiceCallback
    )
    {
    auto UINT uThreadAddr;

                                        // Allocate memory for the smallest
                                        // service request packet.  The size
                                        // will be adjusted at run-time.

    dwPacketSize = sizeof(DRVCOMMDIOCTLPACKET);

    prPacket = (PDRVCOMMDIOCTLPACKET) new BYTE[dwPacketSize];

    if ( !prPacket )
        {
        return ( FALSE );
        }

                                        // Save service callback.

    pfnCallback = pfnServiceCallback;

                                        // Create the main service thread.

    hMainServiceThread = (HANDLE) _beginthreadex ( NULL,
                                                   0,
                                                   MainServiceThread,
                                                   this,
                                                   CREATE_SUSPENDED,
                                                   &uThreadAddr );

    if ( !hMainServiceThread )
        {
                                        // Just assume that the thread limit
                                        // of the application/system has been
                                        // reached.  We could be out of
                                        // memory or it could be something
                                        // else, but who cares what the real
                                        // reason for the failure is.

        SetLastError ( ERROR_MAX_THRDS_REACHED );

        return ( FALSE );
        }

                                        // Tell the driver that we want to
                                        // start a thread for servicing its
                                        // user-mode requests.

    while ( !SendIO ( DIOCTL_SYM_INTERNAL_INIT_SERVICES,
                      0,
                      NULL,
                      0,
                      NULL,
                      0,
                      NULL ) )
        {
        if ( GetLastError() != ERROR_DRVCOMM_UNSAFE )
            {
            SYM_ASSERT ( GetLastError() == ERROR_SUCCESS );
            return ( FALSE );
            }
        }

    bServiceCommEstablished = TRUE;

                                        // Now that the driver knows about
                                        // us, allow our thread to enter the
                                        // driver and retrieve packets.

    ResumeThread ( hMainServiceThread );

    return ( TRUE );
    }

//---------------------------------------------------------------------------
//
// BOOL DriverComm::Send (
//      DWORD  dwServiceCode,
//      PVOID  pvInBuffer,
//      DWORD  dwInBufferSize,
//      PVOID  pvOutBuffer,
//      DWORD  dwOutBufferSize,
//      PDWORD pdwBytesReturned
//      );
//
// This function sends a synchronous request to the driver referenced by
// DriverComm.  DriverComm::Send() should be called only after connection
// with a driver has been established through DriverComm::LoadAndOpen() or
// DriverComm::Open().  This function transfers control to the service
// request callback in the driver associated with DriverComm.
//
// Entry:
//      dwServiceCode    - non-0 service request code.
//      pvInBuffer       - pointer to a memory block containing input data
//                         for the driver.  If this value is NULL, then
//                         dwInBufferSize is ignored.
//      dwInBufferSize   - length of memory block at pvInBuffer.
//      pvOutBuffer      - pointer to a memory block where the driver should
//                         copy any data resulting from the servicing of the
//                         request.  If this parameter is NULL, then
//                         dwOutBufferSize and pdwBytesReturned are ignored.
//      dwOutBufferSize  - length of memory block at pvOutBuffer.
//      pdwBytesReturned - pointer to a DWORD to receive the number of
//                         bytes copied to pvOutBuffer when the call returns.
//                         This parameter should not be NULL if pvOutBuffer
//                         is supplied.
//
// Exit:
//      TRUE if call was successful or FALSE if DriverComm::Send() failed to
//      call driver's service callback.  Use GetLastError() to determine the
//      reason for the failure.
//
//---------------------------------------------------------------------------

//
// This is an inline function.
//

//---------------------------------------------------------------------------
//
// BOOL DriverComm::SendIO (
//      DWORD  dwIoControlCode,
//      DWORD  dwServiceCode,
//      PVOID  pvInBuffer,
//      DWORD  dwInBufferSize,
//      PVOID  pvOutBuffer,
//      DWORD  dwOutBufferSize,
//      PDWORD pdwBytesReturned
//      );
//
// This function sends a synchronous request to the driver referenced by
// DriverComm.  DriverComm::SendIO() should be called only after connection
// with a driver has been established through DriverComm::LoadAndOpen() or
// DriverComm::Open().  This function transfers control to the service
// request callback in the driver associated with DriverComm if
// dwIOControlCode is DIOCTL_SYM_INTERNAL_SERVICE_REQUEST.  All other values
// for dwIoControlCode are used internally.
//
// Entry:
//      dwIoControlCode  - one of DIOCTL_SYM_INTERNAL_??? codes.
//      dwServiceCode    - non-0 service request code, used together with
//                         DIOCTL_SYM_INTERNAL_SERVICE_REQUEST.
//      pvInBuffer       - pointer to a memory block containing input data
//                         for the driver.  If this value is NULL, then
//                         dwInBufferSize is ignored.
//      dwInBufferSize   - length of memory block at pvInBuffer.
//      pvOutBuffer      - pointer to a memory block where the driver should
//                         copy any data resulting from servicing the
//                         request.  If this parameter is NULL, then
//                         dwOutBufferSize and pdwBytesReturned are ignored.
//      dwOutBufferSize  - length of memory block at pvOutBuffer.
//      pdwBytesReturned - pointer to a DWORD to receive the number of
//                         bytes copied to pvOutBuffer when the call returns.
//                         This parameter should not be NULL if pvOutBuffer
//                         is supplied.
//
// Exit:
//      TRUE if call was successful or FALSE if DriverComm::SendIO() failed
//      to call the driver, or if the driver failed to process the request.
//      Use GetLastError() to find out why the function failed.
//
//      If GetLastError() returned ERROR_DRVCOMM_UNSAFE, the client should
//      reissue the call.  Chances are there is another client messing
//      around with the driver.
//
//---------------------------------------------------------------------------
BOOL DriverComm::SendIO (
    DWORD  dwIoControlCode,
    DWORD  dwServiceCode,
    PVOID  pvInBuffer,
    DWORD  dwInBufferSize,
    PVOID  pvOutBuffer,
    DWORD  dwOutBufferSize,
    PDWORD pdwBytesReturned
    )
    {
    auto BOOL         bResult;
    auto HANDLE       hSyncComm;
    auto DRVCOMM_DIOC rSendPacket;
    auto DWORD        dwLastError;
    auto DWORD        dwBytesReturned;

                                        // Fix up the entry parameters,
                                        // and perform parameter validation.

    if ( !pvInBuffer )
        {
        dwInBufferSize = 0;
        }
    else if ( !dwInBufferSize )
        {
        SetLastError ( ERROR_DRVCOMM_BAD_ARGUMENT );

        return ( FALSE );
        }

    if ( !pvOutBuffer )
        {
        dwOutBufferSize = 0;
        pdwBytesReturned = NULL;
        }
    else
        {
        if ( !dwOutBufferSize || !pdwBytesReturned )
            {
            SetLastError ( ERROR_DRVCOMM_BAD_ARGUMENT );

            return ( FALSE );
            }
        }

#define RESERVED_FOR_WIN95_DIOC_GETVERSION   0
#define RESERVED_FOR_WIN95_DIOC_CLOSEHANDLE -1

    SYM_ASSERT ( dwIoControlCode != RESERVED_FOR_WIN95_DIOC_GETVERSION &&
                 dwIoControlCode != RESERVED_FOR_WIN95_DIOC_CLOSEHANDLE );

                                        // Make sure we have name of a driver
                                        // to open.

    if ( !pszDriverName )
        {
        SetLastError ( ERROR_DRVCOMM_NOT_INITIALIZED );

        return ( FALSE );
        }

                                        // Open communication handle to the
                                        // driver.

    switch ( DrvCommGetWindowsType() )
        {
        case DRVCOMM_WIN_95:

            hSyncComm = CreateFile ( pszDriverName,
                                     0,
                                     0,
                                     NULL,
                                     0,
                                     FILE_FLAG_DELETE_ON_CLOSE,
                                     NULL );

            break;

        case DRVCOMM_WIN_NT:

            hSyncComm = CreateFile ( pszDriverName,
                                     GENERIC_READ | GENERIC_WRITE,
                                     FILE_SHARE_READ | FILE_SHARE_WRITE,
                                     NULL,
                                     OPEN_EXISTING,
                                     0,
                                     NULL );

            break;

        default:

            SetLastError ( ERROR_DRVCOMM_NOT_IMPLEMENTED );

            hSyncComm = INVALID_HANDLE_VALUE;
        }

    if ( hSyncComm == INVALID_HANDLE_VALUE )
        {
        return ( FALSE );
        }

                                        // Build a packet to send to the
                                        // driver.

    rSendPacket.dwTag1            = '>>SR';
    rSendPacket.dwServiceCode     = dwServiceCode;
    rSendPacket.lpvInBuffer       = (DWORD)pvInBuffer;
    rSendPacket.cbInBuffer        = dwInBufferSize;
    rSendPacket.lpvOutBuffer      = (DWORD)pvOutBuffer;
    rSendPacket.cbOutBuffer       = dwOutBufferSize;
    rSendPacket.lpcbBytesReturned = (DWORD)pdwBytesReturned;
    rSendPacket.dwTag2            = '<<SR';

                                        // Call the driver.

    bResult = DeviceIoControl ( hSyncComm,
                                dwIoControlCode,
                                &rSendPacket,
                                sizeof(rSendPacket),
                                &dwLastError,
                                sizeof(dwLastError),
                                &dwBytesReturned,
                                NULL );

                                        // Save last error, because
                                        // CloseHandle() may change it.

    if ( !bResult )
        {
        dwLastError = GetLastError();
        }

                                        // Close the communication handle.
                                        // Don't report to the client if
                                        // it failed; that's not important.

    if ( !CloseHandle ( hSyncComm ) )
        {
        SYM_ASSERT ( FALSE );
        }

                                        // Process result from the DIOC call.

    if ( bResult )
        {
        if ( dwBytesReturned == sizeof(dwLastError) )
            {
                                        // Driver returned proper error code.

            if ( dwLastError != ERROR_DRVCOMM_SUCCESS )
                {
                SetLastError ( dwLastError );

                bResult = FALSE;
                }
            }
        else
            {
                                        // Driver did not return the expected
                                        // error code.

            SetLastError ( ERROR_DRVCOMM_NOT_IMPLEMENTED );

            bResult = FALSE;
            }
        }
    else
        {
        SetLastError ( dwLastError );
        }

                                        // Return the status of DIOCTL call.

    return ( bResult );
    }

//---------------------------------------------------------------------------
//
// static unsigned __stdcall MainServiceThread ( void *pvArgList );
//
// This function contains the code for the main service thread of a
// DriverComm class.  This thread is created when the client calls
// DriverComm::RegisterServiceCallback() to start servicing device requests.
// Most of the time this thread will be blocked by the driver.  Whenever
// the driver needs to send a packet due to calls to DriverCommSend() and
// DriverCommAsyncSend(), this thread will be released and it will service
// the packet.  First, it will check if there are any designated service
// threads to process the request in the packet.  If there are, then this
// function will schedule the packet to be serviced by the appropriate
// DesignatedThreads class.  If there are no designated service threads,
// then this function will process the packet right away under the context
// of the main service thread.
//
// Entry:
//      pvArgList - pointer to the DriverComm class whose requests this
//                  thread is servicing.
//
// Exit:
//      This function never returns.  It terminates by calling _endthreadex()
//      with thread exit code 0.
//
//---------------------------------------------------------------------------
static unsigned __stdcall MainServiceThread ( void *pvArgList )
    {
    auto DWORD dwBytesReturned;

                                        // Associate pDCC with the class
                                        // that this thread belongs to.  This
                                        // will make it easier for us to
                                        // access the DriverComm class
                                        // members.

    #define pDCC ((DriverComm*)pvArgList)

                                        // Make sure that our destructor
                                        // knows we have been resumed,
                                        // therefore, we could be blocked by
                                        // the driver.

    pDCC->bMainServiceThreadResumed = TRUE;

                                        // Start a big loop to retrieve
                                        // packets from driver.

    while ( !pDCC->bTerminateMainServiceThread )
        {
        SYM_ASSERT ( pDCC->prPacket );
        SYM_ASSERT ( pDCC->dwPacketSize );

                                        // Get a packet from the driver.
                                        // It is driver's responsibility
                                        // to block this thread if there is
                                        // nothing waiting, unless the driver
                                        // is told not to block.  In this
                                        // case, the driver should return 0
                                        // bytes in dwBytesReturned.

try_dioctl_again:

        if ( !pDCC->SendIO ( DIOCTL_SYM_INTERNAL_GET_DRVCOMMDIOCTL_PACKET,
                             0,
                             NULL,
                             0,
                             pDCC->prPacket,
                             pDCC->dwPacketSize,
                             &dwBytesReturned )
           )
            {
                                        // If the call failed, because the
                                        // driver wasn't called properly,
                                        // then wait a little and keep
                                        // issuing the DIOCTL call.

            if ( GetLastError() == ERROR_DRVCOMM_UNSAFE )
                {
                Sleep ( 100 );
                goto try_dioctl_again;
                }

                                        // If the call failed due to a very
                                        // small packet size, then increase
                                        // the packet size and reissue the
                                        // request.

            if ( GetLastError() == ERROR_DRVCOMM_BUFFER_OVERFLOW )
                {
                auto PVOID pvNewPacket;

                                        // The required packet size is
                                        // placed by the driver in
                                        // dwBytesReturned.  Validate its
                                        // value.

                if ( dwBytesReturned <= pDCC->dwPacketSize )
                    {
                                        // ERROR_DRVCOMM_BUFFER_OVERFLOW was
                                        // NOT set by our driver ?!?!?!?!

                    SYM_ASSERT ( dwBytesReturned > pDCC->dwPacketSize );

                    break;
                    }

                                        // Allocate new packet and update
                                        // the DriverComm class.  If the new
                                        // packet allocation failed, retry
                                        // the call.  If after the resize
                                        // request the driver still can't
                                        // fit the packet, then it will
                                        // cancel it.

                pvNewPacket = new BYTE[dwBytesReturned];

                SYM_ASSERT ( pvNewPacket );

                if ( pvNewPacket )
                    {
                    delete [](PBYTE)pDCC->prPacket;

                    pDCC->dwPacketSize = dwBytesReturned;

                    pDCC->prPacket = (PDRVCOMMDIOCTLPACKET) pvNewPacket;
                    }

                                        // Try to get the packet again.

                continue;
                }

                                        // If the call failed, because of
                                        // some other error, then wait
                                        // a second and try to ask for
                                        // packets again.

            SYM_ASSERT ( GetLastError() == ERROR_SUCCESS );
            Sleep ( 1000 );
            continue;
            }

                                        // If there were no packets, then
                                        // the driver was probably told to
                                        // unblock the thread, or the
                                        // semaphore on which the thread
                                        // should be blocked was in signaled
                                        // state, even without packets in the
                                        // queue.

        if ( !dwBytesReturned )
            {
            continue;
            }

                                        // Verify the returned value.  Check
                                        // the length of pointers vs sizeof
                                        // DWORDs.  Read the comments for
                                        // DRVCOMMDIOCTLPACKET.

        SYM_ASSERT ( sizeof(DRVCOMMDIOCTLPACKET*) == sizeof(DWORD) );

        if ( dwBytesReturned < sizeof(*pDCC->prPacket) - sizeof(pDCC->prPacket->abyData) ||
             dwBytesReturned > pDCC->dwPacketSize )
            {
                                        // Our driver would've never copied
                                        // too little or too many bytes to
                                        // the packet buffer.

            SYM_ASSERT ( dwBytesReturned >= sizeof(*pDCC->prPacket) - sizeof(pDCC->prPacket->abyData) );
            SYM_ASSERT ( dwBytesReturned <= pDCC->dwPacketSize );
            continue;
            }

                                        // If we are here, then most likely
                                        // we have a service request packet.

        SYM_ASSERT ( pDCC->prPacket->dwServiceCode );

        if ( !pDCC->prPacket->dwServiceCode )
            {
            continue;
            }

                                        // See if a DesignatedThreads class
                                        // will take care of this packet.

        auto DesignatedThreads *pDTC;

        for ( pDTC = (DesignatedThreads *)pDCC->pvDesignatedThreads;
              pDTC && !pDTC->ServicePacket ( pDCC->prPacket );
              pDTC = pDTC->Next() );

                                        // If the packet was not processed
                                        // already, then do it on our own.

        if ( !pDTC )
            {
            ImmediateServicePacket ( pDCC->pfnCallback,
                                     pDCC->prPacket );
            }
        }

                                        // We don't need pDCC anymore.

    #undef pDCC
                                        // MS suggests, that we terminate
                                        // the thread this way.

    _endthreadex ( 0 );

    return ( 0 );
    }

//---------------------------------------------------------------------------
//
// DesignatedThreads::DesignatedThreads()
//
// This is the constructor for DesignatedThreads class.  It sets all members
// of the newly allocated class to uninitialized status, except the critical
// section for managing the packet list.  Since the critical section init/
// delete APIs don't return any values we can work with them in the
// constructor/destructor.
//
//---------------------------------------------------------------------------
DesignatedThreads::DesignatedThreads()
    {
                                        // Clear all data in the class.

    memset ( this, 0, sizeof(*this) );

                                        // Initialize critical section for
                                        // managing the linked list with
                                        // packets.

    InitializeCriticalSection ( &rPacketListSync );
    }

//---------------------------------------------------------------------------
//
// DesignatedThreads::~DesignatedThreads()
//
// This is the destructor for DesignatedThreads class.  It releases all
// resources allocated for the class.  If there were any threads created,
// this funciton will wait for them to terminate before completing.
//
//---------------------------------------------------------------------------
DesignatedThreads::~DesignatedThreads()
    {
    auto int iCount;

                                        // If we have a semaphore, then
                                        // we should have an array with
                                        // threads to terminate.

    if ( hSemaphore )
        {
        SYM_ASSERT ( phThreads );

                                        // See if there are any threads
                                        // initialized, and release all of
                                        // them.  They will eventually
                                        // terminate because there will be
                                        // no packets to process.

        iCount = iThreads;

        while ( iCount-- )
            {
            if ( phThreads[iCount] )
                {
                                        // This will hang the application if
                                        // the thread is not responding,
                                        // but if we keep on going the
                                        // results will be worse.

                while ( ! ReleaseSemaphore ( hSemaphore, 1, NULL ) )
                    {
                    SYM_ASSERT ( GetLastError() == ERROR_SUCCESS );
                    }
                }
            }

                                        // Now that we have released all
                                        // threads, wait for them to
                                        // terminate, then close their
                                        // handles.

        iCount = iThreads;

        while ( iCount-- )
            {
            if ( phThreads[iCount] )
                {
                while ( WaitForSingleObject ( phThreads[iCount], INFINITE ) !=
                        WAIT_OBJECT_0 )
                    {
                    SYM_ASSERT ( FALSE );
                    }

                if ( !CloseHandle ( phThreads[iCount] ) )
                    {
                    SYM_ASSERT ( GetLastError() == ERROR_SUCCESS );
                    }
                }
            }

                                        // Delete the semaphore.

        if ( !CloseHandle ( hSemaphore ) )
            {
            SYM_ASSERT ( GetLastError() == ERROR_SUCCESS );
            }
        }

                                        // Release memory for thread handles.

    if ( phThreads )
        {
        delete []phThreads;
        }

                                        // Release memory for code array.

    if ( pdwCodes )
        {
        delete []pdwCodes;
        }

                                        // Delete the critical section.

    DeleteCriticalSection ( &rPacketListSync );
    }

//---------------------------------------------------------------------------
//
// BOOL DesignatedThreads::Create (
//      PDWORD           pdwServiceCodeArray,
//      int              iNumberOfThreads,
//      PDRVCOMMCALLBACK pfnDesignatedServiceCallback
//      );
//
// This function initializes all threads and resources necessary for a
// DesignatedThreads class to be put to work for a DriverComm class.  The
// threads will be put to sleep until a packet is added for processing.
// The more packets are added for processing, the more threads will be
// released to process the packets.
//
// Entry:
//      pdwServiceCodeArray          - a 0-terminated array of service codes
//                                     which will be serviced by the threads
//                                     of this class.
//      iNumberOfThreads             - number of threads to designate to
//                                     servicing the requests specified in
//                                     pdwServiceCodeArray.
//      pfnDesignatedServiceCallback - pointer to a function which will
//                                     service all requests specified in
//                                     pdwServiceCodeArray.
//
// Exit:
//      TRUE if successful, FALSE if error.  If this function failed, use
//      GetLastError() to retrieve extra error information.
//
//---------------------------------------------------------------------------
BOOL DesignatedThreads::Create (
    PDWORD           pdwServiceCodeArray,
    int              iNumberOfThreads,
    PDRVCOMMCALLBACK pfnDesignatedServiceCallback
    )
    {
    auto UINT uThreadAddr;

                                        // Validate input parameters.

    SYM_ASSERT ( pdwServiceCodeArray );
    SYM_ASSERT ( pdwServiceCodeArray[0] );
    SYM_ASSERT ( iNumberOfThreads > 0 );
    SYM_ASSERT ( pfnDesignatedServiceCallback );

    if ( !pdwServiceCodeArray ||
         !pdwServiceCodeArray[0] ||
         iNumberOfThreads <= 0 ||
         !pfnDesignatedServiceCallback )
        {
        SetLastError ( ERROR_DRVCOMM_BAD_ARGUMENT );

        return ( FALSE );
        }

                                        // Save the designated service
                                        // callback.

    pfnCallback = pfnDesignatedServiceCallback;

                                        // Count the number of codes and
                                        // allocate memory to save them.

    for ( iCodes = 0; pdwServiceCodeArray[iCodes]; iCodes++ );

    pdwCodes = new DWORD[iCodes];

    if ( !pdwCodes )
        {
        return ( FALSE );
        }

    memcpy ( pdwCodes, pdwServiceCodeArray, iCodes * sizeof(*pdwCodes) );

                                        // Get the number of threads and
                                        // allocate memory for the thread
                                        // handles.

    iThreads = iNumberOfThreads;

    phThreads = new HANDLE[iThreads];

    if ( !phThreads )
        {
        return ( FALSE );
        }

    ZeroMemory ( phThreads, iThreads * sizeof(*phThreads) );

                                        // Allocate semaphore for blocking
                                        // unused threads.  The assertion
                                        // below is because we hard-code the
                                        // value 0x7FFFFFFF as the greatest
                                        // int possible.

    SYM_ASSERT ( sizeof(int) == sizeof(DWORD) );

    hSemaphore = CreateSemaphore ( NULL, 0, 0x7FFFFFFF, NULL );

    if ( !hSemaphore )
        {
        return ( FALSE );
        }

                                        // Create threads and start them up.

    auto int iCount;

    for ( iCount = 0; iCount < iThreads; iCount++ )
        {
        phThreads[iCount] = (HANDLE) _beginthreadex ( NULL,
                                                      0,
                                                      DesignatedServiceThread,
                                                      this,
                                                      0,
                                                      &uThreadAddr );

        if ( !phThreads[iCount] )
            {
                                        // Just assume that the thread limit
                                        // of the application/system has been
                                        // reached.  We could be out of
                                        // memory or it could be something
                                        // else, but who cares what the real
                                        // reason for the failure is.

            SetLastError ( ERROR_MAX_THRDS_REACHED );

            return ( FALSE );
            }
        }

    return ( TRUE );
    }

//---------------------------------------------------------------------------
//
// DesignatedThreads* DesignatedThreads::Link (
//      DesignatedThreads* pListStart
//      );
//
// This function links the current DesignatedThreads class to a linked list
// of DesignatedThreads classes.
//
// Entry:
//      pListStart - pointer to the first member of the linked list; NULL
//                   if the linked list is empty.
//
// Exit:
//      This function returns a pointer to the new first member of the
//      linked list.  This value should basically be passed as pListStart
//      when the next DesignatedThreads class is being linked to the list.
//
//---------------------------------------------------------------------------

//
// This is an inline function.
//

//---------------------------------------------------------------------------
//
// DesignatedThreads* DesignatedThreads::Next (
//      void
//      );
//
// This function returns a pointer to the next DesignatedThreads class in
// a linked list of DesignatedThreads classes.
//
// Entry:
//      -
//
// Exit:
//      This function returns a pointer to the next class in the list.
//
//---------------------------------------------------------------------------

//
// This is an inline function.
//

//---------------------------------------------------------------------------
//
// BOOL DesignatedThreads::ServicePacket (
//      PDRVCOMMDIOCTLPACKET prPacket
//      );
//
// This function checks if the packet at prPacket can be serviced by the
// current DesignatedThreads class.  If it can be processed, then the
// packet is queued for the next available designated thread to pick it
// up and munch it.  Before queuing, this function will create a copy of
// prPacket, thus prPacket itself can be reused or destroyed upon return
// from this function.
//
// Entry:
//      prPacket - service request packet sent to us by a driver.
//
// Exit:
//      TRUE if packet processed, FALSE if the current class does not want
//      to process this packet, or if error.
//
//      If this function returned FALSE, use GetLastError() to determine the
//      reason for the failure.  If GetLastError() reports ERROR_SUCCESS, it
//      only means that the class was not designated to service prPacket.
//
//---------------------------------------------------------------------------
BOOL DesignatedThreads::ServicePacket (
    PDRVCOMMDIOCTLPACKET prPacket
    )
    {
    auto int iCount;

                                        // Validate input parameters.

    SYM_ASSERT ( prPacket );
    SYM_VERIFY_BUFFER ( prPacket, sizeof(*prPacket) );
    SYM_ASSERT ( iCodes );
    SYM_ASSERT ( pdwCodes );
    SYM_ASSERT ( hSemaphore );

                                        // Check if the current class is
                                        // designated to process this packet.

    for ( iCount = iCodes;
          --iCount >= 0 && pdwCodes[iCount] != prPacket->dwServiceCode;
        );

                                        // If so, then queue the packet and
                                        // signal the designated threads to
                                        // pick it up.

    if ( iCount >= 0 )
        {
        auto PDRVCOMMDIOCTLPACKET prNewPacket;

                                        // Allocate memory for new packet -
                                        // we can't hold on to caller's
                                        // buffer forever.

        prNewPacket = (PDRVCOMMDIOCTLPACKET) new BYTE[prPacket->dwSize];

        if ( prNewPacket )
            {
                                        // Create a copy of the packet.

            memcpy ( prNewPacket, prPacket, prPacket->dwSize );

                                        // Add the packet to the tail of
                                        // the linked list.

            prNewPacket->pNext = NULL;

            EnterCriticalSection ( &rPacketListSync );

            if ( prPacketListHead )
                {
                                        // List is not empty - make the
                                        // packet the tail of the list.

                prPacketListTail->pNext = prNewPacket;

                prPacketListTail = prNewPacket;
                }
            else
                {
                                        // List is empty - make the packet
                                        // both head and tail of the list.

                prPacketListHead = prPacketListTail = prNewPacket;
                }

            LeaveCriticalSection ( &rPacketListSync );

                                        // Signal the designated service
                                        // threads that one of them can
                                        // start working.  This will hang
                                        // the application if the thread is
                                        // not responding, but if we keep on
                                        // going the results will be worse.

            while ( ! ReleaseSemaphore ( hSemaphore, 1, NULL ) )
                {
                SYM_ASSERT ( GetLastError() == ERROR_SUCCESS );
                }

            return ( TRUE );
            }
        }
    else
        {
                                        // This class is not designated to
                                        // service prPacket.  Set the last
                                        // error code ERROR_SUCCESS, but
                                        // still fail the function.

        SetLastError ( ERROR_SUCCESS );
        }

    return ( FALSE );
    }

//---------------------------------------------------------------------------
//
// static unsigned __stdcall DesignatedServiceThread ( void *pvArgList )
//
// This function contains the code for designated service threads.  Each
// service thread is owned by a DesignatedThreads class.  When
// MainServiceThread() determines that a packet has to be serviced by a
// particular DesignatedThreads class, and it tells the class to process it,
// DesignatedThreads::ServicePacket() adds the packet to a linked list and
// releases one of its designated service threads.  Then the designated
// service thread removes the packet from the queue, and processes it.
//
// Entry:
//      pvArgList - pointer to the DesignatedThreads class associated with
//                  the current thread.
//
// Exit:
//      This function never returns.  It terminates by calling _endthreadex()
//      with thread exit code 0.
//
//---------------------------------------------------------------------------
static unsigned __stdcall DesignatedServiceThread ( void *pvArgList )
    {
    auto PDRVCOMMDIOCTLPACKET prPacket;

                                        // Associate pDTC with the class
                                        // that this thread belongs to.  This
                                        // will make it easier for us to
                                        // access the DesignatedThreads
                                        // class members.

    #define pDTC ((DesignatedThreads*)pvArgList)

                                        // Wait for a signal to either
                                        // process a packet, or to terminate
                                        // the thread.

wait_for_packet:

    while ( WaitForSingleObject ( pDTC->hSemaphore, INFINITE ) != WAIT_OBJECT_0 )
        {
        SYM_ASSERT ( FALSE );
        }

                                        // Check if we have a packet to play
                                        // with.  If we don't, this means
                                        // that we were just told to
                                        // terminate.  If we have a packet,
                                        // then process it.

    EnterCriticalSection ( &pDTC->rPacketListSync );

    if ( prPacket = pDTC->prPacketListHead )
        {
                                        // We have a packet.  First, remove
                                        // it from the head of the list.
                                        // Note, that there is no need for
                                        // us to NULL-out the tail of the
                                        // list if this is the last packet -
                                        // the code that adds to it assumes
                                        // that the tail is not valid if the
                                        // head is NULL.

        pDTC->prPacketListHead = prPacket->pNext;

        LeaveCriticalSection ( &pDTC->rPacketListSync );

                                        // Now that the packet has been
                                        // removed, process it.

        ImmediateServicePacket ( pDTC->pfnCallback, prPacket );

                                        // Free the memory allocated for the
                                        // packet.  This memory was allocated
                                        // in DesignatedThreads::ServicePacket()

        delete [](PBYTE)prPacket;

                                        // Wait for next packet.

        goto wait_for_packet;
        }

                                        // We don't have a packet.

    LeaveCriticalSection ( &pDTC->rPacketListSync );

                                        // We don't need pDTC anymore.

    #undef pDTC
                                        // MS suggests, that we terminate
                                        // the thread this way.

    _endthreadex ( 0 );

    return ( 0 );
    }

//---------------------------------------------------------------------------
//
// static void ImmediateServicePacket (
//      PDRVCOMMCALLBACK     pfnCallback,
//      PDRVCOMMDIOCTLPACKET prPacket
//      );
//
// This function services a request packet sent to us by a kernel mode
// driver.  It should be called under the context of a service thread
// (whether the main DriverComm thread, or a designated service thread).
// The function will call pfnCallback with the parameters at prPacket.
// Currently only asynchronous prPacket types are supported.  This means
// that as soon as pfnCallback returns, ImmediateServicePacket() also
// returns.  No status is reported back to the driver.
//
// Entry:
//      pfnCallback - pointer to a DriverComm callback to service prPacket
//      prPacket    - service packet sent to us by the driver
//
// Exit:
//      pfnCallback executed.
//
//---------------------------------------------------------------------------
static void ImmediateServicePacket (
    PDRVCOMMCALLBACK     pfnCallback,
    PDRVCOMMDIOCTLPACKET prPacket
    )
    {
                                        // Validate input parameters.

    SYM_ASSERT ( pfnCallback );
    SYM_ASSERT ( prPacket );

                                        // Only DriverCommAsyncSend() packets
                                        // are currently supported - they
                                        // do not need us to respond back to
                                        // the driver with status of the
                                        // processing.

    pfnCallback ( prPacket->dwServiceCode,
                  prPacket->dwInBufferSize ? &prPacket->abyData[0] : NULL,
                  prPacket->dwInBufferSize,
                  NULL,
                  0,
                  NULL );
    }

//---------------------------------------------------------------------------
//
// static int DrvCommGetWindowsType (
//      VOID
//      );
//
// This function returns the current Windows type.
//
// Entry:
//      -
//
// Exit:
//      DRVCOMM_WIN_NT if running under Windows NT
//      DRVCOMM_WIN_95 if running under Windows 95
//      DRVCOMM_WIN_32S if running under Windows 3.x with Win32 subsystem.
//
//---------------------------------------------------------------------------
static int DrvCommGetWindowsType (
    VOID
    )
    {
    auto DWORD dwVersion;

    dwVersion = GetVersion();

    if ( dwVersion < 0x80000000 )
        {
        return ( DRVCOMM_WIN_NT );
        }
    else if ( LOBYTE ( LOWORD ( dwVersion ) ) < 4 )
        {
        return ( DRVCOMM_WIN_32S );
        }
    else
        return ( DRVCOMM_WIN_95 );
    }
