// FixNAVDlg.cpp : implementation file
//

#include "stdafx.h"
#include "FixNAV.h"
#include "FixNAVDlg.h"
#include "objbase.h"
#include "NAVInfo.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

static TCHAR szDllRegSvr[] = _T("DllRegisterServer");

/////////////////////////////////////////////////////////////////////////////
// CAboutDlg dialog used for App About

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// Dialog Data
	//{{AFX_DATA(CAboutDlg)
	enum { IDD = IDD_ABOUTBOX };
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAboutDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CAboutDlg)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
	//{{AFX_DATA_INIT(CAboutDlg)
	//}}AFX_DATA_INIT
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAboutDlg)
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
	//{{AFX_MSG_MAP(CAboutDlg)
		// No message handlers
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CFixNAVDlg dialog

CFixNAVDlg::CFixNAVDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CFixNAVDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CFixNAVDlg)
	m_strLog = _T("");
	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CFixNAVDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CFixNAVDlg)
	DDX_Control(pDX, IDFIX, m_btnFix);
	DDX_Control(pDX, IDEXIT, m_btnExit);
	DDX_Control(pDX, IDC_SAVE, m_btnSave);
	DDX_Control(pDX, IDC_LOG, m_edtLog);
	DDX_Text(pDX, IDC_LOG, m_strLog);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CFixNAVDlg, CDialog)
	//{{AFX_MSG_MAP(CFixNAVDlg)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDFIX, OnFix)
	ON_BN_CLICKED(IDEXIT, OnExit)
	ON_BN_CLICKED(IDC_SAVE, OnSave)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CFixNAVDlg message handlers

BOOL CFixNAVDlg::OnInitDialog()
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
/*	
    // Torito 8.0
    m_listModules.push_back ( "aboutplg.dll" );
    m_listModules.push_back ( "cfgwiz.dll");
    m_listModules.push_back ( "defalert.dll");
    m_listModules.push_back ( "navactlg.dll");
    m_listModules.push_back ( "navapscr.dll");
    m_listModules.push_back ( "navcomui.dll");
    m_listModules.push_back ( "navlnch.dll");
    m_listModules.push_back ( "navlucbk.dll");
    m_listModules.push_back ( "navopts.dll");
    m_listModules.push_back ( "navshext.dll");
    m_listModules.push_back ( "navstats.dll");
    m_listModules.push_back ( "navtasks.dll");
    m_listModules.push_back ( "navui.dll");
    m_listModules.push_back ( "navwbwnd.dll");
    m_listModules.push_back ( "officeav.dll");
    m_listModules.push_back ( "quar32.dll");
    m_listModules.push_back ( "scandlvr.dll");
*/

    // Kobe 9.0
    m_listModules.push_back ( "aboutplg.dll" );
    m_listModules.push_back ( "cfgwiz.dll");
    m_listModules.push_back ( "defalert.dll");
    m_listModules.push_back ( "navapscr.dll");
    m_listModules.push_back ( "navcomui.dll");
    m_listModules.push_back ( "navlnch.dll");
    m_listModules.push_back ( "navlucbk.dll");
    m_listModules.push_back ( "navopts.dll");
    m_listModules.push_back ( "navshext.dll");
    m_listModules.push_back ( "navstats.dll");
    m_listModules.push_back ( "navtasks.dll");
    m_listModules.push_back ( "navui.dll");
    m_listModules.push_back ( "officeav.dll");
    m_listModules.push_back ( "quar32.dll");
    m_listModules.push_back ( "scandlvr.dll");
    m_listModules.push_back ( "naverror.dll");
    m_listModules.push_back ( "navevent.dll");
	
	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CFixNAVDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

void CFixNAVDlg::OnPaint() 
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
HCURSOR CFixNAVDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}

