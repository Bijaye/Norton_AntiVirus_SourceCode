// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
/* Copyright 1992 - 2003, 2005 Symantec Corporation. All rights reserved.     */
/************************************************************************/
/*                                                                      */
/* $Header:   S:/INCLUDE/VCS/VLIST.H_v   1.0   26 Jan 1996 20:19:30   RJACKSO  $
/* 			                                                */
/* File:  fillbar.c                                                     */
/*                                                                      */
/* Description:                                                         */
/*      Header file for VList Custom Dialog Control                     */
/*                                                                      */
/* See Also:                                                            */
/*                                                                      */
/* Future Enhancements:                                                 */
/*                                                                      */
/* Operating System:  MS-Windows 3.1                                    */
/************************************************************************/
/* $Log:   S:/INCLUDE/VCS/VLIST.H_v  $                */
// 
//    Rev 1.0   26 Jan 1996 20:19:30   RJACKSO
// Initial revision.
// 
//    Rev 1.1   16 Nov 1995 16:48:20   MLELE
// Brought in changes from QAK6. cf QAK6 log 1.0.3.1, DSACKIN. Contains
// info on the VLIST class in SYMGUI.
// 
// 
//    Rev 1.0   09 Oct 1995 01:27:52   DSACKIN
// Initial revision.
/************************************************************************/

#if !defined(__VLIST_H)
#define __VLIST_H

#define VLB_OK             0
#define VLB_ERR           -1
#define VLB_ENDOFFILE     -1

#define VLBS_USEDATAVALUES     0x8000L  
#define VLBS_3DFRAME           0x4000L
#define VLBS_SPEEDSEARCH       0x2000L
#define VLBS_NOTIFY            0x0001L
#define VLBS_NOREDRAW          0x0004L
#define VLBS_OWNERDRAWFIXED    0x0010L
#define VLBS_HASSTRINGS        0x0040L
#define VLBS_USETABSTOPS       0x0080L
#define VLBS_NOINTEGRALHEIGHT  0x0100L
#define VLBS_WANTKEYBOARDINPUT 0x0400L
#define VLBS_DISABLENOSCROLL   0x1000L

// Application->VLIST messages               
// Corresponding to LB_ messages
#define VLB_MSGMIN              (WM_USER+500)
#define VLB_RESETCONTENT        (WM_USER+500)
#define VLB_SETCURSEL           (WM_USER+501)
#define VLB_GETCURSEL           (WM_USER+502)
#define VLB_GETTEXT             (WM_USER+503)
#define VLB_GETTEXTLEN          (WM_USER+504)
#define VLB_GETCOUNT            (WM_USER+505)
#define VLB_SELECTSTRING        (WM_USER+506)
#define VLB_FINDSTRING          (WM_USER+507)
#define VLB_GETITEMRECT         (WM_USER+508)
#define VLB_GETITEMDATA         (WM_USER+509)
#define VLB_SETITEMDATA         (WM_USER+510)
#define VLB_SETITEMHEIGHT       (WM_USER+511)
#define VLB_GETITEMHEIGHT       (WM_USER+512)
#define VLB_FINDSTRINGEXACT     (WM_USER+513)
#define VLB_INITIALIZE          (WM_USER+514)
#define VLB_SETTABSTOPS         (WM_USER+515)
#define VLB_GETTOPINDEX         (WM_USER+516)
#define VLB_SETTOPINDEX         (WM_USER+517)
#define VLB_GETHORIZONTALEXTENT (WM_USER+518)
#define VLB_SETHORIZONTALEXTENT (WM_USER+519)

// Unique to VLIST
#define VLB_UPDATEPAGE          (WM_USER+520)
#define VLB_GETLINES            (WM_USER+521)
#define VLB_PAGEDOWN            (WM_USER+522)
#define VLB_PAGEUP              (WM_USER+523)
#define VLB_MSGMAX				  (WM_USER+523)

// VLIST->Application messages  
// Conflicts with VLB_
#define VLBR_FINDSTRING         (WM_USER+600) 
#define VLBR_FINDSTRINGEXACT    (WM_USER+601) 
#define VLBR_SELECTSTRING       (WM_USER+602) 
#define VLBR_GETITEMDATA        (WM_USER+603)
#define VLBR_GETTEXT            (WM_USER+604)
#define VLBR_GETTEXTLEN         (WM_USER+605)

// Unique Messages
//
#define VLB_FIRST               (WM_USER+606)
#define VLB_PREV                (WM_USER+607)
#define VLB_NEXT                (WM_USER+608)
#define VLB_LAST                (WM_USER+609)
#define VLB_FINDITEM            (WM_USER+610)
#define VLB_RANGE               (WM_USER+611)
#define VLB_FINDPOS             (WM_USER+612)

// VLIST->Application Notifications
#define VLBN_FREEITEM            (WM_USER+700)
#define VLBN_FREEALL             (WM_USER+701)

