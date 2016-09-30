// ----------------------------------------------------------------------------
// PROPRIETARY/CONFIDENTIAL.  Use of this product is subject to license terms.
// Copyright (C) 2005 Symantec Corporation.  All rights reserved.
// ----------------------------------------------------------------------------

#include "stdafx.h"

#include "CClientCallback.h"
#include "AutoProtectWrapper.h" // For CAutoProtectOptions
#include "NAVSettingsHelperEx.h" // For CNavOptSettingsEx
#include "globalevent.h"        // For CGlobalEvent
#include "globalevents.h"
#include "globalmutex.h"        // For SYM_NAVPROXY_MUTEX
#include "NAVPwdExp.h"  // NAV Password headers
#include "NAVPwd_h.h"
#include "NAVPwd_i.c"
#include "AVRESBranding.h" // Required in order to get product name
#include "IWPSettingsInterface.h"

#undef CHECK_HR
#define CHECK_HR(hr)\
if (FAILED(hr))\
{\
	CCTRACEE(_T("HRESULT = %x in %hs(%d)"),hr,__FILE__,__LINE__);\
    result = nsc::NSC_FAIL;\
}

nsc::NSCRESULT CClientCallback::InitNavOptions(CComPtr<INAVOptions> &spNavOptions, 
                                               CComPtr<ISymScriptSafe> &spSymScriptSafe,
                                               nsc::NSCRESULT &result)
                                               const throw()
{
    HRESULT hr = S_OK;
    CComBSTR bstrOptionsClass("Symantec.Norton.AntiVirus.NAVOptions");

    hr = spNavOptions.CoCreateInstance(bstrOptionsClass, NULL, CLSCTX_INPROC);
    CHECK_HR(hr);
    if (SUCCEEDED(hr))
    {
        hr = spNavOptions.QueryInterface(&spSymScriptSafe);
        CHECK_HR(hr);
        if (SUCCEEDED(hr) && (spSymScriptSafe != NULL))
        {
            DWORD dwAccess = ISYMSCRIPTSAFE_UNLOCK_KEYA;
            DWORD dwKey = ISYMSCRIPTSAFE_UNLOCK_KEYB;
            hr = spSymScriptSafe->SetAccess(dwAccess, dwKey);
            CHECK_HR(hr);
            if (!SUCCEEDED(hr))
            {
                TRACEE( _T("Unable to set access for nav options in client callback."));
                return nsc::NSC_FAIL;
            }
        }
        else
        {
            TRACEE( _T("Unable to query for script safe object from nav options in client callback."));
            return nsc::NSC_FAIL;
        }
    }
    else
    {
        TRACEE( _T("Unable to create the nav options object in client callback."));
        return nsc::NSC_FAIL;
    }

    return nsc::NSC_SUCCESS;
}

