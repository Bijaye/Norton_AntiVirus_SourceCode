// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright (c) 2005 Symantec Corporation. All rights reserved.
// LicensingView.cpp : implementation file
//

#include "stdafx.h"
#include "LDVPView.h"
#include "LicensingView.h"
#include "Wizard.h"
#include <atlbase.h>
#include "slic.h"
#include "SLICWrap.h"
#include "SLICLicenseData.h"
#include <mbstring.h>
#include "SymSaferRegistry.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#include "SLICWrap_i.c"

/////////////////////////////////////////////////////////////////////////////
// CLicensingView

IMPLEMENT_DYNCREATE(CLicensingView, CPropertyPage)

CLicensingView::CLicensingView()
	: CWizardPage(CLicensingView::IDD)
{
	//{{AFX_DATA_INIT(CLicensingView)
	m_InstructionText = _T("");
	m_LicExpiryDate = _T("");
	m_LicSerialNumber = _T("");
	m_LicStatus = _T("");
	m_LicStartDate = _T("");
	m_InstructionLink = _T("");
	//}}AFX_DATA_INIT

	m_psp.dwFlags |= PSP_HASHELP;

	//Set the Help Context ID
	SetHelpID( IDD );

}

CLicensingView::~CLicensingView()
{
}

void CLicensingView::DoDataExchange(CDataExchange* pDX)
{
	CWizardPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CLicensingView)
	DDX_Control(pDX, IDC_INSTRUCTIONS_LINK, m_InstructionLinkCtl);
	DDX_Text(pDX, IDC_INSTRUCTIONS_TEXT, m_InstructionText);
	DDX_Text(pDX, IDC_LIC_EXPIRY_DATE, m_LicExpiryDate);
	DDX_Text(pDX, IDC_LIC_SERIAL_NUM, m_LicSerialNumber);
	DDX_Text(pDX, IDC_LIC_STATUS, m_LicStatus);
	DDX_Text(pDX, IDC_LIC_START_DATE, m_LicStartDate);
	DDX_Text(pDX, IDC_INSTRUCTIONS_LINK, m_InstructionLink);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CLicensingView, CPropertyPage)
	//{{AFX_MSG_MAP(CLicensingView)
	ON_BN_CLICKED(IDC_INSTALL_LICENSE, OnInstallLicense)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CLicensingView message handlers

BOOL CLicensingView::OnSetActive() 
{
	CWizardPage::OnSetActive();

	CWizard	*ptrParent = (CWizard*)GetParent();

	// Set the Context-sensitive ID
	SetHelpID( IDD_LICENSING );

	// Parse the raw resouce string to draw out the the menu and taskpad headings.
	CString sMenu, sTaskpad, sRaw;
	sRaw.LoadString( IDS_TASK_LICENSING  );
	ConvertRawMenuString( sRaw, sMenu, sTaskpad );
	
	// Load the options
	ptrParent->HideBack();
	ptrParent->m_strTitle = sTaskpad;
	ptrParent->SetFinishText( IDS_CLOSE );
	ptrParent->SetWizardButtons( PSWIZB_FINISH );

	return TRUE;
}

BOOL CLicensingView::OnWizardFinish() 
{
	// Fire an event telling the App to close the task
	((CLDVPViewApp*)AfxGetApp())->m_ptrCtl->NotifyEndTask();

	return CPropertyPage::OnWizardFinish();
}

void CLicensingView::OnInstallLicense() 
{
	// First, get an instance of the COM object that wraps SLIC.

	HRESULT hr = S_OK;

	// If we can't get an instance, we can assume that this is an unlicensed version.

	CComPtr<ISLICLicenseRepositorySCS> slic;

	if( SUCCEEDED( slic.CoCreateInstance( CLSID_SLICLicenseRepository, NULL, CLSCTX_INPROC_SERVER ) ) )
	{
		// Show the common license install wizard dialogs.

        SLIC_STATUS slic_status = SLICSTATUS_OK;

		hr = slic->InstallLicenseSCS( ((CLDVPViewApp*)AfxGetApp())->m_ptrCtl->GetSafeHwnd(),
                                      &slic_status );

		if( SUCCEEDED( hr ) )
		{
			// If license install succeeded, update the display.

			if( SLIC_SUCCEEDED( slic_status ) )
			{
				UpdateLicenseData();

				UpdateData( FALSE );
			}
		}
	}
}

