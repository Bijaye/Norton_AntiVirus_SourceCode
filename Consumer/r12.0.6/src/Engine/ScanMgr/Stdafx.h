#ifndef STDAFX_H
#define STDAFX_H

#define _WIN32_WINNT 0x0400   // ugh.

#include <windows.h>
#include <tchar.h>
#include <process.h>
#include <crtdbg.h>

#pragma warning (disable: 4786)

#include <map>
#include <string>
#include <vector>
#include <set>
#include <stdexcept>

#include "ccScanInterface.h"
#include "ccEraserInterface.h"
#include "NAVInfo.h"
#include "OptNames.h"

#include "ccLibStd.h"
#include "ccSymDebugOutput.h"

extern HINSTANCE g_hInstance;

extern ccSym::CDebugOutput g_DebugOutput;
extern ccSym::CDebugOutput g_FileDebugOutput;

#endif

