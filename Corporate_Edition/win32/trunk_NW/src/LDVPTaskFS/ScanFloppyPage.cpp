// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright (c) 2005 Symantec Corporation. All rights reserved.
//----------------------------------------------------------------
//
//  File: ScanFloppyPage.cpp
//  Purpose: ScanFloppy Implementation file
//
//	Date: 1-20-97
//
//	Owner: Ken Knapton
//
//	***** Intel Corp. Confidential ******
//
//----------------------------------------------------------------

#include "stdafx.h"
#include "LDVPTaskFS.h"
#include "ScanFloppyPage.h"
#include "Wizard.h"

#include "ScanOptionsDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

IMPLEMENT_DYNCREATE(CScanFloppyPage, CWizardPage)


BEGIN_EVENTSINK_MAP(CScanFloppyPage, CWizardPage)
    //{{AFX_EVENTSINK_MAP(CScanFloppyPage)
	ON_EVENT(CScanFloppyPage, IDC_DRIVES, 3 /* OnSelect */, OnOnSelectDrives, VTS_I4)
	//}}AFX_EVENTSINK_MAP
END_EVENTSINK_MAP()

//----------------------------------------------------------------
// Constructor
//----------------------------------------------------------------
CScanFloppyPage::CScanFloppyPage() : CWizardPage(CScanFloppyPage::IDD)
{
	//{{AFX_DATA_INIT(CScanFloppyPage)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	m_psp.dwFlags |= PSP_HASHELP;
	//Set the Help Context ID
	SetHelpID( IDD );
}

//----------------------------------------------------------------
// Destructor
//----------------------------------------------------------------
CScanFloppyPage::~CScanFloppyPage()
{
}

//----------------------------------------------------------------
// DoDataExchange
//----------------------------------------------------------------
void CScanFloppyPage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CScanFloppyPage)
	DDX_Control(pDX, IDC_DRIVES, m_ShellSel);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CScanFloppyPage, CWizardPage)
	//{{AFX_MSG_MAP(CScanFloppyPage)
	ON_WM_PAINT()
	ON_BN_CLICKED(IDC_OPTIONS, OnOptions)
	ON_WM_LBUTTONDOWN()
	ON_WM_RBUTTONDOWN()
	//}}AFX_MSG_MAP
	ON_COMMAND( UWM_DESTROYING, OnDestroying )
END_MESSAGE_MAP()


//----------------------------------------------------------------
// OnPaint
//----------------------------------------------------------------
void CScanFloppyPage::OnPaint() 
{
	CPaintDC dc(this); // device context for painting
	
	PaintTitle( &dc );
	
}

void CScanFloppyPage::SizePage( int cx, int cy )
{
	//First, call tha base class version of this
	//		virtual method
	CWizardPage::SizePage( cx, cy );

	//Now, size the ShellSel control
	CRect		shellSelRect,			
				buttonRect,
				dlgRect;

	//Figure out the current client rect
	GetClientRect( &dlgRect );

	CWnd *ptrWnd = GetDlgItem( IDC_OPTIONS );
	if( ptrWnd )
	{
		ptrWnd->GetWindowRect( &buttonRect );
		ScreenToClient( &buttonRect );
		ptrWnd->MoveWindow( dlgRect.right - ( buttonRect.right - buttonRect.left ) - 12, dlgRect.bottom - ( buttonRect.bottom - buttonRect.top ), buttonRect.right - buttonRect.left, buttonRect.bottom - buttonRect.top );
	}

	m_ShellSel.GetWindowRect( &shellSelRect );
	ScreenToClient( &shellSelRect );
	m_ShellSel.MoveWindow( shellSelRect.left, shellSelRect.top, dlgRect.right - shellSelRect.left - 12, ( buttonRect.top - 5) - shellSelRect.top );
}

//----------------------------------------------------------------
// OnSetActive
//----------------------------------------------------------------
BOOL CScanFloppyPage::OnSetActive() 
{
	CWizardPage::OnSetActive();
	CString sMenu,sTaskpad,sRaw; // terrym 8-28-00 have to parse the raw resouce string now since
								// both the menu and resource string are enbedded
	CClientPropSheet	*ptrWizParent = (CClientPropSheet*)GetParent();
	UINT				iButton = m_bEnableTheScanButton ? PSWIZB_FINISH : PSWIZB_DISABLEDFINISH;
	
	UINT id = ( ptrWizParent->m_ptrCtrl->Recording() ) ? IDS_SAVE : IDS_SCAN;	
	ptrWizParent->HideBack();
	ptrWizParent->SetFinishText( id );
	ptrWizParent->SetWizardButtons( iButton );

	// terrym 8-28-00 have to parse the raw resouce string now since
	// both the menu and resource string are enbedded	
	sRaw.LoadString( IDS_SCAN_FLOPPY );
	ConvertRawMenuString(sRaw,sMenu,sTaskpad);
	ptrWizParent->m_strTitle=sTaskpad;


	return TRUE;
}

void CScanFloppyPage::OnDestroying()
{
}

