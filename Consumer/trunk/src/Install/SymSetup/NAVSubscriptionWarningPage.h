////////////////////////
//
// PROPRIETARY / CONFIDENTIAL.
// Use of this product is subject to license terms.
// Copyright © 2005 Symantec Corporation.
// All rights reserved.
//
/////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "DefaultSubscriptionWarningPage.h"

class CNAVSubscriptionWarningPage: public CDefaultSubscriptionWarningPage
{
	public:
		BOOL GetSubscriptionDays(DWORD &dwDays);

};