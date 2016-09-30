#ifndef _QSCOMMON_H
#define _QSCOMMON_H

// 
// Hawkings program ID for ourselves.
// 
#define QSCONSOLE_HAWKINGS_ID       "QSADMIN_10"

// 
// Registry keys for console.
// 
const TCHAR REGKEY_QSCONSOLE[]          = _T( "Software\\Symantec\\Norton AntiVirus\\QSConsole" );
const TCHAR REGVAL_INSTALL_DIR[]        = _T( "InstallPath" );
const TCHAR REGVAL_LANGUAGE[]           = _T( "Language" );

const TCHAR REGKEY_INSTALLED_APPS[]     = _T( "Software\\Symantec\\InstalledApps" );
const TCHAR REGKEY_INSTALLED_NAV[]      = _T( "NAVNT" );


// 
// Name of AP manipulation DLL.
// 
const TCHAR NAVAPI_NAME[]               = _T( "NAVAP32.DLL" );

#endif
