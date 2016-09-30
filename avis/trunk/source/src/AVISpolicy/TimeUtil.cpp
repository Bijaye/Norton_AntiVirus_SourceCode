//
// IBM AntiVirus Immune System
//
// File Name: TimeUtil.cpp
// Author:    Milosz Muszynski
//
// Time Utility Class
//
// $Log: $
//

// ------ standard headers ------------------------------------------
#include <time.h>
#include <string>

// ------ custom headers --------------------------------------------
#include "timeutil.h"



// ------ class TimeSrc ----------------------------------------------

//
// TimeSrc
//
// internal class
//

class TimeSrc
{
private:
    static	struct tm *   _tm;
    static	time_t        _clock;

public:
    static	int       sec           () { return _tm->tm_sec;  }   /* seconds after the minute - [0,59] */
    static	int       min           () { return _tm->tm_min;  }   /* minutes after the hour - [0,59] */
    static	int       hour          () { return _tm->tm_hour; }   /* hours since midnight - [0,23] */
    static	int       dayOfTheMonth () { return _tm->tm_mday; }   /* day of the month - [1,31] */
    static	int       month         () { return _tm->tm_mon;  }   /* months since January - [0,11] */
    static	int       year          () { return _tm->tm_year + 1900; }   /* years since 1900 */
    static	int       weekday       () { return _tm->tm_wday; }   /* days since Sunday - [0,6] */
    static	int       dayOfTheYear  () { return _tm->tm_yday; }   /* days since January 1 - [0,365] */

	static	void      get();
	static	void	  getGmt();
};

void TimeSrc::get()
{
   time( &_clock );             /* Get time in seconds */

   _tm = localtime( &_clock );  /* Convert time to struct tm form */
}

void TimeSrc::getGmt()
{
   time( &_clock );             /* Get time in seconds */

   _tzset();					/* Set the timezone variable */

   _clock += _timezone;			/* Adjust the locat time to obtain GMT */

   _tm = localtime( &_clock );  /* Convert time to struct tm form */
}





// ------ static data -----------------------------------------------------

std::string		WeekdayAbbr::_dayNames[ 7  ] = { "Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat" };
std::string		MonthAbbr::_monthNames[ 12 ] = { "Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec" };

struct tm *		TimeSrc::_tm;
time_t			TimeSrc::_clock;

char			TimeUtil::_buffer[ TimeUtil::BufferSize ];


// ------ class TimeUtil ----------------------------------------------


//
// now
//

std::string TimeUtil::now()
{
	TimeSrc::get();

	sprintf( _buffer, "%3s, %d %3s %4.4d %2.2d:%2.2d:%2.2d", WeekdayAbbr( TimeSrc::weekday() ).asString().c_str(),
															 TimeSrc::dayOfTheMonth(),
														   	 MonthAbbr( TimeSrc::month() ).asString().c_str(),
														   	 TimeSrc::year(),
														   	 TimeSrc::hour(),
														   	 TimeSrc::min(),
														   	 TimeSrc::sec() );

	return std::string( _buffer );
}

  
//
// gmt
//

std::string TimeUtil::gmt()
{
	TimeSrc::getGmt();

	sprintf( _buffer, "%3s, %d %3s %4.4d %2.2d:%2.2d:%2.2d GMT", WeekdayAbbr( TimeSrc::weekday() ).asString().c_str(),
																TimeSrc::dayOfTheMonth(),
														   		MonthAbbr( TimeSrc::month() ).asString().c_str(),
														   		TimeSrc::year(),
														   		TimeSrc::hour(),
														   		TimeSrc::min(),
														   		TimeSrc::sec() );

	return std::string( _buffer );
}

  
//
// timeOfDay
//

std::string TimeUtil::timeOfDay()
{
	TimeSrc::get();

	sprintf( _buffer, "%2.2d:%2.2d:%2.2d", TimeSrc::hour(), TimeSrc::min(), TimeSrc::sec() );

	return std::string( _buffer );
}


//
// date
//

std::string TimeUtil::date( const std::string& separator )
{
	TimeSrc::get();

	sprintf( _buffer, (std::string( "%2.2d" ) + separator + "%2.2d" + separator + "%4.4d").c_str(), TimeSrc::month() + 1, TimeSrc::dayOfTheMonth(), TimeSrc::year() );

	return std::string( _buffer );
}


//
// timestamp
//

std::string TimeUtil::timestamp()
{
	TimeSrc::get();

	sprintf( _buffer, "%2.2d/%2.2d/%4.4d %2.2d:%2.2d:%2.2d", 
		     TimeSrc::month(), TimeSrc::dayOfTheMonth(), TimeSrc::year(),
			 TimeSrc::hour(), TimeSrc::min(), TimeSrc::sec() );

	return std::string( _buffer );
}

