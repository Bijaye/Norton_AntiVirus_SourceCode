/* Copyright 1992-94 Symantec Corporation                                  */
/************************************************************************
 *                                                                      *
 * $Header:   S:/SYMKRNL/VCS/FILETIME.C_v   1.3   19 Mar 1997 21:43:12   BILL  $ *
 *                                                                      *
 * Description:                                                         *
 *                                                                      *
 * Contains:                                                            *
 *      FileTimeCompare                                                 *
 *      FileTimeToDosTime                                               *
 *      DosTimeToFileTime                                               *
 *      FileGetDateString()      DOS,W32                                *
 *      FileGetTimeString()      DOS,W32                                *
 *      FileGetDateTimeString()  DOS,W32                                *
 *                                                                      *
 * See Also:                                                            *
 *                                                                      *
 * Note:                                                                *
 *      Used by WIN16/WIN32/DOS/OS2/VXD/NTK                             *
 *                                                                      *
 ************************************************************************
 * $Log:   S:/SYMKRNL/VCS/FILETIME.C_v  $ *
// 
//    Rev 1.3   19 Mar 1997 21:43:12   BILL
// Modified files to build properly under Alpha platform
// 
//    Rev 1.2   15 Aug 1996 10:58:14   BMCCORK
// Changed FileGetTime/Date string functions to STDCALL
// 
//    Rev 1.1   08 Feb 1996 17:03:16   RSTANEV
// Added SYM_NTK support.
//
//    Rev 1.0   26 Jan 1996 20:21:40   JREARDON
// Initial revision.
//
//    Rev 1.22   22 Nov 1995 13:15:08   MLELE
// Brought change from QAK6. cf log 1.18.1.1 -- "Modified function
// FileTimeToDosTime. ConvertFileTimeToSeconds does not work for NLMs.
// So I put it's equivalent C code here for NLMs.
//
//    Rev 1.21   06 Jun 1995 16:52:16   SKURTZ
// Return Date & Time as 0 if gmtime fail.
//
//    Rev 1.20   06 Jun 1995 16:00:38   SKURTZ
// Handle gmtime returning null in FileTimeToDosTime()
//
//    Rev 1.19   09 Mar 1995 21:07:10   RSTANEV
// FileGetDateString(), FileGetTimeString(), FileGetDateTimeString() now
// supported by DOS platform.  Note: If these functions are linked, they
// will pull a library generated in Quake/DOSTOOL!
//
//    Rev 1.18   06 Jan 1995 19:57:06   DAVID
// FileGetDateTimeString() stopped working, so I rewrote it.
//
//    Rev 1.17   13 Dec 1994 14:40:00   MARKK
// Still needed some more magic
//
//    Rev 1.16   13 Dec 1994 14:12:20   MARKK
// Re-Added functions that Brad ifdefed out in WIN32
//
//    Rev 1.15   12 Dec 1994 17:18:30   BRAD
// Added VxD support
//
//    Rev 1.14   01 Nov 1994 13:31:04   JMILLARD
//
// fix some WATCOM compiler whines, ifdef in some functions for NLM
//
//    Rev 1.13   24 Oct 1994 16:23:16   BRUCE
// Added FileGetDate/TimeString() functions for Win32
//
//    Rev 1.12   14 Jul 1994 16:06:10   BRAD
// Changed 'magic value' for 1/1/70 date.  Need to use UTC, rather than local
//
//    Rev 1.11   14 Jul 1994 12:39:46   BRAD
// Need to call LocalFileTime for WIN32, since time is UTC
//
//    Rev 1.10   01 Jun 1994 13:13:38   MARKK
// Don't make extended calls under DX
//
//    Rev 1.9   13 May 1994 17:54:36   BRAD
// Somehow lost all of gmtimeInverse().  Instead, it was calling mktime().
//
//    Rev 1.8   13 May 1994 13:58:42   BRAD
// Missing closing paren introduced when adding OS/2 support
//
//    Rev 1.7   13 May 1994 11:46:26   BRAD
// Now available for OS/2
//
//    Rev 1.6   11 May 1994 21:15:08   BRAD
// We now convert between FILETIMEs and DOS times
//
//    Rev 1.5   02 May 1994 17:47:40   BRUCE
// Changed to use LPCFILETIME
//
//    Rev 1.4   05 Apr 1994 09:09:36   BRAD
// Added guts for FileTimeCompare() for DOS/WIn16
//
//    Rev 1.3   01 Apr 1994 12:21:32   BRUCE
// Added include of undoc.h
//
//    Rev 1.2   31 Mar 1994 13:42:04   BRAD
// Added new INT 21h functions for FileTime <-> DosTime
//
//    Rev 1.1   28 Mar 1994 14:51:06   BRAD
// Cleaned up a warning
//
//    Rev 1.0   25 Mar 1994 18:49:08   BRAD
// Initial revision.
 ************************************************************************/


