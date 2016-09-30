////////////////////////
//
// PROPRIETARY / CONFIDENTIAL.
// Use of this product is subject to license terms.
// Copyright © 2006 Symantec Corporation.
// All rights reserved.
//
/////////////////////////////////////////////////////////////////////////////////////////////////

// InstoptsEditDlg.cpp : implementation file
//

#include "stdafx.h"
#include "InstoptsEdit.h"
#include "InstoptsEditDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CAboutDlg dialog used for App About

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// Dialog Data
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Implementation
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
END_MESSAGE_MAP()


// CInstoptsEditDlg dialog



CInstoptsEditDlg::CInstoptsEditDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CInstoptsEditDlg::IDD, pParent)
{
	m_bString = false;
	m_bDword = false;
	m_bAdd = false;
	m_bDelete = false;
	m_hInstOptsDat;
	m_sFolderName = "";
	m_sName = "";
	m_sValue = "";

	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CInstoptsEditDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CInstoptsEditDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_RADIO_STRING, OnBnClickedRadioString)
	ON_BN_CLICKED(IDC_RADIO_ADD, OnBnClickedRadioAdd)
	ON_BN_CLICKED(IDC_RADIO_DELETE, OnBnClickedRadioDelete)
	//ON_BN_CLICKED(IDOK, OnBnClickedOk)
	ON_BN_CLICKED(IDOK, OnOK)
	ON_EN_CHANGE(IDC_DIR, OnEnChangeDir)
	ON_BN_CLICKED(IDC_RADIO_DWORD, OnBnClickedRadioDword)
	ON_EN_CHANGE(IDC_NAME, OnEnChangeName)
	ON_EN_CHANGE(IDC_VALUE, OnEnChangeValue)
	ON_BN_CLICKED(IDC_BROWSE, OnBnClickedBrowse)
	ON_BN_CLICKED(IDCANCEL, OnBnClickedCancel)
END_MESSAGE_MAP()


// CInstoptsEditDlg message handlers

BOOL CInstoptsEditDlg::OnInitDialog()
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

	// TODO: Add extra initialization here
	
	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CInstoptsEditDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

void CInstoptsEditDlg::OnPaint() 
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

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

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CInstoptsEditDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CInstoptsEditDlg::OnEnChangeDir()
{
	// TODO:  If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.

	// TODO:  Add your control notification handler code here
	GetDlgItemText(IDC_DIR, m_sFolderName.GetBuffer(MAX_PATH), MAX_PATH);
	m_sFolderName.ReleaseBuffer();
}

void CInstoptsEditDlg::OnBnClickedBrowse()
{
	// TODO: Add your control notification handler code here
	BROWSEINFO bi;
	CString sFolderName;
	CString sTitle = "Find the folder containing the instopts.dat files";
	bi.hwndOwner = m_hWnd;
	bi.pidlRoot = NULL;
	bi.lpszTitle = sTitle;
	bi.pszDisplayName = sFolderName.GetBuffer(MAX_PATH);
	bi.ulFlags = BIF_USENEWUI | BIF_NONEWFOLDERBUTTON;
	bi.lpfn = NULL;
	SHGetPathFromIDList(SHBrowseForFolder(&bi), m_sFolderName.GetBuffer(MAX_PATH));
	m_sFolderName.ReleaseBuffer();
	sFolderName.ReleaseBuffer();
	SetDlgItemText(IDC_DIR, m_sFolderName);
}

void CInstoptsEditDlg::OnBnClickedRadioAdd()
{
	m_bAdd = true;
	m_bDelete = false;
}

void CInstoptsEditDlg::OnBnClickedRadioDelete()
{
	m_bAdd = false;
	m_bDelete = true;
}

void CInstoptsEditDlg::OnBnClickedRadioDword()
{
	m_bDword = true;
	m_bString = false;
}

void CInstoptsEditDlg::OnBnClickedRadioString()
{
	m_bDword = false;
	m_bString = true;
}

