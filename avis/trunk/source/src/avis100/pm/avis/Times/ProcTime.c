/*******************************************************************
*                                                                  *
* File name:     ProcTime.c                                        *
*                                                                  *
* Description:   provides a function to get the cpu time used      *
*                by a process                                      *
*                                                                  *
*                                                                  *
* Statement:     Licensed Materials - Property of IBM              *
*                (c) Copyright IBM Corp. 2000                      *
*                                                                  *
* Author:        Jean-Michel Boulay                                *
*                                                                  *
*                U.S. Government Users Restricted Rights - use,    *
*                duplication or disclosure restricted by GSA ADP   *
*                Schedule Contract with IBM Corp.                  *
*                                                                  *
*                                                                  *
*******************************************************************/



#include <stdio.h>
#include <windows.h>
#include <winbase.h>
#include <time.h>

#include "ProcTime.h"


static double
filetime_to_seconds(PFILETIME ft)
{
    double sec;
    __int64 qw = ft->dwHighDateTime;
    qw <<= 32;
    qw |= ft->dwLowDateTime;    
	sec = qw;
	sec = sec / 10000000;  /* a FILETIME is a multiple of 100ns */
    return sec;
}


int avis_win32_times(double * plutime, double * plstime, double * plcutime, double * plcstime)
{
    FILETIME user;
    FILETIME kernel;
    FILETIME dummy;
	clock_t t;
    if (GetProcessTimes(GetCurrentProcess(), &dummy, &dummy, 
                        &kernel,&user)) {
	*plutime = filetime_to_seconds(&user);
	*plstime = filetime_to_seconds(&kernel);
	*plcutime = 0;
	*plcstime = 0;
        
    } else { 
        /* That failed - e.g. Win95 fallback to clock() */
	fprintf(stderr, "GetProcessTimes failed: %d\n", GetLastError());
	t = clock() / CLOCKS_PER_SEC;
	*plutime = t;
	*plstime = 0;
	*plcutime = 0;
	*plcstime = 0;
    }
    return 0;
}
