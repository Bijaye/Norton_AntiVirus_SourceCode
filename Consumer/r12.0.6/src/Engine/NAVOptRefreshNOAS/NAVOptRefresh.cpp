// NAVOptRefresh.cpp : Defines the entry point for the DLL application.
//

#include "stdafx.h"
#define INITIIDS
#if !defined(_NO_SIMON_INITGUID)
    #define SIMON_INITGUID
#endif

#include "plugin.h"         // ccApp plug-in
#include <SIMON.h>
#define INITGUID
#include <initguid.h>

// PEP Headers
#define _PEP_IMPL_
#define _DRM_V2AUTHMAP_IMPL_HYBRID     // Use the product map for the PEP handler and component map for the V2 caller
#define _CSIMONV2AUTHORIZATION
#include "ProductPEPHelper.h" 
#include "cltPepHandlers.h"
#include "PRODINFOPEPImpl.h"

#include "pepClientBase.h"

#include "cltLicenseHelper.h"

#define _CLT_SECURITY_IMPL
#include "cltSecurity.h"

#include "symscriptsafe_h.h"
#include "symscriptsafe_i.c"
#include "NAVLicense_h.h"

#define _INIT_COSVERSIONINFO
#include "SSOsinfo.h"
#include "resource.h"
#include "V2ResAuthGuids.h"


CComModule _Module;
HMODULE     g_hModule = NULL;
HINSTANCE   _g_hInstance = NULL;
CString		_csModuleLocation;
CString		_csModuleFileName;


// ccLib delay loader
// This will allow the ccLib dll to delay load at run-time
#include "ccLibDllLink.h"
#include "ccSymDelayLoader.h"
ccSym::CDelayLoader g_DelayLoader;

#include "ccSymDebugOutput.h"
ccSym::CDebugOutput g_DebugOutput(_T("NAVOptRf"));
IMPLEMENT_CCTRACE(g_DebugOutput);

#include "NAVSettingsHelperEx.h"

#include "NAVOptRefresh.h"
#include "NAVOptionRefreshEngine.h"
#include "NAVOptionRefresh.h"
#include "businessrules.h"
#include "NAVBusinessRules.h"

#include "IWPPrivateSettingsInterface.h"    // For IWP
#include "IWPPrivateSettingsLoader.h"

//============================================================================================
//
// SymInterface map for ccApp plug-in
//
//============================================================================================

// {79887527-FF57-455e-AE5A-E2A3153A647C}
SYM_DEFINE_OBJECT_ID(NAVOPTRF_OBJECT_ID, 
	0x79887527, 0xff57, 0x455e, 0xae, 0x5a, 0xe2, 0xa3, 0x15, 0x3a, 0x64, 0x7c);

DECLARE_CCAPP_PLUGIN(NAVOPTRF_OBJECT_ID)

SYM_OBJECT_MAP_BEGIN()                          
	SYM_OBJECT_ENTRY(NAVOPTRF_OBJECT_ID, CPlugin) 
SYM_OBJECT_MAP_END()

SIMON::CSimonModule _SimonModuleInternal;

//============================================================================================
//
// this is exposed to the outside world
//
//============================================================================================
typedef V2Authorization::CSimonV2AuthorizationT<&_g_hInstance,IDS_AUTH_GUIDS> CMySimonV2Authorization;
BEGIN_SIMON_OBJECT_MAP(ObjectMap)
    SIMON_OBJECT_ENTRY(CLSID_CNAVOptionRefreshEngine	        , CNAVOptionRefreshEngine)
    SIMON_OBJECT_ENTRY(CLSID_CNAVOptionRefresh                  , CNAVOptionRefresh)
	SIMON_OBJECT_ENTRY(NAV::CLSID_CNAVBusinessRules             , CBusinessRules)
	SIMON_OBJECT_ENTRY(V2Authorization::CLSID_V2Authorization   , CMySimonV2Authorization)
    SIMON_OBJECT_ENTRY(SIMON_PEP::CLSID_CPolicyEnforcement      , CProductPEPHandler) 
END_SIMON_OBJECT_MAP()
SIMON::CSimonModule _SimonModule;


CLTSECURITY_VALIDATION_FILE_MAP_BEGIN(_g_digiCheck)
	CLTSECURITY_VALIDATION_FILE_MAP_ENTRY(_T("DefAlert.dll"), 0)
	CLTSECURITY_VALIDATION_FILE_MAP_ENTRY(_T("NAVLUCBK.dll"), 0)