#include "platform.h"
#include "xapi.h"
#include "file.h"
#include <time.h>

#include "symkrnl.h"


#if defined(SYM_WIN32) || defined(SYM_DOS)

#if !defined(SYM_WIN32)
#include "stdos2.h"
#endif

////////////////////////////////////////////////////////////////////////
//
//   FileGetDateString()
//
// Description:
//
//  This function returns a string that represents the date in the
//  passed in FILETIME structure.  This function obeys international settings and
//  will return the date formatted according to the locale settings.
//
//  The non-date related information in the FILETIME structure are ignored.
//
// Parameters:
//  FileTime        - FILETIME whose date you want as a string
//  lpszDateString  - Buffer to receive file's date as a string
//  uBuffSize       - Size of lpszDateString buffer.  Pass 0 to
//                    have the required size returned.  In this case,
//                    lpszDateString is not referenced.
// Returns:
//  If the function succeeds and uBuffSize is non-zero, the return
//  value is the number of bytes written to lpszDateString.
//
//  If the function succeeds and uBuffSize is zero, the return value
//  is the required size, in bytes, for a buffer that can receive
//  the translated string.
//
//  If the function fails, the return value is ERR.
//
// See Also:
//
//   FileGetDateString(), FileGetDateTimeString()
////////////////////////////////////////////////////////////////////////
// 10/17/94 BEM Function created.
////////////////////////////////////////////////////////////////////////
UINT SYM_EXPORT WINAPI FileGetDateString(// Get a string version of a file's date
    FILETIME FileTime,                  // File time whose date you want as a string
    LPSTR lpszDateString,               // Buffer to receive date as string
    UINT uBuffSize)                     // Pass 0 to get required size
{
#if defined(SYM_WIN32)

    SYSTEMTIME SysTime;
    int nRet;
    LCID lcid = GetUserDefaultLCID();

    if (!FileTimeToSystemTime(&FileTime, &SysTime))
        {
        SYM_ASSERT(FALSE);
        return ERR;
        }

    nRet = GetDateFormat(lcid, DATE_SHORTDATE, &SysTime, NULL, lpszDateString, uBuffSize);
    SYM_ASSERT(nRet);
    if (!nRet)
        {
        nRet = ERR;
        }
    return nRet;

#else

    auto UINT uResultSize;
    auto WORD wFATDate, wFATTime, wMonth, wDay, wYear;
    auto char szBuffer[90];

                                        // Convert FILETIME to MM/DD/YY

    if ( FileTimeToDosTime ( &FileTime, &wFATDate, &wFATTime ) == FALSE )
        {
        return ( ERR );
        }

    ConvertEncodedDateToNormal ( wFATDate, &wMonth, &wDay, &wYear );

                                        // Build a local string w/ MM/DD/YY

    ConvertDateToString ( szBuffer, wMonth, wDay, wYear, TRUE );

    uResultSize = STRLEN ( szBuffer ) + 1;

                                        // Was the local buffer long enough?

    if ( uResultSize > sizeof(szBuffer) )
        {
        return ( ERR );
        }

                                        // Update user's buffer

    if ( uBuffSize )
        {
        if ( uBuffSize < uResultSize )
            {
            return ( ERR );
            }

        STRCPY ( lpszDateString, szBuffer );
        }

    return ( uResultSize );

#endif
}


