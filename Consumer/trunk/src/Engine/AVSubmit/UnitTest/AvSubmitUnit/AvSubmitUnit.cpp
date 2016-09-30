////////////////////////
//
// PROPRIETARY / CONFIDENTIAL.
// Use of this product is subject to license terms.
// Copyright © 2006 Symantec Corporation.
// All rights reserved.
//
/////////////////////////////////////////////////////////////////////////////////////////////////

// AvSubmitUnit.cpp : Defines the entry point for the DLL application.
//

#include "stdafx.h"

#define INITIIDS
#define INITGUID
#include <guiddef.h>
#include "ccUnitInterface.h"
#include "TestSuite.h"
#include "ccSymDebugOutput.h"
#include "ccInstanceFactoryInterface.h"
#include "ccStreamInterface.h"
#include "ccSerializeInterface.h"
#include "ccEraserInterface.h"
#include "AVSubmissionInterface.h"
#include "DefUtilsInterface.h"
#include "qbackupinterfaces.h"
#include "AVInterfaces.h"
#include "avqbackuptypes.h"
#include "ccmemoryinterface.h"
#include "ccsymindexvaluecollection.h"

#include "ccSettings/ccSettingsLoader.h"

#include <srx.h>

#include <ccLib/ccSymModuleLifetimeMgrHelper.h>
cc::IModuleLifetimeMgrPtr g_Manager;
IMPLEMENT_MODULEMANAGER(g_Manager);

ccSym::CDebugOutput g_DebugOutput(L"AvSubmitUnit");
IMPLEMENT_CCTRACE(g_DebugOutput);

SYM_OBJECT_MAP_BEGIN()
	SYM_OBJECT_ENTRY(ccUnit::IID_TestSuite, CTestSuite)
SYM_OBJECT_MAP_END()

BOOL APIENTRY DllMain( HANDLE hModule, 
                       DWORD  ul_reason_for_call, 
                       LPVOID lpReserved
					 )
{
	if(ul_reason_for_call == DLL_PROCESS_ATTACH)
		::DisableThreadLibraryCalls((HMODULE)hModule);
    return TRUE;
}