#define IDS_VLBOXNAME         1

typedef struct _VLBStruct {
   int   nCtlID;
   int   nStatus;
   LONG  lData;
   LONG  lIndex;
   LPSTR lpTextPointer;
   LPSTR lpFindString;
} VLBSTRUCT;

typedef VLBSTRUCT FAR*  LPVLBSTRUCT;

typedef struct tagVLBSSMESSAGE {
   WORD   wCmd;
   WORD   wID;
   LPARAM lParam;
} VLBSSMESSAGE, FAR * LPVLBSSMESSAGE;

#define VLIST_CLASSNAME "VList"
extern BOOL WINAPI RegisterVListBox(HINSTANCE);


/****** VListBox control message APIs *****************************************/

#define VListBox_Enable(hwndCtl, fEnable)            EnableWindow((hwndCtl), (fEnable))

#define VListBox_GetCount(hwndCtl)                   ((int)(DWORD)SendMessage((hwndCtl), VLB_GETCOUNT, 0, 0L))
#define VListBox_ResetContent(hwndCtl)               ((BOOL)(DWORD)SendMessage((hwndCtl), VLB_RESETCONTENT, 0, 0L))

                                        // VLIST doesn't support these Yet.
#define VListBox_AddString(hwndCtl, lpsz)            ((int)(DWORD)SendMessage((hwndCtl), LB_ADDSTRING, 0, (LPARAM)(LPCSTR)(lpsz)))
// #define VListBox_InsertString(hwndCtl, index, lpsz)  ((int)(DWORD)SendMessage((hwndCtl), LB_INSERTSTRING, (WPARAM)(int)(index), (LPARAM)(LPCSTR)(lpsz)))

// #define VListBox_AddItemData(hwndCtl, data)          ((int)(DWORD)SendMessage((hwndCtl), LB_ADDSTRING, 0, (LPARAM)(data)))
// #define VListBox_InsertItemData(hwndCtl, index, data) ((int)(DWORD)SendMessage((hwndCtl), LB_INSERTSTRING, (WPARAM)(int)(index), (LPARAM)(data)))

#define VListBox_DeleteString(hwndCtl, index)        ((int)(DWORD)SendMessage((hwndCtl), VLB_DELETESTRING, (WPARAM)(int)(index), 0L))

#define VListBox_GetTextLen(hwndCtl, index)          ((int)(DWORD)SendMessage((hwndCtl), VLB_GETTEXTLEN, (WPARAM)(int)(index), 0L))
#define VListBox_GetText(hwndCtl, index, lpszBuffer)  ((int)(DWORD)SendMessage((hwndCtl), VLB_GETTEXT, (WPARAM)(int)(index), (LPARAM)(LPCSTR)(lpszBuffer)))

#define VListBox_GetItemData(hwndCtl, data)          ((LRESULT)(DWORD)SendMessage((hwndCtl), VLB_GETITEMDATA, 0, (LPARAM)(LPVLBSTRUCT)&(data)))
#define VListBox_SetItemData(hwndCtl, index, data)   ((int)(DWORD)SendMessage((hwndCtl), VLB_SETITEMDATA, (WPARAM)(int)(index), (LPARAM)(data)))

#define VListBox_FindString(hwndCtl, indexStart, lpszFind) ((int)(DWORD)SendMessage((hwndCtl), VLB_FINDSTRING, (WPARAM)(int)(indexStart), (LPARAM)(LPCSTR)(lpszFind)))
// #define VListBox_FindItemData(hwndCtl, indexStart, data) ((int)(DWORD)SendMessage((hwndCtl), LB_FINDSTRING, (WPARAM)(int)(indexStart), (LPARAM)(data)))

// #define VListBox_SetSel(hwndCtl, fSelect, index)     ((int)(DWORD)SendMessage((hwndCtl), LB_SETSEL, (WPARAM)(BOOL)(fSelect), MAKELPARAM((index), 0)))
// #define VListBox_SelItemRange(hwndCtl, fSelect, first, last)    ((int)(DWORD)SendMessage((hwndCtl), LB_SELITEMRANGE, (WPARAM)(BOOL)(fSelect), MAKELPARAM((first), (last))))

#define VListBox_GetCurSel(hwndCtl)                  ((int)(DWORD)SendMessage((hwndCtl), VLB_GETCURSEL, 0, 0L))
#define VListBox_SetCurSel(hwndCtl, index)           ((int)(DWORD)SendMessage((hwndCtl), VLB_SETCURSEL, VLB_FINDITEM, (LPARAM)(index)))

#define VListBox_SelectString(hwndCtl, indexStart, lpszFind) ((int)(DWORD)SendMessage((hwndCtl), VLB_SELECTSTRING, (WPARAM)(int)(indexStart), (LPARAM)(LPCSTR)(lpszFind)))
// #define VListBox_SelectItemData(hwndCtl, indexStart, data)   ((int)(DWORD)SendMessage((hwndCtl), LB_SELECTSTRING, (WPARAM)(int)(indexStart), (LPARAM)(data)))

