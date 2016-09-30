// NAVOptionsProperties.cpp : Implementation of CNAVOptions

#include "StdAfx.h"
#include "DefAlert.h"  // LiveUpdate
#include "NAVOptionsObj.h"
#include "navtrust.h"

#include "NetDetectController_i.c"  // LiveUpdate
#include "ProductRegCOM_i.c"
#include "..\navoptionsres\resource.h"
#include "NAVErrorResource.h"

// For Office 2000 plug in
#define DMSOFFICE_REGKEY _T("Software\\Microsoft\\Office")
const PTCHAR MSOFFICE_REGKEY = DMSOFFICE_REGKEY;
const PTCHAR STR_GUIDMSOfficeAV = _T("CLSID\\{DE1F7EEF-1851-11D3-939E-0004AC1ABE1F}\\ProgID");
DEFINE_GUID(GUID_MSOfficeAntiVirus, 0xde1f7eef, 0x1851, 0x11d3, 0x93, 0x9e, 0x0, 0x04, 0xac, 0x1a, 0xbe, 0x1f);
// CATID for Microsoft Office AntiVirus COM category
// {56FFCC31-D398-11d0-B2AE-00A0C908FA49}
DEFINE_GUID(CATID_MSOfficeAntiVirus, 0x56ffcc30, 0xd398, 0x11d0, 0xb2, 0xae, 0x0, 0xa0, 0xc9, 0x8, 0xfa, 0x49);

class CCleaner
{
	char *m_p;
public:
	CCleaner(char *p = NULL) : m_p(p) {}
	~CCleaner(void) { if (m_p) delete [] m_p; }
	char* operator=(char* p) { return m_p = p; }
};

/////////////////////////////////////////////////////////////////////////
// CProperty: Property encapsulation
/////////////////////////////////////////////////////////////////////////
CNAVOptions::CProperty::CProperty(CNAVOptions* pParent, PWCHAR pszName)
                      : m_eState(Prop_Unknown)
                      , m_eOldState(Prop_Unknown)
                      , IProperty(pParent, pszName)
{
}

HRESULT CNAVOptions::CProperty::Get(BOOL *pbState)
{
	HRESULT hr;

	if (m_rParent.forceError ( ERR_INVALID_POINTER ) || 
        !pbState)
	{
		m_rParent.MakeError (ERR_INVALID_POINTER, E_POINTER, IDS_NAVERROR_INTERNAL);
		return E_POINTER;
	}
	*pbState = VARIANT_FALSE;

	// Re-load custom property if not loaded yet
	if (Prop_Unknown == m_eState && FAILED(hr = Load()))
		return hr;

	*pbState = m_eState;

	return S_OK;
}

HRESULT CNAVOptions::CProperty::Put(BOOL bState)
{
	m_eState = bState ? Prop_True : Prop_False;

	return S_OK;
}

bool CNAVOptions::CProperty::IsDirty(void)
{
	return Prop_Unknown != m_eState && m_eOldState != m_eState;
}

void CNAVOptions::CProperty::Init(void)
{
	m_eOldState = m_eState = Prop_Unknown;
}

HRESULT CNAVOptions::CProperty::Default(const char* pcszDefault)
{
	HRESULT hr;
	DWORD dwDefault;

	// Get default value form the DEFAULT section of the options file
	if (FAILED(hr = m_rParent.Default(pcszDefault, dwDefault)))
		return hr;

	m_eState = static_cast<EState>(dwDefault);

	return S_OK;
}

/////////////////////////////////////////////////////////////////////////
// Properties
/////////////////////////////////////////////////////////////////////////
HRESULT CNAVOptions::InitNDScheduler(void)
{
	HRESULT hr = S_OK;

	// Init the NetDetect scheduling object
    // First check for a digital signature on the COM object
	/*if( forceError ( IDS_ERR_INVALIDSIG_NDSCHEDULER ) ||
        NAVToolbox::NAVTRUST_OK != NAVToolbox::IsSymantecComServer(CLSID_NDScheduler) )
    {
        // Invalid signature
		MakeError (IDS_ERR_INVALIDSIG_NDSCHEDULER, E_ACCESSDENIED, IDS_NAVERROR_NDSCHEDULER);
        return E_ACCESSDENIED;
    }*/

	if (!m_spNDScheduler
	 && SUCCEEDED(hr = m_spNDScheduler.CoCreateInstance( CLSID_NDScheduler, NULL
	                                                   , CLSCTX_INPROC_SERVER )))
		if (E_FAIL == (hr = m_spNDScheduler->Init()))
		{
			// On systems that do not have the MS Scheduler installed
			// We get this error.
			m_spNDScheduler.Release();
            return REGDB_E_CLASSNOTREG;
		}

    if ( forceError (IDS_ERR_CREATE_NDSCHEDULER) )
        hr = E_FAIL;

    if( FAILED(hr) )
        MakeError(IDS_ERR_CREATE_NDSCHEDULER,hr, IDS_NAVERROR_NDSCHEDULER);
	return hr;
}

HRESULT CNAVOptions::InitProductReg(void)
{
	HRESULT hr = S_OK;

	// Init the LiveUpdate ProductReg object
	if (!m_spProductReg)
	{
		bool bALURunning = false;

		// On NT platforms we need to stop ALU from running as the
		// system process by creating it's mutex
		if( g_OSInfo.IsWinNT() )
		{
			// If we have the mutex from a previous check then wait at
			// least 5 seconds before releasing it to ensure that the
			// ALU's instance of LUComServer.exe has gone bye bye

			SECURITY_ATTRIBUTES sa;
			SECURITY_DESCRIPTOR sd;

			// Setup the descriptor
			InitializeSecurityDescriptor(&sd, SECURITY_DESCRIPTOR_REVISION);

			// Add a null DACL to the security descriptor
			SetSecurityDescriptorDacl(&sd, TRUE, (PACL) NULL, FALSE);
			sa.nLength = sizeof(sa); 
			sa.lpSecurityDescriptor = &sd;
			sa.bInheritHandle = TRUE;
			if(NULL == m_hALUMutex) //if we created this mutex, then don't attempt to recreate to check if ALU is running
			{
				m_hALUMutex = CreateMutex(&sa,TRUE,SYM_AUTO_UPDATE_MUTEX);
				bALURunning = ( GetLastError() == ERROR_ALREADY_EXISTS );
			}
			else
			{
				//use the cached value of bALURunning based on our first check for the named mutex object
				bALURunning = m_bALURunning;
			}

		}

		// If we're on 9x -or- NT and ALU is not currently running just create the
		// product reg object
		if( !g_OSInfo.IsWinNT() || !bALURunning )
		{
			m_bALURunning = FALSE;
			hr = m_spProductReg.CoCreateInstance( CLSID_luProductReg, NULL
												  , CLSCTX_INPROC_SERVER | CLSCTX_LOCAL_SERVER );

			if( FAILED(hr) )
			{
				// This failed for a reason other than ALU was running so reset everything
				if( m_hALUMutex )
				{
					CloseHandle(m_hALUMutex);
					m_hALUMutex = NULL;
				}
			}
		}
		else // We are on NT and ALU is running, disable the ALU page by setting the m_bALURunning flag
		{
			m_bALURunning = TRUE;

			// Release the handle so we can try again next time
			CloseHandle(m_hALUMutex);
			m_hALUMutex = NULL;
			return S_FALSE;
		}
	}

	return hr;
}

