// Copyright 1996 Symantec Corporation
//**********************************************************************
//
// $Header:   S:/LOADMGR/VCS/loadmgmt.cpv   1.1   18 Apr 1997 13:54:28   DSACKIN  $
//
// Description: Server load management routines
//
//**********************************************************************
//
// $Log:   S:/LOADMGR/VCS/loadmgmt.cpv  $
// 
//    Rev 1.1   18 Apr 1997 13:54:28   DSACKIN
// Ported from Tesla
// 
//    Rev 1.0   13 Mar 1997 15:11:40   RFULLER
// Initial revision
// 
//    Rev 1.0   08 Nov 1996 11:52:50   DKEEFER
// Initial revision
// 
//    Rev 1.2   10 May 1996 19:02:28   TSmith
// Added flag to prevent access to uninitialized
// load management structure.
//
//    Rev 1.1   24 Apr 1996 18:32:24   TSmith
// Added some OutputDebugString calls.
//
//    Rev 1.0   16 Apr 1996 19:22:14   TSmith
// Initial revision.
//
//    Rev 1.0   10 Aug 1994 10:09:12   GDZIECI
// Baseline Mario from Vishnu with Win32 awk changes applied
//
//    Rev 1.7   04 Dec 1993 14:15:52   JMILLARD
// return error from InitServerLoadManagement
//
//    Rev 1.6   16 Sep 1993 23:46:02   JMILLARD
// fix check of no tables and not initialized
//
//    Rev 1.5   16 Sep 1993 22:20:22   JMILLARD
// check for tables initialized OK
//
//    Rev 1.4   25 May 1993 00:34:24   EHU
// Added symalog.h for log.h.
//
//    Rev 1.3   16 Apr 1993 17:48:42   JMILLARD
// add log.h for DEFAULT_COLOR
//
//    Rev 1.2   13 Apr 1993 22:25:36   JMILLARD
// fix ifdef DEBUG problem
//
//    Rev 1.1   13 Apr 1993 22:20:24   JMILLARD
// change load management stuff
//
//    Rev 1.0   09 Apr 1993 17:07:44   JMILLARD
// Initial revision.
//
//**********************************************************************
#define FUZZY   1

//
// Common includes
//
#include "platform.h"					// cross-platform type definitions
#include "loadmgmt.h"					// local prototypes and typedefs
#include "errcode.h"					// OK, FAILED
#include "comstr.h"						// COM_NO_MEMORY
#include <iostream.h>

//
// Netware specific includes
//
#if defined( SYM_NLM )

	#include "servutil.h"				// GetServerLoad
	#include "cfgfile.h"				// for backlist.h
	#include "backlist.h"				// background event struct
	#include "thrdmgmt.h"				// RelinquishControl

	#if defined( SYM_DEBUG )

		#undef EOF						// for stdio.h
		#include <stdio.h>				// sprintf
		#include "intrcept.h"			// for monitor.h
		#include "nfevlist.h"			// for monitor.h
		#include "pipe.h"				// for navpipe.h
		#include "navpipe.h"			// for monitor.h
		#include "monitor.h"			// PostString
		#include "symalog.h"			// for log.h
		#include "log.h"				// Log and DEFAULT_COLOR
		#include <conio.h>				// ConsolePrintf

	#endif  // SYM_DEBUG

#endif	// SYM_NLM

//
// Win32 specific includes
//
#if defined( SYM_WIN32 )

	#include "ntsrvutl.h"

#endif	// SYM_WIN32

#if defined( FUZZY )

	#include "fuzzy.h"					// fuzzy structures, typedefs, etc.
	#include "fuzyload.h"				// fuzzy routines and tables for server load
										// management
#endif  // FUZZY

#define TRACE ( ERR_TRACE_ID( LOADMGMT_CPP ) )

//**********************************************************************
//
//  local static data
//
//**********************************************************************

#if defined( FUZZY )

BOOL  bTablesNotInitialized = TRUE;
DWORD dwUseCount            = 0;

#endif  // FUZZY


//**********************************************************************
//
//  local prototypes
//
//**********************************************************************

WORD UpdateServerLoadManagement( LPLOADMANAGEMENT lpLoad,
                                 WORD             wServerLoad,
                                 SWORD            swServerLoadChange );


//**********************************************************************
//
//  Main functions
//
//**********************************************************************


