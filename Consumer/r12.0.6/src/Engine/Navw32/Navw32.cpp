
#include "Stdafx.h"
#include <shlwapi.h>
#include <time.h>
#include "Resource.h"
#include "Navw32.h"
#include "Switches.h"
#include "SymInterfaceLoader.h"
#include "ScanTask.h"
#include "OptNames.h"
#include "NAVInfo.h"
#include "NAVError.h"
#include "AVRESBranding.h"
#include "AVccModuleId.h"
#include "SupportIDS.h"
#include <stdexcept>
using std::runtime_error;

#include "StahlSoft.h"
#define _INIT_COSVERSIONINFO
#include "SSOsinfo.h"
#include "NAVSettingsHelperEx.h"

#define NAVAPPIDS_BUILDING
#include "NAVAppIDs.h"

#include "ccResourceLoader.h"
#include "..\navw32res\resource.h"
extern cc::CResourceLoader g_ResLoader;

const LPTSTR SZ_SCANTASKFILENAME_MY_COMPUTER = _T("MyComp.sca");

// Show only relevant detections
//  --- TEMP --- TEMP --- TEMP
//  need to make this part of the ScanMgrInterface
#define SMSETTING_ONLY_RELEVANT_RISKS (SMSETTING)(SMSETTING_OFFICE + 1)

CNavw32::CNavw32()
{
	m_pScanManager = NULL;
	m_bIsOfficeScan = false;
    m_bLogScanStartStop = false;
    m_bEnableEraserScan = true;
	m_strScanName = "";
}

CNavw32::~CNavw32()
{
	// Nuke temporary task files (if any).

	DeleteTempTaskFiles();

	// Clean-up.

	if (m_pScanManager != NULL)
	{
		m_pScanManager->Release();
        m_pScanManager = NULL;
	}
}

void CNavw32::ProcessRebootRemediations()
{
    // Load up scan manager.
    if (m_pScanManager == NULL)
    {
        SYMRESULT symRes = SYM_OK;
        if( SYM_FAILED(symRes = m_ScanManagerLoader.CreateObject(&m_pScanManager)) )
        {
            CCTRACEE(_T("CNavw32::ProcessRebootRemediations() - Failed to load the scan manager. SYMRESULT = 0x%X"), symRes);
            return;
        }
    }

    // Disable Auto-Protect.
    bool bDisableAP = m_APWrapper.DisableAPForThisThread();

    CCTRACEI(_T("CNavw32::ProcessRebootRemediations() - Calling IScanManager::PostRebootRemediate()"));
    m_pScanManager->ScanAndRepair(SWITCH_REMEDIATE_AFTER_REBOOT);

    // Re-enable Auto-Protect.
    if (bDisableAP == true)
    {
        m_APWrapper.EnableAPForThisThread();
    }

    if( m_pScanManager != NULL )
    {
        m_pScanManager->Release();
        m_pScanManager = NULL;
    }
}

