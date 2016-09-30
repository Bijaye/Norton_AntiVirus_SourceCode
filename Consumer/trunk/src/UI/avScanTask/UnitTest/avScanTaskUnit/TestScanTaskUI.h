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

class CTestScanTaskUI:
	public ccUnit::CTestCaseImpl
{
public:
	CTestScanTaskUI(void);
	~CTestScanTaskUI(void);

	// ITestCase
	virtual bool Initialize(ITestHost* pTestHost) throw();
	virtual bool Execute() throw();

	// Test case
	bool LaunchScanTaskUI();
};
