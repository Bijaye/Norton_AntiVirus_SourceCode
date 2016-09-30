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

#include "AvSubmission.h"
#include "Allocator.h"

namespace AVSubmit {

class CAvSubmissionSample : public CAvSubmission, public ::mem::CAllocator<CAvSubmissionSample>
{
public:
	CAvSubmissionSample(void);
	virtual ~CAvSubmissionSample(void);

	virtual bool GetObjectId(SYMOBJECT_ID& oid) const throw();
	virtual HRESULT GetTypeId(GUID& guid) const throw();

protected:
	virtual HRESULT LoadAttributes() throw();
	virtual HRESULT IsSubmittable(cc::IStream* pStream) const throw();

	HRESULT SetClientScanInfo(cc::IKeyValueCollection*) throw();
	HRESULT SetFileInfo(cc::IKeyValueCollection*) throw();
	HRESULT GetFileInfo(ccEraser::IRemediationAction*, const wchar_t*, cc::IKeyValueCollection*) throw();
	HRESULT GetHueristicData(cc::IKeyValueCollection*) throw();
	HRESULT GetRegistryData(cc::IKeyValueCollection*) throw();
	HRESULT GetCustomerData(cc::IKeyValueCollection*) throw();

private:

	CAvSubmissionSample(const CAvSubmissionSample&);
	const CAvSubmissionSample& operator = (const CAvSubmissionSample&);
};



}
