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
#include "SPTestBase.h"
#include "CEventTest.h"

class CSPTestCase1 : public CSPTestBase
{

public:
	CSPTestCase1 (void);
	virtual ~CSPTestCase1 (void);

// ITestCase
	virtual bool Initialize(ccUnit::ITestHost *pTestHost) throw();
	virtual bool Execute() throw();
	bool StopSP();
	bool StartSP();
	bool SymProtectStatus();
	bool GetLocationCount();
	bool FileProtectionEvents();
	bool RegistryProtectionEvents();
	bool ProcessProtectionEvents();
	bool NamedObjectProtection();
	bool RefreshEvent();
	bool SettingValidate();
	bool ConfigUpdate();

	CEventTest m_EventTest;
private:
};
