// NAVUI.cpp : Implementation of DLL Exports.


// Note: Proxy/Stub Information
//      To build a separate proxy/stub DLL, 
//      run nmake -f NAVUIps.mk in the project directory.
#include "stdafx.h"
#include "resource.h"
#define INITGUID
#include <initguid.h>
#include "ISymcobj.h"
#include "Symcguid.h"
#include "prodinfopephelper.h"
#define SIMON_INITGUID
#include <SIMON.h>

// PEP Headers
#define _PEP_IMPL_
#define _DRM_V2AUTHMAP_IMPL
#include "componentpephelper.h"
#include "pepClientBase.h" 


#define __CATID_SystemWorksPlugin

#include "NAVUI.h"
#include "NAVUI_i.c"

#include "SWMisc_i.c"

#define _INIT_COSVERSIONINFO
#include "SSOsinfo.h"

#define SIMON_INITGUID
#include "Simon.h"
#include "NavOptionRefreshHelperInterface.h"
#include "SSCoInitialize.h"

#include "NSWPluginView.h"
#include "NSWCategory.h"
#include "NSWAdvancedCategory.h"
#include "NSWStatusCategory.h"
#include "NSWScanCategory.h"
#include "NSWReportsCategory.h"
//#include "NSWSchedulingCategory.h"
#include "NSWFrame.h"
#include "NSWPluginView2002.h"

#include "Globals.h"

#include <stdio.h>

#include "NAVTrust.h"

#include "NavSettingsHelperEx.h"
#include "AVRESBranding.h"
#include "OptNames.h"
#include "NAVErrorResource.h"
#include "NavLicenseNames.h"
#include "CommandLine.h"

static HINSTANCE sg_hShell32;

CComModule _Module;
SIMON::CSimonModule _SimonModule;

//Resource loading code
#include "..\navuires\resource.h"
#include "ccResourceLoader.h"
#define SYMC_RESOURCE_DLL _T("navui.loc")
CAtlBaseModule _ModuleRes;
::cc::CResourceLoader g_ResLoader(&_ModuleRes, SYMC_RESOURCE_DLL);

// SymInterface map for ccVerifyTrust static lib
SYM_OBJECT_MAP_BEGIN()                          
SYM_OBJECT_MAP_END()                            

static HINSTANCE  _g_hPrimaryResourceInstance = NULL;
HINSTANCE _getPrimaryResourceInstance()
{
    HINSTANCE hInst = NULL;
    try
    {
        if(_g_hPrimaryResourceInstance == NULL)
        {
            TCHAR szPath[_MAX_PATH * 2] = {0};
            DWORD dwRet = ::GetModuleFileName(_Module.GetModuleInstance(),szPath,sizeof(szPath));
            if(dwRet > 0)
            {
                TCHAR* p = _tcsrchr(szPath,_T('\\'));
                *p = _T('\0');
                _tcscat(szPath,_T("\\AVRES.DLL"));
                _g_hPrimaryResourceInstance = (HINSTANCE)::LoadLibrary(szPath);
            }
        }
        hInst = _g_hPrimaryResourceInstance;
    }
    catch(...)
    {
        hInst = NULL;
    }
    return hInst;
}


BEGIN_OBJECT_MAP(ObjectMap)
OBJECT_ENTRY(CLSID_NSWPluginView		, CNSWPluginView)		// the new one
OBJECT_ENTRY(CLSID_NSWPluginView2002	, CNSWPluginView2002)	// legacy, hijacked the old CLSID_NSWPluginView
OBJECT_ENTRY(CLSID_NSWScanCategory		, CNSWScanCategory)
OBJECT_ENTRY(CLSID_NSWStatusCategory	, CNSWStatusCategory)
OBJECT_ENTRY(CLSID_NSWReportsCategory	, CNSWReportsCategory)
//OBJECT_ENTRY(CLSID_NSWSchedulingCategory, CNSWSchedulingCategory)
OBJECT_ENTRY(CLSID_NSWFrame				, CNSWFrame)
OBJECT_ENTRY(CLSID_NSWAdvancedCategory	, CNSWAdvancedCategory)
END_OBJECT_MAP()

