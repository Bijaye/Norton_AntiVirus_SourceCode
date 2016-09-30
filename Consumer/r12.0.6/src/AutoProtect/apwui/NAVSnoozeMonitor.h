#pragma once

///////////////////////////////////////////////////////////////////////////////////////

#include <time.h>

#include "ScriptSafeVerifyTrustCached.h"		// For scripting security

#include "NAVOptions_i.c"
#include "NAVOptions.h"

#include "NAVAPSCR_i.c"
#include "NAVAPSCR.h"

#include "NAVSnoozeSettings.h"
#include "TraceHR.h"

#include "NAVSettings.h"
#include "AutoProtectWrapper.h" // Wraps calls to SAVRT

static const char DRMSettingKeyName[] = "Norton AntiVirus\\NAVOPTS.DAT\\DRM";
static const char DRMFeatureEnabled[] = "FeatureEnabled";

static const char SNOOZE_INIT_FLAG_KEY_NAME[] = "VOLATILE_DATA\\Norton AntiVirus\\Status";
static const char SNOOZE_INIT_FLAG_VALUE_NAME[] = "SnoozeMonitorInitialized";
static const long MAX_SNOOZE_PERIOD = 5 * 60 * 60;		// 5 hours in seconds
static const long SleepUntilReboot = -1;

static bool NAVSnoozeMonitorInitialized = false;

//
//	Snooze Alert Monitor
//
class CNAVSnoozeMonitor
{
private:
	bool m_bActive;
	long m_SnoozedOptions[NAVSnoozeSettingsCount];
	long m_lLastTime;
	bool m_WakeUpNow[NAVSnoozeSettingsCount];
	NAVToolbox::CCSettings m_ccSettings;

public:

	CNAVSnoozeMonitor(void)
	{
		m_bActive = false;
		NAVSnoozeMonitorInitialized = false;
		m_lLastTime = 0;
	}

	~CNAVSnoozeMonitor(void)
	{
	}

	void Init(void)
	{
		TRACEHR (h);

		if (NAVSnoozeMonitorInitialized)
			return;

		// Is it the first run after reboot?
		bool wakeUpAfterReboot = true;

		try
		{
			h %= "Verify Snooze Alert trust";
			h.Verify( NAVToolbox::IsSymantecComServer(CLSID_SnoozeAlertObj) == NAVToolbox::NAVTRUST_OK );

			h %= "Get Settings";
			ccSettings::ISettingsPtr pSettings;

			if (m_ccSettings.GetSettings(SNOOZE_INIT_FLAG_KEY_NAME, &pSettings) != SYM_OK)
			{
				m_ccSettings.CreateSettings(SNOOZE_INIT_FLAG_KEY_NAME, &pSettings);
			}

			DWORD dwFlag = 0;
			if (SYM_SUCCEEDED(pSettings->GetDword(SNOOZE_INIT_FLAG_VALUE_NAME, dwFlag)) && dwFlag != 0)
			{
				wakeUpAfterReboot = false;
			}
			else
			{
				CCTRACEI("First run after reboot");

				pSettings->PutDword(SNOOZE_INIT_FLAG_VALUE_NAME, 1);
				m_ccSettings.PutSettings(pSettings);
			}
	
			NAVSnoozeMonitorInitialized = true;

			h %= "Load snooze settings";

			RefreshSettings(wakeUpAfterReboot);

			if (wakeUpAfterReboot)
				DoWork();						// Wake up sleeping features

		}
		catch (...)
		{
			NAVSnoozeMonitorInitialized = false;	// Disable snooze alert
			h = E_ABORT;
		}
	}

	void RefreshSettings(bool firstRunAfterReboot)
	{
		TRACEHR (h);

		if (!NAVSnoozeMonitorInitialized)
			return;

		try
		{
			ccSettings::ISettingsPtr pSettings = m_ccSettings.GetSettings(NAVSnoozeSettingsKeyName);

			for (int i = 0; i < NAVSnoozeSettingsCount; i++)
			{
				m_SnoozedOptions[i] = 0;
				h << pSettings->GetDword(NAVSnoozeSettingsNames[i], (DWORD&)m_SnoozedOptions[i]);

				CCTRACEI("Snoozing: %d", m_SnoozedOptions[i]);
				m_WakeUpNow[i] = firstRunAfterReboot && m_SnoozedOptions[i] == SleepUntilReboot;
			}

			UpdateActiveStatus();
		}
		catch (...)
		{
			NAVSnoozeMonitorInitialized = false;	// Disable snooze alert
			h = E_ABORT;
		}
	}

