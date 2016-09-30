// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

#define WIN32_LEAN_AND_MEAN		// Exclude rarely-used stuff from Windows headers
// Windows Header Files:
#include <windows.h>

#define _WIN32_WINNT 0x0400

#include "ccLib.h"
#include "ccEvent.h"
#include "ccSingleLock.h"
#include "ccCriticalSection.h"
#include "ccMessageLock.h"
#include "ccEveryoneDacl.h"
#include "ccThread.h"
#include "ccMutex.h"

#include "atlBase.h"
#include "atlstr.h"
#include "atltime.h"
#include <comdef.h> // for _bstr_t

#include "optnames.h"   // Names of options
#include "GlobalEvents.h"
#include "GlobalMutex.h"
#include "OSInfo.h"
#include "NAVInfo.h"

extern CNAVInfo g_NAVInfo;

#include "StatusPropertyNames.h"

