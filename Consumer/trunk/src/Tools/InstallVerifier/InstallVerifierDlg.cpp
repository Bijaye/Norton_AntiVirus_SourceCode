////////////////////////
//
// PROPRIETARY / CONFIDENTIAL.
// Use of this product is subject to license terms.
// Copyright © 2006 Symantec Corporation.
// All rights reserved.
//
/////////////////////////////////////////////////////////////////////////////////////////////////

// InstallVerifierDlg.cpp : implementation file
//

#include "stdafx.h"
#include "InstallVerifier.h"
#include "InstallVerifierDlg.h"
#include ".\installverifierdlg.h"
#include "XMLHelper.h"

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


// CInstallVerifierDlg dialog



CInstallVerifierDlg::CInstallVerifierDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CInstallVerifierDlg::IDD, pParent)
	, m_strMSIPath(_T("c:\\nav.msi"))
	, m_strXMLPath(_T("c:\\a.xml"))
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CInstallVerifierDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_MSI_PATH, m_strMSIPath);
	DDX_Text(pDX, IDC_EDIT_XML_PATH, m_strXMLPath);
}

BEGIN_MESSAGE_MAP(CInstallVerifierDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_BUTTON_GEN_XML, OnBnClickedButtonGenXml)
	ON_BN_CLICKED(IDC_BUTTON_MSI_PATH, OnBnClickedButtonMsiPath)
	ON_BN_CLICKED(IDC_BUTTON_XML_PATH, OnBnClickedButtonXmlPath)
END_MESSAGE_MAP()


// CInstallVerifierDlg message handlers

BOOL CInstallVerifierDlg::OnInitDialog()
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

void CInstallVerifierDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

void CInstallVerifierDlg::OnPaint() 
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
HCURSOR CInstallVerifierDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CInstallVerifierDlg::OnBnClickedButtonGenXml()
{
	UpdateData(true);
	CXMLHelper xmlHelper;
	xmlHelper.MSIToXML( m_strMSIPath, m_strXMLPath);

}

void CInstallVerifierDlg::OnBnClickedButtonMsiPath()
{
	m_strMSIPath = GetFileName("msi", "Microsoft Installers (*.msi)|*.msi|", "Select MSI to parse" ) ;
	UpdateData(false);
}
void CInstallVerifierDlg::OnBnClickedButtonXmlPath()
{
	m_strXMLPath = GetFileName("ini", "Initialization Files (*.xml)|*.xml|", "Select XML File to write to") ;
	UpdateData(false);
}

CString CInstallVerifierDlg::GetFileName(CString strExtension, CString strFiletype, CString strTitle)
{
	CString strFilename = "";

	strFiletype += "All Files (*.*)|*.*|";
	CFileDialog file(TRUE, strExtension,NULL,OFN_EXPLORER|OFN_HIDEREADONLY|OFN_NOTESTFILECREATE, strFiletype);
	file.m_ofn.lpstrTitle  = strTitle;

	int open = file.DoModal();
	if(open == IDOK) 
		strFilename = file.GetPathName();

	return strFilename;
}

