// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright (c) 2005 Symantec Corporation. All rights reserved.
/////////////////////////////////////////////////////////////////////////
//
// STDHELP.H
//
/////////////////////////////////////////////////////////////////////////
//
// $Log:   S:/INCLUDE/VCS/stdhelp.h_v  $
// 
//    Rev 1.5   01 Aug 1997 14:13:44   MBROWN
// Moved #defines from stdhelp.cpp to stdhelp.h to expose them to client apps.
//
//    Rev 1.4   28 Feb 1996 13:23:58   BARRY
// Fix VXD compile -- don't define dependence on LPHELPINFO structure
//
//    Rev 1.3   21 Feb 1996 18:58:16   YVANG
// Added #ifdef STD_POPUP_HELP_INFODESK #undef STD_POPUP_HELP_INFODESK and
//       #ifdef STD_POPUP_HELP_TERFINDER #undef STD_POPUP_HELP_TERMFINDER to
//       avoid redefinition warning messages.
//
//    Rev 1.2   21 Feb 1996 11:50:20   YVANG
// Added a comment which supposed to be included in previous revision check-in.
//
//    Rev 1.1   21 Feb 1996 11:47:00   YVANG
// Merges the NAV function prototypes and #defines.
//
//    Rev 1.0   26 Jan 1996 20:19:10   RJACKSO
// Initial revision.
//
//    Rev 1.53   07 Nov 1995 13:21:14   DCHERMA
// Added SHCM_NO_INFODESK.
//
//    Rev 1.52   02 Aug 1995 00:09:22   AWELCH
// Merge changes from Quake 7.
//
//    Rev 1.51   14 Jun 1995 16:08:38   GEORGEL
// Made StdHelpWhatsThisEx for Win32 only
//
//    Rev 1.50   13 Jun 1995 23:53:56   GEORGEL
// Added StdHelpWhatsThisEx and StdHelpInfoDesk
//
//    Rev 1.49   23 May 1995 15:49:38   REESE
// Enhance context menus to support the Symantec Companion.
//
//    Rev 1.48   03 May 1995 09:41:12   MFALLEN
// This file was moved to the trunk from Branch 6. Too many people were dependent
// on old API's and functionality. My research showed that only NAV was using the
// new APIs but if this change brakes you please let me know.
//
//    Rev 1.37.1.4   09 Mar 1995 05:34:10   GEORGEL
// Added InfoDesk
//
//    Rev 1.37.1.3   02 Mar 1995 23:45:56   GVOGEL
// StdHelpFromHelpBtn should only exist in Win32.
//
//    Rev 1.37.1.2   15 Feb 1995 13:01:02   DLEVITON
// Added STDHELP_INFODESK
//
//    Rev 1.37.1.1   08 Feb 1995 18:57:28   DLEVITON
// Added StdHelpFromHelpBtn.
//
//    Rev 1.37.1.0   14 Jan 1995 02:26:58   PATRICKM
// Branch base for version QAK6
//
//    Rev 1.37   09 Jan 1995 14:03:52   BILL
// Split up ContextMenu functions
//
//    Rev 1.36   05 Jan 1995 16:34:34   BILL
//
//    Rev 1.35   05 Jan 1995 16:28:16   BILL
// Added Subitem query registered message
//
//    Rev 1.34   01 Dec 1994 18:34:28   REESE
// Added SHCM_NO_INITIALIZATION stuff.
//
//    Rev 1.33   14 Nov 1994 16:17:42   REESE
// Added StdHelpWhatsThis prototype.
//
//    Rev 1.32   18 Oct 1994 01:44:40   GEORGEL
// Added another flag for StdHelpContextMenu
//
//    Rev 1.31   17 Oct 1994 15:00:00   GEORGEL
// Minor change to flags and structure for StdHelpContextMenu
//
//    Rev 1.30   13 Oct 1994 03:22:40   GEORGEL
// Added:
//   StdHelpContextMenu function
//   CONTEXTMENUSTRUCT structure
//   SHCM_xxx flags
//
//    Rev 1.29   27 Sep 1994 19:18:40   MFALLEN
//
//    Rev 1.28   31 Aug 1994 13:14:10   GEORGEL
// Added TermFinder
//
//    Rev 1.27   25 Aug 1994 23:28:46   GEORGEL
// Added additional string for StdPopupHelpFromDialog
//
//
//    Rev 1.26   13 Aug 1994 04:35:24   GEORGEL
// Changed the flags for StdPopupHelpFromDialog
//
//    Rev 1.25   13 Aug 1994 01:45:18   GEORGEL
// Changed the flag for StdPopupHelpFromDlg
//
//    Rev 1.24   11 Aug 1994 05:14:36   GEORGEL
// Added StdPopupHelpFromDialog
//
//    Rev 1.23   25 May 1994 15:29:22   GVOGEL
// Changed @Library to @Include in comment blocks for Geologist project.
//
//    Rev 1.22   16 May 1994 05:22:36   GVOGEL
// Added comment blocks for Geologist project.
//
//    Rev 1.21   26 Apr 1994 16:39:24   BRAD
// Removed PVCS Logs
/////////////////////////////////////////////////////////////////////////

