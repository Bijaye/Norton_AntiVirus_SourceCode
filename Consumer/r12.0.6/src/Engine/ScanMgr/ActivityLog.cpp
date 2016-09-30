
#include "Stdafx.h"
#include "Resource.h"
#include "ActivityLog.h"
#include "CommonUIInterface.h"
#include "allnavevents.h"
#include "ccModuleId.h"
#include "AVccModuleId.h"
#include "navversion.h"             // Toolbox
#include "ResourceHelper.h"
#include "ccSymModuleLifetimeMgrHelper.h"
#include "AVRESBranding.h"

void DamageToHighMedLow( DWORD dwDamageValue, ATL::CAtlString& strValue )
{
    switch( dwDamageValue )
    {
    case 0:
    case 1:
    case 2:
        strValue.LoadString(IDS_LOW);
        break;
    case 3:
        strValue.LoadString(IDS_MEDIUM);
        break;
    case 4:
    case 5:
    default:
        strValue.LoadString(IDS_HIGH);
        break;
    }
}

CActivityLog::CActivityLog() : m_ScanType(AType_ManualScanner)
{
}

CActivityLog::~CActivityLog()
{
}


bool CActivityLog::LogScanStart(const ::std::string strScanName)
{
	// Return error if we can't log.
	if( GetLogger() == false )
		return false;

    CFileAPI fileapi;
    fileapi.SwitchAPIToANSI ();

	// Create an event.
    CEventData eventData;

	// Populate event
    eventData.SetData( AV::Event_Base_propType, AV::Event_ID_ScanAction );
    eventData.SetData( AV::Event_ScanAction_propAction, AV::Event_ScanAction_ScanStarted ); 
    eventData.SetData( AV::Event_ScanAction_propFeature, AV_MODULE_ID_NAVW );
    eventData.SetData( AV::Event_ScanAction_propTaskName, strScanName.c_str() );

	// Submit it.
	return SYM_SUCCEEDED( m_pLogger->BroadcastAvEvent(eventData) );
}

bool CActivityLog::LogScanAbort( const ::std::string strScanName )
{
	// Return error if we can't log.
	if( GetLogger() == false )
		return false;

    CFileAPI fileapi;
    fileapi.SwitchAPIToANSI ();

	// Create an event.
    CEventData eventData;

	// Populate event
	eventData.SetData( AV::Event_Base_propType, AV::Event_ID_ScanAction );
	eventData.SetData( AV::Event_ScanAction_propAction, AV::Event_ScanAction_ScanAborted ), 
    eventData.SetData( AV::Event_ScanAction_propFeature, AV_MODULE_ID_NAVW );
    eventData.SetData( AV::Event_ScanAction_propTaskName, strScanName.c_str() );

	// Submit it.
	return SYM_SUCCEEDED( m_pLogger->BroadcastAvEvent(eventData) );
}

bool CActivityLog::LogScanError()
{
	// Return error if we can't log.
	if( GetLogger() == false )
		return false;

    CFileAPI fileapi;
    fileapi.SwitchAPIToANSI ();

	// Create an event.
    CEventData eventData;

	// Populate event
	eventData.SetData( AV::Event_Base_propType, AV::Event_ID_Error );
	eventData.SetData( AV::Event_Error_propMessage, CResourceHelper::LoadString ( IDS_LOG_SCAN_ERROR, g_hInstance ).c_str() );

	eventData.SetData( AV::Event_Error_propErrorID, (long)IDS_LOG_SCAN_ERROR );  
	eventData.SetData( AV::Event_Error_propModuleID, (long)AV_MODULE_ID_NAVW ); 

    // Product version
    eventData.SetData( AV::Event_Error_propProductVersion, m_Version.GetPublicRevision ());

    // Submit it.
    return SYM_SUCCEEDED( m_pLogger->BroadcastAvEvent(eventData) );
}

