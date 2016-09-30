////////////////////////
//
// PROPRIETARY / CONFIDENTIAL.
// Use of this product is subject to license terms.
// Copyright © 2006 Symantec Corporation.
// All rights reserved.
//
/////////////////////////////////////////////////////////////////////////////////////////////////

// stdafx.h : include file for standard system include files,
//      or project specific include files that are used frequently,
//      but are changed infrequently

#if !defined(AFX_STDAFX_H__7D2C5EC4_1F10_4E76_B242_EEFD540BAC32__INCLUDED_)
#define AFX_STDAFX_H__7D2C5EC4_1F10_4E76_B242_EEFD540BAC32__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define _ATL_APARTMENT_THREADED

// TODO: This is a kludge. remove it ASAP
#define _MS_STOCK_PROP_PAGES_H_

#include <atlbase.h>
#include <atlstr.h>
//You may derive a class from CComModule and use it if you want to override
//something, but do not change the name of _Module
extern CComModule _Module;
extern HMODULE g_ResModule;
#include <atlcom.h>
#include <comdef.h>

#include <shlobj.h>


// STL headers
#include <vector>
#include "navtstring.h"

#define SIZEOF( x ) (sizeof(x) / sizeof(*x))

#include "OSInfo.h"
#include "NAVInfo.h"
#include "ccSymCommonClientInfo.h"

// Option names header
#include "OptNames.h"

#include "AutoUpdateDefines.h"  // ALU property names

#include "isErrorResource.h" // error resource ids

#include "ccLib.h"
#include "ccEvent.h"
#include "ccTrace.h"

extern COSInfo  g_OSInfo;
extern CNAVInfo g_NAVInfo;
extern ccSym::CCommonClientInfo g_CCInfo;

extern bool ShowScriptErrors(void);
extern HRESULT Error(const TCHAR* pszClass, UINT uiIDS, const TCHAR* pszLocation);
extern HRESULT Error(const TCHAR* pszClass, const TCHAR* pszLocation, HRESULT hr);

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_STDAFX_H__7D2C5EC4_1F10_4E76_B242_EEFD540BAC32__INCLUDED)