SMRESULT CNavw32::Go(NAVWRESULT &NavwResult)
{
	// Create the Scan Manager object (if necessary).

	SMRESULT Result = SMRESULT_OK;

	if (m_pScanManager == NULL)
	{
        SYMRESULT symRes = SYM_OK;
		if( SYM_FAILED(symRes = m_ScanManagerLoader.CreateObject(&m_pScanManager)) )
		{
            CCTRACEE(_T("CNavw32::Go() - Failed to load the scan manager. SYMRESULT = 0x%X"), symRes);
			return SMRESULT_ERROR_SCANNER;
		}
	}

	// Disable Auto-Protect.

	bool bDisableAP = m_APWrapper.DisableAPForThisThread();

	// We don't want to Windows popping up insert media dialogs if a removable
	// drive (i.e. floppy) is empty. We'll take of it.

	UINT uErrorMode = SetErrorMode(SEM_FAILCRITICALERRORS);

    // Read in the settings
    ReadSettings();

	// Set the start/stop logging flag.

	if (m_bLogScanStartStop)
	{
		m_pScanManager->OverrideSetting(SMSETTING_LOG_START_STOP, true);
	}
	else
	{
		m_pScanManager->OverrideSetting(SMSETTING_LOG_START_STOP, false);
	}

	// Parse the command line arguments.

	m_TaskFiles.clear();
	m_bShouldAddItems = true;

	for (int iIndex = 1; iIndex < __argc; iIndex++)
	{
		// If the argument starts with a forward-slash it's a switch.

		if (__argv[iIndex][0] == _T('/'))
		{
			if (ProcessSwitch(__argv[iIndex]) == false)
			{
				CCTRACEE("Bad switch: %s", __argv[iIndex]);
			}
		}

		// Is it a wildcard item?

		else if (IsWildcardItem(__argv[iIndex]) == true)
		{
			if (ProcessWildcardItem(__argv[iIndex]) == false)
			{
				CCTRACEE("Bad wildcard item: %s", __argv[iIndex]);
			}
		}

		// Otherwise it's an non-wildcard item to scan (i.e. drive, file, folder).

		else
		{
			// If we are performing an OfficeAV scan, scan the item no matter what
			if(m_bIsOfficeScan)
			{
				// Make sure we can access the item.
				if(GetFileAttributes( __argv[iIndex] ) == -1 )
				{
					// Try OEM mode.  If we can access the file in OEM mode,
					// perform the scan in OEM mode.
					SetFileApisToOEM();
					if( GetFileAttributes( __argv[iIndex] ) != -1 )
						m_pScanManager->OverrideSetting(SMSETTING_OEM, true);
					SetFileApisToANSI();
				}

				m_pScanManager->AddItemToScanList(__argv[iIndex]);
			}
			else
			{
				if (CheckItem(__argv[iIndex]) == true)
				{
					m_pScanManager->AddItemToScanList(__argv[iIndex]);
				}
			}
		}
	}

	// Set the Scan name
    //
    // There's no task name, must be a temporary task like a Shell Extension scan.
    // Use "Custom scan" string instead.
    //
    if ( m_strScanName.GetLength() == 0 )
    {
		g_ResLoader.LoadString(IDS_CUSTOM_SCAN_TASK, m_strScanName);      
    }

	m_pScanManager->SetScanName( m_strScanName );

	// If we are told not to scan items, nuke the list.

	if (m_bShouldAddItems == false)
	{
		m_pScanManager->ResetScanList();
	}

	// Kick off the scan if there are items to scan or the user wants us to
	// scan memory or boot records only.

	if ((m_bShouldAddItems == false) ||
		(m_pScanManager->GetScanListItemCount() > 0))
	{
			Result = m_pScanManager->ScanAndRepair(g_szNavAppIdNAVW32);
	}
	
	// Update the last run time for task files.

	if (Result == SMRESULT_OK)
	{
		UpdateTaskTimes();
	}
	
	// Set the infection status depending on how many infections were found/fixed

	if (Result != SMRESULT_OK && Result != SMRESULT_ABORT)
		NavwResult = NAVWRESULT_ERROR;
	else if(m_pScanManager->GetTotalInfectionsFound() == 0)
		NavwResult = NAVWRESULT_NOINFECTIONS;
	else if(m_pScanManager->GetTotalInfectionsFound() != m_pScanManager->GetTotalInfectionsFixed())
		NavwResult = NAVWRESULT_INFECTIONS_REMAIN;
	else
		NavwResult = NAVWRESULT_ALLINFECTIONS_CLEAN;
	
	// Set the infection status depending on how many infections were found/fixed

	if (Result != SMRESULT_OK && Result != SMRESULT_ABORT)
		NavwResult = NAVWRESULT_ERROR;
	else if(m_pScanManager->GetTotalInfectionsFound() == 0)
		NavwResult = NAVWRESULT_NOINFECTIONS;
	else if(m_pScanManager->GetTotalInfectionsFound() != m_pScanManager->GetTotalInfectionsFixed())
		NavwResult = NAVWRESULT_INFECTIONS_REMAIN;
	else
		NavwResult = NAVWRESULT_ALLINFECTIONS_CLEAN;
	
	// Clean-up.

    if( m_pScanManager != NULL )
    {
        m_pScanManager->Release();
        m_pScanManager = NULL;
    }

	// Set the previous error mode.

	SetErrorMode(uErrorMode);

	// Re-enable Auto-Protect.

	if (bDisableAP == true)
	{
		m_APWrapper.EnableAPForThisThread();
	}

	// We're done!

	return Result;
}										  

