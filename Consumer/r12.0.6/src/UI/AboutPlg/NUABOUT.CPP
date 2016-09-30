

// NuAbout.cpp : Implementation of DLL Exports.

// Note: Proxy/Stub Information
//      To build a separate proxy/stub DLL, 
//      run nmake -f NuAboutps.mk in the project directory.

#include "stdafx.h"
#include "resource.h"
#if !defined(_NO_SIMON_INITGUID)
#define SIMON_INITGUID
#endif
#include <SIMON.h>

#define INITGUID
#include "initguid.h"

// PEP Headers

#define _PEP_IMPL_
#define _V2AUTHORIZATION_SIMON
#include "ComponentPepHelper.h"
#include "cltPEPV2Map.h"
#include "prodinfopephelper.h"


#include "NuAbout.h"
#include "NuAbout_i.c"
#include "AboutProp_i.c"

#include "aboutplg.h"
#include "ISymcobj.h"
#include "Symcguid.h"
#include "NuAboutExtension.h"
#include "Navver.h"
#include "AVRESBranding.h"
#include "NAVOptions.h"

#define SZ_INSTALLEDAPPS_NAV_REG_KEY   "Software\\Symantec\\InstalledApps"
#define SZ_INSTALLEDAPPS_NAV_REG_VALUE "NAV"

BOOL UseHiColorBMP ( void );

// SymInterface map for ccVerifyTrust static lib
SYM_OBJECT_MAP_BEGIN()                          
SYM_OBJECT_MAP_END()                            

CComModule _Module;

BEGIN_OBJECT_MAP(ObjectMap)
OBJECT_ENTRY(CLSID_Nav2kAboutExtension, CNav2kAboutExtension)
END_OBJECT_MAP()
SIMON::CSimonModule _SimonModule;
CString _g_csHandlerPath;

/////////////////////////////////////////////////////////////////////////////
// DLL Entry Point

extern "C"
BOOL WINAPI DllMain(HINSTANCE hInstance, DWORD dwReason, LPVOID /*lpReserved*/)
{
    if (dwReason == DLL_PROCESS_ATTACH)
    {
        // Fetch the handler dll path
        ///////////////////////////////////////////////////////////////////////
        CString sModuleLocation;
        TCHAR _szModuleLocation[_MAX_PATH*2] = {0};
        long nRet =::GetModuleFileName(hInstance,_szModuleLocation,sizeof(_szModuleLocation));
        ::GetShortPathName(_szModuleLocation,_szModuleLocation,sizeof(_szModuleLocation));
        CString sModuleFileName = _szModuleLocation;
        if(nRet != 0)
        {
            sModuleLocation = _szModuleLocation;
            TCHAR* pLast = _tcsrchr(_szModuleLocation,_T('\\'));
            *pLast = _T('\0');
            sModuleLocation = _szModuleLocation;
        }
        _Module.Init(ObjectMap, hInstance, &LIBID_Nav2kABOUTLib);
        DisableThreadLibraryCalls(hInstance);
    }
    else if (dwReason == DLL_PROCESS_DETACH)
        _Module.Term();
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
    return _Module.GetClassObject(rclsid, riid, ppv);
}

/////////////////////////////////////////////////////////////////////////////
// DllRegisterServer - Adds entries to the system registry

STDAPI DllRegisterServer(void)
{
    // registers object, typelib and all interfaces in typelib
    return _Module.RegisterServer(TRUE);
}

/////////////////////////////////////////////////////////////////////////////
// DllUnregisterServer - Removes entries from the system registry

STDAPI DllUnregisterServer(void)
{
    return _Module.UnregisterServer(TRUE);
}

/////////////////////////////////////////////////////////////////////////////
// DoStdAbout - Export to display the About Box (for NU modules)

void WINAPI DoStdAbout(PLUGINABOUTSTRUCT* pAboutInfo)
{
	ISymCAboutBox* pAboutBox = NULL;

	if ( SUCCEEDED ( CoInitialize(NULL)))
	{
		HRESULT hResult = CoCreateInstance(CLSID_SymCAboutBox, NULL, CLSCTX_INPROC_SERVER,
			IID_ISymCAboutBox, (void**) &pAboutBox);

		if (SUCCEEDED(hResult))
		{
			// load the logo strip

			HANDLE hImage;

			if ( UseHiColorBMP() )
			{
				// Use the 256-color one
				hImage = LoadBitmap(_Module.GetModuleInstance(),
					MAKEINTRESOURCE(IDB_ABOUT_PAGE1));
			}
			else
			{
				// Use the 16-color one
				hImage = LoadBitmap(_Module.GetModuleInstance(),
					MAKEINTRESOURCE(IDB_ABOUT_PAGE1_16));
			}
            
            ATL::CString sGUID = V2LicensingAuthGuids::SZ_NAV_SUITEOWNERGUID;
            pAboutBox->SetProductPEPHandler(sGUID.AllocSysString());

			// setup the About Box parameters & invoke it
			
			CBrandingRes BrandRes;
			pAboutBox->SetImage(hImage, IMAGE_BITMAP);
			pAboutBox->AddExtension(CLSID_Nav2kAboutExtension);
			pAboutBox->ShowSytemInfoPage();

			if ( pAboutInfo )
			{
				pAboutBox->Invoke(pAboutInfo->hWndOwner, 0L);
			}
			else
			{
				pAboutBox->Invoke(NULL, 0L);
			}
			pAboutBox->Release();
		}

		CoUninitialize();
	}
}


BOOL UseHiColorBMP ( void )
{
    HDC   hDC = GetDC ( ::GetDesktopWindow() );
    int   nColorDepth;
    DWORD dwDisplayableColors;

    BOOL bUseHiColorBmps = FALSE;

    if ( GetDeviceCaps ( hDC, RASTERCAPS ) & RC_PALETTE )
        {
        dwDisplayableColors = GetDeviceCaps ( hDC, SIZEPALETTE );
        bUseHiColorBmps = ( dwDisplayableColors > 256 );
        }
    else
        {
        nColorDepth = GetDeviceCaps ( hDC, BITSPIXEL ) *
                      GetDeviceCaps ( hDC, PLANES );

        if ( nColorDepth  >= 32 )
            dwDisplayableColors = 0xFFFFFFFF;
        else
            dwDisplayableColors = 1L << nColorDepth;

        bUseHiColorBmps = ( dwDisplayableColors > 256 );
        }

    ReleaseDC ( ::GetDesktopWindow(), hDC );

    return bUseHiColorBmps;
}                                       // End UseHiColorBMP ()

