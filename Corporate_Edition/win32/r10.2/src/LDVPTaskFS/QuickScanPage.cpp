// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright (c) 2005 Symantec Corporation. All rights reserved.
//----------------------------------------------------------------
//  File: QuickScanPage.cpp
//  Purpose: CQuickScanPage Implementation file. Implements the 
//			QuickScan feature, which does a generic loadpoint scanning.
//
//	Revisions: 
//	Srikanth Vudathala	- Class created	- 10\10\2004
//----------------------------------------------------------------

#include "stdafx.h"
#include "LDVPTaskFS.h"
#include "QuickScanPage.h"
#include "Wizard.H"

#include "ScanOptionsDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CQuickScanPage property page

IMPLEMENT_DYNCREATE(CQuickScanPage, CWizardPage)


BEGIN_EVENTSINK_MAP(CQuickScanPage, CPropertyPage)
    //{{AFX_EVENTSINK_MAP(CQuickScanPage)
	//}}AFX_EVENTSINK_MAP
END_EVENTSINK_MAP()

//----------------------------------------------------------------
// Constructor
//----------------------------------------------------------------
CQuickScanPage::CQuickScanPage() : CWizardPage(CQuickScanPage::IDD)
{
	//{{AFX_DATA_INIT(CQuickScanPage)
		//NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT

	m_psp.dwFlags |= PSP_HASHELP;
	//Set the Help Context ID
	SetHelpID(IDD);
	m_bInitialized = FALSE;
}

//----------------------------------------------------------------
// Destructor
//----------------------------------------------------------------
CQuickScanPage::~CQuickScanPage()
{
}

//----------------------------------------------------------------
// DoDataExchange
//----------------------------------------------------------------
void CQuickScanPage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CQuickScanPage)
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CQuickScanPage, CPropertyPage)
	//{{AFX_MSG_MAP(CQuickScanPage)
	ON_WM_PAINT()
	ON_BN_CLICKED(IDC_OPTIONS, OnOptions)
	ON_WM_LBUTTONDOWN()
	ON_WM_RBUTTONDOWN()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

//----------------------------------------------------------------
// OnPaint
//----------------------------------------------------------------
BOOL CQuickScanPage::OnSetActive() 
{
	CClientPropSheet	*ptrWizParent = (CClientPropSheet*)GetParent();

	CWizardPage::OnSetActive();
	
	if( InWizardMode() )
	{
		if( ptrWizParent->m_ptrCtrl->Recording() ) 
		{
			ptrWizParent->SetFinishText(IDS_SAVE);
			ptrWizParent->SetWizardButtons(PSWIZB_BACK | PSWIZB_FINISH);
			ptrWizParent->ShowBack();
		}
		else
		{
			// terrym 8-28-00 have to parse the raw resouce string now since
			// both the menu and resource string are enbedded
			CString sMenu,sTaskpad,sRaw;

			sRaw.LoadString(IDS_QUICKSCAN);
			ConvertRawMenuString(sRaw,sMenu,sTaskpad);
		
			ptrWizParent->HideBack();
			ptrWizParent->SetFinishText(IDS_SCAN);
			ptrWizParent->SetWizardButtons(PSWIZB_FINISH);

			ptrWizParent->m_strTitle=sTaskpad;
		}
	}

	return TRUE;
}

//----------------------------------------------------------------
// OnWizardFinish
//----------------------------------------------------------------
BOOL CQuickScanPage::OnWizardFinish() 
{
	CClientPropSheet	*ptrParent = (CClientPropSheet*)GetParent();

    StoreOptions();
	
	//Go to the next page and start the scan
	((CWizard*)ptrParent)->Scan();

	return FALSE;
}

//----------------------------------------------------------------
// SizePage
//----------------------------------------------------------------
void CQuickScanPage::SizePage(int cx, int cy)
{
	//First, call tha base class version of this virtual method
	CWizardPage::SizePage(cx, cy);

	CRect		buttonRect, staticRect,	dlgRect;

	//Figure out the current client rect
	GetClientRect(&dlgRect);

	//Size the controls
	CWnd *ptrWnd = GetDlgItem(IDC_OPTIONS);
	if(ptrWnd)
	{
		ptrWnd->GetWindowRect(&buttonRect);
		ScreenToClient(&buttonRect);
		ptrWnd->MoveWindow( dlgRect.right - (buttonRect.right - buttonRect.left) - 12, dlgRect.bottom - (buttonRect.bottom - buttonRect.top), buttonRect.right - buttonRect.left, buttonRect.bottom - buttonRect.top );
	}

	ptrWnd = GetDlgItem(IDC_STATIC);
	if(ptrWnd)
	{
		ptrWnd->GetWindowRect(&staticRect);
		ScreenToClient(&staticRect);
		ptrWnd->MoveWindow( staticRect.left, staticRect.top, dlgRect.right - staticRect.left - 12, staticRect.bottom - staticRect.top );
	}
}

//----------------------------------------------------------------
// OnPaint
//----------------------------------------------------------------
void CQuickScanPage::OnPaint() 
{
	CPaintDC dc(this); 
	
	if( InWizardMode() )
		PaintTitle(&dc);
}

