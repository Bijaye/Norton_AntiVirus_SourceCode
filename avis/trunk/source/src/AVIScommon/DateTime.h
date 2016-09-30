//
//	IBM AntiVirus Immune System
//
//	File Name:	DateTime.h
//	Author:		Andy Klapper
//
//
//	The DateTime class represents a point in time.  It is stored
//	as the number of seconds from Jan 1, 1970, 12:00am Local Time.
//
//	When passed a string representation of the date and time the
//	following assumptions/restrictions apply:
//
//	HTTP format:	Must be in GMT format.
//	DB format:		Assumed to be in Local Time (that's how the database
//					stores it)
//
//
//	$Log: $
//	
//
//	Preliminary
//
//	IBM/Symantec Confidential
//

#ifndef DATETIME_H
#define DATETIME_H


#include <time.h>
#include <string>
#include <sqltypes.h>

#include "AVIS.h"
#include "AVISCommon.h"

class SQLStmt;

class AVISCOMMON_API DateTime
{
private:
	bool				null;			// a null time stamp?
	TIMESTAMP_STRUCT	timeStamp;		// SQL structure for storing time.

public:
										// creates a timestamp of now
	DateTime(void) :	null(true)
		{ *this = Now(); null = false; };

	explicit DateTime(const char* theTime) : null(true)	{ *this = DateTime(std::string(theTime)); null = false; }
	explicit DateTime(std::string& theTime);		// string
	explicit DateTime(time_t theTime);			// Unix second count, Local Time
	explicit DateTime(struct tm& tm) : null(true) { *this = tm; null = false; };		// tm in LOCAL TIME
										// Explicit (note this is 1 based counting!)
	DateTime(ushort year, ushort month, ushort day,
			 ushort hour = 0, ushort minute = 0, ushort second = 0);

	~DateTime(void) {};

	bool	IsNull(void)	const	{ return null; };
	void	Null(bool isNull)	{ null = isNull; };	// used if the db value is null


	//
	//	Conversion operators
//	operator time_t();
	time_t	UnixSeconds(void);
	operator struct tm();
	DateTime& operator=(struct tm* rhs);
	DateTime& operator=(struct tm& rhs) { return *this = &rhs; };


	double	AsOLEDouble();		// return the variant date time value

	//
	//	Returns the number of seconds between the two DateTimes
	long operator- (DateTime& rhs)
	{
		return this->UnixSeconds() - rhs.UnixSeconds();
	};

	//
	//	Adds/Subtracts some number of seconds from the current time
	DateTime& operator+ (const time_t rhs); // { unixSeconds += rhs; return *this; };
	DateTime& operator- (const time_t rhs);


	//
	//	Comparision operations between DateTimes
//	bool operator< (const DateTime& rhs);

	//
	//	returns -1 if lhs < rhs
	//	returns  1 if lhs > rhs
	//	returns  0 if lhs == rhs
	int	compare(const DateTime& rhs) const
	{
		if (IsNull())
		{
			if (rhs.IsNull())
				return 0;
			else
				return 1;
		}
		else if (rhs.IsNull())
			return -1;

		if (timeStamp.year < rhs.timeStamp.year)
			return -1;
		else if (timeStamp.year > rhs.timeStamp.year)
			return 1;
		else if (timeStamp.month < rhs.timeStamp.month)
			return -1;
		else if (timeStamp.month > rhs.timeStamp.month)
			return 1;
		else if (timeStamp.day < rhs.timeStamp.day)
			return -1;
		else if (timeStamp.day > rhs.timeStamp.day)
			return 1;
		else if (timeStamp.hour < rhs.timeStamp.hour)
			return -1;
		else if (timeStamp.hour > rhs.timeStamp.hour)
			return 1;
		else if (timeStamp.minute < rhs.timeStamp.minute)
			return -1;
		else if (timeStamp.minute > rhs.timeStamp.minute)
			return 1;
		else if (timeStamp.second < rhs.timeStamp.second)
			return -1;
		else if (timeStamp.second > rhs.timeStamp.second)
			return 1;
		else if (timeStamp.fraction < rhs.timeStamp.fraction)
			return -1;
		else
			return 0;
	}

	bool operator< (const DateTime& rhs) { return -1 == compare(rhs); }
	bool operator> (const DateTime& rhs) { return  1 == compare(rhs); };
	bool operator== (const DateTime& rhs) { return  this == &rhs || 0 == compare(rhs); };
	bool operator>=(const DateTime& rhs) { int c = compare(rhs); return  1 == c || 0 == c; };
	bool operator<=(const DateTime& rhs) { int c = compare(rhs); return -1 == c || 0 == c; };

	//
	//	Returns the current date & time
	static DateTime	Now(void) { DateTime	dt(time(NULL)); return dt; };

	bool	AsHttpString(std::string& httpString);
	bool	AsDBString(std::string& dbString);

	std::string	AsHttpString(bool& okay);
	std::string AsDBString(bool& okay);

private:
	enum TimeFormat { TFDatabase, TFHttp };

	static	time_t	localGMTdiff;

	bool	MonthStrToInt(std::string monthStr, ushort& monthInt);
	bool	IntStrToMonth(ushort monthInt, std::string& monthStr);

	bool	Validate(void)
			{
				if ((timeStamp.year	< 2023 && timeStamp.year	> 1970)		&&
					(timeStamp.month< 13   && timeStamp.month	> 0)		&&
					(timeStamp.day	< 32   && timeStamp.day		> 0)		&&
					(timeStamp.hour	< 25   && timeStamp.hour	> -1)		&&
					(timeStamp.minute < 60   && timeStamp.minute  > -1)		&&
					(timeStamp.second < 60   && timeStamp.second  > -1)		)
					return true;
				else 
					return false;
			};


	//
	//	I'd rather make only the methods in SQLStmt that need access to DateTime
	//	member variables friends, but I have some problems with which header file
	//	comes first if I do.

	friend class SQLStmt;
};

#endif
