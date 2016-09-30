// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright (c) 2006, Symantec Corporation, All rights reserved.
// HPPProtection.h : Declaration of the CHPPProtection

#pragma once
#include "resource.h"       // main symbols

#include "HPPProtectionProviderUI.h"
#include "ProtectionProvider.h"
#include "TrustUtil.h"
#include "HppOptions.h"

/**
* @pkg HPPProtection
* @author Terry Marles
* @version 1.0              
*
* This class implements the IProtection interface.  It provides the functionality
* for the realtime scanning configuration
*
* IProtection_Container:  This interface contains methods for the framework
* to enumurate the protection components provided by the snap-in.
*
* IProtection:  This is the interface to access a specifiec protection component.
* For example Auto-protect.  The 
*
**/

// Defines
#define NO_HPP_AP	1  // # of HPP autoprotect classes.

// CHPPProtection

class ATL_NO_VTABLE CHPPProtection : 
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<CHPPProtection, &CLSID_HPPProtection>,
	public IDispatchImpl<IHPPProtection, &IID_IHPPProtection, &LIBID_HPPProtectionProviderUILib, /*wMajor =*/ 1, /*wMinor =*/ 0>,
	public IDispatchImpl<IProtection, &__uuidof(IProtection), &LIBID_ProtectionProviderLib, /* wMajor = */ 1, /* wMinor = */ 0>
{
public:
	CHPPProtection();

	DECLARE_REGISTRY_RESOURCEID(IDR_HPPPROTECTION)


	BEGIN_COM_MAP(CHPPProtection)
		COM_INTERFACE_ENTRY(IHPPProtection)
		COM_INTERFACE_ENTRY2(IDispatch, IProtection)
		COM_INTERFACE_ENTRY(IProtection)
	END_COM_MAP()


	DECLARE_PROTECT_FINAL_CONSTRUCT()

	HRESULT FinalConstruct();
	void FinalRelease();

	//*********************************************************************
	//
	// *** IProtection Methods ***
	//
	//*********************************************************************
public:
	/**
	* get_ID Is called by the UI framwork to get the GUID for the interface
	*
    * @param GUID *  id[in/out]  The method will fill in the GUID
	*
    * @return HRESULT S_OK if successful.  Standard HRESULT error codes if failure
	* 
	*/
	STDMETHOD(get_ID)( GUID *  id);


	/**
	* get_DisplayName Is called by the UI framwork to get the display name for
	* the AP techonology
	*
    * @param BSTR *  shortName[in/out]  pointer to the BSTR is fill in by the 
	* method and contains the Displayable name of the AP technology.
	*
    * @return HRESULT S_OK if successful.  Standard HRESULT error codes if failure
	* 
	*/
	STDMETHOD(get_DisplayName)( BSTR *  shortName);


	/**
	* get_Description Is called by the UI framwork to get a description that gives the
	* user detail on the AP technology
	*
    * @param BSTR *  userDescription[in/out]  pointer to the BSTR is fill in by the 
	* method and contains a detialed description of the AP technology.
	*
    * @return HRESULT S_OK if successful.  Standard HRESULT error codes if failure
	* 
	*/
	STDMETHOD(get_Description)( BSTR *  userDescription);


	/**
	* get_ProtectionStatus Is called by the UI framwork to get the current status of
	* the AP.  Note:  this may be different than what the AP is configured for due to 
	* 
	*
    * @param ProtectionStatus *  currStatus[in/out]  method sets the protection state
	* into this variable
	*
    * @return HRESULT S_OK if successful.  Standard HRESULT error codes if failure
	* 
	*/
	STDMETHOD(get_ProtectionStatus)( ProtectionStatus *  currStatus);


	/**
	* get_ProtectionStatusDescription Is called by the UI framwork to get the current status 
	* text description of the AP.  Note:  this may be different than what the AP is 
	* configured for due to 
	* 
    * @param BSTR *  statusDescription[in/out]  method fill in the description status
	*
    * @return HRESULT S_OK if successful.  Standard HRESULT error codes if failure
	* 
	*/
	STDMETHOD(get_ProtectionStatusDescription)( BSTR *  statusDescription);


	/**
	* get_ProtectionStatusReadOnly Is called by the UI framwork to get the current status 
	* and is not changeable
	* 
    * @param VARIANT_BOOL *  isReadOnly[in/out]  method fill in the status
	*
    * @return HRESULT S_OK if successful.  Standard HRESULT error codes if failure
	* 
	*/
	STDMETHOD(get_ProtectionStatusReadOnly)( VARIANT_BOOL *  isReadOnly);

	/**
	* get_ProtectionConfiguration  Is called by the UI framwork to get the current 
	* configuration status.  For example the configured status could be enable/disabled
	* 
    * @param VARIANT_BOOL *  protectionEnabled[in/out]  method fill in the status
	*
    * @return HRESULT S_OK if successful.  Standard HRESULT error codes if failure
	* 
	*/
	STDMETHOD(get_ProtectionConfiguration)( VARIANT_BOOL *  protectionEnabled);


	/**
	* put_ProtectionConfiguration  Is called by the UI framwork to set the current 
	* configuration status.  
	* 
    * @param VARIANT_BOOL  newStatus[in]  method fill in the status
	*
    * @return HRESULT S_OK if successful.  Standard HRESULT error codes if failure
	* 
	*/
	STDMETHOD(put_ProtectionConfiguration)( VARIANT_BOOL  newStatus);


	/**
	* ShowConfigureUI  Is called by the UI framwork.  This allows the protection
	* technology to display the protection configuration UI
	* 
    * @param HWND  parentWindowHandle[in]  This is the parent window handle.  It is
	* used by the method to attach dialogs to the parent proccess window.
	*
    * @return HRESULT S_OK if successful.  Standard HRESULT error codes if failure
	* 
	*/
	STDMETHOD(ShowConfigureUI)( HWND  parentWindowHandle);



	/**
	* ShowLogUI  Is called by the UI framwork.  This allows the protection
	* technology to display the protection log records
	* 
    * @param HWND  parentWindowHandle[in]  This is the parent window handle.  It is
	* used by the method to attach dialogs to the parent proccess window.
	*
    * @return HRESULT S_OK if successful.  Standard HRESULT error codes if failure
	* 
	*/
	STDMETHOD(ShowLogUI)( HWND  parentWindowHandle);

private:
	/** m_pterGenConfig is a IGenericConfig pointer used by the configuration dialogs
	* to read and write settings to the configuration store.
	*/
	IGenericConfig	*m_ptrGenConfig;
    CTrustVerifier   trustVerifier;
	CHppOptions		m_hppOptions;
};

OBJECT_ENTRY_AUTO(__uuidof(HPPProtection), CHPPProtection)
