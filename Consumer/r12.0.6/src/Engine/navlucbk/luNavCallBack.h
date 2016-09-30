// luNavCallBack.h : Declaration of the CluNavCallBack

#ifndef __LUNAVCALLBACK_H_
#define __LUNAVCALLBACK_H_

#include "resource.h"       // main symbols
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
        if( m_bPreProcessCompleted && !m_bPostProcessCalled )
        {
            CCTRACEE("CluNavCallBack::FinalRelease() - PreProcess() was complete but post process was not called yet. Calling it now to clean up any microdefs left overs.");
            if( m_pNavLu )
            {
                m_pNavLu->PostProcess();
                m_bPostProcessCalled = TRUE;
            }
        }

		if ( m_pNavLu )
			delete m_pNavLu;
		m_pNavLu = NULL;

		// Release the pointer to Product Reg COM, if it exists.
        if( m_ptrProductReg != NULL )
		    m_ptrProductReg = NULL;
	}

//Changing DECLARE_REGISTRY to DECLARE_REGISTRY_RESOURCE becasue of possible problem with string ID IDS_LUNAVCALLBACK_DESC
DECLARE_REGISTRY_RESOURCEID(IDR_LUNAVCALLBACK)

DECLARE_PROTECT_FINAL_CONSTRUCT()

BEGIN_COM_MAP(CluNavCallBack)
	COM_INTERFACE_ENTRY(IluCallback)
END_COM_MAP()

// IluNavCallBack
public:
// IluCallback
	STDMETHOD(OnLUNotify)(tagLU_CALLBACK_TYPE nType, const BSTR strMoniker);
protected:
	CNavLu *m_pNavLu;
	 CComPtr<IluProductReg> m_ptrProductReg;
	BOOL m_bPreProcessCompleted;
	BOOL m_bPostProcessCalled;
};

#endif //__LUNAVCALLBACK_H_
