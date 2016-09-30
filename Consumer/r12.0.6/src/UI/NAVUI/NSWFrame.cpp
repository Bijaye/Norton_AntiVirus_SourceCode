// NSWFrame.cpp : Implementation of CNSWFrame
#include "stdafx.h"
#include "NAVUI.h"
#include "AVRES.h"
//#include "swmisc.h"
//#include <objbase.h>        // COM
//#include <comcat.h>         // ComCat
//#include <initguid.h>
#include "aboutplg.h"   // For the About box plug-in
#include "globals.h"
#include <shellapi.h>

#define NSW_PROD_LIST
#define BTN_ID_LU 30005 // ID for the toolbar buttons, number is defined by NSW
#define BTN_ID_LA 30010
#define BTN_ID_RESCUE 30015

#include "NSWFrame.h"

#include "NAVLnch.h"
#include "AppLauncher.h"
#include "ScriptSafeVerifyTrustCached.h"		// For scripting security
#include "NAVError.h"
#include "GlobalEvents.h"
#include "NAVTrust.h"
#include "NAVHelpLauncher.h"
#include "V2LicensingAuthGuids.h"
#include "DRMNamedProperties.h"
#include "NAVErrorResource.h"
#include "NAVSettingsHelperEx.h"

#include "ccResourceLoader.h"
#include "..\navuires\resource.h"
extern cc::CResourceLoader g_ResLoader;

static const COLORREF s_crToolbarColor = RGB(0, 0, 0);

/////////////////////////////////////////////////////////////////////////////
// CNSWFrame

#pragma optimize("", off)
CNSWFrame::CNSWFrame()
{
	// If we are here, we are running stand-alone, not as part of NSW. &$ ??
	g_bStandAlone = true;
	m_spAppLauncher = NULL;	
}

HRESULT CNSWFrame::FinalConstruct()
{
	StahlSoft::HRX hrx;
	HRESULT hr = S_OK;
	try
	{
		CComBSTR bstrNAVLnch ("Symantec.Norton.AntiVirus.AppLauncher");

		NAVToolbox::NAVTRUSTSTATUS trustStatus;

		// Check the Navlnch dll for a valid symantec signature
		trustStatus = NAVToolbox::IsSymantecComServer(_T("Symantec.Norton.AntiVirus.AppLauncher"));

		if( trustStatus == NAVToolbox::NAVTRUST_ERROR_NOT_SIGNED)
		{
			CCTRACEI ( "CNSWFrame::FinalConstruct - Invalid digital signature on NAV AppLauncher COM Server" );
			return E_ACCESSDENIED;
		}

		// NAVTRUST_ERROR_INVALID_PARAM means the module couldn't be found.
		//
		if ( trustStatus != NAVToolbox::NAVTRUST_ERROR_INVALID_PARAM )
		{
			m_spAppLauncher.CoCreateInstance ( bstrNAVLnch,
				NULL,
				CLSCTX_INPROC_SERVER );

			HRESULT hrAppLauncher = E_FAIL;

			hrAppLauncher = m_spAppLauncher.QueryInterface(&m_spSymScriptSafe);

			if(hrAppLauncher == S_OK)
			{
				// Set appropriate access so that NAVStatus call to IsItSafe() 
				// returns successfully.

				long dwAccess = ISYMSCRIPTSAFE_UNLOCK_KEYA;
				long dwKey = ISYMSCRIPTSAFE_UNLOCK_KEYB;
				m_spSymScriptSafe->SetAccess(dwAccess, dwKey);
			}
		}

		hrx << CBrandingImpl::Initialize();
		hrx << CBrandingImpl::SetValue(NSWFRAME_TOOLBAR_COLOR,s_crToolbarColor);

		// Get product name from AVRES
		if(g_csProductName.IsEmpty())
			g_csProductName.LoadString(_getPrimaryResourceInstance(), IDS_AV_PRODUCT_NAME);

		GetLicensingInfo();
	}
	catch(_com_error& e)
	{
		hr = e.Error();
		ErrorMessageBox (GetDesktopWindow(), IDS_NAVERROR_INTERNAL_REINSTALL, IDS_ERR_BRANDING_CORRUPTED );

		// Call ExitProcess here to exit NMain before it displays 
		//  "Symantec Integrator could not initialize the current frame class."
		ExitProcess(-1);
	}
	catch(...)
	{
		hr = E_UNEXPECTED;
	}
	return hr;
}
#pragma optimize("", on)

STDMETHODIMP CNSWFrame::GetInstance(/*[out, retval]*/ HINSTANCE* phInstance)
{
	STAHLSOFT_HRX_TRY(hr)
	{
		*phInstance = NULL;
		*phInstance = _getPrimaryResourceInstance();
	}
	STAHLSOFT_HRX_CATCH_ALL_RETURN(hr);    
}

