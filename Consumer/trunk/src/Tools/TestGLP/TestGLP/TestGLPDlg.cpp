////////////////////////
//
// PROPRIETARY / CONFIDENTIAL.
// Use of this product is subject to license terms.
// Copyright © 2006 Symantec Corporation.
// All rights reserved.
//
/////////////////////////////////////////////////////////////////////////////////////////////////

// TestGLPDlg.cpp : implementation file
//
//#pragma comment( lib, "rpcrt4")
#include "stdafx.h"
#include "TestGLP.h"
#include "TestGLPDlg.h"
#include ".\testglpdlg.h"

#include "Const.h"
#include "Rpcdce.h"
#include <vector>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif
//typedef vector<int> INTVECTOR;


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


// CTestGLPDlg dialog



CTestGLPDlg::CTestGLPDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CTestGLPDlg::IDD, pParent)
	, m_bRegCheckAll(FALSE)
	, m_bRegistry(FALSE)
	, m_bReg1(FALSE)
	, m_bReg2(FALSE)
	, m_bReg3(FALSE)
	, m_bReg4(FALSE)
	, m_bReg5(FALSE)
	, m_bReg6(FALSE)
	, m_bReg7(FALSE)
	, m_bReg8(FALSE)
	, m_bReg9(FALSE)
	, m_bReg10(FALSE)
	, m_bReg11(FALSE)
	, m_bReg12(FALSE)
	, m_bReg13(FALSE)
	, m_bReg14(FALSE)
	, m_bReg15(FALSE)
	, m_bReg16(FALSE)
	, m_bReg17(FALSE)
	, m_bReg18(FALSE)
	, m_bReg19(FALSE)
	, m_bReg20(FALSE)
	, m_bReg21(FALSE)
	, m_bReg22(FALSE)
	, m_bReg23(FALSE)
	, m_bReg24(FALSE)
	, m_bReg25(FALSE)
	, m_bReg26(FALSE)
	, m_bReg27(FALSE)
	, m_bReg28(FALSE)
	, m_bReg29(FALSE)
	, m_bReg30(FALSE)
	, m_bReg31(FALSE)
	, m_bReg32(FALSE)
	, m_bStartup(FALSE)
	, m_editStartup(_T(""))
	, m_bAUTONT(FALSE)
	, m_bWINBAT(FALSE)
	, m_bAUTOBAT(FALSE)
	, m_bSYS(FALSE)
	, m_bWININI(FALSE)
	, m_bFiles(FALSE)
	, m_iTotal(0)
	, m_strResult(_T(""))
	, m_bReg33(FALSE)
	, m_bReg34(FALSE)
	, m_bReg35(FALSE)
	, m_bReg36(FALSE)
	, m_bReg37(FALSE)
	, m_bReg38(FALSE)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CTestGLPDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);

	DDX_Text(pDX, IDC_EDIT_VIRUSNAME, m_strFullVirusName);
	DDX_Check(pDX, IDC_CHECK_REGISTRY, m_bRegistry);
	DDX_Check(pDX, IDC_CHECK_ALLREG, m_bRegCheckAll);
	DDX_Check(pDX, IDC_CHECK1, m_bReg1);
	DDX_Check(pDX, IDC_CHECK2, m_bReg2);
	DDX_Check(pDX, IDC_CHECK3, m_bReg3);
	DDX_Check(pDX, IDC_CHECK4, m_bReg4);
	DDX_Check(pDX, IDC_CHECK5, m_bReg5);
	DDX_Check(pDX, IDC_CHECK6, m_bReg6);
	DDX_Check(pDX, IDC_CHECK7, m_bReg7);
	DDX_Check(pDX, IDC_CHECK8, m_bReg8);
	DDX_Check(pDX, IDC_CHECK9, m_bReg9);
	DDX_Check(pDX, IDC_CHECK10, m_bReg10);
	DDX_Check(pDX, IDC_CHECK11, m_bReg11);
	DDX_Check(pDX, IDC_CHECK12, m_bReg12);
	DDX_Check(pDX, IDC_CHECK13, m_bReg13);
	DDX_Check(pDX, IDC_CHECK14, m_bReg14);
	DDX_Check(pDX, IDC_CHECK15, m_bReg15);
	DDX_Check(pDX, IDC_CHECK16, m_bReg16);
	DDX_Check(pDX, IDC_CHECK17, m_bReg17);
	DDX_Check(pDX, IDC_CHECK18, m_bReg18);
	DDX_Check(pDX, IDC_CHECK19, m_bReg19);
	DDX_Check(pDX, IDC_CHECK20, m_bReg20);
	DDX_Check(pDX, IDC_CHECK21, m_bReg21);
	DDX_Check(pDX, IDC_CHECK22, m_bReg22);
	DDX_Check(pDX, IDC_CHECK23, m_bReg23);
	DDX_Check(pDX, IDC_CHECK24, m_bReg24);
	DDX_Check(pDX, IDC_CHECK25, m_bReg25);
	DDX_Check(pDX, IDC_CHECK26, m_bReg26);
	DDX_Check(pDX, IDC_CHECK27, m_bReg27);
	DDX_Check(pDX, IDC_CHECK28, m_bReg28);
	DDX_Check(pDX, IDC_CHECK29, m_bReg29);
	DDX_Check(pDX, IDC_CHECK30, m_bReg30);
	DDX_Check(pDX, IDC_CHECK31, m_bReg31);
	DDX_Check(pDX, IDC_CHECK32, m_bReg32);
	DDX_Check(pDX, IDC_CHECK_STARTUP, m_bStartup);
	DDX_Control(pDX, IDC_LIST_START, m_ListStartup);
	DDX_Text(pDX, IDC_EDIT_STARTUP, m_editStartup);
	DDX_Check(pDX, IDC_CHECK_AUTONT, m_bAUTONT);
	DDX_Check(pDX, IDC_CHECK_WINBAT, m_bWINBAT);
	DDX_Check(pDX, IDC_CHECK_AUTOBAT, m_bAUTOBAT);
	DDX_Check(pDX, IDC_CHECK_SYS, m_bSYS);
	DDX_Check(pDX, IDC_CHECK_WININI, m_bWININI);
	DDX_Check(pDX, IDC_CHECK_FILES, m_bFiles);
	DDX_Text(pDX, IDC_TOTAL, m_iTotal);
	DDX_Text(pDX, IDC_RESULT, m_strResult);
	DDX_Check(pDX, IDC_CHECK33, m_bReg33);
	DDX_Check(pDX, IDC_CHECK34, m_bReg34);
	DDX_Check(pDX, IDC_CHECK35, m_bReg35);
	DDX_Check(pDX, IDC_CHECK36, m_bReg36);
	DDX_Check(pDX, IDC_CHECK37, m_bReg37);
	DDX_Check(pDX, IDC_CHECK38, m_bReg38);
}

