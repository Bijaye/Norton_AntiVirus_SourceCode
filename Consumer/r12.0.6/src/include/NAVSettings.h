#pragma once

#include "ccSettingsInterface.h"
#include "ccSettingsManagerHelper.h"
#include "ccSyncLock.h"
#include "TraceHr.h"

namespace NAVToolbox 
{
	class CCSettings
	{
	private:
		ccSettings::CSettingsManagerHelper m_ccSettingsHelper;
		ccSettings::ISettingsManagerPtr m_pSettingsManager;

	public:

		/////////////////////////////////////////////////////////////////////////////

		CCSettings()
		{
		}

		/////////////////////////////////////////////////////////////////////////////

		virtual ~CCSettings()
		{
		}

		/////////////////////////////////////////////////////////////////////////////

		bool Init()
		{
			TRACEHR (h);

			if (m_pSettingsManager)
				return true;

			try
			{
				CreateManager();
			}
			catch (...)
			{
				return false;
			}

			return true;
		}

		/////////////////////////////////////////////////////////////////////////////

		ccSettings::ISettingsManagerPtr GetSettingsManager()
		{
			if (!m_pSettingsManager)
				CreateManager();

			return m_pSettingsManager;
		}

		/////////////////////////////////////////////////////////////////////////////

		SYMRESULT CreateSettings(LPCSTR pszKey, ccSettings::ISettings** pOutput)
		{
			TRACEHR (h);

			if (!m_pSettingsManager)
				CreateManager();

			if ((h = m_pSettingsManager->CreateSettings(pszKey, pOutput)) == SYM_SETTINGS_COM_ERROR)
			{
				h /= _T("Reconnect to ccSettings");

				m_pSettingsManager.Release();
				CreateManager();
				h = m_pSettingsManager->CreateSettings(pszKey, pOutput);
			}

			return h;
		}

		/////////////////////////////////////////////////////////////////////////////

		ccSettings::ISettingsPtr CreateSettings(LPCSTR pszKey)
		{
			TRACEHR (h);

			ccSettings::ISettingsPtr value;
			h << CreateSettings(pszKey, &value);

			return value;
		}

		/////////////////////////////////////////////////////////////////////////////

		SYMRESULT GetSettings(LPCSTR pszKey, ccSettings::ISettings** pOutput)
		{
			TRACEHR (h);

			if (!m_pSettingsManager)
				CreateManager();

			if ((h = m_pSettingsManager->GetSettings(pszKey, pOutput)) == SYM_SETTINGS_COM_ERROR)
			{
				h /= _T("Reconnect to ccSettings");

				m_pSettingsManager.Release();
				CreateManager();
				h = m_pSettingsManager->GetSettings(pszKey, pOutput);
			}

			return h;
		}

		/////////////////////////////////////////////////////////////////////////////

		ccSettings::ISettingsPtr GetSettings(LPCSTR pszKey)
		{
			TRACEHR (h);

			ccSettings::ISettingsPtr value;
			h << GetSettings(pszKey, &value);

			return value;
		}

		/////////////////////////////////////////////////////////////////////////////

		SYMRESULT PutSettings(ccSettings::ISettings* pSettings)
		{
			TRACEHR (h);

			if (!m_pSettingsManager)
				CreateManager();

            ccSettings::ISettingsManager3QIPtr pSettingsMgr3 (m_pSettingsManager);
			if (pSettingsMgr3)
                h = pSettingsMgr3->PutSettings(pSettings, true); // Don't send events!
            else
            {
                assert(false);
                h = m_pSettingsManager->PutSettings(pSettings); // Fall back to old interface
            }

			return h;
		}

		/////////////////////////////////////////////////////////////////////////////

		SYMRESULT EnumSettings(LPCSTR pszKey, ccSettings::IEnumSettings** pOutput)
		{
			TRACEHR (h);

			if (!m_pSettingsManager)
				CreateManager();

			if ((h = m_pSettingsManager->EnumSettings(pszKey, pOutput)) == SYM_SETTINGS_COM_ERROR)
			{
				h /= _T("Reconnect to ccSettings");

				m_pSettingsManager.Release();
				CreateManager();
				h = m_pSettingsManager->EnumSettings(pszKey, pOutput);
			}

			return h;
		}

		/////////////////////////////////////////////////////////////////////////////

		SYMRESULT DeleteSettings(LPCSTR pszKey)
		{
			TRACEHR (h);

			if (!m_pSettingsManager)
				CreateManager();

			if ((h = m_pSettingsManager->DeleteSettings(pszKey)) == SYM_SETTINGS_COM_ERROR)
			{
				h /= _T("Reconnect to ccSettings");

				m_pSettingsManager.Release();
				CreateManager();
				h = m_pSettingsManager->DeleteSettings(pszKey);
			}

			return h;
		}

