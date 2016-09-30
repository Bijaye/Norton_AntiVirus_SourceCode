// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

#define WIN32_LEAN_AND_MEAN		// Exclude rarely-used stuff from Windows headers
// Windows Header Files:
#define _WIN32_WINNT 0x0400
#include <atlbase.h>
extern CComModule _Module;
#include <atlstr.h>
#include <comdef.h>
#include "stahlsoft.h"
#include "StahlSoft_EventHelper.h"

#include "ccLibDllLink.h"
#include "ccLibStd.h"

extern HMODULE	 g_hModule;
extern CString	g_sModuleLocation;
extern CString	g_sModuleFileName;
#if defined(_DBCS) || defined (_MBCS)
#define _txcountof(szString) (sizeof(szString) / (sizeof(*szString) * 2))
#else // _UNICODE || _SBCS
#define _txcountof(szString) (sizeof(szString) / sizeof(*szString))
#endif

// TODO: reference additional headers your program requires here
