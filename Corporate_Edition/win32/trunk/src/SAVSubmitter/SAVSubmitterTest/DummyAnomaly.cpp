// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright (c) 2006 Symantec Corporation. All rights reserved.
//
// DummyQuarantineSample.cpp: implementation of the CDummyQuarantineSample class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "DummyAnomaly.h"
#include "Utils.h"

#include "ccLib.h"
#include "ccSymStringImpl.h"
#include "ccSymKeyValueCollectionImpl.h"
#include "ccSymIndexValueCollectionImpl.h"

#define REF(x) (x)

#define DUMMY_FILE_PATH L"C:\\Program Files\\Dummy Company\\DummyThreatFile.exe"


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CDummyFileRemediationAction::CDummyFileRemediationAction()
{
}

CDummyFileRemediationAction::~CDummyFileRemediationAction()
{
}

HRESULT CDummyFileRemediationAction::Initialize()
{
	HRESULT hResult = S_OK;

	m_ptrProps.Attach(ccSym::CKeyValueCollectionImpl::CreateKeyValueCollectionImpl());
	if (m_ptrProps != NULL)
	{
		m_ptrProps->SetValue(State, (DWORD) NotRemediated);
		m_ptrProps->SetValue(Operation, (DWORD) Delete);
		hResult = SetStringProp(Path, DUMMY_FILE_PATH);
	}
	else
		hResult = E_OUTOFMEMORY;
	return hResult;
}

HRESULT CDummyFileRemediationAction::SetStringProp(ccEraser::IRemediationAction::Property eProp, const WCHAR* tzString)
{
	HRESULT hResult = S_OK;
	cc::IStringPtr ptrTemp;

	ptrTemp.Attach(ccSym::CStringImpl::CreateStringImpl(tzString));
	if (ptrTemp != NULL)
	{
		if (!m_ptrProps->SetValue(eProp, ptrTemp.m_p))
			hResult = E_OUTOFMEMORY;
	}
	else
		hResult = E_OUTOFMEMORY;
	return hResult;
}

ccEraser::eResult CDummyFileRemediationAction::SetStringW(cc::IString*& pStr, const WCHAR* wzStr) const throw()
{
	if (pStr != NULL)
		pStr->Release();
	pStr = ccSym::CStringImpl::CreateStringImpl(wzStr);
	if (pStr != NULL)
		return ccEraser::Success;
	else
		return ccEraser::OutOfMemory;
}

ccEraser::eResult CDummyFileRemediationAction::GetType(ccEraser::eObjectType& type) const throw()
{
	type = ccEraser::FileRemediationActionType;
	return ccEraser::Success;
}

ccEraser::eResult CDummyFileRemediationAction::GetUserData(cc::IKeyValueCollection*& pUserData) const throw()
{
	if (pUserData != NULL)
		pUserData->Release();
	pUserData = NULL;
	return ccEraser::Success;
}

ccEraser::eResult CDummyFileRemediationAction::GetProperties(const cc::IKeyValueCollection*& pProps) const throw()
{
	if (pProps != NULL)
		pProps->Release();
	pProps = m_ptrProps;
	if (pProps != NULL)
		pProps->AddRef();
	return ccEraser::Success;
}

ccEraser::eResult CDummyFileRemediationAction::GetProperties(cc::IKeyValueCollection*& pProps) throw()
{
	if (pProps != NULL)
		pProps->Release();
	pProps = m_ptrProps;
	if (pProps != NULL)
		pProps->AddRef();
	return ccEraser::Success;
}

ccEraser::eResult CDummyFileRemediationAction::GetDescription(cc::IString*& pDescription) const throw()
{
	return SetStringW(pDescription, L"File Remediation, Delete, State: 2, Operation: 1, Path: " DUMMY_FILE_PATH);
}

ccEraser::eResult CDummyFileRemediationAction::GetType(cc::IString*& pType) const throw()
{
	return SetStringW(pType, L"File Remediation");
}

