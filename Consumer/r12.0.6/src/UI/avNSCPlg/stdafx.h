// ----------------------------------------------------------------------------
// PROPRIETARY/CONFIDENTIAL.  Use of this product is subject to license terms.
// Copyright (C) 2005 Symantec Corporation.  All rights reserved.
// ----------------------------------------------------------------------------
// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

#define _WIN32_WINNT 0x0500 // win2k & greater

//#define WIN32_LEAN_AND_MEAN     // Exclude rarely-used stuff from Windows headers
// Windows Header Files:
#include <windows.h>

// turn off c++ exception spec warning in cc headers
#pragma warning(disable:4290)

// TODO: reference additional headers your program requires here
// common client includes
#include "ccTrace.h"
#include "ccSymDebugOutput.h"  //removed due to redefinition of ccLib::CCriticalSection
#include "ccSymStringImpl.h"

// Headers for the Options, Status objects.
#include <atlbase.h>
//You may derive a class from CComModule and use it if you want to override
//something, but do not change the name of _Module
extern CComModule _Module;
#include <atlcom.h>
#include <atlctl.h> // For IObjectSafetyImpl

#include "NAVInfo.h"    // Contains useful about NAV, like install path
#include "DebugLog.h"

#include "ccLib.h"
#include "ccEvent.h"
#include "ccMutex.h"
#include "ccSingleLock.h"

#include "APOptNames.h"
#include "OptNames.h"
#include "ScriptSafeVerifyTrustCached.h"		// For scripting security
#include "NAVTrust.h"
#include "NAVOptions_i.c"	// to read AP options in SARV.dat
#include "NAVOptions.h"
#include "NAVLnch.h"
#include "AppLauncher.h"
#include "NAVAPSCR_i.c"
#include "NAVAPSCR.h"
#include "ScriptableAP.h"
#include "navtasks_i.c"
#include "navtasks.h"

#include "StatusPropertyNames.h"

#include "StatusAutoProtect.h"
#include "IWPSettingsInterface.h"
#include "NAVSettingsHelperEx.h" 
#include "SettingsInterfaceMgr.h"
#include "IWPPrivateSettingsInterface.h"
#include "IWPPrivateSettingsLoader.h"

#include "ccEveryoneDACL.h"

#include "Help_URI.h"

#include "AvEvents.h"
#include "NavEventFactoryLoader.h"
#include "AllNavEvents.h"
#include "NAVEventCommon.h"
#include "ccSymModuleLifetimeMgrHelper.h"

// trace enhancements
#if 1
// #ifdef _DEBUG
#define TRACEI\
    {\
    TCHAR szBuffer[1024];\
    wsprintf(szBuffer, _T("%hs(%d): \n\t"),__FILE__,__LINE__);\
    OutputDebugString (szBuffer);\
    }\
    CCTRACEI
#define TRACEW\
    {\
    TCHAR szBuffer[1024];\
    wsprintf(szBuffer, _T("%hs(%d): \n\t"),__FILE__,__LINE__);\
    OutputDebugString (szBuffer);\
    }\
    CCTRACEW
#define TRACEE \
    {\
    TCHAR szBuffer[1024];\
    wsprintf(szBuffer, _T("%hs(%d): \n\t"),__FILE__,__LINE__);\
    OutputDebugString (szBuffer);\
    }\
    CCTRACEE
#else
#define TRACEI CCTRACEI
#define TRACEW CCTRACEW
#define TRACEE CCTRACEE
#endif

// check HR
#define CHECK_HR(hr)\
if (FAILED(hr))\
{\
	CCTRACEE(_T("HRESULT = %x in %hs(%d)"),hr,__FILE__,__LINE__);\
}

// check NSCRESULT
#define CHECK_NSCRESULT(nscresult)\
if (NSC_FAILED(nscresult))\
{\
	CCTRACEE(_T("NSCRESULT = %x in %hs(%d)"),nscresult,__FILE__,__LINE__);\
}

// process/thread protection
#define CROSS_PROCESS_LOCK()/* \
        ccLib::CEveryoneDACL dacl;\
        dacl.Create();\
        ccLib::CMutex cMutex;\
        BOOL bMutex = cMutex.Create(dacl,FALSE,_T("Global\\avNSCPlg_MUTEX"),TRUE);\
        if (!bMutex)\
        {\
            TRACEI(_T("mutex failed to create, try to open"));\
            if (!cMutex.Open(SYNCHRONIZE,FALSE,_T("Global\\avNSCPlg_MUTEX"),TRUE))\
            {\
                TRACEE(_T("Mutex.Open failed with %d"), ::GetLastError());\
                return nsc::NSC_FAIL;\
            }\
        }\
        ccLib::CSingleLock lock(&cMutex, INFINITE, TRUE);*/



// nsc includes
#include "nscISelfRegistrar.h" 
#include "nscIWrapper.h"
#include "nscIProduct.h"
#include "nscIFeature.h"
#include "nscIClientCallback.h"
#include "nscIServerCallback.h"
#include "nscIServerCallbackInfo.h"
#include "nscIClientCallbackInfo.h"


// nsclib includes
#include "nscLibCAction.h"
#include "nscLibCOnOff.h"
#include "nscLibCStringType.h"
#include "nscLibCHealth.h"
#include "nscLibCFeatureGUIDsCollection.h"

// avNSCPlg includes
#include "avNSCPlg_GUIDs.h"

// path name to this module (avNSCPlg.dll)
extern TCHAR g_szModulePath[MAX_PATH]; 

#include "..\avNSCPlgRes\resource.h"