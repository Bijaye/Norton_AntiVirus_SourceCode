////////////////////////
//
// PROPRIETARY / CONFIDENTIAL.
// Use of this product is subject to license terms.
// Copyright © 2006 Symantec Corporation.
// All rights reserved.
//
/////////////////////////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"

#include "SessionAppWindow.h"
#include "SessionAppWindowThread.h"
#include "SessionAppServerImpl.h"
#include "SessionAppComm.h"

#include <ccServiceLoader.h>
#include <ccInstanceFactory.h>

#include "SessionAppWorkerJob.h"

using namespace SessionApp;

//****************************************************************************
//****************************************************************************
CServerImpl::CServerImpl(void) :
m_hModuleMenu(NULL),
m_iContextMenuId(NULL)
{
}

CServerImpl::~CServerImpl(void)
{
}

//****************************************************************************
//****************************************************************************
HRESULT CServerImpl::Initialize(LPCWSTR wszApplicationName)
{
    StahlSoft::HRX hrx;
    ccLib::CExceptionInfo exceptionInfo;
    try
    {
        CString cszTmpFormat = wszApplicationName;
        cszTmpFormat += _T("_%02d");
        m_cszSessionChannel.Format(cszTmpFormat, GetCurrentSessionId());

        m_shTrayAppReady = ::CreateEvent(NULL, true, false, NULL);

        m_cAppWindowThread.m_pSessionAppServer = this;
        m_cAppWindowThread.m_wndAppWindow.m_pSessionAppServer = this;
        hrx << m_cAppWindowThread.CreateThread();
        hrx << m_cWorkerThread.CreateThread();
        
        if(m_shTrayAppReady)
        {
            DWORD dwRet = ::WaitForSingleObject(m_shTrayAppReady, 5000);
            if(WAIT_OBJECT_0 != dwRet)
                hrx << E_UNEXPECTED;

            m_shTrayAppReady.CloseHandle();
        }

        SYMRESULT sr;
        CAppComm* pAppComm;
        hrx << CAppComm::CreateInstance(pAppComm);
        hrx << CAppComm::CreateObjectFromInstance(pAppComm, m_spCommandHandler);
        pAppComm->m_pSessionAppServer = this;

        //
        // Open up a session channel
        CCTRACEI( CCTRCTX _T("Loading COMM library."));
        sr = ccService::ccServiceMgd_IComLib::CreateObject(GETMODULEMGR(), m_spCommLib);
        hrx << AvProdWidgets::HRESULT_FROM_SYMRESULT(sr);

        CCTRACEI( CCTRCTX _T("Init'ing COMM channel: %s."), m_cszSessionChannel);
        hrx << m_spCommLib->CreateBiDirectionalServer(m_cszSessionChannel, &m_spCommServer);
        hrx << m_spCommServer->RegisterCommand(SessionApp::CMDID_NotifyMessage, m_spCommandHandler);
        hrx << m_spCommServer->RegisterCommand(SessionApp::CMDID_AlertMessage, m_spCommandHandler);
        hrx << m_spCommServer->RegisterCommand(SessionApp::CMDID_SetIconState, m_spCommandHandler);
        hrx << m_spCommServer->RegisterCommand(SessionApp::CMDID_GetIconState, m_spCommandHandler);
        hrx << m_spCommServer->RegisterCommand(SessionApp::CMDID_CustomCommand, m_spCommandHandler);

        m_spCommServer->Listen();


    }
    CCCATCHMEM(exceptionInfo)
    CCCATCHCOM(exceptionInfo);

    if(exceptionInfo.IsException())
        return E_FAIL;

    return S_OK;
}

//****************************************************************************
//****************************************************************************
void CServerImpl::Destroy()
{
    if(m_spCommServer)
        m_spCommServer->Disconnect();

    m_cAppWindowThread.RequestExit();
    m_cWorkerThread.RequestExit();
    if(WAIT_OBJECT_0 != WaitForSingleObject(m_cAppWindowThread.GetThreadHandle(), 5000))
    {
        CCTRCTXW0(_T("Window thread did not close in time.  Terminating!!"));
        m_cAppWindowThread.TerminateThread();
    }

    if(WAIT_OBJECT_0 != WaitForSingleObject(m_cWorkerThread.GetThreadHandle(), 5000))
    {
        CCTRCTXW0(_T("Worker thread did not close in time.  Terminating!!"));
        m_cWorkerThread.TerminateThread();
    }

    m_spCommServer.Release();
    m_spCommLib.Release();
}