bool CNavw32::ProcessTaskFile(const char* szSwitch)
{
    CSymInterfaceLoader taskLoader;
	IScanTaskPtr pScanTask;

    // Load the scan task object
    TCHAR szTasksDll[MAX_PATH] = {0};
    CNAVInfo NavInfo;
    _stprintf(szTasksDll, _T("%s\\NAVTasks.dll"), NavInfo.GetNAVDir());
    if( SYM_OK != taskLoader.Initialize(szTasksDll) )
        return false;

    if( SYM_OK != taskLoader.CreateObject(IID_IScanTask, IID_IScanTask, (void**)&pScanTask) )
        return false;

	const char* pszTaskFilePath = _tcschr(szSwitch, _T(':')) + 1;

    // Load the task file.
	if (!pScanTask->Load(pszTaskFilePath))
	{
		return false;
	}

    // we only want to allow 1 instance of Scan My Computer
    //  running at a time
    if(scanMyComputer == pScanTask->GetType())
    {
        if(IsAlreadyRunning("scanMyComputer", true))
        {
            return true;
        }
    }

    // we only want to allow 1 instance of QuickScan
    //  running at a time
    else if(scanQuick == pScanTask->GetType())
    {
        if(IsAlreadyRunning("scanQuick", true))
        {
            return true;
        }
    }

	// Set the Scan Name 
	m_strScanName = pScanTask->GetName();

	pScanTask->EnumReset();

	// Iterate through the list of tasks.

	for (UINT uiItems = pScanTask->GetItemCount(), ui = 0U; ui < uiItems; ui++)
	{
		// Add the next item to Scan Manager's items to scan list.

        SCANITEMTYPE type;
        SCANITEMSUBTYPE subtype;
        TCHAR chDrive = _TCHAR(' ');
        TCHAR szItemPath[2*MAX_PATH] = {0};
        unsigned long ulVID = 0;

		if (!pScanTask->EnumNext(type, subtype, chDrive, szItemPath, 2*MAX_PATH, ulVID))
		{
			return false;
		}

        // If this is a drive replace the item path with the drive to scan

		if (type == typeDrive)
		{
			_stprintf(szItemPath, "%c:", chDrive);
		}

		// If the item is a folder whose sub-folders should also be scanned,
		// add it directly to the scan list. Otherwise, only add files inside
		// the folder. If this is a filename\VID item pass in the VID as well.

        if(type == typeFileVID || type == typeFileVIDCompressed)
        {
            m_pScanManager->AddItemToScanList(szItemPath, ulVID, (type == typeFileVIDCompressed ? true : false));
        }
        // Make sure the item exists.
        else if( CheckItem(szItemPath) == false )
        {
            continue;
        }
		else if (subtype == subtypeFolder)
		{
			FillWithFiles(szItemPath);
		}
		else
		{
			m_pScanManager->AddItemToScanList(szItemPath);
		}
	}

    // If this is a quick scan task type then set up the quick scan to run
    if( pScanTask->GetType() == scanQuick )
    {
        // Turn on eraser scan and the ProcessSwitch method will do the work
        // of over riding the setting in scan manager for us
        m_bEnableEraserScan = true;

        // Don't scan other items
        m_bShouldAddItems = false;
    }

	// If we're dealing with a non-temporary task file, save its name/path.
	// We'll update its last run time when the scan completes.

	if (_tcsnicmp(szSwitch, SWITCH_TTASKFILE, _tcslen(SWITCH_TTASKFILE)) != 0)
	{
		m_TaskFiles.push_back(pszTaskFilePath);

        // For non-temporary task files, set the Eraser scanning flag to what
        // the user option is set to
        m_pScanManager->OverrideSetting(SMSETTING_RUN_ERASER_SCAN, m_bEnableEraserScan);
	}

	return true;
}

