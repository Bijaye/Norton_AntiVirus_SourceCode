// //////////////////////
//
// PROPRIETARY / CONFIDENTIAL.
// Use of this product is subject to license terms.
// Copyright © 2006 Symantec Corporation.
// All rights reserved.
//
/////////////////////////////////////////////////////////////////////////////////////////////////

#include "Stdafx.h"
#include <shlwapi.h>
#include <time.h>
#include "Resource.h"
#include "Navw32.h"
#include "hrx.h"
#include "Switches.h"
#include "ScanTaskLoader.h"
#include "OptNames.h"
#include "NAVInfo.h"
#include "ISVersion.h"
#include "AVccModuleId.h"
#include "SupportIDS.h"
#include <stdexcept>
using std::runtime_error;
#include <ccEraserInterface.h>

#include <StahlSoft.h>
#define _INIT_COSVERSIONINFO
#include <SSOsinfo.h>
#include "NAVSettingsHelperEx.h"

#include <ccResourceLoader.h>
#include <..\navw32res\resource.h>
extern cc::CResourceLoader g_ResLoader;

#include "SRTSPProtectThread.h"
//#include "AvProdLoggingACP.h"
// Errors
#include "isErrorLoader.h"

using namespace AVModule;
using namespace AVModule::ManualScanner;

using namespace avScanTask;

const LPTSTR SZ_SCANTASKFILENAME_MY_COMPUTER = _T("MyComp.sca");
const LPTSTR SZ_SCANTASKFILENAME_QUICK       = _T("quick.sca");

CNavw32::CNavw32(): m_bSubscriptionCOHOverride(false)
{
    m_bAlreadyRunning = false;
	m_bGlobal = false;
	m_bIsOfficeScan = false;
    m_bIsIMScan = false;
	m_bLogScanStartStop = false;
    m_bIgnoreOnceLow = false;
    m_bScanningNetwork = false;
	m_strScanName = _T("");
	m_eUIMode = avScanUI::eUIMode_Normal;
	m_dwHasAdminPrivilege =  -1;
}

CNavw32::~CNavw32()
{
	// Nuke temporary task files (if any).
	DeleteTempTaskFiles();
}

avScanUI::eScanResult CNavw32::Go(NAVWRESULT& NavwResult)
{
    // We need to load the CAV component in stand-alone mode since the AV service does not start in safe mode
    if(GetSystemMetrics(SM_CLEANBOOT))
    {
        CCTRCTXW0(L"Running in safe mode. Loading the AV stand alone interface");

        if(SYM_FAILED(m_avSafeModeLoader.Initialize()))
            return avScanUI::SRESULT_ERROR_CAV;

        // Get an instance of the stand alone AV object
        if(SYM_FAILED(m_avSafeModeLoader.CreateObject(m_spStandAlone)) || !m_spStandAlone)
            return avScanUI::SRESULT_ERROR_CAV;

        HRESULT hrInitAVSA = m_spStandAlone->Initialize();
        if( FAILED(hrInitAVSA) )
        {
            CCTRCTXW1(L"Failed to initialize the AV stand alone interface: 0x%X", hrInitAVSA);
            return avScanUI::SRESULT_ERROR_CAV;
        }
    }

    // Suppress the AvProdService's reboot dialog while in this method
    CSuppressAvProdSvcRebootDialogSwitch SuppressAvProdSvcRebootDialogSwitch;

    avScanUI::eScanResult eResult = avScanUI::SRESULT_OK;

	// We don't want to Windows popping up insert media dialogs if a removable
	// drive (i.e. floppy) is empty. We'll take of it.
	UINT uErrorMode = SetErrorMode(SEM_FAILCRITICALERRORS);

    // Init the mapdworddata loader
    SYMRESULT symRes = m_avMapDwordDataLoader.Initialize();
    if(SYM_FAILED(symRes))
    {
        CCTRCTXE1(L"Failed to init the MapDwordData loader, symres=%08X", symRes);
    }

#pragma  TODO("Figure out if this is a global scan...")
	AVModule::IAVMapStrDataPtr spOptions;
	AVModule::IAVMapStrDataPtr spTargets;
	if(m_bGlobal)
	{
		// Init the global loader
		if(SYM_FAILED(m_avGSLoader.Initialize()))
            return avScanUI::SRESULT_ERROR_CAV;

		// Get an instance of the global scanner
		AVModule::IAVGlobalScannerPtr spGlobalScan;
		if(SYM_FAILED(m_avGSLoader.CreateObject(spGlobalScan)) || !spGlobalScan)
			return avScanUI::SRESULT_ERROR_CAV;

		// Convert to a base scanner
		m_spScanner = spGlobalScan;
	}
	else
	{
		// Init the context loader
		if(SYM_FAILED(m_avCtxLoader.Initialize()))
			return avScanUI::SRESULT_ERROR_CAV;

		// Get an instance of the context scanner
		AVModule::IAVContextScannerPtr spCtxScan;
		if(SYM_FAILED(m_avCtxLoader.CreateObject(spCtxScan)) || !spCtxScan)
			return avScanUI::SRESULT_ERROR_CAV;

		// Get the scan targets
		spCtxScan->GetScanTargets(spTargets);

		// Convert to a base scanner
		m_spScanner = spCtxScan;
	}

	// Get the scan options
    HRESULT hr = m_spScanner->GetOptions(spOptions);
    if(FAILED(hr))
    {
        CCTRACEE(CCTRCTX L"GetOptions failed ... HR=%08X", hr);
        return avScanUI::SRESULT_ERROR_CAV;
    }

	// Read in the NAVOpts settings
	ReadSettings(spOptions);

	// Parse the command line arguments.
	m_TaskFiles.clear();
 	for(int iIndex = 1; iIndex < __argc; iIndex++)
	{
		// If the argument starts with a forward-slash it's a switch.
		if(__wargv[iIndex][0] == L'/')
		{
			if(ProcessSwitch(__wargv[iIndex], spOptions) == false)
			{
				CCTRCTXI1(L"Abort because of bad switch: %s", __wargv[iIndex]);
				CCTRCTXI0(L"or because user cancels out of builtin custom scan dialog");
				return avScanUI::SRESULT_ABORT;
			}
		}

		// Is it a wildcard item?
		else if(IsWildcardItem(__wargv[iIndex]) == true)
		{
			if(ProcessWildcardItem(__wargv[iIndex]) == false)
			{
				CCTRACEE(_T("Bad wildcard item: %s"), __wargv[iIndex]);
			}
		}

		// Otherwise it's an non-wildcard item to scan (i.e. drive, file, folder).
		else
		{
			// If we are performing an OfficeAV/IM scan, scan the item no matter what
			if(m_bIsOfficeScan || m_bIsIMScan)
			{
				AddTarget(__wargv[iIndex]);
			}
			else
			{
				if(CheckItem(__wargv[iIndex]) == true)
				{
					AddTarget(__wargv[iIndex]);
				}
			}
		}
	}

    // Enable/Disable eraser as appropriate
    COverridableOption::eOptionState eState = m_oOptEraser.GetState(m_bGlobal);
    if(eState == COverridableOption::eState_Enabled)
	    spOptions->SetValue(AV_SCANOPTION_ERASER, true);
    else if(eState == COverridableOption::eState_Disabled)
	    spOptions->SetValue(AV_SCANOPTION_ERASER, false);

    // Enable/Disable stealth scanning as appropriate
    eState = m_oOptStealth.GetState(m_bGlobal);
    if(eState == COverridableOption::eState_Enabled)
	    spOptions->SetValue(AV_SCANOPTION_ENABLESTEALTH, true);
    else if(eState == COverridableOption::eState_Disabled)
	    spOptions->SetValue(AV_SCANOPTION_ENABLESTEALTH, false);

    // Turn on impersonation (using our session ID), if we are scanning network paths
    // OR if this is a non-admin user!
	//ccLib::COSInfo osInfo;

    if(m_bScanningNetwork || !HasAdminPrivilege(true))
    {
       spOptions->SetValue(AV_SCANOPTION_IMPERSONATE_WHILE_SCANNING, true);
       spOptions->SetValue(AV_SCANOPTION_USECLIENTPROCESS_SESSIONID_FORIMPERSONATION, true);
	   CCTRACEI(_T("Doesn't Have Administrator Privilege Impersonating !"));
    }

	CCTRACEI(_T("Has Administrator Privileges"));

	// Set the Scan name
	//
    if(m_bGlobal)
		g_ResLoader.LoadString(IDS_FULL_SCAN, m_strScanName);
    else if(m_bIsIMScan)
		g_ResLoader.LoadString(IDS_IM_SCAN, m_strScanName);
    else if(m_bIsOfficeScan)
		g_ResLoader.LoadString(IDS_OFFICE_SCAN, m_strScanName);

    // There's no task name, must be a temporary task like a Shell Extension scan.
	// Use "Custom scan" string instead.
	if(m_strScanName.GetLength() == 0)
	{
		g_ResLoader.LoadString(IDS_CUSTOM_SCAN_TASK, m_strScanName);      
	}

	// Init the scan UI loader
	if(SYM_FAILED(m_suiLoader.Initialize()))
		return avScanUI::SRESULT_ERROR_SCANUI;

	// Create the scan UI object
	avScanUI::IScanUIPtr spScanUI;
	if(SYM_FAILED(m_suiLoader.CreateObject(spScanUI)))
		return avScanUI::SRESULT_ERROR_SCANUI;

	// Init the scan UI object
	spScanUI->Initialize();

	// Set the scan name
	spScanUI->SetScanName(m_strScanName);

    // Pass on the IgnoreOnceLow flag
    if(m_bIgnoreOnceLow)
        spScanUI->SetLowRiskMode(avScanUI::eRiskMode_IgnoreOnce);

    // Only scan if the SAME scan isn't already running and we have something to do
    if(m_bAlreadyRunning || !ScanHasSomethingToDo(spTargets, spOptions))
    {
        // We have nothing to do... bail
        NavwResult = NAVWRESULT_NOINFECTIONS;
        return avScanUI::SRESULT_OK;
    }

	// Call HandleScan() in avScanUI to start the scan...
	eResult = spScanUI->HandleScan(m_spScanner, m_eUIMode, NavwResult);

	// If the scan succeeded, update the last run time for task files.
    if(eResult == avScanUI::SRESULT_OK)
	{
		UpdateTaskTimes();

        // If the scan was clean, and global or eraser... unlock the system startup lock
        if(NavwResult == NAVWRESULT_NOINFECTIONS
            && (m_bGlobal || m_oOptEraser.GetState(m_bGlobal) == COverridableOption::eState_Enabled))
        {
            // Init the nav opts helper
    	    CNAVOptSettingsEx NavOpts;
		    if(NavOpts.Init())
		    {
                // Unlock the AP:DelayLoad setting (Set AP:DelayLoadLock=0)
                hr = NavOpts.SetValue(NAVAP_DelayLoadLock, (DWORD)0);
		    }
        }
	}

	// Set the previous error mode.
	SetErrorMode(uErrorMode);

	// Clean-up (ensure the scanner objects are gone before the loader)
	m_spScanner.Release();

	// We're done!
	return eResult;
}

