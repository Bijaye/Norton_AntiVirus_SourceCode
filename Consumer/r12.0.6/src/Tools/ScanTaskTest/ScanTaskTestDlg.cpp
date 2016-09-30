// ScanTaskTestDlg.cpp : implementation file
//

#include "stdafx.h"
#include "ScanTaskTest.h"
#include "ScanTaskTestDlg.h"
#include <atlbase.h>
#include "objbase.h"
#include "comdef.h"
#include "NAVTasks.h"   // For INAVTask(s)
#include "ScanTask.h"   // For Lib
#include "NAVInfo.h"    // Toolbox
#include "ScriptSafe.h"
#include "SymScriptSafe.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

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
// CScanTaskTestDlg dialog

CScanTaskTestDlg::CScanTaskTestDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CScanTaskTestDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CScanTaskTestDlg)
	m_strAllUsers = _T("");
	m_strCurrentUser = _T("");
	m_strScanDateRegKey = _T("");
	m_strScanDateTask = _T("");
	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CScanTaskTestDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CScanTaskTestDlg)
	DDX_Text(pDX, IDC_ALLUSERS, m_strAllUsers);
	DDX_Text(pDX, IDC_CURRENTUSER, m_strCurrentUser);
	DDX_Text(pDX, IDC_SCAN_DATE_REGKEY, m_strScanDateRegKey);
	DDX_Text(pDX, IDC_SCAN_DATE_TASK, m_strScanDateTask);
    DDX_Text(pDX, IDC_NSW_ELAPSED, m_strNSWElapsed);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CScanTaskTestDlg, CDialog)
	//{{AFX_MSG_MAP(CScanTaskTestDlg)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_MYCOMPUTER, OnMycomputer)
	ON_BN_CLICKED(IDC_DELETEALL, OnDeleteall)
	ON_BN_CLICKED(IDC_NEW, OnNew)
	ON_BN_CLICKED(IDC_SCAN, OnScan)
	ON_BN_CLICKED(IDC_SCHEDULE_MYCOMP, OnScheduleMycomp)
	ON_BN_CLICKED(IDC_DELETE, OnDelete)
	ON_BN_CLICKED(IDC_FAKE_MY_COMP, OnFakeMyComp)
	ON_BN_CLICKED(IDC_OLD_SCAN, OnOldScan)
	ON_BN_CLICKED(IDC_RESET_FSS, OnResetFss)
	ON_BN_CLICKED(IDC_EDIT, OnEdit)
	ON_BN_CLICKED(IDC_SHOW_MY_COMP_SCHED, OnShowMyCompSched)
	ON_BN_CLICKED(IDC_LOAD_FOLDER, OnLoadFolder)
	ON_BN_CLICKED(IDC_LOAD_FILES, OnLoadFiles)
	ON_BN_CLICKED(IDC_LOAD_MY_COMP, OnLoadMyComp)
	ON_BN_CLICKED(IDC_LOAD_DRIVES, OnLoadDrives)
    ON_BN_CLICKED(IDC_JUSTTASK, JustScanTask)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CScanTaskTestDlg message handlers

BOOL CScanTaskTestDlg::OnInitDialog()
{
    CoInitialize (NULL);

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
	
	CNAVInfo NAVInfo;
    
    m_strAllUsers = NAVInfo.GetNAVCommonDir();
	m_strCurrentUser = NAVInfo.GetNAVUserDir();

    if ( FAILED (m_spTasks.CoCreateInstance ( CLSID_NAVScanTasks )))
    {
        AfxMessageBox ( "Failed to get NAVTasks" );
        return TRUE;
    }

	if( SUCCEEDED( m_spTasks.QueryInterface(&m_spSymScriptSafe)))
	{
		// Set appropriate access so that NAVStatus call to IsItSafe() 
		// returns successfully.

		long dwAccess = ESymScriptSafe_UNLOCK_KEYA;
		long dwKey = ESymScriptSafe_UNLOCK_KEYB;
		m_spSymScriptSafe->SetAccess(dwAccess, dwKey);
    }
    else
        AfxMessageBox ( "Failed to get security");

    RefreshScanDates ();

    UpdateData ( FALSE );

	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CScanTaskTestDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

void CScanTaskTestDlg::OnPaint() 
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
HCURSOR CScanTaskTestDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}