//****************************************************************************
//****************************************************************************
HRESULT CServerImpl::RegisterEventSink(SessionApp::ISessionAppServerSink* pEventSink)
{
    if(!pEventSink)
        return E_INVALIDARG;
    
    if(m_spSessionAppServerSink)
        return E_FAIL;

    m_spSessionAppServerSink = pEventSink;
    return S_OK;
}

//****************************************************************************
//****************************************************************************
HRESULT CServerImpl::UnRegisterEventSink(SessionApp::ISessionAppServerSink* pEventSink)
{
    if(!pEventSink)
        return E_INVALIDARG;
    
    if(m_spSessionAppServerSink != pEventSink)
        return E_INVALIDARG;

    m_spSessionAppServerSink.Release();
    return S_OK;
}

//****************************************************************************
//****************************************************************************
DWORD CServerImpl::GetCurrentSessionId()
{
    typedef BOOL (WINAPI *LPFNProcessIdToSessionId)(DWORD dwProcessId, LPDWORD pdwSessionId);

    // Get a handle to Kernel32
    HMODULE hmKernel32 = GetModuleHandle(_T("Kernel32.dll"));
    if(!hmKernel32)
        return 0; // Session 0

    // Get a function ptr for ProcessIdToSessionId
    LPFNProcessIdToSessionId FNProcessIdToSessionId = NULL;
    FNProcessIdToSessionId = (LPFNProcessIdToSessionId)GetProcAddress(hmKernel32, "ProcessIdToSessionId");
    if(!FNProcessIdToSessionId)
        return 0; // Session 0

    // Use the current process id to get a current session id
    DWORD dwSessionId = NULL;
    DWORD dwProcessId = GetCurrentProcessId();
    BOOL bRet = FNProcessIdToSessionId(dwProcessId, &dwSessionId);
    if(!bRet)
        return 0; // Session 0

    return dwSessionId;
}

//****************************************************************************
//****************************************************************************
HRESULT CServerImpl::GetApplicationName(cc::IString*& pstrApplicationName)
{
    return S_OK;
}

//****************************************************************************
//****************************************************************************
HRESULT CServerImpl::SetupTrayIcon(const DWORD dwAppState, const HMODULE hModule, UINT iIcon)
{
    return m_cAppWindowThread.m_wndAppWindow.SetIcon(dwAppState, hModule, iIcon);
}

//****************************************************************************
//****************************************************************************
HRESULT CServerImpl::SetupToolTipText(const DWORD dwAppState, const HMODULE hModule, UINT iToolTipTextId)
{
    return m_cAppWindowThread.m_wndAppWindow.SetTooltipText(dwAppState, hModule, iToolTipTextId);
}

//****************************************************************************
//****************************************************************************
HRESULT CServerImpl::SetupToolTipText(const DWORD dwAppState, LPCWSTR wszToolTipText)
{
    return m_cAppWindowThread.m_wndAppWindow.SetTooltipText(dwAppState, wszToolTipText);
}

//****************************************************************************
//****************************************************************************
HRESULT CServerImpl::ShowTrayApp(const DWORD dwAppState)
{
    return m_cAppWindowThread.m_wndAppWindow.ShowTrayApp(dwAppState);
}

//****************************************************************************
//****************************************************************************
HRESULT CServerImpl::HideTrayApp()
{
    return m_cAppWindowThread.m_wndAppWindow.HideTrayApp();
}

//****************************************************************************
//****************************************************************************
HRESULT CServerImpl::SetContextMenu(const HMODULE hModule, const UINT iMenuId)
{
    m_hModuleMenu = hModule;
    m_iContextMenuId = iMenuId;
    
    return S_OK;
}

//****************************************************************************
//****************************************************************************
HRESULT CServerImpl::SetDefaultMenuItem(const UINT iItem)
{
    return S_OK;
}


//****************************************************************************
//****************************************************************************
HRESULT CServerImpl::GetOption(const DWORD dwOptionId, DWORD_PTR &pOption)
{
    return S_OK;
}

