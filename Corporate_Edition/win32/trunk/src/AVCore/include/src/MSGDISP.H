/****************************************************************************
// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
** Copyright 1991 - 2003, 2005 Symantec Corporation. All rights reserved.
**
** Module name: MsgDisp.H
**
** $Header:   S:/INCLUDE/VCS/MSGDISP.H_v   1.3   01 Aug 1997 14:15:34   MBROWN  $
**
** Description:
**    NOTE: This file completely includes what was the contents of EMSG.H
**          and NLOOK.H.  This file should be used instead of either EMSG.H
**          or NLOOK.H as they will be going away.
**
**
**    This file supplies all definitions and prototypes necessary for
**    using the message display functions in SYMUTIL.DLL.  There are also
**    several variants of the message/error display functions/macros and
**    related error/string table utility macros/functions.
**
**    Windows.H must be included before this file (Use PLATFORM.H).
**
*****************************************************************************/
/* $Log:   S:/INCLUDE/VCS/MSGDISP.H_v  $*/
// 
//    Rev 1.3   01 Aug 1997 14:15:34   MBROWN
// Increased MESSAGETEXTLEN from 520 to 2048 to fix bug in NDD, which uses a
// 2K message buffer.
//
//    Rev 1.2   21 Mar 1997 18:56:20   bsobel
// Second pass Alpha changes
//
//    Rev 1.1   21 Mar 1997 16:17:46   bsobel
// Changed params for NMsgSubst so it more it was portable to Alpha
//
//    Rev 1.0   26 Jan 1996 20:14:22   RJACKSO
// Initial revision.
//
//    Rev 1.14   24 Oct 1995 13:36:50   ASUZDALT
//
//    Rev 1.13   22 Aug 1994 11:47:38   JMILLARD
// fix comment on define which causes expansion problem for Watcom compiler
//
//    Rev 1.12   25 May 1994 15:30:00   GVOGEL
// Added @Library global for Geologist project.

/*@Library: SYMUTIL*/

#ifndef _MSGDISP_H
#define _MSGDISP_H


#ifndef _NLOOK_H
#define _NLOOK_H

