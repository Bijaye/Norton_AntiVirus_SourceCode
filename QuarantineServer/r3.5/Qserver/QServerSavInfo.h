/*
 * PROPRIETARY/CONFIDENTIAL.  use of this product is subject to license terms.
 * Copyright (c) 2005 Symantec Corporation.  All Rights Reserved.
 *
*/

// QServerNavInfo.h : Declaration of the CQServerNavInfo	

#ifndef __CQSERVERSAVINFO_H_
#define __CQSERVERSAVINFO_H_

#include "resource.h"       // main symbols
#include "avscanner.h"
/////////////////////////////////////////////////////////////////////////////
// CQSERVERNAVINFO
class ATL_NO_VTABLE CQServerSavInfo : 
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<CQServerSavInfo, &CLSID_QServerSavInfo>,
	public IQServerSavInfo,
    public IEnumSavVirusInfo
{
public:
	CQServerSavInfo();
	virtual ~CQServerSavInfo();
	

DECLARE_REGISTRY_RESOURCEID(IDR_QSERVERSAVINFO)
DECLARE_NOT_AGGREGATABLE(CQServerSavInfo)

DECLARE_PROTECT_FINAL_CONSTRUCT()

BEGIN_COM_MAP(CQServerSavInfo)
	COM_INTERFACE_ENTRY(IQServerSavInfo)
	COM_INTERFACE_ENTRY(IEnumSavVirusInfo)
END_COM_MAP()

// IQServerSavInfo
public:
	STDMETHOD(GetSavAPIVersion)(/*[out]*/ VARIANT *v);
	STDMETHOD(GetSavEngVersion)(/*[out]*/ VARIANT *v);
	STDMETHOD(GetSavDefVersion)(/*[out]*/ VARIANT *v);
	STDMETHOD(GetSavSeqNumber)(/*[out]*/ VARIANT *v);
	STDMETHOD(GetSavDefDate)(/*[out]*/ VARIANT *v);
    STDMETHOD(GetEnumVirusInfo)(/*[out,retval]*/ IEnumSavVirusInfo** pEnum);


// IEnumSavVirusInfo
public:
    STDMETHOD( Next )( /*[ in ]*/ ULONG celt, 
			           /*[out]*/  VARIANT *aVirusInfo, 
			           /*[out]*/  ULONG* pceltFetched );

    STDMETHOD( Skip ) ( /*[in]*/ ULONG celt );
    STDMETHOD( Reset ) ();
	STDMETHOD(GetSavNoDefs)(/*[out]*/ ULONG *pulNoDefs);

public:
    // 
    // Overrides
    // 
    void FinalRelease(){};


private:
	static int m_iNoInstances;
    // 
    // Handle to NAVAPI
    // 
    CAVScanner      m_AVScanner;

	HRESULT			m_hrAVScannerStatus;
	
	LEGVIRUSDBINFOEX m_nvVirusDBInfo;

	int				m_iCurrentVIndex;
	};



#endif //__CQSERVERSAVINFO_H_
