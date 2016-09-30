// Copyright 2004-2005 Symantec Corporation. All rights reserved.
// Author: Bertil Askelid <Bertil_Askelid@Symantec.com>
// Intent: Linux implementation of NDK Time Handling functions

#ifndef _ndkTime_h_
#define _ndkTime_h_

#include <time.h>

#include "ndkDefinitions.h"

#ifdef __cplusplus
extern "C" {
#endif

LONG GetCurrentTime();
LONG GetCurrentTicks();
LONG GetHighResolutionTimer();

struct _DOSDate {WORD packed_date;};
struct _DOSTime {WORD packed_time;};

void _ConvertTimeToDOS(time_t calendarTime,
							  struct _DOSDate* fileDate,struct _DOSTime* fileTime);

int GetCPUUtilization();

// Definitions from //depot/Tools/NDK0403/nwsdk/include/nlm/nwtime.h

#ifndef GET_YEAR_FROM_DATE
#define GET_YEAR_FROM_DATE(    date) (((date) & 0xfe00) >> 9)
#endif
#ifndef GET_MONTH_FROM_DATE
#define GET_MONTH_FROM_DATE(   date) (((date) & 0x01e0) >> 5)
#endif
#ifndef GET_DAY_FROM_DATE
#define GET_DAY_FROM_DATE(     date)  ((date) & 0x001f)
#endif
										 
#ifndef GET_HOUR_FROM_TIME
#define GET_HOUR_FROM_TIME(    time) (((time) & 0xf800) >> 11)
#endif
#ifndef GET_MINUTE_FROM_TIME
#define GET_MINUTE_FROM_TIME(  time) (((time) & 0x07e0) >> 5)
#endif
#ifndef GET_BISECOND_FROM_TIME
#define GET_BISECOND_FROM_TIME(time)  ((time) & 0x001f)
#endif

#ifdef __cplusplus
}
#endif
	
#endif // _ndkTime_h_
