// Copyright 1996 Symantec Corporation
//**********************************************************************
//
// $Header:   S:/LOADMGR/VCS/ntsrvutl.cpv   1.1   18 Apr 1997 13:54:24   DSACKIN  $
//
// Description: This module provides the functionality needed to access
//				Windows NT performance counters for CPU load.
//
//				There are three exported functions for this purpose:
//
//					'InitPerfCounter' - Allocates storage, initializes
//					pointers to key data & takes initial measurements.
//
//					'GetCpuLoad' - Refreshes counter data & returns CPU
//					busy percentage to caller.
//
//					'ClosePerfCounter' - Frees memory and sets global
//					pointers to NULL.
//
//				These functions should be called in this order with
//				'GetCpuLoad' being called as many times as desired.
//
//				NOTE: This module currently only supports single CPU
//				systems. However, the data for all CPUs on the system
//				IS present and code to iterate through all performance
//				instances and calculate the average load (or whatever)
//				is all that needs to be added to support multi
//				processors.
//
//**********************************************************************
//
// $Log:   S:/LOADMGR/VCS/ntsrvutl.cpv  $
// 
//    Rev 1.1   18 Apr 1997 13:54:24   DSACKIN
// Ported from Tesla
// 
//    Rev 1.0   13 Mar 1997 15:11:40   RFULLER
// Initial revision
// 
//    Rev 1.0   08 Nov 1996 11:52:44   DKEEFER
// Initial revision
// 
//    Rev 1.2   10 May 1996 19:04:54   TSmith
// Added flag to prevent access to uninitialized
// load management structures.
//
//    Rev 1.1   24 Apr 1996 18:41:36   TSmith
// Corrected a misplaced pack pragma.
//
//    Rev 1.0   16 Apr 1996 19:22:06   TSmith
// Initial revision.
//
//**********************************************************************

//
// Set struct packing value to 8 for WinNT
// then back to 1 for Norton
//
#pragma pack( push, 8 )
#include <windows.h>
#pragma pack( pop )

#include <platform.h>
#include <xapi.h>
#include "ntsrvutl.h"

#define TRACE ( ERR_TRACE_ID( NTSRVUTL_CPP ) )

//
// Packing value to 8 again...
//
#pragma pack( push, 8 )

//**********************************************************************
//
//  local data
//
//**********************************************************************
static LONGLONG liPrevTime;
static LONGLONG liCurTime;
static LONGLONG liCurCpuLoad;
static LONGLONG liPrevCpuLoad;

static PPERF_DATA_BLOCK          pPerformanceData;
static PPERF_OBJECT_TYPE         pPerfObjectType;
static PPERF_COUNTER_DEFINITION  pPerfCounterDef;
static PPERF_INSTANCE_DEFINITION pPerfInstanceDef;

static TCHAR szObjectIndex[ 4 ];
static TCHAR szCounterIndex[ 4 ];

static DWORD dwCounterIndex;
static DWORD dwPerformanceDataSize;
static BOOL  fLoadMgrReady = FALSE;



//**********************************************************************
//
//  local prototypes
//
//**********************************************************************
BOOL GetObjectTitleIndex(
	IN	LPTSTR lptstrCounterName,
	OUT	LPTSTR lptstrCounterNameIndex
);

BOOL UpdatePerformanceData(
	IN LPBYTE lpbPerfDataBuf,
	IN LPTSTR lptstrIndex
);

BOOL HasValidPDBSig(
	IN PPERF_DATA_BLOCK pPDB
);

PPERF_OBJECT_TYPE GetFirstPerfObject(
	IN PPERF_DATA_BLOCK pPerfDataBlock
);

PPERF_COUNTER_DEFINITION GetCounterDefByIndex(
	IN PPERF_OBJECT_TYPE pPerfObject,
	IN DWORD dwIndex
);

PPERF_COUNTER_DEFINITION GetNextCounterDef(
	IN PPERF_COUNTER_DEFINITION pPerfCountDef
);

