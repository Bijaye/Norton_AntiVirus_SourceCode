////////////////////////
//
// PROPRIETARY / CONFIDENTIAL.
// Use of this product is subject to license terms.
// Copyright © 2005 Symantec Corporation.
// All rights reserved.
//
/////////////////////////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "NAVSubscriptionWarningPage.h"
#include "CLTProductHelper.h"
#include "V2LicensingAuthGuids.h"
#include "ProductSuiteOwnerGUID.h"

BOOL CNAVSubscriptionWarningPage::GetSubscriptionDays(DWORD &dwDays)
{
	unsigned long lDays = 0;
	STAHLSOFT_HRX_TRY(hr)
	{
		CCLTProductHelper CLTHelper;
		clt::cltLicenseAttribs cltAttrib = clt::cltLicenseAttrib_Violated;
		hrx << CLTHelper.Initialize();
		hrx << CLTHelper.StartLocalCaching();
		hrx << CLTHelper.GetLicenseAttributes(cltAttrib);
		//Find out the days remaining only if the Product is activated, else we will
		//get trial days remaining.
		if( (clt::cltLicenseAttrib_Active & cltAttrib) == clt::cltLicenseAttrib_Active ) 
		{
			hrx << CLTHelper.GetDaysRemaining(lDays);
			dwDays = static_cast<DWORD>(lDays);
		}
		hrx << CLTHelper.StopLocalCaching();
	}
	STAHLSOFT_HRX_CATCH_ALL_NO_RETURN(hr);

	if(FAILED(hr))
	{
		CCTRACEE(_T("CNISSubscriptionWarningPage::GetSubscriptionDays() - Failed to query subscription days 0x%x"),hr);
        return FALSE;
	}
	else
		return TRUE;
}