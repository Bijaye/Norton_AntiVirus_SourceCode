#pragma once

#include "SymIDSI.h"

class CIDSUpdateWatcher : public ccLib::CThread
{
public:
    CIDSUpdateWatcher(void);
    ~CIDSUpdateWatcher(void);

// CThread
    int Run ();

protected:
    void fireNotification ();
    bool makeEvent ();
    ccLib::CEvent m_event;
};
