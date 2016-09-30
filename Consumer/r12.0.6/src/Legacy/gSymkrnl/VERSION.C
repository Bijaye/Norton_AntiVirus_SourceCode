//////////////////////////////////////////////////////////////////////////
// Copyright 1993 Symantec, Peter Norton Computing Group
//
// Description: This file contains the prototypes and macros for runtime DLL
// version checking
//
// $Header:   S:/SYMKRNL/VCS/version.c_v   1.1   23 Jun 1996 12:32:40   MBARNES  $
//
// $Log:   S:/SYMKRNL/VCS/version.c_v  $
// 
//    Rev 1.1   23 Jun 1996 12:32:40   MBARNES
// Added support for x.y.z.a version #s
// 
//    Rev 1.0   26 Jan 1996 20:22:58   JREARDON
// Initial revision.
// 
//    Rev 1.20   14 Apr 1994 19:01:32   BRAD
// For WIN32, need to look for _CheckVersion@8, rather than CheckVersion
// 
//    Rev 1.19   14 Apr 1994 15:25:00   BRAD
// Use new CheckVersion() method
// 
//    Rev 1.18   14 Apr 1994 11:45:10   BRAD
// Fixed typo
// 
//    Rev 1.17   11 Apr 1994 17:27:42   BRAD
// Don't display hard error, if DLL not found
// 
//    Rev 1.16   31 Mar 1994 14:47:20   BRAD
// 
//    Rev 1.15   29 Mar 1994 12:55:02   BRAD
// Added WIN32 version of CheckVersion
// 
//    Rev 1.14   26 Mar 1994 21:44:16   BRAD
// Use portable messages
// 
//    Rev 1.13   20 Mar 1994 17:19:20   BRAD
// Cleaned up for SYM_WIN32
// 
//    Rev 1.12   15 Mar 1994 12:34:00   BRUCE
// Changed SYM_EXPORT to SYM_EXPORT
// 
//    Rev 1.11   25 Feb 1994 12:22:56   BRUCE
// Ran scripts to change to SYM_ constants and get ready for Win32
// 
//    Rev 1.10   26 Jan 1994 08:27:46   MARKK
// Added SYMIsWin310or311
// 
//    Rev 1.9   28 Oct 1993 15:30:46   DLEVITON
// Check ini setting to allow avoidance of annoying dialog.
// 
//    Rev 1.6.4.2   26 Oct 1993 23:41:14   PGRAVES
// Typo.
// 
//    Rev 1.6.4.1   26 Oct 1993 23:37:00   PGRAVES
// Optimized version checking.
//
//    Rev 1.6.4.0   19 Oct 1993 09:27:52   SCOTTP
// Branch base for version QAK2
//
//    Rev 1.6   25 Jul 1993 11:32:52   DLEVITON
// SYMCheckVersions tries deleting up to 2 suffix chars from module name
// when building function name to pass to GetProcAddress.
//
//
//    Rev 1.5   06 Jul 1993 11:08:04   DLEVITON
// Removed dependence on CTL3D
//
//    Rev 1.4   23 Jun 1993 17:15:46   DLEVITON
// Oops! Changed < to >.
//
//    Rev 1.3   23 Jun 1993 16:30:00   DLEVITON
// Be more considerate of non-Windows platforms.
//
//
//    Rev 1.2   23 Jun 1993 10:42:26   DLEVITON
// Use full module path name when DLLCheckVersion proc address is not found.
//
//
//    Rev 1.1   22 Jun 1993 18:43:52   DLEVITON
// Fixed typo.
//
//    Rev 1.0   22 Jun 1993 12:24:20   DLEVITON
// Support for runtime DLL version checking.
//
/////////////////////////////////////////////////////////////////////////

#include "platform.h"
#include "version.h"
#include "dialogs.h"
#include "symkrnli.h"
#include "quakever.h"