/*@Library: SYMUTIL*/

#ifndef _STDHELP_H
#define _STDHELP_H

/*@STRUCTAPI:*****************************************************************/
typedef struct _HELPTABLE
    {
    UINT        resID;
    UINT        helpID;
    UINT        statusID;
    } HELPTABLE;

typedef HELPTABLE NEAR *NPHELPTABLE;
typedef HELPTABLE FAR  *LPHELPTABLE;
/*****
@Name: HELPTABLE

@Description:
This structure is used to map resource IDs to help IDs. It is part of the
HELPSTRUCT structure.

@Members:
$resID$ Resource ID. Set to zero to signify there are no more entries in the
table.

$helpID$ Help ID corresponding to the resource ID.

$statusID$ Status ID (optional) corresponding to the resource ID.

@See: HELPSTRUCT
@Include: stdhelp.h
******************************************************************************/


/*@STRUCTAPI:*****************************************************************/
typedef struct _HELPSTRUCT
    {
    char        helpFileName[SYM_MAX_PATH+1];
    HINSTANCE   hinst;
    HWND        hwnd;
    HWND        statusWindow;
    LPHELPTABLE helpTab;
    } HELPSTRUCT;

typedef HELPSTRUCT  NEAR  *NPHELPSTRUCT;
typedef HELPSTRUCT  FAR   *LPHELPSTRUCT;
/*****
@Name: HELPSTRUCT

@Description:
This structure is used to initialize (and reinitialize) the standard help
engine.

@Members:
$helpFileName$ Help file name.

$hInst$ Instance of the application.

$hwnd$ Main window for the application.

$statusWindow$ Status line (optional).

$helpTab$ Pointer to help ID table (optional). If this parameter is NULL,
help IDs are equivalent to the resource IDs.  If it is non-null, an entry
with a resID of zero marks the end of the table.

@See: HELPTABLE StdHelpInit StdHelpActivate
@Include: stdhelp.h
******************************************************************************/

/*@STRUCTAPI:*****************************************************************/
typedef struct _CONTEXTMENUSTRUCT
    {
    HWND        hDlg;                         /* Handle of dialog box         */
    LONG        lXYPos;                       /* X and Y of the mouse click   */
    RECT        rectWhatsThis;                /* rect of the What's This item */
    UINT        uiFlags;                      /* Various flags                */
    DWORD*      dwpWhatsThisMap;              /* P to array of DWORD for WhatsThis */
    DWORD*      dwpHowToMap;                  /* P to array of DWORD for HowTo     */
    DWORD*      dwpUpperMenuItemsMap;         /* P to array of DWORD for Add'l items */
    DWORD*      dwpLowerMenuItemsMap;         /* P to array of DWORD for Add'l items */
	HINSTANCE	hInstance;	                  /* If SHCM_NO_INITIALIZATION, then application instance */
	char const * szHelpFile;	              /* If SHCM_NO_INITIALIZATION, then name of help file */
	HMENU       hMenu;                        /* If SHCM_RETURN_HMENU hmenu returned instead of tracking */
    int         iWhatsThisHelpID;             /* Set to current what's this item */
	char*		szCompanionMenuText;	// Text for companion menu item
	char*		szCompanionCmdText;		// Execute this when companion selected
    } CONTEXTMENUSTRUCT;

