/************************************************************************/
/* Copyright 1992 Symantec, Peter Norton Computing Group                */
/************************************************************************/
/*                                                                      */
/* $Header:   S:/SYMKRNL/VCS/SYMINTL.C_v   1.8   08 May 1998 11:49:04   mdunn  $ */
/*                                                                      */
/* Description:                                                         */
/*                                                                      */
/* Contains:                                                            */
/*                                                                      */
/* See Also:                                                            */
/*                                                                      */
/************************************************************************/
/* $Log:   S:/SYMKRNL/VCS/SYMINTL.C_v  $ */
// 
//    Rev 1.8   08 May 1998 11:49:04   mdunn
// NSetIntlLongDate() now uses the GetDateFormat() API on Win32.
// 
//    Rev 1.7   12 Sep 1997 15:12:46   KSACKIN
// Ported changes from QAKG.
// 
//    Rev 1.6.1.1   23 Aug 1997 01:58:50   KSACKIN
// Reworked some of the logic in the NIntlGetTime function to always put the
// AM/PM specifier at the end of the string.  The reason we do this is that 
// on DBCS machines, we were not able to format our strings correctly because of
// bugs in the ConvertStringToDword function.  This will format our time to
// always have to AM/PM at the end, so this should never happen.  This fix is
// mainly for Japan and Korean, as their default is leading AM/PM.
//
//    Rev 1.6.1.0   12 Aug 1997 22:53:16   hkim
// Branch base for version QAKK
//
//    Rev 1.6   11 Jul 1997 15:46:22   KSACKIN
// Now allow a year of 0 when formatting a date.  This is for supporting the
// year 2000.  Also, now instead of if the year > 1900 subtracting 1900, we
// simply mod the year by 100.
//
//    Rev 1.5   04 Dec 1996 17:49:46   BMCCORK
// Commented out use of ImmGetDefaultIMEWnd in debug code - causes link error
//
//    Rev 1.4   04 Dec 1996 14:52:36   RCHINTA
// Added NIMMCreate() and NIMMDestroy() and fixed the problem
// mentioned in rev 1.2 (below)
//
//    Rev 1.3   23 Oct 1996 16:03:28   JBRENNA
// Port Changes from QAKF:
// -r1.2.1.1 JBRENNA:
// Update NSetIntlTime() to put the AmPm symbol in front of the time when
// the international information says that AmPm appears before the time.
// -r1.2.1.2 JBRENNA:
// 1. Modify NGetIntlTime() to pass the beginning of lpBuffer to AdjustHour()
//    when s_bAmPmFollow is FALSE (i.e. when the AmPm string is on the
//    beginning of the buffer).
// 2. Modify AdjustHours() to correctly process an AmPm with lpAMPM points
//    to the beginning of a string. Previously, it dependend on the AmPm string
//    being last.
//
//    Rev 1.2   25 Sep 1996 10:17:54   JALLEE
// Backed out Quake 9 port of NIMMCreate() and NIMMDestroy().  Was directly
// linking, IMM32 not available under NT 3.51.  These functions should return
// with LoadLibrary()/GetProcAddress() at some point in the near future.
//
//    Rev 1.1   20 Sep 1996 18:02:56   JALLEE
// DBCS port from Quake 9, added NIMMCreate() and NIMMDestroy.
//
//    Rev 1.0   26 Jan 1996 20:21:54   JREARDON
// Initial revision.
//
//    Rev 1.37   12 Sep 1995 13:33:02   HENRI
// Removed AM/PM string in 24 hour format time string
//
//    Rev 1.36   26 Jul 1995 15:22:20   BILL
// Merged branch changes
//
//    Rev 1.33.1.2   29 Jun 1995 09:06:30   MARKL
// Fixed syntax error for Win platform (it wouldn't compile).
//
//    Rev 1.33.1.1   28 Jun 1995 09:48:28   HENRI
// Use Win32 API to obtain month strings
//
//    Rev 1.33.1.0   27 Jun 1995 18:51:10   RJACKSO
// Branch base for version QAK8
//
//    Rev 1.33   22 Jun 1995 22:50:24   HENRI
// Fixed long date format routine
//
//    Rev 1.32   22 Jun 1995 19:27:00   HENRI
// Fixed day of week string for Win32
//
//    Rev 1.31   11 Jun 1995 17:36:40   HENRI
// Need to also get the LOCALE_STIME (time separator) string for WIN32
//
//    Rev 1.30   07 Jun 1995 13:55:44   BILL
// Win32 now gets international settings from win32, not win.ini
//
//    Rev 1.29   27 Oct 1994 18:04:12   BRAD
// Removed DBCS specific stuff
//
//    Rev 1.28   14 Jun 1994 22:32:46   BASIL
// Added a call to NIntlInit() in NIntlGetString() to make sure values are
// read in from win.ini.
//
//    Rev 1.27   05 May 1994 13:22:30   BRAD
// Need to include STDDOS.H
//
//    Rev 1.26   05 May 1994 13:15:40   BRAD
// Need STDIO.H
//
//    Rev 1.25   26 Mar 1994 21:37:42   BRAD
//
//    Rev 1.24   15 Mar 1994 12:34:20   BRUCE
// Changed SYM_EXPORT to SYM_EXPORT
//
//    Rev 1.23   04 Mar 1994 13:26:06   BRAD
// WORD -> UINT.
//
//    Rev 1.22   28 Feb 1994 16:25:38   BRAD
// Set params from WORD to UINT.
//
//    Rev 1.21   25 Feb 1994 12:21:12   BRUCE
// Ran scripts to change to SYM_ constants and get ready for Win32
//
//    Rev 1.20   09 Feb 1994 14:05:18   PGRAVES
// Win32.
//
//    Rev 1.19   30 Sep 1993 06:01:04   DWHITE
// Included dbcs.h for Double Byte Support, replaced strchr with DBCS_strchr
//
//    Rev 1.18   29 Sep 1993 18:06:56   RSTANEV
// Fixed the logic of AdjustHours.
//
//    Rev 1.17   02 Sep 1993 22:56:52   PGRAVES
// To improve performance, the WIN.INI section and entry names ("intl",
// "sDate", etc.) are no longer read from the resource file, since they
// are not translated anyway.
//
//    Rev 1.16   29 Aug 1993 12:38:26   MARKL
// One LAST fix to AdjustHours()
//
//    Rev 1.15   19 Aug 1993 12:31:28   MARKL
// Cleaned up AdjustHours() logic such that it is easier for the user
// to enter times in regardless of 12/24 hour mode issues.
//
//    Rev 1.14   02 Aug 1993 20:03:26   MFALLEN
// Commented out system broadcast messages since they are causing software
// incompatibility problems. A proper solution must be found.
//
//    Rev 1.13   26 Jul 1993 12:43:56   MARKL
// The international get time routines use to insist that the user enters
// the time in the 12/24 hour format as specified by control panel.  The
// user may now enter a time in either format and the routines will
// automatically adjust the time for him.  This is much more intuitive.
//
//    Rev 1.12   22 Jul 1993 13:45:18   SGULLEY
// NGetIntlTime now correctly examines seconds ptr before call to adjusthours()
//
//    Rev 1.11   30 Jun 1993 07:46:54   ENRIQUE
// [FIX] Fixed a warning in FormatTime.
//
//    Rev 1.10   29 Jun 1993 22:03:10   DALLEE
// Removed LPSTR casts of args to FormatTime() in NSetIntlTime().
//
//    Rev 1.9   28 Jun 1993 10:29:34   MARKK
// Added FAR to externs to shut up C8
//
//    Rev 1.8   21 Jun 1993 14:30:12   DLEVITON
// Moved date and time functions from DOSTOOL.
//
//    Rev 1.7   17 Jun 1993 23:35:04   BRAD
// Added NIntlTimeToLongString().  This function used to be a DOS
// only function called ConvertElapsedTimeToLongString().
//
//    Rev 1.6   11 Jun 1993 13:48:26   BRAD
// #ifdef SYM_WIN usage of SYMKRNLI.H.
//
//    Rev 1.5   10 Jun 1993 10:07:26   ED
// Uses SYMKRNLI.H now instead of SYMPRIMI.H
//
//    Rev 1.4   21 Apr 1993 22:01:32   MARKL
// Added NIntlGetValidTime() and NIntlGetValidTime().  These work directly
// on edit fields of the format "XX*XX*XX", "XX*XX", "XX*XX*XX" where
// * can be anything or any number of none numeric values.  If the edit
// field specified contain a valid time/date, the values are copied to the
// buffers supplied.  If the edit field does not contain a valid time/date
// then it is reset that that contained in the buffers and an error code is
// returned.   Times 0-23, 0->59; Dates: 1->12, 1->28,29,30,31, 80->???.
//
//    Rev 1.3   20 Apr 1993 20:49:42   MARKL
// Now skips leading blanks in AdjustHours.  The time "8:24 PM" would not
// work since " PM" was not equal to "PM".
//
//    Rev 1.2   16 Mar 1993 08:56:36   MARKL
// Fixed instance handle mismatches and all LoadString() error checking (i.e.
// these routines never worked since every LoadString() failed).
//
//    Rev 1.5   24 Jan 1993 13:02:14   ED
// Adjustments to the last addition
//
//    Rev 1.4   24 Jan 1993 12:29:36   ED
// Added NIntlTimeIsLegal and NIntlDateIsLegal.  These previously were
// DateIsDOSLegal and DateTimeIsDOSLegal in the DOS libraries.
//
//    Rev 1.3   07 Dec 1992 14:36:00   CRAIG
// Fixed a typo.
//
//    Rev 1.2   04 Dec 1992 15:56:38   CRAIG
// Added NIntlGetString() to allow apps to get copies of the international
// strings if they need them for some purpose.
//
//    Rev 1.1   14 Oct 1992 00:24:04   MANSHAN
// Made the instance static.
//
//    Rev 1.0   13 Oct 1992 20:21:20   MANSHAN
// Initial revision.
//
//    Rev 1.16   15 Sep 1992 21:51:34   BRUCE
// Made SYM_EXPORT conversion for exports
//
//    Rev 1.15   14 Sep 1992 22:16:56   PETER
// C++.
//
//    Rev 1.14   11 Sep 1992 19:04:22   LMULCAHY
// quaked
//
//    Rev 1.13   09 Jul 1992 16:59:24   MEIJEN
// Decentralize resources from NWRES to NWIN for REVOLVER
//
//    Rev 1.12   13 Mar 1992 14:33:08   MANSHAN
// Fixed a bug in the formatting the currency.
//
//    Rev 1.11   11 Mar 1992 19:38:14   MANSHAN
// Added the new negative currency formats 8, 9 and 10.
// Used switch statements and format strings.
//
//    Rev 1.10   17 Feb 1992 19:23:38   MANSHAN
// Fixed the 12:00 AM/PM problem.
//
//    Rev 1.9   23 Jan 1992 04:52:06   BRUCE
// Included stdwin.h before nwinutil.h
//
//    Rev 1.8   20 Jan 1992 17:08:22   MANSHAN
// Called NIntlInit() only in FAR procedures.
//
//    Rev 1.7   20 Jan 1992 16:02:58   ENRIQUE
// Added initialization to every function so it could be moved out of
// NWIN's LibMain.
//
//    Rev 1.6   02 Dec 1991 18:55:20   MANSHAN
// Do not adjust the year if the date is invalid.
//
//    Rev 1.5   03 Oct 1991 17:08:06   BRUCE
// Changed (HWND)-1 to HWND_BROADCAST
//
//    Rev 1.4   01 Oct 1991 16:28:02   BRUCE
// "Fixed Up" for winstric.h
//
//    Rev 1.3   17 Sep 1991 15:36:42   MANSHAN
// NIntlFormatFloat() returns ERR if the value is invalid.
//
//    Rev 1.2   17 Sep 1991 14:02:08   MANSHAN
// Display zeros instead of blank fields if the date or time value is invalid.
//
//    Rev 1.1   03 Sep 1991 16:55:30   MANSHAN
// Modified NIntlFormatInteger() to take care of value 0.
//
//    Rev 1.0   07 May 1991 19:54:38   MANSHAN
// Initial revision.
/************************************************************************/
/*                                                                      */
/*  Contains:                                                           */
/*                                                                      */
/*  NGetDlgIntlDate() - Get the date from some dialog box controls      */
/*  NSetDlgIntlDate() - Format the date into some dialog box controls   */
/*  NGetIntlDate()    - Get the date from a buffer                      */
/*  NSetIntlDate()    - Format the date into a buffer                   */
/*  NSetIntlLongDate()- Format the long date into a buffer              */
/*                                                                      */
/*  NGetDlgIntlTime() - Get the time from some dialog box controls      */
/*  NSetDlgIntlTime() - Format the time into some dialog box controls   */
/*  NGetIntlTime()    - Get the time from a buffer                      */
/*  NSetIntlTime()    - Format the time into a buffer                   */
/*                                                                      */
/*----------------------------------------------------------------------*/