nsc::NSCRESULT CClientCallback::InitScriptableAP(CComPtr<IScriptableAutoProtect> &spAP, 
                                CComPtr<ISymScriptSafe> &spSymScriptSafeAP,
                                nsc::NSCRESULT &result)
                                const throw()
{
    HRESULT hr = S_OK;
    CComBSTR bstrAPClass ("NAVAPSCR.ScriptableAutoProtect");

    hr = spAP.CoCreateInstance(bstrAPClass, NULL, CLSCTX_ALL);
    CHECK_HR(hr);
    if (SUCCEEDED(hr))
    {
        hr = spAP.QueryInterface(&spSymScriptSafeAP);
        CHECK_HR(hr);
        if (SUCCEEDED(hr) && (spSymScriptSafeAP != NULL))
        {
            DWORD dwAccess = ISYMSCRIPTSAFE_UNLOCK_KEYA;
            DWORD dwKey = ISYMSCRIPTSAFE_UNLOCK_KEYB;
            hr = spSymScriptSafeAP->SetAccess(dwAccess, dwKey);
            CHECK_HR(hr);
            if (!SUCCEEDED(hr))
            {
                TRACEE( _T("Unable to set access for scriptable AP in client callback."));
                return nsc::NSC_FAIL;
            }
        }
        else
        {
            TRACEE( _T("Unable to query for script safe object from scriptable ap in client callback."));
            return nsc::NSC_FAIL;
        }
    }
    else
    {
        TRACEE( _T("Unable to create the scriptable ap object in client callback."));
        return nsc::NSC_FAIL;
    }

    return nsc::NSC_SUCCESS;
}
nsc::NSCRESULT CClientCallback::InitAppLauncher(CComPtr<IAppLauncher> &spAppLauncher, 
                               CComPtr<ISymScriptSafe> &spSymScriptSafeAppLnch,
                               nsc::NSCRESULT &result)
                               const throw()
{
    HRESULT hr = S_OK;
    CComBSTR bstrNAVLnch("Symantec.Norton.AntiVirus.AppLauncher");

    hr = spAppLauncher.CoCreateInstance ( bstrNAVLnch, NULL, CLSCTX_INPROC_SERVER );
    CHECK_HR(hr);
    if(SUCCEEDED(hr))
    {
        hr = spAppLauncher.QueryInterface(&spSymScriptSafeAppLnch);
        CHECK_HR(hr);
        if (SUCCEEDED (hr) && (spSymScriptSafeAppLnch != NULL))
        {
            // Set appropriate access so that NAVStatus call to IsItSafe() 
            // returns successfully.

            DWORD dwAccess = ISYMSCRIPTSAFE_UNLOCK_KEYA;
            DWORD dwKey = ISYMSCRIPTSAFE_UNLOCK_KEYB;
            hr = spSymScriptSafeAppLnch->SetAccess(dwAccess, dwKey);
            CHECK_HR(hr);
            if (!SUCCEEDED(hr))
            {
                TRACEE( _T("Unable to set access for app launcher in client callback."));
                return nsc::NSC_FAIL;
            }
        }
        else
        {
            TRACEE( _T("Unable to query for script safe object from app launcher in client callback."));
            return nsc::NSC_FAIL;
        }
    }
    else
    {
        TRACEE( _T("Unable to create app launcher object in client callback."));
        return nsc::NSC_FAIL;
    }

    return nsc::NSC_SUCCESS;
}
nsc::NSCRESULT CClientCallback::InitScanTasks(CComPtr<INAVScanTasks> &spTasks, 
                             CComPtr<ISymScriptSafe> &spSymScriptSafeScan,
                             nsc::NSCRESULT &result)
                             const throw()
{
    HRESULT hr = S_OK;
    CComBSTR bstrTasksClass("Symantec.Norton.AntiVirus.ScanTasks");

    hr = spTasks.CoCreateInstance(bstrTasksClass);
    CHECK_HR(hr);
    if (SUCCEEDED(hr))
    {
        hr = spTasks.QueryInterface(&spSymScriptSafeScan);
        CHECK_HR(hr);
        if (SUCCEEDED (hr) && (spSymScriptSafeScan != NULL))
        {				
            DWORD dwAccess = ISYMSCRIPTSAFE_UNLOCK_KEYA;
            DWORD dwKey = ISYMSCRIPTSAFE_UNLOCK_KEYB;
            hr = spSymScriptSafeScan->SetAccess(dwAccess, dwKey);
            CHECK_HR(hr);
            if (!SUCCEEDED(hr))
            {
                TRACEE( _T("Unable to set access for tasks in client callback."));
                return nsc::NSC_FAIL;
            }
        }
        else
        {
            TRACEE( _T("Unable to query for script safe object from scan tasks in client callback."));
            return nsc::NSC_FAIL;
        }
    }
    else
    {
        TRACEE( _T("Unable to create the scan task object in client callback."));
        return nsc::NSC_FAIL;
    }

    return nsc::NSC_SUCCESS;
}

