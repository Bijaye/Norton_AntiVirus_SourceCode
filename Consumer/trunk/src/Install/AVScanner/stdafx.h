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


#define WIN32_LEAN_AND_MEAN		// Exclude rarely-used stuff from Windows headers
// Change these values to use different versions
#define WINVER		0x0400
#define _WIN32_WINNT	0x0500
#define _WIN32_IE	0x0500
#define _RICHEDIT_VER	0x0100

#include <TCHAR.h>
#include <atlbase.h>
#include <atlwin.h>
#include "time.h"

extern CAtlWinModule _Module;

//STL
#include <vector>
#include <map>
#include <set>

// CommonClient
#include "ccLib.h"
#include "ccLibStaticLink.h"
#include "ccSymMemoryImpl.h"
#include "ccToolHelp.h"
#include "ccSystemInfo.h"
#include "ccVerifyTrustLibLink.h"
#include "ccTrace.h"
#include "ccDebugOutput.h"
#include "ccTrace.h"
#include "ccSymDebugOutput.h"
#include "ccSymInterfaceLoader.h"
#include "ccSymFileStreamImpl.h"
#include "ccEraserInterface.h"
#include "ccSerializeInterface.h"
#include "ccScanwInterface.h"

//Resources
#include "AvScannerRes.h"

//Scanner
#pragma warning(push)
#pragma warning(disable:4995)
//IPreFlightScanner::RequestAbort() is not deprecated,  ccEraser::IEraser::RequestAbort is
#include "IPreFlightScanner.h"
#include "PreFlightScanObjectBase.h"
#pragma warning(pop)

extern ccSym::CDebugOutput g_DebugOutput;	// defined in AVScanner.cpp
extern HMODULE g_hModule;					// defined in AVScanner.cpp
extern const TCHAR NULL_CHAR;
extern const TCHAR DIRECTORY_DELIMITER_CHAR;

#ifndef UNUSED_ALWAYS
	#define UNUSED_ALWAYS(x)	x
#endif

#undef STRIZE
#define STRIZE(x) #x
#undef PPSTRIZE
#define PPSTRIZE(x) STRIZE(x)
#undef FILE_LINE
#define FILE_LINE __FILE__ "(" PPSTRIZE(__LINE__) ") : "
#define TODO(msg) message(FILE_LINE "TODO:  " msg)
