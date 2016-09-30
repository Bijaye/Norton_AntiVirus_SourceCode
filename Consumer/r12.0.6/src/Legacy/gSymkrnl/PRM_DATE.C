/* Copyright 1992 Symantec Corporation                                  */
/************************************************************************
 *                                                                      *
 * $Header:   S:/SYMKRNL/VCS/PRM_DATE.C_v   1.0   26 Jan 1996 20:22:20   JREARDON  $ *
 *                                                                      *
 * Description:                                                         *
 *      ConvertTimeToEncoded                                            *
 *      ConvertEncodedTimeToNormal                                      *
 *      ConvertDateToEncoded                                            *
 *      ConvertEncodedDateToNormal                                      *
 *      DateGetCurrentDateTime                                          *
 *                                                                      *
 * Contains:                                                            *
 *                                                                      *
 * See Also:                                                            *
 *                                                                      *
 ************************************************************************
 * $Log:   S:/SYMKRNL/VCS/PRM_DATE.C_v  $ *
// 
//    Rev 1.0   26 Jan 1996 20:22:20   JREARDON
// Initial revision.
// 
//    Rev 1.8   01 Nov 1994 13:30:06   JMILLARD
// 
// ifdef in some functions for NLM platform, some in a dummy way
// 
//    Rev 1.7   07 Jun 1994 03:44:04   GVOGEL
// Added comment blocks for Geologist project.
// 
//    Rev 1.6   15 Mar 1994 12:34:06   BRUCE
// Changed EXPORT to SYM_EXPORT
// 
//    Rev 1.5   25 Feb 1994 12:21:16   BRUCE
// Ran scripts to change to SYM_ constants and get ready for Win32
// 
//    Rev 1.4   12 Oct 1993 17:32:46   EHU
// Ifndef'd SYM_NLM ConvertEncoded{Date,Time}ToNormal.
// 
//    Rev 1.3   02 Jul 1993 08:52:08   DOUGN
// Change for conversion to C8.  Moved "include <dos.h>" to be above the
// "include "platform.h"" line so that MK_FP would be defined by the 
// C8 dos.h include file and platform.h wouldn't redefine it and cause 
// warnings.
// 
// 
//    Rev 1.2   12 Feb 1993 04:26:12   ENRIQUE
// No change.
// 
//    Rev 1.1   10 Sep 1992 18:39:02   BRUCE
// Eliminated unnecesary includes
// 
//    Rev 1.0   26 Aug 1992 17:00:40   ED
// Initial revision.
 ************************************************************************/

/*@Library: SYMKRNL*/

#include <dos.h>
#include "platform.h"
#include "undoc.h"
#include "xapi.h"

/*@API:**********************************************************************
@Declaration: UINT  ConvertTimeToEncoded(UINT wHours, UINT wMinutes, UINT 
wSeconds)

@Description:
This function combines hours, minutes, and seconds values into the 16-bit 
encoded time format used in DOS directory entries.  Seconds are adjusted to the 
closest even value.

@Parameters:
$wHours$ The hours value (0-23).

$wMinutes$ The minutes value (0-59).

$wSeconds$ The seconds value (0-59).

@Returns: 
The time value in encoded format.

@See: ConvertEncodedTimeToNormal ConvertDateToEncoded DateGetCurrentDateTime 
      DOSGetTime DOSGetDate
@Include: xapi.h
@Compatibility: DOS, Win16 and Win32
*****************************************************************************/
UINT SYM_EXPORT WINAPI ConvertTimeToEncoded(UINT wHours, UINT wMinutes, UINT wSeconds)
{
    register UINT time;
    
    time = wHours;
    time <<= 6;
    time += wMinutes;
    time <<= 5;
    time += wSeconds >> 1;

    return (time);
} /* ConvertTimeToEncoded */

/*@API:**********************************************************************
@Declaration: void ConvertEncodedTimeToNormal (UINT wTime, UINT FAR * lpwHours, 
UINT FAR * lpwMinute, UINT FAR * lpwSeconds)

@Description:
This function decodes a 16-bit encoded format time value into hours, minutes, 
and seconds.

@Parameters:
$wTime$ The encoded time value.

$lpwHours$ Pointer to the hours value.

$lpwMinute$ Pointer to the minutes value.

$lpwSeconds$ Pointer to the seconds value.

@See: ConvertTimeToEncoded
@Include: xapi.h
@Compatibility: DOS, Win16 and Win32
*****************************************************************************/
void SYM_EXPORT WINAPI ConvertEncodedTimeToNormal(UINT time, UINT FAR * hour, UINT FAR * minute, UINT FAR * seconds)
{
    register UINT encodedTime = time;


    *hour    = (encodedTime >> 11);
    *minute  = (encodedTime >> 5)  & 0x003F;
    *seconds = (encodedTime & 0x001F) << 1; /* encoded as 2-second intervals */
}


