// EventHandler.h: interface for the CEventHandler class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_EVENTHANDLER_H__0A96BD5E_F3B0_4F1C_973D_77A416B4FC88__INCLUDED_)
#define AFX_EVENTHANDLER_H__0A96BD5E_F3B0_4F1C_973D_77A416B4FC88__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "apwntcmd.h"
#include "navapsvc.h"

class CEventHandler : public INAVAPEventHandler  
{
public:
	CEventHandler( CApwNtCmd & cmdObj );
	virtual ~CEventHandler();

	// IUnknown methods.
	STDMETHOD(QueryInterface)( REFIID iid, void ** ppvObject );
	STDMETHOD_(ULONG,AddRef)();
	STDMETHOD_(ULONG,Release)();

	// INAVAPEventHandler methods.
	STDMETHOD(GetSessionID)( DWORD* pdwSessionID );  
	STDMETHOD(OnEvent)( VARIANT vArrayData );             
	STDMETHOD(OnStopped)();
	STDMETHOD(OnStateChanged)( DWORD dwEnabled );
    STDMETHOD(OnDefAuthenticationFailure) ( DWORD dwRetVal );

private:
	// Disallowed
	CEventHandler();
	CEventHandler( const CEventHandler & );

	// Reference to command object.
	CApwNtCmd&		m_cmdObj;
	
	// Reference count
	DWORD			m_dwRef;

	// Terminal services session ID
	DWORD			m_dwSessionID;
};

#endif // !defined(AFX_EVENTHANDLER_H__0A96BD5E_F3B0_4F1C_973D_77A416B4FC88__INCLUDED_)
