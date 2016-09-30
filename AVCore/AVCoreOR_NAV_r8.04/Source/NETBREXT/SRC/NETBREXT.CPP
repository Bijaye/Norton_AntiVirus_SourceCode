/////////////////////////////////////////////////////////////////////
// NetBrExt.CPP - Created - 11/01/96
//
// Copyright 1992-1996 Symantec, Peter Norton Product Group
/////////////////////////////////////////////////////////////////////
// $Header:   S:/NETBREXT/VCS/netbrext.cpv   1.0   21 Nov 1997 11:52:02   KSACKIN  $
//
// Description:
//    This file will set up the extension module for this dll.  This
//    will be used in preserving, and switching to our module's state
//    in order to do such things as load resources from our module
//    and such.
//
/////////////////////////////////////////////////////////////////////
//
// $Log:   S:/NETBREXT/VCS/netbrext.cpv  $
// 
//    Rev 1.0   21 Nov 1997 11:52:02   KSACKIN
// Initial revision.
// 
//    Rev 1.0   18 Apr 1997 14:03:04   DSACKIN
// Ported from Tesla
// 
//    Rev 1.0   13 Mar 1997 15:16:32   RFULLER
// Initial revision
// 
//    Rev 1.0   20 Nov 1996 01:49:14   ksackin
// Initial revision.
//
/////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include <afxdllx.h>
#include "DllRCSta.h"                   // Resource state switcher.


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


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
//    HINSTANCE hInstance - [IN] Instance of this dll.
//    DWORD dwReason      - [IN] Reason why this function is being
//                              called.  This will be either
//                              DLL_PROCESS_ATTACH or
//                              DLL_PROCESS_DETACH.
//    LPVOID lpvReserved  - [IN] Reserved value.
//
// Returns:
//    int as to the success of the function.
//
// See Also:
//
/////////////////////////////////////////////////////////////////////
// 10/29/96 - KSACKIN - Function Created.
/////////////////////////////////////////////////////////////////////
extern "C" 
int APIENTRY DllMain(
    HINSTANCE hInstance, 
    DWORD dwReason, 
    LPVOID lpReserved
    )
 {
	if (dwReason == DLL_PROCESS_ATTACH)
	{
		TRACE0("NETBREXT.DLL Initializing!\n");
		
		// Extension DLL one-time initialization
		AfxInitExtensionModule(extMyExtension, hInstance);

		// Insert this DLL into the resource chain
		new CDynLinkLibrary(extMyExtension);
	}
	else if (dwReason == DLL_PROCESS_DETACH)
	{
		TRACE0("NETBREXT.DLL Terminating!\n");
	}
	return 1;   // ok
}
