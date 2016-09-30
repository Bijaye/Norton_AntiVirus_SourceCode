///////////////////////////////////////////////////////////////////////////////////////////////////
// Symantec copyright header start
///////////////////////////////////////////////////////////////////////////////////////////////////
//
// PROPRIETARY / CONFIDENTIAL.
// Use of this product is subject to license terms.
// Copyright © 2005 Symantec Corporation.
// All rights reserved.
//
///////////////////////////////////////////////////////////////////////////////////////////////////
// Symantec copyright header stop
///////////////////////////////////////////////////////////////////////////////////////////////////
//
// NAVSuiteOwner.cpp : Defines the entry point for the DLL application.
//
///////////////////////////////////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include <initguid.h>

#define _INIT_xVPS
#define PRODUCTKEY_SUITEOWNERDLL "AVSTE.dll"
#define _INIT_SCSSINSTALLHELPER
#define SIMON_INITGUID
#include <simon.h>

#define INITIIDS
#if !defined(_NO_CC)
#include "ccLibDllLink.h"
#include "ccSymDelayLoader.h"
ccSym::CDelayLoader g_DelayLoader;
#endif

#define _STAHLSOFT_INIT_CRC32
#include "ssStringCRC32.h"

// SIMON Interfaces related to licensing
#include "cltLicenseInterfaces.h"
#include "cltProductKey.h"

#define _INIT_CLTINSTALLHELPER
//#define DECLARE_INSTALLHELPER_SUITEOWNERHMODULE
#include "CLTInstallHelper.h"


// Licensing gateway implementation
#define _INIT_COSVERSIONINFO
#define _INIT_CLTLICENSINGGATEWAY_IMPL
#include "cltLicensingGateway.h"

// V2Authorization
#define _CSIMONV2AUTHORIZATION

#include "cltSecurityValidation.h"

// Trace functionality.
#include "clttrace.h"
IMPLEMENT_CLT_TRACE(_T("AVSTE"))

// License technology specific headers
#define _INIT_LICENSEOBJECT_DATA
#include "SymSLC_ObjectMap.h"
#include "SymSLC_CUSTOM.h"

// Log Free Client.
#include "cltILFC.h"

#include "resource.h"

//============================================================================================
// Globals
//============================================================================================
HMODULE g_hModule = NULL;
CString g_sModuleLocation;
CString g_sModuleFileName;

//============================================================================================
// This OBJECT is used internally by this DLL.
//============================================================================================
BEGIN_SIMON_OBJECT_MAP(ObjectMapInternal)
	SIMON_OBJECT_ENTRIES
END_SIMON_OBJECT_MAP()

SIMON::CSimonModule _SimonModuleInternal;

//============================================================================================
// This is exposed to the outside world.
//============================================================================================
BEGIN_SIMON_OBJECT_MAP(ObjectMap)
    SIMON_OBJECT_ENTRY(CLSID_CLicensingGateway  , clt::CLicensingGateway)
	SIMON_OBJECT_ENTRY(V2Authorization::CLSID_V2Authorization  ,  V2Authorization::CSimonV2Authorization) 
    SIMON_OBJECT_ENTRY(clt_LOGFREECLIENT::CLSID_ILogFreeClientCallImpl  ,  clt_LOGFREECLIENT::CLFCCallImpl)
    SIMON_OBJECT_ENTRY(CLSID_CProductKey  ,  clt::CProductKey)
END_SIMON_OBJECT_MAP()
SIMON::CSimonModule _SimonModule;


//============================================================================================
// Security validation map
//============================================================================================
CLTSECURITY_VALIDATION_FILE_MAP_BEGIN(g_digiCheck)
CLTSECURITY_VALIDATION_FILE_MAP_ENTRY(V2AuthMap::szPartner, clt::security::ValidationPolicyDefault::fValDigitalSignature)
CLTSECURITY_VALIDATION_FILE_MAP_END()

