// Copyright 1993 Symantec Corporation
//***********************************************************************
//
// $Header:   S:/NAVALERT/VCS/DLLMAIN.CPv
//
// Description:
//      This source file contains those routines necessary for starting up
//      and gracefully terminating the NAVALERT.DLL.
//***********************************************************************
// $Log:   S:/NAVALERT/VCS/dllmain.cpv  $
// 
//    Rev 1.0   18 Apr 1997 13:57:14   DSACKIN
// Ported from Tesla
// 
//    Rev 1.0   13 Mar 1997 15:11:52   RFULLER
// Initial revision
// 
//    Rev 1.2   13 Jan 1997 18:55:44   TSmith
// Removed export specifiers from function DllMain.
// 
//    Rev 1.1   12 Nov 1996 22:47:32   KSACKIN
// Modified classes to be MFC Extension classes.  This meant changing the
// exported functions back, and then using a macro in ihe class definition.
// 
//    Rev 1.1   07 Nov 1996 01:08:18   KSACKIN
// Changed some of the exported function to use dllexport.
//
//    Rev 1.0   29 Oct 1996 15:33:16   KSACKIN
// Initial revision.
//
//***********************************************************************

#include "afxwin.h"
#include "at_opts.h"                // for extension module.

//#include "afxdll_.h"

/////////////////////////////////////////////////////////////////////
// Global Variables

HINSTANCE   ghInst = NULL;

AFX_EXTENSION_MODULE extMyExtension = { NULL, NULL };

/////////////////////////////////////////////////////////////////////
// Function Prototypes


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

