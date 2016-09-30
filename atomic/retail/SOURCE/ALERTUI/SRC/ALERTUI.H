/////////////////////////////////////////////////////////////////////
// AlertUI.h : main header file for the ALERTUI DLL
//
// Copyright 1992-1996 Symantec, Peter Norton Product Group
/////////////////////////////////////////////////////////////////////
// $Header:   S:/ALERTUI/VCS/ALERTUI.h_v   1.3   04 Jun 1997 21:14:38   DBUCHES  $
//
// Description:
//  Main header file for the ALERTUI DLL.
//
/////////////////////////////////////////////////////////////////////
//
// $Log:   S:/ALERTUI/VCS/ALERTUI.h_v  $
// 
//    Rev 1.3   04 Jun 1997 21:14:38   DBUCHES
// Changed to support SNMP Alerts.
// 
//    Rev 1.2   27 May 1997 20:18:44   DBUCHES
// Converted to MFC Extension DLL.
// 
//    Rev 1.1   25 Apr 1997 11:03:58   JBRENNA
// Ported from Tesla.
// 
//    Rev 1.1   15 Apr 1997 20:03:10   sedward
// Removed functions now handled by CAlertListPropertyPage class.
//
//    Rev 1.12   02 Feb 1997 16:36:02   SEDWARD
// Added 'CheckGroupboxControl'.
//
//    Rev 1.11   28 Jan 1997 17:20:28   SEDWARD
// Added 'howto' for single edit SMTP record dialog.
//
//    Rev 1.10   19 Jan 1997 23:48:02   SEDWARD
// Added new prototypes.
//
//    Rev 1.9   16 Jan 1997 02:13:50   SEDWARD
// Lots of updates in help maps and menus.
//
//    Rev 1.8   13 Jan 1997 20:41:48   SEDWARD
// Added support for help.
//
//    Rev 1.7   06 Jan 1997 00:42:12   SEDWARD
// Fixed pointer problem in HandleMachineTargetEdit.
//
//    Rev 1.5   09 Dec 1996 20:48:50   SEDWARD
//
//    Rev 1.4   02 Dec 1996 16:34:04   SEDWARD
// Big changes, nearing UI freeze.
//
//    Rev 1.3   27 Nov 1996 18:48:00   SEDWARD
// Hooked up NAVOPTS to local alert message configuration.
//
//    Rev 1.2   26 Nov 1996 17:21:42   SEDWARD
// Leading backslashes no longer written to DAT file, domains get a trailing
// space/asterisk appened to them.
//
//    Rev 1.1   25 Nov 1996 18:18:16   SEDWARD
// Added "at_opts.h".
//
//    Rev 1.0   25 Nov 1996 15:45:06   SEDWARD
// Initial revision.
//
//    Rev 1.0   21 Nov 1996 17:22:28   SEDWARD
// Initial revision.
//
/////////////////////////////////////////////////////////////////////



#ifndef ALERT_UI_HEADER
#define ALERT_UI_HEADER

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "at_uiexp.h"
#include "at_opts.h"        // CAlertOptions class
#include "alrtres.h"        // main symbols
#include "netbrwsd.h"       // MFC net browser


// help filename
#define NAV_HELPFILENAME    _T("NAVWNT.HLP")

// "how to" index values (NOTE: these values indicate each submenu's position
// in the IDR_CONTEXT_HELP_MENU resource in "alertui.rc")
#define HOW_TO_INDEX_NET_MESSENGER              1
#define HOW_TO_INDEX_NLM                        2
#define HOW_TO_INDEX_FORWARD                    3
#define HOW_TO_INDEX_SMTP                       4
#define HOW_TO_INDEX_LOCAL_MACHINE              5
#define HOW_TO_INDEX_ALERT_TARGETS              6
#define HOW_TO_INDEX_SINGLE_SMTP_EDIT           7

// global variables
extern  CAlertOptions       gAlertOptions;
extern  LPNAVOPTIONS        glpNavOptions;      // NAV options

// indices into checkboxes bitmap
const   int         UNCHECKED_ITEM          = 0;
const   int         CHECKED_ITEM            = 1;
const   int         MASTER_NLM_ITEM         = 2;
const   int         UNCHECKED_GRAY          = 3;
const   int         CHECKED_GRAY            = 4;

// defined constants
#define APPEND_DOMAIN_STR                   _T("*")
#define global                              /**/

const   int         EDIT_ADD                = 1;
const   int         EDIT_MODIFY             = 2;
const   int         LIST_ITEM_MAX_STRLEN    = 256;




/////////////////////////////////////////////////////////////////////////////



// non-class functions
CWnd*           CheckGroupboxControl(CPoint*  pointPtr, CWnd**  wndPtrArray);
void            GetEmailDisplayStr(CEMailTarget*  recPtr
                                                , CString&  displayStr);
BOOL            HandleMachineTargetEdit(CAlertTarget*  recPtr, int  mode
                                                , CWnd*  parentWnd);
BOOL            HandleSMTPEdit(CAlertTarget*  recPtr, int  mode = EDIT_ADD);
BOOL            HandleSNMPEdit(CAlertTarget*  recPtr, int  mode = EDIT_ADD);
BOOL            InsertServerBackslashes(CString&  serverStr);
BOOL            IsDomainString(CString&  inputStr);
BOOL            IsValidEmailRec(CEMailTarget*  recPtr);
BOOL            MasterNLMItemInTargetsList(void);

#endif  // ALERT_UI_HEADER