	void DoWork(void)
	{
		TRACEHR (h);

		if (!IsActive())
			return;

		try
		{
			h %= "Get snooze settings";

			ccSettings::ISettingsPtr pSettings = m_ccSettings.GetSettings(NAVSnoozeSettingsKeyName);

			// Check system clock
			h %= "Check system clock";

			long timeNow = time(0);

			if (m_lLastTime != 0)
			{
				if (m_lLastTime > timeNow)
				{
					// The system clock were moved back.

					CCTRACEW("Snooze monitor: The system clock were moved back");

					long d = m_lLastTime - timeNow;
					for (int i = 0; i < NAVSnoozeSettingsCount; i++)
					{
						if (m_SnoozedOptions[i] > 0)
						{
							m_SnoozedOptions[i] -= d;
							h << pSettings->PutDword(NAVSnoozeSettingsNames[i], m_SnoozedOptions[i]);
						}
					}
				}
			}
			m_lLastTime = timeNow;

			//	Check for conditions of waking up
			h %= "Check for wake up";

			long featureEnabled = -1;

			for (int i = 0; i < NAVSnoozeSettingsCount; i++)
			{
				long snooze = m_SnoozedOptions[i];

				if (snooze == 0 || (!m_WakeUpNow[i] && (snooze == SleepUntilReboot || snooze > timeNow)))
					continue;

				// Clear snooze settings
				m_WakeUpNow[i] = false;
				m_SnoozedOptions[i] = 0;
				h << pSettings->PutDword(NAVSnoozeSettingsNames[i], 0);

				if (featureEnabled < 0)
				{
					h %= "Get license status";
					ccSettings::ISettingsPtr pDRMSettings = m_ccSettings.GetSettings(DRMSettingKeyName);
					h << pDRMSettings->GetDword(DRMFeatureEnabled, (DWORD&)featureEnabled);
				}

				if (featureEnabled == 0)
				{
					CCTRACEW("Product disabled");
					continue;
				}

				if (NAVSnoozeFeatures[i] == SnoozeAP)
				{
					h %= "Wake up AP";
					try
					{
						CApwConfig *pConfig = ApwGetConfig();
						h.Verify(pConfig != NULL);

						CApwCmd *pCmd = pConfig->GetCmd();
			            pCmd->EnableDriver();

						ConfigureApStartup(true);
					}
					catch(...)
					{
						h = E_FAIL;
					}
				}
				else
				{
					CComPtr<INAVOptions> spNavOptions;
					h << spNavOptions.CoCreateInstance(CLSID_NAVOptions, NULL, CLSCTX_INPROC);
					SetAccessKey(spNavOptions);

					h %= "Load options";
					h << spNavOptions->Load();

					h %= CString("Wake up ") + NAVSnoozeSettingsNames[i];

					switch (NAVSnoozeFeatures[i])
					{
						case SnoozeIWP:
							spNavOptions->put_IWPUserWantsOn(TRUE);
							break;

						case SnoozeALU:
							spNavOptions->put_LiveUpdate(TRUE);
							break;

						case SnoozeEmailIn:
							spNavOptions->Put(CComBSTR(NAVEMAIL_ScanIncoming), CComVariant(TRUE));
							break;

						case SnoozeEmailOut:
							spNavOptions->Put(CComBSTR(NAVEMAIL_ScanOutgoing), CComVariant(TRUE));
							break;
					}

					h %= "Save options";

					h << spNavOptions->Save();
				}
			}

			h %= "Put Settings";

			h << m_ccSettings.PutSettings(pSettings);

			UpdateActiveStatus();
		}
		catch (...)
		{
			NAVSnoozeMonitorInitialized = false;	// Disable snooze alert
			h = E_ABORT;
		}

	}


