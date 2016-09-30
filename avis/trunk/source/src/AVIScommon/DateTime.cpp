
//
//	DateTime class
//
//	This class provides a way of storing and retrieving date/time information
//	from the database.
//
//	Date time consists of YYYY-MM-DD.HH.MM.SS
//
//
//	Author:	Andy Klapper
//
//	$Log: $
//

#include <stdafx.h>
#include <ole2.h>
#include <stdio.h>
#include <time.h>

#include "DateTime.h"
#include "DateTimeException.h"

using namespace std;


double DateTime::AsOLEDouble()				// return the variant date time value
{
	double	vTime	= 0.0;
	string	httpTime;

	if (!null && AsHttpString(httpTime))
	{
		SYSTEMTIME	sysTime;
		char		weekday[32];
		char		monthStr[32];

		if (7 == sscanf((char *)httpTime.c_str(), "%s %d %s %d %d:%d:%d GMT",
						weekday, &sysTime.wDay, monthStr, &sysTime.wYear,
						&sysTime.wHour, &sysTime.wMinute, &sysTime.wSecond))
		{
			sysTime.wDayOfWeek		= 0;
			sysTime.wMilliseconds	= 0;
			MonthStrToInt(string(monthStr), sysTime.wMonth);
			SystemTimeToVariantTime(&sysTime, &vTime);
		}
	}

	return vTime;
}



static time_t GetLocalGMTdiff(void)
{
	_tzset();
	return _timezone - _daylight*(60*60);		// put in terms of hours
}

time_t DateTime::localGMTdiff = GetLocalGMTdiff();

DateTime::DateTime(std::string& theTime) :	null(true)
{
	char	monthStr[10];
	int		tmp;

	//
	//	First try the HTTP format string, look for the GMT end string
	//
	//	NOTE:	I convert into unix seconds using the tm struct because I do
	//			not want to program the logic of adding hours to a time stamp
	//			at 11:00pm Feb 28 or other tricky little problems that others
	//			have already solved for me.

	if (-1 != theTime.find("GMT"))
	{
		char	weekday[32];
		struct tm	tm;

		if (7 == sscanf((char *)theTime.c_str(), "%s %d %s %d %d:%d:%d GMT",
						weekday, &tm.tm_mday, monthStr, &tm.tm_year,
						&tm.tm_hour, &tm.tm_min, &tm.tm_sec))
		{
			if (tm.tm_year	> 2023 || tm.tm_year	< 1970)
				throw DateTimeException(DateTimeException::DateTimeStr,
										"tm.tm_year	> 2023 || tm.tm_year	< 1970");
			if (tm.tm_mday	> 31   || tm.tm_mday	< 1)
				throw DateTimeException(DateTimeException::DateTimeStr,
										"tm.tm_mday	> 31   || tm.tm_mday	< 1");
			if (tm.tm_hour	> 24   || tm.tm_hour	< 0)
				throw DateTimeException(DateTimeException::DateTimeStr,
										"m.tm_hour	> 24   || tm.tm_hour	< 0");
			if (tm.tm_min	> 59   || tm.tm_min  < 0)
				throw DateTimeException(DateTimeException::DateTimeStr,
										"tm.tm_min	> 59   || tm.tm_min  < 0");
			if (tm.tm_sec	> 59   || tm.tm_sec  < 0)
				throw DateTimeException(DateTimeException::DateTimeStr,
										"tm.tm_sec	> 59   && tm.tm_sec  < 0");

			tm.tm_year	-= 1900;
			tm.tm_isdst	=  false; //_daylight;

			ushort	month;

			if (MonthStrToInt(std::string(monthStr), month))
			{
				if (month > 12 || month < 1)
					throw DateTimeException(DateTimeException::DateTimeStr,
										"month > 12 || month < 1");
				tm.tm_mon	= month -1;
			}
			else
			{
				throw DateTimeException(DateTimeException::DateTimeStr,
										"MonthStrToInt failed");
			}

			time_t	unixSeconds = mktime(&tm);
			unixSeconds -= _timezone; // - _daylight*(60*60);

			struct tm	*lt = localtime(&unixSeconds);

			*this = *lt;
		}
	}
		//
		//	Try it in database format
	else if (6 == (tmp = sscanf((char *) theTime.c_str(), "%d-%d-%d-%d.%d.%d",
						&timeStamp.year, &timeStamp.month, &timeStamp.day,
						&timeStamp.hour, &timeStamp.minute, &timeStamp.second)))
	{
		timeStamp.fraction	= 0;

		if (!Validate())
			throw DateTimeException(DateTimeException::DateTimeStr,
										"Validate failed");
		null				= false;
	}
}

