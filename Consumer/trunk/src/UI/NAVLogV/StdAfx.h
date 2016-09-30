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

#pragma once

// Insert your headers here
#define WIN32_LEAN_AND_MEAN		// Exclude rarely-used stuff from Windows headers

// Change these values to use different versions
#define _WIN32_WINNT    0x501           // Windows XP
#define WINVER		0x0500
#define _WIN32_IE	0x0400
#define _RICHEDIT_VER	0x0100

//Used for pragma message outputs.
#undef STRIZE
#define STRIZE(x) #x
#undef PPSTRIZE
#define PPSTRIZE(x) STRIZE(x)
#define AUTO_FUNCNAME __FILE__ _T("(") PPSTRIZE(__LINE__) _T(") : ") __FUNCTION__ _T(":  ")

#define _S(x)(CString)(LPCTSTR)(x)
#define _I(x)( _ttoi(((CString)(LPCTSTR)x)) )
#define _countof(x)(sizeof(x)/sizeof(*x))

#include <atlbase.h>
#include <atlapp.h>
#include <atlcore.h>

extern CAppModule _Module;

#define _WTL_NO_CSTRING
#include <atlmisc.h>

#include <atlwin.h>

#include <atlframe.h>
#include <atlctrls.h>
#include <atldlgs.h>
#include <atltime.h>

// Helper library for events
//
#include "ccIgnoreWarnings.h"
#include "ccLib.h"
#include "ccLibDllLink.h"
#include "ccResourceLoader.h"
extern cc::CResourceLoader g_Resources;

// TRACE helpers
#include "ccTrace.h"
#include "ccSymDebugOutput.h"

// SymInterface
#include "SymInterface.h"

// StahlSoft
#include "StahlSoft.h"

// COM utils
#include <comdef.h>
#include <commctrl.h>

////////////////////////////////////////////////////////////////////// 
// disable warning C4786: symbol greater than 255 character,
// okay to ignore, MSVC chokes on STL templates!
#pragma warning(disable:4786)

////////////////////////////////////////////////////////////////////// 
// These are intentionally annoying, suppressing for now
#ifdef _DEBUG
#pragma warning(disable:4996)
#endif // _DEBUG

extern bool g_bIsAdmin;         // Is the current user an admin
extern bool g_bRawMode;

// The SymHelp.h has a ton of overlapping defines
//
#pragma warning(push)
#pragma warning(disable:4005)
#include "SymHelp.h"
#pragma warning(pop) // reset to original pragma's

#include "SymHelpLauncher.h"
#include "ResourceHelper.h"
#include "NAVtstring.h"