void CScanTaskTestDlg::OnMycomputer() 
{
    if ( FAILED ( m_spTasks->ScanMyComputer()))
        AfxMessageBox ( "Failed to start My Computer scan");
}

void CScanTaskTestDlg::OnDeleteall() 
{
    if ( FAILED ( m_spTasks -> DeleteAll ()))
        AfxMessageBox ( "Failed to DeleteAll");
}

void CScanTaskTestDlg::OnNew() 
{
    CComPtr <INAVScanTask> spTask;

    if ( S_OK == m_spTasks->Create ( &spTask )) // Create a custom scan and start the wizard
    {
    }
}
    


void CScanTaskTestDlg::OnScan() 
{
    
    HRESULT hr = E_FAIL;

    long lLimit = 0;
    long lIndex = 0;

    m_spTasks->get_Count( &lLimit );

    CComVariant vIndex (lIndex);

    for ( lIndex = 0; lIndex < lLimit; lIndex++)
    {
        CComPtr <INAVScanTask> spTask;
    
        vIndex = lIndex;
    
        if ( FAILED ( hr = m_spTasks -> get_Item( vIndex, &spTask )))
        {
            AfxMessageBox ( "Failed getting item" );
            break;
        }

        CComBSTR bstrTemp;

        if ( SUCCEEDED ( hr = spTask->get_TaskPath( &bstrTemp )))
        {

            long lType = 0;
            spTask->get_TaskType ( &lType );

            if ( lType == scanCustom ) //scanCustom //scanHardDrives
            {
                EScheduled eScheduled;

                spTask->get_Scheduled ( &eScheduled );

                //if ( eScheduled == EScheduled_yes )
                {
                    BOOL bWeekly = FALSE;
                    spTask->Schedule ( &bWeekly );
                }

                BOOL bCanEdit = FALSE;
            
                spTask->get_CanEdit ( &bCanEdit );

                BOOL bCanSchedule = FALSE;
            
                spTask->get_CanSchedule ( &bCanSchedule );

                CScanTask task;

                _bstr_t szTempPath ( bstrTemp, 0 ); // don't copy

                task.Load ( szTempPath );

                CScanItem scanitem;

                task.EnumReset();

                while ( task.EnumNext ( scanitem ) )
                {
                    SCANITEMTYPE iType;
                    SCANITEMSUBTYPE iSubType;
            
                    scanitem.GetItemType (iType, iSubType);

                    switch ( (int) iType )
                    {
                        case typeDrive :
                        {
                                TCHAR cDrive = scanitem.GetDrive ();
                        }
                        break;

                        case typeFolder :
                        case typeFile :
                        {
                            CString strPath;
                            strPath = scanitem.GetPath ();
                        }
                        break;
                    }
                }
            } // end filter on type
        }
    }
}

void CScanTaskTestDlg::OnScheduleMycomp() 
{
    if ( FAILED ( m_spTasks->ScheduleMyComputer()))
        AfxMessageBox ( "Failed to schedule the My Computer task");
}

void CScanTaskTestDlg::OnDelete() 
{
    long lLimit = 0;
    long lIndex = 1;
    
    m_spTasks->get_Count( &lLimit );

    CComVariant vIndex (lIndex);
    
    // Enum the tasks
    //
    for ( lIndex = 0; lIndex < lLimit; lIndex++)
    {
        CComPtr <INAVScanTask> spTask;
    
        vIndex = lIndex;
    
        m_spTasks -> get_Item( vIndex, &spTask );

        long lType = 0;
        spTask->get_TaskType ( &lType );

        if ( lType == scanCustom ) //scanCustom //scanHardDrives
        {
            m_spTasks->Delete ( vIndex );
        }
    }
}

