// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright (c) 2005 Symantec Corporation. All rights reserved.
#include <windows.h>

// Common client ccLib initialization.
#ifdef WIN32
#include "ccLibDllLink.h"

#include "ccTrace.h"
#include "ccSymDebugOutput.h"

static ccSym::CDebugOutput  debugOutput(_T("CliScan"));  // KJS - This debug output is only used by CliScan.  There will be a different one for other users of ccLib.
IMPLEMENT_CCTRACE(debugOutput);
#endif
