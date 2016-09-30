/////////////////////////////////////////////////////////////////////
// DLLMAIN.CPP - Created - 11/20/96
//
// Copyright 1992-1996 Symantec, Peter Norton Product Group
/////////////////////////////////////////////////////////////////////
// $Header:   S:/NETBREXT/VCS/dllmain.cpv   1.0   21 Nov 1997 11:52:00   KSACKIN  $
//
// Description:
//      This source file contains those routines necessary for starting up
//      and gracefully terminating the NetBrows.DLL.
//
/////////////////////////////////////////////////////////////////////
//
// $Log:   S:/NETBREXT/VCS/dllmain.cpv  $
// 
//    Rev 1.0   21 Nov 1997 11:52:00   KSACKIN
// Initial revision.
// 
//    Rev 1.1   27 May 1997 16:11:44   KSACKIN
// Removed the export statement from my dllmain.  It was previously being
// exported in my .lib file, and causing other modules to break.
//
//    Rev 1.0   18 Apr 1997 14:03:02   DSACKIN
// Ported from Tesla
//
//    Rev 1.0   13 Mar 1997 15:16:32   RFULLER
// Initial revision
//
//    Rev 1.0   20 Nov 1996 01:49:12   ksackin
// Initial revision.
//
/////////////////////////////////////////////////////////////////////

#include "afxwin.h"
#include "dllmain.h"


/////////////////////////////////////////////////////////////////////
// Global Variables

AFX_EXTENSION_MODULE extMyExtension = { NULL, NULL };


/////////////////////////////////////////////////////////////////////
// DllMain
//
// Purpose:
//    This function is called when attaching or detaching a process
//    from this dll.
//
// Parameters:
//    HINSTANCE hInstDLL - [IN] Instance of this dll.
//    DWORD dwReason     - [IN] Reason why this function is being
//                              called.  This will be either
//                              DLL_PROCESS_ATTACH or
//                              DLL_PROCESS_DETACH.
//    LPVOID lpvReserved - [IN] Reserved value.
//
// Returns:
//    BOOLEAN as to the success of the function.
//
// See Also:
//
/////////////////////////////////////////////////////////////////////
// 10/29/96 - KSACKIN - Function Created.
/////////////////////////////////////////////////////////////////////
BOOL WINAPI DllMain(
    HINSTANCE hInstDLL,
    DWORD dwReason,
    LPVOID lpvReserved
    )
{
    BOOL        bResult = TRUE;

                                        // Switch of reason for this call.
    switch (dwReason)
        {
        case DLL_PROCESS_ATTACH:
                                        // Attaching to a new process.
                                        // Set up the global extension
                                        // instance for this DLL.
            if ( !AfxInitExtensionModule( extMyExtension, hInstDLL ) )
                return 0;

		                                // Insert this DLL into the resource
                                        // chain
    		new CDynLinkLibrary(extMyExtension);

            break;

        case DLL_PROCESS_DETACH:
                                        // Detaching from this library.  Call
                                        // WEP function for cleanup.
            AfxTermExtensionModule( extMyExtension );
            break;
        }

    return (bResult);
}

