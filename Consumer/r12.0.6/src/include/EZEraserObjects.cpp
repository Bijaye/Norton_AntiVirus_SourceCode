#include "stdafx.h"
#include "EZEraserObjects.h"
#include "ccSymKeyValueCollectionImpl.h"
#include "ccSymStringImpl.h"
#include <math.h>

///////////////////////////////////////////////////////////////////////////
// CEZAnomaly implementation
CEZAnomaly::CEZAnomaly(void) :
    m_bInitialized(false),
    m_bRepairOnly(false)
{
}

CEZAnomaly::~CEZAnomaly(void)
{
}

CEZAnomaly::CEZAnomaly(ccEraser::IAnomaly* pAnomaly)
{
    if( !Initialize(pAnomaly) )
    {
        CCTRACEE(_T("CEZAnomaly::CEZAnomaly() - Initalize failed."));
    }
}

void CEZAnomaly::Reset()
{
    m_spAnomalyProps.Release();
    m_spCategories.Release();
    m_spUserData.Release();
    m_spRemediationList.Release();
    m_spAnomaly.Release();

    m_bInitialized = false;
    m_bRepairOnly = false;
}

bool CEZAnomaly::Initialize(ccEraser::IAnomaly* pAnomaly)
{
    if( pAnomaly == NULL )
    {
        CCTRACEE(_T("CEZAnomaly::Initialize() - Anomaly is null."));
        return false;
    }

    // Reset any existing data
    Reset();

    m_spAnomaly = pAnomaly;

    if( !Initialize() )
    {
        return false;
    }

    return true;
}

bool CEZAnomaly::Initialize()
{
    if ( m_bInitialized )
	{
        return true;
	}

    // Validate anomaly
    if ( m_spAnomaly == NULL )
    {
        CCTRACEE(_T("CEZAnomaly::Initialize() - Internal anomaly pointer is null."));
        return false;
    }

    ccEraser::eResult eRes = ccEraser::Success;

    // Get the properties
    if ( NULL == m_spAnomalyProps.m_p )
    {
        eRes = m_spAnomaly->GetProperties(m_spAnomalyProps);
        if ( ccEraser::Failed(eRes) || m_spAnomalyProps == NULL )
        {
            CCTRACEE(_T("CEZAnomaly::Initialize() - Failed to get the properties. eResult = %d"), eRes);
            return false;
        }
    }

    // Create the user data if necessary
    if ( !CreateUserData() )
    {
        CCTRACEE(_T("CEZAnomaly::Initialize() - Failed to create the user data."));
        return false;
    }

    // Get the user data
    if ( NULL == m_spUserData.m_p )
    {
        ISymBasePtr pSymBase;
        if ( !m_spAnomalyProps->GetValue(ccEraser::IAnomaly::UserData, pSymBase) )
        {
            CCTRACEE(_T("CEZAnomaly::Initialize() - Failed to get the user data symbase value."));
            return false;
        }

        if ( SYM_FAILED(pSymBase->QueryInterface(cc::IID_KeyValueCollection, reinterpret_cast<void**>(&m_spUserData.m_p))) )
        {
            CCTRACEE(_T("CEZAnomaly::Initialize() - Failed to QI for the user data key value collection."));
            return false;
        }
    }

    // Get the anomaly categories
    if ( NULL == m_spCategories.m_p )
    {
        ISymBasePtr pSymBase;
        if ( !m_spAnomalyProps->GetValue(ccEraser::IAnomaly::Categories, pSymBase) )
        {
            CCTRACEE(_T("CEZAnomaly::GetCategories() - Failed to get the categories symbase value."));
            return false;
        }

        // QI for the categories
        if ( SYM_FAILED(pSymBase->QueryInterface(cc::IID_IndexValueCollection, 
                                                 reinterpret_cast<void**>(&m_spCategories.m_p))) )
        {
            CCTRACEE(_T("CEZAnomaly::GetCategories() - Failed to QI for the categories index value collection."));
            return false;
        }
    }

    m_bInitialized = true;

    return true;
}

cc::IKeyValueCollection* CEZAnomaly::GetProperties()
{
    if ( !m_bInitialized && !Initialize() )
    {
        CCTRACEE(_T("CEZAnomaly::GetProperties() - Failed to initialize."));
        return NULL;
    }

    return m_spAnomalyProps.m_p;
}

ccEraser::IRemediationActionList* CEZAnomaly::GetRemediations()
{
    if ( !m_bInitialized && !Initialize() )
    {
        CCTRACEE(_T("CEZAnomaly::GetRemediations() - Failed to initialize."));
        return NULL;
    }

    // Get the remediation action list
    if ( NULL == m_spRemediationList.m_p )
    {
        ccEraser::eResult eRes = 
                      m_spAnomaly->GetRemediationActions(m_spRemediationList);

        if ( ccEraser::Failed( eRes ) )
        {
            CCTRACEE(_T("CEZAnomaly::GetRemediations() - Failed to get remediation list. eResult = %d."), eRes);
            return NULL;
        }
    }

    return m_spRemediationList.m_p;
}

bool CEZAnomaly::CreateUserData()
{
    if ( m_spAnomalyProps.m_p == NULL )
    {
        CCTRACEE(_T("CEZAnomaly::CreateUserData() - Properties is NULL."));
        return false;
    }

    if ( m_spAnomalyProps->GetExists(ccEraser::IAnomaly::UserData) )
    {
        // User data already exists
        return true;
    }
    else
    {
        cc::IKeyValueCollectionPtr pUserData;

        pUserData.Attach(ccSym::CKeyValueCollectionImpl::
                                              CreateKeyValueCollectionImpl());

        if ( !pUserData )
        {
            CCTRACEE(_T("CEZAnomaly::CreateUserData() - Failed to create user data key value collection"));
            return false;
        }

        // Store this in the anomaly
        if ( !m_spAnomalyProps->SetValue(ccEraser::IAnomaly::UserData, 
                                         pUserData) )
        {
            CCTRACEE(_T("CEZAnomaly::CreateUserData() - Failed to set the user data property."));
            return false;
        }
    }

    return true;
}

// User data routine implementation
cc::IKeyValueCollection* CEZAnomaly::GetUserData()
{
    if( !m_bInitialized && !Initialize() )
    {
        CCTRACEE(_T("CEZAnomaly::GetUserData() - Failed to initialize."));
        return NULL;
    }

    return m_spUserData.m_p;
}

bool CEZAnomaly::IsGeneric()
{
    if( !m_bInitialized && !Initialize() )
    {
        CCTRACEE(_T("CEZAnomaly::IsGeneric() - Failed to initialize."));
        return false;
    }

    // Check the anomaly categories
    size_t nCatCount = m_spCategories->GetCount();
    for( size_t nCur = 0; nCur < nCatCount; nCur++ )
    {
        DWORD dwCategory = 0;
        if( !m_spCategories->GetValue(nCur, dwCategory) )
        {
            CCTRACEE(_T("CEZAnomaly::IsGenericAnomaly() - Error getting anomaly category %d."), nCur);
            continue;
        }

        // Is this the generic category?
        if( dwCategory == ccEraser::IAnomaly::GenericLoadPoint )
            return true;
    }

    return false;
}

bool CEZAnomaly::IsSpecific()
{
    if( !m_bInitialized && !Initialize() )
    {
        CCTRACEE(_T("CEZAnomaly::IsSpecific() - Failed to initialize."));
        return false;
    }

    // If the AutoCreated flag is true then this is not a specific anomaly
    bool bAutoCreated = true;
    if( m_spAnomalyProps->GetValue(ccEraser::IAnomaly::AutoCreated, bAutoCreated) && bAutoCreated == true)
    {
        CCTRACEI(_T("CEZAnomaly::IsSpecific() - The AutoCreated flag is set to true. This is a NOT specific anomaly."));
        return false;
    }
    else
    {
        CCTRACEI(_T("CEZAnomaly::IsSpecific() - The AutoCreated flag is false or doesn't exist. This IS a specific anomaly."));
        return true;
    }
}

