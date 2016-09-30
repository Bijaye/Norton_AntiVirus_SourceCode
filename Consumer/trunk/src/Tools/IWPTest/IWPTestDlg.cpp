////////////////////////
//
// PROPRIETARY / CONFIDENTIAL.
// Use of this product is subject to license terms.
// Copyright © 2006 Symantec Corporation.
// All rights reserved.
//
/////////////////////////////////////////////////////////////////////////////////////////////////

// IWPTestDlg.cpp : implementation file
//

#include "stdafx.h"

#define INITIIDS
#include "IWPSettingsInterface.h"
#include "IWPSettingsLoader.h"
#include "SettingsWaitThread.h"
#include "ccDisableVerifyTrustImpl.h"   // For allowing ccAlert to trust us

SYM_OBJECT_MAP_BEGIN()                          
SYM_OBJECT_MAP_END()

// ccLib delay loader
// This will allow the ccLib dll to delay load at run-time

// TRACE implementation
ccSym::CDebugOutput g_DebugOutput(_T("IWPTest"));
IMPLEMENT_CCTRACE(::g_DebugOutput);

#include "IWPTest.h"
#include "IWPTestDlg.h"

#include "StatusPropertyNames.h"        // ccSettings keys

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#define WM_IWP_REFRESH (WM_USER+1)

// CIWPTestDlg dialog



CIWPTestDlg::CIWPTestDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CIWPTestDlg::IDD, pParent)
    , m_dwState(0)
    , m_bIsInstalled(FALSE)
    , m_bCanEnable(FALSE)
    , m_bUserWantsOn(FALSE)
    , m_strStateText(_T(""))
    , m_strStateOKText(_T(""))
    , m_strSettingsChanged(_T(""))
    , m_bFirewallOn(FALSE)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
    m_COMInit = std::auto_ptr <ccLib::CCoInitialize>(new ccLib::CCoInitialize ( ccLib::CCoInitialize::ThreadModel::eSTAModel ));
}

void CIWPTestDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialog::DoDataExchange(pDX);
    DDX_Text(pDX, IDC_STATE, m_dwState);
    DDX_Check(pDX, IDC_IS_INSTALLED, m_bIsInstalled);
    DDX_Check(pDX, IDC_CAN_ENABLE, m_bCanEnable);
    DDX_Check(pDX, IDC_USER_WANTS_ON, m_bUserWantsOn);
    DDX_Check(pDX, IDC_FIREWALL_ON, m_bFirewallOn);
    DDX_Text(pDX, IDC_STATE_TEXT, m_strStateText);
    DDX_Text(pDX, IDC_STATE_OK, m_strStateOKText);
    DDX_Text(pDX, IDC_SETTINGS_CHANGED, m_strSettingsChanged);
}

BEGIN_MESSAGE_MAP(CIWPTestDlg, CDialog)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
    ON_BN_CLICKED(IDC_REFRESH, OnBnClickedRefresh)
    ON_BN_CLICKED(IDC_SAVE, OnBnClickedSave)
    ON_MESSAGE ( WM_IWP_REFRESH, ReloadHandler)
    ON_BN_CLICKED(IDC_RESET_ALES, OnBnClickedResetAles)
    ON_BN_CLICKED(IDC_RESET_FW_RULES, OnBnClickedResetFwRules)
	ON_BN_CLICKED(IDC_LEGACY_YIELD, OnBnClickedLegacyYield)
	ON_BN_CLICKED(IDC_LEGACY_UNYIELD, OnBnClickedLegacyUnYield)
	ON_BN_CLICKED(IDC_YIELD, OnBnClickedYield)
	ON_BN_CLICKED(IDC_UNYIELD, OnBnClickedUnYield)
    ON_BN_CLICKED(IDC_FIREWALL_ON, OnBnClickedFirewallOn)
    ON_BN_CLICKED(IDC_USER_WANTS_ON, OnBnClickedUserWantsOn)
END_MESSAGE_MAP()


// CIWPTestDlg message handlers

LRESULT CIWPTestDlg::ReloadHandler (WPARAM wParam, LPARAM lParam)
{
    OnBnClickedRefresh();
    return 0;
}

BOOL CIWPTestDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

#ifdef _DEBUG
    ::MessageBox ( NULL, "attach here", "IWPTest", MB_OK );
