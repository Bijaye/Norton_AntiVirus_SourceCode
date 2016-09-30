// HomePageConfigDlg.cpp : implementation file
//

#include "stdafx.h"
#include "HomePageConfig.h"
#include "HomePageConfigDlg.h"
#include "HomePageConst.h"
#include ".\homepageconfigdlg.h"

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


// CHomePageConfigDlg dialog



CHomePageConfigDlg::CHomePageConfigDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CHomePageConfigDlg::IDD, pParent)
	, m_bLocal(FALSE)
	, m_bCurrent(FALSE)
	, m_strLocal(_T(""))
	, m_strCurrent(_T(""))
	, m_iNum(0)
	, m_bStress(FALSE)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CHomePageConfigDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Check(pDX, IDC_CHECK_LOCAL, m_bLocal);
	DDX_Check(pDX, IDC_CHECK_CURRENT, m_bCurrent);
	DDX_Text(pDX, IDC_EDIT_LOCAL, m_strLocal);
	DDX_Text(pDX, IDC_EDIT_CURRENT, m_strCurrent);
	DDX_Text(pDX, IDC_EDIT_NUM, m_iNum);
	DDX_Check(pDX, IDC_CHECK_STRESS, m_bStress);
}

BEGIN_MESSAGE_MAP(CHomePageConfigDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_CHECK_LOCAL, OnBnClickedCheckLocal)
	ON_BN_CLICKED(IDC_CHECK_CURRENT, OnBnClickedCheckCurrent)
	ON_BN_CLICKED(IDC_BUTTON_APPLY, OnBnClickedButtonApply)
	ON_BN_CLICKED(IDC_BUTTON_REFRESH, OnBnClickedButtonRefresh)
	ON_BN_CLICKED(IDC_CHECK_STRESS, OnBnClickedCheckStress)
END_MESSAGE_MAP()


// CHomePageConfigDlg message handlers

BOOL CHomePageConfigDlg::OnInitDialog()
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
	LoadHomePageValues();


	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CHomePageConfigDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

void CHomePageConfigDlg::OnPaint() 
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
HCURSOR CHomePageConfigDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}
void CHomePageConfigDlg::LoadHomePageValues()
{
	UpdateData(TRUE);
	HKEY hKeyL = NULL;

	DWORD dwcPathL = 2048;
	TCHAR szPathL[2048];
	DWORD dwTypeL = 0;

	if (ERROR_SUCCESS == RegOpenKeyEx(HKEY_LOCAL_MACHINE, HOMEPAGEKEY, 0, KEY_READ, &hKeyL))
	{
		if (ERROR_SUCCESS == RegQueryValueEx(hKeyL,
									HOMEPAGENAME,
									NULL,
									&dwTypeL,
									LPBYTE (szPathL),
									&dwcPathL))
		{
			m_strLocal = szPathL;
		}
		else
		{
			m_strLocal = _T("");
			MessageBox("Error! fail to query", "Done", MB_ICONERROR| MB_OK);
		}
	}
	else{
		m_strLocal = _T("");
		MessageBox("Error! fail to open", "Done", MB_ICONERROR| MB_OK);
	}

	HKEY hKeyC = NULL;

	DWORD dwcPathC = 1024;
	TCHAR szPathC[1024];
	DWORD dwTypeC = 0;
	if (ERROR_SUCCESS == RegOpenKeyEx(HKEY_CURRENT_USER, HOMEPAGEKEY, 0, KEY_READ, &hKeyC))
	{
		if (ERROR_SUCCESS == RegQueryValueEx(hKeyC,
									HOMEPAGENAME,
									NULL,
									&dwTypeC,
									LPBYTE (szPathC),
									&dwcPathC))
		{
			m_strCurrent = szPathC;
		}
		else
		{
			m_strCurrent = _T("");
			MessageBox("Error! fail to query", "Done", MB_ICONERROR| MB_OK);
		}
	}
	else
	{
		m_strCurrent = _T("");
		MessageBox("Error! fail to open", "Done", MB_ICONERROR| MB_OK);

	}
	UpdateData(FALSE);
	if (hKeyL)
		RegCloseKey(hKeyL);
	if (hKeyC)
		RegCloseKey(hKeyC);

}
void CHomePageConfigDlg::OnBnClickedCheckLocal()
{	
	UpdateData(TRUE);
	if(m_bLocal)
	{
		GetDlgItem(IDC_EDIT_LOCAL)->EnableWindow(TRUE);
		
	}else
		GetDlgItem(IDC_EDIT_LOCAL)->EnableWindow(FALSE);

	UpdateData(FALSE);
}

