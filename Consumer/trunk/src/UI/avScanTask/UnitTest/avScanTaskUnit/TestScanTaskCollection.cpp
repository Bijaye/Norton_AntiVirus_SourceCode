////////////////////////
//
// PROPRIETARY / CONFIDENTIAL.
// Use of this product is subject to license terms.
// Copyright © 2006 Symantec Corporation.
// All rights reserved.
//
/////////////////////////////////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "hrx.h"
#include "TestScanTaskCollection.h"
#include "ccCoinitialize.h"
#include "ScanTaskLoader.h"

using namespace ccLib;
using namespace avScanTask;

//-----------------------------------------------------------------------------------------------------------
CTestScanTaskCollection::CTestScanTaskCollection(void):
ccUnit::CTestCaseImpl(L"ScanTaskUITest",
					  L"ScanTaskUITest - Get alert mode")
{
}

//-----------------------------------------------------------------------------------------------------------
CTestScanTaskCollection::~CTestScanTaskCollection(void)
{
}

//-----------------------------------------------------------------------------------------------------------
bool CTestScanTaskCollection::Initialize(ITestHost* pTestHost) throw()
{
	bool bInit = __super::Initialize(pTestHost);
	if(bInit)
	{
		// TODO: Do some initialization here

	}

	return bInit;
}

//-----------------------------------------------------------------------------------------------------------
bool CTestScanTaskCollection::Execute() throw()
{
	bool bExec = false;

	for(;;)
	{
		CCUNIT_TEST_CONDITION_BREAK(DeleteAllBuiltInScanTasks())
			bExec = true;
		break;
	}

	return bExec;
}

//-----------------------------------------------------------------------------------------------------------
bool CTestScanTaskCollection::DeleteAllBuiltInScanTasks()
{
	STAHLSOFT_HRX_TRY(hr)
	{
		// Initialize COM
		CCoInitialize coInit;
		hrx << coInit.CoInitialize(CCoInitialize::eMTAModel);

		// Create ScanTaskCollection object
		ScanTaskCollection_Loader loader;
		IScanTaskCollectionPtr spScanTaskCollection;
		hrx << (SYM_FAILED(loader.CreateObject(&spScanTaskCollection))? E_FAIL : S_OK);

		// Delete all built-in tasks
		hrx << spScanTaskCollection->DeleteAll();
	}
	STAHLSOFT_HRX_CATCH_ALL_NO_RETURN(hr)

	return (SUCCEEDED(hr)? true : false);
}
