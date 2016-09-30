// Copyright 1996 Symantec Corporation
//**********************************************************************
//
// $Header:   S:/LOADMGR/VCS/fuzzy.cpv   1.1   18 Apr 1997 13:54:28   DSACKIN  $
//
// Description: fuzzy data table and functions
//
//**********************************************************************
//
// $Log:   S:/LOADMGR/VCS/fuzzy.cpv  $
// 
//    Rev 1.1   18 Apr 1997 13:54:28   DSACKIN
// Ported from Tesla
// 
//    Rev 1.0   13 Mar 1997 15:11:40   RFULLER
// Initial revision
// 
//    Rev 1.0   08 Nov 1996 11:52:46   DKEEFER
// Initial revision
// 
//    Rev 1.0   16 Apr 1996 19:22:12   TSmith
// Initial revision.
//
//    Rev 1.2   15 Dec 1994 18:18:00   KEITH
// No change.
//
//    Rev 1.1   10 Nov 1994 09:04:14   JMILLARD
// fix unitialized error return - causes rare failure to start scan because
// it thinks it got a memory error
//
//    Rev 1.0   10 Aug 1994 10:09:12   GDZIECI
// Baseline Mario from Vishnu with Win32 awk changes applied
//
//    Rev 1.3   21 Jan 1994 00:00:42   JMILLARD
// mark fuzzy sets freed when freed, so I don't try to free freed memory
// when the initialize fails part way through, and the tables have already
// been loaded once
//
//    Rev 1.2   16 Sep 1993 22:17:10   JMILLARD
// return error on memory allocation failure
//
//    Rev 1.1   12 May 1993 22:36:52   EHU
// Include <stdlib.h> for min and max prototypes.
//
//    Rev 1.0   13 Apr 1993 22:20:52   JMILLARD
// Initial revision.
//
//**********************************************************************
#include <stdlib.h>         			// min, max
#include "platform.h"       			// cross-platform type definitions
#include "fuzzy.h"          			// local prototypes and typedefs
#include "errcode.h"        			// OK, FAILED
#include "symassrt.h"       			// Assert macros and functions

#define TRACE (ERR_TRACE_ID(FUZZY_CPP))



//**********************************************************************
//
//  local static data
//
//**********************************************************************


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
// BuildFuzzySet()
//
// Description: Build fuzzy set from fuzzy set definitions
//
// Parameters:
//
// Return Value:
//
// See Also:
//
//***************************************************************************
// 04/16/1996 TSMITH   Added code to init the dwSetValue 'pointer'
//
// 04/12/1993 JMILLARD Function Created.
//***************************************************************************

WORD BuildFuzzySets( LPFUZZYSETDEF lpFuzzySetDefs[ ],
                     WORD          wDefinitionCount,
                     LPFUZZYSET    lpFuzzySets[ ] )
{
    int     i, j, k;

    WORD    wErr = OK;

    WORD    wPointCount;
    SDWORD  sdwLowValue;
    SDWORD  sdwHighValue;
    DWORD   dwDiscretePoints;

    DWORD   dwStructSize;
    DWORD   dwDataSize;

    SDWORD  sdwLowXValue;
    SDWORD  sdwLowYValue;
    SDWORD  sdwHighXValue;
    SDWORD  sdwHighYValue;

    SDWORD  sdwYDelta;
    SDWORD  sdwXDelta;

    SDWORD  sdwSlope;
    SDWORD  sdwIntercept;

    LPDWORD lpdwCurrPoint;
    LPSDWORD lpsdwCurrInterval;


    for (i=0; i<wDefinitionCount; i++)
        {

        // number of points in definition

        wPointCount = lpFuzzySetDefs[i]->wNumberOfPoints;

        // first value in set

        sdwLowValue = lpFuzzySetDefs[i]->dwDefinitionPoints[0];

        // last value in set

        sdwHighValue = lpFuzzySetDefs[i]->
                                dwDefinitionPoints[(wPointCount-1) * 2];

        // calculate the number of discrete points in the set

        dwDiscretePoints = sdwHighValue - sdwLowValue + 1;

        // allocate memory for the set

        dwStructSize = sizeof( FUZZYSET );
        dwDataSize   = dwDiscretePoints * sizeof (DWORD);

        lpFuzzySets[ i ] = ( LPFUZZYSET )MemAllocPtr( GHND, dwStructSize + dwDataSize );
		lpFuzzySets[ i ]->dwSetValue = ( LPDWORD )( ( LPBYTE )lpFuzzySets[ i ] + dwStructSize );

        if (lpFuzzySets[i])
            {

            // fill in fixed part of set

            lpFuzzySets[i]->sdwLowValue     = sdwLowValue;
            lpFuzzySets[i]->sdwHighValue    = sdwHighValue;
            lpFuzzySets[i]->dwNumberOfValues= dwDiscretePoints;

            // generate table of set values

            lpdwCurrPoint = lpFuzzySets[i]->dwSetValue;
            lpsdwCurrInterval = lpFuzzySetDefs[i]->dwDefinitionPoints;

            // process each interval (all but last point)

            for (j=0; j<wPointCount-1; j++)
                {
                sdwLowXValue  = *lpsdwCurrInterval++;   // skip first set
                sdwLowYValue  = *lpsdwCurrInterval++;
                sdwHighXValue = lpsdwCurrInterval[0];
                sdwHighYValue = lpsdwCurrInterval[1];

                // calculate line for this interval

                // calculate slope

                sdwYDelta = (sdwHighYValue - sdwLowYValue) *
                                                        FIXED_POINT_SCALE;
                sdwXDelta = sdwHighXValue - sdwLowXValue;

                if (sdwXDelta == 0)
                    {
                    wErr = FAILED;
                    goto BailOut;
                    }

                sdwSlope = sdwYDelta / sdwXDelta;       // scaled slope

                // calculate intercept

                sdwIntercept = sdwLowYValue * FIXED_POINT_SCALE -
                               sdwLowXValue * sdwSlope;

                // fill in all the calculated points

                for (k=sdwLowXValue; k<sdwHighXValue; k++)
                    {
                    *lpdwCurrPoint++ = sdwSlope * k + sdwIntercept;
                    }
                }

            // fill in the last point in the last interval

            *lpdwCurrPoint = sdwSlope * sdwHighXValue + sdwIntercept;

            }
        else
            {
            wErr = FAILED;
            break;
            }
        }

BailOut:
    return (wErr);

}