void CHomePageConfigDlg::OnBnClickedCheckCurrent()
{
	
	UpdateData(TRUE);
	if(m_bCurrent)
	{
		GetDlgItem(IDC_EDIT_CURRENT)->EnableWindow(TRUE);
		
	}else
		GetDlgItem(IDC_EDIT_CURRENT)->EnableWindow(FALSE);

	UpdateData(FALSE);
}

void CHomePageConfigDlg::OnBnClickedCheckStress()
{
	UpdateData(TRUE);
	if(m_bStress)
	{
		GetDlgItem(IDC_EDIT_NUM)->EnableWindow(TRUE);
	}else
		GetDlgItem(IDC_EDIT_NUM)->EnableWindow(FALSE);

	UpdateData(TRUE);

}

void CHomePageConfigDlg::OnBnClickedButtonApply()
{
	int i;
	UpdateData(TRUE);
	if (m_bStress){

		for (i= 0; i<m_iNum; i++)
		{
			m_strLocal.Format( "%d", i ); 
			m_strCurrent.Format( "%d", i );

			SetHomePageValues();
		}
		
	}
	else
		SetHomePageValues();
}

void CHomePageConfigDlg::OnBnClickedButtonRefresh()
{
	UpdateData(TRUE);
	GetDlgItem(IDC_EDIT_CURRENT)->EnableWindow(FALSE);
	GetDlgItem(IDC_EDIT_LOCAL)->EnableWindow(FALSE);
	GetDlgItem(IDC_EDIT_NUM)->EnableWindow(FALSE);
	m_bStress= FALSE;
	m_bLocal = FALSE;
	m_bCurrent = FALSE;
	UpdateData(FALSE);
	
	LoadHomePageValues();
	UpdateData(FALSE);
	
}

void CHomePageConfigDlg::SetHomePageValues()
{
	//UpdateData(TRUE);
	HKEY hKey = NULL;
	DWORD dwDisposition = 0;

	if (m_bLocal)
	{
		LPCTSTR strLocalKey = m_strLocal;
		if (ERROR_SUCCESS == RegCreateKeyEx(HKEY_LOCAL_MACHINE,
                                             HOMEPAGEKEY,
                                             0,
                                             NULL,
                                             REG_OPTION_NON_VOLATILE,
                                             KEY_WRITE,
                                             NULL,
                                             &hKey,
                                             &dwDisposition))
		{
			if (ERROR_SUCCESS == RegSetValueEx(hKey,
											HOMEPAGENAME,
											0,
											REG_SZ, 
											LPBYTE(strLocalKey), 
											(lstrlen(m_strLocal) + 1) * sizeof(TCHAR)))
			{
				if(!m_bStress)
					MessageBox("Home Page Value at HKEY_LOCAL_MACHINE is changed", "Done", MB_ICONEXCLAMATION| MB_OK);

			}
			else
			{
				MessageBox("Error! Fail to set Home Page Value at HKEY_LOCAL_MACHINE", "Done", MB_ICONERROR| MB_OK);
			}
			
		}else
			MessageBox("Error! Fail to set Home Page Value at HKEY_LOCAL_MACHINE", "Done", MB_ICONERROR| MB_OK);

		RegCloseKey(hKey);
	}


	if (m_bCurrent)
	{
		LPCTSTR strCurrentKey = m_strCurrent;

		if (ERROR_SUCCESS == RegCreateKeyEx(HKEY_CURRENT_USER,
                                             HOMEPAGEKEY,
                                             0,
                                             NULL,
                                             REG_OPTION_NON_VOLATILE,
                                             KEY_WRITE,
                                             NULL,
                                             &hKey,
                                             &dwDisposition))
		{
			if (ERROR_SUCCESS == RegSetValueEx(hKey,
											HOMEPAGENAME,
											0,
											REG_SZ, 
											LPBYTE(strCurrentKey), 
											(lstrlen(m_strCurrent) + 1) * sizeof(TCHAR)))
			{
				if(!m_bStress)
					MessageBox("Home Page Value at HKEY_CURRENT_USER is changed", "Done", MB_ICONEXCLAMATION| MB_OK);
			}
			else
			{
					MessageBox("Error! Fail to set Home Page Value at HKEY_CURRENT_USER", "Done", MB_ICONERROR| MB_OK);
			}
			
		}
		else
			MessageBox("Error! Fail to set Home Page Value at HKEY_CURRENT_USER", "Done", MB_ICONERROR| MB_OK);
		RegCloseKey(hKey);
	}
	UpdateData(FALSE);
}


