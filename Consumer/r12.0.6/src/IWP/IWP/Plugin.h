#pragma once

#include "syminterface.h"
#include "ccappplugin.h"
#include "SMHelper.h"
#include "EventData.h"
#include "EMSubscriber.h"
#include "StatusPropertyNames.h"
#include "AvEvents.h"

extern const char IWP_MUTEX_SESSIONWIDE[];
extern const char IWP_MUTEX_MACHINEWIDE[];

class CPlugin :
        public ISymBaseImpl< CSymThreadSafeRefCount >,
	    public ICCAppPlugin,
        public CEMSubscriberSink
{
public:
    CPlugin(void);
    virtual ~CPlugin(void);

	SYM_INTERFACE_MAP_BEGIN()               
		SYM_INTERFACE_ENTRY( IID_CCAppPlugin, ICCAppPlugin )
	SYM_INTERFACE_MAP_END()                 

	// ICCAppPlugin overrides.
	virtual void Run( ICCApp* pCCApp );
	virtual bool CanStopNow();
	virtual void RequestStop();
	virtual const char * GetObjectName();
    virtual unsigned long OnMessage( int iMessage, unsigned long ulParam );

    // EMSubscriber
    void EMSubscriberOnEvent (const ccEvtMgr::CEventEx& Event,
                                ccEvtMgr::CSubscriberEx::EventAction& eAction);
    void EMSubscriberOnShutdown ();

private:
    const char* m_szObjectName;
    ccLib::CEvent m_eventShutdown;
    ccLib::CEvent m_eventNewSubscription;
    std::auto_ptr<CEMSubscriber> m_pEMSubscriber;
    long m_lEndDate; // subscription date

    bool initSubData (void);
    bool subscribeToIDS (void);
};
