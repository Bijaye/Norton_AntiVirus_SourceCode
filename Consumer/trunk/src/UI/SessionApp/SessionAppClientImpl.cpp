////////////////////////
//
// PROPRIETARY / CONFIDENTIAL.
// Use of this product is subject to license terms.
// Copyright © 2006 Symantec Corporation.
// All rights reserved.
//
/////////////////////////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "SessionAppClientImpl.h"

#include "SessionAppNotifyImpl.h"

#include "ccSymStringImpl.h"
#include "ccSymKeyValueCollectionImpl.h"

#include <ccServiceLoader.h>
#include <ccInstanceFactory.h>

#include <WtsApi32.h>

using namespace SessionApp;

CClientImpl::CClientImpl(void)
{
}

CClientImpl::~CClientImpl(void)
{
    Destroy();
}

HRESULT CClientImpl::Initialize(LPCWSTR wszApplicationName, DWORD dwSessionId)
{
    StahlSoft::HRX hrx;
    ccLib::CExceptionInfo exceptionInfo;
    try
    {
        m_dwSessionId = (-1 != dwSessionId) ? dwSessionId : GetActiveSessionId();
        m_dwSessionId = (-1 != m_dwSessionId) ? m_dwSessionId : GetCurrentSessionId();
        
        m_cszAppName = CW2T(wszApplicationName);

        CString cszTmpFormat = CW2T(wszApplicationName);
        cszTmpFormat += _T("_%02d");

        CString cszSessionChannel;
        cszSessionChannel.Format(cszTmpFormat, m_dwSessionId);
        
        SYMRESULT sr;

        //
        // Create an IComLib object
        CCTRACEI( CCTRCTX _T("Loading COMM library."));
        sr = ccService::ccServiceMgd_IComLib::CreateObject(GETMODULEMGR(), m_spCommLib);
        hrx << AvProdWidgets::HRESULT_FROM_SYMRESULT(sr);

        //
        // Create our command handler
        CCTRACEI( CCTRCTX _T("Creating command handler."));
        CSessionAppClientComm* pCommandHandlerRaw;
        hrx << CSessionAppClientComm::CreateInstance(pCommandHandlerRaw);
        hrx << CSessionAppClientComm::CreateObjectFromInstance(pCommandHandlerRaw, m_spCommandHandler);
        pCommandHandlerRaw->m_pSessionAppClient = this;

        

        CCTRACEI( CCTRCTX _T("Init'ing COMM channel: %s."), cszSessionChannel);
        hrx << m_spCommLib->CreateBiDirectionalClient(cszSessionChannel, &m_spCommClient);
        hrx << m_spCommClient->RegisterCommand(SessionApp::CMDID_CustomCommand, m_spCommandHandler);

        m_seReturnCommandReady.Create(NULL, true, false, NULL, false);
    }
    CCCATCHMEM(exceptionInfo)
    CCCATCHCOM(exceptionInfo);

    if(exceptionInfo.IsException())
    {
        CCTRACEE( CCTRCTX _T("Handled exception. %s"), exceptionInfo.GetFullDescription());
        return E_FAIL;
    }

    return S_OK;
}

void CClientImpl::Destroy()
{
    if(m_spCommClient)
        m_spCommClient->Disconnect();

    m_spCommClient.Release();
    m_spCommLib.Release();
    return;
}