typedef CONTEXTMENUSTRUCT  NEAR  *NPCONTEXTMENUSTRUCT;
typedef CONTEXTMENUSTRUCT  FAR   *LPCONTEXTMENUSTRUCT;
/*****
@Name: CONTEXTMENUSTRUCT

@Description:
This structure is used to call the StdHelpContextMenu function.

@Members:
$hDlg$ Handle of dialog box making the call.

$lXYPos$ X and Y of the mouse click (just pass the lParam along).

$rectWhatsThis$ A rect which gets circled when the user selects What's This.

$uiFlags$ Various flags for the function.  TODO: Document them

$dwpWhatsThisMap$  A pointer to an array of DWORDs pairs for an optional
"What's This?" menu item.
If the pointer is NULL, no "What's This?" menu is added to the popup menu.
If the pointer in non-NULL, then an attempt is made to see if there is a
corresponding "What's This?" explanation for the control directly underneath
the mouse pointer.  Unless overriden by a specific flag, the DWORD pairs are
intrepreted as follows:
  1) The first DWORD of each pair is a resource ID for any dialog control
     which has a "What's This?" help explanation.
  2) The second DWORD of each pair is a help ID which corresponds to the
     explanation of the particular dialog control.
  3) The pairs of DWORDS must be terminated with a pair of NULLs.
This intrepretation can be overriden with the (TODO:????) flag.  In such case,
no attempt is made to compare the mouse position with controls of the dialog.
Instead, this pointer is intrepreted as a pointer to a single DWORD which
contains a help ID.

$dwpHowToMap$  A pointer to an array of DWORDs pairs for optional "How To"
help menu items.
If the pointer is NULL, no "How To" menu is added to the popup menu at all.
If the pointer in non-NULL, then a "How To" menu item is added to the popup
menu and various submenus are created below it as indicated by the DWORD
pairs.  The DWORDs are intrepreted in pairs as follows:
  1) The first DWORD of each pair is a resource ID for a string which will be
     loaded and added as an item to the "How To" submenu.
  2) The second DWORD of each pair is a help ID for the help command to which
     the string corresponds.  If a user selects an "How To" menu item, this
     help ID is sent to the windows help system and the user will see the
     help without any further intervention of the calling application.
  3) The pairs of DWORDS must be terminated with a pair of NULLs.

$dwpUpperMenuItemsMap$ A pointer to an array of DWORDs pairs for additional
menu items which will be added to the popup menu ABOVE the help-related
menu items.  A seperator menu item will automatically be placed at the boundry
between the two.
If the pointer is NULL, no attempt is made to add additional menu items.
If the pointer in non-NULL, then the DWORDs to which it points are intrepreted
in pairs as follows:
  1) The first DWORD of each pair is a resource ID for a string which will be
     loaded and appended at the bottom of the popup menu.
  2) The second DWORD of each pair is a command ID for the command to which
     the string corresponds.  If a user selects an additional menu item, this
     command ID is sent back to the calling application in an WM_COMMAND message.
  3) The pairs of DWORDS must be terminated with a pair of NULLs.

$dwpLowerMenuItemsMap$ A pointer to an array of DWORDs pairs for additional
menu items which will be added to the popup menu BELOW the help-related
menu items.  A seperator menu item will automatically be placed at the boundry
between the two.
If the pointer is NULL, no attempt is made to add additional menu items.
If the pointer in non-NULL, then the DWORDs to which it points are intrepreted
in pairs as follows:
  1) The first DWORD of each pair is a resource ID for a string which will be
     loaded and appended at the bottom of the popup menu.
  2) The second DWORD of each pair is a command ID for the command to which
     the string corresponds.  If a user selects an additional menu item, this
     command ID is sent back to the calling application in an WM_COMMAND message.
  3) The pairs of DWORDS must be terminated with a pair of NULLs.

$szCompanionMenuText$ Additional text to display on the context menu (designed
for use with the Symantec Companion.) The field is only used if SHCM_COMPANION
option is set.

$szCompanionCmdText$ Text that is executed when szCompanionMenuText is
selected. The field is only used if SHCM_COMPANION option is set.

@See: HELPTABLE StdHelpContextMenu StdHelpInit StdHelpActivate
@Include: stdhelp.h
******************************************************************************/
#ifdef __cplusplus
extern "C" {
#endif

WORD SYM_EXPORT WINAPI
StdHelpInit(LPHELPSTRUCT i);

WORD SYM_EXPORT WINAPI
StdHelpActivate(HWND h, LPHELPSTRUCT i);

WORD SYM_EXPORT WINAPI
StdHelpFromID(LPHELPSTRUCT i, UINT id);

WORD SYM_EXPORT WINAPI
StdHelpFromDialog(HWND t, UINT s);

WORD SYM_EXPORT WINAPI
StdHelpSetContext(UINT t, UINT s);

UINT SYM_EXPORT WINAPI
StdHelpSetHelpFileID(HWND t, UINT s);

WORD SYM_EXPORT WINAPI
StdHelpFromMenu(HWND t, UINT s, UINT i, long l);

UINT SYM_EXPORT WINAPI StdHelpGetHelpFileID (HWND hWnd);

WORD SYM_EXPORT WINAPI
StdHelpFromHelp(HWND t, UINT s);

#if defined( SYM_WIN32 ) || defined( _WIN32 )
WORD SYM_EXPORT WINAPI
StdHelpFromHelpBtn(HWND hDlg, LPCSTR szKeyword);
#endif

WORD SYM_EXPORT WINAPI
StdHelpShutdown(HWND t);

WORD SYM_EXPORT WINAPI
StdPopupHelpFromDialog (HWND hDlg, LONG lParam, UINT uiFlags,
                        DWORD *dwWhatsThisMap, DWORD *dwHowToMap);

BOOL SYM_EXPORT WINAPI
StdHelpContextMenu (LPCONTEXTMENUSTRUCT i);

BOOL SYM_EXPORT WINAPI
StdHelpProcessContextMenu (LPCONTEXTMENUSTRUCT i, int iMenuItem, BOOL bSendCommand);

BOOL SYM_EXPORT WINAPI
StdHelpWhatsThis ( LPCONTEXTMENUSTRUCT i );

WORD SYM_EXPORT WINAPI StdHelpWithParams(
    HWND        hDlg,                   // Handle of dialog box
    LPSTR       lpszHelpFile,           // Helpfile name, (fully qualified)
    LPHELPTABLE lpHt,                   // Help table
    WORD        wIDD,                   // Unique IDD_* for dialog box
    WORD        wHelp);                 // Help item ID (if known)

#ifndef SYM_VXD
#if defined( SYM_WIN32 ) || defined( _WIN32 )
BOOL SYM_EXPORT WINAPI
StdHelpWhatsThisEx ( LPCONTEXTMENUSTRUCT i, LPHELPINFO j );
#endif
#endif

BOOL SYM_EXPORT WINAPI
StdHelpInfoDesk ( LPCONTEXTMENUSTRUCT i );

#if !defined( SYM_WIN32 ) && !defined( _WIN32 )
// Sets up the debugging mode so calls to Custom Help Macro buttons
// can be debugged by WinHelp author.
void SYM_EXPORT WINAPI SHSetDebug ( HWND hwndApp );

// Removes all buttons from a secondary WinHelp window.
// Sets up the context info required for StdHelpCreateBttn.
void SYM_EXPORT WINAPI SHSetWHC (
     DWORD dwApp,
     DWORD dwContext,
     LPSTR qchPath );

// Creates a button in a secondary WinHelp window.
void SYM_EXPORT WINAPI SHCreatBtn (
     DWORD dwContext,
     LPSTR B_Text,                    // Text to display on button.
     LPSTR H_Macro,                   // WinHelp macro(s) to execute.
     WORD  Enabled );

#endif //!defined( SYM_WIN32 ) || !defined( _WIN32 )


/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
//
// NAV_SPECIFIC_START
//
/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////

WORD SYM_EXPORT WINAPI
StdHelpExInit(LPHELPSTRUCT i);

#if defined( SYM_WIN32 ) || defined( _WIN32 )
WORD SYM_EXPORT WINAPI
StdHelpExFromHelpBtn(HWND hDlg, LPCSTR szKeyword);
#endif

WORD SYM_EXPORT WINAPI
StdExPopupHelpFromDialog (HWND hDlg, LONG lParam, UINT uiFlags,
                        DWORD *dwWhatsThisMap, DWORD *dwHowToMap);

BOOL SYM_EXPORT WINAPI
StdHelpExContextMenu (LPCONTEXTMENUSTRUCT i);

BOOL SYM_EXPORT WINAPI
StdHelpExProcessContextMenu (LPCONTEXTMENUSTRUCT i, int iMenuItem, BOOL bSendCommand);

BOOL SYM_EXPORT WINAPI
StdHelpExWhatsThis ( LPCONTEXTMENUSTRUCT i );

VOID SYM_EXPORT WINAPI
StdHelpExGetHelpInfo( LPHELPSTRUCT lpHelpInf );

typedef int (FAR *LB_ITEMSELECTIONPROC) (HWND hwndListBoxKindOfCtrl);
#define IDH_UNKNOWN  32760

VOID SYM_EXPORT WINAPI
StdHelpExOnContextHelp
(
    HWND    hDlg,                       /* [in] dialog that targer the message*/
    UINT    uMsg,                       /* [in] WM_HELP or WM_CONTEXTMENU     */
    WPARAM  wParam,                     /* [in] wParam from dialog            */
    LPARAM  lParam,                     /* [in] lParam from dialog            */
    DWORD  *pdwarrWhatsThisMap,         /* [in] 0,0 term. arr. of CtrlHelpIDs */
    DWORD  *pdwarrHowToMap,             /* [in] 0,0 term. arr. of CtrlHelpIDs */
    DWORD  *pdwarrLBItemsHelpStrings,   /* [in] 0 term. arr.of ItemsHelpStrIDs*/
    LB_ITEMSELECTIONPROC lpfnGetSelItem /* ret. 0 based idx of sel.item in LB */
);

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
//
// NAV_SPECIFIC_END
//
/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
#ifdef __cplusplus
}
#endif

