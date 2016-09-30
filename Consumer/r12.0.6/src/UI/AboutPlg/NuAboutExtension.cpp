
// NuAboutExtension.cpp : Implementation of CNav2kAboutExtension

#include "stdafx.h"
#include "simon.h"
#include "StahlSoft.h"

// NAVToolbox
#define INITIIDS
#include "NAVHelpLauncher.h"
#include "OSInfo.h"    // Toolbox
#include "NAVInfo.h"    // Toolbox
#include "NAVTrust.h"
#include "ccSettingsInterface.h"

// end NAVToolbox

#include "NuAbout.h"
#include "AboutProp.h"
#include "NuAboutExtension.h"
#include "Navver.h"
#include "tchar.h"  
#include <stdio.h>  // For sprintf
#include "cltLicenseConstants.h"
#include "navwhlp5.h"
#include "OptNames.h"
#include "NAVSettingsHelperEx.h"
#include "NAVInfo.h"
#include "AvresBranding.h"

#include "Password.h"

// PEP Headers
#define _V2AUTHORIZATION_SIMON
#define _SIMON_PEP_
#define _AV_COMPONENT_CLIENT
#include "ComponentPepHelper.h"
#include "PepMiddleman.h"
#include "PepClientBase.h"
#include "PRODInfoPepHelper.h"

#define SZ_INSTALLEDAPPS_NAV_REG_KEY   "Software\\Symantec\\InstalledApps"
#define SZ_INSTALLEDAPPS_NAV_REG_VALUE "NAV"

void PASCAL CenterMain(HWND hWnd);

// Init static vars.
//
WNDPROC CNav2kAboutExtension::m_OldWndProc = NULL;
WNDPROC CNav2kAboutExtension::m_OldResetWndProc = NULL;
WNDPROC CNav2kAboutExtension::m_OldHelpWndProc = NULL;

HWND CNav2kAboutExtension::m_hWndResetPassword = NULL;
HWND CNav2kAboutExtension::m_hWndDlg = NULL;
HWND CNav2kAboutExtension::m_hWndHelp = NULL;
CString CNav2kAboutExtension::m_csProductName;

HRESULT CNav2kAboutExtension::FinalConstruct()
{
	STAHLSOFT_HRX_TRY(hr)
	{
		// Read branding ID from branding.ini file.
		//  Parent product uses these info for
		//  product integration in About box.
		hrx << CBrandingImpl::Initialize();
	}
	STAHLSOFT_HRX_CATCH_ALL_RETURN(hr);
}

STDMETHODIMP CNav2kAboutExtension::AddPages(LPFNADDPROPSHEETPAGE lpfnAddPage, LPARAM lParam)
{
	// add the NU extension property page

	PROPSHEETPAGE Page;
	Page.dwSize = sizeof(Page);
	Page.dwFlags = PSP_DEFAULT | PSP_USEREFPARENT;
	Page.hInstance = _Module.GetModuleInstance();
	Page.pszTemplate = MAKEINTRESOURCE(IDD_EXTENSION);
	Page.pfnDlgProc = ExtPageDlgProc;
	Page.pcRefParent = (UINT*) &(_Module.m_nLockCnt);

	HPROPSHEETPAGE hPage;
	hPage = CreatePropertySheetPage(&Page);

	if (hPage)
	{
		if (!lpfnAddPage(hPage, lParam))
			DestroyPropertySheetPage(hPage);
	}

	return S_OK;
}

STDMETHODIMP CNav2kAboutExtension::ReplacePage(UINT uPageID, LPFNADDPROPSHEETPAGE lpfnReplacePage,
											   LPARAM lParam)
{
	return S_OK;
}

