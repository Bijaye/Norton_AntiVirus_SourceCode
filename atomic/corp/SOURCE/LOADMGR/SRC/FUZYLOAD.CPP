// Copyright 1996 Symantec Corporation
//**********************************************************************
//
// $Header:   S:/LOADMGR/VCS/fuzyload.cpv   1.1   18 Apr 1997 13:54:30   DSACKIN  $
//
// Description: fuzzy data table and functions
//
//**********************************************************************
//
// $Log:   S:/LOADMGR/VCS/fuzyload.cpv  $
// 
//    Rev 1.1   18 Apr 1997 13:54:30   DSACKIN
// Ported from Tesla
// 
//    Rev 1.0   13 Mar 1997 15:11:42   RFULLER
// Initial revision
// 
//    Rev 1.0   08 Nov 1996 11:52:52   DKEEFER
// Initial revision
// 
//    Rev 1.0   16 Apr 1996 19:22:18   TSmith
// Initial revision.
//
//    Rev 1.0   10 Aug 1994 10:09:12   GDZIECI
// Baseline Mario from Vishnu with Win32 awk changes applied
//
//    Rev 1.1   16 Sep 1993 22:19:18   JMILLARD
// return errors from InitializeFuzzyTables
//
//    Rev 1.0   13 Apr 1993 22:20:48   JMILLARD
// Initial revision.
//
//**********************************************************************

#include "platform.h"       // cross-platform type definitions

#include "fuzzy.h"          // general fuzzy structures and defines
#include "fuzyload.h"       // local prototypes and typedefs
#include "errcode.h"        // OK, FAILED

#ifdef DEBUG

#if defined(SYM_NLM)

	#include <conio.h>		// ConsolePrintf

#endif	// SYM_NLM

#endif  // DEBUG

#define TRACE (ERR_TRACE_ID(FUZYDATA_C))



//**********************************************************************
//
//  defines
//
//**********************************************************************


//**********************************************************************
//
//  local static data
//
//**********************************************************************

STATIC LPFUZZYSET lpLoadErrorFuzzySets[LOAD_ERROR_VECTOR_SIZE];
STATIC LPFUZZYSET lpLoadChangeFuzzySets[LOAD_CHANGE_VECTOR_SIZE];
STATIC LPFUZZYSET lpDelayChangeFuzzySets[DELAY_CHANGE_VECTOR_SIZE];


// rules for generating delay changes

// everything ok rule

STATIC FUZZYRULE Rule1 = {LOAD_ERROR_ZERO,
                          LOAD_CHANGE_ZERO,
                          FUZZY_AND,
                          &lpDelayChangeFuzzySets[DELAY_CHANGE_ZERO]};

// both out of balance rules

STATIC FUZZYRULE Rule2 = {LOAD_ERROR_POSITIVE_SMALL,
                          LOAD_CHANGE_POSITIVE_SMALL,
                          FUZZY_AND,
                          &lpDelayChangeFuzzySets[DELAY_CHANGE_POSITIVE_SMALL]};

STATIC FUZZYRULE Rule3 = {LOAD_ERROR_POSITIVE_MEDIUM,
                          LOAD_CHANGE_POSITIVE_MEDIUM,
                          FUZZY_AND,
                          &lpDelayChangeFuzzySets[DELAY_CHANGE_POSITIVE_MEDIUM]};

STATIC FUZZYRULE Rule4 = {LOAD_ERROR_POSITIVE_LARGE,
                          LOAD_CHANGE_POSITIVE_LARGE,
                          FUZZY_AND,
                          &lpDelayChangeFuzzySets[DELAY_CHANGE_POSITIVE_LARGE]};

STATIC FUZZYRULE Rule5 = {LOAD_ERROR_NEGATIVE_SMALL,
                          LOAD_CHANGE_NEGATIVE_SMALL,
                          FUZZY_AND,
                          &lpDelayChangeFuzzySets[DELAY_CHANGE_NEGATIVE_SMALL]};

STATIC FUZZYRULE Rule6 = {LOAD_ERROR_NEGATIVE_MEDIUM,
                          LOAD_CHANGE_NEGATIVE_MEDIUM,
                          FUZZY_AND,
                          &lpDelayChangeFuzzySets[DELAY_CHANGE_NEGATIVE_MEDIUM]};

STATIC FUZZYRULE Rule7 = {LOAD_ERROR_NEGATIVE_LARGE,
                          LOAD_CHANGE_NEGATIVE_LARGE,
                          FUZZY_AND,
                          &lpDelayChangeFuzzySets[DELAY_CHANGE_NEGATIVE_LARGE]};


