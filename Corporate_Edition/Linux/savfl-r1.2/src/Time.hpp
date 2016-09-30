// Copyright 2004-2005 Symantec Corporation. All rights reserved.
// Author: Bertil Askelid <Bertil_Askelid@Symantec.com>
// Intent: Make `timespec' for Linux implementation of NDK Time Handling functions

#ifndef _Time_hpp_
#define _Time_hpp_

extern "C" {
	#include <time.h>
}

void mkTime(unsigned long msec,struct timespec* pTime,bool absolute = false);

#endif // _Time_hpp_
