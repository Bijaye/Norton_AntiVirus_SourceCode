// --------------------------------------------------------------------------
//                 Symantec AntiVirus Corporate Edition
// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
//    Copyright 2002 - 2003, 2005 Symantec Corporation. All rights reserved.
// --------------------------------------------------------------------------
//
//   FILE:     <scandlgs.cpp>
//
//   MODULE:   <scandlgs.dll>
//
//   DESCRIPTION:
//		CShellscanApp class functions.
//
// --------------------------------------------------------------------------
//

#include "stdafx.h"
#include <atlbase.h>
#include "DarwinResCommon.h"
#include "scandlgs.h"
#include "commisc.cpp"
#include "language.c"
#include "clientreg.h"
#include "user.h"
#include "OSUtils.h"
#include "RegUtils.h"
#include "acta.h"
#include "ls.h"

#include "NavCorpH.h"
#include "NavCorpHStr.h"

#include "vphtmlhelp.h"
#include "SymSaferRegistry.h"
#include "SymSaferStrings.h"
#include "VPExceptionHandling.h"
#include "Resource.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

DARWIN_DECLARE_RESOURCE_LOADER(_T("ScanDlgsRes.dll"), _T("ScanDlgs"))

typedef BOOL (WINAPI *NAVAPFunctionPointer) ();
typedef BOOL (WINAPI *PFnCreateProcessAsUserA)(HANDLE, LPCTSTR, LPTSTR, LPSECURITY_ATTRIBUTES, LPSECURITY_ATTRIBUTES, BOOL, DWORD, LPVOID, LPCSTR, LPSTARTUPINFO, LPPROCESS_INFORMATION);

//These are the hard coded names of htm pages inside savhelp.chm.
#define HTML_HELP_USING_SCAN_RESULTS_HTM	"::/IDH_Scan_Results.htm"
#define HTML_HELP_USING_THREAT_HISTORY_HTM	"::/IDH_Threat_History.htm"
#define HTML_HELP_USING_SCAN_HISTORY_HTM	"::/IDH_Scan_History.htm"
#define HTML_HELP_USING_EVENT_LOG_HTM		"::/IDH_Event_Log.htm"
#define HTML_HELP_USING_TAMPER_HISTORY_HTM	"::/IDH_TamperBehavior_History.htm"
#define HTML_HELP_USING_TAKE_ACTION_HTM		"::/IDH_Take_Action.htm"
//The dialog resource ids defined in LdvpView.
#define IDD_VIRUS_HISTORY               335
#define IDD_EVENT_LOG                   336
#define IDD_SCAN_HISTORY                338
#define IDD_TAMPERBEHAVIOR_HISTORY      344

//Include the LDVPCom stuff
#include "ldvpcom.c"

//Import the guids from vpcommon.h
IMP_VPCOMMON_IIDS

/////////////////////////////////////////////////////////////////////////////
// CShellscanApp

BEGIN_MESSAGE_MAP(CShellscanApp, CWinApp)
	//{{AFX_MSG_MAP(CShellscanApp)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CShellscanApp construction

CShellscanApp::CShellscanApp()
{
	// add construction code here,
	// Place all significant initialization in InitInstance
}

/////////////////////////////////////////////////////////////////////////////
// The one and only CShellscanApp object

CShellscanApp theApp;
HINSTANCE hInstLang = NULL; // For access to acta.str resources.



