// Copyright 1996-1997 Symantec, Peter Norton Product Group
//****************************************************************************
//
// $Header:   S:/NAVLU/VCS/NavLuApp.cpv   1.13   10 Nov 1998 08:27:42   JBRENNA  $
//
// Description:
//   Provides the main application instance.
//
// Contains:
//
// See Also:
//****************************************************************************
// $Log:   S:/NAVLU/VCS/NavLuApp.cpv  $
// 
//    Rev 1.13   10 Nov 1998 08:27:42   JBRENNA
// Port PVENKAT from QAKN branch to trunk:
//   Added CNavLuApp::EnablePrivilege to enable 'Shutdown' privilege on NT.  This
//   will enable the application to call ExitWindowsEx successfully otherwise it
//   will fail.  On 95, Enableprivilege will do nothing.
// 
//    Rev 1.12   28 Aug 1998 16:20:20   JBRENNA
// Port changes from QAKN branch to trunk:
// 1. Rev 1.11.1.3   10 Jul 1998 17:25:50   TCASHIN
//    Handle the /SAGESET command line switch. This is used by the Win 98 scheduler
//    to tell us the user want to tweak options. We start the LU control panel
//    applet and exit.
// 2. Rev 1.11.1.2   01 Jul 1998 19:20:44   tcashin
//    Return an exit code of 1 if a reboot is needed.
// 3. Rev 1.11.1.1   29 Jun 1998 14:29:46   tcashin
//    Process the /setup switch. Don't display reboot dialogs or text.
// 
//    Rev 1.11   27 Apr 1998 14:35:14   RStanev
// Added def annuity.
//
//    Rev 1.10   06 Aug 1997 10:29:56   JBRENNA
// Add support for a /silent command line switch.
//
//    Rev 1.9   26 Jul 1997 10:51:14   JBRENNA
// Only allow 1 instance of NavLu to run at a given time. We display an error
// message if another instance attempts to run.
//
//    Rev 1.8   01 Jul 1997 17:05:26   JBRENNA
// Use the CBogusMainWnd class to pretend that we have a window. This causes
// an item to appear in the Win95/WinNT4 task bar.
//
//    Rev 1.7   18 Jun 1997 14:23:26   JBRENNA
// If we are running in /scheduled mode and LiveUpdate said a reboot is
// necessary, we not prompt the user. Asking the user if they want to reboot
// now.
//
//    Rev 1.6   03 Jun 1997 13:29:56   JBRENNA
// 1. Add /PretendHostBusy switch. It is only processed when compiled with
//    _DEBUG.
// 2. Moved the IDS_NAVLU_OPTIONS_KEY string to hard coded in string.h.
// 3. Moved m_lpszRetryCountValue to string.h.
//
//    Rev 1.5   02 Jun 1997 18:17:46   JBRENNA
// Added function comment for ForcedMsgBox().
//
//    Rev 1.4   01 Jun 1997 19:00:42   JBRENNA
// 1. Implement the /Prompt dialog that appears when /Scheduled and /Prompt
//    are specified.
// 2. Implement special processing on OAM_HOSTBUSY results when /Scheduled.
//    Specifically, we will retry 4 more times. If after 4 times we are still
//    getting a OAM_HOSTBUSY, we pop a MessageBox telling the user about the
//    busy and asking if they want to retry. The user can answer Yes or No.
//
//    Rev 1.3   26 May 1997 14:58:02   JBRENNA
// Add CNavLuApp::DisplayFailMsgBox() such that it is displayed when LiveUpdate
// is not successful and we are in /scheduled mode.
//
//    Rev 1.2   23 May 1997 21:33:58   JBRENNA
// 1. Add command line processing. (the /scheduled switch).
// 2. Add the ScheduleDlg call. Is called when the LiveUpdate session was
//    successful.
//
//    Rev 1.1   16 May 1997 11:09:12   JBRENNA
// Use MFC for the application.
//
//    Rev 1.0   10 May 1997 01:21:08   JBRENNA
// Initial revision.
//
//****************************************************************************

#include "StdAfx.h"
#include "platform.h"
#include "NavLuApp.h"
#include "ScheduleDlg.h"
#include "NavLu.h"
#include "onramp.h"
#include "MyUtils.h"
#include "Strings.h"
#include "BogusMainWnd.h"
#include "DefAnnuity.h"
#include "NavLuUtils.h"
#include "resource.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//***************************************************************************
// Defines ******************************************************************
//***************************************************************************

                                        // These values were taken from
                                        // winuser.h.  Unfortunately, you can
                                        // only have one of the values defined
                                        // at one time, so I have appended the
                                        // version to the end

