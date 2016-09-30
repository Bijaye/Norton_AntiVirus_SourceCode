////////////////////////
//
// PROPRIETARY / CONFIDENTIAL.
// Use of this product is subject to license terms.
// Copyright © 2006 Symantec Corporation.
// All rights reserved.
//
/////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "ccEraserInterface.h"
#include "ccString.h"
#include "commonuiinterface.h"
#include "ScanMgrInterface.h"

enum ANOMALYLISTSTATE
{
    LISTSTATE_GREEN = 0,      // All items have been processed successfully 
                              // (exception: low non-virals)
    LISTSTATE_BLUE_REMAINING, // All viral items have been processed, 
                              // there are unprocessed security risks
    LISTSTATE_BLUE_RESOLVED,  // All viral items have been processed, 
                              // there are ignored medium or high security risks
    LISTSTATE_RED,            // Viruses are remaining
};

class CAnomalyHelper
{
public:
    CAnomalyHelper(void);
    ~CAnomalyHelper(void);

    static bool NeedReboot(ccEraser::IAnomalyList* pAnomalyList);
    static size_t GetAnomalyCount(ccEraser::IAnomalyList* pAnomalyList);

    static size_t GetRemainingManualRemovalCount(ccEraser::IAnomalyList* pAnomalyList);
    static bool UnhandledAnomaliesRemain(ccEraser::IAnomalyList* pAnomalyList,
                                         const SMRESPONSE smrResponseMode);
    static bool AnomaliesNeedProcessing(ccEraser::IAnomalyList* pAnomalyList);
    static ANOMALYLISTSTATE AnomalyListState(ccEraser::IAnomalyList* pAnomalyList);
    static UINT GetStatusVerb(CommonUIInfectionStatus eStatus, bool bViral);
    static UINT GetActionToTakeVerb(AnomalyAction eAction); 

    static long GetUnprocessedItemCount(ccEraser::IAnomalyList* pAnomalyList);
    static bool 
         ExcludeRecommendationAccepted( ccEraser::IAnomalyList* pAnomalyList );

    static void ProcessReviewAndIgnoreItems(
                             ccEraser::IAnomalyList* pAnomalyList,
                             bool bProcessIgnore, long& lIgnoreItemsProcessed,
                             bool bProcessReview, long& lReviewItemsProcessed);
};
