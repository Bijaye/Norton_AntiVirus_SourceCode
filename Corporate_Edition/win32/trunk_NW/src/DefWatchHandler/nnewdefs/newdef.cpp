// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright (c) 2005 Symantec Corporation. All rights reserved.

#include "stdafx.h"
#include "newdef.h"
#include "clientreg.h"
#include "resource.h"
#include "SymSaferRegistry.h"
#include "SymSaferStrings.h"


// Global object count
extern LONG g_dwObjs;
extern HINSTANCE g_hInstance;



///////////////////////////////////////////////////////////////////////////////
//
// Function name: CNewDefHandler::CNewDefHandler
//
// Description: 
//
// Return type: 
//
//
///////////////////////////////////////////////////////////////////////////////
// 11/6/98 - DBUCHES: Function created / header added 
///////////////////////////////////////////////////////////////////////////////
CNewDefHandler::CNewDefHandler()
{
    // 
    // Bump global object count.
    // 
    InterlockedIncrement( &g_dwObjs );
    
    // 
    // Set defaults
    // 
    m_dwRef = 0;
}


///////////////////////////////////////////////////////////////////////////////
//
// Function name: CNewDefHandler::~CNewDefHandler
//
// Description: 
//
// Return type: 
//
//
///////////////////////////////////////////////////////////////////////////////
// 11/6/98 - DBUCHES: Function created / header added 
///////////////////////////////////////////////////////////////////////////////
CNewDefHandler::~CNewDefHandler()
{
    // 
    // Cleanup.
    // 
    InterlockedDecrement( &g_dwObjs );
}

///////////////////////////////////////////////////////////////////
// IUnknown implementation

////////////////////////////////////////////////////////////////////////////////
//
// Function name    : CNewDefHandler::QueryInterface
// Description      : This function will return a requested COM interface
// Return type      : STDMETHODIMP
// Argument         : REFIID riid - REFIID of interface requested
// Argument         : void** ppv - pointer to requested interface
//
////////////////////////////////////////////////////////////////////////////////
// 8/28/97 - DBUCHES function created / header added
////////////////////////////////////////////////////////////////////////////////
STDMETHODIMP CNewDefHandler::QueryInterface( REFIID riid, void** ppv )
{
    *ppv = NULL;

    //MessageBox( GetDesktopWindow(), "Here we are", "Foobar", MB_OK );

    if( IsEqualIID( riid, IID_IUnknown )||
        IsEqualIID( riid, IID_DefWatchEventHandler) )
        *ppv = this;

    if( *ppv )
        {
        ((LPUNKNOWN)*ppv)->AddRef();
        return ResultFromScode( S_OK );
        }

    return ResultFromScode( E_NOINTERFACE );
}


////////////////////////////////////////////////////////////////////////////////
//
// Function name    : CNewDefHandler::AddRef()
// Description      : Increments reference count for this object
// Return type      : ULONG
//
////////////////////////////////////////////////////////////////////////////////
// 8/28/97 - DBUCHES function created / header added
////////////////////////////////////////////////////////////////////////////////
STDMETHODIMP_(ULONG) CNewDefHandler::AddRef()
{
    return ++m_dwRef;
}


////////////////////////////////////////////////////////////////////////////////
//
// Function name    : CNewDefHandler::Release()
// Description      : Decrements reference count for this object
// Return type      : ULONG
//
////////////////////////////////////////////////////////////////////////////////
// 8/28/97 - DBUCHES function created / header added
////////////////////////////////////////////////////////////////////////////////
STDMETHODIMP_(ULONG) CNewDefHandler::Release()
{
    if( --m_dwRef )
        return m_dwRef;

    delete this;
    return 0;
}



///////////////////////////////////////////////////////////////////////////////
//
// Function name: CNewDefHandler::OnNewDefsInstalled
//
// Description: This method is called when new virus definitions have arrived.
//
// Return type: HRESULT 
//
//
///////////////////////////////////////////////////////////////////////////////
// 11/6/98 - DBUCHES: Function created / header added 
// 4/8/99  - TCASHIN: Start the DefWatch Wizard
///////////////////////////////////////////////////////////////////////////////
STDMETHODIMP CNewDefHandler::OnNewDefsInstalled()
{
    TCHAR               szCmdLine[MAX_PATH] = {0};
    TCHAR               szHomeDir[MAX_PATH] = {0};
    TCHAR               szWizName[MAX_PATH] = {0};
	TCHAR				szPath[MAX_PATH]	= {0};
	TCHAR				szshrtPath[MAX_PATH]= {0};
    HKEY                hKey;
    DWORD               dwSize = MAX_PATH;
    DWORD               dwType;
    STARTUPINFO         si;
    PROCESS_INFORMATION pi;

    // Init the startupinfo structure
    memset (&si, 0, sizeof(si));
    si.cb = sizeof(si);

    // Can we open the main Intel reg key?
    if ( SUCCEEDED( RegOpenKeyEx ( HKEY_LOCAL_MACHINE,
                                   szReg_Key_Main,
                                   0,
                                   KEY_READ,
                                   &hKey )))
    {
        // Get the value for the installed directory.
        long lRetVal = SymSaferRegQueryValueEx(hKey,
                                        szReg_Val_HomeDir,
                                        NULL,
                                        &dwType,
                                        (LPBYTE) szHomeDir,
                                        &dwSize);

        // Done with the registry.
        RegCloseKey(hKey);

        if ( lRetVal == ERROR_SUCCESS )
        {        
            // there are evil race conditions between def handling in dhwizard
            // and the normal virus definition processing in rtvscan - this has actually been
            // broken forever, but the errors that might happen were fairly minor - with the advent
            // of microdef handling the errors are larger (although still not serious), 
            
            // a fix was put in to delay here, but that caused some issues when shutting down
            // because defwatch wouldn't respond to the shutdown until the delay finished
            // since the issues were fairly minor, we backed this out so we can ship.
            
            // a better fix would be to gate all the definition handling with a global
            // mutex, but that is too large a change for this late in the project.

			_tcscpy(szWizName, "DWHWizrd.exe");

			// Build the command line.
            sssnprintf( szCmdLine, sizeof(szCmdLine), "\"%s\\%s\"", szHomeDir, szWizName );

			// Convert the command line to shortPathName for security 1-12WHB9 
			_tcscpy(szPath,szCmdLine);
			if ( GetShortPathName(szPath, szshrtPath, sizeof(szshrtPath)))
			{	
				_tcscpy(szCmdLine,szshrtPath); // application found and converted short format
			} // if not found then don't change anything

            // Start the DefWatch wizard.
            if ( CreateProcess( NULL,              // App Name
                                szCmdLine,         // Command Line
                                NULL,              // Process Attributes
                                NULL,              // Thread Attributes
                                FALSE,             // Inherit handles                             
                                0,                 // Creation Flags
                                NULL,              // Environment
                                NULL,              // Current directory
                                &si,               // Startup Info
                                &pi) )             // Process Info
            {
                CloseHandle( pi.hThread );
                CloseHandle( pi.hProcess );
            }
        }
    }
 
    return S_OK;
}