//****************************************************************************
//****************************************************************************
HRESULT CServerImpl::SetOption(const DWORD dwOptionId, const DWORD_PTR pOption)
{
    return S_OK;
}

//****************************************************************************
//****************************************************************************
void CServerImpl::OnPrepareMenu(HMENU &hMenu)
{
    Fire_OnPrepareMenu(hMenu);
}

//****************************************************************************
//****************************************************************************
void CServerImpl::OnMenuCommand(UINT iMenuCommandId)
{
    Fire_OnMenuCommand(iMenuCommandId);
}

//****************************************************************************
//****************************************************************************
void CServerImpl::OnTrayAppReady()
{
    if(m_shTrayAppReady)
        ::SetEvent(m_shTrayAppReady);

    Fire_OnAppServerReady();
}

//****************************************************************************
//****************************************************************************
void CServerImpl::OnTrayItemDblClick()
{
    Fire_OnTrayItemDblClick();
}

//****************************************************************************
//****************************************************************************
void CServerImpl::Fire_OnAppServerReady()
{
    if(m_spSessionAppServerSink)
        m_spSessionAppServerSink->OnAppServerReady(this);
}

//****************************************************************************
//****************************************************************************
void CServerImpl::Fire_OnPrepareMenu(HMENU &hMenu)
{
    if(m_spSessionAppServerSink)
        m_spSessionAppServerSink->OnPrepareMenu(this, hMenu);
}

//****************************************************************************
//****************************************************************************
void CServerImpl::Fire_OnMenuCommand(UINT nId)
{
    if(m_spSessionAppServerSink)
        m_spSessionAppServerSink->OnMenuCommand(this, nId);
}

//****************************************************************************
//****************************************************************************
void CServerImpl::Fire_OnTrayItemDblClick()
{
    if(m_spSessionAppServerSink)
        m_spSessionAppServerSink->OnTrayItemDblClick(this);
}


//****************************************************************************
//****************************************************************************
void CServerImpl::OnNotifyMessage(const SYMGUID& gCommandGuid, LPCWSTR szCallbackChannel,
                                               ISymBase* pBase, ISymBase** ppOutBase)
{
    CCTRCTXI0(_T("Command Received."));

    CProcessNotifyMessage* pNotifyMessage = new CProcessNotifyMessage;

    if(pNotifyMessage)
    {
        CSmartRefCountRunnableJob  smRunnable( new CMTAWorkerThread::CContainedRunnableRefCount(pNotifyMessage) );
        pNotifyMessage->m_spCommand = pBase;
        pNotifyMessage->m_hWndParent = m_cAppWindowThread.m_wndAppWindow.m_hWnd;
        
        m_cWorkerThread.GetQueuePointer()->PushItem(smRunnable);
    }

    return;
}

//****************************************************************************
//****************************************************************************
void CServerImpl::OnAlertMessage(const SYMGUID& gCommandGuid, LPCWSTR szCallbackChannel,
                                        ISymBase* pBase, ISymBase** ppOutBase)
{
    CCTRCTXI0(_T("Command Received."));

}

//****************************************************************************
//****************************************************************************
void CServerImpl::OnSetIconState(const SYMGUID& gCommandGuid, LPCWSTR szCallbackChannel,
                                   ISymBase* pBase, ISymBase** ppOutBase)
{
    CCTRCTXI0(_T("Command Received."));
    
    cc::IKeyValueCollectionQIPtr spCommand = pBase;
    if(!spCommand)
        return;

    DWORD dwAppState = NULL;
    if(!spCommand->GetValue((size_t)SessionApp::IconState::eAppState, dwAppState))
        return;

    DWORD dwNotifyState = NULL;
    if(!spCommand->GetValue((size_t)SessionApp::IconState::eNotifyState, dwNotifyState))
        return;

    DWORD dwDuration = NULL;
    spCommand->GetValue((size_t)SessionApp::IconState::eDuration, dwDuration);
    
    ISymBasePtr spTempValue;
    cc::IStringQIPtr spToolTipText;
    if(spCommand->GetValue((size_t)SessionApp::IconState::eToolTip, spTempValue))
        spToolTipText = spTempValue;

    if(SessionApp::AppState::eInvalid != dwAppState)
    {
        m_cAppWindowThread.m_wndAppWindow.ChangeState(dwAppState, NULL, NULL);
    }
    else
    {
        if(!spToolTipText)
            m_cAppWindowThread.m_wndAppWindow.ChangeState(dwNotifyState, dwDuration, NULL);
        else
            m_cAppWindowThread.m_wndAppWindow.ChangeState(dwNotifyState, dwDuration, spToolTipText->GetStringW());
    }
    
    return;
}

