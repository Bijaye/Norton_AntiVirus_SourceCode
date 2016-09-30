/////////////////////////////////////////////////////////////////////////////
// $Header:   S:/SCANDRES/VCS/scandres.cpv   1.2   09 Jun 1998 16:09:08   mdunn  $
/////////////////////////////////////////////////////////////////////////////
//
// CleanPg.cpp: implementation of the CCleanFilesPropertyPage class.
//
/////////////////////////////////////////////////////////////////////////////
// $Log:   S:/SCANDRES/VCS/scandres.cpv  $
// 
//    Rev 1.2   09 Jun 1998 16:09:08   mdunn
// Changed the function to be extern c.
// 
// 
//    Rev 1.1   02 Jun 1998 18:50:56   jtaylor
// Scan and Deliver resource dll.
// 
//    Rev 1.0   02 Jun 1998 18:34:08   jtaylor
// Initial revision.
/////////////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include <afxdllx.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


static AFX_EXTENSION_MODULE ScandresDLL = { NULL, NULL };

extern "C" int APIENTRY
DllMain(HINSTANCE hInstance, DWORD dwReason, LPVOID lpReserved)
{
	// Remove this if you use lpReserved
	UNREFERENCED_PARAMETER(lpReserved);

	if (dwReason == DLL_PROCESS_ATTACH)
	{
		TRACE0("SCANDRES.DLL Initializing!\n");
		
		// Extension DLL one-time initialization
		if (!AfxInitExtensionModule(ScandresDLL, hInstance))
			return 0;

        AfxSetResourceHandle( hInstance );
        

	}
	else if (dwReason == DLL_PROCESS_DETACH)
	{
		TRACE0("SCANDRES.DLL Terminating!\n");
		// Terminate the library before destructors are called
		AfxTermExtensionModule(ScandresDLL);
	}
	return 1;   // ok
}


////////////////////////////////////////////////////////////////////////////////
//
// Function name    : InitScanDeliverResources
// Description      : Used to force loading of this library.
// Return type      : BOOL 
//
////////////////////////////////////////////////////////////////////////////////
// 2/26/98 - DBUCHES function created / header added
////////////////////////////////////////////////////////////////////////////////
extern "C" BOOL _declspec( dllexport ) InitScanDeliverResources()
    {
		// Insert this DLL into the resource chain
		// NOTE: If this Extension DLL is being implicitly linked to by
		//  an MFC Regular DLL (such as an ActiveX Control)
		//  instead of an MFC application, then you will want to
		//  remove this line from DllMain and put it in a separate
		//  function exported from this Extension DLL.  The Regular DLL
		//  that uses this Extension DLL should then explicitly call that
		//  function to initialize this Extension DLL.  Otherwise,
		//  the CDynLinkLibrary object will not be attached to the
		//  Regular DLL's resource chain, and serious problems will
		//  result.

		new CDynLinkLibrary(ScandresDLL);

        // Just return true for now
    return TRUE;
    }
