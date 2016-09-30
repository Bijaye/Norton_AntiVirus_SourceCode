// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright (c) 2005 Symantec Corporation. All rights reserved.
//
//////////////////////////////////////////////////////////////////////

#pragma once

#include "AVNotify.h"
#include "ccSettingsLoader.h"
#include "VPExceptionHandling.h"

enum NotifyType
{
	eReboot = 0,
	eFailure,
	eNone
};

//////////////////////////////////////////////////////////////////////
// CEraserNotify
//
// Implements notification for additional reboots required by
// Spyware remediation.  Also displays a failure notification if
// processing was unsuccessful.  Note that only one notification
// can be processed at a time.
//
class CEraserNotify : public CAVNotify
{
public:
	CEraserNotify();
	virtual ~CEraserNotify();

	void SetTrayWnd(const HWND hTrayWnd);
	NotifyType ProcessNotifyType();

protected:
	virtual Run();
	bool Snooze();
	void SetNotifyTimeout(NotifyType eType);
	void SetupNotifyProperties(NotifyType eType);
	bool ConfirmReboot() const;
	bool InitSettings();
	void InitNotifySettings();
	void RestartSystem() const;

private:
	ccLib::CCriticalSection m_critType;
	NotifyType m_eType;
	HWND m_hTrayWnd;
	HANDLE m_hTimer;
	DWORD m_dwSnoozeTimeout;
	DWORD m_dwNotifyTimeout;

	cc::ccSet_ISettingsManager3 m_ccSetLoader;
	ccSettings::ISettingsManager3Ptr m_pSettingsMgr;
};