void CFixNAVDlg::OnFix() 
{
	UpdateData (TRUE);
    BOOL bSuccess = TRUE;
    CNAVInfo NAVInfo;
    DWORD dwResult = 0;
    stringlist::iterator iterModules;

    // Disable the buttons
    //
    m_btnSave.EnableWindow ( FALSE );
    m_btnExit.EnableWindow ( FALSE );
    m_btnFix.EnableWindow ( FALSE );

    if ( FAILED(CoInitialize (NULL)))
    {
        m_strLog += _S(IDS_Failed_init_COM);
        m_strLog += "\r\n";
        bSuccess = FALSE;
        goto exit;
    }

    if ( 0 == _tcsicmp ( NAVInfo.GetNAVDir (), "\0") )
    {
        m_strLog += _S(IDS_Bad_reg);
        m_strLog += "\r\n";
        bSuccess = FALSE;
        goto exit;
    }

    m_strLog += _S(IDS_Register_modules);
    m_strLog += "\r\n";
    m_strLog += _S(IDS_Base_path);
    m_strLog += " ";
    m_strLog += NAVInfo.GetNAVDir();
    m_strLog += "\r\n";

    UpdateData (FALSE);
    m_edtLog.UpdateWindow ();

    // Loop through all the modules and register them.
    //
    for ( iterModules = m_listModules.begin ();
          iterModules != m_listModules.end ();
          iterModules++ )
    {
        std::string strPath = *iterModules;
    
        if ( "\0" == strPath )
            break;

        m_strLog += _S(IDS_Registering);
        m_strLog += " ";
        m_strLog += strPath.c_str ();
        m_strLog += " - ";

        std::string strTempPath;
        strTempPath = NAVInfo.GetNAVDir();
        strTempPath += "\\";
        strTempPath += strPath;

        HRESULT hrExtendedError = 0;
        
        // Register the module
        //
        dwResult = RegisterModule ( strTempPath.c_str (), &hrExtendedError );

        switch ( dwResult )
        {
            case CFixNAVDlg::SUCCESS :
                m_strLog += _S(IDS_Success);
                m_strLog += "\r\n";
                break;

            case CFixNAVDlg::FAIL_FILE_NOT_FOUND :
                m_strLog += _S(IDS_File_not_found);
                m_strLog += "\r\n";
                bSuccess = FALSE;
                break;

            case CFixNAVDlg::FAIL_LOAD :
                m_strLog += _S(IDS_Failed_to_load);
                m_strLog += "\r\n";
                bSuccess = FALSE;
                break;

            case CFixNAVDlg::FAIL_ENTRY :
                m_strLog += _S(IDS_Failed_to_entry);
                m_strLog += "\r\n";
                bSuccess = FALSE;
                break;

            case CFixNAVDlg::FAIL_REG :
                {
                m_strLog += _S(IDS_Failed_to_reg);
                TCHAR szTemp [256] = {0};
                _ultot ( (ULONG) hrExtendedError, szTemp, 10 );
                m_strLog += szTemp;
                m_strLog += "\r\n";
                bSuccess = FALSE;
                }
                break;

            default:
                m_strLog += _S(IDS_Unknown_error);
                m_strLog += "\r\n";
                bSuccess = FALSE;
                break;
        }

        // Pump messages so we can respond to UI a little.
        // It's like quick and dirty multithreading.
        //
        MSG msg = {0};
		while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		};

        UpdateData (FALSE);
        m_edtLog.UpdateWindow ();
    }

exit:
    m_strLog += _S(IDS_Finished);
    m_strLog += " - ";
    
    if ( bSuccess )
    {
        m_strLog += _S(IDS_No_errors);
        m_strLog += "\r\n";
        AfxMessageBox ( IDS_No_errors, MB_ICONINFORMATION );
    }
    else
    {
        m_strLog += _S(IDS_Errors);
        m_strLog += "\r\n";
        AfxMessageBox ( IDS_Errors, MB_ICONERROR );
    }

    UpdateData (FALSE);
    m_edtLog.UpdateWindow ();

    m_btnSave.EnableWindow ( TRUE );
    m_btnExit.EnableWindow ( TRUE );
    m_btnFix.EnableWindow ( TRUE );

    CoUninitialize ();
}

DWORD CFixNAVDlg::RegisterModule(LPCTSTR lpszModulePath, HRESULT* phrExtendedError)
{
    DWORD dwReturn = CFixNAVDlg::SUCCESS;
    *phrExtendedError = S_OK;

    // Make sure the module exists before installing
    //
    if ( -1 == GetFileAttributes ( lpszModulePath ))
        return FAIL_FILE_NOT_FOUND;
    
    HINSTANCE hInst;

    hInst = ::LoadLibraryEx(lpszModulePath, NULL, LOAD_WITH_ALTERED_SEARCH_PATH);

    if ( !hInst )
        return FAIL_LOAD;
    
    HRESULT (FAR STDAPICALLTYPE * lpDllEntryPoint)(void);
    LPTSTR pszDllEntryPoint = szDllRegSvr;  

    (FARPROC&)lpDllEntryPoint = GetProcAddress(hInst, pszDllEntryPoint);
	
	if (lpDllEntryPoint == NULL)   
	{
        // Can't find the exported function
        //
        dwReturn = CFixNAVDlg::FAIL_ENTRY;
    }
    else
    {
        *phrExtendedError = (*lpDllEntryPoint)();
           
        if (FAILED(*phrExtendedError))
        {
            // Failed during DllRegisterServer
            //
            dwReturn = CFixNAVDlg::FAIL_REG;
        }
        else
            dwReturn = ERROR_SUCCESS;
    }

    if (hInst)
        FreeLibrary ( hInst );

    return dwReturn;
}

void CFixNAVDlg::OnExit() 
{
    OnCancel ();	
}

void CFixNAVDlg::OnSave() 
{
    if ( m_strLog.IsEmpty () )
    {
        AfxMessageBox ( IDS_Log_empty );
        return;
    }

    OPENFILENAME filedata;
    ZeroMemory ( &filedata, sizeof (OPENFILENAME));

    TCHAR szFilePath [MAX_PATH*2] = {0};
    TCHAR szExt [] = _T("txt");

    filedata.lpstrFile = szFilePath;
    filedata.nMaxFile = sizeof ( szFilePath );
    filedata.lpstrDefExt = szExt;
    filedata.hwndOwner = m_hWnd;
    filedata.lStructSize = sizeof ( OPENFILENAME );

    if ( GetSaveFileName ( &filedata ))
    {
        // Write out the file
        //
        CStdioFile File;
        if ( File.Open ( filedata.lpstrFile, CFile::modeCreate | CFile::modeWrite ))
            File.WriteString ( m_strLog );
    }
}
