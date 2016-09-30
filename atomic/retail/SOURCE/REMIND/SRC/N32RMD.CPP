////////////////////////////////////////////////////////////////////////////////////
// $Header:   S:/REMIND/VCS/n32rmd.cpv   1.7   13 Jul 1998 09:50:18   jtaylor  $
////////////////////////////////////////////////////////////////////////////////////
//
// n32rmd.cpp : Defines the class behaviors for the application.
//
////////////////////////////////////////////////////////////////////////////////////
// $Log:   S:/REMIND/VCS/n32rmd.cpv  $
// 
//    Rev 1.7   13 Jul 1998 09:50:18   jtaylor
// Updated the program to get the NAV version number from NAVVER.h
//
//    Rev 1.6   18 Dec 1997 16:08:08   jtaylor
// Let the run once version update the time for next display
//
//    Rev 1.5   16 Dec 1998 17:52:48   jtaylor
// Updated the removal logic.
//
//    Rev 1.4   15 Dec 1997 18:12:46   jtaylor
// Fixed it to verify the time left with every timer event.  Made the run once version not use the timer.  Made the GetSecondsTilLaunch() return 0 when registry read failed.
//
//    Rev 1.3   15 Dec 1997 17:33:58   jtaylor
// fixed the logout problem, updated the timer logic.
//
////////////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "n32rmd.h"
#include "n32rmdDlg.h"
#include "n32behav.h"
#include "navver.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define IS_NT      (BOOL)(GetVersion() < 0x80000000)

#define MILISECONDS_IN_DAY 86400000

BOOL    g_bRunOnce;
CN32rmdDlg *pDlg;

void CALLBACK MyOnTimer(HWND hwnd,UINT uMsg,UINT idEvent,DWORD dwTime )
{
	CN32rmdApp *pApp = (CN32rmdApp *)AfxGetApp();

	// Uninstall the program if NAV has been upgraded.
	if( !NAV_IS_SCANONLY() )
	{
		HKEY	hKey;
		DWORD	dwBufferSize = 256;

		LONG    lReturn = ERROR_INVALID_DATA;
        lReturn = RegOpenKeyEx( HKEY_LOCAL_MACHINE, _T("SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Run"), 0, KEY_SET_VALUE, &hKey );

		if ( lReturn == ERROR_SUCCESS && hKey != NULL )
            lReturn = RegDeleteValue( hKey, _T("Norton AntiVirus Reminder"));

		RegCloseKey(hKey);

		KillTimer(hwnd,idEvent);
		// Destroy the window
		pApp->GetMainWnd()->DestroyWindow();
		return;
	}

	// update the number of miliseconds to wait.  User might have tripped the app.
	DWORD dwSecondsTillLaunch = pApp->GetSecondsTillLaunch();
	pApp->SetMilisecondsLeft( dwSecondsTillLaunch * 1000 );

	// Keep sleeping for a day if more than a day is left
	if( pApp->GetMilisecondsLeft() >= MILISECONDS_IN_DAY )
	{
		// Have it sleep for another day.
		pApp->SetMilisecondsLeft( pApp->GetMilisecondsLeft() - MILISECONDS_IN_DAY );
		return;
	}

	// Kill the timer and set it up for a shorter increment if appropriate.
	KillTimer(hwnd,idEvent);

	// Sleep if there is any time left
	if( pApp->GetMilisecondsLeft() > 0 )
	{
		pApp->GetMainWnd()->SetTimer(256,pApp->GetMilisecondsLeft(),&MyOnTimer);
		pApp->SetMilisecondsLeft( 0 );
		return;
	}

	// Display the dialog
	pDlg->ShowWindow(SW_SHOWNORMAL);
}

/////////////////////////////////////////////////////////////////////////////
// CN32rmdApp

BEGIN_MESSAGE_MAP(CN32rmdApp, CWinApp)
	//{{AFX_MSG_MAP(CN32rmdApp)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG
	ON_COMMAND(ID_HELP, CWinApp::OnHelp)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CN32rmdApp construction

CN32rmdApp::CN32rmdApp()
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}

/////////////////////////////////////////////////////////////////////////////
// The one and only CN32rmdApp object

CN32rmdApp theApp;

/////////////////////////////////////////////////////////////////////////////
// CN32rmdApp initialization

