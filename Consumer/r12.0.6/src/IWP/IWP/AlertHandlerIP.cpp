#include "StdAfx.h"
#include ".\alerthandlerip.h"
#include "resourcehelper.h"         // For LoadString
#include "TrayNotifyUI.h"

#include "Resource.h"
#include "ccOSInfo.h"
#include "ccScanInterface.h"
#include "SymNetDriverApi.h"
#include "AlertUI.h"
#include "ccVersionInfo.h"        // CC

using namespace ccEvtMgr;
using namespace SymNeti;
using namespace cc;
using namespace ccFirewallSettings;

CAlertHandlerIP::CAlertHandlerIP(void)
{
}

CAlertHandlerIP::~CAlertHandlerIP(void)
{
}

// Handles IP and Listen events
//
IAlertData::ALERT_RESULT CAlertHandlerIP::DisplayAlert( SymNeti::CSNAlertEventEx* pSNEvent,
                                                        LPTSTR szAppName,
                                                        IAlertData::ALERT_TYPE eAlertType)
{
	if ( !pSNEvent || !szAppName )
        return IAlertData::NO_ACTION;

    m_pSNEvent = pSNEvent;
    m_eAlertType = eAlertType;
    m_strAppName = szAppName;

    commonInit();

    // Is this NetBIOS traffic?
    //
    // Get the local and remote ports
	DWORD dwLocalPort = 0;
	DWORD dwRemotePort = 0;
    bool bNetBIOS = false;

    pSNEvent->GetLocalPort(&dwLocalPort);
	pSNEvent->GetRemotePort(&dwRemotePort);
    bNetBIOS = isNetBIOS ( dwLocalPort, dwRemotePort );

    // Description
    //
    if ( eAlertType == IAlertData::ISALERTTYPE_IP )
    {
	    if(m_bAppIsKnown)
        {
            std::string strTemp;
            char szFormat [1024] = {0};
            CResourceHelper::LoadString ( bNetBIOS ? IDS_FMTAPPNBDESCRIPTION_INBOUND : IDS_FMTAPPDESCRIPTION_INBOUND,
                                        strTemp, g_hInstance );

            _snprintf ( szFormat, 1024-1, strTemp.c_str(), szAppName );
            m_strDescription = szFormat;
        }
	    else
	    {
            std::string strTemp;
            std::string strUnknownApp;
            char szFormat [1024] = {0};

            CResourceHelper::LoadString ( bNetBIOS ? IDS_FMTAPPNBDESCRIPTION_INBOUND_NOAPP : IDS_FMTAPPDESCRIPTION_INBOUND_NOAPP,
                                        strTemp, g_hInstance );
            CResourceHelper::LoadString ( IDS_ANUNKNOWNAPP, strUnknownApp, g_hInstance );

            _snprintf ( szFormat, 1024-1, strTemp.c_str(), strUnknownApp.c_str() );
            m_strDescription = szFormat;

	    }
    }
    else if ( eAlertType == IAlertData::ISALERTTYPE_LISTEN )
    {
        if ( m_bAppIsKnown )
        {
            char szFormat [1024] = {0};
            std::string strTemp;
            CResourceHelper::LoadString ( IDS_FMTLISTENDESCRIPTION, strTemp, g_hInstance );
            _snprintf ( szFormat, 1024-1, strTemp.c_str(), m_strAppName.c_str());
            m_strDescription = szFormat;
        }
        else
            CResourceHelper::LoadString ( IDS_UNKNOWN_LISTEN_APP, m_strDescription, g_hInstance );
    }

	// Title & help ID
    //
    if ( eAlertType == IAlertData::ISALERTTYPE_IP )
    {
        m_alert.GetAlert()->SetAlertTitle ( g_hInstance, IDS_SECURITYALERT );
        m_alert.dwHelpID = NAV_CSH_IWP_IP_TRAFFIC_ALERT;
    }
    else
    {
        m_alert.GetAlert()->SetAlertTitle ( g_hInstance, IDS_INETACCESSCONTROL );
        m_alert.dwHelpID = NAV_CSH_IWP_LISTEN_ALERT;
    }

    // IP and Listen events
    //
    std::map <int, IAlertData::ALERT_RESULT> mapActions;
    int iActionIndex = 0;

    // Combo box list
    //
    m_alert.AddActionAccelerator ( IDS_PERMIT_RECOMMENDED );
    mapActions[iActionIndex++] = IAlertData::PERMIT_ALL;  // creates a permit rule based on the traffic

    m_alert.AddActionAccelerator ( IDS_BLOCK );
    mapActions[iActionIndex++] = IAlertData::BLOCK_ALL;  // creates a block rule based on the traffic

    // Return the IAlertData result that matches the UI return action.
    //
    int iResult = displayCommonAlert ();

	return mapActions [iResult];
}