#include "platform.h"
#include "xapi.h"
#include "stddos.h"
#include <stdio.h>

#ifdef SYM_WIN
#include "symkrnli.h"                   //  for string IDs
#include "symintl.h"                    //  for international profile values
#endif

#ifdef SYM_WIN32
#include "imm.h"                        // Input Method Manager API

extern HINSTANCE	hImm32Dll;

// ***********************************************************************
// Typedef for dyna-loaded function
typedef HWND (FAR PASCAL * LPFNIMMGETDEFAULTIMEWND)(HWND);

#endif

#ifdef SYM_WIN

extern  HINSTANCE       hInst_SYMKRNL;



/*
**  Local variables:
*/
static int  s_iDate             = INI_DEF_iDATE;
static int  s_iTime             = INI_DEF_iTIME;
static int  s_iTimeLeadingZero  = INI_DEF_iTLZERO;
static int  s_iDigits           = INI_DEF_iDIGITS;
static int  s_iLeadingZero      = INI_DEF_iLZERO;
static int  s_iCurrency         = INI_DEF_iCURRENCY;
static int  s_iCurrNeg          = INI_DEF_iCURRNEG;
static int  s_iCurrDigits       = INI_DEF_iCURRDIGITS;
static char s_szDate[INTL_MAXLEN_sDATE];
static char s_szShortDate[INTL_MAXLEN_sSHORTDATE];
static char s_szLongDate[INTL_MAXLEN_sLONGDATE];
static char s_szTime[INTL_MAXLEN_sTIME];
static char s_szAM[INTL_MAXLEN_sAMPM];
static char s_szDefAM[] = "AM";
static char s_szDefAM2[] = "A.M.";
static char s_szPM[INTL_MAXLEN_sAMPM];
static char s_szDefPM[] = "PM";
static char s_szDefPM2[] = "P.M.";
static char s_szThousand[INTL_MAXLEN_sTHOUSAND];
static char s_szDecimal[INTL_MAXLEN_sDECIMAL];
static char s_szCurrency[INTL_MAXLEN_sCURRENCY];
static char s_szList[INTL_MAXLEN_sLIST];

                                    // TRUE when AMPM string follows the time;
                                    // FALSE when AMPM string appears before
                                    // the time.
static BOOL s_bAmPmFollows = TRUE;

    // Keep this array of pointers in sync with
    // the INTL_ENUM_ values in SYMINTL.H
static LPSTR alpszEnumIndex[] =
    {
    s_szDate, s_szShortDate, s_szLongDate,
    s_szTime, s_szAM, s_szPM,
    s_szThousand, s_szDecimal, s_szCurrency
    };

    // Number of elements in alpszEnumIndex
#define INTL_ENUM_COUNT (sizeof( alpszEnumIndex ) / sizeof( LPSTR ))

/*
**  Macros:
*/
#define FORMAT_INT(lp,dw,n,bLeadingZero)\
    NIntlFormatInteger(lp, dw, n, (char)(bLeadingZero ? '0' : EOS), FALSE)

/*
**  Local procedures:
*/
/* Date */
UINT LOCAL PASCAL AdjustYear(UINT wYear, BOOL bExcludeCentury);
void LOCAL PASCAL FormatDate(LPSTR lpSep,
                             LPSTR lpDate1,
                             LPSTR lpDate2,
                             LPSTR lpDate3,
                             UINT  wMonth,
                             UINT  wDay,
                             UINT  wYear,
                             BOOL  bExcludeCentury);
LPSTR LOCAL PASCAL FormatLongDateField(LPSTR lpBuffer,
                                        LPSTR lpszLongDate,
                                        LPINT lpiLength,
                                        BOOL  bCopySep,
                                        UINT  wMonth,
                                        UINT  wDay,
                                        UINT  wYear);
LPSTR LOCAL PASCAL FormatWeekday(LPSTR lpBuffer,
                                 LPSTR lpLongDate,
                                 LPINT lpiLength,
                                 BOOL  bWeekday,
                                 UINT  wMonth,
                                 UINT  wDay,
                                 UINT  wYear);
int LOCAL PASCAL GetDateFieldLen(LPSTR lpszBuffer, char cLetter);
BOOL LOCAL PASCAL GetDateInput(HWND hDlg, int iIDD, LPUINT lpwInput);
int LOCAL PASCAL GetDayOfWeek(UINT wMonth, UINT wDay, UINT wYear);
LPSTR LOCAL PASCAL GetName(LPSTR lpBuffer, UINT wID, BOOL bLongForm);
BOOL LOCAL PASCAL IsLeapYear(UINT wYear);
BOOL LOCAL PASCAL IsValidDate(UINT wMonth, UINT wDay, UINT wYear, BOOL bInput);
void LOCAL PASCAL RetrieveDate(LPSTR  lpBuffer,
                               LPUINT lpui1,
                               LPUINT lpui2,
                               LPUINT lpui3);
/* Time */
BOOL LOCAL PASCAL AdjustHours(LPUINT lpuiHours, LPSTR lpAMPM);
void LOCAL PASCAL FormatTime(LPSTR lpSep,
                             LPSTR lpAMPM,
                             LPSTR lpHour,
                             LPSTR lpMinute,
                             LPSTR lpSecond,
                             UINT  wHours,
                             UINT  wMinutes,
                             UINT  wSeconds);

BOOL LOCAL PASCAL IsValidTime(UINT wHours, UINT wMinutes, UINT wSeconds);
BOOL LOCAL PASCAL RetrieveTime(HWND hDlg, int iIDDInput, LPUINT lpuiValue);


/************************************************************************/
/*
**  Section 1: Initialization
*/
/************************************************************************/
/*
**  NIntlInit():
**  Store up all the international strings and integers for quick
**      retrival.
**  Return ERR if no change is made; NOERR otherwise.
*/
STATUS SYM_EXPORT WINAPI NIntlInit(LPSTR lpszSection)
{
    static BOOL s_bInitialized = FALSE;
    char szIntl[] = "intl";
#ifdef SYM_WIN32
	char szBuffer[32];
#endif

    if (s_bInitialized  &&  lpszSection == NULL)
        return(NOERR);

    /*
    **  If lpszSection is non-null, it is called when WM_WININICHANGE is
    **      received.  No need to proceed if it is not the international
    **      section.
    */
    if (lpszSection)
        {
        if (STRICMP(lpszSection, szIntl))
            return(ERR);
        // Commented out by Martin Fallenstedt
        // SendMessage(HWND_BROADCAST, UWM_INI_INTL_SAVE, 0, 0L);
        }

#ifdef SYM_WIN32
    /*
    **  Get all the integers:
    */
    GetLocaleInfo(LOCALE_SYSTEM_DEFAULT,LOCALE_IDATE,szBuffer,sizeof(szBuffer));
	s_iDate = atol(szBuffer);
    GetLocaleInfo(LOCALE_SYSTEM_DEFAULT,LOCALE_ITIME,szBuffer,sizeof(szBuffer));
	s_iTime = atol(szBuffer);
    GetLocaleInfo(LOCALE_SYSTEM_DEFAULT,LOCALE_ITLZERO,szBuffer,sizeof(szBuffer));
	s_iTimeLeadingZero = atol(szBuffer);
    GetLocaleInfo(LOCALE_SYSTEM_DEFAULT,LOCALE_IDIGITS,szBuffer,sizeof(szBuffer));
	s_iDigits = atol(szBuffer);
    GetLocaleInfo(LOCALE_SYSTEM_DEFAULT,LOCALE_ILZERO,szBuffer,sizeof(szBuffer));
	s_iLeadingZero = atol(szBuffer);
    GetLocaleInfo(LOCALE_SYSTEM_DEFAULT,LOCALE_ICURRENCY,szBuffer,sizeof(szBuffer));
	s_iCurrency = atol(szBuffer);
    GetLocaleInfo(LOCALE_SYSTEM_DEFAULT,LOCALE_INEGCURR,szBuffer,sizeof(szBuffer));
	s_iCurrNeg= atol(szBuffer);
    GetLocaleInfo(LOCALE_SYSTEM_DEFAULT,LOCALE_ICURRDIGITS,szBuffer,sizeof(szBuffer));
	s_iCurrDigits = atol(szBuffer);
    /*
    **  Get all the strings:
    */
    GetLocaleInfo(LOCALE_SYSTEM_DEFAULT,LOCALE_SDATE,s_szDate,sizeof(s_szDate));
    GetLocaleInfo(LOCALE_SYSTEM_DEFAULT,LOCALE_SSHORTDATE,s_szShortDate,sizeof(s_szShortDate));
    GetLocaleInfo(LOCALE_SYSTEM_DEFAULT,LOCALE_SLONGDATE,s_szLongDate,sizeof(s_szLongDate));
    GetLocaleInfo(LOCALE_SYSTEM_DEFAULT,LOCALE_STIME,s_szTime,sizeof(s_szTime));
    GetLocaleInfo(LOCALE_SYSTEM_DEFAULT,LOCALE_S1159,s_szAM,sizeof(s_szAM));
    GetLocaleInfo(LOCALE_SYSTEM_DEFAULT,LOCALE_S2359,s_szPM,sizeof(s_szPM));
    GetLocaleInfo(LOCALE_SYSTEM_DEFAULT,LOCALE_STHOUSAND,s_szThousand,sizeof(s_szThousand));
    GetLocaleInfo(LOCALE_SYSTEM_DEFAULT,LOCALE_SDECIMAL,s_szDecimal,sizeof(s_szDecimal));
    GetLocaleInfo(LOCALE_SYSTEM_DEFAULT,LOCALE_SCURRENCY,s_szCurrency,sizeof(s_szCurrency));
    GetLocaleInfo(LOCALE_SYSTEM_DEFAULT,LOCALE_SLIST,s_szList,sizeof(s_szList));

    /*
    ** Determine if the Am/Pm string appears before or after the time. In some
    ** international machines (Japanese, Korean) the AmPm string appears
    ** before the time.
    */
    GetLocaleInfo(LOCALE_SYSTEM_DEFAULT,LOCALE_STIMEFORMAT,szBuffer,sizeof(szBuffer));
    if (0 == STRNCMP (szBuffer, "tt", 2))
        s_bAmPmFollows = FALSE;
    else
        s_bAmPmFollows = TRUE;

#else
    /*
    **  Get all the integers:
    */
    s_iDate = GetProfileInt(szIntl, "iDate", INI_DEF_iDATE);
    s_iTime = GetProfileInt(szIntl, "iTime", INI_DEF_iTIME);
    s_iTimeLeadingZero = GetProfileInt(szIntl, "iTLZero", INI_DEF_iTLZERO);
    s_iDigits = GetProfileInt(szIntl, "iDigits", INI_DEF_iDIGITS);
    s_iLeadingZero = GetProfileInt(szIntl, "iLZero", INI_DEF_iLZERO);
    s_iCurrency = GetProfileInt(szIntl, "iCurrency", INI_DEF_iCURRENCY);
    s_iCurrNeg = GetProfileInt(szIntl, "iNegCurr", INI_DEF_iCURRNEG);
    s_iCurrDigits = GetProfileInt(szIntl, "iCurrDigits", INI_DEF_iCURRDIGITS);

    /*
    **  Get all the strings:
    */
    GetProfileString(szIntl, "sDate", "/", s_szDate, sizeof(s_szDate));
    GetProfileString(szIntl, "sShortDate", "M/d/yy", s_szShortDate, sizeof(s_szShortDate));
    GetProfileString(szIntl, "sLongDate", "dddd', 'MMMM' 'dd', 'yyyy", s_szLongDate, sizeof(s_szLongDate));
    GetProfileString(szIntl, "sTime", ":", s_szTime, sizeof(s_szTime));
    GetProfileString(szIntl, "s1159", "AM", s_szAM, sizeof(s_szAM));
    GetProfileString(szIntl, "s2359", "PM", s_szPM, sizeof(s_szPM));
    GetProfileString(szIntl, "sThousand", ",", s_szThousand, sizeof(s_szThousand));
    GetProfileString(szIntl, "sDecimal", ".", s_szDecimal, sizeof(s_szDecimal));
    GetProfileString(szIntl, "sCurrency", "$", s_szCurrency, sizeof(s_szCurrency));
    GetProfileString(szIntl, "sList", ",", s_szList, sizeof(s_szList));
#endif

    // Commented out by Martin Fallenstedt
    // if (lpszSection)
    //     PostMessage(HWND_BROADCAST, UWM_INI_INTL_REFRESH, 0, 0L);

    s_bInitialized = TRUE;

    return(NOERR);
} /* NIntlInit */