////////////////////////////////////////////////////////////////////////
//
//   FileGetTimeString()
//
// Description:
//
//  This function returns a string that represents the time in the
//  passed in FILETIME structure.  This function obeys international settings and
//  will return the time formatted according to the locale settings.
//
//  The non-time related information in the FILETIME structure are ignored.
//
// Parameters:
//  FileTime        - FILETIME whose time you want as a string
//  lpszTimeString  - Buffer to receive file's time as a string
//  uBuffSize       - Size of lpszTimeString buffer.  Pass 0 to
//                    have the required size returned.  In this case,
//                    lpszTimeString is not referenced.
// Returns:
//  If the function succeeds and uBuffSize is non-zero, the return
//  value is the number of bytes written to lpszTimeString.
//
//  If the function succeeds and uBuffSize is zero, the return value
//  is the required size, in bytes, for a buffer that can receive
//  the translated string.
//
//  If the function fails, the return value is ERR.
//
// See Also:
//   FileGetDateString(), FileGetDateTimeString()
//
////////////////////////////////////////////////////////////////////////
// 10/17/94 BEM Function created.
////////////////////////////////////////////////////////////////////////
UINT SYM_EXPORT WINAPI FileGetTimeString(// Get a string version of a file's time
    FILETIME FileTime,                  // File time whose time you want as a string
    LPSTR lpszTimeString,               // Buffer to receive time as string
    UINT uBuffSize)                     // Pass 0 to get required size
{
#if defined(SYM_WIN32)

    SYSTEMTIME SysTime;
    int nRet;
    LCID lcid = GetUserDefaultLCID();

    if (!FileTimeToSystemTime(&FileTime, &SysTime))
        {
        SYM_ASSERT(FALSE);
        return ERR;
        }

    nRet = GetTimeFormat(lcid, 0L, &SysTime, NULL, lpszTimeString, uBuffSize);
    SYM_ASSERT(nRet);
    if (!nRet)
        {
        nRet = ERR;
        }
    return nRet;

#else

    auto UINT uResultSize, uHour, uMinute, uSecond;
    auto WORD wFATDate, wFATTime;
    auto char szBuffer[50];

                                        // Convert FILETIME to HH/MM/SS

    if ( FileTimeToDosTime ( &FileTime, &wFATDate, &wFATTime ) == FALSE )
        {
        return ( ERR );
        }

    ConvertEncodedTimeToNormal ( wFATTime, &uHour, &uMinute, &uSecond );

                                        // Build a local string w/ HH/MM

    ConvertTimeToString ( szBuffer, uHour, uMinute, FALSE );

    uResultSize = STRLEN ( szBuffer ) + 1;

                                        // Was the local buffer long enough?

    if ( uResultSize > sizeof(szBuffer) )
        {
        return ( ERR );
        }

                                        // Update user's buffer

    if ( uBuffSize )
        {
        if ( uBuffSize < uResultSize )
            {
            return ( ERR );
            }

        STRCPY ( lpszTimeString, szBuffer );
        }

    return ( uResultSize );

#endif
}



////////////////////////////////////////////////////////////////////////
//
//   FileGetDateTimeString()
//
// Description:
//
//  This function returns a string that represents the date and time in the
//  passed in FILETIME structure.  This function obeys international settings and
//  will return the time formatted according to the locale settings.
//
// Parameters:
//  FileTime            - FILETIME whose date and time you want as a string
//  lpszDateTimeString  - Buffer to receive file's date and time as a string
//  uBuffSize           - Size of lpszDateTimeString buffer.  Pass 0 to
//                        have the required size returned.  In this case,
//                        lpszDateTimeString is not referenced.
// Returns:
//  If the function succeeds and uBuffSize is non-zero, the return
//  value is the number of bytes written to lpszDateTimeString.
//
//  If the function succeeds and uBuffSize is zero, the return value
//  is the required size, in bytes, for a buffer that can receive
//  the translated string.
//
//  If the function fails, the return value is ERR.
//
// See Also:
//   FileGetDateString(), FileGetTimeString()
//
////////////////////////////////////////////////////////////////////////
// 10/17/94 BEM Function created.
////////////////////////////////////////////////////////////////////////
UINT SYM_EXPORT WINAPI FileGetDateTimeString(// Get a string version of a file's date and time
    FILETIME FileTime,                  // File time whose date and time you want as a string
    LPSTR lpszDateTimeString,           // Buffer to receive date and time as string
    UINT uBuffSize)                     // Pass 0 to get required size
{
    UINT uDateBytes;
    UINT uTimeBytes;
    UINT uSizeLeft;
    LPSTR lpszTime;

					// Get or measure the date string

    uDateBytes = FileGetDateString(FileTime, lpszDateTimeString, uBuffSize);
    if (uDateBytes == ERR)
	return ERR;

					// If getting data
    if (uBuffSize > 0)
        {
					// Error if no room left for time
        if (uDateBytes == uBuffSize)
	    return ERR;
					// Replace terminating null with space

	lpszDateTimeString[uDateBytes-1] = ' ';

					// Set up time buffer just after space

	lpszTime = lpszDateTimeString + uDateBytes;
	uSizeLeft = uBuffSize - uDateBytes;
	}
					// Just measuring, so no time buffer
    else
        {
        lpszTime = NULL;
        uSizeLeft = 0;
	}

					// Get or measure the time string

    uTimeBytes = FileGetTimeString(FileTime, lpszTime, uSizeLeft);
    if (uTimeBytes == ERR)
	return ERR;

					// Return length of concatinated strings
    return uDateBytes + uTimeBytes;
}
#endif


#if !defined(SYM_WIN32)