BOOL CShellscanApp::InitInstance() 
{
//	OleInitialize(NULL);
//	CoInitialize(NULL);
	AfxEnableControlContainer();

	if(SUCCEEDED(g_ResLoader.Initialize()))
		AfxSetResourceHandle(g_ResLoader.GetResourceInstance());

	//Now, load up the proper language help
	CString strLanguage,
			strLangDll;

	if( GetLanguage( strLanguage.GetBuffer( 4 ), strLangDll.GetBuffer(MAX_PATH), "%sRSCA.dll" ) )
	{
		strLanguage.ReleaseBuffer();
		strLangDll.ReleaseBuffer();
	}

	// setup the help path to be savhelp.chm
	if(IsHTMLHelpEnabled())
	{
		if ( GetHelpDir(m_szHelpPath, MAX_PATH) )
		{
			_tcscat(m_szHelpPath,"\\");
		}

		// dwr - 1-22VI55, add SSC help instead of SAV help, make language independent

		// Default help file
		CString sHelpFile = _T("ENUVIEW.CHM");

		// Test for non-english file
		if ( _tcsicmp( strLanguage, _T( "ENU" ) ) ) 
		{
			CString sTemp;
			sTemp.Format("%sVIEW.CHM", LPCTSTR(strLanguage));

			// Does file exist
			if ( _taccess( LPCTSTR(CString(m_szHelpPath) + sTemp), 0 ) ) 
				sHelpFile = sTemp;
		}

		// set help file
		sHelpFile.MakeUpper();
		_tcscat(m_szHelpPath, LPCTSTR(sHelpFile));
	}
	else
	{
		_tcscpy( m_szHelpPath, SAV_HTML_HELP );
	}

    free((void*)m_pszHelpFilePath);
    m_pszHelpFilePath = _tcsdup(m_szHelpPath);

    
    // TCashin 06/03/2002  Ported CRT fix for Siebel Defect # 1-3S7H4 to 8.0
    // BALJIAN 03/22/2002  Fix defect 1-3S7H4
    if (IsWinNT())
    {
        // Build the command that we will use to launch winhlp32.exe.

        // Start with open quote and get the windows directory.

        m_szHelpCommand[0] = _T('\"');
        UINT uResult = GetWindowsDirectory(m_szHelpCommand + 1, sizeof(m_szHelpCommand)/sizeof(*m_szHelpCommand) - 1 );
        if ( (uResult) && (uResult <= (sizeof(m_szHelpCommand)/sizeof(*m_szHelpCommand) - 1) ) )
        {
            // Append htmlhelp.exe and closing quote, then the path to the help file
            ssStrnAppend( m_szHelpCommand, _T("\\htmlhelp.exe\" "), sizeof(m_szHelpCommand) );
            ssStrnAppend( m_szHelpCommand, m_szHelpPath, sizeof(m_szHelpCommand) );
        }
        else
        {
            m_szHelpCommand[0] = _T('\0');
        }
    }
    else
    {
        // Must be Win9x... don't need this member.
        m_szHelpCommand[0] = _T('\0');
    }
    // TCashin 06/03/2002  End CRT fix for Siebel Defect # 1-3S7H4 to 8.0
       
    COleObjectFactory::RegisterAll();

	//Load the AppName
	m_strAppname.LoadString( IDS_APP_NAME );
	m_pszAppName = m_strAppname;

	InitDLL(AfxGetInstanceHandle());

    HKEY hMainKey;
    DWORD dwErr = RegOpenKey(HKEY_LOCAL_MACHINE, REGHEADER, &hMainKey);
    if ( dwErr == ERROR_SUCCESS )
	{
		InitRegUtils(hMainKey);
	}

	return CWinApp::InitInstance();
}

int CShellscanApp::ExitInstance() 
{
    if (m_pszHelpFilePath)
        free((void*)m_pszHelpFilePath);

	m_pszHelpFilePath = NULL;
	m_pszAppName = NULL;
	
	DeinitDLL( AfxGetInstanceHandle() );

	DeinitRegUtils();

//	OleUninitialize();
	return CWinApp::ExitInstance();
}

BOOL CShellscanApp::IsHTMLHelpEnabled()
{
	TCHAR lpBuffer [ _MAX_PATH ];
	
	GetEnvironmentVariable ( NAVCORP_HTML_HELP_ENABLED_VAR, lpBuffer, sizeof (lpBuffer) / sizeof (TCHAR) );
	
	if ( 0 == _tcscmp (lpBuffer, NAVCORP_HTML_HELP_ENABLED_VALUE))
		return TRUE;
	else 
		return FALSE;
}


