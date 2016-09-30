////////////////////////
//
// PROPRIETARY / CONFIDENTIAL.
// Use of this product is subject to license terms.
// Copyright © 2006 Symantec Corporation.
// All rights reserved.
//
/////////////////////////////////////////////////////////////////////////////////////////////////

// stdafx.h : include file for standard system include files,
//  or project specific include files that are used frequently, but
//      are changed infrequently
//

#if !defined(AFX_STDAFX_H__A9DB83DB_A9FD_11D0_BFD1_444553540000__INCLUDED_)
#define AFX_STDAFX_H__A9DB83DB_A9FD_11D0_BFD1_444553540000__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define WIN32_LEAN_AND_MEAN		// Exclude rarely-used stuff from Windows headers

#include <windows.h>
#include <tchar.h>
#include <stdio.h>
#include <stdlib.h>
#include <atlbase.h>

// TODO: reference additional headers your program requires here
#include "resource.h"
#include "OSInfo.h"
#include "NAVInfo.h"
#include "CommonClientInfo.h"
#include "Logging.h"
#include "DriverCheck.h"
#include "Symevent.h"
#include "Engines.h"
#include "Savrt.h"
#include "APChecks.h"
#include "SymScriptSafe.h"
#include "NAVAPSCR.h"

extern HINSTANCE g_hInstance;
extern CLog g_log;
extern COSInfo g_OSInfo;
extern CNAVInfo g_NAVInfo;
extern CCommonClientInfo g_CCInfo;
extern bool g_bReboot;
extern bool g_bRestartNavapsvc;

extern void ShowErrorMsg(UINT uID);

// Common functions (defined in shared.cpp)
bool FileExistsAndSigned(LPTSTR);
bool RegisterComDll(LPTSTR);
bool GetFileVersion(LPTSTR lpszFile, LPTSTR lpszBuf, DWORD dwBufSize);

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_STDAFX_H__A9DB83DB_A9FD_11D0_BFD1_444553540000__INCLUDED_)
