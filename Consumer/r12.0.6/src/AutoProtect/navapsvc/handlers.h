#ifndef _HANDLERS_H_
#define _HANDLERS_H_

#include "ccCriticalSection.h"

/////////////////////////////////////////////////////////////////////////////
// Class declaration for CEventHandlers

// Forward declartion
struct INAVAPEventHandler;

class CEventHandlers
{
public:
	CEventHandlers();
	virtual ~CEventHandlers();

	// Handler manipulation routines.
	void InsertHandler(INAVAPEventHandler* pHandler);
	void RemoveHandler(INAVAPEventHandler* pHandler);
	void RemoveAllHandlers();
	INAVAPEventHandler* LookupHandler( DWORD dwSessionID );

	// Broadcast stop routine.
	void SendStopNotification();

	// Broadcast state change routine.
	void SendStateChange( bool bEnabled );

    // Broadcast data to all handlers
    //
    void BroadcastData ( VARIANT& variant );

    // Broadcast definition authentication failed or caused
    // definitions to revert to an older set
    void SendDefAuthenticationFailure(DWORD dwFailure);

private:
	// Handler map.
    typedef std::map< DWORD, INAVAPEventHandler* > HANDLER_MAP;
	HANDLER_MAP m_Handlers;

	// Critical section used to guard handler map.
	ccLib::CCriticalSection m_csHandlers;

    // If this is not -1 then we need to alert the agent handler that there was
    // a def authentication failure
    DWORD m_dwAuthenticationFailure;
};

#endif // ifdef HANDLERS_H_