bool CNavw32::ProcessSwitch(const char* szSwitch)
{
	if (_tcsicmp(szSwitch, SWITCH_ALL_DRIVES) == 0)
	{
        //
		// Scan all drives (except A: and B:).
        //

        // For the all drives switch, set the eraser scanning flag to what
        // the user option is set to
        m_pScanManager->OverrideSetting(SMSETTING_RUN_ERASER_SCAN, m_bEnableEraserScan);

		FillWithDrives();
	}
	else if (_tcsicmp(szSwitch, SWITCH_LOCAL_DRIVES) == 0)
	{
		// Scan all local drives (except A: and B:).
		// This is equivalent to the "My Computer" scan task file.

		CNAVInfo NavInfo;
		
		char szTask[MAX_PATH];
		_stprintf(szTask, "/TASK:%s\\Tasks\\%s", NavInfo.GetNAVCommonDir(), SZ_SCANTASKFILENAME_MY_COMPUTER );

		return ProcessTaskFile(szTask);
	}
	else if (_tcsicmp(szSwitch, SWITCH_ENABLE_SUBFOLDERS) == 0)
	{
		// Scan sub-folders.

		m_pScanManager->OverrideSetting(SMSETTING_SCAN_SUBFOLDERS, true);
	}
	else if (_tcsicmp(szSwitch, SWITCH_DISABLE_SUBFOLDERS) == 0)
	{
		// Don't scan sub-folders.

		m_pScanManager->OverrideSetting(SMSETTING_SCAN_SUBFOLDERS, false);
	}
	else if (_tcsicmp(szSwitch, SWITCH_ENABLE_BOOTREC) == 0)
	{
		// Scan boot records.

		m_pScanManager->OverrideSetting(SMSETTING_SCAN_BOOTRECS, true);
	}
	else if (_tcsicmp(szSwitch, SWITCH_BOOTREC_ONLY) == 0)
	{
		m_pScanManager->OverrideSetting(SMSETTING_SCAN_MBR, true);
		m_pScanManager->OverrideSetting(SMSETTING_SCAN_MEMORY, false);
		m_pScanManager->OverrideSetting(SMSETTING_SCAN_BOOTRECS, true);
		m_pScanManager->OverrideSetting(SMSETTING_SCAN_FILES, false);

        m_bShouldAddItems = false;
	}
	else if (_tcsicmp(szSwitch, SWITCH_MEMORY_ONLY) == 0)
	{
		m_pScanManager->OverrideSetting(SMSETTING_SCAN_MBR, false);
		m_pScanManager->OverrideSetting(SMSETTING_SCAN_MEMORY, true);
		m_pScanManager->OverrideSetting(SMSETTING_SCAN_BOOTRECS, false);
		m_pScanManager->OverrideSetting(SMSETTING_SCAN_FILES, false);
		
		// Just like the man said. Only memory. Useless on NT.

		m_bShouldAddItems = false;
	}
	else if (_tcsicmp(szSwitch, SWITCH_DISABLE_BOOTREC) == 0)
	{
		// Don't scan boot records and master boot records.

		m_pScanManager->OverrideSetting(SMSETTING_SCAN_MBR, false);
		m_pScanManager->OverrideSetting(SMSETTING_SCAN_BOOTRECS, false);
	}
	else if (_tcsicmp(szSwitch, SWITCH_ENABLE_MEMORY) == 0)
	{
		// Scan memory.

		m_pScanManager->OverrideSetting(SMSETTING_SCAN_MEMORY, true);
	}
	else if (_tcsicmp(szSwitch, SWITCH_DISABLE_MEMORY) == 0)
	{
		// Don't scan memory.

		m_pScanManager->OverrideSetting(SMSETTING_SCAN_MEMORY, false);
	}
    else if (_tcsicmp(szSwitch, SWITCH_ENABLE_ERASER_SCAN) == 0)
	{
		// Scan with Eraser.

        m_bEnableEraserScan = true;
		m_pScanManager->OverrideSetting(SMSETTING_RUN_ERASER_SCAN, true);
	}
	else if (_tcsicmp(szSwitch, SWITCH_DISABLE_ERASER_SCAN) == 0)
	{
		// Don't scan with eraser.

        m_bEnableEraserScan = false;
		m_pScanManager->OverrideSetting(SMSETTING_RUN_ERASER_SCAN, false);
	}
	else if (_tcsnicmp(szSwitch, SWITCH_HEURISTICS, _tcslen(SWITCH_HEURISTICS)) == 0)
	{
		// Set Bloodhound sensitivity level.

		TCHAR chLevel = szSwitch[_tcslen(SWITCH_HEURISTICS)];

		if (chLevel < _T('0') || chLevel > _T('3'))
		{
			return false;
		}

		m_pScanManager->OverrideBloundhoundLevel(chLevel - _T('0'));
	}
	else if (_tcsnicmp(szSwitch, SWITCH_TASKFILE,  _tcslen(SWITCH_TASKFILE))  == 0
	      || _tcsnicmp(szSwitch, SWITCH_TTASKFILE, _tcslen(SWITCH_TTASKFILE)) == 0)
	{
		// Read task file and add the entries to the scan list.

		return ProcessTaskFile(szSwitch);
	}
	else if (_tcsnicmp(szSwitch, SWITCH_NORESULTS, _tcslen(SWITCH_NORESULTS)) == 0)
	{
		// Don't display results.

		m_pScanManager->SetUserInterface(SMUI_NO_RESULTS);
	}
	else if (_tcsnicmp(szSwitch, SWITCH_AUTOMATION, _tcslen(SWITCH_AUTOMATION)) == 0)
	{
		// Don't display any UI.

		m_pScanManager->SetUserInterface(SMUI_NO_UI);
	}
	else if(_tcsnicmp(szSwitch, SWITCH_OFFICEAV, _tcslen(SWITCH_OFFICEAV)) == 0)
	{
		// Set our flag for OfficeAV scanning
		m_bIsOfficeScan = true;

		// Setup Options for OfficeAV scanning
		// Scan in ANSI mode.
		m_pScanManager->OverrideSetting(SMSETTING_OEM, false);

		// Turn off memory, boot records and master boot record scanning.
		m_pScanManager->OverrideSetting(SMSETTING_SCAN_MEMORY, false);
		m_pScanManager->OverrideSetting(SMSETTING_SCAN_BOOTRECS, false);
		m_pScanManager->OverrideSetting(SMSETTING_SCAN_MBR, false);

		// Turn off progress during repairs.
		// Our UI runs into a bug with Office 2000.
		// Refer to Microsoft knowledgebase article Q246018 for more info.
		m_pScanManager->OverrideSetting(SMSETTING_SHOW_REPAIR_PROGRESS, false);

		// Don't display any UI unless there are infections.
		m_pScanManager->SetUserInterface(SMUI_NO_UI_UNLESS_INFECTIONS);


		// Only log infections, not scan starts and stops
		m_pScanManager->OverrideSetting(SMSETTING_LOG_START_STOP, false);
        
        // Turn off the scanning for non-viral threats for OfficeAV scans
        m_pScanManager->OverrideSetting(SMSETTING_SCAN_NON_VIRAL_THREATS, false);

        // Tell scan manager that this is an office scan so it will not load ccEraser
        // unless it has to, this will also cause scan manager to disable multi-threading and
        // eraser scanning
        m_pScanManager->OverrideSetting(SMSETTING_OFFICE, true);
	}
    else if(_tcsnicmp(szSwitch, SWITCH_SILENT, _tcslen(SWITCH_SILENT)) == 0)
    {
        // Don't display any UI unless there are infections.
        m_pScanManager->SetUserInterface(SMUI_NO_UI_UNLESS_INFECTIONS);

        // Only log infections, not scan starts and stops
        m_pScanManager->OverrideSetting(SMSETTING_LOG_START_STOP, false);

        // Do not perform an eraser scan
        m_pScanManager->OverrideSetting(SMSETTING_RUN_ERASER_SCAN, false);
        m_bEnableEraserScan = false;

        // Turn off memory, boot records and master boot record scanning.
        m_pScanManager->OverrideSetting(SMSETTING_SCAN_MEMORY, false);
        m_pScanManager->OverrideSetting(SMSETTING_SCAN_BOOTRECS, false);
        m_pScanManager->OverrideSetting(SMSETTING_SCAN_MBR, false);
    }
    else if(_tcsnicmp(szSwitch, SWITCH_SILENT_ERASER_SCAN, _tcslen(SWITCH_SILENT_ERASER_SCAN)) == 0)
	{
        // Don't display any UI unless there are infections.
		m_pScanManager->SetUserInterface(SMUI_NO_UI_UNLESS_INFECTIONS);

        // Perform an eraser scan
        m_pScanManager->OverrideSetting(SMSETTING_RUN_ERASER_SCAN, true);

        // Don't scan other items
        m_bShouldAddItems = false;

        // Set the scan task name to quick scan
        g_ResLoader.LoadString(IDS_QUICK_SCAN_TASK, m_strScanName);
    }
    else if(_tcsnicmp(szSwitch, SWITCH_QUICK_SCAN, _tcslen(SWITCH_QUICK_SCAN)) == 0)
    {
        // Perform an eraser scan
        m_pScanManager->OverrideSetting(SMSETTING_RUN_ERASER_SCAN, true);

        // Don't scan other items
        m_bShouldAddItems = false;

        // Set the scan task name to quick scan        
		g_ResLoader.LoadString(IDS_QUICK_SCAN_TASK, m_strScanName);
    }
    else if(_tcsnicmp(szSwitch, SWITCH_ENABLE_NONVIRAL, _tcslen(SWITCH_ENABLE_NONVIRAL)) == 0)
    {
        // Enable non-viral scanning
        m_pScanManager->OverrideSetting(SMSETTING_SCAN_NON_VIRAL_THREATS, true);
    }
    else if(_tcsnicmp(szSwitch, SWITCH_DISABLE_RELEVANCY, _tcslen(SWITCH_DISABLE_RELEVANCY)) == 0)
    {
        // Disable discovery of non-relevant risks
        m_pScanManager->OverrideSetting(SMSETTING_ONLY_RELEVANT_RISKS, false);
    }
	else
	{
		// Any other switch is invalid.

		return false;
	}

	return true;
}