bool CNavw32::ScanHasSomethingToDo(AVModule::IAVMapStrData* pTargets, AVModule::IAVMapStrData* pOptions)
{
    HRESULT hr;

    // Global always has something to do...
    if(m_bGlobal)
        return true;

    // Are we doing an eraser scan?
    bool bEraser = false;
    hr = pOptions->GetValue(AV_SCANOPTION_ERASER, bEraser);
    if(SUCCEEDED(hr) && bEraser)
        return true;

    // Are we scanning all master boot records?
    bool bScanAllMBR = false;
	hr = pOptions->GetValue(AV_SCANOPTION_ALL_MBR, bScanAllMBR);
    if(SUCCEEDED(hr) && bScanAllMBR)
        return true;

    // All we scanning all boot records?
    bool bScanAllBR = false;
	hr = pOptions->GetValue(AV_SCANOPTION_ALL_BR, bScanAllBR);
    if(SUCCEEDED(hr) && bScanAllBR)
        return true;

    // Check the options
    bool bScanFiles = true;
    pOptions->GetValue(AV_SCANOPTION_FILES, bScanFiles);

    // Do we have any targets and is file scanning enabled?
	DWORD dwTargets = 0;
    hr = pTargets->GetCount(dwTargets);
    if(SUCCEEDED(hr) && dwTargets > 0 && bScanFiles)
        return true;

    // Nothing to do...
    return false;
}

bool CNavw32::AddTarget(LPCWSTR pszTarget)
{
    // Is the target being added a network path?
    CStringW sTarget = pszTarget;
    int nSlash = sTarget.Find(L'\\', 2);    // start 2 chars in so we pass "X:" or "\\" 
    if(nSlash != -1)
    {
        if(sTarget.Left(2) == L"\\\\")
        {
            // Find the next "\"
            nSlash = sTarget.Find(L'\\', nSlash + 1);
            if(nSlash == -1)
                return false;
        }

        sTarget = sTarget.Left(nSlash + 1);
    }

    DWORD dwDriveType = GetDriveType(sTarget);
    if(dwDriveType == DRIVE_REMOTE)
    {
        m_bScanningNetwork = true;
    }

	// Only context scans support targets...
	// If spCtxScan is NULL, the current scanner doesn't support targets
	// so we can't possibly add the target... this is technically a failure
	// but not worth stopping over, so we will return true.
	AVModule::IAVContextScannerQIPtr spCtxScan(m_spScanner);
	if(!spCtxScan)
		return true;

	// Get the target list from the scanner
	AVModule::IAVMapStrDataPtr spTargets;
	if(FAILED(spCtxScan->GetScanTargets(spTargets)) || !spTargets)
		return false;

	// Get a count of current targets
	DWORD dwCount;
	if(FAILED(spTargets->GetCount(dwCount)))
		return false;

	// Convert the count to a string
	CAtlStringW sCount;
	sCount.Format(L"%d", dwCount);

	// Add this target
	return SUCCEEDED(spTargets->SetValue(sCount, pszTarget));
}

