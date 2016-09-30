// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright (c) 2005 Symantec Corporation. All rights reserved.
// ScanOptionsDlg.cpp : implementation file
//

#include "stdafx.h"
#include "ldvptaskfs.h"
#include "ScanOptionsDlg.h"
#include "vphtmlhelp.h"	//HTMLHelp support for SAV help

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CScanOptionsDlg dialog

CScanOptionsDlg::CScanOptionsDlg(IConfig *ptrConfig, CWnd* pParent /*=NULL*/)
	: CDialog(CScanOptionsDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CScanOptionsDlg)
	//}}AFX_DATA_INIT

	if( ptrConfig )
		ptrConfig->AddRef();

	m_ptrConfig = ptrConfig;
}

CScanOptionsDlg::~CScanOptionsDlg()
{
	if( m_ptrConfig )
		m_ptrConfig->Release();
}

void CScanOptionsDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CScanOptionsDlg)
	DDX_Control(pDX, IDC_UI, m_ctlUI);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CScanOptionsDlg, CDialog)
	//{{AFX_MSG_MAP(CScanOptionsDlg)
	ON_BN_CLICKED(IDC_MAKE_DEFAULT, OnMakeDefault)
	//}}AFX_MSG_MAP
    ON_BN_CLICKED(IDHELP, OnHelp)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CScanOptionsDlg message handlers

BOOL CScanOptionsDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();

    //Set the Context-sensitive ID
    SetHelpID( IDD );

	if( m_ptrConfig )
	{
		m_ctlUI.SetConfigInterface( m_ptrConfig );
		m_ctlUI.Load();
	}

	SetWindowLong( m_ctlUI.m_hWnd, GWL_EXSTYLE, GetWindowLong( m_ctlUI.m_hWnd, GWL_EXSTYLE ) | WS_EX_CONTROLPARENT );
	
	return TRUE;
}

void CScanOptionsDlg::OnOK() 
{
	UpdateData( TRUE );

	m_ctlUI.Store();
	
	CDialog::OnOK();
}

void CScanOptionsDlg::OnCancel() 
{
	CDialog::OnCancel();
}

void CScanOptionsDlg::OnMakeDefault() 
{
    UpdateData( TRUE );

    IScanConfig *ptrDefaultSettings;

    //Query for the IScanConfig interface
    if( SUCCEEDED( CoCreateLDVPObject( CLSID_CliProxy, IID_IScanConfig, (void**)&ptrDefaultSettings ) ) )
    {
        ptrDefaultSettings->Open( NULL, HKEY_VP_USER_SCANS, szReg_Key_Scan_Defaults );
        ptrDefaultSettings->Reset();

        //First, store out the settings
        m_ctlUI.Store();

        //Now copy them
        ptrDefaultSettings->CopyFromInterface( m_ptrConfig );

        // Remove the directories if they exist
        ptrDefaultSettings->SetSubOption(szReg_Key_Directories);
        ptrDefaultSettings->ClearOptions();

        // Remove the files if they exist
        ptrDefaultSettings->SetSubOption(szReg_Key_Files);
        ptrDefaultSettings->ClearOptions();

        // Remove the schedule information if it exist.  If we dont
        // remove this information it will make the custom scans created
        // after this become scheduled scans.
        ptrDefaultSettings->SetSubOption(szReg_Key_ScheduleKey);
        ptrDefaultSettings->ClearOptions();

        ptrDefaultSettings->SetSubOption(NULL);

        ptrDefaultSettings->Release();
    }

    m_ptrConfig->SetSubOption(NULL);
}


////////////////////////////////////////////////////////////////////////
//
// function name: CScanOptionsDlg::WinHelpInternal
//
// description: WinHelpInternal override to trap WinHelpInternal calls and redirect 
//				them to HTMLHelp
// return type: none
//
///////////////////////////////////////////////////////////////////////
// 8/1/03 JGEIGER - Inserted this function to implement HTMLHelp for SAV. 
///////////////////////////////////////////////////////////////////////
void CScanOptionsDlg::WinHelpInternal(DWORD dwData, UINT nCmd) 
{
    CString strHelpFile = SAV_HTML_HELP;

	::HtmlHelp( AfxGetMainWnd()->GetSafeHwnd(),
              strHelpFile,
              HH_HELP_CONTEXT,
              dwData );
}



INT_PTR CScanOptionsDlg::DoModal()
{
    //*************************************************************
    // Lock down the callers temp maps to prevent an exception when 
    // combining OLE controls and dialogs. Specifically, 
    // COccManager::IsDialogMessage needs a temp CWnd object to 
    // survive, but  COleControl::OnPaint calls 
    // AfxLockTempMaps/AfxUnlockTempMaps which causes it to be 
    // deleted if AfxLockTempMaps hasn't been called elsewhere.
    //
    AfxLockTempMaps();
    //*************************************************************

    INT_PTR result = CDialog::DoModal();

    //*************************************************************
    AfxUnlockTempMaps();
    //*************************************************************

    return result;
}