PPERF_INSTANCE_DEFINITION GetFirstInstanceDef(
	IN PPERF_OBJECT_TYPE pPerfObject
);

PPERF_COUNTER_BLOCK GetCounterBlock(
	IN PPERF_INSTANCE_DEFINITION pInstanceDef
);

LONGLONG GetCpuCounterValue(
	IN PPERF_COUNTER_DEFINITION pCounterDef,
	IN PPERF_INSTANCE_DEFINITION pInstanceDef
);

LONGLONG GetCounterTime(
	IN PPERF_DATA_BLOCK pPefDataBlock
);

FLOAT GetLoadPercentage(
    IN PLONGLONG pliCurrentTime,
    IN PLONGLONG pliPreviousTime,
	IN PLONGLONG pliCounterValue,
	IN PLONGLONG pliOldCounterValue
);


//**********************************************************************
//
//  Description: Converts a 'tick' value into number of milliseconds
//				 calling Win32 Sleep.
//
//				 Provides a Win32 specific version of this function from
//				 the same function in NAVNLM code.
//
//   Parameters: Number of clock ticks to wait.
//
//**********************************************************************
//
// 04/15/96 TSMITH	Function created.
//
//**********************************************************************
VOID DelayThreadTicks( IN DWORD dwTicks )
	{
	DWORD dwMilliseconds;

	dwMilliseconds = 1000 * dwTicks;
	dwMilliseconds = dwMilliseconds / TICKS_PER_SECOND +
		( ( dwMilliseconds % TICKS_PER_SECOND ) > 0 ? 1 : 0 );

	Sleep( dwMilliseconds );

	} //



//**********************************************************************
//
//  Description: Immediately relinquishes the remainder of the calling
//				 threads time slice.
//
//				 Provides a Win32 specific version of this function from
//				 the same function in NAVNLM code.
//
//**********************************************************************
//
// 04/15/96 TSMITH	Function created.
//
//**********************************************************************
VOID RelinquishControl( VOID )
	{

	Sleep( 0 );

	} // RelinquishControl



