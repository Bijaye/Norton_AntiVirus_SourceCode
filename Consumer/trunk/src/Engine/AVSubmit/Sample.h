////////////////////////
//
// PROPRIETARY / CONFIDENTIAL.
// Use of this product is subject to license terms.
// Copyright © 2006 Symantec Corporation.
// All rights reserved.
//
/////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "SubmissionBase.h"

namespace AVSubmit {

class CSample :
	public CSubmissionBase,
	public mem::CAllocator<CSample>
{
public:
	CSample(void);
	~CSample(void);

	virtual HRESULT GetTypeId(GUID& typeId) const throw();
	virtual bool GetObjectId(SYMOBJECT_ID&) const throw();

protected:
	//virtual HRESULT SetAVCompressed() throw();
	virtual HRESULT LoadSubmissionData() throw();
	virtual bool MustBePresent() const throw() {return true;}

	HRESULT LoadFromAnomaly() throw();


private:

	bool m_bLoadingFromAnomaly;

	CSample(const CSample&);
	const CSample& operator = (const CSample&);
};

// {D97BA956-7A75-4108-8DCB-E1D3AD6B5508}
DEFINE_GUID(TYPEID_AV_SAMPLE, 0xd97ba956, 0x7a75, 0x4108, 0x8d, 0xcb, 0xe1, 0xd3, 0xad, 0x6b, 0x55, 0x8);

} //namespace AVSubmit