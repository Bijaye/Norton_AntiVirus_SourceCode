//----------------------------------------------------------------------------------
//------ REBOOT.EXE ----------------------------------------------------------------
//----------------------------------------------------------------------------------


//------ Headers -------------------------------------------------------------------
//extern "C" {
#include <windows.h>
#include <winbase.h>
#include <string.h>
#include <stdio.h>
#include <windowsx.h>
#include <tchar.h>
//#include <math.h>
//}
#include "resource.h"
#include "reboot.h"
#include "navwcomm.h"
#include "navapcom.h"


//------ Defines -------------------------------------------------------------------
#define DEFAULT_REBOOT_WAIT     30      //  30 seconds
#define MIN_REBOOT_WAIT          0      //   0 seconds (Minimum custom wait time)
#define MAX_REBOOT_WAIT        600      // 600 seconds (Maximum custom wait time)
#define REBOOT_TIMER_ID         1


//------ Command-line Parameters ---------------------------------------------------
// NOTE: If you add any parameters that are more that one character long, check
// the code in ProcessCommandLine() to make sure it can process the argument.
#define CMD_LINE_SWITCH_SILENT  'S'     // Command-line switch for Silent Mode (i.e., no UI)
#define CMD_LINE_SWITCH_WAIT    'W'     // Command-line switch for a custom wait time
#define CMD_LINE_SWITCH_FORCE   'F'     // Command-line switch to use a forced, not nice, reboot

#define MAX_CMD_LINE_PATH   256


//------ Structures ----------------------------------------------------------------
typedef struct
    {
    LPHBITMAP   lphBmpAnimation;
    LPHBITMAP   lphBmpAnimationMask;
    WORD        wTotalFrames;
    WORD        wCurrentFrame;
    UINT        uAnimationTimerID;
    } ANIMATION;

typedef struct tagOPTIONS
{
	TCHAR   szNNSPath[MAX_PATH];
	int     nImpersonate;
	TCHAR   szImpersonationUserName[MAX_PATH];
	TCHAR   szImpersonationDomain[MAX_PATH];
	TCHAR   szImpersonationPassword[MAX_PATH];
	TCHAR   szSymantecSetupPath[MAX_PATH];
	TCHAR   szSymantecSetupParameters[MAX_PATH];
} OPTIONS, * LPOPTIONS;



//------ Prototypes ----------------------------------------------------------------
int WINAPI WinMain (                    // Main entry point
    HINSTANCE  hInstance,	            // Handle of current instance
    HINSTANCE  hPrevInstance,	        // Handle of previous instance
    LPSTR  lpszCmdLine,	                // Pointer to command line
    int  nCmdShow );	                // Show state of window
BOOL Reboot95 ( void );
BOOL RebootNT ( void );
void SetPrivilege( LPCTSTR lpPrivilege );
void ProcessCommandLine ( LPCSTR lpszCmdLine );
UINT WINAPI RebootWndProc (
    HWND    hDlg,
    UINT    message,
    WPARAM  wParam,
    LPARAM  lParam );
BOOL RegisterRebootClass (           // Register W95 windows class
    void );
HWND RebootWndCreate (           // Creates main dialog wnd
    VOID
    );
BOOL RebootSystem ( void );
BOOL CenterDialog (HWND hWnd);
VOID WINAPI DrawDialogBitmap (          // Draw panel's left-side bitmap
    HWND             hDlg,              // [in] Handle to dialog box
    LPDRAWITEMSTRUCT lpDrawItem);       // [in] What needs to be done
VOID WINAPI AP_AllowShutdown ( void );
VOID PASCAL ClockAnimationStart (      // Starts Animation
    HWND        hDlg
    ) ;
VOID CALLBACK ClockAnimationNextFrame (
    HWND    hWnd,
    UINT    uMessage,
    UINT    uEventID,
    DWORD   dwTime
    );
VOID PASCAL ClockAnimationEnd (
    HWND        hDlg
    );
VOID PASCAL ClockAnimationDrawFrame (
    LPDRAWITEMSTRUCT    lpDrawItem
    );
BOOL WINAPI DrawMaskedBitmap (HDC hDC, HBITMAP hBitmap, HBITMAP hMask, int x, int y);
VOID SetWaitTimeText (HWND hWnd);
BOOL	IsRunningAsAdministrator (VOID);


extern HANDLE WINAPI Spawn (LPOPTIONS lpOptions);


//------ Global Variables ----------------------------------------------------------
                                        // By default, we throw up a UI that tells the
                                        // user we're about to reboot. If the command
                                        // line parameter /S was passed, set this to
                                        // TRUE, and don't display a UI.
BOOL        bUseSilentMode = FALSE;
                                        // Time to wait before reboot. This is compiled
                                        // at the default of DEFAULT_REBOOT_WAIT, but
                                        // can be customized with the command-line switch
                                        // of /W<WAIT>.
DWORD       dwRebootWaitTime = DEFAULT_REBOOT_WAIT;
                                        // By default, we use a nice reboot. If the
                                        // command-line parameter of /F was passed,
                                        // set this to TRUE, and use a forced reboot.
BOOL        bUseForcedReboot = FALSE;
                                        // Global application instance:
HINSTANCE   hAppInstance = NULL;
UINT        uTimerID = NULL;
HWND        hWndMain;                   // Main window / dialog box handle
DLGPROC     lpfn;
OSVERSIONINFO rOSVersionInfo;
ANIMATION   ClockAnimation = {0};
TCHAR       gszCmdLine [ MAX_CMD_LINE_PATH + 1 ];



//----------------------------------------------------------------------------------
//------ winmain()
//----------------------------------------------------------------------------------
int WINAPI WinMain (                    // Main entry point
    HINSTANCE   hInstance,	            // Handle of current instance
    HINSTANCE   hPrevInstance,	        // Handle of previous instance
    LPSTR       lpszCmdLine,	        // Pointer to command line
    int         nCmdShow )	            // Show state of window
  {
    MSG         msg;                    // Message structure
    int         nRet = 0;               // Return code

    hAppInstance = hInstance;

                                        // Cut up command line parameters, and set the
                                        // global variables.
    ProcessCommandLine ( lpszCmdLine );


        if ( RegisterRebootClass() &&
            (hWndMain = RebootWndCreate()) != 0 )
            {

            while ( GetMessage ( &msg, NULL, 0, 0) )
                {
                    if ( !IsDialogMessage( hWndMain, &msg ) )
                        {
                        TranslateMessage ( &msg );
                        DispatchMessage  ( &msg );
                        }
                }


            nRet = msg.wParam;
            }

ExitProcess ( 0 );                      // Could also use _exit()

return nRet;

}


