////////////////////////
//
// PROPRIETARY / CONFIDENTIAL.
// Use of this product is subject to license terms.
// Copyright © 2006 Symantec Corporation.
// All rights reserved.
//
/////////////////////////////////////////////////////////////////////////////////////////////////

// NAVOptionsObj.cpp : Implementation of CNAVOptions
#include "StdAfx.h"

#define INITIIDS

// Licensing section
#include <initguid.h>
#define SIMON_INITGUID
#include "Simon.h"
#include "CLTProductHelper.h"
#include "V2LicensingAuthGuids.h"

#include "EmailProxyInterface.h"  // For Email options
#include "NAVOptionsObj.h"
#include "Navtrust.h"
#include "defutilsinterface.h"
#include "SymProtectControlHelper.h"
#include "ccScanWLoader.h"
#include "IMConfigInterface.h"
#include "ccOSInfo.h"

// IWP UI Section
#include "RuleUIInterface.h"    // FW Sys Rule UI
#include "RuleUILoader.h"
#include "IDSUIInterface.h"     // FW AutoBlock & Exclusions UIs
#include "IDSUILoader.h"
#include "ProgCtrlUIInterface.h"// FW App Rule UI
#include "ProgCtrlUILoader.h"

#include "SNDHelper.h"
#include "SymHTMLDialog.h"
#include "SymHtmlEventIterator.h"

// This if for the CThreatsByVID class to display
// the threat details.
#include "ScanUIInterfaces.h"

// Turn off elements (like IWP) w/o UI (since NAVOptions handles snooze)
#include "isDataNoUIInterface.h"

#include "NAVOptions.h"
#include "ccWebWnd.h"
#include "NAVOptionsObj.h"
#include "BrowserWindow.h"
#include "optnames.h"
#include "isErrorResource.h"
#include "isErrorIDs.h"
#include "ccPasswdExp.h"  // Contains the password ID define for NAV
#include "ccPasswd_h.h"
#include "ccPasswd_i.c"

#define _NAVOPTREFRESH_CONSTANTS
#include "NAVOptRefresh.h"
#include "NavOptionRefreshHelperInterface.h"
#include "StahlSoft.h"
//#define _INIT_COSVERSIONINFO
//#include "SSOsinfo.h"
#include "NAVSettingsCache.h"

#include "ccWebWnd_i.c"

#include "..\navoptionsres\resource.h"

using namespace std;

// Object map for syminterface
SYM_OBJECT_MAP_BEGIN()                          
SYM_OBJECT_MAP_END()

// VOOODOOOOOOOO!!
#pragma data_seg(".SINGLEINSTANCE_SHARED")
volatile HWND   g_hWndNavOpts = NULL;
#pragma data_seg()

#pragma comment(linker, "/section:.SINGLEINSTANCE_SHARED,rws")

/// VOOOOOOOOOODOOOOOOO!!
HHOOK m_hook = NULL;
LRESULT CALLBACK myHook(int code, WPARAM wParam, LPARAM lParam);

