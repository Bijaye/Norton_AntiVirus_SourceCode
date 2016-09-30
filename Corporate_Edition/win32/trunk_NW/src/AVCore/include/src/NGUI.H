//========================================================================
// Copyright 1992 - 2003 Symantec Corporation. All rights reserved.
//------------------------------------------------------------------------
//
// NGUI.H -- Symantec Look Custom Control include file
//            (macro covers to CPALETTE routines)
//
// $Header:   S:/INCLUDE/VCS/ngui.h_v   1.21   18 Feb 1998 12:02:50   mdunn  $
//
//------------------------------------------------------------------------
// $Log:   S:/INCLUDE/VCS/ngui.h_v  $
// 
//    Rev 1.21   18 Feb 1998 12:02:50   mdunn
// Ported ROSWELL and ROSNEC changes from K branch
// 
//    Rev 1.20   16 Oct 1996 16:38:30   HOKEKE
// Changed and moved function prototypes NG_GetDBCSCharSet, NG_GetDBCSFaceName,
// and NG_GetDBCSFontSize from ngfont.cpp.
// 
//    Rev 1.19   08 Oct 1996 17:20:52   MREYNOL
// FAR/NEAR problem with 16-bit parameter pointer.
// 
//    Rev 1.18   08 Oct 1996 16:00:12   MREYNOL
// Added NG_CreateFontIndirect: acts exactly like CreateFontIndirect
// (which takes a LOGFONT struct parameter) except that it changes the
// three LOGFONT struct elements that have to be altered for DBCS font
// if it is running under a DBCS system.
// 
//    Rev 1.17   04 Sep 1996 21:44:02   MRUSSEL
// Added bRepaint=TRUE to original Table_VwrTblDeleteItem().
// 
//    Rev 1.16   04 Sep 1996 21:04:48   MRUSSEL
// Added Table_VwrTblDeleteItemEx().
// 
// 
//    Rev 1.15   15 Aug 1996 13:50:18   MRUSSEL
// Added bRepaint to Table_VwrTblAddNefOid.
// 
//    Rev 1.14   15 Aug 1996 12:55:22   MRUSSEL
// Added Table_VwrTblAddNefOid().
// 
// 
//    Rev 1.13   05 Aug 1996 18:18:24   SEDWARD
// Added NG_SetFontInControl
//
//    Rev 1.12   10 Jul 1996 17:10:06   MRUSSEL
// Added Status set/clear messages.
//
//    Rev 1.11   17 Jun 1996 12:36:28   RANSCHU
// Define EXTERN_C if not already defined.
//
//    Rev 1.10   17 Jun 1996 12:09:28   RANSCHU
// Added prototype for GradientSubclassWnd() for Win32 platform.
//
//    Rev 1.9   09 May 1996 22:52:14   MRUSSEL
// Expose fieldview override in the table control.
//
//    Rev 1.8   22 Apr 1996 23:01:30   MRUSSEL
// Fixed a trailing space in a macro.
//
//    Rev 1.7   22 Apr 1996 16:14:50   MRUSSEL
// Added tbl control macro to set the Notify callback.
//
//    Rev 1.6   12 Apr 1996 09:30:48   MRUSSEL
// Exposed a ton of dialog related stuff for the table control.
//
//    Rev 1.5   23 Mar 1996 17:58:28   MRUSSEL
// Fixed typo.
//
//    Rev 1.4   23 Mar 1996 15:12:36   MRUSSEL
// IsTblChecked now returns a BYTE.
//
//    Rev 1.3   23 Mar 1996 10:32:48   MRUSSEL
// Added messages for Checkbox support.
//
//    Rev 1.2   16 Mar 1996 12:57:08   MRUSSEL
// Fixed return type on one of the new macros.
//
//    Rev 1.1   16 Mar 1996 12:27:08   MRUSSEL
// Added support for disjoint selection in the table control.
//
//    Rev 1.0   26 Jan 1996 20:15:40   RJACKSO
// Initial revision.
//
//    Rev 1.47   02 Jan 1996 12:23:38   JHONG
// Changed NGVwrTblSendDBSpec and NGVwrTblSendDBSpecEx prototype
// to use NGSETSTATUSTRAY instead of NGSETSTATUSTEXT
//
//    Rev 1.46   16 Nov 1995 17:07:22   MLELE
// Brought in QAK6 changes: log 1.30.1.2, DSACKIN. "Changed NGUI.H to
// include VLIST class.
//
//    Rev 1.45   03 Nov 1995 12:46:20   DBUCHES
// Added SymTray_InitEx.
//
//    Rev 1.44   01 Aug 1995 22:51:02   AWELCH
// Moved Quake 7 changes to trunk.
//
//    Rev 1.43   26 Apr 1995 13:24:10   BRAD
// Added NG_SetGroupBoxSunken()
//
//    Rev 1.42.1.6   05 Jun 1995 15:27:02   MICHAELR
// Added msg to retrieve the width of a column in the table control.
//
//    Rev 1.42.1.5   29 May 1995 09:38:08   MICHAELR
// Added NTM_VWRTBL_DELETE_ITEMS.
//
//    Rev 1.42.1.4   24 May 1995 16:06:38   JMCILWAI
// Typo with Table_VwrTblIsInitialized
//
//    Rev 1.42.1.3   24 May 1995 11:20:12   MICHAELR
// Added table control msgs to delete/update a specific rec; for checking
// the control to see if it's ready for access.
//
//    Rev 1.42.1.2   08 May 1995 11:37:34   MICHAELR
// Added NTM_VWRTBL_NO_FIELDVIEW.
//
//    Rev 1.42.1.1   04 May 1995 13:37:38   TORSTEN
// Fixed the enum statement which defines the NTM_* message numbers -- the
// enumeration sequence was being restarted at one point, and the message
// numbers started overlapping.
//
//    Rev 1.42.1.0   24 Apr 1995 20:45:02   SCOTTP
// BRANCH BASE VERSION FOR QAK7
//
//    Rev 1.42   04 Apr 1995 17:48:36   MICHAELR
// Added NTM_VWRTBL_FILESPEC.
//
//    Rev 1.41   18 Mar 1995 09:52:52   MICHAELR
// Tweaked Table_IsSymvwrTbl() to check class name first before sending
// message.
//
//    Rev 1.40   11 Mar 1995 07:58:28   MICHAELR
// Added status tray support to VwrTbl control.
//
//    Rev 1.39   09 Mar 1995 15:17:18   DALLEE
// Use angle brackets for <commctrl.h> - Comes straight from SDK.
// Using "" causes MKDEPS to pick it up as a dependency.
//
//    Rev 1.38   09 Mar 1995 13:34:42   BILL
// Added autodestroybitmaps flag to about/guilt screen
//
//    Rev 1.37   05 Feb 1995 22:44:38   MICHAELR
// Added SelectAll msg for Table Control.
//
//    Rev 1.36   03 Feb 1995 16:58:26   TORSTEN
// Fixed a typo.
//
//    Rev 1.35   03 Feb 1995 14:49:36   TORSTEN
// Added Slider style bitflags.
//
//    Rev 1.34   27 Jan 1995 18:04:42   MICHAELR
// Removed redundant table control message.
//
//    Rev 1.33   27 Jan 1995 14:41:04   MICHAELR
// Added table control messages for the Sort dialog and clpbrd copy.
//
//    Rev 1.32   23 Jan 1995 21:47:12   MICHAELR
// Fixed some errant spaces in the Table control msg cracker macros.
//
//    Rev 1.31   20 Jan 1995 14:51:26   DMITRY
// Added Table_VWR_FLDDlg, Table_VWR_QRYDLG and Table_VWR_GetLOGIC funtion definitions
//
//    Rev 1.30   10 Jan 1995 19:20:36   MFALLEN
// If building for W32 platform use standard W95 progress bar.
//
//    Rev 1.29   22 Dec 1994 16:20:46   DMITRY
// Added NGVwrTblSendDbSpecEx()
//
//    Rev 1.28   16 Dec 1994 18:32:52   BILL
// Added nTimer field to splash screen (seconds to autodestruction)
//
//    Rev 1.27   12 Dec 1994 14:07:30   BILL
// Added BBS_TOOLTIPS to enable automatic tooltip handling for win32
//
//    Rev 1.26   25 Nov 1994 09:05:04   MICHAELR
// Added NTM_VWRTBL_SET_CURROW, NTM_VWRTBL_SETSEL.
//
//    Rev 1.25   22 Nov 1994 21:07:24   BILL
// Added bitmap field to about box struct
//
//    Rev 1.24   11 Nov 1994 16:02:18   MICHAELR
// Added NGTableBroadcastMsg().
//
//    Rev 1.23   10 Nov 1994 21:02:52   MICHAELR
// Added Get/Set stle messages for VwrTbl.
//
//    Rev 1.22   06 Nov 1994 05:58:02   MICHAELR
// Added an NGSETSTATUSTEXT callback to NGVwrTblSendDBSpec().
//
//    Rev 1.21   30 Oct 1994 23:04:52   MICHAELR
// Fixed return value for NGVwrTblSendDBSpec().
//
//    Rev 1.20   30 Oct 1994 16:43:42   MICHAELR
// Added NGVwrTblSendDBSpec().
//
//    Rev 1.19   19 Oct 1994 13:22:12   bill
//
//    Rev 1.18   16 Oct 1994 11:16:16   MICHAELR
// Added Table_VwrTbl... messages to get info about curr db index and
// selection state of the Symvwr table control.
//
//    Rev 1.17   09 Oct 1994 08:48:38   MICHAELR
// Added Macro versions of the VwrTbl inlines to accommodate straight "C".
//
//    Rev 1.16   05 Oct 1994 21:34:52   MICHAELR
// Added Table_VwrTblGetCurrRow/Col().
//
//    Rev 1.15   02 Oct 1994 06:52:06   MICHAELR
// Added msg NTM_VWRTBL_RELOAD_FILE.
//
//    Rev 1.14   29 Sep 1994 13:51:46   MICHAELR
// Added Table_VwrTblGetDbHandle() inline func.
//
//    Rev 1.13   29 Sep 1994 13:12:04   MICHAELR
// Add registered message to allow the retrieval of the db handle from
// the SymVwr table control.
//
//    Rev 1.12   20 Sep 1994 09:28:22   MICHAELR
// 1. Now using 32 bit pointers for the button and column data to allow
//    support for the Symvwr style table control.
// 2. Added support for a Symvwr viewer control that can optionally replace
//    the original listbox for cases where the anticipated volume of data
//    exceeds the capacity of the listbox.
//
//    Rev 1.11   06 Sep 1994 12:25:14   ROWANT
// Added button bar messages: BB_SETNORMALBMPMASK and BB_SETSELECTEDBMPMASK to
// fully support changing button bitmaps dynamically.
//
//    Rev 1.10   30 Aug 1994 08:51:04   DLEVITON
// Play button option on tunnel button.
//
//    Rev 1.9   22 Jun 1994 06:51:48   DLEVITON
// Remove obsolete constants, some of which conflict with winuser.h in Chicago.
//
//    Rev 1.8   20 Jun 1994 14:23:04   DLEVITON
// Ifdef out extended SS_ styles under WIN32.
//
//    Rev 1.7   26 May 1994 23:19:50   BRUCE
// [ENH] Added NTM_SORTCHANGE notification
//
//    Rev 1.6   25 May 1994 15:14:12   GVOGEL
// Changed @Library to @Include in comment blocks for Geologist project.
//
//    Rev 1.5   16 May 1994 04:50:06   GVOGEL
// [Fix] Replaced #defines accidentally removed in previous chkin.
//
//    Rev 1.3   26 Apr 1994 16:40:52   BRAD
// Removed PVCS Logs
//========================================================================

/*@Library: SYMGUI*/

#ifndef _NGUI_H
#define _NGUI_H

#ifdef SYM_WIN32
    #include <commctrl.h>               // Win95 common controls.
#endif

#if defined(SYM_WIN)                    // Only used for SYM_WIN

#include "version.h"
#include "wndclass.h"
#include "passwd.h"
#include "stockbmp.h"


