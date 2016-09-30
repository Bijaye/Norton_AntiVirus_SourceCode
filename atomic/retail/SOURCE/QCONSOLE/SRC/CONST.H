/////////////////////////////////////////////////////////////////////////////
// $Header:   S:/QCONSOLE/VCS/const.h_v   1.5   11 May 1998 15:46:02   DBuches  $
/////////////////////////////////////////////////////////////////////////////
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
#define SYMANTEC                    _T( "Symantec\\Norton AntiVirus" )
#define REGKEY_QUARANTINE_UI        _T( "Software\\Symantec\\Norton AntiVirus\\Norton AntiVirus Quarantine" )
#define REGKEY_QUARANTINE           _T( "Software\\Symantec\\Norton AntiVirus\\Quarantine" )
#define REGKEY_NAV                  _T( "Software\\Symantec\\Norton AntiVirus" )
#define REGKEY_UI_SETTINGS          _T( "UI Settings" )
#define REGKEY_VALUE_LISTVIEW_MODE  _T( "ListMode" )
#define REGKEY_VALUE_WP             _T( "WindowPlacement" )
#define REGKEY_VALUE_COLUMNS        _T( "Columns" )
#define REGKEY_VALUE_SHOWTREE       _T( "ShowTree" )
#define REGKEY_VALUE_TREEWIDTH      _T( "TreeViewWidth" )
#define REGKEY_VALUE_DEFSDATE       _T( "DefsDate" )
#define REGKEY_VALUE_CORPORATE      _T( "CorporateFeatures" )
#define REGKEY_VALUE_REPAIR_PATH    _T( "RepairPath" )

///////////////////////////////////////////////////////////////////////////
// Default vaules
#define DEFAULT_COLUMN_WIDTHS       _T( "130;130;110;100;100;100;100;" )
#define DEFAULT_EXPORT_FILENAME     _T( "Quarantine.txt" )

/////////////////////////////////////////////////////////////////////////////
// Column indexes
#define COLUMN_FILE_NAME    0
#define COLUMN_LOCATION     1
#define COLUMN_QDATE        2
#define COLUMN_SDATE        3
#define COLUMN_STATUS       4
#define COLUMN_VIRUS_NAME   5
#define COLUMN_MAX          ( COLUMN_VIRUS_NAME + 1 )

#endif