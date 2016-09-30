// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright (c) 2006, Symantec Corporation, All rights reserved.
// HPPProtectionProvider.cpp : Implementation of CHPPProtectionProvider

#include "stdafx.h"
#include "HPPProtectionProvider.h"
#include "HPPBasic.h"
#include "terminalsession.h"
#include "servicehelper.h"
#include "SymSaferRegistry.h"
#include "TrustUtil.h"
#if 0
////////////////////////////////////////////////////////////////////////
// Function:    CopyInterfaceFixed
//
// Description: Temporary stub class to resolve C2664 in ATL's 
//				_CopyInterface function due to const mismatch
//				with copy member signature.
//
// Parameters:  None
//
// Returns:     None
//
//
///////////////////////////////////////////////////////////////////////////////
template<class T>
class CopyInterfaceFixed
{
public:
	static HRESULT copy(T** p1, T* const* p2)
	{
		ATLENSURE(p1 != NULL && p2 != NULL);
		*p1 = *p2;
		if (*p1)
			(*p1)->AddRef();
		return S_OK;
	}
	static void init(T** ) {}
	static void destroy(T** p) {if (*p) (*p)->Release();}
};
#endif
////////////////////////////////////////////////////////////////////////
// Function:    IsSAVServiceRunning
//
// Description: check to see if the service is running
//
// Parameters:  BOOL& bRunning The method fills in the status here.
//
// Returns:     None
//
//
///////////////////////////////////////////////////////////////////////////////
BOOL IsSAVServiceRunning(BOOL& bRunning )
{
	BOOL bWin9X = FALSE;
	BOOL bWinNT = FALSE;
	
	OSVERSIONINFO ver;
    ver.dwOSVersionInfoSize = sizeof( OSVERSIONINFO );

    if( GetVersionEx( &ver ) == 0)
		return FALSE;

	if(ver.dwPlatformId == VER_PLATFORM_WIN32_NT)
		bWinNT = TRUE;
	else if(ver.dwPlatformId == VER_PLATFORM_WIN32_WINDOWS)
		bWin9X = TRUE;

	if(bWin9X == TRUE)
	{
		return (IsServiceRunning(_T(SERVICE_NAME_9X), bRunning));
	}
	else if( (bWinNT == TRUE) )
	{
		return (IsServiceRunning(_T(SERVICE_NAME), bRunning));
	}

	return FALSE;
}

