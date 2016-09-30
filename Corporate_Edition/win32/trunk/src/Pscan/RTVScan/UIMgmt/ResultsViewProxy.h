// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright (c) 2006 Symantec Corporation. All rights reserved.

#pragma once
#include <comdef.h>
#include "pscan.h"
#include "scanStatus.h"
#include "rtvscan.h"
#include "sessionmoniker.h"
#include "savui_h.h"

class CScanStatus;


/** This class serves as a proxy for creating results view dialogs in another
  * session/process.  It uses COM to create dialogs in SAVUI.exe in the correct
  * process and communicate with them.
  */
class CResultsViewProxy
{
private:
    _COM_SMARTPTR_TYPEDEF(IResultsViewCOMAdapter,  __uuidof(IResultsViewCOMAdapter));
    _COM_SMARTPTR_TYPEDEF(IResultsViewCOMCallback, __uuidof(IResultsViewCOMCallback));

public:
    /** default and parameterized constructor */
    CResultsViewProxy()
	  :	m_dwSessionId(0),
        m_dwKey(0),
        m_bScanStopped(false)
    {
        memset(&m_objStatDlg, 0, sizeof(m_objStatDlg));
    }
	/** default destructor */
    ~CResultsViewProxy()  
    { 
        //todo: fix...
    }

    void SetScanStopped(void){m_bScanStopped = true;}
    bool GetScanStopped(void)const{ return m_bScanStopped; }

    void RevertCallbacks()
    {
        SAVASSERT( NULL != m_objStatDlg.block );
        if( NULL == m_objStatDlg.block )
            return;

        m_objStatDlg.block->cbProgress = m_objStatDlg.cbProgress;
        m_objStatDlg.block->cbVirus = m_objStatDlg.cbVirus;
    }

    CResultsViewProxy(const CResultsViewProxy& objRhs);

    CResultsViewProxy& operator=(const CResultsViewProxy& objRhs);
private:
	bool IsRealTimeLogger(const CScanStatus& objScanStatus){return IsRealTimeLogger(objScanStatus.logger); };
	bool IsRealTimeLogger(const DWORD dwLoggerType){return LOGGER_Real_Time == dwLoggerType || LOGGER_HPP_Scanning == dwLoggerType;	};
	

// Proxied functions
public:
    DWORD CreateDlg(CScanStatus& objScanStatus, DWORD dwKey);
	DWORD AddLogLine(LPARAM lData, const PEVENTBLOCK pEventBlock);
    DWORD AddProgress(PROGRESSBLOCK& tProgressBlock);
    void  ViewClosed(ULONG bStopScan);

// Other functions
public:
	const STATDLG& GetStatDlg() const{ return m_objStatDlg; }

protected:
	/** cleanup access tokens associated with the view **/
	void CleanupTokens(CScanStatus& objScanStatus);

private:
    DWORD				       m_dwSessionId;
    DWORD                      m_dwKey;
    //m_objStatDlg doesn't use computer variable since this is only for local scans
    STATDLG                    m_objStatDlg; 
    IClassFactoryPtr		   m_IClassFactoryPtr;
    IResultsViewCOMAdapterPtr  m_pCOMAdapter;
    IResultsViewCOMCallbackPtr m_pCOMCallback;
    bool                       m_bScanStopped;
};
