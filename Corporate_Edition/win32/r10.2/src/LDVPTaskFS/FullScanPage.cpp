// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright (c) 2005 Symantec Corporation. All rights reserved.
//----------------------------------------------------------------
//  File: FullScanPage.cpp
//  Purpose: CFullScanPage Implementation file. Implements the 
//			Full Scan feature, which does a complete scan on the
//			system. 
//			It includes 
//				all the files and folders, 
//				generic loadpoints and extended eraser definitions
//
//	Revisions: 
//	Srikanth Vudathala	- Class created	- 10\10\2004
//----------------------------------------------------------------

#include "stdafx.h"
#include "LDVPTaskFS.h"
#include "FullScanPage.h"
#include "Wizard.H"

#include "ScanOptionsDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
//CFullScanPage property page

IMPLEMENT_DYNCREATE(CFullScanPage, CWizardPage)


BEGIN_EVENTSINK_MAP(CFullScanPage, CPropertyPage)
    //{{AFX_EVENTSINK_MAP(CFullScanPage)
	//}}AFX_EVENTSINK_MAP
END_EVENTSINK_MAP()

//----------------------------------------------------------------
// Constructor
//----------------------------------------------------------------
CFullScanPage::CFullScanPage() : CWizardPage(CFullScanPage::IDD)
{
	//{{AFX_DATA_INIT(CFullScanPage)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT

	m_psp.dwFlags |= PSP_HASHELP;
	//Set the Help Context ID
	SetHelpID(IDD);
	m_bInitialized = FALSE;
}

//----------------------------------------------------------------
// Destructor
//----------------------------------------------------------------
CFullScanPage::~CFullScanPage()
{
}

//----------------------------------------------------------------
// DoDataExchange
//----------------------------------------------------------------
void CFullScanPage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CFullScanPage)
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CFullScanPage, CPropertyPage)
	//{{AFX_MSG_MAP(CFullScanPage)
	ON_WM_PAINT()
	ON_BN_CLICKED(IDC_OPTIONS, OnOptions)
	ON_WM_LBUTTONDOWN()
	ON_WM_RBUTTONDOWN()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

//----------------------------------------------------------------
// OnPaint
//----------------------------------------------------------------
BOOL CFullScanPage::OnSetActive() 
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

			sRaw.LoadString(IDS_FULL_SCAN);
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
BOOL CFullScanPage::OnWizardFinish() 
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
void CFullScanPage::SizePage(int cx, int cy)
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
		ptrWnd->GetWindowRect(&buttonRect );
		ScreenToClient(&buttonRect );
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
void CFullScanPage::OnPaint() 
{
	CPaintDC dc(this); 
	
	if( InWizardMode() )
		PaintTitle(&dc);
}

//----------------------------------------------------------------
// OnInitDialog
//----------------------------------------------------------------
BOOL CFullScanPage::OnInitDialog() 
{
	BOOL	bOptionScanMemory = FALSE,
			bOptionScanLoadPoints = FALSE,
			bOptionScanEraserDefs = FALSE;

	CClientPropSheet *ptrWizParent = (CClientPropSheet*)GetParent();
	
	CWizardPage::OnInitDialog();

	//Set the Context-sensitive ID
	SetHelpID(IDD_FULL_SCAN);

	//Set the full scan description
	CString szScanDescription;
	szScanDescription.LoadString(IDS_FULL_SCAN_DESCRIPTION);
	((CWnd*)GetDlgItem(IDC_STATIC))->SetWindowText(szScanDescription);

	//If not initialized, load the default scan options
	if(!m_bInitialized)
	{
		ptrWizParent->OpenScan(TRUE, szReg_Key_Scan_Defaults);
		m_bInitialized = TRUE;
	}

	//Load the full scan specific options
	CConfigObj	fullScanConfig( GetScanConfig() );

	//Srikanth 3/22/05 Defect 1-3SK1IL, 1-3R1O01
	//Get the scan options for memory, loadpoint and eraser definition scanning.
	//Honor user configured changes for only the selectable options.
	bOptionScanMemory		= fullScanConfig.GetOption(szReg_Val_ScanProcesses, REG_DEFAULT_ScanProcesses_FullScan);
	bOptionScanLoadPoints	= fullScanConfig.GetOption(szReg_Val_ScanLoadpoints, REG_DEFAULT_ScanLoadpoints_FullScan);
	bOptionScanEraserDefs	= fullScanConfig.GetOption(szReg_Val_ScanERASERDefs, REG_DEFAULT_ScanERASERDefs_FullScan);

    //Set fixed scan optimization values for unselectable options.
	//If in future, the commented options are selectable, uncomment the checks and they will get the user configured values.
	//if(bOptionScanMemory)
		fullScanConfig.SetOption(szReg_Val_ScanProcesses, REG_DEFAULT_ScanProcesses_FullScan);
	//if(bOptionScanLoadPoints)
		fullScanConfig.SetOption(szReg_Val_ScanLoadpoints, REG_DEFAULT_ScanLoadpoints_FullScan);
	//if(bOptionScanEraserDefs)
		fullScanConfig.SetOption(szReg_Val_ScanERASERDefs, REG_DEFAULT_ScanERASERDefs_FullScan);

    //Force settings for the files that we scan.
	fullScanConfig.SetOption(szReg_Val_ScanAllDrives,  1); // scan the whole system

	//Set the selected scan type
	fullScanConfig.SetOption(szReg_Val_SelectedScanType, SCAN_TYPE_FULL);

	//Finally, if we are a lite client, hide the options button
	if( GetClientType() == CLIENT_TYPE_LIGHT )
	{
		CWnd *ptrWnd = GetDlgItem(IDC_OPTIONS);
		if(ptrWnd)
			ptrWnd->ShowWindow(SW_HIDE);
	}

	return TRUE;
}

//----------------------------------------------------------------
// OnOptions
//----------------------------------------------------------------
void CFullScanPage::OnOptions() 
{
	CScanOptionsDlg	dlg(GetScanConfig(), this);
	
	dlg.DoModal();
	// Retain the focus after we close the options dlg.
	CWnd* pWnd = GetDlgItem(IDC_OPTIONS);

	if(pWnd)
		pWnd->SetFocus();
}

//----------------------------------------------------------------
// OnOk
//----------------------------------------------------------------
void CFullScanPage::OnOK() 
{
	StoreOptions();
	
	CPropertyPage::OnOK();
}


//----------------------------------------------------------------
// OnLButtonDown
//----------------------------------------------------------------
void CFullScanPage::OnLButtonDown(UINT nFlags, CPoint point) 
{
	CPropertyPage::OnLButtonDown(nFlags, point);

	SetFocus();
}

//----------------------------------------------------------------
// OnRButtonDown
//----------------------------------------------------------------
void CFullScanPage::OnRButtonDown(UINT nFlags, CPoint point) 
{
	CPropertyPage::OnRButtonDown(nFlags, point);

	SetFocus();
}

//----------------------------------------------------------------
// StoreOptions
//----------------------------------------------------------------
void CFullScanPage::StoreOptions()
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
