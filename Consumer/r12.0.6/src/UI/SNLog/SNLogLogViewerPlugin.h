#pragma once

#include "ccLogViewerPluginHelper.h"
#include "SNLogSymNetiCategory.h"
#include "ccResourceLoader.h"

#include "SymHelp.h" // For some help defines.

#include "resource.h"
#include "..\SNLogRes\ResResource.h"
#include "ResourceHelper.h"
#include "NAVHelpLauncher.h"    // NAVToolbox

const LPCTSTR SNLOG_DLL   = _T("IWPLog.dll");

// Resource Loader
extern cc::CResourceLoader g_ResLoader;

//
//  todo: After next cc build, make sure this id is in the appropriate header files, and
//        remove the code segment below.
//			- HackMaster P
//
#pragma message("CC_IWP_PLUGIN_ID should be defined in ccLogViewerPluginId.h in common client area, not here.")
#ifndef CC_IWP_PLUGIN_ID
#define CC_IWP_PLUGIN_ID 6000
#endif

class CLogViewerPlugin :
	public CLogViewerPluginHelper
{
public:
    CLogViewerPlugin() {};
    virtual ~CLogViewerPlugin() {};

	DECLARE_LV_CATEGORY_MAP()
	DECLARE_LV_DLL_MAP()

private:
    CLogViewerPlugin(const CLogViewerPlugin&);
    CLogViewerPlugin& operator =(const CLogViewerPlugin&);

public:
    virtual HRESULT GetPluginID(int& iPluginID)
    {
        iPluginID = PluginId;
        return S_OK;
    }

    virtual HRESULT GetProductName(LPTSTR szProductName, 
                                   DWORD& dwSize)
	{
        tstring strProductName;
		TCHAR szTempString[MAX_PATH] = {0};

		if ( !g_ResLoader.LoadString (IDS_PRODUCT_NAME, szTempString, MAX_PATH))
            return E_FAIL;

		strProductName = szTempString;
        // Don't overrun the buffer
        //
        if (dwSize < strProductName.size())
            return E_FAIL;

        _tcsncpy ( szProductName, strProductName.c_str(), strProductName.size());
        dwSize = strProductName.size();
        return S_OK;
	}	

	virtual HRESULT GetStyleSheet(UINT& uID)
	{
		uID = 0;
		return S_OK;
	}

	virtual HRESULT CLogViewerPlugin::LaunchHelp()
	{
		CCTRACEE("CLogViewerPlugin::LaunchHelp()");

		static const DWORD nHelpId = IDH_NAVW_CCLOGVIEW_IWP;

		NAVToolbox::CNAVHelpLauncher Help;
		if (Help.LaunchHelp(nHelpId) == NULL)
		{
			CCTRACEE("Fail to LaunchHelp: %d. Try NAV Help", nHelpId);
			if (Help.LaunchHelp(0) != NULL)
			{
				CCTRACEE("Fail to NAV Help");
				return E_FAIL;
			}
		}
		return S_OK;
	}

protected:
    enum { PluginId = CC_IWP_PLUGIN_ID };
};
SYM_DEFINE_OBJECT_ID(SNLOG_PLUGIN_ID, 
	                 0x9ac3cdea, 0x6847, 0x4eec, 0x8a, 0xe, 0x55, 0x87, 0x6f, 0x7, 0x3, 0x65);
