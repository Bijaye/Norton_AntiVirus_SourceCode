////////////////////////
//
// PROPRIETARY / CONFIDENTIAL.
// Use of this product is subject to license terms.
// Copyright © 2006 Symantec Corporation.
// All rights reserved.
//
/////////////////////////////////////////////////////////////////////////////////////////////////

// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

#define _WIN32_WINNT 0x401

#include <windows.h>

#include <ccLib/ccLib.h>
#include <ccLib/ccLibDllLink.h>
//#include <ccLib/ccLibStaticLink.h>
#include <ccLib/ccTrace.h>
#include <cclib/ccExceptionInfo.h>
#include <ccLib/ccSymInstalledApps.h>
#include <ccLib/ccTaskThread.h>

//synchronization
#include <ccLib/ccReaderPreferenceLock.h>
#include <ccLib/ccReadLock.h>
#include <ccLib/ccWriteLock.h>
#include <ccLib/ccSingleLock.h>
#include <ccLib/ccMutex.h>
#include <ccLib/ccCriticalSection.h>
#include <ccLib/ccEvent.h>
#include <ccLib/ccIgnoreWarnings.h>

#pragma warning(push)
//unreachable code
#pragma warning(disable: 4702)

#include <string>
#include <map>
#include <set>
#include <vector>
#include <deque>
#include <list>

#pragma warning(pop)

#include <atltime.h>








