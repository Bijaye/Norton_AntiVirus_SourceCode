////////////////////////
//
// PROPRIETARY / CONFIDENTIAL.
// Use of this product is subject to license terms.
// Copyright © 2006 Symantec Corporation.
// All rights reserved.
//
/////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "ccUnitTestCaseImpl.h"

using namespace ccLib;
using namespace ccUnit;

class CTestScanTaskCollection:
	public ccUnit::CTestCaseImpl
{
public:
	CTestScanTaskCollection(void);
	~CTestScanTaskCollection(void);

	// ITestCase
	virtual bool Initialize(ITestHost* pTestHost) throw();
	virtual bool Execute() throw();

	// Test case
	bool DeleteAllBuiltInScanTasks();
};
