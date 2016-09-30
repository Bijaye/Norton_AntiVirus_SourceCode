// LUTestInstallDlg.cpp : implementation file
//

#include "stdafx.h"
#include "LUTestInstall.h"
#include "LUTestInstallDlg.h"
#include "OSInfo.h"
#include "AutoUpdateDefines.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CLUTestInstallDlg dialog

CLUTestInstallDlg::CLUTestInstallDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CLUTestInstallDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CLUTestInstallDlg)
	m_strLog = _T("");
	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CLUTestInstallDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CLUTestInstallDlg)
	DDX_Text(pDX, IDC_LOG, m_strLog);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CLUTestInstallDlg, CDialog)
	//{{AFX_MSG_MAP(CLUTestInstallDlg)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_CHECK_STATE, OnCheckState)
	ON_BN_CLICKED(IDC_CLOSE_MUTEX, OnCloseMutex)
	ON_BN_CLICKED(IDC_CREATE_MUTEX, OnCreateMutex)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


CLUTestInstallDlg::~CLUTestInstallDlg ()
{
    closeMutex ();
}

/////////////////////////////////////////////////////////////////////////////
// CLUTestInstallDlg message handlers

BOOL CLUTestInstallDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon
	
	COSInfo OSInfo;
	if( OSInfo.IsWinNT() )
	{
        if ( OSInfo.IsTerminalServicesInstalled () )
        {
            wsprintf ( m_szMutexName, "Global\\%s", SYM_AUTO_UPDATE_MUTEX );
        }
        else
            wsprintf ( m_szMutexName, "%s", SYM_AUTO_UPDATE_MUTEX );
	}

    m_strLog.Empty ();

	return TRUE;  // return TRUE  unless you set the focus to a control
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CLUTestInstallDlg::OnPaint() 
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
HCURSOR CLUTestInstallDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}

void CLUTestInstallDlg::OnCheckState() 
{
	UpdateData ( TRUE );

	//check for LiveUpdate running
    //
    bool bLURunning = false;
    bool bALURunning = false;
    HANDLE hALUMutex = NULL;

	if( NULL != FindWindowEx(NULL, NULL, NULL, _T("LiveUpdate")))
    {
        bLURunning = true;
        m_strLog += "LU running window\r\n";
    }
    else
        m_strLog += "LU not running\r\n";

    if ( !bLURunning )
	{

        hALUMutex = OpenMutex( SYNCHRONIZE, FALSE, m_szMutexName );

        if ( hALUMutex )
        {
            bALURunning = true;
            m_strLog += "ALU mutex already exists\r\n";
        }
        else
            m_strLog += "No ALU mutex\r\n";
	}

    if ( hALUMutex )
    {
        CloseHandle ( hALUMutex );
        hALUMutex = NULL;
    }
    
    UpdateData ( FALSE );	
}

void CLUTestInstallDlg::OnCloseMutex() 
{
	UpdateData ( TRUE );
    
    if ( closeMutex () )
        m_strLog += "Closed mutex\r\n";
    else
        m_strLog += "Mutex not open\r\n";

    UpdateData ( FALSE );

}

void CLUTestInstallDlg::OnCreateMutex() 
{
	UpdateData ( TRUE );

    closeMutex ();

	//open up the LU mutex to prevent ALU from running
	COSInfo OSInfo;
	if( OSInfo.IsWinNT() )
	{

        SECURITY_ATTRIBUTES sa;
		SECURITY_DESCRIPTOR sd;

		// Setup the descriptor
		InitializeSecurityDescriptor(&sd, SECURITY_DESCRIPTOR_REVISION);

		// Add a null DACL to the security descriptor
		SetSecurityDescriptorDacl(&sd, TRUE, (PACL) NULL, FALSE);
		sa.nLength = sizeof(sa); 
		sa.lpSecurityDescriptor = &sd;
		sa.bInheritHandle = TRUE;

        m_hALUMutex = CreateMutex(&sa,TRUE,m_szMutexName);
	}
	else
	{	
		m_hALUMutex = CreateMutex(NULL,TRUE,m_szMutexName);
	}

    if ( ::GetLastError() == ERROR_ALREADY_EXISTS )
        m_strLog += "ALU mutex already exists\r\n";
    else
        m_strLog += "ALU mutex created\r\n";

    UpdateData ( FALSE );

}

bool CLUTestInstallDlg::closeMutex()
{
    if ( m_hALUMutex )
    {
        CloseHandle ( m_hALUMutex );
        m_hALUMutex = NULL;
        return true;
    }

    return false;
}

