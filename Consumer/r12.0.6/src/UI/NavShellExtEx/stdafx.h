// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently,
// but are changed infrequently

#pragma once

#ifndef STRICT
#define STRICT
#endif

// Modify the following defines if you have to target a platform prior to the ones specified below.
// Refer to MSDN for the latest info on corresponding values for different platforms.
#ifndef WINVER				// Allow use of features specific to Windows 95 and Windows NT 4 or later.
#define WINVER 0x0400		// Change this to the appropriate value to target Windows 98 and Windows 2000 or later.
#endif

#ifndef _WIN32_WINNT		// Allow use of features specific to Windows NT 4 or later.
#define _WIN32_WINNT 0x0500	// Change this to the appropriate value to target Windows 2000 or later.
#endif						

#ifndef _WIN32_WINDOWS		// Allow use of features specific to Windows 98 or later.
#define _WIN32_WINDOWS 0x0410 // Change this to the appropriate value to target Windows Me or later.
#endif

#ifndef _WIN32_IE			// Allow use of features specific to IE 4.0 or later.
#define _WIN32_IE 0x0400	// Change this to the appropriate value to target IE 5.0 or later.
#endif

#define _ATL_APARTMENT_THREADED
#define _ATL_NO_AUTOMATIC_NAMESPACE

#define _ATL_CSTRING_EXPLICIT_CONSTRUCTORS	// some CString constructors will be explicit

#define _WIN32_WINNT 0x0502

#include "resource.h"
#include <atlbase.h>
#include <atlcom.h>
using namespace ATL;
#include <shlguid.h>
DEFINE_GUID(IID_IInputObject,           0x68284faa, 0x6a48, 0x11d0, 0x8c, 0x78, 0x0, 0xc0, 0x4f, 0xd9, 0x18, 0xb4);
DEFINE_SHLGUID(IID_IContextMenu,        0x000214E4L, 0, 0);
#include <shlobj.h>
#include <comdef.h>
#include <commctrl.h>

// STL headers
#include <vector>
#define SIZEOF(x) (sizeof(x) / sizeof(x[0]))
typedef ::std::basic_string<TCHAR> TSTRING;
typedef ::std::basic_string<WCHAR> WSTRING;


#include "util.h"
#include "DebugLog.h"
extern CDebugLog g_Log;
#define	MAX_PATH	256
extern class CNavShellExtExModule _AtlModule;
