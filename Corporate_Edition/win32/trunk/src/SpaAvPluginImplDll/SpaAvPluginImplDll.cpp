// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright (c) 2006, Symantec Corporation, All rights reserved.
// EntryPoints.h

// SpaAvPluginImplDll.cpp : Defines the entry point for the DLL application.
//

#include "stdafx.h"
#include "SpaAvPluginImplDll.h"
#include "EntryPoints.h"
#include "EntryPointsLogForwarding.h"
#include "EntryPointsOpState.h"
#include "AVContentUpdateHandler.h"
#include "RegUtils.h"
#include "vpcommon.h"
#include "ProductRegCOM.h"

class CAvPluginImplModule : public CAtlDllModuleT< CAvPluginImplModule >
{
public :
	DECLARE_LIBID(LIBID_PRODUCTREGCOMLib)
};

CAvPluginImplModule _AtlModule;

extern "C" BOOL APIENTRY DllMain( HANDLE hModule, 
                                  DWORD  ul_reason_for_call, 
                                  LPVOID lpReserved
					            )
{
    switch (ul_reason_for_call)
	{
		case DLL_PROCESS_ATTACH:
#ifdef _DEBUG	// BUGBUG
			//if (::MessageBox(NULL, "Do you want to start the debugger?", "AvPluginImpl.dll", MB_YESNO | MB_ICONQUESTION) == IDYES)
			//	DebugBreak();
#endif //_DEBUG
			if (FAILED(::AvPluginImplInit()))
				return FALSE;
			break;
		case DLL_PROCESS_DETACH:
			(void) ::AvPluginImplDeinit();
			break;
		case DLL_THREAD_ATTACH:
		case DLL_THREAD_DETACH:
			break;
    }
    
    return _AtlModule.DllMain(ul_reason_for_call, lpReserved);
}

SPAAVPLUGINIMPLDLL_API HRESULT ParsePolicyXml(IDataManager* poXmlParser, const TCHAR* tzXml)
{
	return ParsePolicyXmlImpl(poXmlParser, tzXml);
}

SPAAVPLUGINIMPLDLL_API HRESULT StartEventForwarder(ISyLog* poSyLog, ICMDProcessPlugin* poCmdPlugin)
{
	return StartEventForwarderImpl(poSyLog, poCmdPlugin);
}

SPAAVPLUGINIMPLDLL_API HRESULT StartEventForwarderEx(ISyLog* poSyLog, ICMDProcessPlugin* poCmdPlugin, const SYMUCHAR* tzSavEventLogDirPath, HANDLE hTerminationEvent)
{
	return StartEventForwarderImplEx(poSyLog, poCmdPlugin, tzSavEventLogDirPath, hTerminationEvent);
}

SPAAVPLUGINIMPLDLL_API HRESULT StopEventForwarder(void)
{
	return StopEventForwarderImpl();
}

SPAAVPLUGINIMPLDLL_API HRESULT GetOpState(SymUtil::ustring& resultStr, bool dirtyOnly)
{
    return GetOpStateImpl(resultStr, dirtyOnly);
}

SPAAVPLUGINIMPLDLL_API HRESULT AddNewCommand(const IDataManager * pDm, const char * pszGuid, const char *pszFeature, const char * pszCmdName, void * userData,
	uint64 arrivalTime,
	uint64 beginTime,
	uint64 lastUpdateTime,
	dword state,
	dword progress,
	dword substate,
	const char * pszSubStateDesc,
	dword estimatedDuration)
{
    return AddNewCommandImpl(pDm, pszGuid, pszFeature,  pszCmdName, userData, arrivalTime,
	        beginTime, lastUpdateTime, state, progress, substate, pszSubStateDesc, estimatedDuration);
}

SPAAVPLUGINIMPLDLL_API HRESULT RefreshCommandStatus(void* data)
{
    return RefreshCommandStatusImpl(data);
}

SPAAVPLUGINIMPLDLL_API HRESULT QueryContentSeqData(const char* moniker,
	const char* requestedSeqData,
	char* closestMatchingSeqData, dword* sizeInAndOutBytes)
{
	// TODO: Consider caching the AvContentUpdateHandler
	AVContentUpdateHandler handler;
	return handler.QueryContentSeqData(moniker, requestedSeqData,
		closestMatchingSeqData, sizeInAndOutBytes);
}

SPAAVPLUGINIMPLDLL_API HRESULT ApplyContent(const char* moniker,
	const char* requestedSeqData,
	const char* uncContentPath,
	const ContentUpdate::PackageType pkgType,
	BOOL* restartRequired)
{
	// TODO: Consider caching the AvContentUpdateHandler
	AVContentUpdateHandler handler;
	return handler.ApplyContent(moniker, requestedSeqData,
		uncContentPath, pkgType, restartRequired);
}

// Used to determine whether the DLL can be unloaded by OLE
STDAPI DllCanUnloadNow(void)
{
    return _AtlModule.DllCanUnloadNow();
}


// Returns a class factory to create an object of the requested type
STDAPI DllGetClassObject(REFCLSID rclsid, REFIID riid, LPVOID* ppv)
{
    return _AtlModule.DllGetClassObject(rclsid, riid, ppv);
}


// DllRegisterServer - Adds entries to the system registry
STDAPI DllRegisterServer(void)
{
    // registers object, typelib and all interfaces in typelib
    HRESULT hr = _AtlModule.DllRegisterServer();
	return hr;
}


// DllUnregisterServer - Removes entries from the system registry
STDAPI DllUnregisterServer(void)
{
	HRESULT hr = _AtlModule.DllUnregisterServer();
	return hr;
}