// Copyright 1996-1997 Symantec, Peter Norton Product Group
//************************************************************************
//
// $Header:   S:/NAVW/VCS/navwhmem.cpv   1.5   15 Jul 1997 14:12:26   DBUCHES  $
//
// Description:
//      This file contains functions which process the /HMEM command-line
//      switch, which is only used when NAV is launched by the Navigator
//      File Manager.
//
//      While running Navigator, the user selects a ton of files in
//      File Manager, and then uses the right-click menu to scan these
//      files. File Manager creates a memory-mapped file containing the
//      names of the selected files, and launches NAV with /HMEM=<x>,
//      where <x> is the handle to the memory-mapped file.
//
// Contains (as exports):
//      CatchMemoryFile, CatchMemoryFile_Cleanup
//
// See Also:
//      Navwmcmd.cpp
//************************************************************************
// $Log:   S:/NAVW/VCS/navwhmem.cpv  $
// 
//    Rev 1.5   15 Jul 1997 14:12:26   DBUCHES
// Added Ctl3d for DEC Alpha
// 
//    Rev 1.4   09 Jun 1997 16:54:46   DBUCHES
// Ifdef'd out Ctl3d stuff for non-intel platfomrs.
// 
//    Rev 1.3   16 May 1997 17:24:06   DALLEE
// Removed conflicting TRACE defines now that we're define SYM_MFC.
// TRACE seemed unused here.
// Removed references to "Netw.h" NAVNET-specific header file.
// 
//    Rev 1.2   09 Apr 1997 15:18:54   TIVANOV
// remove #ifdef NAVSCANW from the code
// to support the right mouse click shell integration
//
//    Rev 1.1   06 Mar 1997 17:20:42   DALLEE
// Removed network and password stub references.
// Related to HYDRA.15 port: no longer required.
//
//    Rev 1.0   06 Feb 1997 21:01:32   RFULLER
// Initial revision
//
//    Rev 1.1   16 Apr 1996 15:05:32   YVANG
// Ported from Kirin.
//
//    Rev 1.1   28 Mar 1996 14:21:46   jworden
// DBCS Enable
//
//    Rev 1.0   30 Jan 1996 15:55:08   BARRY
// Initial revision.
//
//    Rev 1.0   30 Jan 1996 14:50:12   BARRY
// Initial revision.
//
//    Rev 1.2   14 Jan 1996 17:05:08   DALLEE
// Changed #ifdef NAVWNT to NAVSCANW.
//
//    Rev 1.1   22 Dec 1995 17:30:50   GWESTER
// Modified CatchMemoryFile() to also return the size of the data transmitted
// from Norton File Manager.
//
//    Rev 1.0   18 Dec 1995 17:04:00   GWESTER
// Initial revision.
//
//************************************************************************

#include "platform.h"                   // Platform independence
#include <shellapi.h>
#include "windowsx.h"                   // Windows control functions
#include "xapi.h"
#include "disk.h"
#include "undoc.h"

#include "ngui.h"                       // Norton look / feel
#include "ctl3d.h"                      // Microsoft 3D look
#include "msgdisp.h"                    // For Quake Message Display API

#include "common.h"
#include "cmbstr.h"
#include "nonav.h"
#include "navalog.h"
#include "options.h"
#include "virlist.h"
#include "inoc.h"
#include "navw.h"
#include "navwcomm.h"

#ifdef SYM_WIN32
    #include "navapcom.h"
#endif

#ifdef NAVSCANW
    #include "navver.h"
#endif

//************************************************************************
// External data
//************************************************************************

//************************************************************************
// Local definitions
//************************************************************************

                                        // Shared memory for transferring
                                        // drag & drop data:
#define WNDD_SHAREMEM_DATA_NAME     "SYMCP4_WNDD_DATA_%lx"


//************************************************************************
// Local Typedefs
//************************************************************************

//***************************************************************************
// Local prototypes
//***************************************************************************
BOOL LOCAL PASCAL APSignalDataEvent (
    DWORD       dwId
    );

LPVOID LOCAL PASCAL DDLockData (
    DWORD       dwId,
    PHANDLE     pHandle
    );

