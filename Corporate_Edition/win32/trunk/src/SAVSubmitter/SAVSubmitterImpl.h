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

#include "SAVSubmitterInterface.h"
#include "SubmissionEngineInterface.h"
#include "ccStringInterface.h"

class IQuarantineItem2;

namespace SAVSubmission {

class CSAVSubmitterImpl :
	public ISAVSubmitter,
	public ISymBaseImpl<CSymThreadSafeRefCount>
{
public:
	CSAVSubmitterImpl(void);
	virtual ~CSAVSubmitterImpl(void);

	SYM_INTERFACE_MAP_BEGIN()
		SYM_INTERFACE_ENTRY(IID_SAVSubmitter, ISAVSubmitter)
	SYM_INTERFACE_MAP_END()

	// Logically this should be "const IQuarantineItem2* pQuarantineItem" but IQuarantineItem2 doesn't use const appropriately
	virtual HRESULT SubmitQuarantinedSample(IQuarantineItem2* pQuarantineItem, ccScanw::IScannerw* pScanner) throw();
	virtual HRESULT SubmitAVDetection(const ccEraser::IAnomaly* pAnomaly, ccScanw::IScannerw* pScanner) throw();
	virtual HRESULT SubmitCOHSample(const ccEraser::IAnomaly* pAnomaly, ccScanw::IScannerw* pScanner) throw();

protected:
	HRESULT Submit(clfs::ISubmission* pSubmission) const throw();

	HRESULT GetSubmissionEngine(clfs::ISubmissionEnginePtr& ptrEngine) const throw();

	CSAVSubmitterImpl(const CSAVSubmitterImpl&);
	const CSAVSubmitterImpl& operator = (const CSAVSubmitterImpl&);
	bool operator== (const CSAVSubmitterImpl&);

private:
	mutable clfs::ISubmissionEnginePtr m_ptrSubmissionEngine;
};

} //namespace SAVSubmission
