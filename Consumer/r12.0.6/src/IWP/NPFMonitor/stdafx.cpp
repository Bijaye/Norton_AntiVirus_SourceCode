// stdafx.cpp : source file that includes just the standard includes
// NavFWMon.pch will be the pre-compiled header
// stdafx.obj will contain the pre-compiled type information

#include "stdafx.h"

#include "ccSymDebugOutput.h"
ccSym::CDebugOutput g_DebugOutput(_T("npfmonitor"));
IMPLEMENT_CCTRACE(g_DebugOutput);

// Module lifetime management for trust cache
#include "ccSymModuleLifetimeMgrHelper.h"
cc::IModuleLifetimeMgrPtr g_ModuleManager;
IMPLEMENT_MODULEMANAGER(::g_ModuleManager);