/////////////////////////////////////////////////////////////////////////
// LiveUpdate
STDMETHODIMP CNAVOptions::get_LiveUpdate(BOOL *pbLiveUpdate)
{
	return m_LiveUpdate.Get(pbLiveUpdate);
}

STDMETHODIMP CNAVOptions::put_LiveUpdate(BOOL bLiveUpdate)
{
    // Make sure the caller is Symantec signed
    if ( forceError (IDS_ERR_IT_IS_NOT_SAFE) ||
        !IsItSafe())
	{
        MakeError(IDS_ERR_IT_IS_NOT_SAFE, E_FAIL, IDS_NAVERROR_INTERNAL_REINSTALL);
	    return E_FAIL;
    }

	return m_LiveUpdate.Put(bLiveUpdate);
}

HRESULT CNAVOptions::CPropertyLiveUpdate::Load(void)
{
	HRESULT hr;
	BOOL bLiveUpdate;

	// Init the NetDetect scheduling object
	if (FAILED(hr = m_rParent.InitNDScheduler()))
		// If ALU is not installed leave this property un-initlized.
		return REGDB_E_CLASSNOTREG == hr ? S_FALSE : hr;

	if (FAILED(hr = m_rParent.GetNDScheduler()->GetEnabled(&bLiveUpdate)))
	{
		// GetEnabled() failed because it's not there
		if (m_rParent.forceError (IDS_ERR_LOAD_NDSCHEDULER) ||
            FAILED(hr = m_rParent.GetNDScheduler()->AddTask())
		// If we failed to add, it's probably because it exists already.
		 && FAILED(hr = m_rParent.GetNDScheduler()->RestartSchedule())) // So it doesn't trigger right away
        {
            if ( m_rParent.forceError (IDS_ERR_LOAD_NDSCHEDULER))
                hr = E_FAIL;

            m_rParent.MakeError(IDS_ERR_LOAD_NDSCHEDULER,hr, IDS_NAVERROR_INTERNAL);
			return hr;
        }

		if (FAILED(hr = m_rParent.GetNDScheduler()->GetEnabled(&bLiveUpdate))) // Check again
        {
            m_rParent.MakeError(IDS_ERR_LOAD_NDSCHEDULER,hr, IDS_NAVERROR_INTERNAL);
			return hr;
        }
	}

	// If ALU is not installed leave this property un-initlized.
	if (REGDB_E_CLASSNOTREG == hr)
		return S_FALSE;

	if (SUCCEEDED(hr))
		m_eState    =
		m_eOldState = bLiveUpdate ? Prop_True : Prop_False;

	return hr;
}

HRESULT CNAVOptions::CPropertyLiveUpdate::Save(void)
{
    
	HRESULT hr = S_OK;

	if (Prop_Unknown != m_eState && m_eOldState != m_eState)
	{
		// Init the NetDetect scheduling object
		if (FAILED(hr = m_rParent.InitNDScheduler()))
			// If ALU is not installed leave this property un-initlized.
			return REGDB_E_CLASSNOTREG == hr ? S_FALSE : hr;

		switch(m_eState)
		{
		case Prop_True:
			if (FAILED(hr = m_rParent.GetNDScheduler()->Enable()))      // Make it go
			{
				// Enable() failed because it's not there
				if (FAILED(hr = m_rParent.GetNDScheduler()->AddTask())
				// If we failed to add, it's probably because it exists already.
				 && FAILED(hr = m_rParent.GetNDScheduler()->RestartSchedule())) // So it doesn't trigger right away
					return Error(IDS_Err_ALUSave, _T("CPropertyLiveUpdate::Save()"));

				// Try to make it go again.
				if (FAILED(hr = m_rParent.GetNDScheduler()->Enable()))
					return Error(IDS_Err_ALUEnable, _T("CPropertyLiveUpdate::Save()"));
			}
			break;

		case Prop_False:
			if (FAILED(hr = m_rParent.GetNDScheduler()->Disable()))      // Make it stop
			{
				// Disable() failed because it's not there
				if (FAILED(hr = m_rParent.GetNDScheduler()->AddTask())
				// If we failed to add, it's probably because it exists already.
				 && FAILED(hr = m_rParent.GetNDScheduler()->RestartSchedule())) // So it doesn't trigger right away
					return Error(IDS_Err_ALUSave, _T("CPropertyLiveUpdate::Save()"));

				// Try to make it stop again.
				if (FAILED(hr = m_rParent.GetNDScheduler()->Disable()))
					return Error(IDS_Err_ALUDisable, _T("CPropertyLiveUpdate::Save()"));
			}
			break;

		default:
			return S_FALSE;
		}

		m_eOldState = m_eState;  // Property is clean again
	}

	return hr;
}

HRESULT CNAVOptions::CPropertyLiveUpdate::Default(void)
{
	return CProperty::Default(DEFAULT_LiveUpdate);
}

/////////////////////////////////////////////////////////////////////////
// LiveUpdateMode
STDMETHODIMP CNAVOptions::get_LiveUpdateMode(/*[out, retval]*/ EAutoUpdateMode *peLiveUpdateMode)
{
	return m_LiveUpdateMode.Get(peLiveUpdateMode);
}

STDMETHODIMP CNAVOptions::put_LiveUpdateMode(/*[in]*/ EAutoUpdateMode eLiveUpdateMode)
{
    // Make sure the caller is Symantec signed
    if ( forceError (IDS_ERR_IT_IS_NOT_SAFE) ||
        !IsItSafe())
	{
        MakeError(IDS_ERR_IT_IS_NOT_SAFE, E_FAIL, IDS_NAVERROR_INTERNAL_REINSTALL);
	    return E_FAIL;
    }

	return m_LiveUpdateMode.Put(eLiveUpdateMode);
}

