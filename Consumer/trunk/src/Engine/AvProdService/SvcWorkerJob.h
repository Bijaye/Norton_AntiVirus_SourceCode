////////////////////////
//
// PROPRIETARY / CONFIDENTIAL.
// Use of this product is subject to license terms.
// Copyright © 2006 Symantec Corporation.
// All rights reserved.
//
/////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include <SessionAppServerInterface.h>

namespace AvProdSvc
{
    class CWorkerJob : public StahlSoft::CRunnable
    {
    public:
        CWorkerJob(void);
        virtual ~CWorkerJob(void);


        STDMETHOD(IsRunning)();
        STDMETHOD(RequestExit)();

    protected:
        StahlSoft::CSmartHandle m_hRunning;

    };

    class CProcessReconnect : public CWorkerJob
    {
    public:
        CProcessReconnect(void);
        virtual ~CProcessReconnect(void);

        STDMETHOD(Run)();

    public:
		DWORD m_dwWaitTimeout;
    };
    
    class CProcessAsyncRebootRequest : public CWorkerJob
    {
    public:
        CProcessAsyncRebootRequest(void);
        virtual ~CProcessAsyncRebootRequest(void);

        STDMETHOD(Run)();

    };

    class CProcessAutoProtectNotification : public CWorkerJob
    {
    public:
        CProcessAutoProtectNotification(void);
        virtual ~CProcessAutoProtectNotification(void);

        STDMETHOD(Run)();
        
        DWORD m_dwTimeout;
        HANDLE m_hCancelNotification;
        BOOL m_bWorking;
        CString m_cszThreatName;
        DWORD m_dwAvAction;
    };

    class CProcessAutoProtectRemediationAlert : public CWorkerJob
    {
    public:
        CProcessAutoProtectRemediationAlert(void);
        virtual ~CProcessAutoProtectRemediationAlert(void);

        STDMETHOD(Run)();
        
        ISymBasePtr m_spCommand;
        GUID m_guidThreatTrackId;
        CString m_cszJobName;
    };

    class CProcessAsyncAvModuleDisconnect : public CWorkerJob
    {
    public:
        CProcessAsyncAvModuleDisconnect(void);
        virtual ~CProcessAsyncAvModuleDisconnect(void);

        STDMETHOD(Run)();

    };

	class CProcessLogEmailScanResults : public CWorkerJob
	{
	public:
		CProcessLogEmailScanResults(void);
		virtual ~CProcessLogEmailScanResults(void);

		STDMETHOD(Run)();

		GUID m_guidScanId;
	};

	class CProcessLogManualScanResults : public CWorkerJob
	{
	public:
		CProcessLogManualScanResults(void);
		virtual ~CProcessLogManualScanResults(void);

		STDMETHOD(Run)();

		GUID m_guidScanId;
	};

}; // AvProdSvc