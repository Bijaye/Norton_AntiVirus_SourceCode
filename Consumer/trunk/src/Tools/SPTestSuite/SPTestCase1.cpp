////////////////////////
//
// PROPRIETARY / CONFIDENTIAL.
// Use of this product is subject to license terms.
// Copyright © 2006 Symantec Corporation.
// All rights reserved.
//
/////////////////////////////////////////////////////////////////////////////////////////////////

/* *******************************************************************************
'	File		: SPTestCase1.cpp
'	Module		: SymProtect
'	Description	: This file has the definitions of the class CSPTestCase1.
'				  The test cases would call the functions in CEventTest to post
'				  events to SPBBCEvt.Dll
'	Author		: Ignatius
'	Date		: 22nd April 2006 
'	Modified	: 
********************************************************************************** */
#include "stdafx.h"
#include "SPTestCase1.h"

using namespace ATL;
#include "time.h"

CSPTestCase1::CSPTestCase1(void)
{

}

CSPTestCase1::~CSPTestCase1(void)
{

}

bool CSPTestCase1::Initialize(ccUnit::ITestHost *pTestHost) throw()
{
	bool bInit = __super::Initialize(pTestHost);	
	return bInit;	
}

bool CSPTestCase1::Execute() throw()
{
	CCUNIT_TEST_CONDITION_RETURN(InitializeDLL());
	CCUNIT_TEST_CONDITION_RETURN(StopSP());
	CCUNIT_TEST_CONDITION_RETURN(StartSP());
	CCUNIT_TEST_CONDITION_RETURN(GetLocationCount());
	CCUNIT_TEST_CONDITION_RETURN(FileProtectionEvents());
	CCUNIT_TEST_CONDITION_RETURN(RegistryProtectionEvents());
	CCUNIT_TEST_CONDITION_RETURN(ProcessProtectionEvents());
	CCUNIT_TEST_CONDITION_RETURN(NamedObjectProtection());
	CCUNIT_TEST_CONDITION_RETURN(RefreshEvent());
	CCUNIT_TEST_CONDITION_RETURN(SettingValidate());
	CCUNIT_TEST_CONDITION_RETURN(GetLocationCount());
	CCUNIT_TEST_CONDITION_RETURN(ConfigUpdate());
    return true;
}

bool CSPTestCase1::StopSP()
{
	if (!m_EventTest.SendStopSymProtect())
	{
		CCTRACEW(_T("%s Failed \n"), _T("SendStopSymProtect"));
		return false;
	}

	return true;
}

bool CSPTestCase1::StartSP()
{
	if (!m_EventTest.SendStartSymProtect())
	{
		CCTRACEW(_T("%s Failed \n"), _T("SendStartSymProtect"));
		return false;
	}

	return true;
}

bool CSPTestCase1::SymProtectStatus()
{
	if (!m_EventTest.SendGetSymProtectStatus())
	{
		CCTRACEW(_T("%s Failed \n"), _T("SymProtectStatus"));
		return false;
	}

	return true;
}

bool CSPTestCase1::GetLocationCount()
{
    if (!m_EventTest.SendGetLocationCount())
	{
		CCTRACEW(_T("%s Failed \n"), _T("GetLocationCount"));
		return false;
	}
	return true;
}

bool CSPTestCase1::FileProtectionEvents()
{
    if (!m_EventTest.SendSetFileProtection(true,false))
	{
		CCTRACEW(_T("%s Failed \n"), _T("GetLocationCount"));
		return false;
	}

	return true;
//    m_EventTest.SendSetFileProtection(true,true)
//    m_EventTest.SendSetFileProtection(false,false)
}

bool CSPTestCase1::RegistryProtectionEvents()
{
    if (!m_EventTest.SendSetRegistryProtection(true,false))
	{
		CCTRACEW(_T("%s Failed \n"), _T("GetLocationCount"));
		return false;
	}
	return true;
    //m_EventTest.SendSetRegistryProtection(true,true)
    //m_EventTest.SendSetRegistryProtection(false,false)
}

bool CSPTestCase1::ProcessProtectionEvents()
{
    if (!m_EventTest.SendSetProcessProtection(true,false))
	{
		CCTRACEW(_T("%s Failed \n"), _T("GetLocationCount"));
		return false;
	}
	return true;
	//m_EventTest.SendSetProcessProtection(true,true)
    //m_EventTest.SendSetProcessProtection(false,false)
}

bool CSPTestCase1::NamedObjectProtection()
{
	if(!m_EventTest.SendSetNamedObjectProtection(true,false))
	{
		CCTRACEW(_T("%s Failed \n"), _T("NamedObjectProtection"));
		return false;
	}
	return true;
	//m_EventTest.SendSetNamedObjectProtection(true,true)
	//m_EventTest.SendSetNamedObjectProtection(false,false)
}

bool CSPTestCase1::RefreshEvent()
{
    if(!m_EventTest.SendForceRefreshEvent())
	{
		CCTRACEW(_T("%s Failed \n"), _T("RefreshEvent"));
		return false;
	}
	return true;
}

bool CSPTestCase1::SettingValidate()
{
	if(!m_EventTest.SendSettingsValidation())
	{
		CCTRACEW(_T("%s Failed \n"), _T("SettingValidate"));
		return false;
	}
	return true;
}

bool CSPTestCase1::ConfigUpdate()
{
	if(!m_EventTest.SendConfigurationUpdate())
	{
		CCTRACEW(_T("%s Failed \n"), _T("ConfigUpdate"));
		return false;
	}
	return true;
}

