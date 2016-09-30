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
#include "TestScanTaskUI.h"
#include "ccCoinitialize.h"
#include "ScanTaskLoader.h"

using namespace ccLib;
using namespace avScanTask;

//-----------------------------------------------------------------------------------------------------------
CTestScanTaskUI::CTestScanTaskUI(void):
ccUnit::CTestCaseImpl(L"ScanTaskUITest",
					  L"ScanTaskUITest - Get alert mode")
{
}

//-----------------------------------------------------------------------------------------------------------
CTestScanTaskUI::~CTestScanTaskUI(void)
{
}

//-----------------------------------------------------------------------------------------------------------
bool CTestScanTaskUI::Initialize(ITestHost* pTestHost) throw()
{
	bool bInit = __super::Initialize(pTestHost);
	if(bInit)
	{
		// TODO: Do some initialization here

	}

	return bInit;
}

//-----------------------------------------------------------------------------------------------------------
bool CTestScanTaskUI::Execute() throw()
{
	bool bExec = false;

	for(;;)
	{
		CCUNIT_TEST_CONDITION_BREAK(LaunchScanTaskUI())
			bExec = true;
		break;
	}

	return bExec;
}

//-----------------------------------------------------------------------------------------------------------
bool CTestScanTaskUI::LaunchScanTaskUI()
{
	STAHLSOFT_HRX_TRY(hr)
	{
		// Initialize COM
		CCoInitialize coInit;
		hrx << coInit.CoInitialize(CCoInitialize::eMTAModel);

		ScanTaskUI_Loader loader;
		IScanTaskUIPtr spScanTaskUI;
		SYMRESULT sr = SYM_OK;
		
		sr = loader.CreateObject(&spScanTaskUI);
		if(SYM_FAILED(sr))
		{
			hrx << E_FAIL;
		}

		hrx << spScanTaskUI->DisplayUI(GetDesktopWindow());
	}
	STAHLSOFT_HRX_CATCH_ALL_NO_RETURN(hr)

	return (SUCCEEDED(hr)? true : false);
}
