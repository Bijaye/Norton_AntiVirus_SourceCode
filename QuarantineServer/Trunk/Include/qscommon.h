// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright (c) 2005 Symantec Corporation. All rights reserved.
#ifndef _QSCOMMON_H
#define _QSCOMMON_H

// 
// Hawkings program ID for ourselves.
// 
#define QSCONSOLE_HAWKINGS_ID       "QSADMIN_10"

// 
// Registry keys for console.
// 
const TCHAR REGKEY_QSCONSOLE[]          = _T( "Software\\Symantec\\Symantec AntiVirus\\QSConsole" );
const TCHAR REGVAL_INSTALL_DIR[]        = _T( "InstallPath" );
const TCHAR REGVAL_LANGUAGE[]           = _T( "Language" );
const TCHAR REGVAL_REFRESH_INTERVAL[]	= _T( "Refresh Interval" );
const TCHAR REGKEY_INSTALLED_APPS[]     = _T( "Software\\Symantec\\InstalledApps" );
const TCHAR REGKEY_INSTALLED_NAV[]      = _T( "NAVNT" );
// jhill 1/3/99
const TCHAR REGVALUE_SELECTED_QSERVER_VERSION[] = _T("selectedQserverVersion"); 

// tmarles 2-1-00
const TCHAR REGVALUE_GATEWAY_HISTORY[] = _T("Gateway History"); 

// 
// Name of AP manipulation DLL.
// 
const TCHAR NAVAPI_NAME[]               = _T( "NAVAP32.DLL" );


#define REFRESH_TIME_INTERVELBASE	60000L // milliseconds (60 seconds)
#define REFRESH_TIME_DEFAULT		1		// Default to one min if can't read from registry

#define QSVERISON 3
#define QSVERSION_STRING "3.1"
#define QSVERSION2X 2

// tm  11-11-03 default for generate SESA events.  1 = generate, 0 = disable
#define	QS_GENERATE_SESA_EVENTS_DEFAULT		1

const TCHAR CLEAR_STATUS_TEXT[]	 =_T(" | ");

#endif
