// /////////////////////////////////////////////////////////////////////////////////////////////////
// Symantec copyright header start
// //////////////////////
//
// PROPRIETARY / CONFIDENTIAL.
// Use of this product is subject to license terms.
// Copyright © 2004 Symantec Corporation.
// All rights reserved.
//
// /////////////////////////////////////////////////////////////////////////////////////////////////
// Symantec copyright header stop
// //////////////////////
// AVScanner.cpp : Defines the entry point for the DLL application.
//


#define INITIIDS

#include "stdafx.h"
#include "AVScanObject.h"

#include <stdio.h>
#include <string>

#include "ccTrace.h"
#include "ccSymDebugOutput.h"
ccSym::CDebugOutput g_DebugOutput(_T("AVScan"));
IMPLEMENT_CCTRACE(::g_DebugOutput);

using namespace std;

SYM_OBJECT_MAP_BEGIN()                           
	SYM_OBJECT_ENTRY( CLSID_AVScanObject, CAVScanObject )
	SYM_PREINST_ENUM_ENTRY();
SYM_OBJECT_MAP_END()   

SYM_PREINST_SCANNER_MAP_BEGIN()
	SYM_PREINST_SCANNER_ENTRY(CLSID_AVScanObject)
SYM_PREINST_SCANNER_MAP_END()


BOOL APIENTRY DllMain( HANDLE hModule, 
                       DWORD  ul_reason_for_call, 
                       LPVOID lpReserved
					 )
{
    if (DLL_PROCESS_ATTACH == ul_reason_for_call)
    {
        // Performance Optimization: see MSDN docs on DisableThreadLibraryCalls() func.
        DisableThreadLibraryCalls((HMODULE)hModule);
    }

    return TRUE;
}




