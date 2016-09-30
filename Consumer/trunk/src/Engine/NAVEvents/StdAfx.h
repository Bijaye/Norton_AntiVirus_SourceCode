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
#include "atlstr.h"
#include "atltime.h"
#include <comdef.h> // for _bstr_t

// Helper library for events
//
#include "ccIgnoreWarnings.h"
#include "ccLib.h"
#include "ccEvent.h"
#include "ccSingleLock.h"
#include "ccCriticalSection.h"
#include "ccMessageLock.h"
#include "ccEveryoneDacl.h"
#include "ccThread.h"
#include "ccMutex.h"

#include "optnames.h"   // Names of options
#include "GlobalEvents.h"
#include "GlobalMutex.h"
#include "NAVInfo.h"

extern CNAVInfo g_NAVInfo;

// For exported functions
//
#define CC_EVTMGR_EXPORTS

#import "ccEventManager.tlb"

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_STDAFX_H__997B4F97_9E27_480E_9B97_7840FEB5CE19__INCLUDED)
