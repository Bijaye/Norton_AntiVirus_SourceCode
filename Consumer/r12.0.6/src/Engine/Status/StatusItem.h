#pragma once

#include "wchar.h"
#include "EventData.h"
#include "StatusPropertyNames.h"

class CStatusItem  
{
public:
    CStatusItem();
	virtual ~CStatusItem();
	
    // Return the Event ID to listen for changes
    //
    long m_lEventID;
   
	// Passes in the new bag of data from StatusHP
    //
    void UpdateStatus ();

    static void Initialize ( CEventData& edNewData );

    static ccLib::CCriticalSection m_critMasterData;
    static CEventData m_edMasterData;

protected:

    // Is it our first load?
    bool m_bInit;

    // These need to be implemented by the derived classes
    //
    virtual void MakeStatus() = 0;
   
};
