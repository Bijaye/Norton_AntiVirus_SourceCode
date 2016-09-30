////////////////////////
//
// PROPRIETARY / CONFIDENTIAL.
// Use of this product is subject to license terms.
// Copyright © 2006 Symantec Corporation.
// All rights reserved.
//
/////////////////////////////////////////////////////////////////////////////////////////////////

/* *******************************************************************************
'	File		: ExcluTestBase.cpp
'	Module		: Exclusions
'	Description	: This file has the definitions of the class CExcluTestBase.
				  CExcluTestBase will handle the Unit test cases for the Exclsuions.
'	Modified By	: Saranya, Ignatius
********************************************************************************** */
#include "stdafx.h"
#include "SPTestBase.h"

//Includes for cc
#include "SymInterface.h"
#include "SymInterfaceLoader.h"
#include "SPSuite.h"

//Include for SymEvent
#include "SymProtectEventsInterface.h"
#include "SymProtectEventTypes.h"
#include "SymProtectEventsLoader.h"

CSymInterfaceLoader m_csil;

using namespace SymProtectEvt;

CPolicyEventExPtr m_PolicyEventPtr;
SymProtectEvt::SymProtectEvents_CEventFactoryEx m_EventFctryEx;

#include "time.h"

CSPTestBase::CSPTestBase(void):ccUnit::CTestCaseImpl 
	(L"SymProtect Integration", L"SymProtect Integration Testing")
{
}

CSPTestBase::~CSPTestBase(void)
{
}

bool CSPTestBase::Initialize(ccUnit::ITestHost *pTestHost) throw()
{
	bool bInit = __super::Initialize(pTestHost);	
	return bInit;	
}

bool CSPTestBase::Execute() throw()
{
    return true;
}

bool CSPTestBase::InitializeDLL()
{
	SYMRESULT smr = SYM_OK;

	if (CoInitialize(NULL) == S_OK)
	{
		CCTRACEW(L"STARTING COM...\n");
	}
	else
	{
		CCTRACEW(L"Can not start COM, exiting...\n");
	}

	//m_EventFctryEx
	if (SYM_OK == m_csil.Initialize(L"SPBBCEvt.dll"))
	{
		CCTRACEW(L"Initialized DLL \n");
	}
	else
	{
		CCTRACEW(L"Can not initialize DLL, exiting... \n");
		return false;
	}	

	return true;
}

