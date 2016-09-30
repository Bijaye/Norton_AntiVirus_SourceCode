// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright (c) 2006 Symantec Corporation. All rights reserved.
//
// DummyQuarantineSample.h: interface for a hard coded implementation of IQuarantineItem2.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_DUMMYANOMALY_H__B0B2D703_E141_11D2_A455_00A02438707D__INCLUDED_)
#define AFX_DUMMYANOMALY_H__B0B2D703_E141_11D2_A455_00A02438707D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "SymInterface.h"
#include "ccEraserInterface.h"
#include "ccSymKeyValueCollection.h"
#include "ccSerializeInterface.h"
#include "ccSymSerialize.h"
#include "tstring.h"

class CDummyFileRemediationAction :
	public ccEraser::IRemediationAction,
	public cc::ISerialize,
	public ccSym::CSerialize,
	public ISymBaseImpl<CSymThreadSafeRefCount>
{
public:
    // Construction
	CDummyFileRemediationAction();
	virtual ~CDummyFileRemediationAction();

	HRESULT Initialize();

	SYM_INTERFACE_MAP_BEGIN()
		SYM_INTERFACE_ENTRY(ccEraser::IID_ActionBase, ccEraser::IActionBase)
		SYM_INTERFACE_ENTRY(ccEraser::IID_RemediationAction, ccEraser::IRemediationAction)
		SYM_INTERFACE_ENTRY(cc::IID_Serialize, cc::ISerialize)
	SYM_INTERFACE_MAP_END()

	// ccEraser::IActionBase
	virtual ccEraser::eResult GetType(ccEraser::eObjectType& type) const throw();
    virtual ccEraser::eResult GetUserData(cc::IKeyValueCollection*& pUserData) const throw();    
    virtual ccEraser::eResult GetProperties(const cc::IKeyValueCollection*& pProps) const throw();
    virtual ccEraser::eResult GetProperties(cc::IKeyValueCollection*& pProps) throw();
    virtual ccEraser::eResult GetDescription(cc::IString*& pDescription) const throw();
    virtual ccEraser::eResult GetType(cc::IString*& pType) const throw();
    virtual ccEraser::eResult GetOperation(cc::IString*& pOperation) const throw();
    virtual ccEraser::eResult GetOperands(cc::IString*& pOperands) const throw();

	// ccEraser::IRemediationAction
	virtual ccEraser::eResult GetUndoInformation(cc::IStream* pUndoInformation) const throw();
    virtual ccEraser::eResult SaveUndoInformation(const cc::IString* sDirectory, cc::IStream* pUndoInformation) const throw();
    virtual ccEraser::eResult SaveUndoInformation(const cc::IString* sDirectory, const cc::IString* sFileName, cc::IStream* pUndoInformation) const throw();
    virtual ccEraser::eResult SupportsUndo(bool& bSupportsUndo) const throw();
    virtual ccEraser::eResult IsPresent(bool& bIsPresent) const throw();
    virtual ccEraser::eResult Remediate() throw();
    virtual ccEraser::eResult Undo(cc::IStream* pUndoInformation) throw();

	//ISerialize
	CCSYM_IMPLEMENT_SERIALIZE(ccSym::CSerialize);
	//ccSym::CSerialize
	virtual bool Save(ccLib::CArchive&) const throw();
	virtual bool Load(ccLib::CArchive&) throw();
	virtual bool GetObjectId(SYMOBJECT_ID& id) const throw();

protected:
	CDummyFileRemediationAction(const CDummyFileRemediationAction&);
	CDummyFileRemediationAction& operator=(const CDummyFileRemediationAction&);

	ccEraser::eResult SetStringW(cc::IString*& pStr, const WCHAR* wzStr) const throw();

	HRESULT SetStringProp(ccEraser::IRemediationAction::Property eProp, const WCHAR* tzString);

private:
	static const DWORD ARCHIVE_VERSION = 1;

	cc::IKeyValueCollectionPtr	m_ptrProps;
};

SYM_DEFINE_OBJECT_ID(DUMMY_FILE_REMEDIATION_ACTION_OBJECT_ID, 0x69adbd29, 0xcfb, 0x464e, 0xaa, 0x70, 0x1, 0x3, 0x8d, 0xf2, 0x4b, 0x38);


