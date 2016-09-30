// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

#define STRICT
#ifndef _WIN32_WINNT
#define _WIN32_WINNT 0x0400
#endif

#include <windows.h>
#include "ccLib.h"
#include "ccCriticalSection.h"
#include "ccThread.h"
#include "ccMessageLock.h"
#include "ccSingleLock.h"
#include "ccMutex.h"
#include "ccEveryoneDacl.h"

#include <string>

// turning off performance warnings for forcing values to bool.
// e.g. - warning C4800: 'DWORD' : forcing value to bool 'true' or 'false' (performance warning)
//
#pragma warning (disable: 4800 )

extern HINSTANCE g_hInstance;
