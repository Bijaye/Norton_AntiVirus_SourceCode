////////////////////////
//
// PROPRIETARY / CONFIDENTIAL.
// Use of this product is subject to license terms.
// Copyright © 2006 Symantec Corporation.
// All rights reserved.
//
/////////////////////////////////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "ccExceptionInfo.h"
#include "ScanTaskUITestSuite.h"
#include "TestScanTaskUI.h"
#include "TestScanTaskCollection.h"

using namespace ccLib;

CCUNIT_TEST_SUITE_MAP_BEGIN(CScanTaskUITestSuite,
							L"ScanTaskUITest",
							L"ScanTaskUITest")
							CCUNIT_TEST_SUITE_MAP_ENTRY(CTestScanTaskUI)
							//CCUNIT_TEST_SUITE_MAP_ENTRY(CTestScanTaskCollection)
							CCUNIT_TEST_SUITE_MAP_END()