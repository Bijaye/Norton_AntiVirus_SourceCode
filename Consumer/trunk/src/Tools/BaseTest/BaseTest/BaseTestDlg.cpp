////////////////////////
//
// PROPRIETARY / CONFIDENTIAL.
// Use of this product is subject to license terms.
// Copyright © 2006 Symantec Corporation.
// All rights reserved.
//
/////////////////////////////////////////////////////////////////////////////////////////////////

// BaseTestDlg.cpp : implementation file
//

#include "stdafx.h"
#include "BaseTest.h"
#include "BaseTestDlg.h"
#include ".\basetestdlg.h"
#include <vector>
#include "Imagehlp.h"
#include "EnumProcess.h"
#include "EditProcessDlg.h"



#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CAboutDlg dialog used for App About
void Start(void* dummy)
{
	CBaseTestDlg* dlg = (CBaseTestDlg*) dummy;
	while(1){
		
		dlg->m_listView.DeleteAllItems();
		for (int i = 0; i< dlg->m_vProcessName.size(); i++)
		{
			dlg->EnumProcessModules(dlg->m_vProcessName.at(i));
		}
	
		Sleep(1000);
		if (dlg->m_bStop)
			_endthread();
	
	}

}
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


// CBaseTestDlg dialog


//IMPLEMENT_DYNAMIC(CBaseTestDlg, CDialog)
CBaseTestDlg::CBaseTestDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CBaseTestDlg::IDD, pParent)
	, m_bShowAll(FALSE)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CBaseTestDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST1, m_listView);
	DDX_Check(pDX, IDC_CHECK_SHOWALL, m_bShowAll);
}

BEGIN_MESSAGE_MAP(CBaseTestDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_BUTTON_FILE, OnBnClickedButtonFile)
	ON_BN_CLICKED(IDC_BUTTON_EDIT, OnBnClickedButtonEdit)
	ON_BN_CLICKED(IDC_BUTTON_RUN, OnBnClickedButtonRun)
	ON_BN_CLICKED(IDC_BUTTON_START, OnBnClickedButtonStart)
	ON_BN_CLICKED(IDC_BUTTON_STOP, OnBnClickedButtonStop)
END_MESSAGE_MAP()


// CBaseTestDlg message handlers

BOOL CBaseTestDlg::OnInitDialog()
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
	m_listView.InsertColumn(1, _T("Process"), LVCFMT_LEFT, 100);
	m_listView.InsertColumn(2, _T("Module"), LVCFMT_LEFT, 120);
	m_listView.InsertColumn(3, _T("Base"), LVCFMT_LEFT, 100);
	m_listView.InsertColumn(4, _T("Loaded"), LVCFMT_LEFT, 100);
	m_listView.SetExtendedStyle(LVS_EX_GRIDLINES | LVS_EX_FULLROWSELECT  );

	// apps in CC
	m_vProcessName.push_back(_T("ccapp.exe"));
	m_vProcessName.push_back(_T("ccSetMgr.exe"));
	m_vProcessName.push_back(_T("ccEvtMgr.exe"));
	m_vProcessName.push_back(_T("ccProxy.exe"));
	m_vProcessName.push_back(_T("ccLgview.exe"));
	m_vProcessName.push_back(_T("NMain.exe"));
	m_vProcessName.push_back(_T("SMNLnch.exe"));
	m_vProcessName.push_back(_T("SNDInst.exe"));
	m_vProcessName.push_back(_T("sndsrvc.exe"));
	m_vProcessName.push_back(_T("IdsInst.exe"));
	m_vProcessName.push_back(_T("symlcsvc.exe"));

	// NSC
	m_vProcessName.push_back(_T("NSCVMINI.exe"));
	m_vProcessName.push_back(_T("UsrPrmpt.exe"));
	m_vProcessName.push_back(_T("NSCSrvce.exe"));
	//BB
	m_vProcessName.push_back(_T("SPBBCSvc.exe"));
	m_vProcessName.push_back(_T("UpdMgr.exe"));

	// SymSetup 
	m_vProcessName.push_back(_T("{C6F5B6CF-609C-428E-876F-CA83176C021B}.exe"));


	//applications in NAV
	
	m_vProcessName.push_back(_T("CfgWiz.exe"));
	m_vProcessName.push_back(_T("navapsvc.exe"));
	m_vProcessName.push_back(_T("NAVAPW32.exe"));
	m_vProcessName.push_back(_T("NavShcom.exe"));
	m_vProcessName.push_back(_T("Navw32.exe"));
	m_vProcessName.push_back(_T("Navwnt.exe"));
	m_vProcessName.push_back(_T("OPScan.exe"));
	m_vProcessName.push_back(_T("qconsole.exe"));
	m_vProcessName.push_back(_T("SAVScan.exe"));
	m_vProcessName.push_back(_T("WebReg.exe"));
	//IWP
	m_vProcessName.push_back(_T("ALEUpdat.exe"));
	m_vProcessName.push_back(_T("NPFMintor.exe"));


	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CBaseTestDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