STATIC FUZZYRULE Rule8 = {LOAD_ERROR_POSITIVE_SMALL,
                          LOAD_CHANGE_NEGATIVE_SMALL,
                          FUZZY_AND,
                          &lpDelayChangeFuzzySets[DELAY_CHANGE_POSITIVE_SMALL]};

STATIC FUZZYRULE Rule9 = {LOAD_ERROR_POSITIVE_MEDIUM,
                          LOAD_CHANGE_NEGATIVE_MEDIUM,
                          FUZZY_AND,
                          &lpDelayChangeFuzzySets[DELAY_CHANGE_POSITIVE_MEDIUM]};

STATIC FUZZYRULE Rule10= {LOAD_ERROR_POSITIVE_LARGE,
                          LOAD_CHANGE_NEGATIVE_LARGE,
                          FUZZY_AND,
                          &lpDelayChangeFuzzySets[DELAY_CHANGE_POSITIVE_LARGE]};

STATIC FUZZYRULE Rule11= {LOAD_ERROR_NEGATIVE_SMALL,
                          LOAD_CHANGE_POSITIVE_SMALL,
                          FUZZY_AND,
                          &lpDelayChangeFuzzySets[DELAY_CHANGE_NEGATIVE_SMALL]};

STATIC FUZZYRULE Rule12= {LOAD_ERROR_NEGATIVE_MEDIUM,
                          LOAD_CHANGE_POSITIVE_MEDIUM,
                          FUZZY_AND,
                          &lpDelayChangeFuzzySets[DELAY_CHANGE_NEGATIVE_MEDIUM]};

STATIC FUZZYRULE Rule13= {LOAD_ERROR_NEGATIVE_LARGE,
                          LOAD_CHANGE_POSITIVE_LARGE,
                          FUZZY_AND,
                          &lpDelayChangeFuzzySets[DELAY_CHANGE_NEGATIVE_LARGE]};

// load error == 0 rules

STATIC FUZZYRULE Rule14= {LOAD_ERROR_ZERO,
                          LOAD_CHANGE_NEGATIVE_LARGE,
                          FUZZY_AND,
                          &lpDelayChangeFuzzySets[DELAY_CHANGE_ZERO]};

STATIC FUZZYRULE Rule15= {LOAD_ERROR_ZERO,
                          LOAD_CHANGE_NEGATIVE_MEDIUM,
                          FUZZY_AND,
                          &lpDelayChangeFuzzySets[DELAY_CHANGE_ZERO]};

STATIC FUZZYRULE Rule16= {LOAD_ERROR_ZERO,
                          LOAD_CHANGE_NEGATIVE_SMALL,
                          FUZZY_AND,
                          &lpDelayChangeFuzzySets[DELAY_CHANGE_ZERO]};

STATIC FUZZYRULE Rule17= {LOAD_ERROR_ZERO,
                          LOAD_CHANGE_POSITIVE_SMALL,
                          FUZZY_AND,
                          &lpDelayChangeFuzzySets[DELAY_CHANGE_ZERO]};

STATIC FUZZYRULE Rule18= {LOAD_ERROR_ZERO,
                          LOAD_CHANGE_POSITIVE_MEDIUM,
                          FUZZY_AND,
                          &lpDelayChangeFuzzySets[DELAY_CHANGE_ZERO]};

STATIC FUZZYRULE Rule19= {LOAD_ERROR_ZERO,
                          LOAD_CHANGE_POSITIVE_LARGE,
                          FUZZY_AND,
                          &lpDelayChangeFuzzySets[DELAY_CHANGE_ZERO]};

// load change == 0 rules

STATIC FUZZYRULE Rule20= {LOAD_ERROR_POSITIVE_LARGE,
                          LOAD_CHANGE_ZERO,
                          FUZZY_AND,
                          &lpDelayChangeFuzzySets[DELAY_CHANGE_POSITIVE_LARGE]};

STATIC FUZZYRULE Rule21= {LOAD_ERROR_POSITIVE_MEDIUM,
                          LOAD_CHANGE_ZERO,
                          FUZZY_AND,
                          &lpDelayChangeFuzzySets[DELAY_CHANGE_POSITIVE_MEDIUM]};

STATIC FUZZYRULE Rule22= {LOAD_ERROR_POSITIVE_SMALL,
                          LOAD_CHANGE_ZERO,
                          FUZZY_AND,
                          &lpDelayChangeFuzzySets[DELAY_CHANGE_POSITIVE_SMALL]};

