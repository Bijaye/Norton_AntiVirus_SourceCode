#include "stdafx.h"
#include ".\anomalyhelper.h"
#include "resource.h"
#include "..\NAVComUIRes\ResResource.h"
#include "EZEraserObjects.h"

CAnomalyHelper::CAnomalyHelper(void)
{
}

CAnomalyHelper::~CAnomalyHelper(void)
{
}

size_t CAnomalyHelper::GetAnomalyCount(ccEraser::IAnomalyList* pAnomalyList)
{
    if( pAnomalyList == NULL )
    {
        CCTRACEE(_T("CAnomalyHelper::GetAnomalyCount() - The anomaly list is null."));
        return 0;
    }

    // Get the anomaly count
    size_t nAnomalyCount = 0;
    ccEraser::eResult eRes = pAnomalyList->GetCount(nAnomalyCount);

    if( ccEraser::Failed(eRes) )
    {
        CCTRACEE(_T("CAnomalyHelper::GetAnomalyCount() - GetCount for the anomaly list failed. eResult = %d"), eRes);
        return 0;
    }

    return nAnomalyCount;
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
UINT CAnomalyHelper::GetStatusVerb(CommonUIInfectionStatus eStatus, bool bViral)
{
    switch (eStatus )
    {
    case InfectionStatus_Unhandled:
        if( bViral )
            return IDS_INFECTED;
        else
            return IDS_THREAT_INFECTED_STATUS;
    case InfectionStatus_Quarantined:
        return IDS_QUARANTINED;
    case InfectionStatus_Quarantine_Failed:
        return IDS_QUARANTINE_FAILED;
    case InfectionStatus_Repaired:
        return IDS_REPAIRED;
    case InfectionStatus_Repair_Failed:
        return IDS_REPAIR_FAILED;
    case InfectionStatus_Deleted:
        return IDS_REMOVED;
    case InfectionStatus_Delete_Failed:
        return IDS_REMOVE_FAILED;
    case InfectionStatus_Excluded:
        return IDS_EXCLUDED;
    case InfectionStatus_Exclude_Failed:
        return IDS_EXCLUDE_FAILED;
    }

    CCTRACEE(_T("CAnomalyHelper::GetStatusVerb() - Unknown status value = %d."), eStatus);

    if( bViral )
        return IDS_INFECTED;
    else
        return IDS_THREAT_INFECTED_STATUS;
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
UINT CAnomalyHelper::GetActionToTakeVerb(AnomalyAction eAction)
{
    switch (eAction)
    {
    case ACTION_REPAIR:
        return IDS_ACTION_REPAIR;
    case ACTION_QUARANTINE:
        return IDS_ACTION_QUARANTINE;
    case ACTION_REPAIRTHENQUARANTINE:
        return IDS_ACTION_REPTHENQUAR;
    case ACTION_DELETE:
        return IDS_ACTION_REMOVE;
    case ACTION_REPAIRTHENDELETE:
        return IDS_ACTION_REPTHENDELETE;
    case ACTION_EXCLUDE:
        return IDS_ACTION_EXCLUDE;
    case ACTION_IGNORE:
        return IDS_ACTION_IGNORE;
    case ACTION_COMPLETE:
        return IDS_ACTION_IGNORE;
    case ACTION_NOOPERATIONSAVAILABLE:
        return IDS_ACTION_IGNORE;
    case ACTION_REVIEW:
        return IDS_ACTION_REVIEW;
    }

    CCTRACEE(_T("CAnomalyHelper::GetActionToTakeVerb() - Unknown action to take = %d."), eAction);
    return IDS_ACTION_IGNORE;
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
size_t
CAnomalyHelper::GetRemainingManualRemovalCount(ccEraser::IAnomalyList
                                                                * pAnomalyList)
{
    if( pAnomalyList == NULL )
    {
        CCTRACEE(_T("CAnomalyHelper::GetRemainingManaulRemovalCount() - The anomaly list is null."));
        return 0;
    }

    // Get the anomaly count
    size_t nAnomalyCount = 0;
    ccEraser::eResult eRes = pAnomalyList->GetCount(nAnomalyCount);

    if( ccEraser::Failed(eRes) )
    {
        CCTRACEE(_T("CAnomalyHelper::GetRemainingManualRemovalCount() - GetCount for the anomaly list failed. eResult = %d"), eRes);
        return 0;
    }

    if( nAnomalyCount == 0 )
    {
        // That was easy we don't have any anomalies in the list
        CCTRACEI(_T("CAnomalyHelper::GetRemianingManualRemovalCount() - There are no anomalies in the list"));
        return 0;
    }

    size_t nRemainingManualRemovalCount = 0;

    // Filter through each anomaly
    for( size_t nCur = 0; nCur < nAnomalyCount; nCur++ )
    {
        ccEraser::IAnomalyPtr pAnomaly;
        eRes = pAnomalyList->GetItem(nCur, pAnomaly);

        if( ccEraser::Succeeded(eRes) )
        {
            CEZAnomaly ezAnom(pAnomaly);
            AnomalyAction eNextAction;
            CommonUIInfectionStatus eStatus;

            if( !ezAnom.GetAnomalyStatus(eNextAction, eStatus) )
            {
                CCTRACEE(_T("CAnomalyHelper::GetRemainingManualRemovalCount() - Failed to get the status values for item %d"), nCur);
                continue;
            }

            if( ACTION_REVIEW == eNextAction ) 
            {
                nRemainingManualRemovalCount++;
            }
        }
        else
        {
            CCTRACEE(_T("CAnomalyHelper::GetRemainingManualRemovalCount() - Failed to get anomaly %d from the list. eResult = %d"), nCur, eRes);
        }
    }

    return nRemainingManualRemovalCount;
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
bool 
CAnomalyHelper::UnhandledAnomaliesRemain(ccEraser::IAnomalyList* pAnomalyList, 
                                         const SMRESPONSE smrResponseMode )
{
    if( pAnomalyList == NULL )
    {
        CCTRACEE(_T("CAnomalyHelper::UnhandledAnomaliesRemain() - The anomaly list is null."));
        return false;
    }

    // Get the anomaly count
    size_t nAnomalyCount = 0;
    ccEraser::eResult eRes = pAnomalyList->GetCount(nAnomalyCount);

    if( ccEraser::Failed(eRes) )
    {
        CCTRACEE(_T("CAnomalyHelper::UnhandledAnomaliesRemain() - GetCount for the anomaly list failed. eResult = %d"), eRes);
        return false;
    }

    if( nAnomalyCount == 0 )
    {
        // That was easy we don't have any anomalies in the list
        CCTRACEI(_T("CAnomalyHelper::UnhandledAnomaliesRemain() - There are no anomalies in the list"));
        return false;
    }

    // Filter through each anomaly
    for( size_t nCur = 0; nCur < nAnomalyCount; nCur++ )
    {
        ccEraser::IAnomalyPtr pAnomaly;
        eRes = pAnomalyList->GetItem(nCur, pAnomaly);
        if( ccEraser::Succeeded(eRes) )
        {
            CEZAnomaly ezAnom(pAnomaly);
            AnomalyAction eNextAction;
            CommonUIInfectionStatus eStatus;
            if( !ezAnom.GetAnomalyStatus(eNextAction, eStatus) )
            {
                CCTRACEE(_T("CAnomalyHelper::UnhandledAnomaliesRemain() - Failed to get the status values for item %d"), nCur);
                continue;
            }

            // Continue looking if action is complete, or, when in 
            // auto-processing mode, if action is review or no operations
            // are available.  Otherwise, this is an anomaly that needs
            // processing.
            if ( ACTION_COMPLETE == eNextAction || 
                 ( SCAN_MODE_MANUAL != smrResponseMode && 
                   ( ACTION_NOOPERATIONSAVAILABLE == eNextAction ||
                     ACTION_REVIEW == eNextAction ) ) )
            {
                continue;
            }
            else
            {
                return true;
            }
        }
        else
        {
            CCTRACEE(_T("CAnomalyHelper::UnhandledAnomaliesRemain() - Failed to get anomaly %d from the list. eResult = %d"), nCur, eRes);
        }
    }

    return false;
}

void CAnomalyHelper::ProcessReviewAndIgnoreItems(ccEraser::IAnomalyList* pAnomalyList,
                                                        bool bProcessIgnore, long& lIgnoreItemsProcessed,
                                                        bool bProcessReview, long& lReviewItemsProcessed)
{
    // Initialize output values
    lIgnoreItemsProcessed = 0;
    lReviewItemsProcessed = 0;

    if( pAnomalyList == NULL )
    {
        CCTRACEE(_T("CAnomalyHelper::ProcessReviewAndIgnoreItems() - The anomaly list is null."));
        return;
    }

    // Get the anomaly count
    size_t nAnomalyCount = 0;
    ccEraser::eResult eRes = pAnomalyList->GetCount(nAnomalyCount);

    if( ccEraser::Failed(eRes) )
    {
        CCTRACEE(_T("CAnomalyHelper::ProcessReviewAndIgnoreItems() - GetCount for the anomaly list failed. eResult = %d"), eRes);
        return;
    }

    if( nAnomalyCount == 0 )
    {
        // That was easy we don't have any anomalies in the list
        CCTRACEI(_T("CAnomalyHelper::ProcessReviewAndIgnoreItems() - There are no anomalies in the list to process"));
        return;
    }

    // Filter through each anomaly
    for( size_t nCur = 0; nCur < nAnomalyCount; nCur++ )
    {
        ccEraser::IAnomalyPtr pAnomaly;
        eRes = pAnomalyList->GetItem(nCur, pAnomaly);
        if( ccEraser::Succeeded(eRes) )
        {
            CEZAnomaly ezAnom(pAnomaly);
            AnomalyAction eNextAction;
            CommonUIInfectionStatus eStatus;
            if( !ezAnom.GetAnomalyStatus(eNextAction, eStatus) )
            {
                CCTRACEE(_T("CAnomalyHelper::ProcessReviewAndIgnoreItems() - Failed to get the status values for item %d"), nCur);
                continue;
            }

            //
            // Ignore item processing
            //
            if( bProcessIgnore )
            {
                // If this is a low non-viral threat that is being ignored we want to
                // set it to resolved, also if it is a medium-high damage non-viral threat
                // that is manually being set to ignore we want to make it resolved.
                if( ezAnom.IsNonViral() && eNextAction == ACTION_IGNORE )
                {
                    if( ezAnom.GetDamageFlagAverage() < 2 )
                    {
                        // This is a low ranked non-viral threat and it's being ignored, set it to resolved
                        CCTRACEI(_T("CAnomalyHelper::ProcessReviewAndIgnoreItems() - Low ranked non-viral threat with action to ignore, setting to resolved."));
                        ezAnom.SetAnomalyStatus(ACTION_COMPLETE, InfectionStatus_Unhandled);
                        lIgnoreItemsProcessed++;
                    }
                    else
                    {
                        // This is a high/med. ranked non-viral threat and it's being ignored, if the state is still unhandled then
                        // we set this to resolved
                        if( eStatus == InfectionStatus_Unhandled )
                        {
                            CCTRACEI(_T("CAnomalyHelper::ProcessReviewAndIgnoreItems() - Medium or High ranked non-viral threat with action manually changed to ignore, setting to resolved."));
                            ezAnom.SetAnomalyStatus(ACTION_COMPLETE, InfectionStatus_Unhandled);
                            lIgnoreItemsProcessed++;
                        }
                    }
                }
            }

            //
            // Review item processing
            //
            if( bProcessReview )
            {
                if( eNextAction == ACTION_REVIEW )
                {
                    // Set review items to ignored
                    ezAnom.SetAnomalyStatus(ACTION_IGNORE, eStatus);
                    lReviewItemsProcessed++;
                }
            }
        }
        else
        {
            CCTRACEE(_T("CAnomalyHelper::ProcessReviewAndIgnoreItems() - Failed to get anomaly %d from the list. eResult = %d"), nCur, eRes);
        }
    }
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
bool CAnomalyHelper::AnomaliesNeedProcessing(ccEraser::IAnomalyList* pAnomalyList)
{
    if( pAnomalyList == NULL )
    {
        CCTRACEE(_T("CAnomalyHelper::AnomaliesNeedProcessing() - The anomaly list is null."));
        return false;
    }

    // Get the anomaly count
    size_t nAnomalyCount = 0;
    ccEraser::eResult eRes = pAnomalyList->GetCount(nAnomalyCount);

    if( ccEraser::Failed(eRes) )
    {
        CCTRACEE(_T("CAnomalyHelper::AnomaliesNeedProcessing() - GetCount for the anomaly list failed. eResult = %d"), eRes);
        return false;
    }

    if( nAnomalyCount == 0 )
    {
        // That was easy we don't have any anomalies in the list
        CCTRACEI(_T("CAnomalyHelper::AnomaliesNeedProcessing() - There are no anomalies in the list"));
        return false;
    }

    // Filter through each anomaly
    for( size_t nCur = 0; nCur < nAnomalyCount; nCur++ )
    {
        ccEraser::IAnomalyPtr pAnomaly;
        eRes = pAnomalyList->GetItem(nCur, pAnomaly);
        if( ccEraser::Succeeded(eRes) )
        {
            CEZAnomaly ezAnom(pAnomaly);
            AnomalyAction eNextAction;
            CommonUIInfectionStatus eStatus;
            if( !ezAnom.GetAnomalyStatus(eNextAction, eStatus) )
            {
                CCTRACEE(_T("CAnomalyHelper::AnomaliesNeedProcessing() - Failed to get the status values for item %d"), nCur);
                continue;
            }

            // If this item needs action taken then return true
            if( AnomalyNeedsAction(eNextAction) )
            {
                return true;
            }
            else
            {
                continue;
            }
        }
        else
        {
            CCTRACEE(_T("CAnomalyHelper::AnomaliesNeedProcessing() - Failed to get anomaly %d from the list. eResult = %d"), nCur, eRes);
        }
    }

    return false;
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
ANOMALYLISTSTATE CAnomalyHelper::AnomalyListState(ccEraser::IAnomalyList* pAnomalyList)
{
    if( pAnomalyList == NULL )
    {
        CCTRACEE(_T("CAnomalyHelper::AnomalyListState() - The anomaly list is null."));
        return LISTSTATE_BLUE_RESOLVED;
    }

    // Get the anomaly count
    size_t nAnomalyCount = 0;
    ccEraser::eResult eRes = pAnomalyList->GetCount(nAnomalyCount);

    if( ccEraser::Failed(eRes) )
    {
        CCTRACEE(_T("CAnomalyHelper::AnomalyListState() - GetCount for the anomaly list failed. eResult = %d"), eRes);
        return LISTSTATE_BLUE_RESOLVED;
    }

    if( nAnomalyCount == 0 )
    {
        // That was easy we don't have any anomalies in the list
        CCTRACEI(_T("CAnomalyHelper::AnomalyListState() - There are no anomalies in the list"));
        return LISTSTATE_GREEN;
    }

    ANOMALYLISTSTATE retState = LISTSTATE_GREEN;

    // Filter through each anomaly
    for( size_t nCur = 0; nCur < nAnomalyCount; nCur++ )
    {
        ccEraser::IAnomalyPtr pAnomaly;
        eRes = pAnomalyList->GetItem(nCur, pAnomaly);
        if( ccEraser::Succeeded(eRes) )
        {
            CEZAnomaly ezAnom(pAnomaly);
            AnomalyAction eNextAction;
            CommonUIInfectionStatus eStatus;
            if( !ezAnom.GetAnomalyStatus(eNextAction, eStatus) )
            {
                CCTRACEE(_T("CAnomalyHelper::AnomalyListState() - Failed to get the status values for item %d"), nCur);
                continue;
            }

            // Check for blue or red state cases
            if( eNextAction == ACTION_COMPLETE || eNextAction == ACTION_NOOPERATIONSAVAILABLE )
            {
                // Is this a failure value?
                if( InfectionStatusFailure(eStatus) )
                {
                    // If this is a virus then we are in Red state
                    if( ezAnom.IsViral() )
                    {
                        // We are in red state so that's it
                        return LISTSTATE_RED;
                    }
                    else if( ezAnom.IsNonViral() )
                    {
                        // If this is a low non-viral threat that was ignored, then don't set
                        // to blue...cause it's all good to do that
                        if( ezAnom.GetDamageFlagAverage() < 2 && eStatus == InfectionStatus_Unhandled )
                            continue;

                        // If this is COMPLETE and unhandled then it is considered resolved,
                        // but we are still in the blue state, otherwise it is blue and remaining
                        if( eStatus == InfectionStatus_Unhandled && eNextAction == ACTION_COMPLETE )
                            retState = LISTSTATE_BLUE_RESOLVED;
                        else
                            retState = LISTSTATE_BLUE_REMAINING;
                    }
                }
            }
            else
            {
                // This item can still have actions taken on it
                // If this is a virus then we are in Red state
                if( ezAnom.IsViral() )
                {
                    // We are in red state so that's it
                    return LISTSTATE_RED;
                }
                else if( ezAnom.IsNonViral() )
                    retState = LISTSTATE_BLUE_REMAINING;
            }
        }
        else
        {
            CCTRACEE(_T("CAnomalyHelper::AnomalyListState() - Failed to get anomaly %d from the list. eResult = %d"), nCur, eRes);
        }
    }

    CCTRACEI(_T("CAnomalyHelper::AnomalyListState() - returning %d"), retState);
    return retState;
}

bool CAnomalyHelper::NeedReboot(ccEraser::IAnomalyList* pAnomalyList)
{
    if( pAnomalyList == NULL )
    {
        CCTRACEE(_T("CAnomalyHelper::NeedReboot() - The anomaly list is null."));
        return false;
    }

    // Get the anomaly count
    size_t nAnomalyCount = 0;
    ccEraser::eResult eRes = pAnomalyList->GetCount(nAnomalyCount);

    if( ccEraser::Failed(eRes) )
    {
        CCTRACEE(_T("CAnomalyHelper::NeedReboot() - GetCount for the anomaly list failed. eResult = %d"), eRes);
        return false;
    }

    if( nAnomalyCount == 0 )
    {
        // That was easy we don't have any anomalies in the list
        CCTRACEI(_T("CAnomalyHelper::NeedReboot() - There are no anomalies in the list"));
        return false;
    }

    // Filter through each anomaly
    for( size_t nCur = 0; nCur < nAnomalyCount; nCur++ )
    {
        ccEraser::IAnomalyPtr pAnomaly;
        eRes = pAnomalyList->GetItem(nCur, pAnomaly);
        if( ccEraser::Succeeded(eRes) )
        {
            CEZAnomaly ezAnom(pAnomaly);
            if( ezAnom.RequiresReboot() )
            {
                CCTRACEI(_T("CAnomalyHelper::NeedReboot() - This anomaly requires a reboot."));
                return true;
            }
        }
        else
        {
            CCTRACEE(_T("CAnomalyHelper::NoAnomaliesLeftToProcess() - Failed to get anomaly %d from the list. eResult = %d"), nCur, eRes);
        }
    }

    return false;
}

long 
CAnomalyHelper::GetUnprocessedItemCount(ccEraser::IAnomalyList* pAnomalyList)
{
    if( pAnomalyList == NULL )
    {
        CCTRACEE(_T("CAnomalyHelper::GetUnprocessedItemCount() - The anomaly list is null."));
        return 0;
    }

    // Get the anomaly count
    size_t nAnomalyCount = 0;
    ccEraser::eResult eRes = pAnomalyList->GetCount(nAnomalyCount);

    if( ccEraser::Failed(eRes) )
    {
        CCTRACEE(_T("CAnomalyHelper::GetUnprocessedItemCount() - GetCount for the anomaly list failed. eResult = %d"), eRes);
        return 0;
    }

    if( nAnomalyCount == 0 )
    {
        // That was easy we don't have any anomalies in the list
        CCTRACEI(_T("CAnomalyHelper::GetUnprocessedItemCount() - There are no anomalies in the list"));
        return 0;
    }

    long lRemainingCount = 0;

    // Filter through each anomaly
    for( size_t nCur = 0; nCur < nAnomalyCount; nCur++ )
    {
        ccEraser::IAnomalyPtr pAnomaly;
        eRes = pAnomalyList->GetItem(nCur, pAnomaly);
        if( ccEraser::Succeeded(eRes) )
        {
            CEZAnomaly ezAnom(pAnomaly);

            AnomalyAction eNextAction;
            CommonUIInfectionStatus eStatus;

            if( !ezAnom.GetAnomalyStatus(eNextAction, eStatus) )
            {
                CCTRACEE(_T("CAnomalyHelper::GetUnprocessedItemCount() - Failed to get the status values for item %d"), nCur);
                continue;
            }

            if ( InfectionStatusFailure(eStatus) &&
                 ACTION_COMPLETE != eNextAction &&
                 ACTION_NOOPERATIONSAVAILABLE != eNextAction )
            {
                lRemainingCount++;
            }
        }
        else
        {
            CCTRACEE(_T("CAnomalyHelper::GetUnprocessedItemCount() - Failed to get anomaly %d from the list. eResult = %d"), nCur, eRes);
        }
    }

    return lRemainingCount;
}

bool CAnomalyHelper::ExcludeRecommendationAccepted(
                                          ccEraser::IAnomalyList* pAnomalyList)
{
    if( pAnomalyList == NULL )
    {
        CCTRACEE(_T("CAnomalyHelper::ExcludeRecommendationAccepted() - The anomaly list is null."));
        return false;
    }

    // Get the anomaly count
    size_t nAnomalyCount = 0;
    ccEraser::eResult eRes = pAnomalyList->GetCount(nAnomalyCount);

    if( ccEraser::Failed(eRes) )
    {
        CCTRACEE(_T("CAnomalyHelper::ExcludeRecommendationAccepted() - GetCount for the anomaly list failed. eResult = %d"), eRes);
        return false;
    }

    if( nAnomalyCount == 0 )
    {
        // That was easy we don't have any anomalies in the list
        CCTRACEI(_T("CAnomalyHelper::ExcludeRecommendationAccepted() - There are no anomalies in the list"));
        return false;
    }

    // Filter through each anomaly
    for( size_t nCur = 0; nCur < nAnomalyCount; nCur++ )
    {
        ccEraser::IAnomalyPtr pAnomaly;
        eRes = pAnomalyList->GetItem(nCur, pAnomaly);

        if( ccEraser::Succeeded(eRes) )
        {
            CEZAnomaly ezAnom(pAnomaly);

            AnomalyAction eNextAction, eRecommended;
            CommonUIInfectionStatus eStatus;

            if( !ezAnom.GetAnomalyStatus(eNextAction, eStatus) )
            {
                CCTRACEE(_T("CAnomalyHelper::ExcludeRecommendationAccepted() - Failed to get the status values for item %d"), nCur);
                continue;
            }

            if( !ezAnom.GetRecommendedAction(eRecommended) )
            {
                CCTRACEE(_T("CAnomalyHelper::ExcludeRecommendationAccepted() - Failed to get the recommended action for item %d"), nCur);
                continue;
            }

            if( ACTION_EXCLUDE == eNextAction && 
                ACTION_EXCLUDE == eRecommended )
            {
                return true;
            }
        }
        else
        {
            CCTRACEE(_T("CAnomalyHelper::ExcludeRecommendationAccepted() - Failed to get anomaly %d from the list. eResult = %d"), nCur, eRes);
        }
    }

    return false;
}