bool CNavw32::ProcessTaskFile(const TCHAR* szSwitch, AVModule::IAVMapStrData* pOptions)
{
	STAHLSOFT_HRX_TRY(hr)
	{
		ScanTask_Loader loader;
		IScanTaskPtr spScanTask;

		// Load the scan task object
		hrx << (SYM_SUCCEEDED(loader.CreateObject(&spScanTask))? S_OK : E_FAIL);

		const TCHAR* pszTaskFilePath = _tcschr(szSwitch, _T(':')) + 1;

		// Load the task file.
		// Note: ScanTask::Load() for builtin task files returns S_OK if there is item to scan;
		// returns E_FAIL if user cancels out of "Select items to scan" dialog.
		hrx << spScanTask->Load(CT2W(pszTaskFilePath));

		// we only want to allow 1 instance of Scan My Computer
		//  running at a time
		SCANTASKTYPE type = scanUnset;
		hrx << spScanTask->GetType(type);

		if(scanMyComputer == type)
		{
            m_bGlobal = true;
			if(IsAlreadyRunning("scanMyComputer", true))
			{
                m_bAlreadyRunning = true;
				return true;
			}
		}

		// we only want to allow 1 instance of QuickScan
		//  running at a time
		else if(scanQuick == type)
		{
		    // Set the scan task name to quick scan        
		    g_ResLoader.LoadString(IDS_QUICK_SCAN_TASK, m_strScanName);

            if(IsAlreadyRunning("scanQuick", true))
			{
                m_bAlreadyRunning = true;
				return true;
			}
		}
        else if(scanCustom == type)
        {
            // Custom is -MBR, -ALL_BR, +BR, +FILES, +SUBFOLDERS
            pOptions->SetValue(AV_SCANOPTION_ALL_MBR, false);
            pOptions->SetValue(AV_SCANOPTION_ALL_BR, false);
            pOptions->SetValue(AV_SCANOPTION_FILES, true);
            pOptions->SetValue(AV_SCANOPTION_SUBFOLDERS, true);
            pOptions->SetValue(AV_SCANOPTION_BR, true);
        }

		// Set the Scan Name 
		cc::IStringPtr pTaskName;
		hrx << spScanTask->GetName(&pTaskName);
		m_strScanName = pTaskName->GetStringW();

		hrx << spScanTask->EnumReset();

		// Iterate through the list of tasks.
		DWORD dwCount = 0;
		hrx << spScanTask->GetItemCount(dwCount);

		for(UINT uiItems = dwCount, ui = 0U; ui < uiItems; ui++)
		{
			// Add the next item to Scan Manager's items to scan list.
			SCANITEMTYPE type;
			SCANITEMSUBTYPE subtype;
			TCHAR chDrive = _TCHAR(' ');
			TCHAR szItemPath[2*MAX_PATH] = {0};
			cc::IStringPtr pItemPath; 
			CString sItemPath;
			unsigned long ulVID = 0;

			hrx << spScanTask->EnumNext(type, subtype, chDrive, &pItemPath, ulVID);

			// If this is a drive replace the item path with the drive to scan
			if(type == typeDrive)
			{
				sItemPath.Format(L"%c:", chDrive);
			}
			else
			{
				sItemPath = pItemPath->GetStringW();
			}

			// Make sure the item exists(if there's no VID)
			if(!ulVID && CheckItem(sItemPath) == false)
			{
				continue;
			}
			else if(subtype == subtypeFolder)
			{
				FillWithFiles(sItemPath);
			}
			else
			{
				AddTarget(sItemPath);
			}
		}

		// If this is a quick scan task type then set up the quick scan to run
		if(scanQuick == type)
		{
			// Quickscan is +ERASER,+STEALTH,-FILES,-MBR,-BR,
            m_oOptEraser.SetState(COverridableOption::eState_Enabled);
            m_oOptStealth.SetState(COverridableOption::eState_Enabled);
			pOptions->SetValue(AV_SCANOPTION_FILES, false);
			pOptions->SetValue(AV_SCANOPTION_ALL_MBR, false);
			pOptions->SetValue(AV_SCANOPTION_ALL_BR, false);
		}

		// If we're dealing with a non-temporary task file, save its name/path.
		// We'll update its last run time when the scan completes.
		if(_tcsnicmp(szSwitch, SWITCH_TTASKFILE, _tcslen(SWITCH_TTASKFILE)) != 0)
		{
			m_TaskFiles.push_back(pszTaskFilePath);
		}
	}
	STAHLSOFT_HRX_CATCH_ALL_NO_RETURN(hr)

	return (SUCCEEDED(hr)? true : false);
}

