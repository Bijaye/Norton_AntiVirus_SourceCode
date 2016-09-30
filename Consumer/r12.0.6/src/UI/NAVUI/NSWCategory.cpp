// NSWCategory.cpp : Implementation of CNSWCategory
#include "stdafx.h"
#include "NAVUI.h"
#include "NSWCategory.h"
#include "Globals.h"
#include "GlobalEvent.h"
#include "GlobalEvents.h"
#include "NAVTrust.h"
#include "NAVLicenseNames.h"

/////////////////////////////////////////////////////////////////////////////
// CNSWCategory

CNSWCategory::CNSWCategory()
{
    // Set default URL
    //
    _tcscpy ( m_szPanelURL, _T("about:blank"));   // &$ set to 404?
    
    m_hParentWnd = NULL;
    m_uCategoryTitleID = 0;
    m_pWebPanel = NULL;
    m_hControlWnd = NULL;
    CCTRACEI ( "Constructor for page" );
}

CNSWCategory::~CNSWCategory()
{
    
    CCTRACEI ( "Deconstructor for page" );
}

STDMETHODIMP CNSWCategory::GetInstance(/*[out, retval]*/ HINSTANCE* phInstance)
{
	STAHLSOFT_HRX_TRY(hr)
	{
		*phInstance = _Module.GetModuleInstance();
	}
	STAHLSOFT_HRX_CATCH_ALL_RETURN(hr);    
}

STDMETHODIMP CNSWCategory::GetTitleID(/*[out, retval]*/ UINT* pnCategoryTitleID)
{
	STAHLSOFT_HRX_TRY(hr)
	{
        *pnCategoryTitleID = m_uCategoryTitleID;
	}
	STAHLSOFT_HRX_CATCH_ALL_RETURN(hr);    
}

STDMETHODIMP CNSWCategory::CreatePage(/*[in]*/ HWND hParentWnd, /*[in]*/ DWORD dwParam)
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

			//
			//  Only create the panel here since this is the first time.
			//

			// Navigate to the correct web page for this panel
			SetCursor(LoadCursor(NULL, IDC_APPSTARTING));
			CComBSTR bstrURIorHTML = m_szPanelURL;
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

STDMETHODIMP CNSWCategory::ShowPage(/*[in]*/ int nCommand)
{
	STAHLSOFT_HRX_TRY(hr)
	{
        // this is a big hack, but nmain can't be changed to the extent required to fix this correctly, due to backwards compatibility issues
		if(nCommand == SW_SHOWNA)
		{
			ShowWindow(m_hControlWnd, SW_HIDE);        
		}
		else
		{
			// passing the SW_HIDE along to the ccwebwnd handler will unload the html page, and load a blank.html, causing numerous defects including part two of 402291
			if(IsWindow(m_hControlWnd))
			{
				ShowWindow(m_hControlWnd, nCommand);
			}
		}

		//  Request licensing alert here so that the alert will be displayed
		//  on top of NMain window and is active.
		RequestLicensingAlert(m_hParentWnd, HWNDPARENT_PASSED);
	}
	STAHLSOFT_HRX_CATCH_ALL_RETURN(hr);    
}

STDMETHODIMP CNSWCategory::QueryTerminatePage(/*[out, retval]*/ BOOL* pbCanTerminate)
{
	STAHLSOFT_HRX_TRY(hr)
	{
		// ToDo: Add code to allow or stop termination of this categories window
		*pbCanTerminate = TRUE;
	}
	STAHLSOFT_HRX_CATCH_ALL_RETURN(hr);    
}

STDMETHODIMP CNSWCategory::TerminatePage()
{
	STAHLSOFT_HRX_TRY(hr)
	{
		if ( m_pWebPanel) 
		{   
			CCTRACEI ( "Destroying panel :" );
			CCTRACEI ( m_szPanelURL );
			if ( !DestroyWindow(m_hControlWnd) )
				CCTRACEI ( "DestroyWindow () failed!" );
		}
		else
		{
			CCTRACEI ( "TerminatePage () called when no m_pWebPanel" );
		}
	}
	STAHLSOFT_HRX_CATCH_ALL_NO_RETURN(hr);
	if(FAILED(hr))
	{
		CCTRACEE ( "Failure in TerminatePage ()" );
	}
	m_hParentWnd = NULL;
	return S_OK;
}