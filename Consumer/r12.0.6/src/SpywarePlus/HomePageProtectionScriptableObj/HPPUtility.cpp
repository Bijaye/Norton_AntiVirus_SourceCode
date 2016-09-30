#include "StdAfx.h"
#include "HPPUtility.h"

#include "HPPRes.h"

#include "WindowsSecurityAccount.h"
#include "HPPEventHelper.h"

#include "HPPEventEz.h"

CHPPUtility::CHPPUtility(void)
{
}

CHPPUtility::~CHPPUtility(void)
{
}

//****************************************************************************
//****************************************************************************
HRESULT CHPPUtility::SetHomePage(LPCSTR szNewHomePage, BOOL bCurrentUser)
{
    _bstr_t cbszNewHomePage = szNewHomePage;
    return SetHomePage((BSTR)cbszNewHomePage, bCurrentUser);
}

//****************************************************************************
//****************************************************************************
HRESULT CHPPUtility::SetHomePage(BSTR bszNewHomePage, BOOL bCurrentUser)
{
    CHPPEventHelperPtr spHPPEventHelper = new CHPPEventHelper;
    if(!spHPPEventHelper)
        return E_OUTOFMEMORY;

    ccEvtMgr::CEventEx *pEvent;

    SYMRESULT sr = spHPPEventHelper->NewHPPEvent(HPP::Event_ID_HPPChangeHomePage, &pEvent);
    if(SYM_FAILED(sr))
        return 0;

    CHPPEventCommonInterfaceQIPtr spCommonEvent = pEvent;
    if(!spCommonEvent)
    {
        spHPPEventHelper->Destroy();
        return 0;
    }

    HRESULT hr;

    // Add current user name
    hr = CHPPEventEz::AddCurrentUserName(spCommonEvent, HPP::Event_HPPChangeHomePage_propUserName);
    if(FAILED(hr))
    {
        CCTRACEE(_T("CHPPUtility::SetHomePage - failed to add current user name to event."));
    }

    // Add current user SID
    hr = CHPPEventEz::AddCurrentUserSID(spCommonEvent, HPP::Event_HPPChangeHomePage_propUserSID);
    if(FAILED(hr))
    {
        CCTRACEE(_T("CHPPUtility::SetHomePage - failed to add current user SID to event."));
    }

    // Get current session id
    hr = CHPPEventEz::AddCurrentTerminalSessionId(spCommonEvent, HPP::Event_HPPChangeHomePage_propSessionID);
    if(FAILED(hr))
    {
        CCTRACEE(_T("CHPPUtility::SetHomePage - failed to add current session ID to event."));
    }

    // Set current process id
    spCommonEvent->SetPropertyLONG(HPP::Event_HPPChangeHomePage_propProcessID, GetCurrentProcessId());

    CString cszValue;
    BOOL bRet;

    // Set current process path and name
    bRet = GetModuleFileName(NULL, cszValue.GetBuffer(MAX_PATH), MAX_PATH);
    cszValue.ReleaseBuffer();
    if(bRet)
    {
        _bstr_t cbszValue = cszValue;
        spCommonEvent->SetPropertyBSTR(HPP::Event_HPPChangeHomePage_propProcessPath, cbszValue);

        PathStripPath(cszValue.GetBuffer(MAX_PATH));
        cszValue.ReleaseBuffer();

        cbszValue = cszValue;
        spCommonEvent->SetPropertyBSTR(HPP::Event_HPPChangeHomePage_propProcessName, cbszValue);
    }

    spCommonEvent->SetPropertyBSTR(HPP::Event_HPPChangeHomePage_propNewHomePage, bszNewHomePage);
    spCommonEvent->SetPropertyLONG(HPP::Event_HPPChangeHomePage_propLocation, bCurrentUser ? HPP::HPP_LOCATIONS_HKCU : HPP::HPP_LOCATIONS_HKLM );
    
    spCommonEvent.Release();

    long lEventId = 0;
    if(spHPPEventHelper->CreateEvent(*pEvent, true, lEventId) != ccEvtMgr::CError::eNoError)
    {
        CCTRACEE(_T("CreateEvent() failed."));
        return E_FAIL;
    }

    spHPPEventHelper->DeleteHPPEvent(pEvent);
    spHPPEventHelper->Destroy();

    return S_OK;
}

