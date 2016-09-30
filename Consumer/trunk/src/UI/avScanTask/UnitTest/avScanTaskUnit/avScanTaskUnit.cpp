////////////////////////
//
// PROPRIETARY / CONFIDENTIAL.
// Use of this product is subject to license terms.
// Copyright © 2006 Symantec Corporation.
// All rights reserved.
//
/////////////////////////////////////////////////////////////////////////////////////////////////

// avScanTastUnit.cpp : Defines the entry point for the DLL application.
//

#include "stdafx.h"

#define INITIIDS
#include "initguid.h"
#include "SymInterface.h"
#include "ccUnitInterface.h"
#include "ccVerifyTrustInterface.h"
#include "ScanTaskInterface.h"
#include "ScanTaskLoader.h"

// Local files
#include "ScanTaskUITestSuite.h"

#include "ccSymDebugOutput.h"
ccSym::CDebugOutput g_DebugOutput(L"avScanTaskUnit");
IMPLEMENT_CCTRACE(g_DebugOutput);

SYM_OBJECT_MAP_BEGIN()
	SYM_OBJECT_ENTRY(ccUnit::IID_TestSuite, CScanTaskUITestSuite)
SYM_OBJECT_MAP_END()

BOOL APIENTRY DllMain( HANDLE hModule, 
                       DWORD  dwReason, 
                       LPVOID lpReserved
					 )
{
	switch(dwReason)
	{
	case DLL_PROCESS_ATTACH:
		CCTRCTXI0(L"DLL_PROCESS_ATTACH");
		DisableThreadLibraryCalls(static_cast<HMODULE>(hModule));
		break;

	case DLL_PROCESS_DETACH:
		CCTRCTXI0(L"DLL_PROCESS_DETACH");
		break;
	}
	return TRUE;
}

