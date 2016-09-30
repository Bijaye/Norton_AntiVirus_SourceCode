// Copyright 1996 Symantec Corporation
//**********************************************************************
//
// $Header:   S:/include/VCS/ntsrvutl.h_v   1.0   06 Feb 1997 20:52:02   RFULLER  $
//
// Description:
//
//**********************************************************************
//
// $Log:   S:/include/VCS/ntsrvutl.h_v  $
// 
//    Rev 1.0   06 Feb 1997 20:52:02   RFULLER
// Initial revision
// 
//    Rev 1.1   10 May 1996 19:05:44   TSmith
// Added flag to prevent access to uninitialized
// load management structures.
//
//    Rev 1.0   16 Apr 1996 19:26:18   TSmith
// Initial revision.
//
//**********************************************************************
#if !defined( _NTSRVUTL_H )
#define _NTSRVUTL_H

#if !defined( SYM_WIN32 )
#error Invalid platform
#endif

#include <platform.h>

#define ERROR_NOT_FOUND             0x80020000L
#define ERROR_MAX_RETRIES_REACHED   0x80020001L
#define ERROR_INVALID_PERF_DATA     0x80020002L
#define ERROR_LOADMGR_NOT_READY     0x80020003L

#define TICKS_PER_SECOND            18
#define DEFAULT_BUFFER_SIZE         16384
#define DEFAULT_INCREMENT_SIZE      4096
#define MAX_REALLOC_TRIES           10

#define COUNTER_NAMES_KEY _T( "SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion\\Perflib\\009" )
//
// Please don't remove the trailing space in "Counter ".
// This is really how it is in the Windows NT registry!
//
#define COUNTER_NAMES_DATA _T( "Counter " )
#define COUNTER_OBJECT_NAME _T( "Processor" )
#define COUNTER_NAME _T( "% Processor Time" )

VOID DelayThreadTicks(
	IN DWORD dwTicks
);

VOID RelinquishControl(
	VOID
);

BOOL InitPerfCounter(
	IN LPTSTR lptstrObjectName,
	IN LPTSTR lptstrCounterName
);

BOOL GetCpuLoad(
	OUT LPDWORD dwCPULoad
);


VOID ClosePerfCounter(
	VOID
);


#endif	// _NTSRVUTL_H