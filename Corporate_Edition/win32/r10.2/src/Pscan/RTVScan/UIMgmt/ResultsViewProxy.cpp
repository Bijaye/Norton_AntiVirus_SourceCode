// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
//Copyright (c) 2006 Symantec Corporation. All rights reserved.

#include "stdafx.h"
#include <comdef.h>
#include "ResultsViewProxy.h"
#include "ResultsViewCOMCallback.h"
#include "threatqueue.h"
#include "tokenmanager.h"
#include "UIManager.h"
#include "OSUtils.h"
#include "ProgressBlockConfig.h"
#include "ProgressBlockSafeArrayCreatorImpl.h"
#include "safearraycreator.h"

const CLSID CLSID_ResultsViewCOMAdapter = {0x84AC6BE7,0x8CF2,0x4e67,0xA8,0x0E,0x32,0xAC,0xD3,0xD7,0xC3,0x81};
const IID	IID_IResultsViewCOMAdapter  = {0x646C8A12,0x4A6B,0x425c,0x97,0xF4,0xC2,0x2D,0xDA,0x93,0xB7,0x44};


DWORD Progress(CScanStatus* block,PPROGRESSBLOCK pb);
DWORD FoundVirus(CScanStatus* block,PEVENTBLOCK inf);

/** Creates a scan results dialog in the correct session.
  * @param dwKey a key value created and used by CUIManager
  * @see CreateResultsView() in ScanDlgs
  */
DWORD CResultsViewProxy::CreateDlg(CScanStatus& objScanStatus, DWORD dwKey)
{
    if( true == m_bScanStopped )
        return ERROR_ACCESS_DENIED;

	// Store the key and session id.
	m_dwKey		  = dwKey;
	m_dwSessionId = SessionIdValidator(objScanStatus.GetSessionID());

	// Create the status object.
    memset(&m_objStatDlg,0,sizeof(STATDLG));
 
    m_objStatDlg.block       = &objScanStatus;
    m_objStatDlg.cbProgress  = objScanStatus.cbProgress;
    objScanStatus.cbProgress = Progress;
    m_objStatDlg.cbVirus     = objScanStatus.cbVirus;
    objScanStatus.cbVirus    = FoundVirus;


    //create the com object for the notification dlg wrapper
    CSessionMoniker objSessionMoniker(m_dwSessionId);
    DWORD dwRet = objSessionMoniker.CreateMoniker(CLSID_ResultsViewCOMAdapter);
    if( SUCCEEDED(dwRet) )
    {
        dwRet = objSessionMoniker.BindToObject((void**)&m_IClassFactoryPtr);
        if( ERROR_SUCCESS == dwRet )
		{
			dwRet = m_IClassFactoryPtr->CreateInstance(NULL,
														IID_IResultsViewCOMAdapter,
														(void**)&m_pCOMAdapter);
		}
    }

    if( m_pCOMAdapter != NULL )
    {
		// Prepare parameters for the CreateDlg() call.
		CComBSTR bstrScanKey(m_objStatDlg.block->GetScanConfigSubKey());
		DWORD dwFlags = 0;
        DWORD dwType = LOGGER_Real_Time == objScanStatus.logger ? RV_TYPE_AUTOPROTECT : RV_TYPE_SCAN;
		if (m_objStatDlg.block->Flags & BL_REMOTE_SCAN)
			dwFlags |= RV_FLAGS_ADMIN;
		if (LOGGER_Scheduled == m_objStatDlg.block->logger)
			dwFlags |= RV_FLAGS_SCHD_SCAN;
        if ( m_objStatDlg.block->GetIsAdminDefinedScan() )
            dwFlags |= RV_FLAGS_ADMIN_SCAN;

		// Create the dialog.
		dwRet = m_pCOMAdapter->CreateDlg(m_dwKey,
										 (ULONG_PTR)m_objStatDlg.block->GetScanConfigRootKey(),
										 bstrScanKey,
										 dwFlags,
										 m_objStatDlg.block->dwDelayCount,
                                         dwType,
                                         m_objStatDlg.block->GetScanStartTickCount());

        try
        {
	        m_pCOMCallback = new CComObject<CResultsViewCOMCallback>;
		    if( m_pCOMCallback != NULL )
		    {
			    //initialize the class so the callbacks return more than just not implemented
				CResultsViewCOMCallback *pobjCallback =
					(CResultsViewCOMCallback*)m_pCOMCallback.GetInterfacePtr();

				pobjCallback->SetScanInfo(m_dwKey, dwType);
			    pobjCallback->EstablishTrust();
			    dwRet = m_pCOMAdapter->RegisterCallback(m_pCOMCallback);
		    }
        }
        VP_CATCH_MEMORYEXCEPTIONS
        (
            dwRet = ERROR_MEMORY;
		)
    }

    //if there was a failure cleanup
    if( dwRet != ERROR_SUCCESS)
    {
        dprintf("Creation of status dialog failed [%08X]\n",dwRet);
        objScanStatus.cbProgress = m_objStatDlg.cbProgress;
        objScanStatus.cbVirus    = m_objStatDlg.cbVirus;
    }

	// TODO:  Test whether this should be done if dwRet is failure.
    objScanStatus.AddRef();

    return dwRet;
}


/** Adds info on a threat to the scan results dialog.
  * @see AddLogLineToResultsView() in ScanDlgs
  */