/************************************************************************/
/*
**  Section 2: Date
*/
/************************************************************************/
/*
**  NIsAmericanDate():
**  Return FALSE if the date format is D-M-Y.
**  Return TRUE otherwise.
**
**  7/12/91: Requested by Enrique.
*/
BOOL SYM_EXPORT WINAPI NIsAmericanDate(void)
{
    NIntlInit(NULL);

    return(s_iDate == DATE_DMY ? FALSE : TRUE);
} /* NIsAmericanDate */


/************************************************************************/
/*
**  NGetIntlDate():
**  Get the date input from the buffer according to the Short Date Format
**    specified in the International section of WIN.INI.
**
**  This function does not return a value.
**
**  See also: NSetIntlDate().
*/
void SYM_EXPORT WINAPI NGetIntlDate(LPSTR  lpBuffer,
                             LPUINT lpuiMonth,
                             LPUINT lpuiDay,
                             LPUINT lpuiYear)
{
    NIntlInit(NULL);

    switch (s_iDate)
        {
        default:
        case  DATE_MDY:
            RetrieveDate(lpBuffer, lpuiMonth, lpuiDay, lpuiYear);
            break;

        case  DATE_DMY:
            RetrieveDate(lpBuffer, lpuiDay, lpuiMonth, lpuiYear);
            break;

        case  DATE_YMD:
            RetrieveDate(lpBuffer, lpuiYear, lpuiMonth, lpuiDay);
            break;
        } /* switch */
} /* NGetIntlDate */


/************************************************************************/
/*
**  RetrieveDate():
**  Retrieve the values in the order specified.
**  Subprocedure of GetIntlDate().
*/
void LOCAL PASCAL RetrieveDate(LPSTR  lpBuffer,
                               UINT FAR * lpui1,
                               UINT FAR * lpui2,
                               UINT FAR * lpui3)
{
    lpBuffer = ConvertStringToWord(lpui1, lpBuffer);
    lpBuffer = ConvertStringToWord(lpui2, lpBuffer);
    lpBuffer = ConvertStringToWord(lpui3, lpBuffer);
} /* RetrieveDate */


/************************************************************************/
/*
**  NCurrentIntlDateSet():
**  Set the current date into a buffer according to the Short Date Format
**    specified in the International section of WIN.INI.
**  If the date is invalid, a blank is displayed.
**
**  This function returns NOERR no matter what happens.
**
**  See also: NSetIntlDate(), NSetIntlLongDate().
*/
STATUS WINAPI NCurrentIntlDateSet(LPSTR lpBuffer)
{
#ifdef SYM_WIN32
    SYSTEMTIME st;
    NIntlInit(NULL);
    GetSystemTime(&st);
    NSetIntlDate(lpBuffer, st.wMonth, st.wDay, st.wYear);
#else
    UINT    uMonth;
    UINT    uDay;
    UINT    uYear;

    NIntlInit(NULL);

    DOSGetDate(&uMonth, &uDay, &uYear);
    NSetIntlDate(lpBuffer, uMonth, uDay, uYear);
#endif
    return NOERR;
} /* NCurrentIntlDateSet */


/************************************************************************/
/*
**  NSetIntlDate():
**  Set the date input into a buffer according to the Short Date Format
**    specified in the International section of WIN.INI.
**  If the date is invalid, a blank is displayed.
**
**  This function does not return a value.
**
**  See also: NGetIntlDate(), NSetIntlLongDate().
*/
void SYM_EXPORT WINAPI NSetIntlDate(LPSTR lpBuffer,
                                UINT wMonth,
                                UINT wDay,
                                UINT wYear)
{
    char  szSep[2];
    char  szDate1[8];
    char  szDate2[8];
    char  szDate3[8];

    NIntlInit(NULL);

    FormatDate(szSep, szDate1, szDate2, szDate3, wMonth, wDay, wYear, FALSE);
    STRCPY(lpBuffer, szDate1);
    STRCAT(lpBuffer, szSep);
    STRCAT(lpBuffer, szDate2);
    STRCAT(lpBuffer, szSep);
    STRCAT(lpBuffer, szDate3);
} /* NSetIntlDate */


/************************************************************************/
/*
**  NGetDlgIntlDate():
**  Get the date input from a set of date input control according to the
**    Short Date Format specified in the International section of WIN.INI.
**  The argument iIDDInput1 is the ID of the first of five controls, in
**    consecutive order as follows:
**       iIDDInput1     --  ID of first input control (month, day or year)
**       iIDDInput1 + 1 --  ID of first input control (month, day or year)
**       iIDDInput1 + 2 --  ID of first input control (month, day or year)
**
**  Return NDT_NO_INPUT if there is no input.
**  Return NDT_INVALID  if the input date is invalid.
**  Return NDT_VALID    if the input date is valid.
**
**  See also: NSetDlgIntDate().
*/
int SYM_EXPORT WINAPI NGetDlgIntlDate(HWND   hDlg,
                                  UINT   wIDDInput1,
                                  LPUINT lpwMonth,
                                  LPUINT lpwDay,
                                  LPUINT lpwYear)
{
    UINT wInput1;
    UINT wInput2;
    UINT wInput3;

    NIntlInit(NULL);

    /*
    **  Retrieve the input.  Do this outside the switch statement to save
    **    some code.
    */
    if (! GetDateInput(hDlg, wIDDInput1,     &wInput1)
    ||   ! GetDateInput(hDlg, wIDDInput1 + 1, &wInput2)
    ||   ! GetDateInput(hDlg, wIDDInput1 + 2, &wInput3))
        return(NDT_NO_INPUT);

    switch (s_iDate)
        {
        default:
        case  DATE_MDY:
            *lpwMonth = wInput1;
            *lpwDay   = wInput2;
            *lpwYear  = wInput3;
            break;

        case  DATE_DMY:
            *lpwDay   = wInput1;
            *lpwMonth = wInput2;
            *lpwYear  = wInput3;
            break;

        case  DATE_YMD:
            *lpwYear  = wInput1;
            *lpwMonth = wInput2;
            *lpwDay   = wInput3;
            break;
        } /* switch */

    /*
    **  Check for valid format.
    */
    return(IsValidDate(*lpwMonth, *lpwDay, *lpwYear, TRUE) ?
            NDT_VALID : NDT_INVALID);
} /* NGetDlgIntlDate */


/************************************************************************/
/*
**  GetDateInput():
**  Get the value of one component.
**  Return TRUE is there is input and FALSE otherwise.
**  The value is passed back.
**  Need this only because the year can be 0 (2000, that is).]]
**  Subprocedure of GetDlgIntlDate().
*/
BOOL LOCAL PASCAL GetDateInput(HWND hDlg, int iIDD, LPUINT lpwInput)
{
    BOOL  bTranslated;

    *lpwInput = GetDlgItemInt(hDlg, iIDD, &bTranslated, FALSE);
    return(bTranslated);
} /* GetDateInput */


/************************************************************************/
/*
**  NSetDlgIntlDate():
**  Format the date to a set of date input control according to the Short
**    Date Format specified in the International section of WIN.INI.
**  The argument iIDDInput1 is the ID of the first of five controls, in
**    consecutive order as follows:
**       wIDDInput1     --  ID of first input control (month, day or year)
**       wIDDInput1 + 1 --  ID of first input control (month, day or year)
**       wIDDInput1 + 2 --  ID of first input control (month, day or year)
**       wIDDInput1 + 3 --  ID of date separator
**       wIDDInput1 + 4 --  ID of date separator
**
**  This function does not return a value.
**
**  See also: NGetDlgIntDate().
*/
void SYM_EXPORT WINAPI NSetDlgIntlDate(HWND hDlg,
                                   UINT wIDDInput1,
                                   UINT wMonth,
                                   UINT wDay,
                                   UINT wYear,
                                   BOOL bExcludeCentury)
{
    char  szSep[2];
    char  szDate1[8];
    char  szDate2[8];
    char  szDate3[8];

    NIntlInit(NULL);

    FormatDate(szSep, szDate1, szDate2, szDate3, wMonth, wDay, wYear,
                bExcludeCentury);

    /*
    **  Display the resultant text in the controls.
    */
    SetDlgItemText(hDlg, wIDDInput1++, szDate1);
    SetDlgItemText(hDlg, wIDDInput1++, szDate2);
    SetDlgItemText(hDlg, wIDDInput1++, szDate3);
    SetDlgItemText(hDlg, wIDDInput1++, szSep);
    SetDlgItemText(hDlg, wIDDInput1, szSep);
} /* NSetDlgIntlDate */


/************************************************************************/
/*
**  FormatDate():
**  Format the date in the order specified in WIN.INI.
**  Called by SetDlgIntlDate() and SetIntlDate().
*/
void LOCAL PASCAL FormatDate(LPSTR lpSep,
                             LPSTR lpDate1,
                             LPSTR lpDate2,
                             LPSTR lpDate3,
                             UINT wMonth,
                             UINT wDay,
                             UINT wYear,
                             BOOL bExcludeCentury)
{
    LPSTR   lpMonth;
    LPSTR   lpDay;
    LPSTR   lpYear;
    int     iFieldLen;

    STRCPY(lpSep, s_szDate);

    iFieldLen = GetDateFieldLen(s_szShortDate, 'y');

    /*
    **  Check for valid format.
    **  9/17/91: Display zeros instead of empty fields if the date is invalid.
    */
    if (! IsValidDate(wMonth, wDay, wYear, FALSE))
        {
        wMonth = 0;
        wDay   = 0;
        wYear  = 0;
        }
    else
        {
        /*
        **  Need to check the year.  If it is between 80 and 99, assume it is in
        **    the 20th century; else assume it is in the 21st century.
        **  12/2/91: Do not adjust the year if the date is invalid.
        */
        wYear = AdjustYear(wYear, (bExcludeCentury  ||  iFieldLen < 4));
        if (bExcludeCentury  &&  iFieldLen > 2)
            iFieldLen = 2;
        }

    switch (s_iDate)
        {
        default:
        case  DATE_MDY:
            lpMonth = lpDate1;
            lpDay   = lpDate2;
            lpYear  = lpDate3;
            break;

        case  DATE_DMY:
            lpDay   = lpDate1;
            lpMonth = lpDate2;
            lpYear  = lpDate3;
            break;

        case  DATE_YMD:
            lpYear  = lpDate1;
            lpMonth = lpDate2;
            lpDay   = lpDate3;
            break;
        } /* switch */

    /*
    **  12/2/91: Use iFieldLen to format the year.
    */
    FORMAT_INT(lpYear, wYear, iFieldLen, TRUE);
    iFieldLen = GetDateFieldLen(s_szShortDate, 'M');
    FORMAT_INT(lpMonth, wMonth, 2, (iFieldLen > 1));
    iFieldLen = GetDateFieldLen(s_szShortDate, 'd');
    FORMAT_INT(lpDay, wDay, 2, (iFieldLen > 1));
} /* FormatDate */


