// Copyright 1996 Symantec, Peter Norton Computing Group
// -------------------------------------------------------------------------
// SLDATE.CPP
// Implementation of the CDate class
//
// $Header:   S:/DOSTRIAL/VCS/sldate.cpv   1.0   10 Jul 1997 13:33:02   MKEATIN  $
// -------------------------------------------------------------------------
// $Log:   S:/DOSTRIAL/VCS/sldate.cpv  $
// 
//    Rev 1.0   10 Jul 1997 13:33:02   MKEATIN
// Initial revision.
// 
//    Rev 1.0   03 Mar 1997 16:14:44   MKEATIN
// Initial revision.
// 
//    Rev 1.0   23 Jan 1997 16:59:34   JBELDEN
// Initial revision.
// 
//    Rev 1.3   16 Jan 1997 16:27:00   BMCCORK
// Made changes to eliminate warnings
// 
//    Rev 1.2   28 Oct 1996 21:08:30   JLAMB
// Fix to Year prototype
// 
//    Rev 1.1   28 Oct 1996 20:11:14   JLAMB
// Fixes for Win16
//
//    Rev 1.0   28 Oct 1996 19:41:10   JLAMB
// Initial revision.
/////////////////////////////////////////////////////////////////////////////
#include "platform.h"
#include "sldate.h"
#include <time.h>

/////////////////////////////////////////////////////////////////////////////
// CDate Class

const SHORT CDate::anDaysInMonth[12] = {
	31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };

const SHORT CDate::anFirstDayOfEachMonth[12] = { 
	0, 31, 59, 90, 120, 151, 181, 212, 243, 273, 304, 334 };

///////////////////////////////////////
// Construction

CDate::CDate( UINT uDay, UINT uMonth, UINT uYear )
{
	m_dwJulian = ToJulian( uDay, uMonth, uYear );
}

CDate::CDate( const struct tm* tmbuf )
{
	m_dwJulian = ToJulian( tmbuf->tm_mday, tmbuf->tm_mon + 1, 
		1900 + tmbuf->tm_year );
}

///////////////////////////////////////
// Extract - extract to struct tm components

void CDate::Extract( struct tm* tmbuf ) const
{
	if ( !IsValid() ) 
	{ 
		memset( tmbuf, 0, sizeof( tm ));
		return; 
	}

	UINT uDay, uMonth, uYear;
	ToGregorian( uDay, uMonth, uYear );
	
	tmbuf->tm_year = uYear - 1900;
	tmbuf->tm_mon = uMonth - 1;
	tmbuf->tm_mday = uDay;
	tmbuf->tm_wday = WeekDay() % 7;
	tmbuf->tm_yday = anFirstDayOfEachMonth[uMonth - 1] + ( uDay - 1 );
	
	if ( LeapYear( uYear ) && uMonth > 2 )
		++tmbuf->tm_yday;

	tmbuf->tm_hour = tmbuf->tm_min = tmbuf->tm_sec = 0;
	tmbuf->tm_isdst = -1;
}

///////////////////////////////////////
// Compare

int CDate::Compare( const CDate& date ) const
{
	return m_dwJulian == date.m_dwJulian ? 0 : 
		( m_dwJulian > date.m_dwJulian ? 1 : -1 );
}

///////////////////////////////////////
// Year - returns the year of this CDate

UINT CDate::Year() const
{
	UINT uDay, uMonth, uYear;
	ToGregorian( uDay, uMonth, uYear );
	return (UINT)(DWORD)uYear;
}

///////////////////////////////////////
// ToGregorian
/*
	Convert a Julian day number to its corresponding Gregorian calendar
	date.  Algorithm 199 from Communications of the ACM, Volume 6, No. 8,
	(Aug. 1963), p. 444.  Gregorian calendar started on Sep. 14, 1752.

	This function not valid before that.  
*/
void CDate::ToGregorian( UINT& uDay, UINT& uMonth, UINT& uYear ) const
{
	DWORD d;
	DWORD j = m_dwJulian - 1721119;
	uYear = (UINT)((( j << 2 ) - 1 ) / 146097 );

	j = ( j << 2 ) - 1 - 146097 * uYear;
	d = ( j >> 2 );
	j = (( d<< 2 ) + 3 ) / 1461;
	d = ( d << 2 ) + 3 - 1461 * j;
	d = ( d + 4 ) >> 2;

	uMonth = (UINT)( 5 * d - 3 ) / 153;
	d = 5 * d - 3 - 153 * uMonth;
	uDay = (UINT)(( d + 5 ) / 5 );
	uYear = (UINT)( 100 * uYear + j );

	if ( uMonth < 10 )
	{
		uMonth += 3;
	}
	else 
	{
		uMonth -= 9;
		uYear++;
	} 
} 

///////////////////////////////////////
// ToJulian
/*
	Convert Gregorian calendar date to the corresponding Julian day number j.  
	Algorithm 199 from Communications of the ACM, Volume 6, No. 8, (Aug. 1963), 
	p. 444.  Gregorian calendar started on Sep. 14, 1752. 
	
	This function not valid before that.  Returns 0 if the date is invalid.
*/

DWORD CDate::ToJulian( UINT uDay, UINT uMonth, UINT uYear )
{
	if ( uYear <= 99 ) 
		uYear += 1900;

	unsigned yh = uYear;
	unsigned long c, ya, j;

	if ( !DayWithinMonth( uDay, uMonth, uYear )) 
		return 0L;

	if ( uMonth > 2 ) 
	{
		// wash out the leap day
		uMonth -= 3;					
	}
	else       
	{ 
		uMonth += 9;	
		uYear--; 
	}

	c = uYear / 100;
	ya = uYear - 100 * c;
	j = (( 146097 * c )>>2 ) + (( 1461 * ya ) >> 2 ) + 
		( 153 * uMonth + 2 ) / 5 + uDay + 1721119;

	if ( CDate( j ).Year() != yh ) 
		return 0;

	return j;
} 

///////////////////////////////////////
// Today

DWORD CDate::Today()
{
	time_t clk = time( 0 );
	
	struct tm t;
	t = *localtime( &clk );

	return ToJulian( t.tm_mday, t.tm_mon + 1, t.tm_year + 1900 );
}
