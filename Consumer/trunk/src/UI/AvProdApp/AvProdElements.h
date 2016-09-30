////////////////////////
//
// PROPRIETARY / CONFIDENTIAL.
// Use of this product is subject to license terms.
// Copyright © 2006 Symantec Corporation.
// All rights reserved.
//
/////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

namespace AvProdApp
{
    enum {AutoProtectState_Invalid = -1, AutoProtectState_Enabled, AutoProtectState_Disabled, AutoProtectState_Unknown};
    enum {NAVAP_ShowTrayIcon_Invalid = -1, NAVAP_ShowTrayIcon_FALSE = 0, NAVAP_ShowTrayIcon_TRUE };
    enum {DBGOPTS_DebugMenu_Invalid = -1, DBGOPTS_ShowDebugMenu_FALSE = 0, DBGOPTS_ShowDebugMenu_TRUE };

    class CElementBase
    {
        virtual HRESULT GetId(GUID &guidElementID) = 0;
        virtual HRESULT IsConfigurable(bool &bConfigurable) = 0;
        virtual HRESULT GetConfig(DWORD &dwConfig) = 0;
        virtual HRESULT GetState(DWORD &dwState) = 0;
        virtual HRESULT SetConfig(DWORD dwConfig) = 0;
    };

    class CAutoProtectElement : 
        public CElementBase
    {
    public:
        CAutoProtectElement(void);
        virtual ~CAutoProtectElement(void);
        
        virtual HRESULT GetId(GUID &guidElementID) ;
        virtual HRESULT IsConfigurable(bool &bConfigurable) ;
        virtual HRESULT GetConfig(DWORD &dwConfig) ;
        virtual HRESULT GetState(DWORD &dwState) ;
        virtual HRESULT SetConfig(DWORD dwConfig) ;

    protected:
        DWORD m_dwCurrentConfig;
        DWORD m_dwCurrentState;
    };

    class CTrayIconElement :
        public CElementBase
    {
        CTrayIconElement(void);
        virtual ~CTrayIconElement(void);

        virtual HRESULT GetId(GUID &guidElementID) ;
        virtual HRESULT IsConfigurable(bool &bConfigurable) ;
        virtual HRESULT GetConfig(DWORD &dwConfig) ;
        virtual HRESULT GetState(DWORD &dwState) ;
        virtual HRESULT SetConfig(DWORD dwConfig) ;

    protected:
        DWORD m_dwCurrentConfig;
        DWORD m_dwCurrentState;

    };
}