bool CEZAnomaly::ProfileMemoryItems()
{
    if( !m_bInitialized && !Initialize() )
    {
        CCTRACEE(_T("CEZAnomaly::ProfileMemoryItems() - Failed to initialize."));
        return false;
    }

    if( !GetRemediations() )
    {
        CCTRACEE(_T("CEZAnomaly::ProfileMemoryItems() - Failed to get remediations."));
        return false;
    }

    size_t nCount = 0;
    ccEraser::eResult eRes = m_spRemediationList->GetCount(nCount);
    if( ccEraser::Failed(eRes) )
    {
        CCTRACEE(_T("CEZAnomaly::ProfileMemoryItems() - Failed to get the remediation action list count. eResult = %d"), eRes);
        return false;
    }

    bool bRet = false;

    for( size_t nCur=0; nCur<nCount; nCur++ )
    {
        ccEraser::IRemediationActionPtr pRem;
        if( ccEraser::Failed(m_spRemediationList->GetItem(nCur, pRem)) || !pRem )
        {
            CCTRACEE(_T("CEZAnomaly::ProfileMemoryItems() - Failed to get remediation action %d."), nCur);
            continue;
        }

        CEZRemediation ezRem(pRem);

        ccEraser::eObjectType type = ezRem.GetType();
        ccEraser::IRemediationAction::Operation_Type opType = ezRem.GetOperationType();

        // Only check each process or service once, so check on the 
        // suspend operation for processes and on the Stop operation for 
        // services
        if ( (type == ccEraser::ProcessRemediationActionType && 
              opType == ccEraser::IRemediationAction::Suspend)
             || 
             (type == ccEraser::ServiceRemediationActionType && 
              opType == ccEraser::IRemediationAction::Stop) )
        {
            bool bIsPresent = false;

            if( ccEraser::Failed(eRes = pRem->IsPresent(bIsPresent)) )
            {
                CCTRACEE(_T("CEZAnomaly::ProfileMemoryItems() - Failed to get the remediation action presence for item %d. eResult = %d"), nCur, eRes);
                continue;
            }

            if( bIsPresent )
            {
                bRet = true;
                ezRem.SetActiveInMemory(true);

                // If this is a process remediation we also need to mark the 
                // Terminate remediation action as being present
                if( type == ccEraser::ProcessRemediationActionType )
                {
                    // Get the process ID and path so we can find the 
                    // corresponding terminate remediation action and mark it 
                    // present as well
                    DWORD dwProcID = 0;
                    cc::IStringPtr spStrOrigProc;
                    bool bHaveProcID = ezRem.GetProcessRemediationID(dwProcID);
                    bool bHaveProcPath = ezRem.GetPathProperty(spStrOrigProc);

                    CCTRACEI(_T("CEZAnomaly::ProfileMemoryItems() - Found an active Process suspend memory item. Will attempt to find the terminate. Proc ID = %lu Proc Name = %s"), dwProcID, spStrOrigProc ? spStrOrigProc->GetStringA() : "No Process Path");

                    if( !bHaveProcID && !bHaveProcPath )
                    {
                        CCTRACEE(_T("CEZAnomaly::ProfileMemoryItems() - Failed to get a path or ID for the suspend process operation."));
                        continue;
                    }

                    // Search through the remaining remediation actions
                    for( size_t nCurTerm = nCur+1; nCurTerm < nCount; nCurTerm++ )
                    {
                        ccEraser::IRemediationActionPtr pRemProcTerm;
                        if( ccEraser::Failed(m_spRemediationList->GetItem(nCurTerm, pRemProcTerm)) || !pRemProcTerm )
                        {
                            CCTRACEE(_T("CEZAnomaly::ProfileMemoryItems() - Failed to get the remediation (number %d) after the process suspend."), nCurTerm);
                            continue;
                        }

                        CEZRemediation ezProcTermRem(pRemProcTerm);
                        type = ezProcTermRem.GetType();
                        if( type != ccEraser::ProcessRemediationActionType )
                        {
                            continue;
                        }

                        opType = ezProcTermRem.GetOperationType();
                        if( opType != ccEraser::IRemediationAction::Terminate )
                        {
                            continue;
                        }

                        // We have a process terminate remediation action.
                        // Compare the data.
                        if ( bHaveProcID )
                        {
                            DWORD dwProcTermID = 0;

                            if ( ezProcTermRem.GetProcessRemediationID(dwProcTermID) && dwProcID == dwProcTermID )
                            {
                                ezProcTermRem.SetActiveInMemory(true);
                                CCTRACEI(_T("CEZAnomaly::ProfileMemoryItems() - Found the corresponding process terminate remediation for process %d."), dwProcTermID);
                                break;
                            }

                            continue;
                        }

                        //
                        // Suspend that we are trying to match has no
                        // process ID.  Declare match if the terminate
                        // also has no process ID and it has a matching
                        // path.  (Do not break from the for loop, so 
                        // eventually *all* terminates with that criteria
                        // will be set as active.  Necessary because 
                        // there could be multiple instances.)
                        //
                        if ( bHaveProcPath && spStrOrigProc )
                        {
                            cc::IStringPtr spStrProc;
                            DWORD dwProcTermID;

                            if ( !ezProcTermRem.GetProcessRemediationID(dwProcTermID) &&
                                 ezProcTermRem.GetPathProperty(spStrProc) && 
                                 spStrProc &&
                                 0 == _tcsicmp(spStrOrigProc->GetStringA(), 
                                               spStrProc->GetStringA()) )
                            {
                                ezProcTermRem.SetActiveInMemory(true);
                                CCTRACEI(_T("CEZAnomaly::ProfileMemoryItems() - Found the corresponding process terminate remediation for process with path %s."), spStrProc->GetStringA());
                            }
                        }
                    }
                }
                else
                {
                    CCTRACEI(_T("CEZAnomaly::ProfileMemoryItems() - Found an active Service stop memory item."));
                }
            }
            else
                CCTRACEI(_T("CEZAnomaly::ProfileMemoryItems() - Remeidation not present."));
        }
    }

    return bRet;
}

bool CEZAnomaly::SetRequiresReboot()
{
    if( !m_bInitialized && !Initialize() )
    {
        CCTRACEE(_T("CEZAnomaly::SetRequiresReboot() - Failed to initialize."));
        return false;
    }

    // Set the requires reboot value
    if( !m_spUserData->SetValue(AnomalyRequiresReboot, true) )
    {
        CCTRACEE(_T("CEZAnomaly::SetRequiresReboot() - Failed to set anomaly requires a reboot."));
        return false;
    }

    return true;
}

bool CEZAnomaly::RequiresReboot()
{
    if( !m_bInitialized && !Initialize() )
    {
        CCTRACEE(_T("CEZAnomaly::RequiresReboot() - Failed to initialize."));
        return false;
    }

    // Get the reboot value
    bool bReboot = false;
    if( m_spUserData->GetValue(AnomalyRequiresReboot, bReboot) && bReboot == true )
    {
        CCTRACEI(_T("CEZAnomaly::RequiresReboot() - This anomaly requires a reboot."));
        return true;
    }

    return false;
}

bool CEZAnomaly::KnownToHaveDependencies()
{
    if( !m_bInitialized && !Initialize() )
    {
        CCTRACEE(_T("CEZAnomaly::KnownToHaveDependencies() - Failed to initialize."));
        return false;
    }

    DWORD dwDependency = 0;
    if( !m_spAnomalyProps->GetValue(ccEraser::IAnomaly::Dependency, dwDependency) )
    {
        // If the flag does not exist it is the same as not having dependencies
        return false;
    }

    return (dwDependency != 0);
}

DWORD CEZAnomaly::GetDamageFlagValue(DWORD dwFlag)
{
    if( !m_bInitialized && !Initialize() )
    {
        CCTRACEE(_T("CEZAnomaly::GetDamageFlagValue() - Failed to initialize."));
        return 0;
    }

    // Get the Flag
    DWORD dwValue = 0;
    if( !m_spAnomalyProps->GetValue(dwFlag, dwValue) )
    {
        dwValue = GetDefaultDamageFlagValue();
    }

    // We are expecting a value between 0 and 5, so if it's greater than 5 just treat
    // it as a 5
    if( dwValue > 5 )
    {
        CCTRACEW(_T("CEZAnomaly::GetDamageFlagValue(%d) - Damage value is %d, we are expecting a 0-5 value forcing it to 5."), dwFlag, dwValue);
        dwValue = 5;
    }

    return dwValue;
}

DWORD CEZAnomaly::GetDefaultDamageFlagValue()
{
    if( !m_bInitialized && !Initialize() )
    {
        CCTRACEE(_T("CEZAnomaly::GetDefaultDamageFlagValue() - Failed to initialize."));
        return 0;
    }

    if( IsViral() )
        return 5;
    else
    {
        // If there are any "high" level categories return a high value otherwise
        // return a medium value
        size_t nCatCount = m_spCategories->GetCount();
        for( size_t nCur = 0; nCur < nCatCount; nCur++ )
        {
            DWORD dwCategory = 0;
            if( !m_spCategories->GetValue(nCur, dwCategory) )
            {
                CCTRACEE(_T("CScanManager::IsNonViral() - Error getting anomaly category %d."), nCur);
                continue;
            }

            // Does this category match any high non-viral categories?
            if( static_cast<ccEraser::IAnomaly::Anomaly_Category>(dwCategory) ==  ccEraser::IAnomaly::SecurityRisk ||
                static_cast<ccEraser::IAnomaly::Anomaly_Category>(dwCategory) ==  ccEraser::IAnomaly::Hacktool ||
                static_cast<ccEraser::IAnomaly::Anomaly_Category>(dwCategory) ==  ccEraser::IAnomaly::SpyWare ||
                static_cast<ccEraser::IAnomaly::Anomaly_Category>(dwCategory) ==  ccEraser::IAnomaly::Dialer ||
                static_cast<ccEraser::IAnomaly::Anomaly_Category>(dwCategory) ==  ccEraser::IAnomaly::RemoteAccess ||
                static_cast<ccEraser::IAnomaly::Anomaly_Category>(dwCategory) ==  ccEraser::IAnomaly::Adware )
            {
                return 5;
            }
        }

        // This was not any of our high non-viral categories, return medium value
        return 3;
    }
}

DWORD CEZAnomaly::GetDamageFlagAverage()
{   
    if( !m_bInitialized && !Initialize() )
    {
        CCTRACEE(_T("CEZAnomaly::GetDamageFlagAverage() - Failed to initialize."));
        return 0;
    }

    float fTotal = 0;
    DWORD dwValue = 0;
    float fAvg = 0;
    float fValueCount = 0;

    // Grab the default in case we need it
    DWORD dwDefault = GetDefaultDamageFlagValue();

    // Stealth
    if( !m_spAnomalyProps->GetValue(ccEraser::IAnomaly::Stealth, dwValue) )
    {
        dwValue = dwDefault;
    }

    fValueCount++;
    fTotal += dwValue;

    // Removal
    if( !m_spAnomalyProps->GetValue(ccEraser::IAnomaly::Removal, dwValue) )
    {
        dwValue = dwDefault;
    }

    fValueCount++;
    fTotal += dwValue;

    // Performance
    if( !m_spAnomalyProps->GetValue(ccEraser::IAnomaly::Performance, dwValue) )
    {
        dwValue = dwDefault;
    }

    fValueCount++;
    fTotal += dwValue;

    // Privacy
    if( !m_spAnomalyProps->GetValue(ccEraser::IAnomaly::Privacy, dwValue) )
    {
        dwValue = dwDefault;
    }

    fValueCount++;
    fTotal += dwValue;

    if( fValueCount == 0 )
    {
        // This should never happen since we always fall back to the default, but adding it just
        // to uber-avoid dividing by 0.
        cc::IStringPtr pStrName;
        GetName(pStrName);
        CCTRACEW(_T("CEZAnomaly::GetDamageFlagAverage() - There were no damage flags available for threat %s. Using default of %d"), (pStrName!=NULL) ? pStrName->GetStringA() : "Unknown", dwDefault);

        return dwDefault;
    }

    fAvg = fTotal/fValueCount;

    // 0 - 2.49 = Low
    if (fAvg < 2.5)
        dwValue = 1;
    // 2.5 - 3.49 = Medium
    else if( fAvg < 3.5 )
        dwValue = 3;
    // 3.5 or greater = High
    else
        dwValue = 5;

    /* Find out if we want to return the floor or ceiling of this guy by checking the
    // fractional part of the value and rounding up or down
    float fIntegralPart = 0.0;
    float fFractionalPart = modf( fAvg, &fIntegralPart );

    if( fFractionalPart >= 0.5 )
        dwValue = static_cast<DWORD>(ceil(fAvg));
    else
        dwValue = static_cast<DWORD>(floor(fAvg));*/

    return dwValue;
}

