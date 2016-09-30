////////////////////////
//
// PROPRIETARY / CONFIDENTIAL.
// Use of this product is subject to license terms.
// Copyright © 2006 Symantec Corporation.
// All rights reserved.
//
/////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "CEventProvider.h"

class CConfigStates
{
public:
    CConfigStates(void);
    ~CConfigStates(void);
    bool Initialize(void);
    bool SetSymProtectEnabled(bool bEnabled);
    bool SetFileProtection(bool bEnabled, bool bLogOnly);
    bool SetRegistryProtection(bool bEnabled, bool bLogOnly);
    bool SetProcessProtection(bool bEnabled, bool bLogOnly);

private:
    CEventProvider m_EventProvider;
};
