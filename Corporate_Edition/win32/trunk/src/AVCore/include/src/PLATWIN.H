// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
/* Copyright 1992 - 2003, 2005 Symantec Corporation. All rights reserved.     */
/************************************************************************
 *                                                                      *
 * $Header:   S:/INCLUDE/VCS/PLATWIN.H_v   1.0   26 Jan 1996 20:16:02   RJACKSO  $
 *                                                                      *
 * Description:                                                         *
 *      Supplements Windows.h and Windowsx.h.  Contains stuff that most *
 *      Windows apps need.                                              *
 *                                                                      *
 * See Also:                                                            *
 *                                                                      *
 ************************************************************************
 * $Log:   S:/INCLUDE/VCS/PLATWIN.H_v  $ *
// 
//    Rev 1.0   26 Jan 1996 20:16:02   RJACKSO
// Initial revision.
// 
//    Rev 1.22   17 Jan 1995 16:49:54   HENRI
// Double oops
// 
//    Rev 1.21   17 Jan 1995 16:47:52   HENRI
// Oops
// 
//    Rev 1.20   17 Jan 1995 16:44:00   HENRI
// Changed SYMSYSHEAPINFO to use 16 bit handles rather than 32 bit handles
// 
//    Rev 1.19   13 Dec 1994 13:21:14   BILL
// Changed SYSTEMHEAPINFO to SYMSYSTEMHEAPINFO to avoid conflicts
// 
//    Rev 1.18   12 Dec 1994 08:29:00   MARKK
// Ifdef'd the extern C's Bill
// 
//    Rev 1.17   09 Dec 1994 20:37:20   BILL
// extern "C"'d the new functions
// 
//    Rev 1.16   09 Dec 1994 20:02:44   BILL
// Moved stuff from toolhelp.h on win32 side into here
// 
//    Rev 1.15   27 Oct 1994 15:56:02   BRAD
// Removed redefinition of AnsiNext(),AnsiPrev(), and IsAnsiChar()
// 
//    Rev 1.14   26 Apr 1994 16:35:46   BRAD
// Removed PVCS Logs
 ************************************************************************/

#ifndef INCL_PLATWIN
#define INCL_PLATWIN

//***********************************************************************
// These macros are like the ones in Windowsx.h.  They "hide" the
// details as to implementation and also provide portability between
// 16-bit and 32-bit.
//***********************************************************************

//***********************************************************************
// WIN32 stuff
//***********************************************************************

#if defined(SYM_WIN32)
// This replaces the SystemHeapInfo structure from Toolhelp for thunking
// reasons (conflicts were created when we refered to the function by
// the same name but without the dllexport)

typedef WORD HGLOBAL16;
typedef struct tagSYMSYSHEAPINFO
{
    DWORD dwSize;
    WORD wUserFreePercent;
    WORD wGDIFreePercent;
    HGLOBAL16 hUserSegment;             // 16 bit selector
    HGLOBAL16 hGDISegment;              // 16 bit selector 
    HGLOBAL16 hWndSegment;              // 16 bit selector 
    HGLOBAL16 hMenuSegment;             // 16 bit selector 
    HGLOBAL16 hGDI32Segment;            // 16 bit selector 
} SYMSYSHEAPINFO;

#ifdef __cplusplus
extern "C" {
#endif
BOOL __declspec(dllexport) WINAPI SymSystemHeapInfo(SYMSYSHEAPINFO FAR* lpSysHeap);
WORD __declspec(dllexport) WINAPI SymGetFreeSystemResources(WORD fuSysResource);
#ifdef __cplusplus
}
#endif

#define     GetClassIcon(hwnd)          ((HICON)GetClassLong((hwnd), GCL_HICON))
#define     GetClassStyle(hwnd)         ((UINT)GetClassLong((hwnd), GCL_STYLE))
#define     GetClassWndExtra(hwnd)      ((int)GetClassLong((hwnd), GCL_CBWNDEXTRA))
#define     GetClassBackground(hwnd)    ((HBRUSH)GetClassLong((hwnd), GCL_HBRBACKGROUND))
#define     SetClassIcon(hwnd,icon)     (SetClassLong((hwnd), GCL_HICON, (LONG)(icon)))
#define     SetClassStyle(hwnd,style)   (SetClassLong((hwnd), GCL_STYLE, (LONG)(style)))
#define     SetClassCursor(hwnd,cursor) (SetClassLong((hwnd), GCL_HCURSOR, (LONG)(cursor)))
#define     SetClassBackground(hwnd,bg) (SetClassLong((hwnd), GCL_HBRBACKGROUND, (LONG)(bg)))

