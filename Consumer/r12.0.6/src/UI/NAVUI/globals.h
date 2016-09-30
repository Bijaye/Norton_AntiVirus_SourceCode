#ifndef AFX_GLOBALS_H__5E100C81_FD65_11d2_B9D2_00C04FAC114C__INCLUDED_
#define AFX_GLOBALS_H__5E100C81_FD65_11d2_B9D2_00C04FAC114C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <navwhlp5.h>                   // help IDs
#include "naverror.h"
#include "AVccModuleId.h"


extern DJSMAR_LicenseType g_LicenseType;
extern DJSMAR00_LicenseState g_LicenseState;
extern long g_LicenseZone;
extern bool g_bInitLicense;

BOOL 	DoOptionsScreen ( HWND hParentWnd );
void    ErrorMessageBox ( HWND hMainWnd, UINT uNAVErrorResID, UINT ErrorID, UINT uModuleID = AV_MODULE_ID_MAIN_UI );
void    ErrorMessageBox ( HWND hMainWnd, BSTR bstrText, UINT ErrorID, UINT uModuleID = AV_MODULE_ID_MAIN_UI );
BOOL    WarningMessageBox ( HWND hMainWnd, UINT uIDText, UINT uIcon = MB_ICONERROR );
HRESULT GetLicensingInfo();
void    makeError(long lMessageID, long lHResult);
bool    forceError(long lErrorID);
void RequestLicensingAlert(HWND hWndParent, LONG nFlag);
void SetNamedEvent(LPCTSTR lpcEvent);
bool ShouldShowHelpMenuOption(LPCTSTR lpcszMenuOptName);
bool ShouldShowRenewalCenterHelpMenu();
extern "C" __declspec(dllexport) BOOL __stdcall FetchSupportURL(UINT iHint, LPTSTR lpsz);
extern "C" __declspec(dllexport) BOOL __stdcall Upsell(UINT iHint, LPTSTR lpsz);
extern "C" __declspec(dllexport) BOOL __stdcall FetchURL(UINT iHint, LPTSTR lpsz);

#endif  // def AFX_GLOBALS_H__5E100C81_FD65_11d2_B9D2_00C04FAC114C__INCLUDED_