#ifdef __cplusplus
extern "C" {
#endif

#define SYMGUI_VERSION_MAJOR   QUAKE_VERSION_MAJOR
#define SYMGUI_VERSION_MINOR   QUAKE_VERSION_MINOR
#define SYMGUI_BUILD_NUMBER    QUAKE_BUILD_NUMBER

HINSTANCE SYM_EXPORT WINAPI SymGuiGetInstance(void);

// -------------------------------------------------------------------------
// Generic Types for SYMGUI.  Feel free to add any types to the list, but
// ONLY to the end.
// -------------------------------------------------------------------------

typedef enum SYMGUITYPE
    {
    FI_FILTER_NUMERIC = 1,              // edit filter (0-9)
    FI_FILTER_HEXNUMERIC,               // edit filter (0-9, A-F)
    FI_FILTER_ALPHA,                    // edit filter (A-Z)
    FI_FILTER_ALPHANUMERIC,             // edit filter (0-9, A-Z)
    FI_FILTER_FILENAME,                 // edit filter (legal filename chars)
    FI_FILTER_NOWHITE,                  // edit filter (no whitespace)
    } SYMGUITYPE;


// -------------------------------------------------------------------------
// Generic text structure useful for GetText type message when wParam is
// used by something else.
// -------------------------------------------------------------------------

typedef struct NGTEXTREC
    {
    UINT        wSize;
    LPSTR       lpString;
    } NGTEXTREC, FAR *LPNGTEXTREC;

//------------------------------------------------------------------------
// Define "SymantecTableClass" custom control messages, structures and Misc.
//------------------------------------------------------------------------

                    // Define miscellaneous
#define NGTABLE_CURSOR    251

#define NTC_LISTBOX       0x1000        // control ID of the list box

#define NCS_TITLES        0x0001L       // Show titles for columns
#define NCS_SIZEABLE      0x0002L       // Allow sizeable columns
#define NCS_MOVEABLE      0x0004L       // Allow moveable columns
#define NCS_BUTTONS       0x0008L       // Allow buttons on the main window
#define NCS_SORT          0x0010L       // Listbox gets LBS_SORT style
#define NCS_MULTIPLESEL   0x0020L       // Listbox gets LBS_MULTIPLESEL style
#define NCS_NOTIFY        0x0040L       // Listbox gets LBS_NOTIFY style
#define NCS_EXTENDEDSEL   0x0080L       // Listbox gets LBS_EXTENDEDSEL style
#define NCS_SPEEDSEARCH   0x0100L       // Speed search on sorted column.
#define NCS_NOLISTBOX     0x0200L       // Disallows creation of listbox (table)
#define NCS_SINGLECLICK   0x0400L       // Enables single click sorting
#define NCS_SYMVWRTBL     0x0800L       // Uses a Symvwr control instead
                                        //          of a standard listbox

enum {
    // Table Input Messages
    // --------------------------------------------------------------
    NTM_SETCOLUMNS = WM_USER + 501, // Setup the column format
    NTM_ADDRECORD,                  // Adds a record to the table
    NTM_SETTITLEFONT,               // Changes the column title font
    NTM_SETDATAFONT,                // Changes the listbox data font
    NTM_SETBUTTONS,                 // Setup the buttons
    NTM_SETBUTTONFONT,              // Sets the button font
    NTM_ENABLEBUTTON,               // Enables or disables buttons
    NTM_SETSORTCOLUMN,              // Sets the sort column
    NTM_GETCOLUMNS,                 // Get the column format
    NTM_AUTOSIZECOLUMNS,            // Automatically sizes columns for largest field
    NTM_GETFILLRECORD,              // Performs fillrecord on the index specified
    NTM_GETTITLEFONT,               // Retrieves the column title font
    NTM_GETDATAFONT,                // Retrieves the listbox data font
    NTM_FINDSTRING,                 // Results from an LB_FINDSTRING message
    NTM_SORTOFF,                    // Turns off the sort
    NTM_SORTON,                     // Turns on the sort
    NTM_SORT,                       // Re-sorts the table control based on the current sort column
    NTM_GETSORTCOLUMN,              // Retrieves the sorted column number
    NTM_GETLISTBOXHWND,             // Retrieves the listbox hwnd of the table control
    NTM_SETSORTORDER,               // Sets the current sorting order
    NTM_GETSORTORDER,               // Retrieves the current sorting order
    NTM_SHOWSORTDIALOG,             // Displays the sort dialog box
    NTM_NOSELECT,                   // Disables selection in the table
    NTM_GETINDEXFROMID,             // Retrieves the column index from an i.d.
    NTM_GETIDFROMINDEX,             // Retrieves the column id from an index
// new for Quake rev 0
    NTM_SHOWBUTTON,                 // Shows or hides a button

    // Table Notification Messages
    // --------------------------------------------------------------
    NTM_DELETEITEM = WM_USER + 540, // Sent for each listbox WM_DELETEITEM
    NTM_FILLRECORD,                 // Owner must fill in the record
    NTM_SORTCHANGE,                 // Notify owner of change in sort column

// new for Quake rev 6
    NTM_IS_SYMVWR_TBL = WM_USER + 550,// Is this a Symvwr table control?
    NTM_VWRTBL_FILE_SPEC,           // Send a file spec to the viewer hWnd
    NTM_VWRTBL_GET_DBHANDLE,        // Get the db handle of displayed file
    NTM_VWRTBL_RELOAD_FILE,         // Reload/refresh the file
    NTM_VWRTBL_GET_CURROW,          // Get the currently selected row
    NTM_VWRTBL_GET_CURCOL,          // Get the currently selected col
    NTM_VWRTBL_GET_COUNT,           // Get num recs in the current view
    NTM_VWRTBL_GET_DB_TYPE,         // See NTC_DBTYPE_...
    NTM_VWRTBL_GET_DB_CURR_INDEX,   // Get the db index of current view
    NTM_VWRTBL_GET_SEL_STARTROW,    // Selection range info
    NTM_VWRTBL_GET_SEL_STARTCOL,    // Selection range info
    NTM_VWRTBL_GET_SEL_ENDROW,      // Selection range info
    NTM_VWRTBL_GET_SEL_ENDCOL,      // Selection range info
    NTM_VWRTBL_GET_STYLE,           // Get the style dword
    NTM_VWRTBL_SET_STYLE,           // Set the style dword
    NTM_VWRTBL_SET_CURROW,          // Set the current row
    NTM_VWRTBL_SETSEL,              // Set the row\col (row < 64k)
    NTM_VWRTBL_FLDDLG,              // Popup Field Dialog
    NTM_VWRTBL_QRYDLG,              // Popup Query Dialog
    NTM_VWRTBL_GETLOGIC,            // Get back logic assosiated with window
    NTM_VWRTBL_EDITCOPY,            // Copy current selection to clpbrd
    NTM_VWRTBL_SELECTALL,           // Select the entire thing
    NTM_VWRTBL_FILESPEC,            // Send a non-nefdb filespec to the viewer
    NTM_VWRTBL_NO_FIELDVIEW,        // Suppress the field view option
    NTM_VWRTBL_DELETE_ITEM,         // Delete a record
    NTM_VWRTBL_REFRESH_ITEM,        // Force a re-read from the db for a rec
    NTM_VWRTBL_IS_INITIALISED,      // Is it ready to be accessed?
    NTM_VWRTBL_DELETE_ITEMS,        // Delete a block of records
    NTM_VWRTBL_GET_COLWIDTH,        // Get col width
    NTM_VWRTBL_GET_TOT_SELECTED,    // Get total sel recs (supports disjoint)
    NTM_VWRTBL_IS_SELECTED,         // Is rec selected?   (supports disjoint)
    NTM_VWRTBL_GET_SELECTION_ARRAY, // Get the selection state map
    NTM_VWRTBL_IS_CHECKED,          // Is rec checked?
    NTM_VWRTBL_GET_CHECKBOX_ARRAY,  // Get the checkbox state map
    NTM_VWRTBL_FIND_DLG,            // Find
    NTM_VWRTBL_FIND_NEXT,           // Find Next
    NTM_VWRTBL_FIND_PREV,           // Find Prev
    NTM_VWRTBL_GOTO_DLG,            // Goto
    NTM_VWRTBL_SORT_DLG,            // Sort
    NTM_VWRTBL_STYLE_DLG,           // Style
    NTM_VWRTBL_FONT_DLG,            // Font
    NTM_VWRTBL_SET_NOTIFYCB,        // Client notification callback
    NTM_VWRTBL_FIELDVIEW_OVERRIDE,  // Fieldview override
    NTM_VWRTBL_STATUS_SET,          // Status bar set
    NTM_VWRTBL_STATUS_CLEAR,        // Status bar clear
    NTM_VWRTBL_ADD_NEFOID,          // Add a Nefoid to the table control
    };

/*@STRUCTAPI:***************************************************************/
typedef struct
    {
    DWORD       dwSeqNo;
    DWORD       dwNumItems;
    } NTDELITEMS, FAR * LPNTDELITEMS;
/*****
@Name: NTDELITEMS

@Description:
This structure contains information needed to delete a block of records
from the Symvwr table control. It is passed as the lParam in the
NTM_VWRTBL_DELETE_ITEMS message.

@Members:
$dwSeqNo$ Starting 0 based sequence number
$dwNumItems$ Number of items/records to remove

@Include: ngui.h
****************************************************************************/
/*@STRUCTAPI:***************************************************************/
typedef struct
    {
    LPARAM   lParam;
    LPVOID   lpNSCommCB;
    } NTNOTIFYCB, FAR * LPNTNOTIFYCB;
/*****
@Name: NTNOTIFYCB

@Description:
This structure contains information needed to set up an NSCOMMCB callback
as defined in NefScal.h in the Nef global include area. It is passed as the
lParam in the NTM_VWRTBL_SET_NOTIFYCB message.

@Members:
$lParam$ Passed directly back to the callback
$lpNSCommCB$ Assumed to be NSCOMMCB, as defined in NEF\NefScal.h

@Include: ngui.h
****************************************************************************/
                    // Define property/atom strings
#define NT_TABLE_CLASSNAME      QCLASS_NT_TABLE
#define NT_PROP_COLUMNS         "NGUI:NT_ColumnData"
#define NT_PROP_TABLEDATA       "NGUI:NT_TableData"
#define NT_PROP_VWRTBLDATA      "NGUI:NT_VwrTbl"

                    // Define reg msg strings
#define NT_MSG_VWRDBHANDLE      "NGUI:VwrTblDbHandle"

                    // Define NT_COLUMN Flags
#define NTC_FLAG_TITLE_CENTER   0x0001
#define NTC_FLAG_TITLE_LEFT     0x0002
#define NTC_FLAG_TITLE_RIGHT    0x0004
#define NTC_FLAG_HIDDEN         0x0008  // Column is hidden
#define NTC_FLAG_NOSORT         0x0010  // column can't be used to sort


#define NT_MAX_FIELD                    256    // max text length of a field entry
#define NT_COLUMN_TITLE                 64
#define NT_BUTTON_TITLE                 64
#define NT_TITLE_HEIGHT                 20
#define NT_BUTTON_HEIGHT                30
#define NT_MIN_LB_HEIGHT                30
#define NT_SIZERANGE                    5      // +- range of cursor detection when sizing
#define NT_BORDER_SIZE                  0
#define NT_UPDATE_RANGE                 15     // +- range of update region
#define NT_BUTTON_TEXT_X                45
#define NT_BUTTON_TEXT_Y                20
#define NT_BUTTON_BITMAP_X              24
#define NT_BUTTON_BITMAP_Y              24
#define NT_BUTTON_TEXTSMALLBITMAP_X     45
#define NT_BUTTON_TEXTSMALLBITMAP_Y     45
#define NT_BUTTON_TEXTLARGEBITMAP_X     65
#define NT_BUTTON_TEXTLARGEBITMAP_Y     63

                    // Define INTERNAL Table I.D.s
enum {
    NTID_LISTBOX_HWND = 1,
    NTID_NUMCOLUMNS,
    NTID_TITLEFONT,
    NTID_DATAFONT,
    NTID_RECORDBUFFER,
    NTID_NUMBUTTONS,
    NTID_BUTTONBUFFER,
    NTID_BUTTONFONT,
    NTID_SORTCOLUMNID,
    NTID_SORTORDER,
    NTID_NOSELECT
     };


/*@STRUCTAPI:***************************************************************/
typedef struct NT_FINDSTRING
    {
    DWORD       itemData;
    DWORD       listData;
    int         listIndex;
    } NT_FINDSTRING, FAR *LPNT_FINDSTRING;
/*****
@Name: NT_FINDSTRING

@Description:
[[To be completed]]

@Members:
$itemData$ Original item data.
$listData$ Current list item data.
$listIndex$ Current list item index.

@Include: ngui.h
****************************************************************************/

                    // Define NT_FIELD structure
/*@STRUCTAPI:***************************************************************/
typedef struct NT_FIELD
    {
    DWORD       dwID;
    char        szText [NT_MAX_FIELD + 1];
    } NT_FIELD, NEAR * PNT_FIELD, FAR * LPNT_FIELD;
/*****
@Name: NT_FIELD

@Description:
$dwID$ Column I.D. of record.
$szText$ Text for record.

@Members:
$noparam$ [[To be completed]]

@Include: ngui.h
****************************************************************************/

                    // Define fill record format.
/*@STRUCTAPI:***************************************************************/
typedef struct NT_FILLRECORD
    {
    DWORD       dwData;
    WORD        wIndex;
    LPNT_FIELD  lpFields;             // Field array of record
    } NT_FILLRECORD, NEAR * PNT_FILLRECORD, FAR * LPNT_FILLRECORD;
/*****
@Name: NT_FILLRECORD

@Description:
[[To be completed]]

@Members:
$dwData$ Add record data provided.
$wIndex$ Table index of record.
$lpFields$ Field array of record.

@Include: ngui.h
****************************************************************************/

                    // column sort callback function. this
                    // should be declared SYM_EXPORT if the
                    // table code ends up in a DLL
typedef int (CALLBACK *NTSORTPROC)(HWND, const struct NT_FILLRECORD FAR *,
                const struct NT_FILLRECORD FAR *, DWORD);

                    // column paint callback function.
typedef int (CALLBACK *NTPAINTPROC)(HWND, const DRAWITEMSTRUCT FAR *,
                const RECT FAR *, const struct NT_COLUMN FAR *,
                const struct NT_FILLRECORD FAR *);

                    // Define NT_COLUMN structure
/*@STRUCTAPI:***************************************************************/
typedef struct NT_COLUMN
    {
    char                szTitle[NT_COLUMN_TITLE+1];
    DWORD               dwID;
    WORD                wWidth;
    DWORD               dwFlag;
    NTSORTPROC          lpSort;
    NTPAINTPROC         lpPaint;
    } NT_COLUMN, NEAR * PNT_COLUMN, FAR * LPNT_COLUMN;
/*****
@Name: NT_COLUMN

@Description:
This structure contains information regarding a column in a table control.

@Members:
$szTitle$ Specifies the title of the table the column belongs to.

$dwID$ Specifies the column ID.

$wWidth$ Specifies the width of the column.

$dwFlag$ Specifies the tables' window style flags.

$lpSort$ Specifies a function to use for sorting the table.

$lpPaint$ Specifies a function to use for displaying the table.

@Include: ngui.h
****************************************************************************/

                    // Define NT_BUTTON structure
/*@STRUCTAPI:***************************************************************/
typedef struct NT_BUTTON
    {
    char        szTitle[NT_BUTTON_TITLE+1];
    WORD        wID;
    WORD        wWidth;
    WORD        wHeight;
    HWND        hWnd;
    } NT_BUTTON, NEAR * PNT_BUTTON, FAR * LPNT_BUTTON;
/*****
@Name: NT_BUTTON

@Description:
This structure contains information regarding a button at the bottom of a table
control.

@Members:
$szTitle$ Specifies the button's title.
$wID$ Specifies the button's ID.
$wWidth$ Specifies the button's width.
$wHeight$ Specifies the button's height.
$hWnd$ Specifies the button's window handle.

@Include: ngui.h
****************************************************************************/


/************************************************************************
 * Control APIs                                                         *
 ************************************************************************/

#ifdef  __cplusplus

inline UINT Table_SetColumns (HWND hwnd, UINT uiNum, LPNT_COLUMN columndata)
    {
    return (UINT)
    ::SendMessage( hwnd, NTM_SETCOLUMNS, (WPARAM)uiNum, (LPARAM)columndata );
    }

inline UINT Table_SetButtons(HWND hwnd, UINT numbuttons, LPNT_BUTTON buttondata)
    {
    return (UINT)
    ::SendMessage( hwnd, NTM_SETBUTTONS, (WPARAM)numbuttons,
                   (LPARAM)buttondata );
    }

inline VOID Table_SetTitleFont (HWND hwnd, HFONT hfont, BOOL bRedraw)
    {
    ::SendMessage( hwnd, NTM_SETTITLEFONT, (WPARAM) hfont, (LPARAM)bRedraw );
    }

inline VOID Table_SetDataFont (HWND hwnd, HFONT hfont, BOOL bRedraw)
    {
    ::SendMessage( hwnd, NTM_SETDATAFONT, (WPARAM) hfont, (LPARAM)bRedraw );
    }

inline VOID Table_EnableButton (HWND hwnd, WORD buttonID, BOOL fEnable)
    {
    ::SendMessage( hwnd, NTM_ENABLEBUTTON, (WPARAM)fEnable,
           MAKELPARAM(buttonID,0) );
    }

inline VOID Table_SetButtonFont (HWND hwnd, HFONT hfont)
    {
    ::SendMessage( hwnd, NTM_SETBUTTONFONT, (WPARAM) hfont, 0L );
    }

inline void Table_AddRecord (HWND hwnd, LPVOID data)
    {
    ::SendMessage( hwnd, NTM_ADDRECORD, 0, (LPARAM) data );
    }

inline void Table_SetSortColumn (HWND hwnd, DWORD column)
    {
    ::SendMessage (hwnd, NTM_SETSORTCOLUMN, 0, (LPARAM) column);
    }

inline DWORD Table_GetSortColumn (HWND hwnd)
    {
    return (DWORD) ::SendMessage (hwnd, NTM_GETSORTCOLUMN, 0, 0L);
    }

inline WORD Table_GetColumns (HWND hwnd, WORD numcols, LPNT_COLUMN columndata)
    {
    return (WORD)
    ::SendMessage( hwnd, NTM_GETCOLUMNS, (WPARAM)numcols,
               (LPARAM)columndata );
    }

inline VOID Table_AutoSizeColumns (HWND hwnd)
    {
    ::SendMessage (hwnd, NTM_AUTOSIZECOLUMNS, 0, 0L);
    }

inline WORD Table_GetFillRecord (HWND hwnd, WORD wIndex, LPNT_FILLRECORD lpfr)
    {
    return (WORD)
    ::SendMessage (hwnd, NTM_GETFILLRECORD, (WPARAM) wIndex, (LPARAM) lpfr);
    }

inline HFONT Table_GetTitleFont (HWND hwnd)
    {
    return (HFONT)
    ::SendMessage( hwnd, NTM_GETTITLEFONT, 0, 0L );
    }

inline HFONT Table_GetDataFont (HWND hwnd)
    {
    return (HFONT)
    ::SendMessage( hwnd, NTM_GETDATAFONT, 0, 0L );
    }

inline VOID Table_SortOn (HWND hwnd)
    {
    ::SendMessage( hwnd, NTM_SORTON, 0, 0L );
    }

inline VOID Table_SortOff (HWND hwnd)
    {
    ::SendMessage( hwnd, NTM_SORTOFF, 0, 0L );
    }

inline VOID Table_Sort (HWND hwnd)
    {
    ::SendMessage( hwnd, NTM_SORT, 0, 0L );
    }

inline HWND Table_GetListboxHWND (HWND hWnd)
    {
    return (HWND)::SendMessage( hWnd, NTM_GETLISTBOXHWND, 0, 0L );
    }

inline void Table_SetSortOrder (HWND hwnd, WORD order)
    {
    ::SendMessage( hwnd, NTM_SETSORTORDER, (WPARAM) order, 0L );
    }

inline WORD Table_GetSortOrder (HWND hwnd)
    {
    return (WORD) ::SendMessage( hwnd, NTM_GETSORTORDER, 0, 0L );
    }

inline VOID Table_ShowSortDialog (HWND hwnd)
    {
    ::SendMessage( hwnd, NTM_SHOWSORTDIALOG, 0, 0L );
    }

inline VOID Table_NoSelect (HWND hwnd)
    {
    ::SendMessage( hwnd, NTM_NOSELECT, 0, 0L );
    }

inline WORD Table_GetIndexFromID (HWND hwnd, DWORD dwID)
    {
    return (WORD) ::SendMessage( hwnd, NTM_GETINDEXFROMID, 0, dwID );
    }

inline WORD Table_GetIDFromIndex (HWND hwnd, WORD wIndex)
    {
    return (WORD) ::SendMessage( hwnd, NTM_GETIDFROMINDEX, wIndex, 0L );
    }

inline BOOL Table_IsSymVwrTable (HWND hWnd)
   {
   TCHAR szBuf[30];
   if (GetClassName(hWnd, szBuf, sizeof(szBuf)/sizeof(TCHAR) ))
       {
       if (!STRICMP( _T(QCLASS_NT_TABLE), szBuf ))
           return (BOOL) ::SendMessage( hWnd, NTM_IS_SYMVWR_TBL, 0, 0L );
       }
   return(FALSE);
   }
inline DWORD Table_VwrTblGetDbHandle (HWND hwnd)
    {
    return (DWORD) ::SendMessage( hwnd, NTM_VWRTBL_GET_DBHANDLE, 0, 0L );
    }
inline DWORD Table_VwrTblGetCurrRow (HWND hwnd)
    {
    return (DWORD) ::SendMessage( hwnd, NTM_VWRTBL_GET_CURROW, 0, 0L );
    }
inline DWORD Table_VwrTblGetCurrCol (HWND hwnd)
    {
    return (DWORD) ::SendMessage( hwnd, NTM_VWRTBL_GET_CURCOL, 0, 0L );
    }
inline VOID Table_VwrTblRefresh (HWND hwnd)
    {
    ::SendMessage( hwnd, NTM_VWRTBL_RELOAD_FILE, 0, 0L );
    }
inline DWORD Table_VwrTblGetRecCount (HWND hwnd)
    {
    return (DWORD) ::SendMessage( hwnd, NTM_VWRTBL_GET_COUNT, 0, 0L );
    }
inline DWORD Table_VwrTblGetDBType (HWND hwnd)
    {
#define NTC_DBTYPE_NEFSCALE 0x0001
#define NTC_DBTYPE_NEFDB    0x0002

    return (DWORD) ::SendMessage( hwnd, NTM_VWRTBL_GET_DB_TYPE, 0, 0L );
    }
inline DWORD Table_VwrTblGetDBCurrIndex (HWND hwnd)
    {
    return (DWORD) ::SendMessage( hwnd, NTM_VWRTBL_GET_DB_CURR_INDEX, 0, 0L );
    }
inline DWORD Table_VwrTblGetSelStartRow (HWND hwnd)
    {
    return (DWORD) ::SendMessage( hwnd, NTM_VWRTBL_GET_SEL_STARTROW, 0, 0L );
    }
inline DWORD Table_VwrTblGetSelStartCol (HWND hwnd)
    {
    return (DWORD) ::SendMessage( hwnd, NTM_VWRTBL_GET_SEL_STARTCOL, 0, 0L );
    }
inline DWORD Table_VwrTblGetSelEndRow (HWND hwnd)
    {
    return (DWORD) ::SendMessage( hwnd, NTM_VWRTBL_GET_SEL_ENDROW, 0, 0L );
    }
inline DWORD Table_VwrTblGetSelEndCol (HWND hwnd)
    {
    return (DWORD) ::SendMessage( hwnd, NTM_VWRTBL_GET_SEL_ENDCOL, 0, 0L );
    }
inline DWORD Table_VwrTblGetStyle (HWND hwnd)
    {
    return (DWORD) ::SendMessage( hwnd, NTM_VWRTBL_GET_STYLE, 0, 0L );
    }
inline DWORD  Table_VwrTblSetStyle (HWND hwnd, DWORD dwStyle)
    {
    return (DWORD) ::SendMessage( hwnd,
                            NTM_VWRTBL_SET_STYLE, 0, (LPARAM)dwStyle );
    }
inline DWORD  Table_VwrTblSetCurrRow (HWND hwnd, DWORD dwRow)
    {
    return (DWORD) ::SendMessage( hwnd,
                            NTM_VWRTBL_SET_CURROW, 0, (LPARAM)dwRow );
    }
inline DWORD  Table_VwrTblSetSel (HWND hwnd, WORD wRow, WORD wCol)
    {
    return (DWORD) ::SendMessage( hwnd,
                            NTM_VWRTBL_SETSEL, 0, MAKELPARAM(wCol, wRow));
    }

inline DWORD  Table_VwrTblFldDlg (HWND hwnd)
    {
    return  (DWORD) ::SendMessage( hwnd,
                            NTM_VWRTBL_FLDDLG, 0, 0);
    }

inline DWORD  Table_VwrTblQryDlg (HWND hwnd)
    {
    return  (DWORD) ::SendMessage( hwnd,
                           NTM_VWRTBL_QRYDLG, 0, 0);
    }
inline DWORD  Table_VwrTblEditCopy (HWND hwnd)
    {
    return  (DWORD) ::SendMessage( hwnd,
                           NTM_VWRTBL_EDITCOPY, 0, 0);
    }

inline DWORD  Table_VwrTblSelectAll (HWND hwnd)
    {
    return  (DWORD) ::SendMessage( hwnd,
                           NTM_VWRTBL_SELECTALL, 0, 0);
    }

inline DWORD  Table_VwrTblGetLogic (HWND hwnd, LPVOID lpQryLogic)
    {
    return  (DWORD)::SendMessage( hwnd,
                           NTM_VWRTBL_GETLOGIC, 0, (LPARAM)lpQryLogic);
    }
inline DWORD  Table_VwrTblFileSpec (HWND hwnd, LPSTR lpszFileSpec)
    {
    return  (DWORD) ::SendMessage( hwnd,
                           NTM_VWRTBL_FILESPEC, 0, (LPARAM)lpszFileSpec);
    }
inline DWORD  Table_VwrTblNoFldView (HWND hwnd)
    {
    return  (DWORD) ::SendMessage( hwnd, NTM_VWRTBL_NO_FIELDVIEW, 0, 0L);
    }
inline STATUS Table_VwrTblDeleteItem (HWND hwnd, DWORD dwSeqNo)
    {
    return  (STATUS)::SendMessage( hwnd,
                    NTM_VWRTBL_DELETE_ITEM,  (WPARAM)TRUE, (LPARAM)dwSeqNo);
    }
inline STATUS Table_VwrTblDeleteItemEx (HWND hwnd, DWORD dwSeqNo, BOOL bRepaint)
    {
    return  (STATUS)::SendMessage( hwnd,
                    NTM_VWRTBL_DELETE_ITEM, (WPARAM)bRepaint, (LPARAM)dwSeqNo);
    }
inline STATUS Table_VwrTblDeleteItems(HWND hwnd, DWORD dwSeqNo,
                                                    DWORD dwNumItems)
    {
    NTDELITEMS ntdi;
    ntdi.dwSeqNo = dwSeqNo;
    ntdi.dwNumItems = dwNumItems;
    return  (STATUS)::SendMessage( hwnd,
                    NTM_VWRTBL_DELETE_ITEMS, 0, (LPARAM)((LPNTDELITEMS)&ntdi));
    }
inline STATUS Table_VwrTblRefreshItem (HWND hwnd, DWORD dwSeqNo)
    {
    return  (STATUS)::SendMessage( hwnd,
                    NTM_VWRTBL_REFRESH_ITEM, 0, (LPARAM)dwSeqNo);
    }
inline BOOL  Table_VwrTblIsInitialised (HWND hwnd)
    {
    return  (BOOL) ::SendMessage( hwnd, NTM_VWRTBL_IS_INITIALISED, 0, 0L);
    }

inline UINT  Table_VwrTbGetColWidth (HWND hwnd, DWORD dwCol)
    {
    return  (UINT) ::SendMessage( hwnd,
                        NTM_VWRTBL_GET_COLWIDTH, 0, (LPARAM)dwCol);
    }

inline DWORD Table_VwrTblGetTotSelected (HWND hwnd)
    {
    return  (DWORD)::SendMessage( hwnd,
                        NTM_VWRTBL_GET_TOT_SELECTED, 0, 0L);
    }

inline BOOL Table_VwrTblIsSelected (HWND hwnd, DWORD dwRow)
    {
    return  (BOOL)::SendMessage( hwnd,
                        NTM_VWRTBL_IS_SELECTED, 0, (LPARAM)dwRow);
    }


inline DWORD Table_VwrTblGetSelectionArray (HWND hwnd, LPBYTE FAR *lpArray)
    {
                                        // These are the possible values
                                        // for an item in the byte array
#define VWRTBL_SEL_NORMAL       0
#define VWRTBL_SEL_SELECTED     1
#define VWRTBL_SEL_DESELECTED   2
#define VWRTBL_SEL_SHIFTBLOCK   3
    return  (DWORD)::SendMessage( hwnd,
                        NTM_VWRTBL_GET_SELECTION_ARRAY, 0, (LPARAM)lpArray);
    }
inline DWORD Table_VwrTblGetCheckBoxArray (HWND hwnd, LPBYTE FAR *lpArray)
    {
                                        // These are the possible values
                                        // for an item in the byte array
#define VWRTBL_CHECK_OFF        0xff
#define VWRTBL_CHECK_DONT_KNOW  0x00
#define VWRTBL_CHECK_ON         0x01
    return  (DWORD)::SendMessage( hwnd,
                        NTM_VWRTBL_GET_CHECKBOX_ARRAY, 0, (LPARAM)lpArray);
    }

inline BYTE Table_VwrTblIsChecked (HWND hwnd, DWORD dwRow)
    {
    return  (BYTE)::SendMessage( hwnd,
                        NTM_VWRTBL_IS_CHECKED, 0, (LPARAM)dwRow);
    }

inline DWORD  Table_VwrTblFindDlg (HWND hwnd)
    {
    return  (DWORD) ::SendMessage( hwnd,
                           NTM_VWRTBL_FIND_DLG, 0, 0);
    }
inline DWORD  Table_VwrTblFindNext (HWND hwnd)
    {
    return  (DWORD) ::SendMessage( hwnd,
                           NTM_VWRTBL_FIND_NEXT, 0, 0);
    }
inline DWORD  Table_VwrTblFindPrev (HWND hwnd)
    {
    return  (DWORD) ::SendMessage( hwnd,
                           NTM_VWRTBL_FIND_PREV, 0, 0);
    }
inline DWORD  Table_VwrTblGoToDlg (HWND hwnd)
    {
    return  (DWORD) ::SendMessage( hwnd,
                           NTM_VWRTBL_GOTO_DLG, 0, 0);
    }
inline DWORD  Table_VwrTblSortDlg (HWND hwnd)
    {
    return  (DWORD) ::SendMessage( hwnd,
                           NTM_VWRTBL_SORT_DLG, 0, 0);
    }
inline DWORD  Table_VwrTblStyleDlg (HWND hwnd)
    {
    return  (DWORD) ::SendMessage( hwnd,
                           NTM_VWRTBL_STYLE_DLG, 0, 0);
    }
inline DWORD  Table_VwrTblFontDlg (HWND hwnd)
    {
    return  (DWORD) ::SendMessage( hwnd,
                           NTM_VWRTBL_FONT_DLG, 0, 0);
    }
inline DWORD  Table_VwrTblSetNotifyCB(HWND hwnd, LPVOID lpNSCommCB, LPARAM lParam)
    {
    NTNOTIFYCB ncb;
    ncb.lpNSCommCB = lpNSCommCB;
    ncb.lParam = lParam;
    return  (DWORD) ::SendMessage( hwnd,
             NTM_VWRTBL_SET_NOTIFYCB, (WPARAM)sizeof(ncb), (LPARAM)&ncb);
    }

inline void Table_VwrTblFldViewOverride (HWND hwnd, LPSTR lpszFileSpec)
    {
    ::SendMessage( hwnd, NTM_VWRTBL_FIELDVIEW_OVERRIDE, 0, (LPARAM)lpszFileSpec);
    }

inline VOID Table_VwrTblStatusSet (HWND hwnd)
    {
    ::SendMessage( hwnd, NTM_VWRTBL_STATUS_SET, 0, 0L );
    }
inline VOID Table_VwrTblStatusClear (HWND hwnd)
    {
    ::SendMessage( hwnd, NTM_VWRTBL_STATUS_CLEAR, 0, 0L );
    }

inline VOID Table_VwrTblAddNefOid (HWND hwnd, LPVOID lpNefOid, BOOL bRepaint)
    {
    ::SendMessage( hwnd, NTM_VWRTBL_ADD_NEFOID, (WPARAM)bRepaint, (LPARAM)lpNefOid );
    }

// new for Quake rev 0
inline WORD Table_ShowButton (HWND hwnd, WORD buttonID, BOOL fEnable)
    {
    return (WORD)
    ::SendMessage( hwnd, NTM_SHOWBUTTON, (WPARAM) fEnable,
    MAKELPARAM(buttonID,0) );
    }

#else    // #if !defined(__cplusplus)
#define Table_IsSymVwrTable(hwnd) \
            (BOOL)SendMessage( hwnd, NTM_IS_SYMVWR_TBL, 0, 0L )

#define Table_VwrTblGetDbHandle(hwnd) \
            (DWORD)SendMessage( hwnd, NTM_VWRTBL_GET_DBHANDLE, 0, 0L )

#define Table_VwrTblGetCurrRow(hwnd) \
            (DWORD)SendMessage( hwnd, NTM_VWRTBL_GET_CURROW, 0, 0L )

#define Table_VwrTblGetCurrCol(hwnd) \
            (DWORD)SendMessage( hwnd, NTM_VWRTBL_GET_CURCOL, 0, 0L )

#define Table_VwrTblRefresh(hwnd) \
            SendMessage( hwnd, NTM_VWRTBL_RELOAD_FILE, 0, 0L )

#define Table_VwrTblGetRecCount(hwnd) \
            (DWORD) SendMessage( hwnd, NTM_VWRTBL_GET_COUNT, 0, 0L )

#define Table_VwrTblGetDBType(hwnd) \
            (DWORD) SendMessage( hwnd, NTM_VWRTBL_GET_DB_TYPE, 0, 0L )

#define Table_VwrTblGetDBCurrIndex(hwnd) \
            (DWORD) SendMessage( hwnd, NTM_VWRTBL_GET_DB_CURR_INDEX, 0, 0L )

#define Table_VwrTblGetSelStartRow(hwnd) \
            (DWORD) SendMessage( hwnd, NTM_VWRTBL_GET_SEL_STARTROW, 0, 0L )

#define Table_VwrTblGetSelStartCol(hwnd) \
            (DWORD) SendMessage( hwnd, NTM_VWRTBL_GET_SEL_STARTCOL, 0, 0L )

#define Table_VwrTblGetSelEndRow(hwnd) \
            (DWORD) SendMessage( hwnd, NTM_VWRTBL_GET_SEL_ENDROW, 0, 0L )

#define Table_VwrTblGetSelEndCol(hwnd) \
            (DWORD) SendMessage( hwnd, NTM_VWRTBL_GET_SEL_ENDCOL, 0, 0L )

#define Table_VwrTblGetStyle(hwnd)     \
            (DWORD) SendMessage( hwnd, NTM_VWRTBL_GET_STYLE, 0, 0L )

#define Table_VwrTblSetStyle(hwnd, dwStyle)    \
            (DWORD) SendMessage( hwnd,          \
                            NTM_VWRTBL_SET_STYLE, 0, (LPARAM)dwStyle )

#define Table_VwrTblSetCurrRow(hwnd, dwRow)    \
            (DWORD) SendMessage( hwnd,          \
                            NTM_VWRTBL_SET_CURROW, 0, (LPARAM)dwRow )

#define Table_VwrTblSetSel(hwnd, wRow, wCol)   \
            (DWORD) SendMessage( hwnd,          \
                            NTM_VWRTBL_SETSEL, 0, MAKELPARAM(wCol, wRow))

#define Table_VwrTblFldDlg(hwnd) \
            (DWORD) SendMessage( hwnd,          \
                            NTM_VWRTBL_FLDDLG, 0, 0)
#define Table_VwrTblQryDlg(hwnd) \
            (DWORD) SendMessage( hwnd,          \
                            NTM_VWRTBL_QRYDLG, 0, 0)
#define Table_VwrTblEditCopy(hwnd) \
            (DWORD) SendMessage( hwnd,          \
                            NTM_VWRTBL_EDITCOPY, 0, 0)
#define Table_VwrTblSelectAll(hwnd) \
            (DWORD) SendMessage( hwnd,          \
                            NTM_VWRTBL_SELECTALL, 0, 0)

#define Table_VwrTblGetLogic( hwnd, lpQryLogic)  \
            (DWORD) SendMessage( hwnd,          \
                            NTM_VWRTBL_GETLOGIC, 0, (LPARAM)lpQryLogic)

#define Table_VwrTblFileSpec( hwnd, lpszFileSpec) \
            (DWORD) SendMessage(hwnd,          \
                            NTM_VWRTBL_FILESPEC, 0, (LPARAM)lpszFileSpec)
#define Table_VwrTblNoFldView(hwnd) \
            (DWORD) SendMessage( hwnd, NTM_VWRTBL_NO_FIELDVIEW, 0, 0L)

#define Table_VwrTblDeleteItem( hwnd, dwSeqNo)      \
            (STATUS)SendMessage( hwnd,              \
                NTM_VWRTBL_DELETE_ITEM, (WPARAM)TRUE, (LPARAM)dwSeqNo)

#define Table_VwrTblDeleteItemEx( hwnd, dwSeqNo, bRepaint)      \
            (STATUS)SendMessage( hwnd,              \
                NTM_VWRTBL_DELETE_ITEM, (WPARAM)bRepaint, (LPARAM)dwSeqNo)

#define Table_VwrTblDeleteItems(hwnd, dwSeqNo, dwNumItems)              \
    {                                                                   \
    NTDELITEMS ntdi;                                                    \
    ntdi.dwSeqNo = dwSeqNo;                                             \
    ntdi.dwNumItems = dwNumItems;                                       \
    (STATUS)SendMessage( hwnd,                                          \
             NTM_VWRTBL_DELETE_ITEMS, 0, (LPARAM)((LPNTDELITEMS)&ntdi)); \
    }

#define Table_VwrTblRefreshItem( hwnd, dwSeqNo)     \
            (STATUS)SendMessage( hwnd,              \
                    NTM_VWRTBL_REFRESH_ITEM, 0, (LPARAM)dwSeqNo)

#define Table_VwrTblIsInitialised(hwnd) \
            (BOOL)SendMessage(hwnd, NTM_VWRTBL_IS_INITIALISED, 0, 0L)

#define Table_VwrTbGetColWidth(hwnd, dwCol) \
    (UINT)SendMessage(hwnd, NTM_VWRTBL_GET_COLWIDTH, 0, (LPARAM)dwCol)

#define Table_VwrTblGetTotSelected(hwnd) \
            (DWORD)SendMessage(hwnd, NTM_VWRTBL_GET_TOT_SELECTED, 0, 0L)

#define Table_VwrTblIsSelected(hwnd, dwRow) \
            (BOOL)SendMessage(hwnd, NTM_VWRTBL_IS_SELECTED, 0, (LPARAM)dwRow)

#define Table_VwrTblIsChecked(hwnd, dwRow) \
            (BYTE)SendMessage(hwnd, NTM_VWRTBL_IS_CHECKED, 0, (LPARAM)dwRow)

#define Table_VwrTblGetSelectionArray(hwnd, lplpSelArray) \
            (DWORD)SendMessage(hwnd, NTM_VWRTBL_GET_SELECTION_ARRAY, 0, (LPARAM)lplpSelArray)

#define Table_VwrTblGetCheckBoxArray(hwnd, lplpSelArray) \
            (DWORD)SendMessage(hwnd, NTM_VWRTBL_GET_CHECKBOX_ARRAY, 0, (LPARAM)lplpSelArray)

#define Table_VwrTblFindDlg(hwnd) \
            (DWORD) SendMessage( hwnd,          \
                            NTM_VWRTBL_FIND_DLG, 0, 0)
#define Table_VwrTblFindNext(hwnd) \
            (DWORD) SendMessage( hwnd,          \
                            NTM_VWRTBL_FIND_NEXT, 0, 0)
#define Table_VwrTblFindPrev(hwnd) \
            (DWORD) SendMessage( hwnd,          \
                            NTM_VWRTBL_FIND_PREV, 0, 0)
#define Table_VwrTblGoToDlg(hwnd) \
            (DWORD) SendMessage( hwnd,          \
                            NTM_VWRTBL_GOTO_DLG, 0, 0)
#define Table_VwrTblSortDlg(hwnd) \
            (DWORD) SendMessage( hwnd,          \
                            NTM_VWRTBL_SORT_DLG, 0, 0)
#define Table_VwrTblStyleDlg(hwnd) \
            (DWORD) SendMessage( hwnd,          \
                            NTM_VWRTBL_STYLE_DLG, 0, 0)
#define Table_VwrTblFontDlg(hwnd) \
            (DWORD) SendMessage( hwnd,          \
                            NTM_VWRTBL_FONT_DLG, 0, 0)
#define Table_VwrTblSetNotifyCB(hwnd, lpNSCommCB, lParam)   \
{                                                           \
    NTNOTIFYCB ncb;                                         \
    ncb.lpNSCommCB = lpNSCommCB;                            \
    ncb.lParam = lParam;                                    \
    (DWORD)SendMessage(hwnd, NTM_VWRTBL_SET_NOTIFYCB, (WPARAM)sizeof(ncb), (LPARAM)&ncb); \
}
#define Table_VwrTblFldViewOverride( hwnd, lpszFileSpec) \
            SendMessage(hwnd, NTM_VWRTBL_FIELDVIEW_OVERRIDE, 0, (LPARAM)lpszFileSpec)

#define Table_VwrStatusSet(hwnd) \
            SendMessage( hwnd, NTM_VWRTBL_STATUS_SET, 0, 0L )
#define Table_VwrStatusClear(hwnd) \
            SendMessage( hwnd, NTM_VWRTBL_STATUS_CLEAR, 0, 0L )

#define Table_VwrTblAddNefOid(hwnd, lpNefOid, bRepaint) \
            SendMessage( hwnd, NTM_VWRTBL_ADD_NEFOID, (WPARAM)bRepaint, (LPARAM)lpNefOid )

#endif  // __cplusplus

#ifdef  _INC_WINDOWSX

/* BOOL NTM_OnFillRecord(HWND hwnd, NT_FILLRECORD FAR *lpFillRecord); */
#define HANDLE_NTM_FILLRECORD(hwnd, wParam, lParam, fn) \
    MAKELRESULT((BOOL)(fn)((hwnd), (NT_FILLRECORD FAR *)(lParam)), 0)
#define FORWARD_NTM_FILLRECORD(hwnd, lpFillRecord, fn) \
    (BOOL)(DWORD)(fn)((hwnd), NTM_FILLRECORD, 0, (LPARAM)(NT_FILLRECORD FAR *)(lpFillRecord))

/* void NTM_OnDeleteItem(HWND hwnd, UINT itemID, DWORD itemData); */
#define HANDLE_NTM_DELETEITEM(hwnd, wParam, lParam, fn) \
    ((fn)((hwnd), (UINT)(wParam), (DWORD)(lParam)), 0L)
#define FORWARD_NTM_DELETEITEM(hwnd, itemID, itemData, fn) \
    (void)(fn)((hwnd), NTM_DELETEITEM, (WPARAM)(itemID), (LPARAM)(itemData))

#endif

/************************************************************************
 * Table Functions                                                      *
 ************************************************************************/

UINT WINAPI NGTableInit (void);         // Internal
UINT WINAPI NGTableDone (void);         // Internal


HWND SYM_EXPORT WINAPI NGTableCreate (
    HWND        hParent,                // parent window handle
    DWORD       dwStyle,                // table style
    UINT        wID,                    // table identifier
    UINT        wColumns,               // number of columns
    LPNT_COLUMN lpColumnData,           // column data
    UINT        wButtons,               // number of buttons
    LPNT_BUTTON lpButtonData);          // button data


                                        // The VwrTbl uses a Symvwr
                                        // view pane instead of the
                                        // standard list box. This allows
                                        // the perusal of large files
                                        // in excess of 50,000 recs.
                                        // Once the table control has been
                                        // created with the following api
                                        // you can start sending it
                                        // VWR_FILE_SPEC messages
                                        // as defined in Vwr.h and as
                                        // described in Symvwr.doc in
                                        // the quake docs area.
                                        // The button data is the same
                                        // as the original control.
                                        // Column data is not required
                                        // since it is handled by Symvwr.
HWND SYM_EXPORT WINAPI NGVwrTblCreate (
    HWND            hParent,            // parent window handle
    DWORD           dwStyle,            // table style
    UINT            wID,                // table identifier
    UINT            wButtons,           // number of buttons
    LPNT_BUTTON     lpButtonData);      // button data

                                        // Callback used by
                                        // NGVwrTblSendDBSpec()
typedef VOID (WINAPI *NGSETSTATUSTEXT) (
   HWND  hWndParent,                    // [in] hWnd that owns status bar
   HWND  hWndVwrTbl,                    // [in] hWnd of the table control
   LPSTR lpszText                       // [in] Null Terminated Text
);

                                        // Define the trays for the status
                                        // bar
#define NG_TRAY_TEXT      5001
#define NG_TRAY_COUNT     5002
#define NG_TRAY_FILTER    5003
#define NG_TRAY_DATETIME  5004          // DateTime not currently supported
typedef WORD (WINAPI *NGSETSTATUSTRAY) (
    HWND        hWndParent,             // [in] hWnd that owns status bar
    HWND        hWndChild,              // [in] always NULL for Tbl Control
    WORD        wTrayID,                // [in] I.D. of tray
    LPSTR       lpszText                // [in] Text to use
);

STATUS SYM_EXPORT WINAPI PASCAL NGVwrTblSendDBSpec(
    HWND    hWndVwr,                    // [in] VwrTbl window
    LPSTR   lpszFileName,               // [in] File name
    LPSTR   lpszTableName,              // [in] Can be a table name
                                        //   or a FieldView (.FLD) file
                                        //   depending on
                                        //   dwNScalFlags *or*
                                        //   can be an index # using
                                        //   MAKEINTRESOURCE()
    HWND    hWndStatus,                 // [in] A Symgui Status bar or NULL
                                        //      *or*, if lpStatusProc is
                                        //      not NULL, the hWnd that
                                        //      owns the status display --
                                        //      this will be used as the
                                        //      hWndParent of lpStatusProc.
    NGSETSTATUSTRAY lpStatusProc,       // [in] Must be NULL if hWndStatus
                                        //      is a Symgui status bar. If
                                        //      a callback is specified here
                                        //      hWndStatus will be used as
                                        //      the hWndParent when the
                                        //      status callback
                                        //      function is invoked.
    HFONT   hFont,                      // [in] Valid font or NULL
    HFONT   hFontTitle,                 // [in] Font for col titles or NULL
    DWORD   dwNScalFlags,               // [in] see NSCAL_FLAG_...'s
                                        //   in NefScal.h
    DWORD   dwVwrFlags);                // [in] see VWR_STY_.. flags
                                        //   in Vwr.h


/************************************************************************/
/*                                                                      */
/* Helper function to package and send message to the table control     */
/* or database viewer and supplies Query Info                           */
/*                                                                      */
/************************************************************************/
STATUS SYM_EXPORT WINAPI PASCAL NGVwrTblSendDBSpecEx(
       HWND hWndVwr,                    // [in] VwrTbl window
       LPSTR lpszFileName,              // [in] File name
       LPSTR lpszTableName,             // [in] Can be a table name
                                        //   or a FieldView file
                                        //   depending on
                                        //   dwNScalFlags *or*
                                        //   can be an index # using
                                        //   MAKEINTRESOURCE()
       LPCSTR   lpszQryDef,             // Query definition file name or NULL
       LPVOID   lpNLogic,               // Address of Logic structure or NULL if QryDef is passed
       UINT     uLogicNum,              // Number of logic structures or 0

       HWND  hWndStatus,                // [in] A Symgui Status bar or NULL
       NGSETSTATUSTRAY lpStatusProc,    // [in] Optionally used if hWndStatus
                                        //      is not a Symgui status bar
       HFONT hFont,                     // [in] Valid font or NULL
       HFONT hFontTitle,                // [in] Font for col titles or NULL
       DWORD dwNScalFlags,              // [in] see NSCAL_FLAG_...'s
                                        //   in NefScal.h
       DWORD dwVwrFlags);               // [in] VWR_STY_.. flags
                                        //   in Vwr.h

STATUS SYM_EXPORT WINAPI NGTableBroadcastMsg(
                            HWND   hWnd,            // [in] Parent hWnd
                            UINT   uMsg,            // [in]
                            WPARAM wParam,          // [in]
                            LPARAM lParam);         // [in]

/*----------------------------------------------------------------------*/
/*  Standard Style value for all Symantec Dialog boxes                  */
/*----------------------------------------------------------------------*/
#ifndef NDLG_STYLE
#define NDLG_STYLE  WS_CAPTION | WS_SYSMENU | WS_POPUP | DS_MODALFRAME | DS_SETFONT
#endif

/*----------------------------------------------------------------------*/
/* This macro should be used to convert font points into logical units  */
/* for the mapping mode of the given dc.  USE THIS WHEN ASSIGNING       */
/* THE lfHeight FIELD OF THE LOGFONT STRUCTURE!!!                       */
/* Give the desired point size in the pt parameter.                     */
/*----------------------------------------------------------------------*/
#ifndef PT_TO_PIXELS
#define PT_TO_PIXELS(hdc, pt)   \
    MulDiv(-(pt), GetDeviceCaps(hdc, LOGPIXELSY), 72)
#endif

//------------------------------------------------------------------------
// This macro is used to convert the logical units of the value into
// screen units.
//------------------------------------------------------------------------
#define LOGICAL_TO_POINT(hdc, x) \
    (((x) * 72) / GetDeviceCaps (hdc, LOGPIXELSY))


//------------------------------------------------------------------------
// This macro is used for covering the default dialog proc of cpDlgProc
//------------------------------------------------------------------------
#define NG_DefDlgProc(h,m,w,l)   cpDlgProc(h,u,w,l)


//------------------------------------------------------------------------
// Initialization & shutdown routines.
//------------------------------------------------------------------------
WORD SYM_EXPORT WINAPI NG_Init(void);
VOID SYM_EXPORT WINAPI NG_Done(void);

STATUS SYM_EXPORT WINAPI NG_StaticRegister(void);
STATUS SYM_EXPORT WINAPI NG_TrayRegister(void);
STATUS SYM_EXPORT WINAPI NG_ColorBoxRegister(void);
STATUS SYM_EXPORT WINAPI NG_VCRRegister(void);
STATUS SYM_EXPORT WINAPI NG_TableRegister(void);
STATUS SYM_EXPORT WINAPI NG_TunnelRegister(void);
STATUS SYM_EXPORT WINAPI NG_SpinRegister(void);
STATUS SYM_EXPORT WINAPI NG_FillBarRegister(void);
STATUS SYM_EXPORT WINAPI NG_StatusRegister(void);
STATUS SYM_EXPORT WINAPI NG_ButtonBarRegister(void);
STATUS SYM_EXPORT WINAPI NG_DriveListRegister(void);
STATUS SYM_EXPORT WINAPI NG_SliderRegister(void);

BOOL SYM_EXPORT WINAPI NG_ColorChange(void);

                    // Resource definitions
#define IDC_EDIT_SEARCHTEXT         101

#define IDC_CB_SORTCOLUMNS          102
#define IDC_RB_SORTASCENDING        103
#define IDC_RB_SORTDESCENDING       104

                    // Speedsearch flags
#define SPDSRCH_SMARTFILL   0x0001      // Smartfill
#define NGSS_CONSTANTUPDATE 0x0002      // Call callback on all selchanges

                    // Define callback format for Speed-srch
typedef LRESULT (CALLBACK* SSPROC)(
   WORD     wCmd,                       // [in]    Command to perform
   HWND     hWnd,                       // [in]    Listbox handle
   WORD     wID,                        // [in]    Listbox ID
   LPARAM   lParam                      // [in]    Long Parameter
);

                    // Speed-search callback commands and
                    // structures
#define SSCMD_COMPARE      1

/*@STRUCTAPI:****************************************************************/
typedef struct {
   int      nIndex;
   LPSTR    lpszSearchText;
   BOOL     bExact;
} SSCOMPARE, FAR * LPSSCOMPARE;
/*****
@Name: SSCOMPARE

@Description:
This structure contains information utilized by the NG_StringSpeedSearchProc
callback function.

@Members:
$nIndex$ Index of listbox item to compare.

$lpszSearchText$ Far pointer to current text to search for.

$bExact$ Set to TRUE if an exact match is required.

@See: NG_StringSpeedSearchProc
@Include: ngui.h
*****************************************************************************/

#define SSCMD_GETTEXT      2

/*@STRUCTAPI:****************************************************************/
typedef struct {
   int      nIndex;
   LPSTR    lpszBuffer;
   WORD     wMaxBufSize;
} SSGETTEXT, NEAR * NPSSGETTEXT, FAR * LPSSGETTEXT;
/*****
@Name: SSGETTEXT

@Description:
This structure contains information utilized by the NG_StringSpeedSearchProc
callback function.

@Members:
$nIndex$ Index of listbox item to compare.

$lpszBuffer$ Far pointer to buffer to fill.

$wSize$ Size of the lpszBuffer string.

@See: NG_StringSpeedSearchProc
@Include: ngui.h
*****************************************************************************/


                    // LOWORD of lParam indicates
                    // current (FALSE) or final (TRUE)
                    // sel change.  Current is only sent
                    // if SpeedSearch is created with
                    // NGSS_CONSTANTUPDATE flag
#define SSCMD_SETSEL       3

                    // Flags for calls
#define NG_FLAT         0x0000
#define NG_LOWERED      0x0001
#define NG_RAISED       0x0002
#define NG_HORIZ        0x0004
#define NG_VERT         0x0008
#define NG_FRAMED       0x0010


VOID SYM_EXPORT WINAPI NG_Draw3DFrame (
   HWND        hWnd,                    // [in]    Window Handle
   HDC         hDC,                     // [in]    hDC to use
   LPRECT      lpClientRect,            // [in]    Client rect struct
   WORD        wStyle,                  // [in]    NG_RAISED or NG_LOWERED
   int         nBorder                  // [in]    Thickness of border
);

VOID SYM_EXPORT WINAPI NG_DrawGroove (
   HWND        hWnd,                    // [in]    Window Handle
   HDC         hDC,                     // [in]    hDC to use
   LPRECT      lpClientRect,            // [in]    Client rect struct
   WORD        wStyle,                  // [in]    NG_HORIZ or NG_VERT
   BOOL        bIsLowered,              // [in]    Is lowered boolean
   int         nBorder                  // [in]    Thickness of border
);

VOID SYM_EXPORT WINAPI NG_DrawDialogBackground (
   HWND        hWnd,                    // [in]    Dialog Handle
   HDC         hDC                      // [in]    hDC to use
);

VOID SYM_EXPORT WINAPI NG_DrawCanvas (
   HWND        hWnd,                    // [in]    Dialog Handle
   HDC         hDC,                     // [in]    hDC to use
   LPRECT      lpClientRect,            // [in]    Client rect struct
   WORD        wStyle                   // [in]    Style of canvas
);

VOID SYM_EXPORT WINAPI NG_DrawText (
   HWND        hWnd,                    // [in]    Window Handle
   HDC         hDC,                     // [in]    hDC to use
   LPSTR       lpstrText,               // [in]    Text to draw
   int         iStrLength,              // [in]    Size of text
   LPRECT      lpRect,                  // [in]    Client rect struct
   LONG        lStyle                   // [in]    Style of canvas
);


WORD SYM_EXPORT WINAPI NG_FontGet(
    HWND  hWnd,                         // Window to get DC from...
    HFONT FAR *lphFont,                 // If NULL, then free cached fonts
    int nWeight,                        // FW_NORMAL, FW_BOLD, or custom wt
    int nHeight );                      // If zero, then use default height

HFONT SYM_EXPORT WINAPI NG_CreateFontIndirect(
    LPLOGFONT pFontStruct);              // [in] Font structure


VOID SYM_EXPORT WINAPI NGDrawSunkenControl (
   HWND        hwnd,
   HDC         hdc,
   LPRECT      lpRect
);

VOID SYM_EXPORT WINAPI NG_DrawBitmap (
   HDC         hDC,                     // (INPUT) Device context to use
   LPRECT      lpRect,                  // (INPUT) Dimensions of bitmap
   HBITMAP     hBitmap,                 // (INPUT) Bitmap to use
   WORD        wFlags                   // (INPUT) Flags
);


HBITMAP SYM_EXPORT WINAPI NGCreateDisabledBitmap (HBITMAP hBitmap);
BOOL SYM_EXPORT WINAPI NGDrawMaskedBitmap (HDC hDC, HBITMAP hBitmap, HBITMAP hMask, int x, int y);


BOOL SYM_EXPORT WINAPI NG_AddSpeedSearch (
   HWND     hWndListbox,                // [in]    Listbox to attach to
   WORD     wFlags,                     // [in]    Reserved (position info)
   SSPROC   fpCallback,                 // [in]    Callback proc (owner-draw)
   DWORD    dwReserved                  // [in]    Reserved for future
);

VOID SYM_EXPORT WINAPI NG_SetMessageWindow (
   HWND     hWndListbox,                // [in]    Listbox that is subclassed
   HWND     hWndMessageWindow           // [in]    used to set where requests go
);

int SYM_EXPORT WINAPI NG_SpeedSearch (
   HWND     hWndListbox,                // [in]    Listbox window handle
   LPSTR    lpszText,                   // [in]    Text to search for
   int      nStartIndex,                // [in]    Starting index
   BOOL     bExact,                     // [in]    Make exact match?
   BOOL     bForward,                   // [in]    Search direction
   BOOL     bNextMatch,                 // [in]    Skip wStartIndex
   SSPROC   searchproc,                 // [in]    Search Proc
   LPSTR    lpszBuffer,                 // [in]    Text buffer filled
   WORD     wBufSize                    // [in]    Maximum buffer size
);

LRESULT SYM_EXPORT WINAPI NG_StringSpeedSearchProc (
   WORD     wCmd,                       // [in]    Command
   HWND     hWnd,                       // [in]    List box handle
   WORD     wID,                        // [in]    List box ID
   LPARAM   lParam                      // [in]    Long Parameter
);

BOOL SYM_EXPORT WINAPI NG_IsSpeedSearching (
    HWND hWndListbox                    // [in]    Handle to compare or null
);

VOID SYM_EXPORT WINAPI NG_PropSetObject (
   HWND     hWnd,                       // [in]    Window handle to use
   LPVOID   lpObjectPtr,                // [in]    Object pointer to save
   LPSTR    lpszPropName                // [in]    Property Name
);

LPVOID SYM_EXPORT WINAPI NG_PropGetObject (
   HWND     hWnd,                       // [in]    Window handle to use
   LPSTR    lpszPropName                // [in]    Property Name
);

LPVOID SYM_EXPORT WINAPI NG_PropRemoveObject (
   HWND     hWnd,                       // [in]    Window handle to use
   LPSTR    lpszPropName                // [in]    Property Name
);

LONG SYM_EXPORT CALLBACK NGButtonBarWndFn (
   HWND     hWnd,
   UINT     message,
   WPARAM   wParam,
   LPARAM   lParam
);

VOID SYM_EXPORT WINAPI NG_DrawRect (
   HWND        hwnd,
   HDC         hdc,
   LPRECT      lpRect,
   BOOL        bLight,
   int         iBorder
);

VOID SYM_EXPORT WINAPI NG_DrawRoundedRect (
   HWND        hwnd,
   HDC         hdc,
   LPRECT      lpRect,
   HBRUSH      hBrush
);

VOID SYM_EXPORT WINAPI NG_DrawButton (
   HWND        hwnd,
   HDC         hdc,
   LPRECT      lpRect,
   BOOL        bRaised                  // [in]    True/False (true = not selected)
);

VOID SYM_EXPORT WINAPI NG_DrawRectSpecial (
   HWND        hwnd,                    // [in]    Window Handle
   HDC         hdc,                     // [in]    Handle to Device Context
   LPRECT      lpRect,                  // [in]    Pointer to Rectangle
   HBRUSH      hLeft,                   // [in]    Brush for left side
   HBRUSH      hTop,                    // [in]    Brush for top side
   HBRUSH      hRight,                  // [in]    Brush for right side
   HBRUSH      hBottom                  // [in]    Brush for bottom side
);

VOID SYM_EXPORT WINAPI NG_SetControlFont (
   HWND     hWnd,                       // [in]    Dialog Box Window
   HFONT    hFont,                      // [in]    Font to use
   LPSTR    lpszClass,                  // [in]    Control window class or ""
   DWORD    dwStyle                     // [in]    Optional style bit to match
);

VOID SYM_EXPORT WINAPI NG_SetFontsInDialog (
    HWND    hDlg,                       // [in]     Dialog window
    DWORD   dwReserved                  // [in]     Reserved (user zero)
);

VOID SYM_EXPORT WINAPI NG_SetFontInControl(
    HWND  hWnd                          // [in]     Window control
);

VOID SYM_EXPORT WINAPI NG_SetChildSmallFont ( // Sets child to small font
    HWND        hWnd,                   // [in] Parent handle
    WORD        wChildID);              // [in] ID of child control

VOID SYM_EXPORT WINAPI NG_SetChildLargeFont ( // Sets child to large/bold font
    HWND        hWnd,                   // [in] Parent handle
    WORD        wChildID);              // [in] ID of child control

VOID SYM_EXPORT WINAPI NG_SetGroupBoxSunken ( // Sets group box to sunken
    HWND        hWnd,                   // [in] Parent handle
    UINT        uChildID);              // [in] ID of child control

BYTE SYM_EXPORT WINAPI NG_GetDBCSCharSet ( // returns the proper character set identifier
	VOID );

LPTSTR SYM_EXPORT WINAPI NG_GetDBCSFaceName ( // returns the hardcoded DBCS font face name from DBCS.H
	VOID );

int SYM_EXPORT WINAPI NG_GetDBCSFontSize ( // returns the hardcoded DBCS font size from DBCS.H
	VOID );

BOOL SYM_EXPORT WINAPI NG_CenterDialog (HWND hWnd, HWND hParentWnd);

// Flags for NG_StdDlgLook:

#define SDL_NOCENTER            0x0001  // don't do NG_CenterDialog
#define SDL_NOFONTCHANGE        0x0002  // don't do NG_SetFontsInDialog
#define SDL_PARENTCENTER        0x0004  // center dialog based on parent
#define SDL_NOCTL3D             0x0008  // don't do Ctl3d subclass

UINT SYM_EXPORT WINAPI NG_StdDlgLook (HWND hDlg, UINT wFlags);

//------------------------------------------------------------------------
// Status Bar stuff
//------------------------------------------------------------------------

/*@STRUCTAPI:****************************************************************/
typedef struct NGSTATUSHELPREC
    {
    WORD        wMenuID;
    WORD        wMessageID;
    } NGSTATUSHELPREC, FAR *LPNGSTATUSHELPREC;
/*****
@Name: NGSTATUSHELPREC

@Description:
This structure contains information used to display messages about menu items
in the status bar.

@Members:
$wMenuID$ Specifies the ID of the menu to associate with the message.

$wMessageID$ Specifies the string table resource identifier of the message
to display.

@Include: ngui.h
*****************************************************************************/

                    // Tells NGStatus to use an edit
                    // field in lieu of a static text
#define NS_USEEDIT      0x00008000L

                    // Tells NGStatus to use a simple
                    // raised bar instead of a raised
                    // bar with an indented text tray.
                    // Not available with NS_USEEDIT.
#define NS_PLAIN        0x00010000L


LONG SYM_EXPORT CALLBACK NGStatusWndFn (HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);

void SYM_EXPORT WINAPI NGStatusBarHelp (HINSTANCE hInst, HWND hwndStatus,
        WPARAM wParam, LPARAM lParam, LPNGSTATUSHELPREC lpMenuInfo);

void SYM_EXPORT WINAPI NGStatusBarHelp2 (HINSTANCE hInst, HWND hwndStatus,
            UINT nItemID, UINT nFlags, HMENU hSysMenu,
            LPNGSTATUSHELPREC lpMenuInfo);

                    // - Doug N. 5/26/93 Add for Tray Help
void SYM_EXPORT WINAPI NGTrayStatusHelp (HINSTANCE hInst, HWND hwndStatus, WORD wTrayID,
    WPARAM wParam, LPARAM lParam, LPNGSTATUSHELPREC lpMenuInfo);


#ifdef  __cplusplus

inline void StatusBar_SetText(HWND hwndCtl, LPCSTR text)
    {
    ::SendMessage(hwndCtl, WM_SETTEXT, 0, (LPARAM) text);
    }

inline void StatusBar_SetFont(HWND hwndCtl, HFONT hfont, BOOL fRedraw)
    {
    ::SendMessage(hwndCtl, WM_SETFONT, (WPARAM) hfont, MAKELPARAM((WORD) fRedraw, 0));
    }

#else

#define StatusBar_SetText(hwndCtl,text)                 \
    (void)SendMessage((hwndCtl), WM_SETTEXT, 0, (LPARAM)(LPSTR)(text))

#define StatusBar_SetFont(hwndCtl, hfont, fRedraw)      \
    (void)SendMessage((hwndCtl),WM_SETFONT,(WPARAM)(HFONT)(hfont),MAKELPARAM((UINT)(BOOL)(fRedraw), 0))

#endif

//------------------------------------------------------------------------
// Button Bar definitions
//------------------------------------------------------------------------


#define NG_BUTTONBAR_CLASSNAME      QCLASS_NG_BUTTONBAR

                    // Typedef structure windows forgot
                    // to do.
typedef MINMAXINFO FAR *   _LPMINMAXINFO;

                    // Error
#define BB_ERR                -1

                    // Messages
#define BB_ADDBUTTON          WM_USER+1
#define BB_RESETCONTENT       WM_USER+2
#define BB_SETBUTTONSIZE      WM_USER+3
#define BB_GETBUTTONSIZE      WM_USER+4
#define BB_SETMINMAXINFO      WM_USER+5
#define BB_GETMINMAXINFO      WM_USER+6
#define BB_SETSEPARATORSIZE   WM_USER+7
#define BB_GETSEPARATORSIZE   WM_USER+8
#define BB_SETBASE            WM_USER+9
#define BB_GETBASE            WM_USER+10
#define BB_ENABLEBUTTON       WM_USER+11
#define BB_SETITEMDATA        WM_USER+12
#define BB_GETITEMDATA        WM_USER+13
#define BB_SETSTYLE           WM_USER+14
#define BB_GETTEXT            WM_USER+15
#define BB_GETTEXTLENGTH      WM_USER+16
#define BB_SETTEXT            WM_USER+17
#define BB_SETNORMALBMP       WM_USER+18
#define BB_SETSELECTEDBMP     WM_USER+19
#define BB_SETNORMALBMPMASK   WM_USER+20
#define BB_SETSELECTEDBMPMASK WM_USER+21
#define BB_GETSTYLE           WM_USER+22
#define BB_ENABLEBUTTONID     WM_USER+23
#define BB_AUTOSIZEBUTTONS    WM_USER+24

/*@STRUCTAPI:*************************************************************/
typedef struct {
   WORD     wID;
   WORD     wFlags;
   HANDLE   hNormal;
   HANDLE   hSelected;
   LPSTR    lpszText;
   HBITMAP  hNormalMask;
   HBITMAP  hSelectedMask;
} BUTTONDATA, NEAR * NPBUTTONDATA, FAR * LPBUTTONDATA;
/*****
@Name: BUTTONDATA

@Description:
This structure contains information used to display a button in a button bar.

@Members:
$wID$ Specifies the button ID.

$wFlags$ The wFlags field can contain one or more of the following flags:
    ~BBFLAG_ICONS~ The handles in the structure are handles to loaded icon
    resources, rather than icons.
    ~BBFLAG_SEPARATOR~ The button referenced by this structure is actually a
    separator, not a real button.
    ~BBFLAG_TEXT~ Use the text pointed to by the lpszText parameter of the
    structure for text under the button.
    ~BBFLAG_NODROP~ Don't accept draglist messages.
    ~BBFLAG_FILEDROP~ ccept drag/drop messages from NDW or FileMan.
    ~BBFLAG_SELECTED~ Internal use only.
    ~BBFLAG_DONTDELETEOBJECT~ Internal use only.
    ~BBFLAG_DISABLED~ Internal use only.

$hNormal$ Handle to bitmap or icon of normal button.
$hSelected$ Handle to bitmap or icon of selected button.
$lpszText$ Specifies the text to be displayed on the button.
$hNormalMask$ Masks for use with BBFLAG_BITMAPS. This parameter can be NULL.
$hSelectedMask$ Masks for use with BBFLAG_BITMAPS. This parameter can be NULL.

@See: ButtonBar_AddButton
@Include: ngui.h
**************************************************************************/


#ifdef __cplusplus                      // Inlines for C++.

inline int ButtonBar_AddButton(HWND hwndCtl, LPBUTTONDATA lpbuttondata)
{
    return (int) ::SendMessage(hwndCtl, BB_ADDBUTTON, 0, (LPARAM)lpbuttondata);
}

inline BOOL ButtonBar_ResetContent(HWND hwndCtl)
{
    return (BOOL) ::SendMessage(hwndCtl, BB_RESETCONTENT, 0, 0);
}

inline BOOL ButtonBar_SetButtonSize(HWND hwndCtl, int width, int height)
{
    return (BOOL) ::SendMessage(hwndCtl, BB_SETBUTTONSIZE, (WPARAM) FALSE, MAKELPARAM(width, height));
}

inline BOOL ButtonBar_SetButtonSizeRecalc(HWND hwndCtl, int width, int height)
{
    return (BOOL) ::SendMessage(hwndCtl, BB_SETBUTTONSIZE, (WPARAM) TRUE, MAKELPARAM(width, height));
}

inline DWORD ButtonBar_GetButtonSize(HWND hwndCtl)
{
    return (DWORD) ::SendMessage(hwndCtl, BB_GETBUTTONSIZE, 0, 0);
}

inline int ButtonBar_GetButtonHeight(HWND hwndCtl)
{
    return (int) HIWORD(::SendMessage(hwndCtl, BB_GETBUTTONSIZE, 0, 0));
}

inline int ButtonBar_GetButtonWidth(HWND hwndCtl)
{
    return (int) LOWORD(::SendMessage(hwndCtl, BB_GETBUTTONSIZE, 0, 0));
}

inline BOOL ButtonBar_SetMinMaxInfo(HWND hwndCtl, _LPMINMAXINFO lpmmi)
{
    return (BOOL) ::SendMessage(hwndCtl, BB_SETMINMAXINFO, 0, (LPARAM)lpmmi);
}

inline BOOL ButtonBar_GetMinMaxInfo(HWND hwndCtl, _LPMINMAXINFO lpmmi)
{
    return (BOOL) ::SendMessage(hwndCtl, BB_GETMINMAXINFO, 0, (LPARAM)lpmmi);
}

inline BOOL ButtonBar_SetSeparatorSize(HWND hwndCtl, int size)
{
    return (BOOL) ::SendMessage(hwndCtl, BB_SETSEPARATORSIZE, (WPARAM)size, 0);
}

inline WORD ButtonBar_GetSeparatorSize(HWND hwndCtl)
{
    return (WORD) ::SendMessage(hwndCtl, BB_GETSEPARATORSIZE, 0, 0);
}

inline BOOL ButtonBar_SetBase(HWND hwndCtl, int baseID)
{
    return (BOOL) ::SendMessage(hwndCtl, BB_SETBASE, (WPARAM)baseID, 0);
}

inline BOOL ButtonBar_GetBase(HWND hwndCtl, int /* baseID */)
{
    // Why is baseID passed TO this function?  Probably too late to change now...
    return (WORD) ::SendMessage(hwndCtl, BB_GETBASE, 0, 0);
}

inline VOID ButtonBar_EnableButton(HWND hwndCtl, BOOL fSelect, int index)
{
    ::SendMessage(hwndCtl, BB_ENABLEBUTTON, (WPARAM)fSelect, MAKELPARAM(index, 0));
}

inline BOOL ButtonBar_EnableButtonID(HWND hwndCtl, BOOL fSelect, int ID)
{
    return (BOOL) ::SendMessage(hwndCtl, BB_ENABLEBUTTONID, (WPARAM)fSelect, MAKELPARAM(ID, 0));
}

inline LRESULT ButtonBar_SetItemData (HWND hwndCtl, int index, DWORD dwData)
{
    return (LRESULT)::SendMessage ((hwndCtl), BB_SETITEMDATA, (WPARAM)index, (LPARAM)dwData);
}

inline DWORD ButtonBar_GetItemData (HWND hwndCtl,int index)
{
    return (DWORD)::SendMessage ((hwndCtl), BB_GETITEMDATA, (WPARAM)index, 0L);
}

inline LRESULT ButtonBar_GetStyle (HWND hwndCtl)
{
    return (LRESULT)::SendMessage ((hwndCtl), BB_GETSTYLE, 0, 0L);
}

inline LRESULT ButtonBar_SetStyle (HWND hwndCtl, UINT uStyle)
{
    return (LRESULT)::SendMessage ((hwndCtl), BB_SETSTYLE, (WPARAM)uStyle, 0L);
}

inline int ButtonBar_GetText (HWND hwndCtl, int index, LPNGTEXTREC textrec)
{
    return (int) ::SendMessage (hwndCtl, BB_GETTEXT, (WPARAM) index, (LPARAM) textrec);
}

inline int ButtonBar_GetTextLength (HWND hwndCtl, int index)
{
    return (int) ::SendMessage (hwndCtl, BB_GETTEXTLENGTH, (WPARAM) index, 0L);
}

inline int ButtonBar_SetText (HWND hwndCtl, int index, LPNGTEXTREC textrec)
{
    return (int) ::SendMessage (hwndCtl, BB_SETTEXT, (WPARAM) index, (LPARAM) textrec);
}

inline int ButtonBar_SetNormalBmp (HWND hwndCtl, int index, HANDLE hNormal)
{
    return (int) ::SendMessage (hwndCtl, BB_SETNORMALBMP, (WPARAM) index, (LPARAM) hNormal);
}

inline int ButtonBar_SetSelectedBmp (HWND hwndCtl, int index, HANDLE hSelected)
{
    return (int) ::SendMessage (hwndCtl, BB_SETSELECTEDBMP, (WPARAM) index, (LPARAM) hSelected);
}

inline VOID ButtonBar_AutoSizeButtons (HWND hwndCtl)
{
    ::SendMessage(hwndCtl, BB_AUTOSIZEBUTTONS, 0, 0L);
}

#else                                   // Macros for C.

#define ButtonBar_AddButton(hwndCtl,lpbuttondata)      \
    (int)SendMessage((hwndCtl), BB_ADDBUTTON, 0, (LPARAM)(LPBUTTONDATA)(lpbuttondata))

#define ButtonBar_ResetContent(hwndCtl)      \
    (BOOL)SendMessage((hwndCtl), BB_RESETCONTENT, 0, (LPARAM) 0L)

#define ButtonBar_SetButtonSize(hwndCtl,width,height)      \
    (BOOL)SendMessage((hwndCtl), BB_SETBUTTONSIZE, (WPARAM) FALSE, MAKELPARAM((width),(height)))

#define ButtonBar_SetButtonSizeRecalc(hwndCtl,width,height)      \
    (BOOL)SendMessage((hwndCtl), BB_SETBUTTONSIZE, (WPARAM) TRUE, MAKELPARAM((width),(height)))

#define ButtonBar_GetButtonSize(hwndCtl)      \
    (DWORD)SendMessage((hwndCtl), BB_GETBUTTONSIZE, 0, (LPARAM) 0L)

#define ButtonBar_GetButtonHeight(hwndCtl)      \
    (int)HIWORD(SendMessage((hwndCtl), BB_GETBUTTONSIZE, 0, (LPARAM) 0L))

#define ButtonBar_GetButtonWidth(hwndCtl)      \
    (int)LOWORD(SendMessage((hwndCtl), BB_GETBUTTONSIZE, 0, (LPARAM) 0L))

#define ButtonBar_SetMinMaxInfo(hwndCtl,lpmmi)      \
    (BOOL)SendMessage((hwndCtl), BB_SETMINMAXINFO, 0, (LPARAM)(_LPMINMAXINFO)(lpmmi))

#define ButtonBar_GetMinMaxInfo(hwndCtl,lpmmi)      \
    (BOOL)SendMessage((hwndCtl), BB_GETMINMAXINFO, 0, (LPARAM)(_LPMINMAXINFO)(lpmmi))

#define ButtonBar_SetSeparatorSize(hwndCtl,size)      \
    (BOOL)SendMessage((hwndCtl), BB_SETSEPARATORSIZE, (WPARAM)(size), 0L)

#define ButtonBar_GetSeparatorSize(hwndCtl)      \
    (WORD)SendMessage((hwndCtl), BB_GETSEPARATORSIZE, 0, 0L)

#define ButtonBar_SetBase(hwndCtl,baseID)      \
    (BOOL)SendMessage((hwndCtl), BB_SETBASE, (WPARAM)(baseID), 0L)

#define ButtonBar_GetBase(hwndCtl,baseID)      \
    (WORD)SendMessage((hwndCtl), BB_GETBASE, 0, 0L)

#define ButtonBar_EnableButton(hwndCtl,fSelect,index)   \
    (void)SendMessage((hwndCtl), BB_ENABLEBUTTON, (WPARAM)(BOOL)(fSelect), MAKELPARAM((index), 0))

#define ButtonBar_SetItemData (hwndCtl,index,dwData)    \
    (LRESULT)SendMessage ((hwndCtl), BB_SETITEMDATA, (WPARAM)index, (LPARAM)dwData)

#define ButtonBar_GetItemData (hwndCtl,index)    \
    (DWORD)SendMessage ((hwndCtl), BB_GETITEMDATA, (WPARAM)index, 0L)

#define ButtonBar_GetStyle(hwndCtl) \
    (LRESULT)SendMessage ((hwndCtl), BB_GETSTYLE, 0, 0L)

#define ButtonBar_SetStyle(hwndCtl,uStyle) \
    (LRESULT)SendMessage ((hwndCtl), BB_SETSTYLE, (WPARAM)(uStyle), 0L)

#define ButtonBar_GetText(hwndCtl,index,textrec) \
    (int)SendMessage ((hwndCtl), BB_GETTEXT, (WPARAM)(index), (LPARAM)(LPNGTEXTREC)(textrec))

#define ButtonBar_GetTextLength(hwndCtl,index) \
    (int)SendMessage ((hwndCtl), BB_GETTEXTLENGTH, (WPARAM)(index), 0L)

#define ButtonBar_SetText(hwndCtl,index,textrec) \
    (int)SendMessage ((hwndCtl), BB_SETTEXT, (WPARAM)(index), (LPARAM)(LPNGTEXTREC)(textrec))
#define ButtonBar_SetNormalBmp(hwndCtl,index,hNormal) \
    (int)SendMessage ((hwndCtl), BB_SETNORMALBMP, (WPARAM)(index), (LPARAM)(hNormal));
#define ButtonBar_SetSelectedBmp(hwndCtl,index,hSelected) \
    (int)SendMessage ((hwndCtl), BB_SETSELECTEDBMP, (WPARAM)(index), (LPARAM)(hSelected));

#define ButtonBar_AutoSizeButtons(hwndCtl) \
    (void)SendMessage(hwndCtl, BB_AUTOSIZEBUTTONS, 0, 0L)

#endif


#define BBN_FOCUSSET            1       // button bar is getting focus
#define BBN_FOCUSKILL           2       // button bar is losing focus
#define BBN_MOUSEMOVE           3       // mouse is moving over a button
#define BBN_DOWN                4       // button was just pressed down
#define BBN_UP                  5       // button was just released
#define BBN_SELECT              6       // button was pressed and released
#define BBN_DROPFILES           7       // converted WM_DROPFILES message
#define BBN_TABNEXT             8       // button bar tab off next
#define BBN_TABPREV             9       // button bar tab off prev

                    // Define button flags
#define BBFLAG_SELECTED          0x0001
#define BBFLAG_BITMAPS           0x0002
#define BBFLAG_ICONS             0x0004 // not implemented
#define BBFLAG_SEPARATOR         0x0008
#define BBFLAG_TEXT              0x0010
#define BBFLAG_DONTDELETEOBJECT  0x0020
#define BBFLAG_DISABLED          0x0040
#define BBFLAG_NODROP            0x0080 // don't handle any draglist messages
#define BBFLAG_FILEDROP          0x0100 // accept NDW/FileMan drops
#define BBFLAG_NBLOB             0x0200 // Uses Nblob instead of bitmap
#define BBFLAG_HIDDEN            0x0400 // Button is hidden, don't use

// Button Bar Style Flags, for BB_GETSTYLE and BB_SETSTYLE
// ---------------------------------------------------------------------
#define BBS_WINTOOLBARMODE      0x0001
#define BBS_CENTERBUTTONS       0x0002
#define BBS_NORAISEDBKGROUND    0x0004
#define BBS_NOTIFYMOVEMENT      0x0008  // send BBN_MOUSEMOVE notifications
#define BBS_DRAGDROP            0x0010  // pass on draglist messages
#define BBS_DROPFILES           0x0020  // pass on drops from NDW/FileMan
#define BBS_NOTIFYSELECTONLY    0x0040  // only send BBN_SELECT notifications
#define BBS_REDRAW              0x0080  // allow/prevent changes from drawing
#define BBS_DRAWTEXTENABLED     0x0100  // global flag to allow drawing of text
#define BBS_DRAWBITMAPS         0x0200  // global flag to allow drawing of bmps
#define BBS_NOTIFYTAB           0x0400  // send tab notification rather than
                    // tabbing to the next button
#define BBS_CLIP                0x0800  // Don't wrap buttons, clip'em
#define BBS_TOOLTIPS            0x1000  // Support Win32 Tooltip control automatically


                    // Define default rect values
#define BDEF_BUTTON_WIDTH     25
#define BDEF_BUTTON_HEIGHT    25

#define STD_SBAR_HEIGHT                         25

#define STD_BBAR_WIDTH                          45
#define STD_BBAR_HEIGHT                         45

#define STD_BBAR_WIDTH_TEXT_ONLY                45
#define STD_BBAR_HEIGHT_TEXT_ONLY               20

#define STD_BBAR_WIDTH_BITMAP_ONLY              24
#define STD_BBAR_HEIGHT_BITMAP_ONLY             24

                    // Define owner draw type
#define ODT_BUTTONBAR         10

#define TOOL_BAR DIALOG
#define TOOL_SEPARATOR CONTROL "",0,"ToolBar",BBFLAG_SEPARATOR,0,0,0,0
#define TOOL_ITEM CONTROL
#define TOOL_DEFAULTPOS 0,0,0,0

// toolbarName TOOL_BAR TOOL_DEFAULTPOS
// STYLE BBS_CENTERBUTTONS | WS_CHILD
// CAPTION "Caption"
// BEGIN
//     TOOL_SEPARATOR
//     TOOL_ITEM "Text",1,"Bitmap|SelBitmap",BBFLAG_SEPARATOR,TOOL_DEFAULTPOS
//     etc
// END

WORD SYM_EXPORT WINAPI NG_ToolBarFill(
    HWND hWnd,
    HINSTANCE hInst,
    LPSTR lpResource

);



//------------------------------------------------------------------------
// Include code for ngdlg.c
//------------------------------------------------------------------------
/*  */
HWND SYM_EXPORT CALLBACK NG_CreateDialog(HINSTANCE hInst, LPSTR lpsz, HWND hWndParent, LPARAM lParam);
LONG SYM_EXPORT CALLBACK cbDlgKeyProc(HWND hCntrl, UINT msg, WPARAM wParam, LPARAM lParam);
BOOL SYM_EXPORT CALLBACK cbEnumCntrlsProc(HWND hCntrl, LPARAM lParam);
BOOL          NG_InitDlgKeys(HWND hDlg);
BOOL SYM_EXPORT CALLBACK cbEnumCntrlsAccelProc(HWND hCntrl, LPARAM lParam);
HWND          NG_FindCntrlAccel(HWND hDlg, WORD wChar);
void          NG_ClearBeep(void);
WORD          GetDefID(HWND hDlg);
LONG          SetDefID(HWND hDlg, WORD wID);
HWND          GetNextGroupCntrl(HWND hDlg, HWND hCntrl, WORD wDirection);

VOID SYM_EXPORT WINAPI NG_DlgInit(HINSTANCE hInst);
VOID SYM_EXPORT WINAPI NG_DlgDone(void);

LONG SYM_EXPORT CALLBACK cbNumericEditProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

/* from ngui.dll */
//LONG CALLBACK cpDlgProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

/*  */
#define INRANGE(a,x,b) (((x)>=(a)) && ((x)<=(b)))





//------------------------------------------------------------------------
// Definitions from CPALETTE
//------------------------------------------------------------------------
#if defined( __cplusplus )
extern "C" {
#endif  // #if defined( __cplusplus )

#define NG_3DLIGHT_COLOR        (GetSysColor(COLOR_BTNHIGHLIGHT))
#define NG_3DDARK_COLOR         (GetSysColor(COLOR_BTNSHADOW))
#define NG_STATIC_FONTCOLOR     (RGB(0,0,0))
#define NG_STATIC_HILITECOLOR   (RGB(255,255,255))

//#define DEF_BUTTON_BRUSH        2   /* Button background brush         */
//#define DEF_BUTTON_BMUP         3   /* Normal, unfocused button bitmap */
//#define DEF_BUTTON_BMFOCUSUP    4   /* Unpressed, focused button bitmap*/
//#define DEF_BUTTON_BMFOCUSDN    5   /* Pressed, focused button bitmap  */
//#define DEF_BUTTON_BMDISABLED   6   /* Disabled button bitmap          */
//#define DEF_CHBOX_BMEMPTY       7   /* Empty check box bitmap          */
//#define DEF_CHBOX_BMCHECK       8   /* Check check box bitmap          */
//#define DEF_CHBOX_BMFRAMEEMPTY  9   /* Frame check box bitmap          */
//#define DEF_CHBOX_BMFRAMECHECK  10  /* Framed check check box bitmap   */
//#define DEF_CHBOX_BMDISABLEDEMPTY   11  /* Disabled empty check box    */
//#define DEF_CHBOX_BMDISABLEDCHECK   12  /* Disabled check check box    */
//#define DEF_CHBOX_FONT          13  /* Check box font                  */
//#define DEF_CHBOX_FONTCOLOR     14  /* Check box font color            */
//#define DEF_CHBOX_BRUSH         15  /* Check box background brush      */
//#define DEF_RADIO_BMEMPTY       16  /* Empty radio button bitmap       */
//#define DEF_RADIO_BMCHECK       17  /* Check radio button bitmap       */
//#define DEF_RADIO_BMFRAMEEMPTY  18  /* Frame radio button bitmap       */
//#define DEF_RADIO_BMFRAMECHECK  19  /* Framed check radio button bitmap*/
//#define DEF_RADIO_FONT          20  /* Radio button font               */
//#define DEF_RADIO_FONTCOLOR     21  /* Radio button font color         */
//#define DEF_RADIO_BRUSH         22  /* Radio button background brush   */
//#define DEF_METER_FGCOLOR       23  /* Meter remaining area color      */
//#define DEF_METER_BGCOLOR       24  /* Meter progress bar color        */
//#define DEF_SPIN_BMTOPUP        25  /* Vertical spin upper bitmap      */
//#define DEF_SPIN_BMTOPDOWN      26  /* Vertical spin upper depressed bm*/
//#define DEF_SPIN_BMBOTUP        27  /* Vertical spin lower bitmap      */
//#define DEF_SPIN_BMBOTDOWN      28  /* Vertical spin lower depressed bm*/
//#define DEF_SPIN_BMLEFTUP       29  /* Horizontal spin left bitmap     */
//#define DEF_SPIN_BMLEFTDOWN     30  /* Horizontal spin lft depressed bm*/
//#define DEF_SPIN_BMRIGHTUP      31  /* Horizontal spin right bitmap    */
//#define DEF_SPIN_BMRIGHTDOWN    32  /* Horizontal spin rt depressed bm */
//#define DEF_CANVAS_BRUSH        33  /* Canvas background brush         */
//#define DEF_STATIC_BRUSH        34  /* Static background brush         */
//#define DEF_STATIC_BITMAP       35  /* Static bitmap                   */
//#define DEF_STATIC_ICON         36  /* Static icon                     */
//#define DEF_STATIC_FONT         37  /* Static font                     */
//#define DEF_STATIC_FONTCOLOR    38  /* Static font color               */
//#define DEF_STATIC_HILITECOLOR  39  /* Static font highlight color     *
//#define DEF_TOOLBAR_BRUSH       40  /* Tool bar border brush           */
//#define DEF_TOOLBAR_HILITEBRUSH 41  /* Tool bar bitmap background brush*/
//#define DEF_DIALOG_BRUSH        42  /* Dialog box background brush     */
//#define DEF_LENGTH              43  /* Size of default attribute table */


/* Custom messages */

#define WM_CTLATTRIBS     (WM_USER + 0x0BC0)    /* Notification        */
#define CPM_FPENABLE      (WM_USER + 0x0BC8)    /* Command             */
#define CPM_RETFPSTYLE    (WM_USER + 0x0BE0)    /* Command             */


/* Tool bar messages */

#define CPM_TBCLICKED     (WM_USER + 0x0BC9)    /* Notification        */
#define CPM_TBSETSELECT   (WM_USER + 0x0BCA)    /* Command             */
#define CPM_TBGETSELECT   (WM_USER + 0x0BCB)    /* Command             */

/* Meter control messages */

#define CPM_MTSETLIMIT    (WM_USER + 0x0BC8)    /* Command             */
#define CPM_MTGETLIMIT    (WM_USER + 0x0BC9)    /* Command             */
#define CPM_MTSETCURRENT  (WM_USER + 0x0BCA)    /* Command             */
#define CPM_MTGETCURRENT  (WM_USER + 0x0BCB)    /* Command             */

/* Spin button control messages */

#define CPM_SPCYCLE       (WM_USER + 0x0BD8)    /* Notification        */
#define CPM_SPPERIOD      (WM_USER + 0x0BD9)    /* Command             */


/* New WM_CTLCOLOR control types:                                      */
/* Note that in Control Palette there is no such control as a          */
/* CTLCOLOR_BTN.  Instead, the various styles of the standard Windows  */
/* Button control have been separated into Radio Button, Check Box,    */
/* Push Button and Canvas controls.                                    */

#define CTLCOLOR_CANVAS     100
#define CTLCOLOR_METER      101
#define CTLCOLOR_RADIOBTN   102
#define CTLCOLOR_CHECKBOX   103
#define CTLCOLOR_PUSHBTN    104
#define CTLCOLOR_SPIN       105


/* Radio Button & Check Box states                                     */

#define BS_EMPTY            0       /* Not selected.                   */
#define BS_CHECK            1       /* Selected.                       */
#define BS_FRAMEEMPTY       2       /* Not selected/hilighted.         */
#define BS_FRAMECHECK       3       /* Selected/highlighted.           */
#define BS_DISABLEDEMPTY    4       /* Disabled/empty.                 */
#define BS_DISABLEDCHECK    5       /* Disabled/selected.              */


/* Push Button states */

#define PBS_UP          0           /* Normal button state.            */
#define PBS_FOCUSDOWN   1           /* Button pressed.                 */
#define PBS_FOCUSUP     2           /* Focused state.                  */
#define PBS_DISABLED    3           /* Disabled state.                 */


/* Spin Button states */

#define SPS_UP          0           /* Spin button up                  */
#define SPS_DOWN        1           /* Spin button down                */

/* Spin Button location */

#define SPB_INCREASE    0           /* Increasing spin button          */
#define SPB_DECREASE    1           /* Decreasing spin button          */
#define SPB_NOCHANGE    2           /* Make no change to spin count    */


/* Tool Bar states */

#define TBS_UP          0           /* Up or deslected state.          */
#define TBS_DOWN        1           /* Down or selected state.         */


/* Meter states   */

#define MTS_FG          0           /* Drawing foreground.             */
#define MTS_BG          1           /* Drawing background.             */


/* Canvas control styles */

#define CVS_FLAT           0x0000L       /* Flush canvas surface.           */
#define CVS_FRAMED         0x0001L       /* Black framed canvas.            */
#define CVS_LOWERED        0x0002L       /* Indented canvas surface.        */
#define CVS_RAISED         0x0004L       /* Raised canvas surface.          */
#define CVS_HORIZ          0x0008L       /* Horizontal grooved line.        */
#define CVS_VERT           0x0010L       /* Vertical grooved line.          */
#define CVS_FAST           0x0080L       /* Enable fast canvas painting.    */

                    // Add style for sub-classed controls
                    // (Excel 4.0 look on edit & listbox fields)
#define CVS_SUNKENCONTROL  0x0100L



/* Static control styles */
#define SS_FLATTEXT     0x0000L     /* Flush text.                     */
#define SS_LOWEREDTEXT  0x0010L     /* Indented text.                  */
#define SS_RAISEDTEXT   0x0020L     /* Raised text.                    */
#define SS_GRAYEDTEXT   0x0040L     /* "Disabled" text                 */
#define SS_BOTTOM       0x0800L     /* Text aligned with bottom.       */
#define SS_VCENTER      0x1000L     /* Text aligned vertical center.   */
#define SS_SINGLELINE   0x2000L     /* Text displayed on one line.     */
#define SS_EXPANDTABS   0x4000L     /* Tab characters are expanded.    */

/* Meter control styles  */

#define MTS_FLAT        0x00L       /* Meter displays on surface.      */
#define MTS_FRAMED      0x01L       /* Black frame surrounds meter.    */
#define MTS_LOWERED     0x02L       /* Meter displays indented.        */
#define MTS_RAISED      0x04L       /* Meter displays above surface.   */
#define MTS_HORIZ       0x00L       /* Horizontal rectangle.           */
#define MTS_VERT        0x08L       /* Vertical rectangle.             */
#define MTS_USERTEXT    0x10L       /* Display user text in meter.     */


/* Spin button control styles */

#define SPS_VERT        0x00L       /* Vertical spin buttons           */
#define SPS_HORIZ       0x01L       /* Horizontal spin buttons         */


/* WM_CTLATTRIBS control types */

#define CTLATTRIB_STATIC    0       /* Static control                  */
#define CTLATTRIB_PUSHBTN   1       /* Push Button control             */
#define CTLATTRIB_CHECKBOX  2       /* Check Box control               */
#define CTLATTRIB_RADIO     3       /* Radio Button control            */
#define CTLATTRIB_CANVAS    4       /* Canvas control                  */
#define CTLATTRIB_DIALOG    5       /* Dialog box                      */
#define CTLATTRIB_TOOLBAR   6       /* Tool bar control                */
#define CTLATTRIB_METER     7       /* Meter control                   */
#define CTLATTRIB_SPIN      8       /* Spin button control             */


#if defined( __cplusplus )
}
#endif  // #if defined( __cplusplus )


//------------------------------------------------------------------------
// Dialog Control Definitions
//------------------------------------------------------------------------
#define DC_MOVE_UPDOWN     0x0001
#define DC_MOVE_RIGHTLEFT  0x0002
#define DC_STRETCH_UP      0x0010
#define DC_STRETCH_DOWN    0x0020
#define DC_STRETCH_LEFT    0x0040
#define DC_STRETCH_RIGHT   0x0080

/*@STRUCTAPI:***************************************************************/
typedef struct tagDLGCONTROL {
   WORD     wID;                        // Control ID
   WORD     wFlags;                     // Control Flags
} DLGCONTROL, * PDLGCONTROL, FAR * LPDLGCONTROL;
/*****
@Name: DLGCONTROL

@Description:
This structure contains information about a dialog control.

@Members:
$wID$ Specifies the ID of the control.

$wFlags$ Specifies information on the size and location of a control. It can be
one of the following values: [[confirm]]
    ~DC_MOVE_UPDOWN~ Move the control up or down.
    ~DC_MOVE_RIGHTLEFT~ Move the control left or right.
    ~DC_STRETCH_UP~ Allow the control to be enlarged upwards.
    ~DC_STRETCH_DOWN~ Allow the control to be enlarged downwards.
    ~DC_STRETCH_LEFT~ Allow the control to be enlarged to the left.
    ~DC_STRETCH_RIGHT~ Allow the control to be enlarged to the right.

@Include: ngui.h
****************************************************************************/



//------------------------------------------------------------------------
// Function Prototypes
//------------------------------------------------------------------------

VOID SYM_EXPORT WINAPI NG_DlgControlSize (
   HWND         hDlg,                   // [in]    Dialog Window Handle
   LPDLGCONTROL lpControls,             // [in]    Control List
   WORD         wWidthPrev,             // [in]    Previous Width of box
   WORD         wHeightPrev,            // [in]    Previous Height of box
   WORD         wWidthNew,              // [in]    New Width of box
   WORD         wHeightNew              // [in]    New Height of box
);

/************************************************************************/
/* Slider Control                                                       */
/************************************************************************/
HWND SYM_EXPORT WINAPI NG_CreateSlider(HWND hWndParent,
                    int X, int Y,
                    int nWidth, int nHeight);
#define SLIDER_CLASSNAME QCLASS_SLIDER

#define SYMGUI_SLIDER_MSG "Symgui Slider Msgs"
#define SLIDER_STATUS_PCT  1

#define UWM_SL_GET_SLIDE_POS    WM_USER + 100
#define UWM_SL_SET_WNDPOS       WM_USER + 101
#define UWM_SL_SET_WNDSIZE      WM_USER + 102
#define UWM_SL_SET_RANGE        WM_USER + 103
#define UWM_SL_SET_LEVEL        WM_USER + 104
#define UWM_SL_SHOW             WM_USER + 105
#define UWM_SL_GET_STYLE        WM_USER + 106
#define UWM_SL_SET_STYLE        WM_USER + 107

#define SLS_FOCUSRECT           0x00000001L
#define SLS_FRAMERECT           0x00000002L
#define SLS_LED                 0x00000004L

#define SLS_STANDARD            (SLS_FOCUSRECT | SLS_FRAMERECT | SLS_LED)

#define Slider_SetRange(hwnd, min, max)   \
    (VOID)SendMessage((hwnd), UWM_SL_SET_RANGE, min, \
                        (LPARAM)(DWORD)(max))
#define Slider_GetSlidePos(hwnd) \
    (DWORD)SendMessage((hwnd), UWM_SL_GET_SLIDE_POS, 0, 0L)
#define Slider_SetLevel(hwnd, level)      \
    (VOID)SendMessage((hwnd), UWM_SL_SET_LEVEL, 0, (LPARAM)(DWORD)(level))
#define Slider_SetWndSize(hwnd, x, y)                     \
    (VOID)SendMessage((hwnd), UWM_SL_SET_WNDSIZE, 0, MAKELPARAM(x, y))
#define Slider_SetWndPos(hwnd, x, y)             \
    (VOID)SendMessage((hwnd), UWM_SL_SET_WNDPOS, 0, MAKELPARAM(x, y))
#define Slider_Show(hwnd, SliderStyle)            \
    (VOID)SendMessage((hwnd), UWM_SL_SHOW, 0, \
              (LPARAM)(LPSTR)(SliderStyle))
#define Slider_StyleGet(hWnd) \
    (VOID)SendMessage((hWnd), UWM_SL_GET_STYLE, 0, 0L)
#define Slider_StyleSet(hWnd, style) \
    (VOID)SendMessage((hWnd), UWM_SL_SET_STYLE, 0, style)

/*@STRUCTAPI:*************************************************************/
typedef struct
{
    HINSTANCE   hInstance;
    LPSTR       lpSliderBtnBmp;
    LPSTR       lpSliderBtnBmpFocus;
    WORD        wGrooveWidth;
    WORD        wBuiltinBtnStyle;
    BOOL        fDisableFocusRect:1;
    BOOL        fDisableFrameRect:1;
    BOOL        fDisableLED:1;
}
SLIDERSTYLE;
typedef SLIDERSTYLE NEAR *NPSLIDERSTYLE;
typedef SLIDERSTYLE FAR  *LPSLIDERSTYLE;
/****
@Name: SLIDERSTYLE

@Description:
This structure is used by the Slider_Show function (or the UWM_SL_SHOW
message) to determine visual characteristics of the slider.

@Members:
$hInstance$ Specifies the instance for res bmps. If this parameter is NULL the
lpSliderBtnBmp and lpSliderBtnBmpFocus parameters are strings containing file
names on disk. Otherwise they should be assigned values using the
MAKEINTRESOURCE macro.

$lpSliderBtnBmp$ Specifies the slider button bitmap as described previously. The
default is used if this is NULL.

$lpSliderBtnBmpFocus$ Specifies the slider button bitmap when
the button has the focuse as described previously. The
default is used if this is NULL.

$wGrooveWidth$ Specifies the groove width on the sides of the button. The
default value is used if this is NULL.

$wBuiltinBtnStyle$ Specifies the buttons' style. It can be any of the
following values or 0 (indicating to use the default): [[need more info]]
    ~SL_BTN_UP_PTR~ Specifies a built-in slider button style.
    ~SL_BTN_DN_PTR~ Specifies a built-in slider button style.
    ~SL_BTN_RT_PTR~ Specifies a built-in slider button style.
    ~SL_BTN_LT_PTR~ Specifies a built-in slider button style.
    ~SL_BTN_UP_PTR_SHOW_FOCUS~ Indicates that the slider has the focus.
    ~SL_BTN_DN_PTR_SHOW_FOCUS~ Indicates that the slider has the focus.
    ~SL_BTN_RT_PTR_SHOW_FOCUS~ Indicates that the slider has the focus.
    ~SL_BTN_LT_PTR_SHOW_FOCUS~ Indicates that the slider has the focus.

$fDisableFocusRect$ Set to TRUE to disable the focuse rectangle.
$fDisableFrameRect$ Set to TRUE to disable the frame rectangle.
$fDisableLED$ Set to TRUE to disable the LED.

@Include: ngui.h
*************************************************************************/
#define SL_BTN_UP_PTR              1    // Builtin slider button styles
#define SL_BTN_DN_PTR              2    // Builtin slider button styles
#define SL_BTN_RT_PTR              3    // Builtin slider button styles
#define SL_BTN_LT_PTR              4    // Builtin slider button styles
#define SL_BTN_UP_PTR_SHOW_FOCUS   5    // These styles indicate
#define SL_BTN_DN_PTR_SHOW_FOCUS   6    // when the Slider button
#define SL_BTN_RT_PTR_SHOW_FOCUS   7    // has focus
#define SL_BTN_LT_PTR_SHOW_FOCUS   8    //

/************************************************************************/
/* Fillbar stuff starts here                                            */
/************************************************************************/
/************************************************************************/
/* Messages                                                             */
/************************************************************************/
#define UWM_FB_SET_POSITION   WM_USER + 10
#define UWM_FB_SET_SIZE       WM_USER + 15
#define UWM_FB_SET_FILL_COLOR WM_USER + 20
#define UWM_FB_SET_FILL_TITLE WM_USER + 25
#define UWM_FB_SET_FILL_TEXT1 WM_USER + 30
#define UWM_FB_SET_FILL_TEXT2 WM_USER + 35
#define UWM_FB_SET_FILL_RANGE WM_USER + 40
#define UWM_FB_SET_FILL_LEVEL WM_USER + 45
#define UWM_FB_START_FILLBAR  WM_USER + 50
#define UWM_FB_CLOSE_FILLBAR  WM_USER + 55
#define UWM_FB_HIDE_FILLBAR   WM_USER + 60
#define UWM_FB_SHOW_FILLBAR   WM_USER + 65
#define UWM_FB_REINIT_FILLBAR WM_USER + 70
#define UWM_FB_SET_PCT_MSG    WM_USER + 75
#define UWM_FB_BK_DEPRESSED   WM_USER + 80
#define UWM_FB_SET_LEVEL_ULONG WM_USER + 85
#define UWM_FB_SET_RANGE_ULONG WM_USER + 90
#define UWM_FB_SCALE_FONT     WM_USER + 91
#define UWM_FB_SET_FONT       WM_USER + 92
#define UWM_FB_DBLCLK_FONTDLG WM_USER + 93
#define UWM_FB_SHOW_FONTDLG   WM_USER + 94
#define UWM_FB_FONTCREATE_SHOWBUSY WM_USER + 95
#define UWM_FB_SET_TEXT_ALIGNMENT WM_USER + 96
#define UWM_FB_GET_TEXT       WM_USER + 97
#define UWM_FB_GET_FONT       WM_USER + 98
#define UWM_FB_GET_FILL_COLOR WM_USER + 99

#define FB_USEDEFAULT         0

#define FBS_CENTER_TEXT         0x0000  // center text
#define FBS_LEFT_TEXT           0x0001  // left text
#define FBS_3D_LOOK             0x0002  // three-dimensional bar

                                        // ----------------------------------
                                        // For 32-bit windows apps use Win95
                                        // progress bar.
                                        // ----------------------------------
#ifdef SYM_WIN32
    #define FILLBAR_CLASSNAME   PROGRESS_CLASS
#else
    #ifndef FILLBAR_CLASSNAME
        #define FILLBAR_CLASSNAME       QCLASS_FILLBAR
    #endif
#endif

/************************************************************************/
/* Typedefs                                                             */
/************************************************************************/

/*@STRUCTAPI:*************************************************************/
typedef struct
{
  int  nStruclen;
  BOOL b3D;
  int  nLeftTopWidth;
  int  nRightBottomWidth;
  COLORREF crColor;
  BYTE byTextJust;
} FILLBARSTYLE, NEAR *NPFILLBARSTYLE, FAR  *LPFILLBARSTYLE;
/****
@Name: FILLBARSTYLE

@Description:
The FILLBARSTYLE structure is used with the Fillbar_Show function.

@Members:
$nStruclen$ Contains the size of this FILLBARSTYLE structure.

$b3D$ Set to TRUE for a 3D look, FALSE for normal.

$nLeftTopWidth$ Contains dimension of left, top 3D border. If this value is
set to FB_USEDEFAULT, the left/top width of the fill bar is set to 1
pixel. This value is ignored if the b3D field is set to FALSE.

$nRightBottomWidth$ Contains dimension of right, bottom 3D border. If this
value is set to FB_USEDEFAULT,  the right/bottom width of the fill bar will be
set to  2 pixels. This value is ignored if the b3D field is set to FALSE.

$crColor$ contains an RGB COLORREF,  which is used to set the fill bar color.
If the fill bar is 2D, this value will be used to paint both the fill color and
the fill bar text. If the fill bar is 3D, this value is used to paint the text
only. If this value is set to FB_USEDEFAULT, the fill bar color will be set to
RGB(0, 0, 255), i.e pure blue.

$byTextJust$ Indicates how to justify fill bar text. It can be one of the
following values:
    ~FB_TEXT_CENTER~ To center the text.
    ~FB_TEXT_LEFT~ To left justify the text.

@See: Fillbar_Show
@Include: ngui.h
**************************************************************************/

#define FB_TEXT_CENTER          0
#define FB_TEXT_LEFT            1

/************************************************************************/
/* function definitions                                                 */
/************************************************************************/
HWND SYM_EXPORT WINAPI NG_CreateFillBar(HWND hWndParent,
                    int X, int Y,
                    int nWidth, int nHeight);

/***********************************************************************/
/* Fillbar Utility Function                                            */
/***********************************************************************/
WORD SYM_EXPORT WINAPI NG_3DRect(HWND hWnd,
            HDC hDC,
            LPRECT lpRect,
            int nLeftTopWidth,
            int nRightBottomWidth);

#ifdef  __cplusplus

inline void Fillbar_SetPercentMsg(HWND hwnd, BOOL bExtTextOut, LPCSTR message)
    {
    ::SendMessage(hwnd, UWM_FB_SET_PCT_MSG, (WPARAM) bExtTextOut, (LPARAM) message);
    }

#else

#define Fillbar_SetPercentMsg(hwnd, bExtTextOut, message)            \
        (VOID)SendMessage((hwnd),                        \
                    UWM_FB_SET_PCT_MSG,          \
                    (WPARAM)(WORD)(bExtTextOut), \
                    (LPARAM)(LPSTR)(message))

#endif

/***********************************************************************/
/* Message Cracker Api's                                               */
/***********************************************************************/
#ifndef _FILLBAR_H

#ifdef SYM_WIN32
// --------------------------------------------------------------------------
//  32-bit fillbar defines start here.
// --------------------------------------------------------------------------

#define Fillbar_SetSize(hwnd, x, y) \
    SetWindowPos(hwnd,NULL,0,0,x,y,SWP_NOMOVE);

#define Fillbar_SetPosition(hwnd, x, y) \
    SetWindowPos(hwnd,NULL,x,y,0,0,SWP_NOSIZE);

#define Fillbar_SetRange(hwnd, min, max) \
    (VOID)SendMessage((hwnd), PBM_SETRANGE, 0, MAKELPARAM(min, max))

#define Fillbar_SetRangeUlong(hwnd, min, max) \
    ( (void)0 )

#define Fillbar_DblClkFontDlg(hwnd, bEnable) \
    ( (void)0 )

#define Fillbar_ScaleFont(hwnd, bEnable) \
    ( (void)0 )

#define Fillbar_FontCreateShowBusy(hwnd, bEnable) \
    ( (void)0 )

#define Fillbar_SetFont(hwnd, lpLogFont) \
    ( (void)0 )

#define Fillbar_GetFont(hwnd, lpLogFont) \
    ( (void)0 )

#define Fillbar_SetLevel(hwnd, level) \
    (VOID)SendMessage((hwnd), PBM_SETPOS, (WPARAM)(level), 0L)

#define Fillbar_SetLevelUlong(hwnd, level) \
    ( (void)0 )

#define Fillbar_SetColor(hwnd, rgbvalue) \
    ( (void)0 )

#define Fillbar_GetColor(hwnd) \
    ( (void)0 )

#define Fillbar_Reinitialize(hwnd) \
    (VOID)SendMessage((hwnd), PBM_SETPOS, 0, 0L)

#define Fillbar_ShowFontDlg(hwnd) \
    ( (void)0 )

#define Fillbar_Show(hwnd, fillbarStyle) \
    (VOID)ShowWindow(hwnd, SW_SHOWNORMAL)

#define Fillbar_Hide(hwnd) \
    (VOID)ShowWindow(hwnd, SW_HIDE)

#define Fillbar_BkGndDepressed(hwnd, depressed) \
    ( (void)0 )

#define Fillbar_SetTitle(hwnd, title) \
    ( (void)0 )

#define Fillbar_GetText(hwnd, buflen, buf) \
    ( (void)0 )

#define Fillbar_SetText1(hwnd, text) \
    ( (void)0 )

#define Fillbar_SetText2(hwnd, text) \
    ( (void)0 )

#define Fillbar_SetTextAlignment(hwnd, align) \
    ( (void)0 )

#define Fillbar_Close(hwnd) \
    ( (void)0 )

#define Fillbar_Start(hwnd) \
    ( (void)0 )

#else

// --------------------------------------------------------------------------
//  16-bit fillbar defines start here.
// --------------------------------------------------------------------------

#define Fillbar_SetSize(hwnd, x, y)                     \
    (VOID)SendMessage((hwnd), UWM_FB_SET_SIZE, 0, MAKELPARAM(x, y))

#define Fillbar_SetPosition(hwnd, x, y)             \
    (VOID)SendMessage((hwnd), UWM_FB_SET_POSITION, 0, MAKELPARAM(x, y))

#define Fillbar_SetRange(hwnd, min, max)            \
    (VOID)SendMessage((hwnd), UWM_FB_SET_FILL_RANGE, 0, MAKELPARAM(min, max))

#define Fillbar_SetRangeUlong(hwnd, min, max)   \
    (VOID)SendMessage((hwnd), UWM_FB_SET_RANGE_ULONG, min, \
                        (LPARAM)(DWORD)(max))

#define Fillbar_DblClkFontDlg(hwnd, bEnable)    \
    (VOID)SendMessage((hwnd), UWM_FB_DBLCLK_FONTDLG, (WPARAM)(bEnable), 0L)

#define Fillbar_ScaleFont(hwnd, bEnable)                \
    (VOID)SendMessage((hwnd), UWM_FB_SCALE_FONT, (WPARAM)(bEnable), 0L)

#define Fillbar_FontCreateShowBusy(hwnd, bEnable) \
    (VOID)SendMessage((hwnd), UWM_FB_FONTCREATE_SHOWBUSY, \
                    (WPARAM)(bEnable), 0L)

#define Fillbar_SetFont(hwnd, lpLogFont)        \
    (VOID)SendMessage((hwnd), UWM_FB_SET_FONT, 0, (LPARAM)(LPSTR)(lpLogFont))

#define Fillbar_GetFont(hwnd, lpLogFont)        \
    (VOID)SendMessage((hwnd), UWM_FB_GET_FONT, 0, (LPARAM)(LPSTR)(lpLogFont))

#define Fillbar_SetLevel(hwnd, level)               \
    (VOID)SendMessage((hwnd), UWM_FB_SET_FILL_LEVEL, (WPARAM)(level), 0L)

#define Fillbar_SetLevelUlong(hwnd, level)      \
    (VOID)SendMessage((hwnd), UWM_FB_SET_LEVEL_ULONG, 0, \
                        (LPARAM)(DWORD)(level))

#define Fillbar_SetColor(hwnd, rgbvalue)            \
    (VOID)SendMessage((hwnd), UWM_FB_SET_FILL_COLOR, 0, (LPARAM)(rgbvalue))

#define Fillbar_GetColor(hwnd)                  \
    (COLORREF)SendMessage((hwnd), UWM_FB_GET_FILL_COLOR, 0, 0L)

#define Fillbar_Reinitialize(hwnd)                      \
    (VOID)SendMessage((hwnd), UWM_FB_REINIT_FILLBAR, 0, 0L)

#define Fillbar_ShowFontDlg(hwnd)                       \
    (VOID)SendMessage((hwnd), UWM_FB_SHOW_FONTDLG, 0, 0L)

#define Fillbar_Show(hwnd, fillbarStyle)            \
    (VOID)SendMessage((hwnd), UWM_FB_SHOW_FILLBAR, 0, \
              (LPARAM)(LPSTR)(fillbarStyle))

#define Fillbar_Hide(hwnd)                                  \
    (VOID)SendMessage((hwnd), UWM_FB_HIDE_FILLBAR, 0, 0L)

#define Fillbar_BkGndDepressed(hwnd, depressed) \
    (VOID)SendMessage((hwnd), UWM_FB_BK_DEPRESSED, \
            (WPARAM)(WORD)(depressed), 0L)

#define Fillbar_SetTitle(hwnd, title)               \
    (VOID)SendMessage((hwnd), UWM_FB_SET_FILL_TITLE, 0, \
            (LPARAM)((LPSTR)(title)))

#define Fillbar_GetText(hwnd, buflen, buf)                      \
    (VOID)SendMessage((hwnd), UWM_FB_GET_TEXT,          \
            (WPARAM)(UINT)(buflen), (LPARAM)((LPSTR)(buf)))

#define Fillbar_SetText1(hwnd, text)                \
    (VOID)SendMessage((hwnd), UWM_FB_SET_FILL_TEXT1, \
            0, (LPARAM)((LPSTR)(text)))

#define Fillbar_SetText2(hwnd, text)                \
    (VOID)SendMessage((hwnd), UWM_FB_SET_FILL_TEXT2, 0, \
            (LPARAM)((LPSTR)(text)))

#define Fillbar_SetTextAlignment(hwnd, align)               \
    (VOID)SendMessage((hwnd), UWM_FB_SET_TEXT_ALIGNMENT, (WPARAM)(align), 0L)
#define Fillbar_Close(hwnd)                                 \
    (VOID)SendMessage((hwnd), UWM_FB_CLOSE_FILLBAR, 0, 0L)

#define Fillbar_Start(hwnd)                                 \
    (VOID)SendMessage((hwnd), UWM_FB_START_FILLBAR, 0, 0L)

#endif // #ifdef SYM_WIN32
#endif // #ifndef _FILLBAR_H

#ifdef SYM_WIN32 //[
#ifndef EXTERN_C //[
#ifdef __cplusplus //[
#define EXTERN_C extern "C"
#else //][
#define EXTERN_C extern
#endif //]
#endif //]
EXTERN_C void GradientSubclassWnd ( HWND hWnd, HICON hIcon );
#endif //]

/************************************************************************/
/* SpinButton stuff starts here                                         */
/************************************************************************/

/************************************************************************/
/* Global constants                                                     */
/************************************************************************/

#ifndef SPN_BUTTON_CLASS
#define SPN_BUTTON_CLASS        QCLASS_SPN_BUTTON
#define SPN_SCROLL_CLASS        QCLASS_SPN_SCROLL
#endif

#define SB_ERR                  WM_USER+1
#define SB_ADDSTRING            WM_USER+2
#define SB_DELETESTRING         WM_USER+3
#define SB_FINDSTRING           WM_USER+4
#define SB_GETCOUNT             WM_USER+5
#define SB_GETCURSEL            WM_USER+6
#define SB_GETSPNTEXT           WM_USER+7
#define SB_GETSPNTEXTLEN        WM_USER+8
#define SB_INSERTSTRING         WM_USER+9
#define SB_RESETCONTENT         WM_USER+10
#define SB_SETCURSEL            WM_USER+11
#define SB_LIMITTEXT            WM_USER+12
#define SB_INITTYPE             WM_USER+13
#define SB_SETRANGE             WM_USER+14
#define SB_FINDNUM              WM_USER+15
#define SB_GETSPNNUM            WM_USER+16
#define SB_SETINCREMENT         WM_USER+17

#if 0
#define SB_ERR                  CB_ERR
#define SB_ADDSTRING            CB_ADDSTRING
#define SB_DELETESTRING         CB_DELETESTRING
#define SB_FINDSTRING           CB_FINDSTRING
#define SB_GETCOUNT             CB_GETCOUNT
#define SB_GETCURSEL            CB_GETCURSEL
#define SB_GETSPNTEXT           CB_GETLBTEXT
#define SB_GETSPNTEXTLEN        CB_GETLBTEXTLEN
#define SB_INSERTSTRING         CB_INSERTSTRING
#define SB_RESETCONTENT         CB_RESETCONTENT
#define SB_SETCURSEL            CB_SETCURSEL
#define SB_LIMITTEXT            CB_LIMITTEXT
// below are the not so standard actions
#define SB_INITTYPE             (CB_SETCURSEL + 1)
#define SB_SETRANGE             (CB_SETCURSEL + 2)
#define SB_FINDNUM              (CB_SETCURSEL + 3)
#define SB_GETSPNNUM            (CB_SETCURSEL + 4)
#define SB_SETINCREMENT         (CB_SETCURSEL + 5)
#endif

// below are the notifications via WM_COMMAND to parent
#define SBN_KILLFOCUS           CBN_KILLFOCUS
#define SBN_SELCHANGE           CBN_SELCHANGE
#define SBN_SETFOCUS            CBN_SETFOCUS

#define SPN_TYPE_NUMBERS        0x0001
#define SPN_TYPE_STRINGS        0x0002
#define SPN_TYPE_NUMBER_HEX     0x0004
#define SPN_NOHIDETEXT          0x1000
#define SPN_NOWRAP              0x2000
#define SPN_NOEDIT              0x4000

#ifdef  __cplusplus

inline BOOL SpinButton_Enable(HWND hwnd, BOOL fEnable)
    {
    return ::EnableWindow(hwnd, fEnable);
    }

inline int SpinButton_AddString(HWND hwnd, LPCSTR lpsz)
    {
    return (int) (DWORD) ::SendMessage(hwnd, SB_ADDSTRING, 0, (LPARAM) lpsz);
    }

inline int SpinButton_DeleteString(HWND hwnd, int index)
    {
    return (int) (DWORD) ::SendMessage(hwnd, SB_DELETESTRING, (WPARAM) index, 0L);
    }

inline int SpinButton_FindString(HWND hwnd, int indexStart, LPCSTR lpszFind)
    {
    return (int) (DWORD) ::SendMessage(hwnd, SB_FINDSTRING, (WPARAM) indexStart, (LPARAM) lpszFind);
    }

inline void SpinButton_LimitText(HWND hwnd, WORD cchMax)
    {
    ::SendMessage(hwnd, SB_LIMITTEXT, (WPARAM) cchMax, 0L);
    }

inline int SpinButton_GetCount(HWND hwnd)
    {
    return (int) (DWORD) ::SendMessage(hwnd, SB_GETCOUNT, 0, 0L);
    }

inline int SpinButton_GetCurSel(HWND hwnd)
    {
    return (int) (DWORD) ::SendMessage(hwnd, SB_GETCURSEL, 0, 0L);
    }

inline int SpinButton_GetSpnText(HWND hwnd, int index, LPSTR lpsz)
    {
    return (int) (DWORD) ::SendMessage(hwnd, SB_GETSPNTEXT, (WPARAM) index, (LPARAM) lpsz);
    }

inline int SpinButton_GetTextLength(HWND hwnd, int index)
    {
    return (int) (DWORD) ::SendMessage(hwnd, SB_GETSPNTEXTLEN, (WPARAM) index, 0L);
    }

inline int SpinButton_InsertString(HWND hwnd, int index, LPCSTR lpsz)
    {
    return (int) (DWORD) ::SendMessage(hwnd, SB_INSERTSTRING, (WPARAM) index, (LPARAM) lpsz);
    }

inline void SpinButton_ResetContent(HWND hwnd, int mode, LPCSTR lpsz)
    {
    ::SendMessage(hwnd, SB_RESETCONTENT, (WPARAM) mode, (LPARAM) lpsz);
    }

inline int SpinButton_SetCurSel(HWND hwnd, int index)
    {
    return (int) (DWORD) ::SendMessage(hwnd, SB_SETCURSEL, (WPARAM) index, 0L);
    }

inline int SpinButton_SetRange(HWND hwnd, int low, int high)
    {
    return (int) (DWORD) ::SendMessage(hwnd, SB_SETRANGE, 0, MAKELPARAM((WORD) low, (WORD) high));
    }

inline int SpinButton_SetIncrement(HWND hwnd, int incr)
    {
    return (int) (DWORD) ::SendMessage(hwnd, SB_SETINCREMENT, 0, MAKELPARAM((WORD) incr, 0));
    }

inline int SpinButton_FindNum(HWND hwnd, int num)
    {
    return (int) (DWORD) ::SendMessage(hwnd, SB_FINDNUM, 0, MAKELPARAM((WORD) num, 0));
    }

inline int SpinButton_GetSpnNum(HWND hwnd, int index)
    {
    return (int) (DWORD) ::SendMessage(hwnd, SB_GETSPNNUM, (WPARAM) index, 0L);
    }

#else

#define SpinButton_Enable(hwnd, fEnable)        EnableWindow((hwnd), (fEnable))
#define SpinButton_AddString(hwnd, lpsz)        ((int)(DWORD)SendMessage((hwnd), SB_ADDSTRING, 0, (LPARAM)(LPCSTR)(lpsz)))
#define SpinButton_DeleteString(hwnd, index)    ((int)(DWORD)SendMessage((hwnd), SB_DELETESTRING, (WPARAM)(int)(index), 0L))
#define SpinButton_FindString(hwnd, indexStart, lpszFind)   ((int)(DWORD)SendMessage((hwnd), SB_FINDSTRING, (WPARAM)(int)(indexStart), (LPARAM)(LPCSTR)(lpszFind)))
#define SpinButton_LimitText(hwnd, cchMax)      ((void)SendMessage((hwnd), SB_LIMITTEXT, (WPARAM)(cchMax), 0L))
#define SpinButton_GetCount(hwnd)               ((int)(DWORD)SendMessage((hwnd), SB_GETCOUNT, 0, 0L))
#define SpinButton_GetCurSel(hwnd)              ((int)(DWORD)SendMessage((hwnd), SB_GETCURSEL, 0, 0L))
#define SpinButton_GetSpnText(hwnd, index, lpsz)    ((int)(DWORD)SendMessage((hwnd), SB_GETSPNTEXT, (WPARAM)(int)(index), (LPARAM)(LPCSTR)(lpsz)))
#define SpinButton_GetTextLength(hwnd, index)   ((int)(DWORD)SendMessage((hwnd), SB_GETSPNTEXTLEN, (WPARAM)(int)(index), 0L))
#define SpinButton_InsertString(hwnd, index, lpsz)  ((int)(DWORD)SendMessage((hwnd), SB_INSERTSTRING, (WPARAM)(int)(index), (LPARAM)(LPCSTR)(lpsz)))
#define SpinButton_ResetContent(hwnd, mode, lpsz)   ((VOID)SendMessage((hwnd), SB_RESETCONTENT, (WPARAM)(int)(mode), (LPARAM)(LPCSTR)(lpsz)))
#define SpinButton_SetCurSel(hwnd, index)       ((int)(DWORD)SendMessage((hwnd), SB_SETCURSEL, (WPARAM)(WORD)(int)(index), 0L))
#define SpinButton_SetRange(hwnd, low, high)    ((int)(DWORD)SendMessage((hwnd), SB_SETRANGE, 0, MAKELPARAM ((WORD)(low), (WORD)(high))))
#define SpinButton_SetIncrement(hwnd, incr)     ((int)(DWORD)SendMessage((hwnd), SB_SETINCREMENT, 0, MAKELPARAM ((WORD)(incr), 0)))
#define SpinButton_FindNum(hwnd, num)           ((int)(DWORD)SendMessage((hwnd), SB_FINDNUM, 0, MAKELPARAM ((WORD)(num), 0)))
#define SpinButton_GetSpnNum(hwnd, index)       ((int)(DWORD)SendMessage((hwnd), SB_GETSPNNUM, (WPARAM)(WORD)(index), 0L))

#endif // __cplusplus

/************************************************************************/
/* TunnelButton stuff starts here                                       */
/************************************************************************/
#define TB_BUTTON_CLASS        QCLASS_TB_BUTTON
#define TB_TUNNELER_CLASS      QCLASS_TB_TUNNELER
#define TNLS_COMBO  0x00008000          // Creation style for combo tunnel
#define TNLS_PLAY   0x00004000          // Tunnel button has Play btn as well as tunneler

// #ifndef NOCTLMGR

/* Tunnel control styles */
// #ifndef NOWINSTYLES
#define TS_LEFT                 ES_LEFT
#define TS_CENTER               ES_CENTER
#define TS_RIGHT                ES_RIGHT
#define TS_MULTILINE            ES_MULTILINE
#define TS_UPPERCASE            ES_UPPERCASE
#define TS_LOWERCASE            ES_LOWERCASE
#define TS_PASSWORD             ES_PASSWORD
#define TS_AUTOVSCROLL          ES_AUTOVSCROLL
#define TS_AUTOHSCROLL          ES_AUTOHSCROLL
#define TS_NOHIDESEL            ES_NOHIDESEL
#define TS_OEMCONVERT           ES_OEMCONVERT
#if (WINVER >= 0x030a)
#define TS_READONLY             ES_READONLY
#define TS_WANTRETURN           ES_WANTRETURN

#endif  /* WINVER >= 0x030a */
// #endif  /* NOWINSTYLES */

/* Tunnel control messages */
// #ifndef NOWINMESSAGES
#define TM_GETSEL               EM_GETSEL
#define TM_SETSEL               EM_SETSEL
#define TM_GETRECT              EM_GETRECT
#define TM_SETRECT              EM_SETRECT
#define TM_SETRECTNP            EM_SETRECTNP
#define TM_LINESCROLL           EM_LINESCROLL
#define TM_GETMODIFY            EM_GETMODIFY
#define TM_SETMODIFY            EM_SETMODIFY
#define TM_GETLINECOUNT         EM_GETLINECOUNT
#define TM_LINEINDEX            EM_LINEINDEX
#define TM_SETHANDLE            EM_SETHANDLE
#define TM_GETHANDLE            EM_GETHANDLE
#define TM_LINELENGTH           EM_LINELENGTH
#define TM_REPLACESEL           EM_REPLACESEL
#define TM_SETFONT              EM_SETFONT                /* NOT IMPLEMENTED: use WM_SETFONT */
#define TM_GETLINE              EM_GETLINE
#define TM_LIMITTEXT            EM_LIMITTEXT
#define TM_CANUNDO              EM_CANUNDO
#define TM_UNDO                 EM_UNDO
#define TM_FMTLINES             EM_FMTLINES
#define TM_LINEFROMCHAR         EM_LINEFROMCHAR
#define TM_SETWORDBREAK         EM_SETWORDBREAK           /* NOT IMPLEMENTED: use EM_SETWORDBREAK */
#define TM_SETTABSTOPS          EM_SETTABSTOPS
#define TM_SETPASSWORDCHAR      EM_SETPASSWORDCHAR
#define TM_EMPTYUNDOBUFFER      EM_EMPTYUNDOBUFFER
#if (WINVER >= 0x030a)
#define TM_GETFIRSTVISIBLELINE  EM_GETFIRSTVISIBLELINE
#define TM_SETREADONLY          EM_SETREADONLY
#define TM_SETWORDBREAKPROC     EM_SETWORDBREAKPROC
#define TM_GETWORDBREAKPROC     EM_GETWORDBREAKPROC
#define TM_GETPASSWORDCHAR      EM_GETPASSWORDCHAR
#endif  /* WINVER >= 0x030a */
#define TM_MESSAGE_FIRST        TM_GETSEL
#define TM_MESSAGE_LAST         TM_GETPASSWORDCHAR
#define TM_SETPLAY              (WM_USER + 0x0100)        // change state of Play button
#define TBF_PLAY                0       // wParam == TBF_PLAY to show Speaker bitmap
#define TBF_STOP                1       // wParam == TBF_STOP to show Hand bitmap
#define TBF_GRAY                2       // wParam == TBF_GRAY to show disabled Speaker bitmap

// #endif  /* NOWINMESSAGES */

/* Tunnel control notification codes */
#define TN_SETFOCUS             EN_SETFOCUS
#define TN_KILLFOCUS            EN_KILLFOCUS
#define TN_CHANGE               EN_CHANGE
#define TN_UPDATE               EN_UPDATE
#define TN_ERRSPACE             EN_ERRSPACE
#define TN_MAXTEXT              EN_MAXTEXT
#define TN_HSCROLL              EN_HSCROLL
#define TN_VSCROLL              EN_VSCROLL
#define TN_TUNNEL               0x8000
#define TN_PLAY                 0x8001

/* TunnelButton Control API */
#define Tunnel_Enable(hwndCtl, fEnable)           EnableWindow((hwndCtl), (fEnable))

#define Tunnel_GetText(hwndCtl, lpch, cchMax)     GetWindowText((hwndCtl), (lpch), (cchMax))
#define Tunnel_GetTextLength(hwndCtl)             GetWindowTextLength(hwndCtl)
#define Tunnel_SetText(hwndCtl, lpsz)             SetWindowText((hwndCtl), (lpsz))

#define Tunnel_LimitText(hwndCtl, cchMax)         ((void)SendMessage((hwndCtl), TM_LIMITTEXT, (WPARAM)(cchMax), 0L))

#define Tunnel_GetLineCount(hwndCtl)              ((int)(DWORD)SendMessage((hwndCtl), TM_GETLINECOUNT, 0, 0L))
#define Tunnel_GetLine(hwndCtl, line, lpch, cchMax) ((*((int FAR*)(lpch)) = (cchMax)), ((int)(DWORD)SendMessage((hwndCtl), TM_GETLINE, (WPARAM)(int)(line), (LPARAM)(LPSTR)(lpch))))

#define Tunnel_GetRect(hwndCtl, lprc)             ((void)SendMessage((hwndCtl), TM_GETRECT, 0, (LPARAM)(RECT FAR*)(lprc)))
#define Tunnel_SetRect(hwndCtl, lprc)             ((void)SendMessage((hwndCtl), TM_SETRECT, 0, (LPARAM)(const RECT FAR*)(lprc)))
#define Tunnel_SetRectNoPaint(hwndCtl, lprc)      ((void)SendMessage((hwndCtl), TM_SETRECTNP, 0, (LPARAM)(const RECT FAR*)(lprc)))

#define Tunnel_GetSel(hwndCtl)                    ((DWORD)SendMessage((hwndCtl), TM_GETSEL, 0, 0L))
#define Tunnel_SetSel(hwndCtl, ichStart, ichEnd)  ((void)SendMessage((hwndCtl), TM_SETSEL, 0, MAKELPARAM((ichStart), (ichEnd))))
#define Tunnel_ReplaceSel(hwndCtl, lpszReplace)   ((void)SendMessage((hwndCtl), TM_REPLACESEL, 0, (LPARAM)(LPCSTR)(lpszReplace)))

#define Tunnel_GetModify(hwndCtl)                 ((BOOL)(DWORD)SendMessage((hwndCtl), TM_GETMODIFY, 0, 0L))
#define Tunnel_SetModify(hwndCtl, fModified)      ((void)SendMessage((hwndCtl), TM_SETMODIFY, (WPARAM)(UINT)(fModified), 0L))

#define Tunnel_LineFromChar(hwndCtl, ich)         ((int)(DWORD)SendMessage((hwndCtl), TM_LINEFROMCHAR, (WPARAM)(int)(ich), 0L))
#define Tunnel_LineIndex(hwndCtl, line)           ((int)(DWORD)SendMessage((hwndCtl), TM_LINEINDEX, (WPARAM)(int)(line), 0L))
#define Tunnel_LineLength(hwndCtl, line)          ((int)(DWORD)SendMessage((hwndCtl), TM_LINELENGTH, (WPARAM)(int)(line), 0L))

#define Tunnel_Scroll(hwndCtl, dv, dh)            ((void)SendMessage((hwndCtl), TM_LINESCROLL, 0, MAKELPARAM((dv), (dh))))

#define Tunnel_CanUndo(hwndCtl)                   ((BOOL)(DWORD)SendMessage((hwndCtl), TM_CANUNDO, 0, 0L))
#define Tunnel_Undo(hwndCtl)                      ((BOOL)(DWORD)SendMessage((hwndCtl), TM_UNDO, 0, 0L))
#define Tunnel_EmptyUndoBuffer(hwndCtl)           ((void)SendMessage((hwndCtl), TM_EMPTYUNDOBUFFER, 0, 0L))

#define Tunnel_SetPasswordChar(hwndCtl, ch)       ((void)SendMessage((hwndCtl), TM_SETPASSWORDCHAR, (WPARAM)(UINT)(ch), 0L))

#define Tunnel_SetTabStops(hwndCtl, cTabs, lpTabs) ((void)SendMessage((hwndCtl), TM_SETTABSTOPS, (WPARAM)(int)(cTabs), (LPARAM)(const int FAR*)(lpTabs)))

#define Tunnel_FmtLines(hwndCtl, fAddEOL)         ((BOOL)(DWORD)SendMessage((hwndCtl), TM_FMTLINES, (WPARAM)(BOOL)(fAddEOL), 0L))

#define Tunnel_GetHandle(hwndCtl)                 ((HLOCAL)(UINT)(DWORD)SendMessage((hwndCtl), TM_GETHANDLE, 0, 0L))
#define Tunnel_SetHandle(hwndCtl, h)              ((void)SendMessage((hwndCtl), TM_SETHANDLE, (WPARAM)(UINT)(HLOCAL)(h), 0L))

#if (WINVER >= 0x030a)
#define Tunnel_GetFirstVisibleLine(hwndCtl)       ((int)(DWORD)SendMessage((hwndCtl), TM_GETFIRSTVISIBLELINE, 0, 0L))

#define Tunnel_SetReadOnly(hwndCtl, fReadOnly)    ((BOOL)(DWORD)SendMessage((hwndCtl), TM_SETREADONLY, (WPARAM)(BOOL)(fReadOnly), 0L))

#define Tunnel_GetPasswordChar(hwndCtl)           ((char)(DWORD)SendMessage((hwndCtl), TM_GETPASSWORDCHAR, 0, 0L))

#define Tunnel_SetWordBreakProc(hwndCtl, lpfnWordBreak) ((void)SendMessage((hwndCtl), TM_SETWORDBREAKPROC, 0, (LPARAM)(EDITWORDBREAKPROC)(lpfnWordBreak)))
#define Tunnel_GetWordBreakProc(hwndCtl)          ((EDITWORDBREAKPROC)SendMessage((hwndCtl), TM_GETWORDBREAKPROC, 0, 0L))
#endif /* WINVER >= 0x030a */
#define Tunnel_SetPlay(hwndCtl, fStop)           ((void)SendMessage((hwndCtl), TM_SETPLAY, (WPARAM)(UINT)(fStop), 0L))

// #endif  /* NOCTLMGR */

/************************************************************************
 * Drag List stuff begins here                                          *
 ************************************************************************/

BOOL SYM_EXPORT WINAPI NGSetDragList (
    HWND    hListBox,               // list box to become drag server
    UINT    wStyle,                 // style flags (see below)
    HCURSOR hCursorSingle,          // drag cursor for single items
    HCURSOR hCursorMulti,           // drag cursor for multiple items
    UINT wType);                    // list box dragging ID

#define DL_NODROP_CURSOR        1000            // res ID for no drop cursor

                        // registered message string
#define DLM_MESSAGE             "NGUI:Symantec_Drag_List"

/*@STRUCTAPI:****************************************************************/
typedef struct DRAGLISTSTRUCT
    {
    UINT        wMessage;
    HWND        hwndDrag;
    HWND        hwndDrop;
    UINT        wType;
    UINT        wFlags;
    BOOL        bAccept;
    POINT       pt;
    } DRAGLISTSTRUCT;
typedef DRAGLISTSTRUCT          *PDRAGLISTSTRUCT;
typedef DRAGLISTSTRUCT NEAR     *NPDRAGLISTSTRUCT;
typedef DRAGLISTSTRUCT FAR      *LPDRAGLISTSTRUCT;
/*****
@Name:
DRAGLISTSTRUCT

@Description:
This structure stores information about a drag-and-drop operation to (and from)
a drag list box.

@Members:
$wMessage$ Specifies the drag-and-drop function which can be one of the
following:
    ~DLM_DROPOBJECT~ Allow a drop to occur.
    ~DLM_DRAGQUERY~ Query the drag-and-drop server.
    ~DLM_DRAGSTARTQUERY~ Begin a drag-and-drop operation.

$hwndDrag$ Specifies the window where the drag started (source).

$hwndDrop$ Specifies the window where the drop occurred (dest).

$wType$ Specifies the drag window ID.

$wFlags$ Specifies the status of the keyboard. This parameter can be a
combination of the following values:
    ~DL_CTRLKEY~ The Ctrl key was depressed.
    ~DL_ALTKEY~ The Alt key was depressed.
    ~DL_SHIFTKEY~ The Shift key was depressed.

$bAccept$ Set to TRUE to accept a drop.

$pt$ Specifies the original cursor coordinates.

@Include: ngui.h
*****************************************************************************/

// KEYBOARD FLAGS
// -----------------------------------
#define DL_CTRLKEY              0x0001
#define DL_ALTKEY               0x0002
#define DL_SHIFTKEY             0x0004

// DRAG/DROP FUNCTIONS
// -----------------------------------
#define DLM_DROPOBJECT          0x0100
#define DLM_DRAGQUERY           0x0101
#define DLM_DRAGSTARTQUERY  0x0102

// STYLE FLAGS
// -----------------------------------
#define DLS_PARENTNOTIFY        0x0001          // notify parent window
#define DLS_TARGETNOTIFY        0x0002          // notify target window
#define DLS_SAMELISTDROP        0x0004          // drag/drop to same list
#define DLS_RIGHTBUTTON         0x0008          // enable right button drags
#define DLS_SOURCENOTIFY    0x0010      // notify source listbox window
#define DLS_DRAGSTARTQUERY  0x0020      // query before draging

// --------------------------------------------------------------------------
// Message Cracker-type template for handling the drag list message, which
// casts everything properly and calls the specified function which has
// the format:
//
//      void DragMessage(HWND hwnd, int ctrlID, LPDRAGLISTSTRUCT lpInfo)
//
// --------------------------------------------------------------------------

#define HANDLE_DRAG_MESSAGE(hwnd,wParam,lParam,fn)              \
        ((fn)((hwnd),(int)(wParam),(LPDRAGLISTSTRUCT)(lParam)),0L)

void SYM_EXPORT WINAPI NG_RectFromPt (POINT pt, LPRECT lpRect, int nOffset);
int SYM_EXPORT WINAPI NG_ListItemFromPoint (HWND hList, POINT pt);

/************************************************************************
 * Normal Text Static Control                                           *
 ************************************************************************/

#define NGSTATIC        QCLASS_STATIC
// Use NGSTATIC in place of "STATIC" and you should
// get non-bold (normal) static text in your dialog for the
// control where this is used.


/************************************************************************
 * VCR Button stuff begins here                                         *
 ************************************************************************/

                    // VCR button styles; the two low bits
                    // indicate the orientation
#define VCRS_UP         0x00000000UL
#define VCRS_DOWN       0x00000001UL
#define VCRS_LEFT       0x00000002UL
#define VCRS_RIGHT      0x00000003UL
#define VCRS_AUTOREPEAT 0x00000004UL
#define VCRS_AUTOSIZE   0x00000008UL

                    // VCR button notification messages
#define VCRN_LINE       1
#define VCRN_PAGE       2
#define VCRN_END        3

#ifdef __cplusplus
}
#endif

