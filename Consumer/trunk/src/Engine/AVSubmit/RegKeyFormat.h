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

#include "ccEraserInterface.h"
#include "ccStreamInterface.h"

#include <sstream>

namespace AVSubmit {
namespace utils {

class CRegKeyFormat
{
public:
	CRegKeyFormat(void);
	virtual ~CRegKeyFormat(void);

	virtual HRESULT AddObject(ccEraser::IRemediationAction* pAction, cc::IStream* pUndo) throw();
	virtual HRESULT ToString(cc::IString*& pString) throw();

protected:
	std::ostringstream m_oss;
};


}
}