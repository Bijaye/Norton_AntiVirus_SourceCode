#include "stdafx.h"
#include "navapsvc.h"

/////////////////////////////////////////////////////////////////////////////
// CEventHandlers::CEventHandlers()

CEventHandlers::CEventHandlers() : m_dwAuthenticationFailure(-1)
{
	// Intentionally empty.
}

/////////////////////////////////////////////////////////////////////////////
// CEventHandlers::~CEventHandlers()

CEventHandlers::~CEventHandlers()
{
	RemoveAllHandlers();
}

/////////////////////////////////////////////////////////////////////////////
// CEventHandlers::InsertHandler()

void CEventHandlers::InsertHandler(INAVAPEventHandler* pHandler)
{
	ccLib::CSingleLock lock( &m_csHandlers, INFINITE, FALSE );

	// Obtain the session ID for this handler.
	DWORD dwSessionID = 0;
	pHandler->GetSessionID( &dwSessionID );
    CCTRACEI ( _T("CEventHandlers::InsertHandler : Adding Agent for session - %d"), dwSessionID);

	// If we already have a handler for this session, release it
	HANDLER_MAP::iterator it = m_Handlers.find( dwSessionID );
	if( it != m_Handlers.end() )
	{
		try
		{
			(*it).second->Release();
            CCTRACEI ( _T("CEventHandlers::InsertHandler : Agent already in list"));
		}
		catch(...)
		{
			// Don't really care if this blows up.  Client could
			// have terminated without unregistering handler.
			_ASSERT(FALSE);
		}
	}

	try
	{
		// Add the handler to the map.
		m_Handlers[ dwSessionID ] = pHandler;
		pHandler->AddRef();
        CCTRACEI ( _T("CEventHandlers::InsertHandler : Adding Agent for session - %d - SUCCESSFUL"), dwSessionID);

        // Notify the agent of def authentication failure if necessary
        if( -1 != m_dwAuthenticationFailure )
        {
            CCTRACEI ( _T("CEventHandlers::InsertHandler : Sending queued def authentication failure to session - %d"), dwSessionID);
            SendDefAuthenticationFailure(m_dwAuthenticationFailure);
            m_dwAuthenticationFailure = -1;
        }
	}
	catch(...)
	{
		// Don't really care if this blows up.  Client could
		// have supplied us with a bogus pointer.
		_ASSERT(FALSE);
	}
}

/////////////////////////////////////////////////////////////////////////////
// CEventHandlers::RemoveHandler()

void CEventHandlers::RemoveHandler(INAVAPEventHandler* pHandler)
{
    ccLib::CSingleLock lock( &m_csHandlers, INFINITE, FALSE );

	// Obtain the session ID for this handler.
	DWORD dwSessionID = 0;
	pHandler->GetSessionID( &dwSessionID );

	// If we already have a handler for this session, release it
	HANDLER_MAP::iterator it = m_Handlers.find( dwSessionID );
	if( it != m_Handlers.end() )							  
	{
		try
		{
			(*it).second->Release();
		}
		catch(...)
		{
			// Don't really care if this blows up.  Client could
			// have terminated without unregistering handler.
			_ASSERT(FALSE);
		}
		
		// Remove handler from map.								   
		m_Handlers.erase( dwSessionID );
	}
}

/////////////////////////////////////////////////////////////////////////////
// CEventHandlers::RemoveAllHandlers()

void CEventHandlers::RemoveAllHandlers()
{
    ccLib::CSingleLock lock( &m_csHandlers, INFINITE, FALSE );

	// Release all handlers
	HANDLER_MAP::iterator it;
	for( it = m_Handlers.begin(); it != m_Handlers.end(); ++it )
	{
		try
		{
			(*it).second->Release();
		}
		catch(...)
		{
			// Don't really care if this blows up.  Client could
			// have terminated without unregistering handler.
			_ASSERT(FALSE);
		}
	}

	// Remove all entries.
	m_Handlers.clear();
}

/////////////////////////////////////////////////////////////////////////////
// CEventHandlers::RemoveAllHandlers()

