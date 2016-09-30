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
#include "SessionAppClientInterface.h"

#include <ccCriticalSection.h>
#include <ccSingleLock.h>
#include <ccSerializeInterface.h>
#include <ccSymMemoryStreamIMpl.h>
#include <ccInstanceFactory.h>
#include <ccKeyValueCollectionInterface.h>

namespace SessionApp
{
    class CClientImpl : 
        public ISymBaseImpl< CSymThreadSafeRefCount >, 
        public AvProdWidgets::CSymObjectCreator<&SessionApp::IID_SessionAppClient, SessionApp::ISessionAppClient, CClientImpl>,
        public SessionApp::ISessionAppClient
    {
    public:
        CClientImpl(void);
        ~CClientImpl(void);

    public:
        SYM_INTERFACE_MAP_BEGIN()                
            SYM_INTERFACE_ENTRY( SessionApp::IID_SessionAppClient, SessionApp::ISessionAppClient ) 
        SYM_INTERFACE_MAP_END()                  


    public:
        //  ISessionAppClient interface
        virtual HRESULT Initialize(LPCWSTR wszApplicationName, DWORD dwSessionId = -1);
        virtual void Destroy();

        virtual HRESULT GetCurrentAppState(DWORD &dwAppState);
        virtual HRESULT SetCurrentAppState(const DWORD dwAppState);

        virtual HRESULT GetCurrentNotifyState(DWORD &dwNotifyState, LPDWORD pdwRemaining, cc::IString** ppstrToolTip);
        virtual HRESULT SetCurrentNotifyState(const DWORD dwNotifyState, const DWORD dwDuration, LPCWSTR wszToolTip);

        virtual HRESULT CreateNotification(cc::INotify*& pNotification);
        virtual HRESULT CreateAlert(cc::IAlert*& pAlert);

        virtual HRESULT LaunchApplication(LPCWSTR wszCommandLine);

        virtual HRESULT GetUserName(cc::IString*& pstrUserName);
        virtual HRESULT IsAdministrator(bool &bIsAdministrator);
        virtual HRESULT IsGuest(bool &bIsGuest);

        virtual HRESULT GetOption(const DWORD dwOptionId, DWORD_PTR pOption);
        virtual HRESULT SetOption(const DWORD dwOptionId, DWORD_PTR pOption);

        virtual HRESULT SendCommand(const SYMGUID& guidCommandId, ISymBase *pCommand, ISymBase **ppReturn);

        void OnCommand(const SYMGUID& gCommandGuid, LPCWSTR szCallbackChannel,
            ISymBase* pBase, ISymBase** ppOutBase);

    protected:
        DWORD GetActiveSessionId();
        DWORD GetCurrentSessionId();
        HRESULT GetCurrentIconState(LPDWORD pdwAppState, LPDWORD pdwNotifyState = NULL, LPDWORD pdwDuration = NULL, cc::IString** ppstrToolTipText = NULL);
        HRESULT SetCurrentIconState(DWORD dwAppState, DWORD dwNotifyState = SessionApp::NotifyState::eInvalid, DWORD dwDuration = NULL, LPCWSTR wszToolTipText = NULL);

    protected:
        ccService::IBiDirectionalClientPtr m_spCommClient;
        ccService::IComLibPtr m_spCommLib;
        ccService::ICommandHandlerPtr m_spCommandHandler;

        ISymBasePtr m_spReturnCommand;
        
        ccLib::CEvent m_seReturnCommandReady;
        ccLib::CCriticalSection m_CustomCommandLock;

        CString m_cszAppName;
        DWORD m_dwSessionId;
    };

    template<const SYMINTERFACE_ID* _piidInterface, class _Interface>
    class CSymObject2BIN
    {
    public:
        CSymObject2BIN(){ ZeroMemory(&m_guidSymObjectId, sizeof(SYMGUID)); };
        ~CSymObject2BIN(){};

    public:
        HRESULT SetSymObject(_Interface* pSymObject)
        {
            HRESULT hrReturn = E_FAIL;
            m_spStream.Release();

            for(;;)
            {
                cc::ISerializeQIPtr spSerialize = pSymObject;
                if(!spSerialize)
                    break;

                spSerialize->GetObjectId(m_guidSymObjectId);

                m_spStream = ccSym::CMemoryStreamImpl::CreateMemoryStreamImpl();
                if(!m_spStream)
                    break;

                if(!spSerialize->Save(m_spStream))
                    break;

                hrReturn = S_OK;
                break;
            }

            return hrReturn;
        }

        LPVOID GetBIN()
        {
            if(!GetSize())
                return NULL;

            return m_spStream->GetMemory();
        }
        
        cc::IStream* GetStreamObj()
        {
            return (cc::IStream*)m_spStream;
        }

        const SYMGUID * GetObjectId()
        {
            return &m_guidSymObjectId;
        }

        const ULONGLONG GetSize()
        {
            ULONGLONG qdwSize = 0;
            m_spStream->GetSize(qdwSize);

            return qdwSize;
        }

        operator LPVOID()
        {
            return GetBIN();
        }

        operator const ULONGLONG()
        {
            return GetSize();
        }

        operator const size_t()
        {
            return (const size_t)GetSize();
        }

    protected:
        ccSym::CMemoryStreamImplPtr m_spStream;
        SYMGUID m_guidSymObjectId;
    };

    typedef CSymObject2BIN<&IID_SymBase, ISymBase> CSymBase2BIN;

    class CSessionAppClientComm :     
        public ISymBaseImpl< CSymThreadSafeRefCount >,
        public AvProdWidgets::CSymObjectCreator<&ccService::IID_CommandHandler, ccService::ICommandHandler, CSessionAppClientComm>,
        public ccService::ICommandHandler
    {
    public:
        CSessionAppClientComm(){};
        virtual ~CSessionAppClientComm(void){};

        // Interface map
        //
        SYM_INTERFACE_MAP_BEGIN()
            SYM_INTERFACE_ENTRY( ccService::IID_CommandHandler, ccService::ICommandHandler )
        SYM_INTERFACE_MAP_END()

        // ccService::ICommand methods.
        //
        virtual HRESULT Command( const SYMGUID& gCommandGuid, LPCWSTR szCallbackChannel,
        ISymBase* pBase, ISymBase** ppOutBase );

    public:
        CClientImpl* m_pSessionAppClient;

    private:
        // Disallowed
        CSessionAppClientComm( const CSessionAppClientComm& other );
        CSessionAppClientComm& operator =(const CSessionAppClientComm&);

    protected:

    };

    inline HRESULT CSessionAppClientComm::Command( const SYMGUID& gCommandGuid, LPCWSTR szCallbackChannel,
        ISymBase* pBase, ISymBase** ppOutBase )
    {
        if(!m_pSessionAppClient)
            return S_OK;

        m_pSessionAppClient->OnCommand(gCommandGuid, szCallbackChannel, pBase, ppOutBase);
        return S_OK;
    }

}; // SessionApp