// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright (c) 2005 Symantec Corporation. All rights reserved.
// RTSOptionsPage.cpp : implementation file
//

#include "stdafx.h"
#include "LDVPTaskFS.h"
#include "RTSOptionsPage.h"
#include "Wizard.h"
#include "ModalConfig.h"
#include "ExcludePatternsDlg.h"
#include "LDVPActions.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CRTSOptionsPage property page

IMPLEMENT_DYNCREATE(CRTSOptionsPage, CWizardPage)

CRTSOptionsPage::CRTSOptionsPage() : CWizardPage(CRTSOptionsPage::IDD)
{
	//{{AFX_DATA_INIT(CRTSOptionsPage)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	
	m_bEnableLocks = TRUE;
	m_psp.dwFlags |= PSP_HASHELP;
	m_bInitialized = FALSE;
	//Set the Help Context ID
	SetHelpID( IDD );
}

CRTSOptionsPage::~CRTSOptionsPage()
{
}

void CRTSOptionsPage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CRTSOptionsPage)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CRTSOptionsPage, CPropertyPage)
	//{{AFX_MSG_MAP(CRTSOptionsPage)
	ON_WM_PAINT()
	ON_BN_CLICKED(IDC_BROWSE_FILE_TYPES, OnBrowseFileTypes)
	ON_BN_CLICKED(IDC_ENABLE_RTS, OnEnableRts)
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
// CRTSOptionsPage message handlers

BOOL CRTSOptionsPage::OnSetActive() 
{
	CWizardPage::OnSetActive();
	
	CClientPropSheet	*ptrParent = (CClientPropSheet*)GetParent();
	
	if( m_bInitialized )
	{
		return TRUE;
	}

	//Set the Context-sensitive ID
	SetHelpID( IDD_RTS_OPTIONS );

	//Load the options
	if( InWizardMode() )
	{
		ptrParent->HideBack();
		ptrParent->m_strTitle.LoadString( IDS_CONFIG_RTS );
		ptrParent->SetFinishText( IDS_CLOSE );
		ptrParent->SetWizardButtons( PSWIZB_FINISH );
	}

	//Now open the scan options
	ptrParent->OpenScan( FALSE );
	//And update the interface
	SetDialogOptions();
	UpdateInterface();
	SetLocks();
	OnEnableRts();

	m_bInitialized = TRUE;

	return TRUE;
}

void CRTSOptionsPage::OnDestroying()
{
	GetDialogOptions();
}

void CRTSOptionsPage::SetLocks()
{
	UINT		dwID;
	int	  		iLock;
	CConfigObj	config( GetScanConfig() );

	//Set locks on my parent
	CWizardPage::SetLocks();
	
	//Set the locks for this window
	
	//If SCANONACCESS or SCANONMODIFY is locked,
	//	then Enable must also be locked
	dwID = ( config.GetLock( szReg_Val_RTSScanOnOff ) ) ? IDS_LOCKED_OPTION : ID_NO_LOCK_TIP;

	iLock = ( ( dwID == IDS_LOCKED_OPTION )  ||
			  ( config.GetLock( szReg_Val_ScanOnAccess2 ) ) ||
			  ( config.GetLock( szReg_Val_ScanOnModify  ) ) ) ? 1 : 0;

    m_EnabledLock.Create( WS_VISIBLE | WS_CHILD, IDC_ENABLE_RTS, this, IDC_ENABLED_LOCK, dwID );
	m_EnabledLock.SetLockStyle( 0, LOCK_STYLE_USER );
    m_EnabledLock.Lock( iLock );

	//When To Scan
	dwID = ( config.GetLock( szReg_Val_ScanOnAccess2 ) ) ? IDS_LOCKED_OPTION : ID_NO_LOCK_TIP;
	iLock = ( ( dwID == IDS_LOCKED_OPTION ) || config.GetLock( szReg_Val_ScanOnModify ) ) ? 1 : 0;

    m_WhenToScanLock.Create( WS_VISIBLE | WS_CHILD, IDC_WHEN_TO_SCAN, this, IDC_WHEN_TO_SCAN_LOCK, dwID  );
	m_WhenToScanLock.SetLockStyle( 0, LOCK_STYLE_USER );
    m_WhenToScanLock.Lock( iLock );

	//Scan CD
	dwID =  ( config.GetLock( szReg_Val_ScanCDRom ) ) ? IDS_LOCKED_OPTION : ID_NO_LOCK_TIP;
	iLock = (int)( dwID == IDS_LOCKED_OPTION );

    m_ScanCDLock.Create( WS_VISIBLE | WS_CHILD, IDC_SCAN_CDROM, this, IDC_SCANCD_LOCK, dwID  );
	m_ScanCDLock.SetLockStyle( 0, LOCK_STYLE_USER );
    m_ScanCDLock.Lock( iLock );

	//Scan Floppy
	dwID =  ( config.GetLock( szReg_Val_ScanFloppy ) ) ? IDS_LOCKED_OPTION : ID_NO_LOCK_TIP;
	iLock = (int)( dwID == IDS_LOCKED_OPTION );

    m_ScanFloppyLock.Create( WS_VISIBLE | WS_CHILD, IDC_SCAN_FLOPPY, this, IDC_SCANFLOPPY_LOCK, dwID  );
	m_ScanFloppyLock.SetLockStyle( 0, LOCK_STYLE_USER );
    m_ScanFloppyLock.Lock( iLock );
	
	//Deny Access
	dwID =  ( config.GetLock( szReg_Val_DenyAccess ) ) ? IDS_LOCKED_OPTION : ID_NO_LOCK_TIP;
	iLock = (int)( dwID == IDS_LOCKED_OPTION );

    m_DenyAccessLock.Create( WS_VISIBLE | WS_CHILD, IDC_DENY_ACCESS, this, IDC_DENY_ACCESS_LOCK, dwID  );
	m_DenyAccessLock.SetLockStyle( 0, LOCK_STYLE_USER );
    m_DenyAccessLock.Lock( iLock );
}