//===============================================================================
//== DllMain == DllMain == DllMain == DllMain == DllMain == DllMain == DllMain ==
//===============================================================================
BOOL APIENTRY DllMain( HANDLE hModule, 
                       DWORD  ul_reason_for_call, 
                       LPVOID lpReserved
					 )
{
    USES_CONVERSION;
    bool bResult = TRUE;
	clt::CLicensingGateway::m_clsidObject = SYMSLC_OBJECTMAP_CLSID;

    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
        {
            // Performance Optimization: see MSDN docs on DisableThreadLibraryCalls() func.
            DisableThreadLibraryCalls((HMODULE)hModule);

            g_hModule = (HMODULE)hModule;

//            INIT_INSTALLHELPER_SUITEOWNERHMODULE(g_hModule);

            TCHAR szModuleLocation[_MAX_PATH*2] = {0};
            long nRet = ::GetModuleFileName(g_hModule,szModuleLocation,sizeof(szModuleLocation));
			::GetShortPathName(szModuleLocation,szModuleLocation,sizeof(szModuleLocation));
			g_sModuleFileName = szModuleLocation;
            if(nRet != 0)
            {
                // TODO: What's up with the usage of g_sModuleLocation?
                g_sModuleLocation = szModuleLocation;
                TCHAR* pLast = _tcsrchr(szModuleLocation,_T('\\'));
                *pLast = _T('\0');
                g_sModuleLocation = szModuleLocation;
            }
            INIT_LOG(_T("AVSTE"),g_hModule);

            clt::security::g_pSecurityValidationFileEntryMap = (clt::security::_SECURITY_VALIDATION_FILE_ENTRY*)&g_digiCheck;
            clt::security::g_pSecurityValidateRootPath = (LPCTSTR)g_sModuleLocation;
            _SimonModule.Init(ObjectMap);
            _SimonModuleInternal.Init(ObjectMapInternal);
            _initLicensingTechnology();
        }
        break;
    case DLL_PROCESS_DETACH:
        {
            _termLicensingTechnology();
            _SimonModuleInternal.Term();
            _SimonModule.Term();
        }
        break;
    }

    return bResult;
}


//=======================================================================
//== SimonGetClassObject == SimonGetClassObject == SimonGetClassObject ==
//=======================================================================

// this function must be implemented and exported from your library.
HRESULT SIMON_STDMETHODCALLTYPE SimonGetClassObject(
  REFSIMON_CLSID rclsid,    //CLSID for the class object
  REFSIMON_IID riid,        //Reference to the identifier of the interface 
                            // that communicates with the class object
  void** ppv               //Address of output variable that receives the 
                            // interface pointer requested in riid
)
{
    CLT_TRACEI(_T("Enter SimonGetClassObject"));
    return _SimonModule.GetClassObject(rclsid,riid,ppv);
}

//===================================================================================
//== SimonModuleGetLockCount == SimonModuleGetLockCount == SimonModuleGetLockCount ==
//===================================================================================
LONG SIMON_STDMETHODCALLTYPE SimonModuleGetLockCount(void)
{
    CLT_TRACEI(_T("Enter SimonModuleGetLockCount"));
    return _SimonModule.GetLockCount();
}


//==========================================================================
//== SimonGetClassIDArray == SimonGetClassIDArray == SimonGetClassIDArray ==
//==========================================================================
int SIMON_STDMETHODCALLTYPE SimonGetClassIDArray(SIMON_CLASSID_ARRAY* pv, long& nBufferSize)
{
    CLT_TRACEI(_T("Enter SimonGetClassIDArray"));
    return _SimonModule.GetClassIDArray(pv,nBufferSize);
}

//===================================================================================
//== SimonGetLibraryLocation == SimonGetLibraryLocation == SimonGetLibraryLocation ==
//===================================================================================
int SIMON_STDMETHODCALLTYPE SimonGetLibraryLocation(LPWSTR pv, long& nBufferSize)
{
    USES_CONVERSION;

    CLT_TRACEI(_T("SimonGetLibraryLocation: Enter"));

    if(nBufferSize < (_MAX_PATH + 2) )
    {
        nBufferSize = _MAX_PATH + 2;
        CLT_TRACEW(_T("SimonGetLibraryLocation: Buffer size too small, size needs to be at least %d."), nBufferSize);
        return 0; 
    }
    _ASSERTE(pv != NULL);
    if(pv == NULL)
    {
        CLT_TRACEE(_T("SimonGetLibraryLocation: Invalid argument, pv = %p"), pv);
        return -1;
    }

    char* pBuffer = new(std::nothrow) char[nBufferSize];
    if (!pBuffer)
    {
     CLT_TRACEE(_T("SimonGetLibraryLocation: Unable to allocate memory"));
     return -1;
    }
        
    memset(pBuffer, 0, nBufferSize);
    int nRet = 0;
    // What exceptions can be thrown here?
    try
    {
        nRet =::GetModuleFileNameA(g_hModule,pBuffer,nBufferSize);
        if (nRet == 0)
        {
            CLT_TRACEE(_T("SimonGetLibraryLocation: GetModlueFileNameA failed"));
            delete[] pBuffer;
            return nRet;
        }
        
        wcscpy(pv,A2W(pBuffer));
    }
    catch(...)
    {
        CLT_TRACEE(_T("SimonGetLibraryLocation: Unhandled exception"));
        nRet = 0;
    }
    
    delete[] pBuffer;

    CLT_TRACEI(_T("SimonGetLibraryLocation: Exit"));    

    return nRet;
}
