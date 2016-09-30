////////////////////////
//
// PROPRIETARY / CONFIDENTIAL.
// Use of this product is subject to license terms.
// Copyright © 2006 Symantec Corporation.
// All rights reserved.
//
/////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include <ccAlertLoader.h>
#include <ccServiceLoader.h>
#include <SettingsEventHelperLoader.h>
#include <SessionAppServerInterface.h>
#include <AvProdWidgets.h>

#include <uiElementInterface.h>
#include <uiUpdateInterface.h>

#include <isDataClientLoader.h>
#include <uiNISDataElementGuids.h>

#include <AVInterfaceLoader.h>

namespace AvProdApp
{

    class CAppManager : public StahlSoft::CRefCount
    {
    public:
        CAppManager(void);
        virtual ~CAppManager(void);
    public:

    public:
        HRESULT Initialize();
        void Destroy();
        
        HRESULT InitializeSettings();
        
        void OnSettingsChanged(LPCWSTR szSettingsKey, ccSettings::ISettings* pSettings);

        void UpdateTrayAppState();

        HRESULT OpenAvProduct();
        HRESULT ViewRecentHistory();
        HRESULT EnableAutoProtect(BOOL bEnabled);
        HRESULT OpenAvProductOptions();
        
        DWORD GetAutoProtectState();
        bool IsAutoProtectConfigurable();
        bool IsProductOptionsConfigurable();
        bool ShouldShowUI();
        bool ShouldShowDebugMenu();

        LPCTSTR GetProductName();
        
        bool IsAdministratorAccount();
        bool IsGuestAccount();
        
        void SetShutdownRequest(HANDLE hShutdownRequest);
        void OnSessionAppServerExit(DWORD dwExitCode);
        void RequestExit(DWORD dwExitCode);
        DWORD GetExitCode();
        
        void OnElementUpdate(ui::IUpdate* pSource, LPVOID pCookie);

    protected:
        void LaunchElement (SYMGUID guidElementId);

    protected:
        SessionApp::ISessionAppServerPtr m_spSessionAppServer;
        SessionApp::ISessionAppServerSinkPtr m_spSessionAppServerSink;
        
        SettingsEventHelper_ISubject m_ISubjectLoader;
        SettingsEventHelper::ISubjectPtr m_spSubject;

        CString m_cszProductName;

        HANDLE m_hShutdownRequest;
        DWORD  m_dwExitCode;

        DWORD m_dwTrayIconState;
        DWORD m_dwDebugMenuState;

        ui::IProviderPtr m_spISElementProvider;
        ui::IElementPtr m_spAutoProtectStateElement;
        ui::IElementPtr m_spProductOptions;
        
        AVModule::AVLoader_IAVMapStrData AvLoaderMapStrData;
    };

    typedef StahlSoft::CSmartRefCountPtr<CAppManager> CAppManagerPtr;

    class CAppManagerSingleton
    {
    public:
        CAppManagerSingleton(void);
        virtual ~CAppManagerSingleton(void);

    public:
        static HRESULT Initialize()
        {
            m_spAppManager = new CAppManager;
            if(!m_spAppManager)
                return E_OUTOFMEMORY;

            return S_OK;
        };

        static void Destroy(){ m_spAppManager.Release(); };
        static CAppManager* GetAppManager(){ return m_spAppManager; };

    protected:
        static CAppManagerPtr m_spAppManager;
    };


