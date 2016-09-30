////////////////////////
//
// PROPRIETARY / CONFIDENTIAL.
// Use of this product is subject to license terms.
// Copyright © 2006 Symantec Corporation.
// All rights reserved.
//
/////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include <AvProdWidgets.h>
#include <AVInterfaces.h>
#include "SvcMgr.h"

#include <NAVEventFactoryLoader.h>
#include <avevents.h>
#include <allnavevents.h>
#include <AvDefines.h>
#include <AvccModuleId.h>
#include <AVSubmissionInterface.h>
#include <SettingsEventHelperLoader.h>

namespace AvProdSvc
{
    class CAvSinks :
        public ISymBaseImpl< CSymThreadSafeRefCount >, 
        public AvProdWidgets::CSymObjectCreator<&AVModule::IID_AVAutoProtectSink, AVModule::IAVAutoProtectSink, CAvSinks>,
        public AVModule::IAVAutoProtectSink,
        public AVModule::IAVEmailSink,
        public AVModule::IAVDefInfoSink,
        public AVModule::IAVThreatActionRequiredSink,
        public AVModule::IAVServerNotificationSink,
        public SettingsEventHelper::IObserver
    {
    public:
        CAvSinks(void);
        virtual ~CAvSinks(void);

    public:
        SYM_INTERFACE_MAP_BEGIN()                
            SYM_INTERFACE_ENTRY( AVModule::IID_AVAutoProtectSink, AVModule::IAVAutoProtectSink ) 
            SYM_INTERFACE_ENTRY( AVModule::IID_AVEmailSink, AVModule::IAVEmailSink ) 
            SYM_INTERFACE_ENTRY( AVModule::IID_AVThreatActionRequiredSink, AVModule::IAVThreatActionRequiredSink ) 
            SYM_INTERFACE_ENTRY( AVModule::IID_AVDefInfoSink, AVModule::IAVDefInfoSink)
            SYM_INTERFACE_ENTRY( AVModule::IID_AVServerNotificationSink, AVModule::IAVServerNotificationSink)
            SYM_INTERFACE_ENTRY( SettingsEventHelper::IID_Observer, SettingsEventHelper::IObserver ) 
        SYM_INTERFACE_MAP_END()  

		HRESULT Initialize() throw();

    public:  //  IAVAutoProtectSink interface
        virtual void OnAPProcessingThreatBegin(AVModule::IAVMapDwordData* pData);
        virtual void OnAPProcessingThreatComplete(AVModule::IAVMapDwordData* pData);
        virtual void OnAPThreatRedetection(AVModule::IAVMapDwordData* pData);
        virtual void OnAPThreatBlocked(AVModule::IAVMapDwordData* pData);
		virtual void OnAPBootInfection(AVModule::IAVMapDwordData* pMapThreat);

    public:  //  IAVThreatActionRequiredSink interface
        virtual void OnThreatNotification(AVModule::ThreatTracking::ActionRequiredFlag flagARThreatType, AVModule::IAVMapDwordData* pThreat) throw();

    public: // IAVEmailSink interface
        virtual void OnEmailSessionStart(DWORD dwSessionID);
        virtual void OnEmailSessionFinish(DWORD dwSessionID, AVModule::IAVMapStrData* pStats);
        virtual void OnEmailThreatsDetected(AVModule::IAVMapStrData* pEmailInfo, AVModule::IAVMapStrData* pReturnData);
        virtual void OnOEHDetection(AVModule::IAVMapStrData* pOEHInfo, DWORD& dwOEHAction);
    
    public: // IAVServerNotificationSink interface
        virtual void DisconnectNotification(eAVDisconnectReason reason) throw();

    public: // SettingsEventHelper::IObserver interface
        virtual void Update(const ccSettings::CSettingsChangeEventEx *pEvent,
            ccSettings::ISettings *pSettings) throw();

    public: // IAVDefInfoSink
    void OnDefinitionsChange(   const wchar_t* pcwszDefsDir, 
                                DWORD dwYear, 
                                DWORD dwMonth, 
                                DWORD dwDay, 
                                DWORD dwRev) throw();
    public: // Defs info
        DWORD m_dwYear, m_dwMonth, m_dwDay, m_dwRev;
    
    protected: // methods
        void GetEmailThreatDetails(AVModule::IAVMapStrData* pEmailInfo, AVModule::IAVMapStrData* pReturnData, CString& cszDetails, BOOL& bActionRequired, BOOL &bThreatsProcessed);
        void ShowEmailThreatsUI(AVModule::IAVMapStrData* pEmailInfo, AVModule::IAVMapStrData* pReturnData);
        
        void HandleProcessTerminationRequired(AVModule::IAVMapDwordData* pThreat);
        void HandleRebootRequired(AVModule::IAVMapDwordData* pThreat);

	
    protected: // member variables
        void PostProcessThreatComplete(AVModule::IAVMapDwordData* pThreatInfo);
        
        HRESULT IsLowRiskThreat(AVModule::IAVMapDwordData* pThreatInfo);
        HRESULT IsLowRiskThreat(DWORD dwThreatMatrixValue);

		HRESULT AddSubmission(AVModule::IAVMapBase* pData) throw();
    
        HRESULT LaunchQuickScanFromService();
        HRESULT LaunchQuickScanFromCurrentUserSession(bool bIncludeCookies = true);

	protected:
		AVSubmit::IAVSubmissionManagerPtr m_pSubmitMgr;

	};
    

}; // AvProdSvc