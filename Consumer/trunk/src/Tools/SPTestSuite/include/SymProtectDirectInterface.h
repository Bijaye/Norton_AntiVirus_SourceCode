////////////////////////
//
// PROPRIETARY / CONFIDENTIAL.
// Use of this product is subject to license terms.
// Copyright © 2006 Symantec Corporation.
// All rights reserved.
//
/////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "SymInterface.h"

namespace SymProtectEvt
{

class IFallbackConfig : public ISymBase
{
public:
    enum FALLBACK_CONFIG
    {
        USE_LAST_KNOWN_GOOD,
        DISABLE_PROTECTION
    };

    // When SymProtect is unable to load its configuration at load time,
    // these functions specify what action is taken
    virtual SYMRESULT GetFallbackAction(FALLBACK_CONFIG& eAction) const = 0;
    virtual SYMRESULT SetFallbackAction(FALLBACK_CONFIG eAction) = 0;
};
// {F4F7F408-8196-40d3-8C4F-7A386A8E1156}
SYM_DEFINE_INTERFACE_ID(IID_FallbackConfig, 
0xf4f7f408, 0x8196, 0x40d3, 0x8c, 0x4f, 0x7a, 0x38, 0x6a, 0x8e, 0x11, 0x56);

typedef CSymPtr<IFallbackConfig>    IFallbackConfigPtr;
typedef CSymQIPtr<IFallbackConfig, &IID_FallbackConfig> IFallbackConfigQIPtr;

} // namespace