STDMETHODIMP CNAVOptions::Show(/*[in]*/ long hWnd)
{
	HRESULT hr;
	bool bCanIRun;
	CSemaphore sm;
	CCLTProductHelper cltHelper;

	if(FAILED(hr = cltHelper.Initialize()))
	{
		CCTRCTXE1(_T("Error initializing CLT Helper. Bailing out. HR = 0x%08x"), hr);
		return hr;
	}

	if(FAILED(hr = cltHelper.CanIRun(bCanIRun)))
	{
		CCTRCTXE1(_T("Error calling CanIRun. Bailing out. HR = 0x%08x"), hr);
		return hr;
	}

	if(!bCanIRun)
	{
		MakeError(IDS_ERR_LICENSING, E_ACCESSDENIED, IDS_NAVOPTS_ERR_LICENSING);
		m_spError->LogAndDisplay(0);
		return E_ACCESSDENIED;
	}

	if(FAILED(hr = this->InitSymTheme()))
	{
		CCTRCTXW1(_T("Failed to init SymTheme. HR = 0x%08x"), hr);
	}

    switch(hr = sm.Create())
	{

	case S_EXIST:  // Another session is holding the semaphore
	default:       // Error
		break;

	case S_FALSE:  // An instance of the options dialog already exist
            	   // Now look for that instance.

		// Q: Is an instance of Options dialog is still running?
		// Allow one one instance of the options dialog
		if (g_hWndNavOpts != INVALID_HANDLE_VALUE)
		{
			// Just bring the existing instance to the forground.
			::SetForegroundWindow(g_hWndNavOpts);
		}
        else
        {
            CCTRCTXW0(_T("Couldn't find shared HWND - assuming bizarre race condition; terminating."));
        }
        break;
		// Probably closed before we had a chance to find it.
		/* FALL THROUGH */
	case S_OK:  // This is the first instance of the options dialog
	{
        // First check for a digital signature on the NAVWebWindow COM
        // server since it will be used to display the Options UI
		if( forceError (ERR_SECURITY_FAILED) ||
			NAVToolbox::NAVTRUST_OK != NAVToolbox::IsSymantecComServer(CLSID_ccWebWindow) )
        {
            // Invalid signature
			MakeError (ERR_SECURITY_FAILED, E_ACCESSDENIED, IDS_ISSHARED_ERROR_INTERNAL_REINSTALL);
            m_spError->LogAndDisplay(0);
            return E_ACCESSDENIED;
        }

		if(m_bPasswordSupported)
		{
			// Check for password if necessary
			if( NAVToolbox::NAVTRUST_OK != NAVToolbox::IsSymantecComServer(CLSID_Password) )
			{
				// Invalid signature
				MakeError (ERR_SECURITY_FAILED, E_ACCESSDENIED, IDS_ISSHARED_ERROR_INTERNAL_REINSTALL);
				m_spError->LogAndDisplay(0);
				return E_ACCESSDENIED;
			}

			CComPtr<IPassword> spNAVPass;
			if (SUCCEEDED(spNAVPass.CoCreateInstance(CLSID_Password, NULL, CLSCTX_INPROC)))
			{
				CComBSTR bstrProductName(m_csProductName);
				CComBSTR bstrFeatureName(m_csTitle);
				BOOL bEnabled = FALSE;

				// Use the same password as NIS
				if( SUCCEEDED(spNAVPass->put_ProductID(NIS_CONSUMER_PASSWORD_ID)) &&
					SUCCEEDED(spNAVPass->put_ProductName(bstrProductName)) &&
					SUCCEEDED(spNAVPass->put_FeatureName(bstrFeatureName)) &&
					SUCCEEDED(spNAVPass->get_Enabled(&bEnabled)) )
				{
					if( bEnabled )
					{

						// Reusing the bEnabled variable to check if the password
						if( SUCCEEDED(spNAVPass->Check(&bEnabled)) )
						{
							// Check the result of the password check before displaying the options
							if( !bEnabled )
							{
								// Password not input correctly...don't display options
								return S_FALSE;
							}
						}
					}
				}
			}
		}


		// Create a new instance
		const long W = 680, H = 540;  // Dialog size

      // STS 361588
      // When we launch the options dialog, it needs to be offset from the main UI.
      // Since this dialog is so big though, moving it down in Y on a 640x480 display
      // will hide the OK/CANCEL/DEFAULTS buttons so I will only shift right if
      // the vertical display size is less than 600 pixels.
      int iOffsetX = 20;
      int iOffsetY = 36;

      if (GetSystemMetrics(SM_CYSCREEN) < 600)
      {
         iOffsetY = 0;
      }

      // Hook WM_CREATE so we can populate our webwnd's hwnd
      m_hook = SetWindowsHookExW(WH_SHELL, (HOOKPROC)&myHook, NULL, GetCurrentThreadId());

		CComPtr<IccWebWindow> spSymWebWin;

        // Get a pointer to the IDispatch to pass into the options object
        CComPtr<IDispatch> pDispatch;
        CComVariant vNavOpts;
        if( SUCCEEDED(QueryInterface(IID_IDispatch, (void**) &pDispatch)) )
        {
            vNavOpts = pDispatch;
        }
        else
        {
            CCTRACEE(_T("CNAVOptions::Show() - Failed to QI for internal IDispatch interface"));
        }

		// Open dialog
		if (SUCCEEDED(hr = spSymWebWin.CoCreateInstance(CLSID_ccWebWindow, NULL, CLSCTX_INPROC))
		 && SUCCEEDED(hr = spSymWebWin->put_OffsetWindowX(iOffsetX))
		 && SUCCEEDED(hr = spSymWebWin->put_OffsetWindowY(iOffsetY))
		 && SUCCEEDED(hr = spSymWebWin->put_InitialTitle(CComBSTR(m_csTitle)))
		 && SUCCEEDED(hr = spSymWebWin->SetIcon(reinterpret_cast<long>(_Module.GetModuleInstance()), IDI_NAVOPTIONS))
         && SUCCEEDED(hr = spSymWebWin->put_FrameWindowStyle(WS_DLGFRAME | WS_SYSMENU | WS_MINIMIZEBOX))    // Add minimizebox to std ccWebWnd style
         && SUCCEEDED(hr = spSymWebWin->put_ObjectArg(vNavOpts)))
        {
		    hr = spSymWebWin->showModalDialogEx(NULL, CComBSTR(url()), W
			                                                    , H
			                                                    , &CComVariant()
			                                                    , &CComVariant());
            CCTRCTXI1(_T("WebWnd returned 0x%X"), hr);
        }
        else
        {
            CCTRACEE(_T("CNAVOptions::Show() - Failed to create the ccWebWindow for the options dialog"));
        }
 
        if(m_hook != NULL)
        {
            UnhookWindowsHookEx(m_hook);
            m_hook = NULL;
        }

        break;

      }

	}

	if(m_spSkin)
	{
		if(FAILED(hr = m_spSkin->Exit()))
		{
			CCTRCTXW1(_T("Error uninitializing SymTheme. HR = 0x%08x"), hr);
		}
	}

	return hr;
}

