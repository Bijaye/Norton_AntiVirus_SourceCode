// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright (c) 2006 Symantec Corporation. All rights reserved.

#include "stdafx.h"
#include "VPExceptionHandling.h"
#include "virusfoundproxy.h"
#include "resultsviewproxy.h"
#include "uimanager.h"
#include "ScanStatus.h"
#include "storagelistener.h"

CUIManager CUIManager::s_objUIManager;

/** static CGlgManager& GetInstance();
 * 
 * gets an instance to the class
 *
**/
CUIManager& CUIManager::GetInstance()
{
    return s_objUIManager;
}

/** internal createnotification dlg so it can be re-used
  *
  * @returns ERROR_SUCCESS on success
  *          Error code on failure
  */
DWORD CUIManager::CreateNotification(const DWORD dwSessionId,
									 LPCTSTR pcTitle,
									 LPCTSTR pcLogline,
									 LPCTSTR pcDescription)
{
    CVirusFoundProxy objProxy(pcTitle, SessionIdValidator(dwSessionId));
    DWORD dwRet = objProxy.AddMessage(pcLogline, pcDescription);
    if( ERROR_SUCCESS == dwRet )
    {
        try
        {
            m_mapVirusFoundProxies[SessionIdValidator(dwSessionId)] = objProxy;
        }
        VP_CATCH_MEMORYEXCEPTIONS
        (
            dwRet = ERROR_NOT_ENOUGH_MEMORY;
		)
    }

	return dwRet;
}


/** creates a notification dlg wrapper and adds it to the notification dlg map 
  * based on session id
  *
  * Note: access all session ids through the session id validator
  *       this prevents the session id from being session zero on vista
  */
DWORD CUIManager::AddNotificationMessage(const DWORD dwSessionId,
										 LPCTSTR pcTitle, 
                                         LPCTSTR pcLogline,
										 LPCTSTR pcDescription)
{
	DWORD dwRet = ERROR_SUCCESS;
    CGuard objGuard(m_objNotificationsLock);
    //if the notification dlg doesn't exist create it
    if( m_mapVirusFoundProxies.find(SessionIdValidator(dwSessionId)) == m_mapVirusFoundProxies.end() )
    {
        dwRet = CreateNotification(dwSessionId, pcTitle, pcLogline, pcDescription);   
    }
    else
    {
        //we have a notification dlg, so use it
        CVirusFoundProxy &objVirusFoundProxy = m_mapVirusFoundProxies[SessionIdValidator(dwSessionId)];
        dwRet = objVirusFoundProxy.AddMessage(pcLogline, pcDescription);
        if( ERROR_SUCCESS != dwRet ) //failure
        {
            RemoveNotificationDlg(dwSessionId); //session validation done internally
            dwRet = CreateNotification(dwSessionId, pcTitle, pcLogline, pcDescription);
        }
    }

    return dwRet;
}


/** @param objScanStatus information about a scan
  * @returns the key used to store the scan in our internal map
  */
DWORD CUIManager::GetResultsViewKey(const CScanStatus &objScanStatus)
{
    if (IsRealTimeLogger(objScanStatus))
        return SessionIdValidator(objScanStatus.GetSessionID());
    else
        return objScanStatus.ScanID;
}


/** Creates a virus found proxy and dialog.
  * @param objScanStatus An object that contains info about the scan.
  * Its reference count gets incremented, and objScanStatus.dlg gets set
  * to point to a newly allocated STATDLG structure.
  * @returns ERROR_SUCCESS on success, other values on failure
  */
DWORD CUIManager::CreateResultsViewProxy(CScanStatus& objScanStatus)
{
    // Create the proxy in the appropriate map and create the dialog.
    DWORD dwKey = GetResultsViewKey(objScanStatus);
	DWORD dwRet = ERROR_GENERAL;

    try
    {
        ResultsViewMap& mapResultsView = (IsRealTimeLogger(objScanStatus))
										 ? m_mapAutoProtectResultsView
										 : m_mapManualScanResultsView;
     
        //guards the creation of a dialog from being called during a resestablish or a viewclosed() call
        CGuardWriteLock cGuard(m_objResultsViewsRWLock);
        //make sure the map doesn't have a results view first...
        if( mapResultsView.find(dwKey) == mapResultsView.end() )
        {
            CResultsViewProxy objLocalProxy;
            dwRet = objLocalProxy.CreateDlg(objScanStatus, dwKey);
            if( SUCCEEDED(dwRet) )
            {
                mapResultsView[dwKey] = objLocalProxy;
            }
        }
	}
	VP_CATCH_MEMORYEXCEPTIONS
    (
        dwRet = ERROR_NOT_ENOUGH_MEMORY;
	)

	return dwRet;
}

