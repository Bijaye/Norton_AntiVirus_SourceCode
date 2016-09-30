// Copyright 1992-1996 Symantec, Peter Norton Product Group
//************************************************************************
//
// $Header:   S:/include/VCS/common.h_v   1.0   06 Feb 1997 20:51:08   RFULLER  $
//
// Description:
//      This include file defines the constants, typdefs, prototypes for
//      the Norton AntiVirus for DOS and Windows.
//
//************************************************************************
// $Log:   S:/include/VCS/common.h_v  $
// 
//    Rev 1.0   06 Feb 1997 20:51:08   RFULLER
// Initial revision
// 
//    Rev 1.7   19 Oct 1996 20:36:32   JALLEE
// Removed 'extern "C"' from front of proto for NavoptsControlPanel().
// This function is already so declared (with an ifdef for __cplusplus.)
// 
//    Rev 1.6   18 Oct 1996 14:09:22   TIVANOV
// change the define to NAVNET32
//
//    Rev 1.5   18 Oct 1996 13:47:16   TIVANOV
// change the function from cpp to extern "C"
//
//    Rev 1.4   23 Aug 1996 12:28:12   DALLEE
// Put back prototype for NetUserAlreadyExists() so it doesn't get
// name mangled.
//
//    Rev 1.3   17 Jul 1996 08:24:24   JWORDEN
// Add new parameters to prototype of NavOptionsDialog for NAVNETNT
//
//    Rev 1.2   01 May 1996 14:40:32   PVENKAT
// Fixed PLATFORM_NAV_NT (Earlier chkin was mistyped as _95).
//
//    Rev 1.1   01 May 1996 14:23:28   PVENKAT
// Added PLATFORM_NAV_NT.
//
//
//    Rev 1.0   30 Jan 1996 15:46:40   BARRY
// Initial revision.
//
//    Rev 1.0   30 Jan 1996 10:10:58   BARRY
// Initial revision.
//
//    Rev 1.26   24 Oct 1995 19:07:16   MKEATIN
// Disabled Network Alerts
//
//    Rev 1.25   23 Oct 1995 14:34:18   MKEATIN
// Put prototype NetUserAlreadyExists back in for Network Alerts.
//
//    Rev 1.24   16 Oct 1995 18:14:28   JWORDEN
// Replace ENVIRONMENT_ defines with PLATFORM_
// Change wEnvironment parameter in NavOptsControlPanel prototype
// to lpSectionSuffix
//
//    Rev 1.23   19 Sep 1995 08:25:06   JWORDEN
// Add define for ENVIRONMENT_MAX
// Add prototype for GetPlatformFromNNS
//
//    Rev 1.22   13 Sep 1995 10:32:32   JWORDEN
// Preliminary changes for LuigiNet
//
//    Rev 1.21   25 May 1995 20:27:10   DALLEE
// argh... C++ construct only #ifdef __cplusplus.
//
//    Rev 1.20   25 May 1995 20:16:02   DALLEE
// Added 3rd param to NavOptsControlPanel() for DOS so declaration is the same
// as in Windows.  Just pass a bogus value for the 1st panel to display.
//
//    Rev 1.19   25 May 1995 18:48:50   KEITH
// Fix DOS compile.
//
//    Rev 1.18   25 May 1995 18:20:24   KEITH
// Added startup panel number to NavOptsControlPanel
//
//    Rev 1.17   03 May 1995 12:50:30   MFALLEN
// Additional information when options changed.
//
//    Rev 1.16   25 Apr 1995 08:48:50   MFALLEN
// Additional possible return codes.
//
//    Rev 1.15   06 Apr 1995 13:18:24   MFALLEN
// Needed an internal flag to check when exclude stuff changed.
//
//    Rev 1.14   01 Feb 1995 17:19:20   DALLEE
// Removed duplicate prototype of UpdateTSR().
// Prototype lives in NAVUTIL.H
//************************************************************************

#ifndef _COMMON_H
#define _COMMON_H

#include "options.h"

// Bit flags in WORD returned from NavOptsControlPanel()

#define OPTIONS_UPDATETSR           ((WORD)0x0001)
#define OPTIONS_CHANGED             ((WORD)0x0002)
#define OPTIONS_EXCLUDECHANGED      ((WORD)0x0004)
#define OPTIONS_ADDVXD_ERROR        ((WORD)0x0008)
#define OPTIONS_REMOVEVXD_ERROR     ((WORD)0x0010)
#define OPTIONS_HIDEVXDICON         ((WORD)0x0020)
#define OPTIONS_SHOWVXDICON         ((WORD)0x0040)
#define OPTIONS_VXDUNLOADABLE       ((WORD)0x0080)
#define OPTIONS_VXDNOTUNLOADABLE    ((WORD)0x0100)

// Platform flags

#define PLATFORM_NONE             ((WORD)0x0000)
#define PLATFORM_NAV_95           ((WORD)0x0001)
#define PLATFORM_NAV_3            ((WORD)0x0002)
#define PLATFORM_NAV_NT           ((WORD)0x0004)
#define PLATFORM_ALL              ((WORD)0x7fff)

#define PLATFORM_COUNT            2

#ifdef __cplusplus
extern "C" {
#endif

//************************************************************************
// Global Prototypes
//************************************************************************

extern WORD WINAPI NavOptsControlPanel (       // Calls up Options Control Panel
    HWND         hWnd,                  // Handle of parent window
    HINSTANCE    hInstance,             // Handle of Instance
#ifdef NAVNETW
    LPSECTIONSUFFIX   lpSectionSuffix,  // Section suffix structure
#elif defined NAVNET32
    LPSTR        lpszInDataPath,        // Path for input data files
    LPSTR        lpszOutDataPath,       // Path for output data files (optional)
#endif
    WORD         wSetIndex              // Panel number to force on top
#ifdef __cplusplus
                          =0xFFFF       //  default param only for C++
#endif
    );

WORD WINAPI ReadNNSInMemory (LPSECTIONSUFFIX lpSectionSuffix);
WORD WINAPI GetPlatformsFromNNS (void);

BOOL PASCAL NetUserAlreadyExists(LPNETUSERITEM lpNetUserItem, LPNETUSER lpNetUser);

#ifdef __cplusplus
}
#endif

#endif  // _COMMON_H