CNAVOptions::CPropertyLiveUpdateMode::CPropertyLiveUpdateMode(CNAVOptions* pParent)
                                    : IProperty(pParent, L"LiveUpdateMode")
                                    , m_dwMode(AutoUpdateMode_ERROR)
                                    , m_dwOldMode(AutoUpdateMode_ERROR)
{
}

HRESULT CNAVOptions::CPropertyLiveUpdateMode::Get(EAutoUpdateMode *peLiveUpdateMode)
{
	HRESULT hr;

	if (m_rParent.forceError (ERR_INVALID_POINTER) ||
        !peLiveUpdateMode)
	{
		m_rParent.MakeError (ERR_INVALID_POINTER, E_POINTER, IDS_NAVERROR_INTERNAL);
		return E_POINTER;
	}
	*peLiveUpdateMode = AutoUpdateMode_ERROR;

	// Re-load custom property if not loaded yet
	if (AutoUpdateMode_ERROR == m_dwMode && FAILED(hr = Load()))
        // Load() will fill out the error information
		return hr;
	else if( hr == S_FALSE )
	{
		// Unable to get the Product Reg object because we are
		// on NT and ALU is running
		m_dwMode = AutoUpdateMode_ERROR;
	}

	*peLiveUpdateMode = static_cast<EAutoUpdateMode>(m_dwMode);

	return S_OK;
}

HRESULT CNAVOptions::CPropertyLiveUpdateMode::Put(EAutoUpdateMode eLiveUpdateMode)
{
	m_dwMode = eLiveUpdateMode;

	return S_OK;
}

// This will check the LiveUpdate Product Catalog to see what
// mode the VirusDefs are set to update in: silenet or noisy
HRESULT CNAVOptions::CPropertyLiveUpdateMode::Load(void)
{
	HRESULT hr;

	// Init the ProductReg LU object
	hr = m_rParent.InitProductReg();

    if ( m_rParent.forceError (IDS_ERR_INIT_PRODREG))
        hr = E_FAIL;
        
    if (FAILED(hr))
    {
        // Unable to initialize the ProductReg object
        m_rParent.MakeError(IDS_ERR_INIT_PRODREG,hr, IDS_NAVERROR_NDSCHEDULER);
		return hr;
    }
	else if( S_FALSE == hr )
	{
		// The Product Reg COM object is not available because
		// ALU is running and we are on NT
		return S_FALSE;
	}

    // Need to get the virusdefs LU monikers from the registry
    BSTR bstrMonikers[3] = {NULL,NULL,NULL};
    VARIANT vMode;
    VariantInit(&vMode);

    if( m_rParent.forceError (ERR_GET_DEFS_MONIKER) ||
        !m_rParent.GetLUMonikerFromProductNameSubString(_T("MicroDefs"), bstrMonikers) )
    {
        // Error trying to get moniker for virus defs
		m_rParent.GetProductReg().Release();
        m_rParent.MakeError (ERR_GET_DEFS_MONIKER, E_FAIL, IDS_NAVERROR_INTERNAL_REINSTALL);
		return E_FAIL;
    }

    // Make sure the moniker exists for the substring entered
    if( m_rParent.forceError (ERR_NOT_EXIST_MONIKER) ||
        NULL == bstrMonikers[0] )
    {
        // Moniker does not exist for specified substring
		m_rParent.GetProductReg().Release();
        m_rParent.MakeError (ERR_NOT_EXIST_MONIKER, E_FAIL, IDS_NAVERROR_INTERNAL_REINSTALL);
		return E_FAIL;
    }

    // See if the VirusDefs ALU mode is silent or loud
	if (SUCCEEDED(hr = m_rParent.GetProductReg()->GetProperty(bstrMonikers[0],
                                                           _bstr_t( AU_REG_UPDATE_TYPE ),
                                                           &vMode)) )
	{
		if( !(vMode.vt & VT_BSTR) || 0 == (_bstr_t(vMode.bstrVal)).length() )
		{
			// The property does not exist, so add it
			hr = m_rParent.GetProductReg()->SetProperty(bstrMonikers[0],
														_bstr_t( AU_REG_UPDATE_TYPE ),
														_bstr_t( AU_REG_PATCH_SILENT ));

			// Now check it again
			if( SUCCEEDED(hr) )
			{
				hr = m_rParent.GetProductReg()->GetProperty(bstrMonikers[0],
															_bstr_t( AU_REG_UPDATE_TYPE ),
															&vMode);
			}
		}
    }

	if (SUCCEEDED(hr) && (vMode.vt & VT_BSTR) && 0 != (_bstr_t(vMode.bstrVal)).length() )
    {
        // Set the Mode
        if( _bstr_t(AU_REG_PATCH_NOISY) == _bstr_t(vMode.bstrVal) )
            // Noisy mode
		    m_dwOldMode = m_dwMode = 0;
        else
            // Silent mode
            m_dwOldMode = m_dwMode = 3;
    }

    // Free the Monikers
    for( int i=0; NULL != bstrMonikers[i]; i++ )
    {
        SysFreeString(bstrMonikers[i]);
    }

    // Clear the Variant
    VariantClear(&vMode);

    if ( m_rParent.forceError (IDS_ERR_GET_DEFS_LUMODE))
    {
        m_rParent.MakeError(IDS_ERR_GET_DEFS_LUMODE,E_FAIL, IDS_NAVERROR_INTERNAL);
        hr = E_FAIL;
    }

    if( FAILED(hr) )
    {
		m_rParent.GetProductReg().Release();
        m_rParent.MakeError(IDS_ERR_GET_DEFS_LUMODE,hr, IDS_NAVERROR_INTERNAL);
	    return hr;
    }
	m_rParent.GetProductReg().Release();
    return S_OK;
}

