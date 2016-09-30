// Copyright 1998 Symantec Corporation. 
//*************************************************************************
// winmain.cpp - created 11/6/98 1:38:12 PM
//
// $Header$
//
// Description:
//
// Contains:
//
//*************************************************************************
// $Log$
//*************************************************************************

#include "stdafx.h"
#include "defwatch.h"
#include "defwatchservice.h"

///////////////////////////////////////////////////////////////////////////
// 
// Globals
// 
///////////////////////////////////////////////////////////////////////////

// 
// The one and only DefWatchService object
// 
CDefWatchService* g_pDefWatchService;


///////////////////////////////////////////////////////////////////////////
//
// Local functions
//
///////////////////////////////////////////////////////////////////////////

VOID PASCAL DefWatchServiceMain( DWORD argc, LPTSTR *argv );
VOID PASCAL DefWatchServiceHandler( DWORD Opcode );
void PASCAL CreatePsuedoWin9xService( HINSTANCE hInstance, LPSTR lpCmdLine );
BOOL Win95RegisterService();
LRESULT CALLBACK WndProc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam );

///////////////////////////////////////////////////////////////////////////
//
// Local data.
//
///////////////////////////////////////////////////////////////////////////

//***************************************************************************/
// Stuff needed for Windows 9x RegisterServiceProcess()
//***************************************************************************/
typedef DWORD ( PASCAL *PREGISTERSERVICEPROCESS) (DWORD, DWORD);

PREGISTERSERVICEPROCESS g_pRegister = NULL;

#ifndef RSP_SIMPLE_SERVICE
#define RSP_SIMPLE_SERVICE  0x00000001
#endif

///////////////////////////////////////////////////////////////////////////

//*************************************************************************
// WinMain()
//
// WinMain(
//      HINSTANCE hInstance
//      HINSTANCE hPrevInstance
//      LPSTR     lpCmdLine
//      int       nCmdShow )
//
// Description: Main entry point for this module.
//
// Returns: int APIENTRY 
//
//*************************************************************************
// 11/6/98 DBUCHES, created - header added.
//*************************************************************************

int APIENTRY WinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPSTR     lpCmdLine,
                     int       nCmdShow)
{

    // 
    // Are we running on Windows 9x?
    // 
    OSVERSIONINFO ver;
    ver.dwOSVersionInfoSize = sizeof( OSVERSIONINFO );
    GetVersionEx( &ver );
    BOOL bIsWin9x = ( BOOL ) (ver.dwPlatformId == VER_PLATFORM_WIN32_WINDOWS);

    // 
    // Q: Are we running on Windows 9x?
    // 
    if( bIsWin9x )
        {
        
        // 
        // Create our pseudo service for win9x
        // 
        CreatePsuedoWin9xService( hInstance, lpCmdLine );
        }
    else
        {
        //
        // Initialize our service
        //
        SERVICE_TABLE_ENTRY DefWatchDispachTable[] = 
            {
                { DEFWATCH_SERVICE_NAME, DefWatchServiceMain },
                { NULL,             NULL }
            };

        //
        // Register ourselves with the SCM
        //
        if( !StartServiceCtrlDispatcher( DefWatchDispachTable ) )
            {
            DefWatchDebugOut( "StartServiceCtrlDispatcher failed with error code %X", GetLastError() );
            }
        }

    return 0;
}

///////////////////////////////////////////////////////////////////////////

//*************************************************************************
// DefWatchDebugOut()
//
// DefWatchDebugOut(
//      LPSTR String
//      DWORD Status )
//
// Description: Debug output.  Compliles away for release builds.
//
// Returns: VOID 
//
//*************************************************************************
// 11/6/98 DBUCHES, created - header added.
//*************************************************************************

#ifdef _DEBUG
void DefWatchDebugOut(LPSTR String, DWORD Status/* = 0 */)
{
    CHAR  Buffer[512];
    CHAR  szOutput[512];

    if (strlen(String) < 512)
        {
        lstrcpy( szOutput, "DefWatch: " );
        wsprintf(Buffer, String, Status);
        lstrcat( szOutput, Buffer );
        lstrcat( szOutput, "\n" );
        OutputDebugStringA(szOutput);
        }
} 
#endif
///////////////////////////////////////////////////////////////////////////

//*************************************************************************
// DefWatchServiceMain()
//
// DefWatchServiceMain(
//       DWORD argc
//      LPTSTR *argv )
//
// Description: Main entry point for NT service.
//
// Returns: VOID PASCAL 
//
//*************************************************************************
// 11/6/98 DBUCHES, created - header added.
//*************************************************************************

