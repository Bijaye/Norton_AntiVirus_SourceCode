// stdafx.h : include file for standard system include files,
//  or project specific include files that are used frequently, but
//      are changed infrequently
//

#if !defined(AFX_STDAFX_H__5D741795_2499_45E4_ADB1_F9FA8BB53C08__INCLUDED_)
#define AFX_STDAFX_H__5D741795_2499_45E4_ADB1_F9FA8BB53C08__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

// This is needed to get access to ReadDirectoryChangesW() in WinBase.h
#ifndef _WIN32_WINNT
#define _WIN32_WINNT 0x0501
#endif // _WIN32_WINNT

// Insert your headers here
#define WIN32_LEAN_AND_MEAN		// Exclude rarely-used stuff from Windows headers

#define _CRT_SECURE_CPP_OVERLOAD_STANDARD_NAMES 1

#include <windows.h>

// TODO: reference additional headers your program requires here
// This is needed since opstr.h references "ostream" with out the namespace
// specification (std::ostream)
#include <iostream>
using std::ostream;

// Taken from snderror.h
// Actually referencing snderror.h instead of copying these got too messy.
#define OK 0

#include "ccLibDllLink.h"

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_STDAFX_H__5D741795_2499_45E4_ADB1_F9FA8BB53C08__INCLUDED_)