/************************************************************************/
/*
**  AdjustYear():
**  Adjust the year to the appropriate value according to the format, i.e.
**    return the year without the century is bExcludeCentury is TRUE and
**    without if FALSE.
**  Return the value of the year after adjustment.
*/
UINT LOCAL PASCAL AdjustYear(UINT wYear, BOOL bExcludeCentury)
{
    if (bExcludeCentury)
        wYear %= 100;
    else if (wYear < 100)
        wYear += (wYear < 80 ? 2000 : 1900);
    return(wYear);
} /* AdjustYear */


/************************************************************************/
/*
**  NSetIntlLongDate():
**  Set the date input into a buffer according to the Long Date Format
**    specified in the International section of WIN.INI.
**  If the date is invalid, a blank is displayed.
**  The weekday is not included if bWeekday is FALSE.
**
**  This function does not return a value.
**
**  See also: NSetIntlDate().
*/
void SYM_EXPORT WINAPI NSetIntlLongDate(LPSTR lpBuffer,
                                    UINT  wMonth,
                                    UINT  wDay,
                                    UINT  wYear,
                                    BOOL  bWeekday)
{
#ifdef SYM_WIN32

SYSTEMTIME rDate = { wYear, wMonth, 0, wDay };
int        nBufSize;

    // GetDateFormat() requires a buffer size, so I first call it with a
    // buffer size of 0 to get the size needed to hold the date string,
    // then I pass that size in the real call to get the formatted string.
    //
    // ***NOTE*** that bWeekday is ignored - the user's regional settings
    // determine whether long dates contain the weekday.

    nBufSize = GetDateFormat ( (LCID) NULL, DATE_LONGDATE, &rDate,
                               NULL, NULL, 0 );

    GetDateFormat ( (LCID) NULL, DATE_LONGDATE, &rDate,
                    NULL, lpBuffer, nBufSize );

#else
    LPSTR   lpDate;
    int     iFormatLen;

    NIntlInit(NULL);

    lpDate = s_szLongDate;

    lpBuffer = FormatWeekday(lpBuffer, s_szLongDate, &iFormatLen, bWeekday,
                              wMonth, wDay, wYear);
    lpDate += iFormatLen;

    lpBuffer = FormatLongDateField(lpBuffer, lpDate, &iFormatLen, bWeekday,
                                    wMonth, wDay, wYear);
    lpDate += iFormatLen;

    lpBuffer = FormatLongDateField(lpBuffer, lpDate, &iFormatLen, TRUE,
                                    wMonth, wDay, wYear);
    lpDate += iFormatLen;

    lpBuffer = FormatLongDateField(lpBuffer, lpDate, &iFormatLen, TRUE,
                                    wMonth, wDay, wYear);
#endif
} /* NSetIntlLongDate */


/************************************************************************/
/*
**  FormatLongDateField():
**  Format the next field (year, month or day) according the the format.
**  The separation characters before the field are copied first if bCopySep
**    is TRUE.
**  Return the pointer to the terminating null byte.
*/
LPSTR LOCAL PASCAL FormatLongDateField(LPSTR lpBuffer,
                                       LPSTR lpszLongDate,
                                       LPINT lpiLength,
                                       BOOL  bCopySep,
                                       UINT  wMonth,
                                       UINT  wDay,
                                       UINT  wYear)
{
    int   nCopyLen;
    int   nFieldLen;
    LPSTR lpField;

    /*
    **  Set the values in case of invalid format.
    */
    *lpiLength = 0;
    *lpBuffer = EOS;

    /*
    **  Copy the separation characters before the field if necessary.
    */
    if (*lpszLongDate == '\'')
        {
        if ((lpField = STRCHR(lpszLongDate + 1, '\'')) == NULL)
            return(lpBuffer);
        lpField++;
        }
    else
        {
        char    szDateChars[] = { 'd', 'M', 'y', EOS };

        if ((lpField = STRPBRK(lpszLongDate, szDateChars)) == NULL)
            return(lpBuffer);
        }

    *lpiLength = nCopyLen = (int)(lpField - lpszLongDate);
    if (bCopySep)
        {
        if (*lpszLongDate == '\'')
            {
            nCopyLen -= 2;
            lpszLongDate++;
            }
        MEMCPY(lpBuffer, lpszLongDate, nCopyLen);
        lpBuffer += nCopyLen;
        }

    /*
    **  Format the field (either year, month or day).
    */
    nFieldLen = GetDateFieldLen(lpField, *lpField);
    *lpiLength += nFieldLen;

    switch (*lpField)
        {
        case  'd':
            /*
            **  Spec in the Long Date Format    Output Example
            **  ----------------------------    --------------
            **    dd                               01
            **    d                                1
            */
            lpBuffer = FORMAT_INT(lpBuffer, wDay, 2, (nFieldLen > 1));
            break;

        case  'M':
            /*
            **  Spec in the Long Date Format    Output Example
            **  ----------------------------    --------------
            **    MMMM                             January
            **    MMM                              Jan
            **    MM                               01
            **    M                                1
            */
            if (nFieldLen < 3)
                lpBuffer = FORMAT_INT(lpBuffer, wMonth, 2, (nFieldLen > 1));
            else
                {
#ifdef SYM_WIN32
                auto char   szMonth[100];
                auto LCTYPE lcType;
                auto int    iLength;

                lcType = (*lpiLength > 3) ? LOCALE_SMONTHNAME1: LOCALE_SABBREVMONTHNAME1;
                                        // Get the string from the WIN32 API
                iLength = GetLocaleInfo( LOCALE_SYSTEM_DEFAULT, lcType + wMonth-1,
                               szMonth, sizeof(szMonth) );
                STRCPY(lpBuffer, szMonth);
                lpBuffer += (iLength - 1);

#else
                lpBuffer = GetName(lpBuffer, MN_JANUARY+wMonth-1, (nFieldLen > 3));
#endif
                }
            break;

        case  'y':
            /*
            **  Spec in the Long Date Format    Output Example
            **  ----------------------------    --------------
            **    yyyy                             1990
            **    yy                               90
            */
            wYear = AdjustYear(wYear, (nFieldLen < 4));
            lpBuffer = FORMAT_INT(lpBuffer, wYear, 2, (wYear < 10));
            break;
        } /* switch */

    return(lpBuffer);
} /* FormatLongDateField */


/************************************************************************/
/*
**  FormatWeekday():
**  Format the weekday according the the format.
**  Return the pointer to the terminating null byte.
*/
LPSTR LOCAL PASCAL FormatWeekday(LPSTR lpBuffer,
                                  LPSTR lpLongDate,
                                  LPINT lpiLength,
                                  BOOL bWeekday,
                                  UINT wMonth,
                                  UINT wDay,
                                  UINT wYear)
{
    int   iWeekday;

    *lpiLength = (*lpLongDate != 'd' ? 0 : GetDateFieldLen(lpLongDate, 'd'));
    if (bWeekday  &&  *lpiLength > 0)
        {
#ifdef SYM_WIN32
        auto char   szWeekDay[100];
        auto LCTYPE lcType;
        auto int    iLength;
        iWeekday = GetDayOfWeek(wMonth, wDay, wYear);
        if (iWeekday == 0)
            iWeekday = 7;               // change 0 to 7 for sunday

                                        // Abbreviated or long form?
        lcType = (*lpiLength > 3) ? LOCALE_SDAYNAME1 : LOCALE_SABBREVDAYNAME1;
                                        // Get the string from the WIN32 API
        iLength = GetLocaleInfo( LOCALE_SYSTEM_DEFAULT, lcType + iWeekday - 1,
                       szWeekDay, sizeof(szWeekDay) );
        STRCPY(lpBuffer, szWeekDay);
        lpBuffer += (iLength - 1);

#else
        iWeekday = GetDayOfWeek(wMonth, wDay, wYear);
        lpBuffer = GetName(lpBuffer, WDN_SUNDAY+iWeekday, (*lpiLength > 3));
#endif
        }
    else
        *lpBuffer = EOS;
    return(lpBuffer);
} /* FormatWeekday */


/************************************************************************/
/*
**  GetName():
**  Load the string from the string table and copy it to the buffer.
**  If the short form is used, only the first 3 letters are copied.
**  Return the pointer to the terminating null byte.
*/
LPSTR LOCAL PASCAL GetName(LPSTR lpBuffer, UINT wID, BOOL bLongForm)
{
    char  szTemp[20];  /* enough for both month and weekday names */
    int   iLength;

    if (LoadString(hInst_SYMKRNL, wID, szTemp, sizeof(szTemp)))
        {
        iLength = (bLongForm ? STRLEN(szTemp) : 3);
        MEMCPY(lpBuffer, szTemp, iLength);
        lpBuffer += iLength;
        }
    *lpBuffer = '\0';
    return(lpBuffer);
} /* GetName */


/************************************************************************/
/*    Determines day of the week for a date                             *
**    Returns: 0-6, 0 is Sunday; else returns 255 if error              *
**                                                                      *
**    Called:                                                           *
**    DateGetDayOfWeek(wMonth, wDay, wYear)                             *
**    with                                                              *
**    UINT wMonth;                                                      *
**    UINT wDay;                                                        *
**    UINT year;                                                        *
**                                                                      *
**    IMPLEMENTATION HISTORY                                            *
**    03/28/86 Created                                                  *
**    05/04/89 Henri: Supplied new and shorter and algorithm.           *
**    01/16/91 Manshan: Ported to be used in Windows.                   *
**                      Modified the algorithm to be even shorter.      *
**                      Allowed the year to be with or without century. *
*/
int LOCAL PASCAL GetDayOfWeek(UINT wMonth, UINT wDay, UINT wYear)
{
    /*
    **  Add the century if not supplied.
    */
    wYear = AdjustYear(wYear, FALSE);

    if (wMonth <= 2)
        {
        wMonth += 12;
        wYear--;
        }

    return((wDay + wMonth * 2 + (wMonth + 1) * 6 / 10 + wYear +
                wYear / 4 - wYear / 100 + wYear / 400 + 1) % 7);
} /* GetDayOfWeek */


/************************************************************************/
/*
**  IsValidDate():
**  Check if the date is valid.  Even check for leap years.
**  Return TRUE if valid; FALSE otherwise.
**  7/16/91: Added the parameter bInput.  Only 3-digit years are disallowed
**           when it is FALSE (to fix the problem with spreadsheets).
*/
BOOL LOCAL PASCAL IsValidDate(UINT wMonth, UINT wDay, UINT wYear, BOOL bInput)
{
    UINT waLastDay[] = { 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };

    /*
    **  12/2/91: Do not invalidate the year if bInput is FALSE.
    */
    if (bInput  &&  (100 <= wYear  &&  wYear < 1980))
        return(FALSE);

    if (12 < wMonth  ||  wMonth < 1)
        return(FALSE);

    if (wMonth == 2  &&  IsLeapYear(wYear))
        waLastDay[1] = 29;

    return(waLastDay[wMonth - 1] >= wDay  &&  wDay >= 1);
} /* IsValidDate */


/************************************************************************/
/*
**  IsLeapYear():
**  Return TRUE if the year is a leap year; FALSE otherwise.
**  Subprocedure of IsValidDate().
*/
BOOL LOCAL PASCAL IsLeapYear(UINT wYear)
{
    return(wYear % 4 == 0  &&  (wYear % 400 == 0  ||  wYear % 100));
} /* IsLeapYear */


/************************************************************************/
/*
**  GetDateFieldLen():
**  In WIN.INI, the year, month and day are expressed as contiguous 'y's, 'M's
**    and 'd's, respectively.  In the case of the long format, the weekday is
**    also expressed as 'd's but is always the first field.  This function
**    finds the number of occurrences of the specified letter.
**  Return the number of specified letter.
**  This function is used instead of _fstrstr() to eliminate the constant
**    strings needed.
*/
int LOCAL PASCAL GetDateFieldLen(LPSTR lpszBuffer, char cLetter)
{
    int   iCount;

    if ((lpszBuffer = STRCHR(lpszBuffer, cLetter)) == NULL)
        return(0);
    for (iCount = 0; *lpszBuffer++ == cLetter; iCount++)
        ;
    return(iCount);
} /* GetDateFieldLen */