void CBaseTestDlg::OnPaint() 
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
HCURSOR CBaseTestDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CBaseTestDlg::OnBnClickedButtonFile()
{
/*//get signle path path use this part----------------------------------------------------
	CFileDialog file(TRUE, _T("*.dll; *.loc "),NULL,OFN_EXPLORER|OFN_HIDEREADONLY|OFN_NOTESTFILECREATE, "DLLs (*.dll;*.loc)|*.dll; *.loc|All Files (*.*)|*.*|");
	file.m_ofn.lpstrTitle  = _T("Choose your dll");

	int open = file.DoModal();
	if(open == IDOK) 
		m_strFileName = file.GetPathName();
*///-------------------------------------------------------------------------------------
	//GetBaseAddress();

	TCHAR szPath[2068];
	BROWSEINFO info;
	ZeroMemory(&info, sizeof(BROWSEINFO));
	info.lpszTitle = _T("Select the folder which you want to do check for dll base addresses");
	info.ulFlags = BIF_NONEWFOLDERBUTTON | BIF_RETURNONLYFSDIRS | BIF_DONTGOBELOWDOMAIN;

	LPITEMIDLIST item = ::SHBrowseForFolder(&info);
	
	if(item != NULL)
	{
		if(::SHGetPathFromIDList(item, szPath))
		{
 			//m_strPath = (CString) szPath;
			m_vDllName.clear();
			m_vDllPath.clear();
			m_vDllBase.clear();
			m_listView.DeleteAllItems();
			LoadFiles(szPath);
			GetBaseAddress();
		}
		else
		{
			::AfxMessageBox(_T("Cannot get Directory information"));
		}
	}
	else
	{
		//::AfxMessageBox(_T("Cannot get Directory information"));
	}


}
void CBaseTestDlg::GetBaseAddress()
{
	CString strTemp = _T("");
	
	//DWORD Base;
	for (int i = 0 ; i < m_vDllName.size(); i++)
	{
		PLOADED_IMAGE li;

		//PSTR name = m_vDllName.at(i).GetBuffer();
		li = ImageLoad(  m_vDllName.at(i).GetBuffer(),    m_vDllPath.at(i).GetBuffer());

		if (li != NULL)
		{
			PIMAGE_NT_HEADERS32	fh = li->FileHeader;
			IMAGE_OPTIONAL_HEADER oph = fh->OptionalHeader;
			
			strTemp.Format("0x%X", oph.ImageBase);
			m_vDllBase.push_back(strTemp);
			ImageUnload(li);
		}else
		{
			m_vDllBase.push_back(_T("ERROR"));
		}

		const int IDX = m_listView.InsertItem(0, _T(""));		
		//m_listView.SetItemText(
		//CString temp = (CString) m_pvDllName.at(i);
		m_listView.SetItemText(IDX, 1,  m_vDllName.at(i));
		m_listView.SetItemText(IDX, 2, m_vDllBase.at(i) );
	}
}
void CBaseTestDlg::LoadFiles(PSTR strPath)
{

	WIN32_FIND_DATA fd;
	TCHAR tempDir[2068];
	TCHAR tempStr[2068];
	DWORD dwLen = 2068;

	_tcscpy(tempDir, strPath);
	_tcscat(tempDir, _T("\\*.*")); //Will do a wild-card search under specified dir

	HANDLE hFind = ::FindFirstFile(tempDir,&fd);
	if (hFind != INVALID_HANDLE_VALUE)
	{
		do
		{
			CString name = fd.cFileName;
			if (name != _T(".") && name != _T("..")&& (fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
			{	
					//strTemp.Format("%s\\%s",strPath, fd.cFileName);
					_tcscpy(tempStr, strPath);
					_tcscat(tempStr, _T("\\"));
					_tcscat(tempStr, fd.cFileName);
					LoadFiles(tempStr);
					
			}else if (name != _T(".") && name != _T("..") && (!( name.Right(4).CompareNoCase( _T(".dll")) || !(name.Right(4).CompareNoCase(_T(".loc"))))))
			{
				m_vDllName.push_back(fd.cFileName);
				m_vDllPath.push_back(strPath);
			}
		}while (::FindNextFile(hFind, &fd));
		::FindClose(hFind);
	}

}

void CBaseTestDlg::OnBnClickedButtonEdit()
{
	EditProcessDlg Dlg;
	
	Dlg.SetProcessName(&m_vProcessName);
	INT_PTR nResponse = Dlg.DoModal();
	
}

void CBaseTestDlg::OnBnClickedButtonRun()
{
	m_listView.DeleteAllItems();
	for (int i = 0; i< m_vProcessName.size(); i++)
	{
		EnumProcessModules(m_vProcessName.at(i));
	}
	
}
void CBaseTestDlg::EnumProcessModules(LPCTSTR szProcessName)
{
	UpdateData(true);
	CEnumProcess cEnumProcesses;
    BOOL bProcess;
	CEnumProcess::CProcessEntry cProcessEntry;
    bProcess = cEnumProcesses.GetProcessFirst(&cProcessEntry);

	while(bProcess)
    {
		CString cszFileName = cProcessEntry.lpFilename;
		if(0 == cszFileName.CompareNoCase(szProcessName))
        {
			INT i = m_listView.InsertItem(100, szProcessName);

			CEnumProcess::CModuleEntry cModuleEntry;
            BOOL bModule;
            bModule = cEnumProcesses.GetModuleFirst(cProcessEntry.dwPID, &cModuleEntry);
			while(bModule)
            {
				CString cszPreferredBase, cszModuleBase;
                cszModuleBase.Format(_T("0x%08X"), cModuleEntry.pLoadBase);
                cszPreferredBase.Format(_T("0x%08X"), cModuleEntry.pPreferredBase);
                BOOL bWrongLoadAddr = FALSE;
			
				if(cModuleEntry.pLoadBase != cModuleEntry.pPreferredBase)
                    bWrongLoadAddr = TRUE;
				LPSTR szModuleFileName = PathFindFileName(cModuleEntry.lpFilename);

				if (m_bShowAll)
				{
					INT iIndex = m_listView.InsertItem(100, _T(""));
					m_listView.SetItemText(iIndex, 1, szModuleFileName);
					m_listView.SetItemText(iIndex, 3, cszModuleBase);
					m_listView.SetItemText(iIndex, 2, cszPreferredBase);
				}
				else
				{
					if(0 != cszFileName.CompareNoCase(szModuleFileName) && bWrongLoadAddr)
					{
						INT iIndex = m_listView.InsertItem(100, _T(""));
						m_listView.SetItemText(iIndex, 1, szModuleFileName);
						m_listView.SetItemText(iIndex, 3, cszModuleBase);
						m_listView.SetItemText(iIndex, 2, cszPreferredBase);
					}
				}
                bModule = cEnumProcesses.GetModuleNext(cProcessEntry.dwPID, &cModuleEntry);

			}
			
		}
		bProcess = cEnumProcesses.GetProcessNext(&cProcessEntry);

	}

}

void CBaseTestDlg::OnBnClickedButtonStart()
{
	m_bStop = FALSE;
	GetDlgItem(IDC_BUTTON_STOP)->EnableWindow(TRUE);
	GetDlgItem(IDC_BUTTON_START)->EnableWindow(FALSE);
	GetDlgItem(IDC_BUTTON_RUN)->EnableWindow(FALSE);
	GetDlgItem(IDC_BUTTON_FILE)->EnableWindow(FALSE);

	_beginthread( Start, 0, this );
}


void CBaseTestDlg::OnBnClickedButtonStop()
{
	GetDlgItem(IDC_BUTTON_STOP)->EnableWindow(FALSE);
	GetDlgItem(IDC_BUTTON_START)->EnableWindow(TRUE);
	GetDlgItem(IDC_BUTTON_RUN)->EnableWindow(TRUE);
	GetDlgItem(IDC_BUTTON_FILE)->EnableWindow(TRUE);
	m_bStop = TRUE;
}