//***************************************************************************
// FreeFuzzySet()
//
// Description: Frees fuzzy set from memory
//
// Parameters:
//
// Return Value:
//
// See Also:
//
//***************************************************************************
// 04/12/1993 JMILLARD Function Created.
//***************************************************************************

VOID FreeFuzzySets( LPFUZZYSET lpFuzzySets[ ], WORD wDefinitionCount )
{
    int     i;


    for (i=0; i<wDefinitionCount; i++)
        {
        if (lpFuzzySets[i])
            {
            MemFreePtr( lpFuzzySets[i] );
            lpFuzzySets[i] = NULL;      // mark freed
            }
        }
}





//***************************************************************************
// ComputeFuzzyActivationVector()
//
// Description: Compute activation vector from input value and fuzzy set
//
// Parameters:
//
// Return Value:
//
// See Also:
//
//***************************************************************************
// 04/12/1993 JMILLARD Function Created.
//***************************************************************************


WORD ComputeFuzzyActivationVector( SWORD            swInputValue,
                                   FUZZYACTIVATION  ActivationVector[ ],
                                   LPFUZZYSET       lpFuzzySet[ ],
                                   WORD             wVectorSize )
{
    int                 i;
    WORD                wErr = OK;
    FUZZYACTIVATION     FuzzyActivation;

    SDWORD              sdwOffset;


    for (i=0; i<wVectorSize; i++)
        {
        if (swInputValue < lpFuzzySet[i]->sdwLowValue ||
            swInputValue > lpFuzzySet[i]->sdwHighValue)
            {

            // outside of set - activation is zero

            FuzzyActivation = 0;

            }
        else
            {

            // otherwise the activation is simply the value of the set
            // for the input value


            sdwOffset = swInputValue - lpFuzzySet[i]->sdwLowValue;

//          Assert( sdwOffset>=0 & sdwOffset<lpFuzzySet[i]->dwNumberOfValues );

            FuzzyActivation = lpFuzzySet[i]->dwSetValue[sdwOffset];
            }

        ActivationVector[i] = FuzzyActivation;

        }

    return (wErr);

}





//***************************************************************************
// ProcessRuleBank()
//
// Description: Processes the fuzzy input sets using the rule back and
//              and generates a fuzzy output set
//
// Parameters:
//
// Return Value:
//
// See Also:
//
//***************************************************************************
// 04/12/1993 JMILLARD Function Created.
//***************************************************************************

WORD ProcessRuleBank( LPFUZZYRULE       lpRuleSet[ ],
                      WORD              wRuleCount,
                      FUZZYACTIVATION   ActivationVector1[ ],
                      FUZZYACTIVATION   ActivationVector2[ ],
                      LPFUZZYSET        lpFuzzyOutputSet,
                      WORD              wOutputSetSize)
{
    int             i;
    WORD            wErr = OK;

    FUZZYACTIVATION RuleActivation;


    // clear the output set

    MEMSET( lpFuzzyOutputSet->dwSetValue, 0, wOutputSetSize );

    // process each rule

    for (i=0; i<wRuleCount; i++)
        {
        RuleActivation = ComputeRuleActivation( lpRuleSet[i],
                                                ActivationVector1,
                                                ActivationVector2 );

        // if the rule is activated at all, compute contribution to
        // the output fuzzy set

        if (RuleActivation > 0)
            {
            AddRuleOutputToFuzzySet( lpRuleSet[i],
                                     RuleActivation,
                                     CORRELATION_PRODUCT,
                                     lpFuzzyOutputSet );
            }
        }

    return (wErr);

}