//
//	This takes the number of seconds in LOCAL TIME!
DateTime::DateTime(time_t theTime) : null(true)
{
	struct tm	*lt = localtime(&theTime);

	*this	= *lt;

	null	= false;
}


DateTime& DateTime::operator=(struct tm* tm)
{
	timeStamp.year		= tm->tm_year + 1900;
	timeStamp.month		= tm->tm_mon + 1;
	timeStamp.day		= tm->tm_mday;
	timeStamp.hour		= tm->tm_hour;
	timeStamp.minute	= tm->tm_min;
	timeStamp.second	= tm->tm_sec;
	timeStamp.fraction	= 0;

	if (!Validate())
		throw DateTimeException(DateTimeException::DateTimeOpAssgnTm,
										"Validate failed");
	null				= false;

	return *this;
}

DateTime::DateTime(ushort year, ushort month, ushort day,
			 ushort hour, ushort minute, ushort second)
			 :	null(true)
{
	timeStamp.year	= year;
	timeStamp.month	= month;
	timeStamp.day	= day;
	timeStamp.hour	= hour;
	timeStamp.minute= minute;
	timeStamp.second= second;
	timeStamp.fraction=0;

	if (!Validate())
		throw DateTimeException(DateTimeException::DateTimeParts,
										"Validate failed");

	null			= false;
}

//DateTime::operator time_t()
time_t DateTime::UnixSeconds()
{
	struct tm	tmStruct;

	tmStruct.tm_year= timeStamp.year - 1900;
	tmStruct.tm_mon	= timeStamp.month - 1;
	tmStruct.tm_mday= timeStamp.day;
	tmStruct.tm_hour= timeStamp.hour;
	tmStruct.tm_min	= timeStamp.minute;
	tmStruct.tm_sec	= timeStamp.second;
	tmStruct.tm_isdst	= -1; // _daylight;

	return mktime(&tmStruct);
}

DateTime::operator struct tm()
{
	struct tm	tmStruct;

	tmStruct.tm_year= timeStamp.year - 1900;
	tmStruct.tm_mon	= timeStamp.month - 1;
	tmStruct.tm_mday= timeStamp.day;
	tmStruct.tm_hour= timeStamp.hour;
	tmStruct.tm_min	= timeStamp.minute;
	tmStruct.tm_sec	= timeStamp.second;
	tmStruct.tm_isdst	= -1; // _daylight;
	mktime(&tmStruct);						// sets some items like day of week

	return tmStruct;
}


DateTime& DateTime::operator +(const time_t rhs)
{
time_t	us	= this->UnixSeconds();
struct tm *tm1	= localtime(&us);

	time_t	unixSeconds = this->UnixSeconds() + rhs;
	struct tm	*tm = localtime(&unixSeconds);

	*this = *tm;

	return *this;
}



DateTime& DateTime::operator -(const time_t rhs)
{
	time_t	unixSeconds = this->UnixSeconds() - rhs;
	struct tm	*tm = localtime(&unixSeconds);

	*this = *tm;

	return *this;
}


//
//	Return the time in GMT in the format
//
//	Thu, 23 May 1998 00:59:14 GMT