#ifndef SYM_WIN
//////////////////////////////////////////////////////////////////////////
//  SYMCheckVersions
//
//  An application calls this function before calling any of the Symantec
//  DLLs it links to, in order to ensure that the versions of the DLLs are
//  recent enough. The app passes in an array of DLLs to check.
//  SYMCheckVersions calls the "CheckVersion" function for each of
//  of DLLs, using a run-time link. If any of the DLLs is too old, can't
//  be found, or doesn't SYM_EXPORT a "CheckVersion" function, a dialog
//  box informs the user of the nature of the problem.
//  For code simplicity, this function always returns NOERR under platforms
//  other than Windows.
//////////////////////////////////////////////////////////////////////////
int SYM_EXPORT WINAPI SYMCheckVersions( HWND hWnd, UINT uCount, LPVERREC lpVer )
{
    return NOERR;
}

#else

extern HINSTANCE    hInst_SYMKRNL;

//////////////////////////////////////////////////////////////////////////
// Variables global to this source file only.
//////////////////////////////////////////////////////////////////////////

static char         szFmt[] = "%2u.%2u.%2u.%2u";
static char         szFmt1[] = "%s\t%2u.%2u.%2u.%2u\t%s";
static VERREC       Actual = { NULL, SYMKRNL_VERSION_MAJOR,
                               SYMKRNL_VERSION_MINOR, 0,
                               SYMKRNL_BUILD_NUMBER, 0 };
static UINT         _uCount;
static LPVERREC     _lpVer;
static HWND         _hDlg;

//////////////////////////////////////////////////////////////////////////
// Local function prototypes.
//////////////////////////////////////////////////////////////////////////
BOOL SYM_EXPORT CALLBACK VersionDlg( HWND hDlg, UINT uMsg, WPARAM wParam,
                                    LPARAM lParam );
BOOL LOADDS CALLBACK WinVersionDlg(HWND hDlg, UINT wMessage, WPARAM wParam, LPARAM lParam);


//////////////////////////////////////////////////////////////////////////
//  SYMCheckVersions
//
//  An application calls this function before calling any of the Symantec
//  DLLs it links to, in order to ensure that the versions of the DLLs are
//  recent enough. The app passes in an array of DLLs to check.
//  SYMCheckVersions calls the "CheckVersion" function for each of
//  of DLLs, using a run-time link. If any of the DLLs is too old, can't
//  be found, or doesn't SYM_EXPORT a "CheckVersion" function, a dialog
//  box informs the user of the nature of the problem.
//  For code simplicity, this function always returns NOERR under platforms
//  other than Windows.
//
//  Input:  hWnd        app window handle, to use as parent, may be NULL
//          uCount      number of DLLs in the list to check
//          lpVer       array of VERREC, uCount in number
//  Return: ERR         a mismatch was found and the user didn't override
//          NOERR       no mismatch found, or the user chose to override
//////////////////////////////////////////////////////////////////////////
int SYM_EXPORT WINAPI SYMCheckVersions( HWND hWnd, UINT uCount, LPVERREC lpVer )
{

static BOOL             fBusy;
    int                 nDlgRtn;

    // Avoid re-entering.
    if (fBusy)
        return ERR;

    fBusy = TRUE;
    _uCount = uCount;
    _lpVer = lpVer;
    nDlgRtn = DialogBox( hInst_SYMKRNL, MAKEINTRESOURCE(IDD_VERSION), hWnd,
                VersionDlg );
    _hDlg = NULL;
    fBusy = FALSE;
    return (nDlgRtn == IDCANCEL ? NOERR : ERR);
}   // SYMCheckVersions




//////////////////////////////////////////////////////////////////////////
// CheckVersion
//
// This function is run-time linked and executed.  Note that all it does
// is call the static-link version.  That way, other DLLs can static-link
// to the static-link function, and we can still run-time link to a 
// known function name ("CheckVersion").
//
// Input:   lpExpected  expected version
//          CallBack    function to call in case of mismatch
//////////////////////////////////////////////////////////////////////////
void SYM_EXPORT WINAPI CheckVersion( LPVERREC lpExpected,
    CHECKVERCALLBACK_TYP CallBack )
{
    SYMKRNLCheckVersion(lpExpected, CallBack);
}