DWORD CClientImpl::GetActiveSessionId()
{

    // Get a handle to Kernel32
    StahlSoft::CSmartModuleHandle smKernel32 = LoadLibrary(_T("Wtsapi32.dll"));
    if(!smKernel32)
        return 0; // Session 0

    // Get a function ptr for WTSEnumerateSessions
    typedef BOOL (WINAPI *LPFNWTSEnumerateSessionsW)(HANDLE hServer, DWORD dwReserved, DWORD dwVersion, PWTS_SESSION_INFOW *ppSessionInfo, LPDWORD pdwCount);
    LPFNWTSEnumerateSessionsW FNWTSEnumerateSessionsW = NULL;
    FNWTSEnumerateSessionsW = (LPFNWTSEnumerateSessionsW)GetProcAddress(smKernel32, "WTSEnumerateSessionsW");
    if(!FNWTSEnumerateSessionsW)
        return 0; // Session 0

    // Get a function ptr for WTSEnumerateSessions
    typedef BOOL (WINAPI *LPFNWTSFreeMemory)(PVOID pMemory);
    LPFNWTSFreeMemory FNWTSFreeMemory = NULL;
    FNWTSFreeMemory = (LPFNWTSFreeMemory)GetProcAddress(smKernel32, "WTSFreeMemory");
    if(!FNWTSFreeMemory)
        return 0; // Session 0

    DWORD dwActiveSessionId = (DWORD)-1;

    PWTS_SESSION_INFOW pSessionInfo = NULL;
    DWORD dwSessionCount = NULL;

    BOOL bRet = FNWTSEnumerateSessionsW(WTS_CURRENT_SERVER_HANDLE, 0 /* see docs */, 1 /* see docs */, &pSessionInfo, &dwSessionCount);
    if(!bRet)
        return (DWORD)-1;

    for(DWORD dwSessionIndex = 0; dwSessionIndex < dwSessionCount; dwSessionIndex++)
    {
        if(WTSActive == pSessionInfo[dwSessionIndex].State)
        {
            dwActiveSessionId = pSessionInfo[dwSessionIndex].SessionId;
            break;
        }
    }

    FNWTSFreeMemory((PVOID)pSessionInfo);

    return dwActiveSessionId;
}

DWORD CClientImpl::GetCurrentSessionId()
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

HRESULT CClientImpl::GetCurrentIconState(LPDWORD pdwAppState, LPDWORD pdwNotifyState, LPDWORD pdwDuration, cc::IString** ppstrToolTipText)
{
    StahlSoft::HRX hrx;
    ccLib::CExceptionInfo exceptionInfo;
    try
    {
        cc::IKeyValueCollectionPtr spCommand;
		spCommand.Attach(ccSym::CKeyValueCollectionImpl::CreateKeyValueCollectionImpl());
		CCTHROW_BAD_ALLOC(spCommand);

        spCommand->SetValue((size_t)SessionApp::GeneralDetail::eSessionId, GetCurrentSessionId());

        ISymBaseQIPtr spOutValue = spCommand;
        ISymBasePtr spInValue;
        hrx << m_spCommClient->SendCommand(SessionApp::CMDID_GetIconState, spOutValue, &spInValue);

        cc::IKeyValueCollectionConstQIPtr spResponse = spInValue;
        if(!spResponse)
            throw _com_error(E_UNEXPECTED);

        if(pdwAppState)
            if(!spResponse->GetValue((size_t)SessionApp::IconState::eAppState, *pdwAppState))
                throw _com_error(E_UNEXPECTED);

        if(pdwNotifyState)
            if(!spResponse->GetValue((size_t)SessionApp::IconState::eNotifyState, *pdwNotifyState))
                throw _com_error(E_UNEXPECTED);

        if(pdwDuration)
            if(!spResponse->GetValue((size_t)SessionApp::IconState::eDuration, *pdwDuration))
                throw _com_error(E_UNEXPECTED);

        if(ppstrToolTipText)
        {
            ISymBasePtr spToolTipText;
            if(!spResponse->GetValue((size_t)SessionApp::IconState::eToolTip, spToolTipText))
                throw _com_error(E_UNEXPECTED);

            SYMRESULT sr;
            sr = spToolTipText->QueryInterface(cc::IID_String, (void**)ppstrToolTipText);
            hrx << AvProdWidgets::HRESULT_FROM_SYMRESULT(sr);
        }
    }
    CCCATCHMEM(exceptionInfo)
        CCCATCHCOM(exceptionInfo);

    if(exceptionInfo.IsException())
        return E_FAIL;

    return S_OK;
}

