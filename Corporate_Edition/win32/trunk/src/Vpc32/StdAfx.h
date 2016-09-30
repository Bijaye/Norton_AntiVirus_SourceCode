// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright (c) 2005 Symantec Corporation. All rights reserved.
// stdafx.h : include file for standard system include files,
//  or project specific include files that are used frequently, but
//      are changed infrequently
//

// We're targeting 2000 with IE 4

#define _WIN32_WINNT   0x0500
#define WINVER         0x0500
#define _WIN32_IE      0x0400

#define VC_EXTRALEAN		// Exclude rarely-used stuff from Windows headers

#ifndef SYM_WIN32
#define SYM_WIN32           // We need to define these symbols manually when
#define SYM_MFC             // building in MSVC.
#endif

#define _CRT_SECURE_CPP_OVERLOAD_STANDARD_NAMES 1

#include <platform.h>
#include <afxwin.h>         // MFC core and standard components
#include <afxext.h>         // MFC extensions
#include <afxdisp.h>        // MFC OLE automation classes
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>			// MFC support for Windows Common Controls
#endif // _AFX_NO_AFXCMN_SUPPORT

#include "ccLib.h"