nsc::NSCRESULT CClientCallback::Execute(HWND window_in,
							  DWORD cookie_in) 
							  const throw()
{
    CROSS_PROCESS_LOCK();
    TRACEI(_T("CClientCallback::Execute called with cookie %d"),cookie_in);

    nsc::NSCRESULT result = nsc::NSC_SUCCESS;
    HRESULT hr = S_OK;

	// NAVOptions
    CComPtr<INAVOptions> spNavOptions;
    CComPtr<ISymScriptSafe> spSymScriptSafe;
    
    // Scriptable Auto Protect
    CComPtr<IScriptableAutoProtect> spAP;
    CComPtr<ISymScriptSafe> spSymScriptSafeAP;

    // AppLauncher
    CComPtr<IAppLauncher> spAppLauncher;
    CComPtr<ISymScriptSafe> spSymScriptSafeAppLnch;

    // ScanTasks
    CComPtr<INAVScanTasks> spTasks;
    CComPtr<ISymScriptSafe> spSymScriptSafeScan;

    BOOL bResult = TRUE;
    DWORD dwSetValue = 1;
    IWP::IIWPPrivateSettingsPtr m_pIWPSettings;
    SYMRESULT sresult = SYM_OK;
    //CAutoProtectOptions APOptions;
    CNAVOptSettingsEx NavOptSettings;

    /** Fix for defect 1-4HWQVM: Adding NAV Options Password functionality **/
    CComPtr<IPassword> spNAVPass;
    if (SUCCEEDED(spNAVPass.CoCreateInstance(CLSID_Password, NULL, CLSCTX_INPROC)))
    {
        CBrandingRes BrandRes;
        CString csProductName;  // Per management, reusing productname is OK
        csProductName = BrandRes.ProductName();

        CComBSTR bstrProductName(csProductName);
        BOOL bEnabled = FALSE;

        if( SUCCEEDED(spNAVPass->put_ProductID(NAV_CONSUMER_PASSWORD_ID)) &&
            SUCCEEDED(spNAVPass->put_ProductName(bstrProductName)) &&
            SUCCEEDED(spNAVPass->put_FeatureName(bstrProductName)) &&
            SUCCEEDED(spNAVPass->get_Enabled(&bEnabled)) )
        {
            if( bEnabled )
            {

                // Reusing the bEnabled variable to check if the password
                if( SUCCEEDED(spNAVPass->Check(&bEnabled)) )
                {
                    // Check the result of the password check before displaying the options
                    if( !bEnabled )
                    {
                        // Password not input correctly...don't display options
                        return nsc::NSC_FAIL;
                    }
                }
            }
        }
    }
    else
    {
        return nsc::NSC_FAIL; // Do not allow access without password
    }
    /** End Fix for defect 1-4HWQVM **/

    switch (cookie_in)
    {
	case eCOOKIE_ENABLE_AP:
		{
			CCTRCTXI0(_T("enable AP case"));

			if(InitScriptableAP(spAP, spSymScriptSafeAP, result) != nsc::NSC_SUCCESS)
				return nsc::NSC_FAIL;

            HRESULT hResult = E_FAIL;
			hResult = spAP->Configure(true);
			if(FAILED(hResult))
			{
				CCTRCTXE1(_T("Failed IScriptableAutoProtect::Configure(true), hResult=0x%08X"), hResult);
				result = nsc::NSC_FAIL;
			}

            // This call will return when AP is done loading
			hResult = spAP->put_Enabled(true);
			if(FAILED(hResult))
			{
				CCTRCTXE1(_T("Failed IScriptableAutoProtect::put_Enabled(true), hResult=0x%08X"), hResult);
				result = nsc::NSC_FAIL;
			}

            CAutoProtectOptions SavrtOptions;
            if ( SAVRT_SUCCEEDED(SavrtOptions.Load()))
            {
                SavrtOptions.SetDwordValue("STARTUP:LoadVxD", 1);
                SavrtOptions.Save();
            }
			break;
		}

	case eCOOKIE_DISABLE_AP:
		{
			BOOL bStartup = true;
			TRACEI(_T("CClientCallback::Execute called for disable AP"));

			if(InitNavOptions(spNavOptions, spSymScriptSafe, result) != nsc::NSC_SUCCESS)
				return nsc::NSC_FAIL;

			// This cannot be broken down; Snooze is deeply integrated into Options.
			bResult = TRUE;        
			hr = spNavOptions->Snooze((long)window_in, SnoozeAP, FALSE, &bResult);
			CHECK_HR(hr);

			if (!bResult)
			{
				// user cancel
				return nsc::NSC_FAIL;
			}

            LONG lSnoozePeriod = 0;
			hr = spNavOptions->GetSnoozePeriod(SnoozeAP, &lSnoozePeriod);
			CHECK_HR(hr);
			if(lSnoozePeriod > 0) // limited time snooze
			{
				HRESULT hResult = spNavOptions->Put(CComBSTR(STARTUP_LoadVxD), CComVariant(FALSE));
				if(FAILED(hResult))
				{
					CCTRCTXE1(_T("Could not set Startup:LoadVxD option via NAVOptions, hResult=0x%08X"), hResult);
					result = nsc::NSC_FAIL;
				}

				hResult = spNavOptions->Save();
				if(FAILED(hResult))
				{
					CCTRCTXE1(_T("Could not save NAVOptions, hResult=0x%08X"), hResult);
					result = nsc::NSC_FAIL;
				}
			}

			if(InitScriptableAP(spAP, spSymScriptSafeAP, result) != nsc::NSC_SUCCESS)
				return nsc::NSC_FAIL;

			hr = spAP->put_Enabled(false);
			CHECK_HR(hr);
			break;
		}

    case eCOOKIE_CONFIGURE_AUTO_PROTECT:
		TRACEI(_T("Configuring AutoProtect"));

        if(InitNavOptions(spNavOptions, spSymScriptSafe, result) != nsc::NSC_SUCCESS)
            return nsc::NSC_FAIL;

		hr = spNavOptions->ShowPage((long)window_in,ShowPageID_AUTOPROTECT);
		CHECK_HR(hr);

        break;

    case eCOOKIE_CONFIGURE_EMAIL_SCANNING:
		TRACEI(_T("Configuring Email Scanning"));

        if(InitNavOptions(spNavOptions, spSymScriptSafe, result) != nsc::NSC_SUCCESS)
            return nsc::NSC_FAIL;

		hr = spNavOptions->ShowPage((long)window_in,ShowPageID_EMAIL);
		CHECK_HR(hr);

        break;

    case eCOOKIE_CONFIGURE_SPYWARE_PROTECTION:
		TRACEI(_T("Configuring Spyware & Adware Protection"));

        if(InitNavOptions(spNavOptions, spSymScriptSafe, result) != nsc::NSC_SUCCESS)
            return nsc::NSC_FAIL;

		hr = spNavOptions->ShowPage((long)window_in,ShowPageID_SPYWARE);
		CHECK_HR(hr);

        break;

    case eCOOKIE_CONFIGURE_IFP:
		TRACEI(_T("Configuring Inbound Firewall Protection"));

        if(InitNavOptions(spNavOptions, spSymScriptSafe, result) != nsc::NSC_SUCCESS)
            return nsc::NSC_FAIL;

		hr = spNavOptions->ShowPage((long)window_in,ShowPageID_INBOUND_FIREWALL);
		CHECK_HR(hr);

        break;

    case eCOOKIE_CONFIGURE_IM_SCANNING:
		TRACEI(_T("Configuring IM Scanning"));

        if(InitNavOptions(spNavOptions, spSymScriptSafe, result) != nsc::NSC_SUCCESS)
            return nsc::NSC_FAIL;

		hr = spNavOptions->ShowPage((long)window_in,ShowPageID_IM);
		CHECK_HR(hr);

        break;

    case eCOOKIE_CONFIGURE_WORM_BLOCKING:
		TRACEI(_T("Configuring Worm Blocking"));

        if(InitNavOptions(spNavOptions, spSymScriptSafe, result) != nsc::NSC_SUCCESS)
            return nsc::NSC_FAIL;

		hr = spNavOptions->ShowPage((long)window_in,ShowPageID_WORM_BLOCKING);
		CHECK_HR(hr);

        break;

    case eCOOKIE_CONFIGURE_RUN_LU:
		TRACEI(_T("Configuring Run LiveUpdate"));

        if(InitNavOptions(spNavOptions, spSymScriptSafe, result) != nsc::NSC_SUCCESS)
            return nsc::NSC_FAIL;

		hr = spNavOptions->ShowPage((long)window_in,ShowPageID_LIVE_UPDATE);
		CHECK_HR(hr);

        break;

	case eCOOKIE_ENABLE_EMAIL_SCANNING:
		{
			TRACEI(_T("CClientCallback::Execute called for enable email scanning"));

			if(InitNavOptions(spNavOptions, spSymScriptSafe, result) != nsc::NSC_SUCCESS)
				return nsc::NSC_FAIL;

			hr = spNavOptions->Put(CComBSTR(NAVEMAIL_ScanIncoming), CComVariant(TRUE));
			CHECK_HR(hr);
			hr = spNavOptions->Put(CComBSTR(NAVEMAIL_ScanOutgoing), CComVariant(TRUE));
			CHECK_HR(hr);
			hr = spNavOptions->Save();
			CHECK_HR(hr);

			// Wait 7 seconds for a change
            WaitForEmailChange();						
		}
        break;

	case eCOOKIE_DISABLE_EMAIL_SCANNING:
		{
			TRACEI(_T("CClientCallback::Execute called for disable email scanning"));

			if(InitNavOptions(spNavOptions, spSymScriptSafe, result) != nsc::NSC_SUCCESS)
				return nsc::NSC_FAIL;

			bResult = TRUE;

			SnoozeFeature snoozeFeatures = static_cast<SnoozeFeature>(SnoozeEmailIn | SnoozeEmailOut);
			hr = spNavOptions->Snooze((long)window_in, snoozeFeatures, FALSE, &bResult);
			CHECK_HR(hr);

			if (!bResult)
			{
				// user cancel
				return nsc::NSC_FAIL;
			}

			hr = spNavOptions->Put(CComBSTR(NAVEMAIL_ScanIncoming), CComVariant(FALSE));
			CHECK_HR(hr);
			hr = spNavOptions->Put(CComBSTR(NAVEMAIL_ScanOutgoing), CComVariant(FALSE));
			CHECK_HR(hr);
			hr = spNavOptions->Save();
			CHECK_HR(hr);

            // Email proxy is running, wait for the change to happen
            WaitForEmailChange();						
			break;
		}

	case eCOOKIE_ENABLE_SPYWARE_PROTECTION:
		TRACEI(_T("CClientCallback::Execute called for enable spyware protection"));

        if(NavOptSettings.Init())
        {
            hr = NavOptSettings.SetValue(_T("THREAT:Threat6"), (DWORD)TRUE);
            CHECK_HR(hr);

            // Fix for defect 1-4DL265
            // Now we need to send the NavOptions changed event AND the reload AP event
            {
                TRACEI ("CClientCallback::Execute - (Spyware=ON) Sending reload options event");
                CGlobalEvent ge;
#ifndef _UNICODE
                if (ge.Create(SYM_OPTIONS_CHANGED_EVENT))
#else  _UNICODE
                if (ge.CreateW(SYM_OPTIONS_CHANGED_EVENT))
#endif  _UNICODE
                    ::PulseEvent(ge);

                // Wait for AP to sync up - this will fail if AP is not running (off at startup)
                WaitForAPChange ();
            }
        }
        else
        {
            TRACEE(_T("CClientCallback::Execute could not disable spyware through CCSettings."));
            return nsc::NSC_FAIL;
        }
		break;

	case eCOOKIE_DISABLE_SPYWARE_PROTECTION:
		TRACEI(_T("CClientCallback::Execute called for disable spyware protection"));

        if(NavOptSettings.Init())
        {
            hr = NavOptSettings.SetValue(_T("THREAT:Threat6"), (DWORD)FALSE);
            CHECK_HR(hr);

            // Fix for defect where Main UI does not update when disabling Spyware.
            // We need to send the NavOptions changed event AND the reload AP event
            {
                TRACEI ("CClientCallback::Execute - (Spyware=ON) Sending reload options event");
                CGlobalEvent ge;
#ifndef _UNICODE
                if (ge.Create(SYM_OPTIONS_CHANGED_EVENT))
#else  _UNICODE
                if (ge.CreateW(SYM_OPTIONS_CHANGED_EVENT))
#endif  _UNICODE
                    ::PulseEvent(ge);

                // Wait for AP to sync up - this will fail if AP is not running (off at startup)
                WaitForAPChange ();
            }
        }
        else
        {
            TRACEE(_T("CClientCallback::Execute could not disable spyware through CCSettings."));
            return nsc::NSC_FAIL;
        }
		break;

	case eCOOKIE_ENABLE_IFP:
		{
			TRACEI(_T("CClientCallback::Execute called for enable IFP"));

			sresult = IWP_IWPPrivateSettings::CreateObject(GETMODULEMGR(),&m_pIWPSettings);
			if (SYM_FAILED(sresult))
			{
				TRACEE ("No IWP installed in enable IFP");
				return nsc::NSC_FAIL;
			}

			dwSetValue = 1;
			sresult = m_pIWPSettings->SetValue(IWP::IIWPSettings::IWPUserWantsOn, dwSetValue);
			if ( SYM_FAILED(sresult) )
			{
				TRACEE(_T("IWP user wants on failed in enable IFP"));
				return nsc::NSC_FAIL;
			}
			sresult = m_pIWPSettings->Save();
			if (SYM_FAILED(sresult))
			{
				TRACEE(_T("IWP save settings failed in enable IFP"));
				return nsc::NSC_FAIL;
			}
			m_pIWPSettings.Release();

			// Wait for IWP settings change event, because NSC shows error in 
			// mini UI if the callback returns but health is still bad.
            WaitForIWPChange ();
			break;
		}
	case eCOOKIE_DISABLE_IFP:
		{
			TRACEI(_T("CClientCallback::Execute called for disable IFP"));

			sresult = IWP_IWPPrivateSettings::CreateObject(GETMODULEMGR(),&m_pIWPSettings);
			if (SYM_FAILED(sresult))
			{
				TRACEE ("No IWP installed in disable IFP");
				return nsc::NSC_FAIL;
			}

			if(InitNavOptions(spNavOptions, spSymScriptSafe, result) != nsc::NSC_SUCCESS)
				return nsc::NSC_FAIL;

			bResult = TRUE;
			hr = spNavOptions->Snooze((long)window_in, SnoozeIWP, FALSE, &bResult);
			CHECK_HR(hr);

			if (!bResult)
			{
				// user cancel
				return nsc::NSC_FAIL;
			}

			dwSetValue = 0;
			sresult = m_pIWPSettings->SetValue(IWP::IIWPSettings::IWPUserWantsOn, dwSetValue);
			if ( SYM_FAILED(sresult) )
			{
				TRACEE(_T("IWP user wants on failed in disable IFP"));
				return nsc::NSC_FAIL;
			}
			sresult = m_pIWPSettings->Save();
			if (SYM_FAILED(sresult))
			{
				TRACEE(_T("IWP save settings failed in disable IFP"));
				return nsc::NSC_FAIL;
			}
			m_pIWPSettings.Release();

			break;
		}

    case eCOOKIE_ENABLE_IM_SCANNING:
		TRACEI(_T("CClientCallback::Execute called for enable IM scanning"));
        // Reimplemented for 1-44CQ5Y redesign.  Enable/disable only if 1 client installed.
        // Changed per defect 1-4EEXL5. Enable all available clients.

        if(InitNavOptions(spNavOptions, spSymScriptSafe, result) != nsc::NSC_SUCCESS)
            return nsc::NSC_FAIL;

        // Workaround for defect 1-4EEXLC.
		hr = spNavOptions->Put(CComBSTR(IMSCAN_ScanMIM), CComVariant(TRUE));
		CHECK_HR(hr);
		hr = spNavOptions->Put(CComBSTR(IMSCAN_ScanAIM), CComVariant(TRUE));
		CHECK_HR(hr);
		hr = spNavOptions->Put(CComBSTR(IMSCAN_ScanYIM), CComVariant(TRUE));
		CHECK_HR(hr);
        
        hr = spNavOptions->Save();
        CHECK_HR(hr);
 		break;

	case eCOOKIE_DISABLE_IM_SCANNING:
		TRACEI(_T("CClientCallback::Execute called for disable IM scanning"));
        // Reimplemented for 1-44CQ5Y redesign.  Enable/disable only if 1 client installed.
        // Changed per defect 1-4EEXL5. Disable all available clients.

        if(InitNavOptions(spNavOptions, spSymScriptSafe, result) != nsc::NSC_SUCCESS)
            return nsc::NSC_FAIL;

        // Workaround for defect 1-4EEXLC.
		hr = spNavOptions->Put(CComBSTR(IMSCAN_ScanMIM), CComVariant(FALSE));
		CHECK_HR(hr);
		hr = spNavOptions->Put(CComBSTR(IMSCAN_ScanAIM), CComVariant(FALSE));
		CHECK_HR(hr);
		hr = spNavOptions->Put(CComBSTR(IMSCAN_ScanYIM), CComVariant(FALSE));
		CHECK_HR(hr);

        hr = spNavOptions->Save();
        CHECK_HR(hr);
 		break;

	case eCOOKIE_ENABLE_WORM_BLOCKING:
		TRACEI(_T("CClientCallback::Execute called for enable worm blocking"));

        if(InitNavOptions(spNavOptions, spSymScriptSafe, result) != nsc::NSC_SUCCESS)
            return nsc::NSC_FAIL;

		hr = spNavOptions->Put(CComBSTR(NAVEMAIL_OEH), CComVariant(TRUE));
		CHECK_HR(hr);
        // Fix for defect 1-4DNUUL
        hr = spNavOptions->Put(CComBSTR(NAVEMAIL_ScanOutgoing), CComVariant(TRUE));
        CHECK_HR(hr);
		hr = spNavOptions->Save();
		CHECK_HR(hr);

		break;

	case eCOOKIE_DISABLE_WORM_BLOCKING:
		TRACEI(_T("CClientCallback::Execute called for disable worm blocking"));

        if(InitNavOptions(spNavOptions, spSymScriptSafe, result) != nsc::NSC_SUCCESS)
            return nsc::NSC_FAIL;

		hr = spNavOptions->Put(CComBSTR(NAVEMAIL_OEH), CComVariant(FALSE));
		CHECK_HR(hr);
		hr = spNavOptions->Save();
		CHECK_HR(hr);

		break;

	case eCOOKIE_LAUNCH_FSS_AND_WAIT:
		TRACEI(_T("CClientCallback::Execute called for launch FSS & Wait"));

        if(InitScanTasks(spTasks, spSymScriptSafeScan, result) != nsc::NSC_SUCCESS)
            return nsc::NSC_FAIL;

		hr = spTasks->ScanMyComputerAndWait();
		CHECK_HR(hr);

        break;

	case eCOOKIE_LAUNCH_RUN_LU_AND_WAIT:
		TRACEI(_T("CClientCallback::Execute called for launch Run LiveUpdate & Wait"));

        if(InitAppLauncher(spAppLauncher, spSymScriptSafeAppLnch, result) != nsc::NSC_SUCCESS)
            return nsc::NSC_FAIL;

        hr = spAppLauncher->LaunchLiveUpdateAndWait();
		CHECK_HR(hr);

        break;

	case eCOOKIE_LAUNCH_QUARANTINE:
		TRACEI(_T("CClientCallback::Execute called for eCOOKIE_LAUNCH_QUARANTINE"));

        if(InitAppLauncher(spAppLauncher, spSymScriptSafeAppLnch, result) != nsc::NSC_SUCCESS)
            return nsc::NSC_FAIL;

		hr = spAppLauncher->LaunchQuarantine();
		CHECK_HR(hr);

		break;

    case eCOOKIE_INVALID:
    default:

        TRACEE(_T("Unexpected client callback cookie = %d"),cookie_in);
        return nsc::NSC_FAIL;
    }

    return result;
}

