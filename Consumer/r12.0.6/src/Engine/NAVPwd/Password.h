// /////////////////////////////////////////////////////////////////////////////////////////////////
// Symantec copyright header start
// //////////////////////
//
// PROPRIETARY / CONFIDENTIAL.
// Use of this product is subject to license terms.
// Copyright © 2004 Symantec Corporation.
// All rights reserved.
//
// /////////////////////////////////////////////////////////////////////////////////////////////////
// Symantec copyright header stop
// //////////////////////
// Password.h: Definition of the Password class
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_PASSWORD_H__A7EC4893_648A_446F_8102_85E62D9D1A95__INCLUDED_)
#define AFX_PASSWORD_H__A7EC4893_648A_446F_8102_85E62D9D1A95__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "resource.h"       // main symbols
#include <comdef.h>


#include "NAVPwdExp.h"
#include "ScriptSafeVerifyTrustCached.h"		// For scripting security

bool VerifyLoader(void);

/////////////////////////////////////////////////////////////////////////////
// Password

class Password : 
	public IDispatchImpl<IPassword, &IID_IPassword, &LIBID_CCPASSWDLib>, 
	public ISupportErrorInfo,
	public CComObjectRoot,
	public CComCoClass<Password,&CLSID_Password>,
    public CScriptSafe<Password>,  // For SafeScript
    public IObjectSafetyImpl<Password, INTERFACESAFE_FOR_UNTRUSTED_CALLER | INTERFACESAFE_FOR_UNTRUSTED_DATA>

{
public:
	Password();

BEGIN_COM_MAP(Password)
	COM_INTERFACE_ENTRY(IDispatch)
	COM_INTERFACE_ENTRY(IPassword)
	COM_INTERFACE_ENTRY(ISupportErrorInfo)
    COM_INTERFACE_ENTRY(IObjectSafety) 
    COM_INTERFACE_ENTRY(IObjectWithSite)  // For SafeScript
	COM_INTERFACE_ENTRY(ISymScriptSafe)   // Allow programmatic enabling/disabling of safety check
END_COM_MAP()

BEGIN_CATEGORY_MAP(Password)
	IMPLEMENTED_CATEGORY(CATID_SafeForScripting)
END_CATEGORY_MAP()

//DECLARE_NOT_AGGREGATABLE(Password) 
// Remove the comment from the line above if you don't want your object to 
// support aggregation.

private:
	long m_lProductID;
	_bstr_t m_bsFeatureName;
	_bstr_t m_bsUserName;
	_bstr_t m_bsProductName;
//	BOOL m_bUseWindowsAccounts;

	HRESULT CheckReturnCode(HRESULT hr)
	{	
		if(hr == CCPW_USER_CANCEL)
			return S_OK;

		return hr;
	}

public:

DECLARE_REGISTRY_RESOURCEID(IDR_Password)
// ISupportsErrorInfo
	STDMETHOD(InterfaceSupportsErrorInfo)(REFIID riid);

// IPassword
public:
	STDMETHOD(get_UserPasswordSet)(/*[out, retval]*/ BOOL *pVal);
	STDMETHOD(get_Enabled)(/*[out, retval]*/ BOOL *pVal);
	STDMETHOD(put_Enabled)(/*[in]*/ BOOL newVal);
	STDMETHOD(get_ProductName)(/*[out, retval]*/ BSTR *pVal);
	STDMETHOD(put_ProductName)(/*[in]*/ BSTR newVal);
	STDMETHOD(AddUser)(/*[in]*/ BSTR bsPassword, /*[out,retval]*/ BOOL *bSuccess);
	STDMETHOD(Clear)(/*[out, retval]*/ BOOL *bSuccess);
	STDMETHOD(Reset)(/*[out, retval]*/ BOOL *bSuccess);
//	STDMETHOD(get_UseWindowsAccounts)(/*[out, retval]*/ BOOL *pVal);
//	STDMETHOD(put_UseWindowsAccounts)(/*[in]*/ BOOL newVal);
	STDMETHOD(get_UserName)(/*[out, retval]*/ BSTR *pVal);
	STDMETHOD(put_UserName)(/*[in]*/ BSTR newVal);
	STDMETHOD(Check)(/*[out,retval]*/ BOOL *bSuccess);
	STDMETHOD(Set)(/*[in]*/ BOOL bPromptForOldPwd, /*[out,retval]*/ BOOL *bSuccess);
	STDMETHOD(put_FeatureName)(/*[in]*/ BSTR newVal);
	STDMETHOD(put_ProductID)(/*[in]*/ long newVal);
};

#endif // !defined(AFX_PASSWORD_H__A7EC4893_648A_446F_8102_85E62D9D1A95__INCLUDED_)
