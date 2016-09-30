// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright (c) 2005 Symantec Corporation. All rights reserved.
// ScanOptionsPage.cpp : implementation file
//

#include "stdafx.h"
#include "LDVPTaskFS.h"
#include "ScanOptionsPage.h"
#include "Wizard.h"
#include "ModalConfig.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CScanOptionsPage property page

IMPLEMENT_DYNCREATE(CScanOptionsPage, CWizardPage)

CScanOptionsPage::CScanOptionsPage() : CWizardPage(CScanOptionsPage::IDD)
{
	//{{AFX_DATA_INIT(CScanOptionsPage)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	m_psp.dwFlags |= PSP_HASHELP;
	m_bInitialized = FALSE;
	//Set the Help Context ID
	SetHelpID( IDD );
}

CScanOptionsPage::~CScanOptionsPage()
{
}

void CScanOptionsPage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CScanOptionsPage)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CScanOptionsPage, CPropertyPage)
	//{{AFX_MSG_MAP(CScanOptionsPage)
	ON_WM_PAINT()
	ON_BN_CLICKED(IDC_BROWSE_FILE_TYPES, OnBrowseFileTypes)
	ON_BN_CLICKED(IDC_MESSAGE, OnMessage)
	ON_BN_CLICKED(IDC_BROWSE_EXCLUDE_FILES, OnBrowseExcludeFiles)
	ON_BN_CLICKED(IDC_TYPES_ALL, OnTypesAll)
	ON_BN_CLICKED(IDC_TYPES_SELECTED, OnTypesSelected)
	ON_BN_CLICKED(IDC_EXCLUDE_FILES, OnExcludeFiles)
	ON_BN_CLICKED(IDC_MESSAGE_BOX, OnMessageBox)
	//}}AFX_MSG_MAP
	ON_COMMAND( UWM_DESTROYING, OnDestroying )
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CScanOptionsPage message handlers

BOOL CScanOptionsPage::OnSetActive() 
{
	CWizardPage::OnSetActive();
	
	CClientPropSheet *ptrParent = (CClientPropSheet*)GetParent();

	if( m_bInitialized )
	{
		return TRUE;
	}
	//Set the Context-sensitive ID
	SetHelpID( IDD_SCAN_OPTIONS );

	//Load the options
	if( InWizardMode() )
	{
		ptrParent->HideBack();
		ptrParent->m_strTitle.LoadString( IDS_IDLE_SCAN );
		ptrParent->SetFinishText( IDS_CLOSE );
		ptrParent->SetWizardButtons( PSWIZB_FINISH );
	}

	//Now open the scan options
	ptrParent->OpenScan( FALSE );

	CConfigObj	config( GetScanConfig() );

	//And update the interface
	SetDialogOptions();

	UpdateInterface();		

	m_bInitialized = TRUE;

	return TRUE;
}

void CScanOptionsPage::OnDestroying()
{
	GetDialogOptions();
}

LRESULT CScanOptionsPage::OnWizardNext() 
{
	GetDialogOptions();
		
	//I return -1 so the page doesn't change. This is because
	//	there is no page to change to!
	//	The ONLY way I can get the WizardNext message is from 
	//	the main app terminating the task, in which case
	//	I simply want to save out my options. The task will
	//	be terminated immediatly after this.
	return -1;
}

BOOL CScanOptionsPage::OnWizardFinish() 
{
	//Fire an event telling the App to close the task
	((CLDVPTaskFSApp*)AfxGetApp())->m_ptrCtl->NotifyEndTask();

	return CPropertyPage::OnWizardFinish();
}

void CScanOptionsPage::OnPaint() 
{
	CPaintDC dc(this); // device context for painting

	//PaintTitle( &dc );
}

void CScanOptionsPage::OnBrowseFileTypes() 
{
	GetExtensions();
}

void CScanOptionsPage::GetDialogOptions()
{
	//Get my Config object
	CConfigObj	config( GetScanConfig() );

	//Save out the options
	CWizardPage::GetDialogOptions( config );


	//Now, if the check box is checked, I need to save these options
	//	out as the default scan options also
	CButton	*pRemember = (CButton*)GetDlgItem( IDC_REMEMBER_SETTINGS );

	if( pRemember && pRemember->GetCheck() )
	{
		IScanConfig *ptrScanConfig;

		//First, create a new IScanConfig from CliScan
		if( S_OK == CoCreateInstance( CLSID_Cliscan, NULL, CLSCTX_INPROC_SERVER, IID_IScanConfig, (void**)&ptrScanConfig ) )
		{
			//Now, open the Default ScanOptions scan
			ptrScanConfig->Open( NULL, HKEY_VP_USER_SCANS, szReg_Key_Scan_Defaults );
			
			//And put it into a ConfigObj
			CConfigObj	defaultConfig( (IConfig*)ptrScanConfig );

			//And save out the options
			CWizardPage::GetDialogOptions( defaultConfig );

			//Finally, release the reference to the object
			ptrScanConfig->Release();

		}
	}

}

void CScanOptionsPage::SetDialogOptions()
{
	CConfigObj	config( GetScanConfig() );

	CWizardPage::SetDialogOptions( config );


}

void CScanOptionsPage::OnMessage() 
{
	CConfigObj	config( GetScanConfig() );
	CString		strTemp;

	CButton	*ptrButton = (CButton*)GetDlgItem( IDC_MESSAGE_BOX );
	
	GetMessage();

	strTemp = config.GetOption( szReg_Val_Message, "" );
	ptrButton->SetCheck( !strTemp.IsEmpty() );

}

void CScanOptionsPage::OnBrowseExcludeFiles() 
{
	GetExcludeFiles();	
}

void CScanOptionsPage::OnOK() 
{
	//When modal, we get an OnOK. This is where
	//	we need to save out the options.
	GetDialogOptions();	
	
	CPropertyPage::OnOK();
}

void CScanOptionsPage::OnTypesAll() 
{
	UpdateInterface();
}

void CScanOptionsPage::OnTypesSelected() 
{
	UpdateInterface();
}

void CScanOptionsPage::OnExcludeFiles() 
{
	UpdateInterface();	
}

void CScanOptionsPage::OnMessageBox() 
{
	UpdateInterface();	
}
