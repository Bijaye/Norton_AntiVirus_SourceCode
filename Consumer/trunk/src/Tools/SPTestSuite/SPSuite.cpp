////////////////////////
//
// PROPRIETARY / CONFIDENTIAL.
// Use of this product is subject to license terms.
// Copyright © 2006 Symantec Corporation.
// All rights reserved.
//
/////////////////////////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"

//#include "ccExceptionInfo.h"
#include "SPSuite.h"
#include "SPTestCase1.h" 
#include "SPTestCase2.h"

using namespace ccLib;

CCUNIT_TEST_SUITE_MAP_BEGIN(CSPTestSuite, _T("SymProtecUnitTest"), _T("Sym Protect Unit Test suite"))
	CCUNIT_TEST_SUITE_MAP_ENTRY(CSPTestCase1)
	//CCUNIT_TEST_SUITE_MAP_ENTRY(CSPTestCase2)
CCUNIT_TEST_SUITE_MAP_END();