#define     SetWindowID(hwnd,id)        (SetWindowLong((hwnd), GWL_ID, (LONG)(id)))

                                        // They "forgot" this
typedef RGBTRIPLE FAR* LPRGBTRIPLE;

/*----------------------------------------------------------------------*/
/* This macro was in Win 3.1, but removed from Win32
/*----------------------------------------------------------------------*/

#if !defined(HANDLE_WM_CTLCOLOR)
/* HBRUSH Cls_OnCtlColor(HWND hwnd, HDC hdc, HWND hwndChild, int type); */
#define HANDLE_WM_CTLCOLOR(hwnd, wParam, lParam, fn) \
    (LRESULT)(DWORD)(UINT)(HBRUSH)(fn)((hwnd), (HDC)(wParam), (HWND)LOWORD(lParam), (int)(HIWORD(lParam)))
#define FORWARD_WM_CTLCOLOR(hwnd, hdc, hwndChild, type, fn) \
    (HBRUSH)(UINT)(DWORD)(fn)((hwnd), WM_CTLCOLOR, (WPARAM)(HDC)(hdc), MAKELPARAM((UINT)(HWND)(hwndChild), (UINT)(int)(type)))
#endif

// We copied these here, since they make our DllMain easier to maintain
/* Windows Exit Procedure flag values */
#define	WEP_SYSTEM_EXIT	1
#define	WEP_FREE_DLL	0


#else   // WIN16

//***********************************************************************
// WIN16 stuff
//***********************************************************************

#define     GetClassIcon(hwnd)          ((HICON)GetClassWord((hwnd), GCW_HICON))
#define     GetClassStyle(hwnd)         ((UINT)GetClassWord((hwnd), GCW_STYLE))
#define     GetClassWndExtra(hwnd)      ((int)GetClassWord((hwnd), GCW_CBWNDEXTRA))
#define     GetClassBackground(hwnd)    ((HBRUSH)GetClassWord((hwnd), GCW_HBRBACKGROUND))
#define     SetClassIcon(hwnd,icon)     (SetClassWord((hwnd), GCW_HICON, (WORD)(icon)))
#define     SetClassStyle(hwnd,style)   (SetClassWord((hwnd), GCW_STYLE, (WORD)(style)))
#define     SetClassCursor(hwnd,cursor) (SetClassWord((hwnd), GCW_HCURSOR, (WORD)(cursor)))
#define     SetClassBackground(hwnd,bg) (SetClassWord((hwnd), GCW_HBRBACKGROUND, (WORD)(bg)))

#define     SetWindowID(hwnd,id)        (SetWindowWord((hwnd), GWW_ID, (WORD)(id)))


//////////////////////////////////////////////////////////////////////////////
// Dialog Template stuff.  Not documented in WIN16 and also slightly different.
//////////////////////////////////////////////////////////////////////////////
typedef struct {
    DWORD style;
    BYTE  cdit;
    short x;
    short y;
    short cx;
    short cy;
   /* char szMenuName[];        // Variable-length string. */
   /* char szClassName[];       // Variable-length string. */
   /* char szCaptionText[];     // Variable-length string. */
} DLGTEMPLATE;
typedef DLGTEMPLATE *PDLGTEMPLATE;
typedef DLGTEMPLATE FAR *LPDLGTEMPLATE;
typedef const DLGTEMPLATE FAR *LPCDLGTEMPLATE;

/*
 * Dialog item template (dit)
 */
typedef struct {
    short x;
    short y;
    short cx;
    short cy;
    WORD  id;
    DWORD style;
   /* char dtilClass[];         // Variable-length string. */
   /* char dtilText[];          // Variable-length string. */
   /* BYTE dtilInfo;            // # of bytes in following memory block. */
   /* BYTE dtilData;            // Variable-length memory block. */
} DLGITEMTEMPLATE;
typedef DLGITEMTEMPLATE *PDLGITEMTEMPLATE;
typedef DLGITEMTEMPLATE FAR *LPDLGITEMTEMPLATE;

//////////////////////////////////////////////////////////////////////////////
// They added this to WIN32, but undocumented in WIN 3.1
//////////////////////////////////////////////////////////////////////////////
typedef struct tagACCEL
    {
    BYTE    fVirt;
    WORD    key;
    WORD    cmd;
    } ACCEL, *PACCEL, FAR *LPACCEL;


