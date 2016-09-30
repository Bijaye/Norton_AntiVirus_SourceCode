// HPPScriptObj.cpp : Implementation of CHPPScriptObj

#include "stdafx.h"
#include "HPPScriptObj.h"
#include "HPPRes.h"

#include "HPPUtility.h"

// HPPScriptObj

//****************************************************************************
//****************************************************************************
STDMETHODIMP CHPPScriptObj::SetHomePage(BSTR bszNewHomePage, VARIANT_BOOL vbCurrentUser)
{
    _bstr_t cbszNewHomePage = bszNewHomePage;
    BOOL bCurrentUser = (VARIANT_TRUE == vbCurrentUser);
    
    CCTRACEI(_T("CHPPScriptObj::SetHomePage - Request to changed home page to new value received.\n-- New Value:%S"), (LPCWSTR)cbszNewHomePage);
    
    CString cszMessage;
	CString cszTemp;
	CString cszMessageTitle;
    g_Resources.LoadString(IDS_CAT_RESETCONFIRM, cszTemp);
	cszMessage.FormatMessage(cszTemp, (LPCTSTR)cbszNewHomePage);
	g_Resources.LoadString(IDS_CAT_RESETCONFIRMTITLE, cszMessageTitle);
    INT iRet = ::MessageBox(NULL, cszMessage, _T(cszMessageTitle), MB_YESNO);
    if(IDYES == iRet)
    {
        HRESULT hr;
        hr = CHPPUtility::SetHomePage((BSTR)cbszNewHomePage, bCurrentUser);
        if(FAILED(hr))
        {
            CCTRACEE(_T("CHPPScriptObj::SetHomePage - CHPPUtility::SetHomePage(\"%S\", \"%s\") returned 0x%08X"), (LPCWSTR)cbszNewHomePage, bCurrentUser ? _T("true") : _T("false"), hr);
        }
    }

    // always return SUCCESS
    return S_OK;
}


//****************************************************************************
//****************************************************************************
STDMETHODIMP CHPPScriptObj::ResetOptions(void)
{
    HRESULT hr = E_FAIL;
    hr = CHPPUtility::ResetOptions();
    if(FAILED(hr))
    {
        CCTRACEE(_T("CHPPScriptObj::ResetOptions - CHPPUtility::ResetOptions() returned 0x%08X"), hr);
    }
    
    // always return SUCCESS
    return S_OK;
}

//****************************************************************************
//****************************************************************************
STDMETHODIMP CHPPScriptObj::PrepHPPForUninstall(void)
{
    HRESULT hr = E_FAIL;
    hr = CHPPUtility::PrepHPPForUninstall();
    if(FAILED(hr))
    {
        CCTRACEE(_T("CHPPScriptObj::PrepHPPForUninstall - CHPPUtility::PrepHPPForUninstall() returned 0x%08X"), hr);
    }
    
    // always return SUCCESS
    return S_OK;
}
