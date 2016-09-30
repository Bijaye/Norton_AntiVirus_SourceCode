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

class CDetection : 
	public CSubmissionBase, 
	public mem::CAllocator<CDetection>
{
public:
	CDetection(void);
	virtual ~CDetection(void);

	virtual HRESULT GetTypeId(GUID& typeId) const throw();
	virtual bool GetObjectId(SYMOBJECT_ID&) const throw();

protected:
	virtual HRESULT LoadSubmissionData() throw();
	virtual HRESULT CheckBloodhound(DWORD) const throw();
	virtual bool MustBePresent() const throw() {return false;}
	virtual HRESULT SetDetails() throw();

private:

	CDetection(const CDetection&);
	const CDetection& operator = (const CDetection&);

};

// {6E70C46B-7FD6-4D71-A233-B101E9AD72F2}
DEFINE_GUID(TYPEID_AV_DETECTION, 0x6e70c46b, 0x7fd6, 0x4d71, 0xa2, 0x33, 0xb1, 0x1, 0xe9, 0xad, 0x72, 0xf2);

} //namespace AVSubmit