#ifndef _CONST_H_
#define _CONST_H_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/////////////////////////////////////////////////////////////////////////////
//
// Names of all AP drivers.
//

const TCHAR SYMEVENT_SERVICE_NAME[]		= _T("SymEvent");
const TCHAR NAVENG_SERVICE_NAME[]		= _T("NAVENG");
const TCHAR NAVEX_SERVICE_NAME[]		= _T("NAVEX15");
const TCHAR NAVAP_SERVICE_NAME[]		= _T("SAVRT");
const TCHAR NAVAPEL_SERVICE_NAME[]      = _T("SAVRTPEL");
const TCHAR NAVAP_EVENT_NAME[]			= _T("NAV Auto-Protect Event");

/////////////////////////////////////////////////////////////////////////////
//
// Names of definition driver files.
//

#ifdef _ALPHA_
const TCHAR NAVAP_NAVENG_SYS[]			= _T( "NAVENGDA.Sys" );
const TCHAR NAVAP_NAVEX_SYS[]			= _T( "NavExDa.Sys" );
#else
const TCHAR NAVAP_NAVENG_SYS[]			= _T( "NAVENG.Sys" );
const TCHAR NAVAP_NAVEX_SYS[]			= _T( "NavEx15.Sys" );
#endif // #ifdef _ALPHA_

/////////////////////////////////////////////////////////////////////////////
//
// Hawkings Application ID(s) - Note: NON-UNICODE.
//
const char DEFUTILS_APP_ID[]			= "NAVNT_50_AP1";
const char DEFUTILS_APP_ID2[]			= "NAVNT_50_AP2";


/////////////////////////////////////////////////////////////////////////////
//
// Registry keys and values
//

const TCHAR REGKEY_INSTALLED_APPS[] = _T("SOFTWARE\\Symantec\\InstalledApps");
const TCHAR REGVAL_AVENGEDEFS[] = _T("AVENGEDEFS");
const TCHAR REGKEY_QUARANTINE[] = _T("SOFTWARE\\Symantec\\Norton AntiVirus\\Quarantine");
const TCHAR REGVAL_INCOMINGDIR[] = _T("IncomingPath");


/////////////////////////////////////////////////////////////////////////////
// Misc. strings.
const TCHAR DEFAULT_USER_NAME[] = _T("NT_SYSTEM");


/////////////////////////////////////////////////////////////////////////////
// Configuration file name.

const TCHAR CONFIG_FILENAME[] = _T("NAVOPTS.DAT");

/////////////////////////////////////////////////////////////////////////////
// Definition Authentication values
enum AUTHENTICATION_STATUS
{
    AUTHENTICATION_FAILED = 0,
    AUTHENTICATION_REVERTED
};

/////////////////////////////////////////////////////////////////////////////

#endif // ifndef _CONST_H_