STDMETHODIMP CNAVOptions::get_InitialPageSpyware(BOOL* pbShowSpyware)
{
    *pbShowSpyware = (m_eInitialPageID == ShowPageID_SPYWARE) ? VARIANT_TRUE : VARIANT_FALSE;

    return S_OK;
}

STDMETHODIMP CNAVOptions::ShowSpywarePage(long hWnd)
{
    return ShowPage(hWnd, ShowPageID_SPYWARE);
}

STDMETHODIMP CNAVOptions::get_InitialPage(EShowPageID *peShowPageID)
{
    *peShowPageID = m_eInitialPageID;

    return S_OK;
}

STDMETHODIMP CNAVOptions::ShowPage(/*[in]*/ long hWnd, /*[in]*/ EShowPageID eShowPageID)
{
    m_eInitialPageID = eShowPageID;
    return Show(hWnd);
}

STDMETHODIMP CNAVOptions::Help(/*[in]*/ long iID)
{
    m_Help.LaunchHelp(iID);
	return S_OK;
}

STDMETHODIMP CNAVOptions::Browse(/*[in]*/ EBrowseType eBrowseType, /*[in]*/ BSTR bstrInPath
                                                                 , /*[out, retval]*/ BSTR* pbstrOutPath)
{
	USES_CONVERSION;
    HRESULT hr = S_OK;

	if (forceError (ERR_INVALID_POINTER) ||
        !pbstrOutPath)
    {
        MakeError(ERR_INVALID_POINTER, E_POINTER, IDS_ISSHARED_ERROR_INTERNAL);
        return E_POINTER;
    }

    if ( ::SysStringLen ( bstrInPath ) > MAX_PATH )
        return E_POINTER;

	*pbstrOutPath = NULL;

	// Q: is the input path is valid
	PWCHAR pszPath = OLE2W(bstrInPath);
	CBrowserWindow bw(0xFFFFFFFFUL != ::GetFileAttributesW(pszPath) ? pszPath : NULL, eBrowseType);

	hr = bw.Browse(bstrInPath, pbstrOutPath);

    if ( forceError (IDS_ERR_BROWSING_FOLDERS))
        hr = E_FAIL;

    if( FAILED ( hr ) )
    {
        MakeError(IDS_ERR_BROWSING_FOLDERS, hr, IDS_ISSHARED_ERROR_INTERNAL);
        return hr;
    }

    return hr;
}

