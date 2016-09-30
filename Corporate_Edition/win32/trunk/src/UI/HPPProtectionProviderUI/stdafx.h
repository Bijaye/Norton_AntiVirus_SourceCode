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


#include <afxwin.h>
#include <afxdlgs.h>
#include <afxcmn.h>
#ifndef _AFX_NO_OLE_SUPPORT
    #include <afxdisp.h>        // MFC Automation classes
#endif // _AFX_NO_OLE_SUPPORT
#include "resource.h"
#include <atlbase.h>
#include <atlcom.h>
#include "vpcommon.h"
#include "clientreg.h"
#include "comutil.h"
using namespace ATL;