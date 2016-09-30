// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright (c) 2005 Symantec Corporation. All rights reserved.
// RtvBootStrap.cpp : Defines the entry point for the application.
//

#define WIN32_LEAN_AND_MEAN		// Exclude rarely-used stuff from Windows headers
#include <atlbase.h>
CComModule _Module;
#include <atlwin.h>
#include <commctrl.h>
#include "Resource.h"
#include "DarwinResCommon.h"
#include "RtvControler.h"

DARWIN_DECLARE_RESOURCE_LOADER(_T("RTVBSRes.dll"), _T("RTVBS"))

class CDlg : public CDialogImpl<CDlg>
{
	UINT m_uTID;         // Timer ID
	CNAV m_NAV;          // NAV service controler
	bool m_bInOnTimer;   // Q: Are we in the middle of timer processing?
	bool m_bStoped;      // Q: Did we stop the service?
	CWindow m_Progress;  // Progress bar

public:
	enum { IDD = IDD_ABOUTBOX, MAX_WAIT = 600 };

BEGIN_MSG_MAP(CDlg)
	MESSAGE_HANDLER(WM_INITDIALOG,        OnInitDialog)
	MESSAGE_HANDLER(WM_TIMER,             OnTimer)
	MESSAGE_HANDLER(WM_DESTROY,           OnDestroy)
	COMMAND_RANGE_HANDLER(IDOK, IDCANCEL, OnCommand)
END_MSG_MAP()

	LRESULT OnInitDialog(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnTimer(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnDestroy(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnCommand(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
};

LRESULT CDlg::OnInitDialog(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	// Setup
	m_Progress = GetDlgItem(IDC_STARTUP_PROGRESS);
	m_Progress.SendMessage(PBM_SETRANGE, 0, MAKELPARAM(0, MAX_WAIT));
	m_bStoped = m_bInOnTimer = false;
	m_uTID = SetTimer(1, 500);

	return 1UL;
}

LRESULT CDlg::OnTimer(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	TCHAR szMsg[128];

	// Advance progress bar
	if (m_Progress.SendMessage(PBM_STEPIT) == MAX_WAIT)
	{
		// We waited too long
		KillTimer(1);
		m_uTID = 0U;
		if (!m_bInOnTimer)
		{
			LoadString(_Module.GetResourceInstance(), IDS_Err_StartupFailed, szMsg, 128);
			MessageBox(szMsg);
		}
		EndDialog(-1);
	}

	if (m_bInOnTimer)
		return 0UL;
	m_bInOnTimer = true;

	// Check if the service is up already
	if (m_uTID == wParam)
	{
		if (m_NAV.Running())
		{
			m_NAV.Stop();
		}
		else
		{
			// Start the service.
			if (!m_NAV.Start())
			{
				LoadString(_Module.GetResourceInstance(), IDS_Err_StartupFailed, szMsg, 128);
				MessageBox(szMsg);
			}
			KillTimer(1);
			m_uTID = 0U;
			EndDialog(0);
		}
	}

	m_bInOnTimer = false;

	return 0UL;
}

LRESULT CDlg::OnDestroy(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	// Cleanup
	if (m_uTID)
		KillTimer(1);

	return 0UL;
}

LRESULT CDlg::OnCommand(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	// User aborted operation
	EndDialog(wID);
	return TRUE;
}

int APIENTRY WinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPSTR     lpCmdLine,
                     int       nCmdShow)
{
	INITCOMMONCONTROLSEX iccx = { sizeof(iccx), ICC_PROGRESS_CLASS };
	if (hPrevInstance || !InitCommonControlsEx(&iccx))
		return 0;

	_Module.Init(NULL, hInstance);
	if(SUCCEEDED(g_ResLoader.Initialize()))
	{
		_Module.SetResourceInstance(g_ResLoader.GetResourceInstance());
	}
	int ret =  CDlg().DoModal();
	_Module.Term();

	return ret;
}
