// PEPHandler.cpp : Defines the entry point for the DLL application.
//

#include "stdafx.h"
#define _PEP_V2AUTHMAP_IMPL
#if !defined(_NO_SIMON_INITGUID)
    #define SIMON_INITGUID
#endif
#define INITIIDS
#include "SymInterface.h"
#include <DRMTrustHelper.h>

#include "simon.h"
#include <initguid.h>
#define _INIT_V2LICENSINGAUTHGUIDS
#define INIT_RSA_BSAFE_IMPL
#define INIT_RSA_PRIVATE_KEY
#define INIT_RSA_PUBLIC_KEY
#include "V2LicensingAuthGuids.h"
#define _V2AUTHORIZATION_SIMON
#define INIT_V2AUTHORIZATION_IMPL
#include "V2Authorization.h"
#define _SIMON_PEP_
#include "PolicyEnforcement_Simon.h"
CComModule _Module;
#define _DRM_V2AUTHMAP_IMPL
#define _AV_COMPONENT_CLIENT
#include "ComponentPepHelper.h"
#define PRODUCTPEPHELPER_H
#include "AleComponentPepHelper.h"
#include "cltPepConstants.h"
#include "PepMiddleMan.h"
#include "V2LicensingAuthGuids.h"

#include "ccSymDelayLoader.h"
ccSym::CDelayLoader symDelayLoader;

#include "ccSymDebugOutput.h"
#define _NAVOPTREFRESH_CONSTANTS
#include "NAVOptRefresh.h"

ccSym::CDebugOutput g_DebugOutput(_T("avCompbr"));
IMPLEMENT_CCTRACE(::g_DebugOutput);

// SymInterface map for ccVerifyTrust static lib
SYM_OBJECT_MAP_BEGIN()
SYM_OBJECT_MAP_END()

HMODULE g_hModule = NULL;
CString g_sModuleLocation;
CString g_sModuleFileName;

TCHAR szAVComponentPathOwners[] = _T("SOFTWARE\\Symantec\\CCPD\\SuiteOwners\\{0D7956A2-5A08-4ec2-A72C-DF8495A66016}\\Owners");
TCHAR szAVComponentPath[]       = _T("SOFTWARE\\Symantec\\CCPD\\SuiteOwners\\{0D7956A2-5A08-4ec2-A72C-DF8495A66016}");

// Macro that declares standard DRM handler functions.
DECLARE_CLT_PEP_HANDLERS

// Pep handler MAP for AntiVirus component
PEPHANDLER_BEGIN(_PepItem)
	PEPHANDLER_ENTRY(&V2LicensingAuthGuids::GUID_ALE_COMPONENT          ,ALEComponent::POLICY_ID_PAID_CONTENT_ALE_UPDATES       ,BasicDRMMiddleMan::AccessAllowedQueryPolicy<szAVComponentPathOwners>)
	PEPHANDLER_ENTRY(&V2LicensingAuthGuids::GUID_AntiVirus_Component    ,AntiVirusComponent::POLICY_ID_PAID_CONTENT_VDEF_UPDATES,BasicDRMMiddleMan::AccessAllowedQueryPolicy<szAVComponentPathOwners>)
	PEPHANDLER_ENTRY(&clt::pep::CLTGUID, clt::pep::POLICY_ID_QUERY_ABOUT_BOX_TEXT                ,BasicDRMMiddleMan::SeedOnlyQueryPolicy<szAVComponentPathOwners>)
    PEPHANDLER_ENTRY(&clt::pep::CLTGUID, clt::pep::POLICY_ID_QUERY_PRODUCT_STATE                 ,BasicDRMMiddleMan::AccessAllowedQueryPolicy<szAVComponentPathOwners>)
    PEPHANDLER_ENTRY(&clt::pep::CLTGUID, clt::pep::POLICY_ID_QUERY_SUBSCRIPTION_RECORD           ,BasicDRMMiddleMan::SeededQueryPolicy<szAVComponentPath>)
    PEPHANDLER_ENTRY(&clt::pep::CLTGUID, clt::pep::POLICY_ID_ACTIVATE_SUBSCRIPTION               ,BasicDRMMiddleMan::SeededQueryPolicy<szAVComponentPath>)
    PEPHANDLER_ENTRY(&clt::pep::CLTGUID, clt::pep::POLICY_ID_REGISTER_SUBSCRIPTION               ,BasicDRMMiddleMan::SeededQueryPolicy<szAVComponentPath>)
    PEPHANDLER_ENTRY(&clt::pep::CLTGUID, clt::pep::POLICY_ID_QUERY_SUBSCRIPTION_FORCE_RENEWAL    ,BasicDRMMiddleMan::SeededQueryPolicy<szAVComponentPath>)
    PEPHANDLER_ENTRY(&clt::pep::CLTGUID, clt::pep::POLICY_ID_SUBSCRIPTION_MANAGEMENT             ,BasicDRMMiddleMan::SeededQueryPolicy<szAVComponentPath>)
    PEPHANDLER_ENTRY(&clt::pep::CLTGUID, clt::pep::POLICY_ID_QUERY_LICENSE_RECORD				 ,BasicDRMMiddleMan::SeededQueryPolicy<szAVComponentPath>)
    PEPHANDLER_ENTRY(&clt::pep::CLTGUID, clt::pep::POLICY_ID_CACHE_UPDATE_NOTIFY                 ,clt::pep::HandleCacheUpdate)
