// NAVUpgradeAlert.cpp

#include "StdAfx.h"
#include <simon.h>
#include "Resource.h"
#include "..\defalertres\Resource.h"
#include "NAVSettingsHelperEx.h"
#include "NAVUpgradeAlert.h"
#include "cltLicenseConstants.h"
#include "ScriptSafeVerifyTrustCached.h"		// For scripting security
#include "ccSymCommonClientInfo.h"
#include "ccAlertLoader.h"
#include "ccAlertInterface.h"
#include "NavLicense_h.h"
#include "ccWebWnd_h.h"			
#include "StartBrowser.h"
#include "NavLicenseNames.h"
#include "Avres.h"
#include "NavTrust.h"
#include "AVRESBranding.h"

// PEP Headers
#define _V2AUTHORIZATION_SIMON
#include "V2Authorization.h"
#define _SIMON_PEP_
#include "pepClientBase.h" 
#include "cltPepConstants.h"
#include "DRMNamedProperties.h"
#include "SimonCollectionImpl.h"
#include "cltSubscriptionConstants.h"
#include "Subscription_static.h"

#include "ccResourceLoader.h"
extern cc::CResourceLoader g_ResLoader;

const TCHAR PRODUCT_UPGRADE_LAST_DISPLAY[] = _T("LastUpgrade");
const TCHAR PRODUCT_UPGRADE_FIRST_DISPLAY[] = _T("FstUpgrade");

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

			// Get mega nag URL from DJSAlert.dll
			CNAVInfo NAVInfo;
			RECT rectHTML = {0};
			CString csHTML;
			CString csResDLL;
			StahlSoft::CSmartModuleHandle shMod;

			csResDLL.Format(_T("%s\\DJSAlert.dll"), NAVInfo.GetNAVDir());

			CCTRACEI(_T("Loading %s"), csResDLL);
			shMod = LoadLibrary(csResDLL);
			hrx << ((shMod == NULL) ? E_FAIL : S_OK);
		
			CCTRACEI(_T("Get mega upgrade alert html"));
			typedef bool (WINAPI* PFGETHTMLINFO)(SYMALERT_TYPE, LPTSTR, RECT&);
			PFGETHTMLINFO pfGetHTMLInfo = NULL;
			pfGetHTMLInfo = (PFGETHTMLINFO)(GetProcAddress(shMod, "_GetAlertHTMLInfo@12"));
			hrx << ((pfGetHTMLInfo == NULL) ? E_FAIL : S_OK);
			hrx << ((pfGetHTMLInfo(SYMALERT_TYPE_LicProductUpgrade, csHTML.GetBufferSetLength(1024), rectHTML) == false)? E_FAIL : S_OK);
			CCTRACEI(_T("UpgradeAlert=%s, width=%d, height=%d"), csHTML, rectHTML.right, rectHTML.bottom);

			// Check signature of NAVLicense and ccWebWnd before using it.
			hrx << ((NAVToolbox::NAVTRUST_OK != NAVToolbox::IsSymantecComServer(CLSID_NAVLicenseInfo)) ? E_FAIL : S_OK);
			hrx << ((NAVToolbox::NAVTRUST_OK != NAVToolbox::IsSymantecComServer(CLSID_ccWebWindow)) ? E_FAIL : S_OK);

			CComPtr<INAVLicenseInfo> spNavLicense;
			CComPtr<ISymScriptSafe> spSymScriptSafe;
			CComPtr<IDispatch> spDispatch;
			CComPtr<IccWebWindow> spWebCtrl;

			// Get an IDispatch pointer of NAVLicense and pass it to mega nag.
			// Mega nag will use it to retrieve product name. 		
			CCTRACEI(_T("Fetching product IDispatch pointer."));
			hrx << spNavLicense.CoCreateInstance(CLSID_NAVLicenseInfo, NULL, CLSCTX_ALL);
			hrx << spNavLicense.QueryInterface(&spSymScriptSafe);
			hrx << spSymScriptSafe->SetAccess(ISYMSCRIPTSAFE_UNLOCK_KEYA, ISYMSCRIPTSAFE_UNLOCK_KEYB);
			hrx << spNavLicense.QueryInterface(&spDispatch);

			CComBSTR bstrLink(csHTML);
			_variant_t varObjectArg(spDispatch);
			_variant_t vRetVal;
			_variant_t vInput;

			// Show mega nag.			
			CCTRACEI(_T("Show mega nag."));
			hrx << spWebCtrl.CoCreateInstance(CLSID_ccWebWindow, NULL, CLSCTX_ALL);
			hrx << spWebCtrl->SetIcon((long) _Module.GetModuleInstance(), NAVW_ICO);
			hrx << spWebCtrl->put_FrameWindowStyle(WS_SYSMENU | WS_CAPTION);
			hrx << spWebCtrl->put_ObjectArg(varObjectArg);
			hrx << spWebCtrl->showModalDialogEx(NULL, bstrLink, rectHTML.right, rectHTML.bottom, &vInput, &vRetVal);
			CCTRACEI(_T("User selection = %d"), vRetVal.lVal);

			if(vRetVal.lVal == SYMALERT_RESULT_UPGRADE)
			{
				if(m_csDownloadUrl.GetLength() > 0)
				{
					NAVToolbox::CStartBrowser browser;
					browser.ShowURL(m_csDownloadUrl);
				}
			}
		}		
		catch(_com_error& e)
		{
			CCTRACEE(_T("Error while launching upgrade mega alert 0x%08X"), e.Error());
		}

		return true;
	}

	void SetDownloadUrl(CString csUrl)
	{
		m_csDownloadUrl = csUrl;
		CCTRACEI(_T("DownloadUrl = %s"), m_csDownloadUrl);
	}

