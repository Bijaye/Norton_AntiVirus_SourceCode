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
#include <ccStringInterface.h>
#include <ccServiceInterface.h>

#include "SessionAppClientInterface.h"

namespace SessionApp
{
    class ISessionAppServer;
    class ISessionAppServerSink;

    //****************************************************************************
    //  Session App Server Sink interface
    //****************************************************************************
    class ISessionAppServerSink : public ISymBase
    {
    public:
        virtual void OnAppServerReady(SessionApp::ISessionAppServer* pAppServer) throw() = 0;
        virtual void OnAppServerExit(SessionApp::ISessionAppServer* pAppServer, DWORD dwExitCode) throw() = 0;
        virtual void OnPrepareMenu(SessionApp::ISessionAppServer* pAppServer, HMENU &hMenu) throw() = 0;
        virtual void OnMenuCommand(SessionApp::ISessionAppServer* pAppServer, UINT nId) throw() = 0;
        virtual void OnCommand(SessionApp::ISessionAppServer* pAppServer, const SYMGUID& guidCommand, ISymBase *pCommand, ISymBase **ppReturn) throw() = 0;
        virtual void OnTrayItemDblClick(SessionApp::ISessionAppServer* pAppServer) throw() = 0;
    };
    
    // {4FD70B37-E7AA-4f9f-8059-AFF49EE41D79}
    SYM_DEFINE_GUID(IID_SessionAppServerSink, 
        0x4fd70b37, 0xe7aa, 0x4f9f, 0x80, 0x59, 0xaf, 0xf4, 0x9e, 0xe4, 0x1d, 0x79);

    typedef CSymPtr<ISessionAppServerSink> ISessionAppServerSinkPtr;
    typedef CSymQIPtr<ISessionAppServerSinkPtr, &IID_SessionAppServerSink> ISessionAppServerSinkQIPtr;

    //****************************************************************************
    //  Session App Server interface
    //    This class will create a SysTray icon, and provide remote client
    //    management of app and notification state, as well as remote 
    //    Alert and Notification using common client IAlert and INotification 
    //    interfaces.
    //****************************************************************************
    class ISessionAppServer : public ISymBase
    {
    public:
        virtual HRESULT Initialize(LPCWSTR wszApplicationName) throw() = 0;
        virtual void Destroy() throw() = 0;
        
        virtual HRESULT RegisterEventSink(SessionApp::ISessionAppServerSink* pEventSink) throw() = 0;
        virtual HRESULT UnRegisterEventSink(SessionApp::ISessionAppServerSink* pEventSink) throw() = 0;

        virtual HRESULT GetApplicationName(cc::IString*& pstrApplicationName) throw() = 0;

        virtual HRESULT SetupTrayIcon(const DWORD dwAppState, const HMODULE hModule, UINT iIcon) throw() = 0;
        virtual HRESULT SetupToolTipText(const DWORD dwAppState, const HMODULE hModule, UINT iToolTip) throw() = 0;
        virtual HRESULT SetupToolTipText(const DWORD dwAppState, LPCWSTR wszToolTipText) throw() = 0;

        virtual HRESULT ShowTrayApp(DWORD dwState) throw() = 0;
        virtual HRESULT HideTrayApp() throw() = 0;

        virtual HRESULT SetContextMenu(const HMODULE hModule, const UINT iMenu) throw() = 0;
        virtual HRESULT SetDefaultMenuItem(const UINT iItem) throw() = 0;
    
        virtual HRESULT GetOption(const DWORD dwOptionId, DWORD_PTR &pOption) throw() = 0;
        virtual HRESULT SetOption(const DWORD dwOptionId, const DWORD_PTR pOption) throw() = 0;
    };
    
    // {698AA118-8DF8-4e7e-8185-C432B6A511F7}
    SYM_DEFINE_GUID(IID_SessionAppServer, 
        0x698aa118, 0x8df8, 0x4e7e, 0x81, 0x85, 0xc4, 0x32, 0xb6, 0xa5, 0x11, 0xf7);

    typedef CSymPtr<ISessionAppServer> ISessionAppServerPtr;
    typedef CSymQIPtr<ISessionAppServerPtr, &IID_SessionAppServer> ISessionAppServerQIPtr;

}