// NAVEventCommonInt.h : Declaration of the CNAVEventCommonInt

#ifndef __NAVEVENTCOMMONINT_H_
#define __NAVEVENTCOMMONINT_H_

#include "NAVEventResource.h"       // main symbols

#include "NAVEvents.h"
#include "ccSerializableEventImpl.h"

//#include "AllNAVEvents.h"
#include "NAVEventCommon.h"

_COM_SMARTPTR_TYPEDEF(INAVEventCommonInt, __uuidof(INAVEventCommonInt));

/////////////////////////////////////////////////////////////////////////////
// CNAVEventCommonInt
class ATL_NO_VTABLE CNAVEventCommonInt : 
    public CComObjectRootEx<CComMultiThreadModel>,
	public CComCoClass<CNAVEventCommonInt, &CLSID_NAVEventCommonInt>,
    public IDispatchImpl<ccEvtMgr::ISerializableEventImpl<INAVEventCommonInt, CNAVEventCommon>, &IID_INAVEventCommonInt, &LIBID_NAVEVENTSLib>
{
public:
	CNAVEventCommonInt(){}
	virtual ~CNAVEventCommonInt(){}

DECLARE_REGISTRY_RESOURCEID(IDR_NAVEVENTCOMMONINT)
DECLARE_PROTECT_FINAL_CONSTRUCT()

BEGIN_COM_MAP(CNAVEventCommonInt)
    COM_INTERFACE_ENTRY(IEventEx)
	COM_INTERFACE_ENTRY(INAVEventCommonInt)
    COM_INTERFACE_ENTRY(ISerializableEventEx)
    COM_INTERFACE_ENTRY(IDispatch)
END_COM_MAP()


// INAVEventCommonInt
public:
	STDMETHOD(SetPropertyBSTR)(long lPropIndex, BSTR* pbstrValue);
	STDMETHOD(GetPropertyBSTR)(long lPropIndex, BSTR* pbstrValue);
    STDMETHOD(SetPropertyLong)(long lPropIndex, long lValue);
	STDMETHOD(GetPropertyLong)(long lPropIndex, long* plValue);

    
// CNAVEventCommonInt
private:
    CNAVEventCommonInt(const CNAVEventCommonInt&);
    CNAVEventCommonInt& operator =(const CNAVEventCommonInt&);
};

#endif //__NAVEVENTCOMMONINT_H_