/************************************************************************/
/*
**  Section 3: Time
*/
/************************************************************************/
/*
**  NGetIntlTime():
**  Get the time input from the buffer according to the Time Format
**    specified in the International section of WIN.INI.
**  If lpwSeconds is NULL, it is assumed that the string does not contain
**    seconds.
**
**  This function does not return a value.
**
**  See also: NSetIntTime().
**
** 10/23/96 JBRENNA: Modified so that if the intl settings says AmPm at the
**                   lpBuffer, we pass the beginning of the buffer to
**                   AdjustHours().
*/
void SYM_EXPORT WINAPI NGetIntlTime(LPSTR  lpBuffer,
                             UINT FAR * lpuiHours,
                             UINT FAR * lpuiMinutes,
                             UINT FAR * lpuiSeconds)
{
    LPSTR  lpCurBuffer = lpBuffer;
    TCHAR  szTempBuffer[SYM_MAX_PATH];
    BOOL   bOldAmPmFollows;

    NIntlInit(NULL);

                                        // Zero our our new string.
    memset( szTempBuffer, 0, sizeof( szTempBuffer ) );
                                        // Search for the first numerical digit.
    while ( (*lpCurBuffer) &&
            (FALSE == isdigit( *lpCurBuffer)) )
        {
        lpCurBuffer = CharNext( lpCurBuffer );
        }

                                        // Now that we are at the first
                                        // numerical digit, copy this to the
                                        // beginning of the new buffer.
    STRCPY( szTempBuffer, lpCurBuffer );
    STRNCAT( szTempBuffer, lpBuffer, STRLEN( lpBuffer ) - STRLEN( lpCurBuffer ) );

                                        // Set our current buffer to our new
                                        // temp string with the characters
                                        // reversed.
    lpCurBuffer = szTempBuffer;

                                        // Save our old setting.
    bOldAmPmFollows = s_bAmPmFollows;
                                        // Set our AmPm Follows variable to
                                        // always true, as we are making sure
                                        // that is the case.
    s_bAmPmFollows = TRUE;


    lpCurBuffer = ConvertStringToWord(lpuiHours, lpCurBuffer);
    lpCurBuffer = ConvertStringToWord(lpuiMinutes, lpCurBuffer);
    if (lpuiSeconds != NULL )
        {
        if (*lpuiSeconds)
            {
            lpCurBuffer = ConvertStringToWord(lpuiSeconds, lpCurBuffer);
            }
        }

                                    // AmPm is on end of string if
                                    // s_bAmPmFollows; otherwise AmPm is on
                                    // front of string.
	if (s_bAmPmFollows)
	    AdjustHours(lpuiHours, lpCurBuffer);
	else
	    AdjustHours(lpuiHours, lpBuffer);

                                        // Set our AmPm format back to it's
                                        // original state.
     s_bAmPmFollows = bOldAmPmFollows;

} /* NGetIntlTime */


/************************************************************************/
/*
**  AdjustHours():
**  If the time is in 12-hours format and the PM indicator (as specified in
**    WIN.INI) is found after the time, the value of hours will be adjusted.
**  Return FALSE if the indicator is incorrect; TRUE otherwise.
**
** MSL:  MODIFIED TO SKIP LEADING BLANKS.....
** RSTANEV: Added one more check of the status of lpAMPM
** JBRENNA: Use STRNICMP instead of STRICMP so that this function can process
**          the am/pm setting when it occurs at the front of a string.
*/
BOOL LOCAL PASCAL AdjustHours(UINT FAR * lpuiHours, LPSTR lpAMPM)
{
    /*
    **  If the PM indicator is not supplied, no adjustment is needed.
    */

    if (lpAMPM == NULL || *lpAMPM == '\0')
        return(TRUE);

    /*
    **  Skip any leading blanks...
    */
    while (*lpAMPM == ' ')
        lpAMPM = AnsiNext(lpAMPM);

    /*
    **  No need to adjust if the indicator shows AM unless it is midnight.
    **  This could be the 24-hours format.
    */
    if ((STRNICMP(s_szAM,     lpAMPM, STRLEN(s_szAM)) == 0)    ||
        (STRNICMP(s_szDefAM,  lpAMPM, STRLEN(s_szDefAM)) == 0) ||
        (STRNICMP(s_szDefAM2, lpAMPM, STRLEN(s_szDefAM2)) == 0))
        {
        if (*lpuiHours == 12)
            *lpuiHours = 0;
        return(TRUE);
        }

    /*
    **  At this point, the indicator should show PM only in the 12-hours
    **    format.  Check it just in case.
    */
    if ((STRNICMP(s_szPM,     lpAMPM, STRLEN(s_szPM)) == 0)    ||
        (STRNICMP(s_szDefPM,  lpAMPM, STRLEN(s_szDefPM)) == 0) ||
        (STRNICMP(s_szDefPM2, lpAMPM, STRLEN(s_szDefPM2)) == 0))
        {
        if (*lpuiHours < 12)
            *lpuiHours += 12;
        return(TRUE);
        }

    return(FALSE);
} /* AdjustHours */


/************************************************************************/
/*
**  NCurrentIntlTimeSet():
**  Set the Time input into a buffer according to the Short Time Format
**    specified in the International section of WIN.INI.
**  If bDispAMPM is TRUE, the AM/PM indicator is appended at the end of the
**    time separated by a space.
**  If the Time is invalid, a blank is displayed.
**
**  This function returns NOERR no matter what happens.
**
**  See also: NSetIntTime().
*/
STATUS WINAPI NCurrentIntlTimeSet(LPSTR lpBuffer,
                                      BOOL  bDispSeconds,
                                      BOOL  bDispAMPM)
{
#ifdef SYM_WIN32
    SYSTEMTIME st;
    NIntlInit(NULL);
    GetSystemTime(&st);
    NSetIntlTime(lpBuffer, st.wHour, st.wMinute, st.wSecond, bDispSeconds, bDispAMPM);
#else
    UINT    uHour;
    UINT    uMinute;
    UINT    uSecond;
    UINT    uHundredth;

    NIntlInit(NULL);

    DOSGetTime(&uHour, &uMinute, &uSecond, &uHundredth);
    NSetIntlTime(lpBuffer, uHour, uMinute, uSecond, bDispSeconds, bDispAMPM);
#endif
    return NOERR;
} /* NCurrentIntlTimeSet */


/************************************************************************/
/*
**  NSetIntlTime():
**  Set the Time input into a buffer according to the Short Time Format
**    specified in the International section of WIN.INI.
**  If bDispAMPM is TRUE, the AM/PM indicator is appended at the end of the
**    time separated by a space.
**  If the Time is invalid, a blank is displayed.
**
**  This function does not return a value.
**
**  See also: NGetIntTime().
*/
void SYM_EXPORT WINAPI NSetIntlTime(LPSTR lpBuffer,
                                UINT wHours,
                                UINT wMinutes,
                                UINT wSeconds,
                                BOOL  bDispSeconds,
                                BOOL  bDispAMPM)
{
    char  szAMPM[INTL_MAXLEN_sAMPM + 1];
    char  szSpace[2] = { ' ', '\0' };
    char  szSep[2];
    char  szHour[8];
    char  szMinute[8];
    char  szSecond[8];

    NIntlInit(NULL);

    if (s_iTime == TIME_24_HRS)
        bDispAMPM = FALSE;
    /*
    **  Format the time into the buffer according to the international format.
    **  3/1/91: Need to explicitly cast szAMPM to a long pointer.
    */
    FormatTime(szSep, (bDispAMPM) ? (LPSTR)szAMPM : NULL,
               szHour,szMinute,szSecond,
               wHours, wMinutes, wSeconds);

    lpBuffer[0] = EOS;

    if (bDispAMPM && !s_bAmPmFollows)
        {
        STRCAT (lpBuffer, szAMPM);
        STRCAT (lpBuffer, szSpace);
        }

    STRCAT(lpBuffer, szHour);
    STRCAT(lpBuffer, szSep);
    STRCAT(lpBuffer, szMinute);
    if (bDispSeconds)
        {
        STRCAT(lpBuffer, szSep);
        STRCAT(lpBuffer, szSecond);
        }
    if (bDispAMPM && s_bAmPmFollows)
        {
        STRCAT(lpBuffer, szSpace);
        STRCAT(lpBuffer, szAMPM);
        }
} /* NSetIntlTime */


/************************************************************************/
/*
**  FormatTime():
**  Format the Time in the order specified in WIN.INI.
**  Called by SetDlgIntlTime() and SetIntlTime().
*/
void LOCAL PASCAL FormatTime(LPSTR lpSep,
                             LPSTR lpAMPM,
                             LPSTR lpHour,
                             LPSTR lpMinute,
                             LPSTR lpSecond,
                             UINT  wHours,
                             UINT  wMinutes,
                             UINT  wSeconds)
{
    BOOL  b12Hours;
    BOOL  bLeadingZero;

    STRCPY(lpSep, s_szTime);

    /*
    **  Check for valid format.
    **  9/17/91: Display zeros instead of empty fields if the time is invalid.
    */
    if (! IsValidTime(wHours, wMinutes, wSeconds))
        {
#if 1
        wHours   = 0;
        wMinutes = 0;
        wSeconds = 0;
#else
        *lpHour   = EOS;
        *lpMinute = EOS;
        *lpSecond = EOS;
        if (lpAMPM)
            *lpAMPM = EOS;
        return;
#endif
        }

    /*
    **  Use the appropriate format as specified in WIN.INI.
    **  Only Hour can have no leading zero.
    */
    b12Hours = (s_iTime != TIME_24_HRS);
    bLeadingZero = (s_iTimeLeadingZero == LEADING_ZERO_YES);

    /*
    **  The AM and PM indicating strings are the same for the 24-hour format.
    */
    if (lpAMPM)
        STRCPY(lpAMPM, (b12Hours  &&  wHours < 12 ? s_szAM : s_szPM));

    /*
    **  Get the correct Hour display.
    */
    if (b12Hours  &&  wHours > 12)
        wHours -= 12;
    else if (wHours == 24)
        wHours = 0;

    /*
    **  Need to convert 0:mm AM to 12:mm AM.
    */
    if (lpAMPM  &&  b12Hours  &&  wHours == 0)
            wHours = 12;

    FORMAT_INT(lpHour, wHours, 2, bLeadingZero);
    FORMAT_INT(lpMinute, wMinutes, 2, TRUE);
    FORMAT_INT(lpSecond, wSeconds, 2, TRUE);
} /* FormatTime */


/************************************************************************/
/*
**  IsValidTime():
**  Check if the time is valid.
**  The hour must be in 24 hours format.
**  Return TRUE if valid; FALSE otherwise.
*/
BOOL LOCAL PASCAL IsValidTime(UINT wHours, UINT wMinutes, UINT wSeconds)
{
    return(wHours   < 24
        &&  wMinutes < 60
        &&  wSeconds < 60);
} /* IsValidTime */