/*********************************************************************
** FileTimeCompare()
**
** Description:
**    Compares 2 FILETIME structs.  Same usage as WIN32's CompareFileTime().
**    This is just the WIN16/DOS/OS2 version of it.
**
** Return values:
**
** See Also:
**
**
** Future enhancements:
**      Only used for WIN16 and DOS.
**
***********************************************************************
**********************************************************************/
LONG SYM_EXPORT WINAPI FileTimeCompare(
        LPCFILETIME lpFileTime1,
        LPCFILETIME lpFileTime2)
{
    if ( lpFileTime1->dwHighDateTime < lpFileTime2->dwHighDateTime )
        return (-1);
    else if ( lpFileTime1->dwHighDateTime > lpFileTime2->dwHighDateTime )
        return (1);
    else                // The high DWORD is equal.  Check the low DWORD
        {
        if ( lpFileTime1->dwLowDateTime < lpFileTime2->dwLowDateTime )
            return (-1);
        else if ( lpFileTime1->dwLowDateTime > lpFileTime2->dwLowDateTime )
            return (1);
        else
            return (0);                 // They are equal dates
        }
}

#endif

#if !defined(SYM_VXD)

#if !defined(SYM_NTK)


//*********************************************************************
// LOCAL PROTOTYPE
//*********************************************************************
time_t LOCAL PASCAL gmtimeInverse (struct tm *tb);

static FILETIME fileTime1970 =          // 1/1/70 in UTC
    {
    0x45262800,
    0x019DB22A,
    };

time_t          seconds;
struct tm       timeStruct;

#ifndef _M_ALPHA
/*********************************************************************
** SubtractFileTime()
**
** Description:
**    Subtracts a filetime from another.
**    This is just the WIN16/DOS/OS2 version of it.
**
** Return values:
**
** See Also:
**
**
** Future enhancements:
**      Only used for WIN16 and DOS.
**
***********************************************************************
**********************************************************************/
VOID LOCAL PASCAL SubtractFileTime(LPFILETIME lpFileTimeDiff, LPCFILETIME lpFileTime1, LPCFILETIME lpFileTime2)
{
    WORD        wHiHiWord1, wHiLoWord1;
    WORD        wLoHiWord1, wLoLoWord1;
    WORD        wHiHiWord2, wHiLoWord2;
    WORD        wLoHiWord2, wLoLoWord2;


    wHiHiWord1 = HIWORD(lpFileTime1->dwHighDateTime);
    wHiLoWord1 = LOWORD(lpFileTime1->dwHighDateTime);
    wLoHiWord1 = HIWORD(lpFileTime1->dwLowDateTime);
    wLoLoWord1 = LOWORD(lpFileTime1->dwLowDateTime);

    wHiHiWord2 = HIWORD(lpFileTime2->dwHighDateTime);
    wHiLoWord2 = LOWORD(lpFileTime2->dwHighDateTime);
    wLoHiWord2 = HIWORD(lpFileTime2->dwLowDateTime);
    wLoLoWord2 = LOWORD(lpFileTime2->dwLowDateTime);

#ifndef SYM_NLM

    _asm {
        mov     dx, wLoLoWord2
        mov     cx, wLoHiWord2
        mov     bx, wHiLoWord2
        mov     ax, wHiHiWord2

        sub     wLoLoWord1, dx          // Subtract from lo to hi
        sbb     wLoHiWord1, cx
        sbb     wHiLoWord1, bx
        sbb     wHiHiWord1, ax
        }

#endif

    lpFileTimeDiff->dwHighDateTime = MAKELONG (wHiLoWord1, wHiHiWord1);
    lpFileTimeDiff->dwLowDateTime  = MAKELONG (wLoLoWord1, wLoHiWord1);
}
#endif

#ifndef _M_ALPHA
/*********************************************************************
** AddFileTime()
**
** Description:
**    Adds filetimes together.
**    This is just the WIN16/DOS/OS2 version of it.
**
** Return values:
**
** See Also:
**
**
** Future enhancements:
**      Only used for WIN16 and DOS.
**
***********************************************************************
**********************************************************************/
VOID LOCAL PASCAL AddFileTime(LPFILETIME lpFileTimeSum, LPCFILETIME lpFileTime1, LPCFILETIME lpFileTime2)
{
    WORD        wHiHiWord1, wHiLoWord1;
    WORD        wLoHiWord1, wLoLoWord1;
    WORD        wHiHiWord2, wHiLoWord2;
    WORD        wLoHiWord2, wLoLoWord2;


    wHiHiWord1 = HIWORD(lpFileTime1->dwHighDateTime);
    wHiLoWord1 = LOWORD(lpFileTime1->dwHighDateTime);
    wLoHiWord1 = HIWORD(lpFileTime1->dwLowDateTime);
    wLoLoWord1 = LOWORD(lpFileTime1->dwLowDateTime);

    wHiHiWord2 = HIWORD(lpFileTime2->dwHighDateTime);
    wHiLoWord2 = LOWORD(lpFileTime2->dwHighDateTime);
    wLoHiWord2 = HIWORD(lpFileTime2->dwLowDateTime);
    wLoLoWord2 = LOWORD(lpFileTime2->dwLowDateTime);

#ifndef SYM_NLM

    _asm {
        mov     dx, wLoLoWord2
        mov     cx, wLoHiWord2
        mov     bx, wHiLoWord2
        mov     ax, wHiHiWord2

        add     wLoLoWord1, dx          // Add from lo to hi
        adc     wLoHiWord1, cx
        adc     wHiLoWord1, bx
        adc     wHiHiWord1, ax
        }

#endif

    lpFileTimeSum->dwHighDateTime = MAKELONG (wHiLoWord1, wHiHiWord1);
    lpFileTimeSum->dwLowDateTime  = MAKELONG (wLoLoWord1, wLoHiWord1);
}
#endif