//**********************************************************************
// InitServerLoadManagement()
//
// Description: Initialize server load management
//
// Parameters:
//
// Return Value:
//
// See Also:
//
//**********************************************************************
// 04/15/1996 TSMITH   Added Windows NT performance counter inits
//
// 03/31/1993 JMILLARD Function Created.
//**********************************************************************
BOOL InitServerLoadManagement( LPLOADMANAGEMENT lpLoadManagement,
                               DWORD dwTargetServerLoad )
    {
    SYM_ASSERT( lpLoadManagement != NULL );

#if defined( FUZZY )

    // keep track of load management users
    dwUseCount += 1;

    // initialize all the static fuzzy control tables
    if ( bTablesNotInitialized )
        {
        if ( InitializeFuzzyTables( ) != FAILED )
            {
            bTablesNotInitialized = FALSE;
            }
        else
            {
            // free any partial tables
            FreeFuzzyTables( );
            SetLastError( ERROR_NOT_ENOUGH_MEMORY );
            return FALSE;
            }
        }

#endif  // FUZZY

#if defined( SYM_WIN32 )

        // Init performance counters for processor time
        if ( ! InitPerfCounter( COUNTER_OBJECT_NAME, COUNTER_NAME ) )
            {
            DWORD dwErrorCode = GetLastError();
            SetLastError( dwErrorCode );
            return FALSE;
            }

#endif  // SYM_WIN32

    lpLoadManagement->dwTargetLoad   = dwTargetServerLoad;
    lpLoadManagement->dwCurrentDelay = NO_DELAY;

    lpLoadManagement->wTick1DelayCount = 0;
    lpLoadManagement->wTick2DelayCount = 0;

    lpLoadManagement->wThreadSwitch1Count = 0;
    lpLoadManagement->wThreadSwitch1      = 0;
    lpLoadManagement->wThreadSwitch2      = 1;

    lpLoadManagement->bLoadManagementInitialized = TRUE;

    return TRUE;
    }






//***************************************************************************
// DeInitServerLoadManagement()
//
// Description: DeInitialize server load management
//
// Parameters:
//
// Return Value:
//
// See Also:
//
//***************************************************************************
// 04/15/1996 TSMITH   Added WinNT performance counter cleanup call
//
// 03/31/1993 JMILLARD Function Created.
//***************************************************************************

VOID DeInitServerLoadManagement( LPLOADMANAGEMENT lpLoadManagement )
{
    lpLoadManagement->bLoadManagementInitialized = FALSE;

#if defined(FUZZY)

    if (dwUseCount > 0)
        {
        dwUseCount -= 1;            // keep track of load management users
        }

    // free the static fuzzy control tables

    if (dwUseCount == 0)
        {
        if (!bTablesNotInitialized)
            {
            bTablesNotInitialized = TRUE;
            FreeFuzzyTables( );
            }
        }

#endif  // FUZZY

#if defined( SYM_WIN32 )

	ClosePerfCounter();

#endif

}






//***************************************************************************
// ServerLoadManagementDelay()
//
// Description: delay based on server load and target load
//
// Parameters:
//
// Return Value:
//
// See Also:
//
//***************************************************************************
// 03/31/1993 JMILLARD Function Created.
//***************************************************************************

VOID ServerLoadManagementDelay( LPLOADMANAGEMENT lpLoad )
{
    WORD    wLoop;

#if defined( SYM_DEBUG )

	TCHAR szDebugMsg[ 256 ];
	memset( szDebugMsg, 0xFF, sizeof( szDebugMsg ) );
	DWORD dwDelay = lpLoad->dwCurrentDelay;

    SPRINTF( szDebugMsg, _T( "Load management delay is %d\nTarget load is %d\n" ),
        dwDelay, lpLoad->dwTargetLoad );

#if defined(SYM_NLM)

	PostString( szDebugMsg, DEFAULT_COLOR );

#elif defined(SYM_WIN32)

    OutputDebugString( szDebugMsg );

#endif

#endif // SYM_DEBUG

    // run any coarse delays first

    if (lpLoad->wTick1DelayCount != 0)
        {
        DelayThreadTicks( lpLoad->wTick1Delay );
        goto AllDone;
        }

    if (lpLoad->wTick2DelayCount != 0)
        {
        DelayThreadTicks( lpLoad->wTick2Delay );
        goto AllDone;
        }

    // no coarse? - then run the fine delays

    if (lpLoad->wThreadSwitch1Count != 0)
        {
        wLoop = lpLoad->wThreadSwitch1;
        while (wLoop > 0)
            {
            RelinquishControl( );
            wLoop -= 1;
            }
        goto AllDone;
        }

    // this is the default delay if all the others have run out

    wLoop = lpLoad->wThreadSwitch2;
    while (wLoop > 0)
        {
        RelinquishControl( );
        wLoop -= 1;
        }

AllDone:

    return;             // required because of preceding label
}






