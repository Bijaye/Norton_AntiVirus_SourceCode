// Copyright 1997 Symantec, Peter Norton Product Group
//****************************************************************************
//
// $Header:   S:/NAVLUCBK/VCS/strings.h_v   1.0   08 Jan 1999 19:25:16   CEATON  $
//
// Description:
//   Provide constant strings that should not be translated.
//
//****************************************************************************
// $Log:   S:/NAVLUCBK/VCS/strings.h_v  $
// 
//    Rev 1.0   08 Jan 1999 19:25:16   CEATON
// Initial revision.
// 
//    Rev 1.0   14 Dec 1998 17:50:04   BMCCORK
// Initial revision.
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
MYEXTERN LPCTSTR g_szProductMonikerValue	MYINIT(_T("ProductMoniker"));
MYEXTERN LPCTSTR g_szCallbacksHereValue	MYINIT(_T("CallbacksHere"));
MYEXTERN LPCTSTR g_szGroupValue	MYINIT(_T("GroupTogether"));
MYEXTERN LPCTSTR g_szSetSKUValue MYINIT(_T("SetSKUProperty"));
MYEXTERN LPCTSTR g_szSetEnvironment	MYINIT(_T("SetEnvironment"));


// Location of the internal option settings.
MYEXTERN LPCTSTR g_szOptKey                 MYINIT(_T("Software\\Symantec\\Norton AntiVirus\\LiveUpdate"));
MYEXTERN LPCTSTR g_szOptShowSchedAddDlgValue MYINIT(_T("ShowScheduleAddDialog"));
MYEXTERN LPCTSTR g_szOptRetryCountValue     MYINIT(_T("RetryCount"));

// Miscellaneous strings
MYEXTERN LPCTSTR g_szProgramName            MYINIT(_T("LuAll.exe"));
MYEXTERN LPCTSTR g_szAppEventName           MYINIT(_T("NavLu is already running"));

// ProductRegCOM property name strings 
#include "ProductRegCOMNames.h"
MYEXTERN LPCTSTR g_szProduct				MYINIT( LUPR_PRODUCT );
MYEXTERN LPCTSTR g_szVersion				MYINIT( LUPR_VERSION );
MYEXTERN LPCTSTR g_szLanguage				MYINIT( LUPR_LANGUAGE );
MYEXTERN LPCTSTR g_szType					MYINIT( LUPR_TYPE );
MYEXTERN LPCTSTR g_szDescriptiveName		MYINIT( LUPR_DESCRIPTIVENAME );
MYEXTERN LPCTSTR g_szStatusText				MYINIT( LUPR_STATUSTEXT );
MYEXTERN LPCTSTR g_szUpdateStatus			MYINIT( LUPR_UPDATESTATUS );
MYEXTERN LPCTSTR g_szDownloadStatus			MYINIT( LUPR_DOWNLOADSTATUS );
MYEXTERN LPCTSTR g_szPatchStatus			MYINIT( LUPR_PATCHSTATUS );
MYEXTERN LPCTSTR g_szLiveUpdateRetCode		MYINIT( LUPR_LIVEUPDATERETURNCODE );
MYEXTERN LPCTSTR g_szOverrideGroup			MYINIT( LUPR_OVERRIDEGROUP );
MYEXTERN LPCTSTR g_szAbort					MYINIT( LUPR_ABORT );
MYEXTERN LPCTSTR g_szAbortText              MYINIT( LUPR_ABORT_TEXT );


// Cleanup our local macros
#undef MYEXTERN
#undef MYINIT

#endif // __STRINGS_H
