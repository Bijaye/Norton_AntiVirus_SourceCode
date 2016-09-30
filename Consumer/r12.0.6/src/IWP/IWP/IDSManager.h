#pragma once
#include "SymInterface.h"   // for SYMRESULT
#include "SymIDSI.h"
#include "IDSLoader.h"

class CIDSManager
{
public:
    CIDSManager(void);
    ~CIDSManager(void);

    SYMRESULT ResetIDSSettings(LPCSTR lpszProductName = NULL, UINT64 ui64ProductVersion = 0);

    // Call Subscribe() for installs or for updates. 
    // IDSManager will figure out what state we are in for you.
    //
    SYMRESULT Subscribe (time_t timeExpiredDateTime);
    SYMRESULT Unsubscribe ();

    // Set bOn to false for "disable"
    SYMRESULT Enable (bool bOn);

protected:
    std::string m_strLocalProductName;
	UINT64 m_ui64LocalVersion;

    SYMRESULT initSubscriptions ();
    SYMRESULT updateSubscription ( time_t timeExpiredDateTime, BOOL bExpired);

    CIDSLoader m_IDSLoader;
    CSymPtr<SymIDS::IIDSSubscription> m_pIDSSub;
};
