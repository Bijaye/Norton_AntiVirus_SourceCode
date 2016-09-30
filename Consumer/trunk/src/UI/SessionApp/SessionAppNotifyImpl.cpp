////////////////////////
//
// PROPRIETARY / CONFIDENTIAL.
// Use of this product is subject to license terms.
// Copyright © 2006 Symantec Corporation.
// All rights reserved.
//
/////////////////////////////////////////////////////////////////////////////////////////////////

#include "StdAfx.h"

#include "SessionAppClientInterface.h"
#include "SessionAppNotifyImpl.h"

#include "ccInstanceFactory.h"
#include "ccSymStringImpl.h"
#include "ccSymKeyValueCollectionImpl.h"

using namespace SessionApp;

//****************************************************************************
//****************************************************************************
//****************************************************************************
//****************************************************************************
CNotifyClientImpl::CNotifyClientImpl(void)
{
}


CNotifyClientImpl::~CNotifyClientImpl(void)
{
}

//****************************************************************************
//****************************************************************************
HRESULT CNotifyClientImpl::Initialize(LPCWSTR wszApplicationName, DWORD dwSessionId)
{
    StahlSoft::HRX hrx;
    ccLib::CExceptionInfo exceptionInfo;
    try
    {
        m_dwSessionId = dwSessionId;
        m_cszAppName = CW2T(wszApplicationName);
        
        CString cszTmpFormat = CW2T(wszApplicationName);
        cszTmpFormat += _T("_%02d");
        
        CString cszSessionChannel;
        cszSessionChannel.Format(cszTmpFormat, m_dwSessionId);

        SYMRESULT sr;

        //
        // Open up a session channel
        sr = ccService::ccServiceMgd_IComLib::CreateObject(GETMODULEMGR(), m_spCommLib);
        hrx << AvProdWidgets::HRESULT_FROM_SYMRESULT(sr);

        hrx << m_spCommLib->CreateClient(cszSessionChannel, &m_spCommClient);

		m_spCommand.Attach(ccSym::CKeyValueCollectionImpl::CreateKeyValueCollectionImpl());
		CCTHROW_BAD_ALLOC(m_spCommand);

    }
    CCCATCHMEM(exceptionInfo)
        CCCATCHCOM(exceptionInfo);

    if(exceptionInfo.IsException())
        return E_FAIL;

    return S_OK;
}

//****************************************************************************
//****************************************************************************
void CNotifyClientImpl::Destroy()
{
    if(m_spCommClient)
        m_spCommClient->Disconnect();

    m_spCommClient.Release();
    m_spCommLib.Release();
}

//****************************************************************************
//****************************************************************************
cc::INotify::RESULT CNotifyClientImpl::Display(HWND hParentWnd)
{
    HRESULT hr;
    ISymBaseQIPtr spOutData = m_spCommand;
    ISymBasePtr spInData;
    hr = m_spCommClient->SendCommand(SessionApp::CMDID_NotifyMessage, spOutData, &spInData);
    
    return S_OK == hr ? cc::INotify::RESULT::RESULT_OK : cc::INotify::RESULT::RESULT_FAILED;
}

//****************************************************************************
//****************************************************************************
cc::INotify::RESULT CNotifyClientImpl::Hide()
{
    _ASSERT(!_T("Function not implemented"));
    return cc::INotify::RESULT::RESULT_FAILED;
}

//****************************************************************************
//****************************************************************************
bool CNotifyClientImpl::SetIcon(HINSTANCE hInstance, UINT nBigIconID, UINT nSmallIconID)
{
    _ASSERT(!_T("Function not implemented"));
    return false;
}

//****************************************************************************
//****************************************************************************
bool CNotifyClientImpl::SetTitleBitmap(HINSTANCE hInstance, UINT nBitmapID)
{
    _ASSERT(!_T("Function not implemented"));
    return false;
}

//****************************************************************************
//****************************************************************************
bool CNotifyClientImpl::SetTitle(LPCSTR szTitle)
{
    cc::IStringPtr spTitle;
	spTitle.Attach(ccSym::CStringImpl::CreateStringImpl());
	if(spTitle == NULL)
		return false;
    if(!spTitle->SetStringA(szTitle))
		return false;
    return m_spCommand->SetValue((size_t)SessionApp::NotifyMessage::eTitle, spTitle);
}

