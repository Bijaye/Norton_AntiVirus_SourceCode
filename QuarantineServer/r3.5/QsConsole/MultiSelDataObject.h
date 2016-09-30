/*
 * PROPRIETARY/CONFIDENTIAL.  use of this product is subject to license terms.
 * Copyright (c) 2005 Symantec Corporation.  All Rights Reserved.
 *
*/

// MultiSelDataObject.h: interface for the CMultiSelDataObject class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MULTISELDATAOBJECT_H__23726348_CC1E_11D2_8F46_3078302C2030__INCLUDED_)
#define AFX_MULTISELDATAOBJECT_H__23726348_CC1E_11D2_8F46_3078302C2030__INCLUDED_

#if _MSC_VER > 1000
//#pragma once
#endif // _MSC_VER > 1000

#include "atlsnap.h"
#include "qscon.h"
#include "qsconsole.h"

class CMultiSelDataObject : public CSnapInDataObjectImpl,
                            public IQCResultItem
{
public:
BEGIN_COM_MAP(CMultiSelDataObject)
	COM_INTERFACE_ENTRY(IDataObject)
    COM_INTERFACE_ENTRY(IQCResultItem)
    COM_INTERFACE_ENTRY_CHAIN(CSnapInDataObjectImpl)
END_COM_MAP()
    
    CMultiSelDataObject();
	virtual ~CMultiSelDataObject();

public:
    // 
    // Overrides
    // 
	STDMETHOD(GetData)(FORMATETC *pformatetcIn, STGMEDIUM *pmedium);
    STDMETHOD(GetDataHere)(FORMATETC* pformatetc, STGMEDIUM* pmedium);
	STDMETHOD(QueryGetData)(FORMATETC* pformatetc);

    // 
    // IQCResultItem 
    // 
    STDMETHOD(GetItemIDs)( 
        /*[out]*/ ULONG*  ulCount,
		/*[out]*/ ULONG** ulItemArray );

	STDMETHOD(GetQserverItem)( 
        /*[in]*/ ULONG ulItemID,
        /*[out]*/ IUnknown** pItem,
		/*[out]*/ ULONG *pIdentity);

    // 
    // List of selected items.  Populated in IComponent::QueryDataItems.
    // 
    CTypedPtrList< CPtrList, CSnapInItem* > m_ItemList;

    // 
    // Pointer to our component data object.
    //     
    IComponentData*             m_pComponent;
    

    // 
    // Clipboard formats for multiselect stuff.
    // 
    static CLIPFORMAT m_CCF_MULTI_SELECT_SNAPINS;
    static CLIPFORMAT m_CCF_OBJECT_TYPES_IN_MULTI_SELECT;

    static CMultiSelDataObject* GetMultiSelectDataObject( LPDATAOBJECT pDataObject );
};

_declspec( selectany ) CLIPFORMAT CMultiSelDataObject::m_CCF_MULTI_SELECT_SNAPINS = 0;
_declspec( selectany ) CLIPFORMAT CMultiSelDataObject::m_CCF_OBJECT_TYPES_IN_MULTI_SELECT = 0;


#endif // !defined(AFX_MULTISELDATAOBJECT_H__23726348_CC1E_11D2_8F46_3078302C2030__INCLUDED_)