bool CEZAnomaly::GetRecommendedAction(AnomalyAction& action)
{
    if ( !m_bInitialized && !Initialize() )
    {
        CCTRACEE(_T("CEZAnomaly::GetRecommendedAction() - Failed to initialize."));
        return false;
    }

    AnomalyAction            eNextAction;
    CommonUIInfectionStatus  eStatus;

    if ( !GetAnomalyStatus( eNextAction, eStatus ) )
    {
        CCTRACEE(_T("CEZAnomaly::GetRecommendedAction() - Failed to retrive current anomaly status; assuming unhandled."));
        eStatus = InfectionStatus_Unhandled;
    }

    if ( IsViral() ) 
    {
        switch (eStatus) 
        {
            case InfectionStatus_Unhandled:
                action = ACTION_REPAIR;

                break;

            case InfectionStatus_Repair_Failed:
                if ( IsBootRecordAnomaly() || IsMBRAnomaly() || 
                     IsRepairOnlyAnomaly() )
                {
                    action = ACTION_REVIEW;
                }
                else 
                {
                    action = ACTION_QUARANTINE;
                }

                break;

            case InfectionStatus_Quarantine_Failed:
                if ( IsBootRecordAnomaly() || IsMBRAnomaly() || 
                     IsRepairOnlyAnomaly() )
                {
                    // This should never happen
                    CCTRACEE(_T("Got quarantined failed status for anomaly that cannot be quarantined"));
                    action = ACTION_REVIEW;
                }
                else 
                {
                    action = ACTION_DELETE;
                }

                break;

            case InfectionStatus_Delete_Failed:
            case InfectionStatus_CantDelete:
                action = ACTION_REVIEW;

                break;

            case InfectionStatus_Exclude_Failed:
                action = ACTION_IGNORE;
                break;

            default:
                return false;

                break;
        }
    }
    else 
    {
        // Non-virals
       
        // Get the average
        DWORD dwAvg = GetDamageFlagAverage();

        // If the average value is Low (1 returned by the GetDamageFlagAverage 
        // method), we will recommend that they permanently ignore this item
        if ( dwAvg <= 1 )
        {
            if ( InfectionStatus_Exclude_Failed == eStatus) 
            {
                action = ACTION_IGNORE;
            }
            else 
            {
                action = ACTION_EXCLUDE;
            }
        }
        // If the average is 2 or greater we will recommend removal unless
        // we already know that we cannot delete or have failed at an earlier
        // attempt to delete.
        else if ( InfectionStatus_Delete_Failed == eStatus ||
                  InfectionStatus_Quarantine_Failed == eStatus ||
                  InfectionStatus_CantDelete == eStatus ) 
        {
            action = ACTION_REVIEW;
        }
        else  
        {
            action = ACTION_DELETE;
        }
    }

    return true;
}

bool CEZAnomaly::GetGenericDescription(cc::IString*& pSymStrDesc)
{
    if ( !m_bInitialized && !Initialize() )
    {
        CCTRACEE(_T("CEZAnomaly::GetGenericDescription() - Failed to initialize."));
        return false;
    }

    if ( !m_spUserData->GetValue(GenericDescription, 
                                 reinterpret_cast<ISymBase*&>(pSymStrDesc)) )
    {
        CCTRACEE(_T("CEZAnomaly::GetGenericDescription() - Failed to get description string from user data."));
        return false;
    }

    return true;
}

bool CEZAnomaly::IsSingleInfectionAnomaly()
{
    if ( !m_bInitialized && !Initialize() )
    {
        CCTRACEE(_T("CEZAnomaly::IsSingleInfectionAnomaly() - Failed to initialize."));
        return false;
    }

    bool bRet = false;
    if ( !m_spUserData->GetValue(SingleInfectionAnomaly, bRet) )
    {
        // If this is not set then this is not a single infection anomaly
        return false;
    }

    return bRet;
}

bool CEZAnomaly::SetSingleInfectionAnomaly(bool bVal)
{
    if ( !m_bInitialized && !Initialize() )
    {
        CCTRACEE(_T("CEZAnomaly::SetSingleInfectionAnomaly() - Failed to initialize."));
        return false;
    }

    // Set next action
    if ( !m_spUserData->SetValue(SingleInfectionAnomaly, bVal) )
    {
        CCTRACEE(_T("CEZAnomaly::SetSingleInfectionAnomaly() - Failed to set user data SingleInfectionAnomaly to %s."), bVal ? "true" : "false");
        return false;
    }

    return true;
}

void CEZAnomaly::SetAnomalyStatus(AnomalyAction eNextAction, 
                                  CommonUIInfectionStatus eStatus)
{
    if ( !m_bInitialized && !Initialize() )
    {
        CCTRACEE(_T("CEZAnomaly::SetAnomalyStatus() - Failed to initialize."));
        return;
    }

    // Set status
    if ( !m_spUserData->SetValue(CurrentStatus, static_cast<DWORD>(eStatus)) )
    {
        CCTRACEE(_T("CEZAnomaly::SetAnomalyStatus() - Failed to set user data CurrentStatus to %d."), eStatus);
    }

    // Set next action
    if ( !m_spUserData->SetValue(ActionToTake, 
                                 static_cast<DWORD>(eNextAction)) )
    {
        CCTRACEE(_T("CEZAnomaly::SetAnomalyStatus() - Failed to set user data ActionToTake to %d."), eNextAction);
    }
}

bool CEZAnomaly::CantDelete()
{
    if ( !m_bInitialized && !Initialize() )
    {
        CCTRACEE(_T("CEZAnomaly::CantDelete() - Failed to initialize."));
        return false;
    }

    // Check if the status is can't delete
    DWORD dwVal = 0;

    if ( !m_spUserData->GetValue(CurrentStatus, dwVal) )
    {
        // There is no status for this item yet
        return false;
    }

    if ( dwVal == InfectionStatus_CantDelete )
    {
        return true;
    }

    return false;
}

bool CEZAnomaly::GetName(cc::IString*& pStrName)
{
    if ( !m_bInitialized && !Initialize() )
    {
        CCTRACEE(_T("CEZAnomaly::GetName() - Failed to initialize."));
        return false;
    }

    ISymBasePtr pSymBase;

    if ( !m_spAnomalyProps->GetValue(ccEraser::IAnomaly::Name, pSymBase) )
    {
        CCTRACEE(_T("CEZAnomaly::GetName() - Failed to get the name symbase value."));
        return false;
    }

    if ( SYM_FAILED(pSymBase->QueryInterface(cc::IID_String, 
                                             reinterpret_cast<void**>(&pStrName))) )
    {
        CCTRACEE(_T("CEZAnomaly::GetName() - Failed to QI for the name string."));
        return false;
    }

    return true;
}

bool CEZAnomaly::GetID(cc::IString*& pStrID)
{
    if ( !m_bInitialized && !Initialize() )
    {
        CCTRACEE(_T("CEZAnomaly::GetID() - Failed to initialize."));
        return false;
    }

    ISymBasePtr pSymBase;

    if ( !m_spAnomalyProps->GetValue(ccEraser::IAnomaly::ID, pSymBase) )
    {
        CCTRACEW(_T("CEZAnomaly::GetID() - Failed to get the ID symbase value."));
        return false;
    }

    if ( SYM_FAILED(pSymBase->QueryInterface(cc::IID_String, 
                                             reinterpret_cast<void**>(&pStrID))) )
    {
        CCTRACEE(_T("CEZAnomaly::GetID() - Failed to QI for the ID string."));
        return false;
    }

    return true;
}

cc::IIndexValueCollection* CEZAnomaly::GetCategories()
{
    if ( !m_bInitialized && !Initialize() )
    {
        CCTRACEE(_T("CEZAnomaly::GetCategories() - Failed to initialize."));
        return NULL;
    }

    return m_spCategories.m_p;
}

bool CEZAnomaly::IsViral()
{
    if ( !m_bInitialized && !Initialize() )
    {
        CCTRACEE(_T("CEZAnomaly::IsViral() - Failed to initialize."));
        return false;
    }

    // Look for viral categories
    size_t nCatCount = m_spCategories->GetCount();

    for ( size_t nCur = 0; nCur < nCatCount; nCur++ )
    {
        DWORD dwCategory = 0;

        if ( !m_spCategories->GetValue(nCur, dwCategory) )
        {
            CCTRACEE(_T("CScanManager::IsViral() - Error getting anomaly category %d."), nCur);
            continue;
        }

        // Does this category match any viral categories
        if ( static_cast<ccEraser::IAnomaly::Anomaly_Category>(dwCategory) ==  
                                                      ccEraser::IAnomaly::Viral
             ||
             static_cast<ccEraser::IAnomaly::Anomaly_Category>(dwCategory) == 
                                                  ccEraser::IAnomaly::Malicious
             ||
             static_cast<ccEraser::IAnomaly::Anomaly_Category>(dwCategory) == 
                                          ccEraser::IAnomaly::ReservedMalicious 
             ||
             static_cast<ccEraser::IAnomaly::Anomaly_Category>(dwCategory) ==  
                                                ccEraser::IAnomaly::Heuristic )
        {
            return true;
        }
    }

    return false;
}

