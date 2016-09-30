////////////////////////
//
// PROPRIETARY / CONFIDENTIAL.
// Use of this product is subject to license terms.
// Copyright © 2006 Symantec Corporation.
// All rights reserved.
//
/////////////////////////////////////////////////////////////////////////////////////////////////

// avScanTask.cpp : Defines the entry point for the DLL application.
//
#include "stdafx.h"

#define INITIIDS
#include <initguid.h>
#include <Mstask.h>
#include "SymInterface.h"
#include "ScanTaskInterface.h"
#include "ccVerifyTrustInterface.h"
#include "ccSettingsInterface.h"
#include "SymTheme.h"
#include "SymHTMLDocument.h"
#include "SymHTMLDialog.h"
#include "SymHTMLEventIterator.h"
#include "SymHTMLAsyncTask.h"    // szNISTab
#include "ScanTaskUI.h"
#include "ScanTaskLoader.h"
#include "ScanTask.h"
#include "ScanTaskCollection.h"
#include "SymResourceLoader.h"
#include "isErrorInterface.h"
#include "Scheduler.h"

#include "ccSymDebugOutput.h"
ccSym::CDebugOutput g_DebugOutput(L"avScnTsk");
IMPLEMENT_CCTRACE(g_DebugOutput);

SYM_OBJECT_MAP_BEGIN()
	SYM_OBJECT_ENTRY(avScanTask::IID_IScanTaskUI, CScanTaskUI)
	SYM_OBJECT_ENTRY(avScanTask::IID_IScanTask, CScanTask)
	SYM_OBJECT_ENTRY(avScanTask::IID_IScanTaskCollection, CScanTaskCollection)
	SYM_OBJECT_ENTRY(avScanTask::IID_IScheduler, CScheduler)
SYM_OBJECT_MAP_END()

CResourceModule _Module;

//-------------------------------------------------------------------------
BOOL APIENTRY DllMain(HANDLE hModule, 
                       DWORD  dwReason, 
                       LPVOID lpReserved)
{
	switch(dwReason)
	{
	case DLL_PROCESS_ATTACH:
		if(FAILED(_Module.LoadResourceModule(static_cast<HINSTANCE>(hModule))))
		{
			CCTRCTXI0(L"Unable to load resource module");
			return FALSE;
		}

		DisableThreadLibraryCalls(static_cast<HMODULE>(hModule));
		break;

	case DLL_PROCESS_DETACH:
		break;
	}
    return TRUE;
}