#endif

	SYMRESULT result = m_IWPLoader.CreateObject ( &m_pIWPSettings );
    
    if ( SYM_FAILED ( result ))
    {
        AfxMessageBox ( "Error loading IWP" );
        m_bIsInstalled = FALSE;
    }
    else
    {
        // Make sure ccAlert trusts us, even though our .EXE is not signed
        //
        if ( !ccVerifyTrust::CDisableVerifyTrustClient::DisableVerifyTrust(m_pIWPSettings))
            AfxMessageBox ( "DisableVerifyTrust failed");
        else
        {
            m_bIsInstalled = TRUE;
            m_pIWPUpdate = std::auto_ptr<CSettingsWaitThread>(new CSettingsWaitThread(this));
            m_pIWPUpdate->Create (NULL,0,0);
            getSettings();
        }
    }

    UpdateData (FALSE);

	return TRUE;  // return TRUE  unless you set the focus to a control
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CIWPTestDlg::OnPaint() 
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CIWPTestDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CIWPTestDlg::OnWorkDone( LPCWSTR lpszKey )
{
    if ( 0 == wcscmp ( lpszKey, AVStatus::szIWPStatusKey ))
    {
        PostMessage ( WM_IWP_REFRESH, 0, 0);
    }
}

void CIWPTestDlg::getSettings ()
{
    if ( !m_pIWPSettings.m_p )
        return;    

    DWORD dwCanEnable = 0;
    SYMRESULT result = m_pIWPSettings->GetValue (IWP::IIWPSettings::IWPCanEnable, dwCanEnable );
    if ( SYM_FAILED (result))
    {
        ::MessageBox ( NULL, "Error getting IWPCanEnable", "IWPTest", MB_OK );
        return;
    }
    m_bCanEnable = (BOOL) dwCanEnable;

    DWORD dwUserWantsOn = 0;
    result = m_pIWPSettings->GetValue (IWP::IIWPSettings::IWPUserWantsOn, dwUserWantsOn );
    if ( SYM_FAILED (result))
    {
        ::MessageBox ( NULL, "Error getting IWPUserWantsOn", "IWPTest", MB_OK );
        return;
    }
    m_bUserWantsOn = (BOOL) dwUserWantsOn;

    m_dwState = 0;
    result = m_pIWPSettings->GetValue (IWP::IIWPSettings::IWPState, m_dwState );
    if ( SYM_FAILED (result))
    {
        ::MessageBox ( NULL, "Error getting State", "IWPTest", MB_OK );
        return;
    }

    DWORD dwFirewallOn = 0;
    result = m_pIWPSettings->GetValue (IWP::IIWPPrivateSettings::IWPFirewallOn, dwFirewallOn );
    if ( SYM_FAILED (result))
    {
        ::MessageBox ( NULL, "Error getting FirewallOn", "IWPTest", MB_OK );
        return;
    }
    
    m_bFirewallOn = (BOOL) dwFirewallOn;

    switch (m_dwState)
    {
    case IWP::IIWPSettings::IWPSTATE::IWPStateEnabled:
        m_strStateText = "Enabled";
        break;
    case IWP::IIWPSettings::IWPSTATE::IWPStateDisabled:
        m_strStateText = "Disabled";
        break;
    case IWP::IIWPSettings::IWPSTATE::IWPStateError:
        m_strStateText = "General Error!";
        break;
    case IWP::IIWPSettings::IWPSTATE::IWPStateNotAvailable:
        m_strStateText = "IWP not available";
        break;
    case IWP::IIWPSettings::IWPSTATE::IWPStateYielding:
        m_strStateText = "Yielding to another product";
        break;
    default:
        m_strStateText = "Unknown";
        break;
    }

    if (m_dwState < 100 )
        m_strStateOKText = "Error";
    else
        m_strStateOKText = "OK";
}
void CIWPTestDlg::OnBnClickedRefresh()
{
    if ( m_bIsInstalled )
    {
        m_pIWPSettings->Reload ();
        getSettings();
        m_strSettingsChanged = "";
        UpdateData (FALSE);
    }
}

void CIWPTestDlg::OnBnClickedSave()
{
    if ( m_bIsInstalled )
    {
        // Get the data from the dialog
        UpdateData (TRUE);

        try
        {
            ccLib::CCoInitialize COMInit ( ccLib::CCoInitialize::ThreadModel::eSTAModel );
        }
        catch (_com_error err)
        {
        }
        
        SYMRESULT result = m_pIWPSettings->SetValue (IWP::IIWPSettings::IWPUserWantsOn, (DWORD) m_bUserWantsOn );
        if ( SYM_FAILED (result))
        {
            ::MessageBox ( NULL, "Error setting IWPUserWantsOn", "IWPTest", MB_OK );
            return;
        }

        result = m_pIWPSettings->SetValue (IWP::IIWPPrivateSettings::IWPFirewallOn, (DWORD) m_bFirewallOn );
        if ( SYM_FAILED (result))
        {
            ::MessageBox ( NULL, "Error setting IWPFirewallOn", "IWPTest", MB_OK );
            return;
        }

        m_pIWPSettings->Save ();
        m_strSettingsChanged = "";

        UpdateData (FALSE);
    }
}

