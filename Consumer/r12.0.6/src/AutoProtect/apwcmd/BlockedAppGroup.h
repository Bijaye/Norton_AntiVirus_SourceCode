#pragma once

// STL
#include <string>

// This project
#include "BlockedAppUI.h"
#include "eventdata.h"

// Forward declaration.
class CBlockedAppGroup;

class CBlockedAppGroupSink
{
public:
    CBlockedAppGroupSink (){};
    virtual ~CBlockedAppGroupSink (){};

    // Called when the UI closes
    //
    virtual void OnBAGUIClosed (CBlockedAppGroup* pBAG) = 0;
};

// Also known as a CBAG. If the events are the same, except for the Cookie,
// we just group up the cookies together into the same Event Data. We could
// do this with a list of EventData objects, but this saves a lot of memory.
//
class CBlockedAppGroup : 
    public CBlockedAppUISink
{
public:
    CBlockedAppGroup (CEventData& EventData, CBlockedAppGroupSink* pBAGSink = NULL);
    virtual ~CBlockedAppGroup(void);

    // Comparison operator
    //
    bool operator==(const CBlockedAppGroup&) const;

    // Do whatever you need to do for a duplicate item coming in.
    // There's no parameters because it's a dup, they are the same
    // as what's in the BAG.
    //
    void AddDup ();

    // Display the UI, if there's room.
    // Returns true if it can show the UI, false if it's already showing.
    //
    bool Show ();

    // Returns true if the item matches.
    //
    bool Remove ( /*in*/ CEventData& EventData );

    // BAUI Sink
    //
    void OnBlockedAppUICancel();
    void OnBlockedAppUIClosed();

    static void SetWaitBeforeDisplay ( DWORD dwBlockedDelay );

protected:   
    // Time since we displayed the UI, in milliseconds
    //
    CSimpleTimer m_timeSinceCreation;
    static DWORD m_dwBlockedDelay;

    // Hold onto these for the UI
    std::wstring m_strPath;
    long m_lProcessID;

    CBlockedAppGroupSink* m_pBAGSink;

    // Cookies for this BAG
    SAVRT_ROUS64 m_hCookie;

    // UI object
    CBlockedAppUI* m_pUI;

    // Denied!
    CBlockedAppGroup(){};

    // Did we tell the UI to close?
    bool m_bClosingUI;
};