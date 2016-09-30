// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright (c) 2005 Symantec Corporation. All rights reserved.
//
// CI.H
//

#ifndef __CI_H
#define __CI_H

//
// Definitions
//
#define ABOUT_FOLDER_NAME	_T( "\\ldvpciabout" )
#define REG_VP_BASE			_T( "SOFTWARE\\Intel\\LANDesk\\VirusProtect6\\CurrentVersion" )
#define REG_DMI_BASE		_T( "SOFTWARE\\Intel\\LANDesk\\VirusProtect6\\CurrentVersion\\DMI" )
#define REG_DMI_CUT_BASE	_T( "DMI" )
#define REG_DMI_CURRENT_STATE_BASE		REG_DMI_BASE _T( "\\Current State" )
#define REG_DMI_PREVIOUS_STATE_BASE		REG_DMI_BASE _T( "\\Reported State" )
#define REG_DMI_CUT_CURRENT_STATE_BASE	REG_DMI_CUT_BASE _T( "\\Current State" )
#define REG_VP_HOME_VALUE				_T( "Home Directory" )
#define REG_VP_PATTERN_NAME_VALUE		_T( "CurrentPatternName" )
#define REG_VP_PATTERN_DATE_VALUE		_T( "PatternFileDate" )
#define REG_VP_PATTERN_VERSION_VALUE	_T( "UsingPattern" )
#define REG_VP_REALTIME_KEY				_T( "Storages\\FileSystem\\RealtimeScan" )
#define REG_VP_ONOFF_VALUE				_T( "OnOff" )
#define REG_VP_READS_VALUE				_T( "Reads" )
#define REG_VP_WRITES_VALUE				_T( "Writes" )
#define REG_VP_FIRSTMACROACTION_VALUE	_T( "FirstMacroAction" )
#define REG_VP_SECONDMACROACTION_VALUE	_T( "SecondMacroAction" )
#define REG_VP_FIRSTACTION_VALUE		_T( "FirstAction" )
#define REG_VP_SECONDACTION_VALUE		_T( "SecondAction" )
#define REG_VP_EXTS_VALUE				_T( "Exts" )
#define REG_VP_FILETYPE_VALUE			_T( "FileType" )
#define REG_VP_MESSAGEBOX_VALUE			_T( "MessageBox" )
#define REG_VP_MESSAGEBEEP_VALUE		_T( "Beep" )
#define REG_VP_MESSAGETEXT_VALUE		_T( "MessageText" )
#define REG_VP_VENDOR_VALUE				_T( "ScanEngineVendor" )
#define REG_VP_ENGINE_VERSION_VALUE		_T( "ScanEngineVersion" )
#define REG_DMI_PATTERN_VALUE			_T( "Pattern Outdated" )
#define REG_DMI_INFECTED_VALUE			_T( "System Infected" )
#define REG_DMI_CIACTIVE_VALUE			_T( "CI Active" )
#define REG_DMI_FOUND_DURING_SCAN		_T( "Found During Scan" )
#define REG_VP_BACKWEB_BASE				_T( "BackWeb" )
#define REG_VP_BACKWEB_ACTIVE			_T( "OnOff" )
#define REG_DMI_CHECK_PATTERN			_T( "Warn About Pattern File" )

#define DMI_SCANNING_EVENT				_T( "LDVP_DMI_SCANNING_EVENT" )

// The following are used in the title of the debugging window:
#define	MAJOR_VERSION	6
#define MINOR_VERSION	0

#define MAXINFOBUF		2048	// max size for debugging buffer

// class string of unique group in component:
#define DMI_CLASS		"Intel|LDVP Realtime Scan Settings|001"

// The following are used to get the path to the mif file:
#define	LDCM_REG_PATH	"Software\\Intel\\LANDesk\\Client Manager"
#define	DEFAULT_PATH	"c:\\program files\\intel\\ldcm"
#define PATH_TO_MIF		"\\ci\\Intel\\LDVP\\"
#define MIF_NAME		"VPC32.MIF"

#endif // ndef __CI_H
