// MigrationTestDlg.cpp : implementation file
//

#include "stdafx.h"
#include "MigrationTest.h"
#include "MigrationTestDlg.h"
#include "COptionsMigration.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CMigrationTestDlg dialog

CMigrationTestDlg::CMigrationTestDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CMigrationTestDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CMigrationTestDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CMigrationTestDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CMigrationTestDlg)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CMigrationTestDlg, CDialog)
	//{{AFX_MSG_MAP(CMigrationTestDlg)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_WriteToFile, OnWriteToFile)
	ON_BN_CLICKED(IDC_SetFromFile, OnSetFromFile)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMigrationTestDlg message handlers

BOOL CMigrationTestDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon
	
	// TODO: Add extra initialization here
	
	return TRUE;  // return TRUE  unless you set the focus to a control
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CMigrationTestDlg::OnPaint() 
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
HCURSOR CMigrationTestDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}

void CMigrationTestDlg::OnSetFromFile() 
{
	//before we initialize we need to create migration ini file in c:\windows\temp, have user browse
	TCHAR szINIPath[MAX_PATH] = {0};
	GetWindowsDirectory(szINIPath, sizeof(szINIPath));
	_tcscat(szINIPath, _T("\\TEMP"));
	
	OPENFILENAME ofn;
	ZeroMemory(&ofn, sizeof(OPENFILENAME));
	TCHAR szFileName[MAX_PATH * 10] = { _T('\0') };
	TCHAR szFilterString[MAX_PATH] = {0};
	wsprintf(szFilterString, "*.ini\\*.ini\0\0");
	TCHAR *pszLast = _tcsrchr(szFilterString, '\\');
	*pszLast = _T('\0');
	ofn.lStructSize = sizeof(OPENFILENAME);
	ofn.hwndOwner = NULL;
	ofn.Flags = OFN_ENABLESIZING | OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST | OFN_EXPLORER;
	ofn.lpstrFile = szFileName;
	ofn.nMaxFile = sizeof(szFileName);
	ofn.lpstrTitle = "Where's migration.ini? (will be copied to C:\\Windows\\Temp)";
	ofn.lpstrFilter = szFilterString;
	ofn.lpstrInitialDir = szINIPath;
	if (GetOpenFileName(&ofn) == 0)
	{
		::MessageBox(NULL, "You gotta have a migration.ini file", "DOH!", MB_OK | MB_ICONERROR);
		return;
	}
	else
	{
		_tcscat(szINIPath, _T("\\migration.ini"));
		CopyFile(szFileName, szINIPath, FALSE);
	}

	COptionsMigration Migration;
	Migration.Initialize();
	Migration.SetNew();
	
	//launch NAVStub so we can see what the effect was on the currently install options
	HKEY hKey;
	TCHAR szNAVStubPath[MAX_PATH] = {0};
	DWORD dwSize = sizeof(szNAVStubPath);
	if(ERROR_SUCCESS == RegOpenKeyEx(HKEY_LOCAL_MACHINE, _T("Software\\Symantec\\InstalledApps"), REG_OPTION_NON_VOLATILE, KEY_READ, &hKey))
		RegQueryValueEx(hKey, _T("NAV"), NULL, NULL, (BYTE*)szNAVStubPath, &dwSize);
	_tcscat(szNAVStubPath, _T("\\NAVStub.exe"));
	ShellExecute(NULL, "open", szNAVStubPath, NULL, NULL, true);

}

void CMigrationTestDlg::OnWriteToFile() 
{
	//before we initialize we need to delete the current file in c:\windows\temp
	TCHAR szINIPath[MAX_PATH] = {0};
	GetWindowsDirectory(szINIPath, sizeof(szINIPath));
	_tcscat(szINIPath, _T("\\TEMP\\migration.ini"));
	DeleteFile(szINIPath);

	COptionsMigration Migration;
	Migration.Initialize();
	Migration.GetOld();

	//launch associated viewer to open up the migration INI file we just created.
	ShellExecute(NULL, "open", szINIPath, NULL, NULL, true);
}