DWORD CResultsViewProxy::AddLogLine(LPARAM lData, const PEVENTBLOCK pEventBlock)
{
	// Check that we've created the COM object already.
	if (NULL == m_pCOMAdapter || NULL == pEventBlock )
    {
        SAVASSERT(!"Invalid Params for CResultsViewProxy::AddLogLine()");
		return ERROR_INVALID_STATE;
    }

	// Pass the call to the COM object.
    char line[MAX_LOG_LINE_SIZE];
	CComBSTR bstrLogLine(GenerateLogLine(line,pEventBlock));
    
    //add log line to results view
    DWORD dwRet = m_pCOMAdapter->AddLogLine(lData, bstrLogLine);

    //add log line to callback
    if( ERROR_SUCCESS == dwRet && m_objStatDlg.cbVirus )
        dwRet = m_objStatDlg.cbVirus(m_objStatDlg.block, pEventBlock);

    return dwRet;
}

void CResultsViewProxy::ViewClosed(ULONG bStopScan)
{

    if( TRUE == bStopScan )
        SetScanStopped();

    // Save the type of the scan.  (We'll set it in the if statement below.)
	DWORD dwType = RV_TYPE_SCAN;

	// Close the dialog.
    CScanStatus* poScanStatus = m_objStatDlg.block;

	SAVASSERT(poScanStatus != NULL);
	if (poScanStatus != NULL)
		dwType = (LOGGER_Real_Time == poScanStatus->logger)
						? RV_TYPE_AUTOPROTECT
						: RV_TYPE_SCAN;

    // If the user closes a scan dialog during a scan the misc.cpp NotifiyProgress
    // may call StopScan before this routine is called. Avoid redundant calls
    // to StopScan because it over-decrements the CScanStatus reference counting.
    if( ScanRunning(poScanStatus->Status) &&
        poScanStatus->Status != S_STOPPING &&
        TRUE == bStopScan )
    {
        StopScan(poScanStatus->han,FALSE);
    }

    CleanupTokens(*poScanStatus);
    RevertCallbacks();

    // Decrement our reference to the CScanStatus block. This pointer may also
	// be shared in PSTATDLG->block if the scan progress dialog is displayed, so 
	// check the refcount before deleting.

    //to-do fix this isn't safe unless we remove it from headscan...
	poScanStatus->Release();
	if (poScanStatus->GetRefCount() <= 0)
	{
		delete poScanStatus;
		m_objStatDlg.block = NULL;
    }
 
    CUIManager::GetInstance().RemoveResultsView(m_dwKey, dwType);
}

void CResultsViewProxy::CleanupTokens(CScanStatus& objScanStatus)
{
    //we need to clear vids from suppression list
    if( LOGGER_Real_Time == objScanStatus.logger )
    {
        CSavrtThreatQueue* pThreatQueue = CSavrtThreatQueue::Initialize();
        if( NULL != pThreatQueue )
            pThreatQueue->ClearVirusIDSuppressionList();

        CTokenManager& objTokenManager = CTokenManager::GetInstance();
        objTokenManager.RemoveAPTokens(m_dwSessionId);
    }
	else if( LOGGER_Manual == objScanStatus.logger ||
		LOGGER_Scheduled == objScanStatus.logger )
	{
		CTokenManager& objTokenManager = CTokenManager::GetInstance();
		objTokenManager.RemoveOnDemandTokens(objScanStatus.ScanID);
	}
}


/**
 * DWORD AddProgress(PROGRESSBLOCK& tProgressBlock);
 *
 * Adds progress to the progress dialog
 *
**/
DWORD CResultsViewProxy::AddProgress(PROGRESSBLOCK& tProgressBlock)
{
    CProgressBlockSafeArrayCreatorImpl oSACreatorImpl;
	CSafeArrayCreator oSACreator(&oSACreatorImpl);
    DATABLOCK_SAFEARRAY oProgressConfigs;

    CProgressBlockConfig objProgressConfig;
    objProgressConfig.SetProgressBlock(&tProgressBlock);

    DWORD dwRet = ERROR_INVALID_PARAMETER;
	// Convert the configurations into a marshallable type.
	if (oSACreator.GetSafeArray(&objProgressConfig, oProgressConfigs))
	{
		CComVariant vProgressBlock(oProgressConfigs);
        if( m_pCOMAdapter != NULL )
        {
            dwRet = m_pCOMAdapter->AddProgress(vProgressBlock);
        }
    }

    if( ERROR_SUCCESS == dwRet && m_objStatDlg.cbProgress )
        dwRet = m_objStatDlg.cbProgress(m_objStatDlg.block, &tProgressBlock);

    return dwRet;
}

CResultsViewProxy::CResultsViewProxy(const CResultsViewProxy& objRhs)
	:m_dwSessionId(0), m_dwKey(0)
{
	SAVASSERT(0 == objRhs.m_dwKey && 0 == objRhs.m_dwSessionId);
		// The std::map class constructs an unitialized proxy object and
		// constructs another object using the copy constructor.  That's fine.
		// But don't call CreateDlg() and then pass that object into a copy
		// constructor.
    *this = objRhs;
}

CResultsViewProxy& CResultsViewProxy::operator=(const CResultsViewProxy& objRhs)
{
		// Once a proxy object has been initialized, don't overwrite it with another object!
		// The std::map class constructs an unitialized proxy object and
		// constructs another object using the copy constructor.  That's fine.
		// But don't call CreateDlg() and then pass that object into a copy
		// constructor.
    DWORD* pTemp = const_cast<DWORD*>(&m_dwSessionId);
    *pTemp = objRhs.m_dwSessionId;
    pTemp = const_cast<DWORD*>(&m_dwKey);
    *pTemp = objRhs.m_dwKey;
    m_IClassFactoryPtr = objRhs.m_IClassFactoryPtr;
    m_pCOMAdapter	   = objRhs.m_pCOMAdapter;
    m_pCOMCallback	   = objRhs.m_pCOMCallback;

    //should be safe todo since we are just copying pointer addresses
    //and no more memory
    memcpy(&m_objStatDlg,&objRhs.m_objStatDlg,sizeof(STATDLG));

    return *this;
}

