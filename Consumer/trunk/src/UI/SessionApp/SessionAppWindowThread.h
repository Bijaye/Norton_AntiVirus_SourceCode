////////////////////////
//
// PROPRIETARY / CONFIDENTIAL.
// Use of this product is subject to license terms.
// Copyright © 2006 Symantec Corporation.
// All rights reserved.
//
/////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include <SysThreadBase.h>

namespace SessionApp
{
    // Forward declaration
    class CServerImpl;

    class CAppWindowThread : public StahlSoft::CThread
    {
    public:
        CAppWindowThread(void);
        virtual ~CAppWindowThread(void);

    public:
        //+
        //+ CRunnable Overridden Virtual Methods
        //+
        STDMETHOD(Run)();
        STDMETHOD(IsRunning)();
        STDMETHOD(RequestExit)();
    
    public:
        CAppWindow m_wndAppWindow;
        CServerImpl* m_pSessionAppServer;

    protected:
        StahlSoft::CSmartHandle m_hRunning;
    };

}; // AvProdApp