bool CEZAnomaly::IsNonViral()
{
    if ( !m_bInitialized && !Initialize() )
    {
        CCTRACEE(_T("CEZAnomaly::IsNonViral() - Failed to initialize."));
        return false;
    }

    // Look for viral categories
    size_t nCatCount = m_spCategories->GetCount();

    for ( size_t nCur = 0; nCur < nCatCount; nCur++ )
    {
        DWORD dwCategory = 0;

        if ( !m_spCategories->GetValue(nCur, dwCategory) )
        {
            CCTRACEE(_T("CScanManager::IsNonViral() - Error getting anomaly category %d."), nCur);
            continue;
        }

        // Does this category match any viral categories
        if ( static_cast<ccEraser::IAnomaly::Anomaly_Category>(dwCategory) ==
                                               ccEraser::IAnomaly::SecurityRisk 
             ||
             static_cast<ccEraser::IAnomaly::Anomaly_Category>(dwCategory) == 
                                                   ccEraser::IAnomaly::Hacktool 
             ||
             static_cast<ccEraser::IAnomaly::Anomaly_Category>(dwCategory) == 
                                                    ccEraser::IAnomaly::SpyWare 
             ||
             static_cast<ccEraser::IAnomaly::Anomaly_Category>(dwCategory) ==
                                                  ccEraser::IAnomaly::Trackware 
             ||
             static_cast<ccEraser::IAnomaly::Anomaly_Category>(dwCategory) ==
                                                     ccEraser::IAnomaly::Dialer 
             ||
             static_cast<ccEraser::IAnomaly::Anomaly_Category>(dwCategory) ==
                                               ccEraser::IAnomaly::RemoteAccess 
             ||
             static_cast<ccEraser::IAnomaly::Anomaly_Category>(dwCategory) == 
                                                     ccEraser::IAnomaly::Adware 
             ||
             static_cast<ccEraser::IAnomaly::Anomaly_Category>(dwCategory) ==
                                                     ccEraser::IAnomaly::Joke )
        {
            return true;
        }
    }

    return false;
}

DWORD CEZAnomaly::GetVID()
{
    if ( !m_bInitialized && !Initialize() )
    {
        CCTRACEE(_T("CEZAnomaly::IsNonViral() - Failed to initialize."));
        return 0;
    }

    DWORD dwVID = 0;

    if ( !m_spAnomalyProps->GetValue(ccEraser::IAnomaly::VID, dwVID) )
    {
        CCTRACEE(_T("CEZAnomaly::GetVID() - Failed to get the VID property."));
    }

    return dwVID;
}

bool CEZAnomaly::GetAnomalyStatus(AnomalyAction& eNextAction, 
                                  CommonUIInfectionStatus& eStatus)
{
    if ( !m_bInitialized && !Initialize() )
    {
        CCTRACEE(_T("CEZAnomaly::GetAnomalyStatus() - Failed to initialize."));
        return false;
    }

    bool bRet = true;
    DWORD dwVal = 0;

    if ( !m_spUserData->GetValue(ActionToTake, dwVal) )
    {
        CCTRACEE(_T("CEZAnomaly::GetAnomalyStatus() - Failed to get the action to take."));
        bRet = false;
    }
    else
    {
        eNextAction = static_cast<AnomalyAction>(dwVal);
    }

    if ( !m_spUserData->GetValue(CurrentStatus, dwVal) )
    {
        CCTRACEE(_T("CEZAnomaly::GetAnomalyStatus() - Failed to get the status."));
        bRet = false;
    }
    else
    {
        eStatus = static_cast<CommonUIInfectionStatus>(dwVal);
    }

    return bRet;
}

ccEraser::IRemediationAction* CEZAnomaly::GetRemediation(size_t nItem)
{
    if ( !m_bInitialized && !Initialize() )
    {
        CCTRACEE(_T("CEZAnomaly::GetRemediation() - Failed to initialize."));
        return NULL;
    }

    if ( !GetRemediations() )
    {
        CCTRACEE(_T("CEZAnomaly::GetRemediation() - Failed to get remediations."));
        return NULL;
    }

    size_t nCount = GetRemediationCount();

    // Validate index
    if ( nItem >= nCount )
    {
        CCTRACEE(_T("CEZAnomaly::GetRemediation() - Remediaton %d is out of range. Total remediations = %d"), nItem, nCount);
        return NULL;
    }

    ccEraser::IRemediationActionPtr spRemAction;
    ccEraser::eResult eRes = m_spRemediationList->GetItem(nItem, spRemAction);

    if ( ccEraser::Failed(eRes) )
    {
        CCTRACEE(_T("CEZAnomaly::GetRemediation() - Failed to get item %d. eResult = %d"), nItem, eRes);
        return NULL;
    }

    return spRemAction.m_p;
}

size_t CEZAnomaly::GetRemediationCount()
{
    if ( !m_bInitialized && !Initialize() )
    {
        CCTRACEE(_T("CEZAnomaly::GetRemediationCount() - Failed to initialize."));
        return 0;
    }

    if ( !GetRemediations() )
    {
        CCTRACEE(_T("CEZAnomaly::GetRemediationCount() - Failed to get remediations."));
        return 0;
    }

    size_t nCount = 0;
    ccEraser::eResult eRes = m_spRemediationList->GetCount(nCount);

    if ( ccEraser::Failed(eRes) )
    {
        CCTRACEE(_T("CEZAnomaly::GetRemediationCount() - Failed to get the count. eResult = %d"), eRes);
        return 0;
    }

    return nCount;
}

bool CEZAnomaly::SetMemBootType(ScanInfectionTypes type)
{
    if ( !m_bInitialized && !Initialize() )
    {
        CCTRACEE(_T("CEZAnomaly::SetMemBootType() - Failed to initialize."));
        return false;
    }

    if ( !m_spUserData->SetValue(AnomalyMemBootType, 
                                 static_cast<DWORD>(type)) )
    {
        CCTRACEE(_T("CEZAnomaly::SetMemBootType() - Failed to set the type to %d."), type);
        return false;
    }

    return true;
}

bool CEZAnomaly::SetRepairOnly()
{
    if ( !m_bInitialized && !Initialize() )
    {
        CCTRACEE(_T("CEZAnomaly::SetRepairOnly() - Failed to initialize."));
        return false;
    }

    m_bRepairOnly = true;
    return true;
}

bool CEZAnomaly::IsBootRecordAnomaly()
{
    if ( !m_bInitialized && !Initialize() )
    {
        CCTRACEE(_T("CEZAnomaly::IsBootRecordAnomaly() - Failed to initialize."));
        return false;
    }

    DWORD dwType = 0;

    if ( !m_spUserData->GetValue(AnomalyMemBootType, dwType) )
    {
        return false;
    }

    return (static_cast<ScanInfectionTypes>(dwType) == BRInfectionType);
}

bool CEZAnomaly::IsMBRAnomaly()
{
    if ( !m_bInitialized && !Initialize() )
    {
        CCTRACEE(_T("CEZAnomaly::IsMBRAnomaly() - Failed to initialize."));
        return false;
    }

    DWORD dwType = 0;

    if ( !m_spUserData->GetValue(AnomalyMemBootType, dwType) )
    {
        return false;
    }

    return (static_cast<ScanInfectionTypes>(dwType) == MBRInfectionType);
}

bool CEZAnomaly::Is9xMemAnomaly()
{
    if ( !m_bInitialized && !Initialize() )
    {
        CCTRACEE(_T("CEZAnomaly::Is9xMemAnomaly() - Failed to initialize."));
        return false;
    }

    DWORD dwType = 0;

    if ( !m_spUserData->GetValue(AnomalyMemBootType, dwType) )
    {
        return false;
    }

    return (static_cast<ScanInfectionTypes>(dwType) == MemoryInfectionType);
}

bool CEZAnomaly::IsRepairOnlyAnomaly()
{
    if ( !m_bInitialized && !Initialize() )
    {
        CCTRACEE(_T("CEZAnomaly::IsRepairOnlyAnomaly() - Failed to initialize."));
        return false;
    }

    return m_bRepairOnly;
}

CRemediationStats * 
CEZAnomaly::GetRemediationStatistics(LPCTSTR pcszUnknownText /*= NULL*/,
                                     LPCTSTR pcszDefaultRegKeyText /*= NULL*/)
{
    if ( !m_bInitialized && !Initialize() )
    {
        CCTRACEE(_T("CEZAnomaly::GetRemediationStatistics() - Failed to initialize."));
        return NULL;
    }

    m_RemediationStatistics.Initialize( m_spAnomaly, pcszUnknownText, 
                                        pcszDefaultRegKeyText );
    return &m_RemediationStatistics;
}

bool CEZAnomaly::GetQuarantineItemUUID(UUID &uuid)
{
    if ( !m_bInitialized && !Initialize() )
    {
        CCTRACEE(_T("CEZAnomaly::GetQuarantineItemUUID() - Failed to initialize."));
        return false;
    }

    cc::IStringPtr pStrUUID = NULL;

    if ( !m_spUserData->GetValue(AnomalyQuarItemUUID, 
                                 reinterpret_cast<ISymBase*&>(pStrUUID)) )
    {
        return false;
    }
    else if ( pStrUUID == NULL )
    {
        CCTRACEE(_T("CEZAnomaly::GetQuarantineItemUUID() - Error getting the remediation quarantine item UUID string. String is null"));
        return false;
    }

    // Convert the wide string to a UUID
    wchar_t wcharStr[MAX_PATH] = {0};
    wcsncpy(wcharStr, pStrUUID->GetStringW(), MAX_PATH);

    if ( S_OK != IIDFromString(wcharStr, &uuid) )
    {
        CCTRACEE(_T("CEZAnomaly::GetQuarantineItemUUID() - Error converting the string to a UUID."));
        return false;
    }

    return true;
}

