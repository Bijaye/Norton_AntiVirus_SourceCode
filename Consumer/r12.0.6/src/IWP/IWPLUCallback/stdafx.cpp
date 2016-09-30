// stdafx.cpp : source file that includes just the standard includes
// IWPLUCallback.pch will be the pre-compiled header
// stdafx.obj will contain the pre-compiled type information

#include "stdafx.h"


#include "ccLib.h"
#include "ccLibDllLink.h"
#include "ccSymDelayLoader.h"

// ccLib delay loader
// This will allow the ccLib dll to delay load at run-time
ccSym::CDelayLoader g_DelayLoader;

#include "ccTrace.h"
#include "ccSymDebugOutput.h"
ccSym::CDebugOutput g_DebugOutput(_T("IWPLUCBK"));
IMPLEMENT_CCTRACE(::g_DebugOutput);