HRESULT CNAVOptions::CPropertyLiveUpdateMode::Save(void)
{
	if (AutoUpdateMode_ERROR != m_dwMode && m_dwOldMode != m_dwMode)
	{
		HRESULT hr;

		// Init the ProductReg object
        hr = m_rParent.InitProductReg();

        if ( m_rParent.forceError (IDS_ERR_INIT_PRODREG))
            hr = E_FAIL;

        if (FAILED(hr))
        {
            m_rParent.MakeError(IDS_ERR_INIT_PRODREG,hr, IDS_NAVERROR_NDSCHEDULER);
			return hr;
        }
		else if( hr == S_FALSE )
		{
			return S_FALSE;
		}

        // Need to get the virusdefs LU monikers from the registry
        BSTR bstrMonikers[3] = {NULL,NULL,NULL};

        if( m_rParent.forceError (ERR_GET_DEFS_MONIKER) ||
            !m_rParent.GetLUMonikerFromProductNameSubString(_T("MicroDefs"), bstrMonikers) )
        {
            // Error while getting monikers for virus defs
			m_rParent.GetProductReg().Release();
            m_rParent.MakeError (ERR_GET_DEFS_MONIKER, E_FAIL, IDS_NAVERROR_INTERNAL_REINSTALL);
			return E_FAIL;
        }

        // Make sure the moniker exists for the substring entered
        if( m_rParent.forceError (ERR_NOT_EXIST_MONIKER) ||
            NULL == bstrMonikers[0] )
        {
            // Moniker does not exist for specified substring
			m_rParent.GetProductReg().Release();
            m_rParent.MakeError (ERR_NOT_EXIST_MONIKER, E_FAIL, IDS_NAVERROR_INTERNAL_REINSTALL);
			return E_FAIL;
        }

        // Set the VirusDefs Update Mode for both virus def items to the correct mode
        CComBSTR bstrMode;
        if( 0 == m_dwMode )
            // Noisy mode
            bstrMode = AU_REG_PATCH_NOISY;
        else
            // Silent mode
            bstrMode = AU_REG_PATCH_SILENT;

        for( int i=0; NULL != bstrMonikers[i]; i++ )
        {
            hr = m_rParent.GetProductReg()->SetProperty(bstrMonikers[i],
                                                        _bstr_t( AU_REG_UPDATE_TYPE ),
                                                        bstrMode);

            // If setting to Noisy mode also ad the ALU_BYPASS_DELAY value
            if( 0 == m_dwMode )
                hr = m_rParent.GetProductReg()->SetProperty(bstrMonikers[i],
                                                            _bstr_t( ALU_BYPASS_DELAY ),
                                                            _bstr_t( _T("1") ) );
            // If setting to Silent remove the ALU_BYPASS_DELAY value
            else
                hr = m_rParent.GetProductReg()->DeleteProperty(bstrMonikers[i],
                                                            _bstr_t( ALU_BYPASS_DELAY ) );
            // Release the Moniker when done setting it's property
            SysFreeString(bstrMonikers[i]);
        }

		m_dwOldMode = m_dwMode;  // Property is clean again
	}
	m_rParent.GetProductReg().Release();
	return S_OK;
}

bool CNAVOptions::CPropertyLiveUpdateMode::IsDirty(void)
{
	return m_dwOldMode != m_dwMode;
}

void CNAVOptions::CPropertyLiveUpdateMode::Init(void)
{
	m_dwOldMode = m_dwMode = AutoUpdateMode_ERROR;
}

HRESULT CNAVOptions::CPropertyLiveUpdateMode::Default(void)
{
	return m_rParent.Default(DEFAULT_LiveUpdateMode, m_dwMode);
}

/////////////////////////////////////////////////////////////////////////
// OfficePlugin
STDMETHODIMP CNAVOptions::get_OfficePlugin(BOOL *pbOfficePlugin)
{
	return m_OfficePlugin.Get(pbOfficePlugin);
}

STDMETHODIMP CNAVOptions::put_OfficePlugin(BOOL bOfficePlugin)
{
    // Make sure the caller is Symantec signed
    if ( forceError (IDS_ERR_IT_IS_NOT_SAFE) ||
        !IsItSafe())
	{
        MakeError(IDS_ERR_IT_IS_NOT_SAFE, E_FAIL, IDS_NAVERROR_INTERNAL_REINSTALL);
	    return E_FAIL;
    }

	return m_OfficePlugin.Put(bOfficePlugin);
}

HRESULT CNAVOptions::CPropertyOfficePlugin::Load(void)
{
	HRESULT hr;
	TCHAR szOfficeEnumKey[MAX_PATH];
	CRegKey O2KKey;

	// Q: Is any version of MS-Office installed at all?
	if (ERROR_SUCCESS != (hr = O2KKey.Open(HKEY_LOCAL_MACHINE, MSOFFICE_REGKEY, KEY_ENUMERATE_SUB_KEYS)))
		return S_FALSE;
    
    // Some version of Office is installed
    // Enumerate through all the keys and look for one 9.0 (2000) or greater.
	// Loop as long as there are keys and we haven't detected a
	// valid O2K install.
	// We are looking for a 9.x or greater with \\Common\\InstalledRoot key.

	double dVersion = 0.;

	EState eState = Prop_Unknown;
	for (DWORD dwOfficeSize = sizeof(szOfficeEnumKey)
	         , dwIndex = 0; ERROR_SUCCESS == RegEnumKeyEx(O2KKey, dwIndex
	                                                            , szOfficeEnumKey
	                                                            , &dwOfficeSize
	                                                            , NULL, NULL, NULL, NULL); dwIndex++, dwOfficeSize = sizeof(szOfficeEnumKey))
	{
		// No errors and a 9.x or greater version
		if ((dVersion = ::_tcstod(szOfficeEnumKey, NULL)) >= 9.)
		{
			// Now check for a valid install key. Free viewers install all 
			// the above keys, but don't install the \\Common\\InstalledRoot
			// path. This way we can tell if it's a REAL Office install.

			TCHAR szOfficeInstalledKey[MAX_PATH] = DMSOFFICE_REGKEY _T("\\");

			_tcscat(szOfficeInstalledKey, szOfficeEnumKey );
			_tcscat(szOfficeInstalledKey, _T("\\Common\\InstallRoot"));

			CRegKey O2KInstalledKey;

			if (ERROR_SUCCESS ==  O2KInstalledKey.Open(HKEY_LOCAL_MACHINE, szOfficeInstalledKey, KEY_READ))
			{
				TCHAR szPath[MAX_PATH];
				DWORD dwSize = SIZEOF(szPath);
				CRegKey PluginKey;

				// Check if the version is actually installed.
				if (ERROR_SUCCESS != O2KInstalledKey.QueryValue(_T("Path"), NULL, szPath, &dwSize))
					continue;

				// Check if our MS-Office AV plugin is installed.
				if (ERROR_SUCCESS == (hr = PluginKey.Open(HKEY_CLASSES_ROOT, STR_GUIDMSOfficeAV, KEY_QUERY_VALUE)))
				{
					eState = Prop_True;
					break;
				}
				else if (hr == ERROR_FILE_NOT_FOUND)
				{
					eState = Prop_False;
					break;
				}
			}
		}
	}

	if (dVersion > 0.)
		m_eState = m_eOldState = eState;

	return S_OK;
}