#define MB_SERVICE_NOTIFICATION_40      0x00200000L
#define MB_SERVICE_NOTIFICATION_35      0x00040000L


#define NAVLUAPP_EXIT_OK                0
#define NAVLUAPP_EXIT_REBOOT            1


//***************************************************************************
// Module Data              *************************************************
//***************************************************************************

TCHAR   szLiveUpdateSettingsFile[SYM_MAX_PATH] = "s32lucp1.cpl";
TCHAR   szControlPanelApplet[SYM_MAX_PATH]     = "CONTROL.EXE";


BOOL AddNavNTEventLog(int iMessage);

//***************************************************************************
// CNavLuApp implementation *************************************************
//***************************************************************************

/////////////////////////////////////////////////////////////////////////////
// CNavLuApp

BEGIN_MESSAGE_MAP(CNavLuApp, CWinApp)
	//{{AFX_MSG_MAP(CNavLuApp)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG
	ON_COMMAND(ID_HELP, CWinApp::OnHelp)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CNavLuApp construction

CNavLuApp::CNavLuApp()
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}

/////////////////////////////////////////////////////////////////////////////
// The one and only CNavLuApp object

CNavLuApp theApp;

/////////////////////////////////////////////////////////////////////////////
// CNavLuApp initialization

BOOL CNavLuApp::InitInstance()
{
    CNavLu        NavLu (AfxGetInstanceHandle());
    CBogusMainWnd BogusMainWnd;
    WORD          wLuResult;
    BOOL          bLuRetry;
    CString       strAppCaption;
    HANDLE        hAlreadyRunningEvent;
    BOOL          bDefAnnuityQuiet;
    BYTE          abyDABuffer[64];

#ifdef _AFXDLL
	Enable3dControls();			// Call this when using MFC in a shared DLL
#else
	Enable3dControlsStatic();	// Call this when linking to MFC statically
#endif

    // Translate the command line arguments into variables.
    ParseCommandLine (m_CmdLineInfo);

    // If we're started from the Scheduler's Settings button, then 
    // do all the appropriate checking and bail...
    if (m_CmdLineInfo.m_bSageSet)
    {
        ProcessSchedulerSettings();

        goto BailOut;
    } 

    // Use an Named Event to determine if we already have a instance running.
    // This works because named events are system wide. Display an error
    // message when the Named Event already exists.
    hAlreadyRunningEvent = CreateEvent (NULL, FALSE, FALSE, g_szAppEventName);

    if (ERROR_ALREADY_EXISTS == GetLastError())
    {
		// no msgbox if /Silent
		if (!m_CmdLineInfo.m_bSilent)
			ForcedMsgBox (IDS_CAPTION, IDS_ERR_ALREADY_RUNNING,
                      MB_OK | MB_ICONINFORMATION);
                else AddNavNTEventLog(IDS_NTLOGERROR_ALREADYRUNNING);
        goto BailOut;
    }

    // Get the bogus window's caption string.
    strAppCaption.LoadString (IDS_MAIN_WND_TITLE);

    // If the user wanted to be prompted, prompt the user before
    // running the LiveUpdate session. Note that prompt does not
    // occur when we are retrying because of a busy signal.
    if (m_CmdLineInfo.m_bPrompt && !m_CmdLineInfo.m_bInternalRetry)
    {
		// no msgbox if /Silent
		if (!m_CmdLineInfo.m_bSilent)
	        if (IDNO == ForcedMsgBox (IDS_SCHEDLU_TITLE,
	                              IDS_SCHEDLU_PROMPT,
                                  MB_YESNO | MB_ICONQUESTION))
		    {
			    goto BailOut;
			}
    }

    // Remove the internal Retry Count from the registry when we are not
    // using the count.
    if (!m_CmdLineInfo.m_bInternalRetry)
        RemoveRetryCount ();

    // Calculate the bDefAnnuityQuiet flag used by the def annuity APIs.
    bDefAnnuityQuiet = m_CmdLineInfo.m_bSilent || m_CmdLineInfo.m_bScheduled;

    // Set bLuRetry to FALSE upon entering the loop, so that we can validate
    // the definition subscriptions properly.
    bLuRetry = FALSE;

    do {
        // Create a bogus main window if we are running interactively. This
        // ensures we have a icon in the Win95/WinNT4 task bar.
        if (!m_CmdLineInfo.m_bSilent)
            BogusMainWnd.MyCreate (strAppCaption, IDR_MAINFRAME);

        // Call the def annuity module to get the status of the virus definition
        // subscriptions. Make sure that we do this only the first time the loop
        // is executed (we could move this code outside the loop, but
        // unfortunately BogusMainWnd is only available here).
        if (!bLuRetry)
            NavLu.m_DefAnnuityStatus = DefAnnuityPreUpdateCheck (NULL, bDefAnnuityQuiet, abyDABuffer);

        // Call the core LiveUpdate functionality to get definitions, patches, etc.,
        // based on the status of the subscriptions.  If the def annuity encountered
        // any errors, and was not able to run properly, we will act as if the user
        // is totally legal.
        bLuRetry = TRUE;

        if (NavLu.m_DefAnnuityStatus==DefAnnuityUpdatePatches)
            wLuResult = NavLu.DoLiveUpdate (BogusMainWnd, m_CmdLineInfo.m_bSilent, TRUE, m_CmdLineInfo.m_bSetup);
        else if (NavLu.m_DefAnnuityStatus!=DefAnnuityCancelUpdate)
            wLuResult = NavLu.DoLiveUpdate (BogusMainWnd, m_CmdLineInfo.m_bSilent, FALSE, m_CmdLineInfo.m_bSetup);
        else
            bLuRetry = FALSE;

        // If LiveUpdate downloaded something, then let the def annuity module display
        // any reports that it needs to display.  Note, that currently we will force
        // the display even if a reboot needs to be done.
        if (bLuRetry && (wLuResult==OAM_SUCCESS || wLuResult==OAM_SUCCESSREBOOT || wLuResult==OAM_NOTHINGNEW))
            DefAnnuityPostUpdateCheck (NULL, bDefAnnuityQuiet, abyDABuffer);

        // Destroy the bogus main window.
        if (!m_CmdLineInfo.m_bSilent)
            BogusMainWnd.DestroyWindow ();

        // Process the result code from LiveUpdate and determine if
        // we need to retry the LiveUpdate session.
        if (bLuRetry)
            bLuRetry = ProcessLuResult (wLuResult);
    } while (bLuRetry);

BailOut:
    CloseHandle (hAlreadyRunningEvent);

    // Since the dialog has been closed, return FALSE so that we exit the
    //  application, rather than start the application's message pump.
    return FALSE;
}

