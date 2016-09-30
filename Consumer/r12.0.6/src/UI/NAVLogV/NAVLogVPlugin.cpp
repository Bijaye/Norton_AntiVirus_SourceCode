// VirusListPlugin.cpp: implementation of the CVirusListPlugin class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "NAVLogVPlugin.h"
#include "ErrorCategory.h"
#include "resourcehelper.h"
#include "NAVLogV.h"
#include "AVccModuleId.h"
#include "AVccLogViewerPluginId.h"
#include "AVRESBranding.h"
#include <tchar.h>
#include "..\navlogvres\resource.h"


const char RAW_REGISTRY_KEY[]		=	"Software\\Symantec\\Norton AntiVirus";
const char RAW_REGISTRY_VALUE[]		=	"NAVLogVRaw";

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CNAVLogViewerPlugin::CNAVLogViewerPlugin()
{
    g_bRawMode = false;

	HKEY hKey;
	LONG lResult = RegOpenKeyEx(HKEY_LOCAL_MACHINE, RAW_REGISTRY_KEY, 0,
		KEY_READ, &hKey);

	if (lResult == ERROR_SUCCESS)
    {
	    DWORD dwValue = 0;
	    DWORD dwSize = sizeof(dwValue);

	    lResult = RegQueryValueEx(hKey, RAW_REGISTRY_VALUE, NULL, NULL,
		    (BYTE*) &dwValue, &dwSize);

	    RegCloseKey(hKey);

	    if (lResult == ERROR_SUCCESS)
        {
            if ( dwValue == 1 )
	            g_bRawMode = true;
        }
    }
}

CNAVLogViewerPlugin::~CNAVLogViewerPlugin()
{
}

HRESULT CNAVLogViewerPlugin::GetPluginID(int& iPluginID)
{
	iPluginID = CC_NAV_PLUGIN_ID;

	return S_OK;
}

HRESULT CNAVLogViewerPlugin::GetProductName(LPTSTR szProductName, 
	 				   /* [in/out] */ DWORD& dwSize)
{
	CBrandingRes BrandRes;
	CString csProductName(BrandRes.ProductName());
	if(csProductName.GetLength() > 0)
	{
		_tcscpy(szProductName, csProductName);
		dwSize = csProductName.GetLength();
		return S_OK;
	}

	return E_FAIL;

}

HRESULT CNAVLogViewerPlugin::GetCategoryCount(int& iCategoryCount)
{
	iCategoryCount = 3;
	
	return S_OK;
}

HRESULT CNAVLogViewerPlugin::GetCategory(int iIndex, cc::ILogViewerCategory*& pCategory)
{
   
    ISymFactory* pFactory = NULL;

    switch ( iIndex + 1 )
    {
    case CC_NAV_CAT_VIRUS_ALERT :
        GetFactory(NAV_VIRUS_ALERT_CATEGORY_OBJECT_ID, &pFactory);
        break;
    case CC_NAV_CAT_APP_ACTIVITY :
        GetFactory(NAV_APP_CATEGORY_OBJECT_ID, &pFactory);
        break;
    case CC_NAV_CAT_ERRORS :
        GetFactory(NAV_ERROR_CATEGORY_OBJECT_ID, &pFactory);
        break;
    default:
        return E_FAIL;
    }

    // Does an AddRef()
    //
    SYMRESULT hRes = pFactory->CreateInstance(cc::IID_LogViewerCategory, (void**)&pCategory);

    if (SYM_SUCCEEDED(hRes))
	{
		return S_OK;
	}
	return E_FAIL;
}

HRESULT CNAVLogViewerPlugin::GetStyleSheet(UINT& uID)
{
    uID = IDR_STYLESHEET;
    return S_OK;
}

HRESULT CNAVLogViewerPlugin::LaunchHelp()
{
    try
    {
    NAVToolbox::CNAVHelpLauncher NAVHelp;

    if ( NAVHelp.LaunchHelp ( IDH_NAVW_CCLOGVIEW_NAV ))
        return S_OK;
    }
    catch (...)
    {
    }

    return E_FAIL;
}
