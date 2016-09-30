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

#include <AvProdWidgets.h>

namespace AvProdSvc
{
    class CSvcComm :     
        public ISymBaseImpl< CSymThreadSafeRefCount >,
        public AvProdWidgets::CSymObjectCreator<&ccService::IID_CommandHandler, ccService::ICommandHandler, CSvcComm>,
        public ccService::ICommandHandler
    {
    public:
        CSvcComm();
        virtual ~CSvcComm(void);

        // Interface map
        //
        SYM_INTERFACE_MAP_BEGIN()
            SYM_INTERFACE_ENTRY( ccService::IID_CommandHandler, ccService::ICommandHandler )
        SYM_INTERFACE_MAP_END()

        // ccService::ICommand methods.
        //
        virtual HRESULT Command( const SYMGUID& gCommandGuid, LPCWSTR szCallbackChannel,
                                ISymBase* pBase, ISymBase** ppOutBase );

    private:
        // Disallowed
        CSvcComm( const CSvcComm& other );
        CSvcComm& operator =(const CSvcComm&);

    protected:

    };

}; // AvProdSvc