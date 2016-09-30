//////////////////////////////////////////////////////////////////////////////
// PROPRIETARY / CONFIDENTIAL.
// Use of this product is subject to license terms.
// Copyright © 2005 Symantec Corporation.
// All rights reserved.
//////////////////////////////////////////////////////////////////////////////
//
// qconsole.cpp : Defines the class behaviors for the application.
//
/////////////////////////////////////////////////////////////////////////////
// $Log:   S:/QCONSOLE/VCS/qconsole.cpv  $
// 
//    Rev 1.12   14 Aug 1998 12:59:12   mdunn
// Changed trial-expired message to support varying-length trial periods.
// 
//    Rev 1.11   14 Jul 1998 16:05:24   mdunn
// D'oh... include tryint.h only in trial versions.
// 
//    Rev 1.10   14 Jul 1998 15:52:08   mdunn
// Added #include <initguid.h> to get trialware GUIDs declared properly.
// 
//    Rev 1.9   10 Jul 1998 15:10:10   DBuches
// Ignore files passed in on command line.
// 
//    Rev 1.8   01 Jul 1998 18:10:38   VGLOSTE
// Made the function IsTrialPeriodOverridden() a member function instead 
// of a static function.
// 
//    Rev 1.7   29 Jun 1998 16:08:12   VGLOSTE
// Added code to respect a regkey that overrides the trial length.
// 
//    Rev 1.6   17 Jun 1998 10:42:14   mdunn
// Added a very, very important note by the CreateMutex() call.
// 
//    Rev 1.5   11 Jun 1998 15:48:26   DBuches
// Added support for trial ware.
// 
//    Rev 1.4   07 May 1998 15:08:12   DBuches
// Added support for status text.
// 
//    Rev 1.3   04 May 1998 20:50:00   DBuches
// Only prompt for scan when there are items to scan.
// 
//    Rev 1.2   28 Apr 1998 15:24:58   DBuches
// Added single instance support.  Also hooked up NAV about box.
// 
//    Rev 1.1   07 Apr 1998 16:50:38   DBuches
// 1st pass at startup scanning.
// 
//    Rev 1.0   27 Feb 1998 15:08:58   DBuches
// Initial revision.
/////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include <InitGuid.h>
#define INITGUID
#include "iscandeliver.h"
#define SIMON_INITGUID
#include "SIMON.h"

#include "qconsole.h"

// PEP Headers
#define _INIT_V2LICENSINGAUTHGUIDS
#define _V2AUTHORIZATION_SIMON
#define INIT_V2AUTHORIZATION_IMPL
#define _DRM_V2AUTHMAP_IMPL
#define _SIMON_PEP_
#define _AV_COMPONENT_CLIENT
#include "ComponentPepHelper.h"
#include "PepClientBase.h"
#include "cltPepConstants.h"
#include "PepMiddleMan.h"
#include "SuiteOwnerHelper.h"

#include "MainFrm.h"
#include "qconsoleDoc.h"
#include "qconsoleView.h"
#include "const.h"
#include "qresinit.h"
#include "navdoabt.h"
#include "aboutplg.h"
#include "navver.h"
#include "IntroDlg.h"
#include "AVRESBranding.h"
#include "NAVLicenseNames.h"

// Licensing
#include "ccModuleId.h"
#include "AVccModuleId.h"
#include "NAVError.h"
#include "NAVErrorResource.h"

#define _INIT_COSVERSIONINFO
#include "SSOsinfo.h"

// ccLib delay loader
// This will allow the ccLib dll to delay load at run-time
#include "ccLibDllLink.h"
#include "ccSymDelayLoader.h"
ccSym::CDelayLoader g_DelayLoader;

#include "ccTrace.h"
#include "ccSymDebugOutput.h"
ccSym::CDebugOutput g_DebugOutput(_T("QConsole"));
IMPLEMENT_CCTRACE(::g_DebugOutput);

// Crash handler
#include "ccSymCrashHandler.h"
ccSym::CCrashHandler g_CrashHandler;

#include "OSInfo.h"

#include "NAVOptHelperEx.h"
#include "InstOptsNames.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

// SymInterface map for ccVerifyTrust static lib
SYM_OBJECT_MAP_BEGIN()                          
SYM_OBJECT_MAP_END()                            


