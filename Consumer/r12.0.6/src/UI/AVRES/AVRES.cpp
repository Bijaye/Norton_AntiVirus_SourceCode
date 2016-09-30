// AVRES.cpp : Defines the entry point for the DLL application.
//

#include "stdafx.h"
#if !defined(_NO_SIMON_INITGUID)
#define SIMON_INITGUID
#endif
#define INITGUID
#include "initguid.h"
#define _CSIMONV2AUTHORIZATION

#include "StahlSoft.h"
#define _INIT_COSVERSIONINFO
#include "SSOsinfo.h"

#define INITIIDS

#include <SIMON.h>
#include "stdlib.h"
#include "V2ResAuthGuids.h"
#define _V2AUTHORIZATION_SIMON          
#define INIT_V2AUTHORIZATION_IMPL
#define INIT_RSA_BSAFE_IMPL
#define INIT_RSA_PRIVATE_KEY
#define INIT_RSA_PUBLIC_KEY
#include "V2Authorization.h"
#include "AVRES.h"
#include "V2LicensingAuthGuids.h"

HINSTANCE g_hInstance = NULL;
HMODULE g_hModule = NULL;
CString g_sModuleLocation;
CString g_sModuleFileName;
CComModule _Module;
//============================================================================================
//
// this is exposed to the outside world
//
//============================================================================================
typedef V2Authorization::CSimonV2AuthorizationT<&g_hInstance,IDS_AUTH_GUIDS> CMySimonV2Authorization;

BEGIN_SIMON_OBJECT_MAP(ObjectMap)
	SIMON_OBJECT_ENTRY(V2Authorization::CLSID_V2Authorization  ,  CMySimonV2Authorization ) 
END_SIMON_OBJECT_MAP()
SIMON::CSimonModule _SimonModule;

BOOL APIENTRY DllMain( HANDLE hModule, 
                       DWORD  ul_reason_for_call, 
                       LPVOID lpReserved
					 )
{
    USES_CONVERSION;
    BOOL bResult = TRUE;

    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
        {
            // Performance Optimization: see MSDN docs on DisableThreadLibraryCalls() func.
            DisableThreadLibraryCalls((HMODULE)hModule);

            g_hInstance = (HINSTANCE) hModule;
            g_hModule = (HMODULE)hModule;
            TCHAR _szModuleLocation[_MAX_PATH*2] = {0};
            long nRet =::GetModuleFileName(g_hModule,_szModuleLocation,sizeof(_szModuleLocation));
            if(nRet != 0)
            {
                g_sModuleLocation = _szModuleLocation;
                TCHAR* pLast = _tcsrchr(_szModuleLocation,_T('\\'));
                *pLast = _T('\0');
                g_sModuleLocation = _szModuleLocation;
            }
            
            _Module.Init(NULL,HINSTANCE(hModule));
            _SimonModule.Init(ObjectMap);

        }
        break;

    case DLL_PROCESS_DETACH:
        {
            _SimonModule.Term();
            _Module.Term();
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
    return _SimonModule.GetClassObject(rclsid,riid,ppv);
}

//===================================================================================
//== SimonModuleGetLockCount == SimonModuleGetLockCount == SimonModuleGetLockCount ==
//===================================================================================
LONG SIMON_STDMETHODCALLTYPE SimonModuleGetLockCount(void)
{
    return _SimonModule.GetLockCount();
}

