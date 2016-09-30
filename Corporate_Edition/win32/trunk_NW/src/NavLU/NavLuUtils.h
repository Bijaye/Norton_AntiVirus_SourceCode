// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright 1996 - 2003, 2005 Symantec Corporation. All rights reserved.
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

#endif // __NAVLUUTILS_H