/**
 *
 * DWORD AddLogLineToResultsView(CScanStatus& objScanStatus, const PEVENTBLOCK pEventBlock);
 *
 * purpose find the correct results view and add a log line to the view
 *
**/
DWORD CUIManager::AddLogLineToResultsView(CScanStatus& objScanStatus, const PEVENTBLOCK pEventBlock, DWORD userParam)
{
    if( NULL == pEventBlock )
    {
        SAVASSERT(FALSE);
        return ERROR_INVALID_PARAMETER;
    }

    CResultsViewProxy objLocalProxy;
    DWORD dwKey = 0;
    {
        //prevents viewclosed from being called, removing the results view from the map
        //when adding a log line to the results view
        CGuardReadLock cGuard(m_objResultsViewsRWLock);
        // See if we can get the proxy.
	    dwKey = GetResultsViewKey(objScanStatus);
	    ResultsViewMap& mapResultsView = (IsRealTimeLogger(objScanStatus))
										    ? m_mapAutoProtectResultsView
										    : m_mapManualScanResultsView;

        if( mapResultsView.find(dwKey) == mapResultsView.end() )
		    return ERROR_INVALID_PARAMETER;
	    
        objLocalProxy = mapResultsView[dwKey];
    }

    // Add the log line.
    DWORD dwRet = objLocalProxy.AddLogLine((LONG) userParam, pEventBlock);

    if( ERROR_SUCCESS != dwRet )
    {
        //recreate the results view if necessary
        ReEstablishResultsView(objScanStatus);

        {
            //prevents viewclosed from being called, removing the results view from the map
            //when adding a log line to the results view
            CGuardReadLock cGuard(m_objResultsViewsRWLock);
            
            //try to add the log line if it failed.
            //don't want to call into this function again to prevent recursion from happening...
            ResultsViewMap& mapResultsView = (IsRealTimeLogger(objScanStatus))
										    ? m_mapAutoProtectResultsView
										    : m_mapManualScanResultsView;

            if( mapResultsView.find(dwKey) == mapResultsView.end() )
		        return ERROR_INVALID_PARAMETER;

            objLocalProxy = mapResultsView[dwKey];
        }
	    
        // Add the log line.
        dwRet = objLocalProxy.AddLogLine(NULL, pEventBlock);
    }
        
    return dwRet;
}




/**
 * void RemoveResultsView(const DWORD dwKey, const DWORD dwType)
 *
 * cleanup corresponding structures in service related to results view.
 * 
**/
void CUIManager::RemoveResultsView(const DWORD dwKey, const DWORD dwType)
{
	// Check parameters.
	if (dwType != RV_TYPE_AUTOPROTECT && dwType != RV_TYPE_SCAN)
	{
		SAVASSERT(!"RemoveResultsView() got an invalid Type.");
		return;
	}
    
    //prevents someone from accessing the results view when we are trying to cleanup the view
    //and remove all references to it
    CGuardWriteLock cGuard(m_objResultsViewsRWLock);
	// Look up the proxy.
	ResultsViewMap& mapResultsView = (RV_TYPE_AUTOPROTECT == dwType)
										 ? m_mapAutoProtectResultsView
										 : m_mapManualScanResultsView;

    if( mapResultsView.find(dwKey) == mapResultsView.end() )
	{
		SAVASSERT(!"RemoveResultsView() got an invalid key.");
		return;
	}
	// Close the proxy and remove it from our map.
     mapResultsView.erase(dwKey);
}


/**
 *
 * DWORD AddProgressToResultsView(PROGRESSBLOCK& tProgress, CScanStatus& objScanStatus);
 *
**/
DWORD CUIManager::AddProgressToResultsView(PROGRESSBLOCK& tProgress, CScanStatus& objScanStatus)
{
    DWORD dwRet = ERROR_STOP_CALLBACKS;
	DWORD dwKey = GetResultsViewKey(objScanStatus);
		// Even though we know it's a manual scan, call this function anyway
		// to preserve the abstraction (in case the key changes).

    //scoping for read lock
    CResultsViewProxy objLocalProxy;
    {
        //read lock for adding progress. Make sure the proxy isn't deleted from underneath us
        //don't want to have this lock block a callback while the scan is paused.
		//we pause a scan by having the UI not return from AddProgress() until it's unpaused.
        CGuardReadLock cGuard(m_objResultsViewsRWLock);
        if( m_mapManualScanResultsView.find(objScanStatus.ScanID) != m_mapManualScanResultsView.end() )
        {
            objLocalProxy = m_mapManualScanResultsView[dwKey];
            dwRet = ERROR_SUCCESS;            
        }
    }

    //make sure we don't lock around the add progress because AddProgress
    //blocks when the scan is paused...
    if( dwRet == ERROR_SUCCESS )
    {
        dwRet = objLocalProxy.AddProgress(tProgress);
        if( ERROR_STOP_SCAN == dwRet )
            objLocalProxy.SetScanStopped();

        if( ERROR_SUCCESS != dwRet && ERROR_STOP_SCAN != dwRet )
            ReEstablishResultsView(objScanStatus);
    }

    return dwRet;
}

