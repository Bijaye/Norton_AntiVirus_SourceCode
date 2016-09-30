// Copyright 1996-1997 Symantec, Peter Norton Product Group
//****************************************************************************
//
// $Header:   S:/NAVLU/VCS/NavLuUtils.cpv   1.8   24 Nov 1997 14:03:54   jtaylor  $
//
// Description:
//   Integrates LiveUpdate into NAV. This file was moved from the ONRAMP
//   project to here.
//
// Contains:
//
// See Also:
//****************************************************************************
// $Log:   S:/NAVLU/VCS/NavLuUtils.cpv  $
// 
//    Rev 1.8   24 Nov 1997 14:03:54   jtaylor
// Added an optional filename parameter to NavLuIsLuInstalled.  If a filename is specified it will not return TRUE unless that file exists.
//
//    Rev 1.7   15 Sep 1997 17:51:14   DALLEE
// Close handles returned by CreateProcess().
//
//    Rev 1.6   11 Sep 1997 13:25:52   MKEATIN
// Ported changes from branch G.
//
//    Rev 1.4.1.1   25 Aug 1997 15:32:56   JBRENNA
// Update RegOpens to use KEY_READ rather than KEY_ALL_ACCESS.
//
//    Rev 1.4.1.0   12 Aug 1997 22:57:42   DLEWIS
// Branch base for version QAKG
//
//    Rev 1.4   26 Jun 1997 18:12:30   JBRENNA
// Add precompiled header (PCH) support.
//
//    Rev 1.3   03 Jun 1997 13:58:14   JBRENNA
// Add NavLuLaunchLiveUpdate function. This way NAVW can call this to launch
// NavLu32.exe.
//
//    Rev 1.2   02 Jun 1997 18:18:28   JBRENNA
// 1. Moved CNavLu::MyNameAppendFile to here (NavLuNameAppendFile).
// 2. Moved CNavLu::MyFileExists     to here (NavLuFileExists).
// 3. Moved CNavLu::GetLuPathName    to here (NavLuGetLuPathName).
// 4. Renamed NavIsLuInstalled to NavLuIsLuInstalled. This is consistent with
//    the other functions named here.
// 5. Changed NavLuIsLuInstalled to use the functions in this file rather than
//    the CNavLu class. This way we do not need to link in CNavLu.
//
//    Rev 1.1   16 May 1997 11:20:36   JBRENNA
//
// Add support for MFC.
//
//    Rev 1.0   10 May 1997 01:21:16   JBRENNA
// Initial revision.
//
//    Rev 1.12   18 Oct 1996 19:26:58   MKEATIN
// Defined hDelayKey
//
//    Rev 1.11   18 Oct 1996 19:22:14   MKEATIN
// Ported Gryphon code for handling files in use.
//
//    Rev 1.10   30 Sep 1996 16:49:40   MKEATIN
// Changed the LiveUpdate reg value to LiveUpdate1
//
//    Rev 1.9   30 Sep 1996 16:21:40   MKEATIN
// Changed title for the OAM_NOTHINGNEW condition on the Finish Panel.
//
//    Rev 1.8   04 Sep 1996 14:47:36   JBRENNA
// Add run-time check to determine if we should use "NAVNT" or "NAV95" for
// the "HKLM\SOFTWARE\Symantec\InstalledApps" value in IsOldThanOneMonth
// and SetSequenceDate.
//
//    Rev 1.7   30 Jul 1996 11:26:28   MKEATIN
// Ported recent changes from LUIGI
//
//    Rev 1.6   16 Jul 1996 12:27:46   MKEATIN
// Changed NAV95 to NAVNT
//
//    Rev 1.5   16 Jul 1996 12:24:32   MKEATIN
// SetSequenceNumber to zero if VIRSCAN.DAT is not found
//
//    Rev 1.4   12 Jul 1996 10:00:08   MKEATIN
// Call SetSequenceEx() instead of SetSequence().  Also, added RegCloseKey()
// for each RegOpenKey().
//
//    Rev 1.3   17 Jun 1996 18:29:08   MKEATIN
//
//
//    Rev 1.2   17 Jun 1996 18:26:06   MKEATIN
// The onramp command line now comes from the Language area
//
//    Rev 1.1   17 Jun 1996 16:23:02   MKEATIN
// Added bReboot parameter to CallOnramp().  Reboot decisions should be made
// prior to calling CallOnramp().
//
//    Rev 1.0   17 Jun 1996 15:18:48   MKEATIN
// Initial revision.
//****************************************************************************

