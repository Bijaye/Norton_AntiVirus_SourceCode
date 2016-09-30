#include "StdAfx.h"
#include ".\alertui.h"

#include <string>
#include <vector>
#include "resource.h"
#include "NAVHelpLauncher.h"                // For Help

#include "ResourceHelper.h" 

#include "StartBrowser.h"                   // For launching URLs
#include "NAVUIHTM_Resource.h"
#include "NAVInfo.h"
#include "ccModule.h"                       // For loading resources
#include "AVRESBranding.h"
#include "ccSymInstalledApps.h" // for NAV path

#include "SymInterfaceHelpers.h"
extern long g_DLLObjectCount = 0;
CSymInterfaceDllHelper g_symHelperCCAlert;

ATL::CString CAlertUI::csSARCFormat;
ATL::CString CAlertUI::csProductName;

CAlertUI::CAlertUI(void)
{
    // Load ccAlert
    //
    if(SYM_FAILED(AlertFactory.CreateObject(&m_pAlert)))
    {
        CCTRACEE ("CAlertUI::CAlertUI() - failed to create alert");
        m_pAlert = NULL;
    }
    else
    {
		if( csProductName.IsEmpty() || csSARCFormat.IsEmpty())
		{
			CBrandingRes BrandRes;
			csProductName = BrandRes.ProductName();
			csSARCFormat = BrandRes.GetString(IDS_SARC_THREAT_INFO_URL);
		}

        // Load ccAlert with all the needed params and data
        initializeAlert();

    }

    dwHelpID = 0;
}

CAlertUI::~CAlertUI()
{
    // Release ccAlert
    m_pAlert.Release();
}

bool CAlertUI::Run(HWND hWndParent, unsigned long nLinkID, cc::IAlert* pAlert, cc::IAlertCallback::ALERTCALLBACK context)
{
    if ( IAlertCallback::ALERTCALLBACK_TABLE == context )
    {
        // Launch VID link. nLinkID = VirusID
	    try
	    {
            CString csURL;
		    csURL.Format(csSARCFormat, nLinkID);
		    CCTRACEI(_T("ThreatInfo URL=%s"), csURL);

		    NAVToolbox::CStartBrowser browser;

		    if ( !browser.ShowURL ( csURL ))
			    CCTRACEE ( _T("APWCMDNT - CVirusAlertPopupVirusLink::Run failed") );
	    }
	    catch(_com_error& e)
	    {
            CCTRACEE("APWCMDNT - CVirusAlertPopupVirusLink::Run failed on exception %d", e.Error);
	    }

        return true;
    }

    if( IAlertCallback::ALERTCALLBACK_HELP == context )
    {
        NAVToolbox::CNAVHelpLauncher help;
        CCTRACEI(_T("CAlertUI::Run(HELP CALLBACK) : Help ID = %d"), dwHelpID);
        help.LaunchHelp (dwHelpID, hWndParent, true);
    }
    return true;
}

bool CAlertUI::HandleAccelerators(HWND hWndParent, WORD keyCode, DWORD& comboSelection, bool& closeDialog)
{
	DWORD selection = 0;
	for (std::vector<WORD>::const_iterator it = m_accelerators.begin();
		 it != m_accelerators.end();
		 selection++, it++) 
    {
		if (keyCode == *it) 
        {
			//set the drop down and close the dialog
			comboSelection = selection;
			closeDialog = true;
		}
	}
	return true;
}

// Send in the action name with '&' accelerators - e.g. "&Permit"
// The '&' will be remove and the accelerator will be added to
// the handlers.
//
bool CAlertUI::AddActionAccelerator (UINT uResID )
{
    if ( !m_pAlert )
        return false;

    std::string strAction;
    CResourceHelper::LoadString ( uResID, strAction, _Module.GetResourceInstance() );

	WORD wAccelerator = 0;
	LPCSTR str = strAction.c_str();
	while (wAccelerator == 0 && str && str[0] != '\0') 
    {
		str = strchr(str,'&');
		if (str) 
        {
			str++;
			if (str[0] == '&') str++;
			else if (str[0] != '\0')
				wAccelerator = static_cast<WORD>(tolower(*str));
		}
	}

	m_accelerators.push_back (wAccelerator);

	//Accelerators are not displayed using the underlined character in dropdowns. This may
	//be fixed by making the dropdown owner draw in ccAlert(hard), or by removing the '&' (easy).
	//This is an implementation of removing the '&'.

    //make new string without ampersands
	std::string newString = strAction;
	std::string::size_type pos = 0;

	while (pos < newString.length() && (pos = newString.find(_T('&'),pos)) != std::string::npos) 
    {
		newString.erase(pos,1);
		if (pos < newString.length() && newString[pos] == _T('&')) pos++;
	}

    return m_pAlert->AddAction ( newString.c_str()) ? true:false;
}

int CAlertUI::DisplayAlert(HWND hParentWnd, HWND hwndInsertAfter)
{
    if(!m_pAlert.m_p)
    {
        // FAILURE, just return the default option
        return 0;
    }
    if ( !hParentWnd )
        hParentWnd = ::GetDesktopWindow ();

    // Set up the help text and callback if there is a help ID set
    if(dwHelpID)
    {
        // Set Help Text
        m_pAlert->SetHelpText(_Module.GetResourceInstance(), IDS_HELP_BUTTON_TEXT);

        // Set the help callback
        m_pAlert->SetHelpCallback(this);
    }

    // Display the alert
    int iResult = m_pAlert->DisplayAlert(hParentWnd, hwndInsertAfter);

    // Return the result
    return iResult;
}

void CAlertUI::initializeAlert()
{
    if (!m_pAlert)
        return;

    // Set the Window Title
    m_pAlert->SetWindowTitle(csProductName);

    // Set the Windows Icon
    m_pAlert->SetWindowIcon(g_hInstance, IDI_NAV_ICON);

    // Set the Title Bitmap
    m_pAlert->SetTitleBitmap(g_hInstance, IDB_TOPBAR_YEL);  // We only have one alert level anyway

	m_pAlert->SetAcceleratorCallback(this);
}
