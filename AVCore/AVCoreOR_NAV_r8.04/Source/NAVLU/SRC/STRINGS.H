// Copyright 1997 Symantec, Peter Norton Product Group
//****************************************************************************
//
// $Header:   S:/NAVLU/VCS/strings.h_v   1.1   26 Jul 1997 10:52:22   JBRENNA  $
//
// Description:
//   Provide constant strings that should not be translated.
//
//****************************************************************************
// $Log:   S:/NAVLU/VCS/strings.h_v  $
// 
//    Rev 1.1   26 Jul 1997 10:52:22   JBRENNA
// Add g_szAppEventName.
// 
//    Rev 1.0   03 Jun 1997 14:04:36   JBRENNA
// Initial revision.
// 
//****************************************************************************

#ifndef __STRINGS_H
#define __STRINGS_H

// This should only be defined once in any CPP file for this project.
#ifdef BUILDING_NAVLUSTRINGS
#define MYEXTERN
#define MYINIT(x)   = x
#else
#define MYEXTERN    extern
#define MYINIT(x)
#endif

//************************************************************************
// Actual strings. Do NOT translate any string in this file!!!!
//************************************************************************

// Location of the LiveUpdate command lines.
MYEXTERN LPCTSTR g_szCmdLinesKey            MYINIT(_T("SOFTWARE\\Symantec\\Norton AntiVirus\\LiveUpdate\\CmdLines"));
MYEXTERN LPCTSTR g_szCmdLineNKey            MYINIT(_T("SOFTWARE\\Symantec\\Norton AntiVirus\\LiveUpdate\\CmdLines\\CmdLine%d"));
MYEXTERN LPCTSTR g_szOtherSwitchesValue     MYINIT(_T("OtherSwitches"));
MYEXTERN LPCTSTR g_szProductNameValue       MYINIT(_T("ProductName"));
MYEXTERN LPCTSTR g_szProductTypeValue       MYINIT(_T("ProductType"));
MYEXTERN LPCTSTR g_szProductLanguageValue   MYINIT(_T("ProductLanguage"));
MYEXTERN LPCTSTR g_szProductVersionValue    MYINIT(_T("ProductVersion"));

// Location of the internal option settings.
MYEXTERN LPCTSTR g_szOptKey                 MYINIT(_T("Software\\Symantec\\Norton AntiVirus\\LiveUpdate"));
MYEXTERN LPCTSTR g_szOptShowSchedAddDlgValue MYINIT(_T("ShowScheduleAddDialog"));
MYEXTERN LPCTSTR g_szOptRetryCountValue     MYINIT(_T("RetryCount"));

// Miscellaneous strings
MYEXTERN LPCTSTR g_szProgramName            MYINIT(_T("NavLu32.exe"));
MYEXTERN LPCTSTR g_szAppEventName           MYINIT(_T("NavLu is already running"));


// Cleanup our local macros
#undef MYEXTERN
#undef MYINIT

#endif // __STRINGS_H