// Never define/include anything before this.
#include "StdAfx.h"

// Initialize the NavLu strings in String.h. This must be done in this file.
// If the initialization occurs in any other NAVLU project file, the strings
// are undefined for the NavLuUtils.LIB.
#define BUILDING_NAVLUSTRINGS

#include "platform.h"                   // Platform independence
#include "file.h"
#include "xapi.h"
#include "NavLuUtils.h"
#include "strings.h"

//****************************************************************************
// Quake Replacement functions ***********************************************
//****************************************************************************

//////////////////////////////////////////////////////////////////////////
// Description:
//   This function appends the string specified by lpszAdd to the
//   lpszDest string. If lpszDest does not end in a \, one is automatically
//   added before the append occurs.
//
//   This function should correctly handle the following strings.
//
//      lpszDest (in)    lpszAdd     lpszDest (out)
//      ------------------------------------------------------------------
//      s:               abc         s:abc
//      s:.              abc         s:.\abc
//      s:..             abc         s:..\abc
//      s:dir            abc         s:dec\abc
//
// Parameters:
//   lpszDest     in/out Provides the string to append to. This string must
//                       already be allocated to included the number of
//                       characters from lpszAdd.
//   lpszAdd      in     String to append to lpszDest.
//////////////////////////////////////////////////////////////////////////
// 05/14/97 JBRENNA created.
//////////////////////////////////////////////////////////////////////////
void NavLuNameAppendFile (LPTSTR lpszDest, LPCTSTR lpszAdd)
{
    LPTSTR lpszDestCur;

    ASSERT (lpszDest);
    ASSERT (lpszAdd);

    // Find the next to the last character.
    for (lpszDestCur = lpszDest;
         *lpszDestCur != _T('\0') && *(lpszDestCur+1) != _T('\0');
         lpszDestCur = CharNext (lpszDestCur));

    // If we are not at the end of the string (I.E. lpszDestCur points
    // to the last character in lpszDest) ...
    if (*lpszDestCur != _T('\0'))
    {
        // And if the last character is not a : or \ character, append a \ at
        // the end of the destination string. The : character check ensures
        // we do not add \ to a "s:" destination string.
        if (*lpszDestCur != _T(':') && *lpszDestCur != _T('\\'))
        {
            *(lpszDestCur+1) = _T('\\');
            *(lpszDestCur+2) = _T('\0');

            // Move two characters forward. This skips the last character and
            // the newly appended \ character.
            lpszDestCur += 2;
        }
        // Else, the last character is a \. Increment past it.
        else
            lpszDestCur += 1;
    }

    STRCPY (lpszDestCur, lpszAdd);
}

//////////////////////////////////////////////////////////////////////////////
// Description:
//   Determines if the specified file exists on the system. Note that a
//   directory name does not count as a file name.
//
// Returns:
//   TRUE  - file found.
//   FALSE - file not found.
//////////////////////////////////////////////////////////////////////////////
BOOL NavLuFileExists (LPCTSTR lpszFileName)
{
    DWORD dwFileAttrs;

    // Ensure that file name is not NULL.
    ASSERT (lpszFileName);

    dwFileAttrs = GetFileAttributes (lpszFileName);

    if (0xFFFFFFFF == dwFileAttrs)
        return FALSE;
    else if (FILE_ATTRIBUTE_DIRECTORY & dwFileAttrs)
        return FALSE;
    else
        return TRUE;
}