////////////////////////////////////////////////////////////////////////
// Function:    CoCreateScanEngine
//
// Description: Utility function to streamline the code for creating
//				pointer it rtvscan interfaces
//
// Parameters:  IID iInterface Desired interface
//				void **ppv pointer to desired interface.  Filled in by the 
//				routine
//
// Returns:     S_OK if successfull
//
//
///////////////////////////////////////////////////////////////////////////////
HRESULT CoCreateScanEngine( IID iInterface, void **ppv )
{
	HRESULT			hr;
	IVirusProtect	*pVirusProtect = NULL;
	BOOL bServiceRunning = FALSE;

	// First check registry to see if the user wants to the service loaded or not
	HKEY hkey;
	BOOL proxy = FALSE;
	//if we are here then we want CLIPROXY check if it is a non service console session or a remote session
	//if it is and they want CLIPROXY then give them cliscan since non service console session or remote session
	//are not fully functional between cliproxy and rtvscan
	if(IsNonServiceConsoleSession() || IsRemoteSession())
	{
		hr = CoCreateInstance( CLSID_Cliscan, NULL, CLSCTX_INPROC_SERVER, iInterface, ppv );
		return hr;
	}

	if (RegOpenKey(HKEY_LOCAL_MACHINE, _T(szReg_Key_Main), &hkey) == ERROR_SUCCESS) 
	{
		DWORD val=1,size=sizeof(val); // if ServiceWanted value does not exist assume that the user wants it.
        if (SymSaferRegQueryValueEx(hkey, _T(szReg_Val_ServiceWanted), 0, NULL, (PBYTE) &val, &size) != ERROR_SUCCESS)
			val = 1;  

		if (val) 
		{
			val = KEYVAL_NOTRUNNING; // if ServiceRunning value does not exist assume it's not running
			// use define from ClientReg.h for key (renaming to be less obvious)
			SymSaferRegQueryValueEx(hkey, _T(szReg_Val_ServiceRunning), 0, NULL, (PBYTE)&val, &size);
			// also, using a value other than zero to indicate not running
			if (KEYVAL_NOTRUNNING != val)
			{
				proxy = TRUE;
				//looks like the service is running do additional check if rtvscan service is running
				if( IsSAVServiceRunning(bServiceRunning) == TRUE)
				{
					proxy = bServiceRunning;
				}
			}
			// latter I might want to start service if user wanted it but it's not running
//			SymSaferRegQueryValueEx(hkey,szReg_Val_ServiceRunning,0,NULL,(PBYTE)&val,&size);
//			if (KEYVAL_NOTRUNNING == val)
//				start = TRUE;

		}
		RegCloseKey(hkey);
	}

	// Try CliProxy first, then CliScan
	//	NOTE: This is backwards from the COM algorithym, but it meets our needs
	if( proxy && SUCCEEDED( hr = CoCreateInstance( CLSID_CliProxy, NULL, CLSCTX_INPROC_SERVER, IID_IVirusProtect, (void**)&pVirusProtect ) ) )
	{
		//Now I need to make sure the service is running
		IServiceControl	*pServiceControl = NULL;

		if( SUCCEEDED( hr = pVirusProtect->CreateByIID( IID_IServiceControl, (void**)&pServiceControl ) ) )
		{
			//Service is NOT running
			if( pServiceControl->GetStatus() )
				hr = CoCreateInstance( CLSID_Cliscan, NULL, CLSCTX_INPROC_SERVER, iInterface, ppv );
			else //Otherwise, get the interface asked for
				hr = pVirusProtect->QueryInterface( iInterface, ppv );

			//Release my Service Control object
			pServiceControl->Release();
		}

		//and release the interface I got
		pVirusProtect->Release();
	}
	else
		hr = CoCreateInstance( CLSID_Cliscan, NULL, CLSCTX_INPROC_SERVER, iInterface, ppv );

	return hr;
}
////////////////////////////////////////////////////////////////////////
// Function:    CoCreateLDVPObject
//
// Description: Utility function to streamline the code for creating
//				pointer in rtvscan interfaces
//
// Parameters:  CLSID clsid, id of desired interface
//				IID iInterface Desired interface
//				void **ppv pointer to desired interface.  Filled in by the 
//				routine
//
// Returns:     S_OK if successfull
//
//
///////////////////////////////////////////////////////////////////////////////
HRESULT CoCreateLDVPObject( CLSID clsid, IID iInterface, void **ppv )
{
	HRESULT			hr;
	IVirusProtect	*pVirusProtect = NULL;

	//First, I need to get a Scan engine object
	// If they are asking for a CliProxy object, then use the CoCreateScanEngine helper function
	if( clsid == CLSID_CliProxy ) 
		hr = CoCreateScanEngine( IID_IVirusProtect, (void**)&pVirusProtect );
	else
		hr = CoCreateInstance( clsid, NULL, CLSCTX_INPROC_SERVER, IID_IVirusProtect, (void**)&pVirusProtect );

	//If I have a valid scan engine,
	//	create the object asked for
	if( SUCCEEDED( hr ) )
	{
		hr = pVirusProtect->CreateByIID( iInterface, ppv );
		pVirusProtect->Release();
	}

	return hr;
}

//*********************************************************************
//
//		**** CHPPProtectionProvider ****
//
//*********************************************************************


