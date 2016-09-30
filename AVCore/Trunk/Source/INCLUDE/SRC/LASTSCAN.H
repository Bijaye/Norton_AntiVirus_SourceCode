// Copyright 1998 Symantec, Peter Norton Product Group
//***************************************************************************
//
// $Header:   S:/INCLUDE/VCS/lastscan.h_v   1.1   18 Sep 1998 10:53:08   tcashin  $
//
// Description:
//
//      This file contains values for NAVs LastScan registry key. These key
//      are written by NAV when a scan completes.
//
//***************************************************************************
// $Log:   S:/INCLUDE/VCS/lastscan.h_v  $
// 
//    Rev 1.1   18 Sep 1998 10:53:08   tcashin
// Added Log record.
// 
//***************************************************************************
#ifndef _LASTSCAN_H_
#define _LASTSCAN_H_

//***************************************************************************
// LastScan Registry Key
//***************************************************************************
#define NAV_LASTSCAN_KEY            "SOFTWARE\\Symantec\\Norton AntiVirus\\LastScan";
#define NAV_LASTSCAN_KEY_NT         "SOFTWARE\\Symantec\\Norton AntiVirus\\LastScan";


//***************************************************************************
// Boot Records Statistics Keys
//***************************************************************************
#define NAV_LASTSCAN_BRINFECTED     "BRInfected";
#define NAV_LASTSCAN_BRREPAIRED     "BRRepaired";
#define NAV_LASTSCAN_BRSCANNED      "BRScanned";


//***************************************************************************
// File Statistics Keys
//***************************************************************************
#define NAV_LASTSCAN_FILESDELETED   "FilesDeleted";
#define NAV_LASTSCAN_FILESINFECTED  "FilesInfected";
#define NAV_LASTSCAN_FILESQUAR      "FilesQuarantined";
#define NAV_LASTSCAN_FILESREPAIRED  "FilesRepaired";
#define NAV_LASTSCAN_FILESSCANNED   "FilesScanned";


//***************************************************************************
// Master Boot Record Statistics Keys
//***************************************************************************
#define NAV_LASTSCAN_MBRSCANNED     "MBRScanned";
#define NAV_LASTSCAN_MBRINFECTED    "MBRInfected";
#define NAV_LASTSCAN_MBRREPAIRED    "MBRRepaired";


//***************************************************************************
// Summary Statistics Keys
//***************************************************************************
#define NAV_LASTSCAN_DRIVES         "DrivesScanned";
#define NAV_LASTSCAN_SCANTIME       "ScanTime";
#define NAV_LASTSCAN_SYSTEMTIME     "SystemTime";


//***************************************************************************
// Summary Settings Keys
//
// The following keys are bit fields in DWORD reg keys. The values for each 
// field are listed below. Undefined bits should be ignored when parsing and 
// preserved when writing.
//***************************************************************************
#define NAV_LASTSCAN_SCANFILES      "ScanFiles";    // See LASTSCAN_FILETYPES
#define NAV_LASTSCAN_SCANMEMORY     "ScanMemory";   // See LASTSCAN_MEMSCAN
#define NAV_LASTSCAN_SCANSETTINGS   "ScanSettings"; // See LASTSCAN_SETTINGS
#define NAV_LASTSCAN_SCANSUMMARY    "ScanSummary";  // See LASTSCAN_SUMMARY


//***************************************************************************
// Summary Settings Key Values
//***************************************************************************
#define LASTSCAN_SUMMARY_NOINFECTEDITEMS    0x00    // Data values for ScanSummary
#define LASTSCAN_SUMMARY_INFECTEDFILES      0x01
#define LASTSCAN_SUMMARY_INFECTEDBOOT       0x02

#define LASTSCAN_MEMSCAN_NOTSUPPORTED       0x00    // Data values for ScanMemory
#define LASTSCAN_MEMSCAN_YES                0x01
#define LASTSCAN_MEMSCAN_NO                 0x02

#define LASTSCAN_FILETYPES_ALL              0x00    // Data values for ScanFiles
#define LASTSCAN_FILETYPES_PROGRAMFILES     0x01

#define LASTSCAN_SETTINGS_NOSCANCOMPRESSED  0x00    // Data values for ScanSettings
#define LASTSCAN_SETTINGS_SCANCOMPRESSED    0x01


#endif