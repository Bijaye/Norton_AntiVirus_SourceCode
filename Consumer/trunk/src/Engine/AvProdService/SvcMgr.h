////////////////////////
//
// PROPRIETARY / CONFIDENTIAL.
// Use of this product is subject to license terms.
// Copyright © 2006 Symantec Corporation.
// All rights reserved.
//
/////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include <AvProdSvcCommIds.h>
#include <SessionAppClientImpl.h>
#include <AVInterfaceLoader.h>
#include "SvcComm.h"
#include "AvSinks.h"
#include "SvcWorkerThread.h"
#include <SettingsEventHelperLoader.h>
#include "GuidHelper.h"

#include <list>
typedef std::list<CString> LST_STRINGS;

namespace AvProdSvc
{
    typedef std::map<CGuidHelper, ISymBasePtr> MAP_GUID_2_SYMBASEOBJ;
    typedef std::map<CGuidHelper, DWORD> MAP_GUID_2_DWORD;

    //note: CRefCount is not thread safe...
    class CSvcMgr : public StahlSoft::CRefCount
    {
    public: // c'tor / d'tor
        CSvcMgr(void);
        virtual ~CSvcMgr(void);

    public: // methods
        HRESULT Initialize();
		HRESULT PrepareDestroy();
        void Destroy();

        void SpawnDelayedInitialization(DWORD dwTimeout = 5000);

        HRESULT InitializeCommServer();
        HRESULT InitializeAvModuleSinks();
        HRESULT InitializeSettingsObserver();

        HRESULT InitializeSettings();
        void OnSettingsChanged(LPCWSTR szSettingsKey, ccSettings::ISettings* pSettings);
        
        HRESULT SynchronizeEmailSettings(ccSettings::ISettings *pSettings);
        HRESULT SynchronizeAvProdSvcSettings(ccSettings::ISettings *pSettings, bool bReadOnly = true);
        
        HRESULT RestartComputer();
        void HandleAvModuleDisconnect(bool bAsync = true);

        enum{ eLowRiskResponseInvalid = 0, eRespondRemediate, eRespondIgnore, eRespondAskUser };
        enum{ eComponentInvalid = 0, eAutoProtectComponent, eEmailComponent, eOEHComponent };
        enum{ eAvModuleActionSummaryInvalid = 0, eAvActionBlocked, eAvActionRemoved, eAvActionIgnored, eAvActionRedetect };

        DWORD GetLowRiskResponseMode(DWORD dwComponent)
        {
            DWORD dwLowRiskResponse = eLowRiskResponseInvalid;
            switch(dwComponent)
            {
                case eAutoProtectComponent:
                    dwLowRiskResponse = m_dwLowRiskResponseMode_AutoProtect;
                    break;

                case eEmailComponent:
                    dwLowRiskResponse = m_dwLowRiskResponseMode_EMail;
                    break;

                default:
                    // do nothing
                    break;
            };

            return dwLowRiskResponse; 
        };

        DWORD GetResponseMode(DWORD dwComponent)
        {
            DWORD dwResponseMode = eLowRiskResponseInvalid;
            switch(dwComponent)
            {
            case eOEHComponent:
                dwResponseMode = m_dwResponseMode_OEH;
                break;

            default:
                // do nothing
                break;
            };

            return dwResponseMode; 
        };

        DWORD GetNotificationDisplayTimeout() { return m_dwNotificationDisplayTimeout; };
        
        void HandleAvModuleRebootRequest(AVModule::IAVMapDwordData* pThreat);
        void ProcessAsyncRebootRequest();
        
        void PushWork(LPCTSTR szJobName);
        void PopWork(LPCTSTR szJobName);
        void ResetAutoProtectActivity();
        void PushAutoProtectActivity(LPCTSTR szJobName, LPCTSTR szThreatName, DWORD dwAvAction = CSvcMgr::eAvModuleActionSummaryInvalid);
        void PopAutoProtectActivity(LPCTSTR szJobName, AVModule::IAVMapDwordData* pThreatInfo);
        void PopAutoProtectActivity(LPCTSTR szJobName, CString& cszThreatName, DWORD dwAvAction);
        void SetWorkingState(BOOL bWorking);

        void ScheduleNotifyAutoProtectActivity(BOOL bWorking, LPCTSTR szThreatName, DWORD dwAvAction);
        void NotifyAutoProtectActivity(BOOL bWorking, LPCTSTR szThreatName, DWORD dwAvAction);
        void SubmitRemediationRequest(ISymBase* pCommand, GUID guidThreatId);
        bool IsPendingAutoProtectNotification();
        