//////////////////////////////////////////////////////////////////////////////
// They added this to WIN32, but undocumented in WIN 3.1
//////////////////////////////////////////////////////////////////////////////
#ifndef WM_ENTERMENULOOP
#   define WM_ENTERMENULOOP                0x0211
#endif
#ifndef WM_EXITMENULOOP
#   define WM_EXITMENULOOP                 0x0212
#endif

//////////////////////////////////////////////////////////////////////////////
// Declare Windows global selectors
//////////////////////////////////////////////////////////////////////////////
#ifdef __cplusplus
    extern "C" {
#endif

 extern  _cdecl near    _0000H;
 extern  _cdecl near    _0040H;
 extern  _cdecl near    _A000H;
 extern  _cdecl near    _B000H;
 extern  _cdecl near    _B800H;
 extern  _cdecl near    _C000H;
 extern  _cdecl near    _D000H;
 extern  _cdecl near    _E000H;
 extern  _cdecl near    _F000H;

#ifdef __cplusplus
    }
#endif

#define SEG_0000H       (&_0000H)
#define SEG_0040H       (&_0040H)
#define SEG_A000H       (&_A000H)
#define SEG_B000H       (&_B000H)
#define SEG_B800H       (&_B800H)
#define SEG_C000H       (&_C000H)
#define SEG_D000H       (&_D000H)
#define SEG_E000H       (&_E000H)
#define SEG_F000H       (&_F000H)

/*----------------------------------------------------------------------*/
/* Prototypes not defined in WINDOWS.H, but exported from USER, KERNEL,	*/
/*	or GDI.								*/
/*----------------------------------------------------------------------*/

#ifdef __cplusplus
extern "C" {
#endif

VOID    WINAPI DOS3Call(VOID);
LPSTR   WINAPI GetDOSEnvironment(VOID);
WORD    WINAPI GetHeapSpaces(HANDLE);
BOOL    WINAPI IsWinOldApTask(HTASK hTask);

/*
    The following routines are used to get and set the internal ICON
    park position.  They were hacked out.  They have been tested with
    Windows 3.0 and Windows 3.1 (alpha).  They *may* be incompatable with
    future Windows versions.

    wShow takes the ShowWindow parameters: SW_HIDE
                                           SW_SHOWNORMAL
                                           SW_NORMAL
                                           SW_SHOWMINIMIZED

    The LPRECT is a long pointer to a window rect.
    The LPPOINT is a long pointer to the icon park position.

    Either may be NULL to not change current values.
*/
WORD WINAPI SetInternalWindowPos(HWND hWnd, WORD wShow, LPRECT rc, LPPOINT pt);
WORD WINAPI GetInternalWindowPos(HWND hWnd, LPRECT rc, LPPOINT pt);

#define SIWP_TWIGHLIGHTZONE -32767

void WINAPI SwitchToThisWindow(HWND hWnd, BOOL bRestore);
HWND WINAPI GetNextQueueWindow(HWND hWNd, int nDirection);
void WINAPI RepaintScreen(void);
DWORD WINAPI GlobalHandleNoRip(UINT);
VOID WINAPI FillWindow(HWND parent, HWND tager, HDC hdc, HBRUSH brush);

#ifdef __cplusplus
}                                       // End of extern "C" prototypes.
#endif


/****** Alternate porting layer macros ****************************************/
// MS put these in WIN32's WINDOWSX.H, but not in the 16-bit version.  We
// cut them out and fixed them up for WIN16.
//////////////////////////////////////////////////////////////////////////////

/* USER MESSAGES: */

#define GET_WPARAM(wParam,lParam)                   (wParam)
#define GET_LPARAM(wParam,lParam)                   (lParam)

#define GET_WM_ACTIVATE_STATE(wParam,lParam)        (wParam)
#define GET_WM_ACTIVATE_FMINIMIZED(wParam,lParam)   (BOOL)HIWORD(lParam)
#define GET_WM_ACTIVATE_HWND(wParam,lParam)         (HWND)LOWORD(lParam)
#define GET_WM_ACTIVATE_MPS(s, fmin, hwnd)   \
        (WPARAM)(UINT)(s), MAKELPARAM((UINT)(HWND)(hwnd), (UINT)(BOOL)(fmin))

