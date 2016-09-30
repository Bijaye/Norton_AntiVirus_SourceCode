// NAVLicenseNames.h
//
//////////////////////////////////////////////////////////////////////

#if !defined(_NAVLICENSENAMES_H__3C571EF6_8F1E_4ffd_AC95_16D6DD31E93B__INCLUDED_)
#define _NAVLICENSENAMES_H__3C571EF6_8F1E_4ffd_AC95_16D6DD31E93B__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
#include "SuiteOwnerGuids.h"

const TCHAR NAV_MUTEX_NAME[]			= _T("NAVCFGWIZMutex");
const TCHAR SZ_NAV[]					= _T("NAV");
const TCHAR SZ_LICENSING_DLL[] 			= _T("NAVOPTRF.DLL");  

// This GUID is used to find the path to NAVCfgWizRes
//  HKLM\\Software\\Symantec\\Shared Technology\\ConfigWiz\\{NAV_CFGWIZRESGUID}\\DLL Path = "C:\\program files\\norton antivirus\\NAVCfgWizRes.dll"
const TCHAR SZ_NAV_CFGWIZRESGUID[] 		= _T("NAV");
const TCHAR SZ_NAV_LS_SERVICEID[] 		= _T("61D460E0-07C0-11d3-A985-00A0244D507A");                                

const TCHAR MSG_NAV_LICENSING_ALERT[]	= _T("MSG_NAV_LICENSING_ALERT_EBFAF15A-1609-439e-BE92-35AE79BD44AA");
const TCHAR SZ_DEFALERT_WNDCLASSNAME[]	= _T("DefAlert_ClassName_29DABAC8-AB93-43f3-926D-1DDE0C909FDF");
const TCHAR SZ_DEFALERT_WNDNAME[]		= _T("DefAlert_Window_29DABAC8-AB93-43f3-926D-1DDE0C909FDF");

typedef enum HWNDPARENT_FLAG
{
	HWNDPARENT_NULL = 0
	,HWNDPARENT_FIND
	,HWNDPARENT_PASSED
} HWNDPARENT_FLAG;


#define REQUEST_LICENSING_ALERT_ON_DEMAND    (0x00000000)
#define REQUEST_LICENSING_ALERT_ON_CHANGE    (0x00000001)


#endif // !defined(_NAVLICENSENAMES_H__3C571EF6_8F1E_4ffd_AC95_16D6DD31E93B__INCLUDED_)