HRESULT CNAVOptions::CPropertyOfficePlugin::Save(void)
{
	if (Prop_Unknown != m_eState && m_eOldState != m_eState)
	{
		HRESULT hr = S_FALSE;

		// Can't make registry changes
		if (!m_rParent.CanAccessRegKey())
			return Error(IDS_Err_RegistryPermissions, _T("CPropertyOfficePlugin::Save()"));
#ifdef __Self_Register_Office_Plugin__
		if (!m_hInst)
		{
			TCHAR szPluginPath[MAX_PATH];

			::_tcscpy(szPluginPath, g_NAVInfo.GetNAVDir());
			::_tcscat(szPluginPath, _T("\\OFFICEAV.DLL"));

            // Verify the signature on the officeAV plug in dll
            if( NAVToolbox::NAVTRUST_OK != NAVToolbox::IsSymantecSignedImage(szPluginPath) )
            {
                return Error(IDS_Err_OfficeRegister, _T("CPropertyOfficePlugin::Save()"));
            }

			if (NULL == (m_hInst = LoadLibraryEx(szPluginPath, NULL, LOAD_WITH_ALTERED_SEARCH_PATH)))
				return Error(IDS_Err_OfficeRegister, _T("CPropertyOfficePlugin::Save()"));
		}

		typedef DWORD (WINAPI* OFFICE2KREGISTER)(void);
		// Register / Unregister the Office AV DLL
		PTCHAR pszEntryPoint = Prop_True == m_eState ? _T("DllRegisterServer") : _T("DllUnregisterServer");
		OFFICE2KREGISTER pfuncRegisterServer = reinterpret_cast<OFFICE2KREGISTER>(::GetProcAddress(m_hInst, pszEntryPoint));

		if (NULL == pfuncRegisterServer)
			hr = Error(IDS_Err_OfficeRegister, _T("CPropertyOfficePlugin::Save()"));
		else
			hr = pfuncRegisterServer();
#else __Self_Register_Office_Plugin__
		USES_CONVERSION;
		TCHAR szPluginPath[MAX_PATH];
		CComPtr<ITypeLib> spTypeLib;
		CComPtr<ICatRegister> spCatReg;
		CATID cats = CATID_MSOfficeAntiVirus;

		if (FAILED( hr = spCatReg.CoCreateInstance(CLSID_StdComponentCategoriesMgr, 0, CLSCTX_INPROC_SERVER)))
			return hr;

		::_tcscpy(szPluginPath, g_NAVInfo.GetNAVDir());
		::_tcscat(szPluginPath, _T("\\OFFICEAV.DLL"));

		if (Prop_True == m_eState)
		{
			CATEGORYINFO rgcc = { 0 };
			CRegKey key, key1, clsid, curver, clsid1, progid, viprogid, ips32, typelib;

			rgcc.catid = CATID_MSOfficeAntiVirus;
			rgcc.lcid = 0x409;   // english description

			// register the object in the office anti virus category
			if (FAILED(hr = spCatReg->RegisterCategories(1, &rgcc))
			 || FAILED(hr = spCatReg->RegisterClassImplCategories(GUID_MSOfficeAntiVirus, 1, &cats)))
				return hr;

			// Force remove the current entry
			if (ERROR_SUCCESS != ::SHDeleteKey(HKEY_CLASSES_ROOT, _T("CLSID\\{DE1F7EEF-1851-11D3-939E-0004AC1ABE1F}"))
			// Current version
			 || ERROR_SUCCESS != key1.Create(HKEY_CLASSES_ROOT, _T("NortonAntiVirus.OfficeAntiVirus.1"))
			 || ERROR_SUCCESS != key1.SetValue(_T("Symantec Norton AntiVirus OfficeAntiVirus Class"))
			// Current version CLSID
			 || ERROR_SUCCESS != clsid1.Create(HKEY_CLASSES_ROOT, _T("NortonAntiVirus.OfficeAntiVirus.1\\CLSID"))
			 || ERROR_SUCCESS != clsid1.SetValue(_T("{DE1F7EEF-1851-11D3-939E-0004AC1ABE1F}"))
			// Version independent
			 || ERROR_SUCCESS != key.Create(HKEY_CLASSES_ROOT, _T("NortonAntiVirus.OfficeAntiVirus"))
			 || ERROR_SUCCESS != key.SetValue(_T("Symantec Norton AntiVirus OfficeAntiVirus Class"))
			// Version independent CLSID
			 || ERROR_SUCCESS != clsid.Create(HKEY_CLASSES_ROOT, _T("NortonAntiVirus.OfficeAntiVirus\\CLSID"))
			 || ERROR_SUCCESS != clsid.SetValue(_T("{DE1F7EEF-1851-11D3-939E-0004AC1ABE1F}"))
			// Current version
			 || ERROR_SUCCESS != curver.Create(HKEY_CLASSES_ROOT, _T("NortonAntiVirus.OfficeAntiVirus\\CurVer"))
			 || ERROR_SUCCESS != curver.SetValue(_T("NortonAntiVirus.OfficeAntiVirus.1"))
			// ProgID
			 || ERROR_SUCCESS != progid.Create(HKEY_CLASSES_ROOT, _T("CLSID\\{DE1F7EEF-1851-11D3-939E-0004AC1ABE1F}\\ProgID"))
			 || ERROR_SUCCESS != progid.SetValue(_T("NortonAntiVirus.OfficeAntiVirus.1"))
			// Version independent ProgID
			 || ERROR_SUCCESS != viprogid.Create(HKEY_CLASSES_ROOT, _T("CLSID\\{DE1F7EEF-1851-11D3-939E-0004AC1ABE1F}\\VersionIndependentProgID"))
			 || ERROR_SUCCESS != viprogid.SetValue(_T("NortonAntiVirus.OfficeAntiVirus"))
			// Inproc server
			 || ERROR_SUCCESS != ips32.Create(HKEY_CLASSES_ROOT, _T("CLSID\\{DE1F7EEF-1851-11D3-939E-0004AC1ABE1F}\\InprocServer32"))
			 || ERROR_SUCCESS != ips32.SetValue(szPluginPath)
			 || ERROR_SUCCESS != ips32.SetValue(_T("Apartment"), _T("ThreadingModel"))
			// TypeLib
			 || ERROR_SUCCESS != typelib.Create(HKEY_CLASSES_ROOT, _T("CLSID\\{DE1F7EEF-1851-11D3-939E-0004AC1ABE1F}\\TypeLib"))
			 || ERROR_SUCCESS != typelib.SetValue(_T("{DE1F7EE0-1851-11D3-939E-0004AC1ABE1F}")))
				return Error(IDS_Err_OfficeRegister, _T("CPropertyOfficePlugin::Save()"));

			// Register the type library
			if (FAILED(hr = ::LoadTypeLib(T2OLE(szPluginPath), &spTypeLib))
			 || FAILED(hr = ::RegisterTypeLib(spTypeLib, T2OLE(szPluginPath), NULL)))
				return hr;

		}
		else
		{
			TLIBATTR* ptla = NULL;

			if (FAILED(hr = spCatReg->UnRegisterClassImplCategories(GUID_MSOfficeAntiVirus, 1, &cats)))
				return hr;

			// Remove all related keys
			if (ERROR_SUCCESS != ::SHDeleteKey(HKEY_CLASSES_ROOT, _T("NortonAntiVirus.OfficeAntiVirus.1"))
			 || ERROR_SUCCESS != ::SHDeleteKey(HKEY_CLASSES_ROOT, _T("NortonAntiVirus.OfficeAntiVirus"))
			 || ERROR_SUCCESS != ::SHDeleteKey(HKEY_CLASSES_ROOT, _T("CLSID\\{DE1F7EEF-1851-11D3-939E-0004AC1ABE1F}")))
				return Error(IDS_Err_OfficeRegister, _T("CPropertyOfficePlugin::Save()"));

			// Unregister the type library
			if (FAILED(hr = ::LoadTypeLib(T2OLE(szPluginPath), &spTypeLib))
			 || FAILED(hr = spTypeLib->GetLibAttr(&ptla))
			 || FAILED(hr = ::UnRegisterTypeLib(ptla->guid, ptla->wMajorVerNum
			                                              , ptla->wMinorVerNum
			                                              , ptla->lcid
			                                              , ptla->syskind)))
				return hr;

			 spTypeLib->ReleaseTLibAttr(ptla);
		}
#endif __Self_Register_Office_Plugin__
		m_eOldState = m_eState;  // Property is clean again

		return hr;
	}

	return S_FALSE;
}

