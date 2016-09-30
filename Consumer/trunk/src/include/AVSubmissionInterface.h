////////////////////////
//
// PROPRIETARY / CONFIDENTIAL.
// Use of this product is subject to license terms.
// Copyright © 2006 Symantec Corporation.
// All rights reserved.
//
/////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "ccKeyValueCollectionInterface.h"
#include "ccIndexValueCollectionInterface.h"
#include "ccStringInterface.h"
#include "ccStreamInterface.h"

namespace AVModule {
	class IAVMapBase;
}

namespace ccEraser {
	class IAnomaly;
	class IRemediationAction;
}

namespace AVSubmit {

class IAVSubmissionManager : public ISymBase
{
public:

	enum Mode
	{
		eSynchronous,
		eAsynchronous,
	};

	enum SubmissionType
	{
		eSample = 0x01,
		eDetection = 0x02,
		
		eAll = eSample | eDetection,
	};

	//init internal structures and modus operandi
	virtual HRESULT Initialize(Mode eMode) throw() = 0;

	//Add automatically submitted sample and/or detection from the AVModule
	//Sample / detection will automatically be generated based on the data.
	virtual HRESULT Add(AVModule::IAVMapBase* pData, DWORD dwType = eAll) throw() = 0;

	//Add manual submission.
	// setGuid = QBackup set
	// itemGuid = [optional] QBackup Item within the set
	//				if not specified, the manager will inspect the set for a known type, or assume
	//				all stream data within the set are individual samples...
	virtual HRESULT ManualAdd(REFGUID setGuid, REFGUID itemGuid) throw() = 0;

	//Add a manual submission from threat tracking data
	virtual HRESULT ManualAdd(AVModule::IAVMapBase* pData) throw() = 0;
};

SYM_DEFINE_INTERFACE_ID(IID_AVSubmissionManager, 0x90f09db, 0xc729, 0x49a9, 0xa0, 0x5c, 0x48, 0xd3, 0x46, 0x7a, 0xc3, 0xb9);
TYPEDEF_SYM_POINTERS(IAVSubmissionManager, IID_AVSubmissionManager);

class IAVSubmission : public ISymBase
{
public:
	enum ScanResult
	{
		eUnknown,
		eHueristic,
		eCompleted,
	};
	enum Category
	{
		eCategoryHueristic,
		eCategoryAntiVirus,
	};

	enum AttributeKey
	{
		eDateDetected,		//DWORD (time_t)
		eVID,				//DWORD
		eVirusName,			//IString
		eSetId,				//GUID
		eSetItemId,			//GUID
		eManual,			//bool
		eDefsDate,			//SYSTEMTIME
		eDefsRev,			//DWORD
		eDefsSeq,			//DWORD
		eScanResult,		//DWORD (ScanResult)
		eDateQuarantined,	//SYSTEMTIME
		eSampleCategory,	//DWORD (Category)
		eFilePath,			//IString, only for use internally, generally will not be set
		eThreatCats,		//IIndexValueCollection
		eSubEngine,			//IString
		eParentProcess,		//not used yet
		eExePath,			//not used yet

	};

	virtual HRESULT GetAnomaly(ccEraser::IAnomaly*& pAnomaly) const throw() = 0;
	virtual HRESULT GetRemediationCount(size_t& nSize) const throw() = 0;
	virtual HRESULT GetRemediation(size_t nIndex, ccEraser::IRemediationAction*& pRemediation, cc::IStream*& pData) const throw() = 0;
};

SYM_DEFINE_INTERFACE_ID(IID_AVSubmission, 0xbf471b2e, 0xf4d4, 0x4b29, 0x88, 0x5a, 0xca, 0xa3, 0x79, 0x7, 0x61, 0xe7);
TYPEDEF_SYM_POINTERS(IAVSubmission, IID_AVSubmission);

class IAVSubmission2 : public ISymBase
{
public:
	virtual HRESULT SetSubmitted(bool bSubmitted) throw() = 0;
};

SYM_DEFINE_INTERFACE_ID(IID_AVSubmission2, 0x5a3c8301, 0x4355, 0x4673, 0xa1, 0x17, 0x99, 0xe2, 0x76, 0xff, 0x29, 0x7f);
TYPEDEF_SYM_POINTERS(IAVSubmission2, IID_AVSubmission2);

// {19CB3010-F278-4b86-9BDA-564DE2BAAA46}
SYM_DEFINE_OBJECT_ID(CLSID_AVSampleSubmission, 0x19cb3010, 0xf278, 0x4b86, 0x9b, 0xda, 0x56, 0x4d, 0xe2, 0xba, 0xaa, 0x46);

// {2B251657-0693-4a48-9A55-BEA8200E5924}
SYM_DEFINE_OBJECT_ID(CLSID_AVDetectionSubmission, 0x2b251657, 0x693, 0x4a48, 0x9a, 0x55, 0xbe, 0xa8, 0x20, 0xe, 0x59, 0x24);

class IContentFilter : public ISymBase
{
public:
	virtual HRESULT FilterContent(const ISymBase* pIn, ISymBase*& pOut) throw() = 0;
};

// {56256764-397E-4186-8D29-C43A59277057}
SYM_DEFINE_INTERFACE_ID(IID_ContentFilter, 0x56256764, 0x397e, 0x4186, 0x8d, 0x29, 0xc4, 0x3a, 0x59, 0x27, 0x70, 0x57);
TYPEDEF_SYM_POINTERS(IContentFilter, IID_ContentFilter);


} //namespace AVSubmit