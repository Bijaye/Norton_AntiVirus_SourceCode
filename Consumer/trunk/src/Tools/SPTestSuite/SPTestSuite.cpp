////////////////////////
//
// PROPRIETARY / CONFIDENTIAL.
// Use of this product is subject to license terms.
// Copyright © 2006 Symantec Corporation.
// All rights reserved.
//
/////////////////////////////////////////////////////////////////////////////////////////////////

// ExcluTestSuite.cpp : Defines the entry point for the DLL application.
//
#include "StdAfx.h"
#define INITIIDS

#include "ccUnitInterface.h"
#include "ccSymDebugOutput.h"
#include "ccSymCrashHandler.h"
#include "ccTrace.h"

#include "ccLibUnitTestSuite.h"
#include "SymInterface.h"

//Include for SymProtect
#include "SymProtectEventsInterface.h"
#include "SymProtectEventTypes.h"
#include "SymProtectEventsLoader.h"

#include "CEventTest.h"
#include "CEventProvider.h"
#include "ccEventManagerHelper.h"
#include "ccProviderHelper.h"
#include "ccSubscriberHelper.h"
#include "PolicyEventSubscriber.h"

#include "ccTrace.h"
#include "SPSuite.h"
#include "BBEventID.h"

SYM_OBJECT_MAP_BEGIN()
	SYM_OBJECT_ENTRY(ccUnit::IID_TestSuite, CSPTestSuite)
SYM_OBJECT_MAP_END()

ccSym::CDebugOutput g_DebugOutput(_T("SymProtectTest"));
IMPLEMENT_CCTRACE(::g_DebugOutput);


BOOL APIENTRY DllMain( HANDLE hModule, 
                       DWORD  ul_reason_for_call, 
                       LPVOID lpReserved
					 )
{
	switch (ul_reason_for_call)
	{
		case DLL_PROCESS_ATTACH:
		case DLL_THREAD_ATTACH:
		case DLL_THREAD_DETACH:
		case DLL_PROCESS_DETACH:
			break;
	}

    return TRUE;
}

