// NSWStatusCategory.cpp : Implementation of CNSWStatusCategory
#include "stdafx.h"
#include "NAVUI.h"
#include "NSWStatusCategory.h"
#include "Globals.h"
#include "GlobalEvent.h"
#include "GlobalEvents.h"
#include "NAVTrust.h"
#include "NAVLicenseNames.h"

CNSWStatusCategory::CNSWStatusCategory ()
{
	// Format our initial URL.

    wsprintf(m_szPanelURL, _T("res://%s\\NAVUIHTM.DLL/%s"), g_NAVInfo.GetNAVDir(), _T("navstats.htm"));
    m_uCategoryTitleID = IDS_STATUS_CATEGORYTITLE;
}


STDMETHODIMP CNSWStatusCategory::CreatePage(/*[in]*/ HWND hParentWnd, /*[in]*/ DWORD dwParam)
{
    STAHLSOFT_HRX_TRY(hr)
    {
        CCTRACEI ( "Creating page" );
        CCTRACEI ( m_szPanelURL );

        // Check the webwindow dll for a valid symantec signature
        if( NAVToolbox::NAVTRUST_OK != NAVToolbox::IsSymantecComServer(CLSID_ccWebWindow) )
        {
            //CCTRACEI ( "CNSWCategory::CreatePage - Invalid digital signature on ccWebWindow COM Server " );
            hrx << E_ACCESSDENIED;
        }

        m_hParentWnd = hParentWnd;

        if( SUCCEEDED(m_pWebPanel.CoCreateInstance(CLSID_ccWebWindow, NULL, CLSCTX_ALL)) )
        {

            //  Only create the panel here since this is the first time.
            //
			//

            // Navigate to the correct web page for this panel
            //
            SetCursor(LoadCursor(NULL, IDC_APPSTARTING));
            CComBSTR bstrURIorHTML = m_szPanelURL;

            // Inject this object into the webwindow
            IDispatchPtr spDisp = GetUnknown();
            _ASSERTE(spDisp != NULL);
            m_pWebPanel->put_ObjectArg(_variant_t(spDisp.GetInterfacePtr()));

            m_pWebPanel->CreateChildControl((long)m_hParentWnd, 0, bstrURIorHTML, NULL, NULL, TRUE, (long*)&m_hControlWnd);
            SetCursor(LoadCursor(NULL, IDC_ARROW));
        }
    }
    STAHLSOFT_HRX_CATCH_ALL_NO_RETURN(hr);
    if(FAILED(hr))
    {
        CCTRACEE ( "Failure in CreatePage" );
    }
    return hr;
}