STATIC FUZZYRULE Rule23= {LOAD_ERROR_NEGATIVE_SMALL,
                          LOAD_CHANGE_ZERO,
                          FUZZY_AND,
                          &lpDelayChangeFuzzySets[DELAY_CHANGE_NEGATIVE_SMALL]};

STATIC FUZZYRULE Rule24= {LOAD_ERROR_NEGATIVE_MEDIUM,
                          LOAD_CHANGE_ZERO,
                          FUZZY_AND,
                          &lpDelayChangeFuzzySets[DELAY_CHANGE_NEGATIVE_MEDIUM]};

STATIC FUZZYRULE Rule25= {LOAD_ERROR_NEGATIVE_LARGE,
                          LOAD_CHANGE_ZERO,
                          FUZZY_AND,
                          &lpDelayChangeFuzzySets[DELAY_CHANGE_NEGATIVE_LARGE]};

STATIC LPFUZZYRULE lpControlRules[ ] = { &Rule1 , &Rule2 , &Rule3 , &Rule4 ,
                                         &Rule5 , &Rule6 , &Rule7 , &Rule8 ,
                                         &Rule9 , &Rule10, &Rule11, &Rule12,
                                         &Rule13, &Rule14, &Rule15, &Rule16,
                                         &Rule17, &Rule18, &Rule19, &Rule20,
                                         &Rule21, &Rule22, &Rule23, &Rule24,
                                         &Rule25 };


//**********************************************************************
//
//  local prototypes
//
//**********************************************************************



//**********************************************************************
//
//  Main functions
//
//**********************************************************************





//***************************************************************************
// InitializeFuzzyTables()
//
// Description: Initialize fuzzy tables
//
// Parameters:
//
// Return Value:
//
// See Also:
//
//***************************************************************************
// 04/11/1993 JMILLARD Function Created.
//***************************************************************************

