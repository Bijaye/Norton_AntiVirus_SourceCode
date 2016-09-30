#pragma once
#include "TraceHR.h"
#include "ccAlertLoader.h"

class CSnoozeAlert
{
public:
	static BOOL Snooze(long hParentWnd, SnoozeFeature snoozeFeatures, BOOL bCanTurnOffAP);
	static void SetSnoozePeriod(SnoozeFeature snoozeFeatures, long lValue);
	static long GetSnoozePeriod(SnoozeFeature snoozeFeature);
};

class CSnoozeAlertDlg : public cc::IAlertCallback,
	ISymBaseImpl<CSymThreadSafeRefCount>
{
private:
	HWND m_hParent;
	CSnoozeAlertDlg(void) {}

public:

	CSnoozeAlertDlg(HWND hParent)
	{
		m_hParent = hParent;
	}

	SYM_INTERFACE_MAP_BEGIN()
		SYM_INTERFACE_ENTRY(cc::IID_AlertCallback, IAlertCallback)
	SYM_INTERFACE_MAP_END()

	BOOL DisplayAlert(SnoozeFeature snoozeFeatures, BOOL bCanTurnOffAP);

	// ICcAlertCallback methods
	virtual bool Run(HWND hWndParent, unsigned long nData, cc::IAlert* pAlert, cc::IAlertCallback::ALERTCALLBACK context);
};