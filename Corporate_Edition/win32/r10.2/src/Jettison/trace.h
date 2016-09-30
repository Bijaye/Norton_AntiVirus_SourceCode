// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright (c) 2005 Symantec Corporation. All rights reserved.
 


/* 
	Trace.h
*/

#ifndef __TRACE_H__
#define __TRACE_H__

#include <crtdbg.h>
#include <stdarg.h>
#include <stdio.h>
#include <string.h>

#include "SymSaferStrings.h"

#ifdef _DEBUG
#define TRACEMAXSTRING	1024

static char szTraceBuffer[TRACEMAXSTRING];
inline void Trace(const char* format,...)
{
 va_list args;
 va_start(args,format);
 int nBuf;
 nBuf = ssvsnprintf(szTraceBuffer,
                   sizeof(szTraceBuffer),
                   format,
                   args);
 va_end(args);

 _RPT0(_CRT_WARN,szTraceBuffer);
}
#define TraceEx sssnprintf(szTraceBuffer,sizeof(szTraceBuffer),"%s(%d): ", \
                          &strrchr(__FILE__,'\\')[1],__LINE__); \
_RPT0(_CRT_WARN,szTraceBuffer); \
Trace

#else

#define Trace
#define TraceEx 

#endif


#endif // __TRACE_H__

