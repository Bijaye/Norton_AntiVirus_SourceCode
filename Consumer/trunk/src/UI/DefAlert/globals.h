////////////////////////
//
// PROPRIETARY / CONFIDENTIAL.
// Use of this product is subject to license terms.
// Copyright © 2006 Symantec Corporation.
// All rights reserved.
//
/////////////////////////////////////////////////////////////////////////////////////////////////

// globals.h
//
//////////////////////////////////////////////////////////////////////

#if !defined(_GLOBALS_H__7B894158_AB9D_4be3_8982_AA666909E0AF__INCLUDED_)
#define _GLOBALS_H__7B894158_AB9D_4be3_8982_AA666909E0AF__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

typedef struct
{
	SYMALERT_TYPE		Type;
	SYMALERT_CATEGORY	Category;
	LPCSTR				lpszLastDisplay;
	LPCSTR				lpszCycle;
}ALERT_MAP;

typedef struct
{
	HWND hWndParent;
	ALERT_MAP alertMap;
}ALERT_ITEM;

extern const			long   MAX_DEFS_AGE;
extern const			DWORD _5_MINUTES;
extern const			DWORD _6_HOURS;
extern const			DWORD _1_HOUR;
extern const			DWORD _1_DAY;
extern TCHAR			g_szAlertFile[];
extern TCHAR			g_szUserName[];
extern const TCHAR SZ_NAV_SUBEXPIRED_ONSCAN_ALERT_MUTEX[];
extern const TCHAR SZ_NAV_OLDDEFS_ONSCAN_ALERT_MUTEX[];
extern const LPCWSTR SYMSETUP_MUTEX;

// Global function prototypes
bool _LookupAlertMap(SYMALERT_TYPE Type, ALERT_MAP& item);
LPCSTR _LookupALERT_TimeDisplay(SYMALERT_TYPE Type);
HRESULT _GetUserName(void);
HRESULT _GetAlertDataFile(void);
DWORD _GetAlertDword(LPCSTR lpszName, DWORD dwDefaultVal, BOOL bUserName = TRUE);
HRESULT _SetAlertDword(LPCSTR lpszName, DWORD dwVal, BOOL bUserName = TRUE);
HRESULT _SetCollectionAlertDword(LPCSTR lpszName, DWORD dwVal);
void _ErrorMessageBox(HWND hWnd, UINT uErrorID, UINT uNAVErrorResID);
bool _IsActWizRunning();
bool _IsCfgWizRunning();
bool _IsCfgWizFinished();

#endif // !defined(_GLOBALS_H__7B894158_AB9D_4be3_8982_AA666909E0AF__INCLUDED_)
