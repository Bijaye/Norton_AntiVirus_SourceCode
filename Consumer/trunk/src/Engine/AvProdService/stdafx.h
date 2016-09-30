////////////////////////
//
// PROPRIETARY / CONFIDENTIAL.
// Use of this product is subject to license terms.
// Copyright © 2006 Symantec Corporation.
// All rights reserved.
//
/////////////////////////////////////////////////////////////////////////////////////////////////

// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once


#ifndef VC_EXTRALEAN
#define VC_EXTRALEAN		// Exclude rarely-used stuff from Windows headers
#endif

// Modify the following defines if you have to target a platform prior to the ones specified below.
// Refer to MSDN for the latest info on corresponding values for different platforms.
#ifndef WINVER				// Allow use of features specific to Windows 95 and Windows NT 4 or later.
#define WINVER 0x0400		// Change this to the appropriate value to target Windows 98 and Windows 2000 or later.
#endif

#ifndef _WIN32_WINNT		// Allow use of features specific to Windows NT 4 or later.
#define _WIN32_WINNT 0x0400		// Change this to the appropriate value to target Windows 98 and Windows 2000 or later.
#endif						

#ifndef _WIN32_WINDOWS		// Allow use of features specific to Windows 98 or later.
#define _WIN32_WINDOWS 0x0410 // Change this to the appropriate value to target Windows Me or later.
#endif

#ifndef _WIN32_IE			// Allow use of features specific to IE 4.0 or later.
#define _WIN32_IE 0x0500	// Change this to the appropriate value to target IE 5.0 or later.
#endif

#define _ATL_CSTRING_EXPLICIT_CONSTRUCTORS	// some CString constructors will be explicit

//Used for pragma message outputs.
#undef STRIZE
#define STRIZE(x) #x
#undef PPSTRIZE
#define PPSTRIZE(x) STRIZE(x)
#define AUTO_FUNCNAME __FILE__ L"(" PPSTRIZE(__LINE__) L") : " __FUNCTION__ L":  "

#define _S(x)(CString)(LPCTSTR)(x)
#define _I(x)( _ttoi(((CString)(LPCTSTR)x)) )
#define _countof(x)(sizeof(x)/sizeof(*x))

#include <atlbase.h>
#include <atlapp.h>
#include <atltime.h>

extern CAppModule _Module;

// Helper library for events
//
#include <ccIgnoreWarnings.h>
#include <ccLib.h>
#include <ccEvent.h>
#include <ccMessageLock.h>
#include <ccLibDllLink.h>
#include <ccResourceLoader.h>
extern cc::CResourceLoader g_Resources;

// TRACE helpers
#include <ccTrace.h>
#include <ccSymDebugOutput.h>

// SymInterface
#include <SymInterface.h>

#pragma warning (push)

// get rid of some of the unreachable code warnings...
#pragma warning (disable: 4702)

// StahlSoft
#include <StahlSoft.h>

// STL
#include <list>
#include <set>
#include <map>

#pragma warning(pop)
