////////////////////////
//
// PROPRIETARY / CONFIDENTIAL.
// Use of this product is subject to license terms.
// Copyright © 2006 Symantec Corporation.
// All rights reserved.
//
/////////////////////////////////////////////////////////////////////////////////////////////////

// MachineInfoDlg.cpp : implementation file
//

#include "stdafx.h"
#include "MachineInfo.h"
#include "MachineInfoDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CMachineInfoDlg dialog

CMachineInfoDlg::CMachineInfoDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CMachineInfoDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CMachineInfoDlg)
	m_strSavePath = _T("");
	m_bActivityLog = FALSE;
	m_bDebugLogs = FALSE;
	m_bNAVFileVersions = FALSE;
	m_bSystemInfo = FALSE;
	m_bSymRegKeys = FALSE;
	m_bLiveUpdateLogs = FALSE;
	m_bNAVOptions = FALSE;
	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CMachineInfoDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CMachineInfoDlg)
	DDX_Control(pDX, IDC_NAVFILEVERSIONS, m_btnNAVFileVersions);
	DDX_Control(pDX, IDC_NAVOPTIONSFILES, m_btnNAVOptions);
	DDX_Control(pDX, IDC_LIVEUPDATELOGS, m_btnLiveUpdateLogs);
	DDX_Control(pDX, IDC_DEBUGLOGS, m_btnDebugLogs);
	DDX_Control(pDX, IDC_NAVREGKEYS, m_btnSymRegKeys);
	DDX_Control(pDX, IDC_ACTIVITYLOG, m_btnActivityLog);
	DDX_Control(pDX, ID_START, m_btnStart);
	DDX_Control(pDX, IDC_SYSINFO, m_btnSystemInfo);
	DDX_Text(pDX, IDC_SAVEPATH, m_strSavePath);
	DDX_Check(pDX, IDC_ACTIVITYLOG, m_bActivityLog);
	DDX_Check(pDX, IDC_DEBUGLOGS, m_bDebugLogs);
	DDX_Check(pDX, IDC_NAVFILEVERSIONS, m_bNAVFileVersions);
	DDX_Check(pDX, IDC_SYSINFO, m_bSystemInfo);
	DDX_Check(pDX, IDC_NAVREGKEYS, m_bSymRegKeys);
	DDX_Check(pDX, IDC_LIVEUPDATELOGS, m_bLiveUpdateLogs);
	DDX_Check(pDX, IDC_NAVOPTIONSFILES, m_bNAVOptions);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CMachineInfoDlg, CDialog)
	//{{AFX_MSG_MAP(CMachineInfoDlg)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(ID_START, OnStart)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMachineInfoDlg message handlers

BOOL CMachineInfoDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon
	

    m_strSavePath = "C:\\";

    if ( m_MSSystemInfo.IsAvailable () )
        m_bSystemInfo = TRUE;
    else
    {
        m_bSystemInfo = FALSE;
        m_btnSystemInfo.EnableWindow ( FALSE );
    }

    if ( m_ActivityLog.IsAvailable () )
        m_bActivityLog = TRUE;
    else
    {
        m_bActivityLog = FALSE;
        m_btnActivityLog.EnableWindow ( FALSE );
    }

    if ( m_SymRegKeys.IsAvailable () )
        m_bSymRegKeys = TRUE;
    else
    {
        m_bSymRegKeys = FALSE;
        m_btnSymRegKeys.EnableWindow ( FALSE );
    }

    if ( m_DebugLogs.IsAvailable () )
        m_bDebugLogs = TRUE;
    else
    {
        m_bDebugLogs = FALSE;
        m_btnDebugLogs.EnableWindow ( FALSE );
    }

    if ( m_LiveUpdateLogs.IsAvailable () )
        m_bLiveUpdateLogs = TRUE;
    else
    {
        m_bLiveUpdateLogs = FALSE;
        m_btnLiveUpdateLogs.EnableWindow ( FALSE );
    }

    if ( m_NAVOptions.IsAvailable () )
        m_bNAVOptions = TRUE;
    else
    {
        m_bNAVOptions = FALSE;
        m_btnNAVOptions.EnableWindow ( FALSE );
    }

    if ( m_NAVFileVersions.IsAvailable () )
        m_bNAVFileVersions = TRUE;
    else
    {
        m_bNAVFileVersions = FALSE;
        m_btnNAVFileVersions.EnableWindow ( FALSE );
    }

    // Put data into the dialog
    //
    UpdateData ( FALSE );

	return TRUE;  // return TRUE  unless you set the focus to a control
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CMachineInfoDlg::OnPaint() 
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, (WPARAM) dc.GetSafeHdc(), 0);

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

// The system calls this to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CMachineInfoDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}

void CMachineInfoDlg::OnStart() 
{
    UpdateData ( TRUE );

    m_btnStart.EnableWindow ( FALSE );

    if ( m_strSavePath.Right (1) == "\\" )
        m_strSavePath.TrimRight ( "\\" );

    CString strSavePath;
    strSavePath = m_strSavePath;
    strSavePath += "\\Machine Info";
    
    CreateDirectory ( strSavePath, NULL );    

    if ( m_bSystemInfo )
    {
        if ( !m_MSSystemInfo.CreateReport ( strSavePath ))
            AfxMessageBox ( "Failed creating System Info report" );
    }

    if ( m_bActivityLog )
    {
        if ( !m_ActivityLog.CreateReport ( strSavePath ))
            AfxMessageBox ( "Failed creating Activity Log report" );
    }

    if ( m_bSymRegKeys )
    {
        if ( !m_SymRegKeys.CreateReport ( strSavePath ))
            AfxMessageBox ( "Failed creating Symantec Reg Keys report" );
    }

    if ( m_bDebugLogs )
    {
        CString strTempPath;
        strTempPath = strSavePath;
        strTempPath += "\\Debug Logs";
    
        CreateDirectory ( strTempPath, NULL );    

        if ( !m_DebugLogs.CreateReport ( strTempPath ))
            AfxMessageBox ( "Failed creating Debug Logs report" );
    }

    if ( m_bLiveUpdateLogs )
    {
        CString strTempPath;
        strTempPath = strSavePath;
        strTempPath += "\\LiveUpdate Logs";
    
        CreateDirectory ( strTempPath, NULL );    

        if ( !m_LiveUpdateLogs.CreateReport ( strTempPath ))
            AfxMessageBox ( "Failed creating LiveUpdate logs report" );
    }

    if ( m_bNAVOptions )
    {
        CString strTempPath;
        strTempPath = strSavePath;
        strTempPath += "\\NAV Options";
    
        CreateDirectory ( strTempPath, NULL );    

        if ( !m_NAVOptions.CreateReport ( strTempPath ))
            AfxMessageBox ( "Failed creating NAV Options report" );
    }

    if ( m_bNAVFileVersions )
    {
        if ( !m_NAVFileVersions.CreateReport ( strSavePath ))
            AfxMessageBox ( "Failed creating NAV File Versions report" );
    }

    m_btnStart.EnableWindow ( TRUE );
}
