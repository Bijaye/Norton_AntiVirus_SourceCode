////////////////////////
//
// PROPRIETARY / CONFIDENTIAL.
// Use of this product is subject to license terms.
// Copyright © 2006 Symantec Corporation.
// All rights reserved.
//
/////////////////////////////////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include ".\testsuite.h"
#include "SampleTestCases.h"
#include "FilterUnitTest.h"

using namespace ccLib;

CCUNIT_TEST_SUITE_MAP_BEGIN(CTestSuite, L"AVSubmission Module Test Suite", L"Unit Test suite for NAV Submission Engine Integration Module")
	CCUNIT_TEST_SUITE_MAP_ENTRY(SingleSampleNoUndo)		//1
	CCUNIT_TEST_SUITE_MAP_ENTRY(SingleSampleUndo)		//2
	CCUNIT_TEST_SUITE_MAP_ENTRY(MultiSampleNoUndo)		//3
	CCUNIT_TEST_SUITE_MAP_ENTRY(MultiSampleUndo)		//4
	CCUNIT_TEST_SUITE_MAP_ENTRY(SingleDetection)		//5
	CCUNIT_TEST_SUITE_MAP_ENTRY(SingleDetectionUndo)	//6
	CCUNIT_TEST_SUITE_MAP_ENTRY(CSFSample)				//7
	CCUNIT_TEST_SUITE_MAP_ENTRY(COHSample)				//8
	CCUNIT_TEST_SUITE_MAP_ENTRY(CFilterUnitTest)		//9
	CCUNIT_TEST_SUITE_MAP_ENTRY(OLESample)				//10
	CCUNIT_TEST_SUITE_MAP_ENTRY(ManualSample)			//11
	CCUNIT_TEST_SUITE_MAP_ENTRY(AnomalyOnlySample)		//12
	CCUNIT_TEST_SUITE_MAP_ENTRY(OEHTest)				//13
	CCUNIT_TEST_SUITE_MAP_ENTRY(LoopingSingleSample)	//14
CCUNIT_TEST_SUITE_MAP_END()