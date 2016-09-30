////////////////////////
//
// PROPRIETARY / CONFIDENTIAL.
// Use of this product is subject to license terms.
// Copyright © 2006 Symantec Corporation.
// All rights reserved.
//
/////////////////////////////////////////////////////////////////////////////////////////////////

// AVSubmit.cpp : Defines the entry point for the DLL application.
//


#include "stdafx.h"

#define INITIIDS
#define INITGUID

#include  <guiddef.h>
#include <SubmissionEngineLoader.h>
#include <ccLib/ccSymModuleLifetimeMgrHelper.h>
#include <ccLib/ccSymDebugOutput.h>
#include <ccLib/ccInstanceFactory.h>
#include <ccStringInterface.h>
#include <ccIndexValueCollectionInterface.h>
#include <ccKeyValueCollectionInterface.h>
#include <ccLib/ccSymDelayLoader.h>
#include <ccCloneInterface.h>
#include <ccSettings/ccSettingsInterface.h>
#include <ccScanwLoader.h>
#include <ccResourceLoader.h>

#include "Utils.h"
#include "AVSubmissionLoader.h"
#include "QBackupInterfaces.h"
#include "AVSubmissionInterface.h"
#include "AVInterfaces.h"
#include "avQBackupTypes.h"
#include "Sample.h"
#include "Detection.h"
#include "Manager.h"
#include "AVTranslator.h"
#include "AVSubmissionSample.h"
#include "AVSubmissionDetection.h"

cc::IModuleLifetimeMgrPtr g_Manager;
IMPLEMENT_MODULEMANAGER(g_Manager);

SYM_SINGLETON_DEFINE(AVSubmit::CManager);

cc::CResourceLoader g_resources(&_AtlBaseModule, _T("AVSubmit.loc"));

namespace {
	static ccLib::CCriticalSection s_cs;
	static bool InitResources()
	{
		bool bRet = true;
		ccLib::CSingleLock lk(&s_cs, INFINITE, false);
		if(NULL == g_resources.GetResourceInstance())
		{
			if(g_resources.Initialize())
				_AtlBaseModule.SetResourceInstance(g_resources.GetResourceInstance());
			else
			{
				CCTRACEE(_T("Failed to Initialize resources."));
				bRet = false;
			}
		}
		return bRet;
	}
}


SYM_OBJECT_MAP_BEGIN()      
	if(!InitResources()) return SYMERR_INVALID_FILE;
	SYM_OBJECT_ENTRY_SINGLETON(AVSubmit::IID_AVSubmissionManager, AVSubmit::CManager)
	//parent (master) submission objects.
	SYM_OBJECT_ENTRY(AVSubmit::CLSID_AVSampleSubmission, AVSubmit::CSample)
	SYM_OBJECT_ENTRY(AVSubmit::CLSID_AVDetectionSubmission, AVSubmit::CDetection)
	//translators.  OBJID ==> Parent Submission's ISubmission::GetTypeId
	SYM_OBJECT_ENTRY(AVSubmit::TYPEID_AV_DETECTION, AVSubmit::CAvPingTranslator)
	SYM_OBJECT_ENTRY(AVSubmit::TYPEID_AV_SAMPLE, AVSubmit::CAvSampleTranslator)
	//child submissions
	SYM_OBJECT_ENTRY(AVSubmit::CLSID_AvSubmissionSampleChild, AVSubmit::CAvSubmissionSample)
	SYM_OBJECT_ENTRY(AVSubmit::CLSID_AvSubmissionDetectionChild, AVSubmit::CAvSubmissionDetection)
SYM_OBJECT_MAP_END() 

ccSym::CDebugOutput g_DebugOutput(_T("AVSubmit"));
IMPLEMENT_CCTRACE(g_DebugOutput);

BOOL APIENTRY DllMain(HANDLE hModule, DWORD ul_reason_for_call, LPVOID lpReserved)
{
	switch(ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
		DisableThreadLibraryCalls(reinterpret_cast<HMODULE>(hModule));
		break;
	case DLL_PROCESS_DETACH:
		break;
	default:
		break;
	}
    return TRUE;
}