///////////////////////////////////////////////////////////////////////////////
//
// Function name: CShellscanApp::WinHelpInternal
//
// Description  : Launches the appropriate help.  If winhlp32.exe is used,
//				  create the process as the currently logged on user for
//				  security reasons.
//
// Return type  :  None.
//
//
///////////////////////////////////////////////////////////////////////////////
// 3/22/02 - BALJIAN: Fix defect 1-3S7H4 by getting the access token for
//					  the currently logged on user, and creating the
//					  winhlp32.exe process as that user.
// 6/03/02 - TCASHIN  Moved to CRT fix to 8.0
// 8/01/03 - JGEIGER  Changed call from WinHelp to HTMLHelp
///////////////////////////////////////////////////////////////////////////////
void CShellscanApp::WinHelpInternal(DWORD dwData, UINT nCmd)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));
	
	 
	// Try to determine if we have been invoked by the client (SAV) or the console (SSC).
	// The client prefers to HTMLHelp directly.
	// The console provides us a COM object in order to get HTML help.
	if (!IsHTMLHelpEnabled())	//SAVHelp
    {
        // Filter out F1 help on client, but allow the toolbar help button
        // clicks to get through
        if(dwData <= IDD_EMAIL_MESSAGE) // Compare to highest valued dialog ID
        {
            // TCashin 06/03/2002 Begin CRT fix for Siebel Defect # 1-3S7H4
            if (IsWinNT())
            {
	            HANDLE hAccessToken = NULL;

	            // Get the access token for the user who is logged on right now
	            hAccessToken = GetAccessTokenForLoggedOnUser();

	            if (hAccessToken)
	            {
		            HMODULE hAdvapi = NULL;
		            
					TCHAR szPath[MAX_PATH];
					ZeroMemory(szPath, sizeof(szPath));
					::GetSystemDirectory( szPath, MAX_PATH );
					_tcscat( szPath, _T("\\ADVAPI32.DLL"));
		            hAdvapi = LoadLibrary(szPath);

		            if (hAdvapi)
		            {
			            PFnCreateProcessAsUserA pfnCreateProcessAsUserA = NULL;

			            pfnCreateProcessAsUserA = (PFnCreateProcessAsUserA)
				            GetProcAddress(hAdvapi, _T("CreateProcessAsUserA"));

			            if (pfnCreateProcessAsUserA)
			            {
				            STARTUPINFO			si;
				            PROCESS_INFORMATION pi;
				            BOOL bHelpProcessCreated;

				            memset( &si, 0, sizeof( STARTUPINFO ) );
				            si.cb = sizeof( STARTUPINFO );

							// double quote command to prevent possible execution of unexpected program
                            const size_t nCmdLineSize = _tcslen(m_szHelpCommand) + 3;
							TCHAR *szCmdLine = new TCHAR[nCmdLineSize];
							if (! _tcschr( m_szHelpCommand, _T('\"')))
								sssnprintf( szCmdLine, nCmdLineSize, "\"%s\"", m_szHelpCommand );
							else
								strcpy( szCmdLine, m_szHelpCommand );

							// Use the access token to start WinHelp as the logged on user
				            bHelpProcessCreated = pfnCreateProcessAsUserA(
					                hAccessToken,				// handle to user token
    					            NULL,						// name of executable module
	    				            szCmdLine,					// command-line string
		    			            NULL,						// SD
			    		            NULL,						// SD
				    	            FALSE,						// inheritance option
					                NULL,						// creation flags
					                NULL,						// new environment block
					                NULL,						// current directory name
					                &si,						// startup information
    					            &pi) ;						// process information

							delete [] szCmdLine;

							// dwr, 1-32XOB3, security breach defect
							// The first call above can possibly fail on
							// later Windows platforms, due to the renaming
							// of the help file launcher.  Below, we try the
							// newer launcher, hh.exe versus htmlhelp.exe.
							if (!bHelpProcessCreated)
							{
								// try alternate help launcher
								TCHAR szHelp[MAX_PATH+2];
								TCHAR szWindows[MAX_PATH];
								UINT uResult = GetWindowsDirectory(szWindows, MAX_PATH);
								if ((uResult) && (uResult <= MAX_PATH))
								{
									_tcscpy( szHelp, _T("\"") );
                                    ssStrnAppend( szHelp, szWindows, sizeof(szHelp) );

									// construct fully qualified help line
                                    ssStrnAppend( szHelp, _T("\\hh.exe\" "), sizeof(szHelp) );
                                    ssStrnAppend( szHelp, m_szHelpPath, sizeof(szHelp) );

									//Srikanth 3/28/05 Defect 1-3SKUVJ
									//The help context cannot be passed to CreateProcessAsUser, as context sensitive
									//help cannot be invoked by passing the IDDs as command line arguments.
									//Html Help does let the specific htm file to be passed in syntax "hh <path\.chm file>::/<specific .htm file>.
									//In lieu of no context, the main page of the help file will be displayed.
									//Note: As a side note for future reference, windows help will add 0x20000 to the dialog id and try to match with the ids in the .chm file.
									switch(dwData)
									{
									case IDD_VIRUS_FOUND:
									case RV_TYPE_SCAN:
										ssStrnAppend(szHelp, HTML_HELP_USING_SCAN_RESULTS_HTM, sizeof(szHelp));
										break;
									case IDD_VIRUS_HISTORY:
									case RV_TYPE_VIRUSES:
										ssStrnAppend(szHelp, HTML_HELP_USING_THREAT_HISTORY_HTM, sizeof(szHelp));
										break;
									case IDD_EVENT_LOG:
									case RV_TYPE_EVENTS:
										ssStrnAppend(szHelp, HTML_HELP_USING_EVENT_LOG_HTM, sizeof(szHelp));
										break;
									case IDD_SCAN_HISTORY:
									case RV_TYPE_SCANHISTORY:
										ssStrnAppend(szHelp, HTML_HELP_USING_SCAN_HISTORY_HTM, sizeof(szHelp));
										break;
									case IDD_TAMPERBEHAVIOR_HISTORY:
									case RV_TYPE_TAMPERBEHAVIOR:
										ssStrnAppend(szHelp, HTML_HELP_USING_TAMPER_HISTORY_HTM, sizeof(szHelp));
										break;
									case IDD_GETSTATUS:
										ssStrnAppend(szHelp, HTML_HELP_USING_TAKE_ACTION_HTM, sizeof(szHelp));
										break;
									}

									bHelpProcessCreated = pfnCreateProcessAsUserA(
											hAccessToken,				// handle to user token
											NULL,						// name of executable module
											szHelp,						// command-line string
											NULL,						// SD
											NULL,						// SD
											FALSE,						// inheritance option
											NULL,						// creation flags
											NULL,						// new environment block
											NULL,						// current directory name
											&si,						// startup information
											&pi) ;						// process information
								}
							}

							if (!bHelpProcessCreated)
                            {
                                // CreateProcessAsUser() will fail if ScanDlgs is created by the 
                                // within the users context, rather than by RTVScan. Invoke
                                // HTMLHelp the regular way.

								::HtmlHelp( AfxGetMainWnd()->GetSafeHwnd(),
                                    m_szHelpPath,
									HH_HELP_CONTEXT,
									dwData);
                            }
							else
							{
								CloseHandle(pi.hProcess);
								CloseHandle(pi.hThread);
							}
			            }

			            FreeLibrary(hAdvapi);
			            hAdvapi = NULL;
		            }
	            
	            
		            CloseHandle(hAccessToken);
		            hAccessToken = NULL;
	            }
            }
            else
            {
	            // Must be Windows 9x, just call HTMLHelp directly
				::HtmlHelp( AfxGetMainWnd()->GetSafeHwnd(),
                        m_szHelpPath,
						HH_HELP_CONTEXT,
                        dwData);
            }
            // TCashin 06/03/2002 End CRT fix for Siebel Defect # 1-3S7H4


        }
        else
        {
            return;
        }
    }
	else	//SSC help
	{
		BSTR bstrHelpFile;
		CString strHelpFile = m_szHelpPath;
		bstrHelpFile = strHelpFile.AllocSysString();
		if (SUCCEEDED(CoInitialize (NULL)))
		{
			INavCorpConsoleHelp *pIConsoleHTMLHelp;
			if( S_OK == CoCreateInstance( CLSID_NavCorpConsoleHelp, NULL, CLSCTX_INPROC_SERVER, IID_INavCorpConsoleHelp, (void**)&pIConsoleHTMLHelp ) )
			{
				// Invoke the help.
				TRACE1( _T("ShowContextHelp ( %d )\n"),dwData);
				pIConsoleHTMLHelp->ShowContextHelp ( bstrHelpFile, dwData );
				pIConsoleHTMLHelp->Release();
			}
			CoUninitialize();
		}
		SysFreeString (bstrHelpFile );
	}

}