//***************************************************************************
// UpdateServerLoad( )
//
// Description: Updates all server load management based on the current
//              server load
//
// Parameters: None
//
// Return Value:
//
// See Also:
//
//***************************************************************************
// 04/15/1996 TSMITH   Added #ifdef	to support WinNT performance counter use.
//					   Supports update of a single LPLOADMANAGEMENT struct
//					   passed to the function rather than an array of them
//					   as in NAVNLM.
//
// 03/31/1993 JMILLARD Function Created.
//***************************************************************************
WORD UpdateServerLoad(
#if defined( SYM_WIN32 )
	LPLOADMANAGEMENT lpLoad
#else
	VOID
#endif
)
{
#if defined( SYM_NLM )

    LPBACKEVENT lpBackEvent;
    WORD        wErr = OK;

#elif defined( SYM_WIN32 )

	BOOL	fResult;
	DWORD	dwServerLoad;

#endif

    WORD		wServerLoad;
	SWORD       swLoadChange;
    STATIC WORD wLastLoad = 0;
#if defined( SYM_DEBUG )
    TCHAR szDebugMsg[ 256 ];
#endif

#if defined( SYM_NLM )

    wServerLoad = wGetServerLoad( ) / LOAD_SCALE; // get the current load

    // calculate change in load since last time

    swLoadChange = wServerLoad - wLastLoad;


#elif defined( SYM_WIN32 )

	fResult = GetCpuLoad( &dwServerLoad );

#if defined( SYM_DEBUG )
    SPRINTF( szDebugMsg, _T( "dwServerLoad=%d\n" ), dwServerLoad );
    OutputDebugString( szDebugMsg );
#endif

	wServerLoad = ( WORD )dwServerLoad;
	swLoadChange = wServerLoad - wLastLoad;

#endif

#if defined( SYM_NLM )

    // loop through all scheduled events, and update server load management
    // information

    lpBackEvent = pGetFirstBackEventList( );

    while (lpBackEvent)
        {

        UpdateServerLoadManagement( &lpBackEvent->LoadManage,
                                    wServerLoad,
                                    swLoadChange);

        lpBackEvent = lpBackEvent->lpNextBackList;

        }

    // update the server load for next time;

    wLastLoad = wServerLoad;

    return (wErr);

#elif defined( SYM_WIN32 )

	UpdateServerLoadManagement( lpLoad,
								wServerLoad,
								swLoadChange );

	wLastLoad = wServerLoad;

	return fResult;

#endif


}