VOID PASCAL DefWatchServiceMain( DWORD argc, LPTSTR *argv )
{

    //
    // Create service object.
    //
    g_pDefWatchService = new CDefWatchService;

    if( g_pDefWatchService == NULL )
        {
        DefWatchDebugOut( "Failed to allcoate Service object", 0 );
        goto BailOut;
        }

    //
    // Register ourselves with the SCM
    //
    if( FALSE == g_pDefWatchService->Register( DefWatchServiceHandler ) )
        {
        goto BailOut;
        }

    //
    // Fire off our service.
    //
    if( g_pDefWatchService->StartService() == FALSE )
        {
        goto BailOut;
        }

    //
    // Wait here forever until we are signaled to quit
    //
    WaitForSingleObject( g_pDefWatchService->m_hServiceEvent, INFINITE );
    
BailOut:        
    //
    // Cleanup
    //
    if( g_pDefWatchService ) 
        delete g_pDefWatchService;

       
}

///////////////////////////////////////////////////////////////////////////

VOID PASCAL DefWatchServiceHandler( DWORD Opcode )
{
    switch( Opcode )
        {
        case SERVICE_CONTROL_STOP:
            g_pDefWatchService->StopService();
                        
            //
            // Time to bail out.
            //
            SetEvent( g_pDefWatchService->m_hServiceEvent );
            return;

        default:
            DefWatchDebugOut( "DefWatchServiceHandler: Invalid Opcode %X.", Opcode );
            break;

        }

}

//*************************************************************************
// CreatePsuedoWin9xService()
//
// CreatePsuedoWin9xService(
//       HINSTANCE hInstance
//      LPSTR lpCmdLine )
//
// Description:
//
// Returns: void PASCAL 
//
//*************************************************************************
// 11/9/98 DBUCHES, created - header added.
//*************************************************************************

void PASCAL CreatePsuedoWin9xService( HINSTANCE hInstance, LPSTR lpCmdLine )
{
    HANDLE      hMutex;
    HWND        hwndMain;
    WNDCLASS    wc;

    //
    // Make sure only one instance of ourselves is loaded.
    //
    hMutex = CreateMutex( NULL, FALSE, _T("DefWatchDaemonMutex") );
    if( GetLastError() == ERROR_ALREADY_EXISTS )
        {
        CloseHandle( hMutex );
        return;
        }

    // Register the window class for the main window.
    wc.style = 0;
    wc.lpfnWndProc = (WNDPROC) WndProc;
    wc.cbClsExtra = 0;
    wc.cbWndExtra = 0;
    wc.hInstance = hInstance;
    wc.hIcon = NULL;//LoadIcon(hInst, MAKEINTRESOURCE(IDI_APP_ICON));
    wc.hCursor = LoadCursor((HINSTANCE) NULL, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
    wc.lpszClassName = _T("DefWatchDaemonWndClass");
    if (!RegisterClass(&wc))
        goto cleanup;


    // Create our window
    hwndMain = CreateWindow( _T("DefWatchDaemonWndClass"),
                            _T(""),
                            WS_OVERLAPPEDWINDOW,
                            1,
                            1,
                            1,
                            1,
                            (HWND) NULL,
                            (HMENU) NULL,
                            hInstance,
                            (LPVOID) NULL);

    // If the main window cannot be created, terminate     
    // the application.
    if (!hwndMain)
        goto cleanup;

    // Register ourselves as a windows 95 "service"
    if( Win95RegisterService() == FALSE )
        {
        //MessageBeep(0);
        goto cleanup;
        }

    // Show our window hidden.
    ShowWindow(hwndMain, SW_HIDE);
    UpdateWindow(hwndMain);

    //
    // Create service object.
    //
    g_pDefWatchService = new CDefWatchService;

    if( g_pDefWatchService == NULL )
        {
        DefWatchDebugOut( "Failed to allcoate Service object", 0 );
        goto cleanup;
        }

    //
    // Set service object in motion
    //
    if( FALSE == g_pDefWatchService->StartService() )
        {
        DefWatchDebugOut( "Failed to start service object. " );
        goto cleanup;
        }

    // Start the message loop.
    MSG msg;
    while (GetMessage(&msg, (HWND) NULL, 0, 0))
        {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
        }

    // 
    // Stop service object.
    // 
    g_pDefWatchService->StopService();

cleanup:

    // Cleanup
    if( g_pDefWatchService ) 
        delete g_pDefWatchService;

    CloseHandle( hMutex );
}

////////////////////////////////////////////////////////////////////////////
// Function name        : Win95RegisterService
//
// Description      : Registers ourselves as a pseudo-service under Win 95
//
// Return type          : BOOL
//
//
////////////////////////////////////////////////////////////////////////////
// 12/10/97 DBUCHES - Function created / Header added.
////////////////////////////////////////////////////////////////////////////
BOOL Win95RegisterService()
{
    // 
    // Call function if we've been here before.
    // 
    if( g_pRegister )
        {
        return g_pRegister( NULL, RSP_SIMPLE_SERVICE );
        }

    // Get handle to Kernel32
    HMODULE hKernel32;
    hKernel32 = GetModuleHandle( _T("Kernel32") );
    if( hKernel32 == NULL )
        {
        return FALSE;
        }

    // Get pointer to RegisterServiceProcess();
    g_pRegister = (PREGISTERSERVICEPROCESS) GetProcAddress( hKernel32, _T("RegisterServiceProcess"));
    if( g_pRegister == NULL )
        {
        return FALSE;
        }

    // Call our function
    return g_pRegister( NULL, RSP_SIMPLE_SERVICE );
}

////////////////////////////////////////////////////////////////////////////
// Function name    : WndProc
//
// Description      : This is the window proc for our app window under
//                    windows 9x
//
// Return type      : LRESULT CALLBACK
//
// Argument         : HWND hWnd
// Argument         : UINT msg
// Argument         : WPARAM wParam
// Argument         : LPARAM lParam
//
////////////////////////////////////////////////////////////////////////////
// 12/10/97 DBUCHES - Function created / Header added.
////////////////////////////////////////////////////////////////////////////
LRESULT CALLBACK WndProc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam )
{
    return DefWindowProc( hWnd, msg, wParam, lParam );
}