//**********************************************************************
//
//  Description: Returns the index string associated with the object
//				 name of a Windows NT performance counter.
//
//	 Parameters: Pointer to the name of a Windows NT perfromance counter
//				 to retrieve the index string for and a pointer to a
//				 buffer to recieve the index string.
//
//		Returns: TRUE if seccessful, FALSE on error. Call GetLastError()
//				 to retrieve specific error code.
//
//**********************************************************************
//
// 04/15/96 TSMITH	Function created.
//
//**********************************************************************
BOOL GetObjectTitleIndex(
	IN	LPTSTR lptstrCounterName,
	OUT	LPTSTR lptstrCounterNameIndex
)
	{
	LONG	lResult;
	BOOL	fExitCode			= TRUE;
	HKEY	hKeyNames			= INVALID_HANDLE_VALUE;
	DWORD	dwBufSize			= DEFAULT_BUFFER_SIZE;
	DWORD	dwErrorCode			= ERROR_SUCCESS;
	LPTSTR	lptstrIndex			= NULL;
	LPTSTR	lpszzKeyValues		= NULL;
	LPTSTR	lptstrTitleName		= NULL;

	SYM_ASSERT( lptstrCounterName != NULL );
	SYM_ASSERT( lptstrCounterNameIndex != NULL );

	__try
		{
		//
		// Try to open registry key for performance
		// counter names and indexes
		//
		lResult = RegOpenKeyEx( HKEY_LOCAL_MACHINE,
								COUNTER_NAMES_KEY,
								0,
								KEY_QUERY_VALUE,
								&hKeyNames );

		//
		// Give up if we can't open the key
		//
		if ( lResult != ERROR_SUCCESS )
			{
			dwErrorCode = lResult;
			fExitCode = FALSE;
			__leave;
			}

		//
		// Attempt to allocate some memory, give up if allocation fails
		//
		lpszzKeyValues = ( LPTSTR )MemAllocPtr( GHND, dwBufSize );

		if ( lpszzKeyValues == NULL )
			{
			dwErrorCode = ERROR_NOT_ENOUGH_MEMORY;
			fExitCode = FALSE;
			__leave;
			}

		//
		// Try to read the data block from the registry
		//
		lResult = RegQueryValueEx( hKeyNames,
								   COUNTER_NAMES_DATA,
								   NULL,
								   NULL,
								   ( LPBYTE )lpszzKeyValues,
								   &dwBufSize );

		//
		// ERROR_MORE_DATA indicates that our buffer was too small.
		// The required size is returned in dwBufSize.
		//
		if ( lResult == ERROR_MORE_DATA )
			{
			//
			// Try to get more memory using buffer size from
			// RegQueryValueEx, exit if we don't get it
			//
			lpszzKeyValues = ( LPTSTR )MemReAllocPtr( lpszzKeyValues, dwBufSize, GHND );

			if ( lpszzKeyValues == NULL )
				{
				dwErrorCode = ERROR_NOT_ENOUGH_MEMORY;
				fExitCode = FALSE;
				__leave;
				}

			//
			// Now that our buffer size problems are over,
			// try to read the registry values again
			//
			lResult = RegQueryValueEx( hKeyNames,
							   		   COUNTER_NAMES_DATA,
							   		   NULL,
							   		   NULL,
							   		   ( LPBYTE )lpszzKeyValues,
							   		   &dwBufSize );

			//
			// If we still can't read the registry something else is
			// wrong so reflect the error out to the caller and exit
			//
			if ( lResult != ERROR_SUCCESS )
				{
				dwErrorCode = lResult;
				fExitCode = FALSE;
				__leave;
				}

			} // if ERROR_MORE_DATA

		//
		// The following code searches through the szz buffer for the counter name we're
		// interested in. The counter name will be preceded by an 'index' string which is
		// what we really want. For this reason, we save a pointer to the last non
		// matching string so that when the name string is found, we'll already have a
		// pointer to the index string which we can then copy to the callers buffer.
		//

		//
		// Point at first sz string in szz buffer
		//
		lptstrTitleName = lpszzKeyValues;

		//
		// While still a valid string but not the one we want
		//
		while ( *lptstrTitleName && STRCMP( lptstrTitleName, lptstrCounterName ) != 0 )
			{
			//
			// Save pointer to 'previous' string & move to next
			//
			lptstrIndex = lptstrTitleName;
			lptstrTitleName = SzzFindNext( lptstrTitleName );
			}

		//
		// If lptstrTitleName doesn't point to a valid
		// string, the counter name was not found
		//
		if ( ! *lptstrTitleName )
			{
			dwErrorCode = ERROR_NOT_FOUND;
			fExitCode = FALSE;
			__leave;
			}

		//
		// Otherwise the name was found so copy the index to
		// to the callers buffer. If the copy fails assume it's
		// because the buffer passed to us was too small
		//
		if ( STRCPY( lptstrCounterNameIndex, lptstrIndex ) == NULL )
			{
			dwErrorCode = ERROR_INSUFFICIENT_BUFFER;
			fExitCode = FALSE;
			__leave;
			}

		} // __try

	__finally
		{
		if ( hKeyNames != INVALID_HANDLE_VALUE )
			RegCloseKey( hKeyNames );

		if ( lpszzKeyValues != NULL )
			MemFreePtr( lpszzKeyValues );

		SetLastError( dwErrorCode );
		return fExitCode;
		}

	} // GetObjectTitleIndex



//**********************************************************************
//
//  Description: Tests the first four bytes of the buffer passed to it
//				 for the presence of the unicode string "PERF". This
//				 string is the signature for a _PERF_DATA_BLOCK.
//
//	 Parameters: Pointer to a _PERF_DATA_BLOCK.
//
//		Returns: TRUE if the buffer contains the signature else FALSE.
//
//**********************************************************************
//
// 04/15/96 TSMITH	Function created.
//
//**********************************************************************
BOOL HasValidPDBSig( IN	PPERF_DATA_BLOCK pPDB )
	{
	SYM_ASSERT( pPDB != NULL );

	return ( pPDB->Signature[0] == (WCHAR)'P' &&
			 pPDB->Signature[1] == (WCHAR)'E' &&
			 pPDB->Signature[2] == (WCHAR)'R' &&
			 pPDB->Signature[3] == (WCHAR)'F' );

	} // HasValidPDBSig