STDMETHODIMP CNSWFrame::GetTitleNameID(/*[out, retval]*/ UINT* pnTitleNameID)
{
	STAHLSOFT_HRX_TRY(hr)
	{
		*pnTitleNameID = IDS_CAPTIONTEXT;
	}
	STAHLSOFT_HRX_CATCH_ALL_RETURN(hr);    
}

STDMETHODIMP CNSWFrame::GetIconID(/*[out, retval]*/ UINT* pnIconID)
{
	STAHLSOFT_HRX_TRY(hr)
	{
		*pnIconID = IDI_FRAMEICON;
	}
	STAHLSOFT_HRX_CATCH_ALL_RETURN(hr);    
}

STDMETHODIMP CNSWFrame::GetProductBitmapID(/*[in]*/ SYMSW_BITMAPNO eBitmapNo, /*[out, retval]*/ UINT* pnBitmapID)
{
	STAHLSOFT_HRX_TRY(hr)
	{
		*pnBitmapID = 0;
		if(eBitmapNo == eSymSWProductBitmap)
		{
			*pnBitmapID = IDB_PRODLOGO;
		}
	}
	STAHLSOFT_HRX_CATCH_ALL_RETURN(hr);    
}

STDMETHODIMP CNSWFrame::DoHelp(/*[in]*/ HWND hMainWnd)
{
	// This Help is called when in Stand-Alone mode (no SystemWorks, NIS, etc.)
	STAHLSOFT_HRX_TRY(hr)
	{
		NAVToolbox::CNAVHelpLauncher NAVHelp;
		BOOL bRet = (NULL != NAVHelp.LaunchHelp ( NULL, hMainWnd ));
		hrx << (bRet?S_OK:E_FAIL);
	}
	STAHLSOFT_HRX_CATCH_ALL_RETURN(hr);    
}

STDMETHODIMP CNSWFrame::ResetProductEnum()
{
	STAHLSOFT_HRX_TRY(hr)
	{
		m_nEnumLoc = 0;
	}
	STAHLSOFT_HRX_CATCH_ALL_RETURN(hr);    
}

STDMETHODIMP CNSWFrame::GetNextProduct(/*[in]*/ BOOL bCreate, /*[out, retval]*/ INSWIntegratorProduct** ppProduct)
{
	STAHLSOFT_HRX_TRY(hr)
	{
		if(m_nEnumLoc >= sizeof(_pclsProducts) / sizeof(_pclsProducts[0]))
			hrx << E_FAIL;

		if(bCreate == FALSE)
		{
			// SymLTProduct plugin (aka Renewal Center link) is always the last plugin in the list.
			// Check to see if we need to integrate this plugin.
			UINT idxSymLTPlug = (sizeof(_pclsProducts) / sizeof(_pclsProducts[0])) - 1;
			if(m_nEnumLoc == idxSymLTPlug)
			{
                if(!ShouldShowRenewalCenterHelpMenu())
                    hrx << E_FAIL;
			}

			++m_nEnumLoc;
		}
		else if(bCreate == TRUE)
		{
			if(ppProduct == NULL)
                hrx << E_FAIL;

			if(&CLSID_NSCProduct == _pclsProducts[m_nEnumLoc])
			{
				// The Security Console Product.  Check settings to see if we should give it this CLSID
				DWORD dwShow = 1;

				READ_SETTING_START()
					READ_SETTING_DWORD(NSC_FeatureEnabled, dwShow, 1)
				READ_SETTING_END

				// If it's not set to show the console, skip this and go to the next
				if(1 != dwShow)
				{
						m_nEnumLoc++;
				}

			}

			// Check the plugin dll for a valid symantec signature
            NAVToolbox::NAVTRUSTSTATUS status = NAVToolbox::IsSymantecComServer(*_pclsProducts[m_nEnumLoc]);
			if( NAVToolbox::NAVTRUST_OK != status )
			{
				m_nEnumLoc++;
				CCTRACEI ( "CNSWFrame::GetNextProduct - error %d", status );
				hrx << E_ACCESSDENIED;
			}

			HRESULT hr2 = CoCreateInstance(*_pclsProducts[m_nEnumLoc], NULL, CLSCTX_INPROC_SERVER, IID_INSWIntegratorProduct, (void**)ppProduct);

            if(SUCCEEDED(hr2) && ppProduct && *ppProduct)
            {
                CComPtr<IVarBstrCol> spColl;
                hr2 = (*ppProduct)->QueryInterface(IID_IVarBstrCol, (void **)&spColl);

                if(SUCCEEDED(hr2) && spColl.p)
                {
                    spColl->put_Item(_variant_t(_bstr_t(DRM::szDRMSuiteOwner)), _variant_t(_bstr_t(V2LicensingAuthGuids::SZ_NAV_SUITEOWNERGUID)));
                }
            }

			m_nEnumLoc++;
		}
	}
	STAHLSOFT_HRX_CATCH_ALL_RETURN(hr);    
}

