////////////////////////
//
// PROPRIETARY / CONFIDENTIAL.
// Use of this product is subject to license terms.
// Copyright © 2006 Symantec Corporation.
// All rights reserved.
//
/////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "ccLogViewerPluginHelper.h"
#include "SNLogSymNetiCategory.h"
#include "ccResourceLoader.h"

#include "SymHelp.h" // For some help defines.

#include "resource.h"
#include "..\SNLogRes\ResResource.h"
#include "ResourceHelper.h"
#include "SymHelpLauncher.h"    // NAVToolbox

const LPCTSTR SNLOG_DLL   = _T("IWPLog.dll");

// Resource Loader
extern cc::CResourceLoader g_ResLoader;
extern CAtlBaseModule _ModuleRes;

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

    virtual HRESULT GetProductName(LPSTR szProductName, 
                                   DWORD& dwSize)
	{
        return E_NOTIMPL;
	}	

	HRESULT GetProductNameW(LPWSTR szProductName, DWORD& dwSize)
	{
		
		std::wstring strProductName;
		
		
		TCHAR TempString[MAX_PATH];
		bool bRet = g_ResLoader.LoadString(IDS_PRODUCT_NAME, TempString,MAX_PATH);
		if(bRet )
		{
			strProductName = TempString;
			
			if (dwSize < strProductName.size())
				return E_FAIL;

			wcsncpy ( szProductName, strProductName.c_str(), strProductName.size());

			
			dwSize = (DWORD) strProductName.size();
			
			return S_OK;
		}
		
		return E_FAIL;
	}


	virtual HRESULT GetStyleSheet(UINT& uID)
	{
		uID = IDR_IWP_STYLESHEET;
		return S_OK;
	}

	virtual HRESULT CLogViewerPlugin::LaunchHelp()
	{
		CCTRCTXE0 (_T("CLogViewerPlugin::LaunchHelp()"));

		static const DWORD nHelpId = IDH_NAVW_CCLOGVIEW_IWP;

		isshared::CHelpLauncher Help;
		Help.LaunchHelp(nHelpId);
		return S_OK;
	}

protected:
    enum { PluginId = CC_IWP_PLUGIN_ID };
};
SYM_DEFINE_OBJECT_ID(SNLOG_PLUGIN_ID, 
	                 0x9ac3cdea, 0x6847, 0x4eec, 0x8a, 0xe, 0x55, 0x87, 0x6f, 0x7, 0x3, 0x65);
