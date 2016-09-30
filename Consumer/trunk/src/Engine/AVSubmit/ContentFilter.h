////////////////////////
//
// PROPRIETARY / CONFIDENTIAL.
// Use of this product is subject to license terms.
// Copyright © 2006 Symantec Corporation.
// All rights reserved.
//
/////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "AVSubmissionInterface.h"
#include "ccString.h"

namespace AVSubmit {

class CContentFilter
{
protected:
	CContentFilter(void);
	virtual ~CContentFilter(void);

	//IContentFilter
	HRESULT FilterContent(const ISymBase* pIn, ISymBase*& pOut) throw();

private:
	CContentFilter(const CContentFilter&);
	const CContentFilter& operator = (const CContentFilter&);
};

#define IMPLEMENT_CONTENTFILTER(BaseClass)											\
	virtual HRESULT FilterContent(const ISymBase* pIn, ISymBase*& pOut) throw()		\
	{																				\
		return BaseClass::FilterContent(pIn, pOut);									\
	}

}