DWORD LOCAL PASCAL DDSizeOfData (
    LPVOID      lpvData
    );

BOOL LOCAL PASCAL DDUnlockData (
    LPVOID      lpvData,
    HANDLE      handle
    );



//***************************************************************************
// CatchMemoryFile()
//
// Description:
//      Opens the memory-mapped file referred to in HandleIDString,
//      copies this file to a local buffer within this process, and then
//      signals the owner of the memory-mapped file to destroy it. (The
//      owner is whatever process launched NAV with the /HMEM parameter.)
//
//      szHandleID is the string of characters <x> from the
//      command-line parameter /HMEM=<x>.
//
//      If the memory-mapped file is successfully opened, and the
//      contents are successfully copied to a local buffer, this function
//      returns a pointer to that buffer. This function returns NULL in
//      any other case (and cleans up after itself).
//
//      LPTSTR       szHandleID
//          Characters <x> from parameter /HMEM=<x>.
//
//      LPDWORD     lpdwSizeOfData
//          If the pointer returned by this function is a valid buffer,
//          the DWORD pointed to by lpdwSizeOfData will be set to
//          the size of the buffer.
//
//***************************************************************************
// 12/20/1995 GWESTER lpdwSizeOfData parameter added.
// 12/18/1995 GWESTER Function Created.
//***************************************************************************
LPTSTR WINAPI CatchMemoryFile (
    LPTSTR       szHandleID,
    LPDWORD     lpdwSizeOfData
    )
{
(*lpdwSizeOfData) = 0;                  // Initialize this to 0.


DWORD       dwId = 0;
TCHAR        szhMem[20];

                                        // The buffer that will receive
                                        // the contents of the
                                        // memory-mapped file. This
                                        // eventually becomes our return
                                        // value:
LPTSTR       lpStr = NULL;

                                        // szHandleID contains the <x>
                                        // value (as hexadecimal) in a
                                        // string.
                                        // Convert the value to a DWORD
                                        // stored in dwId:
STRNCPY( (LPTSTR)szhMem, szHandleID, 19 );
szhMem[19] = 0;
dwId = strtol( szhMem, NULL, 16 );

                                        // Make sure dwId is a valid number:
if (dwId != 0)
    {
    HANDLE   hData;

                                        // Get handle to the memory-mapped
                                        // file:
    LPTSTR   lpSrc = (LPTSTR)(DDLockData( dwId, &hData ));

    if (lpSrc)
        {
                                        // Get data size:
        DWORD    dwSize = DDSizeOfData( lpSrc );

                                        // Let the outside world know how
                                        // much data there is:
        (*lpdwSizeOfData) = dwSize;

                                        // Allocate the buffer:
        lpStr = (LPTSTR)(GlobalAllocPtr( GHND, dwSize ));

                                        // Make sure space was allocated:
        if (lpStr)
            {
                                        // Create string of zero length:
            *lpStr = 0x00;
            *(lpStr + 1) = 0x00;

                                        // Copy data:
            _fmemcpy( lpStr, lpSrc, (int)dwSize );
            }                           // End if (buffer allocated) ...

                                        // Unlock memory mapped data:
        DDUnlockData( lpSrc, hData );

                                        // Tell File Manager we are done
                                        // with the data (so it can free
                                        // the memory block):
        APSignalDataEvent( dwId );
        }                               // End if (File locked OK) ...
    }                                   // End if (File handle was valid) ...

return lpStr;
}                                       // End CatchMemoryFile()

//***************************************************************************
// CatchMemoryFile_Cleanup()
//
// Description:
//      If CatchMemoryFile() successfully returned a local buffer to
//      NAV, NAV will call this function when it is done using the
//      buffer. This function frees the buffer.
//
//      Returns TRUE if the cleanup succeeds.
//
//***************************************************************************
// 12/18/1995 GWESTER Function Created.
//***************************************************************************
BOOL WINAPI CatchMemoryFile_Cleanup (
    LPTSTR       lpStr
    )
{
if (!lpStr) {return TRUE;}
BOOL ReturnValue = GlobalFreePtr (lpStr);

SYM_ASSERT (!ReturnValue);
return (!ReturnValue);
}                                       // End CatchMemoryFile_Cleanup()



