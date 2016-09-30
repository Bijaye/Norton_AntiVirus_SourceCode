// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright (c) 2005 Symantec Corporation. All rights reserved.
//----------------------------------------------------------------
//
//  File: ScanSelectedPage.cpp
//  Purpose: CScanSelectedPage Implementation file
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
#include "ScanSelectedPage.h"
#include "Wizard.H"

#include "ScanOptionsDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CScanSelectedPage property page

IMPLEMENT_DYNCREATE(CScanSelectedPage, CWizardPage)


BEGIN_EVENTSINK_MAP(CScanSelectedPage, CPropertyPage)
    //{{AFX_EVENTSINK_MAP(CScanSelectedPage)
	ON_EVENT(CScanSelectedPage, IDC_DRIVES, 3 /* OnSelect */, OnOnSelectDrives, VTS_I4)
	//}}AFX_EVENTSINK_MAP
END_EVENTSINK_MAP()

//----------------------------------------------------------------
// Constructor
//----------------------------------------------------------------
CScanSelectedPage::CScanSelectedPage() : CWizardPage(CScanSelectedPage::IDD)
{
	//{{AFX_DATA_INIT(CScanSelectedPage)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT

	m_psp.dwFlags |= PSP_HASHELP;
	//Set the Help Context ID
	SetHelpID( IDD );
	m_bInitialized = FALSE;
}

//----------------------------------------------------------------
// Destructor
//----------------------------------------------------------------
CScanSelectedPage::~CScanSelectedPage()
{
}

//----------------------------------------------------------------
// DoDataExchange
//----------------------------------------------------------------
void CScanSelectedPage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CScanSelectedPage)
	DDX_Control(pDX, IDC_DRIVES, m_ShellSel);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CScanSelectedPage, CPropertyPage)
	//{{AFX_MSG_MAP(CScanSelectedPage)
	ON_WM_PAINT()
	ON_BN_CLICKED(IDC_OPTIONS, OnOptions)
	ON_WM_LBUTTONDOWN()
	ON_WM_RBUTTONDOWN()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

//----------------------------------------------------------------
// OnPaint
//----------------------------------------------------------------
BOOL CScanSelectedPage::OnSetActive() 
{
	CClientPropSheet	*ptrWizParent = (CClientPropSheet*)GetParent();
	DWORD				dwFlag = (m_ShellSel.GetNCheckCount() >= 1 ) ? PSWIZB_FINISH : PSWIZB_DISABLEDFINISH ;

	CWizardPage::OnSetActive();
	
	if( InWizardMode() )
	{
		if( ptrWizParent->m_ptrCtrl->Recording() ) 
		{
			ptrWizParent->SetFinishText( IDS_SAVE );
			ptrWizParent->SetWizardButtons( PSWIZB_BACK | dwFlag );
			ptrWizParent->ShowBack();

		}
		else
		{
			// terrym 8-28-00 have to parse the raw resouce string now since
			// both the menu and resource string are enbedded
			CString sMenu,sTaskpad,sRaw;

			sRaw.LoadString( IDS_SCAN_SELECTED  );
			ConvertRawMenuString(sRaw,sMenu,sTaskpad);
		
			ptrWizParent->HideBack();
			ptrWizParent->SetFinishText( IDS_SCAN);
			ptrWizParent->SetWizardButtons( dwFlag );

			ptrWizParent->m_strTitle=sTaskpad;
		}
	}

	return TRUE;
}


//----------------------------------------------------------------
// OnWizardFinish
//----------------------------------------------------------------
BOOL CScanSelectedPage::OnWizardFinish() 
{
	CClientPropSheet	*ptrParent = (CClientPropSheet*)GetParent();

    StoreOptions();
	
	//Go to the next page
	// and start the scan
	((CWizard*)ptrParent)->Scan();

	return FALSE;
}

void CScanSelectedPage::SizePage( int cx, int cy )
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
// OnPaint
//----------------------------------------------------------------
void CScanSelectedPage::OnPaint() 
{
	CPaintDC dc(this); // device context for painting
	
	if( InWizardMode() )
		PaintTitle( &dc );
}