bool DateTime::AsHttpString(std::string& httpString)
{
	bool	rc	= false;

	if (!null)
	{
		char		buffer[128];
//		time_t		translateTime = (time_t) *this + localGMTdiff;
		time_t		translateTime = this->UnixSeconds(); // + _timezone; // - _daylight*(60*60));

		if (strftime(buffer, 32, "%a, %d %b %Y %H:%M:%S GMT", gmtime(&translateTime)))
		{
			httpString	= buffer;
			rc			= true;
		}
	}

	return rc;
}


//
//	Return the time in Local Time in the format
//
//	1998-8-26-19.11.07

bool DateTime::AsDBString(std::string& httpString)
{
	bool	rc	= false;

	if (!null)
	{
		char		buffer[128];

		if (sprintf(buffer, "%d-%02d-%02d-%02d.%02d.%02d", timeStamp.year,
					timeStamp.month, timeStamp.day, timeStamp.hour,
					timeStamp.minute, timeStamp.second))
		{
			httpString	= buffer;
			rc			= true;
		}
	}

	return rc;
}

std::string DateTime::AsHttpString(bool& okay)
{
	std::string	tmp("");
	okay = AsHttpString(tmp);

	return tmp;
}

std::string DateTime::AsDBString(bool& okay)
{
	std::string tmp("");
	okay = AsDBString(tmp);

	return tmp;
}


static	bool MonthIntToStr(ushort month, std::string& monthStr)
{
	if (month < 1)
		throw DateTimeException(DateTimeException::DateTimeMonthIntToStr,
										"month < 1");
	if (month > 12)
		throw DateTimeException(DateTimeException::DateTimeMonthIntToStr,
										"month > 12");

	switch (month)
	{
	case 1:		monthStr = "Jan";	break;
	case 2:		monthStr = "Feb";	break;
	case 3:		monthStr = "Mar";	break;
	case 4:		monthStr = "Apr";	break;
	case 5:		monthStr = "May";	break;
	case 6:		monthStr = "Jun";	break;
	case 7:		monthStr = "Jul";	break;
	case 8:		monthStr = "Aug";	break;
	case 9:		monthStr = "Sep";	break;
	case 10:	monthStr = "Oct";	break;
	case 11:	monthStr = "Nov";	break;
	case 12:	monthStr = "Dec";	break;

	default: return false;
	}

	return true;
}

bool DateTime::MonthStrToInt(std::string monthStr, ushort& monthInt)
{
	const char*	str = monthStr.c_str();

	monthInt	= 0xFFFF;

	switch (str[0])
	{
	case 'A':
		if ('p' == str[1] && 'r' == str[2])
			monthInt	= 4;
		else if ('u' == str[1] && 'g' == str[2])
			monthInt	= 8;
		break;

	case 'D':
		if ('e' == str[1] && 'c' == str[2])
			monthInt	= 12;
		break;

	case 'F':
		if ('e' == str[1] && 'b' == str[2])
			monthInt	= 2;
		break;

	case 'J':
		if ('a' == str[1] && 'n' == str[2])
			monthInt	= 1;
		else if ('u' == str[1] && 'l' == str[2])
			monthInt	= 7;
		else if ('u' == str[1] && 'n' == str[2])
			monthInt	= 6;
		break;

	case 'M':
		if ('a' == str[1] && 'r' == str[2])
			monthInt	= 3;
		else if ('a' == str[1] && 'y' == str[2])
			monthInt	= 5;
		break;

	case 'N':
		if ('o' == str[1] && 'v' == str[2])
			monthInt	= 11;
		break;

	case 'O':
		if ('c' == str[1] && 't' == str[2])
			monthInt	= 10;
		break;

	case 'S':
		if ('e' == str[1] && 'p' == str[2])
			monthInt	= 9;
		break;

	default:
		throw DateTimeException(DateTimeException::DateTimeMonthStrToInt,
										"InvalidMonth");
	}

	return 0xFFFF != monthInt;
}

