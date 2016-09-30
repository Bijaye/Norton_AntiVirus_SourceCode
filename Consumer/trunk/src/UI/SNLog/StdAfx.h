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

#if !defined(AFX_STDAFX_H__0DC522A1_6303_41DE_9CD1_E2455D9413CE__INCLUDED_)
#define AFX_STDAFX_H__0DC522A1_6303_41DE_9CD1_E2455D9413CE__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define WIN32_LEAN_AND_MEAN
#define _WIN32_WINNT 0x0400
#define _ATL_APARTMENT_THREADED

#include "ccIgnoreWarnings.h"

#include <atlbase.h>

#include <atlcom.h>
#include <comdef.h>
#include <commctrl.h>
#include <string>

typedef ::std::basic_string<TCHAR> tstring;

#include "ccLib.h"
#include "ccSingleLock.h"
#include "ccLibDllLink.h"

#include "ccExceptionInfo.h"

/*
** Name of the Modules Resource DLL
*/ 

#define SYMC_RESOURCE_DLL		_T("IWPLog.loc")

extern HINSTANCE g_hInst;
//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_STDAFX_H__0DC522A1_6303_41DE_9CD1_E2455D9413CE__INCLUDED)