// #define VListBox_GetSel(hwndCtl, index)              ((int)(DWORD)SendMessage((hwndCtl), LB_GETSEL, (WPARAM)(int)(index), 0L))
// #define VListBox_GetSelCount(hwndCtl)                ((int)(DWORD)SendMessage((hwndCtl), LB_GETSELCOUNT, 0, 0L))
#define VListBox_GetTopIndex(hwndCtl)                ((int)(DWORD)SendMessage((hwndCtl), VLB_GETTOPINDEX, 0, 0L))
// #define VListBox_GetSelItems(hwndCtl, cItems, lpItems) ((int)(DWORD)SendMessage((hwndCtl), LB_GETSELITEMS, (WPARAM)(int)(cItems), (LPARAM)(int FAR*)(lpItems)))

#define VListBox_SetTopIndex(hwndCtl, indexTop)      ((int)(DWORD)SendMessage((hwndCtl), VLB_SETTOPINDEX, (WPARAM)(int)(indexTop), 0L))

// #define VListBox_SetColumnWidth(hwndCtl, cxColumn)   ((void)SendMessage((hwndCtl), LB_SETCOLUMNWIDTH, (WPARAM)(int)(cxColumn), 0L))
#define VListBox_GetHorizontalExtent(hwndCtl)        ((int)(DWORD)SendMessage((hwndCtl), VLB_GETHORIZONTALEXTENT, 0, 0L))
#define VListBox_SetHorizontalExtent(hwndCtl, cxExtent)     ((void)SendMessage((hwndCtl), VLB_SETHORIZONTALEXTENT, (WPARAM)(int)(cxExtent), 0L))

#define VListBox_SetTabStops(hwndCtl, cTabs, lpTabs) ((BOOL)(DWORD)SendMessage((hwndCtl), VLB_SETTABSTOPS, (WPARAM)(int)(cTabs), (LPARAM)(int FAR*)(lpTabs)))

#define VListBox_GetItemRect(hwndCtl, index, lprc)   ((int)(DWORD)SendMessage((hwndCtl), VLB_GETITEMRECT, (WPARAM)(int)(index), (LPARAM)(RECT FAR*)(lprc)))

// #define VListBox_SetCaretIndex(hwndCtl, index)       ((int)(DWORD)SendMessage((hwndCtl), LB_SETCARETINDEX, (WPARAM)(int)(index), 0L))
// #define VListBox_GetCaretIndex(hwndCtl)              ((int)(DWORD)SendMessage((hwndCtl), LB_GETCARETINDEX, 0, 0L))


#if (WINVER >= 0x030a)
#define VListBox_FindStringExact(hwndCtl, indexStart, lpszFind) ((int)(DWORD)SendMessage((hwndCtl), VLB_FINDSTRINGEXACT, (WPARAM)(int)(indexStart), (LPARAM)(LPCSTR)(lpszFind)))

#define VListBox_SetItemHeight(hwndCtl, index, cy)   ((int)(DWORD)SendMessage((hwndCtl), VLB_SETITEMHEIGHT, (WPARAM)(int)(index), MAKELPARAM((cy), 0)))
#define VListBox_GetItemHeight(hwndCtl, index)       ((int)(DWORD)SendMessage((hwndCtl), VLB_GETITEMHEIGHT, (WPARAM)(int)(index), 0L))
#endif  /* WINVER >= 0x030a */

// #define VListBox_Dir(hwndCtl, attrs, lpszFileSpec)   ((int)(DWORD)SendMessage((hwndCtl), LB_DIR, (WPARAM)(UINT)(attrs), (LPARAM)(LPCSTR)(lpszFileSpec)))

// --------------------------------------------------------------------------
//  VlistBox functions that are needed to use VLIST
// --------------------------------------------------------------------------

LRESULT SYM_EXPORT CALLBACK VL_NG_VListBoxWndProc (
    HWND    hwnd,
    UINT    message,
    WPARAM  wParam,
    LPARAM  lParam
);

                                        // These are because we had to take
                                        // speed search out of SYMGUI.
BOOL SYM_EXPORT WINAPI VL_NG_AddSpeedSearch (
   HWND     hWndListbox,                // [in]    Listbox to attach to
   WORD     wFlags,                     // [in]    Reserved (position info)
   SSPROC   fpCallback,                 // [in]    Callback proc (owner-draw)
   DWORD    dwReserved                  // [in]    Reserved for future
);

VOID SYM_EXPORT WINAPI VL_NG_SetMessageWindow (
   HWND     hWndListbox,                // [in]    Listbox that is subclassed
   HWND     hWndMessageWindow           // [in]    used to set where requests go
);

#endif       // __VLIST_H