LRESULT CRTSOptionsPage::OnWizardNext() 
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

void CRTSOptionsPage::OnPaint() 
{
	CPaintDC dc(this); // device context for painting

	PaintTitle( &dc );
}


void CRTSOptionsPage::OnBrowseFileTypes() 
{
	GetExtensions();
}

void CRTSOptionsPage::OnEnableRts() 
{
	CButton			*ptrEnableRTS = (CButton*)GetDlgItem( IDC_ENABLE_RTS );
	CLDVPActions	*pActions = (CLDVPActions*)GetDlgItem( IDC_ACTIONS );

	static int	iControls[] ={	IDC_FILE_TYPE_LOCK,
								IDC_COMPRESSED_LOCK,
								IDC_SCANFLOPPY_LOCK,
								IDC_SCANCD_LOCK,
								IDC_EXCLUDE_DIRS_LOCK,
								IDC_WHEN_TO_SCAN_LOCK,
								IDC_EXCLUDE_SIGS_LOCK,
								IDC_CHECKSUM_LOCK,
								IDC_DENY_ACCESS_LOCK };


	//The main check box should always be enabled
	m_EnabledLock.EnableBuddy( TRUE );

	EnableAutoscan( ptrEnableRTS, pActions, iControls, 13 );
}

BOOL CRTSOptionsPage::OnWizardFinish() 
{
	//Fire an event telling the App to close the task
	((CLDVPTaskFSApp*)AfxGetApp())->m_ptrCtl->NotifyEndTask();

	return CPropertyPage::OnWizardFinish();
}