//***************************************************************************
// UpdateServerLoadManagement( )
//
// Description: Updates server load management structure based on the current
//              server load
//
// Parameters:
//
// Return Value:
//
// See Also:
//
//***************************************************************************
// 03/31/1993 JMILLARD Function Created.
//***************************************************************************
WORD UpdateServerLoadManagement( LPLOADMANAGEMENT lpLoad,
                                 WORD             wServerLoad,
                                 SWORD            swServerLoadChange)
{
    WORD    wErr = OK;
    SDWORD  sdwDelayChange;
    DWORD   dwDelay;

#if defined( SYM_DEBUG )

	TCHAR szDebugMsg[ 256 ];
	memset( szDebugMsg, 0xFF, sizeof( szDebugMsg ) );

    SPRINTF( szDebugMsg,
             _T( "wServerLoad=%d\nswServerLoadChange=%d\n" ),
             wServerLoad,
             swServerLoadChange );

    OutputDebugString( szDebugMsg );

#endif // SYM_DEBUG


    // do nothing until the load management is set up. This entry gets
    // queued shortly before the load management is set up.

    if (lpLoad->bLoadManagementInitialized && !bTablesNotInitialized)
        {

#if defined(FUZZY)

        sdwDelayChange = GetNewDelay( &lpLoad->dwCurrentDelay,
                            		  lpLoad->dwTargetLoad,
                                   	  wServerLoad,
                                   	  swServerLoadChange );

#if defined( SYM_DEBUG )
    SPRINTF( szDebugMsg, _T( "sdwDelayChange=%d\n" ), sdwDelayChange );
    OutputDebugString( szDebugMsg );
#endif

#else   // ifdef FUZZY

        if ( lpLoad->dwTargetLoad > wServerLoad )
            {

            // spare server capacity available, use some more, if not already
            // max'ed out

            sdwDelayChange = -LOAD_INCREMENT;

            }
        else if ( lpLoad->dwTargetLoad < wServerLoad )
            {

            // server too busy - back off a bit if not already doing nothing

            sdwDelayChange = LOAD_INCREMENT;

            }
        else
            {

            // things are just right

            sdwDelayChange = 0;

            }

#endif  // ifdef FUZZY

        (void) swServerLoadChange;          // currently unused

        // adjust my delay, clamping to NO_DELAY and INFINITY

        if (sdwDelayChange < 0)
            {
            if (lpLoad->dwCurrentDelay > (NO_DELAY - sdwDelayChange) )
                {
                lpLoad->dwCurrentDelay += sdwDelayChange;
                }
            else
                {
                lpLoad->dwCurrentDelay = NO_DELAY;
                }
            }
        else if ( sdwDelayChange > 0 )
            {

            if (lpLoad->dwCurrentDelay < (INFINITY - sdwDelayChange) )
                {
                lpLoad->dwCurrentDelay += sdwDelayChange;
                }
            else
                {
                lpLoad->dwCurrentDelay = INFINITY;
                }
            }
        }

    // set the load management delays based on the current load setting

    dwDelay = lpLoad->dwCurrentDelay;
    dwDelay = (dwDelay + 2) / 3;        // stretch the scale out a bit

#if defined( SYM_DEBUG )
    SPRINTF( szDebugMsg, _T( "dwDelay=%d\n" ), dwDelay );
    OutputDebugString( szDebugMsg );
#endif

    if (dwDelay == 1)                   // minimum delay
        {
        lpLoad->wTick1DelayCount = 0;
        lpLoad->wTick2DelayCount = 0;

        lpLoad->wThreadSwitch1Count = 0;

        lpLoad->wThreadSwitch2      = 1;
        }

    // thread switch loops

    else if (dwDelay <= MAX_SPIN_BREAKPOINT)
        {
        lpLoad->wTick1DelayCount = 0;
        lpLoad->wTick2DelayCount = 0;
										//
										// Typecast the following assignments
										// to avoid compiler warning C4244
										//
        lpLoad->wThreadSwitch1Count = (WORD)((dwDelay - 2) % DELAY_CYCLE) + 1;
        lpLoad->wThreadSwitch1      = (WORD)(dwDelay - 2) / DELAY_CYCLE + 2;

        lpLoad->wThreadSwitch2      = (WORD)(dwDelay - 2) / DELAY_CYCLE + 1;
        }

    // timed delays, with thread switches as necessary

    else
        {
										//
										// Typecast the following assignments
										// to avoid compiler warning C4244
										//
        lpLoad->wTick1DelayCount = (WORD)((dwDelay - 2) % DELAY_CYCLE) + 1;
        lpLoad->wTick1Delay      = (WORD)(dwDelay - 2) / DELAY_CYCLE + 2 - MAX_SPIN;

        // whatever is left of DELAY_CYCLE, but 1 less tick delay

        lpLoad->wTick2DelayCount = DELAY_CYCLE - lpLoad->wTick1DelayCount;
        lpLoad->wTick2Delay      = (WORD)(dwDelay - 2) / DELAY_CYCLE + 1 - MAX_SPIN;

        if (lpLoad->wTick2Delay == 0)
            {
            lpLoad->wTick2DelayCount = 0;
            }

        lpLoad->wThreadSwitch1Count = 0;

        // in case I run out of tick delays

        lpLoad->wThreadSwitch2      = MAX_SPIN;
        }

/*
#if defined( SYM_DEBUG )

    SPRINTF( szDebugMsg, _T( "Delay t1 %d %d t2 %d %d ts1 %d %d ts2 %d\n" ),
        	 lpLoad->wTick1DelayCount,
        	 lpLoad->wTick1Delay,
        	 lpLoad->wTick2DelayCount,
        	 lpLoad->wTick2Delay,
        	 lpLoad->wThreadSwitch1Count,
        	 lpLoad->wThreadSwitch1,
        	 lpLoad->wThreadSwitch2 );

#if defined( SYM_NLM )

    ConsolePrintf( szDebugMsg );

#elif defined( SYM_WIN32 )

	OutputDebugString( szDebugMsg );

#endif

#endif  // SYM_DEBUG
*/
    return (wErr);
}