void CNavw32::FillWithDrives()
{
	DWORD dwMask = 1;
	DWORD dwDrives = GetLogicalDrives();
	char szRootDir[] = "X:\\";

	for(char chDrive = _T('A'); chDrive <= _T('Z'); chDrive++, dwMask <<= 1)
	{
		// Skip drive A: and B:

		if (chDrive == _T('A') || chDrive == _T('B'))
		{
			continue;
		}

		if (dwDrives & dwMask)
		{
			szRootDir[0] = chDrive;

			// Make sure the disk exists (i.e. removable drives).

			if (DiskExists(szRootDir) == false)
			{
				continue;
			}

			// Add the drive to the items vector.

			m_pScanManager->AddItemToScanList(szRootDir);
		}
	}
}

bool CNavw32::FillWithFiles(const char* szDirectory)
{
	char szFileSpec[MAX_PATH];
	PathCombine(szFileSpec, szDirectory, "*.*");

	WIN32_FIND_DATA FindData;
	HANDLE hFind = FindFirstFile(szFileSpec, &FindData);

	if (hFind == INVALID_HANDLE_VALUE)
	{
		return false;
	}

	char szFilePath[MAX_PATH];

	do
	{
		// Skip sub-directories.

		if (FindData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
		{
			continue;
		}

		// Get the full path to the file.

		PathCombine(szFilePath, szDirectory, FindData.cFileName);

		// Add the file to the scan list. 

		m_pScanManager->AddItemToScanList(szFilePath);
	}
	while(FindNextFile(hFind, &FindData));

	FindClose(hFind);

	return true;
}

bool CNavw32::DiskExists(const char* szRootDir)
{
	char szVolumeName[32];
	DWORD dwComponentLen, dwFileSysFlags;
	
	if (!GetVolumeInformation(szRootDir, szVolumeName, sizeof(szVolumeName),
			NULL, &dwComponentLen, &dwFileSysFlags, NULL, 0))
	{
		return false;
	}

	return true;
}

bool CNavw32::ReadSettings()
{
	bool bResult = true;
	CNAVOptSettingsEx NavOpts;

	try
	{
		if (!NavOpts.Init())
		{
			throw runtime_error("CNavw32::ReadSettings() - Unable to initialize the options settings.");
		}

		DWORD dwValue;
	
		if (FAILED(NavOpts.GetValue(ACTIVITY_LogStartEnd, dwValue, 0)))
		{
            CCTRACEE("CNavw32::ReadSettings() - Unable to read the ACTIVITY:LogStartEnd setting. Setting to false");
		}

		m_bLogScanStartStop = (dwValue ? true : false);

        if (FAILED(NavOpts.GetValue(SCANNER_EnableSideEffectScan, dwValue, 0)))
		{
            CCTRACEE("CNavw32::ReadSettings() - Unable to read the SCANNER:EnableSideEffectScan setting. Setting to false");
		}

		m_bEnableEraserScan = (dwValue ? true : false);
	}
	catch(exception& Ex)
	{
		bResult = false;

		CCTRACEE(Ex.what());
	}

	return bResult;
}

bool CNavw32::CheckItem(const char* szItem)
{
	// First do a quick check to see if the item exists.

	DWORD dwAttribs = GetFileAttributes(szItem);

	if (dwAttribs != INVALID_FILE_ATTRIBUTES)
	{
		// If the item is a file, make sure we can open it.

		bool bResult = true;

		if ((dwAttribs & FILE_ATTRIBUTE_DIRECTORY) == 0)
		{
			HANDLE hFile = CreateFile(szItem, GENERIC_READ, FILE_SHARE_READ, NULL,
				                      OPEN_EXISTING, 0, NULL);

            // Make sure this is not a sneaky reserved DOS name used as a regular folder name
            bool bActuallyItsAValidDirectory = false;
            if( hFile == INVALID_HANDLE_VALUE )
            {
                ATL::CAtlString strFileName = _T("\\\\.\\");
                strFileName += szItem;
                dwAttribs = GetFileAttributes(strFileName);
                
                if( dwAttribs == FILE_ATTRIBUTE_DIRECTORY )
                {
                    CCTRACEI(_T("CNavw32::CheckItem() - %s is actually a directory not a file."), (LPCTSTR)strFileName);
                    bActuallyItsAValidDirectory = true;
                }
            }

			if (hFile != INVALID_HANDLE_VALUE)
			{
				CloseHandle(hFile);
			}
			else if( !bActuallyItsAValidDirectory )
			{
				bResult = false;
                CCTRACEW(_T("CNavw32::CheckItem() - %s is not a valid file. Showing CED."), szItem);
				NAVErrorDisplay(IDS_ACCESS_DENIED, szItem);
			}
		}

		return bResult;
	}
	
	// Display an error if the item is not a drive.

	if (IsItemADrive(szItem) == false)
	{
		NAVErrorDisplay(IDS_BAD_ITEM, szItem);
		return false;
	}

	// If the item is a drive, return true and scan it
	return true;
}

bool CNavw32::IsItemADrive(const char* szItem)
{
	if (_tcslen(szItem) < 2 || _tcslen(szItem) > 3)
	{
		return false;
	}

	if (IsCharAlpha(szItem[0]) == FALSE)
	{
		return false;
	}

	char szDrive1[] = "X:";
	szDrive1[0] = szItem[0];

	char szDrive2[] = "X:\\";
	szDrive2[0] = szItem[0];

	if (_tcsicmp(szItem, szDrive1) == 0)
	{
		return true;
	}

	if (_tcsicmp(szItem, szDrive2) == 0)
	{
		return true;
	}

	return false;
}

void CNavw32::UpdateTaskTimes()
{
	std::vector<string>::iterator Iter;

    // Load up the NavTasks dll to load the scantask object
    CSymInterfaceLoader taskLoader;
    TCHAR szTasksDll[MAX_PATH] = {0};
    CNAVInfo NavInfo;
    _stprintf(szTasksDll, _T("%s\\%s"), NavInfo.GetNAVDir(), _T("NAVTasks.dll"));

    if( SYM_OK == taskLoader.Initialize(szTasksDll) )
    {
	    for(Iter = m_TaskFiles.begin(); Iter != m_TaskFiles.end(); Iter++)
	    {
	        IScanTaskPtr pScanTask;

            // Load the scan task object
            if( SYM_OK == taskLoader.CreateObject(IID_IScanTask, IID_IScanTask, (void**)&pScanTask) )
            {
		        if (pScanTask->Load((*Iter).c_str(), false) == true)
		        {
			        time_t CurrentTime;
			        time(&CurrentTime);
			        pScanTask->SetLastRunTime(CurrentTime);
			        pScanTask->Save();
		        }
            }
	    }
    }
}

bool CNavw32::IsWildcardItem(const char* szItem)
{
	// Search for a wildcard character.
	
	char* pWild = _tcspbrk(szItem, "*?");

	if (pWild == NULL)
	{
		return false;
	}

	// Make sure the wildcard character was found AFTER any trailing backslash.

	if (_tcsrchr(szItem, '\\') > pWild)
	{
		return false;
	}

	return true;
}

bool CNavw32::ProcessWildcardItem(const char* szItem)
{
	// Get the item's directory.

	char szDirectory[MAX_PATH];

	if (ExtractWildcardDir(szItem, szDirectory) == false)
	{
		return false;
	}

	// Find matches and add them to the scan list.

	WIN32_FIND_DATA FindData;
	HANDLE hFind = FindFirstFile(szItem, &FindData);
	
	if (hFind == INVALID_HANDLE_VALUE)
	{
		return false;
	}

	char szFullPath[MAX_PATH];

	do
	{
		if (FindData.cFileName[0] == '.')
		{
			continue;
		}

		_tcscpy(szFullPath, szDirectory);
		_tcscat(szFullPath, FindData.cFileName);

		m_pScanManager->AddItemToScanList(szFullPath);
	}
	while(FindNextFile(hFind, &FindData));

	FindClose(hFind);

	return true;
}

bool CNavw32::ExtractWildcardDir(const char* szItem, char* szDirectory)
{
	// If an absolute path has been specified, translate .. and . sequences.

	if (PathIsUNC(szItem) || (PathGetDriveNumber(szItem) != -1))
	{
		if (PathCanonicalize(szDirectory, szItem) == FALSE)
		{
			return false;
		}
	}

	// Check for a directory with no drive letter, but with a leading backslash.
	// In this case prepend the current drive.

	else if (szItem[0] == '\\')
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

	if (PathRemoveFileSpec(szDirectory))
	{
		PathAddBackslash(szDirectory);
	}

	return true;
}

void CNavw32::DeleteTempTaskFiles()
{
	for (int iIndex = 1; iIndex < __argc; iIndex++)
	{
		if (_tcsnicmp(__argv[iIndex], SWITCH_TTASKFILE, _tcslen(SWITCH_TTASKFILE)) == 0)
		{
			const char* pszTaskFilePath = _tcschr(__argv[iIndex], _T(':')) + 1;

			if (pszTaskFilePath != NULL)
			{
				DeleteFile(pszTaskFilePath);
			}
		}
	}
}

void CNavw32::NAVErrorDisplay(UINT uStringID, const char* szItemName)
{
	// Get title
	CBrandingRes BrandRes;
	CString csProductName(BrandRes.ProductName());

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
    case IDS_ERROR_ERASER_REBOOT_REQUIRED:
        uSupportID = SUPPORT_ID_ERASER_REBOOT_REQUIRED;
        csMessage.Format(csFormat, csProductName);
        break;
	}

	STAHLSOFT_HRX_TRY(hr)
	{
		CComBSTR bstrNAVErr(_T("NAVError.NAVCOMError"));
		CComPtr<INAVCOMError> spNavError;
		hrx << spNavError.CoCreateInstance(bstrNAVErr, NULL, CLSCTX_INPROC_SERVER);

		// Populate the error
		hrx << spNavError->put_ModuleID(AV_MODULE_ID_NAVW);
		hrx << spNavError->put_ErrorID(uSupportID);

		CComBSTR bstrErrorMessage(csMessage);
		hrx << spNavError->put_Message(bstrErrorMessage);

		hrx << spNavError->Show(TRUE, FALSE, NULL);
	}
	STAHLSOFT_HRX_CATCH_ALL_NO_RETURN(hr)
}

BOOL CALLBACK CNavw32SwitchToProc(HWND hwnd, LPARAM lParam);

// SingleInstanceCheck - returns TRUE if another 
bool CNavw32::IsAlreadyRunning(LPCSTR szName, bool bSwitch)
{
    SYMRESULT sr = SYMERR_UNKNOWN;
    bool bAlreadyRunning = false;

    sr = m_SingleInstanceFactory.CreateObject(&m_spSingleInstance);
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
            CCTRACEI(_T("CNavw32::IsAlreadyRunning - Another process has already locked this instance. %X::%X", dwOwnerProcess, dwOwnerThread));
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
    static const DWORD dwClassNameSize = CCDIMOF(szCommonUIWindowClassName);
    static TCHAR szClassName[dwClassNameSize];
    static DWORD dwRet = NULL;
    
    // zero out the buffer, it might be dirty
    ZeroMemory(szClassName, dwClassNameSize);
    dwRet = GetClassName(hwnd, szClassName, dwClassNameSize);
    
    // if the size doesn't match, don't bother
    // with the other comparisons
    if(dwRet != (dwClassNameSize - 1) )
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

