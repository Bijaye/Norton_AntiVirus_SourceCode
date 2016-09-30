// stdafx.h : include file for standard system include files,
//      or project specific include files that are used frequently,
//      but are changed infrequently

#if !defined(AFX_STDAFX_H__95D5AAE3_3545_11D3_A1F9_00805F4B2398__INCLUDED_)
#define AFX_STDAFX_H__95D5AAE3_3545_11D3_A1F9_00805F4B2398__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define STRICT
#ifndef _WIN32_WINNT
#define _WIN32_WINNT 0x0400
#endif

#define _ATL_APARTMENT_THREADED
#define _ATL_NO_UUIDOF

#include <atlbase.h>
//You may derive a class from CComModule and use it if you want to override
//something, but do not change the name of _Module
extern CComModule _Module;

#include <atlcom.h>
#include <shlobj.h>
#include <stdexcept>    // For exceptions


#include "aboutplg.h"

#include "StahlSoft.h"

#include "ccLibStd.h"
extern CString _g_csHandlerPath;

//extern PLUGINABOUTSTRUCT* g_pAboutInfo;

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_STDAFX_H__95D5AAE3_3545_11D3_A1F9_00805F4B2398__INCLUDED)