STDMETHODIMP CNSWFrame::ResetTBButtonEnum()
{
	STAHLSOFT_HRX_TRY(hr)
	{
		m_nTBEnumLoc = 0;
	}
	STAHLSOFT_HRX_CATCH_ALL_RETURN(hr);    
}

STDMETHODIMP CNSWFrame::GetNextTBButton(/*[out]*/ DWORD* pdwButtonID, /*[out]*/ UINT* pnButtonText, /*[out]*/ UINT* pnBitmapUp)
{
	STAHLSOFT_HRX_TRY(hr)
	{
		switch(m_nTBEnumLoc)
		{
		case 0:
			*pdwButtonID = BTN_ID_LU;
			*pnButtonText = IDS_TB_LU;
			*pnBitmapUp = BTN_ID_LU;
			++m_nTBEnumLoc;
			break;

			// **NOTE** Button #2 is rescue.  Since it's the last of our 
			// buttons, I return E_FAIL indicating we're out of buttons.
			// If we later want to add stuff to the right of Rescue,
			// this code will have to be tweaked to skip Rescue on NT.

			// 
			// Show Rescue button only if Rescue is installed.
			// 
			//

		case 1:
			{
				if ( m_spAppLauncher == NULL )
				{
					hrx << E_FAIL;
				}

				BOOL bRDInstalled = FALSE;

				m_spAppLauncher->get_IsRescueDiskInstalled( &bRDInstalled );

				if ( !bRDInstalled )
				{
					hrx << E_FAIL;
				}
				else
				{
					*pdwButtonID = BTN_ID_RESCUE;
					*pnButtonText = IDS_TB_RESCUE;
					*pnBitmapUp = BTN_ID_RESCUE;
				}

				++m_nTBEnumLoc;
			}
			break;

		default:
			hrx << E_FAIL;
			break;
		}
	}
	STAHLSOFT_HRX_CATCH_ALL_RETURN(hr);    
}

STDMETHODIMP CNSWFrame::TBButtonClick(/*[in]*/ DWORD dwButtonID, /*[in]*/ HWND hMainWnd)
{
	USES_CONVERSION;

	STAHLSOFT_HRX_TRY(hr)
	{
		switch(dwButtonID)
		{
		case BTN_ID_LU:
			{
				BOOL bLUInstalled = FALSE;

				if ( m_spAppLauncher == NULL )
				{
					ErrorMessageBox( hMainWnd, IDS_NAVERROR_NAVLNCH_NO_LU, IDS_ERR_LAUNCH_LU );
					hrx << E_FAIL;
				}

				m_spAppLauncher->get_IsLiveUpdateInstalled( &bLUInstalled );

				if ( !bLUInstalled )
				{
					WarningMessageBox ( hMainWnd, IDS_LU_NOT_INSTALLED);

					hrx << E_FAIL;
				}

				// Run LUALL to start the update process.
				HRESULT hr = m_spAppLauncher->LaunchLiveUpdate ();

				if ( FAILED(hr))
				{
					// First try to get the Error information from the AppLauncher object
					// since it may be more detailed than unable to launch LiveUpdat
					INAVCOMError* pNAVError;

					if ( SUCCEEDED (m_spAppLauncher->get_NAVError (&pNAVError)))
					{
						CComBSTR bstrErrorMessage;
						long lErrorID = 0;
						long lModuleID = 0;
						pNAVError->get_Message (&bstrErrorMessage);
						pNAVError->get_ErrorID (&lErrorID);
						pNAVError->get_ModuleID (&lModuleID);

						ErrorMessageBox ( hMainWnd, bstrErrorMessage, lErrorID, lModuleID );
					}
					else
					{
						// Couldn't get the error info from app launcher so
						// display the error that we know about
						ErrorMessageBox( hMainWnd, IDS_NAVERROR_NAVLNCH_NO_LU, IDS_ERR_LAUNCH_LU );
					}
				}

				// Return a success code since we already did the common
				// error display here and we don't want anyone else to do it again
				hr = S_FALSE;
			}
			break;

		case BTN_ID_RESCUE:
			{					
				if ( m_spAppLauncher == NULL )
				{
					ErrorMessageBox ( hMainWnd, IDS_NAVERROR_NAVLNCH_NO_RESCUE, IDS_ERR_LAUNCH_RESCUE );
					hrx << E_FAIL;                
				}

				// Run Rescue Disk

				HRESULT hr = m_spAppLauncher->LaunchRescueDisk ();

				if ( FAILED (hr))
				{
					INAVCOMError* pNAVError;

					if ( SUCCEEDED (m_spAppLauncher->get_NAVError (&pNAVError)))
					{
						CComBSTR bstrErrorMessage;
						long lErrorID = 0;
						long lModuleID = 0;
						pNAVError->get_Message (&bstrErrorMessage);
						pNAVError->get_ErrorID (&lErrorID);
						pNAVError->get_ModuleID (&lModuleID);

						ErrorMessageBox ( hMainWnd, bstrErrorMessage, lErrorID, lModuleID );
					}
					else
					{
						// Couldn't get the error info from app launcher so
						// display the error that we know about
						ErrorMessageBox( hMainWnd, IDS_NAVERROR_NAVLNCH_NO_RESCUE, IDS_ERR_LAUNCH_RESCUE );
					}
				}

				// Return a success code since we already did the common
				// error display here and we don't want anyone else to do it again
				hr = S_FALSE;
			}
			break;

		default:
			hrx << E_NOTIMPL;
			break;
		}
	}
	STAHLSOFT_HRX_CATCH_ALL_RETURN(hr);    
}

