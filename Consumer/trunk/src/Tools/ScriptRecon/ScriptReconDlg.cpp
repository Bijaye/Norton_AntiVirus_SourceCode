////////////////////////
//
// PROPRIETARY / CONFIDENTIAL.
// Use of this product is subject to license terms.
// Copyright © 2006 Symantec Corporation.
// All rights reserved.
//
/////////////////////////////////////////////////////////////////////////////////////////////////

// ScriptReconDlg.cpp : implementation file
//

#include "stdafx.h"
#include "ScriptRecon.h"
#include "ScriptReconDlg.h"
#include "NAVInfo.h"
#include "FindCode.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CAboutDlg dialog used for App About

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// Dialog Data
	//{{AFX_DATA(CAboutDlg)
	enum { IDD = IDD_ABOUTBOX };
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAboutDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CAboutDlg)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
	//{{AFX_DATA_INIT(CAboutDlg)
	//}}AFX_DATA_INIT
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAboutDlg)
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
	//{{AFX_MSG_MAP(CAboutDlg)
		// No message handlers
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CScriptReconDlg dialog

CScriptReconDlg::CScriptReconDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CScriptReconDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CScriptReconDlg)
	m_strFileName = _T("");
	m_dwLineNumber = 0;
	m_strCode = _T("");
	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CScriptReconDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CScriptReconDlg)
	DDX_Control(pDX, IDC_CODE, m_edtCode);
	DDX_Control(pDX, IDC_FILENAME, m_cboURL);
	DDX_CBString(pDX, IDC_FILENAME, m_strFileName);
	DDX_Text(pDX, IDC_LINE, m_dwLineNumber);
	DDV_MinMaxDWord(pDX, m_dwLineNumber, 0, 99999);
	DDX_Text(pDX, IDC_CODE, m_strCode);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CScriptReconDlg, CDialog)
	//{{AFX_MSG_MAP(CScriptReconDlg)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_FIND, OnFind)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CScriptReconDlg message handlers

BOOL CScriptReconDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		CString strAboutMenu;
		strAboutMenu.LoadString(IDS_ABOUTBOX);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon
	
    m_cboURL.AddString ( "NAVUI.dll/advanced.htm" );
    m_cboURL.AddString ( "NAVUI.dll/scan.htm" );
    m_cboURL.AddString ( "NAVUI.dll/scan_list.htm" );
    m_cboURL.AddString ( "NAVUI.dll/reports.htm" );
    m_cboURL.AddString ( "NAVUI.dll/navstats.htm" );
    m_cboURL.AddString ( "NAVOpts.dll/optionsbtm.htm" );
    m_cboURL.AddString ( "NAVOpts.dll/options.htm" );
    m_cboURL.AddString ( "NAVOpts.dll/options_bloodhound.htm" );
    m_cboURL.AddString ( "NAVOpts.dll/options_exclude_dlg.htm" );
    m_cboURL.AddString ( "NAVOpts.dll/options_int_email.htm" );
    m_cboURL.AddString ( "NAVOpts.dll/options_int_lu.htm" );
    m_cboURL.AddString ( "NAVOpts.dll/options_new_ext_dlg.htm" );
    m_cboURL.AddString ( "NAVOpts.dll/options_othr_advanced.htm" );
    m_cboURL.AddString ( "NAVOpts.dll/options_othr_gen.htm" );
    m_cboURL.AddString ( "NAVOpts.dll/options_othr_inoc.htm" );
    m_cboURL.AddString ( "NAVOpts.dll/options_othr_log.htm" );
    m_cboURL.AddString ( "NAVOpts.dll/options_sys_ap.htm" );
    m_cboURL.AddString ( "NAVOpts.dll/options_sys_ap_ad.htm" );
    m_cboURL.AddString ( "NAVOpts.dll/options_sys_excl.htm" );
    m_cboURL.AddString ( "NAVOpts.dll/options_sys_sb.htm" );
    m_cboURL.AddString ( "NAVOpts.dll/options_sys_scan.htm" );
    m_cboURL.AddString ( "NAVOpts.dll/options_sys_sel_ext_dlg.htm" );
    m_cboURL.AddString ( "NAVOpts.dll/optionsmenu.htm" );
    m_cboURL.AddString ( "Cfgwiz.exe/cfgwizroadmap.htm" );
    m_cboURL.AddString ( "Cfgwiz.exe/configuration.htm" );
    m_cboURL.AddString ( "Cfgwiz.exe/livereg.htm" );
    m_cboURL.AddString ( "Cfgwiz.exe/postinstall.htm" );
    m_cboURL.AddString ( "Cfgwiz.exe/postinstallprogress.htm" );
    m_cboURL.AddString ( "Cfgwiz.exe/select.htm" );
    m_cboURL.AddString ( "Cfgwiz.exe/subscription.htm" );
    m_cboURL.AddString ( "Cfgwiz.exe/welcome.htm" );
    m_cboURL.AddString ( "Defalert.dll/defalert.js" );

    m_dwLineNumber = 0;

    m_strCode = "";
    m_edtCode.EnableWindow ( FALSE );
	
	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CScriptReconDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CScriptReconDlg::OnPaint() 
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
HCURSOR CScriptReconDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}

void CScriptReconDlg::OnFind() 
{
	UpdateData (TRUE);

    if ( m_strFileName.IsEmpty () )
    {
        AfxMessageBox ( "No filename entered" );
        return;
    }

    if ( m_dwLineNumber == 0 )
    {
        AfxMessageBox ( "No line number entered" );
        return;
    }

    TCHAR szFileName [MAX_PATH] ={0};
    TCHAR szFilePath [MAX_PATH] ={0};
    TCHAR szResourceName [MAX_PATH] = {0};
    TCHAR szExtension [5] = {0};
    TCHAR szCode [1000] = {0};
    CNAVInfo NAVInfo;

    char* pszTemp = NULL;
    _tcscpy ( szFileName, m_strFileName.GetBuffer (1));
    //m_strFileName.ReleaseBuffer ();

    pszTemp = strtok ( szFileName, "/" ); 
    _tcscpy ( szFileName, pszTemp );


    pszTemp = strtok ( NULL, "\0" ); 
    _tcscpy ( szResourceName, pszTemp );

    if ( !szFileName )
    {
        AfxMessageBox ( "No file name" );
        return;
    }

    if ( !szResourceName )
    {
        AfxMessageBox ( "No resource name" );
        return;
    }

    // Lowercase, just in case
    //
    pszTemp = strlwr ( szResourceName );
    _tcscpy ( szResourceName, pszTemp );

    // Replace the .htm with a .js
    //
    pszTemp = NULL;
    pszTemp = strstr ( szResourceName, ".htm" );
    
    if ( pszTemp )
    {
        szResourceName [pszTemp-szResourceName] = '\0';
        _tcscat ( szResourceName, ".js" );
    }


    // Look up file name and resource for the user
    //
    _stprintf ( szFilePath, "%s\\%s", NAVInfo.GetNAVDir (), szFileName );

    CFindCode Finder;
    
    // IE always reports the line number as one greater than it really is.
    //
    DWORD dwIELine = m_dwLineNumber - 1;

    if ( Finder.FindLine ( szFilePath, szResourceName, dwIELine, szCode, 1000 ))
    {
        m_strCode = szCode;
        m_strCode.TrimLeft ();
    }
    else
        AfxMessageBox ( "Error finding code" );

    if ( m_strCode.IsEmpty () )
        m_edtCode.EnableWindow ( FALSE );
    else 
        m_edtCode.EnableWindow ( TRUE );

    UpdateData ( FALSE );
	
}