/*--------------------------------------------------
  FLAGS for StdHelpProcessContextMenu
---------------------------------------------------*/
#define SPHFD_WHATSTHIS_MENU_ID  1
#define SPHFD_INFODESK_MENU_ID   2
#define SPHFD_INDEX_MENU_ID      3
#define SPHFD_TERMFINDER_MENU_ID 4
#define SPHFD_COMPANION_MENU_ID  5
#define SPHFD_CONTENTS_MENU_ID   6

/*--------------------------------------------------
  FLAGS for StdPopupHelpFromDlg
---------------------------------------------------*/
#define SPHFD_SINGLE_WHATS_THIS        1
#define SPHFD_MAPPED_WHATS_THIS        2

/*--------------------------------------------------
  FLAGS for StdHelpContextMenu
---------------------------------------------------*/
#define SHCM_NO_FLAGS                    0x00000000L
#define SHCM_SINGLE_WHATS_THIS           0x00000001L
#define SHCM_HELP_IN_SUBMENU             0x00000002L
#define SHCM_1ST_WHATS_THIS_IS_DEFAULT   0x00000004L
#define SHCM_NO_INITIALIZATION           0x00000008L
#define SHCM_RETURN_HMENU                0x00000010L
#define SHCM_COMPANION                   0x00000020L
#define SHCM_NO_INFODESK                 0x00000040L
//#define SHCM_                          0x00000080L