bool CNavw32::ProcessSwitch(const wchar_t* szSwitch, AVModule::IAVMapStrData* pOptions)
{
	if(_tcsicmp(szSwitch, SWITCH_ALL_DRIVES) == 0)
	{
		// This is a global scan...
		m_bGlobal = true;

        // Scan all drives (except A: and B:).
		// +FILES
		pOptions->SetValue(AV_SCANOPTION_FILES, true);

		// Add targets
		FillWithDrives();
	}
	else if(_tcsicmp(szSwitch, SWITCH_LOCAL_DRIVES) == 0)
	{
		// This is a global scan...
		m_bGlobal = true;

		// Scan all local drives (except A: and B:).
		// This is equivalent to the "My Computer" scan task file.
		CNAVInfo NavInfo;
		wchar_t szTask[MAX_PATH];
		_stprintf(szTask, _T("/TASK:%s\\Tasks\\%s"), NavInfo.GetNAVCommonDir(), SZ_SCANTASKFILENAME_MY_COMPUTER);

		return ProcessTaskFile(szTask, pOptions);
	}
	else if(_tcsicmp(szSwitch, SWITCH_ENABLE_SUBFOLDERS) == 0)
	{
		// Scan sub-folders.
		pOptions->SetValue(AV_SCANOPTION_SUBFOLDERS, true);
	}
	else if(_tcsicmp(szSwitch, SWITCH_DISABLE_SUBFOLDERS) == 0)
	{
		// Don't scan sub-folders.
		pOptions->SetValue(AV_SCANOPTION_SUBFOLDERS, false);
	}
	else if(_tcsicmp(szSwitch, SWITCH_ENABLE_BOOTREC) == 0)
	{
		// Scan boot records.
		// +MBR,+BR
		pOptions->SetValue(AV_SCANOPTION_ALL_MBR, true);
		pOptions->SetValue(AV_SCANOPTION_ALL_BR, true);
	}
	else if(_tcsicmp(szSwitch, SWITCH_BOOTREC_ONLY) == 0)
	{
		// +MBR,+BR,-FILES,-ERASER
		pOptions->SetValue(AV_SCANOPTION_ALL_MBR, true);
		pOptions->SetValue(AV_SCANOPTION_ALL_BR, true);
		pOptions->SetValue(AV_SCANOPTION_FILES, false);
        m_oOptEraser.SetState(COverridableOption::eState_Disabled);
	}
	else if(_tcsicmp(szSwitch, SWITCH_DISABLE_BOOTREC) == 0)
	{
		// Don't scan boot records and master boot records.
		// -MBR,-BR
		pOptions->SetValue(AV_SCANOPTION_ALL_MBR, false);
		pOptions->SetValue(AV_SCANOPTION_ALL_BR, false);
        pOptions->SetValue(AV_SCANOPTION_BR, false);
	}
	else if(_tcsicmp(szSwitch, SWITCH_ENABLE_ERASER_SCAN) == 0)
	{
		// Scan with Eraser.
		// +ERASER
        m_oOptEraser.SetState(COverridableOption::eState_Enabled);
	}
	else if(_tcsicmp(szSwitch, SWITCH_DISABLE_ERASER_SCAN) == 0)
	{
		// Don't scan with eraser.
		// -ERASER
        m_oOptEraser.SetState(COverridableOption::eState_Disabled);
	}
	else if(_tcsicmp(szSwitch, SWITCH_ENABLE_STEALTH_SCAN) == 0)
	{
		// Scan with VxMS.
        m_oOptStealth.SetState(COverridableOption::eState_Enabled);
	}
	else if(_tcsicmp(szSwitch, SWITCH_DISABLE_STEALTH_SCAN) == 0)
	{
		// Don't scan with VxMS.
        m_oOptStealth.SetState(COverridableOption::eState_Disabled);
	}
	else if(_tcsnicmp(szSwitch, SWITCH_TASKFILE, _tcslen(SWITCH_TASKFILE))  == 0
		|| _tcsnicmp(szSwitch, SWITCH_TTASKFILE, _tcslen(SWITCH_TTASKFILE)) == 0)
	{
		// Read task file and add the entries to the scan list.
		return ProcessTaskFile(szSwitch, pOptions);
	}
	else if(_tcsnicmp(szSwitch, SWITCH_AUTOMATION, _tcslen(SWITCH_AUTOMATION)) == 0)
	{
		// Don't display any UI.
		m_eUIMode = avScanUI::eUIMode_None;
	}
	else if(_tcsnicmp(szSwitch, SWITCH_OFFICEAV, _tcslen(SWITCH_OFFICEAV)) == 0)
	{
		// Set our flag for OfficeAV scanning
		m_bIsOfficeScan = true;

		// Setup Options for OfficeAV scanning

		// Turn off memory, boot records and master boot record scanning.
		pOptions->SetValue(AV_SCANOPTION_ALL_MBR, false);
		pOptions->SetValue(AV_SCANOPTION_ALL_BR, false);
        pOptions->SetValue(AV_SCANOPTION_BR, false);

		// Don't display any UI unless there are infections.
		m_eUIMode = avScanUI::eUIMode_OnlyInfections;

		// Turn off the scanning for non-viral threats for OfficeAV scans
		pOptions->SetValue(AV_SCANOPTION_NONVIRAL, false);
	}
	else if(_tcsnicmp(szSwitch, SWITCH_IMSCAN, _tcslen(SWITCH_IMSCAN)) == 0)
	{
		// Set our flag for IM scanning
		m_bIsIMScan = true;

        // IMSCAN = -MBR, -BR, -ERASER, +NONVIRAL, -STEALTH
        // The last three are already set and don't need to be set here

		// Turn off memory, boot records and master boot record scanning.
		pOptions->SetValue(AV_SCANOPTION_ALL_MBR, false);
        pOptions->SetValue(AV_SCANOPTION_ALL_BR, false);
		pOptions->SetValue(AV_SCANOPTION_BR, false);

        // Get the correct response mode
        GetResponseMode(pOptions, IMSCAN_ResponseMode);

		// Don't display any UI unless there are infections.
		m_eUIMode = avScanUI::eUIMode_OnlyInfections;
	}
	else if(_tcsnicmp(szSwitch, SWITCH_SILENT_ERASER_SCAN, _tcslen(SWITCH_SILENT_ERASER_SCAN)) == 0)
	{
		// Perform an eraser scan (in silent mode)
		// Don't display any UI unless there are infections.
		m_eUIMode = avScanUI::eUIMode_OnlyInfections;

    	// This is equivalent to the "Quick" scan task file.
		CNAVInfo NavInfo;
		wchar_t szTask[MAX_PATH];
		_stprintf(szTask, _T("/TASK:%s\\Tasks\\%s"), NavInfo.GetNAVCommonDir(), SZ_SCANTASKFILENAME_QUICK);

		return ProcessTaskFile(szTask, pOptions);
	}
	else if(_tcsnicmp(szSwitch, SWITCH_QUICK_SCAN, _tcslen(SWITCH_QUICK_SCAN)) == 0)
	{
		// Perform an eraser scan

    	// This is equivalent to the "Quick" scan task file.
		CNAVInfo NavInfo;
		wchar_t szTask[MAX_PATH];
		_stprintf(szTask, _T("/TASK:%s\\Tasks\\%s"), NavInfo.GetNAVCommonDir(), SZ_SCANTASKFILENAME_QUICK);

		return ProcessTaskFile(szTask, pOptions);
    }
	else if(_tcsnicmp(szSwitch, SWITCH_THREADS, _tcslen(SWITCH_THREADS)) == 0)
	{
		// Set number of scanning threads
		LPCWSTR pszVal = szSwitch + wcslen(SWITCH_THREADS);
		DWORD dwVal = _wtol(pszVal);
		if(dwVal != 0)
			pOptions->SetValue(AV_SCANOPTION_THREADS, dwVal);
	}
	else if(_tcsnicmp(szSwitch, SWITCH_PROGINT, _tcslen(SWITCH_PROGINT)) == 0)
	{
		// Set frequency of progress updates
		LPCWSTR pszVal = szSwitch + wcslen(SWITCH_PROGINT);
		DWORD dwVal = _wtol(pszVal);
		if(dwVal != 0)
			pOptions->SetValue(AV_SCANOPTION_PROGINTERVAL, dwVal);
	}
	else if(_tcsnicmp(szSwitch, SWITCH_DECDEPTH, _tcslen(SWITCH_DECDEPTH)) == 0)
	{
		// Set the decomposer depth option
		LPCWSTR pszVal = szSwitch + wcslen(SWITCH_DECDEPTH);
		DWORD dwVal = _wtol(pszVal);
		if(dwVal != 0)
			pOptions->SetValue(AV_SCANOPTION_DEC_DEPTH, dwVal);
	}
    else if(_tcsicmp(szSwitch, SWITCH_NOAUTOREMOVE) == 0)
    {
        m_bIgnoreOnceLow = false;
        pOptions->SetValue(AV_SCANOPTION_RESPONSEMODE, (DWORD)Response_NoAutoRemove);
    }
	else if(_tcsicmp(szSwitch, SWITCH_COH) == 0)
	{
		// Internal switch to perform an eraser scan with COH (automatically fix everything)
		// +ERASER,+COH,-FILES,-MBR,-BR
        m_oOptEraser.SetState(COverridableOption::eState_Enabled);
		pOptions->SetValue(AV_SCANOPTION_FILES, false);
		pOptions->SetValue(AV_SCANOPTION_SUBFOLDERS, false);
		pOptions->SetValue(AV_SCANOPTION_ALL_MBR, false);
        pOptions->SetValue(AV_SCANOPTION_ALL_BR, false);
		pOptions->SetValue(AV_SCANOPTION_BR, false);
		pOptions->SetValue(AV_SCANOPTION_ENABLECOH, true );
		pOptions->SetValue(AV_SCANOPTION_RESPONSEMODE, (DWORD)Response_AutoRemoveAll);
		// pOptions->SetValue(AV_SCANOPTION_RESPONSEMODE, (DWORD)Response_AutoRemoveMedAndHigh);
	}
    else if(_tcsicmp(szSwitch, SWITCH_COHONLY) == 0)
    {
        // Note: if the Eraser detections override map was set in ReadOptions(), our
        //       SetValue() call will replace that map with ours ... This is what we want

        // Create a MapDwordData
        AVModule::IAVMapDwordDataPtr spMap;
        SYMRESULT symRes = m_avMapDwordDataLoader.CreateObject(spMap);
        if(SYM_SUCCEEDED(symRes))
        {
            // Disable everything
            HRESULT hr;
            for(int nType = ccEraser::FirstDetectionType; nType <= ccEraser::LastDetectionType; ++nType)
            {
                hr = spMap->SetValue(nType, false);
                if(FAILED(hr))
                    CCTRCTXE2(L"Failed to setvalue(%d, false) in eraserfilter_detection map (hr=%08X)", nType, hr);
            }

            // Enable COH
            hr = spMap->SetValue(ccEraser::ApplicationHeuristicDetectionActionType, true);
            if(FAILED(hr))
                CCTRCTXE1(L"Failed to set COH detection type to true (hr=%08X)", hr);

            // Set the map
            hr = pOptions->SetValue(AV_SCANOPTION_OVERRIDE_ERASERFILTER_DETECTIONS, spMap);
            if(FAILED(hr))
                CCTRCTXE1(L"Failed to set eraserfilter_detection map in options (hr=%08X)", hr);
        }
        else
            CCTRCTXE1(L"Failed to create MapDWordData object for detection filter override, symRes=%08X", symRes);
    }
    else if(_tcsicmp(szSwitch, SWITCH_DISABLE_COOKIE_SCAN) == 0)
    {
        HRESULT hr;
        // Try to get the current map
        AVModule::IAVMapDwordDataPtr spMap;
        hr = pOptions->GetValue(AV_SCANOPTION_OVERRIDE_ERASERFILTER_DETECTIONS, AVModule::AVDataTypes::eTypeMapDword, reinterpret_cast<ISymBase*&>(spMap));
        if(FAILED(hr))
        {
            CCTRCTXE1(L"Failed to get eraserfilter_detection map in options (hr=%08X) ... creating a new map", hr);
            SYMRESULT symRes = m_avMapDwordDataLoader.CreateObject(spMap);
            if(SYM_SUCCEEDED(symRes))
            {
                hr = S_OK;
            }
            else
            {
                CCTRCTXE1(L"Failed to create a map dword data (sr=%08X)", symRes);
            }
        }

        // If we have a MapDwordData
        if(SUCCEEDED(hr) && spMap)
        {
            // Disable cookies
            hr = spMap->SetValue(ccEraser::CookieDetectionActionType, false);
            if(FAILED(hr))
                CCTRCTXE1(L"Failed to disable cookie detection type (hr=%08X)", hr);

            // Set the map
            hr = pOptions->SetValue(AV_SCANOPTION_OVERRIDE_ERASERFILTER_DETECTIONS, spMap);
            if(FAILED(hr))
                CCTRCTXE1(L"Failed to set eraserfilter_detection map in options (hr=%08X)", hr);
        }
    }
	else
	{
		// Any other switch is invalid.
        NAVErrorDisplay(IDS_INVALID_SWITCH, szSwitch);
		return false;
	}

	return true;
}

