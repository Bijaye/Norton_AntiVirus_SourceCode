// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright 1997 - 2003, 2005 Symantec Corporation. All rights reserved.
//****************************************************************************
//
// $Header:   S:/NAVLU/VCS/STRINGS.H_V   1.1.2.0   22 Jun 1998 19:51:28   RFULLER  $
//
// Description:
//   Provide constant strings that should not be translated.
//
//****************************************************************************
// $Log:   S:/NAVLU/VCS/STRINGS.H_V  $
// 
// Taken from AVCORE NAVLU:
//
//    Rev 1.1.2.0   22 Jun 1998 19:51:28   RFULLER
// Branch base for version QAKN
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
MYEXTERN LPCTSTR g_szCmdLinesKey            MYINIT(_T("SOFTWARE\\INTEL\\LANDesk\\VirusProtect6\\CurrentVersion\\LiveUpdate\\CmdLines"));
MYEXTERN LPCTSTR g_szCmdLineNKey            MYINIT(_T("SOFTWARE\\INTEL\\LANDesk\\VirusProtect6\\CurrentVersion\\LiveUpdate\\CmdLines\\CmdLine%d"));
MYEXTERN LPCTSTR g_szMngProdsKey            MYINIT(_T("SOFTWARE\\Symantec\\Managed Products\\"));
MYEXTERN LPCTSTR g_szMngProdCmdLinesKey     MYINIT(_T("SOFTWARE\\Symantec\\Managed Products\\%s\\LiveUpdate\\CmdLines"));
MYEXTERN LPCTSTR g_szMngProdCmdLineNKey     MYINIT(_T("SOFTWARE\\Symantec\\Managed Products\\%s\\LiveUpdate\\CmdLines\\CmdLine%d"));
MYEXTERN LPCTSTR g_szOtherSwitchesValue     MYINIT(_T("OtherSwitches"));
MYEXTERN LPCTSTR g_szProductNameValue       MYINIT(_T("ProductName"));
MYEXTERN LPCTSTR g_szProductTypeValue       MYINIT(_T("ProductType"));
MYEXTERN LPCTSTR g_szProductLanguageValue   MYINIT(_T("ProductLanguage"));
MYEXTERN LPCTSTR g_szProductVersionValue    MYINIT(_T("ProductVersion"));
MYEXTERN LPCTSTR g_szCallbacksHereValue	  MYINIT(_T("CallbacksHere"));
MYEXTERN LPCTSTR g_szGroupValue	           MYINIT(_T("GroupTogether"));

// Cleanup our local macros
#undef MYEXTERN
#undef MYINIT

#endif // __STRINGS_H