HRESULT CNAVOptions::CPropertyOfficePlugin::Default(void)
{
	return CProperty::Default(DEFAULT_OfficePlugin);
}

/////////////////////////////////////////////////////////////////////////
// StartupScan
STDMETHODIMP CNAVOptions::get_StartupScan(/*[out, retval]*/ BOOL *pbStartupScan)
{
	return m_StartupScan.Get(pbStartupScan);
}

STDMETHODIMP CNAVOptions::put_StartupScan(/*[in]*/BOOL bStartupScan)
{
    // Make sure the caller is Symantec signed
    if (forceError (IDS_ERR_IT_IS_NOT_SAFE) ||
        !IsItSafe())
	{
        MakeError(IDS_ERR_IT_IS_NOT_SAFE, E_FAIL, IDS_NAVERROR_INTERNAL_REINSTALL);
	    return E_FAIL;
    }

	return m_StartupScan.Put(bStartupScan);
}

HRESULT CNAVOptions::CPropertyStartupScan::Load(void)
{
	HRESULT hr;
	PTCHAR pszAutoExecPath;

	// Path to autoexec.bat
	if (FAILED(hr = GetAutoExecBatPath(&pszAutoExecPath)))
		return hr;

	//
	// Find if a navdx.exe is executed from the autoexec.bat
	//
	 StahlSoft::CSmartHandle hAutoExec;
     hAutoExec = ::CreateFile(pszAutoExecPath, GENERIC_READ            // access mode
	                                               , 0                       // share mode
											       , NULL                    // Security Discriptor
											       , OPEN_EXISTING           // how to create
											       , FILE_ATTRIBUTE_NORMAL   // file attributes
											       , NULL );                 // handle to template file

	if (INVALID_HANDLE_VALUE == hAutoExec)
	{
		if (::GetLastError() == ERROR_FILE_NOT_FOUND)
			return S_FALSE;

		return Error(IDS_Err_AutoexcOpen, _T("CPropertyStartupScan::Load()"));
	}

	m_eState    =
	m_eOldState = Prop_False;  // Assume false
	DWORD dwSize = ::GetFileSize(hAutoExec, NULL);

	if (dwSize)
	{
		DWORD dwRead;
		char* lpBuffer = new char[dwSize + 1];
		CCleaner cl(lpBuffer);

		if (lpBuffer && ::ReadFile(hAutoExec, lpBuffer, dwSize, &dwRead, NULL))
		{
			PTCHAR _lpBuffer = lpBuffer;

			lpBuffer[dwSize] = '\0';
			//
			// TODO: This routine is not really a TCHAR routine
			// We must make a char to TCHAR conversion here
			//

			// find a line with "navdx.exe"
			for (bool bFound; !(bFound = FindCommandLine(lpBuffer, _T("navdx.exe"), dwSize)) && *(lpBuffer += dwSize); )
				;

			if (bFound)
			{
				m_eState    =
				m_eOldState = Prop_True;
			}
		}
	}

	return dwSize ? S_OK : S_FALSE;
}

