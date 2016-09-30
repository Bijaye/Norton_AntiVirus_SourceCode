// stdafx.h : include file for standard system include files,
//      or project specific include files that are used frequently,
//      but are changed infrequently

#if !defined(AFX_STDAFX_H__0DC38ED8_FBE8_4894_806E_5308344E7F3E__INCLUDED_)
#define AFX_STDAFX_H__0DC38ED8_FBE8_4894_806E_5308344E7F3E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define STRICT
#define _ATL_APARTMENT_THREADED

// Uncomment these lines for interface debugging. Serious performance slowdown
//
//#ifdef _DEBUG   // For debugging ref counts!
//    #define _ATL_DEBUG_INTERFACES
//#endif

#include <atlbase.h>
#include <atlapp.h>
#include <atlstr.h>

extern CAppModule _Module;

#include <atlcom.h>
#include <atlwin.h>
#include <atlhost.h>
#include <atlddx.h>
#include <atltypes.h>
#include <time.h>
#include <atlctl.h>
#include <atlctrls.h>
#include <atlframe.h>
#include <commctrl.h>

#include <mstask.h> // MS Task Scheduler

#include "ccLibstd.h"
#include "ccLib.h"
#include "SymHelp.h"

/*
** Name of the Modules Resource DLL
*/ 

#define SYMC_RESOURCE_DLL		_T("NAVTasks.loc")

// Toolbox headers
//
#include "NAVInfo.h"
#include "Shlobj.h"
#include "StahlSoft.h"

#include "DebugLog.h"

// Global variables

// Only need one of these objects, ever.
//
extern CNAVInfo g_NAVInfo;
extern CDebugLog g_Log;

#define SIZEOF( x ) (sizeof(x)/sizeof(*x))

//Used for pragma message outputs.
#undef STRIZE
#define STRIZE(x) #x
#undef PPSTRIZE
#define PPSTRIZE(x) STRIZE(x)
#define AUTO_FUNCNAME __FILE__ "(" PPSTRIZE(__LINE__) ") : " __FUNCTION__ ":  "

#ifndef _countof
#define _countof(x)(sizeof(x)/sizeof(*x))
#endif

#ifndef _S
#define _S(x)((CString)(LPCTSTR)x)
#endif

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_STDAFX_H__0DC38ED8_FBE8_4894_806E_5308344E7F3E__INCLUDED)
