#pragma once

#include "SymFWAgtInterface.h"
#include "IWPSettingsInterface.h"
#include "ccThread.h"
#include "IDSUpdateWatcher.h"
#include "IWPSettings.h"
#include "SymInterfaceLoader.h"
#include "IWPSettingsLoader.h"      // For CIWPPathProvider

#include <memory>       // for auto_ptr

class CFirewallAlerts : 
    public ISymBaseImpl< CSymThreadSafeRefCount >,
    public ccLib::CThread,
    public IAlertSink
{
public:
	SYM_INTERFACE_MAP_BEGIN()
		SYM_INTERFACE_ENTRY(IID_AlertSink, IAlertSink )
	SYM_INTERFACE_MAP_END()    
    
    CFirewallAlerts(void);
    virtual ~CFirewallAlerts(void);

// CThread methods
    int Run ();
    
// IAlertSink methods    
    virtual bool OnDisplayAlert(IAlertData* pAlertInfo);

// Called by subscriptions
    //void SetIDSState (bool bIDSOK);

protected:
    typedef CSymInterfaceDLLHelper<&cc::sz_SymFWAgt_dll,
                                    CIWPPathProvider, // NAV Version!
                                    cc::CSymInterfaceTrustedLoader,
                                    ISymFWAgt, 
                                    &IID_SymFWAgt, 
                                    &IID_SymFWAgt> SymFWAgtLoader;

    // You better get this party started.
    bool start ();
    bool stop ();
    ccLib::CCriticalSection m_critFW;
    
    // Is IWP supposed to be on?
    bool isEnabled ();
    
    // Is the alert agent running?
    bool m_bIsRunning;

    // ccLogin support
    //
    void login();
    void logout();

    std::auto_ptr <CIWPSettings> m_pSettings;

    // Events to wait on. Commands from IWPSettings.
    ccLib::CEvent m_eventStateChange;

    // Internal event when we need to refresh
    ccLib::CEvent m_eventResetAgent;         

    std::auto_ptr <SymFWAgtLoader> m_pFWLoader;
    ISymFWAgtPtr m_pSymFwAgt;
    DWORD m_dwID;
    //bool m_bIDSOK;

    // IDS update monitor
    CIDSUpdateWatcher m_IDSUpdateWatcher;
};