/**
 * void CloseResultsView(const DWORD dwKey);
 *
 * @purpose: all callers to close a results view
 *          only closes manual scan results views...
 *
**/
void CUIManager::CloseResultsView(const DWORD dwKey)
{
    CResultsViewProxy objLocalProxy;
    bool bFound = false;
    {
        //make sure we lock when closing the scan
        CGuardWriteLock cGuard(m_objResultsViewsRWLock);
        if( m_mapManualScanResultsView.find(dwKey) != m_mapManualScanResultsView.end() )
        {
            objLocalProxy = m_mapManualScanResultsView[dwKey];
            bFound = true;
        }
    }
    if( true == bFound )
        objLocalProxy.ViewClosed(TRUE);
}

/** 
 * DWORD GetResultsView(const DWORD dwKey, const DWORD dwType, CResultsViewProxy& objResultsViewProxy);
 *
 * @purpose: gets the result view proxy for the corresponding key and type) 
 *
 * @returns: ERROR_SUCCESS on failure
**/
DWORD CUIManager::GetResultsView(const DWORD dwKey, const DWORD dwType, CResultsViewProxy& objResultsViewProxy)
{
    // Check parameters.
	if (dwType != RV_TYPE_AUTOPROTECT && dwType != RV_TYPE_SCAN)
	{
		SAVASSERT(!"GetResultsView() got an invalid key.");
		return ERROR_INVALID_PARAMETER;
	}

    //when checking for the existance of a view make sure noone else can delete
    CGuardReadLock cGuard(m_objResultsViewsRWLock);
	// Look up the proxy.
	ResultsViewMap& mapResultsView = (RV_TYPE_AUTOPROTECT == dwType)
										 ? m_mapAutoProtectResultsView
										 : m_mapManualScanResultsView;

    DWORD dwRet = ERROR_INVALID_PARAMETER;
    if( mapResultsView.find(dwKey) != mapResultsView.end())
    {
        dwRet = ERROR_SUCCESS;
        objResultsViewProxy = mapResultsView[dwKey];
    }

    return dwRet;
}

/**
 *
 * bool CUIManager::ResultsViewExists(const DWORD dwKey, const DWORD dwType)
 *
 * @purpose: Determine if a scan results dialog exists
**/
bool CUIManager::ResultsViewExists(const DWORD dwKey, const DWORD dwType) const
{
    CGuardReadLock cGuard(m_objResultsViewsRWLock);
    // Look up the proxy.
    const ResultsViewMap& mapResultsView = (IsRealTimeLogger(dwType))
											   ? m_mapAutoProtectResultsView
											   : m_mapManualScanResultsView;

    return mapResultsView.find(dwKey) != mapResultsView.end();
}


/** Determine if a scan results dialog exists */
bool CUIManager::ResultsViewExists(const CScanStatus &objScanStatus) const
{
	return ResultsViewExists(GetResultsViewKey(objScanStatus),
								 objScanStatus.logger);
}


/**
 * DWORD ReEstablishResultsView(const CScanStatus& objScanStatus);
 *
 * @purpose: if the results view went away unexpectedly recreate it
 *
**/
DWORD CUIManager::ReEstablishResultsView(CScanStatus& objScanStatus)
{
    {
        //lock during the recreation of the results view... we don't want somoene to be 
        //able to re-create the view if we are cleaning it up
        CGuardWriteLock cGuard(m_objResultsViewsRWLock);
        // See if we can get the proxy.
	    DWORD dwKey	= GetResultsViewKey(objScanStatus);
	    ResultsViewMap& mapResultsView = (IsRealTimeLogger(objScanStatus))
										    ? m_mapAutoProtectResultsView
										    : m_mapManualScanResultsView;

        //return the callbacks to there original location
        SAVASSERT( mapResultsView.find(dwKey) != mapResultsView.end() );
        {
            CResultsViewProxy& objLocalProxy = mapResultsView[dwKey];

            //if the scan has been stopped bail out. We don't want to have a race condition
            //where multiple status messages are accepted, recreating the 
            //scan results view before the view closed is called
            if( objLocalProxy.GetScanStopped() == true )
                return ERROR_STOP_SCAN;

            objLocalProxy.RevertCallbacks();
        }
        
        //release the ref count in scan status
        objScanStatus.Release();
        
        //delete the results view that is dead...
        mapResultsView.erase(dwKey);
    }

    //create new dialog if needed
    DWORD dwRet = ERROR_INVALID_DATA;
    if( GetVal(objScanStatus.hkey,szReg_Value_DisplayStatusDlg,0) &&
        CStorageListener::GetInstance().GetIsUserConnected() )
    {
        dwRet = CreateResultsViewProxy(objScanStatus);
    }

    return dwRet;
}