class CDummyRemediationActionList : 
	public ccEraser::IRemediationActionList,
	public cc::ISerialize,
	public ccSym::CSerialize,
	public ISymBaseImpl<CSymThreadSafeRefCount>
{
    // The IRemediationActionList implementation will also implement the ISerialize interface
public:
    // Construction
	CDummyRemediationActionList();
	virtual ~CDummyRemediationActionList();

	HRESULT Initialize();

	SYM_INTERFACE_MAP_BEGIN()
		SYM_INTERFACE_ENTRY(ccEraser::IID_RemediationActionList, ccEraser::IRemediationActionList)
		SYM_INTERFACE_ENTRY(cc::IID_Serialize, cc::ISerialize)
	SYM_INTERFACE_MAP_END()

	// ccEraser::IRemediationActionList
	virtual ccEraser::eResult Add(ccEraser::IRemediationAction* pAction) throw();
	virtual ccEraser::eResult Remove(size_t nItem) throw();
	virtual ccEraser::eResult GetCount(size_t& dwCount) const throw();
	virtual ccEraser::eResult GetItem(size_t dwItem, ccEraser::IRemediationAction*& pAction) throw();
	virtual ccEraser::eResult Merge(const ccEraser::IRemediationActionList* pOther) throw();

	//ISerialize
	CCSYM_IMPLEMENT_SERIALIZE(ccSym::CSerialize);
	//ccSym::CSerialize
	virtual bool Save(ccLib::CArchive&) const throw();
	virtual bool Load(ccLib::CArchive&) throw();
	virtual bool GetObjectId(SYMOBJECT_ID& id) const throw();

protected:
	CDummyRemediationActionList(const CDummyRemediationActionList&);
	CDummyRemediationActionList& operator=(const CDummyRemediationActionList&);

	bool IsInited() const	{	return (m_ptrCollection.m_p != NULL);	}

private:
	static const DWORD ARCHIVE_VERSION = 1;

	cc::IIndexValueCollectionPtr m_ptrCollection;
};

SYM_DEFINE_OBJECT_ID(DUMMY_REMEDIATION_ACTION_LIST_OBJECT_ID, 0x3db6107, 0xbec6, 0x4361, 0xa0, 0xf1, 0x66, 0x4b, 0x89, 0xba, 0x3e, 0xea);


class CDummyAnomaly :
	public ccEraser::IAnomaly,
	public cc::ISerialize,
	public ccSym::CSerialize,
	public ISymBaseImpl<CSymThreadSafeRefCount>
{
public:
	typedef enum enumAnomalyType { eSimpleAVAnomaly, eSimpleCOHAnomaly };

    // Construction
	CDummyAnomaly(enumAnomalyType eAnomalyType);
	virtual ~CDummyAnomaly();

	HRESULT Initialize();

	SYM_INTERFACE_MAP_BEGIN()
		SYM_INTERFACE_ENTRY(ccEraser::IID_Anomaly, ccEraser::IAnomaly)
		SYM_INTERFACE_ENTRY(cc::IID_Serialize, cc::ISerialize)
	SYM_INTERFACE_MAP_END()

	// ccEraser::IAnomaly
	virtual ccEraser::eResult GetProperties(const cc::IKeyValueCollection*& pProps) const throw();
    virtual ccEraser::eResult GetProperties(cc::IKeyValueCollection*& pProps) throw();

    // The Actions must be called in ascending order and
    // the Undo in descending order
    virtual ccEraser::eResult GetRemediationActions(ccEraser::IRemediationActionList*& pRemediations) throw();
    virtual ccEraser::eResult GetDetectionActions(ccEraser::IDetectionActionList*& pDetections) throw();

    virtual ccEraser::eResult SetRemediationActions(const ccEraser::IRemediationActionList* pRemediations) throw();
    virtual ccEraser::eResult SetDetectionActions(const ccEraser::IDetectionActionList* pDetections) throw();

	//ISerialize
	CCSYM_IMPLEMENT_SERIALIZE(ccSym::CSerialize);
	//ccSym::CSerialize
	virtual bool Save(ccLib::CArchive&) const throw();
	virtual bool Load(ccLib::CArchive&) throw();
	virtual bool GetObjectId(SYMOBJECT_ID& id) const throw();

protected:
	CDummyAnomaly();
	CDummyAnomaly(const CDummyAnomaly&);
	CDummyAnomaly& operator=(const CDummyAnomaly&);

	HRESULT SetStringProp(ccEraser::IAnomaly::Property eProp, const char* tzString);
	HRESULT SetStringProp(ccEraser::IAnomaly::Property eProp, const WCHAR* tzString);

private:
	static const DWORD ARCHIVE_VERSION = 1;

	enumAnomalyType						m_eAnomalyType;
	cc::IKeyValueCollectionPtr			m_ptrProps;
	ccEraser::IRemediationActionListPtr	m_ptrRemediationActions;
};

SYM_DEFINE_OBJECT_ID(DUMMY_ANOMALY_OBJECT_ID, 0xc7290130, 0x4d27, 0x4bd5, 0x90, 0x2b, 0x72, 0x70, 0xc8, 0xad, 0xdd, 0xb4);


// {A770477B-FF95-4474-A79F-09CAF618D7D0}
DEFINE_GUID(DUMMYANOMALYGUID, 0xa770477b, 0xff95, 0x4474, 0xa7, 0x9f, 0x9, 0xca, 0xf6, 0x18, 0xd7, 0xd0);

#endif // !defined(AFX_DUMMYANOMALY_H__B0B2D703_E141_11D2_A455_00A02438707D__INCLUDED_)