bool CActivityLog::LogProcessTermination(ccEraser::IRemediationAction* pProcessRemediation, LPCTSTR pcszProdName, LPCSTR pcszThreatName)
{
    if( pcszProdName == NULL || pcszThreatName == NULL )
    {
        CCTRACEE (_T("CActivityLog::LogProcessTermination() - Product or threat name is NULL"));
        return false;
    }

    CFileAPI fileapi;
    fileapi.SwitchAPIToANSI ();

    // Create a new logger object since this will be happening on any of the scan threads
    // and our main logger object is created on the main scan manager thread
    AV::IAvEventFactoryPtr pLogger;
    if (SYM_FAILED (AV::NAVEventFactory_IAVEventFactory::CreateObject (GETMODULEMGR(), &pLogger)) ||
        pLogger.m_p == NULL )
    {
        CCTRACEE ( _T("CActivityLog::LogProcessTermination() - Could not create IAvFactory object. - %d"), ::GetLastError() );
        return false;
    }

    CEZRemediation ezProcRem(pProcessRemediation);

    // Create an event.
    CEventData eventData;

    // Populate event
	eventData.SetData( AV::Event_Base_propType, AV::Event_ID_ScanAction );
    eventData.SetData( AV::Event_ScanAction_propAction, AV::Event_ScanAction_MemorySideEffect );
    eventData.SetData( AV::Event_ScanAction_propFeature, AV_MODULE_ID_NAVW );

    // Find out if this is before the termination attempt, or a success\failure log event
    ccEraser::eResult eRemediationResult = ccEraser::False;
    bool bHaveRemResult = ezProcRem.GetRemediationResult(eRemediationResult);

    // This is the details column text
    if( !bHaveRemResult )  // means this is an attempt
        eventData.SetData ( AV::Event_ScanAction_propTaskName, CResourceHelper::LoadString ( IDS_LOG_PROCESS_TERMINATE_ATTEMPT_TITLE, g_hInstance ).c_str());
    else
        eventData.SetData ( AV::Event_ScanAction_propTaskName, CResourceHelper::LoadString ( IDS_LOG_PROCESS_TERMINATE_RESULT_TITLE, g_hInstance ).c_str());

    // Bottom display text
    CAtlString strText;
    UINT nResID = 0;
    if( !bHaveRemResult )      // Logging an attempt
        nResID = IDS_LOG_PROCESS_TERMINATE_ATTEMPT;
    else if( ccEraser::Succeeded(eRemediationResult) ) // Logging a successful termination
        nResID = IDS_LOG_PROCESS_TERMINATE_SUCCEEDED;
    else                // Logging a failed termination
        nResID = IDS_LOG_PROCESS_TERMINATE_FAILED;

    // Get the process name
    CString strProcName;
    cc::IStringPtr pStrProcName;
    ezProcRem.GetDisplayString(pStrProcName);
    if( pStrProcName != NULL )
        strProcName = pStrProcName->GetStringA();

    strText.Format(nResID, pcszProdName, strProcName, pcszThreatName);
    eventData.SetData( AV::Event_ScanAction_Display, (LPCTSTR)strText);

    // Submit it.
	return SYM_SUCCEEDED( pLogger->BroadcastAvEvent(eventData) );
}

bool CActivityLog::LogScanSummary(CScanManagerCounts* pScanCounts, DWORD dwScanTime, const ::std::string strScanName)
{
	// Return error if we can't log.
	if( GetLogger() == false )
		return false;

    if( pScanCounts == NULL )
    {
        CCTRACEE ( _T("CActivityLog::LogScanSummary() - Counts are invalid"));
        return false;
    }

    CFileAPI fileapi;
    fileapi.SwitchAPIToANSI ();

	// Create an event.
    CEventData eventData;

	eventData.SetData( AV::Event_Base_propType, AV::Event_ID_ScanAction );
    eventData.SetData( AV::Event_ScanAction_propAction, AV::Event_ScanAction_ScanCompleted ), 
	eventData.SetData( AV::Event_ScanAction_propFeature, AV_MODULE_ID_NAVW );
	eventData.SetData( AV::Event_ScanAction_propTaskName, strScanName.c_str() );

    eventData.SetData( AV::Event_ScanAction_MBRsRepaired,			pScanCounts->GetItemCount(CScanManagerCounts::MBRRepaired) );
	eventData.SetData( AV::Event_ScanAction_MBRsTotalInfected,    	pScanCounts->GetItemCount(CScanManagerCounts::MBRInfected) );
	eventData.SetData( AV::Event_ScanAction_MBRsTotalScanned,    	pScanCounts->GetItemCount(CScanManagerCounts::MBRScanned) );
	eventData.SetData( AV::Event_ScanAction_BootRecsRepaired,     	pScanCounts->GetItemCount(CScanManagerCounts::BRRepaired) );
	eventData.SetData( AV::Event_ScanAction_BootRecsTotalInfected,	pScanCounts->GetItemCount(CScanManagerCounts::BRInfected) );
	eventData.SetData( AV::Event_ScanAction_BootRecsTotalScanned,	pScanCounts->GetItemCount(CScanManagerCounts::BRScanned) );
	eventData.SetData( AV::Event_ScanAction_FilesRepaired,        	pScanCounts->GetItemCount(CScanManagerCounts::Repaired) );
	eventData.SetData( AV::Event_ScanAction_FilesQuarantined,     	pScanCounts->GetItemCount(CScanManagerCounts::Quarantined) );
	eventData.SetData( AV::Event_ScanAction_FilesDeleted,         	pScanCounts->GetTotalItemsDeleted() );
	eventData.SetData( AV::Event_ScanAction_FilesTotalInfected,   	pScanCounts->GetTotalItemsDetected() );
	eventData.SetData( AV::Event_ScanAction_FilesTotalScanned,   	pScanCounts->GetItemCount(CScanManagerCounts::FilesScanned) );
    eventData.SetData( AV::Event_ScanAction_FilesExcluded,   	    pScanCounts->GetItemCount(CScanManagerCounts::NonViralExcluded) );
    eventData.SetData( AV::Event_ScanAction_ScanTime,   	        dwScanTime);

	// Submit it.
    return SYM_SUCCEEDED( m_pLogger->BroadcastAvEvent(eventData) );
}