BOOL CN32rmdApp::InitInstance()
{
	// Standard initialization
	// If you are not using these features and wish to reduce the size
	//  of your final executable, you should remove from the following
	//  the specific initialization routines you do not need.

#ifdef _AFXDLL
	Enable3dControls();			// Call this when using MFC in a shared DLL
#else
	Enable3dControlsStatic();	// Call this when linking to MFC statically
#endif

	pDlg = new CN32rmdDlg();
	m_pMainWnd = pDlg;

    g_bRunOnce = TRUE;
    // Check to see if this program was launched with the /nores switch
    LPTSTR lpszCmdLine = GetCommandLine();

	// If the /res switch is present, have the program go memory resident after displaying.
	// If not, have it popup once immediately and then exit.
    _tcslwr(lpszCmdLine);
    LPTSTR lpszCommand = _tcsstr(lpszCmdLine,_T("/res"));
    if( NULL != lpszCommand )
        g_bRunOnce = FALSE;

	pDlg->Create(IDD_N32RMD_DIALOG,NULL);

	// Uninstall the program if NAV has been upgraded.
	if( !NAV_IS_SCANONLY() )
	{
		HKEY	hKey;
		DWORD	dwBufferSize = 256;

		LONG    lReturn = ERROR_INVALID_DATA;
        lReturn = RegOpenKeyEx( HKEY_LOCAL_MACHINE, _T("SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Run"), 0, KEY_SET_VALUE, &hKey );

		if ( lReturn == ERROR_SUCCESS && hKey != NULL )
            lReturn = RegDeleteValue( hKey, _T("Norton AntiVirus Reminder"));

		RegCloseKey(hKey);

		// Destroy the window
		m_pMainWnd->DestroyWindow();
		return FALSE;
	}

	if( !g_bRunOnce )
	{
		// This returns the number of seconds till launch, or 0 if there is no time left.
		DWORD dwSecondsTillLaunch = GetSecondsTillLaunch();

		// Make sure there
		if( dwSecondsTillLaunch <= 0 )
			dwSecondsTillLaunch = 1;

		// Take the excess time and put it in the buffer.
		if( dwSecondsTillLaunch * 1000 > MILISECONDS_IN_DAY )
		{
			m_pMainWnd->SetTimer(256,MILISECONDS_IN_DAY,&MyOnTimer);
			m_dwTimeLeft = dwSecondsTillLaunch * 1000 - MILISECONDS_IN_DAY;
		}
		else
		{
			m_pMainWnd->SetTimer(256,dwSecondsTillLaunch * 1000,&MyOnTimer);
			m_dwTimeLeft = 0;
		}
	}
	else
		pDlg->ShowWindow(SW_SHOWNORMAL);


	return TRUE;
}

void CN32rmdApp::ReadReminderFile(LPTSTR lpszFullText, DWORD dwSize)
{
	TCHAR szTest[256] = "";
    TCHAR szRegPath[256] = "";
	LPTSTR lpszTest = szTest;
	HKEY	hKey;
	CString test;
	DWORD	dwBufferSize = sizeof(szTest);

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
                               NULL, (unsigned char *)lpszTest, &dwBufferSize );
	}
	RegCloseKey(hKey);

    _tcscat(lpszTest,_T("\\remind.dat"));
    TRY
    {
        CFile file(lpszTest,CFile::modeRead|CFile::shareDenyNone);
        file.Read(lpszFullText,dwSize);
        file.Close();
    }
    CATCH( CFileException, e )
    {
        // If the file cannot be read, abort execution.
        exit(-1);
    }
    END_CATCH
}

time_t CN32rmdApp::GetLaunchTime()
{
	TCHAR szTest[256] = "";
	LPTSTR lpszTest = szTest;
	HKEY	hKey;
	CString test;
	time_t  tLaunchTime;
	DWORD	dwBufferSize = sizeof(tLaunchTime);

    LONG            lReturn = ERROR_INVALID_DATA;

	// Find out how long it is until the dialog should be presented
    lReturn = RegOpenKeyEx( HKEY_LOCAL_MACHINE, _T("SOFTWARE\\Symantec\\Norton AntiVirus\\Reminder"), 0, KEY_READ | KEY_WRITE, &hKey );

	if ( lReturn == ERROR_SUCCESS && hKey != NULL )
	{
        lReturn = RegQueryValueEx( hKey, _T("RemindTime"), NULL,
                               NULL, (LPBYTE)&tLaunchTime, &dwBufferSize );
	}
	RegCloseKey(hKey);

	if(lReturn != ERROR_SUCCESS)
		return 0;

	return tLaunchTime;
}

