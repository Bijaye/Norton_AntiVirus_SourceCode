// /////////////////////////////////////////////////////////////////////////////////////////////////
// Symantec copyright header start
// //////////////////////
//
// PROPRIETARY / CONFIDENTIAL.
// Use of this product is subject to license terms.
// Copyright © 2004 Symantec Corporation.
// All rights reserved.
//
// /////////////////////////////////////////////////////////////////////////////////////////////////
// Symantec copyright header stop
// //////////////////////
// stdafx.h : include file for standard system include files,
//      or project specific include files that are used frequently,
//      but are changed infrequently

#if !defined(AFX_STDAFX_H__C3A301C8_FDB3_4712_9F7E_B7DFA1D8CDFE__INCLUDED_)
#define AFX_STDAFX_H__C3A301C8_FDB3_4712_9F7E_B7DFA1D8CDFE__INCLUDED_

#pragma warning (disable: 4267)

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define STRICT
#ifndef _WIN32_WINNT
#define _WIN32_WINNT 0x0400
#endif
#define _ATL_APARTMENT_THREADED

#include <atlbase.h>
#include <atlcom.h>
#include <atlhost.h>
#include <objsafe.h>
#include <atlctl.h>// For IObjectSafetyImpl

#include "ccLibStd.h"
#include "ccLibDLLLink.h"

#define TRACE (void)0

extern CComModule _Module;

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_STDAFX_H__C3A301C8_FDB3_4712_9F7E_B7DFA1D8CDFE__INCLUDED)
