// NSWPluginView.cpp : Implementation of CNSWPluginView
#include "stdafx.h"
#define __NSWIntegrator_Names_Impl
#include "NAVUI.h"
#include "globals.h"
#include "OSInfo.h"     // Toolbox
#include "IEUtils.h"
#include "Globals.h"
#include "SWMisc.h"
#include "SymLTCHK.h"
#include "SymLTCHK_i.c"
#include "DRMNamedProperties.h"
#include "Simon.h"
#include "NavOptionRefreshHelperInterface.h"
#define NSW_CAT_LIST
#include "NSWPluginView.h"
#include "OEMProductIntegrationDefines.h"
#include "NAVLicenseNames.h"
#import "NavUI.tlb"
#include "NAVTRust.h"
#include "GlobalEvents.h"
#define _NAVOPTREFRESH_CONSTANTS
#include "NavOptRefresh.h"
#include "NAVHelpLauncher.h"
#include "NAVLicenseNames.h"
#include "ccVersionInfo.h" // For preSuite2004
#include "StartNAVcc.h"

#include "winver.h" // For NSW version info
#include "ccResourceLoader.h"
#include "..\navuires\resource.h"
extern cc::CResourceLoader g_ResLoader;

static const COLORREF g_szcrSubItemColor = RGB(255, 0, 0);
StahlSoft::CSmartModuleHandle g_smLibNavOptRefresh;

/////////////////////////////////////////////////////////////////////////////
// CNSWPluginView

CNSWPluginView::CNSWPluginView()
{
}
HRESULT CNSWPluginView::FinalConstruct()
{
    // Make sure ccApp and other CC components are alive and well
    NAVToolbox::CStartNAVcc cNAVccRepair;
    if( !cNAVccRepair.StartCC(true) )
        CCTRACEE(_T("CNSWFrame::FinalConstruct() - cNAVccRepair.StartCC(true) returned false."));
    else
        CCTRACEI(_T("CNSWFrame::FinalConstruct() - cNAVccRepair.StartCC(true) succeeded."));

	STAHLSOFT_HRX_TRY(hr)
	{
		hrx << CBrandingImpl::Initialize();
		hrx << CBrandingImpl::SetValue(NSWPRODUCT_SUBITEM_COLOR, g_szcrSubItemColor);

		// Check to see if this is NAV Professional.

		m_bProEdition = g_NAVInfo.IsNAVProfessional();

		// Create a mutex that other apps can check to see if we are running.
		// Note that this isn't used for multiple-instance checking, since
		// the integrator already handles that.
		//
		m_shMutex = CreateMutex(NULL, TRUE, _T("NAV_Integrator_Plugin_Mutex"));

		GetLicensingInfo();

		// Get product name from AVRES
		if(g_csProductName.IsEmpty())
			g_csProductName.LoadString(_getPrimaryResourceInstance(), IDS_AV_PRODUCT_NAME);

		// Create the NAV Opt Refresh Object
		//  This object is used to force a licensing refresh
		TCHAR szNavOptRf[_MAX_PATH*2] = {0};

		::wsprintf(szNavOptRf,_T("%s\\NAVOPTRF.DLL"), g_NAVInfo.GetNAVDir());
		hrx << NAVToolbox::IsSymantecSignedImage(szNavOptRf);
		g_smLibNavOptRefresh = ::LoadLibrary(szNavOptRf);
		hrx << SIMON::CreateInstanceByDLLInstance(g_smLibNavOptRefresh,CLSID_CNAVOptionRefresh, IID_INAVOptionRefresh, (void**)&m_spOptionRefresh);

        put_Item(_variant_t(_bstr_t(DRM::szDRMSuiteOwner))		,_variant_t(_bstr_t(V2LicensingAuthGuids::SZ_NAV_SUITEOWNERGUID)));
        put_Item(_variant_t(_bstr_t(DRM::szDRMComponentGUID))	,_variant_t(_bstr_t(V2LicensingAuthGuids::SZ_ANTIVIRUS_COMPONENT)));
		put_Item(_variant_t(_bstr_t(DRM::szDRMSuiteOwnerName))	,_variant_t(_bstr_t(g_csProductName)));
		put_Item(_variant_t(_bstr_t(DRM::szDRMLicenseType))		,_variant_t(LONG(g_LicenseType)));
		put_Item(_variant_t(_bstr_t(LiveSubscribe::szServiceId)),_variant_t(_bstr_t(SZ_NAV_LS_SERVICEID)));


		unsigned char * pszCLSID = NULL;
		UuidToString((UUID*)&CLSID_SymLTCategoryHook,&pszCLSID);
		CString csTemp(pszCLSID);
		_bstr_t varBstr((LPCTSTR)csTemp);
		put_Item(_variant_t(_bstr_t(szNSWIntegrator_CategoryHookCLSID)),_variant_t(varBstr));
		::RpcStringFree(&pszCLSID);
	}
	STAHLSOFT_HRX_CATCH_ALL_NO_RETURN(hr);
	_ASSERTE(SUCCEEDED(hr));
	return hr;
}