/////////////////////////////////////////////////////////////////////////////
// Local functions

BOOL CALLBACK EnumWindowProc(HWND hwnd, // handle to parent window 
							 LPARAM lParam); // application-defined value 


/////////////////////////////////////////////////////////////////////////////
// CQconsoleApp

BEGIN_MESSAGE_MAP(CQconsoleApp, CWinApp)
	//{{AFX_MSG_MAP(CQconsoleApp)
	ON_COMMAND(ID_APP_ABOUT, OnAppAbout)
	// NOTE - the ClassWizard will add and remove mapping macros here.
	//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CQconsoleApp construction
SIMON::CSimonModule _SimonModule;

CQconsoleApp::CQconsoleApp()
{
	// Place all significant initialization in InitInstance
	m_nShowCmd = SW_SHOWNORMAL;	
	m_hMutex = NULL;
	m_bQConsoleEnabled = TRUE;
}


/////////////////////////////////////////////////////////////////////////////
// The one and only CQconsoleApp object
CQconsoleApp theApp;


/////////////////////////////////////////////////////////////////////////////
// CQconsoleApp initialization

BOOL CQconsoleApp::InitInstance()
{
	CCTRACEI(_T("%s - Start"), __FUNCTION__);

#ifdef _DEBUG
	MessageBox(NULL, _T("Attach to Debug CQconsoleApp::InitInstance()"), _T("QConsole Debug Break"), MB_OK);
#endif

	// ** WARNING **  If you change the name of the mutex, you MUST also
	// make the same change in the RemoveQConsole() function in NAVINST.CPP
	// of BOTH the NAVINST95 and NAVINSTNT projects.
	m_hMutex = CreateMutex(NULL, FALSE, _T("Quarantine Console Mutex"));

	// Q: Is there another instance of us hanging around?
	if(GetLastError() == ERROR_ALREADY_EXISTS)
	{
        CCTRACEI(_T("%s - \"Quarantine Console Mutex\" already exists"), __FUNCTION__);

		// Locate other window and bring to front.
		HWND hFound = NULL;
		EnumWindows(EnumWindowProc, (LPARAM) &hFound);
		if(hFound != NULL)
		{
			if(IsIconic(hFound))
				ShowWindow(hFound, SW_RESTORE);
			::SetForegroundWindow(hFound);
			return FALSE;
		}
	}

	// Enable the crash handler
    if(g_CrashHandler.LoadOptions() == FALSE)
        CCTRACEE(_T("%s - g_CrashHandler.LoadOptions() == FALSE"), __FUNCTION__);

	if(!IsCfgWizFinished())
	{
		CCTRACEI(_T("%s - ConfigWiz not completed, launch it and exit QConsole."), __FUNCTION__);
		LaunchCfgWiz();
		return FALSE;
	}

    // Initialize OLE
	AfxOleInit();

    // We need to skip certain functionality in safe mode
    bool bIsSafeMode = false;
    if(0 != GetSystemMetrics(SM_CLEANBOOT))
    {
        bIsSafeMode = true;
        CCTRACEI(_T("%s - Running in safe mode"), __FUNCTION__);
    }

	// Must initialize license state here before
	//  create main frame because document in frame
	//  will need to know whether it should enable or disable
	// certain functions.
	DJSMAR00_LicenseState state = DJSMAR00_LicenseState_Violated;

    if(!bIsSafeMode)
	{
#ifndef _DEBUG
		STAHLSOFT_HRX_TRY(hr)
		{
			// Init license state

			// Getting licensing state property needed to enable product features
			CPEPClientBase pepBase;
			hrx << pepBase.Init();
			pepBase.SetContextGuid(clt::pep::CLTGUID);
			pepBase.SetComponentGuid(V2LicensingAuthGuids::SZ_ANTIVIRUS_COMPONENT);
			pepBase.SetPolicyID(clt::pep::POLICY_ID_QUERY_LICENSE_RECORD);
			pepBase.SetCollectionDWORD(clt::pep::SZ_PROPERTY_DISPOSITION,clt::pep::DISPOSITION_NONE);
			pepBase.SetCollectionDWORD(SUBSCRIPTION_STATIC::SZ_SUBSCRIPTION_PROPERTY_DISPOSITION,SUBSCRIPTION_STATIC::DISPOSITION_NONE);

			hrx << pepBase.QueryPolicy();

			hrx << pepBase.GetValueFromCollection(DRM::szDRMLicenseState, (DWORD &)state, DJSMAR00_LicenseState_Violated);

			if((state == DJSMAR00_LicenseState_PURCHASED)
				|| (state == DJSMAR00_LicenseState_TRIAL))
			{
				CCTRACEI(_T("%s - License state Purchased or Trial. Set m_bQConsoleEnabled = TRUE"), __FUNCTION__);
				m_bQConsoleEnabled = TRUE;
			}
			else
			{
				CCTRACEI(_T("%s - License state NOT Purchased or Trial. Set m_bQConsoleEnabled = FALSE"), __FUNCTION__);
				m_bQConsoleEnabled = FALSE;
			}
		}
		STAHLSOFT_HRX_CATCH_ALL_NO_RETURN(hr);
#endif // _DEBUG
	}

	CBrandingRes BrandRes;
	m_sAppName = BrandRes.GetString(IDS_QUARANTINE_APP_NAME);

    COSInfo OSInfo;
    if(!OSInfo.IsAdminProcess())
    {
		CCTRACEI(_T("%s - User is not admin. Exit."), __FUNCTION__);

		CString sMessage;
        sMessage.Format(IDS_ADMIN_RIGHTS, m_sAppName);
        MessageBox(NULL, sMessage, m_sAppName, MB_ICONWARNING | MB_OK);
        return FALSE; // bail out now.. don't decrement reboot count or anything
    }

	// Load our resource DLL
	InitQConsoleResources();

	// Change the registry key under which our settings are stored.
	// You should modify this string to be something appropriate
	// such as the name of your company or organization.
	SetRegistryKey(SYMANTEC);

	// Register the application's document templates.  Document templates
	//  serve as the connection between documents, frame windows and views.

	CSingleDocTemplate* pDocTemplate;
	pDocTemplate = new CSingleDocTemplate(
		IDR_MAINFRAME,
		RUNTIME_CLASS(CQconsoleDoc),
		RUNTIME_CLASS(CMainFrame),       // main SDI frame window
		RUNTIME_CLASS(CQconsoleView));
	AddDocTemplate(pDocTemplate);

	// Parse command line for standard shell commands, DDE, file open
	CCommandLineInfo cmdInfo;
	ParseCommandLine(cmdInfo);

	// Ignore files passed in on command line.
	cmdInfo.m_nShellCommand = CCommandLineInfo::FileNew;
	if(ProcessShellCommand(cmdInfo) == FALSE)
	{
		CCTRACEE(_T("%s - !ProcessShellCommand()"), __FUNCTION__);
		return FALSE;
	}

	// The one and only window has been initialized, so show and update it.
	m_pMainWnd->ShowWindow(m_nShowCmd);
	m_pMainWnd->UpdateWindow();

	CQconsoleDoc* pDoc = (CQconsoleDoc*) ((CFrameWnd*)m_pMainWnd)->GetActiveDocument();
	ASSERT(NULL != pDoc);

	if(m_bQConsoleEnabled)
	{
		if(pDoc->GetIntroEnabled())
		{
			// Intro dialog
			CIntroDlg dlgIntro;
			INT_PTR nIntroRet = -1;
			nIntroRet = dlgIntro.DoModal();
			if(IDOK == nIntroRet)
				pDoc->SetIntroEnabled(dlgIntro.GetIntroEnabled());
			else if(-1 == nIntroRet)
				CCTRACEE(_T("%s - dlgIntro.DoModal()=%d. Could not create dialog"), __FUNCTION__, nIntroRet);
		}

		// Check to see if we need to do a scan at startup.
		BOOL bStartScan;
		pDoc->m_pOpts->GetScanAtStartup(&bStartScan);

		int iItemCount = pDoc->GetQuarantineItemCount();

		if(bStartScan && iItemCount && pDoc->GetStartupScanNeeded())
		{
			CString sTitle((LPCTSTR) IDS_NEW_DEFS_AVAILIABLE);
			CString sText((LPCTSTR) IDS_NEW_DEFS_AVAILIABLE_TEXT);

			if(m_pMainWnd->MessageBox(sText, sTitle, MB_ICONQUESTION | MB_YESNO) == IDYES)
				pDoc->UpdateAllViews(NULL, LISTVIEW_NEW_DEFS, 0);
		}
	}
	else
	{
		// This used to show the old SymTrial message.  We might want to
		// show a new message saying why QConsole ran anyway (it runs so
		// the user can remove stuff from Quarantine, but that's all they
		// can do).

		switch(state)
		{
		case DJSMAR00_LicenseState_EXPIRED:
			{
				// Request DefAlert to display license expired nag and exit.	
				HWND hWndDefAlert = FindWindow(SZ_DEFALERT_WNDCLASSNAME, SZ_DEFALERT_WNDNAME);
				if(hWndDefAlert)
				{
					UINT uLicenseAlertMsgID = ::RegisterWindowMessage(MSG_NAV_LICENSING_ALERT);
					PostMessage(hWndDefAlert, uLicenseAlertMsgID, 0, (LPARAM) m_pMainWnd->m_hWnd);
				}
			}
			break;

		case DJSMAR00_LicenseState_Violated:
			{
				STAHLSOFT_HRX_TRY(hr)
				{
					// Display license violated nag using ccError.
					CComBSTR bstrNAVErr(_T("NAVError.NAVCOMError"));
					CComPtr <INAVCOMError> spNavError;

					// Create, Populate, Log, and Display the error
					hrx << spNavError.CoCreateInstance(bstrNAVErr, NULL, CLSCTX_INPROC_SERVER);
					hrx << spNavError->put_ModuleID(AV_MODULE_ID_QUARANTINE);		// Module ID of DefAlert
					hrx << spNavError->put_ErrorID(IDS_LICENSE_VIOLATED); // Error ID 
					hrx << spNavError->put_ErrorResourceID(IDS_NAVERROR_INTERNAL_REINSTALL);	// Display generic error message
					hrx << spNavError->LogAndDisplay(reinterpret_cast<unsigned long>(m_pMainWnd->m_hWnd));
				}
				STAHLSOFT_HRX_CATCH_ALL_NO_RETURN(hr)
			}
			break;

		default:
			break;
		}
	}

	// Process all messages in our queue.
	MSG msg;
	while(::PeekMessage(&msg, NULL, 0, 0, PM_NOREMOVE))
		PumpMessage();

	// If there are expanded threat items, set status text.
	if(pDoc->GetExpandedThreatItemCount())
		((CFrameWnd*)AfxGetMainWnd())->SetMessageText(IDS_STATUS_TEXT_THREAT_BACKUP);

	CCTRACEI(_T("%s - End"), __FUNCTION__);

	return TRUE;
}