/***
*mktime.c - Normalize user time block structure
*
*   Copyright (c) 1987-1992, Microsoft Corporation. All rights reserved.
*
*Purpose:
*   Mktime converts the user's time structure (possibly incomplete)
*   into a fully defined structure with "normalized" values, and
*   converts it into a time_t value.
*
*******************************************************************************/

// Taken directly from CTIME.H, from std. 'C' source code
#define _DAY_SEC       (24L * 60L * 60L)    /* secs in a day */

#define _YEAR_SEC      (365L * _DAY_SEC)    /* secs in a year */

#define _FOUR_YEAR_SEC     (1461L * _DAY_SEC)   /* secs in a 4 year interval */

#define _DEC_SEC       315532800L       /* secs in 1970-1979 */

#define _BASE_YEAR     70L          /* 1970 is the base year */

#define _BASE_DOW      4            /* 01-01-70 was a Thursday */

#define _LEAP_YEAR_ADJUST  17L          /* Leap years 1900 - 1970 */

#define _MAX_YEAR      138L         /* 2038 is the max year */

/*
 * ChkAdd evaluates to TRUE if dest = src1 + src2 has overflowed
 */
#define ChkAdd(dest, src1, src2)    ( ((src1 >= 0L) && (src2 >= 0L) \
    && (dest < 0L)) || ((src1 < 0L) && (src2 < 0L) && (dest >= 0L)) )

/*
 * ChkMul evaluates to TRUE if dest = src1 * src2 has overflowed
 */
#define ChkMul(dest, src1, src2)    ( dest / src1 != src2 )

/***
*time_t mktime(tb) - Normalize user time block structure
*
*Purpose:
*   Mktime converts a broken-down time, in a tm structure, into calendar
*   time with the same encoding as that of the values returned by the
*   "time" function.  Three practical uses of this routine are:
*       (1) To have mktime fill in the tm_wday and tm_yday
*           values for the user.
*       (2) To pass in a time structure with "out of bounds"
*           values and have mktime.c "normalize" it (e.g., pass
*           in 1/35/87 and get back 2/4/87).
*       (3) Convert a broken-down time into a calendar time (in time_t
*           format).
*Entry:
*   struct tm *tb - pointer to a tm time structure to normalize, convert
*
*Exit:
*   Normal Return:
*   Mktime returns the specified calender time encoded as a value
*   of the type time_t (unsigned long).
*
*   Error Return:
*   If the calendar time cannot be represented mktime returns -1.
*
*Exceptions:
*   None.
*
*******************************************************************************/

