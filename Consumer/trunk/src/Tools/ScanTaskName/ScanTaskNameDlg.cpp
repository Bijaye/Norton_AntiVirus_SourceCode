////////////////////////
//
// PROPRIETARY / CONFIDENTIAL.
// Use of this product is subject to license terms.
// Copyright © 2006 Symantec Corporation.
// All rights reserved.
//
/////////////////////////////////////////////////////////////////////////////////////////////////

// ScanTaskNameDlg.cpp : implementation file
//

#include "stdafx.h"
#include "ScanTaskName.h"
#include "ScanTaskNameDlg.h"
#include "StahlSoft.h"
#include "HNAVOPTS32.h"
#include ".\scantasknamedlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#define NAVOPT_SUCCEEDED(res)	((res == NAVOPTS32_OK)? S_OK : E_FAIL)
const char SZ_TASK_NAME[] = "Name";

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


// CScanTaskNameDlg dialog



CScanTaskNameDlg::CScanTaskNameDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CScanTaskNameDlg::IDD, pParent)
	, m_sNewTaskName(_T(""))
	, m_sCurTaskName(_T(""))
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CScanTaskNameDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_NEW_TASK_NAME, m_sNewTaskName);
	DDX_Text(pDX, IDC_EDIT_CURRENT_TASK_NAME, m_sCurTaskName);
}

BEGIN_MESSAGE_MAP(CScanTaskNameDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_OPEN_FILE, OnBnClickedOpenFile)
	ON_BN_CLICKED(IDC_SAVE_FILE, OnBnClickedSaveFile)
END_MESSAGE_MAP()


// CScanTaskNameDlg message handlers

BOOL CScanTaskNameDlg::OnInitDialog()
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

void CScanTaskNameDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

void CScanTaskNameDlg::OnPaint() 
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
HCURSOR CScanTaskNameDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CScanTaskNameDlg::OnBnClickedOpenFile()
{
	CFileDialog FileDlg(TRUE);
	FileDlg.m_ofn.lpstrFilter = L".sca";

	if(IDOK == FileDlg.DoModal())
	{
		m_sFileName = FileDlg.GetPathName();
		SetDlgItemText(IDC_EDIT_TASK_FILE, m_sFileName.GetString());

		STAHLSOFT_HRX_TRY(hr)
		{
			// Load task file
			CHNAVOPTS32 NavOpts;
			hrx << NAVOPT_SUCCEEDED(NavOpts.Allocate());
			hrx << NAVOPT_SUCCEEDED(NavOpts.Load(m_sFileName.GetString()));

			BYTE byBuffer[(MAX_PATH+1)*2] = {0};
			DWORD dwBufferSize = sizeof(byBuffer);
			ZeroMemory(&byBuffer, dwBufferSize);

			// Read task name
			hrx << NAVOPT_SUCCEEDED(NavOpts.GetBinaryValue(SZ_TASK_NAME, (LPBYTE)&byBuffer, dwBufferSize));
			m_sCurTaskName = (LPCWSTR)(byBuffer);

			// Update UI with new file name
			SetDlgItemText(IDC_EDIT_TASK_FILE, m_sFileName.GetString());

			// Update UI with new task name
			SetDlgItemText(IDC_EDIT_CURRENT_TASK_NAME, m_sCurTaskName.GetString());
		}
		STAHLSOFT_HRX_CATCH_ALL_NO_RETURN(hr)

		if(FAILED(hr))
		{
			CStringW sMsg;
			sMsg.Format(L"Failed to load %s", m_sFileName.GetString());
			MessageBox(sMsg, NULL, MB_OK|MB_ICONSTOP);
		}
	}

	UpdateData(TRUE);
}


void CScanTaskNameDlg::OnBnClickedSaveFile()
{
	STAHLSOFT_HRX_TRY(hr)
	{
		UpdateData();

		CHNAVOPTS32 NavOpts;
		hrx << NAVOPT_SUCCEEDED(NavOpts.Allocate());
		hrx << NAVOPT_SUCCEEDED(NavOpts.Load(m_sFileName.GetString()));

		// Write the task name as binary data
		// because NavOpts support only ANSI string, not UNICODE string.
		LPBYTE lpBuffer = (LPBYTE)(m_sNewTaskName.GetString());
		DWORD dwBufferSize = m_sNewTaskName.GetLength() * sizeof(WCHAR);
		hrx << NAVOPT_SUCCEEDED(NavOpts.SetBinaryValue(SZ_TASK_NAME, lpBuffer, dwBufferSize));
		hrx << NAVOPT_SUCCEEDED(NavOpts.Save(m_sFileName.GetString()));

		m_sCurTaskName = m_sNewTaskName.GetString();

		SetDlgItemText(IDC_EDIT_CURRENT_TASK_NAME, m_sNewTaskName.GetString());

		UpdateData();
	}
	STAHLSOFT_HRX_CATCH_ALL_NO_RETURN(hr)

	if(FAILED(hr))
	{
		CStringW sMsg;
		sMsg.Format(L"Failed to save task name");
		MessageBox(sMsg, NULL, MB_OK|MB_ICONSTOP);
	}
}
