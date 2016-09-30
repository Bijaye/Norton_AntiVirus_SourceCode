#include "StdAfx.h"
#include "EraserScanSink.h"

// IScanSink overrides.
SCANSTATUS CEraserScanSink::OnBusy()
{
	if(m_bAbort)
		return SCAN_ABORT;

	return SCAN_OK;	
}

SCANSTATUS CEraserScanSink::OnError(IScanError* pError)
{
	return SCAN_OK;	
}

SCANSTATUS CEraserScanSink::OnNewItem(const char* pszItem)
{
	if(m_bAbort)
		return SCAN_ABORT;

	return SCAN_OK;
}

SCANSTATUS CEraserScanSink::OnNewFile(const char* pszLongName, const char* pszShortName)
{
	if(m_bAbort)
		return SCAN_ABORT;

	return SCAN_OK;	
}

SCANSTATUS CEraserScanSink::OnNewDirectory(const char* pszLongName, const char* pszShortName)
{
	if(m_bAbort)
		return SCAN_ABORT;

	return SCAN_OK;	
}

SCANSTATUS CEraserScanSink::OnInfectionFound(IScanInfection* pInfection)
{
	return SCAN_OK;	
}

SCANSTATUS CEraserScanSink::OnRepairFailed(IScanInfection* pInfection)
{
    return SCAN_OK;
}

SCANSTATUS CEraserScanSink::OnRemoveMimeComponent(IScanInfection* pInfection, char* pszMessage,
    int iMessageBufferSize)
{
    return SCAN_OK;
}

SCANSTATUS CEraserScanSink::LockVolume(const char* szVolume, bool bLock)
{
    return SCAN_OK;
}

SCANSTATUS CEraserScanSink::OnCleanFile(const char* pszFileName, const char* pszTempFileName)
{
    return SCAN_OK;
}

// IScanSink2 overrides
SCANSTATUS CEraserScanSink::OnBeginThread()
{
    return SCAN_OK;
}

SCANSTATUS CEraserScanSink::OnEndThread()
{
    return SCAN_OK;
}

// IScanProperties overrides
int CEraserScanSink::GetProperty( const char* szProperty, int iDefault )
{
	if( strcmp( szProperty, SCAN_COMPRESSED_FILES ) == 0 )
		return 0; // Don't scan compressed files.

	if( strcmp( szProperty, SCAN_DISABLE_AP ) == 0 )
		return 0; // Don't disable AP because we already do it for ourselves.

	return iDefault;
}

const char * CEraserScanSink::GetProperty( const char* szProperty, const char * szDefault )
{
	return szDefault;
}

// IScanItems
int CEraserScanSink::GetScanItemCount()
{
	return (int) m_vItems.size();
}

const char * CEraserScanSink::GetScanItemPath( int iIndex )
{
	return m_vItems[iIndex].c_str();
}

// ICallback implementation.
ccEraser::eResult CEraserScanSink::PreDetection(ccEraser::IDetectionAction* pDetectAction, const ccEraser::IContext* pContext)
{
    if (m_bAbort)
    {
        return ccEraser::Abort;
    }

    return ccEraser::Continue;
}

ccEraser::eResult CEraserScanSink::PostDetection(ccEraser::IDetectionAction* pDetectAction, ccEraser::eResult eDetectionResult, const ccEraser::IContext* pContext)
{
    if (m_bAbort)
    {
        return ccEraser::Abort;
    }
    
    // If logging is enabled log out successful detections
    if( g_DebugOutput.IsOutputEnabled(ccLib::CDebugOutput::eInformationDebug) )
    {
        cc::IKeyValueCollectionPtr pDetectProps;
        if( ccEraser::Failed(pDetectAction->GetProperties(pDetectProps)) || pDetectProps == NULL )
        {
            CCTRACEE(_T("CScanManager::PostDetection() - Failed to get detection action properties"));
            return ccEraser::Continue;
        }

        // Get the state
        DWORD dwState = 0;
        if( !pDetectProps->GetValue(ccEraser::IDetectionAction::State, dwState) )
        {
            CCTRACEE(_T("CScanManager::PostDetection() - Failed to get detection action state"));
            return ccEraser::Continue;
        }

        // See if this action was detected
        if( dwState == ccEraser::IDetectionAction::Detected )
        {
            cc::IStringPtr pStrDesc;
            if( ccEraser::Succeeded(pDetectAction->GetDescription(pStrDesc)) && pStrDesc != NULL )
            {
                CCTRACEI(_T("CScanManager::PostDetection() - DETECTED: %s"), pStrDesc->GetStringA());
            }
            else
            {
                CCTRACEE(_T("CScanManager::PostDetection() - Failed to get generic detection action description for detected action"));
            }
        }
    }

    return ccEraser::Success;
}

ccEraser::eResult CEraserScanSink::OnAnomalyDetected(ccEraser::IAnomaly* pAnomaly, const ccEraser::IContext* pContext)
{
    if (m_bAbort == true)
        return ccEraser::Abort;
    
    // make sure our Anomaly pointer is good
    if(!pAnomaly)
        return ccEraser::Continue;

    // get the remediation list
    ccEraser::IRemediationActionListPtr pRemediationList;
    pAnomaly->GetRemediationActions(pRemediationList);
    
    // make sure the remediation list is good
    if(!pRemediationList)
        return ccEraser::Continue;
    
    // figure out how many remediations there are
    size_t nCount = 0;
    ccEraser::eResult eRes = pRemediationList->GetCount(nCount);
    if( ccEraser::Failed(eRes) )
        return ccEraser::Continue;
    
    // for each remdiation check if its a memory resident infection.
    for( size_t nItem=0; nItem<nCount; nItem++ )
    {
        ccEraser::IRemediationActionPtr pRem;
        if( ccEraser::Failed(pRemediationList->GetItem(nItem, pRem)) )
            continue;

        ccEraser::eObjectType type;
        if( ccEraser::Failed(eRes = pRem->GetType(type)) )
            continue;

        if( type == ccEraser::ProcessRemediationActionType || type == ccEraser::ServiceRemediationActionType )
        {
            bool bIsPresent = false;
            if( ccEraser::Failed(eRes = pRem->IsPresent(bIsPresent)) )
                continue;

            if( bIsPresent )
            {
                cc::IStreamPtr pUndoInfo = NULL;
                pRem->GetUndoInformation(pUndoInfo);
                pRem->Remediate();
            }
        }
    }

    return ccEraser::Continue;
}

// Abort scan mechanisms
void CEraserScanSink::SetAbortFlag()
{
	m_bAbort = true;
}

bool CEraserScanSink::IsAborted()
{
	return m_bAbort;
}