/////////////////////////////////////////////////////////////////////////////
// Launch About box...

// App command to run the dialog
void CQconsoleApp::OnAppAbout()
{
	HKEY  hBaseKey;
	DWORD dwValueType;
	BOOL  bAbort = FALSE;

	// Store the NAV directory in this string:
	TCHAR szNAVDir[MAX_PATH];
	DWORD dwNAVDirLen = sizeof(szNAVDir);

	// Get the NAV install directory from the CmdLines key.
	//
	// Oops... Fixed KEY_ALL_ACCESS bug under NT...
	// CBROWN 09/22/1999
	if(ERROR_SUCCESS == RegOpenKeyEx(HKEY_LOCAL_MACHINE, REGKEY_INSTALLED_APPS, 0, KEY_READ, &hBaseKey))
	{
		if(ERROR_SUCCESS == RegQueryValueEx(hBaseKey, REGKEY_VALUE_INSTALLED_APPS_NAV, 0, &dwValueType, (LPBYTE)szNAVDir, &dwNAVDirLen))
		{
			if(dwValueType == REG_SZ && dwNAVDirLen)
			{
				// szNAVDir contains the NAV directory
			}
			else
			{
				// Something went wrong; abort after we close the key
				bAbort = TRUE;
			}
		}
	}

	RegCloseKey(hBaseKey);
	if(bAbort)
		return; // Will cause nothing to happen

	SetCurrentDirectory(szNAVDir);

	TCHAR szDllPath[MAX_PATH];
	wsprintf(szDllPath, "%s\\%s", szNAVDir, _T(STR_PLGABOUTLIB));

	HINSTANCE hLibrary = LoadLibrary(szDllPath);
	if(NULL == hLibrary)
		return;

	// Can use szProduct to display a custom app name if we want to, but for 
	// right now we'll just let the About box default to "Norton AntiVirus"...
	//TCHAR szProduct[MAX_PATH];
	//LoadString(_Module.GetModuleInstance(), IDS_CAPTIONTEXT, szProduct, MAX_PATH);

	PLUGINABOUTSTRUCT About;
	ZeroMemory(&About, sizeof(About));
	About.dwStructSize = sizeof(About);
	About.hInst = AfxGetInstanceHandle();
	//About.lpszAppName = szProduct;
	About.hWndOwner = AfxGetMainWnd()->GetSafeHwnd();

	typedef void (WINAPI * PFNDOABOUT)(PLUGINABOUTSTRUCT*);
	PFNDOABOUT pDoAbout = (PFNDOABOUT) GetProcAddress(hLibrary, "DoStdAbout");

	if(pDoAbout)
		pDoAbout(&About);

	FreeLibrary(hLibrary);
}