//****************************************************************************
//****************************************************************************
bool CNotifyClientImpl::SetTitle(LPCWSTR szTitle)
{
    cc::IStringPtr spTitle;
	spTitle.Attach(ccSym::CStringImpl::CreateStringImpl());
	if(spTitle == NULL)
		return false;
    if(!spTitle->SetStringW(szTitle))
		return false;
    return m_spCommand->SetValue((size_t)SessionApp::NotifyMessage::eTitle, spTitle);
}

//****************************************************************************
//****************************************************************************
bool CNotifyClientImpl::SetTitle(HINSTANCE hInst, UINT nID)
{
    _ASSERT(!_T("Function not implemented"));
    return false;
}

//****************************************************************************
//****************************************************************************
bool CNotifyClientImpl::SetTaskBarText(LPCSTR szText)
{
    _ASSERT(!_T("Function not implemented"));
    return false;
}

//****************************************************************************
//****************************************************************************
bool CNotifyClientImpl::SetTaskBarText(LPCWSTR szText)
{
    _ASSERT(!_T("Function not implemented"));
    return false;
}

//****************************************************************************
//****************************************************************************
bool CNotifyClientImpl::SetTaskBarText(HINSTANCE hInst, UINT nID)
{
    _ASSERT(!_T("Function not implemented"));
    return false;
}

//****************************************************************************
//****************************************************************************
bool CNotifyClientImpl::SetProperty(PROPERTIES Property, bool bValue)
{
	ISymBasePtr spTemp;
	if(!m_spCommand->GetValue((size_t)SessionApp::NotifyMessage::eProperties, spTemp))
	{
		spTemp.Attach(ccLib::CInstanceFactory::CreateKeyValueCollectionImpl());
		if(spTemp == NULL)
			return false;

		m_spCommand->SetValue((size_t)SessionApp::NotifyMessage::eProperties, spTemp);
	}

	cc::IKeyValueCollectionQIPtr spProperties = spTemp;
	return spProperties->SetValue((size_t)(size_t)Property, bValue);
}

//****************************************************************************
//****************************************************************************
bool CNotifyClientImpl::SetTimeout(DWORD dwMillisec)
{
    return m_spCommand->SetValue((size_t)SessionApp::NotifyMessage::eDuration, dwMillisec);
}

//****************************************************************************
//****************************************************************************
bool CNotifyClientImpl::SetText(LPCSTR szText)
{
    cc::IStringPtr spText;
	spText.Attach(ccSym::CStringImpl::CreateStringImpl());
	if(spText == NULL)
		return false;
    if(!spText->SetStringA(szText))
		return false;
    return m_spCommand->SetValue((size_t)SessionApp::NotifyMessage::eText, spText);
}

//****************************************************************************
//****************************************************************************
bool CNotifyClientImpl::SetText(LPCWSTR szText)
{
    cc::IStringPtr spText;
	spText.Attach(ccSym::CStringImpl::CreateStringImpl());
	if(spText == NULL)
		return false;
    if(!spText->SetStringW(szText))
		return false;
    return m_spCommand->SetValue((size_t)SessionApp::NotifyMessage::eText, spText);
}

//****************************************************************************
//****************************************************************************
bool CNotifyClientImpl::SetText(HINSTANCE hInst, UINT nID)
{
    _ASSERT(!_T("Function not implemented"));
    return false;
}

//****************************************************************************
//****************************************************************************
bool CNotifyClientImpl::SetTooltip(LPCSTR szToolTip)
{
    cc::IStringPtr spToolTip;
	spToolTip.Attach(ccSym::CStringImpl::CreateStringImpl());
	if(spToolTip == NULL)
		return false;
    if(!spToolTip->SetStringA(szToolTip))
		return false;
    return m_spCommand->SetValue((size_t)SessionApp::NotifyMessage::eToolTip, spToolTip);
}