STDMETHODIMP CNAVOptions::ValidatePath(/*[in]*/ EBrowseType eBrowseType, /*[in]*/ BSTR bstrPath
                                                                       , /*[out, retval]*/ BOOL *pbValid)
{
	USES_CONVERSION;
	DWORD dwAttribute, dwError;

   	if (!pbValid)
		return Error(_T("ValidatePath()"), E_POINTER);
	*pbValid = VARIANT_FALSE;

	if (!bstrPath)
		return Error(_T("ValidatePath()"), E_INVALIDARG);

    if ( ::SysStringLen ( bstrPath ) > MAX_PATH )
        return E_POINTER;

	// Get attribute of the item
	if (0xFFFFFFFFUL != (dwAttribute = ::GetFileAttributes(OLE2T(bstrPath))))
		switch(eBrowseType & ~Browse_Exist)
		{
		case Browse_Directory:
			// For a directory validation we must make sure that there's no file existing with that name
			if(FILE_ATTRIBUTE_DIRECTORY == (FILE_ATTRIBUTE_DIRECTORY & dwAttribute))
				*pbValid = VARIANT_TRUE;
			break;
		case Browse_File:
			// For for file validation we want to make sure that there is no directory with the same name
			if (FILE_ATTRIBUTE_DIRECTORY != (FILE_ATTRIBUTE_DIRECTORY & dwAttribute))
				*pbValid = VARIANT_TRUE;
			break;
		case Browse_Disk:  // TODO: Handle disks
			return Error(IDS_Err_ValidatePath, _T("ValidatePath()"));
		}
	else if (ERROR_FILE_NOT_FOUND == (dwError = ::GetLastError())
	      || ERROR_PATH_NOT_FOUND == dwError)
	{
		if (Browse_Exist != (Browse_Exist & eBrowseType))
			*pbValid = VARIANT_TRUE;  // item doesn't exists
	}
	else
		return Error(IDS_Err_ValidatePath, _T("ValidatePath()"));

	return S_OK;
}

/////////////////////////////////////////////////////////////////////////////
// Properties
STDMETHODIMP CNAVOptions::get_IsTrialValid(/*[out, retval]*/ BOOL *pbIsTrialValid)
{
	if (!pbIsTrialValid)
		return Error(_T("get_IsTrialValid()"), E_POINTER);

	*pbIsTrialValid = VARIANT_FALSE;

	DWORD dwValue = 0;
    if( SUCCEEDED(m_pNavOpts->GetValue(NAVOPTION_FeatureEnabled, dwValue, 0)) )
		*pbIsTrialValid = (dwValue == 0) ? VARIANT_FALSE : VARIANT_TRUE;

	return S_OK;
}

STDMETHODIMP CNAVOptions::get_SystemMode(/*[out, retval]*/ long *plSystemMode)
{
	if (!plSystemMode)
		return Error(_T("get_SystemMode()"), E_POINTER);

	*plSystemMode =  ::GetSystemMetrics(SM_CLEANBOOT);

	return S_OK;
}

STDMETHODIMP CNAVOptions::get_NortonAntiVirusPath(/*[out, retval]*/ BSTR *pbstrNortonAntiVirusPath)
{
	USES_CONVERSION;

	if ( forceError (ERR_INVALID_POINTER) ||
        !pbstrNortonAntiVirusPath)
    {
        MakeError(ERR_INVALID_POINTER, E_POINTER, IDS_ISSHARED_ERROR_INTERNAL);
		return E_POINTER;
    }

    if( forceError (IDS_ERR_GETTING_NAVPATH) )
    {
        MakeError(IDS_ERR_GETTING_NAVPATH, E_FAIL, IDS_ISSHARED_ERROR_INTERNAL_REINSTALL);
		return E_FAIL;
    }

	*pbstrNortonAntiVirusPath = ::SysAllocString(g_NAVInfo.GetNAVDir());

    if( pbstrNortonAntiVirusPath )
        return S_OK;
    else
    {
        MakeError(IDS_ERR_GETTING_NAVPATH, E_FAIL, IDS_ISSHARED_ERROR_INTERNAL_REINSTALL);
		return E_FAIL;
    }
}

// Splits the option name in wcsOptName into an Option Name and a Group Name.
HRESULT CNAVOptions::SplitOptionName( const CStringW wcsOption, /*out*/CStringW& wcsOptName, /*out*/CStringW& wcsOptGroup )
{
	long lIndex;

	// Options must be in GROUP:Name format.
	if( 0 < (lIndex = wcsOption.Find(':')) )
	{
		wcsOptGroup = wcsOption.Left(lIndex);	// Everything to the left of ':'
		wcsOptName = wcsOption.Right(wcsOption.GetLength() - lIndex  - 1); // Everything to the right of ':'
		return S_OK;
	}
	else
		return E_INVALIDARG;
}
 

