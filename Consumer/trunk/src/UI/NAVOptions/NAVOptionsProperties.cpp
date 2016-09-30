////////////////////////
//
// PROPRIETARY / CONFIDENTIAL.
// Use of this product is subject to license terms.
// Copyright © 2006 Symantec Corporation.
// All rights reserved.
//
/////////////////////////////////////////////////////////////////////////////////////////////////

// NAVOptionsProperties.cpp : Implementation of CNAVOptions

#include "StdAfx.h"
#include "DefAlert.h"  // LiveUpdate
#include "NAVOptionsObj.h"
#include "navtrust.h"

#include "NetDetectController_i.c"  // LiveUpdate
#include "ProductRegCOM_i.c"
#include "..\navoptionsres\resource.h"
#include "isErrorResource.h"
#include "isErrorIDs.h"

// For Office 2000 plug in
#define DMSOFFICE_REGKEY _T("Software\\Microsoft\\Office")
const PTCHAR MSOFFICE_REGKEY = DMSOFFICE_REGKEY;
const PTCHAR STR_GUIDMSOfficeAV = _T("CLSID\\{DE1F7EEF-1851-11D3-939E-0004AC1ABE1F}\\ProgID");
DEFINE_GUID(GUID_MSOfficeAntiVirus, 0xde1f7eef, 0x1851, 0x11d3, 0x93, 0x9e, 0x0, 0x04, 0xac, 0x1a, 0xbe, 0x1f);
// CATID for Microsoft Office AntiVirus COM category
// {56FFCC31-D398-11d0-B2AE-00A0C908FA49}
DEFINE_GUID(CATID_MSOfficeAntiVirus, 0x56ffcc30, 0xd398, 0x11d0, 0xb2, 0xae, 0x0, 0xa0, 0xc9, 0x8, 0xfa, 0x49);

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
		m_rParent.MakeError (ERR_INVALID_POINTER, E_POINTER, IDS_ISSHARED_ERROR_INTERNAL);
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

HRESULT CNAVOptions::CProperty::Default(const wchar_t* pcszDefault)
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
// OfficePlugin
STDMETHODIMP CNAVOptions::get_OfficePlugin(BOOL *pbOfficePlugin)
{
	return m_OfficePlugin.Get(pbOfficePlugin);
}

STDMETHODIMP CNAVOptions::put_OfficePlugin(BOOL bOfficePlugin)
{
    // Make sure the caller is Symantec signed
    if ( forceError (IDS_ERR_IT_IS_NOT_SAFE))
	{
        MakeError(IDS_ERR_IT_IS_NOT_SAFE, E_FAIL, IDS_ISSHARED_ERROR_INTERNAL_REINSTALL);
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
		char* pszEntryPoint = Prop_True == m_eState ? "DllRegisterServer" : "DllUnregisterServer";
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