BOOL CALLBACK CNav2kAboutExtension::ExtPageDlgProc(HWND hWnd, UINT nMsg, WPARAM wParam,
												   LPARAM lParam)
{
	switch(nMsg)
	{
	case WM_INITDIALOG:
		OnInitDialog(hWnd);
		return TRUE;
	case WM_KEYDOWN:
		OnKeyDown(hWnd, (int) wParam);
		return TRUE;
	case WM_COMMAND:
		return OnCommand(nMsg, wParam, lParam);
	}

	return FALSE;
}

BOOL CNav2kAboutExtension::OnCommand(UINT nMsg, WPARAM wParam, LPARAM lParam)
{
	// This will handle the cases when the user clicks Enter while
	// one of the buttons has focus
	if( HIWORD(wParam) == BN_CLICKED )
	{
		if( LOWORD(wParam) == IDC_RESET_PASSWORD)
		{
			if( ::IsWindowEnabled(m_hWndResetPassword) )
			{
				OnResetPassword();
				return TRUE;
			}
		}
		if( LOWORD(wParam) == IDC_HELP_BUTTON)
		{
			if( ::IsWindowEnabled(m_hWndHelp) )
			{
				OnHelp();
				return TRUE;
			}
		}
	}
	return FALSE;
}

void CNav2kAboutExtension::OnResetPassword ()
{
	CStringHelper HelperCaption (_Module.GetResourceInstance ());

	CPassword Password;
	if ( Password.ResetPassword ())
		::MessageBox ( m_hWndDlg,
		HelperCaption.LoadString ( IDS_RESET_PASSWORD_OK),
		m_csProductName,
		MB_OK );
	else
		::MessageBox ( m_hWndDlg,
		HelperCaption.LoadString ( IDS_RESET_PASSWORD_FAILED),
		m_csProductName,
		MB_OK | MB_ICONWARNING );
}

// Pointer to DefAnnuityQueryStatus() in SmellyCat DLL
//typedef DWORD (__stdcall *LPFN_DefAnnuityQueryStatus)(LPDWORD, LPDWORD, LPDWORD, LPLONG);