////////////////////////////////////////////////////////////////////////
// Function:    CHPPProtectionProvider::CHPPProtectionProvider
//
// Description: Constructor for the CHPPProtectionProvider
//
// Parameters:  none
//
// Returns:     void
//
//
///////////////////////////////////////////////////////////////////////////////
CHPPProtectionProvider::CHPPProtectionProvider()
{
	m_ptrGenConfig = NULL;
}

////////////////////////////////////////////////////////////////////////
// Function:    CHPPProtectionProvider::FinalConstruct
//
// Description: Final Constructor for the CHPPProtectionProvider.  This allows
//				for work after the object has been created.  We create
//				the AP providers here.
//
// Parameters:  none
//
// Returns:     HRESULT S_OK if successfull, noraml error codes if not.
//
//
///////////////////////////////////////////////////////////////////////////////
HRESULT CHPPProtectionProvider::FinalConstruct()
{
	HRESULT hRc = S_OK;

    // Initialize trust checking
    hRc = trustVerifier.Initialize(CTrustVerifier::VerifyMode_CommonClient);
    if (FAILED(hRc))
        return hRc;

	// lets try to create the AP Provider HPPProtection
	hRc = m_ptrIHPPProtection.CoCreateInstance(CLSID_HPPProtection);
	if (SUCCEEDED(hRc))
	{
		// hey it works finish up the init
		try
		{
			DWORD dwType = CONFIG_TYPE_REALTIME;
			DWORD dwRootID = HKEY_VP_STORAGE_REALTIME;
			CString			strName= szReg_Key_SymHPPS;
			CString			strPageType;
			IGenericConfig	*ptrGenConfig = NULL;

			// save off the IHPPProtection into the protect list.  We need to be
			// able to return the list back to the framework if required.
			m_ptrHPPAutoProtects.push_back(m_ptrIHPPProtection);

			// create the IGenericConfig interface so all of the components and interfaces can read and write to the configuration store.
			if( SUCCEEDED( CoCreateLDVPObject( CLSID_CliProxy, IID_IGenericConfig, (void**)&ptrGenConfig ) ) ) 
				{
					// creation succedded now lets open the config store.
					DWORD dwErr = ptrGenConfig->Open( NULL, dwRootID, CT2A(strName.GetBuffer( strName.GetLength() )) , GC_MODEL_SINGLE /* flags ignored in Cli* objects */ );
					strName.ReleaseBuffer();					
					// bad things if we can't create and open 
					SAVASSERT(ptrGenConfig != NULL);
					if( dwErr != 0 )
					{
						TRACE1("ERROR: Open on IGenericConfig reutrned 0x%X\n", dwErr );
//						AfxMessageBox( IDS_ERROR_NO_OPEN );
					}
					else
					{
						m_ptrGenConfig = ptrGenConfig;
					}
				}
		}
		catch (std::bad_alloc&)
		{
			hRc = E_OUTOFMEMORY;
		}
	}
	// return
	return hRc;
}

////////////////////////////////////////////////////////////////////////
// Function:    CHPPProtectionProvider::FinalRelease
//
// Description: Final Releas for the CHPPProtectionProvider.  This allows
//				for work before the object is destroyed.  We release and
//				free up any interfaces we have stored.
//
// Parameters:  none
//
// Returns:     None
//
//
///////////////////////////////////////////////////////////////////////////////
void CHPPProtectionProvider::FinalRelease() 
{
    trustVerifier.Shutdown();

	// all done with the IGenericConfig pointer.
	if (m_ptrGenConfig != NULL)
	{
		m_ptrGenConfig->Release();
		m_ptrGenConfig = NULL;
	}
	
	// all done with the HPPProtection interface
	if (m_ptrIHPPProtection != NULL)
	{
		m_ptrIHPPProtection.Release();
		m_ptrIHPPProtection= NULL;
	}
	// clear the list of APs
	m_ptrHPPAutoProtects.clear();
}

