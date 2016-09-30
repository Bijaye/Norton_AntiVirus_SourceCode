////////////////////////
//
// PROPRIETARY / CONFIDENTIAL.
// Use of this product is subject to license terms.
// Copyright © 2006 Symantec Corporation.
// All rights reserved.
//
/////////////////////////////////////////////////////////////////////////////////////////////////

// stdafx.h : header file
//
#pragma once

#define WIN32_LEAN_AND_MEAN		// Exclude rarely-used stuff from Windows headers
#define WINVER			0x0500
#define _WIN32_WINNT	0x0502
#define _WIN32_WINDOWS	0x0502
#define _WIN32_IE		0x0600

// Disable warnings that can safely be ignored
#pragma warning(disable : 4018)
#include <ccIgnoreWarnings.h>

// ATL defines
#define _ATL_CSTRING_EXPLICIT_CONSTRUCTORS
#define _ATL_ALL_WARNINGS

// ATL Files
#include <atlbase.h>
#include <atlstr.h>
#include <atlwin.h>
#include <atlapp.h>
#include <shellapi.h>
#include "StahlSoft.h"
#include "ccLib.h"

#ifndef _countof
#define _countof(x)(sizeof(x)/sizeof(*x))
#endif //_countof

#include "SymResourceModule.h"
#include "ccSymPathProvider.h"

//-----------------------------------------------------------------------------
class CResourceModule : public CSymResourceModuleT< CAtlDllModuleT < CResourceModule > >
{
public:
};

extern CResourceModule _Module;
