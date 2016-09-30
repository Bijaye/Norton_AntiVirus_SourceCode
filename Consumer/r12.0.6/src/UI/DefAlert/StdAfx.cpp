// stdafx.cpp : source file that includes just the standard includes
//  stdafx.pch will be the pre-compiled header
//  stdafx.obj will contain the pre-compiled type information

#include "stdafx.h"

// ccLib delay loader
// This will allow the ccLib dll to delay load at run-time
#include "ccLibDllLink.h"
#include "ccSymDelayLoader.h"
ccSym::CDelayLoader g_DelayLoader;

#include "ccTrace.h"
#include "ccSymDebugOutput.h"
ccSym::CDebugOutput g_DebugOutput(_T("defalert"));
IMPLEMENT_CCTRACE(::g_DebugOutput);