HRESULT CClientImpl::SetCurrentIconState(DWORD dwAppState, DWORD dwNotifyState, DWORD dwDuration, LPCWSTR wszToolTipText)
{
    StahlSoft::HRX hrx;
    ccLib::CExceptionInfo exceptionInfo;
    try
    {
        cc::IKeyValueCollectionPtr spCommand;
		spCommand.Attach(ccSym::CKeyValueCollectionImpl::CreateKeyValueCollectionImpl());
		CCTHROW_BAD_ALLOC(spCommand);

        spCommand->SetValue((size_t)SessionApp::IconState::eAppState, dwAppState);
        spCommand->SetValue((size_t)SessionApp::IconState::eNotifyState, dwNotifyState);
        spCommand->SetValue((size_t)SessionApp::IconState::eDuration, dwDuration);
        if(NULL != wszToolTipText)
        {
            cc::IStringPtr spToolTipText;
			spToolTipText.Attach(ccSym::CStringImpl::CreateStringImpl());
			CCTHROW_BAD_ALLOC(spToolTipText);

            if(!spToolTipText->SetString(wszToolTipText))
				hrx << E_FAIL;

            ISymBaseQIPtr spTemp = spToolTipText;
            spCommand->SetValue((size_t)SessionApp::IconState::eToolTip, spTemp);
        }

        ISymBaseQIPtr spOutValue = spCommand;
        ISymBasePtr spInValue;
        hrx << m_spCommClient->SendCommand(SessionApp::CMDID_SetIconState, spOutValue, &spInValue);
    }
    CCCATCHMEM(exceptionInfo)
    CCCATCHCOM(exceptionInfo);

    if(exceptionInfo.IsException())
        return E_FAIL;

    return S_OK;
}

HRESULT CClientImpl::GetCurrentAppState(DWORD &dwAppState)
{
    return GetCurrentIconState(&dwAppState);
}

HRESULT CClientImpl::SetCurrentAppState(const DWORD dwAppState)
{
    return SetCurrentIconState(dwAppState);
}

HRESULT CClientImpl::GetCurrentNotifyState(DWORD &dwNotifyState, LPDWORD pdwRemaining, cc::IString** ppstrToolTip)
{
    return GetCurrentIconState(NULL, &dwNotifyState, pdwRemaining, ppstrToolTip);
}

HRESULT CClientImpl::SetCurrentNotifyState(const DWORD dwNotifyState, const DWORD dwDuration, LPCWSTR wszToolTip)
{
    return SetCurrentIconState(SessionApp::AppState::eInvalid, dwNotifyState, dwDuration, wszToolTip);
}

HRESULT CClientImpl::CreateNotification(cc::INotify*& pNotification)
{
    StahlSoft::HRX hrx;
    ccLib::CExceptionInfo exceptionInfo;
    try
    {
        cc::INotifyPtr spNotify;
        SessionApp::CNotifyClientImpl *pNotify;
        hrx << SessionApp::CNotifyClientImpl::CreateInstance(pNotify);
        hrx << SessionApp::CNotifyClientImpl::CreateObjectFromInstance(pNotify, spNotify);
        hrx << pNotify->Initialize(CT2W(m_cszAppName), m_dwSessionId);
        
        SYMRESULT sr;
        sr = spNotify->QueryInterface(cc::IID_Notify, (void**)&pNotification);
        hrx << AvProdWidgets::HRESULT_FROM_SYMRESULT(sr);
    }
    CCCATCHMEM(exceptionInfo)
    CCCATCHCOM(exceptionInfo);

    if(exceptionInfo.IsException())
        return E_FAIL;

    return S_OK;
}

HRESULT CClientImpl::CreateAlert(cc::IAlert*& pAlert)
{
    return E_NOTIMPL;
}

HRESULT CClientImpl::LaunchApplication(LPCWSTR wszCommandLine)
{
    return E_NOTIMPL;
}

HRESULT CClientImpl::GetUserName(cc::IString*& pstrUserName)
{
    return E_NOTIMPL;
}

HRESULT CClientImpl::IsAdministrator(bool &bIsAdministrator)
{
    return E_NOTIMPL;
}

HRESULT CClientImpl::IsGuest(bool &bIsGuest)
{
    return E_NOTIMPL;
}

HRESULT CClientImpl::GetOption(const DWORD dwOptionId, DWORD_PTR pOption)
{
    return E_NOTIMPL;
}

