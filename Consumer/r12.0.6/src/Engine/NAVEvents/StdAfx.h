// stdafx.h : include file for standard system include files,
//      or project specific include files that are used frequently,
//      but are changed infrequently

#if !defined(AFX_STDAFX_H__997B4F97_9E27_480E_9B97_7840FEB5CE19__INCLUDED_)
#define AFX_STDAFX_H__997B4F97_9E27_480E_9B97_7840FEB5CE19__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define STRICT
#ifndef _WIN32_WINNT
#define _WIN32_WINNT 0x0400
#endif
#define _ATL_APARTMENT_THREADED

#include <atlbase.h>
//You may derive a class from CComModule and use it if you want to override
//something, but do not change the name of _Module
extern CComModule _Module;
#include <atlcom.h>
#include <comdef.h>

// Helper library for events
//
#include "ccIgnoreWarnings.h"
#include "ccLib.h"

// For exported functions
//
#define CC_EVTMGR_EXPORTS

#import "ccEventManager.tlb"

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_STDAFX_H__997B4F97_9E27_480E_9B97_7840FEB5CE19__INCLUDED)
