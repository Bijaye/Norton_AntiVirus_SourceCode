// Copyright 1996 Symantec, Peter Norton Group
//**********************************************************************
//
// $Header:   S:/include/VCS/fuzzy.h_v   1.0   06 Feb 1997 20:52:02   RFULLER  $
//
//**********************************************************************
// $Log:   S:/include/VCS/fuzzy.h_v  $
// 
//    Rev 1.0   06 Feb 1997 20:52:02   RFULLER
// Initial revision
// 
//    Rev 1.0   16 Apr 1996 19:26:14   TSmith
// Initial revision.
//
//    Rev 1.0   15 Aug 1994 16:14:46   GDZIECI
// Baseline Mario from Vishnu with Win32 awk changes applied
//
//    Rev 1.1   23 Jun 1993 20:41:08   JMILLARD
//
//    Rev 1.0   13 Apr 1993 22:19:00   JMILLARD
// Initial revision.
//
/************************************************************************/
#if !defined(_FUZZY_H)
#define _FUZZY_H

//  .h files required to include this .h file

#if !defined(INCL_PLATFORM)
#error platform.h MUST be included before fuzzy.h
#endif

//**********************************************************************
//
//  defines
//
//**********************************************************************

#define FIXED_POINT_SCALE     1000		// scale

// fuzzy operators

#define FUZZY_AND   0
#define FUZZY_OR    1

// fuzzy output correlation types

#define CORRELATION_MINIMUM     0
#define CORRELATION_PRODUCT     1

// fuzzy-to-crisp conversion types

#define CENTROID                0

//**********************************************************************
//
//  Typedefs
//
//**********************************************************************

typedef WORD FUZZYOPERATOR;

// fuzzy set definition - this defines a fuzzy set in line segment form.
// each pair of points defines a continuous part of the fuzzy set. All
// areas outside the defined area are assumed to be zero.

#pragma warning(disable: 4200)			// disable zero length array warning

typedef struct tagFUZZYSETDEF			// complete declaration
    {

    WORD		wNumberOfPoints;
    SDWORD		dwDefinitionPoints[];	// variable size array of points (x,y)
                                        // points must in increasing x order
    } FUZZYSETDEF;

typedef struct tagFUZZYSETDEF FAR *LPFUZZYSETDEF;


// discrete definition of a fuzzy set
typedef struct tagFUZZYSET              // complete declaration
    {

    SDWORD      sdwLowValue;            // first x value for set
    SDWORD      sdwHighValue;           // last x value for set

    DWORD       dwNumberOfValues;       // number of values in set

    LPDWORD		dwSetValue;				// set value for each value of x
                                        // in the set - FIXED_POINT scaled
    } FUZZYSET;

typedef struct tagFUZZYSET FAR *LPFUZZYSET;


// fuzzy rule definition

typedef struct tagFUZZYRULE             // complete declaration
    {

    WORD            wVectorIndex1;
    WORD            wVectorIndex2;
    FUZZYOPERATOR   Operator;
    LPFUZZYSET FAR *lplpResult;

    } FUZZYRULE;

typedef struct tagFUZZYRULE FAR *LPFUZZYRULE;


// fuzzy activation value - these are fixed point numbers, scaled by
// FIXED_POINT_SCALE

typedef DWORD FUZZYACTIVATION;
typedef FUZZYACTIVATION FAR *LPFUZZYACTIVIATION;



//**********************************************************************
//
//  Prototypes
//
//**********************************************************************

WORD BuildFuzzySets( LPFUZZYSETDEF lpFuzzySetDefs[ ],
                     WORD          wDefinitionCount,
                     LPFUZZYSET    lpFuzzySets[ ] );

VOID FreeFuzzySets( LPFUZZYSET lpFuzzySets[ ], WORD wDefinitionCount );

WORD ComputeFuzzyActivationVector( SWORD            swInputValue,
                                   FUZZYACTIVATION  ActivationVector[ ],
                                   LPFUZZYSET       lpFuzzySet[ ],
                                   WORD             wVectorSize );

WORD ProcessRuleBank( LPFUZZYRULE       lpRuleSet[ ],
                      WORD              wRuleCount,
                      FUZZYACTIVATION   ActivationVector1[ ],
                      FUZZYACTIVATION   ActivationVector2[ ],
                      LPFUZZYSET        lpFuzzyOutputSet,
                      WORD              wOutputSetSize);

FUZZYACTIVATION ComputeRuleActivation( LPFUZZYRULE lpRule,
                                       FUZZYACTIVATION ActivationVector1[ ],
                                       FUZZYACTIVATION ActivationVector2[ ] );

WORD AddRuleOutputToFuzzySet( LPFUZZYRULE     lpRule,
                              FUZZYACTIVATION RuleActivation,
                              WORD            wAddType,
                              LPFUZZYSET      lpOutputSet );

SDWORD CrispValueFromFuzzySet( LPFUZZYSET lpFuzzySet, WORD wConversionType );


#endif  // _FUZZY_H
