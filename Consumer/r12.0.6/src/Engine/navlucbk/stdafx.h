// stdafx.h : include file for standard system include files,
//      or project specific include files that are used frequently,
//      but are changed infrequently

#if !defined(AFX_STDAFX_H__3C2E74A4_887E_11D2_B40A_00600831DD76__INCLUDED_)
#define AFX_STDAFX_H__3C2E74A4_887E_11D2_B40A_00600831DD76__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define STRICT
#ifndef _WIN32_WINNT
#define _WIN32_WINNT 0x0400
#endif
#define _ATL_APARTMENT_THREADED

#include <afxwin.h>                     // MFC core and standard components
#include <afxdisp.h>
#include <afxext.h>                     // MFC extensions
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>                     // MFC support for Windows Common Controls
#endif // _AFX_NO_AFXCMN_SUPPORT

#include <atlbase.h>
//You may derive a class from CComModule and use it if you want to override
//something, but do not change the name of _Module
extern CComModule _Module;
#include <atlcom.h>

#include <stdio.h>

#include "ccLibStd.h"
#include "Globals.h"
#include "StahlSoft.h"
#include "ccVerifyTrustLibLink.h"
extern CString _g_csHandlerPath;

bool	_InitResources();
#define INIT_RESOURCES() \
	if( false == _InitResources() )\
	return SYMERR_INVALID_FILE;\

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_STDAFX_H__3C2E74A4_887E_11D2_B40A_00600831DD76__INCLUDED)
