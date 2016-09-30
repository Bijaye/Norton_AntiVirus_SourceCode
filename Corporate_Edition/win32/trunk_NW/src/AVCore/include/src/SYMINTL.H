/************************************************************************/
// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
/* Copyright 1992 - 2003, 2005 Symantec Corporation. All rights reserved.     */
/************************************************************************/
/*                                                                      */
/* $Header:   S:/INCLUDE/VCS/symintl.h_v   1.3   04 Dec 1996 14:48:36   RCHINTA  $ */
/*                                                                      */
/* Description:                                                         */
/*                                                                      */
/* See Also:                                                            */
/*                                                                      */
/************************************************************************/
/* $Log:   S:/INCLUDE/VCS/symintl.h_v  $ */
// 
//    Rev 1.3   04 Dec 1996 14:48:36   RCHINTA
// Added prototypes for NIMMCreate() and NIMMDestroy() and fixed
// the problem mentioned in Rev 1.2 (below)
// 
//    Rev 1.2   25 Sep 1996 10:18:00   JALLEE
// Backed out Quake 9 port of NIMMCreate() and NIMMDestroy().  Was directly
// linking, IMM32 not available under NT 3.51.  These functions should return
// with LoadLibrary()/GetProcAddress() at some point in the near future.
// 
//    Rev 1.1   20 Sep 1996 18:05:26   JALLEE
// Added prototypes for NIMMCreate() and NIMMDestroy().
// 
//    Rev 1.0   26 Jan 1996 20:15:26   RJACKSO
// Initial revision.
// 
//    Rev 1.21   26 Apr 1994 16:36:36   BRAD
// Removed PVCS Logs
/************************************************************************/


