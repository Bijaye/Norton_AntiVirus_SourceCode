/* Copyright 1993,1994 Symantec Corporation                                  */
/************************************************************************
 *                                                                      *
 * $Header:   S:/NAVSERVE/VCS/symtime.h_v   1.0   18 Apr 1997 14:01:26   DSACKIN  $
 *                                                                      *
 * Description:                                                         *
 *      Time structures, like that found in TIME.H (C library), but     *
 *      size-independent (we put them in files).                        *
 *                                                                      *
 * See Also:                                                            *
 *                                                                      *
 ************************************************************************
 * $Log:   S:/NAVSERVE/VCS/SYMTIME.H_v  
// 
//    Rev 1.2   30 Apr 1996 15:41:10   dsackin
// closed the comments properly.
// 
//    Rev 1.1   30 Apr 1996 15:38:06   dsackin
// Remove dubious extra character to avoid Compilation errors
// 
//    Rev 1.0   30 Jan 1996 15:54:22   BARRY
// Initial revision.
// 
//    Rev 1.0   30 Jan 1996 14:40:00   BARRY
// Initial revision.
// 
// *********************************************************************
*/

#ifndef _SYMTIME_H
#define _SYMTIME_H


#include <time.h>                       // time_t

//**********************************************************************
//
//  defines
//
//**********************************************************************

// timeout definitions

#define NO_TIMEOUT              0       // 0 is no timeout

// TimeCompare result defines

#define TIME_EQUAL      0
#define TIME_AFTER      1
#define TIME_BEFORE     2

// Days of the week defines

#define SUNDAY          0
#define MONDAY          1
#define TUESDAY         2
#define WEDNESDAY       3
#define THURSDAY        4
#define FRIDAY          5
#define SATURDAY        6

// AddTimetoWTime unit definitions

#define ADD_MINUTES     0
#define ADD_HOURS       1
#define ADD_DAYS        2
#define ADD_WEEKS       3
#define ADD_MONTHS      4
#define ADD_YEARS       5


//**********************************************************************
//
//  typedefs
//
//**********************************************************************

// structure for managing windows timeouts

typedef struct tagTIMEOUT               // complete declaration
    {

    DWORD   dwStructSize;               // size of basic structure

    DWORD   dwInitialTicks;             // time at time out initialization
    DWORD   dwExpireTicks;              // time when timer expires
    BOOL    bExpireWrapped;             // flags timer crossed maximum time
    BOOL    bNoTimeout;                 // flags no timeout

    } TIMEOUT;

typedef TIMEOUT FAR *LPTIMEOUT;

typedef DWORD TIMEOUT_INTERVAL;


//***********************************************************************
// platform independent time structure - used for disk file structures
//***********************************************************************

typedef struct tagWTIME {
   WORD  tm_sec;                        // seconds after the minute -- [0,61]
   WORD  tm_min;                        // minutes after the hour   -- [0,59]
   WORD  tm_hour;                       // hours after midnight     -- [0,23]
   WORD  tm_mday;                       // day of the month         -- [1,31]
   WORD  tm_mon;                        // months since January     -- [0,11]
   WORD  tm_year;                       // years since 1900                  
   WORD  tm_wday;                       // days since Sunday        -- [0,6] 
   WORD  tm_yday;                       // days since January 1     -- [0,365]
   WORD  tm_isdst;                      // Daylight Savings Time flag
} WTIME, FAR *LPWTIME;

// macros to move time from struct tm to struct WTIME, or vice versa
// handles the conversion to WORD or int transparently - this insulates
// the user from the details of the implementation of the time structs.
// We can't use MEMCPY because the elements of the two structs may not
// be the same size (WORD/int conflict)

// copies one structure to another

#define TMtoTM(ToTime,FromTime)  \
	    (ToTime).tm_sec  = (FromTime).tm_sec; \
	    (ToTime).tm_min  = (FromTime).tm_min; \
	    (ToTime).tm_hour = (FromTime).tm_hour; \
	    (ToTime).tm_mday = (FromTime).tm_mday; \
	    (ToTime).tm_mon  = (FromTime).tm_mon; \
	    (ToTime).tm_year = (FromTime).tm_year; \
	    (ToTime).tm_wday = (FromTime).tm_wday; \
	    (ToTime).tm_yday = (FromTime).tm_yday; \
	    (ToTime).tm_isdst= (FromTime).tm_isdst      // user supplies last ;

// copies one structure to another via pointers

#define pTMtopTM(ToTime,FromTime)  \
	    (ToTime)->tm_sec  = (FromTime)->tm_sec; \
	    (ToTime)->tm_min  = (FromTime)->tm_min; \
	    (ToTime)->tm_hour = (FromTime)->tm_hour; \
	    (ToTime)->tm_mday = (FromTime)->tm_mday; \
	    (ToTime)->tm_mon  = (FromTime)->tm_mon; \
	    (ToTime)->tm_year = (FromTime)->tm_year; \
	    (ToTime)->tm_wday = (FromTime)->tm_wday; \
	    (ToTime)->tm_yday = (FromTime)->tm_yday; \
	    (ToTime)->tm_isdst= (FromTime)->tm_isdst    // user supplies last ;

//**********************************************************************
//
//  Prototypes
//
//**********************************************************************

#ifdef __cplusplus
extern "C" {
#endif


VOID GetLocalWTime( LPWTIME lpwTime );  // returns local time as WTIME
                                        // struct

time_t GetTimeFromWTime( LPWTIME lpwTime ); // convert WTIME to time_t
VOID GetWTimeFromTime( LPWTIME lpwTime, time_t Time );

SDWORD DiffWTime( LPWTIME lpwTime1, LPWTIME lpwTime2 );

VOID SYM_EXPORT WINAPI InitializeTimeout( LPTIMEOUT lpTimeout, DWORD dwTimeout );

BOOL SYM_EXPORT WINAPI TimedOut( LPTIMEOUT lpTimeout );

VOID SYM_EXPORT WINAPI AddTimeToWTime( LPWTIME lpTime,
                                   WORD wTimeUnits,
                                   SDWORD nUnitCount);

WORD SYM_EXPORT WINAPI NextDay( WORD wDay );

WORD SYM_EXPORT WINAPI TimeCompare( LPWTIME lpTime1, LPWTIME lpTime2 );

DWORD SYM_EXPORT WINAPI PCTicksToMilliseconds( DWORD dwTicks );
DWORD SYM_EXPORT WINAPI MillisecondsToPCTicks( DWORD dwMilliseconds );

DWORD SYM_EXPORT WINAPI GetFineLinearTime( VOID );

DWORD GetHighResolutionTimer( VOID );

#ifdef __cplusplus
}
#endif
         

#endif // _SYMTIME_H