HRESULT CNAVOptions::CPropertyStartupScan::Save(void)
{
	HRESULT hr;
	DWORD dwWritten;
	PTCHAR pszAutoExecPath;
	TCHAR szTempDir[MAX_PATH]
	    , szTempFilePath[MAX_PATH]
	    , szAutoExecBackup[MAX_PATH];

	// No change? Wrong OS? We're outa' here
	if (Prop_Unknown == m_eState || m_eOldState == m_eState || g_OSInfo.IsWinNT() || g_OSInfo.IsWinME())
		return S_FALSE;

	// Path to autoexec.bat
	if (FAILED(hr = GetAutoExecBatPath(&pszAutoExecPath)))
		return hr;

	// Path to autoexec.bat backup file
	::_tcscpy(szAutoExecBackup, pszAutoExecPath);
	// change the file extension
	::_tcscpy(::_tcsrchr(szAutoExecBackup, _T('.')), _T(".nav"));
	// Create a backup copy
	::CopyFile(pszAutoExecPath, szAutoExecBackup, FALSE);

	// Get a temp file name from the OS - we'll write the new autoexec to
	// this file first, then at the end, copy it over the existing one.
	::GetTempPath(MAX_PATH, szTempDir);
	::GetTempFileName(szTempDir, _T("dds"), 0, szTempFilePath);

	// Read and buffer the current AUTOEXEC.BAT
    StahlSoft::CSmartHandle hAutoExec;
	hAutoExec = ::CreateFile(pszAutoExecPath, GENERIC_READ            // access mode
	                                               , 0                       // share mode
	                                               , NULL                    // Security Discriptor
	                                               , OPEN_EXISTING           // how to create
	                                               , FILE_ATTRIBUTE_NORMAL   // file attributes
	                                               , NULL );                 // handle to template file
	DWORD dwError = ::GetLastError();

	// Unable to open AUTOEXEC.BAT file
	if (INVALID_HANDLE_VALUE == hAutoExec && dwError != ERROR_FILE_NOT_FOUND)
		return Error(IDS_Err_AutoexcOpen, _T("CPropertyStartupScan::Save()"));

	DWORD dwSize = (dwError == ERROR_FILE_NOT_FOUND) ? 0 : ::GetFileSize(hAutoExec, NULL);
	char* lpBuffer = NULL;
	CCleaner cl;

	// AUTOEXEC.BAT exists
	if (dwSize)
	{
		DWORD dwRead;

		if (!(lpBuffer = new char[dwSize + 1]))
			return E_OUTOFMEMORY;
		cl = lpBuffer;

		// Read the entier file into a buffer for processing
		if (!::ReadFile(hAutoExec, lpBuffer, dwSize, &dwRead, NULL))
			return Error(IDS_Err_AutoexcRead, _T("CPropertyStartupScan::Save()"));
		lpBuffer[dwSize] = '\0';

		//
		// TODO: This routine is not really a TCHAR routine
		// We must make a char to TCHAR conversion here
		//
	}

    hAutoExec.CloseHandle();

	// Create our temp file.
	 StahlSoft::CSmartHandle hAutoExecTemp;
     hAutoExecTemp = ::CreateFile(szTempFilePath, GENERIC_WRITE           // access mode
	                                                  , 0                       // share mode
	                                                  , NULL                    // Security Discriptor
	                                                  , OPEN_EXISTING           // how to create
	                                                  , FILE_ATTRIBUTE_NORMAL   // file attributes
	                                                  , NULL );                 // handle to template file
	// Unable to create a temporary file
	if (INVALID_HANDLE_VALUE == hAutoExecTemp)
		return Error(IDS_Err_TemporaryOpen, _T("CPropertyStartupScan::Save()"));

	// Enable stratup scan
	if (Prop_True == m_eState)
	{
		TCHAR szNavdxPath[MAX_PATH + 16];

		// Build the NAVDX command line.  The command needs to be in OEM since
		// DOS operates only in OEM, and the NAV install dir could contain 
		// hi-ASCII chars.
		::_tcscpy(szNavdxPath, g_NAVInfo.GetNAVDir());
		::_tcscat(szNavdxPath, _T("\\navdx.exe"));

		DWORD dsLen = ::GetShortPathName(szNavdxPath, szNavdxPath, MAX_PATH);

		::_tcscat(szNavdxPath, _T(" /startup\r\n"));

#ifdef _UNICODE
#  error Need to fix the CharToOem call - can't use the same buffer for input and output in Unicode.
#endif

		::CharToOem(szNavdxPath, szNavdxPath);

		// Now go through the existing AUTOEXEC, copying the lines to our
		// temp file and inserting NAVDX where appropriate.
		// If we can't open AUTOEXEC here, that's ok since AUTOEXEC doesn't
		// have to exist on 9x.
		if (!lpBuffer)
		{
			// No AUTOEXEC, so just write the NAVDX command to the temp file.
			if (!::WriteFile(hAutoExecTemp, szNavdxPath, ::_tcslen(szNavdxPath), &dwWritten, NULL))
				return Error(IDS_Err_TempFileWrite, _T("CPropertyStartupScan::Save()"));
		}
		else
		{
			PTCHAR _lpBuffer = lpBuffer;

			// If there's already a line for navdx.exe we're done.
			for (bool bFound; !(bFound = FindCommandLine(lpBuffer, _T("navdx.exe"), dwSize)) && *(lpBuffer += dwSize); )
				;

			if (bFound)
			{
				m_eOldState = m_eState;  // No longer dirty
                hAutoExecTemp.CloseHandle();
				::DeleteFile(szTempFilePath);

				return S_FALSE;
			}

			// First determine whether this is an east-European language system.
			// On those languages, AUTOEXEC has MODE CON CODEPAGE and KEYB 
			// commands, and NAVDX must go after those commands for its output
			// to look right.  We search for a KEYB command, and if there is one,
			// set a flag for use later.

			// Process the file one line at a time looking for the KEYB command
			// If this command exists thi is an east europien keyboard
			for (bool bIsEastEuro; !(bIsEastEuro = FindCommandLine(lpBuffer, _T("keyb"), dwSize)) && *(lpBuffer += dwSize); )
				;

			lpBuffer = _lpBuffer; // Start at the beginning of the buffer

			// For non-Euro languages, NAVDX goes at the beginning of the
			// new AUTOEXEC.
			if (!bIsEastEuro)
			{
				if (!::WriteFile(hAutoExecTemp, szNavdxPath, ::_tcslen(szNavdxPath), &dwWritten, NULL))
					return Error(IDS_Err_TempFileWrite, _T("CPropertyStartupScan::Save()"));
			}
			else
			{
				// Otherwise, we need to loop through the existing AUTOEXEC
				// until we hit a KEYB command, and then stick NAVDX in right after it.
				for (bool bAtInsertPoint; *lpBuffer && ((bAtInsertPoint = FindCommandLine(lpBuffer, _T("keyb"), dwSize))
				                                     || lpBuffer[dwSize]); lpBuffer += dwSize)
				{
					// Copy the current line to the temp file.
					if (!::WriteFile(hAutoExecTemp, lpBuffer, dwSize, &dwWritten, NULL))
						return Error(IDS_Err_TempFileWrite, _T("CPropertyStartupScan::Save()"));

					// And insert the NAVDX line if we're at the insert point.
					if (bAtInsertPoint)
					{
						if (!::WriteFile(hAutoExecTemp, szNavdxPath, ::_tcslen(szNavdxPath), &dwWritten, NULL))
							return Error(IDS_Err_TempFileWrite, _T("CPropertyStartupScan::Save()"));
						lpBuffer += dwSize;
						break;
					}
				}   // end for()
			}   // end else (bIsEastEuro)

			// Now that we have written the NAVDX line, loop thru the rest of
			// the existing AUTOEXEC and copy it to the temp file.
			if (!::WriteFile(hAutoExecTemp, lpBuffer, ::_tcslen(lpBuffer), &dwWritten, NULL))
				return Error(IDS_Err_TempFileWrite, _T("CPropertyStartupScan::Save()"));
		}   // end else (!lpBuffer)
	}   // if (Prop_True == m_eState)
	// Disable stratup scan
	else if (lpBuffer)
	{
		// Loop thru the existing AUTOEXEC, copying all lines except the
		// NAVDX command to the temp file.
		for (bool bFound; !(bFound = FindCommandLine(lpBuffer, _T("navdx"), dwSize)) && *(lpBuffer + dwSize); lpBuffer += dwSize)
		{
			// If we're here, sCommand is not a NAVDX line, so write it
			// to the temp file.
			if (!::WriteFile(hAutoExecTemp, lpBuffer, dwSize, &dwWritten, NULL))
				return Error(IDS_Err_TempFileWrite, _T("CPropertyStartupScan::Save()"));
		}   // end for()

		if (bFound)
			lpBuffer += dwSize;
		// Now that we have written the NAVDX line, write the rest of the existing
		// AUTOEXEC and copy it to the temp file.
		if (!::WriteFile(hAutoExecTemp, lpBuffer, ::_tcslen(lpBuffer), &dwWritten, NULL))
			return Error(IDS_Err_TempFileWrite, _T("CPropertyStartupScan::Save()"));
	}

    hAutoExecTemp.CloseHandle();

	// Make the AUTOEXEC.BAT file writable so we can overwrite it.
	DWORD dwAttributes = ::GetFileAttributes(pszAutoExecPath);

	if (0xFFFFFFFF == dwAttributes && ERROR_FILE_NOT_FOUND != ::GetLastError()
	 || (FILE_ATTRIBUTE_READONLY == (FILE_ATTRIBUTE_READONLY & dwAttributes)
	  && !::SetFileAttributes(pszAutoExecPath, dwAttributes & ~FILE_ATTRIBUTE_READONLY)))
			return Error(IDS_Err_CopyTemporaryFile, _T("CPropertyStartupScan::Save()"));

	// Now the temp file contains the new AUTOEXEC contents, so copy
	// it over the old one.
	BOOL bRet = ::CopyFile(szTempFilePath, pszAutoExecPath, FALSE);

	// If AUTOEXEC.BAT had readonly attributes before restore those same attributes
	if (FILE_ATTRIBUTE_READONLY == (FILE_ATTRIBUTE_READONLY & dwAttributes)
	 && !::SetFileAttributes(pszAutoExecPath, dwAttributes))
			return Error(IDS_Err_CopyTemporaryFile, _T("CPropertyStartupScan::Save()"));

	if (bRet)
		m_eOldState = m_eState;  // No longer dirty
	::DeleteFile(szTempFilePath);

	return bRet ? S_OK : Error(IDS_Err_CopyTemporaryFile, _T("CPropertyStartupScan::Save()"));
}

