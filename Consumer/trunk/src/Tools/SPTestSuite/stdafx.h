////////////////////////
//
// PROPRIETARY / CONFIDENTIAL.
// Use of this product is subject to license terms.
// Copyright © 2006 Symantec Corporation.
// All rights reserved.
//
/////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include <iostream>
#define _WIN32_WINNT    0x501           // Windows XP
#define _WIN32_WINDOWS  _WIN32_WINNT
// Lean and strict
#define WIN32_LEAN_AND_MEAN		
//#define VC_EXTRALEAN		

// ATL defines
//#define _ATL_NO_AUTOMATIC_NAMESPACE
//#define _ATL_CSTRING_EXPLICIT_CONSTRUCTORS	
//#define _ATL_ALL_WARNINGS

//Saranya - Included for CString support
//#include "afx.h"
//#include "atlconv.h"

// Disable warnings that can safely be ignored
#include "ccIgnoreWarnings.h"

// Main ccLib declarations
#include "ccLib.h"
#include "ccMessageLock.h"
#include "ccSingleLock.h"
#include "ccLibStaticLink.h"

#include <windows.h>
//#include <wchar.h>
//#include <iostream>
//#include <tchar.h>
#include <atlstr.h>


//#define INITIIDS