void CLicensingView::UpdateLicenseData()
{
	// Get an instance of the COM object that wraps SLIC.

	HRESULT hr = S_OK;

	// If we can't get an instance, we can assume that this is an unlicensed version.

	CComPtr<ISLICLicenseRepositorySCS> slic;

	if( SUCCEEDED( slic.CoCreateInstance( CLSID_SLICLicenseRepository, NULL, CLSCTX_INPROC_SERVER ) ) )
	{
		CComPtr<ISLICLicense> lic;

		CComBSTR license_status_dscr;
		SLIC_STATUS slic_status = SLICSTATUS_OK;

		hr = slic->CheckLicenseSCS( &slic_status, &license_status_dscr, &lic.p );

		if( SUCCEEDED( hr ) )
		{
			// The call might succeed, but we might not get a license.

			if( lic != NULL )
			{
				// We have a license.

				SSLICLicenseData lic_data;

				hr = lic->GetLicenseData( &lic_data );

				// Convert license dates to localized strings.

				CComBSTR temp;

				lic->GetDateInLocalFormat( lic_data.expiredate, &temp );
				m_LicExpiryDate = temp;

				lic->GetDateInLocalFormat( lic_data.startdate, &temp );
				m_LicStartDate = temp;

				m_LicSerialNumber = lic_data.serial_id;
			}

			m_LicStatus = license_status_dscr;
            
            
            // Load the URL based on the status
            UINT uID = IDS_LICENSE_LINK;
            if (lic != NULL)
            {
			    SLIC_LIFECYCLE_VAL lifecycle;
			    hr = lic->GetLifecycle( &lifecycle );
			    if( SUCCEEDED( hr ) )
			    {
				    switch( lifecycle )
				    {
                    case SLIC_LIFECYCLE_WARN :
                    case SLIC_LIFECYCLE_GRACE :
                    case SLIC_LIFECYCLE_EXPIRED :
                        uID = IDS_LICENSE_RENEW_LINK;
                        break;
                    }
                }
            }

            m_InstructionLink.LoadString( uID );
		}
	}
}

BOOL CLicensingView::OnInitDialog() 
{
    // See if we should hide the Install License button.

    BOOL allow_install = TRUE;
    DWORD type;
    DWORD size = sizeof(allow_install);
    HKEY  lic_key;

    if( ERROR_SUCCESS == RegOpenKeyEx( HKEY_LOCAL_MACHINE, szReg_Key_Main "\\" szReg_Key_Licensing, 0, KEY_READ, &lic_key ) )
    {
        if( (ERROR_SUCCESS != SymSaferRegQueryValueEx( lic_key, 
                                               szReg_Val_LicenseAllowUserInstall,
                                               NULL,
                                               &type,
                                               reinterpret_cast<LPBYTE>( &allow_install ),
                                               &size )) ||
            type != REG_DWORD ||
            size != sizeof(DWORD) )
        {
            allow_install = TRUE;
        }

        RegCloseKey( lic_key );
    }

    if( allow_install == FALSE )
    {
        GetDlgItem( IDC_INSTALL_LICENSE )->EnableWindow( FALSE );
    }

    // Load the instructions and the link.

	m_InstructionText.LoadString( IDS_LICENSE_ADMINTEXT );
    m_InstructionLink.LoadString( IDS_LICENSE_LINK );

    // Initial display of license data.

	UpdateLicenseData();

	CPropertyPage::OnInitDialog();

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
