// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright (c) 2005 Symantec Corporation. All rights reserved.
// stdafx.h : include file for standard system include files,
//      or project specific include files that are used frequently,
//      but are changed infrequently

#if !defined(AFX_STDAFX_H__0F16FA58_8B5A_4be4_A5EE_74F149ECDBFA__INCLUDED_)
#define AFX_STDAFX_H__0F16FA58_8B5A_4be4_A5EE_74F149ECDBFA__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#define STRICT


// We're targeting 2000 and above

#define _WIN32_WINNT   0x0500
#define _WIN32_WINDOWS 0x0500	// defined here only to shut ccLib.h's warnings up
#define WINVER         0x0500

#include <windows.h>


#include "resource.h"

#include "ccDebugOutput.h"

extern ccLib::CDebugOutput g_DebugOutput;

#endif // !defined(AFX_STDAFX_H__0F16FA58_8B5A_4be4_A5EE_74F149ECDBFA__INCLUDED_)
