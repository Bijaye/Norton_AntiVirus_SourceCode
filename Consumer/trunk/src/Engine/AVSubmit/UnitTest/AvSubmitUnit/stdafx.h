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


// Windows Header Files:

#include "winsock2.h"
#include <windows.h>

//#define _SYM_DEBUG_INTERFACES

#include <ccLib/ccLib.h>
#include <ccLib/ccLibStaticLink.h>
#include <ccLib/ccTrace.h>
#include <cclib/ccExceptionInfo.h>
#include <ccLib/ccString.h>
#include "ccFile.h"
#include "ccSymPathProvider.h"
#include "ccLib/ccSymInstalledApps.h"

//synchronization
#include <ccLib/ccReaderPreferenceLock.h>
#include <ccLib/ccReadLock.h>
#include <ccLib/ccWriteLock.h>
#include <ccLib/ccSingleLock.h>
#include <ccLib/ccMutex.h>
#include <ccLib/ccCriticalSection.h>
#include <ccLib/ccEvent.h>
#include <ccLib/ccMessageLock.h>

#include <atlconv.h>
#include <atltime.h>

#pragma warning(push)
//unreachable code
#pragma warning(disable:  4702)

#include <string>
#include <map>
#include <set>
#include <vector>
#include <list>
#include <queue>

#pragma warning(pop)

#define ABORT_ON_FAIL(b) if((b) == false) return false