/*--------------------------------------------------
  Registered message for querying for subitems
  LPARAM contains an LPPOINT defining the cursor
  position.  The WPARAM is current unused
---------------------------------------------------*/
#define SHRMSG_SUBITEM "Symantec:StdHelp:SubItem"


#ifndef IDHELP
#define IDHELP 19999
#endif

/*--------------------------------------------------
  RSTANEV:  Added to avoid conflict with STRIDS.H
---------------------------------------------------*/

#ifdef STD_HELP_INDEX
#undef STD_HELP_INDEX
#endif

#ifdef STD_HELP_KEYBOARD
#undef STD_HELP_KEYBOARD
#endif

#ifdef STD_HELP_COMMANDS
#undef STD_HELP_COMMANDS
#endif

#ifdef STD_HELP_PROCEDURES
#undef STD_HELP_PROCEDURES
#endif

#ifdef STD_HELP_INTRODUCING
#undef STD_HELP_INTRODUCING
#endif

#ifdef STD_HELP_USINGHELP
#undef STD_HELP_USINGHELP
#endif

#ifdef STD_HELP_ABOUT
#undef STD_HELP_ABOUT
#endif

#ifdef STD_HELP_POPUP_FORMAT
#undef STD_HELP_POPUP_FORMAT
#endif

#ifdef STD_HELP_SEARCH
#undef STD_HELP_SEARCH
#endif

#ifdef STD_HELP_HOW_TO_USE
#undef STD_HELP_HOW_TO_USE
#endif

#ifdef STD_HELP_CONTENTS
#undef STD_HELP_CONTENTS
#endif