    class CServerSinkImpl :
        public ISymBaseImpl< CSymThreadSafeRefCount >, 
        public AvProdWidgets::CSymObjectCreator<&SessionApp::IID_SessionAppServerSink, SessionApp::ISessionAppServerSink, CServerSinkImpl>,
        public SessionApp::ISessionAppServerSink,
        public SettingsEventHelper::IObserver,
        public ui::IUpdate
    {
    public: 
        CServerSinkImpl(void);
        ~CServerSinkImpl(void);

    public:
        SYM_INTERFACE_MAP_BEGIN()                
            SYM_INTERFACE_ENTRY( SessionApp::IID_SessionAppServerSink, SessionApp::ISessionAppServerSink ) 
            SYM_INTERFACE_ENTRY( SettingsEventHelper::IID_Observer, SettingsEventHelper::IObserver ) 
            SYM_INTERFACE_ENTRY( ui::IID_Update, ui::IUpdate ) 
        SYM_INTERFACE_MAP_END()                  

    public:  //  ISessionAppServerSink interface
        virtual void OnAppServerReady(SessionApp::ISessionAppServer* pAppServer);
        virtual void OnAppServerExit(SessionApp::ISessionAppServer* pAppServer, DWORD dwExitCode);
        virtual void OnPrepareMenu(SessionApp::ISessionAppServer* pAppServer, HMENU &hMenu);
        virtual void OnMenuCommand(SessionApp::ISessionAppServer* pAppServer, UINT nId);
        virtual void OnCommand(SessionApp::ISessionAppServer* pAppServer, const SYMGUID& guidCommand, ISymBase *pCommand, ISymBase **ppReturn);
        virtual void OnTrayItemDblClick(SessionApp::ISessionAppServer* pAppServer);
    
    public: // IObserver interface
        virtual void Update(const ccSettings::CSettingsChangeEventEx *pEvent,
            ccSettings::ISettings *pSettings) throw();

    public: // IUpdate interface
        virtual HRESULT Update(IUpdate* pSource, LPVOID pCookie) throw();
        virtual HRESULT Register(IUpdate* pUpdate, LPVOID pCookie) throw();
        virtual HRESULT Unregister(IUpdate* pUpdate) throw();

    public:
        void OnMenuCommand_Close();
        void OnMenuCommand_SetTrayIconState(DWORD dwState, DWORD dwDuration, LPCWSTR wszToolTipText);
        void OnCommand_OEHDetection(ISymBase *pCommand, ISymBase **ppReturn);
        void OnCommand_EmailThreatsDetected(ISymBase *pCommand, ISymBase **ppReturn);
        void OnCommand_RebootRequired(ISymBase *pCommand, ISymBase **ppReturn);
        void OnCommand_ProcTermRequired(ISymBase *pCommand, ISymBase **ppReturn);
        void OnCommand_AskUserForThreatRemediation(ISymBase *pCommand, ISymBase **ppReturn);
        void OnCommand_LaunchManualScanner(ISymBase *pCommand, ISymBase **ppReturn);

        void ShowOEHAlert(AVModule::IAVMapStrData* pOEHInfo, DWORD& dwOEHAction);
        void ShowThreatRemediationAlert(AVModule::IAVMapDwordData* pThreatTrackInfo, DWORD& dwAction);

    public:
        CAppManagerPtr m_spAppManager;
    };
    
    class CNotifySink_ViewRecentHistory :
        public ISymBaseImpl< CSymThreadSafeRefCount >, 
        public AvProdWidgets::CSymObjectCreator<&cc::IID_NotifyCallback, cc::INotifyCallback, CNotifySink_ViewRecentHistory>,
        public cc::INotifyCallback
    {
    public:
        CNotifySink_ViewRecentHistory() : m_bAlreadyRun(false) {};
        ~CNotifySink_ViewRecentHistory() {};

    public:
        SYM_INTERFACE_MAP_BEGIN()                
            SYM_INTERFACE_ENTRY( cc::IID_NotifyCallback, cc::INotifyCallback ) 
        SYM_INTERFACE_MAP_END()                  

    public: // INotifyCallback
        virtual bool Run(HWND hWndParent, unsigned long nID, cc::INotify* pAlert)
        {
            // Prevent launching multiple times
            if(m_bAlreadyRun)
                return true;

            m_bAlreadyRun = true;

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
                hrx << spISElementProvider->GetElement ( ISShared::CLSID_NIS_MessageCenter, pElement);
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

    protected:
        bool m_bAlreadyRun;
    };

}; // AvProdApp