#define GET_WM_CHARTOITEM_CHAR(wParam,lParam)       (CHAR)(wParam)
#define GET_WM_CHARTOITEM_POS(wParam,lParam)        HIWORD(lParam)
#define GET_WM_CHARTOITEM_HWND(wParam,lParam)       (HWND)LOWORD(lParam)
#define GET_WM_CHARTOITEM_MPS(ch, pos, hwnd) \
        (WPARAM)(UINT)(ch), MAKELPARAM((UINT)(hwnd), (UINT)(pos))

#define GET_WM_COMMAND_ID(wParam,lParam)            (wParam)
#define GET_WM_COMMAND_HWND(wParam,lParam)          (HWND)LOWORD(lParam)
#define GET_WM_COMMAND_CMD(wParam,lParam)           HIWORD(lParam)
#define GET_WM_COMMAND_MPS(id, hwnd, cmd)    \
        (WPARAM)(int)(id), MAKELPARAM((UINT)(hwnd), (cmd))

#define GET_WM_CTLCOLOR_HDC(wParam,lParam,msg)      (HDC)(wParam)
#define GET_WM_CTLCOLOR_HWND(wParam,lParam,msg)     (HWND)LOWORD(lParam)
#define GET_WM_CTLCOLOR_TYPE(wParam,lParam,msg)     (WORD)HIWORD(lParam)
#define GET_WM_CTLCOLOR_MSG(type)                   (WORD)(WM_CTLCOLORMSGBOX+(type))
#define GET_WM_CTLCOLOR_MPS(hdc, hwnd, type) \
        (WPARAM)(HDC)(hdc), MAKELPARAM((UINT)(HWND)(hwnd), (UINT)(int)(type))

#define GET_WM_MENUSELECT_CMD(wParam,lParam)       (wParam)
#define GET_WM_MENUSELECT_FLAGS(wParam,lParam)     (UINT)(int)(short)LOWORD(lParam)
#define GET_WM_MENUSELECT_HMENU(wParam,lParam)     (HMENU)HIWORD(lParam)
#define GET_WM_MENUSELECT_MPS(cmd, f, hmenu)  \
        ((f) & MF_POPUP) ? (WPARAM)(HMENU)(hmenu) : (WPARAM)(int)(cmd), MAKELPARAM((f), (hmenu))

/* Note: the following are for interpreting MDIclient to MDI child messages. */
#define GET_WM_MDIACTIVATE_FACTIVATE(hwnd,wParam,lParam)  (BOOL)(wParam)
#define GET_WM_MDIACTIVATE_HWNDDEACT(wParam,lParam)       (HWND)HIWORD(lParam)
#define GET_WM_MDIACTIVATE_HWNDACTIVATE(wParam,lParam)    (HWND)LOWORD(lParam)
/* Note: the following is for sending to the MDI client window. */
// Note: This doesn't change between 16 and 32-bit.
//#define GET_WM_MDIACTIVATE_MPS(f, hwndD, hwndA)  (WPARAM)(hwndA), 0

#define GET_WM_MDISETMENU_MPS(hmenuF, hmenuW) \
        (WPARAM)(0), MAKELPARAM((hmenuF), (hmenuW))

#define GET_WM_MENUCHAR_CHAR(wParam,lParam)         (CHAR)(wParam)
#define GET_WM_MENUCHAR_HMENU(wParam,lParam)        (HMENU)HIWORD(lParam)
#define GET_WM_MENUCHAR_FMENU(wParam,lParam)        (BOOL)LOWORD(lParam)
#define GET_WM_MENUCHAR_MPS(ch, hmenu, f)    \
        (WPARAM)(UINT)(ch), MAKELPARAM((f), (UINT)(hmenu))

#define GET_WM_PARENTNOTIFY_MSG(wParam,lParam)      (wParam)
#define GET_WM_PARENTNOTIFY_ID(wParam,lParam)       HIWORD(lParam)
#define GET_WM_PARENTNOTIFY_HWNDCHILD(wParam,lParam) (HWND)LOWORD(lParam)
#define GET_WM_PARENTNOTIFY_X(wParam,lParam)        (int)(short)LOWORD(lParam)
#define GET_WM_PARENTNOTIFY_Y(wParam,lParam)        (int)(short)HIWORD(lParam)
#define GET_WM_PARENTNOTIFY_MPS(msg, id, hwnd) \
        (WPARAM)(UINT)(msg), MAKELPARAM((UINT)(HWND)(hwnd), (UINT)(id))
