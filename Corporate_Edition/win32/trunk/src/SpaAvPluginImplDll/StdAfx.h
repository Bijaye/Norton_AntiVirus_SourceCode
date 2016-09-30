// stdafx.h : include file for standard system include files,
//  or project specific include files that are used frequently, but
//      are changed infrequently
//

#if !defined(AFX_STDAFX_H__5D741795_2499_45E4_ADB1_F9FA8BB53C08__INCLUDED_)
#define AFX_STDAFX_H__5D741795_2499_45E4_ADB1_F9FA8BB53C08__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

// Platform SDK Configuration
#ifndef WINVER
#define WINVER              0x0501
#endif
#ifndef _WIN32_WINNT
#define _WIN32_WINNT        0x0501
#endif
#ifndef _WIN32_WINDOWS
#define _WIN32_WINDOWS      0x0500
#endif
#ifndef _WIN32_IE
#define _WIN32_IE           0x0600
#endif

// Configure ATL, Win32SDK, CRT
#define _ATL_APARTMENT_THREADED
#define _ATL_NO_AUTOMATIC_NAMESPACE
#define _ATL_CSTRING_EXPLICIT_CONSTRUCTORS	// some CString constructors will be explicit
#define WIN32_LEAN_AND_MEAN		// Exclude rarely-used stuff from Windows headers
#ifndef _CRT_SECURE_CPP_OVERLOAD_STANDARD_NAMES
    #define _CRT_SECURE_CPP_OVERLOAD_STANDARD_NAMES 1
#endif

// TODO: reference additional headers your program requires here
// This is needed since opstr.h references "ostream" with out the namespace
// specification (std::ostream)
#include <windows.h>
#include <iostream>
using std::ostream;
#include "ccLibDllLink.h"
#include "ccLib.h"

#include <atlbase.h>
#include <atlcom.h>

// Taken from snderror.h
// Actually referencing snderror.h instead of copying these got too messy.
#define OK 0
using namespace ATL;

#endif // !defined(AFX_STDAFX_H__5D741795_2499_45E4_ADB1_F9FA8BB53C08__INCLUDED_)