//**********************************************************************
//
//  Description: Sets up necessary buffers, pointers, etc. needed to
//				 retrieve CPU load performance data from Windows NT.
//				 Takes initial time and load measurement for subsequent
//				 'percent busy' calculations.
//
//	 Parameters: Pointer to the name of a Windows NT performance object
//				 and the name of a specific counter.
//
//		Returns: TRUE if initialization successful, FALSE otherwise.
//				 Use 'GetLastError' to retrieve error code.
//
//**********************************************************************
//
// 04/15/96 TSMITH	Function created.
//
//**********************************************************************
BOOL InitPerfCounter( IN LPTSTR lptstrObjectName, IN LPTSTR lptstrCounterName )
	{
	LONG	lResult;
	BOOL	fExitCode	= TRUE;
	INT		nMaxTries	= MAX_REALLOC_TRIES;
	HKEY	hKeyNames	= INVALID_HANDLE_VALUE;
	DWORD	dwBufSize	= DEFAULT_BUFFER_SIZE;
	DWORD	dwErrorCode = ERROR_SUCCESS;

	SYM_ASSERT( lptstrObjectName != NULL );
	SYM_ASSERT( lptstrCounterName != NULL );

	//
	// Save the buffer size for later reference.
	// We need to save the value because RegQueryValueEx
	// alters the contents of dwBufSize.
	//
	dwPerformanceDataSize = dwBufSize;

	__try
		{
		if ( ! GetObjectTitleIndex( lptstrObjectName, szObjectIndex ) )
			{
			dwErrorCode = GetLastError();
			fExitCode = FALSE;
			__leave;
			}

		//
		// Attempt to allocate some memory, quit if allocation fails
		//
		pPerformanceData = ( PPERF_DATA_BLOCK )MemAllocPtr( GHND, dwBufSize );

		if ( pPerformanceData == NULL )
			{
			dwErrorCode = ERROR_NOT_ENOUGH_MEMORY;
			fExitCode = FALSE;
			__leave;
			}

		//
		// Retrieve performance data objects
		//
		lResult = RegQueryValueEx( HKEY_PERFORMANCE_DATA,
								   szObjectIndex,
								   NULL,
								   NULL,
								   ( LPBYTE )pPerformanceData,
								   &dwBufSize );

		//
		// ERROR_MORE_DATA indicates the pPerfDataBlock buffer was too small.
		// RegQueryValueEx does NOT return the required buffers size when
		// called with HKEY_PERFORMANCE_DATA. The following code recalls
		// RegQueryValueEx with increasingly large buffer as long as we
		// keep getting ERROR_MORE_DATA. As a safety valve, the loop will
		// stop trying after MAX_REALLOC_TRIES.
		//

		while ( lResult == ERROR_MORE_DATA && nMaxTries-- > 0 )
			{
			//
			// Increment the buffer size & save globally
			//
			dwBufSize += DEFAULT_INCREMENT_SIZE;
			dwPerformanceDataSize = dwBufSize;

			pPerformanceData = ( PPERF_DATA_BLOCK )MemReAllocPtr( pPerformanceData,
																  dwBufSize,
																  GHND );

			if ( pPerformanceData == NULL )
				{
				dwErrorCode = ERROR_NOT_ENOUGH_MEMORY;
				fExitCode = FALSE;
				__leave;
				}

			lResult = RegQueryValueEx( HKEY_PERFORMANCE_DATA,
									   szObjectIndex,
									   NULL,
									   NULL,
									   ( LPBYTE )pPerformanceData,
									   &dwBufSize );
			} // while


		//
		// Set exit/error codes and give up if still getting
		// ERROR_MORE_DATA after MAX_REALLOC_TRIES. This is
		// mostly a sanity check to keep the number of loop
		// iterations from going through the roof.
		//
		if ( nMaxTries == 0 && lResult != ERROR_SUCCESS )
			{
			dwErrorCode = ERROR_MAX_RETRIES_REACHED;
			fExitCode = FALSE;
			__leave;
			}

		if ( ! HasValidPDBSig( pPerformanceData ) )
			{
			dwErrorCode = ERROR_INVALID_PERF_DATA;
			fExitCode = FALSE;
			__leave;
			}

		SYM_ASSERT( pPerformanceData->NumObjectTypes == 1 );

		//
		// Get first (and should be only) performance object
		//
		pPerfObjectType	= GetFirstPerfObject( pPerformanceData );

		//
		// Get the index string of the specific counter
		//
		if ( ! GetObjectTitleIndex( lptstrCounterName, szCounterIndex ) )
			{
			dwErrorCode = GetLastError();
			fExitCode = FALSE;
			__leave;
			}

		//
		// Convert index string to number and use to get
		// the desired performance counter such as
		// "% Processor Time"
		//
		dwCounterIndex = atoi( szCounterIndex );
		pPerfCounterDef	= GetCounterDefByIndex( pPerfObjectType,
												dwCounterIndex );

		SYM_ASSERT( pPerfCounterDef->CounterType == PERF_100NSEC_TIMER_INV );

		pPerfInstanceDef = GetFirstInstanceDef( pPerfObjectType );

		//
		// Get initial CPU load value
		//
		liPrevCpuLoad = GetCpuCounterValue( pPerfCounterDef,
                                            pPerfInstanceDef );

		//
		// Get initial time value
		//
		liPrevTime = GetCounterTime( pPerformanceData );

		} // __try
    __finally
        {
		//
		// If the __finally block was entered because of an error
		// condition, free any allocated memory
		//
        if ( ( dwErrorCode != ERROR_SUCCESS ) && ( pPerformanceData != NULL ) )
            {
            MemFreePtr( pPerformanceData );
            pPerformanceData = NULL;
            }

		SetLastError( dwErrorCode );
        fLoadMgrReady = fExitCode;
		return fExitCode;
		}

	} // InitPerfCounter