INAVAPEventHandler* CEventHandlers::LookupHandler( DWORD dwSessionID )
{
	INAVAPEventHandler* pRet = NULL;		
	
    ccLib::CSingleLock lock( &m_csHandlers, INFINITE, FALSE );

	// Look up handler in map.
	HANDLER_MAP::iterator it = m_Handlers.find( dwSessionID );
	if( it != m_Handlers.end() )							  
	{
		try
		{
			pRet = (*it).second;
			pRet->AddRef();		
		}
		catch(...)
		{
			pRet = NULL;
		}
	}

	return pRet;
}

/////////////////////////////////////////////////////////////////////////////
// CEventHandlers::SendStopNotification()

void CEventHandlers::SendStopNotification()
{
	HANDLER_MAP::iterator it;
	for( it = m_Handlers.begin(); it != m_Handlers.end(); ++it )
	{
		try
		{
			(*it).second->OnStopped();
		}
		catch(...)
		{
			// Don't really care if this blows up.  Client could
			// have terminated without unregistering handler.
			_ASSERT(FALSE);
		}
	}
}

/////////////////////////////////////////////////////////////////////////////
// CEventHandlers::SendStateChange()

void CEventHandlers::SendStateChange( bool bEnabled )
{
	HANDLER_MAP::iterator it;
	for( it = m_Handlers.begin(); it != m_Handlers.end(); ++it )
	{
		try
		{
			(*it).second->OnStateChanged( bEnabled ? TRUE : FALSE );
		}
		catch(...)
		{
			// Don't really care if this blows up.  Client could
			// have terminated without unregistering handler.
			_ASSERT(FALSE);
		}
	}
}

/////////////////////////////////////////////////////////////////////////////
// CEventHandlers::BroadcastData()

void CEventHandlers::BroadcastData ( VARIANT& variant )
{
	HANDLER_MAP::iterator it;
    
    CCTRACEI ( _T("NAVAPSVC : Sending data to %d sessions"),m_Handlers.size ());

	for( it = m_Handlers.begin(); it != m_Handlers.end(); it++ )
	{
		try
		{
			INAVAPEventHandler* pHandler = (*it).second;
            if ( pHandler )
            {
                DWORD dwSessionID = 0;
                pHandler->GetSessionID( &dwSessionID );
                CCTRACEI ( _T("NAVAPSVC : Sending data to session - %d"), dwSessionID );
                pHandler->OnEvent( variant );
            }
		}
		catch(...)
		{
			// Don't really care if this blows up.  Client could
			// have terminated without unregistering handler.
			_ASSERT(FALSE);
            CCTRACEE (_T("NAVAPSVC : CEventHandlers::BroadcastData"));
		}
	}
}

/////////////////////////////////////////////////////////////////////////////
// CEventHandlers::SendDefAuthenticationFailure()
void CEventHandlers::SendDefAuthenticationFailure(DWORD dwFailure)
{
    // If there are no handlers to send the failure to queue it up. This will be the case
    // when the authentication fails during the service startup
    if( m_dwAuthenticationFailure == -1 && m_Handlers.size() == 0 )
    {
        CCTRACEI ( _T("CEventHandlers::SendDefAuthenticationFailure() : There are no handlers to recieve the authentication failure yet. Queueing the event.") );
        m_dwAuthenticationFailure = dwFailure;
        return;
    }

    HANDLER_MAP::iterator it;
    
    CCTRACEI ( _T("CEventHandlers::SendDefAuthenticationFailure() : Sending data to %d sessions"),m_Handlers.size ());

	for( it = m_Handlers.begin(); it != m_Handlers.end(); it++ )
	{
		try
		{
			INAVAPEventHandler* pHandler = (*it).second;
            if ( pHandler )
            {
                DWORD dwSessionID = 0;
                pHandler->GetSessionID( &dwSessionID );
                CCTRACEI ( _T("CEventHandlers::SendDefAuthenticationFailure() : Sending data to session - %d"), dwSessionID );
                pHandler->OnDefAuthenticationFailure(dwFailure);
            }
		}
		catch(...)
		{
			// Don't really care if this blows up.  Client could
			// have terminated without unregistering handler.
			_ASSERT(FALSE);
            CCTRACEE (_T("CEventHandlers::SendDefAuthenticationFailure() : Unhandled exception sending def authentiction failure notification"));
		}
	}
}