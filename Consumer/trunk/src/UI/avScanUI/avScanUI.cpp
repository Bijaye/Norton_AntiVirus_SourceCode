// //////////////////////
//
// PROPRIETARY / CONFIDENTIAL.
// Use of this product is subject to license terms.
// Copyright © 2006 Symantec Corporation.
// All rights reserved.
//
/////////////////////////////////////////////////////////////////////////////////////////////////

// avScanUI.cpp : Defines the entry point for the DLL application.
//
#include "stdafx.h"

#define INITIIDS
#include <SymInterface.h>
#include <AVInterfaces.h>
#include <ccSymKeyValueCollection.h>
#include <ccSymIndexValueCollection.h>
#include "avScanUIInternalInterfaces.h"

// SymHTML includes
#include <SymHTMLElement.h>
#include <SymHTMLComparator.h>
#include <SymHTMLWindow.h>
#include <SymHTMLDialog.h>
#include <SymHTMLMouseEvent.h>
#include <SymHTMLDialogDocumentImpl.h>

// AvProdLogging includes
#include <AvProdLoggingLoader.h>

// AvProdService Reboot Suppression
#include <AvProdSvcClientLoader.h>

// CED
#include <isErrorLoader.h>

// ccAlert
#include <ccAlertLoader.h>

// NPC
#include <uiProviderInterface.h>

// ISData
#include <uiNISDataElementGuids.h>

// ccDelayLoad
#include <ccSymDelayLoader.h>
ccSym::CDelayLoader symDelayLoader;

// TRACE implementation
ccSym::CDebugOutput g_DebugOutput(_T("avScanUI"));
IMPLEMENT_CCTRACE(g_DebugOutput);

// Module lifetime manager
#include <ccSymModuleLifetimeMgrHelper.h>
cc::IModuleLifetimeMgrPtr g_ModuleManager;
IMPLEMENT_MODULEMANAGER(::g_ModuleManager);

// Local headers
#include "ScanUI.h"
#include "EmailScanUI.h"
#include "ScanUIMisc.h"
#include "SingleInstance.h"

using namespace avScanUI;

// SymInterface Object map
SYM_OBJECT_MAP_BEGIN()
    SYM_OBJECT_ENTRY(SYMOBJECT_AVSCANUI, CScanUI)
    SYM_OBJECT_ENTRY(SYMOBJECT_AVEMAILSCANUI, CEmailScanUI)
    SYM_OBJECT_ENTRY(SYMOBJECT_SCANUIMISC, CScanUIMisc)
    SYM_OBJECT_ENTRY(SYMOBJECT_SINGLEINSTANCE, CSingleInstance)
SYM_OBJECT_MAP_END()

// Globals
HINSTANCE g_hInstance;
CResourceModule _Module;

ATL::CDynamicStdCallThunk::CThunksHeap ATL::CDynamicStdCallThunk::s_ThunkHeap;

BOOL APIENTRY DllMain(HANDLE hModule, DWORD dwReason, LPVOID lpReserved)
{
    g_hInstance = (HINSTANCE)hModule;
	if(DLL_PROCESS_ATTACH == dwReason)
	{
        CSingleInstanceData::Startup();
        _Module.Init(NULL, g_hInstance);

        // Init the CResourceModule
        ccLib::CStringW sDir;
        HRESULT hr = S_OK;
        hr = _Module.LoadResourceModule(g_hInstance);
        if(FAILED(hr))
        {
            CCTRACEE(CCTRCTX _T("Unable to load resource module"));
            ASSERT(FALSE);
            return FALSE;
        }

		::DisableThreadLibraryCalls((HMODULE)hModule);
	}
	if(DLL_PROCESS_DETACH == dwReason)
	{
        _Module.Term();
    }
    return TRUE;
}