HRESULT CNAVOptions::registerCommandLines(void)
{
    // Get LU Callback dll
    HINSTANCE hLUCallback = NULL;
    TCHAR szLUCBPath[MAX_PATH*2];

    _tcscpy(szLUCBPath, g_NAVInfo.GetNAVDir());
    _tcscat(szLUCBPath, _T("\\navlucbk.dll"));

    // Verify the signature on the officeAV plug in dll
    if( NAVToolbox::NAVTRUST_OK != NAVToolbox::IsSymantecSignedImage(szLUCBPath) )
    {
        return E_FAIL;
    }

    hLUCallback = LoadLibraryEx(szLUCBPath, NULL, LOAD_WITH_ALTERED_SEARCH_PATH);

    if( NULL != hLUCallback )
    {
        typedef void (WINAPI *pfnREGISTERCMDLINES) (void);
        pfnREGISTERCMDLINES Register = reinterpret_cast<pfnREGISTERCMDLINES>(::GetProcAddress(hLUCallback, "RegisterCmdLines"));

        if( NULL != Register )
        {
            // Let's attempt to register the command lines
            Register();
            return S_OK;
        }
        else
        {
            // Could not retrieve function
            return E_FAIL;
        }
    
    }
    else
    {
        // Could not retrieve callback Dll handle
        return E_FAIL;
    }
}

// Loops through all existing keys in ccSettings and caches their type
// for use by the Get(), Set() and Default() functions.
bool CNAVOptions::EnumerateSettings(_NAVSETTINGSMAP* pMap)
{
    bool bRet = false;

    if( pMap )
    {
        for( _NAVSETTINGSMAP::iterator it = pMap->begin(); it != pMap->end(); it++ )
        {
            if( it->second )
            {
                CSymPtr<ccSettings::IEnumValues> pEnumValues;
                if( SYM_SUCCEEDED((it->second)->EnumValues(&pEnumValues)) )
                {
                    // Get all the data for these values
                    DWORD dwCount = 0;
                    if( SYM_SUCCEEDED( pEnumValues->GetCount(dwCount) ) )
                    {
                        // If we have at least one value consider this enumeration a success
                        if( dwCount > 0 )
                            bRet = true;

                        // Allocate a buffer big enough to hold the largest possible setting name
                        wchar_t *pszItemName = new wchar_t[m_dwMaxName];
                        if( !pszItemName )
                        {
                            bRet = false;
                        }

                        pair<OPTYPEMAP::iterator, bool> pr;

                        for(DWORD i=0; i<dwCount && pszItemName; i++)
                        {
                            DWORD dwSize = m_dwMaxName;
                            DWORD dwType = 0;
                            LPTSTR pszNewVal = NULL;
                            
                            if( SYM_SUCCEEDED(pEnumValues->GetItem(i, pszItemName, dwSize, dwType)) )
                            {
                                std::wstring strOptName;
                                CNAVOptSettingsCache::GetNavoptName(it->first.c_str(), pszItemName, strOptName);

                                // Save the type of this option so we can coerce VARIANTs
                                USES_CONVERSION;
			                    pr = m_OptionsTypes.insert(OPTYPEMAP::value_type(strOptName.c_str(), dwType));
                                _ASSERT(pr.second);  // Validate that such value didn't exist
                            }
                        }

                        if( pszItemName )
                            delete [] pszItemName;
                    }
                }
            }
        }
    }
    return bRet;
}

STDMETHODIMP CNAVOptions::get_ProductName(/*[out, retval]*/ BSTR *pVal)
{
	if(pVal)
	{
		CComBSTR bstrProductName(m_csProductName);
		*pVal = bstrProductName.Detach();
	}

	return S_OK;
}

STDMETHODIMP CNAVOptions::get_FeatureName(/*[out, retval]*/ BSTR *pVal)
{
	if(pVal)
	{
		CComBSTR bstrFeatureName(m_csTitle);
		*pVal = bstrFeatureName.Detach();
	}

	return S_OK;
}