void CNavw32::FillWithDrives()
{
	DWORD dwMask = 1;
	DWORD dwDrives = GetLogicalDrives();
	wchar_t szRootDir[] = L"X:\\";

	for(wchar_t chDrive = L'A'; chDrive <= L'Z'; chDrive++, dwMask <<= 1)
	{
		// Skip drive A: and B:
		if(chDrive == L'A' || chDrive == L'B')
		{
			continue;
		}

		if(dwDrives & dwMask)
		{
			szRootDir[0] = chDrive;

			// Make sure the disk exists (i.e. removable drives).
			if(DiskExists(szRootDir) == false)
			{
				continue;
			}

			AddTarget(szRootDir);
		}
	}
}

bool CNavw32::FillWithFiles(const TCHAR* szDirectory)
{
	TCHAR szFileSpec[MAX_PATH];
	PathCombine(szFileSpec, szDirectory, _T("*.*"));

	WIN32_FIND_DATA FindData;
	HANDLE hFind = FindFirstFile(szFileSpec, &FindData);

	if(hFind == INVALID_HANDLE_VALUE)
	{
		return false;
	}

	TCHAR szFilePath[MAX_PATH];

	do
	{
		// Skip sub-directories.
		if(FindData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
		{
			continue;
		}

		// Get the full path to the file.
		PathCombine(szFilePath, szDirectory, FindData.cFileName);

		// Add the file to the scan list. 
		AddTarget(szFilePath);
	}
	while(FindNextFile(hFind, &FindData));

	FindClose(hFind);

	return true;
}

bool CNavw32::DiskExists(const TCHAR* szRootDir)
{
	TCHAR szVolumeName[32];
	DWORD dwComponentLen, dwFileSysFlags;

	if(!GetVolumeInformation(szRootDir, szVolumeName, sizeof(szVolumeName),
		NULL, &dwComponentLen, &dwFileSysFlags, NULL, 0))
	{
		return false;
	}

	return true;
}

bool CNavw32::ReadSettings(AVModule::IAVMapStrData* pOptions)
{
	bool bResult = true;
	CNAVOptSettingsEx NavOpts;

	try
	{
		if(!NavOpts.Init())
		{
			throw runtime_error("CNavw32::ReadSettings() - Unable to initialize the options settings.");
		}

		m_bLogScanStartStop = true;

		// Get the default eraser option
		DWORD dwValue;
		if(FAILED(NavOpts.GetValue(SCANNER_EnableSideEffectScan, dwValue, 1)))
		{
			CCTRACEE(_T("CNavw32::ReadSettings() - Unable to read the SCANNER:EnableSideEffectScan setting. Setting to true"));
		}
        if(dwValue == 0)
            m_oOptEraser.SetState(COverridableOption::eState_DisabledIfGlobal);
        else
            m_oOptEraser.SetState(COverridableOption::eState_EnabledIfGlobal);

		// Get the default stealth option
		if(FAILED(NavOpts.GetValue(SCANNER_EnableStealthScan, dwValue, 1)))
		{
			CCTRACEE(_T("CNavw32::ReadSettings() - Unable to read the SCANNER:EnableStealthScan setting. Setting to true"));
		}
        if(dwValue == 0)
            m_oOptStealth.SetState(COverridableOption::eState_Disabled);
        else
            m_oOptStealth.SetState(COverridableOption::eState_Enabled);

		// Get the compressed file scanning option (default = false)
		if(FAILED(NavOpts.GetValue(GENERAL_ScanZipFiles, dwValue, 0)))
		{
			CCTRACEE(_T("CNavw32::ReadSettings() - Unable to read the GENERAL:ScanZipFiles setting. Setting to false"));
		}
		pOptions->SetValue(AV_SCANOPTION_COMPRESSED, dwValue);

        // Do we want to detect non-virals (default = true)?
		if(FAILED(NavOpts.GetValue(THREAT_NoThreatCat, dwValue, 0)))
		{
            CCTRACEE(_T("CNavw32::ReadSettings() - Unable to read the THREAT:NoThreatCat setting. Setting nonvirals to on."));
		}
		pOptions->SetValue(AV_SCANOPTION_NONVIRAL, (dwValue==0)?true:false);	// NoThreatCat 0 = Scan for nonvirals

		// Set the number of threads for ccScan (default is 0)
		if(SUCCEEDED(NavOpts.GetValue(SCANNER_ScanThreads, dwValue, 0)))
		{
            pOptions->SetValue(AV_SCANOPTION_THREADS, dwValue);
		}

        // Scan cookies? (default = true)
        if(FAILED(NavOpts.GetValue(SCANNER_ScanTrackingCookies, dwValue, 1)))
        {
            CCTRACEE(_T("CNavw32::ReadSettings() - Unable to read the SCANNER:ScanTrackingCookies setting.  Setting to 1"));
        }

       
        // Create a MapDwordData
        AVModule::IAVMapDwordDataPtr spMap;
        SYMRESULT symRes = m_avMapDwordDataLoader.CreateObject(spMap);
        if(SUCCEEDED(symRes))
        {
            spMap->SetValue(ccEraser::CookieDetectionActionType, dwValue);
            pOptions->SetValue(AV_SCANOPTION_OVERRIDE_ERASERFILTER_DETECTIONS, spMap);
        }
        else
            CCTRCTXE1(L"Failed to create MapDWordData object for detection filter override, symRes=%08X", symRes);

        // Enable clean stealth notifications (Default = true)
        if(FAILED(NavOpts.GetValue(SCANNER_NotifyOnCleanStealth, dwValue, 1)))
        {
            CCTRACEE(_T("CNavw32::ReadSettings() - Unable to read the SCANNER:Mode setting.  Setting to 1"));
        }

        pOptions->SetValue(AV_SCANOPTION_NOTIFYONCLEANSTEALTHFILES, dwValue);

        // Get the response mode for low-priority threats (default is 2 = ask)
        if(FAILED(NavOpts.GetValue(SCANNER_ResponseMode, dwValue, RESPONSE_MODE_ASK)))
        {
            CCTRACEE(_T("CNavw32::ReadSettings() - Unable to read the SCANNER:ResponseMode setting.  Setting to ask"));
        }
        switch(dwValue)
        {
            case RESPONSE_MODE_AUTO:
                pOptions->SetValue(AV_SCANOPTION_RESPONSEMODE, (DWORD)Response_AutoRemoveAll);
                break;
            case RESPONSE_MODE_IGNORE:
                m_bIgnoreOnceLow = true;
                // FALL THROUGH to RESPONSE_MODE_ASK
            case RESPONSE_MODE_ASK:
                pOptions->SetValue(AV_SCANOPTION_RESPONSEMODE, (DWORD)Response_AutoRemoveMedAndHigh);
                break;
            default:
                CCTRACEE(_T("CNavw32::ReadSettings() - Invalid SCANNER:ResponseMode setting.  Setting to ask"));
                pOptions->SetValue(AV_SCANOPTION_RESPONSEMODE, (DWORD)Response_AutoRemoveMedAndHigh);
                break;
        }

		// Set whether or not to run COH (default is 1)
        //  - only matters if eraser is on, so the options UI should show this
        //    as a subitem of "Turn on security risk protection"
		if(FAILED(NavOpts.GetValue(SCANNER_ConfidenceOnlineHeavyEngine, dwValue, 1)))
		{
			CCTRACEE(_T("CNavw32::ReadSettings() - Unable to read the SCANNER:ConfidenceOnlineHeavyEngine setting. Setting to 0"));
			dwValue = 0;
		}

		if(m_bSubscriptionCOHOverride)
		{
			CCTRCTXI0(_T("Subscription COH Override set. Forcibly disabling COH engine."));
			dwValue = 0;
		}

		pOptions->SetValue(AV_SCANOPTION_ENABLECOH, dwValue);

        // Get the maximum file extraction size (default is 50MB)
        if(FAILED(NavOpts.GetValue(SCANNER_DecomposerMaxExtractSize, dwValue, (1024 * 1024 * 50))))
        {
			CCTRACEE(_T("CNavw32::ReadSettings() - Unable to read the SCANNER:DecomposerMaxExtractSize setting. Setting to 50MB"));
        }
        pOptions->SetValue(AV_SCANOPTION_DEC_MAXEXTRACT, dwValue);

        // Get the heuristic level
        //SCANNER:UserHeuristicLevel
	}
	catch(exception& Ex)
	{
		bResult = false;
		ATL::CString strException(Ex.what());
		CCTRACEE(strException);
	}

	return bResult;
}

bool CNavw32::GetResponseMode(AVModule::IAVMapStrData* pOptions, LPCTSTR pszOption)
{
	bool bResult = true;
	CNAVOptSettingsEx NavOpts;

	try
	{
		if(!NavOpts.Init())
		{
			throw runtime_error("CNavw32::ReadSettings() - Unable to initialize the options settings.");
		}

        // Get the response mode for low-priority threats (default is 2 = ask)
        DWORD dwValue;
        if(FAILED(NavOpts.GetValue(pszOption, dwValue, RESPONSE_MODE_ASK)))
        {
            CCTRCTXE1(L"Unable to read the %s setting.  Setting to ask", pszOption);
        }

        m_bIgnoreOnceLow = false;
        switch(dwValue)
        {
            case RESPONSE_MODE_AUTO:
                pOptions->SetValue(AV_SCANOPTION_RESPONSEMODE, (DWORD)Response_AutoRemoveAll);
                break;
            case RESPONSE_MODE_IGNORE:
                m_bIgnoreOnceLow = true;
                // FALL THROUGH to RESPONSE_MODE_ASK
            case RESPONSE_MODE_ASK:
                pOptions->SetValue(AV_SCANOPTION_RESPONSEMODE, (DWORD)Response_AutoRemoveMedAndHigh);
                break;
            default:
                CCTRACEE(_T("CNavw32::ReadSettings() - Invalid SCANNER:ResponseMode setting.  Setting to ask"));
                pOptions->SetValue(AV_SCANOPTION_RESPONSEMODE, (DWORD)Response_AutoRemoveMedAndHigh);
                break;
        }
	}
	catch(exception& Ex)
	{
		bResult = false;
		ATL::CString strException(Ex.what());
		CCTRACEE(strException);
	}

	return bResult;
}

bool CNavw32::CheckItem(const wchar_t* szItem)
{
	// First do a quick check to see if the item exists.
	DWORD dwAttribs = GetFileAttributes(szItem);
	if(dwAttribs != INVALID_FILE_ATTRIBUTES)
	{
		// If the item is a file, make sure we can open it.
		bool bResult = true;
		if((dwAttribs & FILE_ATTRIBUTE_DIRECTORY) == 0)
		{
            SRTSP::CProtectThread srtspControl;
            SRTSP::CSmartUnProtectThread autoSrtsp(&srtspControl);

            HANDLE hFile = CreateFile(szItem, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL);

			// Make sure this is not a sneaky reserved DOS name used as a regular folder name
			bool bActuallyItsAValidDirectory = false;
			if(hFile == INVALID_HANDLE_VALUE)
			{
				ATL::CAtlString strFileName = _T("\\\\.\\");
				strFileName += szItem;
				dwAttribs = GetFileAttributes(strFileName);

				if(dwAttribs & FILE_ATTRIBUTE_DIRECTORY)
				{
					CCTRACEI(_T("CNavw32::CheckItem() - %s is actually a directory not a file."), (LPCTSTR)strFileName);
					bActuallyItsAValidDirectory = true;
				}
                else
                {
                    // Try to open the file using the full UNC
                    hFile = CreateFile(strFileName, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL);
                    if(hFile != INVALID_HANDLE_VALUE)
                    {
					    CCTRACEI(_T("CNavw32::CheckItem() - %s is a file that requires reserved DOS name."), (LPCTSTR)strFileName);
                    }
                }
			}

			if(hFile != INVALID_HANDLE_VALUE)
			{
				CloseHandle(hFile);
			}
			else if(!bActuallyItsAValidDirectory)
			{
				bResult = false;
				CCTRACEW(_T("CNavw32::CheckItem() - %s is not a valid file. Showing CED."), szItem);
				NAVErrorDisplay(IDS_ACCESS_DENIED, szItem);
			}
		}

		return bResult;
	}

	// Display an error if the item is not a drive.
	if(IsItemADrive(szItem) == false)
	{
		NAVErrorDisplay(IDS_BAD_ITEM, szItem);
		return false;
	}

	// If the item is a drive, return true and scan it
	return true;
}

bool CNavw32::IsItemADrive(const TCHAR* szItem)
{
	if(_tcslen(szItem) < 2 || _tcslen(szItem) > 3)
	{
		return false;
	}

	if(IsCharAlpha(szItem[0]) == FALSE)
	{
		return false;
	}

	TCHAR szDrive1[] = _T("X:");
	szDrive1[0] = szItem[0];

	TCHAR szDrive2[] = _T("X:\\");
	szDrive2[0] = szItem[0];

	if(_tcsicmp(szItem, szDrive1) == 0)
	{
		return true;
	}

	if(_tcsicmp(szItem, szDrive2) == 0)
	{
		return true;
	}

	return false;
}

void CNavw32::UpdateTaskTimes()
{
	std::vector<tstring>::iterator Iter;
	ScanTask_Loader loader;

	for(Iter = m_TaskFiles.begin(); Iter != m_TaskFiles.end(); Iter++)
	{
		IScanTaskPtr spScanTask;

		// Load the scan task object
		if(SYM_SUCCEEDED(loader.CreateObject(&spScanTask)))
		{
			if(SUCCEEDED(spScanTask->Load((*Iter).c_str(), false)))
			{
				time_t CurrentTime;
				time(&CurrentTime);
				spScanTask->SetLastRunTime(CurrentTime);
				spScanTask->Save();
			}
		}
	}
}

bool CNavw32::IsWildcardItem(const TCHAR* szItem)
{
	// Search for a wildcard character.
	TCHAR* pWild = _tcspbrk(szItem, _T("*?"));
	if(pWild == NULL)
	{
		return false;
	}

	// Make sure the wildcard character was found AFTER any trailing backslash.
	if(_tcsrchr(szItem, _T('\\')) > pWild)
	{
		return false;
	}

	return true;
}

bool CNavw32::ProcessWildcardItem(const TCHAR* szItem)
{
	// Get the item's directory.
	TCHAR szDirectory[MAX_PATH] = {0};

	if(ExtractWildcardDir(szItem, szDirectory) == false)
	{
		return false;
	}

	// Find matches and add them to the scan list.
	WIN32_FIND_DATA FindData;
	HANDLE hFind = FindFirstFile(szItem, &FindData);

	if(hFind == INVALID_HANDLE_VALUE)
	{
		return false;
	}

	TCHAR szFullPath[MAX_PATH];

	do
	{
		if(FindData.cFileName[0] == _T('.'))
		{
			continue;
		}

		_tcscpy(szFullPath, szDirectory);
		_tcscat(szFullPath, FindData.cFileName);
		AddTarget(szFullPath);
	}
	while(FindNextFile(hFind, &FindData));

	FindClose(hFind);

	return true;
}

bool CNavw32::ExtractWildcardDir(const TCHAR* szItem, TCHAR* szDirectory)
{
	// If an absolute path has been specified, translate .. and . sequences.
	if(PathIsUNC(szItem) || (PathGetDriveNumber(szItem) != -1))
	{
		if(PathCanonicalize(szDirectory, szItem) == FALSE)
		{
			return false;
		}
	}

	// Check for a directory with no drive letter, but with a leading backslash.
	// In this case prepend the current drive.
	else if(szItem[0] == _T('\\'))
	{
		GetCurrentDirectory(MAX_PATH, szDirectory);
		_tcscpy(szDirectory + 2, szItem);
	}

	// A relative path has been specified. Include the current directory.
	else
	{
		GetCurrentDirectory(MAX_PATH, szDirectory);
		PathAppend(szDirectory, szItem);
	}

	// Remove the wildcard spec.
	if(PathRemoveFileSpec(szDirectory))
	{
		PathAddBackslash(szDirectory);
	}

	return true;
}

void CNavw32::DeleteTempTaskFiles()
{
	for(int iIndex = 1; iIndex < __argc; iIndex++)
	{
		if(wcsnicmp(__wargv[iIndex], SWITCH_TTASKFILE, wcslen(SWITCH_TTASKFILE)) == 0)
		{
			// Extract file path from the commandline /TTASK:"C:\...\xxx.sca"
			const wchar_t* pszTaskFilePath = wcschr(__wargv[iIndex], L':') + 1;
			if(pszTaskFilePath != NULL)
			{
				CCTRCTXI1(L"Delete %s", pszTaskFilePath);
				if(!DeleteFile(pszTaskFilePath))
				{
					CCTRCTXE1(L"Failed to delete task file. Error: %d", GetLastError());
				}
			}
		}
	}
}

void CNavw32::NAVErrorDisplay(UINT uStringID, const TCHAR* szItemName)
{
	// Get title
	CString csProductName(CISVersion::GetProductName());

	// Get the tech support ID and format error message.
	CString csFormat, csMessage;
	g_ResLoader.LoadString(uStringID, csFormat);
	UINT uSupportID = 0;

	switch(uStringID)
	{
	case IDS_ERROR_CRITICAL:
		uSupportID = SUPPORT_ID_ERROR_CRITICAL;
		csMessage.Format(csFormat, csProductName);
		break;
	case IDS_ACCESS_DENIED:
		uSupportID = SUPPORT_ID_ACCESED_DENIED;
		csMessage.Format(csFormat, szItemName);
		break;
	case IDS_BAD_ITEM:
		uSupportID = SUPPORT_ID_BAD_ITEM;
		csMessage.Format(csFormat, szItemName);
		break;
	case IDS_ERROR_MEMORY:
		uSupportID = SUPPORT_ID_ERROR_MEMORY;
		csMessage.Format(csFormat, csProductName);
		break;
	case IDS_ERROR_VIRUS_DEFS:
		uSupportID = SUPPORT_ID_ERROR_VIRUS_DEFS;
		csMessage.Format(csFormat, csProductName);
		break;
	case IDS_ERROR_SCANNER:
		uSupportID = SUPPORT_ID_ERROR_SCANNER;
		csMessage.Format(csFormat, csProductName, csProductName);
		break;
	case IDS_EULA_NOT_ACCEPTED:
		uSupportID = SUPPORT_ID_EULA_NOT_ACCEPTED;
		csMessage.Format(csFormat, csProductName);
		break;
	case IDS_CFGWIZ_NOT_COMPLETE:
		uSupportID = SUPPORT_ID_EULA_NOT_ACCEPTED;
		csMessage.Format(csFormat, csProductName);
		break;
	case IDS_ERROR_ERASER_REBOOT_REQUIRED:
		uSupportID = SUPPORT_ID_ERASER_REBOOT_REQUIRED;
		csMessage.Format(csFormat, csProductName);
		break;
	case IDS_LICENSE_VIOLATED:
		uSupportID = SUPPORT_ID_ERROR_LICENSE;
		csMessage.Format(csFormat, csProductName);
		break;
    case IDS_INVALID_SWITCH:
		uSupportID = SUPPORT_ID_BAD_ITEM;
		csMessage.Format(csFormat, szItemName);
		break;
	}

    ISShared::ISShared_IError errorLoader;
    ISShared::IErrorPtr error;
    if ( SYM_SUCCEEDED (errorLoader.CreateObject (GETMODULEMGR(), &error)))
    {
        error->Show ( AV_MODULE_ID_NAVW, 
                    uSupportID,
                    0,    // custom resource
                    NULL, // title
                    csMessage,    // message
                    S_OK, // don't display the hresult
                    true,
                    true,
                    NULL );
    }
}

BOOL CALLBACK CNavw32SwitchToProc(HWND hwnd, LPARAM lParam);

// SingleInstanceCheck - returns TRUE if another 
bool CNavw32::IsAlreadyRunning(LPCSTR szName, bool bSwitch)
{
	SYMRESULT sr = SYMERR_UNKNOWN;
	bool bAlreadyRunning = false;

	sr = m_loaderSingleInstance.CreateObject(&m_spSingleInstance);
	if(SYM_SUCCEEDED(sr) && m_spSingleInstance)
	{
	    DWORD dwOwnerProcess = GetCurrentProcessId(), dwOwnerThread = GetCurrentThreadId();

	    HRESULT hr;
	    hr = m_spSingleInstance->RegisterSingleInstance(szName, bAlreadyRunning, dwOwnerProcess, dwOwnerThread);
	    if(FAILED(hr))
	    {
	        // If this call failed, it is strange, but non-fatal,
	        //  and we should continue running.
	        CCTRACEE(_T("CNavw32::IsAlreadyRunning - ISingleInstance::RegisterSingleInstance() failed. hr=0x%08X. Non fatal, continuing."), hr);
	    }

	    if(bSwitch && bAlreadyRunning && dwOwnerProcess)
	    {
    	    CCTRACEI(_T("CNavw32::IsAlreadyRunning - Another process has already locked this instance. %X::%X"), dwOwnerProcess, dwOwnerThread);
	        EnumWindows(CNavw32SwitchToProc, dwOwnerProcess);
	    }
	}
	else
	{
    	// If this object can't be created, it is weird, but non-fatal.
	    CCTRACEE(_T("CNavw32::IsAlreadyRunning - Unable to create single instance object.  Non fatal, continuing."));
	}

	return bAlreadyRunning;
}

const TCHAR szCommonUIWindowClassName[] = _T("Sym_Common_Scan_Window");

BOOL CALLBACK CNavw32SwitchToProc(HWND hwnd, LPARAM lParam)
{
	//
	// --- NOT THREAD SAFE ---
	//   For optimization reasons, these values 
	//   don't need to init every time this func is called.
	//
	static const DWORD dwClassNameSizeChars = CCDIMOF(szCommonUIWindowClassName);
	static TCHAR szClassName[dwClassNameSizeChars] ={0};
	static DWORD dwRet = NULL;

	// zero out the buffer, it might be dirty
	dwRet = GetClassName(hwnd, szClassName, dwClassNameSizeChars);

	// if the size doesn't match, don't bother
	// with the other comparisons
	if(dwRet != (dwClassNameSizeChars - 1))
		return TRUE;

	// See if this is the string we are
	// searching for.
	if(0 != _tcscmp(szClassName, szCommonUIWindowClassName))
		return TRUE;

	DWORD dwTargetProcess = (DWORD)lParam;
	DWORD dwFoundProcess = NULL;
	GetWindowThreadProcessId(hwnd, &dwFoundProcess);

	// Now find out if it is in the process
	// that we are looking for.
	if(dwTargetProcess != dwFoundProcess)
		return TRUE;

	// Apparently this is the one we
	// are looking for, so switch.
	SwitchToThisWindow(hwnd, TRUE);
	return FALSE;
}

void CNavw32::SetSubscriptionCOHOverride(bool bSubOverride)
{ 
	m_bSubscriptionCOHOverride = bSubOverride; 
}

bool CNavw32::HasAdminPrivilege(bool bPromptForElevation)
{
	ccLib::COSInfo osInfo;
	
	if ( m_dwHasAdminPrivilege != -1)
		return m_dwHasAdminPrivilege;

	if (osInfo.IsWinVista(true) && bPromptForElevation) // Shell Execute the stub, if it works the user is an admin
	{
		TCHAR szPath[_MAX_PATH] = {0};
		size_t sizePath = _MAX_PATH;
		if ( CISSharedPathProvider::GetPath(szPath, sizePath))
		{
			_tcscat ( szPath, _T("\\isUAC.exe"));
			HINSTANCE hInstance = ::ShellExecute(GetActiveWindow(), _T("open"), szPath, NULL, NULL, SW_SHOWNORMAL);
			if ( hInstance <= (HINSTANCE) 32)
			{
				CCTRCTXE2(_T("Failed to launch %s -- %d"), szPath, ::GetLastError());
				m_dwHasAdminPrivilege = 0;
				return m_dwHasAdminPrivilege;
			}

			CCTRCTXI0(_T("Admin privilege confirmed."));
			m_dwHasAdminPrivilege = 1;
			return m_dwHasAdminPrivilege;
		}
		else
		{
			CCTRCTXE0(_T("Failed to get ISShared path.  Falling back to IsAdmin()."));
			m_dwHasAdminPrivilege = osInfo.IsAdministrator() ? 1 : 0;
			return m_dwHasAdminPrivilege;
		}
	}
	else // XP
	{
		m_dwHasAdminPrivilege = osInfo.IsAdministrator() ? 1 : 0;
	}

	return m_dwHasAdminPrivilege;
}