void CRTSOptionsPage::SetDialogOptions()
{
	CButton		*ptrButton;
	CString		strText;
	CComboBox	*pScanWhen;
	CConfigObj	config( GetScanConfig() );

	//Set the proper items in the interface
	//First, call the base class method to update the
	//	interface for the common items
	CWizardPage::SetDialogOptions( config );

	//Now add my own stuff
	// Enable
	if( ptrButton = (CButton*)GetDlgItem( IDC_ENABLE_RTS ) )
	{
		ptrButton->SetCheck( config.GetOption( szReg_Val_RTSScanOnOff, TRUE ) );
	}

	//Fill the Scan When combo box
	if( pScanWhen = (CComboBox*)GetDlgItem( IDC_WHEN_TO_SCAN ) )
	{
		//Both
		strText.LoadString( IDS_BOTH );
		pScanWhen->AddString( strText );
		//Accessed
		strText.LoadString( IDS_ACCESSED );
		pScanWhen->AddString( strText );
		//Modified
		strText.LoadString( IDS_MODIFIED );
		pScanWhen->AddString( strText );

		// and select the appropriate item in the box
		if( config.GetOption( szReg_Val_ScanOnAccess2, TRUE ) && config.GetOption( szReg_Val_ScanOnModify, TRUE ) )
		{
			//Select Both
			pScanWhen->SetCurSel( INDEX_BOTH );
		}
		else if( config.GetOption( szReg_Val_ScanOnModify, TRUE ) )
		{
			//Select Modified
			pScanWhen->SetCurSel( INDEX_MODIFIED );
		}
		else
		{
			//Select Accessed
			pScanWhen->SetCurSel( INDEX_ACCESSED );
		}
	}

	// ScanCDRom
	if( ptrButton = (CButton*)GetDlgItem( IDC_SCAN_CDROM ) )
	{
		ptrButton->SetCheck( config.GetOption( szReg_Val_ScanCDRom, TRUE) );
	}

	// ScanFloppy
	if( ptrButton = (CButton*)GetDlgItem( IDC_SCAN_FLOPPY ) )
	{
		ptrButton->SetCheck( config.GetOption( szReg_Val_ScanFloppy, TRUE ) );
	}

	// DenyAccess
	if( ptrButton = (CButton*)GetDlgItem( IDC_DENY_ACCESS ) )
	{
		ptrButton->SetCheck( config.GetOption( szReg_Val_DenyAccess, TRUE ) );
	}
}

void CRTSOptionsPage::GetDialogOptions()
{
	CButton		*ptrButton;
	CComboBox	*pScanWhen;
	CConfigObj	config( GetScanConfig() );

	//Set the proper items in the interface
	//First, call the base class method to update the
	//	interface for the common items
	CWizardPage::GetDialogOptions( config );

	//Now add my own stuff
	// Enable
	if( ptrButton = (CButton*)GetDlgItem( IDC_ENABLE_RTS ) )
	{
		config.SetOption( szReg_Val_RTSScanOnOff, (BOOL)ptrButton->GetCheck() );
	}

	if( pScanWhen = (CComboBox*)GetDlgItem( IDC_WHEN_TO_SCAN ) )
	{
		int iIndex = pScanWhen->GetCurSel();

		//Now, set the Accessed and Modified flags
		config.SetOption( szReg_Val_ScanOnAccess2, ( (iIndex == INDEX_ACCESSED ) || ( iIndex == INDEX_BOTH ) ) );
		config.SetOption( szReg_Val_ScanOnModify,  ( (iIndex == INDEX_MODIFIED ) || ( iIndex == INDEX_BOTH ) ) );
	}
		
	// ScanCDRom
	if( ptrButton = (CButton*)GetDlgItem( IDC_SCAN_CDROM ) )
	{
		config.SetOption( szReg_Val_ScanCDRom, (BOOL)ptrButton->GetCheck() );
	}

	// ScanFloppy
	if( ptrButton = (CButton*)GetDlgItem( IDC_SCAN_FLOPPY ) )
	{
		config.SetOption( szReg_Val_ScanFloppy, (BOOL)ptrButton->GetCheck() );
	}

	// DenyAccess
	if( ptrButton = (CButton*)GetDlgItem( IDC_DENY_ACCESS ) )
	{
		config.SetOption( szReg_Val_DenyAccess, (BOOL)ptrButton->GetCheck() );
	}
}

void CRTSOptionsPage::OnMessage() 
{
	CButton	*ptrButton = (CButton*)GetDlgItem( IDC_MESSAGE_BOX );
	CString	strTemp;
	CConfigObj	config( GetScanConfig() );

	GetMessage();

	strTemp = config.GetOption( szReg_Val_Message, "" );

	if( !strTemp.IsEmpty() )
	{
		ptrButton->SetCheck( 1 );
	}
	else
	{
		ptrButton->SetCheck( 0 );
	}
}

void CRTSOptionsPage::OnBrowseExcludeFiles() 
{
	GetExcludeFiles();
}

void CRTSOptionsPage::OnOK() 
{
	//When modal, we get an OnOK. This is where
	//	we need to save out the options.
	GetDialogOptions();

	CPropertyPage::OnOK();
}

void CRTSOptionsPage::OnTypesAll() 
{
	UpdateInterface();
}

void CRTSOptionsPage::OnTypesSelected() 
{
	UpdateInterface();
}

void CRTSOptionsPage::OnExcludeFiles() 
{
	UpdateInterface();
}

void CRTSOptionsPage::OnMessageBox() 
{
	UpdateInterface();	
}