//////////////////////////////////////////////////////////////////////////
// SYMKRNLCheckVersion
//
// A DLL which links to SYMKRNL calls this function inside of its own
// <DLL-Name>CheckVersion function, in order to ensure that the version
// of SYMKRNL it is linked to is late enough.
//
// Input:   lpExpected  expected version
//          CallBack    function to call in case of mismatch
//////////////////////////////////////////////////////////////////////////
void SYM_EXPORT WINAPI SYMKRNLCheckVersion( LPVERREC lpExpected,
    CHECKVERCALLBACK_TYP CallBack )
{
    char        szExpected[13];
    char        szActual[13];

    wsprintf( szExpected, szFmt, lpExpected->uMajor, lpExpected->uMinor,
              lpExpected->uRevision, lpExpected->uBuild );
    wsprintf( szActual, szFmt, Actual.uMajor, Actual.uMinor,
              Actual.uRevision, Actual.uBuild  );
    if (STRCMP(szExpected, szActual) > 0)
        CallBack( hInst_SYMKRNL, lpExpected, &Actual );
    return;
}   // SYMKRNLCheckVersion



//////////////////////////////////////////////////////////////////////////
// CheckVersionCallBack
//
// This is the callback function SYMCheckVersions uses to accumulate data
// for the mismatches dialog box.
//////////////////////////////////////////////////////////////////////////
void SYM_EXPORT WINAPI CheckVersionCallBack( HINSTANCE hInst, LPVERREC lpExpected,
    LPVERREC lpFound )
{
    char    szName[256];
    char    szTmp[256+81];
    HWND    hListBox;
    UINT    uComplain;
    BOOL    fAdd = TRUE;

    uComplain = GetPrivateProfileInt( "Defaults", "VersionCheck", -1,
        "symcfg.ini" );
    switch (uComplain)
        {
        case 0xFFFF:
            break;
        case 0:
            fAdd = FALSE;
            break;
        default:
            if (lpExpected->uMajor == lpFound->uMajor &&
                lpExpected->uMinor == lpFound->uMinor &&
                lpExpected->uRevision == lpFound->uRevision &&
                lpExpected->uBuild - lpFound->uBuild <= uComplain)
                fAdd = FALSE;
            break;

        }
    hListBox = GetDlgItem( _hDlg, ID_LISTBOX );
    
    GetModuleFileName( hInst, szName, sizeof(szName) );
    wsprintf( szTmp, "%s\t%2u.%2u.%2u.%2u\t%2u.%2u.%2u.%2u", (LPSTR)szName,
        lpExpected->uMajor, lpExpected->uMinor, lpExpected->uRevision, lpExpected->uBuild,
        lpFound->uMajor, lpFound->uMinor, lpFound->uRevision, lpFound->uBuild );

    // Avoid duplicates.
    if (fAdd && ListBox_FindString( hListBox, -1, szTmp) == LB_ERR)
        ListBox_AddString( hListBox, szTmp );
    return;
}   // CheckVersionCallBack


/*********************************************************************
 * VersionDlg
 * Dialog function for Version box.
 *********************************************************************/