STDMETHODIMP CNAVOptions::get_THREATSBYVID(/*[out, retval]*/ IThreatsByVID **ppThreatsByVID)
{
    // Make sure the caller is Symantec signed
    if (forceError (IDS_ERR_IT_IS_NOT_SAFE))
    {
        MakeError(IDS_ERR_IT_IS_NOT_SAFE, E_FAIL, IDS_ISSHARED_ERROR_INTERNAL_REINSTALL);
        return E_FAIL;
    }

    HRESULT hr = E_FAIL;

    if (forceError (ERR_INVALID_POINTER) || !ppThreatsByVID)
    {
        MakeError(ERR_INVALID_POINTER, E_POINTER, IDS_ISSHARED_ERROR_INTERNAL);
        return E_POINTER;
    }

    *ppThreatsByVID = NULL;

    if( m_spThreatsByVID )
    {
        (*ppThreatsByVID = m_spThreatsByVID)->AddRef();
        return S_OK;
    }

    // Need to create the IThreatsByVID object
    if( FAILED(hr = m_spThreatsByVID.CoCreateInstance(CLSID_ThreatsByVID)) )
    {
        MakeError(IDS_ERR_CREATE_THEATSBYVID, hr, IDS_ISSHARED_ERROR_INTERNAL);
        return hr;
    }

    (*ppThreatsByVID = m_spThreatsByVID)->AddRef();

    return S_OK;
}

HRESULT CNAVOptions::ProcessSnoozeables()
{
	HRESULT hr;
	DWORD dwVal;
    HWND hWnd = g_hWndNavOpts;
	
	if(m_bEmailIncomingEnabledAtStart)
	{
		if(FAILED(m_EmailOptions.GetScanIncoming(dwVal)))
			CCTRCTXW0(_T("Error getting IAVEmail:ScanIncoming."));
		else if(!dwVal)
		{
			if(FAILED(hr = m_spSnoozeDlg->Snooze(hWnd, &(ISShared::CLSID_NIS_EmailScanningInbound), 1)))
			{
				CCTRCTXE1(_T("Error snoozing Inbound Email Scanning. HR = 0x%08x"), hr);
			}
			else if(hr == S_FALSE)
			{
				// Cancel
				if(FAILED(m_EmailOptions.SetScanIncoming(TRUE)))
				{
					CCTRCTXE0(_T("Error resetting IAVEmail:ScanIncoming."));
				}
			}
		}
	}

	if(m_bEmailOutgoingEnabledAtStart)
	{
		if(FAILED(m_EmailOptions.GetScanOutgoing(dwVal)))
			CCTRCTXW0(_T("Error getting IAVEmail:ScanOutgoing."));
		else if(!dwVal)
		{
			if(FAILED(hr = m_spSnoozeDlg->Snooze(hWnd, &(ISShared::CLSID_NIS_EmailScanningOutbound), 1)))
			{
				CCTRCTXE1(_T("Error snoozing Outbound Email Scanning. HR = 0x%08x"), hr);
			}
			else if(hr == S_FALSE)
			{
				// Cancel
				if(FAILED(m_EmailOptions.SetScanOutgoing(TRUE)))
				{
					CCTRCTXE0(_T("Error resetting IAVEmail:ScanOutgoing."));
				}
			}
		}
	}

	// AP
	if(!m_bAPEnabled && m_bAPEnabledAtStart)
	{
		// If AP's was on at the start and the user wants it off, snooze it.
		if(FAILED(hr = m_spSnoozeDlg->Snooze(hWnd, &(ISShared::CLSID_NIS_AutoProtect), 1)))
		{
			CCTRCTXE1(_T("Error snoozing AutoProtect. HR = 0x%08x"), hr);
		}
		else if(hr == S_FALSE)
		{
			m_bAPEnabled = true;
		}
	}

	// IWP
	if(m_IWPOptions.IsDirty())
	{
		bool bOn, bUserOn;
		if(FAILED(hr = m_IWPOptions.GetIWPUserEnabled(bUserOn)))
		{
			CCTRCTXE1(_T("Error while getting IWP User state. HR = 0x%08x"), hr);
		}
		else if(FAILED(hr = m_IWPOptions.IsIWPOn(bOn)))
		{
			CCTRCTXE1(_T("Error while getting current IWP state. HR = 0x%08x"), hr);
		}
		else
		{
			// If IWP is on and the user wants it off, snooze it.
			if( !bUserOn && bOn)
			{
				if(FAILED(hr = m_spSnoozeDlg->Snooze(hWnd, &(ISShared::CLSID_NAV_IWP), 1)))
				{
					CCTRCTXE1(_T("Error snoozing IWP. HR = 0x%08x"), hr);
				}
				else if(hr == S_FALSE)
				{
					if(FAILED(hr = m_IWPOptions.SetIWPUserEnabled(true)))
						CCTRCTXE1(_T("Error resetting IWP User Enabled state to true. Hr = 0x%08x"), hr);
				}
			}
		}
	}

	// ALU
	if(m_ALUWrapper.Dirty() == S_OK)
	{
		bool bEnable;
		if(FAILED(hr = m_ALUWrapper.GetALUUserOn(bEnable)))
		{
			CCTRCTXE1(_T("Failed to check ALU User On setting. HR = 0x%08x"),hr);
		}
		else if(!bEnable)
		{
			// If AP's was on at the start and the user wants it off, snooze it.
			if(FAILED(hr = m_spSnoozeDlg->Snooze(hWnd, &(ISShared::CLSID_NIS_ALU), 1)))
			{
				CCTRCTXE1(_T("Error snoozing ALU. HR = 0x%08x"), hr);
			}
			else if(hr == S_FALSE)
			{
				if(FAILED(hr = m_ALUWrapper.SetALUUserOn(true)))
					CCTRCTXE1(_T("Error resetting ALU User Enabled state to true. Hr = 0x%08x"), hr);
			}
		}
	}

	return S_OK;
}