////////////////////////////////////////////////////////////////////////
// Function:    CHPPProtectionProvider::ShowConfigureUI
//
// Description: Called by the UI Framework.  This is our chance to display	
//				our configuration UI.  This is the basic panel.  This method
//				will do the setup and create the basic panel dialog.
//
// Parameters:  HWND  parentWindowHandle, parent window handle on which to attach	
//				the dialog
//
// Returns:     HRESULT S_OK if successful stander UI error codes if not.
//
//
///////////////////////////////////////////////////////////////////////////////
HRESULT CHPPProtectionProvider::ShowConfigureUI( HWND  parentWindowHandle)
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState());

	if (parentWindowHandle == NULL)
		return E_INVALIDARG;
    // Validate caller security
    if (trustVerifier.IsCallerProcessTrusted() != S_OK)
        return E_ACCESSDENIED;

	// create a CWnd and attach it to the parent window.
	// TODO catch bad alloc.
	CWnd *ptrParentWindow = new CWnd();  // TODO static ??
	ptrParentWindow->Attach(parentWindowHandle);

	// create the basic panel
	CHPPBasic dlgOptions(CONFIG_MODE_CLIENT,0,ptrParentWindow);
	// if we have the generic config pointer set it in the dlg so the
	// dlg can use it.
	if (m_ptrGenConfig != NULL)
		dlgOptions.SetIConfig(m_ptrGenConfig);

	// start the dlg
	dlgOptions.DoModal();

	// fush the config out to the store
	m_ptrGenConfig->WriteData(TRUE);

	// cleanup
	ptrParentWindow->Detach();
	delete ptrParentWindow;
    
	return S_OK;
}
////////////////////////////////////////////////////////////////////////
// Function:    CHPPProtectionProvider::ShowLogUI
//
// Description: Called by the UI Framework.  This is our chance to display	
//				our log UI.  
//
// Parameters:  HWND  parentWindowHandle, parent window handle on which to attach	
//				the dialog
//
// Returns:     HRESULT S_OK if successful stander UI error codes if not.
//
//
///////////////////////////////////////////////////////////////////////////////
HRESULT CHPPProtectionProvider::ShowLogUI( HWND  parentWindowHandle)
{
    // Validate caller security
    if (trustVerifier.IsCallerProcessTrusted() != S_OK)
        return E_ACCESSDENIED;

	MessageBox(parentWindowHandle, _T("Coming soon to a client near you"), _T("HPP Log UI"), MB_OK | MB_ICONINFORMATION);
    return S_OK;
}

