////////////////////////
//
// PROPRIETARY / CONFIDENTIAL.
// Use of this product is subject to license terms.
// Copyright © 2006 Symantec Corporation.
// All rights reserved.
//
/////////////////////////////////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "Provider.h"
#include "Module.h"

#include <StockLists.h>
#include <ccSymStringImpl.h>
#include <SubmissionEngineLoader.h>

//****************************************************************************
//****************************************************************************
//****************************************************************************
//****************************************************************************
CMCAVModule::CMCAVModule(void)
{
}

CMCAVModule::~CMCAVModule(void)
{
}

//****************************************************************************
//****************************************************************************
HRESULT CMCAVModule::Initialize()
{
    HRESULT hrReturn = E_FAIL; 
    STAHLSOFT_HRX_TRY_NO_DECLARE_HR
    {
        hrx <<  MCF::CStockMessageProviderList::CreateObject(m_spMessageProviderList);

        //
        //  Create and init Provider
        //
        MCF::IMessageProviderPtr spProvider;
        hrx << CMCAVProvider::CreateObject(spProvider);
        
        // Add Provider to list
        hrx << m_spMessageProviderList->Add(spProvider);
        
        hrReturn = S_OK;
    }
    STAHLSOFT_HRX_CATCH_ALL_NO_RETURN(hrReturn)

    return hrReturn;
}

//****************************************************************************
//****************************************************************************
HRESULT CMCAVModule::GetMessageProviders(MCF::IMessageProviderList*& pProviderModuleList)
{
    if(!m_spMessageProviderList)
        return E_NOINTERFACE;

    SYMRESULT sr = m_spMessageProviderList->QueryInterface(MCF::IID_MessageProviderList, (void**)&pProviderModuleList);
    return MCF::HRESULT_FROM_SYMRESULT(sr);
}

//****************************************************************************
//****************************************************************************
HRESULT CMCAVModule::QueryTier2Providers(MCF::IStringList*& pTier2ProvidersList)
{
    HRESULT hrReturn = E_FAIL; 
    STAHLSOFT_HRX_TRY_NO_DECLARE_HR
    {
        MCF::IStringListInternalPtr spProviderList;
        MCF::CStockStringList::CreateObject(spProviderList);
        if(!spProviderList)
        {
            CCTRACEE( CCTRCTX _T("Unable to create string list!!"));
            hrx << E_FAIL;
        }

        cc::IStringPtr spProvider;
        spProvider.Attach(ccSym::CStringImpl::CreateStringImpl(CLFS_MCF_PLUGIN_NAME));
        
        spProviderList->Add(spProvider);

        SYMRESULT sr = spProviderList->QueryInterface(MCF::IID_StringList, (void**)&pTier2ProvidersList);
        hrReturn = MCF::HRESULT_FROM_SYMRESULT(sr);
    }
    STAHLSOFT_HRX_CATCH_ALL_NO_RETURN(hrReturn)

    return hrReturn;
}