/************************************************************************/
/*
**  NGetDlgIntlTime():
**  Get the time input from a set of time input control according to the
**    Time Format specified in the International section of WIN.INI.
**  The argument iIDDInput1 is the ID of the first of three controls, in
**    consecutive order as follows:
**  If lpwSeconds is not NULL
**       wIDDInput1     --  ID of Hour
**       wIDDInput1 + 1 --  ID of Minute
**       wIDDInput1 + 2 --  ID of Second
**       wIDDInput1 + 3 --  ID of Time separator
**       wIDDInput1 + 4 --  ID of Time separator
**       wIDDInput1 + 5 --  ID of the AM/PM indication string
**  If lpwSeconds is NULL
**       wIDDInput1     --  ID of Hour
**       wIDDInput1 + 1 --  ID of Minute
**       wIDDInput1 + 2 --  ID of Time separator
**       wIDDInput1 + 3 --  ID of the AM/PM indication string
**
**  Return NDT_NO_INPUT if there is no input.
**  Return NDT_INVALID  if the input time is invalid.
**  Return NDT_VALID    if the input time is valid.
**  Only valid values will be passed back.
**
**  See also: NSetDlgIntTime().
*/
int SYM_EXPORT WINAPI NGetDlgIntlTime(HWND   hDlg,
                                  UINT   wIDDInput1,
                                  LPUINT lpwHours,
                                  LPUINT lpwMinutes,
                                  LPUINT lpwSeconds)
{
    char  szAMPM[INTL_MAXLEN_sAMPM];
    UINT  uHours;
    UINT  uMinutes;
    UINT  uSeconds;

    NIntlInit(NULL);

    /*
    **  Retrieve the input.
    */
    if (! RetrieveTime(hDlg, wIDDInput1++, &uHours)
    ||   ! RetrieveTime(hDlg, wIDDInput1++, &uMinutes))
        return(NDT_NO_INPUT);

    uSeconds = 0;
    if (lpwSeconds)
        {
        RetrieveTime(hDlg, wIDDInput1++, &uSeconds);
        wIDDInput1++;
        }

    /*
    **  Get the AM/PM indicator and check for valid format.
    */
    GetDlgItemText(hDlg, ++wIDDInput1, szAMPM, INTL_MAXLEN_sAMPM);
    if (! AdjustHours(&uHours, szAMPM)
    ||   ! IsValidTime(uHours, uMinutes, uSeconds))
        return(NDT_INVALID);

    *lpwHours = uHours;
    *lpwMinutes = uMinutes;
    if (lpwSeconds)
        *lpwSeconds = uSeconds;
    return(NDT_VALID);
} /* NGetDlgIntlTime */


/************************************************************************/
/*
**  RetrieveTime():
**  Get the hour, minute or second from a control and check for error.
**  Subprocedure of GetDlgIntlTime().
**  Return TRUE if successful, FALSE otherwise.
**  The value will not be passed back if error occurs.
*/
BOOL LOCAL PASCAL RetrieveTime(HWND hDlg, int iIDDInput, LPUINT lpuiValue)
{
    BOOL  bOK;
    UINT  uValue;

    uValue = GetDlgItemInt(hDlg, iIDDInput, &bOK, FALSE);
    if (! bOK)
        return(FALSE);
    *lpuiValue = uValue;
    return(TRUE);
} /* RetrieveTime */


/************************************************************************/
/*
**  NSetDlgIntlTime():
**  Format the time to a set of time input control according to the Time
**    Format specified in the International section of WIN.INI.
**  The argument iIDDInput1 is the ID of the first of five controls, in
**    consecutive order as follows:
**  If bDispSeconds is FALSE:
**       iIDDInput1     --  ID of Hour
**       iIDDInput1 + 1 --  ID of Minute
**       iIDDInput1 + 2 --  ID of Time separator
**       iIDDInput1 + 3 --  ID of the AM/PM indication string
**  If bDispSeconds is TRUE:
**       iIDDInput1     --  ID of Hour
**       iIDDInput1 + 1 --  ID of Minute
**       iIDDInput1 + 2 --  ID of Second
**       iIDDInput1 + 3 --  ID of Time separator
**       iIDDInput1 + 4 --  ID of Time separator
**       iIDDInput1 + 5 --  ID of the AM/PM indication string
**
**  This function does not return a value.
**
**  See also: NGetDlgIntTime().
*/
void SYM_EXPORT WINAPI NSetDlgIntlTime(HWND hDlg,
                                   UINT wIDDInput1,
                                   UINT wHours,
                                   UINT wMinutes,
                                   UINT wSeconds,
                                   BOOL bDispSeconds,
                                   BOOL bDispAMPM)
{
    char  szAMPM[INTL_MAXLEN_sAMPM];
    char  szSep[2];
    char  szHour[8];
    char  szMinute[8];
    char  szSecond[8];

    NIntlInit(NULL);

    FormatTime(szSep, (bDispAMPM ? (LPSTR)szAMPM : NULL),
                szHour, szMinute, szSecond,
                wHours, wMinutes, wSeconds);

    /*
    **  Display the resultant text in the controls.
    */
    SetDlgItemText(hDlg, wIDDInput1++, szHour);
    SetDlgItemText(hDlg, wIDDInput1++, szMinute);
    if (bDispSeconds)
        {
        SetDlgItemText(hDlg, wIDDInput1++, szSecond);
	    SetDlgItemText(hDlg, wIDDInput1++, szSep);
        }
    SetDlgItemText(hDlg, wIDDInput1++, szSep);
    if (bDispAMPM)
        SetDlgItemText(hDlg, wIDDInput1, szAMPM);
} /* NSetDlgIntlTime */


/************************************************************************/
/*
**  Section 4: Number & Currency
*/
#if 0                                   //  Not used yet...
/************************************************************************/
/*
**  NIntlIntegerGet():
**  Convert a string to a DWORD.  The value is passed back via lpdwValue.
**  Any preceding non-numeric characters are skipped.
**  Do not stop when the Thousand Character in the International Section
**      is encountered if the last argument is TRUE.
**
**  Return the pointer after the last numeric character.
**
*/
LPSTR WINAPI NIntlIntegerGet(LPDWORD lpdwValue,
                                  LPSTR   lpBuffer,
                                  BOOL    bWithThousandChar)
{
    NIntlInit(NULL);

   while (! isdigit(*lpBuffer))
      lpBuffer++;

#define IS_SEPARATOR(c)
    (bWithThousandChar  &&  c == s_szThousand[0])

   for (*lpdwValue = 0;
        isdigit(*lpBuffer)  ||  IS_SEPARATOR(*lpBuffer);
        lpBuffer++)
      *lpdwValue = (*lpdwValue * 10) + (*lpBuffer - '0');



   return(lpBuffer);
} /* NIntlIntegerGet */
#endif


/************************************************************************/
/*
**  NIntlFormatInteger():
**  Convert a DWORD in decimal to a string using the international format.
**  If the total of digits exceeds nMinPlaces, the result will be expanded,
**      just like printf().
**  Use '\0' for cPaddingChar if left-justification is desired.
**  Insert the Thousand Character in the International Section if the last
**      argument is TRUE.
**
**  Return the pointer to the terminating null byte.
*/
LPSTR SYM_EXPORT WINAPI NIntlFormatInteger(LPSTR lpBuffer,
                                     DWORD dwValue,
                                     int   nMinPlaces,
                                     char  cPaddingChar,
                                     BOOL  bInsertThousandChar)
{
    int   nPlace = 0;

    NIntlInit(NULL);

    /*
    **  9/3/91: Output '0' if the value is 0.
    */
    if (dwValue == 0)
        lpBuffer[nPlace++] = '0';

    while (dwValue > 0)
        {
        if (bInsertThousandChar  &&  ((nPlace + 1) % 4 == 0))
            lpBuffer[nPlace] = s_szThousand[0];
        else
            {
            lpBuffer[nPlace] = (char)(dwValue % 10 + '0');
            dwValue /= 10;
            }
        nPlace++;
        }

    while (nPlace < nMinPlaces)
        lpBuffer[nPlace++] = cPaddingChar;
    lpBuffer[nPlace] = EOS;

   STRREV(lpBuffer);

   return(StringGetEnd(lpBuffer));
} /* NIntlFormatInteger */


/************************************************************************/
/*
**  Format a number of type double with the international separators.
**  Pass -1 for nPrecision if INI_iDIGITS is to be used.
*/
STATUS SYM_EXPORT WINAPI NIntlFormatFloat(LPSTR  lpszBuffer,
                                  double dValue,
                                  int    nPrecision,
                                  BOOL   bInsertThousandChar)
{
    DWORD   dwIntegral;
    double  dFractional;

    NIntlInit(NULL);

    if (nPrecision < 0)
        nPrecision = s_iDigits;

    /*
    **  Put the minus sign in front if the number is negative.
    */
    if (dValue < 0)
        {
        dValue = -dValue;
        *lpszBuffer++ = '-';
        }

    dwIntegral = (DWORD)dValue;
    dFractional = dValue - dwIntegral;

    /*
    **  9/17/91: Return ERR if the fractional portion is not less than 1.
    */
    if (dFractional >= 1.0)
        {
        *lpszBuffer = EOS;
        return(ERR);
        }

    lpszBuffer = NIntlFormatInteger(lpszBuffer, dwIntegral, 1,
                    (char)(s_iLeadingZero == LEADING_ZERO_YES ? '0' : EOS),
                    bInsertThousandChar);

    if (nPrecision > 0)
        {
        int     nPower = nPrecision;

        *lpszBuffer++ = s_szDecimal[0];
        while (nPower-- > 0)          //  just don't want to call pow()...
            dFractional *= 10.0;
        NIntlFormatInteger(lpszBuffer, (DWORD)(dFractional+0.5),
                            nPrecision, '0', FALSE);
        }

    return(NOERR);
} /* NIntlFormatFloat */


/************************************************************************/
/*
**  Format the currency using the international symbols.
**  Pass -1 for nPrecision if INI_iCURRDIGITS is to be used.
*/
STATUS SYM_EXPORT WINAPI NIntlFormatCurrency(LPSTR  lpszBuffer,
                                     double dValue,
                                     int    nPrecision,
                                     BOOL   bInsertThousandChar)
{
    char    szValue[50];
    BOOL    bSymbolFirst;
    LPSTR   lpszFormat;
    LPSTR   lpsz1;
    LPSTR   lpsz2;

    NIntlInit(NULL);

    if (nPrecision < 0)
        nPrecision = s_iCurrDigits;

    if (dValue >= 0)
        {
        NIntlFormatFloat(szValue, dValue, nPrecision, bInsertThousandChar);
        /*
        **  Format the currency according to s_iCurrency:
        **      0:  $100
        **      1:  100$
        **      2:  $ 100
        **      3:  100 $
        */
        switch (s_iCurrency)
            {
            default:
            case 0:
            case 1:
                lpszFormat = "%s%s";
                bSymbolFirst = (s_iCurrency != 1);
                break;

            case 2:
            case 3:
                lpszFormat = "%s %s";
                bSymbolFirst = (s_iCurrency != 3);
                break;
            } /* switch */
        }
    else
        {
        NIntlFormatFloat(szValue, -dValue, nPrecision, bInsertThousandChar);
        /*
        **  Format the currency according to s_iCurrNeg:
        **  3/11/92: Added formats 8, 9, 10...
        **      0:  ($100)
        **      1:  -$100
        **      2:  $-100
        **      3:  $100-
        **      4:  (100$)
        **      5:  -100$
        **      6:  100-$
        **      7:  100$-
        **      8:  -100 $
        **      9:  -$ 100
        **     10:  100 $-
        */
        switch (s_iCurrNeg)
            {
            case 0:
            case 4:
                lpszFormat = "(%s%s)";
                bSymbolFirst = (s_iCurrNeg != 4);
                break;

            default:
            case 1:
            case 5:
                lpszFormat = "-%s%s";
                bSymbolFirst = (s_iCurrNeg != 5);
                break;

            case 2:
            case 6:
                lpszFormat = "%s-%s";
                bSymbolFirst = (s_iCurrNeg != 6);
                break;

            case 3:
            case 7:
                lpszFormat = "%s%s-";
                bSymbolFirst = (s_iCurrNeg != 7);
                break;

            case 8:
            case 9:
                lpszFormat = "-%s %s";
                bSymbolFirst = (s_iCurrNeg != 8);
                break;

            case 10:
                lpszFormat = "%s %s-";
                bSymbolFirst = FALSE;
                break;
            } /* switch */
        }

    if (bSymbolFirst)
        {
        lpsz1 = s_szCurrency;
        lpsz2 = szValue;
        }
    else
        {
        lpsz1 = szValue;
        lpsz2 = s_szCurrency;
        }
    wsprintf(lpszBuffer, lpszFormat, lpsz1, lpsz2);

    return(NOERR);
} /* NIntlFormatCurrency */

/////////////////////////////////////////////////////////////////////////////
// NIntlGetString
//
// Copies one of this module's internationalized strings to the caller's
// buffer.  wEnum is the index into the alpszEnumIndex of the string to be
// copied; it should be one of the INTL_ENUM_ values in SYMINTL.H.