/////////////////////////////////////////////////////////////////////////////
// CQconsoleApp commands

BOOL CALLBACK EnumWindowProc(HWND hwnd, // handle to parent window 
							 LPARAM lParam) // application-defined value 
{
	// Get title text to compare
	CBrandingRes BrandRes;
	CString sTitleText = BrandRes.GetString(IDS_QUARANTINE_APP_NAME);
	
	// Get window text.
	TCHAR szWindowText[MAX_PATH];
	::GetWindowText(hwnd, szWindowText, MAX_PATH);

	// Shorten text for comparision
	szWindowText[sTitleText.GetLength()] = _T('\0');

	// Check for match
	if(sTitleText == szWindowText)
	{
		// Save off this window handle.
		HWND *pHwnd = (HWND*)lParam;
		*pHwnd = hwnd;
		return FALSE;
	}

	// Continue enumeration.
	return TRUE;
}


////////////////////////////////////////////////////////////////////////////
// Description	    : Called when app shuts down
// 4/28/98 DBUCHES - Function created / Header added.
////////////////////////////////////////////////////////////////////////////
int CQconsoleApp::ExitInstance() 
{
	// Remove single instance mutex.
	if(m_hMutex != NULL)
	{
		CloseHandle(m_hMutex);
		m_hMutex = NULL;
	}

	return CWinApp::ExitInstance();
}