BOOL CShellscanApp::GetHelpDir(LPTSTR szHelpPath, UINT nMax)
{
    const TCHAR szSubKey[] = _T("SOFTWARE\\Symantec\\Symantec AntiVirus Management Snap-In");

    BOOL bRet = FALSE;
    HKEY hkey;

    if (RegOpenKey (HKEY_LOCAL_MACHINE, szSubKey, &hkey) == ERROR_SUCCESS)
    {
        DWORD dwSize = nMax < _MAX_PATH ? nMax : _MAX_PATH;
        TCHAR szTemp[_MAX_PATH];

        DWORD dwRet = SymSaferRegQueryStringValue (hkey, _T("HELPDir"), szTemp, &dwSize);

        if (dwRet == ERROR_SUCCESS)
        {
            vpstrncpy(szHelpPath, szTemp, nMax);
            bRet = TRUE;
        }
        RegCloseKey (hkey);
    }
    return bRet;
}

///////////////////////////////////////////////////////////////////////////////
//
// Function name: EnableAP
//
// Description  : 
//
// Return type  : void 
//
//
///////////////////////////////////////////////////////////////////////////////
// 3/31/99 - DBUCHES: Function created / header added 
// 4/11/99 - TCASHIN: Borrowed from QConsole
// 9/18/99 - MMENDON: Added FreeLibrary to this function
///////////////////////////////////////////////////////////////////////////////
void EnableAP()
{

}


