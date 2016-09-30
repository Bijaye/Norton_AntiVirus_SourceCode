// DJSMAR00.cpp : Defines the entry point for the DLL application.
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
#include "SS_SimpleTextLogger.h"

StahlSoft::CSimpleTextLogger _g_simpleLogger;
void TraceLog(LPCTSTR lpcInfo,LPCTSTR lpszFormat, ...)
{
    va_list args;
    va_start(args, lpszFormat);

    int nBuf;
    TCHAR szBuffer[_MAX_PATH*4] = {0};

    nBuf = _vsntprintf(szBuffer, sizeof(szBuffer), lpszFormat, args);
    ATLASSERT(nBuf < sizeof(szBuffer)); //Output truncated as it was > sizeof(szBuffer)
    CString csTemp;
    DWORD dwID = ::GetCurrentThreadId();
    csTemp.Format(_T("TID(%08x),Log(%s)"),dwID,szBuffer);
    _g_simpleLogger.WriteLineText(csTemp);
    ::OutputDebugString(csTemp);
    va_end(args);
}

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
	SIMON_OBJECT_ENTRY(V2Authorization::CLSID_V2Authorization  ,  V2Authorization::CSimonV2Authorization) 
END_SIMON_OBJECT_MAP()
SIMON::CSimonModule _SimonModule;

namespace DJS_Security
{
void SecurityValidation(LPCTSTR lpcLicensingDLL , DWORD dwFlags)
{
#if defined(_DEBUG)
#define _NO_DIGISIG
#endif

#if !defined(_NO_DIGISIG)
    StahlSoft::HRX hrx;
    ///////////////////////////////////////////////////////////////////////
    // VALIDATE_DigitalSignature
    if( (dwFlags&VALIDATE_DigitalSignature) == VALIDATE_DigitalSignature)
    {
        //  Validate signature of the licensing library.
        //  If the file signature is not valid, treat same as ValidateBoundFile failed.
        hrx << ::IsSymantecSignedImage(lpcLicensingDLL);
    }
#endif// _NO_DIGISIG
}
}//namespace DJS_Security

// StahlSoft::CSmartDataPtr< DJSMAR::_SECURITY_VALIDATION_FILE_ENTRY > _g_spSecurityFileMap;

#include "ModuleFileName.h"
DJSSECURITY_VALIDATION_FILE_MAP_BEGIN(_g_digiCheck)
	DJSSECURITY_VALIDATION_FILE_MAP_ENTRY(_g_szModuleFileName,VALIDATE_DigitalSignature)
DJSSECURITY_VALIDATION_FILE_MAP_END()

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
	DJSMAR::CLicensingGateway::m_clsidObject = DJSMAR00_OBJECTMAP_CLSID;

    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
        {
            CString	csLogFile;
            ::DisableThreadLibraryCalls((HMODULE)hModule);
            _g_hModule = (HMODULE)hModule;
            TCHAR _szModuleLocation[_MAX_PATH*2] = {0};
            long nRet =::GetModuleFileName(_g_hModule,_szModuleLocation,sizeof(_szModuleLocation));
			::GetShortPathName(_szModuleLocation,_szModuleLocation,sizeof(_szModuleLocation));
			_csModuleFileName = _szModuleLocation;
            if(nRet != 0)
            {
                _csModuleLocation = _szModuleLocation;
                TCHAR* pLast = _tcsrchr(_szModuleLocation,_T('\\'));
                *pLast = _T('\0');
                _csModuleLocation = _szModuleLocation;
                csLogFile = _csModuleLocation;
                csLogFile += ("\\DJSMAR00.txt");
                _g_simpleLogger.put_LogFilePath(csLogFile);
                _g_simpleLogger.put_RegistryKey(_T("SymLCUI"));
            }
            TraceLog(_info,_T("DLL_PROCESS_ATTACH"));

			DJSMAR::_g_pSecurityValidationFileEntryMap = (DJSMAR::_SECURITY_VALIDATION_FILE_ENTRY*)&_g_digiCheck;
			DJSMAR::_g_pSecurityValidateRootPath = (LPCTSTR)_csModuleLocation;
			/*
			_g_spSecurityFileMap = new DJSMAR::_SECURITY_VALIDATION_FILE_ENTRY[2];
			_g_pSecurityValidationFileEntryMap = (DJSMAR::_SECURITY_VALIDATION_FILE_ENTRY*)_g_spSecurityFileMap;
			ZeroMemory(_g_pSecurityValidationFileEntryMap,(sizeof(DJSMAR::_SECURITY_VALIDATION_FILE_ENTRY)*2));
			DJSMAR::_SECURITY_VALIDATION_FILE_ENTRY* pItem = (DJSMAR::_SECURITY_VALIDATION_FILE_ENTRY*)_g_spSecurityFileMap;
			pItem->pszFile = (LPCTSTR)_csModuleFileName;
			pItem->dwFlags = VALIDATE_DigitalSignature;
*/
            _Module.Init(NULL,HINSTANCE(hModule));

            _SimonModule.Init(ObjectMap);
            _SimonModuleInternal.Init(ObjectMapInternal);
            _initLicensingTechnology();

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
            TraceLog(_info,_T("DLL_PROCESS_DETACH"));
           _termLicensingTechnology();
            _SimonModuleInternal.Term();
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
    try
    {
        nRet =::GetModuleFileNameA(_g_hModule,pBuffer,nBufferSize);
        if(nRet == 0)
            return nRet;
        
        wcscpy(pv,A2W(pBuffer));
    }
    catch(...)
    {}
    delete [] pBuffer;
    return nRet;
}

