// Copyright 1996 Symantec, Peter Norton Computing Group
// -------------------------------------------------------------------------
// SLDATE.H
// Declaration of the CDate class
//
// $Header:   S:/DOSTRIAL/VCS/sldate.h_v   1.0   10 Jul 1997 13:33:08   MKEATIN  $
// -------------------------------------------------------------------------
// $Log:   S:/DOSTRIAL/VCS/sldate.h_v  $
// 
//    Rev 1.0   10 Jul 1997 13:33:08   MKEATIN
// Initial revision.
// 
//    Rev 1.0   03 Mar 1997 16:14:54   MKEATIN
// Initial revision.
// 
//    Rev 1.0   23 Jan 1997 17:01:02   JBELDEN
// Initial revision.
// 
//    Rev 1.3   16 Jan 1997 16:27:02   BMCCORK
// Made changes to eliminate warnings
// 
//    Rev 1.2   18 Nov 1996 18:35:32   JLAMB
// Added default constructor for CDate
//
//    Rev 1.1   28 Oct 1996 20:11:32   JLAMB
// Fixes for Win16
//
//    Rev 1.0   28 Oct 1996 19:41:50   JLAMB
// Initial revision.
/////////////////////////////////////////////////////////////////////////////

#ifndef __SLDATE_H__
#define __SLDATE_H__

typedef signed short int SHORT;   // this is neeeded for sldate.h

///////////////////////////////////////
// CDate

class CDate
{
public:
	CDate() : m_dwJulian( 0L ) {}
	CDate( DWORD dwJulianDay ) : m_dwJulian( dwJulianDay ) {}
	CDate( const CDate& date ) : m_dwJulian( date.m_dwJulian ) {}
	CDate( UINT uDay, UINT uMonth, UINT uYear );
	CDate( const struct tm* tmbuf );

	// operators
	CDate& operator = ( const CDate& date );
	CDate operator ++ ()		{ return CDate( ++m_dwJulian ); }
	CDate operator -- ()		{ return CDate( --m_dwJulian ); }

	// operations
	void Extract( struct tm* tmbuf ) const;
	int  Compare( const CDate& date ) const;
	UINT Year() const;
	void ToGregorian( UINT& uDay, UINT& uMonth, UINT& uYear ) const;
	
	// attributes
	UINT  Day() const			{ return (UINT)(m_dwJulian - ToJulian( 12, 31, Year() - 1 )); }
	BOOL  IsValid() const		{ return m_dwJulian > 0L; }
	UINT  WeekDay() const		{ return (UINT)(m_dwJulian % 7 + 1); }
	DWORD Hash() const			{ return m_dwJulian; }

	// global friends	
	friend CDate operator + ( const CDate& dt, DWORD dd )
		{ return CDate( dt.m_dwJulian + dd ); }
	friend CDate operator + ( DWORD dd, const CDate& dt )
		{ return CDate( dt.m_dwJulian + dd ); }
	friend CDate operator - ( const CDate& dt, DWORD dd )
		{ return CDate( dt.m_dwJulian - dd ); }
	friend DWORD operator - ( const CDate& d1, const CDate& d2 )
		{ return d1.m_dwJulian - d2.m_dwJulian; }
	friend BOOL	operator < ( const CDate& d1, const CDate& d2 )
		{ return d1.m_dwJulian < d2.m_dwJulian; }
	friend BOOL	operator <= ( const CDate& d1, const CDate& d2 )
		{ return d1.m_dwJulian <= d2.m_dwJulian; }
	friend BOOL	operator > ( const CDate& d1, const CDate& d2 )
		{ return d1.m_dwJulian > d2.m_dwJulian; }
	friend BOOL	operator >= ( const CDate& d1, const CDate& d2 )
		{ return d1.m_dwJulian >= d2.m_dwJulian; }
	friend BOOL	operator == ( const CDate& d1, const CDate& d2 )
		{ return d1.m_dwJulian == d2.m_dwJulian; }
	friend BOOL	operator != ( const CDate& d1, const CDate& d2 )
		{ return d1.m_dwJulian != d2.m_dwJulian; }

	// statics
	static DWORD Today();
	static BOOL  LeapYear( UINT uYear );
	static BOOL  DayWithinMonth( UINT uDay, UINT uMonth, UINT uYear );
	static DWORD ToJulian( UINT uDay, UINT uMonth, UINT uYear );

protected:
	// data validation
	static BOOL AssertWeekDayNumber( unsigned uDay )
		{ return uDay >=1 && uDay <=7; }
	static BOOL AssertIndexOfMonth( unsigned uMonth )
		{ return uMonth >=1 && uMonth <=12; }

private:
	DWORD m_dwJulian;

	static const SHORT anDaysInMonth[12];
	static const SHORT anFirstDayOfEachMonth[12];
};

///////////////////////////////////////
// Inline Implementations

// Assignment operator
inline CDate& CDate::operator = ( const CDate& date ) 
{ 
	m_dwJulian = date.m_dwJulian; 
	return *this; 
}

// Algorithm from K & R, "The C Programming Language", 1st ed.
inline BOOL CDate::LeapYear( UINT uYear )
{
	return ( uYear & 3) == 0 && ( uYear % 100 != 0 || uYear % 400 == 0 );
}

// Is a day (1-31) within a given month?
inline BOOL CDate::DayWithinMonth( UINT uDay, UINT uMonth, UINT uYear )
{
	if ( uDay <= 0 || !AssertIndexOfMonth( uMonth )) return FALSE;
	UINT uDays = anDaysInMonth[ uMonth - 1 ];
	if ( LeapYear( uYear ) && uMonth == 2 ) uDays++;
	return uDay <= uDays;
}

#endif // __CDATE_H__