ccEraser::eResult CDummyFileRemediationAction::GetOperation(cc::IString*& pOperation) const throw()
{
	return SetStringW(pOperation, L"Delete");
}

ccEraser::eResult CDummyFileRemediationAction::GetOperands(cc::IString*& pOperands) const throw()
{
	return SetStringW(pOperands, L"State: 2, Operation: 1, Path: " DUMMY_FILE_PATH);
}

ccEraser::eResult CDummyFileRemediationAction::GetUndoInformation(cc::IStream* pUndoInformation) const throw()
{
	return ccEraser::Success;
}

ccEraser::eResult CDummyFileRemediationAction::SaveUndoInformation(const cc::IString* sDirectory, cc::IStream* pUndoInformation) const throw()
{
	return ccEraser::Success;
}

ccEraser::eResult CDummyFileRemediationAction::SaveUndoInformation(const cc::IString* sDirectory, const cc::IString* sFileName, cc::IStream* pUndoInformation) const throw()
{
	return ccEraser::Success;
}

ccEraser::eResult CDummyFileRemediationAction::SupportsUndo(bool& bSupportsUndo) const throw()
{
	bSupportsUndo = true;
	return ccEraser::Success;
}

ccEraser::eResult CDummyFileRemediationAction::IsPresent(bool& bIsPresent) const throw()
{
	bIsPresent = true;
	return ccEraser::Success;
}

ccEraser::eResult CDummyFileRemediationAction::Remediate() throw()
{
	return ccEraser::Success;
}

ccEraser::eResult CDummyFileRemediationAction::Undo(cc::IStream* pUndoInformation) throw()
{
	return ccEraser::Success;
}

bool CDummyFileRemediationAction::Save(ccLib::CArchive& Archive) const throw()
{
	clfs::utils::BoolX brx;

	STAHLSOFT_HRX_TRY(hr)
	{
		Archive << ARCHIVE_VERSION;
		brx << ccSym::CPersist::Write(m_ptrProps, NULL, NULL, &Archive, NULL, NULL);
	}
	catch(ccLib::CArchive::CArchiveException& )
	{
		CCTRCTXE0(_T("Caught ArchiveException"));
		hr = E_FAIL;
	}
	CATCH_NO_RETURN(hr);
	return SUCCEEDED(hr);
}

bool CDummyFileRemediationAction::Load(ccLib::CArchive& Archive) throw()
{
	clfs::utils::BoolX brx;

	STAHLSOFT_HRX_TRY(hr)
	{
		DWORD dwVer = 0;

		Archive >> dwVer;
		if(dwVer == ARCHIVE_VERSION)
		{
			brx << ccSym::CPersist::Read(m_ptrProps, NULL, NULL, &Archive, NULL, NULL);
		}
		else
		{
			CCTRCTXE2(_T("dwVer [%d] != ARCHIVE_VERSION [%d]"), dwVer, ARCHIVE_VERSION);
		}
	}
	catch(ccLib::CArchive::CArchiveException&)
	{
		CCTRCTXE0(_T("Caught Archive Exception"));
		hr = E_FAIL;
	}
	CATCH_NO_RETURN(hr);
	return SUCCEEDED(hr);
}

bool CDummyFileRemediationAction::GetObjectId(SYMOBJECT_ID& id) const throw()
{
	id = DUMMY_FILE_REMEDIATION_ACTION_OBJECT_ID;
	return true;
}

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CDummyRemediationActionList::CDummyRemediationActionList()
{
}

CDummyRemediationActionList::~CDummyRemediationActionList()
{
}