void CScanTaskTestDlg::OnFakeMyComp() 
{
    time_t timeCurrent;
    time ( &timeCurrent );
    
    //struct tm* pTime;

    //pTime = localtime(&timeCurrent);
    
    //timeCurrent = mktime ( pTime ); 
    
    //TCHAR szTemp [10] ={0};
    //_itot ( pTime->tm_hour, szTemp, 10 );
    
    //AfxMessageBox ( szTemp  );
    
    //_itot ( pTime->tm_mday, szTemp, 10 );
    
    //AfxMessageBox ( szTemp  );

    SetMyCompScanTime ( timeCurrent );	
}

void CScanTaskTestDlg::OnOldScan() 
{
    time_t timeCurrent;
    time ( &timeCurrent );

    struct tm* pTime;

    pTime = localtime(&timeCurrent);

    pTime->tm_mday = 25;    // 25th
    pTime->tm_mon = 2;      // March
    pTime->tm_year = 100;   // 2000

    timeCurrent = mktime ( pTime );
    
    SetMyCompScanTime ( timeCurrent );
	
}

void CScanTaskTestDlg::SetMyCompScanTime(time_t &timeLastRun)
{
    long lLimit = 0;
    long lIndex = 1;
    
    m_spTasks->get_Count( &lLimit );

    CComVariant vIndex (lIndex);
    
    // Enum the tasks
    //
    for ( lIndex = 0; lIndex < lLimit; lIndex++)
    {
        CComPtr <INAVScanTask> spTask;
    
        vIndex = lIndex;
    
        if ( SUCCEEDED (m_spTasks -> get_Item( vIndex, &spTask )))
        {

            long lType = 0;
            spTask->get_TaskType ( &lType );

            if ( lType == scanMyComputer ) //scanCustom //scanHardDrives
            {
            
                CScanTask task;
                CComBSTR bstrTemp;
            
                spTask->get_TaskPath( &bstrTemp );

                _bstr_t szTempPath ( bstrTemp, 1 ); // don't copy

                if ( !task.Load ( szTempPath, false ))
                    ::MessageBox ( ::GetDesktopWindow (), "Error loading task", "ScanTaskTest", MB_OK );
            
                task.SetLastRunTime( timeLastRun );

                if (!task.Save ())
                    ::MessageBox ( ::GetDesktopWindow (), "Error saving task", "ScanTaskTest", MB_OK );

            }
        }
    }
    
    RefreshScanDates ();
}

void CScanTaskTestDlg::OnResetFss() 
{
    time_t time;
    time = 0;
    
    SetMyCompScanTime ( time );
	
}

void CScanTaskTestDlg::OnEdit() 
{
    long lLimit = 0;
    long lIndex = 1;
    
    m_spTasks->get_Count( &lLimit );

    CComVariant vIndex (lIndex);
    
    // Enum the tasks
    //
    for ( lIndex = 0; lIndex < lLimit; lIndex++)
    {
        CComPtr <INAVScanTask> spTask;
    
        vIndex = lIndex;
    
        m_spTasks -> get_Item( vIndex, &spTask );
	    
        BOOL bCanEdit = FALSE;
        spTask->get_CanEdit ( &bCanEdit );

        if ( bCanEdit )
        {
           
            spTask->Edit();
        }
    }
}

void CScanTaskTestDlg::OnShowMyCompSched() 
{
    CComPtr <INAVScanTask> spTask;

    if ( SUCCEEDED ( m_spTasks->get_MyComputer ( &spTask )))
    {

        BOOL bWeekly = FALSE;
        spTask->Schedule ( &bWeekly );
    }
}

void CScanTaskTestDlg::OnLoadFolder() 
{
    long lType = scanFolder;
    LoadAType ( &lType );
    
}

