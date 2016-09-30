////////////////////////
//
// PROPRIETARY / CONFIDENTIAL.
// Use of this product is subject to license terms.
// Copyright © 2006 Symantec Corporation.
// All rights reserved.
//
/////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "NAVInfo.h"
#include "cltProductHelper.h"

// This object collects subscription and licensing data from PEP.
// It uses the NAV License COM wrapper so you don't have to link with PEP directly.
// NAV License has it's own internal copy of the data so if the data
// refreshes and you want the latest data you must destroy and recreate this
// object.
//
// INITIALIZE COM *STA* ON THE CALLING THREAD!! (unless NAV License gets updated to use Both)
//
class CSubscription
{
public:
    CSubscription(void);
    virtual ~CSubscription(void);

    HRESULT LaunchSubscriptionWizard(HWND hWndParent = NULL );
    HRESULT HasUserAgreedToEULA(BOOL* pbAgreed);
    HRESULT IsCfgWizFinished(BOOL* pbFinished);

protected:
    bool init (void);
};
