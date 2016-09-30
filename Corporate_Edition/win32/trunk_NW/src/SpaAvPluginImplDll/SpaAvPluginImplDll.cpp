// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright (c) 2006, Symantec Corporation, All rights reserved.
// EntryPoints.h

// SpaAvPluginImplDll.cpp : Defines the entry point for the DLL application.
//

#include "stdafx.h"
#include "SpaAvPluginImplDll.h"
#include "EntryPoints.h"
#include "EntryPointsLogForwarding.h"
#include "RegUtils.h"
#include "vpcommon.h"

BOOL APIENTRY DllMain( HANDLE hModule, 
                       DWORD  ul_reason_for_call, 
                       LPVOID lpReserved
					 )
{
    switch (ul_reason_for_call)
	{
		case DLL_PROCESS_ATTACH:
//#ifdef _DEBUG
//			if (::MessageBox(NULL, "Do you want to start the debugger?", "AvPluginImpl.dll", MB_YESNO | MB_ICONQUESTION) == IDYES)
//				DebugBreak();
//#endif //_DEBUG
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
    return TRUE;
}

SPAAVPLUGINIMPLDLL_API HRESULT ParsePolicyXml(IDataManager* poXmlParser, const TCHAR* tzXml)
{
	return ParsePolicyXmlImpl(poXmlParser, tzXml);
}

SPAAVPLUGINIMPLDLL_API HRESULT StartEventForwarder(ISyLog* poSyLog)
{
	return StartEventForwarderImpl(poSyLog);
}

SPAAVPLUGINIMPLDLL_API HRESULT StartEventForwarderEx(ISyLog* poSyLog, const SYMUCHAR* tzSavEventLogDirPath, HANDLE hTerminationEvent)
{
	return StartEventForwarderImplEx(poSyLog, tzSavEventLogDirPath, hTerminationEvent);
}

SPAAVPLUGINIMPLDLL_API HRESULT StopEventForwarder(void)
{
	return StopEventForwarderImpl();
}
