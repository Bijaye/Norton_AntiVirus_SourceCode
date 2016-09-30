// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright (c) 2006, Symantec Corporation, All rights reserved.
// EntryPoints.h

#include "AvPluginDllIntfc.h"
#include "SymUnicode.h"

// The following ifdef block is the standard way of creating macros which make exporting 
// from a DLL simpler. All files within this DLL are compiled with the SPAAVPLUGINIMPLDLL_EXPORTS
// symbol defined on the command line. this symbol should not be defined on any project
// that uses this DLL. This way any other project whose source files include this file see 
// SPAAVPLUGINIMPLDLL_API functions as being imported from a DLL, wheras this DLL sees symbols
// defined with this macro as being exported.
#ifdef SPAAVPLUGINIMPLDLL_EXPORTS
#define SPAAVPLUGINIMPLDLL_API __declspec(dllexport)
#else
#define SPAAVPLUGINIMPLDLL_API __declspec(dllimport)
#endif

class IDataManager;

#ifdef __cplusplus
extern "C" {
#endif //__cplusplus

// These functions are exported from the SpaAvPluginImpl.dll
SPAAVPLUGINIMPLDLL_API HRESULT ParsePolicyXml(IDataManager* poXmlParser, const TCHAR* tzXml);
//SPAAVPLUGINIMPLDLL_API DWORD GatherState(???);

SPAAVPLUGINIMPLDLL_API HRESULT StartEventForwarder(ISyLog* poSyLog);
SPAAVPLUGINIMPLDLL_API HRESULT StartEventForwarderEx(ISyLog* poSyLog, const SYMUCHAR* tzSavEventLogDirPath, HANDLE hTerminationEvent);
SPAAVPLUGINIMPLDLL_API HRESULT StopEventForwarder(void);

#ifdef __cplusplus
}
#endif //__cplusplus
