// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright (c) 2005 Symantec Corporation. All rights reserved.

#include "stdafx.h"
#include "newdef.h"
#include "clientreg.h"
#include "resource.h"
#include "SymSaferRegistry.h"
#include "SymSaferStrings.h"
#include "AccessToken.h"
#include "WTSLoader.h"
#include "terminalsession.h"
#include "OSUtils.h"
#include "UserEnv.h"
#include "SavAssert.h"


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
    PROCESS_INFORMATION pi;

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

            CTerminalSession oTerminalSession;
            BOOL bTSEnabled = oTerminalSession.IsTerminalServicesEnabled();
            CAccessToken oToken(TOKEN_INVALID);
            DWORD dwSessionId = 0;

            if (bTSEnabled)
            {
                CWTSLoader& oWTSLoader = CWTSLoader::GetInstance();

                // Find the active user and obtain a primary token to use in CreateProcessAsUser().
                // Enumerate the sessions and find the first active user.  Note that the active user
                // can be a console or a remote session.
                dwSessionId = oWTSLoader.GetFirstActiveSessionId();

                // If success, try to obtain an access token.
                if (dwSessionId != 0xFFFFFFFF)
                    oWTSLoader.QueryUserToken(dwSessionId, oToken);
            }

            BOOL bResult = FALSE;

            if (bTSEnabled == TRUE && oToken)
            {
                WCHAR wszCmdLine[MAX_PATH] = {0};
                DWORD dwRet = 0;

                if (MultiByteToWideChar(CP_ACP, 0, szCmdLine, -1, wszCmdLine, sizeof(wszCmdLine)/sizeof(wszCmdLine[0])) != 0)
                {
                    // Create the environment for the user.
                    PVOID pEnvBlock = NULL;
                    CreateEnvironmentBlock(&pEnvBlock, oToken, FALSE);

                    SAVASSERT(pEnvBlock != NULL);

                    STARTUPINFOW siw;
                    // Init the startupinfo structure
                    memset (&siw, 0, sizeof(siw));
                    siw.cb = sizeof(siw);

                    // Must use the wide version b/c CreateProcessAsUserA will always return 
                    // error when used on W2K with Terminal services.
                    bResult = CreateProcessAsUserW(
                                                    oToken,                     // Client's access token
                                                    NULL,                       // File to execute
                                                    wszCmdLine,                 // Command line
                                                    NULL,                       // Pointer to process SECURITY_ATTRIBUTES
                                                    NULL,                       // Pointer to thread SECURITY_ATTRIBUTES
                                                    FALSE,                      // Handles are not inheritable
                                                    CREATE_UNICODE_ENVIRONMENT, // Creation flags
                                                    pEnvBlock,                  // Pointer to new environment block
                                                    NULL,                       // Name of current directory 
                                                    &siw,                       // Pointer to STARTUPINFO structure
                                                    &pi                         // Receives information about new process
                                                    );

                    if (pEnvBlock)
                        DestroyEnvironmentBlock(pEnvBlock);
                }
            }
            else
            {
                // For OS's that do not implement Terminal Services

                // Extra check for Windows Vista.  Do not call CreateProcess() since
                // it will trigger UIDetect in Session 0.
                if (!IsWindowsVista(true))
                {
                    STARTUPINFO         si;
                    // Init the startupinfo structure
                    memset (&si, 0, sizeof(si));
                    si.cb = sizeof(si);

                    // Start the DefWatch wizard.
                    bResult = CreateProcess( 
                                            NULL,               // App Name
                                            szCmdLine,          // Command Line
                                            NULL,               // Process Attributes
                                            NULL,               // Thread Attributes
                                            FALSE,              // Inherit handles                             
                                            0,                  // Creation Flags
                                            NULL,               // Environment
                                            NULL,               // Current directory
                                            &si,                // Startup Info
                                            &pi                 // Process Info
                                            );
                }
            }

            if (bResult)
            {
                CloseHandle( pi.hThread );
                CloseHandle( pi.hProcess );
            }
        }
    }
 
    return S_OK;
}