//****************************************************************************
// External functions ********************************************************
//****************************************************************************

//////////////////////////////////////////////////////////////////////////////
// Description:
//   Determines the path to the S32LIVE1.DLL.
//
// Returns:
//   TRUE  - S32Live1.DLL found.
//   FALSE - S32Live1.DLL not found.
//////////////////////////////////////////////////////////////////////////////
BOOL NavLuGetLuPathName (LPTSTR lpszLuPathName, DWORD dwPathSize)
{
    LPCTSTR  szLuPathKey  = _T("SOFTWARE\\Symantec\\SharedUsage");
    LPCTSTR  szLuQueryKey = _T("LiveUpdate1");
    BOOL     bFoundPath   = FALSE;
    HKEY     hBaseKey;
    DWORD    dwValueType;

    if (ERROR_SUCCESS == RegOpenKeyEx (HKEY_LOCAL_MACHINE, szLuPathKey, 0, KEY_READ, &hBaseKey))
    {
        if (ERROR_SUCCESS == RegQueryValueEx (hBaseKey, szLuQueryKey, 0, &dwValueType, (LPBYTE)lpszLuPathName, &dwPathSize))
        {
            if (dwValueType == REG_SZ && *lpszLuPathName)
            {
                NavLuNameAppendFile (lpszLuPathName, "S32LIVE1.DLL");
                bFoundPath = TRUE;
            }
        }
        RegCloseKey (hBaseKey);
    }

    return bFoundPath;
}

//////////////////////////////////////////////////////////////////////////
// Description:
//   Determine if the Shared Tech LiveUpdate component is on the machine.
//
// Parameters:
//   lpszLuPathName in  If present, provides a file name and path to verify
//                      the installation of NAVLU.  If that file does not
//                      exist then it returns LiveUpdate not on the machine.
// Returns:
//   TRUE   - LiveUpdate is on the machine.
//   FALSE  - LiveUpdate is not on the machine.
//////////////////////////////////////////////////////////////////////////
BOOL NavLuIsLuInstalled (LPTSTR lpszLuPathName)
{
    BOOL   bFoundPath = FALSE;
    TCHAR  szFile[SYM_MAX_PATH+1];

    // If there is a reference file, check to see if it is there.
    if( NULL != lpszLuPathName && !NavLuFileExists(lpszLuPathName) )
        return FALSE;

    bFoundPath = NavLuGetLuPathName (szFile, sizeof(szFile));

    return bFoundPath && NavLuFileExists (szFile);
}

//////////////////////////////////////////////////////////////////////////
// Description:
//   Actually launch the Nav LiveUpdate program.
//
// Returns:
//   TRUE   - success.
//   FALSE  - failure.
//////////////////////////////////////////////////////////////////////////
BOOL NavLuLaunchLiveUpdate (void)
{
    STARTUPINFO         si;
    PROCESS_INFORMATION pi;
    BOOL                bSuccess;
    TCHAR               szCmdLine[_MAX_PATH];

    memset (&si, 0, sizeof(si));
    si.cb = sizeof(si);

    // Copy this into a non-const buffer. CreateProcess takes a non-const
    // command line.
    _tcscpy (szCmdLine, g_szProgramName);

    // Start the child process.
    bSuccess = CreateProcess (NULL,             // Application name
                              szCmdLine,        // Command line to run.
                              NULL,             // Process handle not inheritable.
                              NULL,             // Thread handle not inheritable.
                              FALSE,            // Set handle inheritance to FALSE.
                              0,                // No creation flags.
                              NULL,             // Use parent's environment block.
                              NULL,             // Use parent's starting directory.
                              &si,              // Pointer to STARTUPINFO structure.
                              &pi);             // Pointer to PROCESS_INFORMATION structure.

    // Close handles returned immediately since we aren't using them.
    if ( bSuccess )
    {
        CloseHandle (pi.hProcess);
        CloseHandle (pi.hThread);
    }

    return bSuccess;
}
