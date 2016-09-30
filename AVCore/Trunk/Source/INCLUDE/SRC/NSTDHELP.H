/////////////////////////////////////////////////////////////////////////
//
// STDHELP.H
//
/////////////////////////////////////////////////////////////////////////
//
// $Log:   S:\include\vcs\nstdhelp.h_v  $
// 
//    Rev 1.6   09 Jan 1996 12:38:42   JWORDEN
// 
//    Rev 1.5.1.0   12 Jul 1995 18:55:18   JREARDON
// Branch base for version QAK8
// 
//    Rev 1.5   15 Jun 1995 16:28:32   GWESTER
// Made a change in the code I just checked in so that
// VxD version can compile.
// 
//    Rev 1.4   15 Jun 1995 15:38:42   GWESTER
// Added Quake's StdHelpInfoDesk(), StdHelpWhatsThisEx() in
// Navw form.
// 
//    Rev 1.3   03 May 1995 13:14:34   MFALLEN
// Macro for StdHelpOnContextHelp
// 
//    Rev 1.2   03 May 1995 12:09:46   MFALLEN
// 
// 
//    Rev 1.1   03 May 1995 11:55:24   MFALLEN
// Added macros.
// 
//    Rev 1.0   03 May 1995 10:23:06   MFALLEN
// Initial revision.
// 
//    Rev 1.0   03 May 1995 10:09:48   MFALLEN
// Initial revision.
//
/////////////////////////////////////////////////////////////////////////

#ifndef _STDHELP_H
#define _STDHELP_H

#define StdHelpInit(i) NavwHelpInit(i)
#define StdHelpGetHelpInfo(l) NavwHelpGetHelpInfo(l)
#define StdHelpActivate(h,i) NavwHelpActivate(h,i)
#define StdHelpFromID(i,id) NavwHelpFromID(i,id)
#define StdHelpFromDialog(t,s) NavwHelpFromDialog(t,s)
#define StdHelpSetContext(t,s) NavwHelpSetContext(t,s)
#define StdHelpSetHelpFileID(t,s) NavwHelpSetHelpFileID(t,s)
#define StdHelpFromMenu(t,s,i,l) NavwHelpFromMenu(t,s,i,l)
#define StdHelpGetHelpFileID(h) NavwHelpGetHelpFileID(h)
#define StdHelpFromHelp(t,s) NavwHelpFromHelp(t,s)
#define StdHelpFromHelpBtn(h,sz) NavwHelpFromHelpBtn(h,sz)
#define StdHelpShutdown(t) NavwHelpShutdown(t)
#define StdPopupHelpFromDialog(h,l,us,dw1,dw2) NavwPopupHelpFromDialog(h,l,us,dw1,dw2)
#define StdHelpContextMenu(i) NavwHelpContextMenu(i)
#define StdHelpProcessContextMenu(i,n,b) NavwHelpProcessContextMenu(i,n,b)
#define StdHelpWhatsThis(i) NavwHelpWhatsThis(i)
#define StdHelpWithParams(h,l,p,w,x) NavwHelpWithParams(h,l,p,w,x)
#define StdHelpOnContextHelp(a,b,c,d,e,f,g,h) NavwHelpOnContextHelp(a,b,c,d,e,f,g,h)
#define StdHelpInfoDesk(i) NavwHelpInfoDesk(i)  
#define StdHelpWhatsThisEx(i,j) NavwHelpWhatsThisEx(i,j)

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

@See: HELPTABLE NavwHelpInit NavwHelpActivate
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
    HINSTANCE    hInstance;                      /* If SHCM_NO_INITIALIZATION, then application instance */
    char const * szHelpFile;                  /* If SHCM_NO_INITIALIZATION, then name of help file */
    HMENU       hMenu;                        /* If SHCM_RETURN_HMENU hmenu returned instead of tracking */
    int         iWhatsThisHelpID;             /* Set to current what's this item */
    } CONTEXTMENUSTRUCT;

