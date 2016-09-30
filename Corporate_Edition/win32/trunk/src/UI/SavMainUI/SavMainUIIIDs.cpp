// WProtect32IIDs.cpp
// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright 2004, 2005 Symantec Corporation. All rights reserved.
//

#include "stdafx.h"

#define INITIIDS
#include "defutilsinterface.h"
#include "ccSettingsManagerHelper.h"
#include "ccSymDebugOutput.h"
#include "ccSettingsManagerHelper.h"
#include "ccCoInitialize.h"
#include "SrtControlInterface.h"
#include "IRiskGEDataStore.h"
#include "IFileNameGEDataStore.h"
#include "IFileHashGEDataStore.h"
#include "IDirectoryGEDataStore.h"

// Instantiate CC's debug object and trace support - must do so or CC won't build
ccSym::CDebugOutput		g_DebugOutput(_T("SavMainUI"));
IMPLEMENT_CCTRACE(g_DebugOutput)

// Declares any objects available from this process. Really the below is to
// get a g_DLLObjectCount instance created.
SYM_OBJECT_MAP_BEGIN()
SYM_OBJECT_MAP_END()

// Function definition for a few core LDVP object functions.  Designed to be included in another CPP file, alas.
#include "ldvpcom.c"

// Import SAV GUIDs
#include "vpcommon.h"
IMP_VPCOMMON_IIDS