void CScanTaskTestDlg::LoadAType( long* plScanType ) 
{
    long lLimit = 0;
    long lIndex = 1;
    
    m_spTasks->get_Count( &lLimit );

    CComVariant vIndex (lIndex);
    
    // Enum the tasks
    //
    for ( lIndex = 0; lIndex < lLimit; lIndex++)
    {
        CComPtr <INAVScanTask> spTask;
    
        vIndex = lIndex;
    
        m_spTasks -> get_Item( vIndex, &spTask );

        long lType = 0;
        spTask->get_TaskType ( &lType );

        if ( lType == *plScanType ) //scanCustom //scanHardDrives
        {
            CScanTask task;
            CComBSTR bstrTemp;
            
            spTask->get_TaskPath( &bstrTemp );

            _bstr_t szTempPath ( bstrTemp, 1 ); // copy

            if ( !task.Load ( szTempPath, 1 ))    // Gimme UI !
                ::MessageBox ( ::GetDesktopWindow (), "Failed to Load", "ScanTaskTest", MB_OK );

            task.EnumReset();

	        // Iterate through the list of tasks.

	        for (UINT uiItems = task.GetItemCount(), ui = 0U; ui < uiItems; ui++)
	        {
		        CScanItem item;

		        // Add the next item to Scan Manager's items to scan list.

		        if (!task.EnumNext(item))
			        break;

		        // Get the item type.

		        SCANITEMTYPE type;
		        SCANITEMSUBTYPE subtype;
		        item.GetItemType(type, subtype);

		        // Is the item a drive?

		        if (type == typeDrive)
		        {
			        char szDrive[] = "X:";
			        szDrive[0] = item.GetDrive();
                    ::MessageBox ( ::GetDesktopWindow (), szDrive, "Drive", MB_OK);
		        }
		        
		        // If it's not a drive, then it's a file or folder.

		        else
		        {
			        if ( subtype == subtypeFolderAndSubs )
                        ::MessageBox ( ::GetDesktopWindow (), item.GetPath(), "Folder+Subs", MB_OK);
                    else if ( subtype == subtypeFolder )
                        ::MessageBox ( ::GetDesktopWindow (), item.GetPath(), "Folder", MB_OK);
                    else if ( subtype == subtypeFile )
                        ::MessageBox ( ::GetDesktopWindow (), item.GetPath(), "File", MB_OK);
		        }
	        }

        }
    }
}

void CScanTaskTestDlg::OnLoadFiles() 
{
    long lType = scanFiles;
    LoadAType ( &lType );	
}

void CScanTaskTestDlg::OnLoadMyComp() 
{
    long lType = scanMyComputer;
    LoadAType ( &lType );	
}

void CScanTaskTestDlg::OnLoadDrives() 
{
    long lType = scanSelectedDrives;
    LoadAType ( &lType );	
}

void CScanTaskTestDlg::JustScanTask ()
{
    CString strMyComputer;
    
    strMyComputer = m_strAllUsers;
    strMyComputer += "\\Tasks\\mycomp.sca";
    
    CScanTask task;
    if ( !task.Load ( strMyComputer, true ))
    {
        ::MessageBox ( ::GetDesktopWindow (), "Failed to Load", "ScanTaskTest", MB_OK );
        return;
    }

    task.EnumReset();

	// Iterate through the list of tasks.

	for (UINT uiItems = task.GetItemCount(), ui = 0U; ui < uiItems; ui++)
	{
		CScanItem item;

		// Add the next item to Scan Manager's items to scan list.

		if (!task.EnumNext(item))
			break;

		// Get the item type.

		SCANITEMTYPE type;
		SCANITEMSUBTYPE subtype;
		item.GetItemType(type, subtype);

		// Is the item a drive?

		if (type == typeDrive)
		{
			char szDrive[] = "X:";
			szDrive[0] = item.GetDrive();
            ::MessageBox ( ::GetDesktopWindow (), szDrive, "Drive", MB_OK);
		}
		
		// If it's not a drive, then it's a file or folder.

		else
		{
			if ( subtype == subtypeFolderAndSubs )
                ::MessageBox ( ::GetDesktopWindow (), item.GetPath(), "Folder+Subs", MB_OK);
            else if ( subtype == subtypeFolder )
                ::MessageBox ( ::GetDesktopWindow (), item.GetPath(), "Folder", MB_OK);
            else if ( subtype == subtypeFile )
                ::MessageBox ( ::GetDesktopWindow (), item.GetPath(), "File", MB_OK);
		}
	}

}