//*************************************************************************
//* Name:           CNavLuApp::EnablePrivilege
//*
//* Description:    To enable given privilege for the current process
//*                 Only on NT.  9x, simply returns SUCCESS.
//*
//* Parameters:
//*     LPTSTR      lpszPrivilege       UNICODE string - privilege name
//*
//* Returns:
//*     BOOL        TRUE on success and FALSE on failure.
//*************************************************************************
//* 09/22/98        PVENKAT         Function Created
//*************************************************************************
BOOL
CNavLuApp::EnablePrivilege ( LPTSTR lpszPrivilege )
{
    OSVERSIONINFO       osInfo ;        // to determine OS
    BOOL                bRet = FALSE ;  // Assume failed.
    HANDLE              hToken ;        // Process handle
    LUID                luid ;
    TOKEN_PRIVILEGES    tokenPrivileges ;
    
    // Get OS.  We need to enable privilege only on NT.
    ZeroMemory ( &osInfo, sizeof ( osInfo ) ) ;
    osInfo.dwOSVersionInfoSize = sizeof ( osInfo ) ;
    GetVersionEx ( &osInfo ) ;

    // Enable privilege on NT.
    if ( osInfo.dwPlatformId == VER_PLATFORM_WIN32_NT )
    {
        // Get the current process token.
        bRet = OpenProcessToken ( GetCurrentProcess (),
                                    TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY,
                                    &hToken ) ;
        if ( bRet )
        {
            // get LUID for the given privilege
            bRet = LookupPrivilegeValue ( 0, lpszPrivilege, &luid ) ;
            if ( bRet )
            {
                tokenPrivileges.PrivilegeCount = 1 ;
                tokenPrivileges.Privileges[0].Luid = luid ;
                tokenPrivileges.Privileges[0].Attributes = 
                                            SE_PRIVILEGE_ENABLED ;

                // Enable the given privilege on the process token.
                bRet = AdjustTokenPrivileges ( hToken, 
                                                FALSE, 
                                                &tokenPrivileges, 
                                                0, 
                                                0, 
                                                0 );
            }

            // Done with the process token.
            CloseHandle ( hToken ) ;
        }
    }
    else    // On 9x nothing to enable, simply return success.
        bRet = TRUE ;

    return bRet ;
}

