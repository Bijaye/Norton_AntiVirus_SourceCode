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
#include <atlstr.h>
#include <comdef.h>
#include <time.h>
#include <fstream>
#include <shellapi.h>
#include "StahlSoft.h"
#if !defined(_NO_CC)
#include "ccLib.h"
#endif //#if !defined(_NO_CC)
// TODO: reference additional headers your program requires here

extern HMODULE g_hModule;
extern CString g_sModuleLocation;
extern CString g_sModuleFileName;

#define REG_SUCCEEDED(rc)   (rc == ERROR_SUCCESS)
#define REG_FAILED(rc)      (rc != ERROR_SUCCESS)

#define _txcountof(array) (sizeof(array)/sizeof(array[0]))

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_STDAFX_H__7C86C480_F613_4E22_A29F_BB5D90D50B0B__INCLUDED_)
