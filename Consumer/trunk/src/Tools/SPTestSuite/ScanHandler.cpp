////////////////////////
//
// PROPRIETARY / CONFIDENTIAL.
// Use of this product is subject to license terms.
// Copyright © 2006 Symantec Corporation.
// All rights reserved.
//
/////////////////////////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ScanHandler.h"

//Includes for cc
#include "SymInterface.h"
#include "SymInterfaceLoader.h"
//#define SIMON_INITGUID
#include "ccSymStringImpl.h"

#include "ccSettingsInterface.h"
#include "NAVSettingsHelperEx.h"

//Includes for ScanManager
#include "ScanMgrInterface.h"
#include "AVScanManagerLoader.h"
#include "AutoProtectWrapper.h"
#include "Switches.h"
#include "NAVWinterface.h"

#define NAVAPPIDS_BUILDING
#include "NAVAppIDs.h"

AV::AVScanManager_IScanManagerFactory m_ScanManagerLoader;
IScanManager* m_pScanmgrPtr;

CSymInterfaceLoader m_csil1;
//CAutoProtectWrapper m_APWrapper;

//std:: vector <CString> m_strVector;
//std:: vector <std::vector <CString> > m_vVector;

CScanHandler::CScanHandler(void)
{
	m_pScanmgrPtr = NULL;
}

CScanHandler::~CScanHandler(void)
{
	if (m_pScanmgrPtr != NULL)
	{
		m_pScanmgrPtr->Release();
        m_pScanmgrPtr = NULL;
	}
}

bool CScanHandler::InitializeScanMgr()
{
	//Initialize and Load ScanMgr DLL
	if (SYM_OK == m_csil1.Initialize("ScanMgr.dll"))
					CCTRACEW(_T("Initialized DLL "));
	else{
		CCTRACEW(_T("Can not initialize DLL, exiting... "));
		return false;
	}

	// Load up scan manager.
    if (m_pScanmgrPtr == NULL)
    {
        SYMRESULT symRes = SYM_OK;
        if( SYM_FAILED(symRes = m_ScanManagerLoader.CreateObject(&m_pScanmgrPtr)) )
        {
            CCTRACEW(_T("Failed to load the scan manager. SYMRESULT = 0x%X"), symRes);
            return false;
        }
    }
 	return true; 
}

bool CScanHandler::CallScanMgr(ATL::CAtlString strTemp)
{
	SMRESULT Result = SMRESULT_OK;
	NAVWRESULT NavwResult;
	
	// Disable Auto-Protect.
	//	bool bDisableAP = m_APWrapper.DisableAPForThisThread();

	UINT uErrorMode = SetErrorMode(SEM_FAILCRITICALERRORS);
	
	//Initialize ccSettings
	InitOptSetting();

	//Reset the Scan List
	m_pScanmgrPtr->ResetScanList();
	
	//Save the Settings for scan
	ProcessSwitch();

	//Add the appropriate folder or file to scan
	m_pScanmgrPtr->AddItemToScanList(strTemp);

	// Set the Scan name
	m_pScanmgrPtr->SetScanName( "ExclusionTestScan" );

	// Kick off the scan if there are items to scan or the user wants us to
	// scan memory or boot records only.
	
	if (m_pScanmgrPtr->GetScanListItemCount() > 0)
	{
		Result = m_pScanmgrPtr->ScanAndRepair(g_szNavAppIdNAVW32);
	}
	
	// Update the last run time for task files.

//	if (Result == SMRESULT_OK)
//	{
//		UpdateTaskTimes();
//	}

	// Set the infection status depending on how many infections were found/fixed
	if (Result != SMRESULT_OK && Result != SMRESULT_ABORT)
		NavwResult = NAVWRESULT_ERROR;
	else if(m_pScanmgrPtr->GetTotalInfectionsFound() == 0)
		NavwResult = NAVWRESULT_NOINFECTIONS;
	else if(m_pScanmgrPtr->GetTotalInfectionsFound() != m_pScanmgrPtr->GetTotalInfectionsFixed())
		NavwResult = NAVWRESULT_INFECTIONS_REMAIN;
	else
		NavwResult = NAVWRESULT_ALLINFECTIONS_CLEAN;
	
	// Set the infection status depending on how many infections were found/fixed
	if (Result != SMRESULT_OK && Result != SMRESULT_ABORT)
		NavwResult = NAVWRESULT_ERROR;
	else if(m_pScanmgrPtr->GetTotalInfectionsFound() == 0)
		NavwResult = NAVWRESULT_NOINFECTIONS;
	else if(m_pScanmgrPtr->GetTotalInfectionsFound() != m_pScanmgrPtr->GetTotalInfectionsFixed())
		NavwResult = NAVWRESULT_INFECTIONS_REMAIN;
	else
		NavwResult = NAVWRESULT_ALLINFECTIONS_CLEAN;
	
	//Check the total infection count for this scan
	if(m_pScanmgrPtr->GetTotalInfectionsFound() > 0){
		fprintf(stdout,"Total Infection Count: %d /n", m_pScanmgrPtr->GetTotalInfectionsFound());
		//Result = SMRESULT_ERROR_VIRUS_DEFS;
		Result = SMRESULT_OK;
	}

	// Clean-up.
    if( m_pScanmgrPtr != NULL )
    {
        m_pScanmgrPtr->Release();
        m_pScanmgrPtr = NULL;
    }

	// Set the previous error mode.
	SetErrorMode(uErrorMode);

	// Re-enable Auto-Protect.
//	if (bDisableAP == true)
//	{
//		m_APWrapper.EnableAPForThisThread();
//	}

	// It's done!
	if (Result == SMRESULT_OK)
		return true;	
	else
		return false;
}

bool CScanHandler::ProcessSwitch()
{
	
	m_pScanmgrPtr->SetUserInterface(SMUI_NO_UI);
	m_pScanmgrPtr->OverrideSetting(SMSETTING_SCAN_MBR, false);
	m_pScanmgrPtr->OverrideSetting(SMSETTING_SCAN_MEMORY, false);
	m_pScanmgrPtr->OverrideSetting(SMSETTING_SCAN_BOOTRECS, false);
	m_pScanmgrPtr->OverrideSetting(SMSETTING_SCAN_FILES, true);
	m_pScanmgrPtr->OverrideSetting(SMSETTING_RUN_ERASER_SCAN, false);
	m_pScanmgrPtr->OverrideSetting(SMSETTING_SCAN_SUBFOLDERS, true);
	m_pScanmgrPtr->OverrideSetting(SMSETTING_LOG_START_STOP, false);
	m_pScanmgrPtr->SetUserInterface(SMUI_NO_RESULTS);

	return true;
	
}

bool CScanHandler::UnInitializeScanMgr()
{
	//Release ScanMgr
	m_pScanmgrPtr->Release();

 	return true; 
}

void CScanHandler::InitOptSetting()
{
    CNAVOptSettingsEx NavSettings;
	if( NavSettings.Init() )
    {
		fprintf(stdout, "Initialized Settings");
		CCTRACEW(_T("Settings Initialized"));
    }
}