BOOL RebootSystem ( void )
    {
    BOOL bRebootSuccessfullyLaunched = FALSE;

    if (rOSVersionInfo.dwPlatformId & VER_PLATFORM_WIN32_NT)
   	    { if (RebootNT()) { bRebootSuccessfullyLaunched = TRUE; } }
   	else if (rOSVersionInfo.dwPlatformId & VER_PLATFORM_WIN32_WINDOWS)
        { if (Reboot95()) { bRebootSuccessfullyLaunched = TRUE; } }

	return bRebootSuccessfullyLaunched;
}


//----------------------------------------------------------------------------------
//------ RebootNT()
//----------------------------------------------------------------------------------
BOOL RebootNT ( void )
{
    UINT uRebootFlags = 0;
                                        // Set the type of reboot:
    if ( bUseForcedReboot )
                                        // Force the reboot:
        { uRebootFlags = (EWX_REBOOT | EWX_FORCE) ; }
        else
                                        // Use a nice reboot:
        { uRebootFlags = (EWX_REBOOT ) ; }

	SetPrivilege(SE_SHUTDOWN_NAME);

	if (ExitWindowsEx( ( uRebootFlags ), NULL))
	    { return(TRUE);	}
	    else
	    { return(FALSE); }
return TRUE;
}                                       // End RebootNT()


//----------------------------------------------------------------------------------
//------ Reboot95()
//----------------------------------------------------------------------------------
BOOL Reboot95 ( void )
{
    UINT uRebootFlags = 0;

    if ( bUseForcedReboot )
                                        // Force the reboot:
        { uRebootFlags = (EWX_REBOOT | EWX_FORCE) ; }
        else
                                        // Use a nice reboot:
        { uRebootFlags = (EWX_REBOOT ) ; }

	if (ExitWindowsEx( ( uRebootFlags ), NULL))
	    { return(TRUE); }
	    else
	    { return(FALSE); }
return TRUE;
}                                       // End Reboot95()


//----------------------------------------------------------------------------------
//------ SetPrivilege()
//----------------------------------------------------------------------------------
void SetPrivilege ( LPCTSTR lpPrivilege )
{
	HANDLE				hProcess, hToken;
	TOKEN_PRIVILEGES	rPreviousTP, rCurrentTP;
	DWORD				nPreviousTPSize = sizeof(TOKEN_PRIVILEGES);
	LUID				luid;
	BOOL				fAdmin;

	fAdmin = IsRunningAsAdministrator ();

	if (fAdmin)
	{
		hProcess = GetCurrentProcess();

		if (!OpenProcessToken(hProcess,
								TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY,
								&hToken))
		{
#ifdef  DEBUG
			MessageBox (NULL, TEXT ("Error in OpenProcessToken"), "ERROR", MB_OK | MB_SETFOREGROUND);
#endif
		    return;
		}
	}
	else
	{
		hProcess = GetCurrentThread ();
		if (!OpenThreadToken(hProcess,
							TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY,
							TRUE,
							&hToken))
		{
			DWORD	dwErr = GetLastError ();
#ifdef  DEBUG
			MessageBox (NULL, TEXT ("Error in OpenThreadToken"), "ERROR", MB_OK | MB_SETFOREGROUND);
#endif
			return;
		}
	}

	if (!LookupPrivilegeValue(NULL, lpPrivilege, &luid))
    {
#ifdef  DEBUG
		MessageBox (NULL, TEXT ("Error in LookUpPrivilegeValue"), "ERROR", MB_OK | MB_SETFOREGROUND);
#endif
        return;
    }

	rCurrentTP.PrivilegeCount = 1;
	rCurrentTP.Privileges[0].Luid = luid;
	rCurrentTP.Privileges[0].Attributes = 0;

	if (!AdjustTokenPrivileges(hToken, FALSE, &rCurrentTP, sizeof(TOKEN_PRIVILEGES), &rPreviousTP, &nPreviousTPSize))
    {
#ifdef  DEBUG
		MessageBox (NULL, TEXT ("Error in AdjustTokenPrivleges 1"), "ERROR", MB_OK | MB_SETFOREGROUND);
#endif
        return;
    }

	rPreviousTP.PrivilegeCount = 1;
	rPreviousTP.Privileges[0].Luid = luid;
	rPreviousTP.Privileges[0].Attributes |= SE_PRIVILEGE_ENABLED;
	rPreviousTP.Privileges[0].Attributes = rPreviousTP.Privileges[0].Attributes;

	if (!AdjustTokenPrivileges(hToken, FALSE, &rPreviousTP, nPreviousTPSize, NULL, NULL))
    {
#ifdef  DEBUG
		MessageBox (NULL, TEXT ("Error in AdjustTokenPrivleges 2"), "ERROR", MB_OK | MB_SETFOREGROUND);
#endif
        return;
    }


}                                       // End SetPrivilege ()


//----------------------------------------------------------------------------------
//------ ProcessCommandLine()
//----------------------------------------------------------------------------------
void ProcessCommandLine ( LPCSTR lpszCmdLine )
{
                                        // Make a local copy of the command line so that
                                        // we can thrash it as we wish:
TCHAR       szCmdLine [ MAX_CMD_LINE_PATH + 1 ];
LPTSTR      lpszTok;

//lstrcpyn ( szCmdLine, lpszCmdLine, MAX_CMD_LINE_PATH );
#ifdef	UNICODE
	MultiByteToWideChar (CP_ACP, MB_COMPOSITE, lpszCmdLine, -1, szCmdLine, sizeof (szCmdLine));
#else
	lstrcpyn ( szCmdLine, lpszCmdLine, MAX_CMD_LINE_PATH );
#endif
CharUpper ( szCmdLine );

lstrcpy (gszCmdLine, szCmdLine);

// Token doesnot exist, then empty the NNS Path (gszCmdLine).
if ( lpszTok = _tcstok (gszCmdLine, TEXT (" ")))
        lstrcpy (gszCmdLine, _tcstok (gszCmdLine, TEXT (" ")));
else
        gszCmdLine[0] = 0;


                                        // Find the first argument:
LPCTSTR lpszCurrentArg  = _tcsstr ( szCmdLine, TEXT ("/") );
DWORD   dwTimeoutArg    = DEFAULT_REBOOT_WAIT;

while ( lpszCurrentArg )
    {
                                        // lpszCurrentArg points to a '/'
    lpszCurrentArg = CharNext ( lpszCurrentArg );
                                        // lpszCurrentArg points to a parameter character
    switch ( *lpszCurrentArg )
        {
        case CMD_LINE_SWITCH_SILENT:    // Switch for silent mode.
            bUseSilentMode = TRUE;
            lpszCurrentArg = CharNext ( lpszCurrentArg );// lpszCurrentArg now points to the space AFTER the parameter
            break;

        case CMD_LINE_SWITCH_WAIT:      // Switch for a custom wait time.
            lpszCurrentArg = CharNext ( lpszCurrentArg );// lpszCurrentArg now points to the number of seconds
            if (!lpszCurrentArg) break;
            if (( _tscanf ( lpszCurrentArg, TEXT ("%i"), &dwTimeoutArg ) ) == 1 )
                {
                if ( ( dwTimeoutArg >= MIN_REBOOT_WAIT ) && ( dwTimeoutArg <= MAX_REBOOT_WAIT ) )
                    { dwRebootWaitTime = dwTimeoutArg; }
                }
                                        // From here, let lpszCurrentArg point to the numeric
                                        // argument. The strstr() below will move it on.
            break;

        case CMD_LINE_SWITCH_FORCE:     // Switch for a forced reboot.
            bUseForcedReboot = TRUE;
            lpszCurrentArg = CharNext ( lpszCurrentArg );// lpszCurrentArg now points to the space AFTER the parameter
            break;

        default:
            break;
        }                               // End switch

                                        // Now, try to find the next parameter.
    lpszCurrentArg = _tcsstr (lpszCurrentArg, TEXT ("/"));

    }                                   // End while() loop

}                                       // End ProcessCommandLine ()


