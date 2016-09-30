#pragma once

#include "IWPSettingsInterface.h"   // For IWP
#include "IWPSettingsLoader.h"
#include "StatusItem.h"

// Sadly, I can't put the sink for this interface into CNAVStatus because the COM macros (AddRef)
// interfere with the SymInterface ones.
//
class CStatusIWP :
    public CStatusItem
{
public:
    CStatusIWP(void) throw (std::runtime_error);
    ~CStatusIWP(void);

//CStatusItem
    void MakeStatus();
};