HRESULT CDummyRemediationActionList::Initialize()
{
	HRESULT hResult = S_OK;

	assert(m_ptrCollection == NULL);
	m_ptrCollection.Attach(ccSym::CIndexValueCollectionImpl::CreateIndexValueCollectionImpl());
	if (m_ptrCollection != NULL)
	{
		CDummyFileRemediationAction* pFileAction = NULL;

		try
		{
			pFileAction = new CDummyFileRemediationAction;
		}
		catch (std::bad_alloc&)
		{
		}
		if (pFileAction != NULL)
		{
			if (SUCCEEDED(pFileAction->Initialize()))
			{
				Add(pFileAction);	// Collection takes sole ownership of pFileAction
			}
			else
			{
				delete pFileAction;
				pFileAction = NULL;
			}
		}
		else
			hResult = E_OUTOFMEMORY;
	}
	else
		hResult = E_OUTOFMEMORY;
	return hResult;
}

ccEraser::eResult CDummyRemediationActionList::Add(ccEraser::IRemediationAction* pAction) throw()
{
	if( pAction == NULL )
		return ccEraser::InvalidArgument;
	if( !IsInited() )
		return ccEraser::NotInitialized;

	if( !m_ptrCollection->AddValue(pAction) )
		return ccEraser::Fail;
	return ccEraser::Success;
}

ccEraser::eResult CDummyRemediationActionList::Remove(size_t nItem) throw()
{
	if( !IsInited() )
		return ccEraser::NotInitialized;

	// Make sure the index given was in range
	size_t nMax = 0;
	if( Failed(GetCount(nMax)) )
		return ccEraser::Fail;

	if( nItem+1 > nMax )
		return ccEraser::InvalidArgument;

	// Remove it
	if ( !m_ptrCollection->Remove(nItem))
	{
		CCTRCTXE0( _T("remove value failed") );
		return ccEraser::Fail;
	}
	return ccEraser::Success;
}

ccEraser::eResult CDummyRemediationActionList::GetCount(size_t& dwCount) const throw()
{
	if( !IsInited() )
		return ccEraser::NotInitialized;

	dwCount = m_ptrCollection->GetCount();
	return ccEraser::Success;
}

ccEraser::eResult CDummyRemediationActionList::GetItem(size_t dwItem, ccEraser::IRemediationAction*& pAction) throw()
{
	if (pAction != NULL)
		pAction->Release();
	pAction = NULL;

	if( !IsInited() )
		return ccEraser::NotInitialized;

	// Make sure the index given was in range
	size_t nMax = 0;
	if( Failed(GetCount(nMax)) )
		return ccEraser::Fail;

	if( dwItem+1 > nMax )
		return ccEraser::InvalidArgument;

    if( !m_ptrCollection->GetExists(dwItem) )
		return ccEraser::InvalidArgument;

	ISymBasePtr ptrBase;
    if( !m_ptrCollection->GetValue(dwItem,ptrBase.m_p) )
    {
        CCTRCTXE1( _T("Couldn't get required property, %d"), dwItem );
        return ccEraser::Fail;
    }
	ccEraser::IRemediationActionQIPtr ptrQIRemediationAction(ptrBase);

	if (ptrQIRemediationAction == NULL)
        return ccEraser::Fail;

	pAction = ptrQIRemediationAction.Detach();
    return ccEraser::Success;
}

ccEraser::eResult CDummyRemediationActionList::Merge(const ccEraser::IRemediationActionList* pOther) throw()
{
	return ccEraser::Success;
}

bool CDummyRemediationActionList::Save(ccLib::CArchive& Archive) const throw()
{
	clfs::utils::BoolX brx;

	STAHLSOFT_HRX_TRY(hr)
	{
		Archive << ARCHIVE_VERSION;
		brx << ccSym::CPersist::Write(m_ptrCollection, NULL, NULL, &Archive, NULL, NULL);
	}
	catch(ccLib::CArchive::CArchiveException& )
	{
		CCTRCTXE0(_T("Caught ArchiveException"));
		hr = E_FAIL;
	}
	CATCH_NO_RETURN(hr);
	return SUCCEEDED(hr);
}

