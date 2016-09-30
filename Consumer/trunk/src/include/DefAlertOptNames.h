////////////////////////
//
// PROPRIETARY / CONFIDENTIAL.
// Use of this product is subject to license terms.
// Copyright © 2006 Symantec Corporation.
// All rights reserved.
//
/////////////////////////////////////////////////////////////////////////////////////////////////

// DefAlertOptNames.h
//
//////////////////////////////////////////////////////////////////////

#if !defined(_DEFALERTOPTNAMES_H__A02D589C_F22C_4684_BEEA_7E514E665651__INCLUDED_)
#define _DEFALERTOPTNAMES_H__A02D589C_F22C_4684_BEEA_7E514E665651__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000


// DefAlert names stored in defalert.ini

const char ALERT_SubsEndDate[]						= "SubsEndDate";
const char ALERT_ALUDisabledShown[]					= "ALUDisabledShown";

const char ALERT_StartTimeOut[]						= "StartTimeOut";
const char ALERT_RepeatTimeOut[]					= "RepeatTimeOut";
const char ALERT_DefsOutOfDateCycle[]				= "DefsCycle";	
const char ALERT_SubsExpiredCycle[]					= "SECycle";
const char ALERT_LicenseWarningCycle[]				= "LWCycle";
const char ALERT_LicenseExpiredCycle[]				= "LECycle";
const char ALERT_InstallTime[]						= "InstallTime";
const char ALERT_LastLicAlert[]						= "LastLicType";
const TCHAR ALERT_EVENT_ProductCorrupt[]			= _T("ALERT_EVENT_ProductCorrupt{6D3B3510-410E-4412-9F3A-F61247BD06C7}");

const char ALERT_LastDisplayOldDefs[]				= "Defs";
const char ALERT_LastDisplaySubWarning[]			= "SW";
const char ALERT_LastDisplaySubExpired[]			= "SE";
const char ALERT_LastDisplaySubWarningAutoRenew[]	= "SWAR";
const char ALERT_LastDisplaySubExpiredAutoRenew[]	= "SEAR";
const char ALERT_LastDisplayLicViolated[]			= "LV";
const char ALERT_LastDisplayLicPreExpired[]			= "LPE";
const char ALERT_LastDisplayLicPreTrial[]			= "LPT";
const char ALERT_LastDisplayLicPreTrialExpired[]	= "LPTE";
const char ALERT_LastDisplayLicWarning[]			= "LW";
const char ALERT_LastDisplayLicExpired[]			= "LE";
const char ALERT_LastDisplayLicKilled[]				= "LK";
const char ALERT_LastDisplayLicPreGrace[]			= "LPG";
const char ALERT_LastDisplayLicPreGraceExpired[]	= "LPGE";
const char ALERT_CurrentAlerts[]					= "CurAlerts";

// Scanner alert values
const char ALERT_ScanDefsOutOfDateCycle[]			= "S_DCyc";	
const char ALERT_ScanSubsExpiredCycle[]				= "S_SECyc";
const char ALERT_ScanLastDisplayOldDefs[]			= "SDefs";
const char ALERT_ScanLastDisplaySubExpired[]		= "SSE";


#endif // !defined(_DEFALERTOPTNAMES_H__A02D589C_F22C_4684_BEEA_7E514E665651__INCLUDED_)