void
Decrypt (LPTSTR pszPlain, LPTSTR pszEncrypted, LPTSTR pszKey)
{
    int len = lstrlen (pszEncrypted) * sizeof (TCHAR);
    LPSTR pTemp = (LPSTR) pszPlain;
    LPSTR lpszEncrypted = (LPSTR) pszEncrypted;
    LPSTR lpszKey = (LPSTR) pszKey;
    LPSTR lpszPlain = (LPSTR) pszPlain;

    for (int i = 0; i < len; i++)
    {
        unsigned char c1 = *lpszEncrypted;
        unsigned char c2 = *lpszKey;
        unsigned char c3 = c1 - c2;

        *lpszPlain++ = c3;
        lpszEncrypted++;
        lpszKey++;
    }
    *lpszPlain++ = TEXT ('\0');
    *lpszPlain = TEXT ('\0');
}


// Key
TCHAR szKey[] = TEXT ("NAVWNT221NAVWNT221NAVWNT221NAVWNT221NAVWNT221NAVWNT221NAVWNT221NAVWNT221NAVWNT221NAVWNT221NAVWNT221NAVWNT221NAVWNT221NAVWNT221NAVWNT221NAVWNT221NAVWNT221NAVWNT221NAVWNT221NAVWNT221NAVWNT221NAVWNT221NAVWNT221NAVWNT221NAVWNT221NAVWNT221NAVWNT221NAVWNT221NAVWNT221NAVWNT221");

BOOL GetOptionsFromNNS(LPOPTIONS lpOptions)
{
    int     nEncrypted;
    TCHAR   szBuf[MAX_PATH + 1];

	// get impersonation setting: whether we will or whether we won't

	lpOptions->nImpersonate = GetPrivateProfileInt(TEXT ("Install/Update"),
												   TEXT ("Impersonate"),
												   0,
												   lpOptions->szNNSPath);

    nEncrypted = GetPrivateProfileInt (TEXT ("Install/Update"),
                                        TEXT ("ImpersonationModified"),
                                        0,
                                        lpOptions->szNNSPath);
    if (nEncrypted == 0)
    {
	    // get the username of who we want to impersonate

	    if (GetPrivateProfileString(TEXT ("Install/Update"),
								    TEXT ("ImpersonationUserName"),
								    TEXT (""),
								    lpOptions->szImpersonationUserName,
								    sizeof(lpOptions->szImpersonationUserName),
								    lpOptions->szNNSPath) == 0)
	    {
		    goto bailout;
	    }

	    // get the domain of the user we want to impersonate

	    if (GetPrivateProfileString(TEXT ("Install/Update"),
								    TEXT ("ImpersonationDomain"),
								    TEXT (""),
								    lpOptions->szImpersonationDomain,
								    sizeof(lpOptions->szImpersonationDomain),
								    lpOptions->szNNSPath) == 0)
	    {
		    goto bailout;
	    }

	    // get the password of the user we want to impersonate

	    if (GetPrivateProfileString(TEXT ("Install/Update"),
								    TEXT ("ImpersonationPassword"),
								    TEXT (""),
								    lpOptions->szImpersonationPassword,
								    sizeof(lpOptions->szImpersonationPassword),
								    lpOptions->szNNSPath) == 0)
	    {
		    goto bailout;
	    }
    }
    else
    {
	    // get the username of who we want to impersonate

	    if (GetPrivateProfileString(TEXT ("Install/Update"),
								    TEXT ("ImpersonationUserName"),
								    TEXT (""),
								    szBuf,
								    sizeof(szBuf),
								    lpOptions->szNNSPath) == 0)
	    {
		    goto bailout;
	    }
        Decrypt (lpOptions->szImpersonationUserName, szBuf, szKey);

	    // get the domain of the user we want to impersonate

	    if (GetPrivateProfileString(TEXT ("Install/Update"),
								    TEXT ("ImpersonationDomain"),
								    TEXT (""),
								    szBuf,
								    sizeof(szBuf),
								    lpOptions->szNNSPath) == 0)
	    {
		    goto bailout;
	    }
        Decrypt (lpOptions->szImpersonationDomain, szBuf, szKey);

	    // get the password of the user we want to impersonate

	    if (GetPrivateProfileString(TEXT ("Install/Update"),
								    TEXT ("ImpersonationPassword"),
								    TEXT (""),
								    szBuf,
								    sizeof (szBuf),
								    lpOptions->szNNSPath) == 0)
	    {
		    goto bailout;
	    }
        Decrypt (lpOptions->szImpersonationPassword, szBuf, szKey);
    }

	// get the network path of where Symantec Setup lives

   lpOptions->szSymantecSetupPath[0] = 0;
   lpOptions->szSymantecSetupParameters[0] = 0;

//    if (GetPrivateProfileString(TEXT ("Install/Update"),
//                         TEXT ("NetworkNTDir"),
//                         TEXT (""),
//                         lpOptions->szSymantecSetupPath,
//                         sizeof(lpOptions->szSymantecSetupPath),
//                         lpOptions->szNNSPath) == 0)
//    {
//       goto bailout;
//    }
//
//    // get the Symantec Setup executable name and parameters
//
//    if (GetPrivateProfileString(TEXT ("Install/Update"),
//                         TEXT ("NAVNTInstall"),
//                         TEXT (""),
//                         lpOptions->szSymantecSetupParameters,
//                         sizeof(lpOptions->szSymantecSetupParameters),
//                         lpOptions->szNNSPath) == 0)
//    {
//       goto bailout;
//    }
//
	return(TRUE);

bailout:

	return(FALSE);

}