bool CDummyRemediationActionList::Load(ccLib::CArchive& Archive) throw()
{
	clfs::utils::BoolX brx;

	STAHLSOFT_HRX_TRY(hr)
	{
		DWORD dwVer = 0;

		Archive >> dwVer;
		if(dwVer == ARCHIVE_VERSION)
		{
			brx << ccSym::CPersist::Read(m_ptrCollection, NULL, NULL, &Archive, NULL, NULL);
		}
		else
		{
			CCTRCTXE2(_T("dwVer [%d] != ARCHIVE_VERSION [%d]"), dwVer, ARCHIVE_VERSION);
		}
	}
	catch(ccLib::CArchive::CArchiveException&)
	{
		CCTRCTXE0(_T("Caught Archive Exception"));
		hr = E_FAIL;
	}
	CATCH_NO_RETURN(hr);
	return SUCCEEDED(hr);
}

bool CDummyRemediationActionList::GetObjectId(SYMOBJECT_ID& id) const throw()
{
	id = DUMMY_REMEDIATION_ACTION_LIST_OBJECT_ID;
	return true;
}


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CDummyAnomaly::CDummyAnomaly(enumAnomalyType eAnomalyType) : 
					m_eAnomalyType(eAnomalyType),
					m_ptrRemediationActions(NULL)
{
}

CDummyAnomaly::~CDummyAnomaly()
{
}

HRESULT CDummyAnomaly::Initialize()
{
	HRESULT hResult = S_OK;

	assert(m_ptrProps == NULL);
	m_ptrProps.Attach(ccSym::CKeyValueCollectionImpl::CreateKeyValueCollectionImpl());
	if (m_ptrProps != NULL)
	{
		m_ptrProps->SetValue(ccEraser::IAnomaly::VID, (DWORD) 0xbeefdead);

		hResult = SetStringProp(ccEraser::IAnomaly::Name, _T("Dummy anomaly for unit testing"));

		WCHAR wszGuid[100];
		StringFromGUID2(DUMMYANOMALYGUID, wszGuid, CCDIMOF(wszGuid));

		if (SUCCEEDED(hResult))
			hResult = SetStringProp(ccEraser::IAnomaly::ID, wszGuid);

		switch(m_eAnomalyType)
		{
		case eSimpleAVAnomaly:
			break;
		case eSimpleCOHAnomaly:
			if (SUCCEEDED(hResult))
				hResult = SetStringProp(ccEraser::IAnomaly::HeuristicEngineVersion, _T("1.2.3.4"));
			break;
		default:
			assert(false);
			break;
		}

		if (SUCCEEDED(hResult))
		{
			CDummyRemediationActionList*	pRemediationActions = NULL;

			assert(m_ptrRemediationActions == NULL);
			try
			{
				pRemediationActions = new CDummyRemediationActionList;
			}
			catch (std::bad_alloc&)
			{
			}
			if (pRemediationActions != NULL)
			{
				hResult = pRemediationActions->Initialize();
				if (FAILED(hResult))
				{
					delete pRemediationActions;
					pRemediationActions = NULL;
				}
			}
			else
				hResult = E_OUTOFMEMORY;
			m_ptrRemediationActions = pRemediationActions;
		}
	}
	else
		hResult = E_OUTOFMEMORY;
	return hResult;
}

ccEraser::eResult CDummyAnomaly::GetProperties(const cc::IKeyValueCollection*& pProps) const throw()
{
	if (pProps != NULL)
		pProps->Release();
	pProps = m_ptrProps;
	if (pProps != NULL)
		pProps->AddRef();
	return ccEraser::Success;
}

ccEraser::eResult CDummyAnomaly::GetProperties(cc::IKeyValueCollection*& pProps) throw()
{
	if (pProps != NULL)
		pProps->Release();
	pProps = m_ptrProps;
	if (pProps != NULL)
		pProps->AddRef();
	return ccEraser::Success;
}

ccEraser::eResult CDummyAnomaly::GetRemediationActions(ccEraser::IRemediationActionList*& pRemediationActions) throw()
{
	if (pRemediationActions != NULL)
	{
		pRemediationActions->Release();
		pRemediationActions = NULL;
	}
	pRemediationActions = m_ptrRemediationActions;
	if (pRemediationActions != NULL)
	{
		pRemediationActions->AddRef();
		return ccEraser::Success;
	}
	else
		return ccEraser::OutOfMemory;
}

