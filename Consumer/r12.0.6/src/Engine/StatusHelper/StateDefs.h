#pragma once

#include "NAVDefutilsLoader.h"
#include "State.h"  // Common state utils
#include "ccThread.h"

class CStateDefs : 
    public CState
{
public:
	CStateDefs(CWMIIntegration* pWMI, CNSCIntegration* pNSC);
	virtual ~CStateDefs();

	// CThread overrides/implementation
    int Run();

    bool Save();

private:
	// Disallowed
	CStateDefs( CStateDefs& other );

	enum {WAIT_INTERVAL = 1000 };
	enum {UPDATE_INTERVAL = 30000 };

    // Persist data for defs
    bool save ();

    long m_lOldStatus;
    time_t m_timeOldDefTime;
    WCHAR m_szOldDefsDate [64];
    long m_lUpdateReason;

    // Full path to our virus defs
    //
    char m_szDefsDir [MAX_PATH];

    ccLib::CEvent m_eventDefs;
};