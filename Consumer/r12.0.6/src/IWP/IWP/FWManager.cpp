#include "StdAfx.h"
#include ".\fwmanager.h"
#include "SymNetAPIVer.h"
#include "build.h"
#include "iacommon.h"      // For Fragmented Action

CFWManager::CFWManager(void)
{
    m_bInit = false;
    m_pSNResetToFactoryDefaults = NULL;
    m_pSNSetFirewallSetting = NULL;
    m_pSNSetProductState = NULL;
    m_pSNInit = NULL;
    m_pSNUnInit = NULL;

    if ( m_modSymNeti.Load (_T("SymNeti.dll")))
	{
		m_pSNResetToFactoryDefaults = (SNRESETTOFACTORYDEFAULTS) ::GetProcAddress ( m_modSymNeti, (LPCTSTR) eSNResetToFactoryDefaults );
		if(!m_pSNResetToFactoryDefaults)
			CCTRACEE("CFWManager::CFWManager(): Failed getting m_pSNResetToFactoryDefaults.");

        m_pSNSetFirewallSetting = (SNSETFIREWALLSETTING) ::GetProcAddress ( m_modSymNeti, (LPCTSTR) eSNSetFirewallSetting);
		if(!m_pSNSetFirewallSetting)
			CCTRACEE("CFWManager::CFWManager(): Failed getting m_pSNSetFirewallSetting.");

        m_pSNSetProductState = (SNSETPRODUCTSTATE) ::GetProcAddress ( m_modSymNeti, (LPCTSTR) eSNSetProductState);
		if(!m_pSNSetProductState)
			CCTRACEE("CFWManager::CFWManager(): Failed getting m_pSNSetProductState.");

        m_pSNInit = (SNINIT) ::GetProcAddress ( m_modSymNeti, (LPCTSTR) eSNInit);
		if(!m_pSNInit)
			CCTRACEE("CFWManager::CFWManager(): Failed getting m_pSNInit.");

        m_pSNUnInit = (SNUNINIT) ::GetProcAddress ( m_modSymNeti, (LPCTSTR) eSNUnInit);
		if(!m_pSNUnInit)
			CCTRACEE("CFWManager::CFWManager(): Failed getting m_pSNUnInit.");

        if (m_pSNInit)
        {
            DWORD dwSuccess = 0;
            HRESULT hr = m_pSNInit(VER_SN_VERSION, &dwSuccess);	
            if ( FAILED (hr))
    	        CCTRACEE("CFWManager::CFWManager() - SNInit: dwSuccess: %d, HRESULT: 0x%x", dwSuccess, hr);
            else
                m_bInit = true;
        }
    }
}

CFWManager::~CFWManager(void)
{
    if (m_pSNUnInit && m_bInit)
        m_pSNUnInit();
}