time_t LOCAL PASCAL gmtimeInverse (struct tm *tb)
{

#ifdef SYM_NLM
    int __near _days[] = {0,1,2,3,4} ;      // this is completely dummy - NLM
                                            // doesn't have this provided
#else
    extern int __near _days[];
#endif

    struct tm *tbtemp;         /* temporary time structure */
    long tmptm1, tmptm2;           /* temps for hour and min. values */
    long tmzone;               /* diff between local and GMT */
    static long tmptm3;        /* temp for second values */

    /*
     * First, make sure tb->tm_year reasonably within range.
     */
    if ( ((tmptm1 = tb->tm_year) < _BASE_YEAR - 1) || (tmptm1 > _MAX_YEAR
      + 1) )
        goto err_mktime;

    /*
     * Adjust month value so it is in the range 0 - 11.  This is because
     * we don't know how many days are in months 12, 13, 14, etc.
     */
    tmptm1 += tb->tm_mon/12;
    tb->tm_mon %= 12;
    if ( tb->tm_mon < 0 ) {
        tb->tm_mon += 12;
        tmptm1--;
    }

    /*
     * Check that year count is still in range.
     */
    if ( (tmptm1 < _BASE_YEAR - 1) || (tmptm1 > _MAX_YEAR + 1) )
        goto err_mktime;

    /*
     * Calculate days elapsed minus one, in the given year, to the given
     * month. Note the leap year adjustment assumes tmptm1 > 0 (thus,
     * BASEYEAR needs to be > 1 to ensure this).
     */
    tmptm2 = _days[tb->tm_mon];
    if ( !(tmptm1 & 3) && (tb->tm_mon > 1) )
        tmptm2++;

#if 1
    tmzone = 0;
#else
    /*
     * Get time zone correction.
     */
    __tzset();
    tmzone = _timezone;
#endif

    /*
     * Calculate elapsed hours. No possibility of overflow in 16-bit world
     * (ints are 16 bits, longs are 32 bits).
     */
    tmptm3 = (  /* 365 days for each elapsed year */
            ((tmptm1 - _BASE_YEAR) * 365L)

            /* 1 day per elapsed leap year */
          + ((tmptm1 - 1L) >> 2) - _LEAP_YEAR_ADJUST

            /* see above */
          + tmptm2

            /* day of month */
          + (long)tb->tm_mday   )

            /* convert to hours */
          * 24L

            /* hours since midnight */
          + (long)tb->tm_hour

            /* hours to UTC */
          + tmzone / 3600L;

#if 0
    /*
     * Reduce tmzone mod hours (the hours have been added in above).
     */
    tmzone %= 3600L;
#endif
    /*
     * Calculate elapsed minutes. Must guard against overflow from here
     * on.
     */
    tmptm1 = tmptm3 * 60L;
    if ( ChkMul(tmptm1, tmptm3, 60L) )
        goto err_mktime;

    tmptm2 = tmptm1 + (long)tb->tm_min;
    if ( ChkAdd(tmptm2, tmptm1, (long)tb->tm_min) )
        goto err_mktime;

    tmptm3 = tmptm2 + tmzone / 60L;
    if ( ChkAdd(tmptm3, tmptm2, tmzone / 60L) )
        goto err_mktime;

#if 0
    /*
     * Reduce tmzone mod minutes (the minutes have added in above).
     */
    tmzone %= 60L;
#endif

    /*
     * Calculate elapsed seconds.
     */
    tmptm1 = tmptm3 * 60L;
    if ( ChkMul(tmptm1, tmptm3, 60L) )
        goto err_mktime;

    tmptm2 = tmptm1 + (long)tb->tm_sec;
    if ( ChkAdd(tmptm2, tmptm1, (long)tb->tm_sec) )
        goto err_mktime;

    tmptm3 = tmptm2 + tmzone;
    if ( ChkAdd(tmptm3, tmptm2, tmzone) )
        goto err_mktime;

    /*
     * Check that the number of elapsed seconds is >= 0.
     */
    if ( tmptm3 < 0L )
        goto err_mktime;

#if 1
    /*
     * Convert this second count back into a time block structure.
     */
    if ( (tbtemp = gmtime((time_t *)&tmptm3)) == NULL )
        goto err_mktime;
#else
    /*
     * Convert this second count back into a time block structure.
     */
    if ( (tbtemp = localtime(&tmptm3)) == NULL )
        goto err_mktime;

    /*
     * We now must compensate for DST. The ANSI rules are to use the
     * passed-in daylight flag if non-negative, otherwise compute if we
     * have DST. Recall that tbtemp has the time without DST compensation,
     * but has computed if we are in DST or not.
     */
    if ( (tb->tm_isdst > 0) || ((tb->tm_isdst < 0) && (tbtemp->tm_isdst
      > 0)) ) {
        /* compensate for DST */
        tmptm3 -= 3600L;
        if ( tmptm3 < 0L )
        goto err_mktime;
        tbtemp = localtime(&tmptm3);
    }
#endif

    /*
     * Update *tb and return the calculated time_t value
     */
    *tb = *tbtemp;
    return ( (time_t)tmptm3 );

    /*
     * All error paths end up here.
     */
err_mktime:
    return ( (time_t)(-1L) );
}

#endif // #if !defined(SYM_NTK)

/*********************************************************************
** FileTimeToDosTime()
**
** Description:
**    Converts the FileTime struct to a DOS date and time.
**
** Return values:
**
** See Also:
**
**
** Future enhancements:
**      Only used for WIN16/DOS/OS2/NTK
**
***********************************************************************
**********************************************************************/
//
// 11/22/95 MLELE  ConvertFileTimeToSeconds does not work for NLMs.
//                 So, I put C code here to achieve the same for NLMs.
//                 (Brought over from QAK6.)
//***********************************************************************