bool CClientCallback::WaitForChange (HANDLE hEvent, DWORD dwWaitTimeout /*ms*/) const
{
    // Bail if we get WM_QUIT
    ccLib::CMessageLock lock (TRUE, TRUE);
    DWORD dwResult = lock.Lock ( hEvent, 7*1000 );
    
    switch ( dwResult )
    {
    case WAIT_FAILED:
        CCTRCTXE0(_T("Wait failed"));
        return false;
    case WAIT_TIMEOUT:
        CCTRCTXW0(_T("Timed out waiting"));
        return false;
    default:
		CCTRCTXI0(_T("Change completed"));
        return true;
    }
}

void CClientCallback::WaitForAPChange () const
{
    // Wait for AP to finish loading
    CCTRCTXI0(_T("Waiting for AP to reload..."));
    CGlobalEvent eventAPDone; // These are NULL DACLs so they will work with legacy products
    if(eventAPDone.Create(SYM_REFRESH_AP_STATUS_EVENT))
    {
        WaitForChange ( eventAPDone, 7*1000);
    }
    else
	    CCTRCTXE0(_T("Failed to create the AP event") );
}

// We don't need to wait for the proxy to be running.
// If the proxy isn't running we won't get a change anyway. Same thing.
//
void CClientCallback::WaitForEmailChange () const
{
    CCTRCTXI0(_T("Waiting for Email to reload..."));
    CGlobalEvent eventEmail; // These are NULL DACLs so they will work with legacy products
    if(eventEmail.Create(SYM_REFRESH_NAVPROXY_STATUS_EVENT))
    {
        WaitForChange (eventEmail, 7*1000);
    }
    else
	    CCTRCTXE0("Failed to create the Email event" );
}

void CClientCallback::WaitForIWPChange () const
{
    CCTRCTXI0(_T("Waiting for IWP to reload..."));

    // Wait for IWP settings change event, because NSC shows error in 
    // mini UI if the callback returns but health is still bad.
    CGlobalEvent eventIWPDone; // These are NULL DACLs so they will work with legacy products
    if(eventIWPDone.Create(IWP::SYM_REFRESH_IWP_EVENT))
    {
        WaitForChange ( eventIWPDone, 7*1000);
    }
    else
	    CCTRCTXE0("Failed to create the IWP event" );
}