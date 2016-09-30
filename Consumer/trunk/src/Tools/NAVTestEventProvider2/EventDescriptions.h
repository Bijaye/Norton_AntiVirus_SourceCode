////////////////////////
//
// PROPRIETARY / CONFIDENTIAL.
// Use of this product is subject to license terms.
// Copyright © 2006 Symantec Corporation.
// All rights reserved.
//
/////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "EventData.h"

#include <vector>

typedef std::vector <ccLib::CString> vecEventTypes;

// Description of each Event - holds all the other events
//
class CEventDescriptions
{
public:
    CEventDescriptions(void);
    ~CEventDescriptions(void);

    enum InternalEventIDs
    {
        FileVirus = 0,
        Spyware,
        Mixed, // one virus, one spyware
        VirusLike,
        //ScriptBlocking,  // Took it out for NAV 2006
        ContainerVirus,
        ContainerSpyware,
        ContainerMixed, // one virus, one spyware
        BRVirus,
        MBRVirus,
        Scan,
        Error,
        // Add OEH alert too - see ScanMgr/ActivityLog.cpp
        LAST_EVENT // always last so we can iterate
    };

    vecEventTypes m_vecEventTypes;

    static bool MakeEvent ( CEventData& ReturnEvent /*out*/,
                            long lInternalEventID /*in*/,
                            LPCSTR szCustomText /*in*/);  
protected:
    static long m_lSessionID;
    static ccLib::CString m_strVirusName;
    static ccLib::CString m_strSpywareName;
    static long m_lVirusID;
};

