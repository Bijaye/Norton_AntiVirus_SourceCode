// Copyright 1997 Symantec, Peter Norton Product Group
//***************************************************************************
//
// $Header:   S:/NAVSDK/VCS/context.cpv   1.1   19 Mar 1998 11:43:18   DALLEE  $
//
// Description:
//
//      This file contains routines for managing the per thread data
//      NAVSDK keeps for scanning context information.
//
//      Currently, context information is not provided in all the engine
//      callbacks where NAVSDK requires it -- data file location, and
//      configuration queries. In addition, the (admittedly old) AVAPI
//      interface does not accept user context information, so there is no
//      way to pass this information to AVENGE or NAVEX in any case.
//
//      NAVSDK works around this problem by allocating context information
//      on a per thread basis. On entry to each exported API, the
//      context information is copied into a per thread allocated block
//      of memory. Each thread will have a copy of the global context
//      information for a scan handle, but will have differing data
//      for last virus found.
//
// Contains:
//    NAVSDK internal:
//      AllocScanContextStorage()
//      FreeScanContextStorage()
//      GetScanContext()
//      SetScanContext()
//      SynchronizeScanContext()
//
//***************************************************************************
// $Log:   S:/NAVSDK/VCS/context.cpv  $
// 
//    Rev 1.1   19 Mar 1998 11:43:18   DALLEE
// Enable use of same scanning handle among multiple threads.
// Now saving per thread copies of oem context information, not
// just a pointer to original info.
// This allows storage of unique last virus found info.
// 
//    Rev 1.0   04 Dec 1997 16:15:20   DALLEE
// Initial revision.
//***************************************************************************

#include "platform.h"
#include "avapi.h"
#include "navsdk.h"
#include "oemapi.h"

//***********************************************************************
// Static Data
//***********************************************************************
static  DWORD   s_dwScanContextTls = 0xFFFFFFFF;


/*@API:**********************************************************************
@Declaration:
    BOOL WINAPI AllocScanContextStorage()

@Description:
    This routine allocates a thread local storage for storing per scan
    context information. 
    This must be called once before any possible access to VirusLibraryInit(). 
    Should be called from DllMain process attach.
    FreeScanContextStorage() should be called before program exit.

@Parameters:

@Returns:
    If successful, TRUE, otherwise, FALSE.

@See:
    FreeScanContextStorage()
    GetScanContext()
    SetScanContext()

@Include:
    oemapi.h

@Compatibility: W32
****************************************************************************/
BOOL WINAPI AllocScanContextStorage ( )
{
    SYM_ASSERT( 0xFFFFFFFF == s_dwScanContextTls );

    s_dwScanContextTls = TlsAlloc();
    SetScanContext( NULL );

    return ( 0xFFFFFFFF != s_dwScanContextTls );
} // AllocScanContextStorage()


/*@API:**********************************************************************
@Declaration:
    BOOL WINAPI FreeScanContextStorage()

@Description:
    This routine frees the thread local storage used for storing per scan
    context information. 
    This must be called all possible access to VirusLibrary functions.
    Should be called from DllMain process detach.

@Parameters:

@Returns:
    If successful, TRUE, otherwise, FALSE.

@See:
    AllocScanContextStorage()
    GetScanContext()
    SetScanContext()

@Include:
    oemapi.h

@Compatibility: W32
****************************************************************************/
BOOL WINAPI FreeScanContextStorage ( )
{
    auto    BOOL    bSuccess;

    if ( 0xFFFFFFFF != s_dwScanContextTls )
    {
        bSuccess = TlsFree( s_dwScanContextTls );
        s_dwScanContextTls = 0xFFFFFFFF;
    }
    else
    {
        bSuccess = TRUE;
    }

    return ( bSuccess );
} // FreeScanContextStorage()


/*@API:**********************************************************************
@Declaration:
    LPOEMCONTEXT WINAPI GetScanContext()

@Description:
    This routine returns the pointer to the per thread (per scan) 
    OEM context information stored by a prior call to SetScanContext().
    This makes per scan information available during engine callbacks
    where the engine does not provide this.

@Parameters:

@Returns:
    If successful, pointer to OEM Context.
    Otherwise, returns NULL.

@See:
    AllocScanContextStorage()
    FreeScanContextStorage()
    SetScanContext()

@Include:
    oemapi.h

@Compatibility: W32
****************************************************************************/
LPOEMCONTEXT WINAPI GetScanContext ( )
{
    return ( 0xFFFFFFFF != s_dwScanContextTls ?
                (LPOEMCONTEXT)TlsGetValue( s_dwScanContextTls ) : NULL );
} // GetScanContext()


/*@API:**********************************************************************
@Declaration:
    BOOL WINAPI SetScanContext(
        LPOEMCONTEXT    lpOEMContext )

@Description:
    This routine sets the stored pointer to the per thread (per scan) 
    OEM context information.
    This makes per scan information available during engine callbacks
    where the engine does not provide this.

@Parameters:

@Returns:
    If successful, TRUE.
    Otherwise, returns FALSE.

@See:
    AllocScanContextStorage()
    FreeScanContextStorage()
    GetScanContext()

@Include:
    oemapi.h

@Compatibility: W32
****************************************************************************/
BOOL WINAPI SetScanContext ( 
    LPOEMCONTEXT    lpOEMContext )
{
    return ( 0xFFFFFFFF != s_dwScanContextTls ?
                TlsSetValue( s_dwScanContextTls, lpOEMContext ) : FALSE );
} // SetScanContext()


/*@API:**********************************************************************
@Declaration:
    BOOL WINAPI SynchronizeScanContext(
        LPOEMCONTEXT    lpOEMContext )

@Description:
    This routine ensures that a block has been allocated in this thread to
    track context data, then copies the context information passed in the
    lpOEMContext parameter.
    This makes per scan information available during engine callbacks
    where the engine does not provide this.

@Parameters:

@Returns:
    If successful, TRUE.
    Otherwise, returns FALSE.

@See:
    AllocScanContextStorage()
    FreeScanContextStorage()
    GetScanContext()

@Include:
    oemapi.h

@Compatibility: W32
****************************************************************************/
BOOL WINAPI SynchronizeScanContext ( 
    LPOEMCONTEXT    lpOEMContext )
{
    auto    LPOEMCONTEXT    lpThreadContext;
    auto    BOOL            bSuccess;

    bSuccess = FALSE;

    lpThreadContext = GetScanContext();

    // If no current allocated context, attempt to allocate one.

    if ( NULL == lpThreadContext )
    {
        lpThreadContext = (LPOEMCONTEXT)MemAllocPtr( GHND, sizeof( OEMVIRUSCONTEXT ) );

        if ( NULL != lpThreadContext )
        {
            if ( FALSE == SetScanContext( lpThreadContext ) )
            {
                MemFreePtr( lpThreadContext );
                lpThreadContext = NULL;
            }
        }
    }

    if ( NULL != lpThreadContext )
    {
        if ( lpThreadContext->hContext != lpOEMContext->hContext )
        {
            // If we're changing contexts, need to copy information.
            
            MEMCPY( lpThreadContext, lpOEMContext, sizeof( *lpOEMContext ) );
        }

        bSuccess = TRUE;
    }

    return ( bSuccess );
} // SynchrnoizeScanContext()