#ifndef NOPASSWORD

// ************************************************************** //
// These defines are used by the password verification dialog box //
// when the application request that a modeless dialog box is to  //
// be used. It is not enough with a standard TRUE/FALSE return.   //
// It is required that the calling application is notified if the //
// dialog box is destroyed, due to the parent being destroyed.    //
// These return codes can be expected to be returned in the       //
// PASSWORDSTRUCT hModeless member.  MArTiN                       //
// ************************************************************** //

#define PASSWORDMODELESS_DESTROYED  0
#define PASSWORDMODELESS_VALID      1
#define PASSWORDMODELESS_INVALID    2

/*@STRUCTAPI:****************************************************************/
typedef struct tagPASSWORDSTRUCT
{
    DWORD   dwSize;
    HWND    hParent;
    DWORD   dwPassword;
    UINT    uMaxCharsToAccept;
    BOOL    bCaseSensitive;
    BOOL    bOemConvert;
    UINT    uHelpID;
    WORD    wVirtKey;
    DWORD   dwKeyFlags;
    HWND    hModeless;

#ifdef __cplusplus
    WINAPI tagPASSWORDSTRUCT()
      {}

    WINAPI tagPASSWORDSTRUCT(           // default constructor
    HWND c_hParent,
    DWORD c_dwPassword = 0,
    UINT c_uMaxCharsToAccept = MAXIMUM_PASSWORD_LEN,
    BOOL c_bCaseSensitive = FALSE,
    BOOL c_bOemConvert = FALSE,
    UINT c_uHelpID = 0
    )
    {   dwSize = sizeof(tagPASSWORDSTRUCT);
    hParent = c_hParent;
    dwPassword = c_dwPassword;
    uMaxCharsToAccept = c_uMaxCharsToAccept;
    bCaseSensitive = c_bCaseSensitive;
    bOemConvert = c_bOemConvert;
    uHelpID = c_uHelpID;
    wVirtKey = 0;
    dwKeyFlags = 0;
    hModeless = NULL;
    }
#endif
} PASSWORDSTRUCT, FAR *LPPASSWORDSTRUCT;
/*****
@Name: PASSWORDSTRUCT

@Description:
This structure contains information necessary to implement password dialog
boxes. Notice that if you're using C++ the structure can automatically be
initialized with default values.

@Members:
$dwSize$ Specifies the size of this size.

$hParent$ Specifies the parent window.

$dwPassword$ The CRC of the user's password is placed here.

$uMaxCharsToAccept$ Specifies the maximum number of password characters to
accept.

$bCaseSensitive$ Specifies that the password is case sensitive.

$bOemConvert$ Set to TRUE to convert from ANSI to OEM on an user's password.

$uHelpID$ Specifies the help ID to display up when help is needed.

$wVirtKey$ Specifies a virtual key code.

$dwKeyFlags$ Specifies key Flags.

$hModeless$ Use modeless password dialog and window handle.

$tagPASSWORDSTRUCT()$ C++ constructor. Actually should be removed, or use an
assertion to make sure the constructor that follows is always used in C++ code.

$tagPASSWORDSTRUCT(HWND,DWORD,UINT,BOOL,BOOL,UINT)$ C++ constructor that
should be used to initialize this structure. The only parameter that is
required here is the parent's HWND.

@Include: SYMUTIL
*****************************************************************************/

