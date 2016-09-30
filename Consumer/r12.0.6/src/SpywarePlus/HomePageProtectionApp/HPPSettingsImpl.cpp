#include "StdAfx.h"

#include "HPPSettingsImpl.h"

#include "HPPEventsInterface.h"

#include "NAVSettingsHelperEx.h"
#include "OptNames.h"




CHPPSettingsImpl::CHPPSettingsImpl(void)
{
    m_lStartupBehavior = HPP::HPP_STARTUP_RUN_AT_STARTUP;
    m_lBlockBehavior = HPP::HPP_ACTION_BLOCK;
    m_lAlertFrequency = HPP::HPP_ALERT_FREQUENCY_ON_HOME_PAGE_ACCESS;
    m_lFeatureState = HPP::HPP_FEATURE_STATE_ENABLED;
    m_lFirstRunDialog = HPP::HPP_FIRST_RUN_DIALOG_SHOW;
}

CHPPSettingsImpl::~CHPPSettingsImpl(void)
{
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
HRESULT CHPPSettingsImpl::ReadSettings()
{
    DWORD dwGet = NULL;
    HRESULT hr;
    CNAVOptSettingsEx NavOpts;
    if (!NavOpts.Init())
        return E_UNEXPECTED;
    
    dwGet = HPP::HPP_FEATURE_STATE_ENABLED;
    hr = NavOpts.GetValue(HOMEPAGEPROTECTION_FeatureEnabled, dwGet, HPP::HPP_FEATURE_STATE_ENABLED);
    if(SUCCEEDED(hr))
        m_lFeatureState = dwGet;
    else 
        CCTRACEE(_T("NavOpts.GetValue(HOMEPAGEPROTECTION_FeatureEnabled) failed. hr = 0x%08X\n"), hr);

    dwGet = HPP::HPP_STARTUP_RUN_AT_STARTUP;
    hr = NavOpts.GetValue(HOMEPAGEPROTECTION_Enabled, dwGet, HPP::HPP_STARTUP_RUN_AT_STARTUP);
    if(SUCCEEDED(hr))
        m_lStartupBehavior = dwGet;
    else 
        CCTRACEE(_T("NavOpts.GetValue(HOMEPAGEPROTECTION_Enabled) failed. hr = 0x%08X\n"), hr);

    dwGet = HPP::HPP_ACTION_BLOCK;
    hr = NavOpts.GetValue(HOMEPAGEPROTECTION_Respond, dwGet, HPP::HPP_ACTION_BLOCK);
    if(SUCCEEDED(hr))
        m_lBlockBehavior = dwGet;
    else 
        CCTRACEE(_T("NavOpts.GetValue(HOMEPAGEPROTECTION_Respond) failed. hr = 0x%08X\n"), hr);

    dwGet = HPP::HPP_ALERT_FREQUENCY_ON_HOME_PAGE_ACCESS;
    hr = NavOpts.GetValue(HOMEPAGEPROTECTION_Control, dwGet, HPP::HPP_ALERT_FREQUENCY_ON_HOME_PAGE_ACCESS);
    if(SUCCEEDED(hr))
        m_lAlertFrequency = dwGet;
    else 
        CCTRACEE(_T("NavOpts.GetValue(HOMEPAGEPROTECTION_Control) failed. hr = 0x%08X\n"), hr);

    dwGet = HPP::HPP_FIRST_RUN_DIALOG_SHOW;
    hr = NavOpts.GetValue(HOMEPAGEPROTECTION_FirstRunDialog, dwGet, HPP::HPP_FIRST_RUN_DIALOG_SHOW);
    if(SUCCEEDED(hr))
        m_lFirstRunDialog = dwGet;
    else 
        CCTRACEE(_T("NavOpts.GetValue(HOMEPAGEPROTECTION_FirstRunDialog) failed. hr = 0x%08X\n"), hr);

    return S_OK;
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
HRESULT CHPPSettingsImpl::WriteSettings()
{
    CNAVOptSettingsEx NavOpts;
    if (!NavOpts.Init())
        return E_UNEXPECTED;
    
    HRESULT hr = E_FAIL;

    hr = NavOpts.SetValue(HOMEPAGEPROTECTION_FeatureEnabled, m_lFeatureState, false);
    if(FAILED(hr))
        CCTRACEE(_T("NavOpts.GetValue(HOMEPAGEPROTECTION_FeatureEnabled) failed. hr = 0x%08X\n"), hr);

    hr = NavOpts.SetValue(HOMEPAGEPROTECTION_Enabled, m_lStartupBehavior, false);
    if(FAILED(hr))
        CCTRACEE(_T("NavOpts.GetValue(HOMEPAGEPROTECTION_Enabled) failed. hr = 0x%08X\n"), hr);

    hr = NavOpts.SetValue(HOMEPAGEPROTECTION_Respond, m_lBlockBehavior, false);
    if(FAILED(hr))
        CCTRACEE(_T("NavOpts.GetValue(HOMEPAGEPROTECTION_Respond) failed. hr = 0x%08X\n"), hr);

    hr = NavOpts.SetValue(HOMEPAGEPROTECTION_Control, m_lAlertFrequency, false);
    if(FAILED(hr))
        CCTRACEE(_T("NavOpts.GetValue(HOMEPAGEPROTECTION_Control) failed. hr = 0x%08X\n"), hr);

    hr = NavOpts.SetValue(HOMEPAGEPROTECTION_FirstRunDialog, m_lFirstRunDialog, false);
    if(FAILED(hr))
        CCTRACEE(_T("NavOpts.GetValue(HOMEPAGEPROTECTION_Control) failed. hr = 0x%08X\n"), hr);

    if ( !NavOpts.Save())
        CCTRACEE(_T("NavOpts.Save() failed.\n"));

    return S_OK;
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
HRESULT CHPPSettingsImpl::SetFeatureState(LONG lFeatureState)
{
    m_lFeatureState = lFeatureState;
    return S_OK;
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
HRESULT CHPPSettingsImpl::GetFeatureState(LPLONG plFeatureState)
{
	 if(!plFeatureState)
         return E_POINTER;

     *plFeatureState = m_lFeatureState;
     return S_OK;
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
HRESULT CHPPSettingsImpl::SetRunAtStartup(LONG lStartupBehavior)
{
    m_lStartupBehavior = lStartupBehavior;
    return S_OK;
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
HRESULT CHPPSettingsImpl::GetRunAtStartup(LPLONG plStartupBehavior)
{
    if(!plStartupBehavior)
        return E_POINTER;

    *plStartupBehavior = m_lStartupBehavior;
    return S_OK;
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
HRESULT CHPPSettingsImpl::SetDefaultBlockAction(LONG lBlockBehavior)
{
    m_lBlockBehavior = lBlockBehavior;
    return S_OK;
}


//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
HRESULT CHPPSettingsImpl::GetDefaultBlockAction(LPLONG plBlockBehavior)
{
    if(!plBlockBehavior)
        return E_POINTER;

    *plBlockBehavior = m_lBlockBehavior;
    return S_OK;
}


//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
HRESULT CHPPSettingsImpl::SetAlertFrequency(LONG lAlertFrequency)
{
    m_lAlertFrequency = lAlertFrequency;
    return S_OK;
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
HRESULT CHPPSettingsImpl::GetAlertFrequency(LPLONG plAlertFrequency)
{
    if(!plAlertFrequency)
        return E_POINTER;

    *plAlertFrequency = m_lAlertFrequency;
    return S_OK;
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
HRESULT CHPPSettingsImpl::SetFirstRunDialog(LONG lFirstRunDialog)
{
    m_lFirstRunDialog = lFirstRunDialog;
    return S_OK;
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
HRESULT CHPPSettingsImpl::GetFirstRunDialog(LPLONG plFirstRunDialog)
{
    if(!plFirstRunDialog)
        return E_POINTER;

    *plFirstRunDialog = m_lFirstRunDialog;
    return S_OK;
}