        HRESULT GetCountOfUnresolvedThreats(DWORD &dwCount);
        bool ThreatHasBeenResolved(AVModule::IAVMapDwordData* pThreatTrackInfo);
		
        void HandleProcessTerminationRequired(AVModule::IAVMapDwordData* pThreatTrackInfo);

		void LogEmailScanResults(REFGUID guidScanId, bool bAsync = true);
		void LogManualScanResults(REFGUID guidScanId, bool bAsync = true);

    public: // COMM Messages
        void OnDisableRebootDialog(const SYMGUID& gCommandGuid, LPCWSTR szCallbackChannel,
            ISymBase* pBase, ISymBase** ppOutBase);

        void OnEnableRebootDialog(const SYMGUID& gCommandGuid, LPCWSTR szCallbackChannel,
            ISymBase* pBase, ISymBase** ppOutBase);
    
		void OnLogManualScanResults(const SYMGUID& gCommandGuid, LPCWSTR szCallbackChannel,
			ISymBase* pBase, ISymBase** ppOutBase);

    protected: // methods
        void QueryUserForReboot();
        void GenerateRemediationRequests();
        
        void AddToThreatProcessingCount(REFGUID guidThreatTrackId);
        size_t ClearThreatProcessingCount();
        
	protected: // members
        ccService::IServerPtr m_spCommServer;
        ccService::ICommandHandlerPtr m_spCommandHandler;
        ccService::IComLibPtr m_spCommLib;
        
        AVModule::IAVAutoProtectSinkPtr m_spAvSink;

        AVModule::IAVEmailPtr m_spAvEmail;
        AVModule::IAVAutoProtectPtr m_spAvAutoProtect;
        AVModule::IAVThreatInfoPtr m_spAvThreatInfo;
        AVModule::IAVDefInfoPtr m_spAvDefs;
        AVModule::IAVServerNotificationPtr m_spAvServerNotification;

        SettingsEventHelper_ISubject m_ISubjectLoader;
        SettingsEventHelper::ISubjectPtr m_spSubject;

		CMTAWorkerMessageThread m_cWorkerThread;
        CMTAWorkerMessageThread m_cAutoProtectNotificationsThread;
		
        DWORD m_dwConnectionAttemptCount;
        
        DWORD m_dwConnectionAttemptCount_CommServer;
        DWORD m_dwConnectionAttemptCount_AvModule;
        DWORD m_dwConnectionAttemptCount_SettingsObserver;
        
        bool m_bCommServerConnected;
        bool m_bAvModuleSinksConnected;
        bool m_bSettingsObserverConnected;

        DWORD m_dwLowRiskResponseMode_AutoProtect;
        DWORD m_dwLowRiskResponseMode_EMail;
        DWORD m_dwResponseMode_OEH;

        DWORD m_dwNotificationDisplayTimeout;
        
        DWORD m_dwProcTermDefault;
        CTime m_ctProcTermTimeout;
        BOOL m_bRequestProcTermAlready;


        BOOL m_bRequestRebootAlready;
        BOOL m_bDisableRebootDialog;
        BOOL m_bPendingRebootRequest;
        BOOL m_bShouldDisplayProcessingBegin;

        DWORD m_dwDisableRebootDialogLockCount;

        mutable ccLib::CCriticalSection m_Crit;
        StahlSoft::CSmartHandle m_shAutoProtectNotificationEvent; // if this is signalled, then there are no pending notifies

        MAP_GUID_2_SYMBASEOBJ m_mapRemediationRequests;
        ccLib::CCriticalSection m_RemediationRequestsLock;
        
        MAP_GUID_2_DWORD m_mapThreatProcessingCount;
        ccLib::CCriticalSection m_ThreatProcessingCountLock;

        mutable ccLib::CCriticalSection m_WorkingJobsLock;
        LST_STRINGS m_lstWorkingJobs;
        LST_STRINGS m_lstAutoProtectActivityJobs;
    };

    typedef StahlSoft::CSmartRefCountPtr<CSvcMgr> CSvcMgrPtr;

    class CSvcMgrSingleton
    {
    public:
        CSvcMgrSingleton(void);
        virtual ~CSvcMgrSingleton(void);

    public:
        static HRESULT Initialize()
        {
			m_spAppMgr = new (std::nothrow) CSvcMgr;
            if(!m_spAppMgr)
                return E_OUTOFMEMORY;

            return S_OK;
        };

        static void Destroy(){ m_spAppMgr.Release(); };
        static CSvcMgr* GetSvcMgr(){ return m_spAppMgr; };

    protected:
        static CSvcMgrPtr m_spAppMgr;

    };

}; // AvProdSvc