//**********************************************************************
//
//  Description: Reports the system load expressed as a percentage of
//				 CPU 'busy' time. Refreshes performance data, makes
//				 calculations and sets up for next measurement.
//
//				 'InitPerfCounter' must be called first to open NT
//				 performance counters.
//
//	 Parameters: Pointer to a DWORD that will receive server load.
//
//		Returns: TRUE if successful, otherwise FALSE. Use 'GetLastError'
//				 to retrieve error code.
//
//**********************************************************************
//
// 04/15/96 TSMITH	Function created.
//
//**********************************************************************
BOOL GetCpuLoad( OUT LPDWORD dwCPULoad )
	{
	LONG lResult;
	FLOAT PercentBusy;

    SYM_ASSERT( dwCPULoad != NULL );
    SYM_ASSERT( fLoadMgrReady );

    if ( ! fLoadMgrReady )
        {
        SetLastError( ERROR_LOADMGR_NOT_READY );
        return FALSE;
        }

	UpdatePerformanceData( ( LPBYTE )pPerformanceData, szObjectIndex );
	lResult = GetLastError();

	if ( lResult != ERROR_SUCCESS )
		{
		SetLastError( lResult );
		return FALSE;
		}

	//
	// Get current time and load measurements
	//
	liCurTime = GetCounterTime( pPerformanceData );

	liCurCpuLoad = GetCpuCounterValue( pPerfCounterDef,
									 pPerfInstanceDef );

	//
	// Calculate the server load
	//
	PercentBusy = GetLoadPercentage( &liCurTime,
									 &liPrevTime,
									 &liCurCpuLoad,
									 &liPrevCpuLoad );
	//
	// Save current measurements for next time
	//
	liPrevTime = liCurTime;
	liPrevCpuLoad = liCurCpuLoad;

	//
	// Copy the CPU load measurement to
	// callers buffer and return
	//
    *dwCPULoad = ( DWORD )PercentBusy;
	return TRUE;

	} // GetCpuLoad


