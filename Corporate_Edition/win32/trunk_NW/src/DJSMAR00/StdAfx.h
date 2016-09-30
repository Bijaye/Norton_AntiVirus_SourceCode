// stdafx.h : include file for standard system include files,
//  or project specific include files that are used frequently, but
//      are changed infrequently
//

#if !defined(AFX_STDAFX_H__7C86C480_F613_4E22_A29F_BB5D90D50B0B__INCLUDED_)
#define AFX_STDAFX_H__7C86C480_F613_4E22_A29F_BB5D90D50B0B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef _WIN32_WINNT		// Allow use of features specific to Windows NT 4 or later.
#define _WIN32_WINNT 0x0400	// Change this to the appropriate value to target Windows 2000 or later.
#endif						

// Insert your headers here
#define WIN32_LEAN_AND_MEAN		// Exclude rarely-used stuff from Windows headers
#include <atlbase.h>
extern CComModule _Module;
#include <atlstr.h>
#include <comdef.h>
#include <time.h>
#include <fstream>
#include "StahlSoft.h"
// TODO: reference additional headers your program requires here

extern HMODULE _g_hModule;
extern CString _csModuleLocation;
extern CString _csModuleFileName;

#define REG_SUCCEEDED(rc)   (rc == ERROR_SUCCESS)
#define REG_FAILED(rc)      (rc != ERROR_SUCCESS)
#define _TRACE_LOG
void TraceLog(LPCTSTR lpcInfo,LPCTSTR lpszFormat, ...);
//#define _LIVE_REG

#if defined(_TRACE_LOG)
#define SUPER_LOG_SEVERITY_INFO			"info"
#define SUPER_LOG_SEVERITY_WARNING		"warning"
#define SUPER_LOG_SEVERITY_ERROR		"error"
#define SUPER_LOG_SEVERITY_ERRORX		"exception" //exception error (Use it or leave it)
//#define infox __LINE__, SUPER_LOG_SEVERITY_INFO, __FILE__, __TIMESTAMP__, __DATE__, __FUNCDNAME__
#define warnx __LINE__, SUPER_LOG_SEVERITY_WARNING, __FILE__, __TIMESTAMP__, __DATE__, __FUNCDNAME__
#define err_err __LINE__, SUPER_LOG_SEVERITY_ERROR, __FILE__, __TIMESTAMP__, __DATE__, __FUNCDNAME__
#define errxx __LINE__, SUPER_LOG_SEVERITY_ERRORX, __FILE__, __TIMESTAMP__, __DATE__, __FUNCDNAME__

#define infox __LINE__,__FUNCDNAME__

#define _info ((LPCTSTR)infox)
#define _errx ((LPCTSTR)errxx)
#define _warn ((LPCTSTR)warnx)
#define _err ((LPCTSTR)err_err)
#endif

#if defined(_DBCS) || defined (_MBCS)
#define _txcountof(szString) (sizeof(szString) / (sizeof(*szString) * 2))
#else // _UNICODE || _SBCS
#define __txcountof(szString) (sizeof(szString) / sizeof(*szString))
#endif

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_STDAFX_H__7C86C480_F613_4E22_A29F_BB5D90D50B0B__INCLUDED_)
