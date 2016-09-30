// EventHandler.cpp: implementation of the CEventHandler class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include <windows.h>
#include <tchar.h>
#include <ole2.h>
#include "EventHandler.h"
#include "apwcfg.h"
#include "apwutil.h"
#include "const.h"
#include "apwres.h"
#include "EventData.h"

DWORD g_dwSessionID = 0;

//////////////////////////////////////////////////////////////////////
// CEventHandler::CEventHandler()

CEventHandler::CEventHandler( CApwNtCmd & cmdObj ) :
	m_cmdObj( cmdObj ),
	m_dwRef(0),
	m_dwSessionID(0)
{
	// Try to get pointer to ProcessIDtoSessionID function
	// exported from kernel32.dll.  This only works in 
	// terminal services aware environments.
	typedef BOOL (WINAPI *pfnProcessIDtoSessionID)( DWORD, DWORD* );
	pfnProcessIDtoSessionID pfn;
	HMODULE hMod = GetModuleHandle( _T("kernel32") );
	if( hMod )
	{
		pfn = ( pfnProcessIDtoSessionID ) GetProcAddress( hMod, "ProcessIdToSessionId" );
		if( pfn )
		{
			// Call ProcessIDtoSessionID() routine.
			pfn( GetCurrentProcessId(), &g_dwSessionID );
		}
	}
}

//////////////////////////////////////////////////////////////////////
// CEventHandler::~CEventHandler()

CEventHandler::~CEventHandler()
{
	// Intentionally empty.
}

//////////////////////////////////////////////////////////////////////
// CEventHandler::QueryInterface()

STDMETHODIMP CEventHandler::QueryInterface( REFIID iid, void ** ppvObject )
{
	*ppvObject = NULL;
	
    if( IsEqualIID( iid, IID_IUnknown )||
		IsEqualIID( iid, __uuidof( INAVAPEventHandler ) ) )
	{
		*ppvObject = this;
	}

    if( *ppvObject )
    {
        ((LPUNKNOWN)*ppvObject)->AddRef();
        return S_OK;
    }

    return E_NOINTERFACE;
}

//////////////////////////////////////////////////////////////////////
// CEventHandler::AddRef()

STDMETHODIMP_(ULONG) CEventHandler::AddRef()
{
	return InterlockedIncrement( (long*) &m_dwRef );
}

//////////////////////////////////////////////////////////////////////
// CEventHandler::Release()

STDMETHODIMP_(ULONG) CEventHandler::Release()
{
	if( InterlockedDecrement( (long*) &m_dwRef ) == 0 )
	{
		delete this;
		return 0;
	}

	return m_dwRef;
}

//////////////////////////////////////////////////////////////////////
// CEventHandler::GetSessionID()

STDMETHODIMP CEventHandler::GetSessionID( DWORD* pdwSessionID )
{
    *pdwSessionID = g_dwSessionID;
	return S_OK;
}

//////////////////////////////////////////////////////////////////////
// CEventHandler::OnEvent()

STDMETHODIMP CEventHandler::OnEvent( VARIANT vArrayData )             
{
	// Make sure we have the correct data.
	if( vArrayData.vt != ( VT_ARRAY | VT_UI1 ) )
		return E_INVALIDARG;

	// Convert and insert this into the event queue for processing.
    //
    CEventData* pNewData = new CEventData (vArrayData.parray);
    m_cmdObj.QueueEvent( pNewData );

    
	return S_OK;
}

//////////////////////////////////////////////////////////////////////
// CEventHandler::OnStopped()

STDMETHODIMP CEventHandler::OnStopped( )             
{
	m_cmdObj.OnServiceStopped();
	return S_OK;
}

//////////////////////////////////////////////////////////////////////
// CEventHandler::OnStopped()

STDMETHODIMP CEventHandler::OnStateChanged( DWORD dwEnabled )
{
	// This call ends up updating the config object.
	ApwUpdateUI();
	return S_OK;
}

STDMETHODIMP CEventHandler::OnDefAuthenticationFailure( DWORD dwRetVal )
{
    // Send the def authentication alert to the queue to be displayed
    if( dwRetVal == AUTHENTICATION_FAILED )
    {
        m_cmdObj.DisplayDefAuthenticationFailure(IDS_ERR_NO_AUTH_DEFS);
    }
    else if( dwRetVal == AUTHENTICATION_REVERTED )
    {
        m_cmdObj.DisplayDefAuthenticationFailure(IDS_ERR_DEFS_REVERTED);
    }

    return S_OK;
}