BOOL Exists(LPTSTR pszNNSPath)
{
	DWORD dwResult = {0};

	// using the GetFileAttributes to test existence of
	// the NNS file

	if (pszNNSPath == NULL)
	{
		goto bailout;
	}
	else
	{
		dwResult = GetFileAttributes(pszNNSPath);

		if (dwResult == 0xFFFFFFFF)
		{
			goto bailout;
		}
	}

	return(TRUE);

bailout:

	return(FALSE);

}
//******************************************************************************
//*																			   *
//*	IsRunningAsAdministrator												   *
//* 																		   *
//*		To check whether the current Process is running on Administrator	   *
//*		Context.  We browse the list of Groups that current user belongs and   *
//* 	compare each of them against Admin Group.							   *
//*																			   *
//*	Parameters:																   *
//*		- None -															   *
//*																			   *
//*	Returns:																   *
//*		BOOL		TRUE  if current user belongs to ADMIN group			   *
//*					FALSE if not.											   *
//*																			   *
//******************************************************************************
//*	1/14/97		PVENKAT		Function created.								   *
//******************************************************************************
BOOL
IsRunningAsAdministrator (VOID)
{
	HANDLE 						hProcess;			// Current Process
	TOKEN_GROUPS				*pTG = NULL;		// Token Groups
	DWORD						cbTokenGroups;		// # of Groups
	DWORD						dwGroup;			// for Index
	PSID						psidAdmin;			// Administrator SID
	SID_IDENTIFIER_AUTHORITY	SystemSidAuthority = SECURITY_NT_AUTHORITY;
	BOOL						fAdmin = FALSE;		// Return Value

	// First we must open a handle to tha access token for this thread.
	if (OpenProcessToken (GetCurrentProcess (), TOKEN_QUERY, &hProcess))
	{
		// We must query the size of group information associated with
		// the token.  Note that we expect a FALSE result from
		// GetTokenInformation because we've given it a NULL buffer.
		// On Exit cbTokenGroups will tell us the size of Group Information.

		if (!GetTokenInformation (hProcess, TokenGroups, NULL, 0, &cbTokenGroups))
		{
			// Allocate a buffer for the Group Info
			if ( (pTG = (TOKEN_GROUPS *) GlobalAlloc (GPTR,
                                                        cbTokenGroups))
                        != NULL)
			{
				// Get the Group Info.
				if (GetTokenInformation (hProcess, 			// Process Handle
										TokenGroups, 		// Group - Token
										pTG, 				// buffer
										cbTokenGroups, 		// buffer size
										&cbTokenGroups))	// bytes returned
				{
					// Create SID for Admin Group
					if (AllocateAndInitializeSid (&SystemSidAuthority,
													2,
													SECURITY_BUILTIN_DOMAIN_RID,
													DOMAIN_ALIAS_RID_ADMINS,
													0,
													0,
													0,
													0,
													0,
													0,
													&psidAdmin) )
					{
						// Go thru all groups until we find that we are a
						// member of Admin.
						for (dwGroup = 0;
								!fAdmin && dwGroup < pTG->GroupCount;
								dwGroup++)
						{
							// Compare the Current Group SID with Admin SID
							if (EqualSid (pTG->Groups[dwGroup].Sid, psidAdmin))
								fAdmin = TRUE;	// We are member of Admin Group.
						}
						FreeSid (psidAdmin);	// Clean up SID buffer
					}
				}
				GlobalFree (pTG);		// Clean up the Token Group Buffer.
			}
		}
	}
	return fAdmin;
}


VOID
Impersonate (void)
{
    OPTIONS rOptions;
    HANDLE  hToken;
    BOOL    fAdmin;

    lstrcpy (rOptions.szNNSPath, gszCmdLine);

    if (!Exists (rOptions.szNNSPath))
        goto bailout;

    if (!GetOptionsFromNNS (&rOptions))
        goto bailout;

    fAdmin = IsRunningAsAdministrator ();

    if (!fAdmin && rOptions.nImpersonate == 1)
    {

//        if (!LogonUser (rOptions.szImpersonationUserName,
//                        rOptions.szImpersonationDomain,
//                        rOptions.szImpersonationPassword,
//                        LOGON32_LOGON_BATCH,
//                        LOGON32_PROVIDER_DEFAULT,
//                        &hToken))
        if ( (hToken = Spawn (&rOptions)) == NULL)
        {
#ifdef  DEBUG
            _tprintf (TEXT ("LogonUser failed\n"));
#endif
            goto bailout;
        }

        if (!ImpersonateLoggedOnUser (hToken))
        {
#ifdef  DEBUG
            _tprintf (TEXT ("ImpersonateLoggedOnUser failed"));
#endif
            goto bailout;
        }
    }

bailout:
    return;
}


UINT WINAPI RebootWndProc (
    HWND    hDlg,
    UINT    message,
    WPARAM  wParam,
    LPARAM  lParam
    )
{
    auto    BOOL    bReturn = FALSE;
    auto    DWORD   dwRebootTimeInMsec;
    static  BOOL    bRebootFailure = FALSE;

    switch(message)
        {
        case WM_INITDIALOG:
                                        // Set the OS:
	        rOSVersionInfo.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
	        GetVersionEx(&rOSVersionInfo);
                                        // Center the dialog:
            CenterDialog ( hDlg );

#ifdef	IMPERSONATE
            // Marconi: Impersonate If necessary.
            Impersonate ();
#endif
                                        // If not silent mode, show the window and make
                                        // it a top-level window.
            if ( !bUseSilentMode )
                {
                SetWindowPos ( hDlg, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_SHOWWINDOW );
                }

                                        // Put an estimate of the wait time into
                                        // a string on the panel:
            SetWaitTimeText ( hDlg );

            dwRebootTimeInMsec = dwRebootWaitTime * 1000;
                                        // If reboot delay is zero msec, set it to
                                        // 400 msec just to give the system a little
                                        // catchup time.
            if ( dwRebootTimeInMsec == 0 ) { dwRebootTimeInMsec = 400 ; }
            uTimerID = SetTimer(
                    hDlg,
                    REBOOT_TIMER_ID,
                    dwRebootTimeInMsec,
                    NULL
                    );

            ClockAnimationStart ( hDlg );
            bReturn = TRUE;
            break;

        case WM_DRAWITEM:
            switch (((LPDRAWITEMSTRUCT)lParam)->CtlID)
                {
                case ID_REBOOTBMP:
                    DrawDialogBitmap (hDlg, (LPDRAWITEMSTRUCT)lParam);
                    break;

                case IDC_ANIMATION:
                    ClockAnimationDrawFrame ( (LPDRAWITEMSTRUCT)lParam );
                    break;

                default:
                    return FALSE;
                }
           break;


        case WM_TIMER:
            KillTimer ( hDlg , REBOOT_TIMER_ID );
            if  (
                ( rOSVersionInfo.dwPlatformId & VER_PLATFORM_WIN32_WINDOWS ) &&
                ( bUseForcedReboot ))
                {
                                        // If this is a forced reboot under Windows 95,
                                        // tell the NAV95 AutoProtect not to stand
                                        // in the way:
                AP_AllowShutdown();
                }

                                        // This statement does not return and
                                        // move on to the next line until all
                                        // Apps in the system have either agreed
                                        // to close, or one said "No".
            if (!RebootSystem()) { bRebootFailure = TRUE; }
            //bRebootFailure = TRUE;    // Used to debug
            ShowWindow ( hDlg , SW_HIDE );
            if (bRebootFailure)
                {
                TCHAR   szMsg[MAX_PATH + 1];
                TCHAR   szTitle[MAX_PATH + 1];

                LoadString (hAppInstance,
                            IDS_TITLE,
                            szTitle,
                            sizeof (szTitle));
                LoadString (hAppInstance,
                            IDS_ERR_UNABLE_REBOOT,
                            szMsg,
                            sizeof (szMsg));

                MessageBox
                    ( hDlg,
                    szMsg,
                    szTitle,
                    MB_OK | MB_SETFOREGROUND);
                }
            DestroyWindow ( hDlg );
            bReturn = 0;
            break;

        case WM_DESTROY:
            ClockAnimationEnd ( hDlg );
            PostQuitMessage(0);
            bReturn = 0;
            break;

        }                               // End switch

    return ( bReturn );
}




