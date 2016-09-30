////////////////////////
//
// PROPRIETARY / CONFIDENTIAL.
// Use of this product is subject to license terms.
// Copyright © 2006 Symantec Corporation.
// All rights reserved.
//
/////////////////////////////////////////////////////////////////////////////////////////////////

// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently,
// but are changed infrequently

#pragma once

#if !defined(AFX_STDAFX_H__AC31C20B_45D0_4C59_8C59_624E15E9E019__INCLUDED_)
#define AFX_STDAFX_H__AC31C20B_45D0_4C59_8C59_624E15E9E019__INCLUDED_

#ifndef STRICT
#define STRICT
#endif

// Modify the following defines if you have to target a platform prior to the ones specified below.
// Refer to MSDN for the latest info on corresponding values for different platforms.
#ifndef WINVER				// Allow use of features specific to Windows 95 and Windows NT 4 or later.
#define WINVER 0x0400		// Change this to the appropriate value to target Windows 98 and Windows 2000 or later.
#endif

#ifndef _WIN32_WINNT		// Allow use of features specific to Windows XP or later.
#define _WIN32_WINNT 0x0501	// Change this to the appropriate value to target Windows XP or later.
#endif						

#ifndef _WIN32_WINDOWS		// Allow use of features specific to Windows 98 or later.
#define _WIN32_WINDOWS 0x0410 // Change this to the appropriate value to target Windows Me or later.
#endif

#ifndef _WIN32_IE			// Allow use of features specific to IE 6.0 or later.
#define _WIN32_IE 0x0600
#endif

#define _ATL_APARTMENT_THREADED
#define _ATL_NO_AUTOMATIC_NAMESPACE

#define _ATL_CSTRING_EXPLICIT_CONSTRUCTORS	// some CString constructors will be explicit


#include "resource.h"

#include <atlbase.h>
//You may derive a class from CComModule and use it if you want to override
//something, but do not change the name of _Module

#include <atlcom.h>
using namespace ATL;

#include <shlobj.h>
#include <comdef.h>
// STL headers
#include <vector>

#define SIZEOF(x) (sizeof(x) / sizeof(x[0]))

#include "ccOSInfo.h"
#include "NAVInfo.h"
#include "StahlSoft.h"
#include "NAVtstring.h"

#include "ccLib.h"

typedef ::std::basic_string<TCHAR> TSTRING;
typedef ::std::basic_string<WCHAR> WSTRING;

#endif // !defined(AFX_STDAFX_H__AC31C20B_45D0_4C59_8C59_624E15E9E019__INCLUDED)