typedef void (WINAPI * PFNDOABOUT)(PLUGINABOUTSTRUCT*);

STDMETHODIMP CNSWFrame::DoAbout ( HWND hParentWnd )
{
	HRESULT   hr = S_OK;
	TCHAR     szDllPath[MAX_PATH] = {0};
	TCHAR     szProduct[MAX_PATH] = {0};
	HINSTANCE hLibrary = NULL;
	PLUGINABOUTSTRUCT About = {0};
	PFNDOABOUT pDoAbout = NULL;

	SetCurrentDirectory(g_NAVInfo.GetNAVDir ());

	wsprintf(szDllPath, "%s\\%s", g_NAVInfo.GetNAVDir (), _T(STR_PLGABOUTLIB));

	if( NAVToolbox::NAVTRUST_OK != NAVToolbox::IsSymantecSignedImage(szDllPath) )
	{
		ErrorMessageBox ( hParentWnd, IDS_NAVERROR_INTERNAL_REINSTALL, IDS_ERR_NO_ABOUT );
		return E_FAIL;
	}

	hLibrary = LoadLibraryEx(szDllPath, NULL, LOAD_WITH_ALTERED_SEARCH_PATH);

	if ( NULL == hLibrary || forceError (IDS_ERR_NO_ABOUT) )
	{
		ErrorMessageBox ( hParentWnd, IDS_NAVERROR_INTERNAL_REINSTALL, IDS_ERR_NO_ABOUT );
		return E_FAIL;
	}

	// Put the product name in szProduct
	LoadString (_getPrimaryResourceInstance(), IDS_CAPTIONTEXT, szProduct, MAX_PATH);

	ZeroMemory(&About, sizeof(About));
	About.dwStructSize = sizeof(About);
	About.hInst = _getPrimaryResourceInstance();
	About.lpszAppName = szProduct;
	About.hIcon = (HICON) IDI_FRAMEICON;
	About.hWndOwner = hParentWnd;

	(FARPROC&) pDoAbout = GetProcAddress ( hLibrary, "DoStdAbout" );

	if (NULL != pDoAbout )
	{
		pDoAbout ( &About );
	}
	else
	{
		hr = E_FAIL;
	}

	FreeLibrary ( hLibrary );

	return hr;
}

STDMETHODIMP CNSWFrame::SetValue(/*[in]*/ DWORD dwValueID, /*[in]*/ DWORD dwValue)
{
	STAHLSOFT_HRX_TRY(hr)
	{
		hr = CBrandingImpl::SetValue(dwValueID,dwValue);
	}
	STAHLSOFT_HRX_CATCH_ALL_RETURN(hr);    
}

STDMETHODIMP CNSWFrame::GetValue(/*[in]*/ DWORD dwValueID, /*[out, retval]*/ DWORD* pdwValue)
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

STDMETHODIMP CNSWFrame::GetTechSuppURL (/*[out, retval]*/ LPTSTR szURL)
{
	_tcscpy ( szURL, _T("\0"));

	return S_OK;
}

STDMETHODIMP CNSWFrame::ShowTechSuppMenu (/*[out, retval]*/ BOOL* pbShowMenu)
{
	*pbShowMenu = FALSE;
	return S_OK;
}

STDMETHODIMP CNSWFrame::ShowSymantecMenu (/*[out, retval]*/ BOOL* pbShowMenu)
{
	*pbShowMenu = FALSE;

	return S_OK;
}

STDMETHODIMP CNSWFrame::ShowProductReg(/*[out, retval]*/ BOOL* pbShowMenu)
{
	*pbShowMenu = FALSE;

	return S_OK;
}

STDMETHODIMP CNSWFrame::DoProductReg(/*[in]*/ HWND hParentWnd)
{
	return E_FAIL;
}