// Returns TRUE if it worked
BOOL RegisterRebootClass (           // Register W95 windows class
    void
    )
{



    auto    ATOM        aAtom = 0;
    auto    WNDCLASSEX  wcex = { 0 };
    memset ( &wcex, 0, sizeof(wcex) );

    wcex.style          = CS_NOCLOSE;
    wcex.lpfnWndProc    = (WNDPROC)RebootWndProc;
    wcex.cbClsExtra     = 0;
    wcex.cbWndExtra     = DLGWINDOWEXTRA;
    wcex.hInstance      = hAppInstance;
    //wcex.hIcon          = lpwc->hIcon;
    wcex.hCursor        = NULL;
    wcex.hbrBackground  = (HBRUSH)(COLOR_BTNFACE + 1);
    wcex.lpszMenuName   = NULL;
    wcex.lpszClassName  = REBOOT_MAIN_WND_CLASS;

                                        // -----------------------------------
                                        // Added elements for Windows 95:
                                        // -----------------------------------

    wcex.cbSize         = sizeof(WNDCLASSEX);

#if 0
    wcex.hIconSm = (HICON)LoadImage (
                                wcex.hInstance,
                                MAKEINTRESOURCE(IDI_NAVW_APP_SMALL),
                                IMAGE_ICON,
                                16,16,0
                                );

    wcex.hIcon = (HICON)LoadImage (
                                wcex.hInstance,
                                MAKEINTRESOURCE(IDI_NAVW_APP),
                                IMAGE_ICON,
                                32,32,0
                                );
#endif

    aAtom = RegisterClassEx(&wcex);

    return ( aAtom != 0 );
}


HWND RebootWndCreate (           // Creates main dialog wnd
    VOID
    )
{
    auto    HWND    hWndRet = NULL;     // Return value

    lpfn = (DLGPROC) RebootWndProc ;

    hWndRet = CreateDialog (hAppInstance, MAKEINTRESOURCE (IDD_DIALOG1),
                           NULL, lpfn);

    return ( hWndRet );
}



BOOL CenterDialog (HWND hWnd)
{
    int         xPopup, yPopup;
    int         cxPopup, cyPopup;
    int         cxScreen, cyScreen;
    RECT        rcWindow,rcScreen;

            /* retrieve main display dimensions */
    if (!SystemParametersInfo(SPI_GETWORKAREA,0,&rcScreen,FALSE))
        {
        rcScreen.top  = rcScreen.left = 0;
        rcScreen.right = GetSystemMetrics(SM_CXSCREEN);
        rcScreen.bottom = GetSystemMetrics(SM_CYSCREEN);
        }

            /* retrieve popup rectangle */
    GetWindowRect (hWnd, &rcWindow);

            /* calculate popup size */
    cxPopup = rcWindow.right - rcWindow.left;
    cyPopup = rcWindow.bottom - rcWindow.top;

                                        // Calculate Screen Height/Width
    cxScreen = rcScreen.right - rcScreen.left;
    cyScreen = rcScreen.bottom - rcScreen.top;

        xPopup = rcScreen.left + ((cxScreen - cxPopup) / 2);
        yPopup = rcScreen.top + ((cyScreen - cyPopup) / 2);

                /* move window to new location and display */
    MoveWindow(hWnd, (xPopup > 0) ? xPopup : 0, (yPopup > 0) ? yPopup : 0, cxPopup, cyPopup, TRUE);

    /* normal return */
    return (TRUE);
}



