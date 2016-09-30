// DefAlertOptNames.h
//
//////////////////////////////////////////////////////////////////////

#if !defined(_DEFALERTOPTNAMES_H__A02D589C_F22C_4684_BEEA_7E514E665651__INCLUDED_)
#define _DEFALERTOPTNAMES_H__A02D589C_F22C_4684_BEEA_7E514E665651__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000


// DefAlert names stored in defalert.ini

const TCHAR ALERT_SubsEndDate[]					= _T("SubsEndDate");
const TCHAR ALERT_ALUDisabledShown[]			= _T("ALUDisabledShown");

const TCHAR ALERT_StartTimeOut[]				= _T("StartTimeOut");
const TCHAR ALERT_RepeatTimeOut[]				= _T("RepeatTimeOut");
const TCHAR ALERT_DefsOutOfDateCycle[]			= _T("DefsCycle");	
const TCHAR ALERT_SubsExpiredCycle[]			= _T("SECycle");
const TCHAR ALERT_LicenseWarningCycle[]			= _T("LWCycle");
const TCHAR ALERT_LicenseExpiredCycle[]			= _T("LECycle");
const TCHAR ALERT_InstallTime[]					= _T("InstallTime");
const TCHAR ALERT_LastLicAlert[]				= _T("LastLicType");
const TCHAR ALERT_EVENT_ProductCorrupt[]		= _T("ALERT_EVENT_ProductCorrupt{6D3B3510-410E-4412-9F3A-F61247BD06C7}");

const TCHAR ALERT_LastDisplayOldDefs[]				= _T("Defs");
const TCHAR ALERT_LastDisplaySubWarning[]			= _T("SW");
const TCHAR ALERT_LastDisplaySubExpired[]			= _T("SE");
const TCHAR ALERT_LastDisplaySubWarningAutoRenew[]	= _T("SWAR");
const TCHAR ALERT_LastDisplaySubExpiredAutoRenew[]	= _T("SEAR");
const TCHAR ALERT_LastDisplayLicViolated[]			= _T("LV");
const TCHAR ALERT_LastDisplayLicPreExpired[]		= _T("LPE");
const TCHAR ALERT_LastDisplayLicPreTrial[]			= _T("LPT");
const TCHAR ALERT_LastDisplayLicPreTrialExpired[]	= _T("LPTE");
const TCHAR ALERT_LastDisplayLicWarning[]			= _T("LW");
const TCHAR ALERT_LastDisplayLicExpired[]			= _T("LE");
const TCHAR ALERT_LastDisplayLicKilled[]			= _T("LK");
const TCHAR ALERT_LastDisplayLicPreGrace[]			= _T("LPG");
const TCHAR ALERT_LastDisplayLicPreGraceExpired[]	= _T("LPGE");
const TCHAR ALERT_CurrentAlerts[]					= _T("CurAlerts");

// Scanner alert values
const TCHAR ALERT_ScanDefsOutOfDateCycle[]			= _T("S_DCyc");	
const TCHAR ALERT_ScanSubsExpiredCycle[]			= _T("S_SECyc");
const TCHAR ALERT_ScanLastDisplayOldDefs[]			= _T("SDefs");
const TCHAR ALERT_ScanLastDisplaySubExpired[]		= _T("SSE");


#endif // !defined(_DEFALERTOPTNAMES_H__A02D589C_F22C_4684_BEEA_7E514E665651__INCLUDED_)
