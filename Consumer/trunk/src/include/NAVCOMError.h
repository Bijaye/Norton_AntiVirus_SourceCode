////////////////////////
//
// PROPRIETARY / CONFIDENTIAL.
// Use of this product is subject to license terms.
// Copyright © 2006 Symantec Corporation.
// All rights reserved.
//
/////////////////////////////////////////////////////////////////////////////////////////////////

// NAVCOMError.h : Declaration of the CNAVCOMError

#ifndef __NAVCOMERROR_H_
#define __NAVCOMERROR_H_

#include "NAVErrorResource.h"       // main symbols

#include <string>

#include "AvEvents.h"
#include "AllNAVEvents.h"
#include "NAVVersion.h"
#include "NAVError.h"
#include "ccSymCommonClientInfo.h"
#include "NAVInfo.h"
#include "optnames.h"

#include "ccErrorDisplay.h"

/////////////////////////////////////////////////////////////////////////////
// CNAVCOMError
class ATL_NO_VTABLE CNAVCOMError : 
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<CNAVCOMError, &CLSID_NAVCOMError>,
	public IDispatchImpl<INAVCOMError, &IID_INAVCOMError, &LIBID_NAVERRORLib>,
    public IObjectSafetyImpl<CNAVCOMError, INTERFACESAFE_FOR_UNTRUSTED_CALLER>
{
public:
	CNAVCOMError();
    virtual ~CNAVCOMError ();

DECLARE_REGISTRY_RESOURCEID(IDR_NAVCOMERROR)

DECLARE_PROTECT_FINAL_CONSTRUCT()

BEGIN_CATEGORY_MAP(CNAVCOMError)
    IMPLEMENTED_CATEGORY(CATID_SafeForInitializing)
    IMPLEMENTED_CATEGORY(CATID_SafeForScripting)
END_CATEGORY_MAP()

BEGIN_COM_MAP(CNAVCOMError)
	COM_INTERFACE_ENTRY(INAVCOMError)
	COM_INTERFACE_ENTRY(IDispatch)
    COM_INTERFACE_ENTRY(IObjectSafety)
END_COM_MAP()

// INAVCOMError
public:
	// These properties will FORCE errors to occur.
    //
    STDMETHOD(get_ForcedModuleID)(/*[out, retval]*/ long *pVal);
	STDMETHOD(put_ForcedModuleID)(/*[in]*/ long newVal);
	STDMETHOD(get_ForcedErrorID)(/*[out, retval]*/ long *pVal);
	STDMETHOD(put_ForcedErrorID)(/*[in]*/ long newVal);
	
    // Show the error dialog and/or log the error
    //
    STDMETHOD(Show)(BOOL bDisplay, BOOL bLog, long hWnd);

    // Log and display the error. Still here for backwards compatibility.
    //
	STDMETHOD(LogAndDisplay)(/*[in]*/ long hwnd);

    // *** NAV Error properties ***
    //
    
    // Why is HResult not an SCODE? Because script doesn't treat SCODE's as
    // numbers so we can't display or interact with them. Weak JSCRIPT, weak.
    //
	STDMETHOD(get_HResult)(/*[out, retval]*/ long *pVal);
	STDMETHOD(put_HResult)(/*[in]*/ long newVal);

    // Description of the error
    //
    STDMETHOD(get_Message)(/*[out, retval]*/ BSTR *pVal);
    STDMETHOD(put_Message)(/*[in]*/ BSTR newVal);

    // Symantec globally unique error ID
    //
	STDMETHOD(get_ErrorID)(/*[out, retval]*/ long *pVal);
    STDMETHOD(put_ErrorID)(/*[in]*/ long newVal);

    // Symantec globally unique module ID
    //
	STDMETHOD(get_ModuleID)(/*[out, retval]*/ long *pVal);
    STDMETHOD(put_ModuleID)(/*[in]*/ long newVal);

	// Specify to use generic error message
	//
	STDMETHOD(get_ErrorResourceID)(/*[out, retval]*/ long *pVal);
	STDMETHOD(put_ErrorResourceID)(/*[in]*/ long newVal);

protected:
	long m_lForcedModuleID;
	long m_lForcedErrorID;
	void init();
	bool CreateErrorString();
	
    HRESULT Display (/*[in]*/ long hwnd);
	HRESULT Log();

    CComPtr <ICreateErrorInfo> m_spCreateErrorInfo;
	CString m_strMessage;
	CString m_strCaption;
	CString m_strProductName;

    long m_lErrorResult;
    long m_lErrorID;
    long m_lModuleID;
    long m_lHelpContextID;
	long m_lErrorResID;

    bool m_bAlreadyLogged;
};

#endif //__NAVCOMERROR_H_