ccEraser::eResult CDummyAnomaly::GetDetectionActions(ccEraser::IDetectionActionList*& ) throw()
{
	return ccEraser::Fail;
}

ccEraser::eResult CDummyAnomaly::SetRemediationActions(const ccEraser::IRemediationActionList* ) throw()
{
	return ccEraser::Fail;
}

ccEraser::eResult CDummyAnomaly::SetDetectionActions(const ccEraser::IDetectionActionList* ) throw()
{
	return ccEraser::Fail;
}

HRESULT CDummyAnomaly::SetStringProp(ccEraser::IAnomaly::Property eProp, const char* tzString)
{
	HRESULT hResult = S_OK;
	cc::IStringPtr ptrTemp;

	ptrTemp.Attach(ccSym::CStringImpl::CreateStringImpl(tzString));
	if (ptrTemp != NULL)
	{
		if (!m_ptrProps->SetValue(eProp, ptrTemp.m_p))
			hResult = E_OUTOFMEMORY;
	}
	else
		hResult = E_OUTOFMEMORY;
	return hResult;
}

HRESULT CDummyAnomaly::SetStringProp(ccEraser::IAnomaly::Property eProp, const WCHAR* tzString)
{
	HRESULT hResult = S_OK;
	cc::IStringPtr ptrTemp;

	ptrTemp.Attach(ccSym::CStringImpl::CreateStringImpl(tzString));
	if (ptrTemp != NULL)
	{
		if (!m_ptrProps->SetValue(eProp, ptrTemp.m_p))
			hResult = E_OUTOFMEMORY;
	}
	else
		hResult = E_OUTOFMEMORY;
	return hResult;
}

bool CDummyAnomaly::Save(ccLib::CArchive& Archive) const throw()
{
	clfs::utils::BoolX brx;

	STAHLSOFT_HRX_TRY(hr)
	{
		Archive << ARCHIVE_VERSION;
		Archive << (DWORD)m_eAnomalyType;
		brx << ccSym::CPersist::Write(m_ptrProps, NULL, NULL, &Archive, NULL, NULL);
		brx << ccSym::CPersist::Write(m_ptrRemediationActions, NULL, NULL, &Archive, NULL, NULL);
	}
	catch(ccLib::CArchive::CArchiveException& )
	{
		CCTRCTXE0(_T("Caught ArchiveException"));
		hr = E_FAIL;
	}
	CATCH_NO_RETURN(hr);
	return SUCCEEDED(hr);
}

bool CDummyAnomaly::Load(ccLib::CArchive& Archive) throw()
{
	clfs::utils::BoolX brx;

	STAHLSOFT_HRX_TRY(hr)
	{
		DWORD dwVer = 0;

		Archive >> dwVer;
		if(dwVer == ARCHIVE_VERSION)
		{
			DWORD dwAnomalyType = 0;

			Archive >> dwAnomalyType;
			m_eAnomalyType = (enumAnomalyType)dwAnomalyType;
			brx << ccSym::CPersist::Read(m_ptrProps, NULL, NULL, &Archive, NULL, NULL);
			brx << ccSym::CPersist::Read(m_ptrRemediationActions, NULL, NULL, &Archive, NULL, NULL);
		}
		else
		{
			CCTRCTXE2(_T("dwVer [%d] != ARCHIVE_VERSION [%d]"), dwVer, ARCHIVE_VERSION);
		}
	}
	catch(ccLib::CArchive::CArchiveException&)
	{
		CCTRCTXE0(_T("Caught Archive Exception"));
		hr = E_FAIL;
	}
	CATCH_NO_RETURN(hr);
	return SUCCEEDED(hr);
}

bool CDummyAnomaly::GetObjectId(SYMOBJECT_ID& id) const throw()
{
	id = DUMMY_ANOMALY_OBJECT_ID;
	return true;
}
