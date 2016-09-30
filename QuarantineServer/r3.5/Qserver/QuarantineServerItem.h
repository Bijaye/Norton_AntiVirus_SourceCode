/*
 * PROPRIETARY/CONFIDENTIAL.  use of this product is subject to license terms.
 * Copyright (c) 2005 Symantec Corporation.  All Rights Reserved.
 *
*/

// QuarantineServerItem.h : Declaration of the CQuarantineServerItem

#ifndef __QUARANTINESERVERITEM_H_
#define __QUARANTINESERVERITEM_H_

#include "resource.h"       // main symbols
#include "qspak.h"

/////////////////////////////////////////////////////////////////////////////
// CQuarantineServerItem
class ATL_NO_VTABLE CQuarantineServerItem : 
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<CQuarantineServerItem, &CLSID_QuarantineServerItem>,
	public IQuarantineServerItem
{
public:
    CQuarantineServerItem() :
    m_bChanged( FALSE ),
    m_hItem( NULL ),
    m_ulItemID( 0 )
	{
	}

DECLARE_REGISTRY_RESOURCEID(IDR_QUARANTINESERVERITEM)
DECLARE_NOT_AGGREGATABLE(CQuarantineServerItem)

DECLARE_PROTECT_FINAL_CONSTRUCT()

BEGIN_COM_MAP(CQuarantineServerItem)
	COM_INTERFACE_ENTRY(IQuarantineServerItem)
END_COM_MAP()

// IQuarantineServerItem
public:
    STDMETHOD( GetValue )(/* [in] */ BSTR bstrFieldName, 
                          /* [out] */ VARIANT *v );	
    STDMETHOD( SetValue )(/* [in] */ BSTR bstrFieldName,
                          /* [in] */ VARIANT v );			
    STDMETHOD( GetAllValues )( /* [out] */  VARIANT *v );
    STDMETHOD( Commit)();

public:
    STDMETHOD(Initialize)( BSTR strFileName, ULONG ulItemID );

    virtual void FinalRelease();

private:
    // 
    // Handle to open item.
    // 
    HQSERVERITEM    m_hItem;
    
    // 
    // Changed flag
    // 
    BOOL            m_bChanged;
    
    // 
    // Identifier for this file.
    // 
    ULONG           m_ulItemID;
};

#endif //__QUARANTINESERVERITEM_H_
