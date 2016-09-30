// stdafx.cpp : source file that includes just the standard includes
//  stdafx.pch will be the pre-compiled header
//  stdafx.obj will contain the pre-compiled type information

#include "stdafx.h"

#ifdef _ATL_STATIC_REGISTRY
#include <statreg.h>
#include <statreg.cpp>
#endif

#include <atlimpl.cpp>

// ccLib delay loader
// This will allow the ccLib dll to delay load at run-time
//#include "ccLibStaticLink.h"
//#include "ccSymDelayLoader.h"
//ccSym::CDelayLoader g_DelayLoader;

#include "ccSymDebugOutput.h"
ccSym::CDebugOutput g_DebugOutput(_T("NAVTask"));
IMPLEMENT_CCTRACE(::g_DebugOutput);