//***************************************************************************
// APSignalDataEvent()
//
// Description:
//      Signals the owner of the memory-mapped data that we are done
//      using it. The owner will then free the data. After this function
//      is called, access to the data is not possible.
//
//      Returns TRUE if the owner of the data is properly signaled.
//
//***************************************************************************
// 12/18/1995 GWESTER Changed from WINAPI to LOCAL PASCAL.
// 12/18/1995 GWESTER Function Ported From Navigator NT.
//***************************************************************************
BOOL LOCAL PASCAL APSignalDataEvent (
    DWORD       dwId
    )
{
BOOL     fRtn = FALSE;
TCHAR    szName[80];
HANDLE   hEvent;

wsprintf( szName, "FM4_SpawnData_%lx", dwId );
hEvent = OpenEvent( EVENT_ALL_ACCESS, TRUE, szName );
if (hEvent)
    {
    fRtn = SetEvent( hEvent );
    CloseHandle( hEvent );
    }

return fRtn;
}                                       // End APSignalDataEvent()

//***************************************************************************
// DDLockData()
//
// Description:
//      Locks the memory mapped file of the specified ID.
//
//      Returns pointer to data block and fills in memory mapped file
//      handle. If return value is NULL or the handle is NULL, an error
//      has occurred and the call has failed. This generally occurs if
//      the data of the specified ID does not exist or has somehow been
//      previously freed.
//
//***************************************************************************
// 12/18/1995 GWESTER Changed from WINAPI to LOCAL PASCAL.
// 12/18/1995 GWESTER Function Ported From Navigator NT.
//***************************************************************************
LPVOID LOCAL PASCAL DDLockData (
    DWORD       dwId,
    PHANDLE     pHandle
    )
{
LPVOID   lpvData = NULL;

if (pHandle)
    {
    TCHAR szName[128];

    wsprintf( szName, WNDD_SHAREMEM_DATA_NAME, dwId );
    *pHandle = OpenFileMapping( FILE_MAP_ALL_ACCESS, FALSE, szName );
    if (*pHandle)
        {
        lpvData = MapViewOfFile( *pHandle, FILE_MAP_ALL_ACCESS, 0, 0, 0 );
        if (!lpvData)
            {
            CloseHandle( *pHandle );
            *pHandle = NULL;
            }
            else
            {
            lpvData = (((LPDWORD)lpvData) + 1);
            }
        }
    }

return lpvData;
}                                       // End DDLockData()

//***************************************************************************
// DDSizeOfData()
//
// Description:
//      Returns the size (bytes) of the given memory mapped data block.
//
//***************************************************************************
// 12/18/1995 GWESTER Changed from WINAPI to LOCAL PASCAL.
// 12/18/1995 GWESTER Function Ported From Navigator NT.
//***************************************************************************
DWORD LOCAL PASCAL DDSizeOfData (
    LPVOID      lpvData
    )
{
DWORD dwRtn;

if (lpvData)
    dwRtn = *(((LPDWORD)lpvData) - 1);

return dwRtn;
}                                       // End DDSizeOfData()

//***************************************************************************
// DDUnlockData()
//
// Description:
//      Unlocks the given memory mapped data and closes the given handle.
//
//      Returns TRUE on success, FALSE on failure.
//
//***************************************************************************
// 12/18/1995 GWESTER Changed from WINAPI to LOCAL PASCAL.
// 12/18/1995 GWESTER Function Ported From Navigator NT.
//***************************************************************************
BOOL LOCAL PASCAL DDUnlockData (
    LPVOID      lpvData,
    HANDLE      handle
    )
{
BOOL  fRtn = FALSE;

if (lpvData && handle)
    {
    lpvData = ((LPDWORD)lpvData) - 1;
    fRtn = UnmapViewOfFile( lpvData );
    if (fRtn)
        fRtn = CloseHandle( handle );
    }

return fRtn;
}                                       // End DDUnlockData()
