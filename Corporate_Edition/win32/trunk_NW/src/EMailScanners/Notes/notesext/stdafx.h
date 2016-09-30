// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright (c) 2005 Symantec Corporation. All rights reserved.
// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once


#define WIN32_LEAN_AND_MEAN		// Exclude rarely-used stuff from Windows headers

#define _CRT_SECURE_CPP_OVERLOAD_STANDARD_NAMES 1

// Windows Header Files:
#include <windows.h>
// C RunTime Header Files
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>

#include "savassert.h"

// VC8 has built in support for _countof()
#if _MSC_VER >= 1400

    #define CCDIMOF _countof

#else // _MSC_VER >= 1400

    #define CCDIMOF(Array) (sizeof(Array) / sizeof(Array[0]))

#endif // _MSC_VER >= 1400


// TODO: reference additional headers your program requires here