WORD InitializeFuzzyTables( VOID )
{
    WORD wErr = OK;

    // input load fuzzy set definitions

    STATIC FUZZYSETDEF LoadErrorNegativeLarge =
        {
        3, {-100,1, -60,1, -40,0}
        };
    STATIC FUZZYSETDEF LoadErrorNegativeMedium =
        {
        3, {-60,0, -40,1, -20,0}
        };
    STATIC FUZZYSETDEF LoadErrorNegativeSmall =
        {
        3, {-40,0, -20,1, 0,0}
        };
    STATIC FUZZYSETDEF LoadErrorZero =
        {
        3, {-20,0, 0,1, 20,0}
        };
    STATIC FUZZYSETDEF LoadErrorPositiveSmall =
        {
        3, {0,0, 20,1, 40,0}
        };
    STATIC FUZZYSETDEF LoadErrorPositiveMedium =
        {
        3, {20,0, 40,1, 60,0}
        };
    STATIC FUZZYSETDEF LoadErrorPositiveLarge =
        {
        3, {40,0, 60,1, 100,1}
        };

    STATIC LPFUZZYSETDEF lpLoadErrorFuzzySetDefs[LOAD_ERROR_VECTOR_SIZE] =
        {
        &LoadErrorNegativeLarge,
        &LoadErrorNegativeMedium,
        &LoadErrorNegativeSmall,
        &LoadErrorZero,
        &LoadErrorPositiveSmall,
        &LoadErrorPositiveMedium,
        &LoadErrorPositiveLarge
        };

    // input load change fuzzy set definitions

    STATIC FUZZYSETDEF LoadChangeNegativeLarge =
        {
        3, {-100,1, -95,1, -90,0}
        };
    STATIC FUZZYSETDEF LoadChangeNegativeMedium =
        {
        3, {-95,0, -90,1, -85,0}
        };
    STATIC FUZZYSETDEF LoadChangeNegativeSmall =
        {
        3, {-90,0, -85,1, -80,0}
        };
    STATIC FUZZYSETDEF LoadChangeZero =
        {
        4, {-85,0, -80,1, 80,1, 85,0}
        };
    STATIC FUZZYSETDEF LoadChangePositiveSmall =
        {
        3, {80,0, 85,1, 90,0}
        };
    STATIC FUZZYSETDEF LoadChangePositiveMedium =
        {
        3, {85,0, 90,1, 95,0}
        };
    STATIC FUZZYSETDEF LoadChangePositiveLarge =
        {
        3, {90,0, 95,1, 100,1}
        };

    STATIC LPFUZZYSETDEF lpLoadChangeFuzzySetDefs[LOAD_CHANGE_VECTOR_SIZE] =
        {
        &LoadChangeNegativeLarge,
        &LoadChangeNegativeMedium,
        &LoadChangeNegativeSmall,
        &LoadChangeZero,
        &LoadChangePositiveSmall,
        &LoadChangePositiveMedium,
        &LoadChangePositiveLarge
        };


    // output delay change fuzzy set definition

    STATIC FUZZYSETDEF DelayChangeNegativeLarge =
        {
        3, {-100,1, -40,1, -20,0}
        };
    STATIC FUZZYSETDEF DelayChangeNegativeMedium =
        {
        3, {-30,0, -20,1, -10,0}
        };
    STATIC FUZZYSETDEF DelayChangeNegativeSmall =
        {
        3, {-20,0, -10,1, -0,0}
        };
    STATIC FUZZYSETDEF DelayChangeZero =
        {
        3, {-7,0, 0,1, 7,0}
        };
    STATIC FUZZYSETDEF DelayChangePositiveSmall =
        {
        3, {0,0, 10,1, 20,0}
        };
    STATIC FUZZYSETDEF DelayChangePositiveMedium =
        {
        3, {10,0, 20,1, 30,0}
        };
    STATIC FUZZYSETDEF DelayChangePositiveLarge =
        {
        3, {20,0, 40,1, 100,1}
        };

    STATIC LPFUZZYSETDEF lpDelayChangeFuzzySetDefs[DELAY_CHANGE_VECTOR_SIZE] =
        {
        &DelayChangeNegativeLarge,
        &DelayChangeNegativeMedium,
        &DelayChangeNegativeSmall,
        &DelayChangeZero,
        &DelayChangePositiveSmall,
        &DelayChangePositiveMedium,
        &DelayChangePositiveLarge
        };


    // initialize all the static fuzzy control tables

    // build fuzzy sets from definitions

    if (BuildFuzzySets( lpLoadErrorFuzzySetDefs,
                        LOAD_ERROR_VECTOR_SIZE,
                        lpLoadErrorFuzzySets ) == FAILED)
        {
        wErr = FAILED;
        goto BailOut;
        }

    if (BuildFuzzySets( lpLoadChangeFuzzySetDefs,
                        LOAD_CHANGE_VECTOR_SIZE,
                        lpLoadChangeFuzzySets ) == FAILED)
        {
        wErr = FAILED;
        goto BailOut;
        }

    if (BuildFuzzySets( lpDelayChangeFuzzySetDefs,
                        DELAY_CHANGE_VECTOR_SIZE,
                        lpDelayChangeFuzzySets ) == FAILED)
        {
        wErr = FAILED;
        goto BailOut;
        }

BailOut:
    return (wErr);
}





//***************************************************************************
// FreeFuzzyTables()
//
// Description: Free fuzzy tables
//
// Parameters:
//
// Return Value:
//
// See Also:
//
//***************************************************************************
// 04/11/1993 JMILLARD Function Created.
//***************************************************************************

VOID FreeFuzzyTables( VOID )
{

    // free all the static fuzzy control tables

    FreeFuzzySets( lpLoadErrorFuzzySets,
                   LOAD_ERROR_VECTOR_SIZE );

    FreeFuzzySets( lpLoadChangeFuzzySets,
                   LOAD_CHANGE_VECTOR_SIZE );

    FreeFuzzySets( lpDelayChangeFuzzySets,
                   DELAY_CHANGE_VECTOR_SIZE );

}





//***************************************************************************
//
// Description: Calculates new delay from current delay,
//                                        target load,
//                                        current load,
//                                        load change
//
//***************************************************************************
// 04/15/1996 TSMITH   Added code to init FUZZYSET.dwSetValue pointer to
//					   support the change in that struct member from
//					   DWORD[ ] to LPDWORD.		
//
// 04/11/1993 JMILLARD Function Created.
//***************************************************************************

