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
#define _WIN32_WINNT 0x0400	// Change this to the appropriate value to target Windows 2000 or later.
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

// turns off ATL's hiding of some common and often safely ignored warning messages
#define _ATL_ALL_WARNINGS

//Used for pragma message outputs.
#undef STRIZE
#define STRIZE(x) #x
#undef PPSTRIZE
#define PPSTRIZE(x) STRIZE(x)
#define AUTO_FUNCNAME __FILE__ "(" PPSTRIZE(__LINE__) ") : " __FUNCTION__ ":  "

#define _S(x)(CString)(LPCTSTR)(x)
#define _I(x)( _ttoi(((CString)(LPCTSTR)x)) )
#define _countof(x)(sizeof(x)/sizeof(*x))

#define HPP_REGISTRY_EVENT_ID  3501
#define HPP_SYSTEM_EVENT_ID  3502

#include <new>
#include "resource.h"
#include <atlbase.h>
#include <atlcom.h>
#include <atlstr.h>

#include "ccLibStd.h"
#include "ccError.h"
#include "ccLibDllLink.h"
#include "ccResourceLoader.h"
extern cc::CResourceLoader g_Resources;

// HPP Resources
#include "HPPRes.h"

using namespace ATL;
using namespace std;