bool CEZAnomaly::SetQuarantineItemUUID(UUID uuid)
{
    if ( !m_bInitialized && !Initialize() )
    {
        CCTRACEE(_T("CEZAnomaly::SetQuarantineItemUUID() - Failed to initialize."));
        return false;
    }

    // Convert the UUID to a wide string for storage
    wchar_t wcharStr[MAX_PATH] = {0};

    if ( 0 == StringFromGUID2(uuid, wcharStr, MAX_PATH ) )
    {
        CCTRACEE(_T("CEZAnomaly::SetQuarantineItemUUID() - Failed to convert the UUID to a wide string."));
        return false;
    }

    // Create an IString to store it in the user data
    cc::IStringPtr pStrUUID;
    pStrUUID.Attach(ccSym::CStringImpl::CreateStringImpl());

    if ( pStrUUID == NULL )
    {
        CCTRACEE(_T("CEZAnomaly::SetQuarantineItemUUID() - Failed to create string implementation"));
        return false;
    }

    // Set the string
    pStrUUID->SetStringW(wcharStr);

    if ( !m_spUserData->SetValue(AnomalyQuarItemUUID, pStrUUID) )
    {
        CCTRACEE(_T("CEZAnomaly::SetQuarantineItemUUID() - Error setting the remediation quarantine item UUID string."));
        return false;
    }

    return true;
}

///////////////////////////////////////////////////////////////////////////
// CEZREmediation implementation

CEZRemediation::CEZRemediation(ccEraser::IRemediationAction* pRem)
{
    m_bInitialized = false;
    m_spRemediation = pRem;

    if ( m_spRemediation == NULL )
    {
        CCTRACEE(_T("CEZRemediation::CEZRemediation() - Remediation action is null."));
        return;
    }

    // Get the properties
    ccEraser::eResult eRes = m_spRemediation->GetProperties(m_spProps);

    if ( ccEraser::Failed(eRes) )
    {   
        CCTRACEE(_T("CEZRemediation::CEZRemediation() - Error getting remediation action properties."), eRes);
        return;
    }

    // Get the type
    eRes = m_spRemediation->GetType(m_Type);

    if ( ccEraser::Failed(eRes) )
    {
        CCTRACEE(_T("CEZRemediation::CEZRemediation() - Error getting remediation action type."), eRes);
        return;
    }

    m_bInitialized = true;
}

CEZRemediation::~CEZRemediation()
{
}

cc::IKeyValueCollection* CEZRemediation::GetProperties()
{
    if( !m_bInitialized )
    {
        CCTRACEE(_T("CEZRemediation::GetProperties() - Not initialized."));
        return NULL;
    }

    return m_spProps.m_p;
}

cc::IKeyValueCollection* CEZRemediation::GetUserData()
{
    if( !m_bInitialized )
    {
        CCTRACEE(_T("CEZRemediation::GetUserData() - Not initialized."));
        return NULL;
    }

    // If we already have them just return what we have
    if( m_spUserData != NULL )
    {
        return m_spUserData;
    }

    // Create the user data if necessary
    if( !m_spProps->GetExists(ccEraser::IRemediationAction::UserData) )
    {
        m_spUserData.Attach(ccSym::CKeyValueCollectionImpl::CreateKeyValueCollectionImpl());
        if( m_spUserData )
        {
            // Store the collection as the remediation action user data
            if( !m_spProps->SetValue(ccEraser::IRemediationAction::UserData, m_spUserData) )
            {
                CCTRACEE(_T("CEZRemediation::GetUserData() - Failed to store user data."));
            }
        }
        else
        {
            CCTRACEE(_T("CEZRemediation::GetUserData() - Failed to create user data collection."));
        }
    }
    else
    {
        // Grab the existing user data values
        ISymBasePtr pSymBase;
        if( !m_spProps->GetValue(ccEraser::IRemediationAction::UserData, pSymBase) )
        {
            CCTRACEE(_T("CEZRemediation::GetUserData() - Failed to retrieve the existing user data collection."));
        }
        else if( SYM_FAILED(pSymBase->QueryInterface(cc::IID_KeyValueCollection, reinterpret_cast<void**>(&m_spUserData))) )
        {
            CCTRACEE(_T("CEZRemediation::GetUserData() - Failed to QI for the key value collection."));
        }
    }

    return m_spUserData.m_p;
}

ccEraser::eObjectType CEZRemediation::GetType()
{
    if( !m_bInitialized )
    {
        CCTRACEE(_T("CEZRemediation::GetType() - Not initialized."));
        return static_cast<ccEraser::eObjectType>(-1);
    }

    return m_Type;
}

bool CEZRemediation::IsScanInfectionType()
{
    if( !m_bInitialized )
    {
        CCTRACEE(_T("CEZRemediation::IsScanInfection() - Not initialized."));
        return false;
    }

    return (m_Type == ccEraser::InfectionRemediationActionType);
}

ScanInfectionTypes CEZRemediation::GetScanInfectionType()
{
    ScanInfectionTypes type = static_cast<ScanInfectionTypes>(-1);
    if( !m_bInitialized )
    {
        CCTRACEE(_T("CEZRemediation::GetScanInfectionType() - Not initialized."));
        return type;
    }

    if( m_Type != ccEraser::InfectionRemediationActionType )
    {
        CCTRACEE(_T("CEZRemediation::GetScanInfectionType() - Not a scan infection remediation."));
        return type;
    }

    // Make sure we have the user data collection handy
    if( GetUserData() == NULL )
    {
        CCTRACEE(_T("CEZRemediation::GetScanInfectionType() - Error getting user data"));
        return type;
    }

    DWORD dwType = 0;
    if( m_spUserData->GetValue(ScanInfectionType, dwType) )
    {
        type = static_cast<ScanInfectionTypes>(dwType);
    }
    else
        CCTRACEE(_T("CEZRemediation::GetScanInfectionType() - Error getting the type for scan infection item"));

    return type;
}

bool CEZRemediation::IsProcessType()
{
    if( !m_bInitialized )
    {
        CCTRACEE(_T("CEZRemediation::IsProcessType() - Not initialized."));
        return false;
    }

    if( m_Type == ccEraser::ProcessRemediationActionType )
        return true;

    if( m_Type == ccEraser::ServiceRemediationActionType && GetOperationType() == ccEraser::IRemediationAction::Stop )
        return true;

    return false;
}

IScanInfection* CEZRemediation::GetScanInfection()
{
    if ( !m_bInitialized )
    {
        CCTRACEE(_T("CEZRemediation::GetScanInfection() - Not initialized."));
        return NULL;
    }

    if ( !IsScanInfectionType() )
    {
        CCTRACEE(_T("CEZRemediation::GetScanInfection() - This is not a scan infection remediation action."));
        return NULL;
    }

    // Get the scan infection item
    ISymBasePtr pSymBase;
    if ( !m_spProps->GetValue(ccEraser::IRemediationAction::ScanInfection, 
         pSymBase) )
    {
        CCTRACEE(_T("CEZRemediation::GetScanInfection() - Failed to get the scan infection object."));
        return NULL;
    }

    IScanInfectionQIPtr pQIScanInfection = pSymBase;
    if ( NULL == pQIScanInfection.m_p )
    {
        CCTRACEE(_T("CEZRemediation::GetScanInfection() - Failed to QI for the scan infection object."));
        return NULL;
    }

    return pQIScanInfection.m_p;
}

bool CEZRemediation::GetDisplayString(cc::IString*& pStrDisp, LPCTSTR pcszDefaultRegKeyName /*= NULL*/)
{
    if( !m_bInitialized )
    {
        CCTRACEE(_T("CEZRemediation::GetDisplayString() - Not initialized."));
        return false;
    }

    ATL::CAtlString strTemp;

    switch( m_Type )
    {
    case ccEraser::InfectionRemediationActionType:
        // The display string is stored in the user data so make sure we have the user data collection
        if( GetUserData() == NULL )
        {
            CCTRACEE(_T("CEZRemediation::GetDisplayString() - Error getting user data"));
            return false;
        }

        if( !m_spUserData->GetValue(RemediationDescription, reinterpret_cast<ISymBase*&>(pStrDisp)) )
        {
            CCTRACEE(_T("CEZRemediation::GetDisplayString() - Error getting the remediation description for scan infection item"));
            return false;
        }
        else if( pStrDisp == NULL )
        {
            CCTRACEE(_T("CEZRemediation::GetDisplayString() - Error getting the remediation description for scan infection item. Display string is null"));
            return false;
        }
        break;
    case ccEraser::FileRemediationActionType:
    case ccEraser::DirectoryRemediationActionType:
    case ccEraser::ProcessRemediationActionType:
    case ccEraser::COMRemediationActionType:
    case ccEraser::BatchRemediationActionType:
    case ccEraser::INIRemediationActionType:
        return GetPathProperty(pStrDisp);
        break;
    case ccEraser::RegistryRemediationActionType:
        // Get the key name
        if( !m_spProps->GetValue(ccEraser::IRemediationAction::KeyName, reinterpret_cast<ISymBase*&>(pStrDisp)) || pStrDisp == NULL )
        {
            CCTRACEE(_T("CEZRemediation::GetDisplayString() - Error getting the registry remediation key name."));
            return false;
        }

        // If there is no value name available then we're done, otherwise we append the value name for display
        if( !m_spProps->GetExists(ccEraser::IRemediationAction::ValueName) )
        {
            return true;
        }

        // Save off the key name
        strTemp = pStrDisp->GetStringA();
        strTemp += "\\";
        pStrDisp->Release();
        pStrDisp = NULL;

        // Get the value name
        if( !m_spProps->GetValue(ccEraser::IRemediationAction::ValueName, reinterpret_cast<ISymBase*&>(pStrDisp)) || pStrDisp == NULL )
        {
            CCTRACEE(_T("CEZRemediation::GetDisplayString() - Error getting the registry remediation value name."));
            return false;
        }

        // If the value name is an empty string ccEraser is refering to the default value
        if( _tcslen(pStrDisp->GetStringA()) == 0 )
        {
            // If no default text was passed in, use hard-coded default text
            if( pcszDefaultRegKeyName == NULL )
                strTemp += "(Default)";
            else
                strTemp += pcszDefaultRegKeyName;
        }
        else
            strTemp += pStrDisp->GetStringA();

        pStrDisp->Release();
        pStrDisp = NULL;

        // Assign the full registry key path and value to the output string
        pStrDisp = ccSym::CStringImpl::CreateStringImpl(strTemp);
        break;
    case ccEraser::ServiceRemediationActionType:
        // Get the service name
        if( !m_spProps->GetValue(ccEraser::IRemediationAction::ServiceName, reinterpret_cast<ISymBase*&>(pStrDisp)) || pStrDisp == NULL )
        {
            CCTRACEW(_T("CEZRemediation::GetDisplayString() - Error getting the service remediation name."));
            return false;
        }
        break;
    case ccEraser::HostsRemediationActionType:
        // Get the hosts Domain Name entry
        if( !m_spProps->GetValue(ccEraser::IRemediationAction::DomainName, reinterpret_cast<ISymBase*&>(pStrDisp)) || pStrDisp == NULL )
        {
            CCTRACEW(_T("CEZRemediation::GetDisplayString() - Error getting the Hosts remediation action domain name."));
        }
        else
        {
            strTemp = pStrDisp->GetStringA();
            pStrDisp->Release();
            pStrDisp = NULL;
        }

        // Get the hosts IP address
        if( !m_spProps->GetValue(ccEraser::IRemediationAction::IPAddress, reinterpret_cast<ISymBase*&>(pStrDisp)) || pStrDisp == NULL )
        {
            CCTRACEW(_T("CEZRemediation::GetDisplayString() - Error getting the Hosts remediation action IP address."));
        }
        
        if( strTemp.IsEmpty() && pStrDisp == NULL )
        {
            CCTRACEE(_T("CEZRemediation::GetDisplayString() - Error getting any Hosts remediation action display data."));
            return false;
        }

        // Concat the Domain name and IP address if we have both
        if( !strTemp.IsEmpty() )
        {
            if( pStrDisp )
            {
                // Append the IP Address to the host name
                strTemp += " - ";
                strTemp += pStrDisp->GetStringA();
                pStrDisp->Release();
                pStrDisp = NULL;
            }

            pStrDisp = ccSym::CStringImpl::CreateStringImpl(strTemp);
        }
        break;
    case ccEraser::LSPRemediationActionType:
        // Get the Provider ID
        if( !m_spProps->GetValue(ccEraser::IRemediationAction::ProviderID, reinterpret_cast<ISymBase*&>(pStrDisp)) || pStrDisp == NULL )
        {
            // Failed to get the LSP provider ID attempt to use the path instead
            CCTRACEW(_T("CEZRemediation::GetDisplayString() - Failed to get the LSP remediation action provider ID. Attempting to use the path property."));

            return GetPathProperty(pStrDisp);
        }
        break;
    default:
        CCTRACEW(_T("CEZRemediation::GetDisplayString() - Remediation action type is unknown. Type = %d."), m_Type);
        return false;
    }

    return true;
}

