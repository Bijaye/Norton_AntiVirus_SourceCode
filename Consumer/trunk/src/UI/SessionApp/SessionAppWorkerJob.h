////////////////////////
//
// PROPRIETARY / CONFIDENTIAL.
// Use of this product is subject to license terms.
// Copyright © 2006 Symantec Corporation.
// All rights reserved.
//
/////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "SessionAppServerInterface.h"

#include <AvProdWidgets.h>
#include <ccAlertLoader.h>

#include <isDataClientLoader.h>
#include <uiNISDataElementGuids.h>

namespace SessionApp
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

    class CProcessNotifyMessage : public CWorkerJob
    {
    public:
        CProcessNotifyMessage(void);
        virtual ~CProcessNotifyMessage(void);

        STDMETHOD(Run)();

    public:
        cc::IKeyValueCollectionQIPtr m_spCommand;
        HWND m_hWndParent;
    };

    class CNotifySink_ViewRecentHistory :
        public ISymBaseImpl< CSymThreadSafeRefCount >, 
        public AvProdWidgets::CSymObjectCreator<&cc::IID_NotifyCallback, cc::INotifyCallback, CNotifySink_ViewRecentHistory>,
        public cc::INotifyCallback
    {
    public:
        CNotifySink_ViewRecentHistory() {};
        ~CNotifySink_ViewRecentHistory() {};

    public:
        SYM_INTERFACE_MAP_BEGIN()                
            SYM_INTERFACE_ENTRY( cc::IID_NotifyCallback, cc::INotifyCallback ) 
        SYM_INTERFACE_MAP_END()                  

    public: // INotifyCallback
        virtual bool Run(HWND hWndParent, unsigned long nID, cc::INotify* pAlert)
        {
            CCTRACEI( CCTRCTX _T("Handling nID=%d"), nID);
            
            SYMGUID guidElementId;
            if(100 == nID)
                memcpy(&guidElementId, &ISShared::CLSID_NIS_ManageQuarantine, sizeof(SYMGUID)); // Quarantine
            else if(110 == nID)
                memcpy(&guidElementId, &ISShared::CLSID_NIS_ManageUnresolvedRisks, sizeof(SYMGUID)); // Unresolved Risks
            else if(120 == nID)
                memcpy(&guidElementId, &ISShared::CLSID_NIS_ManageResolvedRisks, sizeof(SYMGUID)); // Security Risks
            else
                memcpy(&guidElementId, &ISShared::CLSID_NIS_MessageCenter, sizeof(SYMGUID)); // Message Center

            StahlSoft::HRX hrx;
            ccLib::CExceptionInfo exceptionInfo;
            try
            {
                SYMRESULT sr;
                ISShared::ISShared_IProvider ProviderLoader;
                ui::IProviderPtr spISElementProvider;
                sr = ProviderLoader.CreateObject(GETMODULEMGR(), &spISElementProvider);
                hrx << AvProdWidgets::HRESULT_FROM_SYMRESULT(sr);

                ui::IElementPtr pElement;
                hrx << spISElementProvider->GetElement ( guidElementId, pElement);
                hrx << pElement->Configure (::GetDesktopWindow(), NULL);

                CCTRACEI( CCTRCTX _T("Element successfully launched"));
            }
            CCCATCHMEM(exceptionInfo)
            CCCATCHCOM(exceptionInfo);

            if(exceptionInfo.IsException())
            {
                CCTRACEE( CCTRCTX _T("Unable to launch element. Error: %s"), exceptionInfo.GetDescription());
            }

            return true;
        }
    };
    
    class CProcessCustomCommand : public CWorkerJob
    {
    public:
        CProcessCustomCommand(void);
        virtual ~CProcessCustomCommand(void);

        STDMETHOD(Run)();

    public:
        DWORD m_dwStartTime;

        SYMGUID m_guidCommand;
        SYMGUID m_guidActualCommand;

        ISymBasePtr m_spCommand;
        SessionApp::ISessionAppServerSinkPtr m_spSessionAppServerSink;
        
        CStringW m_cwszCallbackChannel;
        ccService::IBiDirectionalServerPtr m_spCommServer;
        SessionApp::ISessionAppServerPtr m_spAppServer;
    };


}; // SessionApp