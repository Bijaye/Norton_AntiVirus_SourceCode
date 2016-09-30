//
// Bogus precompiled header file needed so we can use toolbox classes.
//
#ifndef _WIN32_WINNT
#define _WIN32_WINNT 0x0400
#endif

#include "windows.h"
#include "tchar.h"
#include "wchar.h"
#include <string>
#include <xstring> // wstring, etc.
#include <vector>
#include <queue>

extern std::string g_strCCAlertPath;
extern "C"      HINSTANCE               g_hInstance;
extern DWORD    g_dwSessionID;  // Session ID for this agent. Set in EventHandler.cpp

#include "ccLib.h"
#include "ccThread.h"
#include "ccSingleLock.h"
#include "ccMessageLock.h"
#include "ccCriticalSection.h"

// StahlSoft
//
#include <crtdbg.h>
#include "atlbase.h"
#include "atlstr.h"
#include <comdef.h> // for _bstr_t
#include "StahlSoft.h"
#include "apwntres.h"        // For resources
#include "ResourceHelper.h"

#include "savrt32.h"    // For Cookies
#include "AutoProtectWrapper.h"

// The SymHelp.h has a ton of overlapping defines
//
#pragma warning(push)
#pragma warning(disable:4005)
#include "SymHelp.h"
#pragma warning(pop) // reset to original pragma's

extern CString _g_csHandlerPath;
extern CAtlBaseModule _Module;

// DON'T PUT ANYTHING WITH SYMINTERFACE IN HERE ! OTHERWISE YOU GET "unresolved external symbol" ERRORS

