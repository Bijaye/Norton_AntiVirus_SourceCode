// stdafx.h : include file for standard system include files,
//  or project specific include files that are used frequently, but
//      are changed infrequently
//

#if !defined(AFX_STDAFX_H__7F794644_7B08_40D6_A3D4_4008E518CEF0__INCLUDED_)
#define AFX_STDAFX_H__7F794644_7B08_40D6_A3D4_4008E518CEF0__INCLUDED_

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
#include "StahlSoft.h"
#include "runnablehostthread.h"
#include "SSCoInitialize.h"
#include "DefAlertOptNames.h"
#include "ccVerifyTrustLibLink.h"
#include "ccTrace.h"
#include "ccLib.h"
#include "ccCoInitialize.h"

extern HMODULE g_hModule;
extern CString _csModuleLocation;
extern CString _csModuleFileName;
extern HINSTANCE _g_hInstance;

#if defined(_DBCS) || defined (_MBCS)
#define _txcountof(szString) (sizeof(szString) / (sizeof(*szString) * 2))
#else // _UNICODE || _SBCS
#define _txcountof(szString) (sizeof(szString) / sizeof(*szString))
#endif

// TODO: reference additional headers your program requires here

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_STDAFX_H__7F794644_7B08_40D6_A3D4_4008E518CEF0__INCLUDED_)