void CActivityLog::SetScanType( int nType )
{
    m_ScanType = static_cast<ActivityScanType>(nType);
}

bool CActivityLog::LogAllInfections(ccEraser::IAnomalyList* pAnomList)
{
	// Return error if we can't log.
	if( GetLogger() == false )
    {
        CCTRACEE(_T("CActivityLog::LogAllInfections() - Failed to get the logger object"));
		return false;
    }

    if( pAnomList == NULL )
    {
        CCTRACEE(_T("CActivityLog::LogAllInfections() - Anomaly list is null"));
        return false;
    }

    size_t nTotal = 0;
    ccEraser::eResult eRes = pAnomList->GetCount(nTotal);

    if( ccEraser::Failed(eRes) )
    {
        CCTRACEE(_T("CActivityLog::LogAllInfections() - Failed to get anomaly list count. eResult = %d"), eRes);
        return false;
    }

    CFileAPI fileapi;
    fileapi.SwitchAPIToANSI ();

    // Loop through each of the anomalies in the list
	for(size_t nCur = 0; nCur < nTotal; nCur++)
	{
        ccEraser::IAnomalyPtr pCurAnomaly;
        eRes = pAnomList->GetItem(nCur, pCurAnomaly);

        if( ccEraser::Failed(eRes) )
        {
            CCTRACEE(_T("CActivityLog::LogAllInfections() - Failed to get anomaly item %d. eResult = %d"), nCur, eRes);
            continue;
        }

        CEZAnomaly ezAnom;
        if( !ezAnom.Initialize(pCurAnomaly) )
        {
            CCTRACEE(_T("CActivityLog::LogAllInfections() - Failed to get anomaly data for item %d."), nCur);
            continue;
        }

        // Create an event.
        CEventData eventData;

        // Add properties.
        eventData.SetData( AV::Event_Base_propType, AV::Event_ID_Threat );
        eventData.SetData( AV::Event_Threat_propEventSubType, AV::Event_Threat );
        eventData.SetData( AV::Event_Threat_propFeature, AV_MODULE_ID_NAVW );

        // Set the type property to either file, BR, MBR, or Memory
        if( ezAnom.Is9xMemAnomaly() )
            eventData.SetData( AV::Event_Threat_propObjectType, AV::Event_Threat_ObjectType_Memory);
        else if( ezAnom.IsMBRAnomaly() )
            eventData.SetData( AV::Event_Threat_propObjectType, AV::Event_Threat_ObjectType_MasterBootRecord);
        else if( ezAnom.IsBootRecordAnomaly() )
            eventData.SetData( AV::Event_Threat_propObjectType, AV::Event_Threat_ObjectType_BootRecord);
        else
            eventData.SetData( AV::Event_Threat_propObjectType, AV::Event_Threat_ObjectType_File);

        // Definitions revision
        eventData.SetData( AV::Event_Threat_propDefsRevision, m_strDefsRevision.c_str() );

        // Product version
        eventData.SetData( AV::Event_Threat_propProductVersion, m_Version.GetPublicRevision ());


        // Fill in the threat specific data
        CEventData threat;
        cc::IStringPtr pccStrName;
        ezAnom.GetName(pccStrName);
        threat.SetData ( AV::Event_ThreatEntry_propVirusName, pccStrName->GetStringA());
        DWORD dwVID = ezAnom.GetVID();
        threat.SetData ( AV::Event_ThreatEntry_propVirusID, dwVID);

        // Fill in the status. (Action taken:)
        AnomalyAction eAction;
        CommonUIInfectionStatus eStatus;
        ezAnom.GetAnomalyStatus(eAction, eStatus);
        BYTE byAction = GetLogAction( GetActivityState(eStatus, ezAnom.IsNonViral()) );
        threat.SetData ( AV::Event_ThreatEntry_propActionCount, 1 );
        threat.SetData ( AV::Event_ThreatEntry_propActionData, &byAction, 1 );

        // Fill in the threat categories
        CAtlString strCats;
        cc::IIndexValueCollectionPtr pCats = ezAnom.GetCategories();
        if( pCats != NULL )
        {
            size_t nCatCount = pCats->GetCount();
            for( size_t nCatIndex=0; nCatIndex < nCatCount; nCatIndex++ )
            {
                char szCurCat[65] = {0};
                DWORD dwCurCategory = 0;
                if( pCats->GetValue(nCatIndex, dwCurCategory) )
                {
                    _snprintf(szCurCat, 65, "%u ", dwCurCategory);
                    strCats += szCurCat;
                }
            }
            threat.SetData ( AV::Event_ThreatEntry_propThreatCatCount, nCatCount );
            threat.SetData ( AV::Event_ThreatEntry_propThreatCategories, (LPCTSTR)strCats );
        }

        // Get the threat matrix text for non-virals
        if( ezAnom.IsNonViral() )
        {
            // Overall
            strCats.Empty();
            DamageToHighMedLow(ezAnom.GetDamageFlagAverage(), strCats);
            threat.SetData ( AV::Event_ThreatEntry_propThreatMatrixOverall, (LPCTSTR)strCats );

            // Performance
            strCats.Empty();
            DamageToHighMedLow(ezAnom.GetDamageFlagValue(ccEraser::IAnomaly::Performance), strCats);
            threat.SetData ( AV::Event_ThreatEntry_propThreatMatrixPerformance, (LPCTSTR)strCats );

            // Privacy
            strCats.Empty();
            DamageToHighMedLow(ezAnom.GetDamageFlagValue(ccEraser::IAnomaly::Privacy), strCats);
            threat.SetData ( AV::Event_ThreatEntry_propThreatMatrixPrivacy, (LPCTSTR)strCats );

            // Removal
            strCats.Empty();
            DamageToHighMedLow(ezAnom.GetDamageFlagValue(ccEraser::IAnomaly::Removal), strCats);
            threat.SetData ( AV::Event_ThreatEntry_propThreatMatrixRemoval, (LPCTSTR)strCats );

            // Stealth
            strCats.Empty();
            DamageToHighMedLow(ezAnom.GetDamageFlagValue(ccEraser::IAnomaly::Stealth), strCats);
            threat.SetData ( AV::Event_ThreatEntry_propThreatMatrixStealth, (LPCTSTR)strCats );
        }

        // Fill in the source and description
        CEventData SubFile;

        // Set the source
        CAtlString strSource;
        UINT nResSourceID = IDS_LOG_SOURCE_MANUAL;
        switch( m_ScanType )
        {
        case AType_EmailScanner:
            nResSourceID = IDS_LOG_SOURCE_EMAIL;
            break;
        case AType_IMScanner:
            nResSourceID = IDS_LOG_SOURCE_IM;
            break;
        case AType_OfficeScanner:
            nResSourceID = IDS_LOG_SOURCE_OFFICE;
            break;
        }
        strSource.Format(nResSourceID);
        SubFile.SetData ( AV::Event_ThreatSubFile_propFileName, (LPCTSTR)strSource );

        // Build the Description
        CAtlString strDescription;

        // First load up the header
        UINT nResHeaderID = IDS_LOG_DESCRIPTION_HEADER_HANDLED;
        if( eStatus == InfectionStatus_Unhandled )
            nResHeaderID = IDS_LOG_DESCRIPTION_HEADER_NOT_HANDLED;

        strDescription.LoadString(nResHeaderID);

        // Get the default registry key text and unknown text to use from our resources so it
        // will be localized, reusing some strings that we're done with
        strSource.Empty();
        strCats.Empty();
        strSource.LoadString(IDS_UNKOWN_TEXT);
        strCats.LoadString(IDS_DEFAULT_REGISTRY_KEY_TEXT);
        CRemediationStats* pRemStats = ezAnom.GetRemediationStatistics(strSource, strCats);
        BuildAnomalyDescription(pRemStats, eStatus, strDescription);
        SubFile.SetData ( AV::Event_ThreatSubFile_propFileDescription, (LPCTSTR)strDescription );

        // Save
        CEventData AllThreats;
        AllThreats.SetNode ( 0, threat );

        SubFile.SetNode ( AV::Event_ThreatSubFile_propThreatData, AllThreats);

        CEventData AllFiles;
        AllFiles.SetNode ( 0, SubFile );
        eventData.SetNode ( AV::Event_Threat_propSubFileData, AllFiles );

        if( SYM_FAILED( m_pLogger->BroadcastAvEvent(eventData) ) )
        {
            CCTRACEE(_T("CActivityLog::LogAllInfections() - Failed to BroadcastAVEvent() for item %d failed."), nCur);
        }
	}
	
	return true;
}

