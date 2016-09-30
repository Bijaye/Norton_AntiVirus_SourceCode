#include "stdafx.h"

#include "basealertui.h"
#include <atlstr.h>
#include "ccSymCommonClientInfo.h"
#include "ccResourceLoader.h"

#include "NAVHelpLauncher.h"
#include "ResourceHelper.h"
#include "symhelp.h"
#include "resource.h"
#include "..\NAVComUIRes\ResResource.h"
#include "AVRESBranding.h"

// Resource Loader
extern cc::CResourceLoader g_ResLoader;

CBaseAlertUI::tstring CBaseAlertUI::m_strCCAlertPath;
CSymInterfaceDllHelper CBaseAlertUI::m_symHelperCCAlert;
CString CBaseAlertUI::m_strProductName;

CBaseAlertUI::CBaseAlertUI(void) :
    m_Action(OEHACTION_NONE)
{
    if ( m_strCCAlertPath.empty ())
    {
        ccLib::CString sTempCCPath;
        DWORD dwSize = MAX_PATH;
        if ( ccSym::CCommonClientInfo::GetCCDirectory (sTempCCPath))
        {
            m_strCCAlertPath = sTempCCPath;
            m_strCCAlertPath += _T("\\ccAlert.dll");
        }
    }

    if ( SYM_FAILED (m_symHelperCCAlert.Initialize(m_strCCAlertPath.c_str())))
    {
        CCTRACEE(_T("COEHeurUIDlg::COEHeurUIDlg - SymHelper failed Initialize"));
    }
    else
    {
        // Get the Alert object
        //
        SYMRESULT symRes = m_symHelperCCAlert.CreateObject(cc::IID_Alert, cc::IID_Alert, (void**) &m_pUI);
        if ( SYM_FAILED (symRes ))
        {
            CCTRACEE(_T("COEHeurUIDlg::COEHeurUIDlg - SymHelper failed CreateObject m_pUI"));
        }
    }

	if(m_strProductName.IsEmpty())
	{
		CBrandingRes BrandRes;
		m_strProductName = BrandRes.ProductName();
	}
}

CBaseAlertUI::~CBaseAlertUI(void)
{
}

int CBaseAlertUI::DoModal ()
{
	g_ResLoader.Initialize();
    // ********************************************************************************
    //
    // Callbacks - these must go before table rows for the callbacks in the rows
    //
    m_pHelpLink = new CBaseAlertUIHelp;

    // Set up the Alert Assistant stuff
    //
    m_pUI->SetHelpCallback ( m_pHelpLink );
    tstring strHelpButton;
    CResourceHelper::LoadString ( IDS_HELP, strHelpButton, g_ResLoader.GetResourceInstance() );
    m_pUI->SetHelpText ( strHelpButton.c_str() );
    //
    // ********************************************************************************

    // Set the dialog Icon.
    m_pUI->SetWindowIcon (g_ResLoader.GetResourceInstance(), IDI_NAV);

    // Set the dialog title.
    m_pUI->SetWindowTitle ( m_strProductName );
    
    // Show! - returns index of selected action
    //
    return m_pUI->DisplayAlert();
}

OEHACTION CBaseAlertUI::GetUserAction()
{
	return m_Action;
}


bool CBaseAlertUIHelp::Run(HWND hWndParent, unsigned long nData, cc::IAlert* pAlert, cc::IAlertCallback::ALERTCALLBACK context)
{
	// Launch NAV's help file with context ID.

    NAVToolbox::CNAVHelpLauncher Help;

    if ( !Help.LaunchHelp ( IDH_NAVW_AP_ALERT_EMAIL_MALICIOUS_WORM_HELP_LINK, NULL, true  ))
	{
		CCTRACEE("Failed to launch Help");
    	return false;
	}

    return true;
}


