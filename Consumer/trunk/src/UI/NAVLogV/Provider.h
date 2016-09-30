////////////////////////
//
// PROPRIETARY / CONFIDENTIAL.
// Use of this product is subject to license terms.
// Copyright © 2006 Symantec Corporation.
// All rights reserved.
//
/////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "ccLogManagerHelper.h"
#include "ccEventManagerHelper.h"
#include "ccSubscriberHelper.h"

#include "StockFrameworkObjectsInterface.h"
#include "MCFWidgets.h"

#include "AvInterfaceLoader.h"
#include "QBackupLoader.h"
#include "SearchContextHelper.h"

//
// MCAntivirus Provider ID
//
// {10607CDD-FBD0-4f26-8D01-888BE27DF76E}
DEFINE_GUID(MCPROVIDERID_MCAntivirusProviderId, 
            0x10607cdd, 0xfbd0, 0x4f26, 0x8d, 0x1, 0x88, 0x8b, 0xe2, 0x7d, 0xf7, 0x6e);


class CMCAVProvider : 
    public MCF::CSymObjectCreator<&MCF::IID_MessageProvider, MCF::IMessageProvider, CMCAVProvider>,
    public MCF::IMessageProvider,
    public ccEvtMgr::CSubscriberHelper

{
public:
    CMCAVProvider(void);
    virtual ~CMCAVProvider(void);

    SYM_INTERFACE_MAP_BEGIN()               
        SYM_INTERFACE_ENTRY(MCF::IID_MessageProvider, MCF::IMessageProvider)
    SYM_INTERFACE_MAP_END()    

public: // IMessageProvider interface
    virtual HRESULT Initialize();
    virtual HRESULT Uninitialize();
    virtual HRESULT GetCategories(MCF::ICategoryList*& pCategoriesList);
    virtual HRESULT RegisterProviderSink(MCF::IMessageProviderSink* pMessageProviderSink);
    virtual HRESULT GetProviderId(GUID& guidProviderId);
    virtual HRESULT QueryMessages(MCF::ISearchContext* pSearchContext, MCF::IMessageList*& pMessageList);
    virtual HRESULT RenderMessage(REFGUID RenderFormat, MCF::IMessage* pMessage, ISymBase*& pRenderer);

public: // CSubscriberEx overrides
    virtual ccEvtMgr::CError::ErrorType OnEvent(long nSubscriberId, 
                                                const ccEvtMgr::CEventEx& Event,
                                                EventAction& eAction) throw();
    virtual ccEvtMgr::CError::ErrorType OnShutdown() throw();


protected:  // Methods
    HRESULT RetrieveEvents(DWORD dwEventId, MCF::IMessageListInternal* pEventList, MCF::CSearchContextDomainHelper& cSearchHelper);
    HRESULT RetrieveAvModuleItems(MCF::IMessageListInternal* pEventList, MCF::CSearchContextDomainHelper& cSearchHelper);

    HRESULT VerifyConfigState();
    HRESULT VerifyLicenseState();

protected: // Member variables

    // The AV stand-alone interface is used during safe mode so the CAV components load in-proc
    AVModule::AVLoader_IAVStandAlone m_avSafeModeLoader;
    AVModule::IAVStandAlonePtr m_spStandAlone;

    // Pointer to event manager object
    ccEvtMgr::CEventFactoryEx2Ptr m_pEventFactory;
    ccEvtMgr::CProxyFactoryExPtr m_pProxyFactory;
    
    ccEvtMgr::CEventManagerHelper m_EventManager; 
    ccEvtMgr::CLogManagerHelper m_LogManagerHelper;
    
    MCF::IMessageProviderSinkPtr m_pMessageProviderSink;
    
    AVModule::AVLoader_IAVThreatInfo AvThreatInfoLoader;
    bool m_bIsSafeMode;
    bool m_bEvtMgrDestroyed;
    bool m_bInitialized;
};
