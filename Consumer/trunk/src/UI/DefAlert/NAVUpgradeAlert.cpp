////////////////////////
//
// PROPRIETARY / CONFIDENTIAL.
// Use of this product is subject to license terms.
// Copyright © 2006 Symantec Corporation.
// All rights reserved.
//
/////////////////////////////////////////////////////////////////////////////////////////////////

// NAVUpgradeAlert.cpp

#include "StdAfx.h"
#include "Resource.h"
#include "..\defalertres\Resource.h"
#include "NAVSettingsHelperEx.h"
#include "NAVUpgradeAlert.h"
#include "CLTProductHelper.h"
#include "ccAlertLoader.h"
#include "ccAlertInterface.h"
#include "isResource.h"
#include "ISVersion.h"

#include "ccResourceLoader.h"
extern cc::CResourceLoader g_ResLoader;

const char PRODUCT_UPGRADE_LAST_DISPLAY[] = "LastUpgrade";
const char PRODUCT_UPGRADE_FIRST_DISPLAY[] = "FstUpgrade";

using namespace cc;

// Callback class that is used in upgrade notification alert.
class CUpgradeNotifyCallback: 
	public INotifyCallback
	,public ISymBaseImpl<CSymThreadSafeRefCount>
{
public:
	SYM_INTERFACE_MAP_BEGIN()
		SYM_INTERFACE_ENTRY(IID_NotifyCallback, INotifyCallback)
	SYM_INTERFACE_MAP_END()

	virtual bool Run(HWND hWndParent, unsigned long nLinkID, INotify* pAlert)
	{
		CCTRACEI(_T("Begin CUpgradeNotifyCallback::Run"));

		STAHLSOFT_HRX_TRY(hr)
		{
			CCTRACEI(_T("If an upgrade balloon is currently active, hide it."));

			if(pAlert)
				pAlert->Hide();

			CCTRACEI(_T("Get mega upgrade alert info."));
			CCLTProductHelper cltHelper;
			hrx << cltHelper.Initialize();
			hrx << cltHelper.LaunchAlert(SYMALERT_TYPE_LicProductUpgrade);
		}		
		catch(_com_error& e)
		{
			CCTRACEE(_T("Error while launching upgrade mega alert 0x%08X"), e.Error());
		}

		return true;
	}
};