private:
	CString m_csDownloadUrl;
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
		StahlSoft::CSmartDataPtr<BYTE> spUrl;
		DWORD dwFirstTime = 0;		// Stores the Current Time
		DWORD dwLastTime = 0;		// Stores the First time we popped the alert

		CCTRACEI(_T("Get upgrade notification data data"));

		CPEPClientBase pepBase;
		hrx << pepBase.Init();
		pepBase.SetContextGuid(clt::pep::CLTGUID);
		pepBase.SetComponentGuid(V2LicensingAuthGuids::SZ_ANTIVIRUS_COMPONENT);
		pepBase.SetPolicyID(clt::pep::POLICY_ID_QUERY_LICENSE_RECORD);
		pepBase.SetCollectionDWORD(clt::pep::SZ_PROPERTY_DISPOSITION,clt::pep::DISPOSITION_NONE);
        pepBase.SetCollectionDWORD(SUBSCRIPTION_STATIC::SZ_SUBSCRIPTION_PROPERTY_DISPOSITION, SUBSCRIPTION_STATIC::DISPOSITION_NONE);

		hrx << pepBase.QueryPolicy();

		hrx << pepBase.GetValueFromCollection(DRM::szDRMSCSSMessageCode, (DWORD &)dwMessageCode, 0);
		hrx << pepBase.GetValueFromCollection(DRM::szDRMSCSSShowNDaysTotal, (DWORD &)dwShow_N_Days, 0);
		hrx << pepBase.GetValueFromCollection(DRM::szDRMSCSSShowEveryNDays, (DWORD &)dwShow_Every_N_Days, 0);
		hrx << pepBase.GetValueFromCollection(DRM::szDRMSCSSBalloonAutoHideSeconds, (DWORD &)dwAuto_Hide_Seconds, 0);
        hrx << pepBase.GetCollectionData(DRM::szDRMSCSSMessageURL, spUrl);

		CString csDownloadUrl = (LPCSTR)spUrl.m_p;

		CCTRACEI(_T("message_code=%d"), dwMessageCode);
		CCTRACEI(_T("show_n_days_total=%d"), dwShow_N_Days);
		CCTRACEI(_T("show_every_n_days=%d"), dwShow_Every_N_Days);
		CCTRACEI(_T("balloon_auto_hide_seconds=%d"), dwAuto_Hide_Seconds);
		CCTRACEI(_T("message_url=%s"), csDownloadUrl);

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
				cb.SetDownloadUrl(csDownloadUrl);
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
					CBrandingRes BrandRes;

					csProductName = BrandRes.ProductName();
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
					cb.SetDownloadUrl(csDownloadUrl);

					pNotify->SetHyperlinkCallback(&cb);
					pNotify->SetProperty(INotify::PROPERTY_TASKBAR_ENTRY, true);
					pNotify->SetIcon(_Module.GetModuleInstance(), NAVW_ICO, 0);
					pNotify->SetTitle(csProductName);
					pNotify->SetText(csText);
					pNotify->SetTimeout(dwAuto_Hide_Seconds * 1000);
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