void CInstoptsEditDlg::OnEnChangeName()
{
	CString sName;
	// TODO:  If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.

	// TODO:  Add your control notification handler code here
	GetDlgItemText(IDC_NAME, m_sName.GetBuffer(MAX_PATH), MAX_PATH);
	m_sName.ReleaseBuffer();
}

void CInstoptsEditDlg::OnEnChangeValue()
{
	// TODO:  If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.

	// TODO:  Add your control notification handler code here
	GetDlgItemText(IDC_VALUE, m_sValue.GetBuffer(MAX_PATH), MAX_PATH);
	m_sValue.ReleaseBuffer();
}

void CInstoptsEditDlg::OnOK()
{
	TCHAR szFileWithPath[MAX_PATH] = {0};
	TCHAR szCurrentDir[MAX_PATH] = {0};	

	if (m_sFolderName == "")
	{
		TCHAR *pszBackSlash = NULL;
		GetModuleFileName(NULL, szCurrentDir, sizeof(szCurrentDir));
		pszBackSlash = _tcsrchr(szCurrentDir, _T('\\'));
		*pszBackSlash = _T('\0');
		wsprintf(szFileWithPath, _T("%s\\*.dat"), szCurrentDir);
	}
	else
        wsprintf(szFileWithPath, _T("%s\\*.dat"), m_sFolderName);

	WIN32_FIND_DATA FindFileData = {0};

	// Initialize the options library

	NAVOPTS32_STATUS Status = NavOpts32_Allocate(&m_hInstOptsDat);
	if (Status != NAVOPTS32_OK)
		MessageBox("Unable to initialize the options library.", "Error", MB_ICONERROR | MB_OK);

	HANDLE hFile = FindFirstFile(szFileWithPath, &FindFileData);
	BOOL bFileFound = true;

	while(INVALID_HANDLE_VALUE != hFile && bFileFound)
	{
		if (szCurrentDir == NULL)
			wsprintf(szFileWithPath, _T("%s\\%s"), m_sFolderName, FindFileData.cFileName);
		else
			wsprintf(szFileWithPath, _T("%s\\%s"), szCurrentDir, FindFileData.cFileName);

		Status = NavOpts32_Load(szFileWithPath, m_hInstOptsDat, true);
		if (Status != NAVOPTS32_OK)
		{
			TCHAR szMsg[MAX_PATH] = {0};
			wsprintf(szMsg, _T("Unable to load %s"), FindFileData.cFileName);
			MessageBox(szMsg, "Error", MB_ICONERROR | MB_OK);	
			break;
		}

		NavOpts32_RemoveValue(m_hInstOptsDat, m_sName);

		if ( m_bAdd && (m_sName != "") && (m_sValue != "") )
		{
			if ( true == m_bDword )
			{
				DWORD dwValue = _ttoi(m_sValue);
				NAVOPTS32_STATUS Status = NavOpts32_SetDwordValue(m_hInstOptsDat, m_sName, dwValue);
			}
			if ( true == m_bString)
			{
				Status = NavOpts32_SetStringValue(m_hInstOptsDat, m_sName, m_sValue);
			}
			if (Status != NAVOPTS32_OK)
			{
				TCHAR szMsg[MAX_PATH] = {0};
				wsprintf(szMsg, _T("Unable to add value to %s"), FindFileData.cFileName);
				MessageBox(szMsg, "Error", MB_ICONERROR | MB_OK);
				break;
			}
		}

		Status = NavOpts32_Save(FindFileData.cFileName, m_hInstOptsDat);
		
		if (Status != NAVOPTS32_OK)
		{
			TCHAR szMsg[MAX_PATH] = {0};
			wsprintf(szMsg, _T("Unable to save to %s"), FindFileData.cFileName);
			MessageBox(szMsg, "Error", MB_ICONERROR | MB_OK);
			break;
		}
		bFileFound = FindNextFile(hFile, &FindFileData);
	}

	MessageBox("Changes had been applied", "InstoptsEdit", MB_ICONINFORMATION | MB_OK);
}

void CInstoptsEditDlg::OnBnClickedCancel()
{
	// TODO: Add your control notification handler code here
	OnCancel();
}
