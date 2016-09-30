#include "stdafx.h"
#include "navhelplauncher.h"
#include <atlbase.h>
#include <htmlhelp.h>

namespace NAVToolbox
{

// Help File Registry path and value names
const TCHAR HELPKEYPATH[] = _T("Software\\Microsoft\\Windows\\HTML Help");
const TCHAR HELPKEYVALUE[] = _T("symhelp.chm");
const TCHAR HELPFILENAMEWITHBACKSLASH[] = _T("\\symhelp.chm");

CNAVHelpLauncher::CNAVHelpLauncher(void)
{
    *m_szNAVHelpFile = _T('\0');
}

CNAVHelpLauncher::~CNAVHelpLauncher(void)
{
}

LPTSTR CNAVHelpLauncher::GetHelpPath ()
{
    if ( m_szNAVHelpFile[0] == '\0' )
    {
        if( !InitGetNAVHelpFile () )
            return NULL;
    }

    return m_szNAVHelpFile;
}

//-----------------------------------------------------------------------------
// Function: LaunchHelp()
//
// Purpose:  Launch HTML Help topic in a "safe" way. (i.e. Make sure Help
//           doesn't get launched in the local system security context).
//-----------------------------------------------------------------------------

HWND CNAVHelpLauncher::LaunchHelp ( DWORD dwHelpID /*= 0*/, HWND hParentWnd /*= NULL*/, bool bTopMost /*= false*/ )
{
    if ( m_szNAVHelpFile[0] == '\0' )
    {
        if( !InitGetNAVHelpFile () )
		{
			CCTRACEE("Fail to init Nav help");
            return NULL;
		}
    }

    HWND hwndRet = NULL;

    UINT uCommand = HH_HELP_CONTEXT;
    if ( dwHelpID == 0 )
        uCommand = HH_DISPLAY_TOC;

    if( hParentWnd == NULL )
        hParentWnd = ::GetDesktopWindow();

    // Just call HtmlHelp() to launch the help file/topic

	CCTRACEI("Launch help: '%s', %d", m_szNAVHelpFile, dwHelpID);

    // Make sure we are NOT running as a service before launching help in the standard way.
    // This is because HtmlHelp() launches the Help Viewer in OUR security context. This
    // would be a security vulnerability if we were to do this from a service.
    if ( !RunningAsAService() )
    {
        if( !(hwndRet = ::HtmlHelp(hParentWnd, m_szNAVHelpFile, uCommand, dwHelpID)) )
        {
            /*  // I'm leaving this in here in case we need to debug it. -GPOLK 6/7/2003
            typedef struct tagHH_LAST_ERROR { int cbStruct; 
                                            HRESULT hr;
                                            BSTR description;
                                            } HH_LAST_ERROR;

            HH_LAST_ERROR hherr;
            HRESULT hr = E_FAIL;
            hherr.cbStruct = sizeof(HH_LAST_ERROR);
            HtmlHelp(hParentWnd, m_szNAVHelpFile, HH_GET_LAST_ERROR, (DWORD) &hherr);
            if (FAILED(hr) && hherr.description!=NULL)
            {
                char szTemp [1024] = {0};
                wsprintf(szTemp, "ERROR %08X\n%S", hherr.hr, hherr.description);
                ::MessageBox ( hParentWnd, szTemp, "NAV Help Error - comment this before shipping!!", MB_OK );
                // free the string after use
                ::SysFreeString(hherr.description);
            }
		    */

		    CCTRACEE("Fail to launch NAV help (%d)", dwHelpID);
		    if (dwHelpID == 0)
	            return NULL;

		    CCTRACEE("Try again without context");
	        if( !(hwndRet = ::HtmlHelp(hParentWnd, m_szNAVHelpFile, HH_DISPLAY_TOC, 0)) )
		    {
			    CCTRACEE("Fail to launch help file='%s'", m_szNAVHelpFile);
	            return NULL;
		    }
        }

        // Set the help window to a topmost window if specified by caller
        if( bTopMost )
        {
            SetWindowPos(hwndRet, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE );
        }
    }
    else
    {
        CCTRACEW("CNAVHelpLauncher::LaunchHelp() - launching help from a service?!");

        // We're running as a service, so launch help in the context of the logged on user.
        LaunchHelpAsUser( m_szNAVHelpFile, dwHelpID );
    }

    return hwndRet;
}

bool CNAVHelpLauncher::InitGetNAVHelpFile()
{
    bool bRet = false;
    // Get the help file path from the registry
    CRegKey helpRegKey;
    if( ERROR_SUCCESS == helpRegKey.Open(HKEY_LOCAL_MACHINE, HELPKEYPATH, KEY_QUERY_VALUE) )
    {
        DWORD dwSize = sizeof(m_szNAVHelpFile);
        if( ERROR_SUCCESS == helpRegKey.QueryStringValue(HELPKEYVALUE, m_szNAVHelpFile, &dwSize) )
        {
            //
            // Now append the file name
            //

            // If there is a trailing backslash remove it
            PathRemoveBackslash(m_szNAVHelpFile);
            _tcscat(m_szNAVHelpFile, HELPFILENAMEWITHBACKSLASH);

            // Use OEM path if necessary
            if( !AreFileApisANSI() )
            {
                // Just in case this is using wide chars use another buffer
                TCHAR szTemp[MAX_PATH] = {0};
                CharToOemBuff(m_szNAVHelpFile, szTemp, MAX_PATH);
                _tcscpy(m_szNAVHelpFile, szTemp);
            }

            bRet = true;
        }
        helpRegKey.Close();
    }

    return bRet;
}

//-----------------------------------------------------------------------------
// Function: LaunchHelpAsUser()
//
// Purpose:  This function will launch Microsoft's built-in HTML Help viewer,
//           in the context of the logged-on user.
//-----------------------------------------------------------------------------
BOOL CNAVHelpLauncher::LaunchHelpAsUser( LPCTSTR pszHelpFile, DWORD dwHelpID )
{
    PROCESS_INFORMATION pi = { 0 };
    STARTUPINFO si   = { 0 };
    si.cb            = sizeof(si);
    si.dwFlags       = STARTF_USESHOWWINDOW;
    si.wShowWindow   = SW_NORMAL;
    BOOL bv          = FALSE;

    // Form the command line to launch
    TCHAR szWinDir[MAX_PATH]      = { _T("") };
    TCHAR szCmdLine[MAX_PATH * 2] = { _T("") };
    GetWindowsDirectory( szWinDir, sizeof(szWinDir) / sizeof(TCHAR) );

    // If you want to call into the older WinHelp engine, then just replace these lines
    // with the following command-line (<path to windows>\\winhlp32.exe -n%ld %s)
    if ( dwHelpID )
        wsprintf( szCmdLine, _T("\"%s\\hh.exe\" -mapid%ld %s"), szWinDir, dwHelpID, pszHelpFile );
    else
        wsprintf( szCmdLine, _T("\"%s\\hh.exe\" %s"), szWinDir, pszHelpFile );

    HANDLE hUserToken = GetCurrentUserToken();
    if ( hUserToken )
    {
        // Launch hh.exe with our .chm help file
        bv = CreateProcessAsUser( hUserToken, 0, szCmdLine, 0, 0, 0, 0, 0, 0, &si, &pi );
        if ( bv )
        {
            CloseHandle( pi.hThread );
            CloseHandle( pi.hProcess );
        }

        CloseHandle( hUserToken );
    }

    return bv;
}

//---------------------------------------------------------------------------
// Function: RunningAsAService()
//
// Purpose:  Returns TRUE if we're in a process that is running in
//           the local system account. We then assume this is a service.
//---------------------------------------------------------------------------
BOOL CNAVHelpLauncher::RunningAsAService()
{
    BOOL bv = FALSE;
    HANDLE hProcess = GetCurrentProcess();
    HANDLE hToken = NULL;
    PTOKEN_USER pTokenUser = NULL;
    PSID LocalSystemSid = NULL;
    PSID Sid = NULL;
    DWORD dwSize = 0;

    OSVERSIONINFO osInfo = { 0 };
    osInfo.dwOSVersionInfoSize = sizeof(osInfo);
    GetVersionEx( &osInfo );
    if ( osInfo.dwPlatformId != VER_PLATFORM_WIN32_NT )
        return FALSE;

    // Check to see if this thread is impersonating. If it is, then save the impersonation
    // token because we're going to have to turn off the impersonation temporarily
    HANDLE hImpersonationToken = NULL;
    if ( OpenThreadToken(GetCurrentThread(), MAXIMUM_ALLOWED, TRUE, &hImpersonationToken) )
    {
        // Turn off impersonation temporarily so that we can open the process token below
        RevertToSelf();
    }

    __try
    {
        // First get this process's access token
        if ( !OpenProcessToken(hProcess, TOKEN_QUERY, &hToken) )
            __leave;

        // Then get the "User account information" out of the access token
        // in the form of a SID (Security Identifier)
        // It will be a variable-length structure so let's first find out
        // how much space we need
        if ( !GetTokenInformation(hToken, TokenUser, NULL, 0, &dwSize) &&
            (GetLastError() != ERROR_INSUFFICIENT_BUFFER) )
            __leave;

        // Allocate space for this information
        pTokenUser = (PTOKEN_USER) malloc(dwSize);
        if ( !GetTokenInformation(hToken, TokenUser, pTokenUser, dwSize, &dwSize) )
            __leave;

        // Got the SID!!
        Sid = pTokenUser->User.Sid;

        SID_IDENTIFIER_AUTHORITY SIDAuth = SECURITY_NT_AUTHORITY;
        if ( !AllocateAndInitializeSid( &SIDAuth, 1, SECURITY_LOCAL_SYSTEM_RID,
             0,0,0,0,0,0,0, &LocalSystemSid ) )
            __leave;

        if ( !IsValidSid(Sid) || !IsValidSid(LocalSystemSid) )
            __leave;

        // Now let's compare this process's SID with the
        // Local System Account SID to see if they're equal
        if ( EqualSid(Sid, LocalSystemSid) )
            bv = TRUE;
    }

    __finally 
    {
        // Clean up
        if ( hToken )
            CloseHandle( hToken );

        if ( pTokenUser )
            free( pTokenUser );

        if ( LocalSystemSid )
            FreeSid( LocalSystemSid );

        // If this thread was impersonating before, then restore the impersonation once again
        if ( hImpersonationToken )
        {
            SetThreadToken( NULL, hImpersonationToken );
            CloseHandle( hImpersonationToken );
        }
    }

    return bv;
}

//-----------------------------------------------------------------------------
// Func:GetUserTokenViaShellProcess()
// 
// Purpose:This function retrieves the currently logged-on user's access token
//by opening the access token associated with the only process we can
//be sure is opened in the context of the interactively logged-on
//user... the shell process, which in most cases is explorer.exe.
//
// Note:** Important **
//If you call this function... you must remember to *close* the token
//when you're done with it. Otherwise there will be a resource leak.
//-----------------------------------------------------------------------------
// 08/08/2001  Alan Feldman  function created
//-----------------------------------------------------------------------------
HANDLE CNAVHelpLauncher::GetCurrentUserToken()
{
    HWND    hDesktopWnd    = NULL;
    HANDLE  hShellProcess  = NULL;
    HANDLE  hUserToken     = NULL;
    DWORD   dwShellProcID  = NULL;
    DWORD   dwLastError    = NULL;

    __try
    {
        // This function used to be "undocumented" but is now fully supported by Microsoft since
        // the anti-trust settlement came out. It has existed as an exported user32.dll function
        // in every OS since Windows 95 and NT 4.0. If you aren't using the latest Platform SDK
        // header files just use this declaration:
        // extern "C" { __declspec(dllimport) HWND WINAPI GetShellWindow( void ); }
        hDesktopWnd = GetShellWindow();
        if ( !hDesktopWnd )
        {
            dwLastError = GetLastError();
            CCTRACEE("CNAVHelpLauncher::GetCurrentUserToken(): Couldn't get a handle to the Shell window.");
            __leave;
        }

        GetWindowThreadProcessId( hDesktopWnd, &dwShellProcID );
        if ( !dwShellProcID )
        {
            dwLastError = GetLastError();
            CCTRACEE("CNAVHelpLauncher::GetCurrentUserToken(): Couldn't get the process id of the shell window. error = %ld", GetLastError() );
            __leave;
        }

        hShellProcess = OpenProcess( MAXIMUM_ALLOWED, FALSE, dwShellProcID );
        if ( !hShellProcess )
        {
            dwLastError = GetLastError();
            CCTRACEE("CNAVHelpLauncher::GetCurrentUserToken(): Couldn't get a handle to the Shell Process Object. error = %ld", GetLastError() );
            __leave;
        }

        OpenProcessToken( hShellProcess, MAXIMUM_ALLOWED, &hUserToken );
        if ( !hUserToken )
        {
            dwLastError = GetLastError();
            CCTRACEE("CNAVHelpLauncher::GetCurrentUserToken(): Couldn't get a handle to the user's token.");
            __leave;
        }
    }

    __finally
    {
        if ( hShellProcess )
        CloseHandle( hShellProcess );
    }

    if ( hUserToken )
        CCTRACEI("CNAVHelpLauncher::GetCurrentUserToken(): User token is 0x%X", hUserToken );
    else
        SetLastError( dwLastError );

    return hUserToken;
}

}// End namespace NAVToolBox