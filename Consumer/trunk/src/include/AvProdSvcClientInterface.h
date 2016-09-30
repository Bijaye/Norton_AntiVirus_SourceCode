////////////////////////
//
// PROPRIETARY / CONFIDENTIAL.
// Use of this product is subject to license terms.
// Copyright © 2006 Symantec Corporation.
// All rights reserved.
//
/////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include <SymInterface.h>

namespace AvProdSvc
{
    class IAvProdSvcClient : public ISymBase
    {
    public:
        virtual HRESULT Initialize() throw() = 0;
        virtual void Destroy() throw() = 0;
        virtual HRESULT DisableRebootDialog(cc::IString** ppstrClientCookie = NULL) throw() = 0;
        virtual HRESULT EnableRebootDialog(const cc::IString* pstrClientCookie = NULL, bool bProcessPendingRebootRequests = true, bool bForceRebootDialog = false) throw() = 0;
		virtual HRESULT LogManualScanResults(REFGUID guidScanId) throw() = 0;
	};
    
    // {63742053-57F9-497f-AF60-20808FB768C2}
    SYM_DEFINE_GUID(IID_AvProdSvcClient, 
        0x63742053, 0x57f9, 0x497f, 0xaf, 0x60, 0x20, 0x80, 0x8f, 0xb7, 0x68, 0xc2);

    typedef CSymPtr<IAvProdSvcClient> IAvProdSvcClientPtr;
    typedef CSymQIPtr<IAvProdSvcClientPtr, &IID_AvProdSvcClient> IAvProdSvcClientQIPtr;

}