//****************************************************************************
//****************************************************************************
bool CNotifyClientImpl::SetTooltip(LPCWSTR szToolTip)
{
    cc::IStringPtr spToolTip;
	spToolTip.Attach(ccSym::CStringImpl::CreateStringImpl());
	if(spToolTip == NULL)
		return false;
    if(!spToolTip->SetStringW(szToolTip))
		return false;
    return m_spCommand->SetValue((size_t)SessionApp::NotifyMessage::eToolTip, spToolTip);
}

//****************************************************************************
//****************************************************************************
bool CNotifyClientImpl::SetTooltip(HINSTANCE hInst, UINT nID)
{
    _ASSERT(!_T("Function not implemented"));
   return false;
}

//****************************************************************************
//****************************************************************************
bool CNotifyClientImpl::SetPercent(UINT nPercentage)
{
    _ASSERT(!_T("Function not implemented"));
   return false;
}

//****************************************************************************
//****************************************************************************
bool CNotifyClientImpl::SetButton(BUTTONTYPE Type, LPCSTR szButtonText)
{
    _ASSERT(!_T("Function not implemented"));
   return false;
}

//****************************************************************************
//****************************************************************************
bool CNotifyClientImpl::SetButton(BUTTONTYPE Type, LPCWSTR szButtonText)
{
    cc::IStringPtr spButtonText;
    spButtonText.Attach(ccSym::CStringImpl::CreateStringImpl());
    
    if(spButtonText == NULL)
        return false;
    
    if(!spButtonText->SetStringW(szButtonText))
        return false;
    
    size_t nKey = NULL;
    switch(Type)
    {
    case BUTTONTYPE_OK:
    case BUTTONTYPE_CANCEL:
        _ASSERT(!_T("Function not implemented"));
        break;

    case BUTTONTYPE_MOREINFO:
        nKey = (size_t)SessionApp::NotifyMessage::eMoreInfo;
        break;
    }
    
    if(NULL == nKey)
        return false;

    return m_spCommand->SetValue(nKey, spButtonText);
}

//****************************************************************************
//****************************************************************************
bool CNotifyClientImpl::SetButton(BUTTONTYPE Type, HINSTANCE hInst, UINT nID)
{
    _ASSERT(!_T("Function not implemented"));
   return false;
}

//****************************************************************************
//****************************************************************************
bool CNotifyClientImpl::SetButton(BUTTONTYPE Type, LPCSTR szButtonText, 
                       cc::INotifyCallback* pCallback)
{
    _ASSERT(!_T("Function not implemented"));
   return false;
}

//****************************************************************************
//****************************************************************************
bool CNotifyClientImpl::SetButton(BUTTONTYPE Type, LPCWSTR szButtonText, 
                       cc::INotifyCallback* pCallback)
{
    _ASSERT(!_T("Function not implemented"));
   return false;
}

//****************************************************************************
//****************************************************************************
bool CNotifyClientImpl::SetButton(BUTTONTYPE Type, HINSTANCE hInst, UINT nID, 
                       cc::INotifyCallback* pCallback)
{
    _ASSERT(!_T("Function not implemented"));
   return false;
}

//****************************************************************************
//****************************************************************************
bool CNotifyClientImpl::Reset()
{
    _ASSERT(!_T("Function not implemented"));
   return false;
}

//****************************************************************************
//****************************************************************************
bool CNotifyClientImpl::AddCheckbox(LPCSTR szButtonText)
{
    _ASSERT(!_T("Function not implemented"));
   return false;
}

//****************************************************************************
//****************************************************************************
bool CNotifyClientImpl::AddCheckbox(LPCWSTR szButtonText)
{
    _ASSERT(!_T("Function not implemented"));
   return false;
}

//****************************************************************************
//****************************************************************************
bool CNotifyClientImpl::SetCheckboxState(long lState)
{
    _ASSERT(!_T("Function not implemented"));
   return false;
}

//****************************************************************************
//****************************************************************************
long CNotifyClientImpl::GetCheckboxState()
{
    _ASSERT(!_T("Function not implemented"));
   return -1;
}

//****************************************************************************
//****************************************************************************
bool CNotifyClientImpl::SetHyperlinkCallback(cc::INotifyCallback* pCallback)
{
    _ASSERT(!_T("Function not implemented"));
   return false;
}