cc::IIndexValueCollection* CEZRemediation::GetReferencedFilePaths()
{
    if( !m_bInitialized )
    {
        CCTRACEE(_T("CEZRemediation::GetReferencedFilePaths() - Not initialized."));
        return false;
    }

    cc::IIndexValueCollectionQIPtr spQIIndexFiles = NULL;
    ISymBasePtr pSymBase = NULL;

    if( !m_spProps->GetValue(ccEraser::IRemediationAction::ReferencedFilePaths, pSymBase) )
    {
        CCTRACEW(_T("CEZRemediation::GetReferencedFilePaths() - Unable to get symbase value."));
        return NULL;
    }

    spQIIndexFiles = pSymBase;
    if( spQIIndexFiles.m_p == NULL )
    {
        CCTRACEE(_T("CEZRemediation::GetReferencedFilePaths() - Unable to QI for the index value collection."));
        return NULL;
    }

    return spQIIndexFiles.m_p;
}

bool CEZRemediation::GetPathProperty(cc::IString*& pStrPath)
{
    if( !m_bInitialized )
    {
        CCTRACEE(_T("CEZRemediation::GetPathProperty() - Not initialized."));
        return false;
    }

    // Get the path property
    ISymBasePtr pSymBase;
    if( !m_spProps->GetValue(ccEraser::IRemediationAction::Path, pSymBase) )
    {
        CCTRACEE(_T("CEZRemediation::GetPathProperty() - Failed to get the path base interface."));
        return false;
    }

    if( SYM_FAILED(pSymBase->QueryInterface(cc::IID_String, reinterpret_cast<void**>(&pStrPath))) )
    {
        CCTRACEE(_T("CEZRemediation::GetPathProperty() - Failed to QI for the path."));
        return false;
    }

    return true;
}

bool CEZRemediation::GetProcessRemediationID(DWORD& dwProcID)
{
    if( !m_bInitialized )
    {
        CCTRACEE(_T("CEZRemediation::GetProcessRemediationID() - Not initialized."));
        return false;
    }

    if( m_Type != ccEraser::ProcessRemediationActionType )
    {
        CCTRACEE(_T("CEZRemediation::GetProcessRemediationID() - Not a process remediation."));
        return false;
    }

    // Get the ProcessID property
    if( !m_spProps->GetValue(ccEraser::IRemediationAction::ProcessID, dwProcID) )
    {
        CCTRACEW(_T("CEZRemediation::GetProcessRemediationID() - Failed to get the process ID property."));
        return false;
    }

    return true;
}

bool CEZRemediation::SetManualRebootRequired()
{
    if( !m_bInitialized )
    {
        CCTRACEE(_T("CEZRemediation::SetManualRebootRequired() - Not initialized."));
        return false;
    }

    if( GetUserData() == NULL )
    {
        CCTRACEE(_T("CEZRemediation::SetManualRebootRequired() - Error getting user data"));
        return false;
    }

    if( !m_spUserData->SetValue(RemediationRequiresRebooot, true) )
    {
        CCTRACEE(_T("CEZRemediation::SetManualRebootRequired() - Failed to set property in user data."));
        return false;
    }

    return true;
}

bool CEZRemediation::ManualRebootRequired()
{
    if( !m_bInitialized )
    {
        CCTRACEE(_T("CEZRemediation::ManualRebootRequired() - Not initialized."));
        return false;
    }

    if( GetUserData() == NULL )
    {
        CCTRACEE(_T("CEZRemediation::ManualRebootRequired() - Error getting user data"));
        return false;
    }

    bool bResult = false;
    if( m_spUserData->GetValue(RemediationRequiresRebooot, bResult) && bResult == true )
        return true;

    return false;
}

ccEraser::IRemediationAction::Action_State CEZRemediation::GetState()
{
    ccEraser::IRemediationAction::Action_State state = ccEraser::IRemediationAction::NotRemediated;
    if( !m_bInitialized )
    {
        CCTRACEE(_T("CEZRemediation::GetState() - Not initialized."));
        return state;
    }

    DWORD dwVal;
    if( !m_spProps->GetValue(ccEraser::IRemediationAction::State, dwVal) )
    {
        CCTRACEE(_T("CEZRemediation::GetState() - Failed to get the remediation state."));
        return state;
    }

    state = static_cast<ccEraser::IRemediationAction::Action_State>(dwVal);

    return state;
}

bool CEZRemediation::GetRemediationResult(ccEraser::eResult& eRemResult)
{
    if( !m_bInitialized )
    {
        CCTRACEE(_T("CEZRemediation::GetRemediationResult() - Not initialized."));
        return false;
    }

    // If the result does not exist then this has not been attempted so return false
    if( !m_spProps->GetExists(ccEraser::IRemediationAction::RemediationResult) )
    {
        return false;
    }

    // Get the result
    DWORD dwResult;
    if( !m_spProps->GetValue(ccEraser::IRemediationAction::RemediationResult, dwResult) )
    {
        CCTRACEE(_T("CEZRemediation::GetRemediationResult() - Failed to get the remediation result."));
        return false;
    }

    eRemResult = static_cast<ccEraser::eResult>(dwResult);

    return true;
}

bool CEZRemediation::SetRemediationResult(ccEraser::eResult eRemResult)
{
    if( !m_bInitialized )
    {
        CCTRACEE(_T("CEZRemediation::SetRemediationResult() - Not initialized."));
        return false;
    }

    // Set the result
    if( !m_spProps->SetValue(ccEraser::IRemediationAction::RemediationResult, static_cast<DWORD>(eRemResult)) )
    {
        CCTRACEE(_T("CEZRemediation::SetRemediationResult() - Failed to set the remediation result."));
        return false;
    }

    return true;
}

bool CEZRemediation::SetState(ccEraser::IRemediationAction::Action_State eState)
{
    if( !m_bInitialized )
    {
        CCTRACEE(_T("CEZRemediation::SetState() - Not initialized."));
        return false;
    }

    // Set the result
    if( !m_spProps->SetValue(ccEraser::IRemediationAction::State, static_cast<DWORD>(eState)) )
    {
        CCTRACEE(_T("CEZRemediation::SetState() - Failed to set the remediation state."));
        return false;
    }

    return true;
}

bool 
CEZRemediation::SetThreatOperation(
                    ccEraser::IRemediationAction::Handle_Threat_Ops operation)
{
    if( !m_bInitialized )
    {
        CCTRACEE(_T("CEZRemediation::SetThreatOperation() - Not initialized."));
        return false;
    }

    if ( !m_spProps->SetValue(
                           ccEraser::IRemediationAction::HandleThreatOperation,
                           static_cast<DWORD>(operation)) )
    {
        CCTRACEE(_T("CEZRemediation::SetThreatOperation() - Failed to set the handle threat operation property"));
        return false;
    }

    return true;
}

ccEraser::IRemediationAction::Handle_Threat_Ops 
CEZRemediation::GetThreatOperation()
{
    DWORD dwOperation = ccEraser::IRemediationAction::RepairOnly;

    if ( !m_bInitialized )
    {
        CCTRACEE(_T("CEZRemediation::GetThreatOperation() - Not initialized."));
        return static_cast<ccEraser::IRemediationAction::Handle_Threat_Ops>
                                                                 (dwOperation);
    }

    if ( !m_spProps->GetValue(
                           ccEraser::IRemediationAction::HandleThreatOperation,
                           dwOperation) )
    {
        CCTRACEE(_T("CEZRemediation::GetThreatOperation() - Failed to get the handle threat operation property"));
    }

    return static_cast<ccEraser::IRemediationAction::Handle_Threat_Ops>
                                                                 (dwOperation);
}

 
bool CEZRemediation::GetRecommendedOperation(DWORD &dwOperation)
{
    if ( !m_bInitialized )
    {
        CCTRACEE(_T("CEZRemediation::GetRecommendedOperation() - Not initialized."));
        return false;
    }

    return m_spProps->GetValue( ccEraser::IRemediationAction::RecommendedThreatOperation,
                                dwOperation);
}