HRESULT CNAVUpgradeAlert::CheckProductUpgrade()
{
	CCTRACEI(_T("Begin CNAVUpgradeAlert::CheckProductUpgrade()"));

	// If aggregated, don't show alert.
	DWORD dwAggregated = 0;	
	
	READ_SETTING_START()
		READ_SETTING_DWORD(LICENSE_Aggregated, dwAggregated, 0)
	READ_SETTING_END

	if(dwAggregated)
	{
		CCTRACEI(_T("Suppress upgrade alert since NAV is a child product."));
		return S_OK;
	}

	// This is temporary until we find where this code is declared and assigned.
	const int UPGRADE_MESSAGE_CODE = 200;
	
	STAHLSOFT_HRX_TRY(hr)
	{
		// Upgrade notification data
		DWORD dwMessageCode = 0;
		DWORD dwShow_N_Days = 0;
		DWORD dwShow_Every_N_Days = 0;
		DWORD dwAuto_Hide_Seconds = 0;
		DWORD dwFirstTime = 0;		// Stores the Current Time
		DWORD dwLastTime = 0;		// Stores the First time we popped the alert

		CCTRACEI(_T("Get upgrade notification data data"));

        CCLTProductHelper cltHelper;
        if(!SUCCEEDED(hr = cltHelper.Initialize()))
        {
            CCTRCTXE1(_T("Could not initialize CLT Helper. HR = 0x%08X"), hr);
            return hr;
        }
        hrx << cltHelper.GetLicenseDwordProperty(clt::szSCSSMessageCode, 
                                                dwMessageCode);
		hrx << cltHelper.GetLicenseDwordProperty(clt::szSCSSShowNDaysTotal, 
                                                dwShow_N_Days);
		hrx << cltHelper.GetLicenseDwordProperty(clt::szSCSSShowEveryNDays, 
                                                dwShow_Every_N_Days);
		hrx << cltHelper.GetLicenseDwordProperty(clt::szSCSSBalloonAutoHideSeconds, 
                                                dwAuto_Hide_Seconds);

		CCTRACEI(_T("message_code=%d"), dwMessageCode);
		CCTRACEI(_T("show_n_days_total=%d"), dwShow_N_Days);
		CCTRACEI(_T("show_every_n_days=%d"), dwShow_Every_N_Days);
		CCTRACEI(_T("balloon_auto_hide_seconds=%d"), dwAuto_Hide_Seconds);

		if (dwMessageCode == UPGRADE_MESSAGE_CODE)
		{
			// Check to see if this is our first time by searching for first time display in alert cache.
			// First time alert, pop up mega nag.
			// Subsequent alerts, pop up bubble nag first.
			long nFirstDisplay = _GetAlertDword(PRODUCT_UPGRADE_FIRST_DISPLAY, 0);  

			if(nFirstDisplay == 0)
			{
				// First time alert, pop up mega nag using callback.
				CCTRACEI(_T("First notification upgrade alert - show mega nag."));

				CUpgradeNotifyCallback cb;
				cb.Run(NULL, 0, NULL);

				// Set First Time and Last Time in the data store.
				time_t currentTime;
				time(&currentTime);
				_SetAlertDword(PRODUCT_UPGRADE_FIRST_DISPLAY, currentTime);
				_SetAlertDword(PRODUCT_UPGRADE_LAST_DISPLAY, currentTime);
			}
			else
 			{

				CCTRACEI(_T("Subsequent notification upgrade alert - check to see if it's time to display bubble nag."));

				// Now check to see if it is time to show another alert?

				// 1) See if the duration between now and the last alert is beyond
				// the show_every_n_days length
			
				// 2) See if we have been popping up the alerts for xxx days
				// Check against show_n_days
				
				time_t currentTime;
				time(&currentTime);

				dwFirstTime = _GetAlertDword(PRODUCT_UPGRADE_FIRST_DISPLAY, 0);
				dwLastTime = _GetAlertDword(PRODUCT_UPGRADE_LAST_DISPLAY, 0);

				CCTRACEI(_T("Subsequent notification upgrade alert - FirstDisplay:%d, LastDisplay:%d, CurrentTime:%d")
					,dwFirstTime, dwLastTime, currentTime);

				if (((currentTime - dwLastTime) > dwShow_Every_N_Days * _1_DAY) && 
					((currentTime - dwFirstTime) < dwShow_N_Days * _1_DAY))
				{
					CCTRACEI(_T("Display bubble nag."));

					CString csProductName, csFormat, csText;
					ccLib::CString csTitle;
					
					CISVersion::LoadString(csTitle, IDS_SYMANTEC);
					csProductName = CISVersion::GetProductName();
					g_ResLoader.LoadString(IDS_UPGRADE_NOTIFICATION_SHORTTEXT, csFormat);
					csText.Format(csFormat, csProductName);


					// And here is that bubble...
					cc::ccAlert_INotify2 ccNotify;

					// ccNotify Box
					cc::INotify2Ptr pNotify;
					cc::INotify2QIPtr pNotify2;

					CCTRACEI(_T("Initialize and Create alert object"));
					// Create the UI object.
					if (SYM_FAILED(ccNotify.Initialize()) ||
						SYM_FAILED(ccNotify.CreateObject(pNotify.m_p)) ||
						pNotify == NULL)
					{

						CCTRACEE(_T("CNAVUpgradeAlert::CheckProductUpgrade - Failed to create ccNotify object"));
						hrx << E_FAIL;
					}

					// Set properties
					CUpgradeNotifyCallback cb;

					hrx << (!pNotify->SetHyperlinkCallback(&cb)? E_FAIL : S_OK);
					hrx << (!pNotify->SetTitle(csTitle)? E_FAIL : S_OK);
					hrx << (!pNotify->SetText(csText)? E_FAIL : S_OK);
					hrx << (!pNotify->SetTimeout(dwAuto_Hide_Seconds * 1000)? E_FAIL : S_OK);
					pNotify->Display(NULL);
					
					// Set last display time
					_SetAlertDword(PRODUCT_UPGRADE_LAST_DISPLAY, currentTime);
				}
				else
				{
					CCTRACEI(_T("It's not time to display bubble nag yet."));
				}
			}
		}
	}
	catch(_com_error& e)
	{
		hr = e.Error();
		CCTRACEE(_T("Exception thrown - 0x%08X"), hr);
	}

	return hr;
}
