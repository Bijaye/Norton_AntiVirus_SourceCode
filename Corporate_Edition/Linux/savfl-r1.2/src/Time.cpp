// Copyright 2004-2005 Symantec Corporation. All rights reserved.
// Author: Bertil Askelid <Bertil_Askelid@Symantec.com>
// Intent: Make `timespec' for Linux implementation of NDK Time Handling functions

// ================== Time ==============================================================

extern "C" {
	#include <sys/time.h>
}

#include "Time.hpp"

// Creation of a `timespec' C structure from time in milliseconds to be used by time
// dependent Posix functions. A flag indicates when the time has to be in an absolute
// form, i.e. the current wallclock plus the `msec' interval. Note that absolute time is
// as if in the UTC timezone

void mkTime(unsigned long msec,struct timespec* pTime,bool absolute) {
	pTime->tv_sec	= msec / 1000;
	pTime->tv_nsec = (msec % 1000) * 1000 * 1000;
	if (absolute) {
		struct timeval now; struct timezone tz = {0,0};
		gettimeofday(&now,&tz); // time since the Epoch (00:00:00 UTC, January 1, 1970)

		pTime->tv_sec	+= now.tv_sec;
		pTime->tv_nsec += now.tv_usec * 1000;
	}
}
