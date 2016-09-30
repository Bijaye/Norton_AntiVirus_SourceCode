// /////////////////////////////////////////////////////////////////////////////////////////////////
// Symantec copyright header start
// //////////////////////
//
// PROPRIETARY / CONFIDENTIAL.
// Use of this product is subject to license terms.
// Copyright © 2006 Symantec Corporation.
// All rights reserved.
//
// /////////////////////////////////////////////////////////////////////////////////////////////////
// Symantec copyright header stop
// //////////////////////
#pragma once

#include "AVSubmissionInterface.h"
#include "SubmissionBase.h"
#include "Allocator.h"
#include "SAVQuarantineExtractor.h"

class IQuarantineItem2;

namespace SAVSubmission {

class CSAVQuarantineSubmission :
	public CSubmissionBase,
	public mem::CAllocator<CSAVQuarantineSubmission, ccScanw::IScannerw*>
{
public:
	/* When constructing a submission we need the scanner. But when we're deserializing the submission
	* within the Submission Engine we need to support creating an instance using the default constructor
	* in which case a scanner is not needed.
	*/
	CSAVQuarantineSubmission();
	CSAVQuarantineSubmission(ccScanw::IScannerw* pScanner);

	virtual ~CSAVQuarantineSubmission(void);

	virtual HRESULT GetTypeId(GUID& typeId) const throw();
	virtual bool GetObjectId(SYMOBJECT_ID&) const throw();
	// Logically this should be "const IQuarantineItem2* pQuarantineItem" but IQuarantineItem2 doesn't use const appropriately
	virtual HRESULT Initialize(IQuarantineItem2* pQuarantineItem) throw();

	//IAVSubmission
	virtual HRESULT GetAnomaly(ccEraser::IAnomaly*& pAnomaly) const throw();
	virtual HRESULT GetRemediationCount(size_t& nSize) const throw();
	virtual HRESULT GetRemediation(size_t nIndex, ccEraser::IRemediationAction*& pRemediation, cc::IStream*& pStream) const throw();

protected:

private:
	CSAVQuarantineSubmission(const CSAVQuarantineSubmission&);
	const CSAVQuarantineSubmission& operator = (const CSAVQuarantineSubmission&);
	bool operator== (const CSAVQuarantineSubmission&);

	IQuarantineItem2* m_pQuarantineItem;
	ccEraser::IAnomalyConstPtr m_ptrAnomaly;
	CSAVQuarantineExtractor m_oQuarantineExtractor;
};

// This comes from the consumer team's AVSubmit project.
// TODO:JJM: It really should be put in a header file of the SubSDK.
DEFINE_GUID(TYPEID_AV_SAMPLE, 0xd97ba956, 0x7a75, 0x4108, 0x8d, 0xcb, 0xe1, 0xd3, 0xad, 0x6b, 0x55, 0x8);

} //namespace SAVSubmission