LPSTR SYM_EXPORT WINAPI NIntlGetString( LPSTR lpBuffer, UINT wEnum )
{
                                        //-------------------------------
                                        // Make sure string are read in.
                                        //-------------------------------
    NIntlInit(NULL);

    if (wEnum < INTL_ENUM_COUNT)
        STRCPY( lpBuffer, alpszEnumIndex[wEnum] );
    else
        *lpBuffer = '\0';

    return lpBuffer;
}


// -------------------------------------------------------------------
// NIntlDateIsLegal
//
// This function verifies that a date is within the limits of DOS, and
// therefore verifies if it can be stamped on a file.
//
// Parameters:
//	year		the full year value to check (i.e., 1991)
//	month		the month value to check
//	day		the day value to check
//
// Returns:
//	Non-zero if the date is legal.  Otherwise, zero is returned.
// -------------------------------------------------------------------

BOOL SYM_EXPORT WINAPI NIntlDateIsLegal (UINT wYear, UINT wMonth, UINT wDay)
{
    BOOL        bResult = FALSE;
                                        // don't even bother if not
                                        // complete date
    if (wMonth && wDay)
        {
        wYear = AdjustYear (wYear, FALSE);

                                        // check for valid DOS years
        if (wYear >= 1980 && wYear <= 2099)
            bResult = IsValidDate (wMonth, wDay, wYear, TRUE);
        }

    return (bResult);
}


// -------------------------------------------------------------------
// NIntlTimeIsLegal
//
// This function verifies that a time is within the limits of DOS, and
// therefore verifies if it can be stamped on a file.
//
// Parameters:
//	hour		the hour value to check
//	minute		the minute value to check
//	second		the second value to check
//	military	TRUE if 24-hour time is acceptable
//
// Returns:
//	Non-zero if the date is legal.  Otherwise, zero is returned.
// -------------------------------------------------------------------

BOOL SYM_EXPORT WINAPI NIntlTimeIsLegal (UINT wHour, UINT wMinute,
					UINT wSecond, BOOL b24Hour)
{
    BOOL        bResult = FALSE;

    if (b24Hour)                        // check for a valid hour
	{
	if (wHour <= 23)
	    bResult = TRUE;
	}
    else
	{
	if (wHour >= 1 && wHour <= 12)
	    bResult = TRUE;
	}

    if (bResult)			// did the hour check pass?
	bResult = IsValidTime (wHour, wMinute, wSecond);

    return (bResult);
}



//***************************************************************************
// NIntlGetValidTime()
//
// Description:
//      Optains time from control.  If time is invalid, the time in the edit
// field is set to the value passed in to this routine.  The time that
// is processed must be as follows:
//
//      Hours:          0 -> 23
//      Min             0 -> 59
//
// Parameters:
//      hWnd                    [in] Window handle of control
//      lpwHour                 [in/out] Buffer to receive hours
//      lpwMin                  [in/out] Buffer to receive minutes
//      bDispAMPM               [in] TRUE if AM/PM displayed
//
// Return Value:
//      NOERR                   Time was valid
//      ERR                     Time entered was invalid
//***************************************************************************
// 04/21/1993 MARKL Function Created.
//***************************************************************************
STATUS SYM_EXPORT WINAPI NIntlGetValidTime( // Gets valid time
    HWND        hWnd,                   // [in] Handle to time field
    LPUINT      lpwHour,                // [in/out] Hour
    LPUINT      lpwMin,                 // [in/out] Minutes
    BOOL        bDispAMPM)              // [in] TRUE if AM/PM displayed
{
    char        szTimeBuf[INTL_MAXLEN_sLONGDATE + 1];
    STATUS      wRet = NOERR;
    UINT        wHour;
    UINT        wMin;

                                        // Find out what time the user
                                        // has entered.
    GetWindowText (hWnd, szTimeBuf, sizeof (szTimeBuf));
    NGetIntlTime (szTimeBuf, &wHour, &wMin, NULL);

    if (NIntlTimeIsLegal (wHour, wMin, 0, TRUE))
        {
        *lpwHour = wHour;
        *lpwMin  = wMin;
        }
    else
        {
                                        // The time is bolvine scat.  Reset.
        wRet = ERR;
        NSetIntlTime (szTimeBuf, *lpwHour, *lpwMin, 0, FALSE, bDispAMPM);
        SetWindowText (hWnd, szTimeBuf);
        }

                                        // Return to caller
    return (wRet);
}



//***************************************************************************
// NIntlGetValidDate()
//
// Description:
//      Optains date from control.  If date is invalid, the time in the edit
// field is set to the value passed in to this routine.  The date must be
// of the following:
//
//      Month:  1 -> 12
//      Date:   1 -> 28, 29, 30, 31 (as required by month/year)
//      Year:   80 -> ???
//
// Parameters:
//      hWnd                    [in] Window handle of control
//      lpwMonth                [in/out] Buffer to receive month
//      lpwDay                  [in/out] Buffer to receive day
//      lpwYear                 [in/out] Buffer to receive year
//
// Return Value:
//      NOERR                   Date was valid
//      ERR                     Date entered was invalid
//***************************************************************************
// 04/21/1993 MARKL Function Created.
//***************************************************************************
STATUS SYM_EXPORT WINAPI NIntlGetValidDate( // Gets valid date
    HWND        hWnd,                   // [in] Handle to date field
    LPUINT      lpwMonth,               // [in/out] Month
    LPUINT      lpwDate,                // [in/out] Date
    LPUINT      lpwYear)                // [in/out] Year
{
    char        szDateBuf[INTL_MAXLEN_sLONGDATE + 1];
    STATUS      wRet = ERR;
    UINT        wMonth;
    UINT        wDate;
    UINT        wYear;

                                        // Find out what time the user
                                        // has entered.
    GetWindowText (hWnd, szDateBuf, sizeof (szDateBuf));
    NGetIntlDate (szDateBuf,
                  &wMonth,
                  &wDate,
                  &wYear);

    if (NIntlDateIsLegal (wYear, wMonth, wDate))
        {
        wRet      = NOERR;

        wYear %= 100;

        *lpwYear  = wYear;
        *lpwMonth = wMonth;
        *lpwDate  = wDate;
        }
    else
        {
        NSetIntlDate (szDateBuf,
                      *lpwMonth,
                      *lpwDate,
                      *lpwYear);
        SetWindowText (hWnd, szDateBuf);
        }

                                        // Return to caller after seeing
                                        // if we need to reset the field.
    return (wRet);
}
#else // not SYM_WIN


void LOCAL PASCAL FormatTime (BYTE *pSep, BYTE *pAMPM, BYTE *pHour,
			BYTE *pMinute, BYTE *pSecond, UINT wHours,
			UINT wMinutes, UINT wSeconds);

/**************************************************************************
 **  NSetIntlLongDate ():
 **
 **  Set the date input into a buffer according to the Long Date Format
 **    specified in the International section of WIN.INI.
 **  If the date is invalid, a blank is displayed.
 **  The weekday is not included if bWeekday is FALSE.
 **
 **  This function does not return a value.
 **
 **  See also: NSetIntlDate ().
 **************************************************************************/

void PASCAL NSetIntlLongDate (BYTE *pBuffer, UINT uMonth, UINT uDay,
				UINT uYear, BOOL bWeekday)
{
    if (uYear < 1900)
	uYear += 1900;

    if (uYear < 1980)
	uYear += 100;

    _ConvertDateToString (pBuffer, (WORD)uMonth, (WORD)uDay, (WORD)uYear, FALSE, (BYTE)bWeekday);

} /* NSetIntlLongDate */

/************************************************************************/
/*
**  NSetIntlDate ():
**  Set the date input into a buffer according to the Short Date Format
**    specified in the International section of WIN.INI.
**  If the date is invalid, a blank is displayed.
**
**  This function does not return a value.
**
**  See also: NGetIntlDate (), NSetIntlLongDate ().
*/
void PASCAL NSetIntlDate (BYTE *pBuffer, UINT wMonth, UINT wDay, UINT wYear)
{
    auto	BYTE	*pFormat;
    auto	UINT	wYear1900;


    wYear1900 = wYear;

    if (wYear1900 < 1900)
	wYear1900 += 1900;

    if (wYear1900 < 1980)
	wYear1900 += 100;

    if (DateIsDOSLegal (wYear1900, wMonth, wDay) == TRUE)
	{
	*pBuffer = 0;

    	switch (international.dateFormat)
	    {
	    case 0:				/* USA, or mm/dd/yy Format	*/
	    	pFormat = "%d2%c1%02d3%c1%02d4";
	    	break;

	    case 1:				/* Europe, or dd.mm.yy Format 	*/
	    	pFormat = "%d3%c1%02d2%c1%02d4";
	    	break;

	    case 2:				/* Japanese, or yy-mm-dd Format */
	    	pFormat = "%d4%c1%02d2%c1%02d3";
	    	break;
	    }
					// if necessary, clip the year
	FastStringPrint (pBuffer, pFormat, international.dateChar,
			 wMonth, wDay, wYear % 100);
	}
    else
	*pBuffer = '\0';

} /* FormatDate */


void PASCAL NSetDlgIntlDate (DIALOGREC drDialog, void *pItem,
			     UINT wMonth, UINT wDay, UINT wYear,
			     BOOL bExcludeCentury)
{
    auto	BYTE	szScratch[80];

    NSetIntlDate (szScratch, wMonth, wDay, wYear);

    DialogSetItemText (drDialog, pItem, szScratch);
}

/************************************************************************/
/*
**  NSetIntlTime ():
**  Set the Time input into a buffer according to the Short Time Format
**    specified in the International section of WIN.INI.
**  If bDispAMPM is TRUE, the AM/PM indicator is appended at the end of the
**    time separated by a space.
**  If the Time is invalid, a blank is displayed.
**
**  This function does not return a value.
**
**  See also: NGetIntTime ().
*/

void PASCAL NSetIntlTime (BYTE *pBuffer, UINT uHours, UINT uMinutes,
		UINT uSeconds, BOOL bDispSeconds, BOOL bDispAMPM)
{
    char  szAMPM [10];
    char  szSpace [2] = { ' ', '\0' };
    char  szSep [2];
    char  szHour [8];
    char  szMinute [8];
    char  szSecond [8];

    /*
    **  Format the time into the buffer according to the international format.
    **  3/1/91: Need to explicitly cast szAMPM to a long pointer.
    */
    FormatTime (szSep,
                (bDispAMPM) ? szAMPM : NULL,
                szHour, szMinute, szSecond,
                uHours, uMinutes, uSeconds);

    STRCPY (pBuffer, szHour);
    STRCAT (pBuffer, szSep);
    STRCAT (pBuffer, szMinute);

    if (bDispSeconds)
        {
        STRCAT (pBuffer, szSep);
        STRCAT (pBuffer, szSecond);
        }

    if (bDispAMPM)
        {
        STRCAT (pBuffer, szSpace);
        STRCAT (pBuffer, szAMPM);
        }

} /* NSetIntlTime */


/************************************************************************/
/*
**  FormatTime ():
**  Format the Time in the order specified in WIN.INI.
**  Called by SetDlgIntlTime () and SetIntlTime ().
*/
void LOCAL PASCAL FormatTime (BYTE *pSep, BYTE *pAMPM, BYTE *pHour,
			BYTE *pMinute, BYTE *pSecond, UINT wHours,
			UINT wMinutes, UINT wSeconds)
{
    extern char FAR AM[];
    extern char FAR PM[];
    BYTE  b12Hours;

    pSep[0] = international.timeChar;
    pSep[1] = '\0';

    /*
    **  Check for valid format.
    */
    if (!DateIsDOSLegalTime (wHours, wMinutes, wSeconds, TRUE))
        {
        *pHour   = EOS;
        *pMinute = EOS;
        *pSecond = EOS;

        if (pAMPM)
            *pAMPM = EOS;

        return;
        }

    /*
    **  Use the appropriate format as specified in WIN.INI.
    **  Only Hour can have no leading zero.
    */
    b12Hours = (BYTE) (international.timeFormat == 0);

    /*
    **  The AM and PM indicating strings are the same for the 24-hour format.
    */
    if (pAMPM)
        STRCPY (pAMPM, (b12Hours && wHours < 12 ? AM : PM));

    /*
    **  Get the correct Hour display.
    */
    if (b12Hours  &&  wHours > 12)
        wHours -= 12;
    else if (wHours == 24)
        wHours = 0;

    /*
    **  Need to convert 0:mm AM to 12:mm AM.
    */
    if (pAMPM  &&  b12Hours  &&  wHours == 0)
            wHours = 12;

    ConvertWordToString(wHours, pHour);

    ConvertWordToString(wMinutes+100, pMinute);
    pMinute[0] = pMinute[1];
    pMinute[1] = pMinute[2];
    pMinute[2] = EOS;

    ConvertWordToString(wSeconds+100, pSecond);
    pSecond[0] = pSecond[1];
    pSecond[1] = pSecond[2];
    pSecond[2] = EOS;

} /* FormatTime */