void CNav2kAboutExtension::OnInitDialog(HWND hWnd)
{
	// get product name from AVRES.DLL
	CBrandingRes BrandRes;
	m_csProductName = BrandRes.ProductName();

	// make the application name bold

	SetBoldfaceFont(GetDlgItem(hWnd, IDC_MODULE_NAME));

	// load the application icon
	HICON hIcon = LoadIcon(_Module.GetModuleInstance(), MAKEINTRESOURCE(IDI_NAVW_ICON));
	SendMessage(GetDlgItem(hWnd, IDC_MODULE_ICON), STM_SETICON,
		(WPARAM) hIcon, 0L);

	bool bStandAlone = true;
	DWORD dwSize = MAX_PATH;

	try
	{
		// Check to see if NAV is a suite child product.
		DWORD dwAggregated = 0;

		READ_SETTING_START()
			READ_SETTING_DWORD(LICENSE_Aggregated, dwAggregated, 0)
		READ_SETTING_END

		if(dwAggregated)
		{
			bStandAlone = false;
			CCTRACEI(_T("CNav2kAboutExtension::OnInitDialog - Hide product key, customer ID, and subscription data since NAV is a child product."));
		}
	}
	catch(_com_error& e)
	{
		CCTRACEE(_T("CNav2kAboutExtension::OnInitDialog - Failed to access Settings for aggregated flag - %s"), e.ErrorMessage());
	}

	DJSMAR_LicenseType type = DJSMAR_LicenseType_Violated;
	DJSMAR00_LicenseState state = DJSMAR00_LicenseState_Violated; 
	TCHAR szVendorName[MAX_PATH] = {0};
	TCHAR szCustomerID[MAX_PATH] = {0};

	STAHLSOFT_HRX_TRY(hr)
	{
        // Get the real owner of this UI component and PEP to him
        CRegKey key;
        CString csKey( DRMREG::SZ_SUITE_OWNER_KEY );
        csKey = csKey  + "\\" + V2LicensingAuthGuids::SZ_ANTIVIRUS_COMPONENT;

        BasicDRMMiddleMan::CPreferedOwnerEnum<NULL, NULL> poe(csKey, V2LicensingAuthGuids::SZ_ANTIVIRUS_COMPONENT);

        CString csOwnerSeed;
        poe.GetOwnerGUID(0, csOwnerSeed);

		// Getting licensing and subscription properties needed to enable product features
		CPEPClientBase pepBase;
		hrx << pepBase.Init();
		pepBase.SetContextGuid(clt::pep::CLTGUID);
		pepBase.SetComponentGuid(V2LicensingAuthGuids::SZ_ANTIVIRUS_COMPONENT);
		pepBase.SetPolicyID(clt::pep::POLICY_ID_QUERY_LICENSE_RECORD);
		pepBase.SetCollectionDWORD(clt::pep::SZ_PROPERTY_DISPOSITION,clt::pep::DISPOSITION_NONE);
		pepBase.SetCollectionDWORD(SUBSCRIPTION_STATIC::SZ_SUBSCRIPTION_PROPERTY_DISPOSITION,SUBSCRIPTION_STATIC::DISPOSITION_NONE);

        hrx << pepBase.QueryPolicy();

		// Getting licensing properties
		hrx << pepBase.GetValueFromCollection(DRM::szDRMLicenseType, (DWORD &)type, DJSMAR_LicenseType_Violated);
		hrx << pepBase.GetValueFromCollection(DRM::szDRMLicenseState, (DWORD &)state, DJSMAR00_LicenseState_Violated);

		BOOL bRegister = FALSE;
		if(type == DJSMAR_LicenseType_Rental)
		{
			bRegister = FALSE;

			StahlSoft::CSmartDataPtr<BYTE> spBuff;
            pepBase.GetCollectionData(DRM::szDRMSCSSVendorName, spBuff);

			if(spBuff.m_p)
			{
				// Setup SCSS vendor name text (i.e. "for T-Online")
				CString csFormat;
				csFormat.LoadString(_Module.GetModuleInstance(), IDS_RENTAL_VENDOR_NAME);
				wsprintf(szVendorName, csFormat, (LPCTSTR) spBuff.m_p);
			}
		}
		else if(bStandAlone)
		{
			bRegister = TRUE;

			// Get customer ID
			StahlSoft::CSmartDataPtr<BYTE> spBuff;
			pepBase.GetCollectionData(DRM::szDRMCustomerID, spBuff);

			if(spBuff.m_p)
				_tcscpy(szCustomerID, (LPCTSTR) spBuff.m_p);
		}

		pepBase.SetCollectionDWORD(PRODINFO::szPICanRegisterProduct, (DWORD) bRegister);

		CCTRACEI(_T("License Type=%d, License state=%d"), type, state);
	}
	catch(_com_error& e)
	{
		CCTRACEE(_T("CNav2kAboutExtension::OnInitDialog() - Exception while accessing Settings object %s"), e.ErrorMessage());
	}

	// Display product key, customer ID, and subscription info only if NAV is standalone.
	if(state != DJSMAR00_LicenseState_Violated && bStandAlone)
	{
		if(type == DJSMAR_LicenseType_Rental)
		{
			CPEPClientBase pepBase;
			hrx << pepBase.Init();
			pepBase.SetContextGuid(PRODINFO::PRODINFOGUID);
			pepBase.SetComponentGuid(V2LicensingAuthGuids::SZ_NAV_SUITEOWNERGUID);
			pepBase.SetPolicyID(PRODINFO::POLICY_ID_QUERY_PRODUCT_INFORMATION);

			hrx << pepBase.QueryPolicy();

			// Get media SKU
			StahlSoft::CSmartDataPtr<BYTE> spbySKUMedia;
			pepBase.GetCollectionData(PRODINFO::szPISymSKUMedia, spbySKUMedia);

			if(spbySKUMedia.m_p)
			{
				// Setup text displayed
				//		Symantec Online Services
				//		Media SKU: xxxxxxxx
				CString csCombined;
				CString csCombinedFormat;
				csCombinedFormat.LoadString(IDS_RENTAL_PROD_INFO);
				csCombined.Format(csCombinedFormat, (LPCTSTR)spbySKUMedia.m_p);

				// Show text
				SetDlgItemText(hWnd, IDC_DEFSUBSCRIPTIONSTATUS, csCombined);
				ShowWindow(GetDlgItem(hWnd, IDC_DEFSUBSCRIPTIONSTATUS), SW_SHOW);
			}
			else
			{
				CCTRACEE(CCTRCTX _T("Media SKU not found"));
			}
		}
		else
		{
			STAHLSOFT_HRX_TRY(hr2)
			{
                // Get the real owner of this UI component and PEP to him
                CRegKey key;
                CString csKey( DRMREG::SZ_SUITE_OWNER_KEY );
                csKey = csKey  + "\\" + V2LicensingAuthGuids::SZ_ANTIVIRUS_COMPONENT;

                BasicDRMMiddleMan::CPreferedOwnerEnum<NULL, NULL> poe(csKey, V2LicensingAuthGuids::SZ_ANTIVIRUS_COMPONENT);

                CString csOwnerSeed;
                poe.GetOwnerGUID(0, csOwnerSeed);

				// Getting licensing and subscription properties needed to enable product features
				CPEPClientBase pepBase;
				hrx << pepBase.Init();
                pepBase.SetContextGuid(PRODINFO::PRODINFOGUID);
                pepBase.SetComponentGuid(V2LicensingAuthGuids::SZ_NAV_SUITEOWNERGUID);
                pepBase.SetPolicyID(PRODINFO::POLICY_ID_QUERY_PRODUCT_INFORMATION);		

				hrx << pepBase.QueryPolicy();

				StahlSoft::CSmartDataPtr<BYTE> spbyMessage; 
				spbyMessage.Release();
                
                CString sCombined;

				COSInfo OSInfo;
				if(OSInfo.IsAdminProcess())
				{
					// Get family sku
                    pepBase.GetCollectionData(PRODINFO::szPISymSKUFamily, spbyMessage);

					if(spbyMessage.m_p)
					{
                        sCombined.AppendFormat(IDS_FAMILY_SKU, (TCHAR*)spbyMessage.m_p);
					}
					else
					{
						CCTRACEI(_T("CNav2kAboutExtension::OnInitDialog() - Family SKU is not available."));
					}
                    
                    spbyMessage.Release();
        
                    // Get product SKU
                    pepBase.GetCollectionData(PRODINFO::szPISymSkuCurrent, spbyMessage);

					if(spbyMessage.m_p)
					{
                        sCombined.AppendFormat(IDS_PRODUCT_SKU, (TCHAR*)spbyMessage.m_p);
					}
					else
					{
						CCTRACEI(_T("CNav2kAboutExtension::OnInitDialog() - Product SKU is not available."));
					}

                    spbyMessage.Release();
                    
					// Don't show Cipher info that we can't disclose to our vendors
                    // Get Vendor ID
                    //pepBase.GetCollectionData(PRODINFO::szPIVendorID, spbyMessage);

					//if(spbyMessage.m_p)
					//{
					//    sCombined.AppendFormat(IDS_VENDOR_ID, (TCHAR*)spbyMessage.m_p);
					//}
					//else
					//{
					//	CCTRACEI(_T("CNav2kAboutExtension::OnInitDialog() - Vendor ID is not available."));
					//}

                    //spbyMessage.Release();

                    // Get Vendor Tag
                    //pepBase.GetCollectionData(PRODINFO::szPIVendorTag, spbyMessage);

					//if(spbyMessage.m_p)
					//{
                    //    sCombined.AppendFormat(IDS_VENDOR_TAG, (TCHAR*)spbyMessage.m_p);
					//}
					//else
					//{
					//	CCTRACEI(_T("CNav2kAboutExtension::OnInitDialog() - Vendor Tag is not available."));
					//}

                    //spbyMessage.Release();

					if(_tcslen(szCustomerID) > 0)
					{
                        sCombined.AppendFormat(IDS_CUSTOMER_ID, szCustomerID);
					}
					else
					{
						CCTRACEI(_T("CNav2kAboutExtension::OnInitDialog() - customer ID is not available."));
					}

                    //spbyMessage.Release();
                    
                    // Get Enpoint ID
                    pepBase.Reset();
                    pepBase.SetContextGuid(clt::pep::CLTGUID);
                    pepBase.SetComponentGuid(V2LicensingAuthGuids::SZ_ANTIVIRUS_COMPONENT);
                    pepBase.SetPolicyID(clt::pep::POLICY_ID_QUERY_LICENSE_RECORD);

                    pepBase.GetCollectionData(DRM::szDRMEndPointID, spbyMessage);

					if(spbyMessage.m_p)
					{
                        sCombined.AppendFormat(IDS_ENDPOINT_ID, (TCHAR*)spbyMessage.m_p);
					}
					else
					{
						CCTRACEI(_T("CNav2kAboutExtension::OnInitDialog() - Enpoint ID is not available."));
					}

                    SetDlgItemText(hWnd, IDC_DEFSUBSCRIPTIONSTATUS, sCombined);
                    ShowWindow(GetDlgItem(hWnd, IDC_DEFSUBSCRIPTIONSTATUS), SW_SHOW);
				}               
			}
			STAHLSOFT_HRX_CATCH_ALL_NO_RETURN(hr);
		}
	}

	// Read our version number from VERSION.DAT.  If the file can't be read,
	// fall back to the DLL version number (from navver.h).
	TCHAR szVersion[80];
	TCHAR szVersionWord[80];
	TCHAR szVersionDatPath [MAX_PATH] = {0};
	TCHAR szPublicRevision [80];
	TCHAR szIsOEM[80] = {0};
	TCHAR szProductName[MAX_PATH] = {0};
	TCHAR szVSKU[80] = {0};
	BOOL  bSuccess = FALSE;

	// Get product name from branding DLL.
	CNAVInfo NAVInfo;
	CString csResDll;
	csResDll.Format(_T("%s\\AVRES.DLL"), NAVInfo.GetNAVDir());
	StahlSoft::CSmartModuleHandle shMod(LoadLibrary(csResDll));
	if((HANDLE)shMod != (HANDLE)NULL)
	{
		LoadString(shMod, IDS_AV_PRODUCT_NAME, szProductName, sizeof(szProductName)/sizeof(TCHAR));
		SetDlgItemText(hWnd, IDC_MODULE_NAME, szProductName);
	}

	LoadString (_Module.GetModuleInstance(), IDS_VERSION, szVersionWord, sizeof (szVersionWord));

	CRegKey RegKey;
	dwSize = 0;

	if ( ERROR_SUCCESS == 
		RegKey.Open ( HKEY_LOCAL_MACHINE, SZ_INSTALLEDAPPS_NAV_REG_KEY,
		KEY_READ ))
	{
		dwSize = sizeof(szVersionDatPath);

		if ( ERROR_SUCCESS == 
			RegKey.QueryStringValue ( SZ_INSTALLEDAPPS_NAV_REG_VALUE, szVersionDatPath,
			&dwSize ))
		{
			lstrcat ( szVersionDatPath, _T("\\version.dat") );

			// Read the public revision number.

			GetPrivateProfileString ( _T("Versions"), _T("PublicRevision"),
				_T("*"), szPublicRevision, 80, szVersionDatPath );

			if ( 0 != lstrcmp ( szVersion, _T("*") ))
			{
				bSuccess = TRUE;
				wsprintf ( szVersion, _T("%s%s"), szVersionWord, szPublicRevision );
			}
		}

		RegKey.Close();
	}

	// If we couldn't read VERSION.DAT, fall back to the DLL version numbers.
	if ( !bSuccess )
	{
		wsprintf(szVersion, "%s%s.%s.%s", szVersionWord, VER_STR_PRODVERMAJOR, VER_STR_PRODVERMINOR, VER_STR_PRODVERSUBMINOR);
	}

	SetDlgItemText(hWnd, IDC_NAV_VERSION, szVersion);

	GetPrivateProfileString(_T("Versions"), _T("NAVOEM"), _T("0"), 
		szIsOEM, sizeof(szIsOEM), szVersionDatPath);

	// if its an OEM build then we need to display "(OEM)" before the vendor name
	// if its not an OEM build but we have a vendor name (probably some time of managed deal like T-online)
	// then just display the vendor name
	if (_tcscmp(szIsOEM, _T("0")) != 0)
	{
		GetPrivateProfileString(_T("Versions"), _T("OEMVendor"), _T(""),
			szVendorName, sizeof(szVendorName), szVersionDatPath);

		LoadString(_Module.GetModuleInstance(), IDS_OEM, szIsOEM, sizeof(szIsOEM));

		wsprintf(szProductName, "%s (%s)", szProductName, szIsOEM);

		SetDlgItemText(hWnd, IDC_MODULE_NAME, szProductName);

		if (_tcslen(szVendorName) > 0)
			SetDlgItemText(hWnd, IDC_VENDOR_NAME, szVendorName);

	}
	else if(_tcslen(szVendorName) > 0)
	{
			SetDlgItemText(hWnd, IDC_VENDOR_NAME, szVendorName);
	}

	// hook into the window procedure for the module icon
	// we need to do this to intercept WM_KEYDOWN messages since
	// dialogs don't recieve this message

	m_OldWndProc = (WNDPROC) SetWindowLong(GetDlgItem(hWnd, IDC_MODULE_ICON),
		GWL_WNDPROC, (DWORD) KeyProc);

	m_hWndResetPassword = GetDlgItem ( hWnd, IDC_RESET_PASSWORD );
	m_hWndHelp = GetDlgItem ( hWnd, IDC_HELP_BUTTON );

	m_OldResetWndProc = (WNDPROC) SetWindowLong( m_hWndResetPassword,
		GWL_WNDPROC,
		(DWORD) ButtonProc);

	m_OldHelpWndProc = (WNDPROC) SetWindowLong( m_hWndHelp,
		GWL_WNDPROC,
		(DWORD) ButtonProc);    

	//SetFocus (GetDlgItem (hWnd,IDC_MODULE_NAME));


	// Disable the button if 
	// 1. License is not valid.
	// 2. Password protection is off.
	// 3. The user is not an Admin.
	//
	COSInfo OSInfo;
	CPassword Password;
	bool bLicenseValid = (state == DJSMAR00_LicenseState_PURCHASED || state == DJSMAR00_LicenseState_TRIAL);

	if ( !bLicenseValid || !Password.IsPasswordCheckEnabled () || !OSInfo.IsAdminProcess() )
	{
		EnableWindow ( m_hWndResetPassword, FALSE );
	}

	m_hWndDlg = hWnd;
}

