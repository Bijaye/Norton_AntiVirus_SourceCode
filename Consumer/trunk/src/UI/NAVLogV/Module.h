////////////////////////
//
// PROPRIETARY / CONFIDENTIAL.
// Use of this product is subject to license terms.
// Copyright © 2006 Symantec Corporation.
// All rights reserved.
//
/////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "StockFrameworkObjectsInterface.h"

//
// MCAV Module Class ID
//
// {18190B0A-A198-43a6-BFFF-F023B7FC0977}
SYM_DEFINE_INTERFACE_ID(CLSID_MCAVModule, 
        0x18190b0a, 0xa198, 0x43a6, 0xbf, 0xff, 0xf0, 0x23, 0xb7, 0xfc, 0x9, 0x77);

class CMCAVModule : 
    public ISymBaseImpl<CSymThreadSafeRefCount>,
    public MCF::IProviderModule
{
public:
    CMCAVModule(void);
    virtual ~CMCAVModule(void);

    SYM_INTERFACE_MAP_BEGIN()               
        SYM_INTERFACE_ENTRY(MCF::IID_ProviderModule, MCF::IProviderModule)
    SYM_INTERFACE_MAP_END()    

public: // IProviderModule methods
    virtual HRESULT Initialize();
    virtual HRESULT GetMessageProviders(MCF::IMessageProviderList*& pProviderModuleList);
    virtual HRESULT QueryTier2Providers(MCF::IStringList*& pTier2ProvidersList);

protected:
    MCF::IMessageProviderListInternalPtr m_spMessageProviderList;

};