//****************************************************************************
//****************************************************************************
HRESULT CHPPUtility::ResetOptions(void)
{
    CHPPEventHelperPtr spHPPEventHelper = new CHPPEventHelper;
    if(!spHPPEventHelper)
        return E_OUTOFMEMORY;

    ccEvtMgr::CEventEx *pEvent;

    SYMRESULT sr = spHPPEventHelper->NewHPPEvent(HPP::Event_ID_OptionsChanged, &pEvent);
    if(SYM_FAILED(sr))
    {
        CCTRACEE(_T("CHPPScriptObj::ResetOptions - CreateEvent(HPP::Event_ID_OptionsChanged) failed."));
        return E_FAIL;
    }

    CHPPEventCommonInterfaceQIPtr spCommonEvent = pEvent;
    if(!spCommonEvent)
    {
        spHPPEventHelper->Destroy();
        return 0;
    }
    
    HRESULT hr;

    // Add current user name
    hr = CHPPEventEz::AddCurrentUserName(spCommonEvent, HPP::Event_HPPOptionsChanged_propUserName);
    if(FAILED(hr))
    {
        CCTRACEE(_T("CHPPUtility::ResetOptions - failed to add current user name to event."));
    }

    // Add current user SID
    hr = CHPPEventEz::AddCurrentUserSID(spCommonEvent, HPP::Event_HPPOptionsChanged_propUserSID);
    if(FAILED(hr))
    {
        CCTRACEE(_T("CHPPUtility::ResetOptions - failed to add current user SID to event."));
    }

    // Get current session id
    hr = CHPPEventEz::AddCurrentTerminalSessionId(spCommonEvent, HPP::Event_HPPOptionsChanged_propSessionID);
    if(FAILED(hr))
    {
        CCTRACEE(_T("CHPPUtility::ResetOptions - failed to add current session ID to event."));
    }

    // Set current process id
    spCommonEvent->SetPropertyLONG(HPP::Event_HPPOptionsChanged_propProcessID, GetCurrentProcessId());

    CString cszValue;
    BOOL bRet;

    // Set current process path and name
    bRet = GetModuleFileName(NULL, cszValue.GetBuffer(MAX_PATH), MAX_PATH);
    cszValue.ReleaseBuffer();
    if(bRet)
    {
        _bstr_t cbszValue = cszValue;
        spCommonEvent->SetPropertyBSTR(HPP::Event_HPPOptionsChanged_propProcessPath, cbszValue);

        PathStripPath(cszValue.GetBuffer(MAX_PATH));
        cszValue.ReleaseBuffer();

        cbszValue = cszValue;
        spCommonEvent->SetPropertyBSTR(HPP::Event_HPPOptionsChanged_propProcessName, cbszValue);
    }

    spCommonEvent.Release();

    long lEventId = 0;
    if(spHPPEventHelper->CreateEvent(*pEvent, true, lEventId) != ccEvtMgr::CError::eNoError)
    {
        CCTRACEE(_T("CreateEvent() from OptionsChangedEvent failed."));
        return E_FAIL;
    }
    
    spHPPEventHelper->DeleteHPPEvent(pEvent);
    spHPPEventHelper->Destroy();

    return S_OK;
}

//****************************************************************************
//****************************************************************************
HRESULT CHPPUtility::PrepHPPForUninstall(void)
{
    CHPPEventHelperPtr spHPPEventHelper = new CHPPEventHelper;
    if(!spHPPEventHelper)
        return E_OUTOFMEMORY;

    if(!spHPPEventHelper->PrepHPPForUninstall())
    {
        CCTRACEE(_T("CHPPEventHelper::PrepHPPForUninstall() - returned false."));
    }
    
    spHPPEventHelper->Destroy();

    return S_OK;
}