DWORD CN32rmdApp::GetSecondsTillLaunch()
{
	DWORD	dwSecondsTillLaunch;

	// Find out when the dialog was supposed to be launched.
	CTime	tmLaunchTime(GetLaunchTime());

	if(0 == GetLaunchTime())
		return 0;

	CTime	tmCurrentTime = CTime::GetCurrentTime();

	// IF the time to launch has already passed, return 0
	if( tmLaunchTime < tmCurrentTime )
		return 0;

	// Calculate the time till launch.
	CTimeSpan tsTimeTillLaunch = tmLaunchTime - tmCurrentTime;

	// Only return a positive number.
	dwSecondsTillLaunch = tsTimeTillLaunch.GetTotalSeconds();
	if( dwSecondsTillLaunch > 0 )
		return dwSecondsTillLaunch;

	return 0;
}

void CN32rmdApp::IncrementLaunchTime()
{
	HKEY	hKey;
	time_t  tLaunchTime;
	DWORD	dwBufferSize = sizeof(tLaunchTime);
    LONG    lReturn = ERROR_INVALID_DATA;

	// Base all new times off the current time.
	CTime tmLaunchTime = CTime::GetCurrentTime();

	// Set the new time in the registry
	CTimeSpan tsTimeBetweenLaunches(30,0,0,0);
	tmLaunchTime += tsTimeBetweenLaunches;
	tLaunchTime = tmLaunchTime.GetTime();

    lReturn = RegOpenKeyEx( HKEY_LOCAL_MACHINE, _T("SOFTWARE\\Symantec\\Norton AntiVirus\\Reminder"), 0, KEY_WRITE, &hKey );

    if ( lReturn == ERROR_SUCCESS && hKey != NULL )
	{
        lReturn = RegSetValueEx( hKey, _T("RemindTime"), NULL,
                               REG_BINARY, (LPBYTE)&tLaunchTime, dwBufferSize );
	}
	RegCloseKey(hKey);

    // If there was a problem creating the remind time registry key, then
    // stop the program from running around again.
    if ( lReturn != ERROR_SUCCESS )
        g_bRunOnce = TRUE;
}


int CN32rmdApp::ExitInstance()
{
	// TODO: Add your specialized code here and/or call the base class
	//Free up the memory for the dialog
	if( pDlg )
	{
		delete pDlg;
		pDlg = NULL;
	}

	return CWinApp::ExitInstance();
}

// This is called during the dialog's onOk handler.
void CN32rmdApp::DialogClosed()
{
	// Setup the timer for the next iteration.
	IncrementLaunchTime();

	if( TRUE == g_bRunOnce )
	{
		// Close the application
		m_pMainWnd->DestroyWindow();
	}
	else
	{
		DWORD dwSecondsTillLaunch = GetSecondsTillLaunch();

		// If the increment is more than a day, sleep for 1 day.
		if( dwSecondsTillLaunch * 1000 >= MILISECONDS_IN_DAY )
		{
			m_pMainWnd->SetTimer(256,MILISECONDS_IN_DAY,&MyOnTimer);
			SetMilisecondsLeft( dwSecondsTillLaunch * 1000 - MILISECONDS_IN_DAY );
			return;
		}

		// If the increment is less than a day but > 0 sleep for the whole time.
		if( dwSecondsTillLaunch > 0 )
		{
			m_pMainWnd->SetTimer(256,dwSecondsTillLaunch * 1000,&MyOnTimer);
			SetMilisecondsLeft( 0 );
			return;
		}

		// Otherwise, do not sleep.....close the application.
		// do not have it repeat the dialog infinitely.
		m_pMainWnd->DestroyWindow();
	}
}

DWORD CN32rmdApp::GetMilisecondsLeft()
{
	return m_dwTimeLeft;
}

VOID CN32rmdApp::SetMilisecondsLeft(DWORD dwTime)
{
	m_dwTimeLeft = dwTime;
}
