/////////////////////////////////////////////////////////////////////
// At_UIExp.h : main header file for the ALERTUI DLL
//
// Copyright 1992-1996 Symantec, Peter Norton Product Group
/////////////////////////////////////////////////////////////////////
// $Header:   S:/INCLUDE/VCS/AT_UIEXP.H_v   1.2   27 May 1997 22:16:04   DBUCHES  $
//
// Description:
//  This file contains the exported functions for ALERTUI.DLL.
//
/////////////////////////////////////////////////////////////////////
//
// $Log:   S:/INCLUDE/VCS/AT_UIEXP.H_v  $
// 
//    Rev 1.2   27 May 1997 22:16:04   DBUCHES
// Added SaveAlertOptions()
// 
//    Rev 1.1   27 May 1997 20:16:34   DBUCHES
// Changes to support change to MFC Extension DLL.
// 
//    Rev 1.0   24 Apr 1997 20:49:40   JBRENNA
// Initial revision.
// 
//    Rev 1.0   13 Mar 1997 15:10:46   RFULLER
// Initial revision
// 
//    Rev 1.8   28 Jan 1997 20:00:24   SEDWARD
// Added 'InitControlsEx' export function.
//
//    Rev 1.7   03 Dec 1996 19:49:52   SEDWARD
// Changed MASTER_NLM_ITEM_STR
//
//    Rev 1.6   02 Dec 1996 19:34:04   SEDWARD
// Added 'GetAlertOptDlgHwnd'
//
//    Rev 1.5   27 Nov 1996 18:46:36   SEDWARD
// Added NAVOPTS argument to 'CreateAlertOptsDlg' function.
//
//    Rev 1.4   26 Nov 1996 21:29:26   SEDWARD
// Added quake include files.
//
//    Rev 1.3   26 Nov 1996 01:58:04   DSACKIN
// Added the _T() back to the string.
//
//    Rev 1.2   26 Nov 1996 01:55:22   DSACKIN
// Removed the _T() wrapper around the MASTER NLM string.
//
//    Rev 1.1   26 Nov 1996 00:54:08   SEDWARD
// Added master NLM string definition.
//
//    Rev 1.0   22 Nov 1996 11:11:10   SEDWARD
// Initial revision.
//
//    Rev 1.0   21 Nov 1996 17:22:28   SEDWARD
// Initial revision.
//
/////////////////////////////////////////////////////////////////////

#ifndef AT_UIEXP_HEADER
#define AT_UIEXP_HEADER

#include    "platform.h"
#include    "options.h"     // for LPNAVOPTIONS

#define DllImport	__declspec( dllimport )
#define DllExport	__declspec( dllexport )


#define MASTER_NLM_ITEM_STR     _T("Include All NetWare Servers")

DllExport   BOOL    AlertOptsOK(void);
DllExport   BOOL    DestroyAlertOptsDlg(void);
DllExport   HWND    GetAlertOptDlgHwnd(void);
DllExport   void    InitControlsEx(void);

extern "C" {
void __declspec( dllexport ) InitAlertExtDll(void);
void __declspec( dllexport ) SaveAlertOptions(void);
DllExport   LPVOID  CreateAlertOptsDlg(LPNAVOPTIONS  lpNavOptions);
}

// pointer to InitAlertExtDLL()
typedef void (*LPFNINITALERTEXTDLL)(void);
// pointer to CreateAlertOptsDlg()
typedef LPVOID (*LPFNCREATEALERTOPTSDLG)(LPNAVOPTIONS);
// Save options
typedef void (*LPFNSAVEALERTOPTS)(void);

#endif  // AT_UIEXP_HEADER