#ifdef __cplusplus
extern "C" {
#endif

/*
**  Maximum lengths (including the terminating null) for international strings:
*/
#define INTL_MAXLEN_sDATE           2
#define INTL_MAXLEN_sSHORTDATE      16
#define INTL_MAXLEN_sLONGDATE       40
#define INTL_MAXLEN_sTIME           2
#define INTL_MAXLEN_sAMPM           9
#define INTL_MAXLEN_sTHOUSAND       2
#define INTL_MAXLEN_sDECIMAL        2
#define INTL_MAXLEN_sLIST           2
#define INTL_MAXLEN_sCURRENCY       9
/* Generic (assuming Long Date is the longest string) */
#define INTL_MAXLEN_KEYNAME         40
#define INTL_MAXLEN_KEYVALUE        INTL_MAXLEN_sLONGDATE


/*
**  Date formats:
*/
#define DATE_MDY            0
#define DATE_DMY            1
#define DATE_YMD            2
/*
**  Time formats:
*/
#define TIME_12_HRS         0
#define TIME_24_HRS         1
/*
**  Leading Zeros:
*/
#define LEADING_ZERO_NO     0
#define LEADING_ZERO_YES    1

/*
**  Return values for NGetDlgIntlDate() and NGetDlgIntlTime():
*/
#define NDT_VALID               1
#define NDT_NO_INPUT            0
#define NDT_INVALID             -1

/*
**  Default values for International Integers:
*/
#define INI_DEF_iDATE       DATE_MDY
#define INI_DEF_iTIME       TIME_12_HRS
#define INI_DEF_iTLZERO     LEADING_ZERO_NO
#define INI_DEF_iDIGITS     2
#define INI_DEF_iLZERO      LEADING_ZERO_YES
#define INI_DEF_iCURRENCY   0
#define INI_DEF_iCURRNEG    0
#define INI_DEF_iCURRDIGITS 2

/*
**  Enumeration values for NIntlGetString():
*/
#define INTL_ENUM_DATE      0
#define INTL_ENUM_SHORTDATE 1
#define INTL_ENUM_LONGDATE  2
#define INTL_ENUM_TIME      3
#define INTL_ENUM_AM        4
#define INTL_ENUM_PM        5
#define INTL_ENUM_THOUSAND  6
#define INTL_ENUM_DECIMAL   7
#define INTL_ENUM_CURRENCY  8

/*
**  Internationalization Function Prototypes:
*/
#if defined(SYM_WIN)
STATUS SYM_EXPORT WINAPI NIntlInit( LPSTR lpszSection );
#else
#  define NIntlInit(lpszSection)
#endif
void SYM_EXPORT WINAPI NGetIntlDate(LPSTR  lpBuffer,
                                LPUINT lpuiMonth,
                                LPUINT lpuiDay,
                                LPUINT lpuiYear);
void SYM_EXPORT WINAPI NSetIntlDate(LPSTR  lpBuffer,
                                UINT   uMonth,
                                UINT   uDay,
                                UINT   uYear);
#if defined(SYM_WIN)
int SYM_EXPORT WINAPI NGetDlgIntlDate(HWND   hDlg,
                                  UINT   wIDDInput1,
                                  LPUINT lpwMonth,
                                  LPUINT lpwDay,
                                  LPUINT lpwYear);
int SYM_EXPORT WINAPI NGetDlgIntlTime(HWND   hDlg,
                                  UINT   wIDDInput1,
                                  LPUINT lpwHours,
                                  LPUINT lpwMinutes,
                                  LPUINT lpwSeconds);
void SYM_EXPORT WINAPI NSetDlgIntlDate(HWND hDlg,
                                   UINT wIDDInput1,
                                   UINT wMonth,
                                   UINT wDay,
                                   UINT wYear,
                                   BOOL bExcludeCentury);
void SYM_EXPORT WINAPI NSetDlgIntlTime(HWND hDlg,
                                   UINT wIDDInput1,
                                   UINT wHours,
                                   UINT wMinutes,
                                   UINT wSeconds,
                                   BOOL bDispSeconds,
                                   BOOL bDispAMPM);
#elif defined(SYM_DOS)
int PASCAL NGetDlgIntlDate (DIALOGREC drDialog, void *pItem,
			    LPWORD lpwMonth, LPWORD lpwDay, LPWORD lpwYear);
int PASCAL NGetDlgIntlTime (DIALOGREC drDialog, void *pItem,
                                LPWORD lpwHours, LPWORD lpwMinutes,
                                LPWORD lpwSeconds);
void PASCAL NSetDlgIntlTime (DIALOGREC drDialog, void *pItem,
                             UINT uHours, UINT uMinutes, UINT uSeconds,
                             BOOL bDispSeconds, BOOL bDispAMPM);
void PASCAL NSetDlgIntlDate (DIALOGREC drDialog, void *pItem, 
			     UINT uMonth, UINT uDay, UINT uYear, 
			     BOOL bExcludeCentury);
#endif                                  // not SYM_WIN
void SYM_EXPORT WINAPI NSetIntlLongDate(LPSTR lpBuffer,
                                    UINT  wMonth,
                                    UINT  wDay,
                                    UINT  wYear,
                                    BOOL  bWeekday);
void SYM_EXPORT WINAPI NGetIntlTime(LPSTR  lpBuffer,
                                LPUINT lpuiHours,
                                LPUINT lpuiMinutes,
                                LPUINT lpuiSeconds);
void SYM_EXPORT WINAPI NSetIntlTime(LPSTR lpBuffer,
                                UINT  wHours,
                                UINT  wMinutes,
                                UINT  wSeconds,
                                BOOL  bDispSeconds,
                                BOOL  bDispAMPM);
STATUS SYM_EXPORT WINAPI NIntlFormatCurrency( LPSTR  lpszBuffer,
                                     double dValue,
                                     int    nPrecision,
                                     BOOL   bInsertThousandChar );
STATUS SYM_EXPORT WINAPI NIntlFormatFloat( LPSTR  lpszBuffer,
                                  double dValue,
                                  int    nPrecision,
                                  BOOL   bInsertThousandChar );
LPSTR SYM_EXPORT WINAPI NIntlFormatInteger( LPSTR lpBuffer,
                                     DWORD dwValue,
                                     int   nMinPlaces,
                                     char  cPaddingChar,
                                     BOOL  bInsertThousandChar );
BOOL SYM_EXPORT WINAPI NIsAmericanDate( void );
LPSTR SYM_EXPORT WINAPI NIntlGetString( LPSTR lpBuffer, UINT wEnum );

BOOL SYM_EXPORT WINAPI NIntlDateIsLegal (UINT wYear, UINT wMonth, UINT wDay);
BOOL SYM_EXPORT WINAPI NIntlTimeIsLegal (UINT wHour, UINT wMin, UINT wSec, BOOL b24Hour);

STATUS SYM_EXPORT WINAPI NIntlGetValidTime( // Gets valid time
    HWND        hWnd,                   // [in] Handle to time field
    LPUINT      lpwHour,                // [in/out] Hour
    LPUINT      lpwMin,                 // [in/out] Minutes
    BOOL        bDispAMPM);             // [in] TRUE if AM/PM displayed

STATUS SYM_EXPORT WINAPI NIntlGetValidDate( // Gets valid date
    HWND        hWnd,                   // [in] Handle to date field
    LPUINT      lpwMonth,               // [in/out] Month
    LPUINT      lpwDate,                // [in/out] Date
    LPUINT      lpwYear);               // [in/out] Year

LPSTR SYM_EXPORT WINAPI NIntlTimeToLongString(
    LPSTR       lpszBuffer, 
    UINT        uHour, 
    UINT        uMinute, 
    UINT        uSecond);

/*********************************************************************
 * Functions to enable/disable the IMM Input Method Manager.
 * Called from SET_FOCUS and KILL_FOCUS to turn off the IME.
 * - RowanT 10/17/95
 *********************************************************************/

HWND SYM_EXPORT WINAPI NImmCreate(HWND hWndOwner);
BOOL SYM_EXPORT WINAPI NImmDestroy(HWND hWndOwner);

#ifdef __cplusplus
}
#endif