#ifdef __cplusplus                      // restarting _cplusplus block for rest of file
extern "C" {
#endif

/*
**  Function Prototypes:
*/

BOOL SYM_EXPORT WINAPI PasswordChange(LPPASSWORDSTRUCT lpPasswordStruct);
BOOL SYM_EXPORT WINAPI PasswordVerify(LPPASSWORDSTRUCT lpPasswordStruct);

#endif


/************************************************************************
 * Color Box stuff begins here                                          *
 ************************************************************************/

#define CLASS_COLORBOX          QCLASS_COLORBOX

#define CBXS_16COLORS           0x00000000L     // 16 colors (8 x 2)
#define CBXS_32COLORS           0x00000001L     // 16 colors (8 x 4)
#define CBXS_48COLORS           0x00000002L     // 16 colors (8 x 6)
#define CBXS_64COLORS           0x00000003L     // 16 colors (8 x 8)
#define CBXS_LOADDEFAULT        0x00000004L     // load default colors
#define CBXS_NOTIFY             0x00000008L     // send notification codes

#define CBX_ERR                 (-1)

enum
    {
    CBX_GETCOUNT = WM_USER,             // return total color count
    CBX_SETCURSEL,                      // set current control selection
    CBX_GETCURSEL,                      // get current control selection
    CBX_GETCOLOR,                       // get the color of a selected box
    CBX_SETCOLOR,                       // set the color of a selected box
    CBX_SETCOLORARRAY,                  // set all colors in the control
    CBX_GETCOLORARRAY,                  // get all colors in the control
    CBX_FINDCOLOR,                      // get index of the color specified
    };

enum
    {
    CBXN_SELCHANGE,
    CBXN_SETFOCUS,
    CBXN_KILLFOCUS,
    };
                    // use this to return the total
                    // number of colors in the control
                    // (16, 32, 48, 64)
#define ColorBox_GetCount(hwndCtl)                      \
        ((int)(DWORD)SendMessage((hwndCtl), CBX_GETCOUNT, 0, 0L))

                    // use these to get or set the
                    // selected color
#define ColorBox_SetCurSel(hwndCtl, index)              \
        ((int)(DWORD)SendMessage((hwndCtl), CBX_SETCURSEL, (WPARAM)(int)(index), 0L))
#define ColorBox_GetCurSel(hwndCtl)                     \
        ((int)(DWORD)SendMessage((hwndCtl), CBX_GETCURSEL, 0, 0L))

                    // use these to get and/or set
                    // individual colors displayed in
                    // the control
#define ColorBox_GetColor(hwndCtl, index)               \
        ((COLORREF)SendMessage((hwndCtl), CBX_GETCOLOR, (WPARAM)(int)(index), 0L))
#define ColorBox_SetColor(hwndCtl, index, data)         \
        ((COLORREF)SendMessage((hwndCtl), CBX_SETCOLOR, (WPARAM)(int)(index), (LPARAM)(data)))

                    // use these to get and/or set all
                    // colors displayed in the control
#define ColorBox_SetColorArray(hwndCtl, lpColors)       \
        ((BOOL)(DWORD)SendMessage((hwndCtl), CBX_SETCOLORARRAY, 0, (LPARAM)(COLORREF FAR*)(lpColors)))
#define ColorBox_GetColorArray(hwndCtl, lpColors)       \
        ((BOOL)(DWORD)SendMessage((hwndCtl), CBX_GETCOLORARRAY, 0, (LPARAM)(COLORREF FAR*)(lpColors)))

#define ColorBox_FindColor(hwndCtl, data)               \
        ((int)SendMessage((hwndCtl), CBX_FINDCOLOR, 0, (LPARAM)(data)))


BOOL WINAPI ColorBoxInit (HINSTANCE hInst);
void WINAPI ColorBoxCleanup (void);
//void WINAPI ColorBoxSysColorChange (void);


//***************************************************************************
// VList Control
//***************************************************************************

BOOL WINAPI NG_VListRegister (void);
LRESULT SYM_EXPORT CALLBACK NG_VListBoxWndProc (
    HWND    hwnd,
    UINT    message,
    WPARAM  wParam,
    LPARAM  lParam
);


//***************************************************************************
// Standard Windows About Box
//***************************************************************************

                    // Standard Windows About Box
                    // flags
#define SAF_ENABLEHOOKPROC      0x00000001L
#define SAF_ENABLETEMPLATE      0x00000002L
#define SAF_AUTODESTROYBITMAPS  0x00000003L  // Auto destroy bitmaps

                    // Standard Windows About Box
                    // structure
/*@STRUCTAPI:****************************************************************/
typedef struct {
    DWORD       dwStructSize;
    DWORD       dwFlags;
    HWND        hWndOwner;
    HINSTANCE   hInst;
    HICON       hIcon;
    LPCSTR      lpszAppName;
    LPCSTR      lpszVersion;
    LPCSTR      lpszCopyright;
    LPCSTR      lpszPerson;
    LPCSTR      lpszCompany;
    LPCSTR      lpszBuildString;
    DLGPROC     fpHookProc;
    LPCSTR      lpszTemplate;
    DWORD       dwUserData;
    LPCSTR      lpszSerialID;
    HBITMAP     hBitmap;
    int         nTimer;
} STDABOUTSTRUCT, FAR * LPSTDABOUTSTRUCT;
/*****
@Name: STDABOUTSTRUCT

@Description:
This structure contains information used to display the About box for a Peter
Symantec Computing Group application.

@Members:
$dwStructSize$ Size of this structure.
$dwFlags$ Flags for display.
$hWndOwner$ Handle or parent window.
$hInst$ Instance of caller.
$hIcon$ Icon Resource.
$lpszAppName$ Application name to use.
$lpszVersion$ Application version to use.
$lpszCopyright$ Copyright to use.
$lpszPerson$ Who product belongs to.
$lpszCompany$ User's company name.
$lpszBuildString$ Build String    (optional).
$fpHookProc$ Hook Proc       (optional).
$lpszTemplate$ Custom Template (optional).
$dwUserData$ User data for hook proc (optional).
$lpszSerialID$ Application serial id to use.

@See: StdAboutBox
@Include: ngui.h
*****************************************************************************/

WORD SYM_EXPORT WINAPI StdAboutBox (        // Standard Windows About Box
    LPSTDABOUTSTRUCT    lpabout         // [in]     About Structure pointer
);

HWND SYM_EXPORT WINAPI StdGuiltBoxCreate (  // Standard Windows Guilt Box
    LPSTDABOUTSTRUCT    lpabout         // [in]     Guilt Structure pointer
);

VOID SYM_EXPORT WINAPI StdGuiltBoxDestroy(
    HWND    hDlg
);


//--------------------------------------------------------------------------
// NGUI Tray Control
//
// The Tray Control is a status bar-like control that provides a series
// of trays to draw text in.
//
// - Doug Neal 5/25/93
//--------------------------------------------------------------------------

BOOL SYM_EXPORT WINAPI NGTrayInit (VOID);

BOOL SYM_EXPORT WINAPI NGTrayDone (VOID);

                    // Define custom control class name
                    // to be used...
#define NT_TRAY_CLASSNAME       QCLASS_NT_TRAY


                    // Error
#define TRAY_MSG_ERR            -1

                    // Messages
#define TRAY_MSG_ADDTRAY        WM_USER+1       // Add Tray to Control
                        // wParam - not used
                        // lParam - LPTRAYDATA structure

#define TRAY_MSG_RESETCONTENT   WM_USER+2       // Delete All Trays in Control
                        // wParam - not used
                        // lParam - not used

#define TRAY_MSG_SETTEXT        WM_USER+3       // Set the text in a tray
                        // wParam - Tray I.D.
                        // lParam - LPSTR Text

#define TRAY_MSG_GETTEXT        WM_USER+4       // Get the text in a tray
                        // wParam - Tray I.D.
                        // lParam - LPSTR Text (must be large enough)


                    // Tray Data Flags
#define TRAY_FLAG_SHRINKWITHSIZE        0x00000001L     // Tray will shrink when window is resized
#define TRAY_FLAG_SHIFTWITHSIZE         0x00000002L     // Tray will shift left/right when window is resized
#define TRAY_FLAG_LEFTJUSTIFY           0x00000004L     // Text will be left justified
#define TRAY_FLAG_RIGHTJUSTIFY          0x00000008L     // Text will be right justified
#define TRAY_FLAG_CENTERJUSTIFY         0x00000010L     // Text will be centered


typedef struct {
    WORD    wID;                        // Tray I.D.
    DWORD   dwFlags;                    // Flags
    RECT    rect;                       // Client Rect of Tray (left,top,right,bottom are window relative)
    LONG    lReserved;                  // Reserved for future use
} TRAYDATA, NEAR * NPTRAYDATA, FAR * LPTRAYDATA;


//////////////////////////////////////////////////////////////////////////
// SYMGUICheckVersion
//
// A DLL which links to SYMGUI calls this function inside of its own
// <DLL-Name>CheckVersion function, in order to ensure that the version
// of SYMGUI it is linked to is late enough.
//
// Input:   lpExpected  expected version
//          CallBack    function to call in case of mismatch
//////////////////////////////////////////////////////////////////////////
void SYM_EXPORT WINAPI SYMGUICheckVersion( LPVERREC lpExpected,
    CHECKVERCALLBACK_TYP CallBack );


// -------------------------------------------------------------------------
// Edit Control Filters
// -------------------------------------------------------------------------

UINT SYM_EXPORT WINAPI EditFilterInsert (HWND hEdit, SYMGUITYPE nFilterType);

// -------------------------------------------------------------------------
// Defines for the standard bitmaps to use in a Toolbar
// -------------------------------------------------------------------------
#define NGBMP_BASE              2000
#define NGBMP_OPEN              NGBMP_BASE
#define NGBMP_SAVE              2001
#define NGBMP_CLOSE             2002
#define NGBMP_NEW               2003
#define NGBMP_PRINT             2004
#define NGBMP_CUT               2005
#define NGBMP_COPY              2006
#define NGBMP_PASTE             2007
#define NGBMP_DELETE            2008
#define NGBMP_UNDO              2009
#define NGBMP_FIND              2010
#define NGBMP_FIND_PREV         2011
#define NGBMP_FIND_NEXT         2012
#define NGBMP_FONT              2013
#define NGBMP_HELP              2014
#define NGBMP_RUN               2015
#define NGBMP_COMPILE           2016
#define NGBMP_REPLACE           2017
#define NGBMP_EXIT              2018
#define NGBMP_CNTRL             2019
#define NGBMP_MKDIR             2020
#define NGBMP_DOS               2021
#define NGBMP_DRVWND            2022
#define NGBMP_QA                2023
#define NGBMP_FILTER            2024
#define NGBMP_FILE_PANE         2025
#define NGBMP_FILE_SEL          2026
#define NGBMP_MOVE              2027
#define NGBMP_REFRESH           2028
#define NGBMP_SORT_NAME         2029
#define NGBMP_SORT_TYPE         2030
#define NGBMP_TREE_PANE         2031
#define NGBMP_VIEW              2032
#define NGBMP_VIEW_PANE         2033
#define NGBMP_ABORT             2034
#define NGBMP_FIND_TEXT         2035
#define NGBMP_DIALOG_EDITOR     2036
#define NGBMP_STYLE             2037
#define NGBMP_TEST              2038

// -------------------------------------------------------------------------
// Defines for the standard bitmaps to use in the Slider Control
// -------------------------------------------------------------------------
#define NG_SLBMP_HORZUP         2100
#define NG_SLBMP_HORZUPF        2101
#define NG_SLBMP_HORZDN         2102
#define NG_SLBMP_HORZDNF        2103
#define NG_SLBMP_VERTLT         2104
#define NG_SLBMP_VERTLTF        2105
#define NG_SLBMP_VERTRT         2106
#define NG_SLBMP_VERTRTF        2107

#define STOCKBITMAPS           24000

HBITMAP SYM_EXPORT WINAPI NGStockBitmapGet (
    UINT uIndex);                       // NGBMP_XXX id


////////////////////////////////////////////////////////////////////////
//
//  Functions for system-color stock brushes:
//
VOID WINAPI NGStockBrushesDelete(void);     // called in NG_Done() only
HBRUSH SYM_EXPORT WINAPI NGStockBrushGet( int nDspElement );

/* Resource ID for 3dcheck.bmp (for .lib version of ctl3d) */
#ifndef CTL3D_3DCHECK
#define CTL3D_3DCHECK 26567
#endif


#if defined(SYM_WIN32)

////////////////////////////////////////////////////////////////////////
//
//  Functions for Windows 95 TrayNotification Area
//

#define SYMTRAY_DEFAULT 0
#define SYMTRAY_DONTDELETEICONS 0x00000001

DWORD SYM_EXPORT WINAPI SymTray_Init(
    HWND hWnd,                          // Callers HWND, will receive SYSCOMMAND SC_RESTORE when tray button hit unless uNotification set
    UINT uID,                           // Notification index (allows multiple indicators per app) can use SYMTRAY_DEFAULT
    UINT uNotification,                 // Message to send to hWnd on WM_LBUTTONDOWN (can be SYMTRAY_DEFAULT to generate SYSCOMMAND SC_RESTORE)
    HICON FAR * phIcon,                 // Pointer to array of icons for animation (must specify at least ONE)
    UINT uIconCount,                    // Count of array icons for animation (can be SYMTRAY_DEFAULT)
    UINT uDelay,                        // Animation step rate in milliseconds (can be SYMTRAY_DEFAULT)
    LPSTR lpToolTip);                   // Tooltip text (can be SYMTRAY_DEFAULT for owners window title)

DWORD SYM_EXPORT WINAPI SymTray_InitEx(
    HWND hWnd,                          // Callers HWND, will receive SYSCOMMAND SC_RESTORE when tray button hit unless uNotification set
    UINT uID,                           // Notification index (allows multiple indicators per app) can use SYMTRAY_DEFAULT
    UINT uNotification,                 // Message to send to hWnd on WM_LBUTTONDOWN (can be SYMTRAY_DEFAULT to generate SYSCOMMAND SC_RESTORE)
    HICON FAR * phIcon,                 // Pointer to array of icons for animation (must specify at least ONE)
    UINT uIconCount,                    // Count of array icons for animation (can be SYMTRAY_DEFAULT)
    UINT uDelay,                        // Animation step rate in milliseconds (can be SYMTRAY_DEFAULT)
    LPSTR lpToolTip,                    // Tooltip text (can be SYMTRAY_DEFAULT for owners window title)
    UINT uFlags);                       // Flags (can be SYMTRAY_DEFAULT or SYMTRAY_DONTDELETEICONS)

STATUS SYM_EXPORT WINAPI SymTray_Update(
    DWORD dwTray,                       // SymTray handle (returned from SymTray_Init call)
    HICON FAR * hIcon,                  // Pointer to array of icons for animation, may be NULL if not updating
    UINT uIconCount,                    // Count of array icons for animation (can be SYMTRAY_DEFAULT)
    UINT uDelay,                        // Animation step rate in milliseconds (can be SYMTRAY_DEFAULT)
    LPSTR lpToolTip);                   // Tooltip text (can be SYMTRAY_DEFAULT if not updating, text, or "" to clear)

STATUS SYM_EXPORT WINAPI SymTray_Done(
    DWORD dwTray);                      // SymTray handle (returned from SymTray_Init call)

#endif




#ifdef __cplusplus
}
#endif

#endif  // SYM_WIN

#endif
