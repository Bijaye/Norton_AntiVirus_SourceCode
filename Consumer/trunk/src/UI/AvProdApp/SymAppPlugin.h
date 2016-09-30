////////////////////////
//
// PROPRIETARY / CONFIDENTIAL.
// Use of this product is subject to license terms.
// Copyright © 2006 Symantec Corporation.
// All rights reserved.
//
/////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include <ccAppPlugin.h>

namespace AvProdApp
{
    //
    // AvProd SymApp Plugin Class ID
    //
    // {3C9E3F21-2187-4d36-80C2-27AFEA2682E9}
    SYM_DEFINE_INTERFACE_ID(CLSID_SymAppPlugin, 
        0x3c9e3f21, 0x2187, 0x4d36, 0x80, 0xc2, 0x27, 0xaf, 0xea, 0x26, 0x82, 0xe9);

    class CSymAppPlugin : 
        public ISymBaseImpl< CSymThreadSafeRefCount >, 
        public ICCAppPlugin
    {
    public:
        CSymAppPlugin(void);
        virtual ~CSymAppPlugin(void);

        SYM_INTERFACE_MAP_BEGIN()                
            SYM_INTERFACE_ENTRY( IID_CCAppPlugin, ICCAppPlugin ) 
            SYM_INTERFACE_MAP_END()                  

            // ICCAppPlugin
            virtual void Run( ICCApp* pCCApp );
        virtual bool CanStopNow();
        virtual void RequestStop() ;
        virtual const char * GetObjectName();
        virtual unsigned long OnMessage( int iMessage, unsigned long ulParam );


    protected:
        // Pointer to ISymApp object.
        CSymPtr< ICCApp >	m_pApp;

        // Thread terminationg event
        ccLib::CEvent	m_endSessionEvent;

        // Thread terminationg event
        ccLib::CEvent	m_ccEvtMgrReconnectEvent;


    };

}; // AvProdApp