BEGIN_MESSAGE_MAP(CTestGLPDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_MESSAGE(WM_DROPFILES, OnDropFiles)
	ON_BN_CLICKED(IDC_MODIFY, OnBnClickedModify)
	ON_BN_CLICKED(IDC_CHECK_REGISTRY, OnBnClickedCheckRegistry)
	ON_BN_CLICKED(IDC_CHECK_ALLREG, OnBnClickedCheckAllreg)
	ON_BN_CLICKED(IDC_CHECK_STARTUP, OnBnClickedCheckStartup)
	ON_BN_CLICKED(IDC_CHECK_FILES, OnBnClickedCheckFiles)
	ON_BN_CLICKED(IDC_CLEAR, OnBnClickedClear)
	ON_BN_CLICKED(IDC_SAVE, OnBnClickedSave)
END_MESSAGE_MAP()


// CTestGLPDlg message handlers

BOOL CTestGLPDlg::OnInitDialog()
{
	CDialog::OnInitDialog();
	::DragAcceptFiles(GetSafeHwnd(), TRUE);
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
	m_ListStartup.ModifyStyle(NULL, LVS_REPORT);
	m_ListStartup.InsertColumn(1, _T("Select"), LVCFMT_LEFT, 100);
	m_ListStartup.InsertColumn(2, _T("Location"), LVCFMT_LEFT, 400);

	m_ListStartup.SetExtendedStyle(LVS_EX_GRIDLINES | LVS_EX_FULLROWSELECT  | LVS_EX_CHECKBOXES );
	//InitListStartupFolders();
	GetStartupFolders();

	for(int i=0; i < m_strVector.size(); i++)
	{
		const int IDX = m_ListStartup.InsertItem(i, _T(""));
		CString strStartup ( m_strVector.at(i));
		CString strRegStartup (m_strRegVector.at(i));
		m_ListStartup.SetItemText(IDX, 1, strStartup);
		m_ListStartup.SetItemText(IDX, 0, strRegStartup);
	}

	UpdateData(FALSE);

	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CTestGLPDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

void CTestGLPDlg::OnPaint() 
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
HCURSOR CTestGLPDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

LRESULT CTestGLPDlg::OnDropFiles(WPARAM wParam, LPARAM lParam)
{
	TCHAR szFileName[MAX_PATH];
	::DragQueryFile((HDROP) wParam, 0, szFileName, sizeof(szFileName));
	::DragFinish((HDROP) wParam);
	SetDlgItemText(IDC_EDIT_VIRUSNAME, szFileName);
	return 0;
}

void CTestGLPDlg::OnBnClickedCheckRegistry()
{
	UpdateData(TRUE);
	if(m_bRegistry)
	{
		GetDlgItem(IDC_CHECK_ALLREG)->EnableWindow(TRUE);
		GetDlgItem(IDC_CHECK1)->EnableWindow(TRUE);
		GetDlgItem(IDC_CHECK2)->EnableWindow(TRUE);
		GetDlgItem(IDC_CHECK3)->EnableWindow(TRUE);
		GetDlgItem(IDC_CHECK4)->EnableWindow(TRUE);
		GetDlgItem(IDC_CHECK5)->EnableWindow(TRUE);
		GetDlgItem(IDC_CHECK6)->EnableWindow(TRUE);
		GetDlgItem(IDC_CHECK7)->EnableWindow(TRUE);
		GetDlgItem(IDC_CHECK8)->EnableWindow(TRUE);
		GetDlgItem(IDC_CHECK9)->EnableWindow(TRUE);
		GetDlgItem(IDC_CHECK10)->EnableWindow(TRUE);
		GetDlgItem(IDC_CHECK11)->EnableWindow(TRUE);
		GetDlgItem(IDC_CHECK12)->EnableWindow(TRUE);
		GetDlgItem(IDC_CHECK13)->EnableWindow(TRUE);
		GetDlgItem(IDC_CHECK14)->EnableWindow(TRUE);
		GetDlgItem(IDC_CHECK15)->EnableWindow(TRUE);
		GetDlgItem(IDC_CHECK16)->EnableWindow(TRUE);
		GetDlgItem(IDC_CHECK17)->EnableWindow(TRUE);
		GetDlgItem(IDC_CHECK18)->EnableWindow(TRUE);
		GetDlgItem(IDC_CHECK19)->EnableWindow(TRUE);
		GetDlgItem(IDC_CHECK20)->EnableWindow(TRUE);
		GetDlgItem(IDC_CHECK21)->EnableWindow(TRUE);
		GetDlgItem(IDC_CHECK22)->EnableWindow(TRUE);
		GetDlgItem(IDC_CHECK23)->EnableWindow(TRUE);
		GetDlgItem(IDC_CHECK24)->EnableWindow(TRUE);
		GetDlgItem(IDC_CHECK25)->EnableWindow(TRUE);
		GetDlgItem(IDC_CHECK26)->EnableWindow(TRUE);
		GetDlgItem(IDC_CHECK27)->EnableWindow(TRUE);
		GetDlgItem(IDC_CHECK28)->EnableWindow(TRUE);
		GetDlgItem(IDC_CHECK29)->EnableWindow(TRUE);
		GetDlgItem(IDC_CHECK30)->EnableWindow(TRUE);
		GetDlgItem(IDC_CHECK31)->EnableWindow(TRUE);
		GetDlgItem(IDC_CHECK32)->EnableWindow(TRUE);
		GetDlgItem(IDC_CHECK33)->EnableWindow(TRUE);
		GetDlgItem(IDC_CHECK34)->EnableWindow(TRUE);
		GetDlgItem(IDC_CHECK35)->EnableWindow(TRUE);
		GetDlgItem(IDC_CHECK36)->EnableWindow(TRUE);
		GetDlgItem(IDC_CHECK37)->EnableWindow(TRUE);
		GetDlgItem(IDC_CHECK38)->EnableWindow(TRUE);

		UpdateData(FALSE);
	} else
	{
		GetDlgItem(IDC_CHECK_ALLREG)->EnableWindow(FALSE);
		GetDlgItem(IDC_CHECK1)->EnableWindow(FALSE);
		GetDlgItem(IDC_CHECK2)->EnableWindow(FALSE);
		GetDlgItem(IDC_CHECK3)->EnableWindow(FALSE);
		GetDlgItem(IDC_CHECK4)->EnableWindow(FALSE);
		GetDlgItem(IDC_CHECK5)->EnableWindow(FALSE);
		GetDlgItem(IDC_CHECK6)->EnableWindow(FALSE);
		GetDlgItem(IDC_CHECK7)->EnableWindow(FALSE);
		GetDlgItem(IDC_CHECK8)->EnableWindow(FALSE);
		GetDlgItem(IDC_CHECK9)->EnableWindow(FALSE);
		GetDlgItem(IDC_CHECK10)->EnableWindow(FALSE);
		GetDlgItem(IDC_CHECK11)->EnableWindow(FALSE);
		GetDlgItem(IDC_CHECK12)->EnableWindow(FALSE);
		GetDlgItem(IDC_CHECK13)->EnableWindow(FALSE);
		GetDlgItem(IDC_CHECK14)->EnableWindow(FALSE);
		GetDlgItem(IDC_CHECK15)->EnableWindow(FALSE);
		GetDlgItem(IDC_CHECK16)->EnableWindow(FALSE);
		GetDlgItem(IDC_CHECK17)->EnableWindow(FALSE);
		GetDlgItem(IDC_CHECK18)->EnableWindow(FALSE);
		GetDlgItem(IDC_CHECK19)->EnableWindow(FALSE);
		GetDlgItem(IDC_CHECK20)->EnableWindow(FALSE);
		GetDlgItem(IDC_CHECK21)->EnableWindow(FALSE);
		GetDlgItem(IDC_CHECK22)->EnableWindow(FALSE);
		GetDlgItem(IDC_CHECK23)->EnableWindow(FALSE);
		GetDlgItem(IDC_CHECK24)->EnableWindow(FALSE);
		GetDlgItem(IDC_CHECK25)->EnableWindow(FALSE);
		GetDlgItem(IDC_CHECK26)->EnableWindow(FALSE);
		GetDlgItem(IDC_CHECK27)->EnableWindow(FALSE);
		GetDlgItem(IDC_CHECK28)->EnableWindow(FALSE);
		GetDlgItem(IDC_CHECK29)->EnableWindow(FALSE);
		GetDlgItem(IDC_CHECK30)->EnableWindow(FALSE);
		GetDlgItem(IDC_CHECK31)->EnableWindow(FALSE);
		GetDlgItem(IDC_CHECK32)->EnableWindow(FALSE);
		GetDlgItem(IDC_CHECK33)->EnableWindow(FALSE);
		GetDlgItem(IDC_CHECK34)->EnableWindow(FALSE);
		GetDlgItem(IDC_CHECK35)->EnableWindow(FALSE);
		GetDlgItem(IDC_CHECK36)->EnableWindow(FALSE);
		GetDlgItem(IDC_CHECK37)->EnableWindow(FALSE);
		GetDlgItem(IDC_CHECK38)->EnableWindow(FALSE);


		m_bReg1 = FALSE;
		m_bReg2 = FALSE;
		m_bReg3 = FALSE;
		m_bReg4 = FALSE;
		m_bReg5 = FALSE;
		m_bReg6 = FALSE;
		m_bReg7 = FALSE;
		m_bReg8 = FALSE;
		m_bReg9 = FALSE;
		m_bReg10 = FALSE;
		m_bReg11 = FALSE;
		m_bReg12 = FALSE;
		m_bReg13 = FALSE;
		m_bReg14 = FALSE;
		m_bReg15 = FALSE;
		m_bReg16 = FALSE;
		m_bReg17 = FALSE;
		m_bReg18 = FALSE;
		m_bReg19 = FALSE;
		m_bReg20 = FALSE;
		m_bReg21 = FALSE;
		m_bReg22 = FALSE;
		m_bReg23 = FALSE;
		m_bReg24 = FALSE;
		m_bReg25 = FALSE;
		m_bReg26 = FALSE;
		m_bReg27 = FALSE;
		m_bReg28 = FALSE;
		m_bReg29 = FALSE;
		m_bReg30 = FALSE;
		m_bReg31 = FALSE;
		m_bReg32 = FALSE;
		m_bReg33 = FALSE;
		m_bReg34 = FALSE;
		m_bReg35 = FALSE;
		m_bReg36 = FALSE;
		m_bReg37 = FALSE;
		m_bReg38 = FALSE;

		UpdateData(FALSE);
	}

}

void CTestGLPDlg::OnBnClickedCheckAllreg()
{
	UpdateData(TRUE);
	if (m_bRegCheckAll){
		m_bReg1 = TRUE;
		m_bReg2 = TRUE;
		m_bReg3 = TRUE;
		m_bReg4 = TRUE;
		m_bReg5 = TRUE;
		m_bReg6 = TRUE;
		m_bReg7 = TRUE;
		m_bReg8 = TRUE;
		m_bReg9 = TRUE;
		m_bReg10 = TRUE;
		m_bReg11 = TRUE;
		m_bReg12 = TRUE;
		m_bReg13 = TRUE;
		m_bReg14 = TRUE;
		m_bReg15 = TRUE;
		m_bReg16 = TRUE;
		m_bReg17 = TRUE;
		m_bReg18 = TRUE;
		m_bReg19 = TRUE;
		m_bReg20 = TRUE;
		m_bReg21 = TRUE;
		m_bReg22 = TRUE;
		m_bReg23 = TRUE;
		m_bReg24 = TRUE;
		m_bReg25 = TRUE;
		m_bReg26 = TRUE;
		m_bReg27 = TRUE;
		m_bReg28 = TRUE;
		m_bReg29 = TRUE;
		m_bReg30 = TRUE;
		m_bReg31 = TRUE;
		m_bReg32 = TRUE;
		m_bReg33 = TRUE;
		m_bReg34 = TRUE;
		m_bReg35 = TRUE;
		m_bReg36 = TRUE;
		m_bReg37 = TRUE;
		m_bReg38 = TRUE;
	
		GetDlgItem(IDC_CHECK1)->EnableWindow(FALSE);
		GetDlgItem(IDC_CHECK2)->EnableWindow(FALSE);
		GetDlgItem(IDC_CHECK3)->EnableWindow(FALSE);
		GetDlgItem(IDC_CHECK4)->EnableWindow(FALSE);
		GetDlgItem(IDC_CHECK5)->EnableWindow(FALSE);
		GetDlgItem(IDC_CHECK6)->EnableWindow(FALSE);
		GetDlgItem(IDC_CHECK7)->EnableWindow(FALSE);
		GetDlgItem(IDC_CHECK8)->EnableWindow(FALSE);
		GetDlgItem(IDC_CHECK9)->EnableWindow(FALSE);
		GetDlgItem(IDC_CHECK10)->EnableWindow(FALSE);
		GetDlgItem(IDC_CHECK11)->EnableWindow(FALSE);
		GetDlgItem(IDC_CHECK12)->EnableWindow(FALSE);
		GetDlgItem(IDC_CHECK13)->EnableWindow(FALSE);
		GetDlgItem(IDC_CHECK14)->EnableWindow(FALSE);
		GetDlgItem(IDC_CHECK15)->EnableWindow(FALSE);
		GetDlgItem(IDC_CHECK16)->EnableWindow(FALSE);
		GetDlgItem(IDC_CHECK17)->EnableWindow(FALSE);
		GetDlgItem(IDC_CHECK18)->EnableWindow(FALSE);
		GetDlgItem(IDC_CHECK19)->EnableWindow(FALSE);
		GetDlgItem(IDC_CHECK20)->EnableWindow(FALSE);
		GetDlgItem(IDC_CHECK21)->EnableWindow(FALSE);
		GetDlgItem(IDC_CHECK22)->EnableWindow(FALSE);
		GetDlgItem(IDC_CHECK23)->EnableWindow(FALSE);
		GetDlgItem(IDC_CHECK24)->EnableWindow(FALSE);
		GetDlgItem(IDC_CHECK25)->EnableWindow(FALSE);
		GetDlgItem(IDC_CHECK26)->EnableWindow(FALSE);
		GetDlgItem(IDC_CHECK27)->EnableWindow(FALSE);
		GetDlgItem(IDC_CHECK28)->EnableWindow(FALSE);
		GetDlgItem(IDC_CHECK29)->EnableWindow(FALSE);
		GetDlgItem(IDC_CHECK30)->EnableWindow(FALSE);
		GetDlgItem(IDC_CHECK31)->EnableWindow(FALSE);
		GetDlgItem(IDC_CHECK32)->EnableWindow(FALSE);
		GetDlgItem(IDC_CHECK33)->EnableWindow(FALSE);
		GetDlgItem(IDC_CHECK34)->EnableWindow(FALSE);
		GetDlgItem(IDC_CHECK35)->EnableWindow(FALSE);
		GetDlgItem(IDC_CHECK36)->EnableWindow(FALSE);
		GetDlgItem(IDC_CHECK37)->EnableWindow(FALSE);
		GetDlgItem(IDC_CHECK38)->EnableWindow(FALSE);

		UpdateData(FALSE);
	
	} else {

		GetDlgItem(IDC_CHECK1)->EnableWindow(TRUE);
		GetDlgItem(IDC_CHECK2)->EnableWindow(TRUE);
		GetDlgItem(IDC_CHECK3)->EnableWindow(TRUE);
		GetDlgItem(IDC_CHECK4)->EnableWindow(TRUE);
		GetDlgItem(IDC_CHECK5)->EnableWindow(TRUE);
		GetDlgItem(IDC_CHECK6)->EnableWindow(TRUE);
		GetDlgItem(IDC_CHECK7)->EnableWindow(TRUE);
		GetDlgItem(IDC_CHECK8)->EnableWindow(TRUE);
		GetDlgItem(IDC_CHECK9)->EnableWindow(TRUE);
		GetDlgItem(IDC_CHECK10)->EnableWindow(TRUE);
		GetDlgItem(IDC_CHECK11)->EnableWindow(TRUE);
		GetDlgItem(IDC_CHECK12)->EnableWindow(TRUE);
		GetDlgItem(IDC_CHECK13)->EnableWindow(TRUE);
		GetDlgItem(IDC_CHECK14)->EnableWindow(TRUE);
		GetDlgItem(IDC_CHECK15)->EnableWindow(TRUE);
		GetDlgItem(IDC_CHECK16)->EnableWindow(TRUE);
		GetDlgItem(IDC_CHECK17)->EnableWindow(TRUE);
		GetDlgItem(IDC_CHECK18)->EnableWindow(TRUE);
		GetDlgItem(IDC_CHECK19)->EnableWindow(TRUE);
		GetDlgItem(IDC_CHECK20)->EnableWindow(TRUE);
		GetDlgItem(IDC_CHECK21)->EnableWindow(TRUE);
		GetDlgItem(IDC_CHECK22)->EnableWindow(TRUE);
		GetDlgItem(IDC_CHECK23)->EnableWindow(TRUE);
		GetDlgItem(IDC_CHECK24)->EnableWindow(TRUE);
		GetDlgItem(IDC_CHECK25)->EnableWindow(TRUE);
		GetDlgItem(IDC_CHECK26)->EnableWindow(TRUE);
		GetDlgItem(IDC_CHECK27)->EnableWindow(TRUE);
		GetDlgItem(IDC_CHECK28)->EnableWindow(TRUE);
		GetDlgItem(IDC_CHECK29)->EnableWindow(TRUE);
		GetDlgItem(IDC_CHECK30)->EnableWindow(TRUE);
		GetDlgItem(IDC_CHECK31)->EnableWindow(TRUE);
		GetDlgItem(IDC_CHECK32)->EnableWindow(TRUE);
		GetDlgItem(IDC_CHECK33)->EnableWindow(TRUE);
		GetDlgItem(IDC_CHECK34)->EnableWindow(TRUE);
		GetDlgItem(IDC_CHECK35)->EnableWindow(TRUE);
		GetDlgItem(IDC_CHECK36)->EnableWindow(TRUE);
		GetDlgItem(IDC_CHECK37)->EnableWindow(TRUE);
		GetDlgItem(IDC_CHECK38)->EnableWindow(TRUE);
		UpdateData(FALSE);
	}
}

void CTestGLPDlg::OnBnClickedCheckStartup()
{
	UpdateData(TRUE);
	if (m_bStartup){
		GetDlgItem(IDC_LIST_START)->EnableWindow(TRUE);
		GetDlgItem(IDC_EDIT_STARTUP)->EnableWindow(TRUE);
		
	}else{
		GetDlgItem(IDC_LIST_START)->EnableWindow(FALSE);
		GetDlgItem(IDC_EDIT_STARTUP)->EnableWindow(FALSE);
		m_editStartup = "";
		for (int i=0; i<m_ListStartup.GetItemCount(); i++)
		{
			m_ListStartup.SetCheck(i,0);
		}

	}
	UpdateData(FALSE);
}
void CTestGLPDlg::OnBnClickedCheckFiles()
{
	UpdateData(TRUE);
	if (m_bFiles){
		GetDlgItem(IDC_CHECK_AUTONT)->EnableWindow(TRUE);
		GetDlgItem(IDC_CHECK_WINBAT)->EnableWindow(TRUE);
		GetDlgItem(IDC_CHECK_AUTOBAT)->EnableWindow(TRUE);
		GetDlgItem(IDC_CHECK_SYS)->EnableWindow(TRUE);
		GetDlgItem(IDC_CHECK_WININI)->EnableWindow(TRUE);
	}else{
		GetDlgItem(IDC_CHECK_AUTONT)->EnableWindow(FALSE);
		GetDlgItem(IDC_CHECK_WINBAT)->EnableWindow(FALSE);
		GetDlgItem(IDC_CHECK_AUTOBAT)->EnableWindow(FALSE);
		GetDlgItem(IDC_CHECK_SYS)->EnableWindow(FALSE);
		GetDlgItem(IDC_CHECK_WININI)->EnableWindow(FALSE);

		m_bAUTONT = FALSE;
		m_bWINBAT = FALSE;
		m_bAUTOBAT = FALSE;
		m_bSYS = FALSE;
		m_bWININI = FALSE;
	}
	UpdateData(FALSE);
}

void CTestGLPDlg::OnBnClickedModify()
{
	UpdateData(TRUE);

	if(m_bReg1)
	{
		m_strVirusName = GetVirusName();
		ModifyRegistry(HKEY_LOCAL_MACHINE,REG1);
	}
	if(m_bReg2)
	{
		m_strVirusName = _T("StubPath");
		CString strTemp;
		strTemp.Format(_T("%s\\{%s}"),REG2,GenerateUID());
		ModifyRegistry(HKEY_LOCAL_MACHINE, strTemp);
	}
	if(m_bReg3)
	{
		m_strVirusName ="{" +  GenerateUID() + "}";
		ModifyRegistry(HKEY_LOCAL_MACHINE, REG3);
	}
	if(m_bReg4)
	{
		m_strVirusName = GetVirusName();
		CString strTemp;
		strTemp.Format (_T("%s\\{%s}"),REG4, GenerateUID());
		ModifyRegistry(HKEY_LOCAL_MACHINE, strTemp);
	}
	if(m_bReg5)
	{
		m_strVirusName =_T("{") +  GenerateUID() + _T("}");
		ModifyRegistry(HKEY_LOCAL_MACHINE, REG5);
	}
	if(m_bReg6)
	{
		m_strVirusName =_T("{") +  GenerateUID() + _T("}");
		ModifyRegistry(HKEY_LOCAL_MACHINE, REG6);
	}
	if(m_bReg7)
	{
		m_strVirusName = GetVirusName();
		ModifyRegistry(HKEY_LOCAL_MACHINE,REG7);
	}
	if(m_bReg8)
	{
		m_strVirusName = GetVirusName();
		ModifyRegistry(HKEY_LOCAL_MACHINE,REG8);
	}
	if(m_bReg9)
	{
		m_strVirusName = GetVirusName();
		ModifyRegistry(HKEY_LOCAL_MACHINE,REG9);
	}
	if(m_bReg10)
	{
		m_strVirusName = GetVirusName();
		ModifyRegistry(HKEY_LOCAL_MACHINE,REG10);
	}
	if(m_bReg11)
	{
		m_strVirusName = GetVirusName();
		ModifyRegistry(HKEY_LOCAL_MACHINE,REG11);
	}
	if(m_bReg12)
	{
		m_strVirusName = GetVirusName();
		ModifyRegistry(HKEY_LOCAL_MACHINE,REG12);
	}
	if(m_bReg13)
	{
		m_strVirusName = GetVirusName();
		ModifyRegistry(HKEY_LOCAL_MACHINE,REG13);
	}
	if(m_bReg14)
	{
		m_strVirusName =_T("{") +  GenerateUID() + _T("}");
		ModifyRegistry(HKEY_LOCAL_MACHINE, REG14);
	}
	if(m_bReg15)
	{
		m_strVirusName =_T("Debugger");
		CString strSubKey = REG15;
		strSubKey = strSubKey + _T("\\") + GetVirusName() ;
		ModifyRegistry(HKEY_LOCAL_MACHINE, strSubKey);
	}
	if(m_bReg16)
	{
		m_strVirusName =_T("AppInit_DLLs");
		ModifyRegistry(HKEY_LOCAL_MACHINE, REG16);
	}
	if(m_bReg17)
	{
		m_strVirusName = _T("Shell");
		ModifyRegistry(HKEY_LOCAL_MACHINE, REG17);
	}
	if(m_bReg18)
	{
		m_strVirusName = _T("DLLName");
		CString strSubKey = REG18;
		strSubKey = strSubKey + _T("\\") + GetVirusName() ;
		ModifyRegistry(HKEY_LOCAL_MACHINE, strSubKey);
	}
	if(m_bReg19)
	{
		m_strVirusName = GetVirusName();
		ModifyRegistry(HKEY_LOCAL_MACHINE, REG19);
	}
	if(m_bReg20)
	{
		m_strVirusName = GetVirusName();
		ModifyRegistry(HKEY_LOCAL_MACHINE, REG20);
	}
	if(m_bReg21)
	{
		m_strVirusName = _T("StaticVxD");
		CString strSubKey = REG21;
		strSubKey = strSubKey + _T("\\") + GetVirusName() ;
		ModifyRegistry(HKEY_LOCAL_MACHINE, strSubKey);
	}
	if(m_bReg22)
	{
		m_strVirusName = _T("SCRNSAVE.EXE");
		ModifyUserKeys(HKEY_USERS, REG22);
	}
	if(m_bReg23)
	{
		m_strVirusName = GetVirusName();
		ModifyUserKeys(HKEY_USERS, REG23);
	}
	if(m_bReg24)
	{
		m_strVirusName = GetVirusName();
		ModifyUserKeys(HKEY_USERS, REG24);
	}
	if(m_bReg25)
	{
		m_strVirusName = GetVirusName();
		ModifyUserKeys(HKEY_USERS, REG25);
	}
	if(m_bReg26)
	{
		m_strVirusName = GetVirusName();
		ModifyUserKeys(HKEY_USERS, REG26);
	}
	if(m_bReg27)
	{
		m_strVirusName = GetVirusName();
		ModifyUserKeys(HKEY_USERS, REG27);
	}
	if(m_bReg28)
	{
		m_strVirusName = GetVirusName();
		ModifyUserKeys(HKEY_USERS, REG28);
	}
	if(m_bReg29)
	{
		m_strVirusName = _T("Run");
		ModifyUserKeys(HKEY_USERS, REG29);
		m_strVirusName = _T("Load");
		ModifyUserKeys(HKEY_USERS, REG29);
	}
	if(m_bReg30)
	{
		m_strVirusName = GetVirusName();
		ModifyUserKeys(HKEY_USERS, REG30);
	}
	if(m_bReg31)
	{
		m_strVirusName = GetVirusName();
		ModifyUserKeys(HKEY_USERS, REG31);
	}
	if(m_bReg32)
	{
		m_strVirusName = GetVirusName();
		ModifyUserKeys(HKEY_USERS, REG32);
	}
	if(m_bReg33)
	{
		m_strVirusName =_T("Userinit");
		ModifyRegistry(HKEY_LOCAL_MACHINE,REG33);
	}
	if(m_bReg34)
	{
		m_strVirusName = _T("");
		ModifyRegistry(HKEY_CLASSES_ROOT, REG34);
	}
	if(m_bReg35)
	{
		m_strVirusName = _T("");
		ModifyRegistry(HKEY_CLASSES_ROOT, REG35);
	}
	if(m_bReg36)
	{
		m_strVirusName = _T("");
		ModifyRegistry(HKEY_CLASSES_ROOT, REG36);
	}
	if(m_bReg37)
	{
		m_strVirusName = _T("");
		ModifyRegistry(HKEY_CLASSES_ROOT, REG37);
	}
	if(m_bReg38)
	{
		m_strVirusName = _T("");
		ModifyRegistry(HKEY_CLASSES_ROOT, REG38);
	}
	
	ModifyStartUpFodlers();
	
	TCHAR dirSystem[MAX_PATH + 1] = { 0 };
	GetSystemDirectory( dirSystem, MAX_PATH + 1 );//get the systems directory
	
	TCHAR dirWindows[MAX_PATH +1] = {0};
	GetWindowsDirectory( dirWindows, MAX_PATH +1);

	CString strTemp = dirWindows;
	CString dirSystemRoot = strTemp.Left(2);
	
	if (m_bAUTONT)
	{
		CString strAutoNT;
		strAutoNT.Format(_T("%s\\autoexec.nt"),dirSystem);
		ModifyBatchFile(strAutoNT);
	}
	if (m_bWINBAT)
	{
		CString strWinBAT;
		strWinBAT.Format(_T("%s\\winstart.bat"),dirSystemRoot);
		ModifyBatchFile(strWinBAT);
		strWinBAT.Format(_T("%s\\winstart.bat"),dirWindows);
		ModifyBatchFile(strWinBAT);
	}
	if (m_bAUTOBAT)
	{
		CString strAutoBAT;
		strAutoBAT.Format(_T("%s\\autoexec.bat"),dirSystemRoot);
		ModifyBatchFile(strAutoBAT);
	}
	if (m_bSYS)
	{
		ModifyINIFile(_T("system.ini"));
	}
	if (m_bWININI)
	{
		ModifyINIFile(_T("win.ini"));
	}

	UpdateData(FALSE);
	CEdit * result = (CEdit*) GetDlgItem(IDC_RESULT);
	result->LineScroll(result->GetLineCount(), 0);
	
	
	UpdateWindow();
}
CString CTestGLPDlg::GetVirusName()
{
	try {
		CFile virusFile(m_strFullVirusName, CFile::modeRead);
		return virusFile.GetFileName();
	} catch(CFileException* pEx) {
		m_strResult += _T("Error! can't get virus name, file does not exist or can not open file. \r\n");
	}
	return "";
}
CString CTestGLPDlg::GenerateUID()
{
	CString sATUID;
	unsigned char* psTemp = NULL;
	UUID Atuid;
	
	HRESULT hr;
	hr = UuidCreate(&Atuid);
	if (hr == RPC_S_OK)
	{
		hr = UuidToString(&Atuid, &psTemp);
		if (hr == RPC_S_OK)
		{
			sATUID = psTemp; 
			RpcStringFree(&psTemp);
		}
	}
	return sATUID;
}
void CTestGLPDlg::GetStartupFolders()
{

	m_strVector.clear(); 
	m_strRegVector.clear();
	m_strKeyBase.clear();
	m_strKeyPath.clear();
	m_strRegName.clear();

	HKEY hKey = NULL;
	HKEY hSubKey = NULL;
	CString RegPath;
	
	DWORD dwcPath = 1024;
	TCHAR szPath[1024];
	DWORD dwType = 0;

	if (ERROR_SUCCESS == RegOpenKeyEx(HKEY_LOCAL_MACHINE, STARTUP1, 0, KEY_READ, &hKey))
	{
		
			if (ERROR_SUCCESS == RegQueryValueEx(hKey,
									_T("Common Startup"),
									NULL,
									&dwType,
									LPBYTE (szPath),
									&dwcPath))
			{
				RegPath = CString(_T("HKEY_LOCAL_MACHINE\\"))+STARTUP1+_T("\\Common Startup"); 
				m_strVector.push_back(szPath);
				m_strRegVector.push_back(RegPath);
				m_strKeyBase.push_back(HKEY_LOCAL_MACHINE);
				m_strKeyPath.push_back(STARTUP1);
				m_strRegName.push_back(_T("Common Startup"));
			}
		dwcPath = 1024;
		szPath[1024];
		dwType = 0;

			if (ERROR_SUCCESS == RegQueryValueEx(hKey,
									_T("Startup"),
									NULL,
									&dwType,
									LPBYTE (szPath),
									&dwcPath) )
			{
				RegPath = CString(_T("HKEY_LOCAL_MACHINE\\"))+STARTUP1+_T("\\Startup");
				m_strVector.push_back(szPath);
				m_strRegVector.push_back(RegPath);
				m_strKeyBase.push_back(HKEY_LOCAL_MACHINE);
				m_strKeyPath.push_back(STARTUP1);
				m_strRegName.push_back(_T("Startup"));
			}
	RegCloseKey(hKey);
	}
	
	if (ERROR_SUCCESS == RegOpenKeyEx(HKEY_USERS, NULL, 0 , KEY_READ, &hKey))
	{
		DWORD dwIndex = 0;
		TCHAR szName[MAX_PATH];
		DWORD dwBuffSize = MAX_PATH;
		LONG retVal;
		
		do {
			
			retVal = RegEnumKeyEx(hKey, dwIndex, szName, &dwBuffSize, NULL, NULL, NULL, NULL);
			if (ERROR_SUCCESS == retVal)
			{
				CString strName(szName);
				CString strUserSubKey = strName + _T("\\")+ STARTUP1;
				if (ERROR_SUCCESS == RegOpenKeyEx(HKEY_USERS, strUserSubKey, 0, KEY_READ, &hSubKey))
				{
					if (ERROR_SUCCESS == RegQueryValueEx(hSubKey,
									_T("Startup"),
									NULL,
									&dwType,
									LPBYTE (szPath),
									&dwcPath))
					{	
						RegPath = CString(_T("HKEY_USERS\\"))+ strUserSubKey +_T("Startup");
						m_strVector.push_back(szPath);
						m_strRegVector.push_back(RegPath);
						m_strKeyBase.push_back(HKEY_USERS);
						m_strKeyPath.push_back(strUserSubKey);
						m_strRegName.push_back(_T("Startup"));
					}
						RegCloseKey(hSubKey);
				}
				
			}
		dwIndex++;
		dwBuffSize = MAX_PATH;
		dwcPath = 1024;
		szPath[1024];
		dwType = 0;
		
		}while(ERROR_NO_MORE_ITEMS != retVal);
	}
	RegCloseKey(hKey);
	
}
void CTestGLPDlg::UpdateStartupFolders()
{
	//GetStartupFolders();

	for(int i=0; i < m_strVector.size(); i++)
	{
		CString strStartup ( m_strVector.at(i));
		CString strRegStartup (m_strRegVector.at(i));
		m_ListStartup.SetItemText(i, 1, strStartup);
		m_ListStartup.SetItemText(i, 0, strRegStartup);
	}

}
//Modify a single registry
BOOL CTestGLPDlg::ModifyRegistry(HKEY keybase,CString strSubKey)
{
	
	HKEY hKey = NULL;
	DWORD dwDisposition = 0;
	LPCTSTR strVirusPath = m_strFullVirusName;

	if (ERROR_SUCCESS == RegCreateKeyEx(keybase,
                                             strSubKey,
                                             0,
                                             NULL,
                                             REG_OPTION_NON_VOLATILE,
                                             KEY_WRITE,
                                             NULL,
                                             &hKey,
                                             &dwDisposition))
    {
		if (ERROR_SUCCESS == RegSetValueEx(hKey,
											m_strVirusName,
											0,
											REG_SZ, 
											LPBYTE(strVirusPath), 
											(lstrlen(strVirusPath) + 1) * sizeof(TCHAR)))
		{
			m_iTotal++;
			CString strTemp;
			strTemp.Format(_T("Success! Set Registry %s\\%s name: %s to value %s \r\n") , "HKLM", strSubKey, m_strVirusName, m_strFullVirusName); 
			m_strResult += strTemp;
			RegCloseKey(hKey);
			return TRUE;
		}else{
			CString strTemp;
			strTemp.Format(_T("Failed to Set Registry %s\\%s name: %s to value %s \r\n") , "HKLM", strSubKey, m_strVirusName, m_strFullVirusName); 
			m_strResult += strTemp;
		}
	
	}else {
		CString strTemp;
		strTemp.Format(_T("Failed to Set Registry %s\\%s name: %s to value %s \r\n") , "HKLM", strSubKey, m_strVirusName, m_strFullVirusName); 
		m_strResult += strTemp;		
	}
	RegCloseKey(hKey);
	return FALSE;
}
//Modify all subkeys 
/* Not used in the project
BOOL CTestGLPDlg::ModifySubKeys(HKEY keybase, CString strSubKey)
{
	HKEY hKey = NULL;
	HKEY hSubKey = NULL;
	DWORD dwDisposition = 0;
	LPCTSTR strVirusPath = m_strFullVirusName;


	DWORD dwIndex = 0;
	TCHAR szName[MAX_PATH];
	DWORD dwBuffSize = MAX_PATH;
	LONG retVal;

	if (ERROR_SUCCESS == RegOpenKeyEx(keybase,strSubKey,0,KEY_ALL_ACCESS, &hKey))
    {
		do {
			retVal = RegEnumKeyEx(hKey, dwIndex, szName, &dwBuffSize, NULL, NULL, NULL, NULL);
			if (retVal == ERROR_SUCCESS)
			{
				CString strName( szName );
				if (ERROR_SUCCESS == RegCreateKeyEx(hKey,
                                             strName,
                                             0,
                                             NULL,
                                             REG_OPTION_NON_VOLATILE,
                                             KEY_WRITE,
                                             NULL,
                                             &hSubKey,
                                             &dwDisposition))
				{
						if(ERROR_SUCCESS ==	RegSetValueEx(hSubKey,
											m_strVirusName,
											0,
											REG_SZ, 
											LPBYTE(strVirusPath), 
											(lstrlen(strVirusPath) + 1) * sizeof(TCHAR)))
						{ 
							m_iTotal++;	
							RegCloseKey(hSubKey);
						}	
				}
			}
			dwIndex++;
			dwBuffSize = MAX_PATH;
		}while(ERROR_NO_MORE_ITEMS != retVal);
	}
	
	RegCloseKey(hKey);
	return TRUE;
}
*/
//Modify all Users Keys
BOOL CTestGLPDlg::ModifyUserKeys(HKEY keybase, CString strSubKey)
{
	HKEY hKey = NULL;
	HKEY hSubKey = NULL;
	DWORD dwDisposition = 0;
	LPCTSTR strVirusPath = m_strFullVirusName;


	DWORD dwIndex = 0;
	TCHAR szName[MAX_PATH];
	DWORD dwBuffSize = MAX_PATH;
	LONG retVal;
	
	if (ERROR_SUCCESS == RegOpenKeyEx(keybase, NULL, 0, KEY_ALL_ACCESS, &hKey))
	{
		do{
			retVal = RegEnumKeyEx(hKey, dwIndex, szName, &dwBuffSize, NULL, NULL, NULL, NULL);
			if (retVal == ERROR_SUCCESS)
			{
				CString strName(szName);
				CString strUserSubKey = strName + _T("\\")+ strSubKey;
				if (ERROR_SUCCESS == RegCreateKeyEx(keybase,
                                             strUserSubKey,
                                             0,
                                             NULL,
                                             REG_OPTION_NON_VOLATILE,
                                             KEY_WRITE,
                                             NULL,
                                             &hSubKey,
                                             &dwDisposition))
				{
					if(ERROR_SUCCESS == RegSetValueEx(hSubKey,
														m_strVirusName,
														0,
														REG_SZ,
														LPBYTE(strVirusPath),
														(lstrlen(strVirusPath) + 1) * sizeof(TCHAR)))
					{
						m_iTotal++;	
						CString strTemp;
						strTemp.Format(_T("Success! Set Registry %s\\%s name: %s to value %s \r\n") , "HKUS", strUserSubKey, m_strVirusName, m_strFullVirusName);
						m_strResult+=strTemp;
						RegCloseKey(hSubKey);
					}else{
					
						CString strTemp;
						strTemp.Format(_T("Failed to Set Registry %s\\%s name: %s to value %s \r\n") , "HKUS", strUserSubKey, m_strVirusName, m_strFullVirusName);
						m_strResult+=strTemp;
					}
				}else{
					CString strTemp;
					strTemp.Format(_T("Failed to Set Registry %s\\%s name: %s to value %s \r\n") , "HKUS", strUserSubKey, m_strVirusName, m_strFullVirusName);
					m_strResult+=strTemp;
				}
			}
			dwIndex++;
			dwBuffSize = MAX_PATH;
		}while(ERROR_NO_MORE_ITEMS != retVal);
	}
	return TRUE;
}


BOOL CTestGLPDlg::ModifyStartUpFodlers()
{

	for (int i=0; i<m_ListStartup.GetItemCount(); i++)
	{
		if (m_ListStartup.GetCheck(i))
		{
			HKEY hKey = NULL;
			DWORD dwDisposition = 0;
			LPCTSTR strStartup = m_editStartup;


			if (ERROR_SUCCESS == RegOpenKeyEx( m_strKeyBase.at(i),
                                             m_strKeyPath.at(i),
											 0,
											 KEY_ALL_ACCESS,
											 &hKey))
			{
				if (ERROR_SUCCESS == RegSetValueEx(hKey,
											m_strRegName.at(i),
											0,
											REG_SZ, 
											LPBYTE(strStartup), 
											(lstrlen(strStartup) + 1) * sizeof(TCHAR)))
				{
				
					CString strTemp;
					strTemp.Format(_T("Success! Set Startup Folder %s at Registry: %s \r\n") , strStartup, m_strRegVector.at(i) );
					m_strResult+=strTemp;
					m_iTotal++;
				}else{
					CString strTemp;
					strTemp.Format(_T("Fail to Set Startup Folder %s at Registry: %s \r\n") , strStartup, m_strRegVector.at(i) );
					m_strResult+=strTemp;
				}
			}else{
				CString strTemp;
				strTemp.Format(_T("Fail to Set Startup Folder %s at Registry: %s \r\n") , strStartup, m_strRegVector.at(i) );
				m_strResult+=strTemp;
			}
		}
	}
		GetStartupFolders();
		UpdateStartupFolders();
		//UpdateData(FALSE);
		return TRUE;
}
BOOL CTestGLPDlg::ModifyBatchFile(CString strFileName)
{
	CString newline = "\r\n";

	CFile cFile;
	if (cFile.Open(strFileName, CFile::modeCreate | CFile::modeNoTruncate | CFile::modeWrite)){
		cFile.Seek(0,CFile::end);
		cFile.Write(newline, newline.GetLength());
		cFile.Write(m_strFullVirusName, m_strFullVirusName.GetLength());
		cFile.Close();
	
		CString strTemp;
		strTemp.Format(_T("Success! Added %s in batch file: %s \r\n") , m_strFullVirusName, strFileName);
		m_strResult+=strTemp;
		m_iTotal++;	
		return TRUE;
	}else{
		CString strTemp;
		strTemp.Format(_T("Fail to Add %s in batch file: %s \r\n") , m_strFullVirusName, strFileName);
		m_strResult+=strTemp;

	}
	return FALSE;
}
BOOL CTestGLPDlg::ModifyINIFile(CString strFileName)
{
	if (!strFileName.Compare(_T("system.ini")))
	{
		if ( WritePrivateProfileString(_T("boot"), _T("shell"), m_strFullVirusName.GetBuffer(), strFileName) 
			&& WritePrivateProfileString(_T("boot"), _T("scnsave.exe"), m_strFullVirusName.GetBuffer(), strFileName))
		{
			CString strTemp;
			strTemp.Format(_T("Success! Added %s in INI file: %s boot section: shell and scrnsave.exe keys \r\n") , m_strFullVirusName, strFileName);
			m_strResult+=strTemp;
			
			m_iTotal++;	
			m_iTotal++;	
			return TRUE;
		}else {
			CString strTemp;
			strTemp.Format(_T("Fail to Add %s in INI file: %s boot section: shell and scrnsave.exe keys \r\n") , m_strFullVirusName, strFileName);
			m_strResult+=strTemp;
		}
	}else if(!strFileName.Compare(_T("win.ini")))
	{
		if ( WritePrivateProfileString(_T("windows"), _T("load"), m_strFullVirusName.GetBuffer(), strFileName) 
			&& WritePrivateProfileString(_T("windows"), _T("run"), m_strFullVirusName.GetBuffer(), strFileName))
		{
			CString strTemp;
			strTemp.Format(_T("Success! Added %s in INI file: %s boot section: load and run keys \r\n") , m_strFullVirusName, strFileName);
			m_strResult+=strTemp;
			m_iTotal++;	
			m_iTotal++;	
			return TRUE;
		}else{
			CString strTemp;
			strTemp.Format(_T("Fail to Add %s in INI file: %s boot section: load and run keys \r\n") , m_strFullVirusName, strFileName);
			m_strResult+=strTemp;
		}
	}

	return FALSE;
}


void CTestGLPDlg::OnBnClickedClear()
{
	m_strResult = "";
	UpdateData(FALSE);
}

void CTestGLPDlg::OnBnClickedSave()
{
	CString strTemp;
	SYSTEMTIME systemTime;
	GetLocalTime(&systemTime);
	

	TCHAR szFilters[] = _T("Microsoft txt File (*.txt)|*.txt;*.ini;*.log|All Files (*.*)|*.*||");
	CFileDialog dlg (FALSE, _T("*.txt; *.ini;*.log"), _T("*.txt;*.ini;*.log"),
		OFN_FILEMUSTEXIST | OFN_HIDEREADONLY, szFilters);
	if ( IDOK == dlg.DoModal() )
	{
	    CString strOutputFile = dlg.GetPathName();

		CFile cfile;
		cfile.Open(strOutputFile, CFile::modeCreate | CFile::modeNoTruncate | CFile::modeWrite);
		cfile.Seek(0,CFile::end);

		strTemp.Format( _T("----TestGLP Start Log----  %d-%d-%d  %d:%d:%d\r\n" ), systemTime.wMonth,systemTime.wDay,systemTime.wYear,systemTime.wHour,systemTime.wMinute,systemTime.wSecond);
		cfile.Write(strTemp,strTemp.GetLength());

		cfile.Write(m_strResult, m_strResult.GetLength());
		strTemp = _T("----END of Log---- \r\n");
		cfile.Write(strTemp, strTemp.GetLength());
		cfile.Close();

	}
}
