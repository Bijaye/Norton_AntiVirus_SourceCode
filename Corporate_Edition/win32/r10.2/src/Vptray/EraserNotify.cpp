// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright (c) 2005 Symantec Corporation. All rights reserved.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "resource.h"
#include "EraserNotify.h"
#include "ccMessageLock.h"
#include "ccSingleLock.h"

#include "ClientCCSettingNames.h"

CEraserNotify::CEraserNotify() : 
	m_eType(eNone), m_hTrayWnd(NULL), m_hTimer(NULL),
	m_dwSnoozeTimeout(0), m_dwNotifyTimeout(0)
{
}

CEraserNotify::~CEraserNotify()
{
	if (m_hTimer)
	{
		CloseHandle(m_hTimer);
		m_hTimer = NULL;
	}
}

void CEraserNotify::SetTrayWnd(const HWND hTrayWnd)
{
	m_hTrayWnd = hTrayWnd;
}

bool CEraserNotify::InitSettings()
{
	bool bRet = false;

	if (SYM_SUCCEEDED(m_ccSetLoader.CreateObject(m_pSettingsMgr.m_p)))
		bRet = true;

	return bRet;
}

void CEraserNotify::InitNotifySettings()
{
	if (m_pSettingsMgr != NULL)
	{
		ccSettings::ISettingsPtr pSettings;

		SYMRESULT sr = m_pSettingsMgr->CreateSettings(szCCSet_Key_Reboot_Processing_Options, &pSettings);
		if (SYM_SUCCEEDED(sr) && pSettings != NULL)
		{
			// Get the snooze timeout value
			DWORD dwVal = 0;
			if (SYM_SUCCEEDED(pSettings->GetDword(szCCSet_Val_Snooze_Timeout, dwVal)))
				m_dwSnoozeTimeout = dwVal;
			else
				m_dwSnoozeTimeout = DEFAULT_Val_Snooze_Timeout;

			if (SYM_SUCCEEDED(pSettings->GetDword(szCCSet_Val_Notify_Timeout, dwVal)))
				m_dwNotifyTimeout = dwVal;
			else
				m_dwNotifyTimeout = DEFAULT_Val_Notify_Timeout;
		}
	}
}

int CEraserNotify::Run()
{
	CCTRACEI(_T("%s : Starting Eraser notify thread.\n"), __FUNCTION__);

	HRESULT hr = ccLib::CCoInitialize::CoInitialize(ccLib::CCoInitialize::eMTAModel);

	// Return immediately if we failed to initialize COM.  (COM is required for ccSettings)
	if (FAILED(hr))
		return 0;

	// Return immediately if we failed to initialize ccSettings.
	if (!InitSettings())
		return 0;

	// Initialize internal settings.  Use defaults if not present.
	InitNotifySettings();

	// Create the snooze timer.
	m_hTimer = CreateWaitableTimer(NULL, TRUE, NULL);

	NotifyType eType = eNone;

	// Determine the notify type if one hasn't been set.
	ccLib::CSingleLock Lock(&m_critType, INFINITE, FALSE);
	
	if (m_eType == eNone)
		eType = ProcessNotifyType();
	else
		eType = m_eType;

	Lock.Unlock();

	// Do we need to display a notify window?
	if (eType != eNone)
	{
		// Loop for reboot notifications.
		bool bQuit = false;
		for (;;)
		{
			SetupNotifyProperties(eType);
			SetNotifyTimeout(eType);

			cc::INotify::RESULT res = cc::INotify::RESULT_ERROR_UNKNOWN;

			if (m_pNotify != NULL)
				res = m_pNotify->Display(NULL);

			CCTRACEI(_T("%s : ccNotify return code: %d\n"), __FUNCTION__, res);

			if (eType == eReboot)
			{
				if (res == cc::INotify::RESULT_OK)
				{
					// Post a message to the main thread that we need to reboot.
					if (ConfirmReboot())
					{
						RestartSystem();
						bQuit = true;
					}
				}
				else
				{
					// Snooze unless the thread is exiting.
					if (!Snooze())
						bQuit = true;
				}
			}
			else
			{
				// Not a reboot warning:  No need to continue looping.
				bQuit = true;
			}

			// Reset the notify window attributes
			if (m_pNotify != NULL)
				m_pNotify->Reset();

			InitNotify();

			if (bQuit == true)
			{
				m_eType = eNone;
				break;
			}
		}
	}

	ccLib::CCoInitialize::CoUninitialize();

	return 0;
}

void CEraserNotify::SetNotifyTimeout(NotifyType eType)
{
	if (eType == eReboot)
		if (m_pNotify != NULL)
			m_pNotify->SetTimeout(m_dwNotifyTimeout * 1000);
}