////////////////////////////////////////////////////////////////////////////
// Function name    : DefWatchLogEvent
//
// Argument         : LPSTR lpszParam1
// Argument         : LPSTR lpszParam2
// Argument         : LPSTR lpszParam3
// Argument         : LPSTR lpszParam4
//
////////////////////////////////////////////////////////////////////////////
// 12/10/97 DBUCHES - Function created / Header added.
////////////////////////////////////////////////////////////////////////////
void DefWatchLogEvent( DWORD dwMsgID,
					   WORD wType /* = EVENTLOG_ERROR_TYPE */, 					   
					   LPSTR lpszParam1 /* = NULL */,
					   LPSTR lpszParam2 /* = NULL */,
					   LPSTR lpszParam3 /* = NULL */,
					   LPSTR lpszParam4 /* = NULL */ )
{
	auto	LPSTR	aParams[ 4 ];
	auto	WORD	wParamCount = 0;
	auto	HANDLE	hEventSource;

    // 
    // Q: Are we running on Windows 9x?  If so, bail out
    // 
    OSVERSIONINFO ver;
    ver.dwOSVersionInfoSize = sizeof( OSVERSIONINFO );
    GetVersionEx( &ver );
    BOOL bIsWin9x = ( BOOL ) (ver.dwPlatformId == VER_PLATFORM_WIN32_WINDOWS);
    if( bIsWin9x )
		return;

	// 
    // Construct paramater array.
    // 
	aParams[0] = lpszParam1;
	aParams[1] = lpszParam2;
	aParams[2] = lpszParam3;
	aParams[3] = lpszParam4;

	// 
    // Compute number of parameters.
    //
	for( int i = 0; i < 4; i++ )
		wParamCount += (aParams[i] == NULL) ? 0 : 1;
	
	// 
    // Set ourselves up with the event mangaer
    // 
	hEventSource = RegisterEventSource( NULL, DEFWATCH_SERVICE_NAME );
	if( hEventSource == NULL )
		{
		// 
        // Badness occured.  Too bad.
        // 
		DefWatchDebugOut( "Error reporting event." );
		return;
		}

	// 
    // Report event
    // 
	if( !ReportEvent (
		  hEventSource,             // Handle to Event Source
		  wType,					// Event type to log
		  wType,                    // Event category
		  dwMsgID,					// Event ID (from the MC file)
		  NULL,                     // Security ID
		  wParamCount,              // Number of strings
		  0,
		  (LPCSTR * )aParams,       // Array of strings to merge with MSG
		  NULL) )                   // Address of RAW data (not used)
				 
		{
		// 
        // Badness occured.  Too bad.
        // 
		DefWatchDebugOut( "Error reporting event." );
		}

	// 
    // Cleanup
	// 
	DeregisterEventSource( hEventSource );

}