//****************************************************************************
//****************************************************************************
void CServerImpl::OnGetIconState(const SYMGUID& gCommandGuid, LPCWSTR szCallbackChannel,
                                           ISymBase* pBase, ISymBase** ppOutBase)
{
    CCTRCTXI0(_T("Command Received."));

    cc::IKeyValueCollectionQIPtr spCommand = pBase;
    if(!spCommand)
        return;
    
    CString cszToolTipText;
    DWORD dwAppState = NULL, dwNotifyState = NULL, dwDuration = NULL;
    m_cAppWindowThread.m_wndAppWindow.GetCurrentIconState(dwAppState, dwNotifyState, dwDuration, cszToolTipText);

    spCommand->SetValue((size_t)SessionApp::IconState::eAppState, dwAppState);
    spCommand->SetValue((size_t)SessionApp::IconState::eNotifyState, dwNotifyState);
    spCommand->SetValue((size_t)SessionApp::IconState::eDuration, dwDuration);
    
    cc::IStringQIPtr spToolTipText = ccLib::CInstanceFactory::CreateStringImpl();
    spToolTipText->SetString(cszToolTipText);
    spCommand->SetValue((size_t)SessionApp::IconState::eToolTip, spToolTipText);

    spCommand->QueryInterface(cc::IID_KeyValueCollection, (void**)ppOutBase);

    return;
}

//****************************************************************************
//****************************************************************************
void CServerImpl::OnCommand(const SYMGUID& gCommandGuid, LPCWSTR szCallbackChannel,
                                           ISymBase* pBase, ISymBase** ppOutBase)
{
    CCTRCTXI0(_T("Custom Command Received."));
    cc::IKeyValueCollectionQIPtr spIncoming = pBase;
    if(!spIncoming)
    {
        CCTRACEE( CCTRCTX _T("Unexpected data type from OnCommand()"));
        return;
    }

    SYMGUID guidActualCommandId;
    GUID *pActualCommandId = (GUID*)&guidActualCommandId;
    spIncoming->GetValue((size_t)SessionApp::CustomCommand::eCommandId, *pActualCommandId);

    size_t nDataSize = spIncoming->GetSize((size_t)SessionApp::CustomCommand::eCommandData);
    if(nDataSize)
    {
        StahlSoft::CSmartDataPtr<BYTE> spData = new BYTE[nDataSize];
        spIncoming->GetValue((size_t)SessionApp::CustomCommand::eCommandData, (LPVOID)(LPBYTE)spData, nDataSize);
        
        GUID guidObjectId;
        spIncoming->GetValue((size_t)SessionApp::CustomCommand::eCommandDataGuid, guidObjectId);

        CBIN2SymBase BIN2SymBase;
        BIN2SymBase.SetBIN((SYMGUID*)&guidObjectId, (LPVOID)(LPBYTE)spData, nDataSize);

        ISymBasePtr spActualData = BIN2SymBase;
        if(spActualData)
        {
            CProcessCustomCommand* pCustomCommand = new CProcessCustomCommand;
            if(pCustomCommand)
            {
                pCustomCommand->m_spCommand = spActualData;
                pCustomCommand->m_spSessionAppServerSink = m_spSessionAppServerSink;
                pCustomCommand->m_spCommServer = m_spCommServer;
                pCustomCommand->m_spAppServer = this;
                pCustomCommand->m_cwszCallbackChannel = szCallbackChannel;
                pCustomCommand->m_dwStartTime = GetTickCount();

                memcpy(&(pCustomCommand->m_guidCommand), &gCommandGuid, sizeof(SYMGUID));
                memcpy(&(pCustomCommand->m_guidActualCommand), &guidActualCommandId, sizeof(SYMGUID));

                CSmartRefCountRunnableJob  smRunnable( new CMTAWorkerThread::CContainedRunnableRefCount(pCustomCommand) );
                m_cWorkerThread.GetQueuePointer()->PushItem(smRunnable);
            }
        }
    }

    return;
}
