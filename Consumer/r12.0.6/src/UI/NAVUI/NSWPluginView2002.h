// NSWFrame2002.h
//
//////////////////////////////////////////////////////////////////////

#if !defined(_NSWFRAME2002_H__44F14CE3_A5CC_4249_B7CB_A599F8262B82__INCLUDED_)
#define _NSWFRAME2002_H__44F14CE3_A5CC_4249_B7CB_A599F8262B82__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000



#include "resource.h"       // main symbols
#include "brandingimpl.h"

/////////////////////////////////////////////////////////////////////////////
// CNSWFrame2002
class ATL_NO_VTABLE CNSWPluginView2002 : 
	 public CComObjectRootEx<CComSingleThreadModel>
	,public CComCoClass<CNSWPluginView2002, &CLSID_NSWPluginView2002>
    ,public CBrandingImpl
    ,public IUnknown // odd thing, but we need this object to have a least one interface of its own.
{
public:
	CNSWPluginView2002()
	{
	}

DECLARE_REGISTRY_RESOURCEID(IDR_NSWPLUGINVIEW2002)
DECLARE_GET_CONTROLLING_UNKNOWN()

DECLARE_PROTECT_FINAL_CONSTRUCT()
	HRESULT FinalConstruct();

BEGIN_COM_MAP(CNSWPluginView2002)
    COM_INTERFACE_ENTRY(IUnknown) // this map stuff starts asserting if we dont have at least one interface
	COM_INTERFACE_ENTRY_AGGREGATE(IID_INSWIntegratorProduct		, m_pInner.p)
	COM_INTERFACE_ENTRY_AGGREGATE(IID_IVarBstrCol				, m_pInner.p)
	COM_INTERFACE_ENTRY_AGGREGATE(IID_INSWHelpMenuEx			, m_pInner.p)
END_COM_MAP()

public:
	CComPtr<IUnknown> m_pInner;
};

#endif // !defined(_NSWFRAME2002_H__44F14CE3_A5CC_4249_B7CB_A599F8262B82__INCLUDED_)
