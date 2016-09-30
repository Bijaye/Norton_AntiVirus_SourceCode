// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently,
// but are changed infrequently

#pragma once

#ifndef STRICT
#define STRICT
#endif

// Platform SDK Configuration
#ifndef WINVER
#define WINVER              0x0501
#endif
#ifndef _WIN32_WINNT
#define _WIN32_WINNT        0x0501
#endif
#ifndef _WIN32_WINDOWS
#define _WIN32_WINDOWS      0x0500
#endif
#ifndef _WIN32_IE
#define _WIN32_IE           0x0600
#endif

// Configure ATL
#define _ATL_APARTMENT_THREADED
#define _ATL_NO_AUTOMATIC_NAMESPACE
#define _ATL_CSTRING_EXPLICIT_CONSTRUCTORS	// some CString constructors will be explicit

// ATL includes
#include <afxwin.h>
#include <afxext.h>
#include <afxcmn.h>
#ifndef _AFX_NO_OLE_SUPPORT
    #include <afxdisp.h>        // MFC Automation classes
#endif // _AFX_NO_OLE_SUPPORT
#include "resource.h"
#include <atlbase.h>
#include <atlcom.h>
#include "ccLibDllLink.h"
#include "ccLib.h"

using namespace ATL;

// Official Symantec colors
#define COLOR_SYMANTEC_YELLOW       RGB(255,204,0)
#define COLOR_NAVBAR_BACKGROUND     RGB(95, 107, 141)
#define COLOR_ACCENT_TANGERINE      RGB(242, 127, 26)
#define COLOR_ACCENT_NAVY           RGB(103, 139, 168)
#define COLOR_ACCENT_GRAY           RGB(154, 145, 140)
#define COLOR_ACCENT_OLIVE          RGB(147, 148, 106)
#define COLOR_ACCENT_PLUM           RGB(127, 99, 119)
#define COLOR_ACCENT_BRICK          RGB(196, 18, 0)
#define COLOR_WHITE                 RGB(255,255,255)
