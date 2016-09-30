// stdafx.h : include file for standard system include files,
//      or project specific include files that are used frequently,
//      but are changed infrequently

#if !defined(AFX_STDAFX_H__7C2F7DEF_E12E_403A_9C22_F1D8501574EC__INCLUDED_)
#define AFX_STDAFX_H__7C2F7DEF_E12E_403A_9C22_F1D8501574EC__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#pragma warning (disable: 4018)
#pragma warning (disable: 4244)
#pragma warning (disable: 4800)

#define STRICT
#ifndef _WIN32_WINNT
#define _WIN32_WINNT 0x0400
#endif
#define _ATL_APARTMENT_THREADED

#include <atlbase.h>
//You may derive a class from CComModule and use it if you want to override
//something, but do not change the name of _Module
extern CComModule _Module;
#include <atlcom.h>
#include <atlwin.h>
#include <atlctl.h>
#include <atlstr.h>
#include <comdef.h>
#include <process.h>
#include <map>
#include <time.h>
#include <shlobj.h>

#include "DefAlertOptNames.h"
#include "ccTrace.h"
#include "stahlsoft.h"
#include "SymAlertStatic.h"
#include "globals.h"
#include "NAVInfo.h"                            
#include "OSInfo.h"                             
#include "navopt32.h"       
#include "OptNames.h"
#include "AVRES.h"

#include "ccLibStd.h"


#define REG_FAILED(rc)      (rc != ERROR_SUCCESS)
#define REG_SUCCEEDED(rc)   (rc == ERROR_SUCCESS)

#define NAVTRUST_FAILED(rc)     (rc != NAVTRUST_OK)
#define NAVTRUST_SUCCEEDED(rc)  (rc == NAVTRUST_OK)

extern CString _g_csHandlerPath;
//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_STDAFX_H__7C2F7DEF_E12E_403A_9C22_F1D8501574EC__INCLUDED)