//----------------------------------------------------------------
// OnInitDialog
//----------------------------------------------------------------
BOOL CScanSelectedPage::OnInitDialog() 
{
	CClientPropSheet	*ptrWizParent = (CClientPropSheet*)GetParent();
	
	CWizardPage::OnInitDialog();


	//Set the Context-sensitive ID
	SetHelpID( IDD_SCAN_SELECTED );

	if( !m_bInitialized )
	{
		ptrWizParent->OpenScan( TRUE, szReg_Key_Scan_Defaults );
 
		m_ShellSel.SetBShowCDROM( TRUE );
				
        // STS defect #148115 - we no longer want to show the
        // email folders
        m_ShellSel.SetBShowMail(FALSE);

		//If we are a connected client, then we need to ask for
		//	a password when scanning network drives. If not, we don't
		DWORD dwType = GetClientType();
		m_ShellSel.SetShowPassword(( dwType == CLIENT_TYPE_CONNECTED ) ||
									( dwType == CLIENT_TYPE_SOMETIMES_CONNECTED ) );

		//I need to initialize ShellSel with the storage so I can
		//	get Mail folders if they are snapped in.
		if( IVPStorage *iStorage = GetStorage() )
		{
			m_ShellSel.SetStorage( iStorage );
			iStorage->Release();
		}
		else
			ASSERT(0);

		//Now, load ShellSel with the previous settings
		m_ShellSel.LoadConfig( GetScanConfig(), szReg_Key_Directories, szReg_Key_Files );

		m_ShellSel.SetActivate(1);

	    // Set the selected scan type, to make sure that the proper scan options
        // will be shown.
    	CConfigObj	customScanConfig( GetScanConfig() );
	    customScanConfig.SetOption(szReg_Val_SelectedScanType, SCAN_TYPE_CUSTOM);

		m_bInitialized = TRUE;
	}
	
	//Finally, if we are a client lite, hide the options button
	if( GetClientType() == CLIENT_TYPE_LIGHT )
	{
		CWnd *ptrWnd = GetDlgItem( IDC_OPTIONS );

		if( ptrWnd )
			ptrWnd->ShowWindow( SW_HIDE );
	}

	OnOnSelectDrives(0);

	return TRUE;
}

//----------------------------------------------------------------
// OnSelChangedDrives
//----------------------------------------------------------------
void CScanSelectedPage::OnOnSelectDrives(long itemId) 
{
	DWORD dwFlag;

	//Get a pointer to the wizard
	CClientPropSheet *ptrWizard = (CClientPropSheet*)GetParent();

	UINT id = ( ptrWizard->m_ptrCtrl->Recording() ) ? IDS_SAVE : IDS_SCAN;	

	int iCount = m_ShellSel.GetNCheckCount();

	ptrWizard->SetFinishText( id );

	
	dwFlag = ( ptrWizard->m_ptrCtrl->Recording() ) ? PSWIZB_BACK : 0;

	//If the selection is valid, enable the Finish button
	if( iCount )
	{
		ptrWizard->SetWizardButtons( dwFlag | PSWIZB_FINISH );
	}
	else
	{
		ptrWizard->SetWizardButtons( dwFlag | PSWIZB_DISABLEDFINISH );
	}

	if( dwFlag )
		ptrWizard->ShowBack();
	
}

void CScanSelectedPage::OnOptions() 
{
	CScanOptionsDlg	dlg( GetScanConfig(), this );
	
	dlg.DoModal();
	// Retain the focus after we close the options dlg.
	CWnd* pWnd = GetDlgItem(IDC_OPTIONS);

	if(pWnd)
		pWnd->SetFocus();
}


void CScanSelectedPage::OnOK() 
{
	StoreOptions();
	
	CPropertyPage::OnOK();
}

void CScanSelectedPage::StoreOptions()
{
	CClientPropSheet	*ptrParent = (CClientPropSheet*)GetParent();
	CConfigObj			config( GetScanConfig() );
	time_t tNow = time(NULL);

	//Reset the items in case we got here from a 
	//	"Back" action.
	config.SetOption( szReg_Val_ScanAllDrives, (DWORD)0 );

	//Set the selected scan type
	config.SetOption(szReg_Val_SelectedScanType, SCAN_TYPE_CUSTOM);

	// Reset the created time and last start time for
	// Missed events.
	config.SetSubOption( szReg_Key_ScheduleKey );
	config.SetOption( szReg_Val_Schedule_Created, (DWORD)tNow );
	config.SetOption( szReg_Val_Schedule_LastStart, (DWORD)0 );
	config.SetOption( szReg_Val_Schedule_SkipEvent, (DWORD)0 );
	config.SetSubOption( "" );

	//Tell ShellSel to save out to my IConfig pointer
	// I need to addref it, and ShellSel will release it when it is done
	IConfig *pConfig = config.GetConfig();
	m_ShellSel.SaveConfig( pConfig, szReg_Key_Directories, szReg_Key_Files );
	pConfig->Release();
}

/*------------------------------------------------------------------
*
* $History: $
*
*
  ------------------------------------------------------------------*/


void CScanSelectedPage::OnLButtonDown(UINT nFlags, CPoint point) 
{
	CPropertyPage::OnLButtonDown(nFlags, point);

	SetFocus();
}

void CScanSelectedPage::OnRButtonDown(UINT nFlags, CPoint point) 
{
	CPropertyPage::OnRButtonDown(nFlags, point);

	SetFocus();
}
