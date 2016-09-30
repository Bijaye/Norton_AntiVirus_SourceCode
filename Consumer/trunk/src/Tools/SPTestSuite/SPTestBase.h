////////////////////////
//
// PROPRIETARY / CONFIDENTIAL.
// Use of this product is subject to license terms.
// Copyright © 2006 Symantec Corporation.
// All rights reserved.
//
/////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#define _WIN32_DCOM
#ifdef WIN32
    #pragma warning ( disable : 4250 )
    #pragma warning( disable : 4290 )
#endif

#include "stdafx.h"
#include "ccUnitTestCaseImpl.h"
#include "ccString.h"
#include "ccLibStd.h"


class CSPTestBase : public ccUnit::CTestCaseImpl
{
public:

	CSPTestBase (void);
	virtual ~CSPTestBase (void);

// ITestCase
	virtual bool Initialize(ccUnit::ITestHost *pTestHost) throw();
	virtual bool Execute() throw();

// Test Cases
	bool InitializeDLL();
};