#define GET_WM_PARENTNOTIFY2_MPS(msg, x, y) \
        (WPARAM)(UINT)(msg), MAKELPARAM((UINT)(x), (UINT)(y))

#define GET_WM_VKEYTOITEM_CODE(wParam,lParam)       (int)(short)(wParam)
#define GET_WM_VKEYTOITEM_ITEM(wParam,lParam)       HIWORD(lParam)
#define GET_WM_VKEYTOITEM_HWND(wParam,lParam)       (HWND)LOWORD(lParam)
#define GET_WM_VKEYTOITEM_MPS(code, item, hwnd) \
        (WPARAM)(UINT)(code), MAKELPARAM((UINT)(hwnd), (UINT)(item))

#define GET_WM_CHANGECBCHAIN_HWNDNEXT(wParam,lParam) (HWND)LOWORD(lParam)

#define GET_WM_HSCROLL_CODE(wParam,lParam)          (wParam)
#define GET_WM_HSCROLL_POS(wParam,lParam)           LOWORD(lParam)
#define GET_WM_HSCROLL_HWND(wParam,lParam)          (HWND)HIWORD(lParam)
#define GET_WM_HSCROLL_MPS(code, pos, hwnd)    \
        (WPARAM)(UINT)(code), MAKELPARAM((pos), (UINT)(hwnd))

#define GET_WM_VSCROLL_CODE(wParam,lParam)          (wParam)
#define GET_WM_VSCROLL_POS(wParam,lParam)           LOWORD(lParam)
#define GET_WM_VSCROLL_HWND(wParam,lParam)          (HWND)HIWORD(lParam)
#define GET_WM_VSCROLL_MPS(code, pos, hwnd)    \
        (WPARAM)(UINT)(code), MAKELPARAM((pos), (UINT)(hwnd))

#endif

//***********************************************************************
// COMMON STUFF FOR WIN16 and WIN32 that was left out.
//***********************************************************************

/*----------------------------------------------------------------------*/
// Marks the end of an accelerator table.
/*----------------------------------------------------------------------*/
#define ACCEL_END           ((BYTE)0x80)

/*----------------------------------------------------------------------*/
/* Constants for processing the return values from IsDlgButtonChecked   */
/* and CheckDlgButton                                                   */
/*----------------------------------------------------------------------*/
#define BTN_UNCHECKED       0
#define BTN_CHECKED         1
#define BTN_GREY            2

/*----------------------------------------------------------------------*/
/*----------------------------------------------------------------------*/
typedef void (FAR PASCAL * DEVMODEPROC)(HWND,HINSTANCE,LPSTR,LPSTR);

#if 0
// For now, don't redefine.  See if anyone notices!  Also, permanently
// remove IsAnsiChar().  SHOULDN'T EVER BE USED!  BK.
/*----------------------------------------------------------------------*/
/* Covers for AnsiNext() and AnsiPrev() to speed things up in the       */
/* U.S. market since double-byte characters don't apply.                */
/*----------------------------------------------------------------------*/
#if !defined(DBYTECHARS)
    #undef AnsiNext
    #define AnsiNext(lp)      ((*((LPSTR)lp)!='\0') ? (((LPSTR)lp)+1) : ((LPSTR)lp))
    #undef AnsiPrev
    #define AnsiPrev(lpS,lpC) (((LPSTR)lpC>(LPSTR)lpS) ? (((LPSTR)lpC)-1) : ((LPSTR)lpS))

/*----------------------------------------------------------------------*/
/* IsAnsiChar() macro returns whether or not the current single byte    */
/* character is an Ansi character that is supported by Windows.  See    */
/* the ANSI Table found in Appendix D, SDK Reference Manual Vol 2.      */
/*----------------------------------------------------------------------*/
    #define IsAnsiChar(x)       (((((BYTE) x) > 31) && (((BYTE) x) < 127)) || ((((BYTE) x) > 159) && (((BYTE) x) <= 255)))
#endif
#endif

/*----------------------------------------------------------------------*/
/* This macro should be used to convert font points into logical units  */
/* for the mapping mode of the given dc.  USE THIS WHEN ASSIGNING       */
/* THE lfHeight FIELD OF THE LOGFONT STRUCTURE!!!                       */
/* Give the desired point size in the pt parameter.                     */
/*----------------------------------------------------------------------*/
#define PT_TO_PIXELS(hdc, pt)	\
    MulDiv(-(pt), GetDeviceCaps(hdc, LOGPIXELSY), 72)

#endif