CNSWPluginView::~CNSWPluginView()
{
}

void CNSWPluginView::FinalRelease()
{
	STAHLSOFT_HRX_TRY(hr)
	{
		m_spOptionRefresh.Release();
	}
	STAHLSOFT_HRX_CATCH_ALL_NO_RETURN(hr);    
}

STDMETHODIMP CNSWPluginView::GetInstance(/*[out, retval]*/ HINSTANCE* phInstance)
{
	STAHLSOFT_HRX_TRY(hr)
	{
		*phInstance = NULL;
		*phInstance = _getPrimaryResourceInstance();
	}
	STAHLSOFT_HRX_CATCH_ALL_RETURN(hr);    
}

STDMETHODIMP CNSWPluginView::GetProductNameID(/*[out, retval]*/ UINT* pnProductNameID)
{
	STAHLSOFT_HRX_TRY(hr)
	{
		*pnProductNameID = 0;
		*pnProductNameID = IDS_PRODUCTNAME;
	}
	STAHLSOFT_HRX_CATCH_ALL_RETURN(hr);    
}

STDMETHODIMP CNSWPluginView::GetProductBitmapID(/*[in]*/ SYMSW_BITMAPNO eBitmapNo, /*[out, retval]*/ UINT* pnProductBitmapID)
{
	STAHLSOFT_HRX_TRY(hr)
	{
		switch(eBitmapNo)
		{
		case eSymSWBitmap_Up:
			//&$ *pnProductBitmapID = IDB_CATEGORY_BTN_UP;
			break;

		case eSymSWBitmap_Over:
			//&$ *pnProductBitmapID = IDB_CATEGORY_BTN_HOT;
			break;

		case eSymSWBitmap_Down:
			//&$ *pnProductBitmapID = IDB_CATEGORY_BTN_DOWN;
			break;

		case eSymSWBitmap_Disabled:
			//&$ *pnProductBitmapID = IDB_CATEGORY_BTN_UP;
			break;

		default:
			return E_FAIL;
			break;
		}
	}
	STAHLSOFT_HRX_CATCH_ALL_RETURN(hr);    
}

STDMETHODIMP CNSWPluginView::GetProductOrder(/*[out, retval]*/ UINT* pnOrderLoc)
{
	STAHLSOFT_HRX_TRY(hr)
	{
		*pnOrderLoc = PLUG_ORDER;
	}
	STAHLSOFT_HRX_CATCH_ALL_RETURN(hr);    
}

STDMETHODIMP CNSWPluginView::HasOptions(/*[out, retval]*/ BOOL* pbHasOptions)
{
	STAHLSOFT_HRX_TRY(hr)
	{	
    	*pbHasOptions = TRUE;
	}
	STAHLSOFT_HRX_CATCH_ALL_RETURN(hr);    
}

STDMETHODIMP CNSWPluginView::DoOptions(/*[in]*/ HWND hParentWnd)
{
	STAHLSOFT_HRX_TRY(hr)
	{
		BOOL bRet = DoOptionsScreen ( hParentWnd );
		hrx << (bRet?S_OK:E_FAIL);
	}
	STAHLSOFT_HRX_CATCH_ALL_RETURN(hr);    
}

STDMETHODIMP CNSWPluginView::HasHelp(/*[out, retval]*/ BOOL* pbHasHelp)
{
	STAHLSOFT_HRX_TRY(hr)
	{
		if ( !g_bStandAlone && getPre2004Suite ())
            *pbHasHelp = TRUE;
        else
            *pbHasHelp = FALSE;
	}
	STAHLSOFT_HRX_CATCH_ALL_RETURN(hr);    
}

STDMETHODIMP CNSWPluginView::DoHelp(/*[in]*/ int nIndex, /*[in]*/ HWND hParentWnd)
{
	// This Help gets called when we are not-standalone, like in NSW or NIS.
	//
	STAHLSOFT_HRX_TRY(hr)
	{
		NAVToolbox::CNAVHelpLauncher NAVHelp;
		BOOL bRet = (NULL != NAVHelp.LaunchHelp ( nIndex, hParentWnd ));
		hrx << (bRet?S_OK:E_FAIL);
	}
	STAHLSOFT_HRX_CATCH_ALL_RETURN(hr);    
}