HRESULT CClientImpl::SetOption(const DWORD dwOptionId, DWORD_PTR pOption)
{
    return E_NOTIMPL;
}

HRESULT CClientImpl::SendCommand(const SYMGUID& guidCommandId, ISymBase *pCommand, ISymBase **ppReturn)
{
    //--~~--//--~~--//--~~--//--~~--//--~~--//--~~--//--~~--//--~~--//--~~--//--~~--//--~~--//--~~--
    ccLib::CSingleLock lock ( &m_CustomCommandLock, INFINITE, FALSE );
    //--~~--//--~~--//--~~--//--~~--//--~~--//--~~--//--~~--//--~~--//--~~--//--~~--//--~~--//--~~--

    StahlSoft::HRX hrx;
    ccLib::CExceptionInfo exceptionInfo;
    try
    {
        HRESULT hr;
        cc::IKeyValueCollectionPtr spAltCommand;
		spAltCommand.Attach(ccSym::CKeyValueCollectionImpl::CreateKeyValueCollectionImpl());
		CCTHROW_BAD_ALLOC(spAltCommand);
        
        GUID *pAltCommandId = (GUID*)&guidCommandId;
        if(!spAltCommand->SetValue((size_t)SessionApp::CustomCommand::eCommandId, *pAltCommandId))
        {
            CCTRACEE( CCTRCTX _T("IKeyValueCollection::SetValue(CustomCommand::eCommandId) failed."));
            hrx << E_UNEXPECTED;
       }

        CSymBase2BIN SymBase2BIN;
        hr = SymBase2BIN.SetSymObject(pCommand);
        if(FAILED(hr))
        {
            CCTRACEE( CCTRCTX _T("CSymBase2BIN::SetSymObject() failed. hr=0x%08X"), hr);
            hrx << hr;
        }

        if(!spAltCommand->SetValue((size_t)SessionApp::CustomCommand::eCommandData, (LPVOID)SymBase2BIN, (size_t)SymBase2BIN))
        {
            CCTRACEE( CCTRCTX _T("IKeyValueCollection::SetValue(CustomCommand::eCommandData) failed."));
            hrx << E_UNEXPECTED;
        }

        GUID *pCommandDataGuid = (GUID*)SymBase2BIN.GetObjectId();
        if(!spAltCommand->SetValue((size_t)SessionApp::CustomCommand::eCommandDataGuid, *pCommandDataGuid))
        {
            CCTRACEE( CCTRCTX _T("IKeyValueCollection::SetValue(CustomCommand::eCommandId) failed."));
            hrx << E_UNEXPECTED;
        }

        // Ensure everything is set up to fake
        //  the synchronous call
        m_spReturnCommand.Release();
        m_seReturnCommandReady.ResetEvent();

        CCTRACEI( CCTRCTX _T("Sending Bi-Directional custom command"));
        ISymBaseQIPtr spBase = spAltCommand;
        hrx << m_spCommClient->SendCommand(SessionApp::CMDID_CustomCommand, spAltCommand, ppReturn);
        
        const DWORD dwWaitTimeout = 1000 * 60 * 5; // 5 minutes
        ccLib::CSingleLock lock ( &m_seReturnCommandReady, dwWaitTimeout, FALSE );
        if(m_spReturnCommand)
            m_spReturnCommand->QueryInterface(IID_SymBase, (void**)ppReturn);
        
    }
    CCCATCHMEM(exceptionInfo)
    CCCATCHCOM(exceptionInfo);

    if(exceptionInfo.IsException())
        return E_FAIL;

    return S_OK;
}

//****************************************************************************
//****************************************************************************
void CClientImpl::OnCommand(const SYMGUID& gCommandGuid, LPCWSTR szCallbackChannel,
                            ISymBase* pBase, ISymBase** ppOutBase)
{
    CCTRCTXI0(_T("Bi-Directional command received."));
    
    // Set the return data
    m_spReturnCommand = pBase;

    // Release the thread waiting for return command
    m_seReturnCommandReady.SetEvent();

    return;
}
