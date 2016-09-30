// NAVSuiteOwner.cpp : Defines the entry point for the DLL application.
//

#include "stdafx.h"
#if !defined(_NO_SIMON_INITGUID)
    #define SIMON_INITGUID
#endif
#include <SIMON.h>

#define INITGUID
#include "initguid.h"
#define _CSIMONV2AUTHORIZATION
#define _V2AUTHORIZATION_SIMON          
#define INIT_V2AUTHORIZATION_IMPL
#define INIT_RSA_BSAFE_IMPL
#define INIT_RSA_PRIVATE_KEY
#define INIT_RSA_PUBLIC_KEY
#include "V2Authorization.h"

#define _INIT_COSVERSIONINFO
#include "DJSMAR00.h"
CComModule _Module;
#include "resource.h"
#include "DJSMAR00_CUSTOM.h"
HMODULE _g_hModule = NULL;
CString _csModuleLocation;
CString _csModuleFileName;
#define _INIT_DJSLICENSINGGATEWAY_IMPL
#include "DJSLicensingGateway.h"
#define _INIT_LICENSEOBJECT_DATA
#include "DJSMAR00_ObjectMap.h"

//============================================================================================
//
// this is OBJECTs for use internally to this DLL
//
//============================================================================================
BEGIN_SIMON_OBJECT_MAP(ObjectMapInternal)
	SIMON_OBJECT_ENTRIES
END_SIMON_OBJECT_MAP()

SIMON::CSimonModule _SimonModuleInternal;

//============================================================================================
//
// this is exposed to the outside world
//
//============================================================================================
BEGIN_SIMON_OBJECT_MAP(ObjectMap)
	SIMON_OBJECT_ENTRY(CLSID_CLicensingGateway  , DJSMAR::CLicensingGateway)
END_SIMON_OBJECT_MAP()
SIMON::CSimonModule _SimonModule;

namespace DJS_Security
{
void SecurityValidation(LPCTSTR lpcLicensingDLL , DWORD dwFlags)
{
}
}//namespace DJS_Security

//===============================================================================
//== DllMain == DllMain == DllMain == DllMain == DllMain == DllMain == DllMain ==
//===============================================================================
BOOL APIENTRY DllMain( HANDLE hModule, 
                       DWORD  ul_reason_for_call, 
                       LPVOID lpReserved
					 )
{
    USES_CONVERSION;
	DJSMAR::CLicensingGateway::m_clsidObject = DJSMAR00_OBJECTMAP_CLSID;

    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
        {
            _g_hModule = (HMODULE)hModule;
            TCHAR _szModuleLocation[_MAX_PATH*2] = {0};
            long nRet =::GetModuleFileName(_g_hModule,_szModuleLocation,_countof(_szModuleLocation));
			::GetShortPathName(_szModuleLocation,_szModuleLocation,_countof(_szModuleLocation));
			_csModuleFileName = _szModuleLocation;
            if(nRet != 0)
            {
                _csModuleLocation = _szModuleLocation;
                TCHAR* pLast = _tcsrchr(_szModuleLocation,_T('\\'));
                *pLast = _T('\0');
                _csModuleLocation = _szModuleLocation;
            }
            
            _Module.Init(NULL,HINSTANCE(hModule));
            _SimonModuleInternal.Init(ObjectMapInternal);
            _SimonModule.Init(ObjectMap);
        }
        break;
    case DLL_THREAD_ATTACH:
        {

        }
        break;
    case DLL_THREAD_DETACH:
        {

        }
        break;
    case DLL_PROCESS_DETACH:
        {
            _SimonModule.Term();
			_SimonModuleInternal.Term();
            _Module.Term();
        }
        break;
    }
    return TRUE;

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