SDWORD GetNewDelay( LPDWORD  lpdwDelay,
                    DWORD    dwTargetLoad,
                    WORD     wServerLoad,
                    SWORD    swServerLoadChange )
{
    FUZZYACTIVATION LoadErrorActivation[LOAD_ERROR_VECTOR_SIZE];
    FUZZYACTIVATION LoadChangeActivation[DELAY_CHANGE_VECTOR_SIZE];

    SWORD           swLoadError;
    SDWORD          sdwDelayChange;
#if defined( SYM_DEBUG )
	TCHAR szDebugMsg[ 256 ];
#endif

	struct
		{
		FUZZYSET DelayChangeFuzzySet;

		// following provides the variable data area

		DWORD DelayChangeFuzzySetValues
			[DELAY_CHANGE_MAX-DELAY_CHANGE_MIN+1];

		} DelayChangeExtended;

	DelayChangeExtended.DelayChangeFuzzySet.dwSetValue =
		&DelayChangeExtended.DelayChangeFuzzySetValues[ 0 ];

#if defined( SYM_DEBUG )
	memset( szDebugMsg, 0xFF, sizeof( szDebugMsg ) );
#endif

	DelayChangeExtended.DelayChangeFuzzySet.sdwLowValue = DELAY_CHANGE_MIN;
    DelayChangeExtended.DelayChangeFuzzySet.sdwHighValue = DELAY_CHANGE_MAX;
    DelayChangeExtended.DelayChangeFuzzySet.dwNumberOfValues =
                            DELAY_CHANGE_MAX-DELAY_CHANGE_MIN+1;

    (void) lpdwDelay;           // currently unused

    // error between target and actual load
#pragma warning(disable: 4244)
    swLoadError = (SWORD)wServerLoad - (SDWORD)dwTargetLoad;
#pragma warning(default: 4244)

    // compute fuzzy activation vectors for inputs

#ifdef DEBUG

    SPRINTF( szDebugMsg, _T( "Load %d Error %d Load Change %d\n" ),
             wServerLoad, swLoadError, swServerLoadChange );

#if defined( SYM_NLM )

    ConsolePrintf( szDebugMsg );

#elif defined(SYM_WIN32)

	OutputDebugString( szDebugMsg );

#endif

#endif  // SYM_DEBUG

    ComputeFuzzyActivationVector( swLoadError,
                                  LoadErrorActivation,
                                  lpLoadErrorFuzzySets,
                                  LOAD_ERROR_VECTOR_SIZE );

    ComputeFuzzyActivationVector( swServerLoadChange,
                                  LoadChangeActivation,
                                  lpLoadChangeFuzzySets,
                                  LOAD_ERROR_VECTOR_SIZE );

#if defined( SYM_DEBUG )

    SPRINTF( szDebugMsg, "Error %d %d %d %d %d %d %d\n",
			 LoadErrorActivation[0],
        	 LoadErrorActivation[1],
        	 LoadErrorActivation[2],
        	 LoadErrorActivation[3],
        	 LoadErrorActivation[4],
        	 LoadErrorActivation[5],
        	 LoadErrorActivation[6] );

#if defined( SYM_NLM )

	ConsolePrintf( szDebugMsg );

#elif defined( SYM_WIN32 )

	OutputDebugString( szDebugMsg );

#endif

    SPRINTF( szDebugMsg, "Change %d %d %d %d %d %d %d\n",
        	 LoadChangeActivation[0],
        	 LoadChangeActivation[1],
        	 LoadChangeActivation[2],
        	 LoadChangeActivation[3],
        	 LoadChangeActivation[4],
        	 LoadChangeActivation[5],
        	 LoadChangeActivation[6] );

#if defined( SYM_NLM )

	ConsolePrintf( szDebugMsg );

#elif defined( SYM_WIN32 )

	OutputDebugString( szDebugMsg );

#endif

#endif  // SYM_DEBUG

    // process the rules, and generate the DelayChangeFuzzySet

    ProcessRuleBank( lpControlRules,
                     sizeof( lpControlRules ) / sizeof( LPFUZZYRULE ),
                     LoadErrorActivation,
                     LoadChangeActivation,
                     &DelayChangeExtended.DelayChangeFuzzySet,
                     sizeof(
                        DelayChangeExtended.DelayChangeFuzzySetValues));

    sdwDelayChange =
	CrispValueFromFuzzySet( &DelayChangeExtended.DelayChangeFuzzySet,
							CENTROID );

#if defined( SYM_DEBUG )

    SPRINTF( szDebugMsg, _T( " Delay %d delay change %d\n" ), *lpdwDelay, sdwDelayChange );

#if defined(SYM_NLM)

	ConsolePrintf( szDebugMsg );

#elif defined( SYM_WIN32 )

	OutputDebugString( szDebugMsg );

#endif

#endif  // SYM_DEBUG

    return ( sdwDelayChange );

}