CLTSECURITY_VALIDATION_FILE_MAP_END()

void ListGUIDs(V2Authorization::_AUTHORIZATION_GUID_ENTRY * GUIDList, LPCSTR szDisplayName);

//===============================================================================
//== DllMain == DllMain == DllMain == DllMain == DllMain == DllMain == DllMain ==
//===============================================================================
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

            g_hModule      = (HMODULE)hModule;
            _g_hInstance    = (HINSTANCE)hModule;
            TCHAR _szModuleLocation[_MAX_PATH*2] = {0};
            long nRet =::GetModuleFileName(g_hModule,_szModuleLocation,sizeof(_szModuleLocation));
			::GetShortPathName(_szModuleLocation,_szModuleLocation,sizeof(_szModuleLocation));
			_csModuleFileName = _szModuleLocation;
            if(nRet != 0)
            {
                _csModuleLocation = _szModuleLocation;
                TCHAR* pLast = _tcsrchr(_szModuleLocation,_T('\\'));
                *pLast = _T('\0');
                _csModuleLocation = _szModuleLocation;
            }
            
			clt::security::g_pSecurityValidationFileEntryMap = (clt::security::_SECURITY_VALIDATION_FILE_ENTRY*)&_g_digiCheck;
			clt::security::g_pSecurityValidateRootPath = (LPCTSTR)_csModuleLocation;

            _Module.Init(NULL,HINSTANCE(hModule));
            _SimonModule.Init(ObjectMap);

			// CCTRACE the guid lists for debugging purposes
			ListGUIDs(ThisObjectAuthorizationMap, _T("GlobalThisMap"));
			ListGUIDs(V2AuthMap::ThisObjectAuthorizationMap, _T("ThisMap"));
			ListGUIDs(V2AuthMap::ThatObjectAuthorizationMap, _T("ThatMap"));
			ListGUIDs(V2AuthMap::PartnerObjectAuthorizationMap, _T("PartnerMap"));
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


//==========================================================================
//== SimonGetClassIDArray == SimonGetClassIDArray == SimonGetClassIDArray ==
//==========================================================================
int SIMON_STDMETHODCALLTYPE SimonGetClassIDArray(SIMON_CLASSID_ARRAY* pv, long& nBufferSize)
{
    return _SimonModule.GetClassIDArray(pv,nBufferSize);
}

//===================================================================================
//== SimonGetLibraryLocation == SimonGetLibraryLocation == SimonGetLibraryLocation ==
//===================================================================================
int SIMON_STDMETHODCALLTYPE SimonGetLibraryLocation(LPWSTR pv, long& nBufferSize)
{
    USES_CONVERSION;
    if(nBufferSize < (_MAX_PATH + 2) )
    {
        nBufferSize = _MAX_PATH + 2;
        return 0; 
    }
    _ASSERTE(pv != NULL);
    if(pv == NULL)
        return -1;

    char* pBuffer = new char[nBufferSize];
    ZeroMemory(pBuffer,nBufferSize);
    int nRet = 0;

    nRet =::GetModuleFileNameA(g_hModule,pBuffer,nBufferSize);
    if(nRet == 0)
        return nRet;

    wcscpy(pv,A2W(pBuffer));
    delete [] pBuffer;
    return nRet;
}

// Authenticate ourselves as the shipping (post-beta) NAVOPTRF.DLL
void GetAuthGUID(GUID& guid)
{
	guid = GUID_NAVOPTRF_AUTH;
}

void ListGUIDs(V2Authorization::_AUTHORIZATION_GUID_ENTRY GUIDList[], LPCSTR szDisplayName)
{
	int iMapIndex = 0;

	while(GUIDList[iMapIndex].pguid != NULL)
	{
		LPOLESTR tOleStr = NULL;
		StringFromCLSID(*GUIDList[iMapIndex].pguid, &tOleStr);

		if (tOleStr)
		{
			CCTRACEI(_T("ListGUIDs - %s[%d] = %S"), szDisplayName, iMapIndex, tOleStr);
			CoTaskMemFree(tOleStr);
		}
		else
		{
			CCTRACEI(_T("ListGUIDs - Failed to convert GUID %s[%d] to a string"), szDisplayName, iMapIndex);
			break;
		}

		iMapIndex++;
	}
}