void CActivityLog::BuildAnomalyDescription(CRemediationStats* pRemStats, CommonUIInfectionStatus eAnomalyStatus, CAtlString& strDescription)
{
    if( pRemStats == NULL )
    {
        CCTRACEE(_T("CActivityLog::BuildAnomalyDescription() - The remediation statistics are null."));
        return;
    }

    // Enumerate through all possible remediation types
    for( int nCurType = typeFirst; nCurType <= typeLast ; nCurType++ )
    {
        // Make sure we have items of this type associated with this anomaly
        DWORD dwCount = pRemStats->GetRemediationCount(static_cast<AffectedTypes>(nCurType));
        if( dwCount == 0 )
            continue;

        CAtlString strTemp;

        UINT nResID;
        switch(nCurType)
        {
        case typeFile:
            nResID = IDS_LOG_AFFECTED_FILES;
            break;
        case typeProcess:
            nResID = IDS_LOG_AFFECTED_PROCS;
            break;
        case typeBR:
            nResID = IDS_LOG_AFFECTED_BRS;
            break;
        case typeMBR:
            nResID = IDS_LOG_AFFECTED_MBRS;
            break;
        case typeReg:
            nResID = IDS_LOG_AFFECTED_REGKEYS;
            break;
        case typeBatch:
            nResID = IDS_LOG_AFFECTED_BATCH;
            break;
        case typeINI:
            nResID = IDS_LOG_AFFECTED_INI;
            break;
        case typeService:
            nResID = IDS_LOG_AFFECTED_SERVICES;
            break;
        case typeCOM:
            nResID = IDS_LOG_AFFECTED_COM;
            break;
        case typeUnkown:
            nResID = IDS_LOG_AFFECTED_UNKNOWN;
            break;
        default:
            CCTRACEE(_T("CActivityLog::BuildAnomalyDescription() - Unknown affected area type."));
            continue;
        }

        // Now get these item types and add them
        CRemediationStats::REMEDIATIONSTATSVEC vItems = pRemStats->GetItems(static_cast<AffectedTypes>(nCurType));

        // Format the heading string for these types
        strTemp.Format(nResID, vItems.size());

        strDescription += strTemp;

        CRemediationStats::REMEDIATIONSTATSVEC::iterator Iter;
        for(Iter = vItems.begin(); Iter != vItems.end(); Iter++ )
        {
            // First the remediation description
            strDescription += (*Iter).first;

            // If the state is not unhandled or can't delete then we will also append the status
            if( eAnomalyStatus != InfectionStatus_Unhandled && eAnomalyStatus != InfectionStatus_CantDelete )
            {
                strDescription += " - ";
                strDescription += (*Iter).second;
            }

            strDescription += "\n";
        }

        strDescription += "\n";
    }
}

