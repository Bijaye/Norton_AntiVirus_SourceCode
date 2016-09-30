////////////////////////
//
// PROPRIETARY / CONFIDENTIAL.
// Use of this product is subject to license terms.
// Copyright © 2006 Symantec Corporation.
// All rights reserved.
//
/////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once
#include <ccServiceInterface.h>

namespace AvProdSvc
{
    
    class CSymSvcPlugin :
        public ISymBaseImpl< CSymThreadSafeRefCount >,
        public ccService::IServicePlugin
    {
    public:
        CSymSvcPlugin();
        virtual ~CSymSvcPlugin(void);

        // Interface map
        //
        SYM_INTERFACE_MAP_BEGIN()
            SYM_INTERFACE_ENTRY( ccService::IID_ServicePlugin, ccService::IServicePlugin )
        SYM_INTERFACE_MAP_END()

        // ccService::ISymSvcPlugin Methods
        virtual HRESULT Load( ccService::IService* pCommand ) throw();
        virtual HRESULT PrepareDestroy( DestroyCause eCause ) throw();
        virtual HRESULT Destroy( ) throw();

    private:
        // Disallowed
        CSymSvcPlugin( const CSymSvcPlugin& other );
        CSymSvcPlugin& operator =(const CSymSvcPlugin&);

    };

}; // AvProdSvc