////////////////////////////////////////////////////////////////////////////////////
// $Header:   S:/REMIND/VCS/n32rmddlg.cpv   1.9   13 Jul 1998 09:50:18   jtaylor  $
////////////////////////////////////////////////////////////////////////////////////
//
// n32rmdDlg.cpp : implementation file
//
////////////////////////////////////////////////////////////////////////////////////
// $Log:   S:/REMIND/VCS/n32rmddlg.cpv  $
// 
//    Rev 1.9   13 Jul 1998 09:50:18   jtaylor
// Updated the program to get the NAV version number from NAVVER.h
//
//    Rev 1.8   17 Dec 1997 21:13:16   jtaylor
// Prevented the user from closing the window with Alt-f4 or the x button.
//
//    Rev 1.7   16 Dec 2004 18:43:48   jtaylor
// Added code to select yes by default when reopening window.
//
//    Rev 1.6   15 Dec 1997 17:33:58   jtaylor
// fixed the logout problem, updated the timer logic.
//
////////////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "n32rmd.h"
#include "n32rmdDlg.h"
#include "n32behav.h"
#include "navver.h"
#include <time.h>
#include <tchar.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define IS_NT      (BOOL)(GetVersion() < 0x80000000)
extern BOOL g_bRunOnce;


#define ACTIVATE_TIMER 1

/////////////////////////////////////////////////////////////////////////////
// CN32rmdDlg dialog

CN32rmdDlg::CN32rmdDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CN32rmdDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CN32rmdDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

BEGIN_MESSAGE_MAP(CN32rmdDlg, CDialog)
	//{{AFX_MSG_MAP(CN32rmdDlg)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_NOSCANNOW, OnNoscannow)
	ON_BN_CLICKED(IDC_SCANNOW, OnScannow)
	ON_WM_CLOSE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CN32rmdDlg message handlers

BOOL CN32rmdDlg::OnInitDialog()
{
	m_bLaunchScanner = TRUE;
	TCHAR szTest[25000] = "";
	LPTSTR lpszTest = szTest;

	if( !NAV_IS_SCANONLY() )
		return TRUE;

	CDialog::OnInitDialog();

	CN32rmdApp *pApp = (CN32rmdApp *)AfxGetApp();

	// read in the text of the reminder file.
	pApp->ReadReminderFile(lpszTest, sizeof(szTest));

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	// TODO: Add extra initialization here
	SetDlgItemText(IDC_UPSELL_TEXTBOX,lpszTest);
	CheckRadioButton(IDC_SCANNOW,IDC_NOSCANNOW,IDC_SCANNOW);

	return TRUE;  // return TRUE  unless you set the focus to a control
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CN32rmdDlg::OnPaint()
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
HCURSOR CN32rmdDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}

void CN32rmdDlg::OnNoscannow()
{
	m_bLaunchScanner = FALSE;
}

void CN32rmdDlg::OnScannow()
{
	m_bLaunchScanner = TRUE;
}

void CN32rmdDlg::OnOK()
{
	ShowWindow(SW_HIDE);

	if( TRUE == m_bLaunchScanner )
		{
        // Launch NAV
        TCHAR szCommand[256] = "";
        TCHAR szRegPath[256] = "";
        TCHAR szDir[256] = "";
        LPTSTR lpszCommand = szCommand;
        LPTSTR lpszDir = szDir;
        HKEY    hKey;
        CString test;
        DWORD   dwBufferSize = sizeof(szDir);

        LONG            lReturn = ERROR_INVALID_DATA;

        // construct the main install key name
        if( IS_NT )
            _tcscpy(szRegPath, _T("SOFTWARE\\Symantec\\Norton AntiVirus NT\\Install\\"));
        else
            _tcscpy(szRegPath, _T("SOFTWARE\\Symantec\\Norton AntiVirus\\Install\\"));

        // append the version number
        _tcscat(szRegPath, _T(NAV_INSTALLKEY_VER));

        // Open the correct Key
        if( IS_NT )
            lReturn = RegOpenKeyEx( HKEY_LOCAL_MACHINE, szRegPath , 0, KEY_READ, &hKey );
        else
            lReturn = RegOpenKeyEx( HKEY_LOCAL_MACHINE, szRegPath , 0, KEY_READ, &hKey );

        if ( lReturn == ERROR_SUCCESS && hKey != NULL )
        {
            lReturn = RegQueryValueEx( hKey, _T("InstallDir"), NULL,
                                NULL, (unsigned char *)lpszCommand, &dwBufferSize );
        }
        RegCloseKey(hKey);

        // Copy the directory
        _tcscpy(lpszDir, lpszCommand);

        if( IS_NT )
            _tcscat(lpszCommand,_T("\\navwnt.exe"));
        else
            _tcscat(lpszCommand,_T("\\navw32.exe"));

        // Launch NAV
        ShellExecute(NULL,NULL,lpszCommand,_T(" /L"),lpszDir,SW_SHOWNORMAL);
		}

	// Set the initial button status and variable status
	CheckRadioButton(IDC_SCANNOW,IDC_NOSCANNOW,IDC_SCANNOW);
	m_bLaunchScanner = TRUE;

	CN32rmdApp *pApp = (CN32rmdApp *)AfxGetApp();
	pApp->DialogClosed();
}



void CN32rmdDlg::OnClose()
{
	// TODO: Add your message handler code here and/or call default
	// Set the initial button status and variable status
	CheckRadioButton(IDC_NOSCANNOW,IDC_NOSCANNOW,IDC_SCANNOW);
	m_bLaunchScanner = FALSE;

	OnOK();
}

