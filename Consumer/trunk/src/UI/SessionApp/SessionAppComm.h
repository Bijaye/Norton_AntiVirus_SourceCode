////////////////////////
//
// PROPRIETARY / CONFIDENTIAL.
// Use of this product is subject to license terms.
// Copyright © 2006 Symantec Corporation.
// All rights reserved.
//
/////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once
#include "SessionAppServerInterface.h"

#include <AvProdWidgets.h>

namespace SessionApp
{
    // Forward declaration
    class CServerImpl;

    class CAppComm :     
        public ISymBaseImpl< CSymThreadSafeRefCount >,
        public AvProdWidgets::CSymObjectCreator<&ccService::IID_CommandHandler, ccService::ICommandHandler, CAppComm>,
        public ccService::ICommandHandler
    {
    public:
        CAppComm();
        virtual ~CAppComm(void);

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
        CServerImpl* m_pSessionAppServer;

    private:
        // Disallowed
        CAppComm( const CAppComm& other );
        CAppComm& operator =(const CAppComm&);

    protected:

    };

}; // SessionApp