ccEraser::IRemediationAction::Operation_Type CEZRemediation::GetOperationType()
{
    ccEraser::IRemediationAction::Operation_Type opType = 
                               ccEraser::IRemediationAction::UnknownOperation;

    if ( !m_bInitialized )
    {
        CCTRACEE(_T("CEZRemediation::GetOperationType() - Not initialized."));
        return opType;
    }

    DWORD dwVal = 0;
    if ( m_spProps->GetValue(ccEraser::IRemediationAction::Operation, dwVal) )
    {
        opType = static_cast<ccEraser::IRemediationAction::Operation_Type>
                                                                      (dwVal);
    }
    else
    {
        CCTRACEE(_T("CEZRemediation::GetOperationType() - Failed to retrieve the operation type."));
    }
    
    return opType;
}

bool 
CEZRemediation::SetOperationType(ccEraser::IRemediationAction::Operation_Type 
                                                                          type)
{
    if ( !m_bInitialized )
    {
        CCTRACEE(_T("CEZRemediation::SetOperationType() - Not initialized."));
        return false;
    }

    if ( !m_spProps->SetValue(ccEraser::IRemediationAction::Operation, 
                              static_cast<DWORD>(type)) )
    {
        CCTRACEE(_T("CEZRemediation::SetOperationType() - Failed to set the operation type."));
        return false;
    }

    return true;
}

bool CEZRemediation::SetStatusText(LPCTSTR pstrStatus)
{
    if ( !m_bInitialized )
    {
        CCTRACEE(_T("CEZRemediation::SetStatusText() - Not initialized."));
        return false;
    }

    if ( pstrStatus == NULL )
    {
        CCTRACEE(_T("CEZRemediation::SetStatusText() - String is null"));
        return false;
    }

    if ( NULL == GetUserData() )
    {
        CCTRACEE(_T("CEZRemediation::SetStatusText() - Error getting user data"));
        return false;
    }

    cc::IStringPtr pSymstrStatus;
    pSymstrStatus.Attach(ccSym::CStringImpl::CreateStringImpl(pstrStatus));

    if ( pSymstrStatus == NULL )
    {
        CCTRACEE(_T("CEZRemediation::SetStatusText() - Failed to allocate string object"));
        return false;
    }

    if ( !m_spUserData->SetValue(RemediationStatusText, pSymstrStatus) )
    {
        CCTRACEE(_T("CEZRemediation::SetStatusText() - Error setting the remediation status text"));
        return false;
    }

    return true;
}

bool CEZRemediation::ShouldSaveRemediation()
{
    if ( !m_bInitialized )
    {
        CCTRACEE(_T("CEZRemediation::ShouldSaveRemediation() - Not initialized."));
        return false;
    }

    // We don't want to save process remediations
    if ( m_Type == ccEraser::ProcessRemediationActionType )
    {
        return false;
    }

    // We don't want to save service stop remediations, just the remove 
    // remediations
    if ( m_Type == ccEraser::ServiceRemediationActionType )
    {
        ccEraser::IRemediationAction::Operation_Type opType =
                                                           GetOperationType();

        if ( opType == ccEraser::IRemediationAction::Stop )
        {
            return false;
        }
    }

    // If the status is that the item did not exist or require action then 
    // there's no point in saving it
    ccEraser::eResult eRes = ccEraser::Fail;

    if ( GetRemediationResult(eRes) && (eRes == ccEraser::FileNotFound || 
                                        eRes == ccEraser::NothingToDo) )
    {
        return false;
    }

    return true;
}

bool CEZRemediation::GetStatusText(cc::IString*& pstrStatus)
{
    if( !m_bInitialized )
    {
        CCTRACEE(_T("CEZRemediation::GetStatusText() - Not initialized."));
        return false;
    }

    if( NULL == GetUserData() )
    {
        CCTRACEE(_T("CEZRemediation::GetStatusText() - Error getting user data"));
        return false;
    }

    if( !m_spUserData->GetValue(RemediationStatusText, reinterpret_cast<ISymBase*&>(pstrStatus)) )
    {
        CCTRACEE(_T("CEZRemediation::GetStatusText() - Error getting the remediation status text"));
        return false;
    }
    else if( pstrStatus == NULL )
    {
        CCTRACEE(_T("CEZRemediation::GetStatusText() - Error getting the remediation status text. String is null"));
        return false;
    }

    return true;
}

bool CEZRemediation::IsRemediationPresent()
{
    if( !m_bInitialized )
    {
        CCTRACEE(_T("CEZRemediation::IsRemediationPresent() - Not initialized."));
        return true;
    }

    if( NULL == GetUserData() )
    {
        CCTRACEE(_T("CEZRemediation::IsRemediationPresent() - Error getting user data"));
        return true;
    }

    bool bPresent = true;
    if( !m_spUserData->GetValue(RemediationPresent, bPresent) )
    {
        // The value does not exist so treat it as present
        return true;
    }

    // Return the value from the user data
    return bPresent;
}

bool CEZRemediation::SetRemediationPresent(bool bPresent)
{
    if( !m_bInitialized )
    {
        CCTRACEE(_T("CEZRemediation::SetRemediationPresent() - Not initialized."));
        return false;
    }

    if( NULL == GetUserData() )
    {
        CCTRACEE(_T("CEZRemediation::SetRemediationPresent() - Error getting user data"));
        return false;
    }

    if( !m_spUserData->SetValue(RemediationPresent, bPresent) )
    {
        CCTRACEE(_T("CEZRemediation::SetRemediationPresent() - Error setting the remediation present property."));
        return false;
    }

    return true;
}

bool CEZRemediation::IsActiveInMemory()
{
    if( !m_bInitialized )
    {
        CCTRACEE(_T("CEZRemediation::IsActiveInMemory() - Not initialized."));
        return false;
    }

    if( NULL == GetUserData() )
    {
        CCTRACEE(_T("CEZRemediation::IsActiveInMemory() - Error getting user data"));
        return false;
    }

    bool bActive = false;
    if( !m_spUserData->GetValue(ActiveInMemory, bActive) )
    {
        // The value does not exist so treat it as not active
        CCTRACEW(_T("CEZRemediation::IsActiveInMemory() - No ActiveInMemory value in the user data"));

        return false;
    }

    // Return the value from the user data
    return bActive;
}

bool CEZRemediation::SetActiveInMemory(bool bActive)
{
    if( !m_bInitialized )
    {
        CCTRACEE(_T("CEZRemediation::SetActiveInMemory() - Not initialized."));
        return false;
    }

    if( NULL == GetUserData() )
    {
        CCTRACEE(_T("CEZRemediation::SetActiveInMemory() - Error getting user data"));
        return false;
    }

    if( !m_spUserData->SetValue(ActiveInMemory, bActive) )
    {
        CCTRACEE(_T("CEZRemediation::SetActiveInMemory() - Error setting the ActiveInMemory property."));
        return false;
    }

    return true;
}

bool CEZRemediation::SetQuarantineItemUUID(UUID uuid)
{
    if( !m_bInitialized )
    {
        CCTRACEE(_T("CEZRemediation::SetQuarantineItemUUID() - Not initialized."));
        return false;
    }

    if( NULL == GetUserData() )
    {
        CCTRACEE(_T("CEZRemediation::SetQuarantineItemUUID() - Error getting user data"));
        return false;
    }

    // Convert the UUID to a wide string for storage
    wchar_t wcharStr[MAX_PATH] = {0};
    if( 0 == StringFromGUID2(uuid, wcharStr, MAX_PATH ) )
    {
        CCTRACEE(_T("CEZRemediation::SetQuarantineItemUUID() - Failed to convert the UUID to a wide string."));
        return false;
    }

    // Create an IString to store it in the user data
    cc::IStringPtr pStrUUID;
    pStrUUID.Attach(ccSym::CStringImpl::CreateStringImpl());

    if( pStrUUID == NULL )
    {
        CCTRACEE(_T("CEZRemediation::SetQuarantineItemUUID() - Failed to create string implementation"));
        return false;
    }

    // Set the string
    pStrUUID->SetStringW(wcharStr);

    if( !m_spUserData->SetValue(QuarantineItemUUID, pStrUUID) )
    {
        CCTRACEE(_T("CEZRemediation::SetQuarantineItemUUID() - Error setting the remediation quarantine item UUID string."));
        return false;
    }

    return true;
}

bool CEZRemediation::GetQuarantineItemUUID(UUID &uuid)
{
    if( !m_bInitialized )
    {
        CCTRACEE(_T("CEZRemediation::GetQuarantineItemUUID() - Not initialized."));
        return false;
    }

    if( NULL == GetUserData() )
    {
        CCTRACEE(_T("CEZRemediation::GetQuarantineItemUUID() - Error getting user data"));
        return false;
    }

    cc::IStringPtr pStrUUID = NULL;
    if( !m_spUserData->GetValue(QuarantineItemUUID, reinterpret_cast<ISymBase*&>(pStrUUID)) )
    {
        return false;
    }
    else if( pStrUUID == NULL )
    {
        CCTRACEE(_T("CEZRemediation::GetQuarantineItemUUID() - Error getting the remediation quarantine item UUID string. String is null"));
        return false;
    }

    // Convert the wide string to a UUID
    wchar_t wcharStr[MAX_PATH] = {0};
    wcsncpy(wcharStr, pStrUUID->GetStringW(), MAX_PATH);
    if(S_OK != IIDFromString(wcharStr, &uuid) )
    {
        CCTRACEE(_T("CEZRemediation::GetQuarantineItemUUID() - Error converting the string to a UUID."));
        return false;
    }

    return true;
}

