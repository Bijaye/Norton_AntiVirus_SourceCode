/*
 * PROPRIETARY/CONFIDENTIAL.  use of this product is subject to license terms.
 * Copyright (c) 2005 Symantec Corporation.  All Rights Reserved.
 *
*/

// QserverConfig.h : Declaration of the CQserverConfig

#ifndef __QSERVERCONFIG_H_
#define __QSERVERCONFIG_H_

#include "resource.h"       // main symbols

/////////////////////////////////////////////////////////////////////////////
// CQserverConfig
class ATL_NO_VTABLE CQserverConfig : 
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<CQserverConfig, &CLSID_QserverConfig>,
	public IQserverConfig
{
public:
    CQserverConfig() : 
      m_bChanged( FALSE ),
      m_bAttached( TRUE )
	{

	}

DECLARE_REGISTRY_RESOURCEID(IDR_QSERVERCONFIG)
DECLARE_NOT_AGGREGATABLE(CQserverConfig)

DECLARE_PROTECT_FINAL_CONSTRUCT()

BEGIN_COM_MAP(CQserverConfig)
	COM_INTERFACE_ENTRY(IQserverConfig)
END_COM_MAP()

// IQserverConfig
public:
    STDMETHOD( GetValue )(/* [in] */ BSTR bstrFieldName, 
                          /* [out] */ VARIANT *v );	
    STDMETHOD( SetValue )(/* [in] */ BSTR bstrFieldName,
                          /* [in] */ VARIANT v );
    STDMETHOD( Detach )();                          

// Overrides
public:
    void FinalRelease();

private:
    BOOL        m_bChanged;
    BOOL        m_bAttached;
};

#endif //__QSERVERCONFIG_H_
