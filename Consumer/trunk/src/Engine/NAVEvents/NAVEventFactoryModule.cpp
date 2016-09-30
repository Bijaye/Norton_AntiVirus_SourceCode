////////////////////////
//
// PROPRIETARY / CONFIDENTIAL.
// Use of this product is subject to license terms.
// Copyright © 2006 Symantec Corporation.
// All rights reserved.
//
/////////////////////////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"

#define INITIIDS
#include "SymInterface.h"
#include "NAVTrust.h"
#include "ccSettingsInterface.h"
#include "ccEventManagerHelperEx.h"
#include "naveventfactorymodule.h"
#include "naveventfactory.h"
#include "naveventlogfactory.h"
#include "naveventfactoryloader.h"
#include "naveventcommon.h"
#include "naveventproxyfactory.h"
#include "AvFactory.h"


// ccEraser interface
#include "ccEraserInterface.h"

SYM_TRUSTED_CACHE_CLIENT_OBJECT_MAP_BEGIN()
    SYM_OBJECT_ENTRY(ccEvtMgr::IID_EventManagerModuleEx, CNAVEventFactoryModule)
    SYM_OBJECT_ENTRY(ccEvtMgr::IID_EventFactoryEx, CNAVEventFactory)
    SYM_OBJECT_ENTRY(ccEvtMgr::IID_EventFactoryEx2, CNAVEventFactory)
    SYM_OBJECT_ENTRY(ccEvtMgr::IID_ProxyFactoryEx, CNAVEventProxyFactory)
    SYM_OBJECT_ENTRY(IID_NAVEventCommon, CNAVEventCommon)
    SYM_OBJECT_ENTRY(AV::IID_AvEventFactory, CAvFactory )
SYM_OBJECT_MAP_END()

#ifdef _DEBUG
    #define new DEBUG_NEW
#endif

using namespace ccLib;
using namespace ccEvtMgr;

CNAVEventFactoryModule::CNAVEventFactoryModule(void) :
    m_nModuleId(0)
{
    //::MessageBox ( ::GetDesktopWindow (), "Break", "NAVEvents", MB_OK );
    m_EventFactory.AddRef();
    m_LogFactory.AddRef();
}

CNAVEventFactoryModule::~CNAVEventFactoryModule(void)
{
    Destroy();
}

CError::ErrorType CNAVEventFactoryModule::Create(long nModuleId,
                                                 CEventManagerEx* pEventManager, 
                                                 CLogManagerEx* pLogManager)
{
    if ( NAVToolbox::NAVTRUST_OK != NAVToolbox::IsSymantecApplication ())
        return ccEvtMgr::CError::eFatalError;

    // This DLL is also a COM DLL and the interfaces need to be 
    // registered for successful operation
    _Module.RegisterServer(TRUE);

    // Register the event and log factory
    CError::ErrorType eError = CError::eNoError;
    for (;;)
    {
        // Register the event factory
        if (m_EventFactory.Register(nModuleId, pEventManager) == FALSE)
        {
            CCTRACEE(_T("CNAVEventFactoryModule::Create() : m_EventFactory.Register() == FALSE\n"));
            eError = CError::eFatalError;
            break;
        }
    
        // Register the log factory
        ccLib::CString strNAVDir;
        ccSym::CInstalledApps::GetNAVDirectory( strNAVDir );

        if (m_LogFactory.Register(nModuleId,
                                  pLogManager,
                                  &m_EventFactory,
                                  strNAVDir) == FALSE)
        {
            CCTRACEE(_T("CNAVEventFactoryModule::Create() : m_LogFactory.Register() == FALSE\n"));
            eError = CError::eFatalError;
            break;
        }

        break;
    }

    if (eError != CError::eNoError)
    {
        Destroy();
    }
    else
    {
        // Save input
        m_nModuleId = nModuleId;
        m_pEventManager = pEventManager;
        m_pLogManager = pLogManager;
    }
    return eError;
}

CError::ErrorType CNAVEventFactoryModule::Destroy()
{
    // Unregister the log factory
    CError::ErrorType eError = PrepareDestroy();
    if (eError != CError::eNoError)
    {
        CCTRACEE(_T("CNAVEventFactoryModule::Destroy() : PrepareDestroy() != CError::eNoError\n"));
        return eError;
    }

    // Unregister the event factory
    if (m_EventFactory.Unregister() == FALSE)
    {
        CCTRACEE(_T("CNAVEventFactoryModule::Destroy() : m_EventFactory.Unregister() == FALSE\n"));
        return CError::eFatalError;
    }

    // Clear 
    m_nModuleId = 0;
    m_pEventManager.Release();
    m_pLogManager.Release();

    return CError::eNoError;
}

CError::ErrorType CNAVEventFactoryModule::PrepareDestroy()
{
    // Unregister the log factory
    m_LogFactory.Unregister();
    return CError::eNoError;
}