void CIWPTestDlg::OnBnClickedResetAles()
{
    SYMRESULT result = m_pIWPSettings->InstallDefaultAles();
    if ( SYM_FAILED (result))
        AfxMessageBox ( "Error resetting Ales" );
    else
        AfxMessageBox ( "ALEs reset" );
}

void CIWPTestDlg::OnBnClickedResetFwRules()
{
    SYMRESULT result = m_pIWPSettings->InstallDefaultFWRules();
    if ( SYM_FAILED (result))
        AfxMessageBox ( "Error resetting Firewall rules" );
    else
        AfxMessageBox ( "Firewall rules reset" );
}

void CIWPTestDlg::OnBnClickedLegacyYield()
{
	// first create an ini file in the temp dir called nisver.dat
	CString strTempPath;
	GetTempPath(MAX_PATH, strTempPath.GetBuffer(MAX_PATH));
	strTempPath.ReleaseBuffer();
	
	CString strINIFile;
	strINIFile.Format("%s\\NISVER.dat", strTempPath);

	WritePrivateProfileString(_T("Versions"), _T("PublicVersion"), _T("6.0.0.0"), strINIFile);

	// now create the regkey
	CRegKey rkInstalledApps;
	rkInstalledApps.Open(HKEY_LOCAL_MACHINE, _T("Software\\Symantec\\InstalledApps"));
	rkInstalledApps.SetStringValue(_T("Internet Security"), strTempPath);

	// the service should have detected that key creation, validated the ini file and done its thing
}

void CIWPTestDlg::OnBnClickedLegacyUnYield()
{
	// find the NIS dir and then delete NISVer.dat and the key
	CRegKey rkInstalledApps;
	CString strNISPath;
	DWORD dwSize = MAX_PATH;
	rkInstalledApps.Open(HKEY_LOCAL_MACHINE, _T("Software\\Symantec\\InstalledApps"));
	rkInstalledApps.QueryStringValue(_T("Internet Security"), strNISPath.GetBuffer(dwSize), &dwSize);
	strNISPath.ReleaseBuffer();
	rkInstalledApps.DeleteValue(_T("Internet Security"));

	CString strINIFile;
	strINIFile.Format("%s\\NISVER.dat", strNISPath);

	DeleteFile(strINIFile);
	
	// the service isn't running right now if we're yielding so this needs a reboot
	AfxMessageBox ( "You need to reboot before this will take effect." );
}

void CIWPTestDlg::OnBnClickedYield()
{
	HRESULT hrCOM = CoInitialize(NULL);
	
	if(FAILED(hrCOM))
		AfxMessageBox ( "Yield Failed." );
    
	IWP_IWPSettings IWPLoader;
	{
		IWP::IIWPSettingsPtr pIWPSettings;
		if(SYM_FAILED(IWPLoader.CreateObject(&pIWPSettings)))
		{
			AfxMessageBox ( "Failed getting IWP object." );	
			return;
		}
		
		CString strID;
		GetDlgItemText(IDC_YIELD_ID, strID);
        
		if(SYM_FAILED(pIWPSettings->SetYield(1, strID)))
			AfxMessageBox ( "Yield Set." );
		else
			AfxMessageBox ( "Yield Failed." );
	}		

	if(SUCCEEDED(hrCOM))
		CoUninitialize();
}

void CIWPTestDlg::OnBnClickedUnYield()
{
	HRESULT hrCOM = CoInitialize(NULL);
	
	if(FAILED(hrCOM))
		AfxMessageBox ( "Yield Failed." );

	IWP_IWPSettings IWPLoader;
	{
		IWP::IIWPSettingsPtr pIWPSettings;
		if(SYM_FAILED(IWPLoader.CreateObject(&pIWPSettings)))
		{
			AfxMessageBox ( "Failed getting IWP object." );	
			return;
		}
		
		CString strID;
		GetDlgItemText(IDC_YIELD_ID, strID);
        
		if(SYM_FAILED(pIWPSettings->SetYield(0, strID)))
			AfxMessageBox ( "UnYield Succeeded." );
		else
			AfxMessageBox ( "UnYield Failed." );
	}
	
	if(SUCCEEDED(hrCOM))
		CoUninitialize();
}
void CIWPTestDlg::OnBnClickedFirewallOn()
{
    UpdateData (TRUE);
    m_strSettingsChanged = "Settings Changed !!!";
    UpdateData (FALSE);
}

void CIWPTestDlg::OnBnClickedUserWantsOn()
{
    UpdateData (TRUE);
    m_strSettingsChanged = "Settings Changed !!!";
    UpdateData (FALSE);
}
