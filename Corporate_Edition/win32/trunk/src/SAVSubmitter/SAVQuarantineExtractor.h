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

#include <vector>

#include "Allocator.h"
#include "ccEraserInterface.h"

class IQuarantineItem2;

namespace ccEraser
{
	class IAnomaly;
	class IRemediationAction;
};

namespace cc
{
	class IStream;
};

namespace SAVSubmission {

class CSAVQuarantineExtractor :
	public mem::CAllocator<CSAVQuarantineExtractor, IQuarantineItem2*>
{
public:
	/* When constructing a submission we need the quarantine item. But when we're deserializing the submission
	* within the Submission Engine we need to support creating an instance using the default constructor
	* in which case a quarantine item is not needed.
	*/
	CSAVQuarantineExtractor();
	CSAVQuarantineExtractor(IQuarantineItem2* pQuarItem);

	virtual ~CSAVQuarantineExtractor(void);

	// Logically this should be "const IQuarantineItem2* pQuarantineItem" but IQuarantineItem2 doesn't use const appropriately
	virtual HRESULT Initialize(IQuarantineItem2* pQuarItem) throw();

	// should be the same as IAVSubmission
	virtual HRESULT GetAnomaly(ccEraser::IAnomaly*& pAnomaly) const throw();
	virtual HRESULT GetRemediationCount(size_t& nSize) const throw();
	virtual HRESULT GetRemediation(size_t nIndex, ccEraser::IRemediationAction*& pRemediation, cc::IStream*& pStream) const throw();

protected:

	struct Remediation {
		typedef std::vector<Remediation> List;
		cc::IStreamPtr pStream;
		ccEraser::IRemediationActionPtr pRemediation;
		GUID guidUndo;
		Remediation() : guidUndo(GUID_NULL) {}
	};

	struct Data {
		Remediation::List list;
		ccEraser::IAnomalyPtr pAnomaly;
		bool bInit;
		Data() : bInit(false) {}
		void reset()
		{
			list.clear();
			pAnomaly.Release();
			bInit = false;
		}
	};

private:
	CSAVQuarantineExtractor(const CSAVQuarantineExtractor&);
	const CSAVQuarantineExtractor& operator = (const CSAVQuarantineExtractor&);
	bool operator== (const CSAVQuarantineExtractor&);

	ccEraser::IAnomalyConstPtr m_ptrAnomaly;
	IQuarantineItem2* m_pQuarItem;
	Data m_data;
};

} //namespace SAVSubmission