//////////////////////////////////////////////////////////////////////////
// Description:
//   Process the return value from the LiveUpdate session. This may involve
//   displaying message boxes to note failures.
//
// Returns
//   TRUE   - When we should attempt another LiveUpdate session. Sometimes
//            used on OAM_HOSTBUSY LiveUpdate results.
//   FALSE  - Do not retry the LiveUpdate session.
//////////////////////////////////////////////////////////////////////////
BOOL CNavLuApp::ProcessLuResult (WORD wLuResult)
{
    BOOL         bLuRetry = FALSE;
    BOOL         bReboot  = FALSE;
    CScheduleDlg SchedDlg;
    BOOL         bSuccess;

    // This is for debugging purposes only. It should never be used in the
    // released product.
#ifdef _DEBUG
    if (m_CmdLineInfo.m_bPretendHostBusy)
        wLuResult = OAM_HOSTBUSY;
    else if (m_CmdLineInfo.m_bPretendSuccessReboot)
        wLuResult = OAM_SUCCESSREBOOT;
#endif

    // Set the default exit code.
    m_ExitCode = NAVLUAPP_EXIT_OK;

    // Translate the LiveUpdate result code into a Success/Fail meaning.
    switch (wLuResult)
    {
        case OAM_SUCCESS:
        case OAM_NOTHINGNEW:
            bSuccess = TRUE;
            break;

        case OAM_SUCCESSREBOOT:
            bSuccess = TRUE;

            m_ExitCode = NAVLUAPP_EXIT_REBOOT;

            if (m_CmdLineInfo.m_bSetup)
            {
                break;
            }

            if (m_CmdLineInfo.m_bScheduled)
            {
				int nBtn;
				// no msgbox if /Silent
				if (!m_CmdLineInfo.m_bSilent)
					nBtn = ForcedMsgBox (IDS_SCHEDLU_TITLE,
                                         IDS_SCHEDLU_REBOOT,
                                         MB_YESNO | MB_ICONQUESTION);
				else
					nBtn = IDYES;	// go ahead and reboot if Silent

                if (nBtn == IDYES)
                {
                    // Enable privilege on NT for the current process
                    // to shutdown.  EnablePrivilege only acts on NT.
                    // On 9x, simply returns success.
                    EnablePrivilege ( SE_SHUTDOWN_NAME ) ;

                    // NOTE:
                    // Ignore the return value from EnablePrivilege.
                    // This can fail only on NT, if the current user has
                    // does not enough rights to update the process token.
                    // In this case, ExitWindowsEx will any way fail.

                    // Shutdown now.
                    BOOL bAllExited = ExitWindowsEx (EWX_REBOOT, 0);
                    
                    // Show an error if one of the process failed to exit.
                    if (!bAllExited)
						// no msgbox if /Silent
						if (!m_CmdLineInfo.m_bSilent)
					        ForcedMsgBox (IDS_SCHEDLU_TITLE,
                                      IDS_SCHEDLU_REBOOT_FAILED,
                                      MB_OK | MB_ICONSTOP);
                    else AddNavNTEventLog(IDS_NTLOGERROR_REBOOTERR);
                }
            }

            break;

        case OAM_HOSTBUSY:
            bSuccess = TRUE;

            // Busy signals are processed as special cases.
            if (m_CmdLineInfo.m_bScheduled)
            {
                DWORD dwRetryCnt = GetRetryCount ();

                // We try 4 times before giving up.
                if (dwRetryCnt < 4)
                {
                    CMyUtils MyUtils;
                    MyUtils.AddNpsSchedLuRetryEvent ();

                    SetRetryCount (dwRetryCnt+1);
                }
                else
                {
                    // Notify the user of the Busy Signal and ask them
                    // if they want to run the LiveUpdate now.
                    int nBtn;
					// no msgbox if /Silent
					if (!m_CmdLineInfo.m_bSilent)
						nBtn = ForcedMsgBox (IDS_SCHEDLU_TITLE,
                                             IDS_SCHEDLU_BUSY,
                                             MB_YESNO | MB_ICONQUESTION);
					else
                                        {
                                                AddNavNTEventLog(IDS_NTLOGERROR_COMMTIMEOUT);
						nBtn = IDNO;	// no retry if Silent
                                        }

                    bLuRetry = (nBtn == IDYES);
                }
            }

            break;

        default:
            bSuccess = FALSE;
            break;
    }

    if (bSuccess)
    {
        // If we are running in Silent mode or Scheduled mode, we do not
        // display the Schedule Dialog.

        BOOL bNotDisplayDlg = m_CmdLineInfo.m_bSilent || m_CmdLineInfo.m_bScheduled;

        if (!bNotDisplayDlg && SchedDlg.ShouldDisplay())
        {
            SchedDlg.DoModal ();
        }
    }
    else
    {
        if (m_CmdLineInfo.m_bScheduled)
        {
			// no msgbox if /Silent
			if (!m_CmdLineInfo.m_bSilent)
			    ForcedMsgBox (IDS_SCHEDLU_FAILED_TITLE,
                          IDS_SCHEDLU_FAILED,
                          MB_OK | MB_ICONSTOP);
                        else AddNavNTEventLog(IDS_NTLOGERROR_LUFAILURE);
        }
    }

    return bLuRetry;
}