/////////////////////////////////////////////////////////////////////////////

static bool g_SafeMode = false;

/////////////////////////////////////////////////////////////////////////////

void Scan()
{
	CString strExePath;
	CString strParameters;
	strExePath.Format(_T("%s\\navw32.exe"), g_NAVInfo.GetNAVDir());
	strParameters.Format(_T("/task:\"%s\\Tasks\\mycomp.sca\""), g_NAVInfo.GetNAVCommonDir());
	if (ShellExecute(::GetDesktopWindow(), _T("open"), strExePath, strParameters, NULL , SW_SHOW) <= (HINSTANCE)32)
	{
		CString strText, strFormat;
		g_ResLoader.LoadString(IDS_ERR_FAILED_LAUNCH_SCAN, strFormat);
		strText.Format(strFormat, g_csProductName, g_csProductName);
		MessageBox(NULL, strText, g_csProductName, MB_OK | MB_ICONERROR);
	}
}

/////////////////////////////////////////////////////////////////////////////
// DLL Entry Point

extern "C"
BOOL WINAPI DllMain(HINSTANCE hInstance, DWORD dwReason, LPVOID /*lpReserved*/)
{
    if (dwReason == DLL_PROCESS_ATTACH)
    {
       // Don't load if we are uninstalling!
        //
        LPCTSTR szName = _T("NAVInstallMutex");

        // Create the single instance lock
        TCHAR szEvent[_MAX_PATH];
        ZeroMemory(szEvent, sizeof(szEvent));

        if (g_OSInfo.IsTerminalServicesInstalled() != FALSE)
        {
            // Prepend the Global\ namespace when running on terminal server
            lstrcat(szEvent, _T("Global\\"));
        }
        lstrcat(szEvent, szName);
        HANDLE hInstanceLock = OpenMutex(SYNCHRONIZE, FALSE, szEvent);
        if (hInstanceLock != NULL)
        {
            // Mutex opened thus Install is running
            CloseHandle(hInstanceLock);
            hInstanceLock = NULL;
            return FALSE;
        }

        // Mutex exists thus Install is running


        _Module.Init(ObjectMap, hInstance, &LIBID_NAVUILib);
        DisableThreadLibraryCalls(hInstance);

        // Initializes the AxWin hosting code used by CWebWnd
        //
        AtlAxWinInit();
    }
    else if (dwReason == DLL_PROCESS_DETACH)
    {
        _Module.Term();
        CCTRACEI ( "Exiting" );
        if(_g_hPrimaryResourceInstance)
            ::FreeLibrary(_g_hPrimaryResourceInstance);
    }
    return TRUE;    // ok
}

/////////////////////////////////////////////////////////////////////////////
// Used to determine whether the DLL can be unloaded by OLE

STDAPI DllCanUnloadNow(void)
{
    return (_Module.GetLockCount()==0) ? S_OK : S_FALSE;
}

/////////////////////////////////////////////////////////////////////////////
// Returns a class factory to create an object of the requested type

STDAPI DllGetClassObject(REFCLSID rclsid, REFIID riid, LPVOID* ppv)
{
    g_SafeMode = ::GetSystemMetrics(SM_CLEANBOOT) != 0;

	if (g_SafeMode)
	{
		CBrandingRes BrandRes;
		g_csProductName = BrandRes.ProductName();

		CString csText, csFormat;

		g_ResLoader.LoadString(IDS_SAFE_MODE, csFormat);
		csText.Format(csFormat, g_csProductName);

		if (::MessageBox(NULL, csText, g_csProductName, MB_YESNO | MB_ICONQUESTION) == IDYES)
		{
			Scan();
		}
		// Call ExitProcess here to exit NMain before it displays 
		//  "Symantec Integrator could not initialize ..."
                ExitProcess(-1);
		return E_FAIL;
	}

    // Force load failure if the loading application or this module
    // is not signed by Symantec.

	static bool bTrustVerified = false;
	if (!bTrustVerified)
	{
        TCHAR szModuleName [MAX_PATH*2] = {0};
        GetModuleFileName (_Module.GetModuleInstance(), szModuleName, sizeof ( szModuleName ));
    
        CCTRACEI (szModuleName);
    
        if( NAVToolbox::NAVTRUST_OK != NAVToolbox::IsSymantecApplication() ||
            NAVToolbox::NAVTRUST_OK != NAVToolbox::IsSymantecSignedImage(szModuleName) )
        {
            ErrorMessageBox( ::GetDesktopWindow(), IDS_NAVERROR_INTERNAL_REINSTALL, IDS_ERR_SECURITY_FAILED );
		    // Call ExitProcess here to exit NMain before it displays 
		    //  "Symantec Integrator could not initialize ..."
		    ExitProcess(-1);
            return E_ACCESSDENIED;
        }
		    bTrustVerified = true;
	}

    return _Module.GetClassObject(rclsid, riid, ppv);
}