VOID WINAPI DrawDialogBitmap (          // Draw panel's left-side bitmap
    HWND             hDlg,              // [in] Handle to dialog box
    LPDRAWITEMSTRUCT lpDrawItem)        // [in] What needs to be done
{
    HPEN        hLightPen = NULL;
    HPEN        hDarkPen = NULL;
    HPEN        hOldPen = NULL;

    HBITMAP     hBmp            = NULL;
    HBITMAP     hBmpMem         = NULL;

    HBITMAP     hBmpOldMem      = NULL; // Save old bmp
    HBITMAP     hBmpOldStretch  = NULL; // Save old bmp

                                        // Stretching will be done from
                                        // a mem DC to another mem DC.
                                        // (some actual screen DC's may
                                        // not support StrecthBlt()).

    HDC         hDCMem          = NULL; // Mem DC for orig/pre-strech bitmap.
    HDC         hDCStretch      = NULL; // Mem DC for stretched bitmap.

    BITMAP      BitmapInfo;
    RECT        rBitmap;


                                        // Get the current panel's bitmap.
    hBmp = LoadBitmap(hAppInstance, MAKEINTRESOURCE( IDB_REBOOT ));

                                        // Get the size of the client rect
                                        // and create compatible DCs as
                                        // well as a compatible bitmap the
                                        // size of our client rect.
    hDCMem     = CreateCompatibleDC ( lpDrawItem->hDC );
    hDCStretch = CreateCompatibleDC ( lpDrawItem->hDC );

                                        // Adjust our bitmap rectangle
                                        // coordinates.
    rBitmap.left   = 0;
    rBitmap.top    = 0;
    rBitmap.right  = lpDrawItem->rcItem.right - lpDrawItem->rcItem.left;
    rBitmap.bottom = lpDrawItem->rcItem.bottom - lpDrawItem->rcItem.top;

    hBmpMem    = CreateCompatibleBitmap ( lpDrawItem->hDC,
                                          rBitmap.right,
                                          rBitmap.bottom );


    if ( (NULL == hDCMem) ||
         (NULL == hDCStretch) ||
         (NULL == hBmpMem) ||
         (NULL == hBmp) )
        {
        goto BailOut;                   // Flee, flee!
        }

                                        // Select bitmaps into off-screen DCs.
    hBmpOldMem        = (HBITMAP)SelectObject(hDCMem, hBmp);
    hBmpOldStretch = (HBITMAP)SelectObject(hDCStretch, hBmpMem);

                                        // Stretch bitmap to 2nd mem DC.
    GetObject ( hBmp, sizeof ( BITMAP ), &BitmapInfo );

    StretchBlt( hDCStretch,
                0, 0,
                rBitmap.right, rBitmap.bottom,
                hDCMem,
                0, 0,
                BitmapInfo.bmWidth, BitmapInfo.bmHeight,
                SRCCOPY );

                                        // Now blit the changes to the
                                        // screen DC.
    BitBlt( lpDrawItem->hDC,
            lpDrawItem->rcItem.left,
            lpDrawItem->rcItem.top,
            lpDrawItem->rcItem.right - lpDrawItem->rcItem.left,
            lpDrawItem->rcItem.bottom - lpDrawItem->rcItem.top,
            hDCStretch,
            0, 0,
            SRCCOPY );

                                        // ------------------------------------
                                        // Now, draw a frame around the bitmap:
                                        // ------------------------------------
    hLightPen = CreatePen (PS_SOLID, 1, GetSysColor (COLOR_3DHILIGHT) );
    hDarkPen = CreatePen (PS_SOLID, 1, GetSysColor (COLOR_3DSHADOW) );

    if ( (NULL == hLightPen) || (NULL == hDarkPen) )
        {
        goto BailOut;                   // Flee!
        }

    hOldPen = (HPEN)SelectObject (lpDrawItem->hDC, hDarkPen);

    MoveToEx (lpDrawItem->hDC,lpDrawItem->rcItem.left,lpDrawItem->rcItem.top,NULL);
    LineTo (lpDrawItem->hDC,lpDrawItem->rcItem.right,lpDrawItem->rcItem.top);
    MoveToEx (lpDrawItem->hDC,lpDrawItem->rcItem.left,lpDrawItem->rcItem.top,NULL);
    LineTo (lpDrawItem->hDC,lpDrawItem->rcItem.left,lpDrawItem->rcItem.bottom);

    SelectObject (lpDrawItem->hDC, hLightPen);

    MoveToEx (lpDrawItem->hDC,lpDrawItem->rcItem.left,(lpDrawItem->rcItem.bottom)-1,NULL);
    LineTo (lpDrawItem->hDC,(lpDrawItem->rcItem.right)+0,(lpDrawItem->rcItem.bottom)-1);
    MoveToEx (lpDrawItem->hDC,(lpDrawItem->rcItem.right)-1,(lpDrawItem->rcItem.bottom)-1,NULL);
    LineTo (lpDrawItem->hDC,(lpDrawItem->rcItem.right)-1,(lpDrawItem->rcItem.top)-1);

BailOut:
    if ( hDCMem )
        {
        if ( hBmpOldMem )
            SelectObject ( hDCMem, hBmpOldMem );
        DeleteDC ( hDCMem );
        }

    if ( hDCStretch )
        {
        if ( hBmpOldStretch )
            SelectObject ( hDCStretch, hBmpOldStretch );
        DeleteDC ( hDCStretch );
        }
    if ( hBmpMem )
        DeleteObject ( hBmpMem );
    if ( hBmp )
        DeleteObject ( hBmp );

    if ( hOldPen )
        SelectObject (lpDrawItem->hDC, hOldPen);

    DeleteObject ( hLightPen );
    DeleteObject ( hDarkPen );

}                                       // End DrawDialogBitmap()


VOID WINAPI AP_AllowShutdown ( void )
// Tells the NAV95 AP to let us shut down the system.
{
    unsigned int uNavApWMessage = 0L;
    HWND         hNAVAP;

    uNavApWMessage = RegisterWindowMessage (NAVW_NAVAPW_COMM);

    hNAVAP = FindWindow(NAVAP_WND_CLASS, NULL);

    if ( (uNavApWMessage != NULL) && (hNAVAP != NULL) )
        {
        PostMessage ( hNAVAP,
                        uNavApWMessage,
                        NAVW_CMD_UNLOADNAVAPW,
                        0L);
        }

}                                       // End AP_AllowShutdown()



