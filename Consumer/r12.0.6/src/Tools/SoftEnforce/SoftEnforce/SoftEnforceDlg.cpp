// SoftEnforceDlg.cpp : implementation file
//

#include "stdafx.h"
#include "SoftEnforce.h"
#include "SoftEnforceDlg.h"
#include ".\softenforcedlg.h"
#include "Enforce.h"

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


// CSoftEnforceDlg dialog



CSoftEnforceDlg::CSoftEnforceDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CSoftEnforceDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CSoftEnforceDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_MAIN_LIST, m_listView);
	DDX_Control(pDX, IDC_LIST_OUTPUT, m_listOut);
}

BEGIN_MESSAGE_MAP(CSoftEnforceDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_BUTTON_LOAD, OnBnClickedButtonLoad)
	ON_BN_CLICKED(IDC_BUTTON_RUN, OnBnClickedButtonRun)
	ON_BN_CLICKED(IDC_BUTTON_CREATE, OnBnClickedButtonCreate)
	ON_BN_CLICKED(IDC_BUTTON_EDIT, OnBnClickedButtonEdit)
END_MESSAGE_MAP()


// CSoftEnforceDlg message handlers

BOOL CSoftEnforceDlg::OnInitDialog()
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


	m_listView.ModifyStyle(NULL, LVS_REPORT);
	m_listView.InsertColumn(1, _T("Product Name"), LVCFMT_LEFT, 300);
	m_listView.InsertColumn(2, _T("Key"), LVCFMT_LEFT, 300);
	m_listView.SetExtendedStyle(LVS_EX_GRIDLINES | LVS_EX_FULLROWSELECT |LVS_EX_CHECKBOXES  );



	m_listOut.ModifyStyle(NULL, LVS_REPORT);
	m_listOut.InsertColumn(1, _T("Detect"), LVCFMT_LEFT, 100);
	m_listOut.InsertColumn(2, _T("Product Name"), LVCFMT_LEFT, 300);
	m_listOut.InsertColumn(3, _T("Key"), LVCFMT_LEFT, 200);
	m_listOut.SetExtendedStyle(LVS_EX_GRIDLINES | LVS_EX_FULLROWSELECT  );

	
	

	m_ImageList.Create(16, 16, ILC_COLOR, 0, 2);

		
	m_ImageList.Add(AfxGetApp()->LoadIcon(IDI_ICON_PASS));
	m_ImageList.Add(AfxGetApp()->LoadIcon(IDI_ICON_FAIL));


	m_listOut.SetImageList(&m_ImageList, 1);
	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CSoftEnforceDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

void CSoftEnforceDlg::OnPaint() 
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
HCURSOR CSoftEnforceDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CSoftEnforceDlg::OnBnClickedButtonLoad()
{
	m_listView.DeleteAllItems();
	Enforce ef;
	ef.LoadData();

	for (int i = 0; i < ef.m_vData.size(); i++)
	{
		for (int j = 1; j < ef.m_vData.at(i).size(); j++)
		{
			const int IDX = m_listView.InsertItem(0, _T(""));
			

			m_listView.SetItemText(IDX, 0, ef.m_vData.at(i).at(0));
			m_listView.SetItemText(IDX, 1, ef.m_vData.at(i).at(j));
		}

	}
}

void CSoftEnforceDlg::OnBnClickedButtonRun()
{
	m_listOut.DeleteAllItems();
	Enforce ef;
	ef.LoadData();
	

	for (int i = 0; i < ef.m_vData.size(); i++)
	{
		for (int j = 1; j < ef.m_vData.at(i).size(); j++)
		{
		
			if (ef.SoftCheck(ef.m_vData.at(i).at(j)))
			{
				const int IDX = m_listOut.InsertItem(0, _T("FOUND!"),1);
				m_listOut.SetItemText(IDX, 1, ef.m_vData.at(i).at(0));
				m_listOut.SetItemText(IDX, 2, ef.m_vData.at(i).at(j));

				
			}
			else
			{
				const int IDX = m_listOut.InsertItem(0, _T("NOT FOUND!"),0);			
				m_listOut.SetItemText(IDX, 1, ef.m_vData.at(i).at(0));
				m_listOut.SetItemText(IDX, 2, ef.m_vData.at(i).at(j));
				
			}
				


		}

	}

}

void CSoftEnforceDlg::OnBnClickedButtonCreate()
{
	Enforce ef;
	ef.LoadData();

	int count = 0;
	CString strTemp;

	for (int i=0; i<m_listView.GetItemCount(); i++)
	{
		if (m_listView.GetCheck(i))
		{
			ef.CreateKeys(m_listView.GetItemText(i,1));
			count ++;
		}
	}

	strTemp.Format(_T("%d key(s) modified"), count);
	MessageBox(strTemp, _T("Done"), MB_ICONEXCLAMATION| MB_OK);
	count = 0;
	ef.LoadData();
}

void CSoftEnforceDlg::OnBnClickedButtonEdit()
{
	Enforce ef;
	ef.EditData();
}
