////////////////////////
//
// PROPRIETARY / CONFIDENTIAL.
// Use of this product is subject to license terms.
// Copyright © 2006 Symantec Corporation.
// All rights reserved.
//
/////////////////////////////////////////////////////////////////////////////////////////////////

// stdafx.h : include file for standard system include files,
//      or project specific include files that are used frequently,
//      but are changed infrequently

#if !defined(AFX_STDAFX_H__B96187C8_D322_4490_BE83_6B3290876030__INCLUDED_)
#define AFX_STDAFX_H__B96187C8_D322_4490_BE83_6B3290876030__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define STRICT
#ifndef _WIN32_WINNT
#define _WIN32_WINNT 0x0400
#endif
#define _ATL_APARTMENT_THREADED

#include <atlbase.h>
#include <atlconv.h>	// For W2A() etc.
#include <atlcom.h>
#include <atlwin.h>
#include <atlctl.h>
//You may derive a class from CComModule and use it if you want to override
//something, but do not change the name of _Module
extern CComModule _Module;


#include <stdexcept>
using std::runtime_error;

#include "StahlSoft.h"

#include "shlobj.h"
#include "ccLib.h"
#include "ccLibDLLLink.h"

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_STDAFX_H__B96187C8_D322_4490_BE83_6B3290876030__INCLUDED)