	DWORD GetTimeout(void)
	{
		DWORD timeout = INFINITE;

		if (IsActive())
		{
			long timeNow = time(0);
			long lTimeout = 0x7FFFFFFF;					// Max. value
			for (int i = 0; i < NAVSnoozeSettingsCount; i++)
			{
				long snooze = m_SnoozedOptions[i];

				if (snooze > 0)
				{
					long l = max(0, snooze - timeNow);

					if (l > MAX_SNOOZE_PERIOD)
					{
						CCTRACEW("Snooze period is too long. Wake up now");
						m_WakeUpNow[i] = true;
						lTimeout = 0;
						break;
					}

					if (l < lTimeout)
						lTimeout = l;
				}
			}
			timeout = ((DWORD)lTimeout) * 1000;
		}

		CCTRACEI("Timeout: %ld", timeout);
		return timeout;
	}

private:

	bool IsActive(void)
	{
		bool result = NAVSnoozeMonitorInitialized && m_bActive;
		CCTRACEI("Snoozing active: %s", result? "Yes" : "No");
		return result;
	}

	void UpdateActiveStatus(void)
	{
		for (int i = 0; i < NAVSnoozeSettingsCount; i++)
		{
			if (m_WakeUpNow[i] || m_SnoozedOptions[i] > 0)
			{
				m_bActive = true;
				return;
			}
		}
		m_bActive = false;
		m_lLastTime = 0;
	}

	static void ConfigureApStartup(bool bEnable)
	{
		TRACEHR(h);

		CComPtr<IScriptableAutoProtect> spAP;
		h << spAP.CoCreateInstance(CLSID_ScriptableAutoProtect, NULL, CLSCTX_ALL);              
		SetAccessKey(spAP);

		h %= bEnable ? "Enable AP startup" : "Disable AP startup";
		h << spAP->Configure(bEnable);

        CAutoProtectOptions SavrtOptions;
        if ( SAVRT_SUCCEEDED(SavrtOptions.Load()))
        {
            SavrtOptions.SetDwordValue("STARTUP:LoadVxD", bEnable ? 1 : 0);
            SavrtOptions.Save();
        }
	}

	static void SetAccessKey(IUnknown* pObject)
	{
		TRACEHR(h);

		CComPtr<ISymScriptSafe> spSymScriptSafe;

		h << pObject->QueryInterface(&spSymScriptSafe);
		h << spSymScriptSafe->SetAccess(ISYMSCRIPTSAFE_UNLOCK_KEYA, ISYMSCRIPTSAFE_UNLOCK_KEYB);
	}

public:

	static BOOL SnoozeAutoProtect(bool bEnable)
	{
		TRACEHR(h);
		BOOL bResult = TRUE;

		if (!NAVSnoozeMonitorInitialized)
			return TRUE;	// Snooze alert functionality is disabled

		h %= bEnable ? "Enable AP" : "Disable AP";

		try
		{
			CComPtr<ISnoozeAlert> spSnoozeAlert;
			h << spSnoozeAlert.CoCreateInstance(CLSID_SnoozeAlertObj, NULL, CLSCTX_INPROC);

			long lValue = 0;

			if (bEnable)
			{
				h = spSnoozeAlert->GetSnoozePeriod(SnoozeAP, &lValue);
				if (h == S_OK && lValue != 0)
				{
					h = spSnoozeAlert->SetSnoozePeriod(SnoozeAP, 0);	// Clean up snooze settings.
				}

				ConfigureApStartup(true);
			}
			else
			{
				h = spSnoozeAlert->Snooze(0, SnoozeAP, FALSE, &bResult);

				if (h == S_OK && bResult)
				{
					h = spSnoozeAlert->GetSnoozePeriod(SnoozeAP, &lValue);
					if (h == S_OK && lValue > 0)
						ConfigureApStartup(false);
				}
			}
		}
		catch(...)
		{
			h = E_FAIL;
		}

		if (FAILED(h))
		{
			bResult = TRUE;	// Disable snooze alert and let to do everything the user wants
		}

		return bResult;
	}

};