bool CQconsoleApp::IsCfgWizFinished()
{
	bool bFinished = false;	
	CString csCfgWizDat;

	if(SuiteOwnerHelper::GetOwnerCfgWizDat(csCfgWizDat))
	{
		CNAVOptFileEx CfgWizOpts;
		if(CfgWizOpts.Init(csCfgWizDat, FALSE))
		{
			DWORD dwValue = 0;
			CfgWizOpts.GetValue(InstallToolBox::CFGWIZ_Finished, dwValue, 0);
			if(dwValue == 1)
			{
				bFinished = true;
			}
		}
	}

	CCTRACEI("%s - return %s", __FUNCTION__, (bFinished ? "TRUE" : "FALSE"));
	return bFinished;
}


BOOL CQconsoleApp::LaunchCfgWiz()
{	
	BOOL bCreate = FALSE;

	CString csApp, csParam;
	if(SuiteOwnerHelper::GetOwnerCfgWizCmdLine(csApp, csParam))
	{
		CString csCmdLine;
		csCmdLine.Format(_T("\"%s\" %s"), csApp, csParam);

		// Launch ConfigWiz
		STARTUPINFO si = {0};
		si.cb = sizeof(STARTUPINFO);
		PROCESS_INFORMATION pi = {0};

		bCreate = CreateProcess(NULL, csCmdLine.GetBuffer(), NULL, NULL, FALSE, DETACHED_PROCESS, NULL, NULL, &si, &pi);
		DWORD dwError = GetLastError();
		if(bCreate)
		{
			CCTRACEI("%s - CreateProcess(%s)", __FUNCTION__, csCmdLine);
			CloseHandle(pi.hThread);
			CloseHandle(pi.hProcess);
		}
		else
		{
			CCTRACEI("%s - Failed CreateProcess(%s), GetLastError=%d", __FUNCTION__, csCmdLine, dwError);
		}
	}

	return bCreate;
}
