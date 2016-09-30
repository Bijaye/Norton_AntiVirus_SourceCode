// Copyright 2004-2005 Symantec Corporation. All rights reserved.
// Author: Bertil Askelid <Bertil_Askelid@Symantec.com>
// Intent: Linux implementation of NDK Time Handling functions

// ================== ndkTime ===========================================================

extern "C" {
	#include <sys/time.h>
	#include <sys/types.h>
	#include <sys/stat.h>
	#include <fcntl.h>
	#include <math.h>
	#include <string.h>
	#include <stdlib.h>
	#include <stdio.h>

	#include <ndkIntegers.h>

	#include <ndkTime.h>
}

// --------------------- Facilities -----------------------------------------------------

class ProcessStartTime {
public:
	ProcessStartTime();

	void now(struct timeval* time);
	LONG getProcessClockTicks();

private:
	struct timeval  _t0;	// seconds since the Epoch (00:00:00 UTC, January 1, 1970)
	struct timezone _tz;
};
ProcessStartTime::ProcessStartTime() {
	_tz.tz_minuteswest = 0; _tz.tz_dsttime = 0; now(&_t0);
}
void ProcessStartTime::now(struct timeval* time) {gettimeofday(time,&_tz);}

LONG ProcessStartTime::getProcessClockTicks() {
	struct timeval t; now(&t);
	return
		(t.tv_sec  - _t0.tv_sec ) * 18 +
		(t.tv_usec - _t0.tv_usec) * 18 / 1000 / 1000;
}

static ProcessStartTime t0;

// --------------------- Process Clock Ticks --------------------------------------------

// The number of clock ticks (1/18th second) since the server was last loaded and began
// execution.
//
// This call is useful to determine the current relative time in order to determine the
// elapsed time between events. The current time value less the value returned at the
// beginning of an event is the elapsed time since the event occurred in 1/18th second
// clock ticks. It requires more than 2,761 days (over 7.5 years) of continuous server
// operation before this timer will roll over.

// The standard Linux process timing functions have a clock tick that is *much* more than
// 18 times a second. Therefore, we have to revert to this in order to maintain the
// precision and endurance of the NetWare function `GetCurrentTime'.

LONG GetCurrentTime() {return t0.getProcessClockTicks();}

// Returns the current server up-time in ticks (approximately eighteenths of a second).

LONG GetCurrentTicks() {return GetCurrentTime();}

// --------------------- Timer ----------------------------------------------------------

// Returns the current time in 100 microsecond increments.

LONG GetHighResolutionTimer() {
	struct timeval t; t0.now(&t);
	return t.tv_sec * 10 * 1000 + t.tv_usec / 100;
}

// --------------------- Convert to DOS Time Format -------------------------------------

// Converts calendar time to DOS-style date and time (UTC):
//
//		fileDate: Y		  M	D		  Year:	F-9	0 - 119 years since 1980
//					 7654321076543210	  Month: 8-5	1 - 12
//					 FEDCBA9876543210	  Day:	4-0	1 - 31
//					 Byte 1	Byte 0
//
//		fileTime: H		M		S*2	  Hour:	F-B	0 - 23
//					 7654321076543210	  Min:	A-5	0 - 59
//					 FEDCBA9876543210	  Sec/2: 4-0	0 - 29 two second increments
//					 Byte 1	Byte 0
//
// Calendar time represents the time in seconds since 12:00:00 am, January 1, 1970 UTC

#define PUT_YEAR_IN_DATE(    year,    date) date |= (WORD) (((year)    <<  9) & 0xfe00)
#define PUT_MONTH_IN_DATE(   month,   date) date |= (WORD) (((month)   <<  5) & 0x01e0)
#define PUT_DAY_IN_DATE(     day,     date) date |= (WORD) ((day)             & 0x001f)

#define PUT_HOUR_IN_TIME(    hour,    time) time |= (WORD) (((hour)    << 11) & 0xf800)
#define PUT_MINUTE_IN_TIME(  minute,  time) time |= (WORD) (((minute)  <<  5) & 0x07e0)
#define PUT_BISECOND_IN_TIME(bisecond,time) time |= (WORD) ((bisecond)        & 0x001f)

void _ConvertTimeToDOS(time_t calendarTime,
							  struct _DOSDate* fileDate,struct _DOSTime* fileTime) {
	
	struct tm time; gmtime_r(&calendarTime,&time);
	
	WORD *pDate = &fileDate->packed_date,*pTime = &fileTime->packed_time;
	memset(pDate,0,sizeof(WORD)); memset(pTime,0,sizeof(WORD));

	PUT_YEAR_IN_DATE(		time.tm_year - 80,*pDate);
	PUT_MONTH_IN_DATE(	time.tm_mon + 1,	*pDate);
	PUT_DAY_IN_DATE(		time.tm_mday,		*pDate);

	PUT_HOUR_IN_TIME(		time.tm_hour,	   *pTime);
	PUT_MINUTE_IN_TIME(	time.tm_min,	   *pTime);
	PUT_BISECOND_IN_TIME(time.tm_sec / 2,  *pTime);
}

// --------------------- CPU Utilization ------------------------------------------------

int GetCPUUtilization() {
   char *uptime = "/proc/uptime",*ro = "r",*format = "%f %f";
   FILE* file;
   float up0, up, idle0, idle;
   int fail = 0;
   struct timespec time;
   time.tv_sec = 0;
   time.tv_nsec = 50 * 1000 * 1000;

   if (! (file = fopen(uptime,ro))) return fail;
   fscanf(file,format,&up0,&idle0); fclose(file); nanosleep(&time,NULL);
   if (! (file = fopen(uptime,ro))) return fail;
   fscanf(file,format,&up,&idle); fclose(file);

	return (up -= up0) == 0 ? 0 :
		min(abs((int)round(100.0 - ((100.0 * (idle - idle0)) / up))), 100);
}