typedef CONTEXTMENUSTRUCT  NEAR  *NPCONTEXTMENUSTRUCT;
typedef CONTEXTMENUSTRUCT  FAR   *LPCONTEXTMENUSTRUCT;
/*****
@Name: CONTEXTMENUSTRUCT

@Description:
This structure is used to call the NavwHelpContextMenu function.

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

@See: HELPTABLE NavwHelpContextMenu NavwHelpInit NavwHelpActivate
@Include: stdhelp.h
******************************************************************************/
#ifdef __cplusplus
extern "C" {
#endif

WORD SYM_EXPORT WINAPI
NavwHelpInit(LPHELPSTRUCT i);

VOID SYM_EXPORT WINAPI 
NavwHelpGetHelpInfo( LPHELPSTRUCT lpHelpInf );

WORD SYM_EXPORT WINAPI
NavwHelpActivate(HWND h, LPHELPSTRUCT i);

WORD SYM_EXPORT WINAPI
NavwHelpFromID(LPHELPSTRUCT i, UINT id);

WORD SYM_EXPORT WINAPI
NavwHelpFromDialog(HWND t, UINT s);

WORD SYM_EXPORT WINAPI
NavwHelpSetContext(UINT t, UINT s);

UINT SYM_EXPORT WINAPI
NavwHelpSetHelpFileID(HWND t, UINT s);

WORD SYM_EXPORT WINAPI
NavwHelpFromMenu(HWND t, UINT s, UINT i, long l);

UINT SYM_EXPORT WINAPI NavwHelpGetHelpFileID (HWND hWnd);

WORD SYM_EXPORT WINAPI
NavwHelpFromHelp(HWND t, UINT s);

#if defined( SYM_WIN32 ) || defined( _WIN32 )
WORD SYM_EXPORT WINAPI
NavwHelpFromHelpBtn(HWND hDlg, LPCSTR szKeyword);

typedef int (FAR *LB_ITEMSELECTIONPROC)(HWND hwndListBoxKindOfCtrl);
#define IDH_UNKNOWN 32760

VOID SYM_EXPORT WINAPI NavwHelpOnContextHelp
(                  
    HWND    hDlg,                       
    UINT    uMsg,    
    WPARAM  wParam,                     
    LPARAM  lParam,                     
    DWORD  *pdwarrWhatsThisMap,         
    DWORD  *pdwarrHowToMap,             
    DWORD  *pdwarrLBItemsHelpStrings,   
    LB_ITEMSELECTIONPROC lpfnGetSelItem
);

#endif

WORD SYM_EXPORT WINAPI
NavwHelpShutdown(HWND t);

WORD SYM_EXPORT WINAPI
NavwPopupHelpFromDialog (HWND hDlg, LONG lParam, UINT uiFlags,
                        DWORD *dwWhatsThisMap, DWORD *dwHowToMap);

BOOL SYM_EXPORT WINAPI
NavwHelpContextMenu (LPCONTEXTMENUSTRUCT i);

BOOL SYM_EXPORT WINAPI
NavwHelpProcessContextMenu (LPCONTEXTMENUSTRUCT i, int iMenuItem, BOOL bSendCommand);

BOOL SYM_EXPORT WINAPI
NavwHelpWhatsThis ( LPCONTEXTMENUSTRUCT i );

WORD SYM_EXPORT WINAPI NavwHelpWithParams(
    HWND        hDlg,                   // Handle of dialog box
    LPSTR       lpszHelpFile,           // Helpfile name, (fully qualified)
    LPHELPTABLE lpHt,                   // Help table
    WORD        wIDD,                   // Unique IDD_* for dialog box
    WORD        wHelp);                 // Help item ID (if known)

#if defined( SYM_WIN32 ) 
BOOL SYM_EXPORT WINAPI
NavwHelpWhatsThisEx ( LPCONTEXTMENUSTRUCT i, LPHELPINFO j );
#endif

BOOL SYM_EXPORT WINAPI
NavwHelpInfoDesk ( LPCONTEXTMENUSTRUCT i );

#if !defined( SYM_WIN32 ) && !defined( _WIN32 )
// Sets up the debugging mode so calls to Custom Help Macro buttons
// can be debugged by WinHelp author.
void SYM_EXPORT WINAPI SHSetDebug ( HWND hwndApp );

// Removes all buttons from a secondary WinHelp window.
// Sets up the context info required for NavwHelpCreateBttn.
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

#ifdef __cplusplus
}
#endif


/*--------------------------------------------------
  FLAGS for StdPopupHelpFromDlg
---------------------------------------------------*/
#define SPHFD_SINGLE_WHATS_THIS        1
#define SPHFD_MAPPED_WHATS_THIS        2

/*--------------------------------------------------
  FLAGS for NavwHelpContextMenu
---------------------------------------------------*/
#define SHCM_NO_FLAGS                    0x00000000L
#define SHCM_SINGLE_WHATS_THIS           0x00000001L
#define SHCM_HELP_IN_SUBMENU             0x00000002L
#define SHCM_1ST_WHATS_THIS_IS_DEFAULT   0x00000004L
#define SHCM_NO_INITIALIZATION           0x00000008L
#define SHCM_RETURN_HMENU                0x00000010L
//#define SHCM_                          0x00000020L
//#define SHCM_                          0x00000040L
//#define SHCM_                          0x00000080L


/*--------------------------------------------------
  Registered message for querying for subitems
  LPARAM contains an LPPOINT defining the cursor
  position.  The WPARAM is current unused
---------------------------------------------------*/
#define SHRMSG_SUBITEM "Norton:NavwHelp:SubItem"


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
#define STD_POPUP_HELP_INDEX      15397
#define STD_POPUP_HELP_HOWTO      15398
#define STD_POPUP_HELP_CONTENTS   15399
#define STD_POPUP_HELP_TASK       15400
#define STD_POPUP_HELP_TERMFINDER 15401
#define STD_POPUP_HELP_INFODESK   15402

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










