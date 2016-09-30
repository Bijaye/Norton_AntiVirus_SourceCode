#ifndef PERF_H
#define PERF_H


#include "platform.h"




// Defines ULARGE_INTEGER and LARGE_INTEGER,
// and the functions:
//
// Returns true if hardware supports counter, lpFrequency is set to counts per second
// BOOL QueryPerformanceFrequency(
//     LARGE_INTEGER *lpFrequency   // current frequency
// );
//
// Returns true if success, lpPerformanceCount is current performance count.
// BOOL QueryPerformanceCounter(
//     LARGE_INTEGER *lpPerformanceCount   // counter value
// );





typedef union _LARGE_INTEGER { 
  struct {
#ifdef SYM_LINUX_X86
    unsigned long LowPart; 
    unsigned long HighPart; 
#elif defined(BIG_ENDIAN)
    unsigned long HighPart; 
    unsigned long LowPart; 
#else
    unsigned long LowPart; 
    unsigned long HighPart; 
#endif
  } HighLow;
#ifndef SYM_NLM
  unsigned long long QuadPart;
#else
  // No performance monitoring for Netware, we never
  // use this variable, so just make it long.
  unsigned long QuadPart;
#endif
} LARGE_INTEGER; 






typedef LARGE_INTEGER ULARGE_INTEGER;

BOOL QueryPerformanceFrequency(LARGE_INTEGER *lpFrequency);

BOOL QueryPerformanceCounter(LARGE_INTEGER *lpPerformanceCount);


#endif // #ifndef PERF_H
