/*
 * PROPRIETARY/CONFIDENTIAL.  use of this product is subject to license terms.
 * Copyright (c) 2005 Symantec Corporation.  All Rights Reserved.
 *
*/

// AvisConfig.h : Declaration of CAvisConfig


#ifndef __AVISSERVERCONFIG_H_
#define __AVISSERVERCONFIG_H_

#include "resource.h"       // main symbols

/////////////////////////////////////////////////////////////////////////////
// CAvisConfig        
class ATL_NO_VTABLE CAvisConfig : 
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<CAvisConfig, &CLSID_AvisConfig>,
	public IAvisConfig
{
public:
    CAvisConfig() : 
      m_bChanged( FALSE ),
      m_bAttached( TRUE )
	{
	}

// DECLARE_REGISTRY_RESOURCEID(IDR_aserver)    
DECLARE_REGISTRY_RESOURCEID(IDR_AVISCONFIG) 
DECLARE_NOT_AGGREGATABLE(CAvisConfig)

DECLARE_PROTECT_FINAL_CONSTRUCT()

BEGIN_COM_MAP(CAvisConfig)
	COM_INTERFACE_ENTRY(IAvisConfig)
END_COM_MAP()

// IAvisConfig
public:
    STDMETHOD( GetValue )(/* [in] */ BSTR bstrFieldName, /*[in]*/ BSTR bstrKeyName, 
                          /* [out] */ VARIANT *v );	
    STDMETHOD( SetValue )(/* [in] */ BSTR bstrFieldName,/*[in]*/ BSTR bstrKeyName, 
                          /* [in] */ VARIANT v );
    STDMETHOD( Detach )();                          

// Overrides
public:
    void FinalRelease();

private:
    BOOL        m_bChanged;
    BOOL        m_bAttached;
};

#endif //__AVISSERVERCONFIG_H_

