////////////////////////
//
// PROPRIETARY / CONFIDENTIAL.
// Use of this product is subject to license terms.
// Copyright © 2006 Symantec Corporation.
// All rights reserved.
//
/////////////////////////////////////////////////////////////////////////////////////////////////

// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

#include <iostream>

#define _WIN32_WINNT    0x501           // Windows XP
#define _WIN32_WINDOWS  _WIN32_WINNT
#define _WIN32_IE       0x600           // IE 6.0+

#define WIN32_LEAN_AND_MEAN		// Exclude rarely-used stuff from Windows headers
#include <windows.h>
#include <atlstr.h>

#include "ccLibStd.h"
#include "ccLibStaticLink.h"