// Themes the window. Thanks to the funky voodoo of API hooking, this should cause
// ccWebWnd to get skinned by SkinMagic.
HRESULT CNAVOptions::InitSymTheme()
{    
	SYMRESULT sym_result = SYM_OK;
	sym_result = symtheme::loader::ISymSkinWindow::CreateObject( GETMODULEMGR(), &m_spSkin );

	if( SYM_FAILED(sym_result) )
	{
		CCTRACEE(CCTRCTX L"Unable to create SymTheme object");
		ATLASSERT(FALSE);
		return E_FAIL;
	}

	// Init the skin engine

	HRESULT hr = S_OK;
	hr = m_spSkin->Init( _Module.GetModuleInstance(), NULL );

	if( FAILED(hr) )
	{
		CCTRACEE(CCTRCTX L"Unable to init the SymTheme object");
		ATLASSERT(FALSE);
		return hr;
	}

	hr = m_spSkin->LoadSkin( symtheme::SKINID_SymantecCommon );

	if( FAILED(hr) )
	{
		CCTRACEE(CCTRCTX L"Unable to load our SymTheme SMF file");
		ATLASSERT(FALSE);
		return hr;
	}

    hr = m_spSkin->SetDialogSkin( symtheme::sections::OptionsDialog );
	
	if( FAILED(hr) )
	{
		CCTRCTXE1(_T("Failed to set dialog skin. HR = 0x%08x"), hr);
		return hr;
	}

	CCTRCTXI0(_T("SymTheme loaded successfully."));
	return S_OK;
}

// Ugly voodoo
LRESULT CALLBACK myHook(int code, WPARAM wParam, LPARAM lParam)
{
    if(code == HSHELL_WINDOWCREATED)
    {
        g_hWndNavOpts = (HWND)wParam;
        UnhookWindowsHookEx(m_hook);
        m_hook = NULL;
    }
    return 0;
}

HRESULT CNAVOptions::get_Is64Bit(BOOL* pVal)
{
    if( forceError ( ERR_INVALID_POINTER ) || NULL == pVal )
    {
        MakeError (ERR_INVALID_POINTER, E_POINTER, IDS_ISSHARED_ERROR_INTERNAL);
        return E_POINTER;
    }

    *pVal = m_bWin64?TRUE:FALSE;
    return S_OK;
}

HRESULT CNAVOptions::get_IsVista(BOOL* pVal)
{
    if( forceError ( ERR_INVALID_POINTER ) || NULL == pVal )
    {
        MakeError (ERR_INVALID_POINTER, E_POINTER, IDS_ISSHARED_ERROR_INTERNAL);
        return E_POINTER;
    }
    *pVal = m_bVista?TRUE:FALSE;
    return S_OK;
}
