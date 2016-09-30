// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright (c) 2006, Symantec Corporation, All rights reserved.
// HPPProtectionProvider.h : Declaration of the CHPPProtectionProvider


/**
* @pkg HPPProtectionProvider
* @author Terry Marles
* @version 1.0              
*
* This class implements several interfaces required in order to plug
* Into the SAV Client UI.  The interfaces are:
*
* IProtection_Provider: This is the main interface that the snap-in needs
* to implement.  This is the UI level for the technology being presented.
* It allows the frame work to query the snap in for functionality and has the 
* snap-in able to display UI Panels.
*
* IProtection_Container:  This interface contains methods for the framework
* to enumurate the protection components provided by the snap-in.
*
* IProtection:  This is the interface to access a specifiec protection component.
* For example Auto-protect.  The strange thing here is that the top level module has
* to implement this interface in order for the snap-in to init correctly.  Even if
* the implementation of the protection technology is implemented in another class.
* that decision probably needs to be re-vistited.
*
**/

#pragma once
#include "resource.h"       // main symbols

#define PROTECTIONPROVIDER_HELPERTYPES_NONAMESPACE
#include "HPPProtectionProviderUI.h"
#include "HPPProtection.h"
#include "ProtectionProvider.h"
#include "TrustUtil.h"
#include "HppOptions.h"

#if defined(_WIN32_WCE) && !defined(_CE_DCOM) && !defined(_CE_ALLOW_SINGLE_THREADED_OBJECTS_IN_MTA)
#error "Single-threaded COM objects are not properly supported on Windows CE platform, such as the Windows Mobile platforms that do not include full DCOM support. Define _CE_ALLOW_SINGLE_THREADED_OBJECTS_IN_MTA to force ATL to support creating single-thread COM object's and allow use of it's single-threaded COM object implementations. The threading model in your rgs file was set to 'Free' as that is the only threading model supported in non DCOM Windows CE platforms."
#endif


// CHPPProtectionProvider