//***************************************************************************
// ClockAnimationStart()
//
// Description:
//      Initiates animation for the Clock.  Loads up the bitmaps and masks
//      we'll need, as well as gets the show on the road for us.
//
// Parameters:
//      hDlg                            [in] Dialog box handle
//      lpAnimation                     [out] receives animation ptr
//
// See Also:
//      ClockAnimationEnd, ClockAnimationNextFrame, ClockAnimationDrawFrame
//
//***************************************************************************
// 05/06/1995 GWESTER Function Ported.
//***************************************************************************
VOID PASCAL ClockAnimationStart (      // Starts Animation
    HWND        hDlg
    )
{
    HGLOBAL hAnimationMem, hMaskMem;

                                        // -----------------------------------
                                        // Array of BMP IDs which will be used
                                        // as animated images in a cycle.
                                        // Note: ZERO TERMINATION is a MUST !
    static UINT aBmpIds [] =            // -----------------------------------
        {
        ID_NAVCLOCK_ANIM_BMP_1,
        ID_NAVCLOCK_ANIM_BMP_2,
        ID_NAVCLOCK_ANIM_BMP_3,
        ID_NAVCLOCK_ANIM_BMP_4,
        ID_NAVCLOCK_ANIM_BMP_5,
        ID_NAVCLOCK_ANIM_BMP_6,
        ID_NAVCLOCK_ANIM_BMP_7,
        ID_NAVCLOCK_ANIM_BMP_8,
        ID_NAVCLOCK_ANIM_BMP_9,
        ID_NAVCLOCK_ANIM_BMP_10,
        ID_NAVCLOCK_ANIM_BMP_11,
        ID_NAVCLOCK_ANIM_BMP_12,
        0
        };
                                        // -----------------------------------
                                        // Array of BMP Mask IDs which will be
                                        // used as animated images in a cycle.
    static UINT aBmpMaskIds [] =        // Note: ZERO TERMINATION is a MUST !
        {                               // -----------------------------------
        ID_NAVCLOCK_ANIM_MASK_BMP_1,
        ID_NAVCLOCK_ANIM_MASK_BMP_2,
        ID_NAVCLOCK_ANIM_MASK_BMP_3,
        ID_NAVCLOCK_ANIM_MASK_BMP_4,
        ID_NAVCLOCK_ANIM_MASK_BMP_5,
        ID_NAVCLOCK_ANIM_MASK_BMP_6,
        ID_NAVCLOCK_ANIM_MASK_BMP_7,
        ID_NAVCLOCK_ANIM_MASK_BMP_8,
        ID_NAVCLOCK_ANIM_MASK_BMP_9,
        ID_NAVCLOCK_ANIM_MASK_BMP_10,
        ID_NAVCLOCK_ANIM_MASK_BMP_11,
        ID_NAVCLOCK_ANIM_MASK_BMP_12,
        0
        };

    WORD        wCurBmp;

                                        // Allocate the memory needed for
                                        // our animation bitmaps and their
                                        // masks.
    ClockAnimation.wTotalFrames    = sizeof (aBmpIds) / sizeof (UINT) - 1;
    hAnimationMem = GlobalAlloc ( GMEM_ZEROINIT | GMEM_MOVEABLE,
                                 ClockAnimation.wTotalFrames * sizeof (HBITMAP) );
    ClockAnimation.lphBmpAnimation = (LPHBITMAP) GlobalLock ( hAnimationMem );
    hMaskMem = GlobalAlloc ( GMEM_ZEROINIT | GMEM_MOVEABLE,
                                 ClockAnimation.wTotalFrames * sizeof (HBITMAP) );
    ClockAnimation.lphBmpAnimationMask = (LPHBITMAP) GlobalLock ( hMaskMem );

    if ( ClockAnimation.lphBmpAnimation && ClockAnimation.lphBmpAnimationMask )
        {
                                        // Load up our bitmaps and their
                                        // masks.
        for ( wCurBmp = 0;
              aBmpIds[wCurBmp] && ( wCurBmp < ClockAnimation.wTotalFrames );
              wCurBmp++ )
            {
            ClockAnimation.lphBmpAnimation[wCurBmp] =
                LoadBitmap ( hAppInstance,
                             MAKEINTRESOURCE ( aBmpIds[wCurBmp] ) );
            ClockAnimation.lphBmpAnimationMask[wCurBmp] =
                LoadBitmap ( hAppInstance,
                             MAKEINTRESOURCE ( aBmpMaskIds[wCurBmp] ) );
            }
        if ( wCurBmp != ClockAnimation.wTotalFrames )
            ClockAnimation.wTotalFrames = wCurBmp;

                                        // Start our animation.
        ClockAnimation.uAnimationTimerID = SetTimer ( hDlg, 6, 100,
                                                 (TIMERPROC) ClockAnimationNextFrame );
        }
}



//***************************************************************************
// ClockAnimationNextFrame()
//
// Description:
//      Forces the animation to move on to the next frame.
// 
// Parameters:
//      hDlg                            [in] Dialog box handle
// 
// See Also: 
//      ClockAnimationDrawFrame
// 
//***************************************************************************
// 05/06/1995 GWESTER Function Ported.                                   
//***************************************************************************
VOID CALLBACK ClockAnimationNextFrame (
    HWND    hWnd,
    UINT    uMessage,
    UINT    uEventID,
    DWORD   dwTime
    )
{
    InvalidateRect ( GetDlgItem ( hWnd, IDC_ANIMATION ), NULL, FALSE );

    ClockAnimation.wCurrentFrame++;
    if ( ClockAnimation.wCurrentFrame == ClockAnimation.wTotalFrames )
        ClockAnimation.wCurrentFrame = 0;
}



//***************************************************************************
// ClockAnimationEnd()
// 
// Description:
//      Terminates the animation, cleaning up all resources, memory, and
//      timer.
// 
// Parameters:
//      hDlg                            [in] Dialog box handle
// 
// See Also: 
//      ClockAnimationStart
// 
//***************************************************************************
// 05/06/1995 GWESTER Function Ported.                                   
//***************************************************************************
VOID PASCAL ClockAnimationEnd (
    HWND        hDlg
    )
{
    WORD wCurBmp;

                                        // Stop our animation!
    if ( ClockAnimation.uAnimationTimerID )
        KillTimer ( hDlg, ClockAnimation.uAnimationTimerID );

                                        // Free up our resoureces.
    if ( ClockAnimation.lphBmpAnimation && ClockAnimation.lphBmpAnimationMask )
        {
        for ( wCurBmp = 0; wCurBmp < ClockAnimation.wTotalFrames; wCurBmp++ )
            {
            if ( ClockAnimation.lphBmpAnimation[wCurBmp] )
                DeleteObject ( ClockAnimation.lphBmpAnimation[wCurBmp] );
            if ( ClockAnimation.lphBmpAnimationMask[wCurBmp] )
                DeleteObject ( ClockAnimation.lphBmpAnimationMask[wCurBmp] );
            }
        }

                                        // Free our memory.
    if ( ClockAnimation.lphBmpAnimation )
        {
        GlobalUnlock ( ClockAnimation.lphBmpAnimation );
        GlobalFree ( ClockAnimation.lphBmpAnimation );
        }
    if ( ClockAnimation.lphBmpAnimationMask )
        {
        GlobalUnlock ( ClockAnimation.lphBmpAnimationMask );
        GlobalFree ( ClockAnimation.lphBmpAnimationMask );
        }
    ClockAnimation.lphBmpAnimation = ClockAnimation.lphBmpAnimationMask = NULL;
}



