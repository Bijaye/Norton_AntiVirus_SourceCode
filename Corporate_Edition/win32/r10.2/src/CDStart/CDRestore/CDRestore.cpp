// CDRestore.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "CDRestore.h"
#include "resource.h"
#include "wow64helpers.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

DWORD InstallSAV(HWND);
BOOL GetAppDirectory(LPTSTR, DWORD);
void DisplayError(HWND, UINT);
DWORD Execute(HWND, LPCTSTR, LPCTSTR, int, bool);

DWORD InstallSAV(HWND hwnd)
{
	DWORD dwReturn = 0;

	TCHAR cszSAV32[] = _T("\\SAV\\SymantecAntiVirusCorporateEdition-Trial.exe");
	TCHAR cszSAV64[] = _T("\\SAVWin64\\x86\\SymantecAntiVirusCorporateEditionWin64-Trial.exe");
	if (IsWin64())
	{
		dwReturn = Execute(hwnd, cszSAV64, NULL, IDS_ERROR_SAV, true);
	}
	else
	{
		dwReturn = Execute(hwnd, cszSAV32, NULL, IDS_ERROR_SAV, true);
	}
	return dwReturn;
}

BOOL GetAppDirectory( LPTSTR szDirectory, DWORD nSize)
{
    if( GetModuleFileName( NULL, szDirectory, nSize ) == FALSE )
    {
        szDirectory[0] = _T('\0');
        return FALSE;
    }

    LPTSTR pTrail = NULL;
    for( LPTSTR p = szDirectory; p && *p; p = CharNext(p) )
    {
        if( *p == _T('\\') )
        {
            pTrail = p;
        }
    }
    
    if( pTrail )
    {
        *pTrail = _T('\0');
    }

    return( pTrail ? TRUE : FALSE );
}

void DisplayError (HWND hwndParent, UINT uErrorString) 
{
    const UINT BUFFER_SIZE = 1024;
    TCHAR szTemp[BUFFER_SIZE];
    TCHAR szTitle[BUFFER_SIZE];

    LoadString(AfxGetResourceHandle(), uErrorString, szTemp, BUFFER_SIZE);
    LoadString(AfxGetResourceHandle(), IDS_ERROR_TITLE, szTitle, BUFFER_SIZE);

    MessageBox(hwndParent, szTemp , szTitle, MB_OK);
}

DWORD Execute(HWND hwnd, LPCTSTR strCmd, LPCTSTR strParam, int nErrorCode, bool bWait)
{
    DWORD dwReturn = ERROR_SUCCESS;
    TCHAR szFilePath[MAX_PATH+1] = _T("");
    TCHAR szDirectory[MAX_PATH+1] = _T("");
    GetAppDirectory( szDirectory, MAX_PATH+1);

    if (_tcslen(szDirectory) + _tcslen(strCmd) <= MAX_PATH)
    {
        _tcscat(szFilePath, szDirectory);
        _tcscat(szFilePath, strCmd);
    }
    else
    {
        DisplayError(hwnd,IDS_ERROR_BUFFER_SIZE);
        return ERROR_BUFFER_OVERFLOW;
    }

    SHELLEXECUTEINFO Info = {0};
	Info.cbSize = sizeof(Info);
	Info.fMask = SEE_MASK_FLAG_NO_UI | SEE_MASK_NOCLOSEPROCESS;
	Info.hwnd = hwnd;
    Info.lpVerb = _T("open");
	Info.lpFile = szFilePath;
	Info.lpParameters = strParam;
	Info.nShow = SW_SHOWNORMAL;

    if (ShellExecuteEx(&Info))
    {
        if (bWait)
        {
            ::ShowWindow(hwnd,SW_HIDE);
            while(MsgWaitForMultipleObjects (1, &Info.hProcess,
				FALSE, 1000, QS_ALLINPUT) != WAIT_OBJECT_0)
		    {
			    MSG msg = {0};
			    while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
			    {
				    TranslateMessage(&msg);
				    DispatchMessage(&msg);
			    };
		    }

            // process has ended - get return value
            ::GetExitCodeProcess(Info.hProcess, &dwReturn);
            ::ShowWindow(hwnd,SW_SHOW);
        }

        ::CloseHandle(Info.hProcess);
    }
    else
    {
        dwReturn = ::GetLastError();
        DisplayError(hwnd, nErrorCode);
    }

    return dwReturn;
}

// CCDRestoreApp

BEGIN_MESSAGE_MAP(CCDRestoreApp, CWinApp)
	ON_COMMAND(ID_HELP, CWinApp::OnHelp)
END_MESSAGE_MAP()


// CCDRestoreApp construction

CCDRestoreApp::CCDRestoreApp()
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}


// The one and only CCDRestoreApp object

CCDRestoreApp theApp;


// CCDRestoreApp initialization

BOOL CCDRestoreApp::InitInstance()
{
	CWinApp::InitInstance();

	// Standard initialization
	// If you are not using these features and wish to reduce the size
	// of your final executable, you should remove from the following
	// the specific initialization routines you do not need
	// Change the registry key under which our settings are stored
	// TODO: You should modify this string to be something appropriate
	// such as the name of your company or organization
	//SetRegistryKey(_T("Local AppWizard-Generated Applications"));
	
	InstallSAV(NULL);


	// Since the dialog has been closed, return FALSE so that we exit the
	//  application, rather than start the application's message pump.
	return FALSE;
}