class ATL_NO_VTABLE CHPPProtectionProvider : 
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<CHPPProtectionProvider, &CLSID_HPPProtectionProvider>,
	public IDispatchImpl<IHPPProtectionProvider, &IID_IHPPProtectionProvider, &LIBID_HPPProtectionProviderUILib, /*wMajor =*/ 1, /*wMinor =*/ 0>,
	public IDispatchImpl<IProtection_Container, &__uuidof(IProtection_Container), &LIBID_ProtectionProviderLib, /* wMajor = */ 1, /* wMinor = */ 0>,
	public IDispatchImpl<IProtection_Provider, &__uuidof(IProtection_Provider), &LIBID_ProtectionProviderLib, /* wMajor = */ 1, /* wMinor = */ 0>,
    public IDispatchImpl<IProtection_DefinitionInfo, &__uuidof(IProtection_DefinitionInfo), &LIBID_ProtectionProviderLib, /* wMajor = */ 1, /* wMinor = */ 0>,
	public IDispatchImpl<IProtection, &__uuidof(IProtection), &LIBID_ProtectionProviderLib, /* wMajor = */ 1, /* wMinor = */ 0>
{
public:
	CHPPProtectionProvider();

	DECLARE_REGISTRY_RESOURCEID(IDR_HPPPROTECTIONPROVIDER)
	// BEGIN_CATGORY_MAP is how the UI framework detects with snap-ins are available.  You must implement this if 
	// the frame work is to pick up the snap-in
	BEGIN_CATEGORY_MAP(CSavProtectionProvider)
		IMPLEMENTED_CATEGORY(CATID_ProtectionProvider)
	END_CATEGORY_MAP()


	BEGIN_COM_MAP(CHPPProtectionProvider)
		COM_INTERFACE_ENTRY(IHPPProtectionProvider)
		COM_INTERFACE_ENTRY2(IDispatch, IProtection_Container)
		COM_INTERFACE_ENTRY(IProtection_Container)
        COM_INTERFACE_ENTRY(IProtection_DefinitionInfo)
		COM_INTERFACE_ENTRY(IProtection_Provider)
		COM_INTERFACE_ENTRY(IProtection)
	END_COM_MAP()


	DECLARE_PROTECT_FINAL_CONSTRUCT()

	HRESULT FinalConstruct();
	void FinalRelease();

    // IProtection_DefinitionInfo Methods
    STDMETHOD(get_Date)( SYSTEMTIME* defsDate );
    STDMETHOD(get_RevisionNo)( unsigned int* revisionNo );
    STDMETHOD(get_ShortDescription)( BSTR* shortDescription );

	//*********************************************************************
	//
	// *** IProtection_Provider Methods ***
	//
	//*********************************************************************
public:
	/**
	* ShowConfigureUI Is called by the UI framwork when the component is selected and 
	* configure is selected.  The snap-in at this point is to display the generic configuration
	* settings.
	*
    * @param HWND  parentWindowHandle [in]  The handle to the parent window for which to attach
	* the configuration panel
	*
    * @return HRESULT S_OK if successful.  Standard HRESULT error codes if failure
	* 
	*/
	STDMETHOD(ShowConfigureUI)( HWND  parentWindowHandle);


	/**
	* ShowLogUI Is called by the UI framwork when the component is selected and 
	* Display log is selected.  The snap-in at this point is to display the log file contents
	*
    * @param HWND  parentWindowHandle [in]  The handle to the parent window for which to attach
	* the log display panel
	*
    * @return HRESULT S_OK if successful.  Standard HRESULT error codes if failure
	* 
	*/
	STDMETHOD(ShowLogUI)( HWND  parentWindowHandle);


	/**
	* get_DisplayName Is called by the UI framwork when the framework need to display the		
	* name of the component/technology
	*
    * @param BSTR *  displayName[in/out]  Name of the component/technology is returned in the 
	* BSTR.
	*
    * @return HRESULT S_OK if successful.  Standard HRESULT error codes if failure
	* 
	*/
	STDMETHOD(get_DisplayName)( BSTR *  displayName);


	/**
	* get_Autoprotects Is called by the UI framwork when the framework need to display the
	* AutoProtection technologies that it is hosting.  One example would be SAV hosting AP, Internet
	* mail, and exchange.
	*
    * @param IProtection_Container * *  autoprotects [in/out]  This rountin will allocate
	* an IProtection_Container.  This container will contain all of the autoprotection technologyies 
	* supported by this module.  For HPP there is only one.
	*
    * @return HRESULT S_OK if successful.  Standard HRESULT error codes if failure
	* 
	*/
	STDMETHOD(get_Autoprotects)( IProtection_Container * *  autoprotects);


	/**
	* get_PrimaryAutoprotect Is called by the UI framwork when the framework need to get the
    * main autoprotect that is responsible for most of the protection.  In this case since
    * we have only one autoprotect, it's that one.
	*
    * @param IProtection * *  PrimaryAutoprotect [out]  
	*
    * @return HRESULT S_OK if successful.  Standard HRESULT error codes if failure
	* 
	*/
    STDMETHOD(get_PrimaryAutoprotect)( IProtection** primaryAutoprotect );

	/**
	* get_ID Is called by the UI framwork when the framework needs to obtain the GUID of the component.
	*
    * @param GUID *  id[out]  just returns the GUID for this component
	*
    * @return HRESULT S_OK if successful.  Standard HRESULT error codes if failure
	* 
	*/
	STDMETHOD(get_ID)( GUID *  id);


	/**
	* get_SplashGraphic Is called by the UI framwork when the framework needs to
    * get the splash graphic to show for us
	*
    * @param HGDIOBJ *  id[out]  Returns an HBITMAP of the bitmap to display
	*
    * @return HRESULT S_OK if successful.  Standard HRESULT error codes if failure
	* 
	*/
    STDMETHOD(get_SplashGraphic)( HGDIOBJ* bitmapHandle );

	/**
	* get_Installed Is called by the UI framwork when the framework need check with the componet (HPP) to 
	* make sure that it is properly installed
	*
    * @param VARIANT_BOOL* isInstalled  [in/out]  TRUE if properly installed FALSE otherwise.
	*
    * @return HRESULT S_OK if successful.  Standard HRESULT error codes if failure
	* 
	*/
	STDMETHOD(get_Installed)( VARIANT_BOOL* isInstalled );


	/**
	* get_ConfigureableScans Is called by the UI framwork when the framework is displaying all 
	* of the configurable types of scans for this component.  For example in the case of SAV it 
	* can be FULL, QUICK, or Custom scans.
	*
    * @param IProtection_ConfigureableScan_Container** scans [in/out]  Allocate and fill out the 
	* container that contains all of the configurable scans supported by HPP.
	*
    * @return HRESULT S_OK if successful.  Standard HRESULT error codes if failure
	* 
	*/
	STDMETHOD(get_ConfigureableScans)( IProtection_ConfigureableScan_Container** scans );


	/**
	* get_AdministrativeExceptions Is called by the UI framwork as part of the global exception 
	* handling.  It lets HPP return its administrator based exceptions.
	*
    * @param IExceptionItem_Container** administrativeExceptions [in/out]  Allocate and fills out the 
	* container that contains all of the administrative exceptions that HPP knows about.
	*
    * @return HRESULT S_OK if successful.  Standard HRESULT error codes if failure
	* 
	*/
	STDMETHOD(get_AdministrativeExceptions)( IExceptionItem_Container** administrativeExceptions );


	/**
	* get_LocalExceptions Is called by the UI framwork as part of the global exception 
	* handling.  It lets HPP return its local user based exceptions.
	*
    * @param IExceptionItem_Container** administrativeExceptions [in/out]  Allocate and fills out the 
	* container that contains all of the administrative exceptions that HPP knows about.
	*
    * @return HRESULT S_OK if successful.  Standard HRESULT error codes if failure
	* 
	*/
	STDMETHOD(get_LocalExceptions)( IExceptionItem_Container** localExceptions );



	/**
	* put_LocalExceptions Is called by the UI framwork as part of the global exception 
	* handling.  It lets HPP set local user exceptions.
	*
    * @param IExceptionItem_Container* administrativeExceptions [in]  Hands in a container of 
	* exceptions.  This list was compiled from the local UI.  This container will be used to 
	* enum all of the execptions for the HPP engine.
	*
    * @return HRESULT S_OK if successful.  Standard HRESULT error codes if failure
	* 
	*/
	STDMETHOD(put_LocalExceptions)( IExceptionItem_Container* newLocalExceptions );
private:
	// Data
	/**
	* we have to have an instance of this interface in order for the frame work to correctly init us.
	*/
	CComPtr<IProtection> m_ptrIHPPProtection;		
	
	/**
	* List of all autoprotects supported by this protection technology in our case there were only be one but the interfaces expect a list.
	*/
	ProtectionList		m_ptrHPPAutoProtects;		
	
	/**
	* pointer to the IGenConfig interface.  We use this to read and write to the configuration settings out to the registry.  The life time is the 
	* lifetime of this object
	*/
	IGenericConfig	*m_ptrGenConfig;				
    CTrustVerifier   trustVerifier;
	CHppOptions		m_hppOptions;

	//*********************************************************************
	//
	// *** IProtection_Container Methods ***
	//
	//*********************************************************************
public:
	/**
	* get_ProtectionCount Is called by the UI framwork.  The number of protection objects are to be 
	* returned.
	*
    * @param long *  noItems [in/out]  Pointer to a long that is to be filled in by the
	* rountine.
	*
    * @return HRESULT S_OK if successful.  Standard HRESULT error codes if failure
	* 
	*/


	STDMETHOD(get_ProtectionCount)( long *  noItems);
	/**
	* get_Item Is called by the UI framwork to get a pointer to a protection interface of the 
	* specified protection object.  
	*
    * @param GUID  itemID[in]  GUID of the protection object.
	*
    * @param IProtection * *  protection [in/out]  pointer to a protection interface. This will be
	* filled in by the routine.
	*
	* @return HRESULT S_OK if successful.  Standard HRESULT error codes if failure
	* 
	*/
	STDMETHOD(get_Item)( GUID  itemID,  IProtection * *  protection);


	/**
	* get__NewEnum Is called by the UI framwork in order to enumerate all of the protection objects
	* provided by the snap-in
	*
    * @param IEnumProtection * *  enumerator [in/out]  pointer to an enumurator.  This is filled in by t
	* the routine.
	*
	* @return HRESULT S_OK if successful.  Standard HRESULT error codes if failure
	* 
	*/
	STDMETHOD(get__NewEnum)( IEnumProtection * *  enumerator);



	//*********************************************************************
	//
	// *** IProtection Methods ***
	//
	//	Note:  For sum reason the main snap-in protection_provider must
	//		   implement this interface even if it does not do anything.
	//			the real IProtection is implemented as part of CHPPProtection
	//*********************************************************************
public:
	STDMETHOD(get_Description)( BSTR * userDescription);
	STDMETHOD(get_ProtectionStatus)( ProtectionStatus * currStatus)
	{
		if (currStatus == NULL)
			return E_INVALIDARG;
		
		// Validate caller security
		if (trustVerifier.IsCallerProcessTrusted() != S_OK)
			return E_ACCESSDENIED;

		if (m_hppOptions.IsHPPFullyEnabled())
			*currStatus = ProtectionStatus_On;
		else
			*currStatus = ProtectionStatus_Off;

		return S_OK;
	}
	STDMETHOD(get_ProtectionStatusDescription)( BSTR * statusDescription)
	{
		// Add your function implementation here.
		return E_NOTIMPL;
	}
	STDMETHOD(get_ProtectionStatusReadOnly)( VARIANT_BOOL * isReadOnly)
	{
		// Add your function implementation here.
		return E_NOTIMPL;
	}
	STDMETHOD(get_ProtectionConfiguration)( VARIANT_BOOL * protectionEnabled)
	{
		// Add your function implementation here.
		return E_NOTIMPL;
	}
	STDMETHOD(put_ProtectionConfiguration)( VARIANT_BOOL newStatus)
	{
		// Add your function implementation here.
		return E_NOTIMPL;
	}
};


OBJECT_ENTRY_AUTO(__uuidof(HPPProtectionProvider), CHPPProtectionProvider)
