/*
 * PROPRIETARY/CONFIDENTIAL.  use of this product is subject to license terms.
 * Copyright (c) 2005 Symantec Corporation.  All Rights Reserved.
 *
*/

// QuarantineServerII.h : Declaration of the CQuarantineServerII

#ifndef __QUARANTINESERVERII_H_
#define __QUARANTINESERVERII_H_

#include "resource.h"       // main symbols

/////////////////////////////////////////////////////////////////////////////
// CQuarantineServerII
class ATL_NO_VTABLE CQuarantineServerII : 
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<CQuarantineServerII, &CLSID_QuarantineServerII>,
	public IQuarantineServerII
{
public:
	CQuarantineServerII()
	{
	}

DECLARE_REGISTRY_RESOURCEID(IDR_QUARANTINESERVERII)
DECLARE_NOT_AGGREGATABLE(CQuarantineServerII)

DECLARE_PROTECT_FINAL_CONSTRUCT()

BEGIN_COM_MAP(CQuarantineServerII)
	COM_INTERFACE_ENTRY(IQuarantineServerII)
END_COM_MAP()

// IQuarantineServerII
public:
	STDMETHOD(OpenPurgeLog)(/*[out]*/ ULONG* pulLogHandle);
	STDMETHOD(ResetQuarantineConfigInterface)(void);
	STDMETHOD(GetVersion)(/*[out]*/ ULONG* pulVersion);

private:
	HRESULT GetVersionFromReg(ULONG *pulVer);
};

#endif //__QUARANTINESERVERII_H_