////////////////////////////////////////////////////////////////////////
// Function:    CHPPProtectionProvider::get_DisplayName
//
// Description: Called by the UI Framework.  This is our chance to display	
//				our log UI.  
//
// Parameters:  BSTR *  displayName, The name of this ProtectionProvider.
//				
//
// Returns:     HRESULT S_OK if successful stander UI error codes if not.
//
//
///////////////////////////////////////////////////////////////////////////////
HRESULT CHPPProtectionProvider::get_DisplayName( BSTR *  displayName)
{
	CComBSTR bstrDisplayName;

	// Validate parameters
	if (displayName == NULL)
		return E_INVALIDARG;

    // Validate caller security
    if (trustVerifier.IsCallerProcessTrusted() != S_OK)
        return E_ACCESSDENIED;

	// load the string from the rc
	// TODO try catch.
	bstrDisplayName.LoadString(IDS_HPP_PROTECTION_PROVIDER_DISAPLAYNAME);

	*displayName = bstrDisplayName.Detach();
	
	return S_OK;
}
////////////////////////////////////////////////////////////////////////
// Function:    CHPPProtectionProvider::get_Autoprotects
//
// Description: Called by the UI Framework.  Here we return the list 
//				of autoprotects.  In the case of HPPProtectionProvider there
//				is only one.  We may implement one per engine later
//
// Parameters:  IProtection_Container * *  autoprotects, The list of autoprotects
//				
//
// Returns:     HRESULT S_OK if successful stander UI error codes if not.
//
//
///////////////////////////////////////////////////////////////////////////////
HRESULT CHPPProtectionProvider::get_Autoprotects( IProtection_Container * *  autoprotects)
{
	// Validate parameters
	if (autoprotects == NULL)
		return E_INVALIDARG;

    // Validate caller security
    if (trustVerifier.IsCallerProcessTrusted() != S_OK)
        return E_ACCESSDENIED;

	// return the list of autoprotects that we setup in FinalConstruct
	return QueryInterface(IID_IProtection_Container, reinterpret_cast<void**>(autoprotects));
}
////////////////////////////////////////////////////////////////////////
// Function:    CHPPProtectionProvider::get_PrimaryAutoprotect
//
// Description: Called by the UI Framework.  Here we return the autoprotect
//              that represents most of our protection, if we are so structured.
//              We have only one, so we return that one.
//
// Parameters:  IProtection * *  autoprotects, The primary autoprotect
//				
//
// Returns:     HRESULT S_OK if successful stander UI error codes if not.
//
//
///////////////////////////////////////////////////////////////////////////////
HRESULT CHPPProtectionProvider::get_PrimaryAutoprotect( IProtection** primaryAutoprotect )
{
    // Validate caller security
    if (trustVerifier.IsCallerProcessTrusted() != S_OK)
        return E_ACCESSDENIED;

	if (m_ptrIHPPProtection != NULL)
    {
        *primaryAutoprotect = m_ptrIHPPProtection;
        (*primaryAutoprotect)->AddRef();
        return S_OK;
    }
    else
    {
        return E_FAIL;
    }
}

////////////////////////////////////////////////////////////////////////
// Function:    CHPPProtectionProvider::get_ID
//
// Description: Called by the UI Framework.  Here we return the GUID for this
//				Protection Provider
//
// Parameters:  GUID *  id, variable to set our GUID 
//				
//
// Returns:     HRESULT S_OK if successful stander UI error codes if not.
//
//
///////////////////////////////////////////////////////////////////////////////
HRESULT CHPPProtectionProvider::get_ID( GUID *  id)
{
	// Validate parameter
	if (id == NULL)
		return E_INVALIDARG;

    // Validate caller security
    if (trustVerifier.IsCallerProcessTrusted() != S_OK)
        return E_ACCESSDENIED;

	// set our GUID
	*id = CLSID_HPPProtectionProvider;
	return S_OK;
}
////////////////////////////////////////////////////////////////////////
// Function:    CHPPProtectionProvider::get_SplashGraphic
//
// Description: Called by the UI framwork when the framework needs to
//              get the splash graphic to show for us.
//
// Parameters:  HGDIOBJ *  id, set to our splash bitmap
//				
//
// Returns:     HRESULT S_OK if successful stander UI error codes if not.
//
//
///////////////////////////////////////////////////////////////////////////////
HRESULT CHPPProtectionProvider::get_SplashGraphic( HGDIOBJ* bitmapHandle )
{
    // Validate caller security
    if (trustVerifier.IsCallerProcessTrusted() != S_OK)
        return E_ACCESSDENIED;

	// No graphic to display at the moment - let the UI use it's default.
    return E_FAIL;
}

//*********************************************************************
//
//		**** IProtection_Container Methods ****
//
//*********************************************************************

////////////////////////////////////////////////////////////////////////
// Function:    CHPPProtectionProvider::get_ProtectionCount
//
// Description: Called by the UI Framework.  We return the number of 
//				protection providers that we support.
//
// Parameters:  long *  noItems, set the # of APs
//				
//
// Returns:     HRESULT S_OK if successful stander UI error codes if not.
//
//
///////////////////////////////////////////////////////////////////////////////
HRESULT CHPPProtectionProvider::get_ProtectionCount( long *  noItems)
{
    // Validate caller security
    if (trustVerifier.IsCallerProcessTrusted() != S_OK)
        return E_ACCESSDENIED;

	// Validate
	if (noItems == NULL)
		return E_INVALIDARG;
	
	// There is always AP for HPP
	*noItems = NO_HPP_AP;

	return S_OK;
}


