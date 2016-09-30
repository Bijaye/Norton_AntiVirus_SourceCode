// /////////////////////////////////////////////////////////////////////////////////////////////////
// Symantec copyright header start
// //////////////////////
//
// PROPRIETARY / CONFIDENTIAL.
// Use of this product is subject to license terms.
// Copyright © 2006 Symantec Corporation.
// All rights reserved.
//
// /////////////////////////////////////////////////////////////////////////////////////////////////
// Symantec copyright header stop
// //////////////////////
// AVSubmit.cpp : Defines the entry point for the DLL application.
//


#include "stdafx.h"

#include "SAVSubmitterInterface.h"
#include "SAVSubmitterImpl.h"
#include "SAVQuarantineSubmission.h"
#include "SAVAVDetection.h"
#include "utils.h"

#include "DarwinResCommon.h"

#include "ccSettings/ccSettingsLoader.h"
#include "ccSymDebugOutput.h"
#include "ccTrace.h"
#include "ccLib.h"

#include "hrx.h"
#include "srx.h"


DARWIN_DECLARE_RESOURCE_LOADER(_T("SAVSubmitterRes.dll"), _T("SAVSubmitter"));


SYM_OBJECT_MAP_BEGIN()
	SYM_OBJECT_ENTRY(SAVSubmission::CLSID_SAVSubmitter, SAVSubmission::CSAVSubmitterImpl)
	SYM_OBJECT_ENTRY(SAVSubmission::CLSID_SAVQuarantineSubmission, SAVSubmission::CSAVQuarantineSubmission)
	SYM_OBJECT_ENTRY(SAVSubmission::CLSID_SAVAVDetection, SAVSubmission::CSAVAVDetection)
SYM_OBJECT_MAP_END()

ccLib::CDebugOutput g_DebugOutput(_T("SAVSubmitter"));
IMPLEMENT_CCTRACE(g_DebugOutput);

namespace globals
{

static HMODULE g_hModule = NULL;

}

BOOL APIENTRY DllMain(HANDLE hModule, DWORD ul_reason_for_call, LPVOID /*lpReserved*/)
{
	switch(ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
		globals::g_hModule = (HMODULE)hModule;
		DisableThreadLibraryCalls(reinterpret_cast<HMODULE>(hModule));
		// Now initialize the ATL managed resource list to include our localized resource DLL
		g_ResLoader.Initialize(false);
		_AtlBaseModule.AddResourceInstance(g_ResLoader.GetResourceInstance());
		break;
	case DLL_PROCESS_DETACH:
		break;
	default:
		break;
	}
    return TRUE;
}

namespace {

	static const WCHAR* const g_szSettingsKey = L"SYMINTERFACE\\CLASSES\\SubmissionEngine";

	SYMRESULT RegUnreg(bool bReg, const GUID* pId) throw (srx_exception)
	{
		SYMINTERFACE_SRX_TRY(sr)
		{
			srx << (pId == NULL ? SYMERR_UNKNOWN : SYM_OK);

			cc::ccSet_ISettingsManager setLdr;
			ccSettings::ISettingsManagerPtr pManager;
			ccSettings::ISettingsPtr pSettings;
			TCHAR szGuid[clfs::utils::GUID_MAX_CHARS];
			srx << (clfs::utils::GUIDToString(*pId, szGuid) ? SYM_OK : SYMERR_UNKNOWN);
			srx << setLdr.CreateObject(&pManager.m_p);
			srx << pManager->CreateSettings(g_szSettingsKey, &pSettings.m_p);
			if(bReg)
			{
				TCHAR szPath[MAX_PATH] = {0};
				srx << (GetModuleFileName(globals::g_hModule, szPath, MAX_PATH) == 0 ? SYMERR_UNKNOWN : SYM_OK);
				srx << pSettings->PutString(szGuid, szPath);
			}
			else
			{
				srx << pSettings->DeleteValue(szGuid);
			}
			srx << pManager->PutSettings(pSettings);

		}
		SYMINTERFACE_SRX_CATCH_ALL_NO_RETURN(sr);
		if(SYM_FAILED(sr))
		{
			CCTRCTXE2(_T("%s Failed: 0x%08X"), bReg ? _T("Register") : _T("Unregister"), sr);
		}
		else
		{
			CCTRCTXI2(_T("%s Succeeded: 0x%08X"), bReg ? _T("Register") : _T("Unregister"), sr);
		}
		return sr;
	}
}

STDAPI DllRegisterServer()
{
	if (SYM_FAILED(RegUnreg(true, (GUID*)&SAVSubmission::CLSID_SAVQuarantineSubmission)) ||
		SYM_FAILED(RegUnreg(true, (GUID*)&SAVSubmission::CLSID_SAVAVDetection)))
		return E_FAIL;
	return S_OK;
}

STDAPI DllUnregisterServer()
{
	if (SYM_FAILED(RegUnreg(false, (GUID*)&SAVSubmission::CLSID_SAVQuarantineSubmission)) ||
		SYM_FAILED(RegUnreg(false, (GUID*)&SAVSubmission::CLSID_SAVAVDetection)))
		return E_FAIL;
	return S_OK;
}