//----------------------------------------------------------------
// OnInitDialog
//----------------------------------------------------------------
BOOL CQuickScanPage::OnInitDialog() 
{
	BOOL	bOptionScanMemory = FALSE,
			bOptionScanLoadPoints = FALSE,
			bOptionScanEraserDefs = FALSE;

	CClientPropSheet *ptrWizParent = (CClientPropSheet*)GetParent();
	
	CWizardPage::OnInitDialog();

	//Set the Context-sensitive ID
	SetHelpID(IDD_QUICK_SCAN);

	//Set the quick scan description
	CString szScanDescription;
	szScanDescription.LoadString(IDS_QUICKSCAN_DESCRIPTION);
	((CWnd*)GetDlgItem(IDC_STATIC))->SetWindowText(szScanDescription);

	//If not initialized, load the default scan options
	if(!m_bInitialized)
	{
		ptrWizParent->OpenScan(TRUE, szReg_Key_Scan_Defaults);
		m_bInitialized = TRUE;
	}

	// Load the quickscan specific options
	CConfigObj	quickScanConfig( GetScanConfig() );


	//Srikanth 3/22/05 Defect 1-3SK1IL, 1-3R1O01
	//Get the scan options for memory, loadpoint and eraser definition scanning.
	//Honor user configured changes for only the selectable options.
	bOptionScanMemory		= quickScanConfig.GetOption(szReg_Val_ScanProcesses, REG_DEFAULT_ScanProcesses_QuickScan);
	bOptionScanLoadPoints	= quickScanConfig.GetOption(szReg_Val_ScanLoadpoints, REG_DEFAULT_ScanLoadpoints_QuickScan);
	bOptionScanEraserDefs	= quickScanConfig.GetOption(szReg_Val_ScanERASERDefs, REG_DEFAULT_ScanERASERDefs_QuickScan);

    //Set fixed scan optimization values for unselectable options.
	//If in future, the commented options are selectable, uncomment the checks and they will get the user configured values.
	//if(bOptionScanMemory)
		quickScanConfig.SetOption(szReg_Val_ScanProcesses,  REG_DEFAULT_ScanProcesses_QuickScan);
	//if(bOptionScanLoadPoints)
		quickScanConfig.SetOption(szReg_Val_ScanLoadpoints, REG_DEFAULT_ScanLoadpoints_QuickScan);
	if(bOptionScanEraserDefs)
		quickScanConfig.SetOption(szReg_Val_ScanERASERDefs, REG_DEFAULT_ScanERASERDefs_QuickScan);

    // Force settings for the files that we scan.
	quickScanConfig.SetOption(szReg_Val_ScanAllDrives,  0); // don't scan the whole system

	// Set the selected scan type
	quickScanConfig.SetOption(szReg_Val_SelectedScanType, SCAN_TYPE_QUICK);

	//Finally, if we are a lite client, hide the options button
	if( GetClientType() == CLIENT_TYPE_LIGHT )
	{
		CWnd *ptrWnd = GetDlgItem( IDC_OPTIONS );
		if(ptrWnd)
			ptrWnd->ShowWindow(SW_HIDE);
	}

	return TRUE;
}

//----------------------------------------------------------------
// OnOptions
//----------------------------------------------------------------
void CQuickScanPage::OnOptions() 
{
	CScanOptionsDlg	dlg( GetScanConfig(), this );
	
	dlg.DoModal();
	// Retain the focus after we close the options dlg.
	CWnd* pWnd = GetDlgItem(IDC_OPTIONS);

	if(pWnd)
		pWnd->SetFocus();
}

//----------------------------------------------------------------
// OnOK
//----------------------------------------------------------------
void CQuickScanPage::OnOK() 
{
	StoreOptions();
	
	CPropertyPage::OnOK();
}

//----------------------------------------------------------------
// OnLButtonDown
//----------------------------------------------------------------
void CQuickScanPage::OnLButtonDown(UINT nFlags, CPoint point) 
{
	CPropertyPage::OnLButtonDown(nFlags, point);

	SetFocus();
}

//----------------------------------------------------------------
// OnRButtonDown
//----------------------------------------------------------------
void CQuickScanPage::OnRButtonDown(UINT nFlags, CPoint point) 
{
	CPropertyPage::OnRButtonDown(nFlags, point);

	SetFocus();
}

//----------------------------------------------------------------
// StoreOptions
//----------------------------------------------------------------
void CQuickScanPage::StoreOptions()
{
	CClientPropSheet *ptrParent = (CClientPropSheet*)GetParent();
	CConfigObj config( GetScanConfig() );
	time_t tNow = time(NULL);

	//Reset the created time and last start time for Missed events.
	config.SetSubOption(szReg_Key_ScheduleKey);
	config.SetOption(szReg_Val_Schedule_Created, (DWORD)tNow);
	config.SetOption(szReg_Val_Schedule_LastStart, (DWORD)0);
	config.SetOption(szReg_Val_Schedule_SkipEvent, (DWORD)0);
	config.SetSubOption("");
}