/////////////////////////////////////////////////////////////////////////////

#define SPRINTGUID(rgGUID, textout) wsprintf(textout, _T("{%08X-%04X-%04X-%02X%02X-%02X%02X%02X%02X%02X%02X}"), rgGUID.Data1, rgGUID.Data2, rgGUID.Data3, rgGUID.Data4[0], rgGUID.Data4[1], rgGUID.Data4[2], rgGUID.Data4[3], rgGUID.Data4[4], rgGUID.Data4[5], rgGUID.Data4[6], rgGUID.Data4[7]);
const LPTSTR _szCompList = _T("Component Categories\\%s");
const LPTSTR _szCompRegFormat = _T("CLSID\\%s\\Implemented Categories\\%s");

BOOL RegisterInCategory(const GUID& rgCatID, const CLSID& rgComponent, LPCTSTR pszCatTitle, BOOL bRegister)
{
	TCHAR szCatGuid[256];
	TCHAR szCompGuid[256];
	TCHAR szRegKey[2048];
	CRegKey regkey;

	// Create the text versions of the GUID
	SPRINTGUID(rgCatID, szCatGuid);
	SPRINTGUID(rgComponent, szCompGuid);

	if(bRegister)
	{
		// Create the category
		wsprintf(szRegKey, _szCompList, szCatGuid);
		if(regkey.Create(HKEY_CLASSES_ROOT, szRegKey) != ERROR_SUCCESS)
			return FALSE;

		if(RegSetValueEx(regkey, _T("409"), NULL, REG_SZ, (BYTE*)pszCatTitle, _tcslen(pszCatTitle)) != ERROR_SUCCESS)
		{
			return FALSE;
		}

		// Register the component in the category
		wsprintf(szRegKey, _szCompRegFormat, szCompGuid, szCatGuid);
		if(regkey.Create(HKEY_CLASSES_ROOT, szRegKey) != ERROR_SUCCESS)
		{
			return FALSE;
		}
	}
	else
	{
		// Remove the component from the category
		wsprintf(szRegKey, _szCompRegFormat, szCompGuid, szCatGuid);
		if(RegDeleteKey(HKEY_CLASSES_ROOT, szRegKey) != ERROR_SUCCESS)
			return FALSE;
	}

	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// DllRegisterServer - Adds entries to the system registry

STDAPI DllRegisterServer(void)
{
HRESULT hr;

    hr = _Module.RegisterServer(TRUE);

    if ( FAILED(hr) )
        return hr;

    if ( RegisterInCategory ( CATID_SystemWorksPlugin, CLSID_NSWPluginView2002,
                              szSystemworks_Plugin_Catid_Desc, TRUE ) )
	{
		// Write out a .NSI file that can be specified on the command line for integrator.exe
		TCHAR szFile[MAX_PATH * 2];

		GetModuleFileName(_Module.GetModuleInstance(), szFile, MAX_PATH);
		LPTSTR pszFile = _tcsrchr(szFile, '.');
		_tcscpy(pszFile, _T(".nsi") );
		_tcslwr(szFile);

		FILE* f = _tfopen ( szFile, _T("wb") );
		fwrite(&CLSID_NSWFrame.Data1, sizeof(CLSID_NSWFrame.Data1), 1, f);
		fwrite(&CLSID_NSWFrame.Data2, sizeof(CLSID_NSWFrame.Data2), 1, f);
		fwrite(&CLSID_NSWFrame.Data3, sizeof(CLSID_NSWFrame.Data3), 1, f);
		fwrite(&CLSID_NSWFrame.Data4, sizeof(CLSID_NSWFrame.Data4[0]), sizeof(CLSID_NSWFrame.Data4) / sizeof(CLSID_NSWFrame.Data4[0]), f);

		fclose(f);
	}
	
    return hr;
}

/////////////////////////////////////////////////////////////////////////////
// DllUnregisterServer - Removes entries from the system registry

STDAPI DllUnregisterServer(void)
{
    RegisterInCategory ( CATID_SystemWorksPlugin, CLSID_NSWPluginView,
                         szSystemworks_Plugin_Catid_Desc, FALSE );
    return _Module.UnregisterServer(TRUE);
}


extern "C" __declspec(dllexport) BOOL __stdcall FetchSupportURL(UINT iHint, LPTSTR lpsz)
{
	BOOL bRet = FALSE;

	STAHLSOFT_HRX_TRY(hr)
	{
		DJSMAR_LicenseType type = DJSMAR_LicenseType_Violated;

		CPEPClientBase pepBase;
		hrx << pepBase.Init();
		pepBase.SetContextGuid(clt::pep::CLTGUID);
		pepBase.SetComponentGuid(V2LicensingAuthGuids::SZ_ANTIVIRUS_COMPONENT);
		pepBase.SetPolicyID(clt::pep::POLICY_ID_QUERY_LICENSE_RECORD);
		pepBase.SetCollectionDWORD(clt::pep::SZ_PROPERTY_DISPOSITION,clt::pep::DISPOSITION_NONE);
		pepBase.SetCollectionDWORD(SUBSCRIPTION_STATIC::SZ_SUBSCRIPTION_PROPERTY_DISPOSITION,SUBSCRIPTION_STATIC::DISPOSITION_NONE);

		hrx << pepBase.QueryPolicy();

		hrx << pepBase.GetValueFromCollection(DRM::szDRMLicenseType, (DWORD &)type, DJSMAR_LicenseType_Violated);

		if(type == DJSMAR_LicenseType_Rental)
		{
			StahlSoft::CSmartDataPtr<BYTE> spBuff;		

			pepBase.GetCollectionData(DRM::szDRMSCSSTechsupportURL, spBuff);			

			_tcsncpy(lpsz, (LPCSTR) spBuff.m_p, MAX_PATH);

			bRet = TRUE;

			CCTRACEI(_T("SCSSTechSupportURL = %s"), lpsz);
		}
		else
		{
			// Get URL from AVRES.dll.
			CString csBuffer;
			if(0 < LoadString(_getPrimaryResourceInstance(), IDS_TECHSUPP_WEBSITE, csBuffer.GetBufferSetLength(MAX_PATH), MAX_PATH))
			{
				_tcscpy(lpsz, csBuffer);
				bRet = TRUE;
				CCTRACEI(_T("TechSupportURL = %s"), lpsz);
			}
		}
	}
	STAHLSOFT_HRX_CATCH_ALL_NO_RETURN(hr)

	return bRet;
}

////////////////////////////////////////////////////////////////////////////////
//Upsell(): the upsell function takes its hint and looks up the url, then
// using "intimate knowledge" of the product looks up the various parameters needed
// to be passed to the webserver. we assume lpsz is already allocted and is MAX_PATH TCHARS
//
extern "C" __declspec(dllexport) BOOL __stdcall Upsell(UINT iHint, LPTSTR lpsz)
{	
	//
	//Verify that lpsz is not NULL
	//
	if(NULL == lpsz)
	{
		return false;
	}
	
	//
	//Get the URL to the Digial Rivers upsell site
	//
	TCHAR szURL[MAX_PATH] = {0};
	if(0 == LoadString(_getPrimaryResourceInstance(), iHint, szURL, sizeof(szURL)))
	{
		return false;
	}
	
	//
	//Get the OEM and Version values from the NAV regkey
	//
	CRegKey key;
	BOOL bisOEM = FALSE;
	TCHAR szVersion[MAX_PATH] = {0};

	if(ERROR_SUCCESS == key.Open(HKEY_LOCAL_MACHINE, _T("Software\\Symantec\\Norton AntiVirus"), KEY_READ))
	{
		DWORD dwOEM = 0;
		if(ERROR_SUCCESS == key.QueryDWORDValue(_T("OEM"), dwOEM))
			bisOEM = (dwOEM != 0) ? TRUE : FALSE;

		DWORD dwSize = sizeof(szVersion)/sizeof(TCHAR);
		key.QueryStringValue(_T("version"), szVersion, &dwSize);
	}

	//
	//Get the System Locale
	//
	TCHAR szLocale[MAX_PATH] = {0};
	GetLocaleInfo(LOCALE_SYSTEM_DEFAULT, LOCALE_IDEFAULTLANGUAGE, szLocale, sizeof(szLocale));
	
	//
	//Get the OEMVendor and and Language
	//
	TCHAR szVersionDatPath[MAX_PATH] = {0};
	TCHAR szVendorName[MAX_PATH] = {0};
	TCHAR szLang[MAX_PATH] = {0};
	_tcscpy(szVersionDatPath, g_NAVInfo.GetNAVDir());
	_tcscat(szVersionDatPath, "\\Version.dat");
	GetPrivateProfileString(_T("Versions"), _T("OEMVendor"), _T(""), szVendorName, sizeof(szVendorName), szVersionDatPath);
	GetPrivateProfileString(_T("Versions"), _T("Language"), _T("EN"), szLang, sizeof(szLang), szVersionDatPath);
	
	
	wsprintf(lpsz, "%s?ProductName=%s&Version=%s&Language=%s&Type=%s&VendorName=%s&Locale=%s",
			/*URL*/ szURL,
			/*ProductName*/ _T("NortonAntiVirus"),
			/*ProductVersion*/ szVersion,
			/*ProductLang*/ szLang,
			/*ProductType*/	(bisOEM ? _T("OEM") : _T("RETAIL")),
			/*VendorName*/	szVendorName,
			/*CustomerLocale*/ szLocale);

	CCTRACEI(_T("NAVUI:: UpsellURL: %s"), lpsz);

	return true;
}

////////////////////////////////////////////////////////////////////////////////
//Upsell(): the upsell function takes its hint and looks up the url, then
// using "intimate knowledge" of the product looks up the various parameters needed
// to be passed to the webserver. we assume lpsz is already allocted and is MAX_PATH TCHARS
//
extern "C" __declspec(dllexport) BOOL __stdcall FetchURL(UINT iHint, LPTSTR lpsz)
{
    BOOL bRet = TRUE;
    try
    {
        CString csPath;
		TCHAR szBuffer[MAX_PATH] = {0};
        TCHAR szPath[_MAX_PATH * 2] = {0};
        DWORD dwRet = ::GetModuleFileName(_getPrimaryResourceInstance(),szPath,sizeof(szPath)/sizeof(TCHAR));
        if(dwRet > 0)
        {
            TCHAR* p = _tcsrchr(szPath,_T('\\'));
            *p = _T('\0');
        }

		if(0 == LoadString(_getPrimaryResourceInstance(), iHint, szBuffer, sizeof(szBuffer)/sizeof(TCHAR)))
			return FALSE;

        CString csUrl(szBuffer);
		csUrl.TrimLeft();
		csUrl.TrimRight();
		if(csUrl.GetLength() <= 0)
			return FALSE;

        StahlSoft::CCommandLine cmdLine;
        cmdLine.Parse(csUrl);
        
        CString csCommandString;
        bool bLocal = cmdLine.HasSwitch(_T("/local"));
        if(bLocal)
        {
			StahlSoft::StringType item;
            cmdLine.GetArgument(_T("/local"),0,item);
            csCommandString.Format( _T("%s\\%s") ,szPath,item.c_str());
            ::GetShortPathName(csCommandString,szPath,sizeof(szPath));
            csCommandString = szPath;
        }
        else
        {
            csCommandString = csUrl;
        }
        strcpy(lpsz,csCommandString);
    }
    catch(...)
    {
        bRet = FALSE;
    }
    return bRet;
}