PEPHANDLER_END()

class CPEPSinkObjectTest  :
	public SIMON_PEP::CPEPSinkObjectT2V2<CPEPSinkObjectTest
    ,(V2Authorization::_AUTHORIZATION_GUID_ENTRY*)&(DRMPEPSinkMustHaveThisMap),(SIMON_PEP::PEPHANDLER_ITEM*)&_PepItem>  
{
public:
};

//============================================================================================
//
// this is exposed to the outside world
//
//============================================================================================
BEGIN_SIMON_OBJECT_MAP(ObjectMap)
	SIMON_OBJECT_ENTRY(SIMON_PEP::CLSID_CPolicyEnforcement, CPEPSinkObjectTest) 
END_SIMON_OBJECT_MAP()
SIMON::CSimonModule _SimonModule;

//===============================================================================
//== DllMain == DllMain == DllMain == DllMain == DllMain == DllMain == DllMain ==
//===============================================================================
BOOL APIENTRY DllMain( HANDLE hModule, 
                       DWORD  ul_reason_for_call, 
                       LPVOID lpReserved
					 )
{
    USES_CONVERSION;

    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
        {
            // Performance Optimization: see MSDN docs on DisableThreadLibraryCalls() func.
            DisableThreadLibraryCalls((HMODULE)hModule);
            
            g_hModule = (HMODULE)hModule;
            TCHAR _szModuleLocation[_MAX_PATH*2] = {0};
            long nRet =::GetModuleFileName(g_hModule,_szModuleLocation,_txcountof(_szModuleLocation));
			::GetShortPathName(_szModuleLocation,_szModuleLocation,_txcountof(_szModuleLocation));
			g_sModuleFileName = _szModuleLocation;
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

namespace clt
{
    namespace pep
    {
    /////////////////////////////////////////////////////////////////////////////////////////////
    // Function HandleCacheUpdate
    //
    // Description:
    // NAV specific handler for cache updates
    /////////////////////////////////////////////////////////////////////////////////////////////
    inline HRESULT HandleCacheUpdate(PVOID pThis, SIMON_COLLECTION::IDataCollection* pCollection)
    {
        // Tell main UI to refresh license status
        CCTRACEI("AVCompBR::HandleCacheUpdate - fire event to NavOptRf to refresh cache");

        StahlSoft::CSmartHandle shRefreshUI = ::CreateEvent(NULL,FALSE,FALSE,szNAVOptionRefreshEngine_OnDemandRefreshEvent);
        ::SetEvent(shRefreshUI);

        return S_OK;
    }
    } // end: namespace pep
}// end namespace clt