//**********************************************************************
//
//  Description: Returns a pointer to a _PERF_OBJECT_TYPE structure
//				 within a _PERF_DATA_BLOCK. Performs no validation on
//				 the _PERF_DATA_BLOCK structure passed to it. Depends
//				 on the documented structures of Windows NT performance
//				 data & will need to be modified if that structure
//				 ever changes.
//
//	 Parameters: Pointer to a _PERF_DATA_BLOCK.
//
//		Returns: Pointer to a _PERF_OBJECT_TYPE structure.
//
//**********************************************************************
//
// 04/15/96 TSMITH	Function created.
//
//**********************************************************************
PPERF_OBJECT_TYPE GetFirstPerfObject( IN PPERF_DATA_BLOCK pPerfDataBlock )
	{
	SYM_ASSERT( pPerfDataBlock != NULL );

	return ( PPERF_OBJECT_TYPE )( ( LPBYTE )pPerfDataBlock +
		pPerfDataBlock->HeaderLength );

	} // GetFirstPerfObject


//**********************************************************************
//
//  Description: Searches for a specified _PERF_COUNTER_DEFINITION in
//				 a _PERF_OBJECT_TYPE. The index of a specific counter
//				 must be retrieved from the registry, converted to a
//				 number and passed to this function.
//
//	 Parameters: Pointer to a _PERF_OBJECT_TYPE buffer and an index
//				 number used to search for a specific
//				 _PERF_COUNTER_DEFINITION.
//
//		Returns: Pointer to a _PERF_COUNTER_DEFINITON whose index number
//				 matches that passed to the function.
//
//**********************************************************************
//
// 04/15/96 TSMITH	Function created.
//
//**********************************************************************
PPERF_COUNTER_DEFINITION GetCounterDefByIndex( IN PPERF_OBJECT_TYPE pPerfObject,
											   IN DWORD dwIndex )
	{
	SYM_ASSERT( pPerfObject != NULL );

	PPERF_COUNTER_DEFINITION pPCD;

	pPCD = ( PPERF_COUNTER_DEFINITION )( ( LPBYTE )pPerfObject +
		pPerfObject->HeaderLength );

	while ( pPCD->CounterNameTitleIndex != dwIndex )
		pPCD = GetNextCounterDef( pPCD );

	return pPCD;

	} // GetCounterDefByIndex



//**********************************************************************
//
//  Description: Returns a pointer to the _PERF_COUNTER_DEFINITION
//				 after the one passed to it.
//
//				 No validation is performed & no end of list marker
//				 exists in the performance data structures. Use
//				 '_PERF_OBJECT_TYPE.NumCounters' to walk list of
//				 counters.
//
//	 Parameters: Pointer to a _PERF_COUNTER_DEFINITION
//
//**********************************************************************
//
// 04/15/96 TSMITH	Function created.
//
//**********************************************************************
PPERF_COUNTER_DEFINITION GetNextCounterDef( IN PPERF_COUNTER_DEFINITION pPerfCountDef )
	{
	SYM_ASSERT( pPerfCounterDef != NULL );

	return ( PPERF_COUNTER_DEFINITION )( ( LPBYTE )pPerfCountDef +
		pPerfCountDef->ByteLength );

	} // GetNextCounterDef



//**********************************************************************
//
//  Description: Returns a pointer to the first _PERF_INSTANCE_DEFINITION
//				 in a _PERF_OBJECT_TYPE structure.
//
//	 Parameters: Pointer to a _PERF_OBJECT_TYPE structure.
//
//**********************************************************************
//
// 04/15/96 TSMITH	Function created.
//
//**********************************************************************
PPERF_INSTANCE_DEFINITION GetFirstInstanceDef( IN PPERF_OBJECT_TYPE pPerfObject )
	{
	PPERF_INSTANCE_DEFINITION pPID;

	SYM_ASSERT( pPerfCounterDef != NULL );

	pPID = ( PPERF_INSTANCE_DEFINITION )( ( LPBYTE )pPerfObject +
		pPerfObject->DefinitionLength );

	return pPID;

	} // GetFirstInstanceDef



