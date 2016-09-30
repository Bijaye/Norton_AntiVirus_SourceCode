// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright (c) 2005 Symantec Corporation. All rights reserved.
// stdafx.h : include file for standard system include files,
//      or project specific include files that are used frequently,
//      but are changed infrequently

#if !defined(AFX_STDAFX_H__6F952B54_BCEE_11D1_82D6_00A0C9749EEF__INCLUDED_)
#define AFX_STDAFX_H__6F952B54_BCEE_11D1_82D6_00A0C9749EEF__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#define STRICT


// We're targeting 2000 with IE 4

#define _WIN32_WINNT   0x0500
#define WINVER         0x0500
#define _WIN32_IE      0x0400

#define _ATL_APARTMENT_THREADED


#include <atlbase.h>
//You may derive a class from CComModule and use it if you want to override
//something, but do not change the name of _Module
extern CComModule _Module;
#include <atlcom.h>

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_STDAFX_H__6F952B54_BCEE_11D1_82D6_00A0C9749EEF__INCLUDED)