bool CActivityLog::LogOEHInfection(const char* szWormPath, ActivityState State)
{
    // Return error if we can't log.
    if( GetLogger() == false )
        return false;

    CFileAPI fileapi;
    fileapi.SwitchAPIToANSI ();

    // Create an event.
    CEventData eventData;

    // Add properties.
    eventData.SetData( AV::Event_Base_propType, AV::Event_ID_Threat );
    eventData.SetData( AV::Event_Threat_propEventSubType, AV::Event_Threat );
    eventData.SetData( AV::Event_Threat_propFeature, AV_MODULE_ID_NAVW );
    eventData.SetData( AV::Event_Threat_propObjectType, AV::Event_Threat_ObjectType_File);

    // Defs revision
    eventData.SetData( AV::Event_Threat_propDefsRevision, m_strDefsRevision.c_str() );

    // Product version
    eventData.SetData( AV::Event_Threat_propProductVersion, m_Version.GetPublicRevision ());

    CEventData SubFile;
    SubFile.SetData ( AV::Event_ThreatSubFile_propFileName, szWormPath );

    CEventData threat;
    threat.SetData ( AV::Event_ThreatEntry_propVirusName, CResourceHelper::LoadString (IDS_LOG_OEH_VIRUSNAME, g_hInstance).c_str() );

    // Get the activity state of the infection.
    BYTE byAction = GetLogAction( State );
    threat.SetData ( AV::Event_ThreatEntry_propActionCount, 1 );
    threat.SetData ( AV::Event_ThreatEntry_propActionData, &byAction, 1 );

    // Save
    CEventData AllThreats;
    AllThreats.SetNode ( 0, threat );

    SubFile.SetNode ( AV::Event_ThreatSubFile_propThreatData, AllThreats);

    CEventData AllFiles;
    AllFiles.SetNode ( 0, SubFile );
    eventData.SetNode ( AV::Event_Threat_propSubFileData, AllFiles );

    return SYM_SUCCEEDED( m_pLogger->BroadcastAvEvent(eventData) );
}