SYMRESULT CFWManager::SetFirewallSettings (bool bOn, bool bAllClients /*false*/, bool bFirewallOnly /*false*/)
{
	if ( !m_bInit ||
         !m_pSNSetProductState || 
         !m_pSNSetFirewallSetting)
        return SYMERR_UNKNOWN;

    CCTRACEI("CFWManager::SetFirewallSettings - On:%d  AllClients:%d  FirewallOnly:%d", bOn, bAllClients, bFirewallOnly);

    HRESULT hr = E_FAIL;

    //GS_IDS_STATE - handled by IDSManager via the new SymInterface API calls.
    int iLastClient = bAllClients ? eCLIENTID_Last : eCLIENTID_Nav_FW+1;
    int iClient = bAllClients ? 1 : eCLIENTID_Nav_FW; /*don't use 0, it's invalid*/

    for (; iClient != iLastClient; iClient++ ) 
    {
        if ( !bFirewallOnly )
        {
            // Set firewall to run at startup
            //
            hr = m_pSNSetProductState(iClient, GS_INITIAL_STATE, bOn ? 1 : 2); // 1 = auto, 2 = manual
            if ( FAILED(hr))
                CCTRACEE ("CFWManager::setFirewallSettings - failed GS_INITIAL_STATE");

            hr = m_pSNSetProductState(iClient, GS_CURRENT_STATE, bOn ? 1 : 0);
                if ( FAILED(hr))
                    CCTRACEE ("CFWManager::setFirewallSettings - failed GS_CURRENT_STATE");
        }

        hr = m_pSNSetProductState(iClient, GS_FIREWALL_STATE, bOn ? 1 : 0);
        if ( FAILED(hr))
            CCTRACEE ("CFWManager::setFirewallSettings - failed GS_FIREWALL_STATE");
    }

    // Advanced FW settings
    //
    hr = m_pSNSetFirewallSetting (FS_StealthBlocked, 1);
    if ( FAILED(hr))
        CCTRACEE ("CFWManager::setFirewallSettings - failed FS_StealthBlocked");

    hr = m_pSNSetFirewallSetting (FS_BlockIGMP, 0);
    if ( FAILED(hr))
        CCTRACEE ("CFWManager::setFirewallSettings - failed FS_BlockIGMP");

    hr = m_pSNSetFirewallSetting (FS_AlertAccessedUnusedPorts, 0);
    if ( FAILED(hr))
        CCTRACEE ("CFWManager::setFirewallSettings - failed FS_AlertAccessedUnusedPorts");

    hr = m_pSNSetFirewallSetting (FS_EnableLocationAwareness, 0); 
    if ( FAILED(hr))
        CCTRACEE ("CFWManager::setFirewallSettings - failed FS_EnableLocationAwareness");

    // Make sure we get UDP traffic events - defect 1-2XEM72
    //    
    hr = m_pSNSetFirewallSetting (FS_DatagramReceiveAction, 2); 
    if ( FAILED(hr))
        CCTRACEE ("CFWManager::setFirewallSettings - failed FS_DatagramReceiveAction");    

    // turn off Port Scan detection, not supported by IWP
    hr = m_pSNSetFirewallSetting (FS_ScanDetectEnabled, 0);
    if ( FAILED(hr))
        CCTRACEE ("CFWManager::setFirewallSettings - failed FS_ScanDetectEnabled");

    // Allow inbound connections from an IP
    // after we have initiated an outbound connection to that IP
    hr = m_pSNSetFirewallSetting (FS_FilterOutbound, 0);
    if ( FAILED(hr))
        CCTRACEE ("CFWManager::setFirewallSettings - failed FS_FilterOutbound");

    // Port Blocking - more like Port Allowing, actually. Don't log, since we don't block any
    // connections with these settings.
    //
    hr = m_pSNSetFirewallSetting (FS_PortBlockingEnabled, 1);
    if ( FAILED(hr))
        CCTRACEE ("CFWManager::setFirewallSettings - failed FS_PortBlockingEnabled");
    
    hr = m_pSNSetFirewallSetting (FS_PBDecisionFlags, PBFLAG_ALLOWWHENFOUND);    
    if ( FAILED(hr))
        CCTRACEE ("CFWManager::setFirewallSettings - failed FS_PBDecisionFlags");

    return SYM_OK;
}

SYMRESULT CFWManager::ResetSymNetDrvSettings(PFACTORYRESETPARAMS pFRP)
{
	CCTRACEI("CFWManager::ResetSymNetDrvSettings()");

	if (m_bInit && m_pSNResetToFactoryDefaults)
    {
    	
	    PFACTORYRESETPARAMS pFRPtoUSE;
	    FACTORYRESETPARAMS LocalFRP;

	    // if pFRP was passed in then make our own and set it up for NAV
	    if(!pFRP)
	    {
		    ZeroMemory(&LocalFRP, sizeof(FACTORYRESETPARAMS));
		    LocalFRP.structsize = sizeof(FACTORYRESETPARAMS);
		    LocalFRP.eProductID = eCLIENTID_Nav_FW;
		    LocalFRP.MajorVersion = VER_NUM_PRODVERMAJOR; // from build.h
		    LocalFRP.MinorVersion = VER_NUM_PRODVERMINOR;

		    pFRPtoUSE = &LocalFRP;
	    }
	    else
	    {
		    pFRPtoUSE = pFRP;
	    }

		CCTRACEI("CFWManager::ResetSymNetDrvSettings() - Resetting to: ProductID = %d, MajorVer = %d, MinorVer = %d.", 
					pFRPtoUSE->eProductID, pFRPtoUSE->MajorVersion, pFRPtoUSE->MinorVersion );
    		
		HRESULT hr = m_pSNResetToFactoryDefaults(pFRPtoUSE);
		if(SUCCEEDED(hr))
		{
			CCTRACEI("CFWManager::ResetSymNetDrvSettings(): SNResetToFactoryDefaults() Succeeded.");
			return SYM_OK;
		}
		else
		{
			CCTRACEE("CFWManager::ResetSymNetDrvSettings(): SNResetToFactoryDefaults() == %d", hr);
			return SYMERR_UNKNOWN;
		}
	}

	// shoudn't get here
	return SYMERR_UNKNOWN;
}