BOOL SYM_EXPORT WINAPI FileTimeToDosTime(
        LPCFILETIME lpFileTime,
        LPWORD lpFatDate,
        LPWORD lpFatTime)
{
#if defined(SYM_NTK)

    auto LARGE_INTEGER FileTime;
    auto TIME_FIELDS   TimeFields;

    SYM_ASSERT ( lpFileTime );
    SYM_ASSERT ( lpFatDate );
    SYM_ASSERT ( lpFatTime );

    FileTime.LowPart  = lpFileTime->dwLowDateTime;
    FileTime.HighPart = lpFileTime->dwHighDateTime;

    RtlTimeToTimeFields ( &FileTime, &TimeFields );

    *lpFatDate = ( TimeFields.Day ) |
                 ( TimeFields.Month << 5 ) |
                 ( ( TimeFields.Year - 1980 ) << 9 );

    *lpFatTime = ( TimeFields.Second / 2 ) |
                 ( TimeFields.Minute << 5 ) |
                 ( TimeFields.Hour << 11 );

    return ( TRUE );

#elif defined(SYM_WIN32)
    FILETIME    localFileTime;
                                        // First convert to local time
    FileTimeToLocalFileTime(lpFileTime, &localFileTime);
    return(FileTimeToDosDateTime(&localFileTime, lpFatDate, lpFatTime));
#else
    WORD        wDate, wTime;
    BOOL        bResult;

#if defined(SYM_WIN16) || defined(SYM_DOS)
    BOOL        bSupported = TRUE;
#endif

    wDate = wTime = 0;
                                        //-------------------------------
                                        // First try using LFN scheme.
                                        //-------------------------------

#if defined(SYM_WIN16) || defined(SYM_DOS)
    _asm
        {
        push    ds                      ; Save the DS value
        push    si

#if !defined(SYM_DOSX)
        mov     ax,71A7h                ; Extended function
        lds     si,lpFileTime           ; Get the filetime
        xor     bx,bx                   ; FileTime->DosTime

        stc                             ; Assume not supported
        DOS                             ; Make the DOS call
        jnc     noErr
        cmp     ax, 7100h               ; Unsupported?
        mov     ax, 0                   ; Return FALSE (error)
        jne     done                    ; No, supported, but error
#endif
        mov     bSupported, 0           ; Not supported.  Use old method
        jmp     short done
noErr:
        mov     ax, 1                   ; Return TRUE
        mov     wDate,dx                ; Get Dos Date
        mov     wTime,cx                ; Get Dos Time

done:
        mov     bResult, ax
        pop     si
        pop     ds
        }


    if ( !bSupported )
#endif
        {
        FILETIME        fileTimeDiff;

                                        // We need to compute this ourselves
                                        // Note that this will only work for
                                        // dates since 1/1/1970.  This is
                                        // because we use the std 'C' routines
                                        // and also because DOS dates can
                                        // only be in the range 1980-2099.
                                        // Also DOS's File second is only
                                        // precise to within 2-second intervals,
                                        // not 100-ns. intervals.

                                        // Algorithm is as follows:
                                        //
                                        //   seconds = (fileTime - fileTime1970)
                                        //             / (10^7)
                                        //
                                        // Once we have seconds since 1970,
                                        // give it to the std 'C' library,
                                        // which will compute the month/day/year.
                                        // Then encode it into DOS dates/times.
                                        //
        if ( FileTimeCompare(lpFileTime, &fileTime1970) > 0 )
            {
            struct tm* lpTM;


            SubtractFileTime(&fileTimeDiff, lpFileTime, &fileTime1970);

            //
            //  ConvertFileTimeToSeconds does not work for NLMs.
            //  So, NLMs will do the same in C here.
            //
#if !defined(SYM_NLM)

            seconds = ConvertFileTimeToSeconds(&fileTimeDiff);

#else   // else, if NLM

            {
            DWORD   dwHighDate = fileTimeDiff.dwHighDateTime;
            DWORD   dwLowDate  = fileTimeDiff.dwLowDateTime;

            //  Divide fileTimeDiff by 0x989680 (= 10^7)
            seconds = 0;
            while (dwHighDate > 0 || dwLowDate >= 0x989680)
                {

                if (dwLowDate < 0x989680)
                    dwHighDate--;

                dwLowDate -= 0x989680;
                seconds++;

                }
            }

#endif  // endif ! SYM_NLM

            lpTM = gmtime(&seconds);

            if (lpTM == NULL)
                {
                wDate = wTime = 0;
                bResult = FALSE;
                }
            else
                {
                MEMCPY(&timeStruct, lpTM, sizeof(struct tm));
                wDate = ConvertDateToEncoded(timeStruct.tm_mon+1, timeStruct.tm_mday, timeStruct.tm_year+1900);
                wTime = ConvertTimeToEncoded(timeStruct.tm_hour, timeStruct.tm_min, timeStruct.tm_sec);
                bResult = TRUE;
                }
            }
        else
            {
            wDate = wTime = 0;
            bResult = FALSE;
            }
        }

    *lpFatDate = wDate;
    *lpFatTime = wTime;
    return(bResult);
#endif
}


