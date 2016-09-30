// stdafx.h : include file for standard system include files,
//  or project specific include files that are used frequently, but
//      are changed infrequently
//

#if !defined(AFX_STDAFX_H__CC33D112_B10B_44C6_BA68_54D5183CE404__INCLUDED_)
#define AFX_STDAFX_H__CC33D112_B10B_44C6_BA68_54D5183CE404__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


// Insert your headers here
#define WIN32_LEAN_AND_MEAN		// Exclude rarely-used stuff from Windows headers
#define _WIN32_WINNT 0x0400

#include <windows.h>

#include <atlbase.h>
#include <atlstr.h>
extern CAtlBaseModule _Module;
#include <atlcom.h>

#include <comdef.h>
#include <commctrl.h>

////////////////////////////////////////////////////////////////////// 
// disable warning C4786: symbol greater than 255 character,
// okay to ignore, MSVC chokes on STL templates!
#pragma warning(disable:4786)

#include "StahlSoft.h"

// For EventManager
//
#include "ccLibStd.h"

extern bool g_bIsAdmin;         // Is the current user an admin
extern bool g_bRawMode;

// The SymHelp.h has a ton of overlapping defines
//
#pragma warning(push)
#pragma warning(disable:4005)
#include "SymHelp.h"
#pragma warning(pop) // reset to original pragma's

#include "NAVHelpLauncher.h"
#include "ResourceHelper.h"

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_STDAFX_H__CC33D112_B10B_44C6_BA68_54D5183CE404__INCLUDED_)