STDMETHODIMP CNSWPluginView::ResetCategoryEnum()
{
	STAHLSOFT_HRX_TRY(hr)
	{
		m_nEnumLoc = 0;
	}
	STAHLSOFT_HRX_CATCH_ALL_RETURN(hr);    
}

STDMETHODIMP CNSWPluginView::GetNextCategory(/*[in]*/ BOOL bCreate, /*[out, retval]*/ INSWCategory** ppCategory)
{

	STAHLSOFT_HRX_TRY(hr)
	{		
		// Make sure IE 4.01 + is present
		//
		NAVToolbox::CIEUtils IE;

		if ( IE.m_iMajorVersion < 4 )
		{
			WarningMessageBox ( ::GetDesktopWindow (), IDS_NO_IE, MB_ICONSTOP );
			hrx << E_FAIL;
		}

		if (!m_bProEdition)
		{
			if(m_nEnumLoc >= sizeof(_pclsCategories) / sizeof(_pclsCategories[0]))
				hrx << E_FAIL;
		}
		else
		{
			if(m_nEnumLoc >= sizeof(_pclsCategoriesPro) / sizeof(_pclsCategoriesPro[0]))
				hrx << E_FAIL;	
		}

		if(bCreate == FALSE)
		{
			if(ppCategory != NULL)
				hrx << E_FAIL;

			++m_nEnumLoc;
		}
		else if(bCreate == TRUE)
		{
			if(ppCategory == NULL)
				hrx << E_FAIL;

			HRESULT hrRes = S_OK;

			if (!m_bProEdition)
			{
				// Check the plugin dll for a valid symantec signature
				if( NAVToolbox::NAVTRUST_OK != NAVToolbox::IsSymantecComServer(*_pclsCategories[m_nEnumLoc]) )
				{
					CCTRACEI ( "CNSWPluginView::GetNextCategory - Invalid digital signature." );
					hrx << E_ACCESSDENIED;
				}

				hrRes = CoCreateInstance(*_pclsCategories[m_nEnumLoc], NULL, CLSCTX_INPROC_SERVER,
					IID_INSWCategory, (void**) ppCategory);
			}
			else
			{
				// Check the plugin dll for a valid symantec signature
				if( NAVToolbox::NAVTRUST_OK != NAVToolbox::IsSymantecComServer(*_pclsCategoriesPro[m_nEnumLoc]) )
				{
					CCTRACEI ( "CNSWPluginView::GetNextCategory - Invalid digital signature (Pro Cataegory)." );
					hrx << E_ACCESSDENIED;
				}

				hrRes = CoCreateInstance(*_pclsCategoriesPro[m_nEnumLoc], NULL, CLSCTX_INPROC_SERVER,
					IID_INSWCategory, (void**) ppCategory);
			}
            
			if(SUCCEEDED(hrRes) && ppCategory && *ppCategory)
			{
                CComQIPtr<IVarBstrCol> spColl(*ppCategory);
                // Copy the collection from the product to each category
                if(spColl.p)
                {
                    VARIANT_BOOL bExists = VARIANT_FALSE;
                    _variant_t varKeyName = _bstr_t(DRM::szDRMSuiteOwner);
                    _COLLECTION_BASE::get_Exists(varKeyName, &bExists);

                    if(VARIANT_TRUE == bExists)
                    {
                        CComVariant varSuiteOwner;
                        _COLLECTION_BASE::get_Item(varKeyName, &varSuiteOwner);

                        if(varSuiteOwner.vt == VT_BSTR)
                            spColl->put_Item(varKeyName, varSuiteOwner);
                    }
                }

				++m_nEnumLoc;
			}
			hrx << hrRes;
		}
	}
	STAHLSOFT_HRX_CATCH_ALL_RETURN(hr);    
}

STDMETHODIMP CNSWPluginView::SetValue(/*[in]*/ DWORD dwValueID, /*[in]*/ DWORD dwValue)
{
	STAHLSOFT_HRX_TRY(hr)
	{
		hr = CBrandingImpl::SetValue(dwValueID,dwValue);
	}
	STAHLSOFT_HRX_CATCH_ALL_RETURN(hr);    
}