NotifyType CEraserNotify::ProcessNotifyType()
{
	ccLib::CSingleLock Lock(&m_critType, INFINITE, FALSE);

	if (m_pSettingsMgr != NULL)
	{
		ccSettings::ISettingsPtr pSettings;

		SYMRESULT sr = m_pSettingsMgr->CreateSettings(szCCSet_Key_Reboot_Processing_Options, &pSettings);
		if (SYM_SUCCEEDED(sr) && pSettings != NULL)
		{
			// Should we display a reboot notification?
			DWORD dwVal = 0;
			if (SYM_SUCCEEDED(pSettings->GetDword(szCCSet_Val_Eraser_Reboot, dwVal)))
			{
				if (dwVal >= 1)
				{
					m_eType = eReboot;

					pSettings->DeleteValue(szCCSet_Val_Eraser_Reboot);
					m_pSettingsMgr->PutSettings(pSettings, true);
				}
			}
			else // Should we display a failure notification?
			if (SYM_SUCCEEDED(pSettings->GetDword(szCCSet_Val_Eraser_Fail, dwVal)))
			{
				if (dwVal >= 1)
				{
					m_eType = eFailure;

					pSettings->DeleteValue(szCCSet_Val_Eraser_Fail);
					m_pSettingsMgr->PutSettings(pSettings, true);
				}
			}
		}
	}

	return m_eType;
}

void CEraserNotify::SetupNotifyProperties(NotifyType eType)
{
	if (m_pNotify != NULL)
	{
		try
		{
			ccLib::CString sButtonOk;
			ccLib::CString sButtonCancel;
			ccLib::CString sText;

			switch (eType)
			{
			case eReboot:
				{
					sButtonOk.LoadString(IDS_REBOOT);
					sButtonCancel.LoadString(IDS_REBOOT_LATER);
					sText.LoadString(IDS_REMEDIATION_REBOOT_WARNING);

					m_pNotify->SetProperty(cc::INotify::PROPERTIES::PROPERTY_OK_BUTTON, true);
					m_pNotify->SetProperty(cc::INotify::PROPERTIES::PROPERTY_CANCEL_BUTTON, true);

					m_pNotify->SetButton(cc::INotify::BUTTONTYPE_OK, sButtonOk);
					m_pNotify->SetButton(cc::INotify::BUTTONTYPE_CANCEL, sButtonCancel);
				}
				break;
			case eFailure:
				{
					sText.LoadString(IDS_REMEDIATION_FAILURE);
					sButtonOk.LoadString(IDS_OK);

					m_pNotify->SetProperty(cc::INotify::PROPERTIES::PROPERTY_OK_BUTTON, true);
					m_pNotify->SetButton(cc::INotify::BUTTONTYPE_OK, sButtonOk);
				}
				break;
			default:
				break;
			}

			m_pNotify->SetText(sText);
		}
		VP_CATCH_MEMORYEXCEPTIONS
		(
			CCTRACEE(_T("%s : Caught memory exception.\n"), __FUNCTION__);
		)
	}
}

bool CEraserNotify::Snooze()
{
	bool bRet = false;

	if (m_hTimer)
	{
		LARGE_INTEGER liDueTime = {0};

		liDueTime.QuadPart = -((__int64)(m_dwSnoozeTimeout * 10000000));

		if (SetWaitableTimer(m_hTimer, &liDueTime, 0, NULL, NULL, 0))
		{
			// Wait for either the timer to expire or the thread to exit.
			HANDLE hEvents[] = {m_hTimer, m_Terminating.GetHandle()};

			// Pump messages while waiting
			ccLib::CMessageLock msgWait(TRUE, TRUE);

			DWORD dwRet = msgWait.Lock(CCDIMOF(hEvents), hEvents, FALSE, INFINITE, FALSE);

			// Did the timer expire?
			if (dwRet == WAIT_OBJECT_0)
				bRet = true;
		}
	}

	return bRet;
}

bool CEraserNotify::ConfirmReboot() const
{
	bool bRet = false;

	try
	{
		ccLib::CString sConfirm;
		ccLib::CString sApp;

		sConfirm.LoadString(IDS_REBOOT_CONFIRM);
		sApp.LoadString(IDS_APP_TITLE);

		int nRet = MessageBox(NULL, sConfirm.GetBuffer(), sApp.GetBuffer(), MB_YESNO | MB_ICONWARNING);

		if (nRet == IDYES)
			bRet = true;
	}
	VP_CATCH_MEMORYEXCEPTIONS
	(
		CCTRACEE(_T("%s : Caught memory exception.\n"), __FUNCTION__);
	)

	return bRet;
}

void CEraserNotify::RestartSystem() const
{
	HANDLE hToken;
	TOKEN_PRIVILEGES tkp;

	if (OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hToken))
	{
		if (LookupPrivilegeValue(NULL, TEXT("SeShutdownPrivilege"), &tkp.Privileges[0].Luid))
		{
			tkp.PrivilegeCount = 1;
			tkp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;

			if (AdjustTokenPrivileges(hToken, FALSE, &tkp, 0, (PTOKEN_PRIVILEGES)NULL, 0) == FALSE)
				CCTRACEE(_T("%s : Failed to add shutdown privilege to token.  Error: %d\n"), __FUNCTION__, GetLastError());
		}

		CloseHandle(hToken);
	}

	ExitWindowsEx(EWX_REBOOT, 0);
}