/*@API:**********************************************************************
@Declaration: void  DateGetCurrentDateTime (UINT FAR * lpwDate, UINT FAR * 
lpwTime)

@Description:
This function gets the current DOS time and date.  The values are then encoded 
as 16-bit values used in directory entries.

@Parameters:
$lpwDate$ Pointer to the encoded date value.

$lpwTime$ Pointer to the encoded time value.

@Returns: 
There is no return value for this function.

@See: DOSGetTime DOSGetDate
@Include: xapi.h
@Compatibility: DOS, Win16 and Win32
*****************************************************************************/
VOID SYM_EXPORT WINAPI DateGetCurrentDateTime (UINT FAR * datePtr, UINT FAR * timePtr)
{
    auto	UINT		month;
    auto	UINT		day;
    auto	UINT		year;
    auto	UINT		hour;
    auto	UINT		minute;
    auto	UINT		second;
    auto	UINT		hund;
					/*------------------------------*/
					/* Get the current date		*/
					/*------------------------------*/
    DOSGetDate (&month, &day, &year);
    *datePtr = ConvertDateToEncoded (month, day, year);
					/*------------------------------*/
					/* Get the current time		*/
					/*------------------------------*/
    DOSGetTime (&hour, &minute, &second, &hund);
    *timePtr = ConvertTimeToEncoded (hour, minute, second);
}

/*@API:**********************************************************************
@Declaration: UINT ConvertDateToEncoded(UINT wMonth, UINT wDay, UINT wYear)

@Description:
This function combines month, day, and year values into the 16-bit encoded date 
format used in DOS directory entries.  The year value is based on the number of 
years since 1980, with 1980 being equal to zero.  The year value may also be 
entered as a value starting with 1980.

@Parameters:
$wMonths$ The months value (1-23).

$wDay$ The day value (1-31).

$wYear$ The year value (0-127, or 1980-2107).

@Returns: 
The time value in encoded format.

@See: ConvertEncodedDateToNormal ConvertTimeToEncoded DateGetCurrentDateTime 
      DOSGetTime DOSGetDate
@Include: xapi.h
@Compatibility: DOS, Win16 and Win32
*****************************************************************************/
UINT SYM_EXPORT WINAPI ConvertDateToEncoded(UINT wMonth, UINT wDay, UINT wYear)
{
    register UINT date;

    /*
    **  Make the year starts at 1980.
    **  Assume 21st century if it is less than 80.
    */
    if (wYear >= 1980)
        wYear -= 1980;
    else if (wYear >= 80)
        wYear -= 80;
    else
        wYear += 20;

    date = wYear;
    date <<= 4;
    date += wMonth;
    date <<= 5;
    return (date + wDay);
} /* ConvertDateToEncoded */

/*@API:**********************************************************************
@Declaration: void ConvertEncodedDateToNormal (UINT wDate, UINT FAR * lpwMonth, 
UINT FAR * lpwDay, UINT FAR * lpwYear)

@Description:
This function decodes a 16-bit encoded date value into month, day, and year.  
The year value is 1980-adjusted.

@Parameters:
$wDate$ The encoded date value.

$lpwMonth$ Pointer to the month value.

$lpwDay$ Pointer to the day value.

$lpwYear$ Pointer to the year value.

@See: ConvertDateToEncoded ConvertEncodedTimeToNormal DateGetCurrentDateTime 
DOSGetTime DOSGetDate

@Include: xapi.h

@Compatibility: DOS, Win16 and Win32
*****************************************************************************/
void SYM_EXPORT WINAPI ConvertEncodedDateToNormal(UINT date, UINT FAR * month, UINT FAR * day, UINT FAR * year)
{
    register UINT encodedDate = date;


    *year  = (encodedDate >> 9) + 1980;
    *month = (encodedDate >> 5) & 0x000F;
    *day   = encodedDate & 0x001F;
}

