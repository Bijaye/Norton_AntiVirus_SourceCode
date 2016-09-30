#include "StdAfx.h"
#include "FeaturePassThrough.h"
#include "ccSymModuleLifetimeMgrHelper.h"

#include "AdminCheckFeature.h"


// factory define
#define DEFAULT_FACTORY(_ClassType, _InterfaceID) \
	_ClassType* p = NULL; \
	try \
	{ \
		p = new _ClassType; \
	} \
	SYMINTCATCHMEMORYEXECPTION() \
	if(NULL == p) \
	{ \
		CCTRCTXE0(_T("out of memory")); \
		return SYMERR_OUTOFMEMORY; \
	} \
	SYMRESULT result = p->QueryInterface(_InterfaceID, ppvObject); \
	if(SYM_FAILED(result)) \
	{ \
		CCTRCTXE1(_T("interface not found: %d"), result); \
		delete p; \
		return result; \
	} \
	p->AttachModuleManager(pModuleMgr ? pModuleMgr : GETMODULEMGR());

CFeaturePassThrough::CFeaturePassThrough(void)
{
}

CFeaturePassThrough::~CFeaturePassThrough(void)
{
}

SYMRESULT CFeaturePassThrough::GetFeaturePassThruObject(REFSYMOBJECT_ID oid, REFSYMINTERFACE_ID iid, void** ppvObject, cc::IString* psOwner, ::cc::IModuleLifetimeMgr* pModuleMgr, DWORD& dwDisposition)
{
	if(NULL == ppvObject)
	{
		CCTRCTXE0(_T("ppvObject is NULL"));
		return SYMERR_INVALIDARG;
	}

	*ppvObject = NULL;

	dwDisposition = IFeaturePassThru::DISPOSITION_CONTAINER;
	DEFAULT_FACTORY(CAdminCheckFeature, iid);

	return SYM_OK;
}