//***************************************************************************
// ComputeRuleActivation()
//
// Description: Computes activation of rule from sets contributing to rule
//
// Parameters:
//
// Return Value:
//
// See Also:
//
//***************************************************************************
// 04/12/1993 JMILLARD Function Created.
//***************************************************************************

FUZZYACTIVATION ComputeRuleActivation( LPFUZZYRULE lpRule,
                                       FUZZYACTIVATION ActivationVector1[ ],
                                       FUZZYACTIVATION ActivationVector2[ ] )
{
    FUZZYACTIVATION Activation1;
    FUZZYACTIVATION Activation2;

    FUZZYACTIVATION Activation;

    // compute the activations of the rule antecedents

    Activation1 = ActivationVector1[lpRule->wVectorIndex1];
    Activation2 = ActivationVector2[lpRule->wVectorIndex2];

    // compute the rule activation

    switch (lpRule->Operator)
        {
        case FUZZY_AND:
            {

            // fuzzy 'and' is the minimum of the two activations - the idea
            // is that 'and' should as hard as the hardest of the two
            // antecedents

            Activation = min( Activation1, Activation2 );

            break;

            }

        case FUZZY_OR:
            {

            // fuzzy 'or' is the maximum of the two activations - the idea
            // is that 'or' is as easy as the easiest of the two
            // antecedents

            Activation = max( Activation1, Activation2 );

            break;

            }
        }

    return (Activation);

}





//***************************************************************************
// AddRuleToFuzzySet()
//
// Description: Adds the consequent of an activated rule to the fuzzy
//              output set
//
// Parameters:
//
// Return Value:
//
// See Also:
//
//***************************************************************************
// 04/12/1993 JMILLARD Function Created.
//***************************************************************************

WORD AddRuleOutputToFuzzySet( LPFUZZYRULE     lpRule,
                              FUZZYACTIVATION RuleActivation,
                              WORD            wAddType,
                              LPFUZZYSET      lpOutputSet )
{
    int         i;
    WORD        wErr = OK;

    LPFUZZYSET  lpSetToAdd = *lpRule->lplpResult;
    SDWORD      sdwValueOffset;


    sdwValueOffset = lpSetToAdd->sdwLowValue - lpOutputSet->sdwLowValue;

    switch (wAddType)
        {
        case CORRELATION_MINIMUM:
            {

            // correlation minimum simply clamps the output fuzzy set to
            // the activation value

            for (i=0; i<lpSetToAdd->dwNumberOfValues; i++)
                {
                lpOutputSet->dwSetValue[sdwValueOffset + i] +=
                    min( lpSetToAdd->dwSetValue[i], RuleActivation );
                }

            break;
            }

        case CORRELATION_PRODUCT:
            {

            // correlation product multiplies the output fuzzy set by the
            // activation value to get the resultant set

            for (i=0; i<lpSetToAdd->dwNumberOfValues; i++)
                {
                lpOutputSet->dwSetValue[sdwValueOffset + i] +=
                    (lpSetToAdd->dwSetValue[i] * RuleActivation) /
                                                        FIXED_POINT_SCALE;
                }

            break;
            }
        }

    return (wErr);

}





//***************************************************************************
// CrispValueFromFuzzySet()
//
// Description: Returns a crisp value from a fuzzy set
//
// Parameters:
//
// Return Value:
//
// See Also:
//
//***************************************************************************
// 04/12/1993 JMILLARD Function Created.
//***************************************************************************

SDWORD CrispValueFromFuzzySet( LPFUZZYSET lpFuzzySet, WORD wConversionType )
{
    int     i;
    SDWORD  ActivationSum = 0;
    SDWORD  ActivationProduct = 0;

    SDWORD  CrispValue;

    switch (wConversionType)
        {
        case CENTROID:
            {

            for (i=0; i<lpFuzzySet->dwNumberOfValues; i++)
                {
                ActivationSum += lpFuzzySet->dwSetValue[i];
                ActivationProduct += lpFuzzySet->dwSetValue[i] *
                    (lpFuzzySet->sdwLowValue + i);
                }

            if (ActivationSum > 0)
                {
                CrispValue = ActivationProduct / ActivationSum;
                }
            else
                {

                // attempting centroid of empty fuzzy set

//              Assert( ActivationSum != 0 );

                CrispValue = 0;     // divide error == no change for now

                }
            }
        }

    return (CrispValue);

}
