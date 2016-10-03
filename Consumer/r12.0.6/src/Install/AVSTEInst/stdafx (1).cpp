// stdafx.cpp : source file that includes just the standard includes
// IWPInst.pch will be the pre-compiled header
// stdafx.obj will contain the pre-compiled type information

#include "stdafx.h"

HINSTANCE ghInstance;

#include "ccSymDebugOutput.h"
ccSym::CDebugOutput g_DebugOutput(_T("AVSTEInst"));
IMPLEMENT_CCTRACE(::g_DebugOutput);