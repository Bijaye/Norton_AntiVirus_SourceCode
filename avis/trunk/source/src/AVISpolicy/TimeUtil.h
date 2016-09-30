//
// IBM AntiVirus Immune System
//
// File Name: TimeUtil.h
// Author:    Milosz Muszynski
//
// Time Utility Class
//
// $Log: $
//

#ifndef _TIMEUTIL_H_
#define _TIMEUTIL_H_


class WeekdayAbbr
{
private:
	int		_day;
	static  std::string	 _dayNames[ 7 ];
public:
	explicit WeekdayAbbr( int day ) : _day( day % 7 ) {}

	std::string& asString() { return _dayNames[ _day ]; }
};


class MonthAbbr
{
private:
	int		_month;
	static  std::string  _monthNames[ 12 ];
public:
	explicit MonthAbbr( int month ) : _month( month % 12 ) {}

	std::string& asString() { return _monthNames[ _month ]; }
};


class TimeUtil
{
private:
	enum                        { BufferSize = 1024 };
	static char					_buffer[ BufferSize ];

public:
	static std::string now();
	static std::string timeOfDay();
	static std::string date( const std::string& separator = "/" );
	static std::string timestamp();
	static std::string gmt();
};


#endif _TIMEUTIL_H_
