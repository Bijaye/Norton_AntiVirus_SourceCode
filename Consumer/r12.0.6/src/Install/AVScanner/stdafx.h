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

// CommonClient
#include "ccLibStd.h"
#include "ccLibStaticLink.h"
#include "ccToolHelp.h"
#include "ccSystemInfo.h"
#include "ccVerifyTrustLibLink.h"
#include "ccTrace.h"
#include "ccDebugOutput.h"
#include "ccTrace.h"
#include "ccSymDebugOutput.h"
#include "ccSymInterfaceLoader.h"
#include "ccSymFileStreamImpl.h"
#include "EraserScanSink.h"
#include "ccEraserInterface.h"

//Resources
#include "Resource.h"

//Scanner
#include "IPreFlightScanner.h"
#include "PreFlightScanObjectBase.h"

//NAV 
#include "IQuaran.h"
#include "QUAR32.H"

extern ccSym::CDebugOutput g_DebugOutput;
