// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright (c) 2006 Symantec Corporation. All rights reserved.

#pragma once
#include <windows.h>
#include <map>
#include <comdef.h>
#include "pscan.h"
#include "guard.h"
#include "OSUtils.h"
#include "virusfoundproxy.h"
#include "resultsviewproxy.h"

class CResultsViewCOMCallback;
/** class CUIManager
  *
  * Singleton class that has a maps that hold references to virus found dlgs and results view dlgs
  *
**/
class CUIManager
{
public:
    ~CUIManager()
    {
    }

    static CUIManager& GetInstance();

    /** creates a notification dlg wrapper and adds it to the notification dlg map 
      * based on session id
      */
    DWORD AddNotificationMessage(const DWORD dwSessionId,
								 LPCTSTR pcTitle,
								 LPCTSTR pcLogline,
								 LPCTSTR pcDescription);

    /**
     * void RemoveNotificationDlg(const DWORD dwSessionId)
     *
     * purpose: removes notification dlgs from map
     *
    **/
    void RemoveNotificationDlg(const DWORD dwSessionId)
    {   
        CGuard objGuard(m_objNotificationsLock);
        m_mapVirusFoundProxies.erase(SessionIdValidator(dwSessionId));
    }

    /**
     *
     * DWORD CreateResultsViewProxy(CScanStatus& objScanStatus);
     *
     * @purpose: Create a results view dialog for displaying virus notifications
     *
    **/
	DWORD CreateResultsViewProxy(CScanStatus& objScanStatus);

    /**
     * void CloseResultsView(const DWORD dwKey);
     *
     * @purpose: all callers to close a results view
     *          only closes manual scan results views...
     *
    **/
    void CloseResultsView(const DWORD dwKey);

    /**
     *
     * DWORD AddLogLineToResultsView(CScanStatus& objScanStatus, const PEVENTBLOCK pEventBlock);
     *
     * purpose find the correct results view and add a log line to the view
     *
    **/
    DWORD AddLogLineToResultsView(CScanStatus& objScanStatus, const PEVENTBLOCK pEventBlock);

    /**
     *
     * DWORD AddProgressToResultsView(PROGRESSBLOCK& tProgress, CScanStatus& objScanStatus);
     *
    **/
    DWORD AddProgressToResultsView(PROGRESSBLOCK& tProgress, CScanStatus& objScanStatus);

    /**
     *
     * bool ResultsViewExists(const DWORD dwKey, const DWORD dwType)
     *
     * @purpose: Determine if a scan results dialog exists
    **/
    bool ResultsViewExists(const DWORD dwKey, const DWORD dwType) const;
	bool ResultsViewExists(const CScanStatus &objScanStatus) const;

protected:
    /** hidden constructor, copy contructor, assignement operator **/
    CUIManager(){}
    void operator = (const CUIManager&);
    CUIManager(const CUIManager&);

    DWORD CreateNotification(const DWORD dwSessionId,
							 LPCTSTR pcTitle,
							 LPCTSTR pcLogline,
							 LPCTSTR pcDescription);

	static DWORD GetResultsViewKey(const CScanStatus &objScanStatus);

    //access for remove resultsview
    friend void CResultsViewProxy::ViewClosed(ULONG bStopScan);
    friend CResultsViewCOMCallback;

    /**
     * void RemoveResultsView(const DWORD dwKey, const DWORD dwType)
     *
     * cleanup corresponding structures in service related to results view.
     * should only be called from the resultsview proxy object
     * removes the results view from the map that associates the lists together
     * 
    **/
    void RemoveResultsView(const DWORD dwKey, const DWORD dwType);

    /** 
     * DWORD GetResultsView(const DWORD dwKey, const DWORD dwType, CResultsViewProxy& objResultsViewProxy);
     *
     * @purpose: gets the result view proxy for the corresponding key and type) 
     *
     * @returns: ERROR_SUCCESS on failure
    **/
    DWORD GetResultsView(const DWORD dwKey, const DWORD dwType, CResultsViewProxy& objResultsViewProxy);

    /**
     * DWORD ReEstablishResultsView(const CScanStatus& objScanStatus);
     *
     * @purpose: if the results view went away unexpectedly recreate it
     *
    **/
    DWORD ReEstablishResultsView(CScanStatus& objScanStatus);

private:
    static CUIManager s_objUIManager;

	typedef std::map<DWORD, CVirusFoundProxy>   VirusFoundMap;
	typedef std::map<DWORD, CResultsViewProxy>  ResultsViewMap;

	CLock				 m_objNotificationsLock;
    mutable CRWLockGuard m_objResultsViewsRWLock;
    VirusFoundMap		 m_mapVirusFoundProxies;
    ResultsViewMap		 m_mapManualScanResultsView;
    ResultsViewMap		 m_mapAutoProtectResultsView;
};