void CNav2kAboutExtension::OnKeyDown(HWND hWnd, int nVirtualKey)
{
	// display the VSKU number from the DefAnnuity section in version.dat if Ctrl+V was pressed

	if (nVirtualKey == 'V' && GetAsyncKeyState(VK_CONTROL))
	{
		ShowWindow(GetDlgItem(hWnd, IDC_BUILD), SW_SHOW);
	}
}

LRESULT CALLBACK CNav2kAboutExtension::KeyProc(HWND hWnd, UINT nMsg, WPARAM wParam,
											   LPARAM lParam)
{
	// if the module icon gets a WM_SYSKEYDOWN message,
	// with the correct hotkeys call the correct button
	if(nMsg == WM_SYSKEYDOWN)
	{
		if( (int)wParam == 'R' )
		{
			if( ::IsWindowEnabled(m_hWndResetPassword) )
			{
				OnResetPassword ();
				return TRUE;
			}
		}
		else if( (int)wParam == 'H' )
		{
			if( ::IsWindowEnabled(m_hWndHelp) )
			{
				OnHelp();
				return TRUE;
			}
		}
	}

	// if the module icon gets a WM_KEYDOWN message,
	// pass it to its parent window for processing..
	if (nMsg == WM_KEYDOWN)
	{
		SendMessage(GetParent(hWnd), nMsg, wParam, lParam);
	}

	return CallWindowProc(m_OldWndProc, hWnd, nMsg, wParam, lParam);
}