/*********************************************************************
** DosTimeToFileTime()
**
** Description:
**    Converts the Dos Time date/time to a fileTime struct.
**
** Return values:
**
** See Also:
**
**
***********************************************************************
**********************************************************************/
BOOL SYM_EXPORT WINAPI DosTimeToFileTime(
        WORD wFatDate,
        WORD wFatTime,
        LPFILETIME lpFileTime)
{
#if defined(SYM_NTK)

    auto TIME_FIELDS   TimeFields;
    auto LARGE_INTEGER FileTime;

    SYM_ASSERT ( lpFileTime );

    TimeFields.Year         = (wFatDate >> 9) + 1980;
    TimeFields.Month        = (wFatDate >> 5) & 0x000F;
    TimeFields.Day          = wFatDate & 0x001F;
    TimeFields.Hour         = wFatTime >> 11;
    TimeFields.Minute       = ( wFatTime >> 5 ) & 0x003F;
    TimeFields.Second       = ( wFatTime & 0x001F ) * 2;
    TimeFields.Milliseconds = 0;

    if ( RtlTimeFieldsToTime ( &TimeFields, &FileTime ) )
        {
        lpFileTime->dwLowDateTime  = FileTime.LowPart;
        lpFileTime->dwHighDateTime = FileTime.HighPart;
        return ( TRUE );
        }

    return ( FALSE );

#elif defined(SYM_WIN32)
    FILETIME    localFileTime;

    DosDateTimeToFileTime(wFatDate, wFatTime, &localFileTime);
                                        // Convert to UTC
    return(LocalFileTimeToFileTime(&localFileTime, lpFileTime));
#else
    BOOL        bResult;

#if defined(SYM_WIN16) || defined(SYM_DOS)

    BOOL        bSupported = TRUE;

#endif


                                        // Clear result
    MEMSET(lpFileTime, 0, sizeof(FILETIME));
                                        //-------------------------------
                                        // First try using LFN scheme.
                                        //-------------------------------
#if defined(SYM_WIN16) || defined(SYM_DOS)

    _asm
        {
        push    es                      ; Save the DS value
        push    di

#if !defined(SYM_DOSX)
        mov     ax,71A7h                ; Extended function
        mov     bx,1                    ; DosTime->FileTime (don't use 10ms)
        mov     cx,wFatTime             ; Get Dos time
        mov     dx,wFatDate             ; Get Dos date
        les     di,lpFileTime           ; Get fileTime

        stc                             ; Assume not supported
        DOS                             ; Make the DOS call
        jnc     noErr
        cmp     ax, 7100h               ; Unsupported?
        xor     ax, ax                  ; Return FALSE (error)
        jne     done                    ; No, supported, but error
#endif
        mov     bSupported, 0           ; Not supported.  Use old method
        jmp     short done
noErr:
        mov     ax, 1                   ; Return TRUE

done:
        mov     bResult, ax
        pop     di
        pop     es
        }

    if ( !bSupported )
#endif
        {
        FILETIME        fileTimeSoFar;

                                        // We need to compute this ourselves
                                        // Note that this will only work for
                                        // dates since 1/1/1970.  This is
                                        // because we use the std 'C' routines
                                        // and also because DOS dates can
                                        // only be in the range 1980-2099.
                                        // Also DOS's File second is only
                                        // precise to within 2-second intervals,
                                        // not 100-ns. intervals.

                                        // Algorithm is as follows:
                                        //
                                        // Decode DOS date/time into month/day/year
                                        // Send it to std. 'C' library, which
                                        // will give us # of seconds since
                                        // 1/1/70.
                                        //
                                        // fileTime = (seconds * 10^7) +
                                        //            fileTime1970
                                        //
        ConvertEncodedDateToNormal(wFatDate, (UINT FAR *)&timeStruct.tm_mon,
                                             (UINT FAR *)&timeStruct.tm_mday,
                                             (UINT FAR *)&timeStruct.tm_year);
        ConvertEncodedTimeToNormal(wFatTime, (UINT FAR *)&timeStruct.tm_hour,
                                             (UINT FAR *)&timeStruct.tm_min,
                                             (UINT FAR *)&timeStruct.tm_sec);

                                        // Update timeStruct
        timeStruct.tm_mon--;            // This is base 0 (0-11)
        timeStruct.tm_year -= 1900;     // Year since 1900

                                        // Get # of seconds since 1/1/70
        seconds = gmtimeInverse(&timeStruct);

        ConvertSecondsToFileTime(&fileTimeSoFar, seconds);
        AddFileTime(lpFileTime, &fileTimeSoFar, &fileTime1970);

        bResult = TRUE;
        }

    return(bResult);
#endif
}

#endif // #if !defined(SYM_VXD)

