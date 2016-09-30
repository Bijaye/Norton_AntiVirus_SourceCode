#pragma once

#include "ccappplugin.h"


class CPlugin :
        public ISymBaseImpl< CSymThreadSafeRefCount >,
	    public ICCAppPlugin
{
public:
    CPlugin(void);
    virtual ~CPlugin(void);

	SYM_INTERFACE_MAP_BEGIN()               
		SYM_INTERFACE_ENTRY( IID_CCAppPlugin, ICCAppPlugin )
	SYM_INTERFACE_MAP_END()                 

	// ICCAppPlugin overrides.
    virtual void Run( ICCApp* pCCApp ){};
	virtual bool CanStopNow();
	virtual void RequestStop();
	virtual const char * GetObjectName();
    virtual unsigned long OnMessage( int iMessage, unsigned long ulParam );

    const char* m_szObjectName;


protected:
    ccLib::CEvent m_eventShutdown;
	bool m_shutdown;
};