void CScanTaskTestDlg::RefreshScanDates()
{
    ResetScanTasks ();

    SYSTEMTIME stLastScan;
    DWORD      dwBufSize = sizeof(SYSTEMTIME);
    HKEY       hKey;

    if ( ERROR_SUCCESS == RegOpenKeyEx ( HKEY_LOCAL_MACHINE,
                                         _T("SOFTWARE\\Symantec\\Norton AntiVirus\\LastScan"),
                                         0, KEY_QUERY_VALUE, &hKey ))
    {
        if ( ERROR_SUCCESS == RegQueryValueEx (
                                  hKey, _T("SystemTime"), NULL, NULL,
                                  (LPBYTE) &stLastScan, &dwBufSize ))
        {
            COleDateTime timeLastScan (stLastScan);

            m_strScanDateRegKey = timeLastScan.Format( "%B %d, %Y" );

        }

        RegCloseKey ( hKey );
    }
    else
        m_strScanDateRegKey = "None";
    
    CComPtr <INAVScanTask> spTask;

    if ( SUCCEEDED ( m_spTasks->get_MyComputer ( &spTask )))
    {
        DATE dateLastRun;
        if ( S_OK == spTask->get_LastRunTime ( &dateLastRun ))
        {
            m_strScanDateTask = COleDateTime(dateLastRun).Format( "%B %d, %Y" );
        }
        else 
            m_strScanDateTask = "None";
    }

    DWORD dwElapsed = GetDaysElapsed (&stLastScan);
    TCHAR szTemp [100] = {0};
    _ultot ( dwElapsed, szTemp, 10 );
    m_strNSWElapsed = szTemp;

    UpdateData ( FALSE );
}

void CScanTaskTestDlg::ResetScanTasks()
{
    m_spTasks.Release ();
    m_spSymScriptSafe.Release ();

    if ( FAILED (m_spTasks.CoCreateInstance ( CLSID_NAVScanTasks )))
    {
        AfxMessageBox ( "Failed to get NAVTasks" );
    }

	if( SUCCEEDED( m_spTasks.QueryInterface(&m_spSymScriptSafe)))
	{
		// Set appropriate access so that NAVStatus call to IsItSafe() 
		// returns successfully.

		long dwAccess = ESymScriptSafe_UNLOCK_KEYA;
		long dwKey = ESymScriptSafe_UNLOCK_KEYB;
		m_spSymScriptSafe->SetAccess(dwAccess, dwKey);
    }
    else
        AfxMessageBox ( "Failed to get security");
}

// Code taken from NSW 2001 for testing our backwards compatibility 
//
DWORD CScanTaskTestDlg::GetDaysElapsed(SYSTEMTIME* pstStart)
{
    SYSTEMTIME st;
    FILETIME   ftStart, ftNow;
    ULONGLONG  ulStart, ulNow, ulDiff;
    DWORD      dwDaysElapsed;

    // Get the current date and compare it to the start date.  Everything is
    // done with FILETIME's (64-bit numbers) to make the comparisons easy.

    // First convert the start date to a 64-bit number.

    SystemTimeToFileTime ( pstStart, &ftStart );

    ulStart = ftStart.dwHighDateTime;
    ulStart <<= 32;
    ulStart |= ftStart.dwLowDateTime;

    // Ditto for the current date.

    GetLocalTime ( &st );
    SystemTimeToFileTime ( &st, &ftNow );

    ulNow = ftNow.dwHighDateTime;
    ulNow <<= 32;
    ulNow |= ftNow.dwLowDateTime;
   
    // If the start date is in the future, we don't have anything to do,
    // so bail out.

    if ( ulStart > ulNow )
        return 0;


    // Calculate the difference in the dates.

    ulDiff = ulNow - ulStart;

    // u64Diff is now the age in 100 ns intervals.

    ulDiff /= 10000000;                 // Convert to seconds...
    ulDiff /= 60*60*24;                 // and then to days.

    // Convert the age to a DWORD.
    // This will not cause any problems unless the defs are 4.3 billion days 
    // old!

    dwDaysElapsed = static_cast<DWORD>( ulDiff );
    return dwDaysElapsed;
}