////////////////////////////////////////////////////////////////////////
// Function:    CHPPProtectionProvider::get_Item
//
// Description: Called by the UI Framework.  return the protection provider 
//				specified by the GUID.  If we had a list we would have to do
//				a lookup, but we only have one so return that one.
//
// Parameters:  GUID  itemID, The GUID of the Protection that is desired.
//
//				IProtection * *  protection, pointer to the Protection Interface
//				to be filled in by the method
//				
//
// Returns:     HRESULT S_OK if successful stander UI error codes if not.
//
//
///////////////////////////////////////////////////////////////////////////////
HRESULT CHPPProtectionProvider::get_Item( GUID  itemID,  IProtection * *  protection)
{
	HRESULT hRc = E_FAIL;
	GUID	guidHPPProtect = GUID_NULL;

    // Validate caller security
    if (trustVerifier.IsCallerProcessTrusted() != S_OK)
        return E_ACCESSDENIED;

	if (protection == NULL)
		return E_INVALIDARG;

	// make sure we have it.
	if (SUCCEEDED(m_ptrIHPPProtection->get_ID(&guidHPPProtect)))
	{
		// check to make sure they match.
		if(itemID == guidHPPProtect)
		{
			// they match setup return
			*protection = m_ptrIHPPProtection;	
			(*protection)->AddRef();
			hRc = S_OK;
		}
	}

	return hRc;
}


////////////////////////////////////////////////////////////////////////
// Function:    CHPPProtectionProvider::get__NewEnum
//
// Description: Called by the UI Framework.  Gets an enum interface to enum
//				the protection list
//
// Parameters:  IEnumProtection**  enumerator, enumerater of the protection list
//
//
// Returns:     HRESULT S_OK if successful stander UI error codes if not.
//
//
///////////////////////////////////////////////////////////////////////////////
HRESULT CHPPProtectionProvider::get__NewEnum( IEnumProtection**  enumerator)
{
    typedef CComEnumOnSTL< IEnumProtection, &IID_IEnumProtection, IProtection*, CopyInterfaceFixed<IProtection>, ProtectionList > CComEnumProtectionOnList;
    CComObject<CComEnumProtectionOnList>*           actualEnumerator        = NULL;
	HRESULT											returnValHR				= E_FAIL;

    // Validate parameters
    if (enumerator == NULL)
        return E_POINTER;

    // Validate caller security
    if (trustVerifier.IsCallerProcessTrusted() != S_OK)
        return E_ACCESSDENIED;

	// TODO need to valide caller by ccVerify Trust.
    returnValHR = CComObject<CComEnumProtectionOnList>::CreateInstance(&actualEnumerator);
    if (SUCCEEDED(returnValHR))
    {
        returnValHR = actualEnumerator->Init(NULL, m_ptrHPPAutoProtects);
        if (SUCCEEDED(returnValHR))
            returnValHR = actualEnumerator->QueryInterface(IID_IEnumProtection, (void**) enumerator);
    }
	return returnValHR;
}

HRESULT CHPPProtectionProvider::get_AdministrativeExceptions( IExceptionItem_Container** administrativeExceptions )
{
    return E_NOTIMPL;
}

HRESULT CHPPProtectionProvider::get_LocalExceptions( IExceptionItem_Container** localExceptions )
{
    return E_NOTIMPL;
}

HRESULT CHPPProtectionProvider::put_LocalExceptions( IExceptionItem_Container* newLocalExceptions )
{
    return E_NOTIMPL;
}
HRESULT CHPPProtectionProvider::get_Installed( VARIANT_BOOL* isInstalled )
{
    return E_NOTIMPL;
}
HRESULT CHPPProtectionProvider::get_ConfigureableScans( IProtection_ConfigureableScan_Container** scans )
{
    return E_NOTIMPL;
}
