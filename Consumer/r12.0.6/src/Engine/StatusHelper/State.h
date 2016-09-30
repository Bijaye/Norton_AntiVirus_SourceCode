#pragma once

#include "EventData.h"
#include "AllNavEvents.h"
#include "AvEvents.h"
#include "StatusPropertyNames.h"

#include "NAVTrust.h"
#include "NAVSettingsHelperEx.h"    // For options

// We need to call WMI/NSC when a change happens
#include "WMIIntegration.h"
#include "NSCIntegration.h"

// Work items that are invoked when clients request data or when changes occur.
//
class CState : public ccLib::CThread
{
public:
    CState(CWMIIntegration* pWMI, CNSCIntegration* pNSC);
    virtual ~CState(void);

    // CThread
    int Run ();
    BOOL InitInstance();

    // CState - for windows event updates
    
    static void GetCurrentStatus ( CEventData& edCurrentStatus );

    long m_lEventID;            // What CC event ID is this for?

    // Derived classes must implement:
    //
    // Save - Called when an update has occurred. Save the new data.
    //      - Returns true if there was a change in the state.
    //
    virtual bool Save () = 0;

    // Called before GetCurrentStatus. Override this if you need
    // to always refresh the state. See defect # 1-2USCHR.
    //
    virtual void HardRefresh (){return;}

    // Time-to-refresh. Derived classes can overwrite this value
    // to implement a timed refresh for time-sensitive data. 1-4IPLJJ
    DWORD m_dwTimeout;

protected:
    CState( CState& other ){};
    CState(void){};
    
    // Broadcast changes to subscribers.
    void sendData (void);

    // Short date format
    void formatDate ( long lProperty, ATL::CTime time );

    // For most objects they listen for Windows events
    //

    // Called to get the handle to wait on.
    //
    std::vector<HANDLE> m_vecWaitEvents;
    bool makeEvent ( LPCSTR szEventName, BOOL bGlobal, ccLib::CEvent& event );

    static CEventData m_edStatus;                // Master container for all the status data for this feature
    static ccLib::CCriticalSection m_critStatus; // Crit section for the edStatus container

    // For our null dacl
    static ATL::CDacl m_nulldacl;
    static ATL::CSecurityDesc m_secdesc;
    static ATL::CSecurityAttributes m_secatt;

    // Integration
    CWMIIntegration* m_pWMI;
    CNSCIntegration* m_pNSC;
    bool m_bInit;

    // Persisted status
    static bool m_bStatusLoaded;
    static bool m_bLoadedOld;   // Was there old data at startup?
    void loadPersistedStatus ();
    void savePersistedStatus ();
};
