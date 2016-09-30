// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright (c) 2005 Symantec Corporation. All rights reserved.
// stdafx.h : include file for standard system include files,
//  or project specific include files that are used frequently, but
//      are changed infrequently
//

#if !defined(AFX_STDAFX_H__031077D7_FB09_4818_B1F7_B999AFF45B50__INCLUDED_)
#define AFX_STDAFX_H__031077D7_FB09_4818_B1F7_B999AFF45B50__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


// Insert your headers here
#define WIN32_LEAN_AND_MEAN		// Exclude rarely-used stuff from Windows headers

#define _CRT_SECURE_CPP_OVERLOAD_STANDARD_NAMES 1

#include <windows.h>
#include <tchar.h>

// Warning 4786 apears often when using the C++ Library, so turn it off.
// ("identifier was truncated to '255' characters in the browser information")
#pragma warning(disable : 4786)

#include <vector>
#include <string>

using namespace std;

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_STDAFX_H__031077D7_FB09_4818_B1F7_B999AFF45B50__INCLUDED_)
