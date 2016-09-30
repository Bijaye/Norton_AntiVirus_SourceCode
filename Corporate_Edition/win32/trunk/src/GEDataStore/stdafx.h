// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright (c) 2004, 2005, Symantec Corporation, All rights reserved.
// stdafx.h

#ifndef __GEDATASTORE_PRECOMPILED_HEADER__
#define __GEDATASTORE_PRECOMPILED_HEADER__


// We're targeting NT 4 and 98 with IE 4

#define _WIN32_WINNT   0x0400
#define _WIN32_IE      0x0400


#define VC_EXTRALEAN                // Exclude rarely-used stuff from Windows headers

// Prior to the inclusion of WinSock2, set the hard-coded array size for Select.
// The default is 64, but we want to handle 1000's of concurrent conncetions.

#define _CRT_SECURE_CPP_OVERLOAD_STANDARD_NAMES 1

#include <windows.h>
#include <assert.h>
#include <stdio.h>
#include <tchar.h>

// Use the C-RunTime's debug heap in debug builds.

#ifdef _DEBUG
    #define _CRTDBG_MAP_ALLOC
    #include <crtdbg.h>
#endif


// array_sizeof to help with element count as opposed to byte count.

#define array_sizeof(x) (sizeof(x) / sizeof(x[0]))

// array_end to help with fixed array iteration

#define array_end(x)    ((x) + array_sizeof(x))

// MIN and MAX macros

#define MAX(a,b)            (((a) > (b)) ? (a) : (b))
#define MIN(a,b)            (((a) < (b)) ? (a) : (b))


#endif // __GEDATASTORE_PRECOMPILED_HEADER__

