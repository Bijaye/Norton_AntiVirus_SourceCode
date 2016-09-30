// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright 1998 - 2005 Symantec Corporation. All rights reserved. 
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
#include <new>
#include "SymSaferStrings.h"

///////////////////////////////////////////////////////////////////////////
// 
// Globals
// 
///////////////////////////////////////////////////////////////////////////

// 
// The one and only DefWatchService object
// 
CDefWatchService* g_pDefWatchService = NULL;


///////////////////////////////////////////////////////////////////////////
//
// Local functions
//
///////////////////////////////////////////////////////////////////////////

VOID PASCAL DefWatchServiceMain( DWORD argc, LPTSTR *argv );
VOID PASCAL DefWatchServiceHandler( DWORD Opcode );

///////////////////////////////////////////////////////////////////////////
//
// Local data.
//
///////////////////////////////////////////////////////////////////////////


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
        sssnprintf(Buffer, sizeof(Buffer), String, Status);
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
	g_pDefWatchService = new(std::nothrow) CDefWatchService;

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

