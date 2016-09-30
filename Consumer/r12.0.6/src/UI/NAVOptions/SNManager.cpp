#include "StdAfx.h"
#include "snmanager.h"
#include "SymNetAPIVer.h"

CSNManager::CSNManager(void)
{
    m_bInit = false;
    m_pSNGetFirewallSetting = NULL;
    m_pSNSetFirewallSetting = NULL;
    m_pSNInit = NULL;
    m_pSNUnInit = NULL;

    if ( m_modSymNeti.Load (_T("SymNeti.dll")))
	{
        m_pSNSetFirewallSetting = (SNSETFIREWALLSETTING) ::GetProcAddress ( m_modSymNeti, (LPCTSTR) eSNSetFirewallSetting);
		if(!m_pSNSetFirewallSetting)
			CCTRACEE("CSNManager::CSNManager(): Failed getting m_pSNSetFirewallSetting.");

        m_pSNGetFirewallSetting = (SNGETFIREWALLSETTING) ::GetProcAddress ( m_modSymNeti, (LPCTSTR) eSNGetFirewallSetting);
		if(!m_pSNSetFirewallSetting)
			CCTRACEE("CSNManager::CSNManager(): Failed getting m_pSNGetFirewallSetting.");

        m_pSNInit = (SNINIT) ::GetProcAddress ( m_modSymNeti, (LPCTSTR) eSNInit);
		if(!m_pSNInit)
			CCTRACEE("CSNManager::CSNManager(): Failed getting m_pSNInit.");

        m_pSNUnInit = (SNUNINIT) ::GetProcAddress ( m_modSymNeti, (LPCTSTR) eSNUnInit);
		if(!m_pSNUnInit)
			CCTRACEE("CSNManager::CSNManager(): Failed getting m_pSNUnInit.");

        DWORD dwSuccess = 0;
        HRESULT hr = m_pSNInit(VER_SN_VERSION, &dwSuccess);	
        if ( FAILED (hr))
    	    CCTRACEE("SNInit: dwSuccess: %d, HRESULT: 0x%x", dwSuccess, hr);
        else
            m_bInit = true;
    }
}

CSNManager::~CSNManager(void)
{
    if (m_pSNUnInit && m_bInit)
        m_pSNUnInit();
}

HRESULT CSNManager::SetAutoBlockEnabled (bool bOn)
{
    if( !m_pSNSetFirewallSetting)
        return E_POINTER;

	if ( !m_bInit )
        return E_FAIL;
         

    HRESULT hr = S_OK;

    // Turn on/off auto correct
    //
    hr = m_pSNSetFirewallSetting (FS_AutoCorrectEnabled, bOn ? 1 : 0);
    if ( FAILED(hr))
    {
        CCTRACEE ("CSNManager::SetAutoBlockEnabled - failed FS_AutoCorrectEnabled. Return code = 0x%X", hr);
    }

    return hr;
}

HRESULT CSNManager::GetAutoBlockEnabled (bool &bOn)
{
    if( !m_pSNGetFirewallSetting)
        return E_POINTER;

	if ( !m_bInit )
        return E_FAIL;
         

    HRESULT hr = S_OK;

    DWORD dwEnabled = 0;
    hr = m_pSNGetFirewallSetting (FS_AutoCorrectEnabled, &dwEnabled);
    if ( FAILED(hr))
    {
        CCTRACEE ("CSNManager::GetAutoBlockEnabled - failed FS_AutoCorrectEnabled. Return code = 0x%X", hr);
    }
    
    bOn = dwEnabled ? true : false;

    return hr;
}