#ifdef STD_HELP_QUICKHELP
#undef STD_HELP_QUICKHELP
#endif

#ifdef STD_HELP_TUTORIAL
#undef STD_HELP_TUTORIAL
#endif

#ifdef STD_HELP_README
#undef STD_HELP_README
#endif

#ifdef STD_HELP_ERR_FILE
#undef STD_HELP_ERR_FILE
#endif

#ifdef STD_HELP_ERR_TOPIC
#undef STD_HELP_ERR_TOPIC
#endif

#ifdef STD_HELP_ERR_DIALOG
#undef STD_HELP_ERR_DIALOG
#endif

/*--------------------------------------------------
  YVANG:  Added to avoid redefinition warning message
---------------------------------------------------*/
#ifdef STD_POPUP_HELP_INFODESK
#undef STD_POPUP_HELP_INFODESK
#endif

#ifdef STD_POPUP_HELP_TERMFINDER
#undef STD_POPUP_HELP_TERMFINDER
#endif



// Duplicated in sihelps.h for resources.
#define STD_HELP_INDEX            15360 /* ERR_NUM( 60,   0) */
#define STD_HELP_KEYBOARD         15361 /* ERR_NUM( 60,   1) */
#define STD_HELP_COMMANDS         15362 /* ERR_NUM( 60,   2) */
#define STD_HELP_PROCEDURES       15363 /* ERR_NUM( 60,   3) */
#define STD_HELP_INTRODUCING      15364 /* ERR_NUM( 60,   4) */
#define STD_HELP_USINGHELP        15365 /* ERR_NUM( 60,   5) */
#define STD_HELP_ABOUT            15366 /* ERR_NUM( 60,   6) */
#define STD_HELP_POPUP_FORMAT     15367 /* ERR_NUM( 60,   7) */
#define STD_HELP_SEARCH            15368  /* ERR_NUM( 60,   8) */
#define STD_HELP_HOW_TO_USE        15369  /* ERR_NUM ( 60, 9)  */
#define STD_HELP_CONTENTS          15370  /* ERR_NUM ( 60, 10) */
#define STD_HELP_QUICKHELP         15371  /* ERR_NUM (60 , 11) */
#define STD_HELP_TUTORIAL          15372  /* ERR_NUM (60,  12) */
#define STD_HELP_README            15373  /* ERR_NUM (60,  13) */
#define STD_HELP_INFODESK          15374  /* ERR_NUM (60,  14) */
#define STD_HELP_ERR_FILE         15388 /* ERR_NUM( 60,  28) */
#define STD_HELP_ERR_TOPIC        15389 /* ERR_NUM( 60,  29) */
#define STD_HELP_ERR_DIALOG       15390 /* ERR_NUM( 60,  30) */
#define STD_HELP_BTN_SET          15391 /* ERR_NUM( 60,  31) */
#define STD_HELP_BTN_EDIT         15392 /* ERR_NUM( 60,  32) */
#define STD_HELP_BTN_TIP          15393 /* ERR_NUM( 60,  33) */
#define STD_HELP_BTN_LIST         15394 /* ERR_NUM( 60,  34) */
#define STD_HELP_BTN_CLOSE        15395 /* ERR_NUM( 60,  35) */

#define STD_POPUP_HELP_WHATSTHIS  15396
#define STD_POPUP_HELP_INDEX      15397   // defined by NAV
#define STD_POPUP_HELP_HOWTO      15398
#define STD_POPUP_HELP_CONTENTS   15399   // defined by NAV
#define STD_POPUP_HELP_TASK       15400
#define STD_POPUP_HELP_INFODESK   15401
#define STD_POPUP_HELP_TERMFINDER 15402   // defined by NAV (used be 15401)

/************************************************************************/
/* Exported function warnings/errors                                    */
/************************************************************************/
#define STD_HELP_NO_FILE        ERR_SEV_NUM (ERR_SEV_WARNING, \
                                             STD_HELP_ERR_FILE)
#define STD_HELP_NO_TOPIC       ERR_SEV_NUM (ERR_SEV_WARNING, \
                                             STD_HELP_ERR_TOPIC)
#define STD_HELP_NO_DIALOG      ERR_SEV_NUM (ERR_SEV_WARNING, \
                                             STD_HELP_ERR_DIALOG)

#endif