//////////////////////////////////////////////////////////////////////////
// Description:
//   Display a message box with the specified title and message. This
//   MessageBox will appear on Win95, WinNT (no user logged in), and
//   WinNT (user logged in).
//
// Parameters:
//   uTitleId      [in] Resource ID for the caption.
//   uMsgId        [in] Resource ID for the message text.
//   uMsgBoxFlags  [in] Flags used to pass to MessageBox.
//
// Returns
//   The result from the MessageBox call.
//////////////////////////////////////////////////////////////////////////
int CNavLuApp::ForcedMsgBox (UINT uTitleId, UINT uMsgId, UINT uMsgBoxFlags) const
{
    CString        strTitle;
    CString        strMsg;
    OSVERSIONINFO  rOSVersion;

    // Figure out what version of NT we are running. This ensures that we use
    // the right flags on the message box. Remember that WinNT 3.51 and
    // WinNT 4.0 have different settings for the MB_SERVICE_NOTIFICATION flag.

    rOSVersion.dwOSVersionInfoSize = sizeof (OSVERSIONINFO);

    if (GetVersionEx (&rOSVersion))
    {
        if (rOSVersion.dwPlatformId == VER_PLATFORM_WIN32_NT)
        {
            if (rOSVersion.dwMajorVersion >= 4)
            {
                uMsgBoxFlags |= MB_SERVICE_NOTIFICATION_40;
            }
            else
            {
                // Well, we are on an WinNT system, but it is not 4.0 or
                // greater. That means we are on 3.51. Use the 3.51 flag.
                uMsgBoxFlags |= MB_SERVICE_NOTIFICATION_35;
            }
        }
    }

    // Load the error message.
    strTitle.LoadString (uTitleId);
    strMsg.LoadString   (uMsgId);

    return ::MessageBox (NULL, strMsg, strTitle, uMsgBoxFlags);
}

DWORD CNavLuApp::GetRetryCount () const
{
    auto  DWORD      dwCount       = 0;
    auto  DWORD      dwValueBufLen;
    auto  HKEY       hBaseKey;
    auto  DWORD      dwValueType;

    //  Get the value from the key.
    if (ERROR_SUCCESS == RegOpenKeyEx (HKEY_LOCAL_MACHINE, g_szOptKey, 0, KEY_READ|KEY_WRITE, &hBaseKey))
    {
        dwValueBufLen = sizeof (dwCount);
        if (ERROR_SUCCESS == RegQueryValueEx (hBaseKey, g_szOptRetryCountValue, 0, &dwValueType, (LPBYTE)&dwCount, &dwValueBufLen))
        {
            if (dwValueType != REG_DWORD || !dwValueBufLen)
                dwCount = 0;
        }
    }

    RegCloseKey(hBaseKey);

    return dwCount;
}

void CNavLuApp::SetRetryCount (DWORD dwCount) const
{
    auto  HKEY       hBaseKey;

    //  Get the value from the key.
    if (ERROR_SUCCESS == RegOpenKeyEx (HKEY_LOCAL_MACHINE, g_szOptKey, 0, KEY_READ|KEY_WRITE, &hBaseKey))
    {
        RegSetValueEx (hBaseKey, g_szOptRetryCountValue, 0, REG_DWORD, (LPBYTE)&dwCount, sizeof (dwCount));
    }

    RegCloseKey(hBaseKey);
}