//****************************************************************************
//****************************************************************************
bool CNotifyClientImpl::GetSize(long& cx, long& cy)
{
    m_spCommand->GetValue((size_t)SessionApp::NotifyMessage::eWidth, cx);
    m_spCommand->GetValue((size_t)SessionApp::NotifyMessage::eHeight, cy);

    return false;
}

//****************************************************************************
//****************************************************************************
bool CNotifyClientImpl::SetSize(long cx, long cy)
{
    m_spCommand->SetValue((size_t)SessionApp::NotifyMessage::eWidth, cx);
    m_spCommand->SetValue((size_t)SessionApp::NotifyMessage::eHeight, cy);

    return false;
}

//****************************************************************************
//****************************************************************************
DWORD CNotifyClientImpl::GetTimeout()
{
    _ASSERT(!_T("Function not implemented"));
   return DWORD(-1);
}

//****************************************************************************
//****************************************************************************
UINT CNotifyClientImpl::GetPercent()
{
    _ASSERT(!_T("Function not implemented"));
   return DWORD(-1);
}

//****************************************************************************
//****************************************************************************
bool CNotifyClientImpl::GetTitle(LPSTR szTitle, DWORD& dwSize)
{
    _ASSERT(!_T("Function not implemented"));
   return false;
}

//****************************************************************************
//****************************************************************************
bool CNotifyClientImpl::GetTitle(LPWSTR szTitle, DWORD& dwSize)
{
    _ASSERT(!_T("Function not implemented"));
   return false;
}

//****************************************************************************
//****************************************************************************
bool CNotifyClientImpl::GetTaskBarText(LPSTR szText, DWORD& dwSize)
{
    _ASSERT(!_T("Function not implemented"));
   return false;
}

//****************************************************************************
//****************************************************************************
bool CNotifyClientImpl::GetTaskBarText(LPWSTR szText, DWORD& dwSize)
{
    _ASSERT(!_T("Function not implemented"));
   return false;
}

//****************************************************************************
//****************************************************************************
bool CNotifyClientImpl::GetProperty(PROPERTIES Property)
{
    _ASSERT(!_T("Function not implemented"));
   return false;
}

//****************************************************************************
//****************************************************************************
bool CNotifyClientImpl::GetText(LPSTR szText, DWORD& dwSize)
{
    _ASSERT(!_T("Function not implemented"));
   return false;
}

//****************************************************************************
//****************************************************************************
bool CNotifyClientImpl::GetText(LPWSTR szText, DWORD& dwSize)
{
    _ASSERT(!_T("Function not implemented"));
   return false;
}

//****************************************************************************
//****************************************************************************
bool CNotifyClientImpl::GetTooltip(LPSTR szTooltip, DWORD& dwSize)
{
    _ASSERT(!_T("Function not implemented"));
   return false;
}

//****************************************************************************
//****************************************************************************
bool CNotifyClientImpl::GetTooltip(LPWSTR szTooltip, DWORD& dwSize)
{
    _ASSERT(!_T("Function not implemented"));
   return false;
}

//****************************************************************************
//****************************************************************************
bool CNotifyClientImpl::GetButton(BUTTONTYPE Type, LPSTR szButtonText, DWORD& dwSize, cc::INotifyCallback** ppCallback)
{
    _ASSERT(!_T("Function not implemented"));
   return false;
}

//****************************************************************************
//****************************************************************************
bool CNotifyClientImpl::GetButton(BUTTONTYPE Type, LPWSTR szButtonText, DWORD& dwSize, cc::INotifyCallback** ppCallback)	
{
    _ASSERT(!_T("Function not implemented"));
   return false;
}

//****************************************************************************
//****************************************************************************
bool CNotifyClientImpl::GetIcon(HINSTANCE& hInstance, UINT& nBigIconID, UINT& nSmallIconID)
{
    _ASSERT(!_T("Function not implemented"));
   return false;
}

//****************************************************************************
//****************************************************************************
bool CNotifyClientImpl::GetTitleBitmap(HINSTANCE& hInstance, UINT& nBitmapID)
{
    _ASSERT(!_T("Function not implemented"));
   return false;
}


//****************************************************************************
//****************************************************************************
cc::INotify::RESULT CNotifyClientImpl::Display(HWND hParentWnd, HANDLE hEvent)
{
   return Display(hParentWnd);
}