#if     0
/************************************************************************/
/*
**  IsValidTime ():
**  Check if the time is valid.
**  The hour must be in 24 hours format.
**  Return TRUE if valid; FALSE otherwise.
*/
BYTE LOCAL PASCAL IsValidTime (WORD wHours, WORD wMinutes, WORD wSeconds)
{
    return ((BYTE) (wHours < 24 && wMinutes < 60 && wSeconds < 60));
} /* IsValidTime */
#endif


void PASCAL NSetDlgIntlTime (DIALOGREC drDialog, void *pItem,
                             UINT wHours, UINT wMinutes, UINT wSeconds,
                             BOOL bDispSeconds, BOOL bDispAMPM)
{
    auto	BYTE	szScratch [80];

    NSetIntlTime (szScratch, wHours, wMinutes, wSeconds,
    			(BYTE) bDispSeconds, (BYTE) bDispAMPM);

    DialogSetItemText (drDialog, pItem, szScratch);
}

int PASCAL NGetDlgIntlDate (DIALOGREC drDialog, void *pItem,
			    LPWORD lpwMonth, LPWORD lpwDay, LPWORD lpwYear)
{
    BYTE	szScratch[30];
    BYTE	*pText = szScratch;
    int		a,b,c;

    DialogGetItemText (drDialog, pItem, szScratch, sizeof (szScratch));

    if (*pText == '\0')
        return (ERR);

    /* Skip any junk before the first digits. */

    pText = szScratch;
    while (*pText != '\0' && !CharIsNumeric(*pText))
        pText++;
    if (*pText == '\0')
        return (0);


    /* Read the first number and skip past it. */

    ConvertStringToWord(&a, pText);
    while (*pText != '\0' && CharIsNumeric(*pText))
        pText++;
    if (*pText == '\0')
        return (ERR);


    /* Skip over the first separator. */

    if (*pText != '/' && *pText != '-' && *pText != '.' && *pText != international.dateChar)
        return (ERR);
    pText++;
    if (!CharIsNumeric(*pText))
        return (ERR);


    /* Read the second number and skip past it. */

    ConvertStringToWord(&b, pText);
    while (*pText != '\0' && CharIsNumeric(*pText))
        pText++;
    if (*pText == '\0')
        return (ERR);


    /* Skip over the second separator. */

    if (*pText != '/' && *pText != '-' && *pText != '.' && *pText != international.dateChar)
        return (ERR);
    pText++;
    if (!CharIsNumeric(*pText))
        return (ERR);


    /* Read the third number and skip past it. */

    ConvertStringToWord(&c, pText);
    while (*pText != '\0' && CharIsNumeric(*pText))
        pText++;


    switch (international.dateFormat)
        {
        case 0:
	default:
            *lpwMonth = a;
            *lpwDay   = b;
            *lpwYear  = c;
            break;

        case 1:
            *lpwMonth = b;
            *lpwDay   = a;
            *lpwYear  = c;
            break;

        case 2:
            *lpwMonth = b;
            *lpwDay   = c;
            *lpwYear  = a;
            break;
        }

    if (*lpwYear < 100)
        *lpwYear += (*lpwYear < 70) ? 2000 : 1900;

    if (!DateIsDOSLegal (*lpwYear, *lpwMonth, *lpwDay))
	return (ERR);

    return (NOERR);

} /* NGetDlgIntlDate */

int PASCAL NGetDlgIntlTime (DIALOGREC drDialog, void *pItem,
                                LPWORD lpwHours, LPWORD lpwMinutes,
                                LPWORD lpwSeconds)
{
    extern	BYTE FAR	AM[];
    extern	BYTE FAR	PM[];
    auto	BYTE		szScratch[30];
    auto	BYTE	       *pText = szScratch;
    auto	BYTE	       *pScratch;
    auto	BYTE		timeString[2] = { international.timeChar, 0 };


    *lpwHours = 0;
    *lpwMinutes = 0;
    *lpwSeconds = 0;

    DialogGetItemText (drDialog, pItem, szScratch, sizeof (szScratch));
    STRLWR (szScratch);

    if ((pText = (BYTE*) StringSearch (szScratch,
            timeString, STRLEN (szScratch))) != (void *) -1)
        {
	/* Make sure we have digits on both sides of timeChar */

        if (!CharIsNumeric(*(pText-1)) || !CharIsNumeric(*(pText+1)))
            return (ERR);

	/* Skip back over digits before timeChar, then read *lpwHourss */

        for (pScratch = pText - 1; CharIsNumeric(*pScratch) ;)
            pScratch--;
        ConvertStringToWord(lpwHours, pScratch + 1);
        if (*lpwHours > 23)
            return (ERR);

	/* Read digits after timeChar to get *lpwMinutess,
	 * then skip past those digits. */

        if (!CharIsNumeric(*(pText + 1)))
            return (ERR);
        ConvertStringToWord(lpwMinutes, pText + 1);
        if (*lpwMinutes > 59)
            return (ERR);
        for (pScratch = pText + 1; CharIsNumeric(*pScratch) ;)
            pScratch++;

	/* If there is a timeChar followed by more digits,
	 * skip over the timeChar and read the number as *lpwSecondss. */

	if (*pScratch == international.timeChar && CharIsNumeric(*++pScratch))
            ConvertStringToWord(lpwSeconds, pScratch);

        if (international.timeFormat == INTL_TIME_12HOUR)
            {
	        /* Look for an AM or PM string anywhere after the
	        * *lpwMinutess (or *lpwSecondss) and adjust the *lpwHourss accordingly. */

            if (StringSearch (pScratch, AM, STRLEN(pText)) != (BYTE *) -1)
                {
                if (*lpwHours == 12)
                    {
                    *lpwHours = 0;
                    }
                else if (*lpwHours > 12)
                    {
                    return (ERR);
                    }
                }
            else if (StringSearch(pText, PM, STRLEN(pText)) != (BYTE *) -1)
	        {
                if (*lpwHours != 12)
                    *lpwHours += 12;
	        }
            }

        if (!DateIsDOSLegalTime (*lpwHours, *lpwMinutes, *lpwSeconds, TRUE))
            return (ERR);
        }
    else
        return (ERR);

    return (NOERR);
}

#endif          // SYM_WIN


/*----------------------------------------------------------------------*
 * Formats elapsed time display                                         *
 *                                                                      *
 *   format:  2 hours, 1 minute, 23 seconds                             *
 *                                                                      *
 * 08/03/89 Brad: created                                               *
 * 06/30/90 Brad: Removed 'hundredths', since it is very inaccurate.    *
 *      Also changed so doesn't always print seconds, like it           *
 *      did before (This was changed by someone).                       *
 *----------------------------------------------------------------------*/
LPSTR SYM_EXPORT WINAPI NIntlTimeToLongString(LPSTR lpszBuffer,
                                          UINT uHour,
                                          UINT uMinute,
                                          UINT uSecond)
{
#ifndef SYM_WIN
    extern BYTE		* FAR hourString[];
    extern BYTE		* FAR minuteString[];
    extern BYTE		* FAR secondString[];
#endif
    BOOL	        bComma;
    char		spaceString[2];
    char		separatorString[2];
    char                szBuffer[80+1];
    UINT                uIndex;



    bComma = FALSE;

    spaceString [0] = ' ';
#ifdef SYM_WIN
    separatorString [0] = s_szList[0];
#else
    separatorString [0] = international.separatorChar;
#endif

    					/* Null terminate string	*/
    lpszBuffer [0] = spaceString [1] = separatorString [1] = EOS;

    if (uHour)
      	{
        bComma = TRUE;

        uIndex = (uHour == 1) ? 0 : 1;
#ifdef SYM_WIN
        LoadString(hInst_SYMKRNL, IDS_HOUR+uIndex, szBuffer, sizeof(szBuffer));
#else
        STRCPY(szBuffer, hourString[uIndex]);
#endif
	SPRINTF (lpszBuffer, szBuffer, uHour);
	}

    if (uMinute)
      	{
        if (bComma)
            {
	    STRCAT (lpszBuffer, separatorString);
	    if (separatorString[0] != ' ')
	    	STRCAT (lpszBuffer, spaceString);
	    }

        bComma = TRUE;

        uIndex = (uMinute == 1) ? 0 : 1;
#ifdef SYM_WIN
        LoadString(hInst_SYMKRNL, IDS_MINUTE+uIndex, szBuffer, sizeof(szBuffer));
#else
        STRCPY(szBuffer, minuteString[uIndex]);
#endif
	SPRINTF (lpszBuffer + STRLEN(lpszBuffer), szBuffer, uMinute);
	}

    if (uSecond)
    	{
    	if (bComma)
            {
            STRCAT (lpszBuffer, separatorString);
            if (separatorString[0] != ' ')
            	STRCAT (lpszBuffer, spaceString);
            }

        uIndex = (uSecond == 1) ? 0 : 1;
#ifdef SYM_WIN
        LoadString(hInst_SYMKRNL, IDS_SECOND+uIndex, szBuffer, sizeof(szBuffer));
#else
        STRCPY(szBuffer, secondString[uIndex]);
#endif
	SPRINTF (lpszBuffer + STRLEN(lpszBuffer), szBuffer, uSecond);
	}

    return (lpszBuffer);
}

/****************************************************************************/
//
//  IMMDestroy():
//
//  Kills the Input Method Manager.
//
#ifdef SYM_WIN32

BOOL SYM_EXPORT WINAPI NImmDestroy(HWND hWndOwner)
{
	LPFNIMMGETDEFAULTIMEWND lpfnImmGetDefaultIMEWnd = NULL;

#if SYM_DEBUG
// BEM - THIS DEBUG CODE FORCES A LINK TO ImmGetDefaultIMEWnd(), which is exactly what the
//       LoadLibrary()/GetProcAddress() stuff is here to avoid!  This causes a link error,
//       so I am therefore commenting this out.
//    char    szText[50];
//    wsprintf(szText, "NDL: IMM Destroyed, HWND=%04X, Owner: %04X\r\n", ImmGetDefaultIMEWnd(hWndOwner), hWndOwner);
//    OutputDebugString(szText);
#endif

		if(hImm32Dll)
		{
			lpfnImmGetDefaultIMEWnd =
					(LPFNIMMGETDEFAULTIMEWND)GetProcAddress(hImm32Dll, "ImmGetDefaultIMEWnd");
		}

    return(DestroyWindow(lpfnImmGetDefaultIMEWnd(hWndOwner)));

}

/****************************************************************************/
//
//  IMMCreate():
//
//  Creates the Input Method Manager.
//
HWND SYM_EXPORT WINAPI NImmCreate(HWND hWndOwner)
{

    auto    HWND    hWndIME = NULL;

	hImm32Dll = LoadLibrary ("IMM32.DLL");

	if (hImm32Dll)
	{
		hWndIME = CreateWindowEx(0,
                             "IME",                  // IME class
                             NULL,                   // no window title
                             WS_DISABLED | WS_POPUP, // disabled window
                             0, 0, 0, 0,             // no need to set size
                             hWndOwner,                   // owner window
                             NULL,
                             (HINSTANCE) GetWindowLong(hWndIME, GWL_HINSTANCE),
                             NULL);
	}

    return(hWndIME);

}
#endif