ActivityState CActivityLog::GetActivityState(CommonUIInfectionStatus eStatus, bool bNonViral)
{
    switch (eStatus)
    {
    case InfectionStatus_Repaired:
        return AState_Repaired;
    case InfectionStatus_Deleted:
        if(bNonViral)
            return AState_Removed;
        else
            return AState_Deleted;
    case InfectionStatus_Quarantined:
        return AState_Quarantined;
    case InfectionStatus_Repair_Failed:
        return AState_Repair_Failed;
    case InfectionStatus_Quarantine_Failed:
        return AState_Quarantine_Failed;
    case InfectionStatus_Delete_Failed:
       return AState_Delete_Failed;
    case InfectionStatus_Excluded:
       return AState_Excluded;
    case InfectionStatus_Exclude_Failed:
       return AState_Exclude_Failed;
    default:
        return AState_Infected;
    }
}

bool CActivityLog::GetLogger()
{
    // If we have already created the logger object, return true
	if( m_pLogger.m_p != NULL )
		return true;
	
	// If we are in safe mode, we can not do logging so return false
	if( GetSystemMetrics(SM_CLEANBOOT) != 0 )
		return false;

	// Create the logger object.
    if (SYM_FAILED (AV::NAVEventFactory_IAVEventFactory::CreateObject (GETMODULEMGR(), &m_pLogger)) ||
        m_pLogger.m_p == NULL )
    {
        CCTRACEE ( _T("CActivityLog - Could not create IAvFactory object. - %d"), ::GetLastError() );
        m_pLogger = NULL;
        return false;
    }

	// If we created a logger object return true
	return true;
}

// Convert the ActivityState into a loggable action value.
//
long CActivityLog::GetLogAction(ActivityState AState)
{
	long lAction = 0;

	switch(AState)
	{
	case AState_Repaired:
        lAction = AV::Event_Action_Repaired;
		break;
	case AState_Repair_Failed:
        lAction = AV::Event_Action_RepairFailed;
		break;
	case AState_Quarantined:
        lAction = AV::Event_Action_Quarantined;
		break;
	case AState_Quarantine_Failed:
        lAction = AV::Event_Action_QuarantinedFailed;
		break;
	case AState_Deleted:
        lAction = AV::Event_Action_Deleted;
		break;
	case AState_Delete_Failed:
        lAction = AV::Event_Action_DeleteFailed;
		break;
	case AState_Auto_Deleted:
        lAction = AV::Event_Action_Auto_Deleted;
		break;
	case AState_Infected:
        lAction = AV::Event_Action_NoActionTaken;
		break;
	case AState_Allowed:
        lAction = AV::Event_Action_Allowed;
		break;
	case AState_Blocked:
        lAction = AV::Event_Action_Blocked;
		break;
	case AState_Authorized:
        lAction = AV::Event_Action_Authorized;
        break;
    case AState_Excluded:
        lAction = AV::Event_Action_Excluded;
		break;
    case AState_Exclude_Failed:
        lAction = AV::Event_Action_Exclude_Failed;
        break;
    case AState_Removed:
        lAction = AV::Event_Action_Removed;
        break;
    }

    return lAction;
}

void CActivityLog::SetDefsRevision ( const ::std::string strDefsRevision  )
{
    m_strDefsRevision = strDefsRevision;
}