		/////////////////////////////////////////////////////////////////////////////

		SYMRESULT CreateSettings(LPCWSTR pszKey, ccSettings::ISettings** pOutput)
		{
			TRACEHR (h);

			if (!m_pSettingsManager)
				CreateManager();

			if ((h = m_pSettingsManager->CreateSettings(pszKey, pOutput)) == SYM_SETTINGS_COM_ERROR)
			{
				h /= _T("Reconnect to ccSettings");

				m_pSettingsManager.Release();
				CreateManager();
				h = m_pSettingsManager->CreateSettings(pszKey, pOutput);
			}

			return h;
		}

		/////////////////////////////////////////////////////////////////////////////

		ccSettings::ISettingsPtr CreateSettings(LPCWSTR pszKey)
		{
			TRACEHR (h);

			ccSettings::ISettingsPtr value;
			h << CreateSettings(pszKey, &value);

			return value;
		}

		/////////////////////////////////////////////////////////////////////////////

		SYMRESULT GetSettings(LPCWSTR pszKey, ccSettings::ISettings** pOutput)
		{
			TRACEHR (h);

			if (!m_pSettingsManager)
				CreateManager();

			if ((h = m_pSettingsManager->GetSettings(pszKey, pOutput)) == SYM_SETTINGS_COM_ERROR)
			{
				h /= _T("Reconnect to ccSettings");

				m_pSettingsManager.Release();
				CreateManager();
				h = m_pSettingsManager->GetSettings(pszKey, pOutput);
			}

			return h;
		}

		/////////////////////////////////////////////////////////////////////////////

		ccSettings::ISettingsPtr GetSettings(LPCWSTR pszKey)
		{
			TRACEHR (h);

			ccSettings::ISettingsPtr value;
			h << GetSettings(pszKey, &value);

			return value;
		}

		/////////////////////////////////////////////////////////////////////////////

		SYMRESULT EnumSettings(LPCWSTR pszKey, ccSettings::IEnumSettings** pOutput)
		{
			TRACEHR (h);

			if (!m_pSettingsManager)
				CreateManager();

			if ((h = m_pSettingsManager->EnumSettings(pszKey, pOutput)) == SYM_SETTINGS_COM_ERROR)
			{
				h /= _T("Reconnect to ccSettings");

				m_pSettingsManager.Release();
				CreateManager();
				h = m_pSettingsManager->EnumSettings(pszKey, pOutput);
			}

			return h;
		}

		/////////////////////////////////////////////////////////////////////////////

		SYMRESULT DeleteSettings(LPCWSTR pszKey)
		{
			TRACEHR (h);

			if (!m_pSettingsManager)
				CreateManager();

			if ((h = m_pSettingsManager->DeleteSettings(pszKey)) == SYM_SETTINGS_COM_ERROR)
			{
				h /= _T("Reconnect to ccSettings");

				m_pSettingsManager.Release();
				CreateManager();
				h = m_pSettingsManager->DeleteSettings(pszKey);
			}

			return h;
		}

		/////////////////////////////////////////////////////////////////////////////

	private:

		/////////////////////////////////////////////////////////////////////////////

		void CCSettings::CreateManager()
		{
			TRACEHR (h);

			int waitCount = 3 * 60;		// 3 min.?! -- the value is taken from SMHelper.

            // Don't wait for the settings manager in safe mode because it's not running
            if( !GetSystemMetrics(SM_CLEANBOOT) )
            {
			    while (!ccSettings::CSettingsManagerHelper::IsSettingsManagerActive())
			    {
				    h.Verify(--waitCount > 0, SYM_SETTINGS_SVC_NOT_RUNNING);

				    h %= _T("Waiting for SettingsManager...");
				    h.Verify(ccSettings::CSettingsManagerHelper::IsSettingsManagerInstalled(), SYM_SETTINGS_SVC_NOT_RUNNING);

				    ccLib::CMessageLock msgLock (TRUE, TRUE);
				    ccLib::CEvent eventNothing;
				    eventNothing.Create (NULL, FALSE, FALSE, NULL, FALSE);

				    msgLock.Lock (eventNothing, 1000);
			    }
            }
            else
            {
                h %= _T("Not waiting for SettingsManager to be active in safe mode...");
            }


			h %= _T("Create SettingsManager");
			h << m_ccSettingsHelper.Create(m_pSettingsManager.m_p);
		}

		/////////////////////////////////////////////////////////////////////////////
	};
};
