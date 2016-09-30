// /////////////////////////////////////////////////////////////////////////////////////////////////
// Symantec copyright header start
// //////////////////////
//
// PROPRIETARY / CONFIDENTIAL.
// Use of this product is subject to license terms.
// Copyright © 2004 Symantec Corporation.
// All rights reserved.
//
// /////////////////////////////////////////////////////////////////////////////////////////////////
// Symantec copyright header stop
// //////////////////////
//// SessionDlg.cpp : implementation file
//

#include "stdafx.h"
#include "Sketchbook.h"
#include "build.h"
#include "sessiondlg.h"
#include "ccsplitpath.h"

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
public:
	CString m_strVersion;
	CString m_strCopyright;
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
, m_strCopyright(VER_STR_COPYRIGHT)
{
	m_strVersion.Format(_T("Version: %u.%u.%u.%u"), VER_NUM_PRODVERMAJOR, VER_NUM_PRODVERMINOR, VER_NUM_PRODVERSUBMINOR, VER_NUM_BUILDNUMBER);
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//DDX_Text(pDX, IDC_VERSION, m_strVersion);
	//DDX_Text(pDX, IDC_COPYRIGHT, m_strCopyright);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
END_MESSAGE_MAP()



// CSessionDlg dialog

IMPLEMENT_DYNAMIC(CSessionDlg, CDialog)
CSessionDlg::CSessionDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CSessionDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

CSessionDlg::~CSessionDlg()
{
}

void CSessionDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CSessionDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_CLOSE()
	ON_BN_CLICKED(IDC_NEWDEFINITION, OnBnClickedNewdefinition)
	ON_BN_CLICKED(IDC_NEWINSTANCE, OnBnClickedNewinstance)
	ON_MESSAGE(WM_MODELESS_CLOSED, OnModeLessClose)
END_MESSAGE_MAP()

BOOL CSessionDlg::OnInitDialog()
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


void CSessionDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

void CSessionDlg::OnPaint() 
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
HCURSOR CSessionDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CSessionDlg::OnBnClickedNewdefinition()
{
	// TODO: Add your control notification handler code here
// TODO: Add your control notification handler code here
	CFileDialog dlgFile(TRUE, NULL, _T("*.xml"), OFN_HIDEREADONLY | OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_NOCHANGEDIR, _T("ccEraser Text Files (*.xml)|*.xml|ccEraser Binary Files (*.bin)|*.bin||"));

	if (dlgFile.DoModal() == IDOK)
	{
		ccLib::CString strPath;
		ccLib::CSplitPath::GetProcessDirectory(strPath);
		strPath += BINFILE;
		::SetFileAttributes(strPath, FILE_ATTRIBUTE_NORMAL);

		if(dlgFile.GetFileExt() == _T("xml"))
		{
			::DeleteFile(strPath);

			CString strCmd;
			strCmd.Format(_T("EsrBinCp.exe /X \"%s\" \"%s\""), dlgFile.GetPathName(),strPath);

			STARTUPINFO si;
			PROCESS_INFORMATION pi;

			ZeroMemory(&si, sizeof(si));
			si.cb = sizeof(si);
			ZeroMemory(&pi, sizeof(pi));

			::CreateProcess(_T("EsrBinCp.exe"), strCmd.GetBuffer(), NULL, NULL, TRUE, 0, NULL, NULL, &si, &pi);
			::WaitForSingleObject(pi.hProcess, INFINITE);
			CloseHandle(pi.hThread);
			CloseHandle(pi.hProcess);
		}
		else
		{
			if(strPath.CompareNoCase(dlgFile.GetPathName()) != 0)
			{
				::CopyFile(dlgFile.GetPathName(), strPath, FALSE);
		}
	}
}

}

void CSessionDlg::OnBnClickedNewinstance()
{
	// TODO: Add your control notification handler code here

	CSketchbookDlg * Dlg = new CSketchbookDlg;
	Dlg->Create(IDD_SKETCHBOOK_DIALOG, GetDesktopWindow() );
	Dlg->ShowWindow(SW_SHOWNA );
	Dlg->SetOwner(this);
	listWnd.AddTail(Dlg);

	if(Dlg->m_pEraser == NULL)
	{
		Dlg->EndDialog(0);
	}
}

void CSessionDlg::OnCancel()
{
	while(!listWnd.IsEmpty())
	{
		CSketchbookDlg * Dlg = listWnd.GetHead();
		if(!Dlg->IsWindowEnabled())
		{
			Dlg->SetForegroundWindow();
			return;
		}
		else
		{
			Dlg->DestroyWindow();
			listWnd.RemoveHead();
		}
	}

	CDialog::OnCancel();
}

LRESULT CSessionDlg::OnModeLessClose(WPARAM wParam, LPARAM lParam)
{
	listWnd.RemoveAt(listWnd.Find((CSketchbookDlg *)lParam));

	return 0;
}
