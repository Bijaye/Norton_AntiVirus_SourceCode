#ifdef WIN32
#ifdef REAL_SERVICE

#ifndef __Win2KUserLogon_h__
#define __Win2KUserLogon_h__
#include "stdafx.h"
#include "AccessChecker.h"
#include "ScanStatus.h"
#include "WinUser.h"
#include "Walker.h"
#include "UIManager.h"

class CWin2KUserLogon
{
public:
	CWin2KUserLogon();
	virtual ~CWin2KUserLogon();
	bool HandleUserLogOn();
	bool HandleUserLogOff();
	void SetLoggedOff() {m_bIsLoggedOff = true;};
	void CheckInitialLogon();
	HANDLE GetAccessToken() {return m_hAccessToken;};

private: 
	bool m_bIsLoggedOff;
	DWORD m_dwPid;
	HANDLE m_hAccessToken;
	TCHAR m_szUserName[1024];
	bool m_bStartLogonDetection;

	DWORD ProcessLogon();
	void ProcessLogout();
	DWORD GetSidString(PSID sid, char *out, size_t out_bytes);
	bool IsCopiedDefaultScanOptions(PHKEY phCustomTasksKey);
	DWORD CheckUserKey(PSID sid,DWORD RootID, HKEY *phUserKey);
	HRESULT CopyDefaultScanOptions(void);
};










#endif __Win2KUserLogon_h__
#endif
#endif