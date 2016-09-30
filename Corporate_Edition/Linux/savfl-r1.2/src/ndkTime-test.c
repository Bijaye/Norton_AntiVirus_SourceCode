// Copyright 2004-2005 Symantec Corporation. All rights reserved.
// Author: Bertil Askelid <Bertil_Askelid@Symantec.com>
// Intent: Test Linux implementation of NDK Time Handling functions

// ================== ndkTime-test ======================================================

#include <sys/time.h>

#include <ndkTime.h>

#include "ndkDebug.h"

// --------------------- Process Clock Ticks --------------------------------------------

static void processClockTicks() {
	long t0,t1,t;

	dbgLng("t0 = GetCurrentTime() -> %d",t0 = GetCurrentTime());

	dbgAwait(5 * 1000);	// 90 clocks ticks

	dbgLng("t1 = GetCurrentTime() -> %d",t1 = GetCurrentTime());
	dbgLng("t1 - t0 == %d",t = t1 - t0);
}

// --------------------- Timer ----------------------------------------------------------

static void timer() {
	long t0,t1,t;

	dbgLng("t0 = GetHighResolutionTimer() -> %u", t0 = GetHighResolutionTimer());

	dbgAwait(5 * 1000);	// 50 * 1000 in 100 usec increments

	dbgLng("t1 = GetHighResolutionTimer() -> %u", t1 = GetHighResolutionTimer());
	dbgLng("t1 - t0 == %u", t = t1 - t0);
}

// --------------------- Convert to DOS Time Format -------------------------------------

static void convertToDOStime() {
	struct timeval time; struct timezone tz = {0,0}; struct tm dateTime;
	struct _DOSDate fileDate; struct _DOSTime fileTime; WORD wDate,wTime;
	int year,month,day,hour,minute,bisecond;

	gettimeofday(&time,&tz); gmtime_r(&time.tv_sec,&dateTime);
	
	dbgNil("_ConvertTimeToDOS(time.tv_sec,&fileDate,&fileTime)");
	_ConvertTimeToDOS(time.tv_sec,&fileDate,&fileTime);

	wDate = fileDate.packed_date; wTime = fileTime.packed_time;
	
	dbgInt("year     = GET_YEAR_FROM_DATE(    wDate) -> %i (since 1980)",
			 year      = GET_YEAR_FROM_DATE(    wDate));
	dbgInt("month    = GET_MONTH_FROM_DATE(   wDate) -> %i",
			 month     = GET_MONTH_FROM_DATE(   wDate));
	dbgInt("day      = GET_DAY_FROM_DATE(     wDate) -> %i",
			 day       = GET_DAY_FROM_DATE(     wDate));

	dbgInt("hour     = GET_HOUR_FROM_TIME(    wTime) -> %i",
			 hour      = GET_HOUR_FROM_TIME(    wTime));
	dbgInt("minute   = GET_MINUTE_FROM_TIME(  wTime) -> %i",
			 minute    = GET_MINUTE_FROM_TIME(  wTime));
	dbgInt("bisecond = GET_BISECOND_FROM_TIME(wTime) -> %i",
			 bisecond  = GET_BISECOND_FROM_TIME(wTime));

	wfInt("convertToDOStime","GET_YEAR_FROM_DATE()",    dateTime.tm_year - 80,year);
	wfInt("convertToDOStime","GET_MONTH_FROM_DATE()",   dateTime.tm_mon + 1,  month);	
	wfInt("convertToDOStime","GET_DAY_FROM_DATE()",     dateTime.tm_mday,     day);	

	wfInt("convertToDOStime","GET_HOUR_FROM_TIME()",    dateTime.tm_hour,     hour);	
	wfInt("convertToDOStime","GET_MINUTE_FROM_TIME()",  dateTime.tm_min,      minute);
	wfInt("convertToDOStime","GET_BISECOND_FROM_TIME()",dateTime.tm_sec / 2,  bisecond);
}

// --------------------- CPU Utilization ------------------------------------------------

static void cpuUtilization() {
	dbgInt("GetCPUUtilization() -> %i%%",GetCPUUtilization());
	dbgAwait(1000);
	dbgInt("GetCPUUtilization() -> %i%%",GetCPUUtilization());
	dbgAwait(1000);
	dbgInt("GetCPUUtilization() -> %i%%",GetCPUUtilization());
}

// --------------------- main -----------------------------------------------------------

int main(int argc,char* argv[]) {
	if (dbgInit(argc,argv,"Time")) {
		dbgNL("processClockTicks()"); processClockTicks();
		dbgNL("timer()");             timer();
		dbgNL("convertToDOStime()");  convertToDOStime();
		dbgNL("cpuUtilization()");    cpuUtilization();
	}
	return dbgFinal();
}
