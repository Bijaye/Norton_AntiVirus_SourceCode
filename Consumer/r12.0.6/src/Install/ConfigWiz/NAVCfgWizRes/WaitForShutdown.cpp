// WaitForShutdown.cpp: implementation of the CWaitForShutdown class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "WaitForShutdown.h"
#include "StahlSoft.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CWaitForShutdown::CWaitForShutdown()
{
    m_strWindowName = _T("NAV_WAITFORSHUTDOWN");

    // Create event object.
	m_shShutdownEvent = CreateEvent( NULL, FALSE, FALSE, NULL ); // auto-reset since it's a local event
	
    // Construct our message pump thread
    //
    unsigned int uiThreadID;

    m_ulMonitorThread = _beginthreadex ( NULL, 0, threadProc, this, 0, &uiThreadID );

    if ( m_ulMonitorThread )
    {
    }
}
void CWaitForShutdown::Terminate()
{
    if(m_ulMonitorThread)
    {

        if ( ::SetEvent ( m_shShutdownEvent ) )
        {
            // Clean things up.

            StahlSoft::WaitForSingleObjectWithMessageLoop( (void*)(m_ulMonitorThread),
                INFINITE);

            CCTRACEI ( "Shutdown thread is shutting down" );     
        }
        ::CloseHandle((HANDLE)m_ulMonitorThread);
        m_ulMonitorThread = 0;
    }
}

CWaitForShutdown::~CWaitForShutdown()
{
    Terminate();
}

bool CWaitForShutdown::GetShutdown()
{
    bool bTemp = false;
    //--~~--//--~~--//--~~--//--~~--//--~~--//--~~--//--~~--
    StahlSoft::CSmartCritSection smCrit(&m_shCritShutdown);
    //--~~--//--~~--//--~~--//--~~--//--~~--//--~~--//--~~--
    
    bTemp = m_bShutdown;

    return bTemp;
}

void CWaitForShutdown::createWindow ()
{
	//If the window class has not been registered, then do so.
	WNDCLASS wc;
	if(!GetClassInfo(m_hInstance, m_strWindowName.c_str(), &wc))
	{
		ZeroMemory(&wc, sizeof(wc));
		wc.style          = CS_HREDRAW | CS_VREDRAW;
        wc.lpfnWndProc    = (WNDPROC) CWaitForShutdown::WndProc;
		wc.cbClsExtra     = 0;
		wc.cbWndExtra     = 0;
		wc.hInstance      = m_hInstance;
		wc.hIcon          = NULL;
		wc.hCursor        = NULL;
		wc.hbrBackground  = NULL;
		wc.lpszMenuName   = NULL;
		wc.lpszClassName  = m_strWindowName.c_str();
		RegisterClass(&wc);
	}

	// Create the window.
	m_hWnd = CreateWindow( m_strWindowName.c_str(), 
						   TEXT(""),
						   WS_POPUPWINDOW | WS_DISABLED, 
						   -100,  // Offscreen
						   -100,  // Offscreen
						   1,
						   1,
						   NULL,
						   NULL,
						   m_hInstance,
						   static_cast<void*>( this ) );

}

//
//  FUNCTION: WndProc(HWND, unsigned, WORD, LONG)
//
//  PURPOSE:  Processes messages for the main window.
//
//  WM_COMMAND	- process the application menu
//  WM_PAINT	- Paint the main window
//  WM_DESTROY	- post a quit message and return
//
//
LRESULT CALLBACK CWaitForShutdown::WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	// Handle create message
	if( uMsg == WM_CREATE )
	{
		// Save off our this pointer.
		CREATESTRUCT* pcs = reinterpret_cast< CREATESTRUCT* >( lParam );
		SetWindowLong( hWnd, GWL_USERDATA, reinterpret_cast<long>( pcs->lpCreateParams ) );
		return 0;
	}

	// Get our this pointer
	CWaitForShutdown* pThis = reinterpret_cast< CWaitForShutdown* >
		( GetWindowLong( hWnd, GWL_USERDATA ) );
	
	// Q: is this our message?
	if( pThis != NULL && uMsg == WM_QUERYENDSESSION )
	{
        pThis -> setShutdown ( true );
        CCTRACEI ( "Got shutdown in thread", MB_OK );
	}
    
	return DefWindowProc(hWnd, uMsg, wParam, lParam);
}

unsigned int __stdcall CWaitForShutdown::threadProc(void* pv)
{
    if ( !pv )
        return 0;
    
    CWaitForShutdown* pThis = static_cast<CWaitForShutdown*> (pv);
    return pThis->InternalThreadProc();

}
unsigned int CWaitForShutdown::InternalThreadProc()
{
    try
    {
        createWindow( );

        StahlSoft::WaitForMultipleObjectsWithMessageLoop ( m_shShutdownEvent, INFINITE );
        if( m_hWnd )
            DestroyWindow( m_hWnd );

        UnregisterClass ( m_strWindowName.c_str(), m_hInstance );
    }
    catch(...){}
    return 0;
}

void CWaitForShutdown::setShutdown(bool bShutdown)
{
    //--~~--//--~~--//--~~--//--~~--//--~~--//--~~--//--~~--
    StahlSoft::CSmartCritSection smCrit(&m_shCritShutdown);
    //--~~--//--~~--//--~~--//--~~--//--~~--//--~~--//--~~--
    
    m_bShutdown = bShutdown;
}
