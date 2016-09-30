//////////////////////////////////////////////////////////////////////////////
// PROPRIETARY / CONFIDENTIAL.
// Use of this product is subject to license terms.
// Copyright © 2005 Symantec Corporation.
// All rights reserved.
//////////////////////////////////////////////////////////////////////////////
//
// const.h - contains constant values for Quarantine
//
/////////////////////////////////////////////////////////////////////////////
// $Log:   S:/QCONSOLE/VCS/const.h_v  $
// 
//    Rev 1.5   11 May 1998 15:46:02   DBuches
// Added more reg keus.
// 
//    Rev 1.4   27 Apr 1998 16:10:22   DBuches
// Added Corporate reg keys.
// 
//    Rev 1.3   07 Apr 1998 16:49:00   DBuches
// Added reg keys.
// 
//    Rev 1.2   03 Apr 1998 13:27:56   DBuches
// Removed un-needed column constant.
// 
//    Rev 1.1   06 Mar 1998 11:19:20   DBuches
// Added more UI regkeys.
// 
//    Rev 1.0   27 Feb 1998 15:10:24   DBuches
// Initial revision.
/////////////////////////////////////////////////////////////////////////////

#ifndef _CONST_H
#define _CONST_H

///////////////////////////////////////////////////////////////////////////
// Registry keys
const TCHAR SYMANTEC[]                        = _T("Symantec\\Norton AntiVirus");
const TCHAR REGKEY_QUARANTINE_UI[]            = _T("Software\\Symantec\\Norton AntiVirus\\Norton Quarantine and Restore");
const TCHAR REGKEY_QUARANTINE[]               = _T("Software\\Symantec\\Norton AntiVirus\\Quarantine");
const TCHAR REGKEY_INSTALLED_APPS[]           = _T("Software\\Symantec\\InstalledApps");
const TCHAR REGKEY_UI_SETTINGS[]              = _T("UI Settings");
const TCHAR REGKEY_VALUE_WP[]                 = _T("WindowPlacement");
const TCHAR REGKEY_VALUE_COLUMNS_VIRAL[]      = _T("ColumnsV");
const TCHAR REGKEY_VALUE_COLUMNS_EXPANDED[]   = _T("ColumnsE");
const TCHAR REGKEY_VALUE_TREEWIDTH[]          = _T("TreeViewWidth");
const TCHAR REGKEY_VALUE_DEFSDATE[]           = _T("DefsDate");
const TCHAR REGKEY_VALUE_SHOW_INTRO[]         = _T("ShowIntro");
const TCHAR REGKEY_VALUE_INSTALLED_APPS_NAV[] = _T("NAV");


///////////////////////////////////////////////////////////////////////////
// Default vaules
const TCHAR DEFAULT_COLUMN_WIDTHS[] = _T("130;120;80;80;130;135;130;100;");

/////////////////////////////////////////////////////////////////////////////
// Column indexes
//const int COLUMN_MIN         = 0;
const int COLUMN_THREAT_NAME  = 0;
const int COLUMN_QDATE        = 1;
const int COLUMN_CATEGORY     = 2;
const int COLUMN_RISK_LEVEL   = 3;
const int COLUMN_DEPENDENCIES = 4;
const int COLUMN_SDATE        = 5;
const int COLUMN_STATUS       = 6;
//const int COLUMN_MAX          = COLUMN_STATUS;
//const int COLUMN_COUNT        = (COLUMN_STATUS + 1);

// Alternate columns for viral view
//const int COLUMN_FILE_NAME    = 7;
//const int COLUMN_LOCATION     = 8;


//////////////////////////////////////////////////////////////////////////////
const int THREAT_PACKET_VERSION = 1;

#endif
