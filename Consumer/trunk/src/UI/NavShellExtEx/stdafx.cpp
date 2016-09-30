////////////////////////
//
// PROPRIETARY / CONFIDENTIAL.
// Use of this product is subject to license terms.
// Copyright © 2006 Symantec Corporation.
// All rights reserved.
//
/////////////////////////////////////////////////////////////////////////////////////////////////

// stdafx.cpp : source file that includes just the standard includes
// NavShellExtEx.pch will be the pre-compiled header
// stdafx.obj will contain the pre-compiled type information

#include "stdafx.h"

// CC headers
#include "ccLibStaticLink.h"
#include "ccSymCommonClientInfo.h"
#include "ccTrace.h"
#include "ccSymDebugOutput.h"
ccSym::CDebugOutput g_DebugOutput(L"NavShExt");
IMPLEMENT_CCTRACE(::g_DebugOutput);