STDMETHODIMP CNSWPluginView::GetValue(/*[in]*/ DWORD dwValueID, /*[out, retval]*/ DWORD* pdwValue)
{
	STAHLSOFT_HRX_TRY(hr)
	{
		hr = CBrandingImpl::GetValue(dwValueID,pdwValue);
		hr = ((hr == S_FALSE)?E_NOTIMPL:hr); // Herb_Stahl: here for legacy issues.  Totally breaks COM Rules, but
		// Certain frame classes expect an E_NOTIMPL to be returned
		// if a dwValueID is not in this list. 
	}
	STAHLSOFT_HRX_CATCH_ALL_RETURN(hr);    
}


STDMETHODIMP CNSWPluginView::ResetMenuItemEnum()
{
	return m_HelpMenu.ResetMenuItemEnum ();
}

STDMETHODIMP CNSWPluginView::GetNextMenuItem( WORD *pdwItemID,
											 SYMSW_MENUITEMTYPE *pdwItemType,
											 UINT *pnItemText)
{
	return m_HelpMenu.GetNextMenuItem ( pdwItemID,
		pdwItemType,
		pnItemText);
}

STDMETHODIMP CNSWPluginView::DoMenuItem ( WORD dwItemID,
										 HWND hMainWnd)
{
	return m_HelpMenu.DoMenuItem ( dwItemID,
		hMainWnd );
}

STDMETHODIMP CNSWPluginView::put_Item( 
									  /* [in] */ VARIANT vKey,
									  /* [in] */ VARIANT newVal)
{
	STAHLSOFT_HRX_TRY(hr)
	{
		do
		{
			if(vKey.vt == VT_BSTR)
			{
				_bstr_t varBstr = vKey;
				if(varBstr == _bstr_t(DRM::szDRMLicenseRefresh))
				{
					m_spOptionRefresh->Refresh(REFRESH_COMMIT_ON_CHANGE_FULLY);
					RequestLicensingAlert(NULL, HWNDPARENT_FIND);

					break; // no need to store this thing locally, since it is only a trigger mechanism.
				}
				else if((varBstr == _bstr_t(DRM::szDRMCategoryHook_Unhooked)) 
					|| (varBstr == _bstr_t(DRM::szDRMCategoryHook_Hooked)))				
				{
					// Refresh license, but ask not display an alert.
					m_spOptionRefresh->Refresh(REFRESH_COMMIT_ON_CHANGE_FULLY|REFRESH_SILENT);

					// Request an alert here so that the alert will be modal to main UI.
					RequestLicensingAlert(NULL, HWNDPARENT_FIND);
				}
			}
			hrx << _COLLECTION_BASE::put_Item(vKey,newVal);
		}while(FALSE);
	}
	STAHLSOFT_HRX_CATCH_ALL_NO_RETURN(hr);
	_ASSERTE(SUCCEEDED(hr));
	return hr;

}

// See defect # 421299 
bool CNSWPluginView::getPre2004Suite ()
{
    // We are running in one of these two suites, SystemWorks or NIS. Figure out
    // which one. We can't tell which suite we are running in though so we still
    // might display Help when we shouldn't. That's why they call it cheese.

    // Get SystemWorks version
    //
    CRegKey regApps;
    if ( ERROR_SUCCESS == regApps.Open ( HKEY_LOCAL_MACHINE, _T("Software\\Symantec\\InstalledApps"), KEY_QUERY_VALUE))
    {
        // Get NIS version
        //
        TCHAR szNISPath [MAX_PATH] = {0};
        DWORD dwSize = MAX_PATH;
        if ( ERROR_SUCCESS == regApps.QueryStringValue( _T("Internet Security"), (LPTSTR) &szNISPath, &dwSize ))
        {
            // NIS 2004+ is installed. This key didn't exist before. Don't display the help
            return false;
        }

        TCHAR szNSWPath [MAX_PATH] = {0};
        if ( ERROR_SUCCESS == regApps.QueryStringValue ( _T("Norton SystemWorks"), (LPTSTR) &szNSWPath, &dwSize ))
        {
            // Get the swplugin.dll version
            //
            _tcscat ( szNSWPath, _T("\\swplugin.dll") );
            
            DWORD dwVersionInfoHandle = 0;
            DWORD dwResourceSize = 0;

			ccLib::CVersionInfo version;
			version.Load ( szNSWPath );
            LPCTSTR szResult = version.GetProductVersion ();

			TCHAR szResultCopy[MAX_PATH] = {0};
			_tcsncpy(szResultCopy, szResult, MAX_PATH);

            _tcstok ( szResultCopy, _T("."));
            int iMajorVersion = _ttoi ( szResult );

            // If NSW 2003 or earlier, return true
            //
            if ( iMajorVersion < 2004 )
            {
                return true;
            }
        }
    }

    return false;   // 2004+
}
