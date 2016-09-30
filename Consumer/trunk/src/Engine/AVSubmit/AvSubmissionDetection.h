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
#include "allocator.h"

namespace AVSubmit {

class CAvSubmissionDetection : public CAvSubmission, public mem::CAllocator<CAvSubmissionDetection>
{
public:
	CAvSubmissionDetection(void);
	virtual ~CAvSubmissionDetection(void);

	virtual bool GetObjectId(SYMOBJECT_ID& oid) const throw();
	virtual HRESULT GetTypeId(GUID& guid) const throw();
	virtual HRESULT GetSubmissionData(const cc::IKeyValueCollection*, cc::IStream*&) throw();

private:
	CAvSubmissionDetection(const CAvSubmissionDetection&);
	const CAvSubmissionDetection& operator = (const CAvSubmissionDetection&);
};

}