LRESULT CALLBACK CNav2kAboutExtension::ButtonProc(HWND hWnd, UINT nMsg, WPARAM wParam,
												  LPARAM lParam)
{
	// If the user clicks on the space button or on the left mouse button
	// with one of the buttons highlighted perform the expected action.
	if ( (nMsg == WM_KEYDOWN && (int)wParam == VK_SPACE) || nMsg == WM_LBUTTONDOWN )
	{
		if ( hWnd == m_hWndResetPassword )
		{
			SetFocus(hWnd);
			OnResetPassword ();
			return TRUE;
		}

		else if ( hWnd == m_hWndHelp )
		{
			SetFocus(hWnd);
			OnHelp ();
			return TRUE;
		}
	}

	// If the space or left mouse button is not clicked use the default handler for
	// the button.
	if( hWnd == m_hWndResetPassword)
		return CallWindowProc(m_OldResetWndProc, hWnd, nMsg, wParam, lParam);
	else
		return CallWindowProc(m_OldHelpWndProc, hWnd, nMsg, wParam, lParam);
}

BOOL CNav2kAboutExtension::SetBoldfaceFont(HWND hWnd)
{
	HFONT hFont = (HFONT) SendMessage(GetParent(hWnd), WM_GETFONT, 0, 0);

	if (hFont == NULL)
		return FALSE;

	LOGFONT LogFont;
	if (GetObject(hFont, sizeof(LOGFONT), &LogFont) == 0)
		return FALSE;

	if (GetSystemMetrics(SM_DBCSENABLED) == FALSE)
		LogFont.lfWeight = 700;

	HFONT hBoldFont = CreateFontIndirect(&LogFont);

	if (hBoldFont == NULL)
		return FALSE;

	SendMessage(hWnd, WM_SETFONT, (WPARAM) hBoldFont, MAKELPARAM(TRUE, 0));

	return TRUE;
}

void CNav2kAboutExtension::OnHelp()
{
	try
	{
		NAVToolbox::CNAVHelpLauncher NAVHelp;

		NAVHelp.LaunchHelp ( IDH_NAVW_PASSWORD_RESET_HELP_BTN );
	}
	catch (...)
	{
	}
}

STDMETHODIMP CNav2kAboutExtension::SetValue(DWORD dwValueID, DWORD dwValue)
{
	STAHLSOFT_HRX_TRY(hr)
	{
		hrx << CBrandingImpl::SetValue(dwValueID,dwValue);
	}
	STAHLSOFT_HRX_CATCH_ALL_RETURN(hr);
}

STDMETHODIMP CNav2kAboutExtension::GetValue(DWORD dwValueID, DWORD* pdwValue)
{
	STAHLSOFT_HRX_TRY(hr)
	{
		hrx << CBrandingImpl::GetValue(dwValueID,pdwValue);
	}
	STAHLSOFT_HRX_CATCH_ALL_RETURN(hr);
}
