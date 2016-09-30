// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright 1996 - 2003, 2005 Symantec Corporation. All rights reserved.
//****************************************************************************
//
// $Header:   S:/NAVLU/VCS/navluutils.cpv   1.8.1.0   22 Jun 1998 19:51:26   RFULLER  $
//
// Description:
//   Integrates LiveUpdate into NAV. This file was moved from the ONRAMP
//   project to here.
//
// Contains:
//
// See Also:
//****************************************************************************
// $Log:   S:/NAVLU/VCS/navluutils.cpv  $
// 
// DALLEE - Taken from AVCORE NAVLU:
//
//    Rev 1.8.1.0   22 Jun 1998 19:51:26   RFULLER
// Branch base for version QAKN
// 
//    Rev 1.8   24 Nov 1997 14:03:54   jtaylor
// Added an optional filename parameter to NavLuIsLuInstalled.  If a filename is specified it will not return TRUE unless that file exists.
//****************************************************************************

// Never define/include anything before this.
#include "StdAfx.h"

// Initialize the NavLu strings in String.h. This must be done in this file.
// If the initialization occurs in any other NAVLU project file, the strings
// are undefined for the NavLuUtils.LIB.
#define BUILDING_NAVLUSTRINGS

#include "NavLuUtils.h"
#include "strings.h"
#include "SymSaferRegistry.h"
#include "SymSaferStrings.h"

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

    _tcscpy (lpszDestCur, lpszAdd);
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
        if (ERROR_SUCCESS == SymSaferRegQueryValueEx (hBaseKey, szLuQueryKey, 0, &dwValueType, (LPBYTE)lpszLuPathName, &dwPathSize))
        {
            if (dwValueType == REG_SZ && *lpszLuPathName)
            {
                NavLuNameAppendFile (lpszLuPathName, _T("S32LIVE1.DLL"));
                bFoundPath = TRUE;
            }
        }
        RegCloseKey (hBaseKey);
    }

    return bFoundPath;
}