//----------------------------------------------------------------
// OnInitDialog
//----------------------------------------------------------------
BOOL CScanFloppyPage::OnInitDialog() 
{
	CWizardPage::OnInitDialog();
	CClientPropSheet	*ptrWizParent = (CClientPropSheet*)GetParent();

	//Set up the Context-sensitive help
	SetHelpID( IDD_SCAN_FLOPPY );

	//Get the first floppy drive letter.
	//	It will usually be "A", but not always,
	//	so I need to get it this way.
	DWORD	dwDrives = GetLogicalDrives(),
			dwMask = 1;
	BOOL	bFoundAFloppy = FALSE;
	CString strFirstFloppy;

	while( !bFoundAFloppy && (dwMask < 134217728) )
	{
		if( dwMask & dwDrives )
		{
			strFirstFloppy.Format( "%c:", (dwMask/2) + 'A' );

			if( GetDriveType( (LPCTSTR)strFirstFloppy ) == DRIVE_REMOVABLE )
			{
				bFoundAFloppy = TRUE;
				break;
			}
		}

		dwMask *= 2;
	}

	//I need to open the scan before I try to add the scan item
	ptrWizParent->OpenScan( TRUE, szReg_Key_Scan_Defaults );
	
	CConfigObj	config( GetScanConfig() );

	if( bFoundAFloppy )
	{
		//Set the first floppy drive into the scan options
		config.SetSubOption( szReg_Key_Directories );
		//m_ShellSel.SetCheckedItem( strFirstFloppy, (long)TRUE );
		config.SetOption( strFirstFloppy, (DWORD)1 );
		config.SetSubOption( "" );
	}

	//I need to initialize ShellSel with the storage so I can
	//	get Mail folders if they are snapped in.
	if( IVPStorage *iStorage = GetStorage() )
	{
		m_ShellSel.SetStorage( iStorage );
		iStorage->Release();
	}

	//Now, load ShellSel with the previous settings
	if( IConfig *pConfig = config.GetConfig() )
	{
		m_ShellSel.LoadConfig( pConfig, szReg_Key_Directories, szReg_Key_Files );
		pConfig->Release();
	}

	m_ShellSel.SetActivate(1);

	//Make sure the Finish button is properly enabled/disabled
	OnOnSelectDrives(0);

	//Finally, if we are a client lite, hide the options button
	if( GetClientType() == CLIENT_TYPE_LIGHT )
	{
		CWnd *ptrWnd = GetDlgItem( IDC_OPTIONS );

		if( ptrWnd )
			ptrWnd->ShowWindow( SW_HIDE );
	}

	return TRUE;
}

//----------------------------------------------------------------
// OnSelChangedDrives
//----------------------------------------------------------------

void CScanFloppyPage::OnOnSelectDrives(long itemId) 
{
	//Get a pointer to the wizard
	CClientPropSheet *ptrWizard = (CClientPropSheet*)GetParent();

	UINT id = ( ptrWizard->m_ptrCtrl->Recording() ) ? IDS_SAVE : IDS_SCAN;	

	int iCount = m_ShellSel.GetNCheckCount();

	ptrWizard->SetFinishText( id );

	//If the selection is valid, enable the Finish button
	if( iCount )
	{
		m_bEnableTheScanButton = TRUE;
		ptrWizard->SetWizardButtons( PSWIZB_FINISH );
	}
	else
	{
		m_bEnableTheScanButton = FALSE;
		ptrWizard->SetWizardButtons( PSWIZB_DISABLEDFINISH );
	}
	
}

//----------------------------------------------------------------
// OnWizardFinish
//----------------------------------------------------------------
BOOL CScanFloppyPage::OnWizardFinish()
{
	CClientPropSheet	*ptrParent = (CClientPropSheet*)GetParent();
	CConfigObj			config( GetScanConfig() );

	config.SetOption( szReg_Val_ScanAllDrives, (DWORD)0 );

	//Tell ShellSel to save out to my IConfig pointer
	// I need to addref it, and ShellSel will release it when it is done
	IConfig *pConfig = config.GetConfig();
	m_ShellSel.SaveConfig( pConfig, szReg_Key_Directories, szReg_Key_Files );
	pConfig->Release();
	
	//Go to the next page
	// and start the scan
	((CWizard*)ptrParent)->Scan();

	return FALSE;
}

void CScanFloppyPage::OnOptions() 
{
	CScanOptionsDlg	dlg( GetScanConfig(), this );
	
	dlg.DoModal();

	CWnd* pWnd = GetDlgItem(IDC_OPTIONS);

	if(pWnd)
		pWnd->SetFocus();
}

/*------------------------------------------------------------------
*
* $History: $
*
*
  ------------------------------------------------------------------*/

void CScanFloppyPage::OnLButtonDown(UINT nFlags, CPoint point) 
{
	CWizardPage::OnLButtonDown(nFlags, point);

	SetFocus();
}

void CScanFloppyPage::OnRButtonDown(UINT nFlags, CPoint point) 
{
	CWizardPage::OnRButtonDown(nFlags, point);

	SetFocus();
}