bool CEZRemediation::SetScanInfection(IScanInfection* pInfection)
{
    if ( !m_bInitialized )
    {
        CCTRACEE(_T("CEZRemediation::SetScanInfection() - Not initialized."));
        return false;
    }

    // Store the scan infection
    if( !m_spProps->SetValue(ccEraser::IRemediationAction::ScanInfection, 
                             pInfection) )
    {
        CCTRACEE(_T("CEZRemediation::SetScanInfection() - Failed to set the scan infection."));
        return false;
    }

    return true;
}

///////////////////////////////////////////////////////////////////////////
// CRemediationStats implementation

CRemediationStats::CRemediationStats(void) : m_bInitialized(false)
{
}

CRemediationStats::~CRemediationStats(void)
{
}

bool CRemediationStats::Clear()
{
    // Reset the strings
    m_vFile.clear();
    m_vProc.clear();
    m_vReg.clear();
    m_vCOM.clear();
    m_vService.clear();
    m_vBatch.clear();
    m_vINI.clear();
    m_vBR.clear();
    m_vMBR.clear();
    m_vUnknown.clear();
    m_vLSP.clear();
    m_vHosts.clear();

    m_bInitialized = false;

    return true;
}

bool CRemediationStats::Initialize(ccEraser::IAnomaly* pAnomaly, LPCTSTR pcszUnknownText /*=NULL*/, LPCTSTR pcszDefaultRegKeyText /*=NULL*/)
{
    if( m_bInitialized )
        return true;

    if( pAnomaly == NULL )
    {
        CCTRACEE(_T("CRemediationStats::Initialize() - Anomaly is null"));
        return false;
    }

    CEZAnomaly ezAnom;
    if( !ezAnom.Initialize(pAnomaly) )
    {
        CCTRACEE(_T("CRemediationStats::Initialize() - Anomaly initialization failed."));
        return false;
    }

    // Reset the strings
    m_vFile.clear();
    m_vProc.clear();
    m_vReg.clear();
    m_vCOM.clear();
    m_vService.clear();
    m_vBatch.clear();
    m_vINI.clear();
    m_vBR.clear();
    m_vMBR.clear();
    m_vUnknown.clear();
    m_vLSP.clear();
    m_vHosts.clear();

    // Get the next action and status
    AnomalyAction eAction;
    CommonUIInfectionStatus eStatus;
    ezAnom.GetAnomalyStatus(eAction, eStatus);

    // Find out if this viral or non-viral
    bool bNonViral = ezAnom.IsNonViral();

    //
    // Gather the remediation data from this anomaly
    //

    // Get the remediation list
    ccEraser::IRemediationActionListPtr pRemediateList = ezAnom.GetRemediations();
    if( !pRemediateList )
    {
        CCTRACEE(_T("CRemediationStats::Initialize() - Failed to get the remediation action list."));
        return false;
    }

    size_t nCount = ezAnom.GetRemediationCount();

    // Now go through each remediation action
    for(size_t nCur = 0; nCur < nCount; nCur++)
    {
        // Get the current action
        ccEraser::IRemediationActionPtr pAction = ezAnom.GetRemediation(nCur);
        if( !pAction )
        {
            CCTRACEE(_T("CRemediationStats::Initialize() - Error getting remediation action for item %d. Skipping this action."), nCur);
            continue;
        }

        CEZRemediation ezRem(pAction);

        // Do not display if it is not present
        if( !ezRem.IsRemediationPresent() )
        {
            if( g_DebugOutput.IsOutputEnabled(ccLib::CDebugOutput::eInformationDebug) )
            {
                cc::IStringPtr spStrDesc;
                ATL::CAtlString strDesc;
                if( ccEraser::Succeeded(pAction->GetDescription(spStrDesc)) && spStrDesc )
                    strDesc = spStrDesc->GetStringA();
                CCTRACEI(_T("CRemediationStats::Initialize() - Remediation %s set to not present. Not displaying."), strDesc.GetString());
            }
            continue;
        }

        // Get the type for this remediation action
        ccEraser::eObjectType type = ezRem.GetType();

        REMEDIATIONPAIR remPair;

        // Get the display string
        cc::IStringPtr pStrDesc;
        if( !ezRem.GetDisplayString(pStrDesc, pcszDefaultRegKeyText) || pStrDesc == NULL )
        {
            // This may be an item we don't know about yet...let's first attempt to get the display string from
            // the path property of the remediation action, if that fails fall back to unknown text
            if( pStrDesc )
                pStrDesc.Release();

            if( ezRem.GetPathProperty(pStrDesc) && pStrDesc != NULL )
            {
                CCTRACEW(_T("CRemediationStats::Initialize() - Using the path property as the display text for this unknown remediation action. = %s"), pStrDesc->GetStringA());
                remPair.first = pStrDesc->GetStringA();
            }
            else
            {
                CCTRACEW(_T("CRemediationStats::Initialize() - Error getting display string for item %d, remediation action type is %d. Using unknown text for the display = %s."), nCur, type, pcszUnknownText ? pcszUnknownText : "Unknown");
                if( pcszUnknownText )
                    remPair.first = pcszUnknownText;
                else
                    remPair.first = "Unknown";
            }
        }
        else
        {
            remPair.first = pStrDesc->GetStringA();
        }

        // Release display string if we have one
        if( pStrDesc )
            pStrDesc.Release();

        // Get the status string
        if( !ezRem.GetStatusText(pStrDesc) || pStrDesc == NULL )
        {
            // Should always have status text stored so log this as an error
            CCTRACEE(_T("CRemediationStats::Initialize() - Error getting status string for item %d, remediation action type is %d. Using unknown text for the status = %s."), nCur, type, pcszUnknownText ? pcszUnknownText : "Unknown");
            if( pcszUnknownText )
                remPair.second = pcszUnknownText;
            else
                remPair.second = "Unknown";
        }
        else
            remPair.second = pStrDesc->GetStringA();

        if( pStrDesc )
            pStrDesc.Release();

        // What type is this
        if( type == ccEraser::InfectionRemediationActionType )
        {
            // Get the scan infection type
            ScanInfectionTypes typeInfection = ezRem.GetScanInfectionType();

            // Update appropriate vector depending on the type
            switch(typeInfection)
            {
            case FileInfectionType:
            case CompressedInfectinType:
                m_vFile.push_back(remPair);
                break;
            case MemoryInfectionType:
                m_vProc.push_back(remPair);
                break;
            case BRInfectionType:
                m_vBR.push_back(remPair);
                break;
            case MBRInfectionType:
                m_vMBR.push_back(remPair);
                break;
            default:
                CCTRACEE(_T("CRemediationStats::Initialize() - Invalid scan infection type"));
                continue;
            }
        }
        else // This is not a scan infection type 
        {
            // Save to the correct vector of items
            switch( type )
            {
            case ccEraser::FileRemediationActionType:
            case ccEraser::DirectoryRemediationActionType:
                m_vFile.push_back(remPair);
                break;
            case ccEraser::ProcessRemediationActionType:
                // We don't want to save off suspend actions, only the terminates to avoid duplicates
                if( ccEraser::IRemediationAction::Suspend != ezRem.GetOperationType() )
                {
                    m_vProc.push_back(remPair);
                }
                break;
            case ccEraser::RegistryRemediationActionType:
                m_vReg.push_back(remPair);
                break;
            case ccEraser::ServiceRemediationActionType:
                // We don't want to save off stop actions, only the remove to avoid duplicates
                if( ccEraser::IRemediationAction::Stop != ezRem.GetOperationType() )
                {
                    m_vService.push_back(remPair);
                }
                break;
            case ccEraser::COMRemediationActionType:
                m_vCOM.push_back(remPair);
                break;
            case ccEraser::BatchRemediationActionType:
                m_vBatch.push_back(remPair);
                break;
            case ccEraser::INIRemediationActionType:
                m_vINI.push_back(remPair);
                break;
            case ccEraser::HostsRemediationActionType:
                m_vHosts.push_back(remPair);
                break;
            case ccEraser::LSPRemediationActionType:
                m_vLSP.push_back(remPair);
                break;
            default:
                CCTRACEW(_T("CRemediationStats::Initialize() - Remediation action type for item %d is unknown. Type = %d."), nCur, type);
                m_vUnknown.push_back(remPair);
                continue;
            }
        }
    }

    m_bInitialized = true;

    return true;
}

DWORD CRemediationStats::GetRemediationCount(AffectedTypes type)
{
    switch( type )
    {
    case typeFile:
        return m_vFile.size();
    case typeProcess:
        return m_vProc.size();
    case typeBR:
        return m_vBR.size();
    case typeMBR:
        return m_vMBR.size();
    case typeReg:
        return m_vReg.size();
    case typeBatch:
        return m_vBatch.size();
    case typeINI:
        return m_vINI.size();
    case typeService:
        return m_vService.size();
    case typeCOM:
        return m_vCOM.size();
    case typeHosts:
        return m_vHosts.size();
    case typeLSP:
        return m_vLSP.size();
    case typeUnkown:
        return m_vUnknown.size();
    default:
        return 0;
    }
}

CRemediationStats::REMEDIATIONSTATSVEC CRemediationStats::GetItems(AffectedTypes type)
{
    switch( type )
    {
    case typeFile:
        return m_vFile;
    case typeProcess:
        return m_vProc;
    case typeBR:
        return m_vBR;
    case typeMBR:
        return m_vMBR;
    case typeReg:
        return m_vReg;
    case typeBatch:
        return m_vBatch;
    case typeINI:
        return m_vINI;
    case typeService:
        return m_vService;
    case typeCOM:
        return m_vCOM;
    case typeHosts:
        return m_vHosts;
    case typeLSP:
        return m_vLSP;
    case typeUnkown:
        return m_vUnknown;
    default:
        CCTRACEE(_T("CRemediationStats::GetItems() - Invalid type %d"), type);
        // return an empty vector
        REMEDIATIONSTATSVEC vTemp;
        vTemp.clear();
        return vTemp;
    }
}