//**********************************************************************
//
//  Description: Returns a pointer to _PERF_COUNTER_BLOCK of a
//				 _PERF_INSTANCE_DEFINITION.
//
//	 Parameters: Pointer to a _PERF_INSTANCE_DEFINITION.
//
//**********************************************************************
//
// 04/15/96 TSMITH	Function created.
//
//**********************************************************************
PPERF_COUNTER_BLOCK GetCounterBlock( IN PPERF_INSTANCE_DEFINITION pInstanceDef )
	{
	SYM_ASSERT( pInstanceDef != NULL );

	return ( PPERF_COUNTER_BLOCK )( ( LPBYTE )pInstanceDef +
		pInstanceDef->ByteLength );

	} // GetCounterBlock



//**********************************************************************
//
//  Description: Returns the actual value of a specific Windows NT
//				 performance counter.
//
//	 Parameters: Pointer to a _PERF_COUNTER_DEFINITON and a
//				 _PERF_INSTANCE_DEFINITION.
//
//**********************************************************************
//
// 04/15/96 TSMITH	Function created.
//
//**********************************************************************
LONGLONG GetCpuCounterValue( IN PPERF_COUNTER_DEFINITION pCounterDef,
							 IN PPERF_INSTANCE_DEFINITION pInstanceDef )

	{
    PPERF_COUNTER_BLOCK pCtrBlock;
	LONGLONG liCounterValue;

	SYM_ASSERT( pCounterDef != NULL );
	SYM_ASSERT( pInstanceDef != NULL );

    pCtrBlock = GetCounterBlock( pInstanceDef );

    liCounterValue = *( PLONGLONG )( ( LPBYTE )pCtrBlock + pCounterDef->CounterOffset );
	return liCounterValue;

	} // GetCpuCounterValue



//**********************************************************************
//
//  Description: Closes NT performance data, releases memory allocated
//				 by 'InitPerfCounter' and sets global pointers to NULL.
//
//**********************************************************************
//
// 04/15/96 TSMITH	Function created.
//
//**********************************************************************
VOID ClosePerfCounter( VOID )
	{
	RegCloseKey( HKEY_PERFORMANCE_DATA );

	if ( pPerformanceData != NULL )
		MemFreePtr( pPerformanceData );

    pPerformanceData    = NULL;
    pPerfObjectType     = NULL;
	pPerfCounterDef		= NULL;
	pPerfInstanceDef	= NULL;
    fLoadMgrReady       = FALSE;

    } // ClosePerfCounter



//**********************************************************************
//
//  Description: Updates the performance data buffer and refreshes the
//				 global pointers to the various perfromance data structs.
//
//	 Parameters: Pointer to a valid _PERF_DATA_BLOCK buffer and a
//				 pointer to the index string of a performance object to
//				 refresh.
//
//		Returns: TRUE if update successful, otherwise false. Use
//				 'GetLastError' to retrieve error code.
//
//**********************************************************************
//
// 04/15/96 TSMITH	Function created.
//
//**********************************************************************
BOOL UpdatePerformanceData(
	IN	LPBYTE lpbPerfDataBuf,
	IN	LPTSTR lptstrIndex
)
	{
	LONG	lResult;
	BOOL	fExitCode = TRUE;
	DWORD	dwErrorCode = ERROR_SUCCESS;
	DWORD	dwBufSize = dwPerformanceDataSize;

	SYM_ASSERT( lpbPerfDataBuf != NULL );
	SYM_ASSERT( lptstrIndex != NULL );

	__try
		{
		//
		// Refresh performance data
		//
		lResult = RegQueryValueEx( HKEY_PERFORMANCE_DATA,
								   lptstrIndex,
								   NULL,
								   NULL,
								   lpbPerfDataBuf,
								   &dwBufSize );

		//
		// Check for success & validate buffer
		//
		if ( lResult != ERROR_SUCCESS )
			{
			dwErrorCode = lResult;
			fExitCode = FALSE;
			__leave;
			}

		if ( ! HasValidPDBSig( ( PPERF_DATA_BLOCK )lpbPerfDataBuf ) )
			{
			dwErrorCode = ERROR_INVALID_PERF_DATA;
			fExitCode = FALSE;
			__leave;
			}

		//
		// Refresh pointers to performance data structures
		//
		pPerfObjectType	= GetFirstPerfObject( pPerformanceData );

		pPerfCounterDef	= GetCounterDefByIndex( pPerfObjectType,
												dwCounterIndex );

		pPerfInstanceDef = GetFirstInstanceDef( pPerfObjectType );

		}
	__finally
		{
		SetLastError( dwErrorCode );
		return fExitCode;
		}

	} // UpdatePerformanceData