HRESULT CNAVOptions::CPropertyStartupScan::Default(void)
{
	return CProperty::Default(DEFAULT_StartupScan);
}

bool CNAVOptions::CPropertyStartupScan::FindCommandLine(PTCHAR pszLine, PTCHAR pszCommand, DWORD& dwLineLength)
{
	PTCHAR pszNewLine = ::_tcschr(pszLine, _T('\n'));

	// Find the size of the line
	if (pszNewLine)
		dwLineLength = pszNewLine - pszLine + 1;
	else
		dwLineLength = ::_tcslen(pszLine);

	// Skip white space at the beginning of the line
	for ( ; *pszLine && ::_istspace(*pszLine); pszLine++)
		;

	// Empty line
	if (!*pszLine ||  _T('\n') == *pszLine)
		return false;

	// Advance past the '@' charachter used for supressing echo
	if (_T('@') == *pszLine)
	{
		pszLine++;

		// Empty line
		if (!*pszLine ||  _T('\n') == *pszLine)
			return false;
	}

	// This line is commented out
	if (!::_tcsnicmp("rem", pszLine, 3) && ::isspace(pszLine[3]))
		return false;

	bool bStartOfLine = true;

	// check if the AUTOEXEC.BAT line contains a DOS command
	for (int l = ::_tcslen(pszCommand); *pszLine && _T('\n') != *pszLine; pszLine++, bStartOfLine = false)
	{
		// Q: is this the beginning of the command
		if (tolower(*pszLine) != tolower(*pszCommand))
			continue;
		// Q: is this a "command" boundery
		//    This is a "command" boudry if we are at the beginning of the line
		//    or the charachter just preceeding the current position is not
		//    a path "delimiter"
		if (!bStartOfLine
		 && _T('\\') != pszLine[-1]
		 && _T(':') != pszLine[-1])
			continue;
		// This is the command if it's an exact match followed by
		// a white space or null char or a dot ('.')
		if (!::_tcsnicmp(pszLine, pszCommand, l)                // Match the command
		 && (!pszLine[l]                                        // Test for nil
		  || ::_istspace(pszLine[l])                            // Test for ws
		  || _T('.') != pszLine[l-3] && _T('.') == pszLine[l])) // Test for dot
			return true;
	}

	return false;
}

HRESULT CNAVOptions::CPropertyStartupScan::GetAutoExecBatPath(PTCHAR *pszAutoExecBat)
{
	CRegKey       RegKey;
	static TCHAR  szDir[MAX_PATH] = _T("C:\\");  // Reasonable default
    unsigned long nDirNameLength = sizeof(szDir);

	*pszAutoExecBat = NULL;

	if (szDir[3])
	{
		*pszAutoExecBat = szDir;
		return S_OK;
	}

	// Can't make registry changes
	if (!m_rParent.CanAccessRegKey())
		return Error(IDS_Err_RegistryPermissions, _T("CPropertyStartupScan::GetAutoExecBatPath()"));

	// In NT 4 and Win95, the registry will work.
	// In NT 3.5x, the SystemRoot environment variable
	// will be used.
	// If all else fails, we'll try thunks (but I doubt
	// that code path ever gets taken).

	// For NT 4....
	if (ERROR_SUCCESS == RegKey.Open( HKEY_LOCAL_MACHINE
	                                , _T("SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Setup")
	                                , KEY_READ )
	 && ERROR_SUCCESS == RegKey.QueryValue(_T("BootDir"), NULL, szDir, &nDirNameLength))
		szDir[3] = _T('\0');
	else if (::GetVersion() < 0x8000000) // we're on NT
	{                                // For NT 3.5x, use the SystemRoot environment varialbe
		::GetEnvironmentVariable(_T("SystemRoot"), szDir, sizeof(szDir) );
		szDir[3] = _T('\0');
	}

	::_tcscat(szDir, _T("autoexec.bat"));
	*pszAutoExecBat = szDir;

	return S_OK;
}
