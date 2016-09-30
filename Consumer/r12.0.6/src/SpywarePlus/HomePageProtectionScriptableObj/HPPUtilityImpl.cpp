#include "StdAfx.h"
#include "HPPUtilityInterface.h"
#include "HPPUtilityImpl.h"

#include "HPPRes.h"

#include "HPPUtility.h"

CHPPUtilityImpl::CHPPUtilityImpl(void)
{
}

CHPPUtilityImpl::~CHPPUtilityImpl(void)
{
}


//****************************************************************************
//****************************************************************************
HRESULT CHPPUtilityImpl::SetHomePage(LPCSTR szNewHomePage, BOOL bCurrentUser)
{
    HRESULT hr;
    hr = CHPPUtility::SetHomePage(szNewHomePage, bCurrentUser);
    if(FAILED(hr))
    {
        CCTRACEE(_T("CHPPUtilityImpl::SetHomePage - CHPPUtility::SetHomePage(\"%s\", \"%s\") returned 0x%08X"), szNewHomePage, bCurrentUser ? _T("true") : _T("false"), hr);
        return E_FAIL;
    }

    return S_OK;
}


//****************************************************************************
//****************************************************************************
HRESULT CHPPUtilityImpl::ResetOptions(void)
{
    HRESULT hr = E_FAIL;
    hr = CHPPUtility::ResetOptions();
    if(FAILED(hr))
    {
        CCTRACEE(_T("CHPPUtilityImpl::ResetOptions - CHPPUtility::ResetOptions() returned 0x%08X"), hr);
        return E_FAIL;
    }

    return S_OK;
}

//****************************************************************************
//****************************************************************************
HRESULT CHPPUtilityImpl::PrepHPPForUninstall(void)
{
    HRESULT hr = E_FAIL;
    hr = CHPPUtility::PrepHPPForUninstall();
    if(FAILED(hr))
    {
        CCTRACEE(_T("CHPPUtilityImpl::PrepHPPForUninstall - CHPPUtility::PrepHPPForUninstall() returned 0x%08X"), hr);
        return E_FAIL;
    }

    return S_OK;
}
