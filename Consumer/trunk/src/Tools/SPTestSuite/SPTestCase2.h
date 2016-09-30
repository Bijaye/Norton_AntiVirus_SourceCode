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
#include "PolicyEventSubscriber.h"
#include "SymProtectEventTypes.h"

class CSPTestCase2 : public CSPTestBase,
					public ISymProtectPolicyEventCallback
{

public:
	CSPTestCase2 (void);
	virtual ~CSPTestCase2 (void);

// ITestCase
	virtual bool Initialize(ccUnit::ITestHost *pTestHost) throw();
	virtual bool Execute() throw();

// Policy event callback
	void HandleSymProtectEvent(const SymProtectEvt::CPolicyEventEx* const pEvent);
	CEventTest m_EventTest;

private:
	bool SpPolicyEvent(const SymProtectEvt::CPolicyEventEx* const pEvent);
	bool StartSymProtect();
	bool RunProcess();

	CSymStaticRefCount<CSymProtectPolicyEventSubscriber>    m_PolicyEvents;
	//CFile m_file;
	//CFileStatus m_filestatus;
};
