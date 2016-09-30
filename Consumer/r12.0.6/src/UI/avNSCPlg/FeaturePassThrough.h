#pragma once

#include "FeaturePassThruInterface.h"

class CFeaturePassThrough :
	public ISymBaseImpl< CSymThreadSafeRefCount >,
	public IFeaturePassThru
{
public:
	CFeaturePassThrough(void);
	virtual ~CFeaturePassThrough(void);

	SYM_INTERFACE_MAP_BEGIN()
		SYM_INTERFACE_ENTRY(IID_IFeaturePassThru, IFeaturePassThru)
	SYM_INTERFACE_MAP_END()

public:
	SYMRESULT GetFeaturePassThruObject(REFSYMOBJECT_ID oid, REFSYMINTERFACE_ID iid, void** ppvObject, cc::IString* psOwner, ::cc::IModuleLifetimeMgr* pModuleMgr, DWORD& dwDisposition);
};