//**********************************************************************
//
//  Description: Returns the time of the current performance data from
//				 the 100 nanosecond timer.
//
//	 Parameters: Pointer to a valid _PERF_DATA_BLOCK.
//
//**********************************************************************
//
// 04/15/96 TSMITH	Function created.
//
//**********************************************************************
LONGLONG GetCounterTime( IN PPERF_DATA_BLOCK pPerfDataBlock )
	{
	LONGLONG nSecs;

	SYM_ASSERT( pPerfDataBlock != NULL );

	nSecs = pPerformanceData->PerfTime100nSec.QuadPart;

	return nSecs;

	} // GetCounterTime



//**********************************************************************
//
//  Description: Calculates the percentage of CPU time busy based on the
//				 following formula from the book 'Optimizing Windows NT'
//
//				 ( 1 - ( CountDelta / TimeDelta ) ) * 100
//
//				 Currently, only single CPU systems are supported by
//				 this function.
//
//	 Parameters: IN PLONGLONG pliCurrentTime
//					Pointer to time of current performance data.
//
//				 IN PLONGLONG pliPreviousTime
//					Pointer to time of last performance data.
//
//				 IN PLONGLONG pliCounterValue
//					Pointer to the current CPU load value.
//
//				 IN PLONGLONG pliOldCounterValue
//					Pointer to last CPU load value.
//
//		Returns: Floating point representation of CPU busy time.
//
//**********************************************************************
//
// 04/15/96 TSMITH	Function created.
//
//**********************************************************************
FLOAT GetLoadPercentage(
	IN PLONGLONG pliCurrentTime,
    IN PLONGLONG pliPreviousTime,
	IN PLONGLONG pliCounterValue,
	IN PLONGLONG pliOldCounterValue
)
	{
    FLOAT   eTimeInterval;
    FLOAT   eDifference;
    FLOAT   eFraction;
    FLOAT   eCount;

    LONGLONG   liTimeInterval;
    LONGLONG   liDifference;

	//
    // Get the amount of time that has passed since the last sample
	//
	liTimeInterval = *pliCurrentTime - *pliPreviousTime;
	eTimeInterval = ( FLOAT )( liTimeInterval );

    if ( eTimeInterval <= 0.0f )
       return ( FLOAT ) 0.0f;

    //
	// Get the current and previous counts.
	//
    liDifference = *pliCounterValue - *pliOldCounterValue;

    //
	// Get the number of counts in this time interval.
    // (1, 2, 3 or any number of seconds could have gone by since
    // the last sample)
	//
    eDifference = ( FLOAT )( liDifference );
	eFraction = eDifference;

	//
    // Calculate the fraction of time used by what were measuring.
	//
	eCount = eFraction / eTimeInterval;

	//
    // Take care of inversion.
	//
	eCount = ( FLOAT ) 1.0 - eCount;
    eCount *= 100.0f;

    if ( eCount < 0.0f )
		eCount = 0.0f ;

	if ( eCount > 100.0f )
		eCount = 100.f;

    return( eCount );

	} // GetLoadPercentage

#pragma pack( pop )