///////////////////////////////////////////////////////////////////////////////
//
// Function name: DisableAP
//
// Description  : 
//
// Return type  : void 
//
//
///////////////////////////////////////////////////////////////////////////////
// 3/31/99 - DBUCHES: Function created / header added 
// 4/11/99 - TCASHIN: Borrowed from QConsole
// 9/18/99 - MMENDON: Moved LoadLibrary to this function
///////////////////////////////////////////////////////////////////////////////
void DisableAP()
{
    

}




///////////////////////////////////////////////////////////////////////
// Exports Nessesary to support COM in MFC

STDAPI DllGetClassObject(REFCLSID rclsid, REFIID riid, LPVOID* ppv)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	return AfxDllGetClassObject(rclsid, riid, ppv);
}

STDAPI DllCanUnloadNow(void)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	return AfxDllCanUnloadNow();
}

// by exporting DllRegisterServer, you can use regsvr.exe
STDAPI DllRegisterServer(void)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	COleObjectFactory::UpdateRegistryAll();

	// we need to manual add theading model Aparentment
	HKEY hKey=NULL;
	if(ERROR_SUCCESS == RegOpenKeyEx( HKEY_CLASSES_ROOT,"CLSID\\{4DEF8DD1-C4D1-11D1-82DA-00A0C9749EEF}\\InProcServer32",0,KEY_READ|KEY_WRITE,&hKey))
	{
		RegSetValueEx(hKey,"ThreadingModel",0,REG_SZ,(LPBYTE)"Apartment",lstrlen("Apartment")+1);
 		RegCloseKey(hKey);
	}

	return S_OK;
}


// by exporting DllRegisterServer, you can use regsvr.exe
STDAPI DllUnregisterServer(void)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	//COleObjectFactory::UpdateRegistryAll();
	return S_OK;
}





