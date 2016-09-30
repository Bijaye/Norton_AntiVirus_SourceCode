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
#include <AvProdSvcClientInterface.h>
#include <ccServiceInterface.h>

namespace AvProdSvc
{
    class CAvProdSvcClientImpl :
        public ISymBaseImpl< CSymThreadSafeRefCount >, 
        public AvProdWidgets::CSymObjectCreator<&IID_AvProdSvcClient, IAvProdSvcClient, CAvProdSvcClientImpl>,
        public IAvProdSvcClient
    {
    public:
        CAvProdSvcClientImpl(void);
        virtual ~CAvProdSvcClientImpl(void);

    public: // Interface Map
        SYM_INTERFACE_MAP_BEGIN()                
            SYM_INTERFACE_ENTRY(IID_AvProdSvcClient, IAvProdSvcClient ) 
        SYM_INTERFACE_MAP_END()                  

    public: // IAvProdSvcClient implementation
        virtual HRESULT Initialize() throw();
        virtual void Destroy() throw();
        virtual HRESULT DisableRebootDialog(cc::IString** ppstrClientCookie = NULL) throw();
        virtual HRESULT EnableRebootDialog(const cc::IString* pstrClientCookie = NULL, bool bProcessPendingRebootRequests = true, bool bForceRebootDialog = false) throw();
		virtual HRESULT LogManualScanResults(REFGUID guidScanId) throw();

    protected: // members
        ccService::IClientPtr m_spCommClient;
        ccService::IComLibPtr m_spCommLib;
        cc::IStringPtr m_spClientCookie;

    }; // class CAvProdSvcClientImpl

}; // namespace AvProdSvc
