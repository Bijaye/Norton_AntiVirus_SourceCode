// stdafx.h : include file for standard system include files,
//  or project specific include files that are used frequently, but
//      are changed infrequently
//

#if !defined(AFX_STDAFX_H__A1905215_D400_48AE_9186_9134D9B9ED39__INCLUDED_)
#define AFX_STDAFX_H__A1905215_D400_48AE_9186_9134D9B9ED39__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


// Insert your headers here
#define WIN32_LEAN_AND_MEAN		// Exclude rarely-used stuff from Windows headers
#define _WIN32_WINNT 0x0400
#include <atlbase.h>
extern CComModule _Module;
#include <atlstr.h>
#include <atlcom.h>
#include <atlctl.h>

#include <comdef.h>
#include <time.h>
#include <fstream>

#include "stahlsoft.h"
#include "ccTrace.h"					// in StdAfx.h, so everyone can call CCTRACE*

extern HMODULE _g_hModule;
extern CString _csModuleLocation;
extern CString _csModuleFileName;
//extern CDebugLog _g_Log;

#define REG_SUCCEEDED(rc)	(rc == ERROR_SUCCESS)
#define REG_FAILED(rc)		(rc != ERROR_SUCCESS)
#define _countof(x)(sizeof(x)/sizeof(*x)) 

#define infox __LINE__,__FUNCDNAME__
#define _info ((LPCTSTR)infox)

// TODO: reference additional headers your program requires here

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_STDAFX_H__A1905215_D400_48AE_9186_9134D9B9ED39__INCLUDED_)
