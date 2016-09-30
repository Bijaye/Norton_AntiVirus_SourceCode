/*
 * PROPRIETARY/CONFIDENTIAL.  use of this product is subject to license terms.
 * Copyright (c) 2005 Symantec Corporation.  All Rights Reserved.
 *
*/

// EnumQserverItemsVariant.h : Declaration of the CEnumQserverItemsVariant

#ifndef __ENUMQSERVERITEMSVARIANT_H_
#define __ENUMQSERVERITEMSVARIANT_H_

#include "resource.h"       // main symbols
#include "Enumerator.h"
#include "qspak.h"



/////////////////////////////////////////////////////////////////////////////
// CEnumQserverItemsVariant
class ATL_NO_VTABLE CEnumQserverItemsVariant : 
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<CEnumQserverItemsVariant, &CLSID_EnumQserverItemsVariant>,
	public IEnumQserverItemsVariant
{
public:
    CEnumQserverItemsVariant();
    CEnumQserverItemsVariant( LPCTSTR szQuarantinePath );


DECLARE_REGISTRY_RESOURCEID(IDR_ENUMQSERVERITEMSVARIANT)
DECLARE_NOT_AGGREGATABLE(CEnumQserverItemsVariant)

DECLARE_PROTECT_FINAL_CONSTRUCT()

BEGIN_COM_MAP(CEnumQserverItemsVariant)
	COM_INTERFACE_ENTRY(IEnumQserverItemsVariant)
END_COM_MAP()

// IEnumQserverItemsVariant
public:
    STDMETHOD( Next )(   /* [ in ] */ ULONG celt, 
			             /* [ out, size_is( celt ), length_is(*pceltFetched) ] */ 
			              VARIANT *aQserverItems, 
			             /* [ out ]  */ ULONG* pceltFetched );

    STDMETHOD( Skip ) ( /* [in ] */ ULONG celt );
    STDMETHOD( Reset ) ();
    STDMETHOD( Clone ) (/* [ out ] */ IEnumQserverItemsVariant** ppenum );

public:    
    STDMETHOD( Initialize )( /* [ in ] */ BSTR strQuarantineFolder,
						     /* [ in ] */ VARIANT vArray );



public:
    // 
    // Overrides
    // 
    void FinalRelease();


private:
    struct DATAFIELD
        {
        LPSTR           pszFieldName;
        VARTYPE         vType;
        };

    
    STDMETHOD(GetVariantFields)( LPCTSTR szFileName, VARIANT *pVariant );
    STDMETHOD(GetBSTRVal)( HQSERVERITEM hItem, LPSTR szFieldName, int iIndex, SAFEARRAY *psa );
    STDMETHOD(GetULONGVal)( HQSERVERITEM hItem, LPSTR szFieldName, int iIndex, SAFEARRAY *psa );
    STDMETHOD(GetBinaryVal)( HQSERVERITEM hItem, LPSTR szFieldName, int iIndex, SAFEARRAY *psa );
    STDMETHOD(GetDateVal)( HQSERVERITEM hItem, LPSTR szFieldName, int iIndex, SAFEARRAY *psa );

private:
    // 
    // Enumeration helper object.
    // 
    CEnumerator*    m_pEnumerator;

    // 
    // Path where quarantine items live.
    // 
    TCHAR           m_szQuarantineDir[ MAX_PATH + 1];

    // 
    // Linked list of field names.
    // 
    DATAFIELD*      m_pList;

    // 
    // Number data fields requested.
    // 
    int             m_iArraySize;


};

#endif //__ENUMQSERVERITEMSVARIANT_H_
