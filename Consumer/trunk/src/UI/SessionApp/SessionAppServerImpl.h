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

#include "SessionAppServerInterface.h"
#include "SessionAppWindowThread.h"
#include "SessionAppWorkerThread.h"

#include <ccSerializeInterface.h>
#include <ccSymMemoryStreamIMpl.h>
#include <ccInstanceFactory.h>
#include <ccKeyValueCollectionInterface.h>

namespace SessionApp
{

    //****************************************************************************
    //  Session App Server class
    //    This class will create a SysTray icon, and provide remote client
    //    management of app and notification state, as well as remote 
    //    Alert and Notification using common client IAlert and INotification 
    //    interfaces.
    //****************************************************************************
    class CServerImpl :
        public ISymBaseImpl< CSymThreadSafeRefCount >, 
        public AvProdWidgets::CSymObjectCreator<&SessionApp::IID_SessionAppServer, SessionApp::ISessionAppServer, CServerImpl>,
        public SessionApp::ISessionAppServer
    {
    public:
        CServerImpl(void);
        virtual ~CServerImpl(void);
    public:
        SYM_INTERFACE_MAP_BEGIN()                
            SYM_INTERFACE_ENTRY( SessionApp::IID_SessionAppServer, SessionApp::ISessionAppServer ) 
        SYM_INTERFACE_MAP_END()                  

    public:  // ISessionAppServer interface
        virtual HRESULT Initialize(LPCWSTR wszApplicationName);
        virtual void Destroy();
        virtual HRESULT RegisterEventSink(SessionApp::ISessionAppServerSink* pEventSink);
        virtual HRESULT UnRegisterEventSink(SessionApp::ISessionAppServerSink* pEventSink);
        virtual HRESULT GetApplicationName(cc::IString*& pstrApplicationName);
        virtual HRESULT SetupTrayIcon(const DWORD dwState, const HMODULE hModule, UINT iIcon);
        virtual HRESULT SetupToolTipText(const DWORD dwState, const HMODULE hModule, UINT iToolTip);
        virtual HRESULT SetupToolTipText(const DWORD dwState, LPCWSTR wszToolTipText);
        virtual HRESULT ShowTrayApp(DWORD dwState);
        virtual HRESULT HideTrayApp();
        virtual HRESULT SetContextMenu(const HMODULE hModule, const UINT iMenu);
        virtual HRESULT SetDefaultMenuItem(const UINT iItem);
        virtual HRESULT GetOption(const DWORD dwOptionId, DWORD_PTR &pOption);
        virtual HRESULT SetOption(const DWORD dwOptionId, const DWORD_PTR pOption);

    public:  // ISessionAppServerSink callbacks
        virtual void Fire_OnAppServerReady();
        virtual void Fire_OnPrepareMenu(HMENU &hMenu);
        virtual void Fire_OnMenuCommand(UINT nId);
        virtual void Fire_OnTrayItemDblClick();

    public:
        void OnPrepareMenu(HMENU &hMenu);
        void OnMenuCommand(UINT iMenuCommandId);
        void OnTrayAppReady();
        void OnTrayItemDblClick();

    public:
        void OnNotifyMessage(const SYMGUID& gCommandGuid, LPCWSTR szCallbackChannel,
            ISymBase* pBase, ISymBase** ppOutBase);

        void OnAlertMessage(const SYMGUID& gCommandGuid, LPCWSTR szCallbackChannel,
            ISymBase* pBase, ISymBase** ppOutBase);

        void OnSetIconState(const SYMGUID& gCommandGuid, LPCWSTR szCallbackChannel,
            ISymBase* pBase, ISymBase** ppOutBase);

        void OnGetIconState(const SYMGUID& gCommandGuid, LPCWSTR szCallbackChannel,
            ISymBase* pBase, ISymBase** ppOutBase);
    
        void OnCommand(const SYMGUID& gCommandGuid, LPCWSTR szCallbackChannel,
            ISymBase* pBase, ISymBase** ppOutBase);

    protected:
        DWORD GetCurrentSessionId();

    protected:
        CAppWindowThread m_cAppWindowThread;
        CMTAWorkerMessageThread m_cWorkerThread;

        CString m_cszProductName;
        CString m_cszSessionChannel;

        ccService::IBiDirectionalServerPtr m_spCommServer;
        ccService::ICommandHandlerPtr m_spCommandHandler;
        ccService::IComLibPtr m_spCommLib;
        
        SessionApp::ISessionAppServerSinkPtr m_spSessionAppServerSink;

        HMODULE m_hModuleMenu;
        UINT m_iContextMenuId;

        StahlSoft::CSmartHandle m_shTrayAppReady;
    };

    template<const SYMINTERFACE_ID* _piidInterface, class _Interface>
    class CBIN2SymObject
    {
    public: // c'tor / d'tor
        CBIN2SymObject(){};
        ~CBIN2SymObject(){};

    public: // methods
        HRESULT SetBIN(const SYMGUID* pObjId, LPVOID pData, const ULONGLONG qdwDataSize)
        {
            HRESULT hrReturn = E_FAIL;
            m_spSymObject.Release();

            for(;;)
            {
                ccSym::CMemoryStreamImplPtr spStream = ccSym::CMemoryStreamImpl::CreateMemoryStreamImpl();
                if(!spStream)
                    break;

                spStream->GetMemory().Reference(pData, (size_t)qdwDataSize);

                cc::IInstanceFactoryQIPtr spInstanceFactory;
                spInstanceFactory = ccLib::CInstanceFactory::GetInstanceFactory();
                if(!spInstanceFactory)
                    break;

                SYMRESULT sr;
                sr = spInstanceFactory->CreateInstance(*pObjId, *_piidInterface, (void**)&m_spSymObject);
                if(SYM_FAILED(sr) || !m_spSymObject)
                    break;

                cc::ISerializeQIPtr spSerialize = m_spSymObject;
                if(!spSerialize)
                    break;

                if(!spSerialize->Load(spStream))
                {
                    m_spSymObject.Release();
                    break;
                }

                hrReturn = S_OK;
                break;
            }

            return hrReturn;        
        };

        operator _Interface*()
        {
            return m_spSymObject;
        }

    protected:
        CSymQIPtr<_Interface, _piidInterface> m_spSymObject;

    };
    
    typedef CBIN2SymObject<&IID_SymBase, ISymBase> CBIN2SymBase;

}; // SessionApp