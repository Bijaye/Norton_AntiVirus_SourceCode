// luNavCallBack.h : Declaration of the CluNavCallBack

#ifndef __LUNAVCALLBACK_H_
#define __LUNAVCALLBACK_H_

#include "resource.h"       // main symbols
#import "PRODUCTREGCOM.TLB" raw_interfaces_only, raw_native_types, no_namespace, named_guids 
#include "defutils.h"
#include "NavLu.h"

/////////////////////////////////////////////////////////////////////////////
// CluNavCallBack
class ATL_NO_VTABLE CluNavCallBack : 
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<CluNavCallBack, &CLSID_luNavCallBack>,
	public IluCallback
{
public:
	CluNavCallBack() : m_pNavLu( NULL ), m_bPostProcessCalled( FALSE ), m_bPreProcessCompleted( FALSE )
	{
	}

	void FinalRelease()
	{
		if ( m_pNavLu )
			delete m_pNavLu;
		m_pNavLu = NULL;

		// Release the pointer to Product Reg COM, if it exists.
		m_ptrProductReg = NULL;
	}

DECLARE_REGISTRY( {09C9DBC1-893D-11D2-B40A-00600831DD76}, _T("LiveUpdate.luNavCallBack.1"), _T("LiveUpdate.luNavCallBack"), IDS_LUNAVCALLBACK_DESC, THREADFLAGS_APARTMENT )

DECLARE_PROTECT_FINAL_CONSTRUCT()

BEGIN_COM_MAP(CluNavCallBack)
	COM_INTERFACE_ENTRY(IluCallback)
END_COM_MAP()

// IluNavCallBack
public:
// IluCallback
	STDMETHOD(OnLUNotify)(tagLU_CALLBACK_TYPE nType, BSTR strMoniker);
protected:
	CNavLu *m_pNavLu;
	IluProductRegPtr m_ptrProductReg;
	BOOL m_bPreProcessCompleted;
	BOOL m_bPostProcessCalled;
};

#endif //__LUNAVCALLBACK_H_