void CNavLuApp::RemoveRetryCount () const
{
    auto  HKEY       hBaseKey;

    //  Get the value from the key.
    if (ERROR_SUCCESS == RegOpenKeyEx (HKEY_LOCAL_MACHINE, g_szOptKey, 0, KEY_READ|KEY_WRITE, &hBaseKey))
    {
        RegDeleteValue (hBaseKey, g_szOptRetryCountValue);
    }

    RegCloseKey(hBaseKey);
}


int CNavLuApp::ExitInstance() 
{	
	return (m_ExitCode);
}


void CNavLuApp::ProcessSchedulerSettings()
{    
    auto    TCHAR    szPath[SYM_MAX_PATH]   = {0};
    auto    TCHAR    szLUSettings[SYM_MAX_PATH]   = {0};

    if (GetSystemDirectory(szPath, SYM_MAX_PATH))
    {
        _tcscat(szLUSettings, szPath);
        _tcscat(szLUSettings, "\\");
        _tcscat(szLUSettings, szLiveUpdateSettingsFile);

        if (NavLuFileExists(szLUSettings))
        {
            // Start the LiveUpdate control panel applet
            ::ShellExecute( NULL,
                            _T("open"),
                            szControlPanelApplet,
                            szLUSettings,
                            NULL,
                            SW_SHOWNORMAL );
        }
        else
        {
            // Put up a dialog that says "Sorry, can't change settings"
            CDialog cNoSettings(IDD_NOLUSETTINGS);

            cNoSettings.DoModal();
        }
    }
    
    return;
}

BOOL AddNavNTEventLog(int iMessage)
{
	HKEY hklm;
	WORD wType = EVENTLOG_ERROR_TYPE;	// type = error
	WORD wCategory = 2;		// information (vs. virus found - only 2 supported by s32alogn);
	DWORD dwEventID = 0x1002;	// generic error (one parm);
	WORD wNumStrings = 1;
	char szErrorString[_MAX_PATH];
	LPCTSTR lpStrings[2];

	lpStrings[0] = (char *)&szErrorString;

	// make sure the platforms supports event logging
	OSVERSIONINFO osv;
	osv.dwOSVersionInfoSize = sizeof(osv);
	if (GetVersionEx(&osv) == FALSE)
		return FALSE;
	if (osv.dwPlatformId != VER_PLATFORM_WIN32_NT)
		return FALSE;

	// Get the current process access token.
	HANDLE hAccessToken;
	if (OpenProcessToken(GetCurrentProcess(), TOKEN_QUERY, &hAccessToken) == FALSE)
		return FALSE;

	// Get the TOKEN_USER structure from the access token, 
	// which contains the Sid representing the caller,
    // and close the access token.
	PTOKEN_USER ptgUser;
	#define SID_BUF_SIZE 1024
	BYTE UserBuf[SID_BUF_SIZE];
	DWORD cbUserBufSize = SID_BUF_SIZE;
	ptgUser = (PTOKEN_USER) UserBuf;
	BOOL bSuccess = GetTokenInformation(hAccessToken, TokenUser, ptgUser, cbUserBufSize, &cbUserBufSize);
	CloseHandle(hAccessToken);
	if (!bSuccess)
		return FALSE;

	// open registry
	LONG lRegError = RegConnectRegistry(NULL, HKEY_LOCAL_MACHINE, &hklm);
	if (lRegError != ERROR_SUCCESS)
		return FALSE;

	// Get a handle to the event log. (use symalog)
	HANDLE hLog = RegisterEventSource(NULL, "Norton AntiVirus");
	if (hLog == NULL)
		return FALSE;

	// load error string
	LoadString(NULL, iMessage, szErrorString, sizeof(szErrorString));
    
	// Now report the event, which will add this event to the event log.
	bSuccess = ReportEvent(hLog,		// event-log handle
		wType,							// event type
		wCategory,						// category
		dwEventID,						// event ID
		ptgUser->User.Sid,				// Sid representing caller
		wNumStrings,					// number of substitution strings
		0,								// no binary data
		lpStrings,						// string array
		NULL);							// address of data

	if (!bSuccess)
		DWORD dwErr = GetLastError();

    // Close handle to the event log.
	DeregisterEventSource(hLog);

	return TRUE;
}