BOOL SYM_EXPORT CALLBACK VersionDlg( HWND hDlg, UINT uMsg, WPARAM wParam,
    LPARAM lParam )
{
static int              TabStops[2] = { 174, 203 }; // dialog units
    char                szErr[81];
    char                szTmp[256+81];
    HINSTANCE           hLib;
    HWND                hListBox;
    UINT                ii;
    DLLCHECKVERSION_TYP DLLCheckVersion;
    LPVERREC            lpVer;
    BOOL                rtn = TRUE;
    UINT                uErrMode;

    switch (uMsg)
    {
    case WM_INITDIALOG:
        _hDlg = hDlg;       // for use by callback
        lpVer = _lpVer;
        hListBox = GetDlgItem( hDlg, ID_LISTBOX );
                                        // Don't display hard error, if dll
                                        // not found.
        uErrMode = SetErrorMode(SEM_FAILCRITICALERRORS | SEM_NOOPENFILEERRORBOX);
        for (ii = 0; ii < _uCount; ii++, lpVer++)
            {
            hLib = LoadLibrary( lpVer->lpName );
            if (hLib < (HINSTANCE) HINSTANCE_ERROR)
                {
                LoadString( hInst_SYMKRNL, IDS_NOTFOUND, szErr, sizeof(szErr) );
                wsprintf( szTmp, szFmt1, lpVer->lpName,
                          lpVer->uMajor, lpVer->uMinor, lpVer->uRevision,
                          lpVer->uBuild, (LPSTR)szErr );
                ListBox_AddString( hListBox, szTmp );
                }
            else
                {
                                        // Get CheckVersion proc address
                DLLCheckVersion = (DLLCHECKVERSION_TYP) GetProcAddress(hLib, 
#if defined(SYM_WIN32)
                                                            "_CheckVersion@8");
#else
                                                            "CheckVersion");
#endif
                
                if (DLLCheckVersion)
                    DLLCheckVersion( lpVer, CheckVersionCallBack );
                else
                    {   // DLLCheckVersion not found, so version is unknown.
                    char    szName[256];

                    GetModuleFileName( hLib, szName, sizeof(szName) );
                    LoadString( hInst_SYMKRNL, IDS_UNKNOWN, szErr, sizeof(szErr) );
                    wsprintf( szTmp, szFmt1, (LPSTR)szName,
                              lpVer->uMajor, lpVer->uMinor, lpVer->uRevision,
                              lpVer->uBuild, (LPSTR)szErr );
                    ListBox_AddString( hListBox, szTmp );
                    }   // DLLCheckVersion not found, so version is unknown.
                    
                FreeLibrary( hLib );
                }
            }   // for ii
        SetErrorMode(uErrMode);         // Restore error mode

        if (ListBox_GetCount( hListBox ) > 0)
            ListBox_SetTabStops( hListBox, 2, TabStops );
        else
            EndDialog( hDlg, IDCANCEL );        // Allow app to continue.
        break;  // WM_INITDIALOG

    case WM_COMMAND:
        switch (GET_WM_COMMAND_ID(wParam, lParam))
        {
        case IDOK:
            // fall thru
        case IDCANCEL:
            EndDialog( hDlg, GET_WM_COMMAND_ID(wParam, lParam));
            break;
        default:
            break;
        }
        break;  // WM_COMMAND

    default:
        rtn = FALSE;
        break;
    }   // switch uMsg
    return rtn;
}   // VersionDlg

//////////////////////////////////////////////////////////////////////////
// SYMIsWin310or311
//
// Checks to make sure we are on Windows version 3.10 or WFW 3.11.
//
//////////////////////////////////////////////////////////////////////////

BOOL SYM_EXPORT WINAPI SYMIsWin310or311(void)
{
    auto	DWORD	dwVersion = GetVersion();
    
    if (LOBYTE(LOWORD(dwVersion)) == 3)
	if (HIBYTE(LOWORD(dwVersion)) == 10 ||
	    HIBYTE(LOWORD(dwVersion)) == 11)
	    return (TRUE);
	    
    DialogBox(hInst_SYMKRNL,
	      MAKEINTRESOURCE(IDD_UNSUPPORTED_WINDOWS_VER), 
	      NULL,
	      WinVersionDlg);
	      
    return (FALSE);
}

BOOL LOADDS CALLBACK WinVersionDlg(HWND hDlg, UINT wMessage, WPARAM wParam, LPARAM lParam)
{
    switch (wMessage)
        {
        case WM_INITDIALOG:
            return (TRUE);

        case WM_COMMAND:
            switch (GET_WM_COMMAND_ID(wParam, lParam))
                {
                case IDCANCEL:
                case IDOK:
                    EndDialog(hDlg, (WORD) (GET_WM_COMMAND_ID(wParam, lParam) == IDOK) );
                    break;
                }
            break;
        }

    return (FALSE);
}

#endif // SYM_WIN

