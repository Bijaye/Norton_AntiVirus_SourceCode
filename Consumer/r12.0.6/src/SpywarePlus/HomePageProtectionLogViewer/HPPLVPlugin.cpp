#include "StdAfx.h"
#include "resource.h"
#include "HPPLVInterface.h"

#include "HPPLVPlugin.h"
#include "HPPLVActivityLogCategory.h"

#include "NAVHelpLauncher.h"    // NAVToolbox

#include "SymHelp.h"

CHPPLVPlugin::CHPPLVPlugin(void)
{
}

CHPPLVPlugin::~CHPPLVPlugin(void)
{
}

HRESULT CHPPLVPlugin::GetPluginID(int& iPluginID)
{
    iPluginID = PluginId;
    return S_OK;
}

HRESULT CHPPLVPlugin::GetProductName(LPTSTR szProductName, 
                               DWORD& dwSize)
{
    CString cszProductName;

    g_Resources.LoadString(IDS_HPPLV_PLUGIN_NAME, cszProductName);

    _tcscpy(szProductName, cszProductName);
    dwSize = cszProductName.GetLength();

    return S_OK;
}	

HRESULT CHPPLVPlugin::GetStyleSheet(UINT& uID)
{
    uID = 0;
    return S_OK;
}

HRESULT CHPPLVPlugin::GetCategoryCount(int& iCategoryCount)
{
    iCategoryCount = 1;
    return S_OK;
}

HRESULT CHPPLVPlugin::GetCategory(int iIndex, cc::ILogViewerCategory*& pCategory)
{
    HRESULT hrReturn = E_FAIL;
    SYMRESULT sr;

    switch(iIndex)
    {
        case 0:
            {
                CHPPLVActivityLogCategory *pCat = new CHPPLVActivityLogCategory;
                if(!pCat)
                {
                    hrReturn = E_OUTOFMEMORY;
                    break;
                }
                
                if(!pCat->CreateEx())
                {
                    delete pCat;
                    hrReturn = E_UNEXPECTED;
                    break;
                }
                
                sr = pCat->QueryInterface(cc::IID_LogViewerCategory, (void**)&pCategory);
                if(SYM_FAILED(sr) || !pCategory)
                {
                    delete pCat;
                    pCategory = NULL;
                    hrReturn = E_NOINTERFACE;
                    break;
                }
                
                hrReturn = S_OK;
            }
            break;

        default:
            hrReturn = E_NOINTERFACE;
            break;

    }
    
    return hrReturn;
}

HRESULT CHPPLVPlugin::LaunchHelp()
{
		CCTRACEE("CHPPLVPlugin::LaunchHelp()");

		static const DWORD nHelpId = IDH_NAVW_CCLOGVIEW_BROWSER_CONFIG;

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