//***************************************************************************
// ClockAnimationDrawFrame()
// 
// Description:
//      This routine draws a frame of animation, taking in to account the
//      background, and doing so such that there is no flicker.
// 
// Parameters:
//      lpDrawItem                      [in] DrawItem from ownder draw msg
// 
// See Also: 
//      ClockAnimationStart, ClockAnimationNextFrame
// 
//***************************************************************************
// 05/06/1995 GWESTER Function Ported.                                   
//***************************************************************************
VOID PASCAL ClockAnimationDrawFrame (
    LPDRAWITEMSTRUCT    lpDrawItem
    )
{
    HBITMAP     hBmp            = NULL;
    HBITMAP     hBmpMask        = NULL;
    HBITMAP     hBmpMem         = NULL;
    HBITMAP     hBmpOld         = NULL;
    HDC         hDCMem          = NULL;
    RECT        rBitmap;

                                        // Get the bitmap and the mask we
                                        // will be working with.
    hBmp     = ClockAnimation.lphBmpAnimation[ClockAnimation.wCurrentFrame];
    hBmpMask = ClockAnimation.lphBmpAnimationMask[ClockAnimation.wCurrentFrame];

    if ( hBmp && hBmpMask )
        {
                                        // Get the size of the client rect
                                        // and create a compatible DC as
                                        // well as a compatible bitmap the
                                        // size of our client rect.
        hDCMem  = CreateCompatibleDC ( lpDrawItem->hDC );
        hBmpMem = CreateCompatibleBitmap ( lpDrawItem->hDC,
                                           lpDrawItem->rcItem.right - 
                                               lpDrawItem->rcItem.left,
                                           lpDrawItem->rcItem.bottom - 
                                               lpDrawItem->rcItem.top );

        if ( hDCMem && hBmpMem )
            {
                                        // Select the bitmap into the off-
                                        // screen DC,
            hBmpOld = (HBITMAP)SelectObject ( hDCMem, hBmpMem );

                                        // Adjust our bitmap rectangle
                                        // coordinates.
            rBitmap.left   = 0;
            rBitmap.top    = 0;
            rBitmap.right  = lpDrawItem->rcItem.right - lpDrawItem->rcItem.left;
            rBitmap.bottom = lpDrawItem->rcItem.bottom - lpDrawItem->rcItem.top;

                                        // In our off-screen DC, erase the
                                        // background such that we don't
                                        // leave trails.
            SetBkColor ( hDCMem, GetSysColor ( COLOR_BTNFACE ) );
            ExtTextOut ( hDCMem, 0, 0, ETO_OPAQUE, &rBitmap, TEXT (""), 0, NULL );

                                        // In our off-screen DC, now draw
                                        // our bitmap with its mask.
            DrawMaskedBitmap ( hDCMem, hBmp, hBmpMask, 
                                 lpDrawItem->rcItem.left, lpDrawItem->rcItem.top );

                                        // Now blit the changes to the 
                                        // screen DC for a nice smooth 
                                        // effect with no flicker.
            BitBlt ( lpDrawItem->hDC,
                     lpDrawItem->rcItem.left, lpDrawItem->rcItem.top,
                     lpDrawItem->rcItem.right - lpDrawItem->rcItem.left,
                     lpDrawItem->rcItem.bottom - lpDrawItem->rcItem.top,
                     hDCMem,
                     0, 0,
                     SRCCOPY );
            }
        }

                                        // Time for cleanup.
    if ( hDCMem )
        {
        SelectObject ( hDCMem, hBmpOld );
        DeleteDC ( hDCMem );
        }
    if ( hBmpMem )
        {
        DeleteObject ( hBmpMem );
        }
}


/************************************************************************
 *                                                                      *
 * Description:                                                         *
 *      Draws a bitmap using a bitmap mask, so bitmaps can appear in    *
 *      list boxes and things and be highlighted, etc.  The mask        *
 *      allows the current background color to show through, according  *
 *      to how the mask is constructed.  This function assumes that     *
 *      the bitmaps are the same size, which they really should be!     *
 *                                                                      *
 * Parameters:                                                          *
 *                                                                      *
 *      hDC             Device context to draw on                       *
 *                                                                      *
 *      hBitmap         Bitmap to draw                                  *
 *                                                                      *
 *      hMask           Bitmap containing a mask for hBitmap            *
 *                                                                      *
 *      x, y            Where to draw the bitmap                        *
 *                                                                      *
 * Return Value:                                                        *
 *      The return value is TRUE if the bitmap was drawn.               *
 *                                                                      *
 * See Also:                                                            *
 *                                                                      *
 ************************************************************************
 * 12/05/1992 ED Function Created.                                      *
 ************************************************************************/

BOOL WINAPI DrawMaskedBitmap (HDC hDC, HBITMAP hBitmap, HBITMAP hMask, int x, int y)
{
    HBITMAP     hOldBitmap;
    BITMAP      bm;
    HDC         hMemDC;
    int         nBkMode;
    BOOL        bResult = FALSE;


                                        // size the bitmap
    GetObject (hBitmap, sizeof(BITMAP), &bm);

    if (hMemDC = CreateCompatibleDC (hDC))
        {
        nBkMode = SetBkMode (hDC, OPAQUE);

        hOldBitmap = SelectBitmap (hMemDC, hMask);

                                        /* Output the Mask.   */
        BitBlt (hDC, x, y, bm.bmWidth, bm.bmHeight,
                hMemDC, 0, 0, 0x7700E6);

        SelectBitmap (hMemDC, hBitmap);

                                        /* Output the Image   */
        BitBlt (hDC, x, y, bm.bmWidth, bm.bmHeight,
              hMemDC, 0, 0, 0x990066);

        SetBkMode (hDC,nBkMode);
        SelectBitmap (hMemDC, hOldBitmap);

        DeleteDC (hMemDC);

        bResult = TRUE;
        }

    return (bResult);
}


VOID SetWaitTimeText (HWND hWnd)
{
                                        // If we hit any errors while doing this calculation,
                                        // we just exit out and leave the string blank.

                                        // Hard code the function to only work for zero
                                        // to ten minutes:
if ( ! ( ( dwRebootWaitTime > 0 ) && ( dwRebootWaitTime <= 600 ) ) )
    {return;}
int iWaitMinutes;

div_t div_result;
div_result = div( dwRebootWaitTime, 60 );

iWaitMinutes = div_result.quot;

if ((iWaitMinutes < 0) || (iWaitMinutes > 10)) {return;}

if (div_result.rem > 0 )
    {                                   // Round up
    iWaitMinutes++;
    }

TCHAR szTimeText [50];

switch ( iWaitMinutes )
    {
    case 0:
    case 1:
        LoadString ( (HINSTANCE) GetWindowLong (hWnd, GWL_HINSTANCE),
                        IDS_STR_ONE_MIN,
                        szTimeText,
                        sizeof (szTimeText));
        break;
    case 2:
        LoadString ( (HINSTANCE) GetWindowLong (hWnd, GWL_HINSTANCE),
                        IDS_STR_TWO_MIN,
                        szTimeText,
                        sizeof (szTimeText));
        break;

    case 3:
        LoadString ( (HINSTANCE) GetWindowLong (hWnd, GWL_HINSTANCE),
                        IDS_STR_THREE_MIN,
                        szTimeText,
                        sizeof (szTimeText));
        break;

    case 4:
    case 5:
        LoadString ( (HINSTANCE) GetWindowLong (hWnd, GWL_HINSTANCE),
                        IDS_STR_FIVE_MIN,
                        szTimeText,
                        sizeof (szTimeText));
        break;

    default:
        LoadString ( (HINSTANCE) GetWindowLong (hWnd, GWL_HINSTANCE),
                        IDS_STR_TEN_MIN,
                        szTimeText,
                        sizeof (szTimeText));
        break;

    }

TCHAR szWaitText [200];
TCHAR szWaitText1 [200];

        LoadString ( (HINSTANCE) GetWindowLong (hWnd, GWL_HINSTANCE),
                        IDS_STR_X_MINS,
                        szWaitText1,
                        sizeof (szTimeText));
wsprintf ( szWaitText, szWaitText1, szTimeText );

Static_SetText (GetDlgItem(hWnd, IDC_WAIT_ESTIMATE), szWaitText);

}
