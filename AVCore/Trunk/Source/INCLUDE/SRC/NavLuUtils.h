// Copyright 1996 Symantec, Peter Norton Product Group
//************************************************************************
//
// $Header:   S:/INCLUDE/VCS/NavLuUtils.h_v   1.3   24 Nov 1997 14:04:00   jtaylor  $
//
// Description: Prototypes for NAVOR32.DLL's export functions
//
// Contains:
//
// See Also:
//************************************************************************
// $Log:   S:/INCLUDE/VCS/NavLuUtils.h_v  $
// 
//    Rev 1.3   24 Nov 1997 14:04:00   jtaylor
// Added an optional filename parameter to NavLuIsLuInstalled.  If a filename is specified it will not return TRUE unless that file exists.
//
//    Rev 1.2   03 Jun 1997 14:01:30   JBRENNA
// Add NavLuLaunchLiveUpdate prototype.
//
//    Rev 1.1   02 Jun 1997 18:24:26   JBRENNA
// Added prototypes for NavLuNameAppendFile, NavLuFileExists,
// NavLuGetLuPathName, and NavLuIsLuInstalled.
//
//    Rev 1.0   10 May 1997 00:27:08   JBRENNA
// Initial revision.
//
//    Rev 1.1   17 Jun 1996 16:21:32   MKEATIN
// Added BOOL bReboot parameter to CallOnramp() prototype
//
//    Rev 1.0   17 Jun 1996 15:39:42   MKEATIN
// Initial revision.
//************************************************************************

#ifndef __NAVLUUTILS_H
#define __NAVLUUTILS_H

/* CORE replacement functions */

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
void NavLuNameAppendFile (LPTSTR  lpszDest, LPCTSTR lpszAdd);

//////////////////////////////////////////////////////////////////////////////
// Description:
//   Determines if the specified file exists on the system. Note that a
//   directory name does not count as a file name.
//
// Returns:
//   TRUE  - file found.
//   FALSE - file not found.
//////////////////////////////////////////////////////////////////////////////
BOOL NavLuFileExists     (LPCTSTR lpszFileName);


/* NavLu Utility functions */

//////////////////////////////////////////////////////////////////////////////
// Description:
//   Determines the path to the S32LIVE1.DLL.
//
// Returns:
//   TRUE  - S32Live1.DLL found.
//   FALSE - S32Live1.DLL not found.
//////////////////////////////////////////////////////////////////////////////
BOOL NavLuGetLuPathName (LPTSTR lpszLuPathName, DWORD dwPathSize);

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
BOOL NavLuIsLuInstalled (LPTSTR lpszLuPathName = NULL);

//////////////////////////////////////////////////////////////////////////
// Description:
//   Actually launch the Nav LiveUpdate program.
//
// Returns:
//   TRUE   - success.
//   FALSE  - failure.
//////////////////////////////////////////////////////////////////////////
BOOL NavLuLaunchLiveUpdate (void);

#endif // __NAVLUUTILS_H