#ifdef __cplusplus
extern "C" {
#endif

#ifndef _INC_STDARG
#include <stdarg.h>
#endif

/*  Standard Style value for all Symantec Dialog boxes                    */

#ifndef NDLG_STYLE
#define NDLG_STYLE  WS_CAPTION | WS_SYSMENU | WS_POPUP | DS_MODALFRAME | DS_SETFONT
#endif

/*  Maximum size for messages */
#define  BUTTONTEXTLEN  22              // max chars in button text including '&' and '\0'
#define  MESSAGETEXTLEN 2048            // max message text allowed
#define  CAPTIONTEXTLEN 80              // max caption text length
#define  ERRORTEXTLEN   200             // max extra error text length

/*  Flag extensions for messages */
#define MB_YESALLNOCANCEL  0x0006
#define IDYESTOALL         8


void SYM_EXPORT WINAPI   ShadowDlgCreate(HWND hDlg);
VOID SYM_EXPORT WINAPI   ShadowWindow(HWND,HWND);    /* window, shadowwindow */

#define MoveShadow(x) ShadowWindow(x,GetNextWindow(x,GW_HWNDPREV))

BOOL SYM_EXPORT WINAPI CenterPopup(HWND hWnd, HWND hParentWnd);

#define STARTWAIT() SetCursor(LoadCursor(NULL,IDC_WAIT))
#define ENDWAIT(x)  SetCursor(x)

WORD SYM_EXPORT WINAPI MessageDisplay(
    HWND           hWndParent,          // Parent window
    LPSTR          message,             // Message text
    LPCSTR          caption,            // Dialog box caption
    WORD           flags,               // Message flags, these flags are the
					// same flags used with the windows
					// MessageBox routine
    LPSTR          errortext,           // This is a special window
					// that will show up only if this
					// value is NOT NULL
    LPSTR FAR     *text);               // Button text, This is an array of
					// pointers to strings these strings
					// will be used instead of the normal
					// strings.  The number of strings
					// passed will be the number of
					// buttons in the messagebox

WORD SYM_EXPORT FAR cdecl NMessageDisplay(
	 HINSTANCE hInst,
	 HWND   hWndParent,     // Parent window
	 WORD   wIDMessage,     // Message text ID, 0 for NULL
	 WORD   wIDCaption,     // Dialog box caption ID, 0 for NULL
	 WORD   wFlags,         // MessageBox() flags
	 WORD   wIDErrorText,   // Error text ID, 0 for NULL
	 ...    );              // 0 to 4 button IDs of type WORD, ended by 0

#endif /* ifndef _NLOOK_H */

WORD SYM_EXPORT FAR cdecl MessageDisplayEx(
    HINSTANCE  hInst,       // Instance for resource IDs (may be NULL if no %r)
    HWND       hWndParent,  // Parent window
    LPCSTR     lpszMsgFmt,  // Message text: format string, the parameters are
			    // passed at the end of this parameter list
    LPSTR      lpszCaption, // Dialog box caption
    WORD       wFlags,      // Message flags, same as windows MessageBox proc
    LPSTR      lpszErrTxt,  // This is a special window that will show up only
			    // if this value is NOT NULL
    LPSTR FAR  *text,       // Button text, This is an array of pointers to
			    // strings and will be used instead of the normal
			    // button strings.  The number of strings passed
			    // will be the number of buttons in the messagebox
    ... );                  // Arguments for message parameter substitution

WORD SYM_EXPORT FAR cdecl NMessageDisplayEx(
	 HINSTANCE hInst,      // Instance for module which contains the string resources
	 HWND      hWndParent, // Parent window
	 WORD      wIDMsgFmt,  // Message format text ID, 0 for NULL
	 WORD      wIDCaption, // Dialog box caption ID, 0 for NULL
	 WORD      wFlags,     // MessageBox() flags
	 WORD      wIDErrTxt,  // Error text ID, 0 for NULL
	 LPWORD    lpwIDButton,// 0 to 4 button ID array of type WORD, ended by 0
	 ...   );              // Arguments for parameter substitution: wIDMessage

int SYM_EXPORT FAR cdecl Nwsprintf(
   HINSTANCE hInst,       // Instance for module which contains the string resources
			  // may be NULL is no string resources are indicated
   LPSTR     lpszOutput,  // Output buffer for resulting string
   LPCSTR    lpszFormat,  // Format control string
   ...);                  // Zero or more optional arguments determined by the
			  // format string.

// You must include stdarg.h to use this function
//#ifdef _INC_STDARG
int SYM_EXPORT WINAPI Nwvsprintf(
   HINSTANCE hInst,       // Instance for module which contains the string resources
			  // may be NULL is no string resources are indicated
   LPSTR     lpszOutput,  // Output buffer for resulting string
   LPCSTR    lpszFormat,  // Format control string
   va_list   args);       // Variable arguments which are determined by the format string
// LPWORD    lpwArgList); // Points to an array of variable arguments
                          // which are determined by the format string
//#endif

WORD SYM_EXPORT WINAPI DisplayExtError(     // Display error message related to
					// extended error.
    LPWORD lpwErr,                      // If NULL, or if contents are 0, then
					// we get the error.  If non-null and
					// contents are 0, we return the
					// extended error here.  If non-null
					// and non-zero, then the contents
					// are used as the extended error.
    LPCSTR lpString);                   // Optional error string to be
					// displayed in addition to the
					// extended error's string.


WORD SYM_EXPORT FAR cdecl MsgDisplay (
    HINSTANCE   hInst,      // [in] Instance for module which contains the string resources
    HWND        hWndParent, // [in] Parent window
    LPCSTR      lpMsgFmt,   // [in] Message format text or ID, 0 for NULL via MAKEINTERESOURCE
    LPCSTR      lpCaption,  // [in] Dialog box caption text or ID, 0 for NULL via MAKEINTERESOURCE
    UINT        wFlags,     // [in] MessageBox() flags
    ...   );                // [in] Arguments for parameter substitution: lpMessage



/***********************************************************************
**
**   An error code is a 16 bit code (left/high to right/low):
**   First two bits are the severity (fatal, error, or warning).
**   Next six bits indicate a string table in the resource DLL (an index).
**   The low byte is an index into the string table for a particular message.
**   The table and message index portions together are the error number.
**
**   An error code with no bits set (0) indicates no error.
**
**   Trace IDss are used in addition to the error code to uniquely
**   identify the generator of a particular error.  Each particular call
**   to NMsgBox() should have an unique ID (the dwTraceID parameter).
**   Each file will have an unique ID which can be used with the
**   ERR_TRACE_ID() macro to generate a unique ID.
**                   (More on this to come...)
**
** Macros:
**  ERR_GET_SEV(wErrorCode)            Returns the severity portion of
**                                     wErrorCode.  The severity can be
**                                     one of: ERR_SEV_FATAL,
**                                     ERR_SEV_ERROR, or ERR_SEV_WARNING.
**  ERR_SET_SEV(wErrorCode, wSeverity) Sets the severity of wErrorCode to
**                                     wSeverity
**  ERR_IS_ERROR(wErrorCode)           Returns TRUE if wErrorCode has
**                                     severity of ERROR or FATAL.
**
**  ERR_GET_TBL(wErrorCode)            Returns the table index portion of
**                                     wErrorCode (0 to 63)
**  ERR_SET_TBL(wErrorCode, wTblIndex) Sets the table index portion of
**                                     wErrorCode to wTblIndex
**
**  ERR_GET_MSG(wErrorCode)            Returns the message index portion of
**                                     wErrorCode (0 to 255)
**  ERR_SET_MSG(wErrorCode, wMsgIndex) Sets the message index portion of
**                                     wErrorCode to wMsgIndex
**
**  ERR_GET_NUM(wErrorCode)            Returns the error number portion of
**                                     wErrorCode (0 to 16383).  This is
**                                     the combination of the msg and tbl bits
**  ERR_SET_NUM(wErrorCode, wErrNum)   Sets the error number portion of
**                                                                                                      wErrorCode to wErrNum
**                                                                                                      wErrorCode to wErrNum
**  The following macros are used for generating error codes.  They must                               wErrorCode to wErrNum
**  be used as rvalues.                                                                                 wErrorCode to wErrNum
**
**  ERR_NUM(wTbl, wMsg)                Constructs the error number portion
**                                     only from the table and message
**  ERR_SEV_NUM(wSev, wErrNum)         Constructs a complete error from
**                                     the severity and the error number
**  ERR_SEV_TBL_MSG(wSev, wTbl, wMsg)  Constructs a complete error from
**                                     the basic components
** See Also:
**
** Future Enhancements:
**
** Processor:  Microsoft C 6.0
** Operating System:  MS-Windows 3.0
**********************************************************************/
#ifndef _EMSG_INCLUDED
#define _EMSG_INCLUDED


/**********************************************************************
** The following is for when no error condition exists and the function
** is returning normally.
**********************************************************************/
#ifndef NOERR
   #define NOERR        0
#endif

/**********************************************************************
** The following macro can be used to generate a unique ID for a call
** to NMsgBox().  The wFileID parameter should be an identifier defined
** in fileids.h for the file from which the call to NMsgBox() is made.
** The wFileID goes into the high word, and the line number goes into
** the low word.
**********************************************************************/
#define ERR_TRACE_ID(wFileID) (MAKELONG(__LINE__, wFileID))


/**********************************************************************
** Bit masks and macros for decoding/encoding error messages.
**********************************************************************/

/********************** ERR_SEV_BITS **********************************/
#define ERR_SEV_BITS    0xC000  /* Mask which gives the severity bits */
   /*
   ** The following macro can be used to assign the severity bits
   ** within an error code by coding:
   ** ERR_SET_SEVERITY(wErrorCode, wSeverity);
   ** Where wSeverity is one of:
   ** ERR_SEV_FATAL, ERR_SEV_ERROR, or ERR_SEV_WARNING (see below).
   */
#define ERR_SET_SEV(wErrorCode, wSeverity) \
   (wErrorCode |= wSeverity)

#define ERR_GET_SEV(wErrorCode) ((wErrorCode) & ERR_SEV_BITS)
   /*
   ** ERR_GET_SEV(wErrorCode) will equal one of the following
   ** severity codes:
   */
#define ERR_SEV_FATAL   0xC000  /* Code & ERR_SEV_FATAL is TRUE if FATAL */
#define ERR_SEV_ERROR   0x8000  /* Code & ERR_SEV_ERROR is TRUE if ERROR */
#define ERR_SEV_WARNING 0x4000  /* Code & ERR_SEV_WARNING is TRUE if WARNING */

   /*
   ** The following macro returns TRUE if the wErrorCode has severity of
   ** FATAL or ERROR.
   */
#define ERR_IS_ERROR(wErrorCode) (ERR_GET_SEV(wErrorCode) > ERR_SEV_WARNING)

/********************** ERR_TBL_BITS **********************************/
#define ERR_TBL_BITS    0x3F00          /* Mask which gives the table index bits */
#define ERR_TBL_MAX     0x3F            /* Maximum table index                  */
   /*
   ** The following macro returns the table index (0 to 63) from the code
   */
#define ERR_GET_TBL(wErrorCode) (HIBYTE((wErrorCode) & ERR_TBL_BITS))
   /*
   ** The following macro can be used to set the table index bits of
   ** wErrorCode to the index wIndex (0 to 63).
   ** This is accomplished by shifting the low byte of the index
   ** over to the hi byte position, and-ing that with the ERR_TBL_BITS
   ** to mask out any extra bits, and finaly or-ing the result with
   ** the error code to set bits 0-3 of the hi byte of the wErrorCode.
   */
#define ERR_SET_TBL(wErrorCode, wTblIndex) \
   (wErrorCode |= (((wTblIndex) << 8) & ERR_TBL_BITS))

/********************** ERR_MSG_BITS **********************************/
#define ERR_MSG_BITS    0x00FF          /* Mask which gives the message index bits*/
#define ERR_MSG_MAX     ERR_MSG_BITS    /* Max msg index                 */
   /*
   ** The following macro returns the error message index (0 to 255)
   */
#define ERR_GET_MSG(wErrorCode) ((wErrorCode) & ERR_MSG_BITS)
   /*
   ** The following macro sets the message index bits of the wErrorCode
   ** to the index wMsg (0 to 255)
   */
#define ERR_SET_MSG(wErrorCode, wMsgIndex) \
   (wErrorCode |= ((wMsgIndex) & ERR_MSG_BITS))

/********************** ERR_NUM_BITS **********************************/
   /*
   ** The ERR_NUM_BITS are a combination of the tbl and msg bits
   ** (which are all but the severity bits).
   */
#define ERR_NUM_BITS (~ERR_SEV_BITS)    /* Mask which gives number bits  */
#define ERR_NUM_MAX  ERR_NUM_BITS       /* Max error number              */
   /*
   ** The following macro returns error number portion of wErrorCode
   */
#define ERR_GET_NUM(wErrorCode) ((wErrorCode) & ERR_NUM_BITS)
   /*
   ** The following macro sets the error number bits of wErrorCode
   ** to wErrNum.
   */
#define ERR_SET_NUM(wErrorCode, wErrNum) \
   (wErrorCode |= ((wErrNum) & ERR_NUM_BITS))

   /*
   ** The following macro
   ** constructs the error number portion from
   ** only from the table (0-63) and message number (0-255) portions.
   */
#define ERR_NUM(wTblIndex, wMsgIndex) \
   ((((wTblIndex) << 8) & ERR_NUM_BITS) | ((wMsgIndex) & ERR_MSG_BITS))

   /*
   ** The following macro returns a complete error number constructed
   ** from the severity and the error number portions
   */
#define ERR_SEV_NUM(wSeverity, wErrNum) \
   ((wSeverity) | ((wErrNum) & ERR_NUM_BITS))

   /*
   ** The following macro returns a complete error number constructed
   ** from the severity, the table index, and the message index portions.
   */
#define ERR_SEV_TBL_MSG(wSev, wTbl, wMsg) \
   ((wSev) | ERR_NUM(wTbl, wMsg))

/**********************************************************************
** The following macro can be used to construct a complete error number
** from a severity code and error message number in much fewer
** characters than with the ERR_SEV_NUM() macro.
**
** Combined with the use of a TRACE definition such as "#define TRACE
** ERR_TRACE_ID(_FILE_C)" at the top of a source file, this can simplify
** an NMsgErrBox() call to the point where it will fit on one line of
** source code.  For example, the traditional
**
**  NMsgBox (hWnd, ERR_SEV_NUM(ERR_SEV_FATAL, ERR_FILE_OPEN),
**            ERR_TRACE_ID(_FILE_C), (LPSTR)NULL, -1, (LPSTR) "testfile");
**
** becomes
**
**  NMsgErrBox (hWnd, MKERR(FATAL, ERR_FILE_OPEN), TRACE, (LPSTR) "testfile");
**********************************************************************/

#define MKERR(sev,type)   (ERR_SEV_NUM(ERR_SEV_##sev, type))

/**********************************************************************
** Function prototypes: (MsgDisp.DLL)
**********************************************************************/

int SYM_EXPORT FAR _cdecl NMsgErrBox (      // Display error message
   HINSTANCE hInst,                     // where to load string from
   HWND      hWnd,                      // Parent window (or NULL)
   WORD      wErrorCode,                // Error code
   DWORD     dwTraceID,                 // Unique ID of source of error
   ...);                                // FAR String(s) for subst'n in msg

int SYM_EXPORT FAR _cdecl NMsgBox(          // Display error message, customized
   HINSTANCE hInst,                     // where to load string from
   HWND      hWnd,                      // Parent window (or NULL)
   WORD      wErrorCode,                // Error code
   DWORD     dwTraceID,                 // Unique ID of source of error
   LPCSTR    lpszUCaption,              // User supplied cap, or NULL 4 def
   int       nUStyle,                   // User supplied style or -1 for deflt
   ...);                                // FAR String(s) for subst in msg

int SYM_EXPORT FAR _cdecl NMsgGet(          // Retrieve message
   HINSTANCE hInst,                     // where to load string from
   WORD      wMsgCode,                  // String ID
   LPSTR     lpszOutput,                // Place to return message
   WORD      wOutputLen,                // Length of lpszOutput buffer
   ...);                                // FAR String(s) for subst in msg

// You must include stdarg.h to use this function
//#ifdef _INC_STDARG
int SYM_EXPORT WINAPI NMsgSubst(            // Perform substitutions on %n strings
   LPSTR     szInput,                   // string containing %ns for subst
   va_list args,
//   LPSTR FAR *lplpszInsertStrs,         // Array of 10 strings to be subst'ed
   LPSTR     szOutput,                  // Place to return result
   WORD      wOutputLen);               // Length of output buffer
//#endif
					// Debugging functions...
void errSetResSrc(                      // Set src of res for error strings
   HANDLE    hResInst);                 // Handle to inst to retrieve resrces

#endif /* ifndef _EMSG_INCLUDED */

#ifdef __cplusplus
}
#endif


#endif /* ifndef _MSGDISP_H */
