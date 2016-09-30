#include "Stdafx.h"
#include <time.h>
#include "Resource.h"
#include "ScanManager.h"
#include "Quarantine.h"
#include "OSInfo.h"
#include "FileAPI.h"
#include "Utils.h"
#include "Shlwapi.h"
#include "NavInfo.h"
#include "Quar32.h"
#include "CategoryToggle.h"
#include "AVRESBranding.h"

#include "ccModuleNames.h"
#include "NAVError.h"
#include "ccModuleId.h"
#include "AVccModuleId.h"
#include "NAVVer.h"

#include "NAVSettingsHelperEx.h"
#include "navopt32.h"

#define NAVAPPIDS_BUILDING
#include "NAVAppIDs.h"
#include "switches.h"

#include "ccSymStringImpl.h"
#include "ccSymMemoryStreamImpl.h"
#include "ccSymFileStreamImpl.h"
#include "ccSymIndexValueCollectionImpl.h"
#include "ccSymKeyValueCollectionImpl.h"

#include "ScanPause.h"
#include "ResourceHelper.h"

#include <ShlObj.h>

#pragma once
// Lean
#define WIN32_LEAN_AND_MEAN		
#define VC_EXTRALEAN

// ATL defines
#define _ATL_NO_AUTOMATIC_NAMESPACE
#define _ATL_CSTRING_EXPLICIT_CONSTRUCTORS	
#define _ATL_ALL_WARNINGS
// Disable warnings that can safely be ignored
#include "ccIgnoreWarnings.h"
// Include windows
#include <windows.h>
#include <tchar.h>

// For QA automation reg key querying
#include <atlbase.h>
#include <atlstr.h>

// These are the defined names for the reboot remediation handling
static const TCHAR REMEDIATION_DATA_FOLDER[] = _T("RemediationData");
static const TCHAR REMEDIATION_RUNONCE_KEY[] = _T("POSTRBT");

using namespace std;

#undef DBGBRK
#if defined(_DEBUG) && defined(WIN32)
    #define DBGBRK() __asm { int 3 }
#else
    #define DBGBRK() (void)0
#endif


// Show only relevant detections
//  --- TEMP --- TEMP --- TEMP
//  need to make this part of the ScanMgrInterface
#define SMSETTING_ONLY_RELEVANT_RISKS (SMSETTING_OFFICE + 1)

ccLib::CCriticalSection CScanManager::m_critScanSink;

///////////////////////////////////////////////////////////////////////////////
//
// Construction.
//
CScanManager::CScanManager() : m_cSmartScan(m_SmartScan)
{
    // break here $&
    // ::MessageBox ( ::GetDesktopWindow (), "ScanMgr", "Start", MB_OK );

    // We do file comparison in lower case mode. According to MSDN not all 
    // characters will convert properly to lower case unless setlocale() is 
    // called.
    // MSDN note:
    //      "You will need to call setlocale for _wcsicmp to work with 
    //       Latin 1 characters. The C locale is in effect by default, 
    //       so, for example, ä will not compare equal to Ä.  Call 
    //       setlocale with any locale other than the C locale before 
    //       the call to _wcsicmp"
    //
    // Setting locale with a blank string to set the native environment
    setlocale(LC_ALL, "");

    try
    {
        m_pEraserScanSink = NULL;
        m_pEraserCallback = NULL;
        m_pScanSink = NULL;
        m_pScanProperties = NULL;
        m_pScanItems = NULL;

        m_pScanner = NULL;
        m_bAbortScan = false;
        m_bPauseScan = false;
        m_pCommonUI = NULL;
        m_UserInterface = SMUI_COMPLETE;
        m_hWndParent = NULL;
        m_hUIReadyEvent = NULL;
        m_hUIResumeEvent = NULL;
        m_dwTimeToScan = 0;
        m_lTotalTimePaused = 0;
        m_lCurTimePaused = 0;
        m_dwMaxDecomposerExtractSize = 0x7FFFFFFF;
        m_iMaxDecomposerExtractDepth = 10;
        m_strScanName = "";
        m_NonViralResponse = THREAT_SCAN_MODE_MANUAL;
        m_dwLastKnownThreatCategory = SCAN_THREAT_TYPE_JOKE;
        m_dwFirstNonViralThreatCategory = SCAN_THREAT_TYPE_SECURITY_RISK;
        m_iBloodhoundLevel = 2;
        m_bEraserScanning = false;
        m_bAdditionalScan = false;
        m_bFullScan = false;
        m_dwScanThreads = 0;
        m_bLogFiles = false;
        m_pActivityLog = NULL;
        ZeroMemory(m_szTestDefs, sizeof(m_szTestDefs));
        m_dwScanDllDepth = 10;
        m_bTerminateProcessPrompted = false;
        m_bTerminateProcessAccepted = false;
        m_bAutomaticMode = true;
        m_lDamageControlItems = 0;
        m_bIgnoreOnAnomalyDetected = false;

        // Initialize default settings.

        m_Settings[SMSETTING_OEM] = true;
        m_Settings[SMSETTING_EMAIL] = false;
        m_Settings[SMSETTING_IM] = false,
        m_Settings[SMSETTING_OEH] = false;
        m_Settings[SMSETTING_OEH_SILENT] = false;
        m_Settings[SMSETTING_OFFICE] = false;
        m_Settings[SMSETTING_ALERT] = false;
        m_Settings[SMSETTING_BACKUP_REPAIRS] = true;
        m_Settings[SMSETTING_IN_MEMORY_DECOMPOSITION] = true;
        m_Settings[SMSETTING_SCAN_SUBFOLDERS] = true;
        m_Settings[SMSETTING_SCAN_MEMORY] = true;
        m_Settings[SMSETTING_SCAN_BOOTRECS] = true;
        m_Settings[SMSETTING_SCAN_MBR] = true;
        m_Settings[SMSETTING_SCAN_COMPRESSED_FILES] = true;
        m_Settings[SMSETTING_SMARTSCAN] = false;
        m_Settings[SMSETTING_LOG_START_STOP] = false;
        m_Settings[SMSETTING_LOG_INFECTIONS] = true;
        m_Settings[SMSETTING_SHOW_REPAIR_PROGRESS] = true;
        m_Settings[SMSETTING_SCAN_FILES] = true;
        m_Settings[SMSETTING_SCAN_NETWORK_FILES] = true;
        m_Settings[SMSETTING_SCAN_NON_VIRAL_THREATS] = true;
        m_Settings[SMSETTING_BACKUP_DELETED_THREATS] = true;
        m_Settings[SMSETTING_RUN_ERASER_SCAN] = false;
        m_Settings[SMSETTING_DONT_SCAN_MEMORY] = false;
        m_Settings[SMSETTING_SCAN_DLLS] = false;
        m_Settings[SMSETTING_SCAN_CLIENT_COMPLIANCY] = false;
        m_Settings[SMSETTING_SCAN_GENERIC_LOAD_POINTS] = true;
        m_Settings[SMSETTING_ONLY_RELEVANT_RISKS] = true;

        // Initialize the default threat category enabled settings
        for( DWORD i=m_dwFirstNonViralThreatCategory; 
             i <= m_dwLastKnownThreatCategory; i++ )
        {
            m_CategoryEnabled[i] = true;
        }

        // Now attempt to initialize these values from the navopts.dat file
        Initialize();

        // Only enable the files scanned logging if there is a specific registry key created
        // and the debug log settings to log information is set
        CRegKey regFileKey;
        if( ERROR_SUCCESS == regFileKey.Open(HKEY_LOCAL_MACHINE, _T("SOFTWARE\\Symantec\\Debug\\DebugOutput"), KEY_READ) )
        {
            DWORD dwVal = 0;
            if( ERROR_SUCCESS == regFileKey.QueryDWORDValue( _T("FilesScannedLog"), dwVal) )
            {
                if( dwVal != 0 )
                {
                    m_bLogFiles = true;
                    CCTRACEI("The ScanManager file logging registry key is enabled. If information ccTrace is enabled files scanned will be logged to FilesScanned.log");
                }
            }
        }

		// Get product name
		CBrandingRes BrandRes;
		m_strProductName = BrandRes.ProductName();
    }
    catch(...)
    {
        CCTRACEE("CScanManager::CScanManager() - Caught an exception creating the scan manager");
    }
}

CScanManager::~CScanManager()
{
	// Clean-up.
	if (m_pCommonUI != NULL)
	{
		delete m_pCommonUI;
        m_pCommonUI = NULL;
	}

    // Free the eraser objects
    if( m_pAnomalyList != NULL )
    {
        CCTRACEI(_T("CScanManager::~CScanManager() - Releasing the anomaly list object."));
        m_pAnomalyList.Release();
    }

    if( m_pNonRelevantAnomalyList != NULL )
    {
        CCTRACEI(_T("CScanManager::~CScanManager() - Releasing the non relevant anomaly list object."));
        m_pNonRelevantAnomalyList.Release();
    }

    if( m_pEraser != NULL )
    {
        CCTRACEI(_T("CScanManager::~CScanManager() - Releasing the eraser object."));
        m_pEraser.Release();
    }

    if( m_pInputFilter != NULL )
    {
        CCTRACEI(_T("CScanManager::~CScanManager() - Releasing the input filter object."));
        m_pInputFilter.Release();
    }

    if( m_pOutputFilter != NULL )
    {
        CCTRACEI(_T("CScanManager::~CScanManager() - Releasing the output filter object."));
        m_pOutputFilter.Release();
    }

    if( m_pEraserScanSink != NULL )
    {      
		// Make sure this sink never calls back into this instance of the scan manager since it will be gone
        m_pEraserScanSink->DeInitialize();

        CCTRACEI(_T("CScanManager::~CScanManager() - Releasing the eraser scan sink object."));
        unsigned long ulRef = m_pEraserScanSink->Release();
        CCTRACEI(_T("CScanManager::~CScanManager() - m_pEraserScanSink has a ref count of %d after releasing. address %p"), ulRef, m_pEraserScanSink);

        m_pEraserScanSink = NULL;
    }

    if( m_pEraserCallback != NULL )
    {
        // Make sure this sink never calls back into this instance of the scan manager since it will be gone
        m_pEraserCallback->DeInitialize();

        CCTRACEI(_T("CScanManager::~CScanManager() - Releasing the eraser callback object."));
        unsigned long ulRef = m_pEraserCallback->Release();
        CCTRACEI(_T("CScanManager::~CScanManager() - m_pEraserCallback has a ref count of %d after releasing. address %p"), ulRef, m_pEraserCallback);

        m_pEraserCallback = NULL;
    }

    // Free our IScanner object
    if (m_pScanner != NULL)
    {
        unsigned long ulRef = m_pScanner->Release();
        CCTRACEI(_T("CScanManager::~CScanManager() - m_pScanner has a ref count of %d after releasing. address %p"), ulRef, m_pScanner);
        m_pScanner = NULL;
    }

    // Free our ccScan sink implementations
    if( m_pScanSink != NULL )
    {
        // Make sure this sink never calls back into this instance of the scan manager since it will be gone
        m_pScanSink->DeInitialize();

        CCTRACEI(_T("CScanManager::~CScanManager() - Releasing the regular scan sink object."));
        unsigned long ulRef = m_pScanSink->Release();
        CCTRACEI(_T("CScanManager::~CScanManager() - m_pScanSink has a ref count of %d after releasing. address %p"), ulRef, m_pScanSink);

        m_pScanSink = NULL;
    }

    if( m_pScanProperties != NULL )
    {
        // Make sure this sink never calls back into this instance of the scan manager since it will be gone
        m_pScanProperties->DeInitialize();

        CCTRACEI(_T("CScanManager::~CScanManager() - Releasing the scan properties object."));
        unsigned long ulRef = m_pScanProperties->Release();
        CCTRACEI(_T("CScanManager::~CScanManager() - m_pScanProperties has a ref count of %d after releasing. address %p"), ulRef, m_pScanProperties);

        m_pScanProperties = NULL;
    }

    if( m_pScanItems != NULL )
    {
        // Make sure this sink never calls back into this instance of the scan manager since it will be gone
        m_pScanItems->DeInitialize();

        CCTRACEI(_T("CScanManager::~CScanManager() - Releasing the scan items object."));
        unsigned long ulRef = m_pScanItems->Release();
        CCTRACEI(_T("CScanManager::~CScanManager() - m_pScanItems has a ref count of %d after releasing. address %p"), ulRef, m_pScanItems);

        m_pScanItems = NULL;
    }

	if (m_hUIReadyEvent != NULL)
	{
		CloseHandle(m_hUIReadyEvent);
	}

	if (m_hUIResumeEvent != NULL)
	{
		CloseHandle(m_hUIResumeEvent);
	}
}

void CScanManager::Initialize()
{
	// Initialize the SmartScan class.
	if (m_SmartScan.Initialize() == false)
	{
        CCTRACEE("Error initializing extension list - defaulting to scanning all files.");
	}

    // Initialize exclusions
    CCTRACEI(_T("CScanManager::Initialize() - Initializing the exclusions."));
    if( m_Exclusions.Initialize() == false )
    {
        CCTRACEE(_T("CScanManager::Initialize() - Error while initializing exclusions."));
    }
    CCTRACEI(_T("CScanManager::Initialize() - Finished initializing the exclusions."));

	// Read settings from NAV options.
	DWORD dwValue;
	CNAVOptSettingsEx NavOpts;

	try
	{
        if( NavOpts.Init() )
        {
		    if (FAILED(NavOpts.GetValue(SCANNER_Memory, dwValue, 0)))
                CCTRACEE("Unable to read the memory setting.");
            else
                m_Settings[SMSETTING_SCAN_MEMORY] = (dwValue ? true : false);
    		
		    if (FAILED(NavOpts.GetValue(SCANNER_BootRecs, dwValue, 0)))
                CCTRACEE("Unable to read the boot record setting.");
            else
                m_Settings[SMSETTING_SCAN_BOOTRECS] = (dwValue ? true : false);

		    if (FAILED(NavOpts.GetValue(SCANNER_MasterBootRec, dwValue, 0)))
			    CCTRACEE("Unable to read the master boot record setting.");
            else
		        m_Settings[SMSETTING_SCAN_MBR] = (dwValue ? true : false);
    		
		    if (FAILED(NavOpts.GetValue(GENERAL_ScanZipFiles, dwValue, 0)))
			    CCTRACEE("Unable to read the compressed file setting.");
            else
		        m_Settings[SMSETTING_SCAN_COMPRESSED_FILES] = (dwValue ? true : false);

		    if (FAILED(NavOpts.GetValue(SCANNER_ScanAll, dwValue, 0)))
			    CCTRACEE("Unable to read the scan all files setting.");
            else
		        m_Settings[SMSETTING_SMARTSCAN] = (dwValue ? false : true);

		    if (FAILED(NavOpts.GetValue(SCANNER_HeuristicLevel, dwValue, 0)))
			    CCTRACEE("Unable to read the Bloodhound setting.");
            else
		        m_iBloodhoundLevel = (int) dwValue;

		    if (FAILED(NavOpts.GetValue(SCANNER_ResponseMode, dwValue, 0)))
			    CCTRACEE("Unable to read the response mode setting.");
            else
		        m_Response = (SMRESPONSE) dwValue;

		    if (FAILED(NavOpts.GetValue(SCANNER_BackupRep, dwValue, 0)))
			    CCTRACEE("Unable to read the backup repairs setting.");
            else
		        m_Settings[SMSETTING_BACKUP_REPAIRS] = (dwValue ? true : false);

		    if (FAILED(NavOpts.GetValue(ACTIVITY_LogKnown, dwValue, 0)))
			    CCTRACEE("Unable to read the log known infections setting.");
            else
		        m_Settings[SMSETTING_LOG_INFECTIONS] = (dwValue ? true : false);
	
		    if (FAILED(NavOpts.GetValue(SCANNER_ScanNetwork_Files, dwValue, 0)))
			    CCTRACEE("Unable to read the network file setting.");
            else
		        m_Settings[SMSETTING_SCAN_NETWORK_FILES] = (dwValue ? true : false);

			// If there is no THREAT:NoThreatCat setting or it is 0 then make sure the
			// threat categorization values are read in
			if(FAILED(NavOpts.GetValue(THREAT_NoThreatCat, dwValue, 0)) || (dwValue == 0))
            {
                if (FAILED(NavOpts.GetValue(SCANNER_ThreatCatEnabled, dwValue, 0)))
			        CCTRACEE("Unable to read the threat categorization enabled setting.");
                else
                    m_Settings[SMSETTING_SCAN_NON_VIRAL_THREATS] = (dwValue ? true : false);
    	
		        if (FAILED(NavOpts.GetValue(SCANNER_ThreatCatResponse, dwValue, 0)))
			        CCTRACEE("Unable to read the threat categorization response mode setting.");
                else
                    m_NonViralResponse = (SMNONVIRALRESPONSE) dwValue;
    	
		        if (FAILED(NavOpts.GetValue(SCANNER_BackUpThreat, dwValue, 0)))
			        CCTRACEE("Unable to read the backup threats setting.");
                else
                    m_Settings[SMSETTING_BACKUP_DELETED_THREATS] = (dwValue ? true : false);

		        if (FAILED(NavOpts.GetValue(THREAT_LastKnownCategory, dwValue, SCAN_THREAT_TYPE_JOKE)))
			        CCTRACEE("Unable to read the threat last known category option.");
                else
                    m_dwLastKnownThreatCategory = dwValue;

		        if (FAILED(NavOpts.GetValue(THREAT_FirstKnownNonViral, dwValue, SCAN_THREAT_TYPE_SECURITY_RISK)))
			        CCTRACEE("Unable to read the threat first known category option.");
                else
                    m_dwFirstNonViralThreatCategory = dwValue;

                // If all categories are disabled then disable threat categorization scanning
                // completely
                bool bACategoryIsEnabled = false;
                for( DWORD i=m_dwFirstNonViralThreatCategory; i <= m_dwLastKnownThreatCategory; i++ )
                {
                    // Create the string for the current threat category option
                    TCHAR szCurCatOpt[MAX_PATH] = _T("THREAT:Threat");
                    TCHAR szCurCatNum[20] = {0};
                    _tcscat(szCurCatOpt, _itot(i, szCurCatNum, 10));

		            if (FAILED(NavOpts.GetValue(szCurCatOpt, dwValue, 0)))
			            CCTRACEE("Unable to read threat category enabled setting = %s", szCurCatOpt);
                    else
                    {
                        if( dwValue != 0 )
                        {
                            m_CategoryEnabled[i] = true;
                            bACategoryIsEnabled = true;
                        }
                        else
                            m_CategoryEnabled[i] = false;
                    }
                }

                if( !bACategoryIsEnabled )
                {
                    CCTRACEI("CScanManager::Initialize() - All threat categories are disabled so scanning for non-viral threats is being turned off.");
                    m_Settings[SMSETTING_SCAN_NON_VIRAL_THREATS] = false;
                }
            }
            else
            {
                m_Settings[SMSETTING_SCAN_NON_VIRAL_THREATS] = false;
			    CCTRACEI("The NoThreatCat value is set. Disabling threat categorization scanning.");
            }

            if (FAILED(NavOpts.GetValue(SCANNER_ScanThreads, dwValue, 0)))
            {
                // The number of scan threads to use was not set in the settings
                // use the number of processors for the # of threads
                SYSTEM_INFO si;
			    GetSystemInfo(&si);

                DWORD dwNumProcs = GetNumberOfPhysicalProcessors();

                if( dwNumProcs == 1 )
                    m_dwScanThreads = 0;
                else
			        m_dwScanThreads = dwNumProcs;

                CCTRACEI("Using the # of physical processors to determine the number of threads to scan with. Using %d threads (Note single processor machines will run single-threaded aka: 0 scan threads).", m_dwScanThreads);
            }
            else
            {
                CCTRACEI("%s setting will set the scanner to use %d threads.", SCANNER_ScanThreads, dwValue);
                m_dwScanThreads = dwValue;
            }

            if (FAILED(NavOpts.GetValue(SCANNER_DisableMemoryScan, dwValue, 0)))
                CCTRACEI("Unable to read the disable side effect memory scan setting.");
            else
                m_Settings[SMSETTING_DONT_SCAN_MEMORY] = (dwValue ? true : false);

            if( SUCCEEDED(NavOpts.GetValue(SCANNER_TestDefs, m_szTestDefs, MAX_PATH, "")) )
            {
                CCTRACEI("CScanManager::Initialize() - Using test defs: %s", m_szTestDefs);
            }

            if (FAILED(NavOpts.GetValue(SCANNER_ScanDlls, dwValue, 0)))
                CCTRACEI("Unable to read the scan DLLs setting.");
            else
                m_Settings[SMSETTING_SCAN_DLLS] = (dwValue ? true : false);

            if (FAILED(NavOpts.GetValue(SCANNER_ScanDllDepth, dwValue, 10)))
                CCTRACEI("Unable to read the scan DLL depth setting.");
            else
                m_dwScanDllDepth = dwValue;

            if (FAILED(NavOpts.GetValue(SCANNER_ScanClientCompliancy, dwValue, 0)))
                CCTRACEI("Unable to read the scan client compliancy setting.");
            else
                m_Settings[SMSETTING_SCAN_CLIENT_COMPLIANCY] = (dwValue ? true : false);
        }
        else
		{
			CCTRACEE("Unable to initialize the options library.");

			// If in safe mode, ccSettings is not available, so get 
			// THREAT_NoThreatCat from NAVOpts.dat.  For the rest of the 
			// settings, we'll just use the hard coded defaults.

			if(0 != GetSystemMetrics(SM_CLEANBOOT))
			{
				CCTRACEI(_T("%s - Running in safe mode."), __FUNCTION__);

				TCHAR szNAVOpts[MAX_PATH] = {0};
				HNAVOPTS32 hOptions = NULL;
				DWORD dwSize = MAX_PATH;
				DWORD dwValue = 0;
				HKEY hKey = NULL;

				CNAVInfo NAVInfo;
				_tcscpy(szNAVOpts, NAVInfo.GetNAVOptPath());

				NAVOPTS32_STATUS Status = NavOpts32_Allocate(&hOptions);
				if(NAVOPTS32_OK == Status)
				{
					// initialize the NAVOpts.dat option file.
					Status = NavOpts32_Load(szNAVOpts, hOptions, true);
					if(NAVOPTS32_OK == Status)
					{
						Status = NavOpts32_GetDwordValue(hOptions, THREAT_NoThreatCat, &dwValue, 0);
						if(NAVOPTS32_OK == Status)
						{
							m_Settings[SMSETTING_SCAN_NON_VIRAL_THREATS] = (0 == dwValue);
							CCTRACEI(_T("NoThreatCat value=%s"), __FUNCTION__, dwValue);
						}
					}
					else
					{
						CCTRACEE(_T("%s - Unable to load the NAVOpts.dat option file."), __FUNCTION__);
					}
				}
				else
				{
					CCTRACEE(_T("%s - Unable to initialize options library."), __FUNCTION__);
				}

				// Clean up
				if(hOptions)
					NavOpts32_Free(hOptions);
			}
		}
	}
	catch(exception& Ex)
	{
        CCTRACEE(_T("Caught Exception in CScanManager::Initialize(): "),Ex.what());
	}
}

///////////////////////////////////////////////////////////////////////////////
//
// IScanManager implementation.
//

bool CScanManager::AddItemToScanList(const char* szItem)
{
    CCTRACEI(_T("CScanManager::AddItemToScanList() - Argument is %s"), szItem);

	bool bResult = true;

	try
	{
        // Don't add duplicates
        std::vector<std::string>::iterator Iter;
        for( Iter = m_ItemsToScan.begin(); Iter != m_ItemsToScan.end(); Iter++ )
        {
            if( _tcsicmp(Iter->c_str(), szItem) == 0 )
            {
                CCTRACEI(_T("CScanManager::AddItemToScanList() - %s is a duplicate item, not adding to scan list."), szItem);
                return true;
            }
        }

		if(m_Settings[SMSETTING_SCAN_NETWORK_FILES])
			m_ItemsToScan.push_back(szItem);
		else
		{
			if(!IsNetworkDrive(szItem))
				m_ItemsToScan.push_back(szItem);
		}
	}
	catch(exception& Ex)
	{
		bResult = false;

		CCTRACEE(Ex.what());
	}

	return bResult;
}

bool CScanManager::AddItemToScanList(const char* szItem, unsigned long ulVID, bool bCompressed)
{
    CCTRACEI(_T("CScanManager::AddItemToScanList() - Item argument is %s, VID argument is %lu and compressed is %s" ), szItem, ulVID, (bCompressed ? "true" : "false"));

    if( szItem == NULL )
        return false;

    if( bCompressed )
    {
        CCTRACEI(_T("CScanManager::AddItemToScanList() - %s is a compressed item adding to regular file scan list."), szItem);
        AddItemToScanList(szItem);
        return true;
    }

    // Create an IString for the path
    cc::IStringPtr pStrFile;
    pStrFile.Attach(ccSym::CStringImpl::CreateStringImpl(szItem));
    if( !pStrFile )
    {
        CCTRACEE(_T("CScanManager::AddItemToScanList() - Failed to create IStringImpl for item %s"), szItem);
        return false;
    }

    cc::IIndexValueCollectionPtr spPaths;

    // Do we already have a hit for this VID?
    FILEVIDMAP::iterator Iter = m_mapFileVidsToScan.find(ulVID);
    if( Iter != m_mapFileVidsToScan.end() )
    {
        // Yes, get the strings
        spPaths = (*Iter).second;
        if( !spPaths )
        {
            CCTRACEE(_T("CScanManager::AddItemToScanList() - Failed to get existing paths for %lu"), ulVID);
            return false;
        }

        CCTRACEI(_T("CScanManager::AddItemToScanList() - Adding path = %s VID = %lu to an existing collection."), szItem, ulVID);
    }
    else
    {
        // Create an empty index collection for the paths        
        spPaths.Attach(ccSym::CIndexValueCollectionImpl::CreateIndexValueCollectionImpl());

        // Add the collection to our map
        m_mapFileVidsToScan[ulVID] = spPaths;

        CCTRACEI(_T("CScanManager::AddItemToScanList() - Adding path = %s VID = %lu to a new collection."), szItem, ulVID);
    }

    spPaths->AddValue(pStrFile);

    return true;
}

bool CScanManager::IsNetworkDrive(const char* szItem)
{
	// Get the drive root.
	
	char szDrive[_MAX_PATH];
	char szDir[_MAX_DIR];
	char szFileName[_MAX_FNAME];
	char szExt[_MAX_EXT];
	
	_splitpath(szItem, szDrive, szDir, szFileName, szExt);
	
	if (PathIsUNC(szItem))
		return true;

	// Do not add files on network drives to our scan list.

	if(GetDriveType(szDrive) != DRIVE_REMOTE)
		return false;

	return true;  
}

void CScanManager::ResetScanList()
{
	m_ItemsToScan.clear();
    m_mapFileVidsToScan.clear();
}

int CScanManager::GetScanListItemCount()
{
	return m_ItemsToScan.size() + m_mapFileVidsToScan.size();
}

void CScanManager::SetParentWindow(HWND hWnd)
{
	m_hWndParent = hWnd;
}

//
// This returns the total number of items detected both viral and non-viral
//
int CScanManager::GetTotalInfectionsFound()
{
	return m_ScanMgrCounts.GetTotalItemsDetected();
}

// This returns the number of items processed successfully both viral and non-viral
int CScanManager::GetTotalInfectionsFixed()
{
	return m_ScanMgrCounts.GetTotalItemsResolved();
}

void CScanManager::DeleteObject()
{
	// Decrement our reference count.
	Release();
}

SMRESULT CScanManager::ScanAndRepair(const char* szAppID)
{
	SMRESULT Result = SMRESULT_OK;

    // Check if this is the post reboot processing call
    if ( _tcsnicmp(szAppID, SWITCH_REMEDIATE_AFTER_REBOOT, 
         _tcslen(SWITCH_REMEDIATE_AFTER_REBOOT)) == 0 )
    {
        CCTRACEI("CScanManager::ScanAndRepair() - Attempting to perform post reboot remediations");
        PerformPostRebootRemediations();
        return SMRESULT_OK;
    }

    m_strAppID = szAppID;

    CCTRACEI("CScanManager::ScanAndRepair() - Scanning for %d items with app ID %s", m_ItemsToScan.size(), m_strAppID.c_str());

    // Clean-up the remains of the previous scan.
    m_bAbortScan = false;
    m_bPauseScan = false;
    m_bTerminateProcessPrompted = false;
    m_bTerminateProcessAccepted = false;
    m_bAutomaticMode = true;
    m_bFullScan = false;
    m_bAdditionalScan = false;
    m_lDamageControlItems = 0;

    // Set up our infection merging class
    m_InfectionMerge.empty();
    if( m_Settings[SMSETTING_OEM] == false )
    {
        m_InfectionMerge.SetToANSIMode();
    }

    // Reset all of our scan counts
    m_ScanMgrCounts.Reset();

    if (!m_eventScanError.IsEvent())
        m_eventScanError.Create(NULL, TRUE, FALSE, NULL, FALSE);
    else
        m_eventScanError.ResetEvent();

	if (m_hUIReadyEvent != NULL)
	{
		CloseHandle(m_hUIReadyEvent);
		m_hUIReadyEvent = NULL;
	}

	if (m_hUIResumeEvent != NULL)
	{
		CloseHandle(m_hUIResumeEvent);
		m_hUIResumeEvent = NULL;
	}

	// Reset the display names class.
	m_DisplayNames.SetScanType(m_Settings[SMSETTING_EMAIL],
		(m_Response == SCAN_MODE_AUTO_REPAIR_DELETE));

	// Create the logger object.
	m_pActivityLog = new CActivityLog();

	if (m_pActivityLog == NULL)
    {
        CCTRACEE(_T("CScanManager::ScanAndRepair() - Failed to create activity log class. No logging will be available."));
    }

    // Sanity check to ensure we don't run an Eraser scan for office, IM, or email scans, also run
    // these scans single threaded
    if( m_Settings[SMSETTING_OEH_SILENT] || m_Settings[SMSETTING_OEH] ||
        m_Settings[SMSETTING_IM] || m_Settings[SMSETTING_EMAIL] || m_Settings[SMSETTING_OFFICE] )
    {
        CCTRACEI(_T("CScanManager::ScanAndRepair() - This is an Office, IM, or Email scan we will scan single threaded and disable eraser and generic load point scanning"));
        // Always run single threaded scans
        m_dwScanThreads = 0;

        // Disable eraser scanning
        m_Settings[SMSETTING_RUN_ERASER_SCAN] = false;

        // Do not run a generic load point scan if we get a detection during
        // these scans
        m_Settings[SMSETTING_SCAN_GENERIC_LOAD_POINTS] = false;
    }

	// Initialize OEH if this feature is enabled.
	if (m_Settings[SMSETTING_OEH] == true)
	{
		m_OEHScan.Open(m_Settings[SMSETTING_OEH_SILENT], &m_Quarantine,
			m_pActivityLog, m_dwClientPID, m_strClientImage,
			m_strEmailSubject, m_strEmailSender, m_strEmailRecipient);
	}

    // Get the scanner object for the normal file scan if necessary
    if( m_pScanner == NULL )
    {
        CCTRACEI(_T("CScanManager::ScanAndRepair() - Loading the ccScan Scanner"));
        long lFlags = 0;

        if (m_Settings[SMSETTING_OEM] == true)
        {
            CCTRACEI(_T("CScanManager::ScanAndRepair() - Loading the ccScan Scanner in OEM mode"));
            lFlags |= IScanner::ISCANNER_OEM_SCAN;
        }

        if (m_Settings[SMSETTING_IN_MEMORY_DECOMPOSITION] == true)
        {
            CCTRACEI(_T("CScanManager::ScanAndRepair() - Using in memory decomposition"));
            lFlags |= IScanner::ISCANNER_USE_INMEMORY_DECOMPOSER;
        }

        SMRESULT Result = GetScanner(m_strAppID.c_str(), m_iBloodhoundLevel, lFlags);

        if (Result != SMRESULT_OK || m_pScanner == NULL)
        {
            CCTRACEE(_T("CScanManager::ScanAndRepair() - Failed to get the ccScan IScanner object. Error = %d. Aborting the scan."), Result);
			m_pActivityLog->LogScanError();
            if( Result == SMRESULT_ERROR_DEF_AUTHENTICATION_FAILURE )
            {
                // For def authentication failures the Scan Manager has already alerted
                // the user so we should return Abort to the client
                CCTRACEE(_T("CScanManager::ScanAndRepair() - Error. Definition authentication has failed"));
                
                if( m_pActivityLog != NULL )
                    delete m_pActivityLog;

                return SMRESULT_ABORT;
            }
			
            return Result;
        }
        CCTRACEI(_T("CScanManager::ScanAndRepair() - Completed loading the IScanner object"));
    }

    // Load up ccEraser
    if( m_pEraser == NULL )
    {
        Result = GetEraser();
        if (Result != SMRESULT_OK)
        {
            CCTRACEE(_T("CScanManager::ScanAndRepair() - Failed to load Eraser objects. Aborting."));

            if( m_pActivityLog != NULL )
                delete m_pActivityLog;

            return Result;
        }

        // Set ccEraser settings
        Result = SetEraserSettings();
        if (Result != SMRESULT_OK)
        {
            CCTRACEE(_T("CScanManager::ScanAndRepair() - Failed to set Eraser settings. Aborting."));

            if( m_pActivityLog != NULL )
                delete m_pActivityLog;

            return Result;
        }
    }

    // Free the list if it still exists
    if( m_pAnomalyList != NULL )
        m_pAnomalyList.Release();

    // Create the anomaly list object
    ccEraser::eResult resLoadList = m_pEraser->CreateObject(ccEraser::AnomalyListType ,ccEraser::IID_AnomalyList, reinterpret_cast<void**>(&m_pAnomalyList));
    if( ccEraser::Failed(resLoadList) || m_pAnomalyList == NULL )
    {
        CCTRACEE(_T("CScanManager::ScanAndRepair() - Error creating anomaly list. ccEraser::eResult = %d"), resLoadList);
        return SMRESULT_ERROR_ERASER;
    }
	
	// Log the start of the scan.
	if (m_Settings[SMSETTING_LOG_START_STOP] == true && m_pActivityLog != NULL )
	{
		m_pActivityLog->LogScanStart(m_strScanName);
	}

	// Disable Auto-Protect.
	m_AutoProtectToggle.DisableAPForThisThread();

	// Make sure there is 20 Megs of free disk space so we can deal with infections.
	CNAVInfo NavInfo;
	ULARGE_INTEGER FreeBytesAvailable, TotalNumberOfBytes, TotalNumberOfFreeBytes;
	
	GetDiskFreeSpaceEx(NavInfo.GetNAVDir(), &FreeBytesAvailable, &TotalNumberOfBytes, &TotalNumberOfFreeBytes);
    if( TotalNumberOfFreeBytes.QuadPart < (20 * 1024 * 1024) )
    {
        CString szMessageBuf;
        LONG lMessageSize;
        TCHAR szMessage[MAX_PATH];
        LoadString(g_hInstance, IDS_INSUFFICIENT_DISK_SPACE, szMessage, MAX_PATH);

        // Get Drive Letter	
        TCHAR szDrive[ _MAX_DRIVE ];
        _tsplitpath(NavInfo.GetNAVDir(), szDrive, NULL, NULL, NULL );

        lMessageSize = _tcslen(szMessage) + (2*_tcslen(szDrive)) + _tcslen(m_strProductName) + 1;
        _stprintf(szMessageBuf.GetBuffer(lMessageSize), szMessage, m_strProductName, szDrive, szDrive);
        szMessageBuf.ReleaseBuffer();
        MessageBox(NULL, szMessageBuf ,m_strProductName, MB_OK);
    }

	// Tell Windows that we'll take care of critical errors.
	UINT uOriginalErrorMode = SetErrorMode(SEM_FAILCRITICALERRORS);

    // Open the quarantine object for the scan
    CCTRACEI(_T("CScanManager::ScanAndRepair() - Opening quarantine object."));
    if( !m_Quarantine.Open(m_pScanner) )
        CCTRACEE(_T("CScanManager::ScanAndRepair() - Failed to open the quarantine wrapper."));
    CCTRACEI(_T("CScanManager::ScanAndRepair() - Finished open for quarantine object."));

	// Perform the scan/repair.
	if ((m_UserInterface == SMUI_COMPLETE) || (m_UserInterface == SMUI_NO_RESULTS))
	{
		Result = ScanAndRepairWithProgress();
	}
	else
	{
		Result = ScanAndRepairWithoutProgress();
	}

	// Restore the previous error mode.
	SetErrorMode(uOriginalErrorMode);

	// Close the OEH engine.
	if (m_Settings[SMSETTING_OEH] == true)
	{
		m_OEHScan.Close();
	}

    // Notify AP that this scan is complete
    m_NotifyAPSpywareDetection.ScanComplete();

	// Re-enable Auto-Protect.
	m_AutoProtectToggle.EnableAPForThisThread();

	// Log infections.
    
	if (m_Settings[SMSETTING_LOG_INFECTIONS] == true && m_pActivityLog != NULL )
	{
		// Make sure we have infections to log.
        if( m_pAnomalyList != NULL && m_pScanner != NULL &&
            m_ScanMgrCounts.GetTotalItemsDetected() > 0 )
		{
			// Set the date of the defs in the logger
            //
            CScanPtr <IScanDefinitionsInfo> spDefInfo;
            if ( SCAN_OK == m_pScanner->GetDefinitionsInfo ( &spDefInfo ))
            {
                if ( spDefInfo )
                {
                    //
                    // Create time structure containing def date.
                    //

                    // Time in secs / secs / hours / days
                    time_t timeDefTime;
                    SYSTEMTIME stimeDefDate;
                    struct tm * ptmDefDate;

                    // Get def date from the engine
                    //
                    timeDefTime = spDefInfo->GetDate ();

                    // Convert def date
                    //
                    ptmDefDate = localtime ( &timeDefTime );

                    if( ptmDefDate )
                    {
                        stimeDefDate.wYear = (ptmDefDate->tm_year) + 1900;
                        stimeDefDate.wMonth = (ptmDefDate->tm_mon) + 1;  // talk about retarded, 0-11 for months...
                        stimeDefDate.wDay = ptmDefDate->tm_mday;

                        unsigned long ulRev = spDefInfo->GetRevision();

                        // YYYYMMDDRRRR = 12 chars
                        //
                        TCHAR szDefsRevision [13] = {0};    
                        _stprintf ( szDefsRevision, _T("%04u%02u%02u%04u"), stimeDefDate.wYear, stimeDefDate.wMonth, stimeDefDate.wDay, ulRev );
                        m_pActivityLog->SetDefsRevision ( szDefsRevision );
                    }
                    else
                    {
                        CCTRACEE(_T("CScanManager::ScanAndRepair() - Error getting the definitions date."));
                    }
                }
            }   // end def date

            ActivityScanType scanType = AType_ManualScanner;
            if( m_Settings[SMSETTING_OFFICE] == true )
                scanType = AType_OfficeScanner;
            else if( m_Settings[SMSETTING_IM] == true )
                scanType = AType_IMScanner;
            else if( m_Settings[SMSETTING_EMAIL] == true )
                scanType = AType_EmailScanner;
            
            m_pActivityLog->SetScanType(scanType);
            CCTRACEI(_T("CScanManager::ScanAndRepair() - Logging all infections."));
            m_pActivityLog->LogAllInfections(m_pAnomalyList);
            CCTRACEI(_T("CScanManager::ScanAndRepair() - Finished logging infections."));
		}
	}

	// Log the end of the scan.
	if (m_Settings[SMSETTING_LOG_START_STOP] == true && m_pActivityLog != NULL)
	{
		switch(Result)
		{
		case SMRESULT_OK:
			m_pActivityLog->LogScanSummary(&m_ScanMgrCounts, m_dwTimeToScan, m_strScanName);
			break;
		case SMRESULT_ABORT:
			m_pActivityLog->LogScanAbort(m_strScanName);
			break;
		default:
			m_pActivityLog->LogScanError();
			break;
		}
	}

	// Delete ActivityLog Object
    if( m_pActivityLog )
	    delete m_pActivityLog;

    // Clear vid to anomaly map used for this scan, although it should be at 0 now
    CCTRACEI(_T("CScanManager::ScanAndRepair() - Map of infections to anomalies contains %d items. clearing."), m_mapInfectionsToAnomalies.size());
    m_mapInfectionsToAnomalies.clear();

    // Clear the anomaly list from this scan since it may have references to quarantine items
    // and we don't want to keep the memory around anyway
    if( m_pAnomalyList != NULL )
    {
        CCTRACEI(_T("CScanManager::ScanAndRepair() - Releasing the anomaly list"));
        m_pAnomalyList.Release();
        CCTRACEI(_T("CScanManager::ScanAndRepair() - Released the anomaly list"));
    }

    // Close the quarantine wrapper class.
    CCTRACEI(_T("CScanManager::ScanAndRepair() - Closing quarantine object."));
    m_Quarantine.Close();
    CCTRACEI(_T("CScanManager::ScanAndRepair() - Closed quarantine object."));

    // Empty the reboot file paths
    m_vRebootFiles.clear();

    // Clear the infection merge data
    m_InfectionMerge.empty();

	// We're done!
	return Result;
}

void CScanManager::OverrideSetting(SMSETTING Setting, bool bEnable)
{
	m_Settings[Setting] = bEnable;
}

void CScanManager::OverrideResponse(SMRESPONSE Response)
{
	m_Response = Response;
}

void CScanManager::OverrideNonViralResponse(SMNONVIRALRESPONSE Response)
{
    m_NonViralResponse = Response;
}

void CScanManager::OverrideBloundhoundLevel(int iLevel)
{
	m_iBloodhoundLevel = iLevel;
}

void CScanManager::OverrideMaxDecomposerExtractSize(unsigned long uBytes)
{
	m_dwMaxDecomposerExtractSize = uBytes;
}

void CScanManager::OverrideMaxDecomposerExtractDepth(int iLevels)
{
	m_iMaxDecomposerExtractDepth = iLevels;
}

void CScanManager::SetUserInterface(SMUSERINTERFACE UserInterface)
{
    CCTRACEI(_T("CScanManager::SetUserInterface() - Setting user interface to %d"), UserInterface);
	m_UserInterface = UserInterface;
}

void CScanManager::SetScanName(const char* szScanName)
{
	m_strScanName = szScanName;
}

void CScanManager::SetEmailInfo(const char* szSubject, const char* szSender,
								const char* szRecipient)
{
	m_strEmailSubject = szSubject;
	m_strEmailSender = szSender;
	m_strEmailRecipient = szRecipient;
}

void CScanManager::SetOEHInfo(DWORD dwClientPID, const char* szClientImage)
{
	m_dwClientPID = dwClientPID;
	m_strClientImage = szClientImage;
}

///////////////////////////////////////////////////////////////////////////////
//
// IScanSink implementation.
//

SCANSTATUS CScanManager::OnBusy()
{
	return HandlePauseAbort(_T("OnBusy"));
}

SCANSTATUS CScanManager::OnError( IScanError* pError )
{
    //
    // (GVOGEL)
    // NOTE:  IT IS NOW POSSIBLE FOR THIS METHOD TO BE CALLED ON MULTIPLE THREADS!
    //        This means that any objects you use or calls you make should be thread-
    //        safe. Great care has been taken to avoid any need for synchronization
    //        (like critical sections) as well in order to keep performance optimum.
    //

	// Let the user know about out of disk space errors.

	SCANSTATUS Status = SCAN_OK;

	if ((pError->GetFileName() != NULL) &&
		(pError->GetErrorCode() == SCAN_ERROR_DISK_FULL))
	{
        // See if the user has already decided to continue
        if (m_eventScanError.Lock(0, TRUE))
            return SCAN_OK;
        
        // Prevent multiple "out of disk space errors"
        m_eventScanError.SetEvent();

		TCHAR szFormat[MAX_PATH];
		LoadString(g_hInstance, IDS_ERROR_DISK_FULL, szFormat,
			sizeof(szFormat));
		TCHAR szMessage[MAX_PATH];
		wsprintf(szMessage, szFormat, m_strProductName, pError->GetFileName());
		
		if (MessageBox(NULL, szMessage, m_strProductName, MB_OK | MB_YESNO | MB_SETFOREGROUND) == IDNO)
        {
			Status = SCAN_ABORT;
        }
        else
        {
            // reset "out of disk space errors"
            m_eventScanError.ResetEvent();
        }
	}
    else
    {
        // reset "out of disk space errors"
        m_eventScanError.ResetEvent();
    }

    if (pError->GetErrorCode() == SCAN_ERROR_DEFINITIONS)
    {
        // Format text with product name
        CString strText, strFileName = pError->GetFileName();
        UINT uErrorId = NULL;
        if (!strFileName.IsEmpty())
            uErrorId = IDS_ERROR_DEFINITIONS;
        else
            uErrorId = IDS_ERROR_DEFINITIONS_NO_FILE;

        strText.Format(uErrorId, m_strProductName, strFileName);

        // Get a NAVError object
        CComBSTR bstrText(strText);

        CComBSTR bstrNAVErr(_T("NAVError.NAVCOMError"));
        CComPtr <INAVCOMError> spNavError;

        // Create, Populate, Log, and Display the error
        if( SUCCEEDED( spNavError.CoCreateInstance(bstrNAVErr, NULL, CLSCTX_INPROC_SERVER) ) )
        {
            if( SUCCEEDED(spNavError->put_ModuleID(AV_MODULE_ID_SCANMANAGER)) &&
                SUCCEEDED(spNavError->put_ErrorID(uErrorId)) &&
                SUCCEEDED(spNavError->put_Message(bstrText)) )
            {
                BOOL bDisplay = (m_UserInterface == SMUI_COMPLETE) || (m_UserInterface == SMUI_NO_RESULTS);
                spNavError->Show(bDisplay, TRUE, NULL);
            }
            else
            {
                CCTRACEE(_T("CScanManager::OnError() - Failed to fill out the NAV error object"));
            }
        }
        else
        {
            CCTRACEE(_T("CScanManager::OnError() - Failed to create the NAV error object"));
        }

    }

    return Status;
}

SCANSTATUS CScanManager::OnNewItem( const char * pszItem )
{
    //
    // (GVOGEL)
    // NOTE:  IT IS NOW POSSIBLE FOR THIS METHOD TO BE CALLED ON MULTIPLE 
    //        THREADS!
    //
    //        This means that any objects you use or calls you make should 
    //        be thread-safe. Great care has been taken to avoid any need 
    //        for synchronization (like critical sections) as well in order 
    //        to keep performance optimum.
    //
    return HandlePauseAbort(_T("OnNewItem"));
}

SCANSTATUS CScanManager::OnNewFile( const char * pszLongName, 
                                    const char * pszShortName )
{
    //
    // (GVOGEL)
    // NOTE:  IT IS NOW POSSIBLE FOR THIS METHOD TO BE CALLED ON MULTIPLE 
    //        THREADS!
    //
    //        This means that any objects you use or calls you make should 
    //        be thread-safe. Great care has been taken to avoid any need 
    //        for synchronization (like critical sections) as well in order 
    //        to keep performance optimum.
    //
    if( SCAN_ABORT == HandlePauseAbort(_T("OnNewFile")) )
    {
        return SCAN_ABORT;
    }

    // Check the file against SmartScan (if necessary).
    if (m_Settings[SMSETTING_SMARTSCAN] == true)
    {
        // Convert the file names to OEM.
        // SmartScan uses OEM file names.
        vector<char> vOemLongName;
        const char* pszOemLong;

        // Eraser scans are always run in ANSI mode
        if (m_Settings[SMSETTING_OEM] == true && !m_bEraserScanning)
        {
            pszOemLong = pszLongName;
        }
        else
        {
            int iBufferLenLong = _tcslen(pszLongName);
            vOemLongName.reserve(iBufferLenLong + 1);
            CharToOem(pszLongName, &vOemLongName[0]);
            pszOemLong = &vOemLongName[0];
        }

        if (m_cSmartScan.IsMatch(pszOemLong) == false)
        {
            return SCAN_FALSE;
        }
    }

    // Update the files scanned count
    m_ScanMgrCounts.IncrementItemCount(CScanManagerCounts::FilesScanned);

    // Update the file to display with this file
    m_ScanMgrCounts.UpdateCurrentFolder(pszLongName, m_Settings[SMSETTING_OEM]);

    // Log the file before scanning it ?
    if( m_bLogFiles ) 
    {
        g_FileDebugOutput.DebugOutputInformation(_T("New File to scan:\r\nLong = %s\r\nShort = %s"), pszLongName, pszShortName);
    }
	
    return SCAN_OK;
}

SCANSTATUS CScanManager::OnNewDirectory( const char * pszLongName, 
                                         const char * pszShortName )
{
    //
    // (GVOGEL)
    // NOTE:  IT IS NOW POSSIBLE FOR THIS METHOD TO BE CALLED ON MULTIPLE 
    //        THREADS!
    //
    //        This means that any objects you use or calls you make should 
    //        be thread-safe. Great care has been taken to avoid any need 
    //        for synchronization (like critical sections) as well in order 
    //        to keep performance optimum.
    //
    //
    if( SCAN_ABORT == HandlePauseAbort(_T("OnNewDirectory")) )
        return SCAN_ABORT;

    // Don't scan this directory if it's fully excluded and it's 
    // subdirectories are excluded
    NavExclusions::IExclusion::ExclusionState eState, requiredState;

    if(m_Settings[SMSETTING_SCAN_NON_VIRAL_THREATS] == true)
    {
        requiredState = NavExclusions::IExclusion::ExclusionState::EXCLUDE_ALL;
    }
    else
    {
        // Viral only
        requiredState = (NavExclusions::IExclusion::ExclusionState)
            (NavExclusions::IExclusion::ExclusionState::EXCLUDE_VIRAL |
             NavExclusions::IExclusion::ExclusionState::EXCLUDE_SUBDIRS_VIRAL);
    }

    eState = (m_Exclusions.IsPathMatch(pszLongName, m_Settings[SMSETTING_OEM], requiredState));

    if((eState & requiredState) == requiredState)
    {
        CCTRACEI(_T("CScanManager::OnNewDirectory() - Path exclusion exists for %s"), pszLongName);
        return SCAN_FALSE;
    }

	// If scan subfolders is disabled then only scan the folder if
	// it's an item in our scan list.

	if (m_Settings[SMSETTING_SCAN_SUBFOLDERS] == false)
	{
		std::vector<string>::iterator Iter;

		for(Iter = m_ItemsToScan.begin(); Iter != m_ItemsToScan.end(); Iter++)
		{
			if (_tcsicmp(pszLongName, (*Iter).c_str()) == 0)
			{
				return SCAN_OK;
			}
		}

        CCTRACEI(_T("CScanManager::OnNewDirectory() - Subfolder scanning is disabled and the folder %s is not in our list of scan items. Not scanning."), pszLongName);
		return SCAN_FALSE;
	}

	return SCAN_OK;
}

SCANSTATUS CScanManager::OnInfectionFound( IScanInfection* pInfection )
{
    //
    // (GVOGEL)
    // NOTE:  IT IS NOW POSSIBLE FOR THIS METHOD TO BE CALLED ON MULTIPLE 
    //        THREADS!
    //
    //        This means that any objects you use or calls you make should be 
    //        thread-safe. Since this method is not time critical event 
    //        (unlike OnNewFile), use of synchronization like critical 
    //        sections does not need to be avoided and can be used to ease 
    //        making this method thread safe.
    //
    // (Synchronization OK)
    //
    CCTRACEI(_T("CScanManager::OnInfectionFound() - BEGIN."));

    if ( SCAN_ABORT == HandlePauseAbort(_T("OnInfectionFound")) )
    {
        return SCAN_ABORT;
    }

    // Thread safety is obtained by Pausing all other scan threads running using
    // the ccScan pause method
    CScanPause scanPause(m_pScanner);

    // If this item is excluded ignore it now
    bool bNonViral = false;

    if ( IsNonViral(pInfection) )
    {
        bNonViral = true;

        if ( AreCategoriesDisabled(pInfection) )
        {
            CCTRACEI(_T("CScanManager::OnInfectionFound() - The categories for threat %s are disabled, non generating an anomaly"), pInfection->GetVirusName());
            return SCAN_FALSE;
        }
    }
    else if ( !IsViral(pInfection) )
    {
        CCTRACEW("CScanManager::OnInfectionFound() - This infection is not a known viral or non-viral category. Ignoring.");
        return SCAN_FALSE;
    }

    if (m_Exclusions.IsMatch(pInfection))
    {
        CCTRACEI(_T("CScanManager::OnInfectionFound() - Threat %s was excluded."), pInfection->GetVirusName());
        return SCAN_FALSE;
    }

    IScanBootInfectionQIPtr pBR = pInfection;
    IScanMBRInfectionQIPtr pMBR = pInfection;

    if (pBR.m_p != NULL || pMBR.m_p != NULL)
    {
        return DealWithBootScanInfection(pInfection);
    }

    // Compressed infections will have an anomaly generated just for the one 
    // compressed item and will be processed inline.
    IScanCompressedFileInfection2QIPtr pCompressed = pInfection;

    if ( pCompressed.m_p != NULL )
    {
        return OnCompressedInfectionFound(pCompressed, bNonViral);
    }

    // Do we already have this same scan infection item somewhere in an 
    // existing anomaly that has been detected?
    if ( m_InfectionMerge.CheckInfection(pInfection) )
    {
        CCTRACEW(_T("CScanManager::OnInfectionFound() - The scan infection item for %s already in an anomaly we have. We will ignore this item."), pInfection->GetVirusName());
        return SCAN_FALSE;
    }

    // Do we have a specific anomaly with the same VID? If so we need to just 
    // add this scan infection to that item
    if ( AddInfectionToExistingSpecificAnomaly(pInfection) )
    {
        CCTRACEI(_T("CScanManager::OnInfectionFound() - Infection %s vid = %lu was added to an exisitng specific anomaly"), pInfection->GetVirusName(), pInfection->GetVirusID());

        // Also add this infection to our list of infections detected
        if ( false == m_InfectionMerge.MergeInfection(pInfection) )
        {
            CCTRACEW(_T("CScanManager::OnInfectionFound() - The merge already reported that this item is detected..."));
        }

        return SCAN_FALSE;
    }

    CCTRACEI(_T("CScanManager::OnInfectionFound() - Generating an anomaly for infection = %s. vid = %lu."), pInfection->GetVirusName(), pInfection->GetVirusID());
    ccEraser::IAnomalyPtr pAnomaly;
    bool bGenerated = GenerateAnomalyFromInfection(pInfection, pAnomaly, 
                                                   InfectionStatus_Unhandled, 
                                                   ACTION_REPAIR);
    CCTRACEI(_T("CScanManager::OnInfectionFound() - Finshed Generating anomaly."));

    if ( !bGenerated || pAnomaly == NULL )
    {
        CCTRACEW(_T("CScanManager::OnInfectionFound() - GenerateAnomalyFromInfection faild for %s or GLP scan detected it already or it's remediation actions were not present."), pInfection->GetVirusName());
        return SCAN_FALSE;
    }

    CEZAnomaly EZAnomaly;

    if ( !EZAnomaly.Initialize(pAnomaly) )
    {
        CCTRACEE(_T("CScanManager::OnInfectionFound() - Failed to wrap generated anomaly"));
        return SCAN_FALSE;
    }

    HandleAnomaly(&EZAnomaly);

    // Return false so the scan engine does not attempt any operations
    return SCAN_FALSE;
}

SCANSTATUS CScanManager::OnCompressedInfectionFound(IScanCompressedFileInfection2* pCompressed, bool bNonViral)
{
    //
    // NOTE:  The function is only called from within OnInfectionFound() which is already made
    //        thread safe, so we will not obtain a lock here
    //
    // (Synchronization OK)
    //
    if( pCompressed == NULL )
    {
        CCTRACEE(_T("CScanManager::OnCompressedInfectionFound() - Compressed infection is null."));
        return SCAN_FALSE;
    }

    // Get the base scan infection
    IScanInfectionQIPtr pInfection = pCompressed;

    if( pInfection.m_p == NULL )
    {
        CCTRACEE(_T("CScanManager::OnCompressedInfectionFound() - Failed to QI for the base scan infection."));
        return SCAN_FALSE;
    }

    // Get the container name
    int nComponentCount = pCompressed->GetComponentCount();
    CAtlString strContainerName = pCompressed->GetComponent(nComponentCount-1);
    strContainerName.MakeLower();

    // For non-viral items, see if the container itself was already detected as an
    // IScanFileInfection item, in which case we can dispose of the scan infections inside the container
    if( bNonViral )
    {
        if( m_InfectionMerge.CheckContainer(pCompressed) )
        {
            CCTRACEW(_T("CScanManager::OnCompressedInfectionFound() - The container for the compressed infection item %s is already in an anomaly we have. We will ignore this item."), (LPCTSTR)strContainerName);
            return SCAN_FALSE;
        }
    }

    // Is there already a map entry for this item?
    COMPRESSED_ITEM_MAP::iterator Iter = m_mapCompressedItems.find(strContainerName);
    if( Iter == m_mapCompressedItems.end() )
    {
        // There is no entry in the map for this container, create a new one
        COMPRESSED_ITEM_VECTOR vItems;
        vItems.push_back(pCompressed);
        m_mapCompressedItems[strContainerName] = vItems;
    }
    else
    {
        // There is already an entry for this container, add to it
        (*Iter).second.push_back(pCompressed);
    }

    CCTRACEI(_T("CScanManager::OnCompressedInfectionFound() - This is a compressed infection."));

    if( !bNonViral ) // This is a virus
    {
        // Generate an anomaly for this compressed infection right now
        ccEraser::IAnomalyPtr spAnomCreated;
        bool bGenerated = GenerateAnomalyFromInfection(pInfection, spAnomCreated, InfectionStatus_Unhandled, ACTION_REPAIR);

        if( !bGenerated || spAnomCreated == NULL )
        {
            CCTRACEE(_T("CScanManager::OnCompressedInfectionFound() - Failed to generate viral anomaly."));
            return SCAN_FALSE;
        }

        if( m_Response != SCAN_MODE_MANUAL )
        {
            if( m_Settings[SMSETTING_BACKUP_REPAIRS] == true )
            {
                // Create a quarantine backup item
                std::string strFileName;
                m_DisplayNames.GetDisplayNameForFile(pInfection, strFileName);
                if( !m_Quarantine.Add(pInfection, QFILE_STATUS_BACKUP_FILE, false, strFileName.c_str()) )
                {
                    CCTRACEE(_T("CScanManager::OnCompressedInfectionFound() - Failed to backup viral compressed infection file %s to quarantine, not repairing automatically."), strFileName.c_str());

                    RemoveFromCompressedMap(pInfection);

                    // Don't do anything automatically
                    return SCAN_FALSE;
                }
            }

            CCTRACEI(_T("CScanManager::OnCompressedInfectionFound() - This is compressed virus infection , attempting to auto-repair."));

            // Keep track of the anomaly based on the infection so we can look it up if repair fails
            m_mapInfectionsToAnomalies[pInfection] = spAnomCreated.m_p;

            // Keep track of the fact that we are attempting auto-repair
            pInfection->SetUserData(InfectionStatus_Repaired);

            return SCAN_OK;
        }
        else
        {
            CCTRACEI(_T("CScanManager::OnCompressedInfectionFound() - This is a compressed virus, viral response is manual so not handling now."));
            
            // Remove this item from the post process container item map
            RemoveFromCompressedMap(pInfection);

            // Don't do anything automatically
            return SCAN_FALSE;
        }
    }
    else // This is non-viral
    {
        // Generate an anomaly for this infection right now
        ccEraser::IAnomalyPtr spAnomCreated;
        bool bGenerated = GenerateAnomalyFromInfection(pInfection, spAnomCreated, InfectionStatus_Unhandled, ACTION_DELETE);

        if( !bGenerated || spAnomCreated == NULL )
        {
            CCTRACEE(_T("CScanManager::OnCompressedInfectionFound() - Failed to generate non-viral anomaly."));
        }

        CEZAnomaly EZAnom(spAnomCreated);

        // Get the recommended action
        AnomalyAction action = ACTION_DELETE;
        EZAnom.GetRecommendedAction(action);

        if( m_NonViralResponse == THREAT_SCAN_MODE_AUTO_DELETE )
        {
            if( m_Settings[SMSETTING_BACKUP_DELETED_THREATS] == true )
            {
                // Create a quarantine backup item
                std::string strFileName;
                m_DisplayNames.GetDisplayNameForFile(pInfection, strFileName);
                if( !m_Quarantine.Add(pInfection, QFILE_STATUS_THREAT_BACKUP, false, strFileName.c_str()) )
                {
                    CCTRACEE(_T("CScanManager::OnCompressedInfectionFound() - Failed to backup non-viral compressed infection file %s to quarantine, not deleteing automatically."), strFileName.c_str());

                    // Set the recommended action
                    EZAnom.SetAnomalyStatus(action,
                                            InfectionStatus_Quarantine_Failed);

                    RemoveFromCompressedMap(pInfection);

                    // Don't do anything automatically
                    return SCAN_FALSE;
                }
            }

            CCTRACEI(_T("CScanManager::OnCompressedInfectionFound() - This is a compressed non-viral item, attempting to auto-delete."));

            // Keep track of the anomaly based on the infection in case the delete fails we will be able
            // to look up the anomaly
            m_mapInfectionsToAnomalies[pInfection] = spAnomCreated.m_p;
            
            // Keep track of the fact that we are attempting auto-delete
            pInfection->SetUserData(InfectionStatus_Deleted);

            // Do a deep delete for email scanning a regular delete for all others
            if( m_Settings[SMSETTING_EMAIL] == true )
                return SCAN_DEEP_DELETE;
            else
                return SCAN_DELETE;
        }
        else // The response mode is either manual or Norton Anti-Virus recommended
        {
            // If the response mode is to use the damage flags and the damage flags say auto-delete
            // then auto-delete, We ignore the dependency flag for compressed items
            if( m_NonViralResponse == THREAT_SCAN_MODE_USE_DAMAGE_FLAG && 
                ShouldAutoDeleteBasedOnDamageFlags(&EZAnom, false) )
            {
                // Backup if necessary
                if( m_Settings[SMSETTING_BACKUP_DELETED_THREATS] == true )
                {
                    // Create a quarantine backup item
                    std::string strFileName;
                    m_DisplayNames.GetDisplayNameForFile(pInfection, strFileName);
                    if( !m_Quarantine.Add(pInfection, QFILE_STATUS_THREAT_BACKUP, false, strFileName.c_str()) )
                    {
                        CCTRACEE(_T("CScanManager::OnCompressedInfectionFound() - Failed to backup non-viral compressed infection file %s to quarantine, not deleteing automatically."), strFileName.c_str());

                        EZAnom.SetAnomalyStatus(action, 
                                                InfectionStatus_Quarantine_Failed);

                        RemoveFromCompressedMap(pInfection);

                        // Don't do anything automatically
                        return SCAN_FALSE;
                    }
                }

                CCTRACEI(_T("CScanManager::OnCompressedInfectionFound() - This is a compressed non-viral item, using norton anti-virus recommended action which is to auto-delete."));

                // Keep track of the anomaly based on the infection in case the delete fails we will be able
                // to look up the anomaly
                m_mapInfectionsToAnomalies[pInfection] = spAnomCreated.m_p;

                // Keep track of the fact that we are attempting auto-delete
                pInfection->SetUserData(InfectionStatus_Deleted);

                // Do a deep delete for email scanning a regular delete for all others
                if( m_Settings[SMSETTING_EMAIL] == true )
                    return SCAN_DEEP_DELETE;
                else
                    return SCAN_DELETE;
            }
            // The mode is either manual -OR- use damage flags and damage flags say not to
            // auto-delete so either way we will use the symantec recommended action
            else
            {
                CString recommendation;

                switch (action)
                {
				    case ACTION_DELETE:
                        recommendation = "Delete";
                        break;

				    case ACTION_REVIEW:
                        recommendation = "Review";
                        break;

				    case ACTION_EXCLUDE:
                        recommendation = "Exclude";
                        break;

                    default:
                        //
                        // For non-virals, recommendation should always
                        // be one of the preceding three
                        //
                        recommendation = "Error";

                        CCTRACEE(_T("CScanManager::OnCompressedInfectionFound() - Got a non-viral recommended action %d.  Should only get delete, review or exclude"), action);
                        break;
                }

                if( m_NonViralResponse == THREAT_SCAN_MODE_USE_DAMAGE_FLAG )
                {
                    CCTRACEI(_T("CScanManager::OnCompressedInfectionFound() - This is a compressed non-viral item, using norton anti-virus recommended action which is to recommend %s."), recommendation);

                    // Bump up the number of damage control items that we have skipped processing on
                    m_lDamageControlItems++;
                }
                else
                {
                    CCTRACEI(_T("CScanManager::OnCompressedInfectionFound() - This is a compressed non-viral item, non-viral scanning response mode is manual, setting to recommended value of %s."), recommendation);
                }

                EZAnom.SetAnomalyStatus(action, InfectionStatus_Unhandled);

                // Remove this item from the post process container map
                RemoveFromCompressedMap(pInfection);

                // Don't do any automatic handling
                return SCAN_FALSE;
            }
        }
    }

    // default to scan false
    return SCAN_FALSE;
}

SCANSTATUS CScanManager::OnNewCompressedChild(
               ::cc::IDeccomposerContainerObject * pIDecomposerContainerObject,
               const char * pszChildName)
{
    if( SCAN_ABORT == HandlePauseAbort(_T("OnNewCompressedChild")) )
    {
        return SCAN_ABORT;
    }

    // Update the file count
    m_ScanMgrCounts.IncrementItemCount(CScanManagerCounts::FilesScanned);

    return SCAN_OK;
}

//
// This method will process any compressed items found during the scan with matching container path
// names.  If the path names are NULL then all items will be processed.
//
void CScanManager::OnPostProcessContainer(const char * pszLongName, const char * pszShortName )
{
    // Synchronize this callback but do not force all other threads to pause
    ccLib::CSingleLock Lock(&m_critScanSink, INFINITE, FALSE);

    if( m_mapCompressedItems.empty() )
        return;

    COMPRESSED_ITEM_MAP::iterator IterMap;

    //
    // Compressed items will now need the successfully processed anomalies set to the correct status
    //

    bool bProcessAll = (pszLongName == NULL && pszShortName == NULL);

    if( bProcessAll )
    {
        // Assign the map iterator to the first item
        IterMap = m_mapCompressedItems.begin();
        CCTRACEI(_T("CScanManager::OnPostProcessContainer(NULL,NULL) - Called for all remaining items. There are %d containers with items to be processed"), m_mapCompressedItems.size());
    }
    else
    {
        // Look for compressed path in our map
        CAtlString strName = pszLongName;
        strName.MakeLower();
        IterMap = m_mapCompressedItems.find(strName);
        if( IterMap == m_mapCompressedItems.end() )
        {
            strName = pszShortName;
            strName.MakeLower();
            IterMap = m_mapCompressedItems.find(strName);
            if( IterMap == m_mapCompressedItems.end() )
            {
                CCTRACEI(_T("CScanManager::OnPostProcessContainer() - Did not find any infections in this container to process. %s"), pszLongName);
                return;
            }
        }

        CCTRACEI(_T("CScanManager::OnPostProcessContainer() - Called for container %s. There are %d items in this container to process"), pszLongName, (*IterMap).second.size());
    }

    // If we are processing all items walk through the entire map, otherwise just
    // break out after the vector item we cared
    for( ; IterMap != m_mapCompressedItems.end(); IterMap++ )
    {
        CCTRACEI(_T("CScanManager::OnPostProcessContainer() - There are %d items in the current container to process"), (*IterMap).second.size());

        // Go through the vector of IterMap processing these items then clear it from the map
        COMPRESSED_ITEM_VECTOR::iterator IterVec;
        for( IterVec = (*IterMap).second.begin(); IterVec != (*IterMap).second.end(); IterVec++ )
        {
            // Set the status based on the infection status
            IScanInfectionQIPtr pQIScanInfection = (*IterVec);

            if( !pQIScanInfection )
            {
                CCTRACEE(_T("CScanManager::OnPostProcessContainer() - Failed to QI for the IScanInfection."));
                continue;
            }

            // Get the anomaly for this infection
            ccEraser::IAnomalyPtr spAnomaly;
            SCANINFECTIONTOANOMALYMAP::iterator Iter = m_mapInfectionsToAnomalies.find(pQIScanInfection);
            if( Iter == m_mapInfectionsToAnomalies.end()  )
            {
                CCTRACEE(_T("CScanManager::OnPostProcessContainer() - This anomaly is not in our map of infections."));
                continue;
            }
            else
            {
                spAnomaly = (*Iter).second;

                if( spAnomaly == NULL )
                {
                    CCTRACEE(_T("CScanManager::OnPostProcessContainer() - The anomaly in our map is bad."));
                    continue;
                }
            }

            CEZAnomaly ezAnom(spAnomaly);

            bool bNonViral = IsNonViral(pQIScanInfection);

            int nStatus = pQIScanInfection->GetStatus();

            AnomalyAction eAction;

            if( !bNonViral )
            {
                eAction = ACTION_REPAIR;
            }
            else
            {
                // This is a non-viral anomaly set the next action to the recommended one
                if( !ezAnom.GetRecommendedAction(eAction) )
                {
                    eAction = ACTION_DELETE;
                }
            }

            CommonUIInfectionStatus eStatus = InfectionStatus_Unhandled;

            // Update the anomaly status based on the status of the actual scan infection
            // item we were working with
            if( nStatus == IScanInfection::REPAIRED )
            {
                m_ScanMgrCounts.IncrementItemCount(CScanManagerCounts::Repaired);

                eStatus = InfectionStatus_Repaired;
                eAction = ACTION_COMPLETE;

                // Set the scan infection user data for status logging and display purposes
                pQIScanInfection->SetUserData(InfectionStatus_Repaired);
            }
            else if( nStatus == IScanInfection::DELETED )
            {
                eStatus = InfectionStatus_Deleted;
                eAction = ACTION_COMPLETE;

                // Was it actually quarantined?
                if( !bNonViral )
                {
                    long lUserData = pQIScanInfection->GetUserData();

                    if( lUserData == InfectionStatus_Quarantined )
                    {
                        eStatus = InfectionStatus_Quarantined;
                        m_ScanMgrCounts.IncrementItemCount(CScanManagerCounts::Quarantined);
                    }
                    else // The was either repair deleted or deleted
                    {
                        m_ScanMgrCounts.IncrementItemCount(CScanManagerCounts::ViralDeleted);

                        // Set the scan infection user data for status logging and display purposes
                        pQIScanInfection->SetUserData(InfectionStatus_Deleted);
                    }
                }
                else
                {
                    m_ScanMgrCounts.IncrementItemCount(CScanManagerCounts::NonViralDeleted);

                    // Set the scan infection user data for status logging and display purposes
                    pQIScanInfection->SetUserData(InfectionStatus_Deleted);
                }
            }
            else
            {
                //
                // This item is still infected
                //

                // If there is a backup item created delete it
                long lCurStatus = pQIScanInfection->GetUserData();

                if( (lCurStatus == InfectionStatus_Quarantined) ||
                    (bNonViral && lCurStatus == InfectionStatus_Deleted && m_Settings[SMSETTING_BACKUP_DELETED_THREATS] == true) ||
                    (!bNonViral && lCurStatus == InfectionStatus_Repaired && m_Settings[SMSETTING_BACKUP_REPAIRS] == true)
                    )
                {
                    // Delete the quarantine backup item
                    CCTRACEI(_T("CScanManager::OnPostProcessContainer() - Removing the quarantine backup item for this item because it is unhandled. Threat = %s"), pQIScanInfection->GetVirusName());
                    m_Quarantine.Delete(pQIScanInfection);
                }

                // Set status to unhandled
                pQIScanInfection->SetUserData(InfectionStatus_Unhandled);
            }

            // Set the anomaly status to the correct values
            CCTRACEI(_T("CScanManager::OnPostProcessContainer() - Setting anomaly action to %d and status to %d"), eAction, eStatus);
            ezAnom.SetAnomalyStatus(eAction, eStatus);

            // Set the remediation status text
            m_DisplayNames.UpdateAllRemediationStatusText(&ezAnom);

            // Remove this from the map
            m_mapInfectionsToAnomalies.erase(pQIScanInfection);
        }

        // clear the vector that was just processed
        (*IterMap).second.clear();

        // If we are not processing all items break out now
        if( !bProcessAll )
        {
            // remove this entry from the map
            m_mapCompressedItems.erase(IterMap);

            break;
        }
    }

    if( bProcessAll )
    {
        // Clear compressed items map
        m_mapCompressedItems.clear();
    }

    CCTRACEI(_T("CScanManager::OnPostProcessContainer() - Completed container processing. There are %d containers with items to be processed"), m_mapCompressedItems.size());
}

SCANSTATUS CScanManager::OnRepairFailed( IScanInfection* pInfection )
{
    //
    // (GVOGEL)
    // NOTE:  IT IS NOW POSSIBLE FOR THIS METHOD TO BE CALLED ON MULTIPLE THREADS!
    //        This means that any objects you use or calls you make should be thread-
    //        safe. Since this method is not time critical event (unlike OnNewFile),
    //        use of synchronization like critical sections does not need to be avoided
    //        and can be used to ease making this method thread safe.
    //
    // (Synchronization OK)
    //

    if( SCAN_ABORT == HandlePauseAbort(_T("OnRepairFailed")) )
        return SCAN_ABORT;

    // Synchronize access by pausing all other scan threads
    CScanPause pause(m_pScanner);

    CCTRACEI(_T("CScanManager::OnRepairFailed() - For infection %s."), pInfection->GetVirusName());

    if( m_Response == SCAN_MODE_AUTO_REPAIR_QUARANTINE )
    {
        // If a backup item exists convert it to a quarantine item
        bool bQuarantined = false;
        if( m_Settings[SMSETTING_BACKUP_REPAIRS] )
        {
            CSymPtr<IQuarantineItem> pQuarItem = NULL;
            if( m_Quarantine.RetrieveAssociatedQuarantineItem(pInfection, &pQuarItem) )
            {
                if( SUCCEEDED(pQuarItem->SetFileStatus(QFILE_STATUS_QUARANTINED)) && SUCCEEDED(pQuarItem->SaveItem()) )
                {
                    CCTRACEI(_T("CScanManager::OnRepairFailed() - Converted backup item to quarantined item."));
                    bQuarantined = true;
                }
                else
                {
                    CCTRACEE(_T("CScanManager::OnRepairFailed() - Failed to convert backup item to quarantine item. Will attempt to create a new one."));
                }
            }
        }

        if( !bQuarantined )
        {
            // Create a new quarantine item
            std::string strFileName;
            m_DisplayNames.GetDisplayNameForFile(pInfection, strFileName);
            if( m_Quarantine.Add(pInfection, QFILE_STATUS_QUARANTINED, false, strFileName.c_str()) )
            {
                bQuarantined = true;
            }
            else
            {
                CCTRACEE(_T("CScanManager::OnRepairFailed() - Failed to create a new quarantine backup item for %s."), strFileName.c_str());
            }
        }

        // If the quarantine was successful we will now tell ccScan to delete the infection
        // If the quarantine failed we will set this infection to repair failed and let the manual
        // processing attempt to quarantine the infection
        if( bQuarantined )
        {
            CCTRACEI(_T("CScanManager::OnRepairFailed() - Added to quarantine, now telling ccScan to try to delete it."));
            pInfection->SetUserData(InfectionStatus_Quarantined);

            // Do a deep delete for email scanning a regular delete for all others
            if( m_Settings[SMSETTING_EMAIL] == true )
                return SCAN_DEEP_DELETE;
            else
                return SCAN_DELETE;
        }
    }
    else if( m_Response == SCAN_MODE_AUTO_REPAIR_DELETE )
    {
        CCTRACEI(_T("CScanManager::OnRepairFailed() - Response set to auto-repair delete, telling ccScan to try to delete it."));

        // Delete backup item if it exists
        if( m_Settings[SMSETTING_BACKUP_REPAIRS] )
        {
            CCTRACEI(_T("CScanManager::OnRepairFailed() - Removing quarantine viral backup item since repair failed."));
            m_Quarantine.Delete(pInfection);
        }

        pInfection->SetUserData(InfectionStatus_Deleted);

        // Do a deep delete for email scanning a regular delete for all others
        if( m_Settings[SMSETTING_EMAIL] == true )
            return SCAN_DEEP_DELETE;
        else
            return SCAN_DELETE;
    }

    //
    // If we get here we want to set this anomaly to repair failed
    //

    CCTRACEW(_T("CScanManager::OnRepairFailed() - Setting the infection to repair failed"));

    // Delete backup item if it exists
    if( m_Settings[SMSETTING_BACKUP_REPAIRS] )
    {
        CCTRACEI(_T("CScanManager::OnRepairFailed() - Removing quarantine viral backup item since repair failed."));
        m_Quarantine.Delete(pInfection);
    }

    // Set the scan infection to repair failed
    pInfection->SetUserData(InfectionStatus_Repair_Failed);

    // Set anomaly to repair failed.
    ccEraser::IAnomalyPtr spAnomaly;
    SCANINFECTIONTOANOMALYMAP::iterator Iter = m_mapInfectionsToAnomalies.find(pInfection);
    if( Iter == m_mapInfectionsToAnomalies.end()  )
    {
        CCTRACEE(_T("CScanManager::OnRepairFailed() - This anomaly is not in our map of infections."));
        return SCAN_OK;
    }
    else
    {
        spAnomaly = (*Iter).second;

        if( spAnomaly == NULL )
        {
            CCTRACEE(_T("CScanManager::OnRepairFailed() - The anomaly in our map is bad."));
            return SCAN_OK;
        }
    }

    CEZAnomaly ezAnom(spAnomaly);
    ezAnom.SetAnomalyStatus(ACTION_QUARANTINE, InfectionStatus_Repair_Failed);

    // Update the status text for the remediation
    m_DisplayNames.UpdateAllRemediationStatusText(&ezAnom);

    // Remove this from the compressed map
    RemoveFromCompressedMap(pInfection);

    // Remove this from the infection to anomaly map
    m_mapInfectionsToAnomalies.erase(pInfection);
  
    return SCAN_OK;
}

SCANSTATUS CScanManager::OnDeleteFailed( IScanInfection* pInfection )
{
    //
    // NOTE:  IT IS NOW POSSIBLE FOR THIS METHOD TO BE CALLED ON MULTIPLE THREADS!
    //        This means that any objects you use or calls you make should be thread-
    //        safe. Since this method is not time critical event (unlike OnNewFile),
    //        use of synchronization like critical sections does not need to be avoided
    //        and can be used to ease making this method thread safe.
    //
    // (Synchronization OK)
    //

    if( SCAN_ABORT == HandlePauseAbort(_T("OnDeleteFailed")) )
        return SCAN_ABORT;

    // Synchronize access by pausing all other scan threads
    CScanPause pause(m_pScanner);

    CCTRACEI(_T("CScanManager::OnDeleteFailed() - Called for infection %s."), pInfection->GetVirusName());

    // If we remove the IScanFileInfection of a compressed file via auto-delete\repair then ccScan
    // will call us with OnDeleteFailed() when it fails to process that container...first checking to see
    // if we actually have a scan infection in our map of compressed items that we are trying to process
    ccEraser::IAnomalyPtr spAnomaly;

    SCANINFECTIONTOANOMALYMAP::iterator Iter = m_mapInfectionsToAnomalies.find(pInfection);
    if( Iter == m_mapInfectionsToAnomalies.end()  )
    {
        CCTRACEW(_T("CScanManager::OnDeleteFailed() - This anomaly is not in our map of infections."));
        return SCAN_OK;
    }
    else
    {
        spAnomaly = (*Iter).second;

        if( spAnomaly == NULL )
        {
            CCTRACEE(_T("CScanManager::OnDeleteFailed() - The anomaly in our map is bad."));
            return SCAN_OK;
        }
    }

    // Set the appropriate status for this item
    bool bNonViral = IsNonViral(pInfection);
    AnomalyAction eNextAction = ACTION_REVIEW;
    CommonUIInfectionStatus eStatus = InfectionStatus_Delete_Failed;

    if( !bNonViral )
    {
        if( m_Response == SCAN_MODE_AUTO_REPAIR_QUARANTINE )
        {
            CCTRACEI(_T("CScanManager::OnDeleteFailed() - Removing qurantine item for virus infection %s. Setting to quarantine failed."), pInfection->GetVirusName());

            m_Quarantine.Delete(pInfection);

            pInfection->SetUserData(InfectionStatus_Quarantine_Failed);

            // Set this item to quarantine failed.
            eNextAction = ACTION_DELETE;
            eStatus = InfectionStatus_Quarantine_Failed;
        }
        else // The response mode was to auto-delete
        {
            pInfection->SetUserData(InfectionStatus_Delete_Failed);

            CCTRACEI(_T("CScanManager::OnDeleteFailed() - Auto-Delete failed for virus infection %s. Setting to delete failed."), pInfection->GetVirusName());
        }
    }
    else
    {
        CCTRACEI(_T("CScanManager::OnDeleteFailed() - Delete failed for non-viral infection %s. Setting to delete failed and review."), pInfection->GetVirusName());

        // Remove quarantine backup item if it exists
        if( m_Settings[SMSETTING_BACKUP_DELETED_THREATS] == true )
        {
            CCTRACEI(_T("CScanManager::OnDeleteFailed() - Removing qurantine backup item for non-viral infection %s"), pInfection->GetVirusName());
            m_Quarantine.Delete(pInfection);
        }

        pInfection->SetUserData(InfectionStatus_Delete_Failed);

        eNextAction = ACTION_REVIEW;
    }

    // Set the anomaly status
    CEZAnomaly ezAnom(spAnomaly);
    ezAnom.SetAnomalyStatus(eNextAction, eStatus);
    m_DisplayNames.UpdateAllRemediationStatusText(&ezAnom);

    // Remove this from the compressed map
    RemoveFromCompressedMap(pInfection);

    // Remove this from the infection to anomaly map
    m_mapInfectionsToAnomalies.erase(pInfection);
 
    return SCAN_OK;
}

SCANSTATUS CScanManager::OnRemoveMimeComponent( IScanInfection* pInfection, char* pszMessage,
                                    int iMessageBufferSize )
{
    //
    // (GVOGEL)
    // NOTE:  IT IS NOW POSSIBLE FOR THIS METHOD TO BE CALLED ON MULTIPLE THREADS!
    //        This means that any objects you use or calls you make should be thread-
    //        safe. Since this method is not time critical event (unlike OnNewFile),
    //        use of synchronization like critical sections does not need to be avoided
    //        and can be used to ease making this method thread safe.
    //
    // (Synchronization OK)
    //

	// Generate the MIME replacement text if we're doing an e-mail scan.
	if (m_Settings[SMSETTING_EMAIL] == false)
	{
		return SCAN_OK;
	}

	CScanPtr<IScanCompressedFileInfection> pCompressedInfection;
	SYMRESULT Status = pInfection->QueryInterface(IID_ScanCompressedFileInfection,
			(void**) &pCompressedInfection);

	if (SYM_FAILED(Status))
	{
		return SCAN_OK;
	}

	string strFileName;
	m_DisplayNames.GetDisplayNameForFile(pCompressedInfection, strFileName);
		
	TCHAR szFormat[MAX_PATH];
	LoadString(g_hInstance, IDS_REMOVE_MIME, szFormat, MAX_PATH);

	_snprintf(pszMessage, iMessageBufferSize, szFormat, m_strProductName, strFileName.c_str(),
		pInfection->GetVirusName());

    CCTRACEI("CScanManager::OnRemoveMimeComponent() - Using text %s", pszMessage);

	return SCAN_REPLACE;
}

SCANSTATUS CScanManager::OnCleanFile( const char * pszFileName,
                        const char * pszTempFileName )
{
    //
    // (GVOGEL)
    // NOTE:  IT IS NOW POSSIBLE FOR THIS METHOD TO BE CALLED ON MULTIPLE THREADS!
    //        This means that any objects you use or calls you make should be thread-
    //        safe. Great care has been taken to avoid any need for synchronization
    //        (like critical sections) as well in order to keep performance optimum.
    //
    // The OEH functionality should not be used with multi-threaded scans. There's no
    // technical limitation as long as OEH is thread-safe but the performance gains
    // of multi-threading aren't very appreciable for the limited number of items OEH
    // processes (email queues).
    //

    if( SCAN_ABORT == HandlePauseAbort(_T("OnCleanFile")) )
        return SCAN_ABORT;

    // Bail if OEH scanning is disabled.

	if (m_Settings[SMSETTING_OEH] == false)
	{
		return SCAN_OK;
	}

	// Do our OEH check.

	if (m_OEHScan.Check(pszTempFileName, m_bAbortScan) == true)
	{
		if (m_bAbortScan == true)
        {
            CCTRACEI(_T("CScanManager::OnCleanFile() - returning scan abort because of the OEH Check."));
			return SCAN_ABORT;
        }
	}

	return SCAN_OK;
}

//////////////////////////////////////////////////////////////////////
// IScanSink2 methods.

SCANSTATUS CScanManager::OnBeginThread()
{
    // Synchronization required for our stl objects
    ccLib::CSingleLock Lock(&m_critScanSink, INFINITE, FALSE);

    //
    // COM is needed for display names conversion routine. The map ensures COM is only initialized once.
    //
    DWORD dwID = GetCurrentThreadId();

    map<DWORD, bool>::iterator Iter = m_ComThreadInitMap.find(dwID);

    if( Iter == m_ComThreadInitMap.end() || (*Iter).second == false )
    {
        CCTRACEI(_T("CScanManager::OnBeginThread() - Initializing COM for scan thread id = %d."), dwID);

        if (FAILED(CoInitialize(NULL)))
        {
            CCTRACEE(_T("CScanManager::OnBeginThread() - Failed to initialize COM for scan thread id = %d."), dwID);
            m_ComThreadInitMap[dwID] = false;
        }
        else
            m_ComThreadInitMap[dwID] = true;
    }

    // Make sure AP is disabled for this thread
    m_AutoProtectToggle.DisableAPForThisThread();

    return SCAN_OK;
}

SCANSTATUS CScanManager::OnEndThread()
{
    // Synchronize access
    ccLib::CSingleLock Lock(&m_critScanSink, INFINITE, FALSE);

    //
    // Cleanup.
    //

    // Uninitialize COM for this thread if it succeeded on the begin thread
    DWORD dwID = GetCurrentThreadId();
    
    map<DWORD, bool>::iterator Iter = m_ComThreadInitMap.find(dwID);

    if (Iter == m_ComThreadInitMap.end())
    {
        CCTRACEE(_T("CScanManager::OnEndThread() - Thread ID %d is not in the COM initialized map."), dwID);
    }
    else if( (*Iter).second == true )
    {
        CCTRACEI(_T("CScanManager::OnEndThread() - Uninitializing COM for scan thread id = %d."), dwID);
        CoUninitialize();
        m_ComThreadInitMap[dwID] = false;
    }

    // Re-enable AP on this thread
    m_AutoProtectToggle.EnableAPForThisThread();

    return SCAN_OK;
}

///////////////////////////////////////////////////////////////////////////////
//
// IScanProperties implementation.
//

int CScanManager::GetProperty(const char* szProperty, int iDefault)
{
	if (strcmp(szProperty, SCAN_COMPRESSED_FILES) == 0)
	{
        // Disable compressed file scanning for eraser scans
        if( m_bEraserScanning )
            return 0;

		if (m_Settings[SMSETTING_SCAN_COMPRESSED_FILES] == true)
		{
			return 1;
		}
		else
		{
			return 0;
		}
	}

	if ((strcmp(szProperty, SCAN_BOOT_RECORDS) == 0) ||
		(strcmp(szProperty, SCAN_ALL_BOOT_RECORDS) == 0))
	{
        // Disable boot record scanning for eraser scans
        if( m_bEraserScanning )
            return 0;

		if (m_Settings[SMSETTING_SCAN_BOOTRECS] == true)
		{
			return 1;
		}
		else
		{
			return 0;
		}
	}

	if ((strcmp(szProperty, SCAN_MASTER_BOOT_RECORDS) == 0) ||
		(strcmp(szProperty, SCAN_ALLMASTER_BOOT_RECORDS) == 0))
	{
        // Disable MBR scanning for eraser scans
        if( m_bEraserScanning )
            return 0;

		if (m_Settings[SMSETTING_SCAN_MBR] == true)
		{
			return 1;
		}
		else
		{
			return 0;
		}
	}

	if (strcmp(szProperty, SCAN_MEMORY) == 0)
	{
        // Disable memory scanning for eraser scans
        if( m_bEraserScanning )
            return 0;

		if (m_Settings[SMSETTING_SCAN_MEMORY] == true)
		{
			return 1;
		}
		else
		{
			return 0;
		}
	}

	if (strcmp(szProperty, SCAN_FILES) == 0)
	{
        // Enabled file scanning for eraser scans
        if( m_bEraserScanning )
            return 1;

		if (m_Settings[SMSETTING_SCAN_FILES] == true)
		{
			return 1;
		}
		else
		{
			return 0;
		}
	}

	if (strcmp(szProperty, SCAN_DECOMPOSER_MAX_DEPTH) == 0)
	{
		return m_iMaxDecomposerExtractDepth;
	}

    if (strcmp(szProperty, SCAN_NON_VIRAL_THREATS) == 0)
    {
        // Enable non-viral threat scanning if the client turned it on and
        // at least one category is enabled
        if (m_Settings[SMSETTING_SCAN_NON_VIRAL_THREATS] == true)
		{
            std::map<DWORD, bool>::iterator Iter;
            for( Iter = m_CategoryEnabled.begin(); Iter != m_CategoryEnabled.end(); Iter++ )
            {
                if( (*Iter).second == true )
                    return 1;
            }
		}

		return 0;
    }

    // Always disable the AP property so the engines don't disable AP
    // before each file operation since we already handle unprotecting our
    // own process
    if (strcmp(szProperty, SCAN_DISABLE_AP) == 0)
    {
        return 0;
    }

    if (strcmp(szProperty, SCAN_THREADS) == 0)
    {
        // Disable multi-thread scanning if the setting tells us to and for
        // eraser scans
        if ( m_bEraserScanning )
        {
            return 0;
        }
        else
        {
            CCTRACEI(_T("CScanManager::GetProperty() - Using %d threads for scan."), m_dwScanThreads);
            return m_dwScanThreads;
        }
    }

    if (strcmp(szProperty, SCAN_DISK_ORDER) == 0)
    {
        // Enable disk order scanning
        return 1;
    }

    if (strcmp(szProperty, SCAN_ALTERNATE_DATA_STREAM) == 0)
    {
        // Enable ADS scanning
        return 1;
    }

	return iDefault;
}

const char* CScanManager::GetProperty(const char* szProperty, const char* szDefault)
{
    if (strcmp(szProperty, SCAN_MIME_REPLACEMENT_FILE) == 0)
    {
        if( strMimeReplacement.GetLength() == 0 )
        {
			CString csFormat;
			csFormat = "%s ";
			csFormat += SCAN_MIME_REPLACEMENT_FILE;
			strMimeReplacement.Format(csFormat, m_strProductName);
        }
        return (LPCTSTR)strMimeReplacement;
    }
    
	return szDefault;
}

///////////////////////////////////////////////////////////////////////////////
//
// IScanItems implementation.
	
int CScanManager::GetScanItemCount()
{
	return m_ItemsToScan.size();
}

const char* CScanManager::GetScanItemPath(int iIndex)
{
	return m_ItemsToScan.at(iIndex).c_str();
}

///////////////////////////////////////////////////////////////////////////////
//
// ccEraser::ICallback implementation.
ccEraser::eResult CScanManager::PreDetection(ccEraser::IDetectionAction* pDetectAction, const ccEraser::IContext* pContext)
{
    if( SCAN_ABORT == HandlePauseAbort(_T("PreDetection")) )
        return ccEraser::Abort;

    return ccEraser::Continue;
}

ccEraser::eResult CScanManager::PostDetection(ccEraser::IDetectionAction* pDetectAction, ccEraser::eResult eDetectionResult, const ccEraser::IContext* pContext)
{
    if( SCAN_ABORT == HandlePauseAbort(_T("PostDetection")) )
        return ccEraser::Abort;

    // If logging is enabled log out successful detections
    if( g_DebugOutput.IsOutputEnabled(ccLib::CDebugOutput::eInformationDebug) )
    {
        cc::IKeyValueCollectionPtr pDetectProps;
        if( ccEraser::Failed(pDetectAction->GetProperties(pDetectProps)) || pDetectProps == NULL )
        {
            CCTRACEE(_T("CScanManager::PostDetection() - Failed to get detection action properties"));
            return ccEraser::Continue;
        }

        // Get the state
        DWORD dwState = 0;
        if( !pDetectProps->GetValue(ccEraser::IDetectionAction::State, dwState) )
        {
            CCTRACEE(_T("CScanManager::PostDetection() - Failed to get detection action state"));
            return ccEraser::Continue;
        }

        // See if this action was detected
        if( dwState == ccEraser::IDetectionAction::Detected )
        {
            cc::IStringPtr pStrDesc;
            if( ccEraser::Succeeded(pDetectAction->GetDescription(pStrDesc)) && pStrDesc != NULL )
            {
                CCTRACEI(_T("CScanManager::PostDetection() - DETECTED: %s"), pStrDesc->GetStringA());
            }
            else
            {
                CCTRACEE(_T("CScanManager::PostDetection() - Failed to get generic detection action description for detected action"));
            }
        }
    }

    return ccEraser::Continue;
}

//
// Eraser scans are performed in single threaded mode so we don't have to worry
// about synchronization here.
//
ccEraser::eResult 
CScanManager::OnAnomalyDetected(ccEraser::IAnomaly* pAnomaly, 
                                const ccEraser::IContext* pContext)
{
    if ( SCAN_ABORT == HandlePauseAbort(_T("OnAnomalyDetected")) )
    {
        return ccEraser::Abort;
    }

    if ( !pAnomaly )
    {
        CCTRACEE(_T("CScanManager::OnAnomalyDetected() - Anomaly invalid."));
        return ccEraser::Continue;
    }
    
    // Are we in DoSingleGenericLoadPointScan() and want only
    //  relevant detections?
    if(m_bIgnoreOnAnomalyDetected)
        return ccEraser::Continue;

    CEZAnomaly EZAnomaly;

    if ( !EZAnomaly.Initialize(pAnomaly) )
    {
        CCTRACEE(_T("CScanManager::OnAnomalyDetected() - Failed to initialize the anomaly data."));
        return ccEraser::Continue;
    }

    if ( g_DebugOutput.IsOutputEnabled(ccLib::CDebugOutput::eInformationDebug) )
    {
        cc::IStringPtr spStrName;
        cc::IStringPtr spStrID;
        ATL::CAtlString strName;
        ATL::CAtlString strID;

        if( EZAnomaly.GetName(spStrName) && spStrName )
        {
            strName = spStrName->GetStringA();
        }

        if( EZAnomaly.GetID(spStrID) && spStrID )
        {
            strID = spStrID->GetStringA();
        }

        CCTRACEI("CScanManager::OnAnomalyDetected() - Callback hit for Name = %s ID = %s.", strName.GetString(), strID.GetString());
    }

    // Ignore this item if it's excluded
    //
    if ( m_Exclusions.IsMatch(EZAnomaly) )
    {
        CCTRACEI(_T("CScanManager::OnAnomalyDetected() - The anomaly matched an exclusion, this anomaly will be ignored."));
        return ccEraser::Continue;
    }

    CScanManagerCounts::eSMCountType eCountType = CScanManagerCounts::Viral;

    if ( EZAnomaly.IsViral() )
    {
        CCTRACEI("CScanManager::OnAnomalyDetected() - This is a viral anomaly.");
        eCountType = CScanManagerCounts::Viral;
    }
    else if ( EZAnomaly.IsNonViral() )
    {
        CCTRACEI("CScanManager::OnAnomalyDetected() - This is a non-viral anomaly.");
        eCountType = CScanManagerCounts::NonViral;
    }
    else
    {
        CCTRACEW("CScanManager::OnAnomalyDetected() - This anomaly is not a known viral or non-viral anomaly. Ignoring.");
        return ccEraser::Continue;
    }

    // Check for the presence of the anomalies remediation actions
    if ( !CheckPresenceOfRemediationActions(&EZAnomaly) )
    {
        CCTRACEW(_T("CScanManager::OnAnomalyDetected() - None of the remediation actions for this anomaly were present. Skipping."));
        return ccEraser::Continue;
    }

    // Add this to our list and increment the detection count
    ccEraser::eResult eRes = m_pAnomalyList->Add(EZAnomaly);

    if ( ccEraser::Failed(eRes) )
    {
        CCTRACEE(_T("CScanManager::OnAnomalyDetected() - Failed to add anomaly to the list. eResult = %d"), eRes);
        return ccEraser::Continue;
    }
    else
    {
        m_ScanMgrCounts.IncrementItemCount(eCountType);
    }

    // If this is specific insert the VID into our map
    if ( EZAnomaly.IsSpecific() )
    {
        size_t nIndex = 0;
        m_pAnomalyList->GetCount(nIndex);
        nIndex--;
        DWORD dwVID = EZAnomaly.GetVID();

        if ( dwVID != 0 )
        {
            m_mapSpecificAnomalyIndex[dwVID] = nIndex;
            CCTRACEI(_T("CScanManager::OnAnomalyDetected() - Added VID = %lu Index = %d to specific anomaly map."), dwVID, nIndex);
        }
        else
        {
            CCTRACEE(_T("CScanManager::OnAnomalyDetected() - This is a specific anomaly, but the VID is 0. Not adding to the specific map."));
        }
    }

    // Merge the scan infection items
    MergeScanInfectionItems(&EZAnomaly);

    // Set the common UI generic text
    m_DisplayNames.SetGenericText(EZAnomaly);

    // Attempt to handle this anomaly automatically
    HandleAnomaly(&EZAnomaly);

    // User aborted scan?
    if (m_bAbortScan == true)
    {
        CCTRACEI(_T("CScanManager::OnAnomalyDetected() - returning ccEraser::Abort."));
        return ccEraser::Abort;
    }

    return ccEraser::Continue;
}

bool 
CScanManager::AddInfectionToExistingNonRelevantAnomaly(IScanInfection* pInfection)
{
    if(!m_mapNonRelevantAnomalyIndex.size() || NULL == m_pNonRelevantAnomalyList.m_p)
        return false;

    ccEraser::eResult eRes;

    ANOMALYVIDINDEXMAP::iterator Iter = 
        m_mapNonRelevantAnomalyIndex.find(pInfection->GetVirusID());

    if ( Iter == m_mapNonRelevantAnomalyIndex.end() )
    {
        return false;
    }

    CCTRACEI(_T("CScanManager::AddInfectionToExistingNonRelevantAnomaly() - VID %lu already has a specific non-relevant anomaly. Will add this infection item to the existing anomaly."), pInfection->GetVirusID());

    // Get the existing anomaly
    ccEraser::IAnomalyPtr pAnomaly;

    if ( ccEraser::Failed( eRes = m_pNonRelevantAnomalyList->GetItem( (*Iter).second, 
        pAnomaly) ) )
    {
        CCTRACEE(_T("CScanManager::AddInfectionToExistingNonRelevantAnomaly() - Failed to get existing specific anomaly at index %d. eResult = %d. Going to create a full anomaly."), (*Iter).second, eRes);
        return false;
    }
    
    CEZAnomaly ezAnomaly;

    if ( !ezAnomaly.Initialize(pAnomaly) )
    {
        CCTRACEE(_T("CScanManager::AddInfectionToExistingNonRelevantAnomaly() - Failed to initialize the anomaly data."));
        return false;
    }


    //
    // Do we need to add an InfectedFileRemediation 
    //  for this IScanFileInfection?
    //

    BOOL bAddInfectedFileRemediation = TRUE;

    // Get the scan file infection so we can get a filename to compare
    IScanFileInfection2QIPtr spInfectionScanFileInfection = pInfection;
    if(spInfectionScanFileInfection)
    {
        // Get the file name for our infection
        CString cszInfectionFileName = spInfectionScanFileInfection->GetLongFileName();

        // Get the remediation list
        ccEraser::IRemediationActionListPtr spRemediateList = ezAnomaly.GetRemediations();
        if( spRemediateList == NULL )
        {
            CCTRACEE(_T("CScanManager::AddInfectionToExistingNonRelevantAnomaly() - Failed to get remediation action list."));
            return false;
        }

        size_t nCount = ezAnomaly.GetRemediationCount();

        // Now we loop through each remediation action looking for IScanInfection items
        for(size_t nCurRem = 0; nCurRem < nCount; nCurRem++)
        {
            ccEraser::IRemediationActionPtr spAction = ezAnomaly.GetRemediation(nCurRem);
            if( spAction == NULL )
            {
                CCTRACEE(_T("CScanManager::AddInfectionToExistingNonRelevantAnomaly() - Failed to get remediation action item %d."), nCurRem);
                continue;
            }

            // Wrap with remediation helper class
            CEZRemediation ezRem(spAction);
            if(ezRem.IsScanInfectionType())
            {
                // Get the scan infection item
                IScanInfectionPtr spScanInfection = ezRem.GetScanInfection();
                if(!spScanInfection)
                    continue;

                // Get the scan file infection item
                IScanFileInfection2QIPtr spRemediationScanFileInfection = spScanInfection;
                if(!spRemediationScanFileInfection)
                    continue;

                CString cszRemediationFileName = spRemediationScanFileInfection->GetLongFileName();

                if(0 == cszInfectionFileName.CompareNoCase(cszRemediationFileName))
                {
                    bAddInfectedFileRemediation = FALSE;
                    break;
                }

            }
        }
    }

    //
    // If there is not already an infected file remediation for this
    //  anomaly, add it
    //

    if(bAddInfectedFileRemediation)

    {
        // Create the remediation action
        ccEraser::IRemediationActionPtr spRem;

        if ( !GenerateInfectionRemediationAction(pInfection, spRem) || 
            spRem == NULL )
        {
            CCTRACEE(_T("CScanManager::AddInfectionToExistingNonRelevantAnomaly() - Failed to generate the remediation action."));
            return false;
        }

        CEZRemediation EZRemToAdd(spRem);

        // Set the specific flag for this infection remediation action
        cc::IKeyValueCollectionPtr spRemProps = EZRemToAdd.GetProperties();

        if ( spRemProps != NULL )
        {
            spRemProps->SetValue(ccEraser::IRemediationAction::Specific, true);
        }

        // Add the remediation to the existing anomaly remediation action list
        ccEraser::IRemediationActionListPtr pRemList = ezAnomaly.GetRemediations();

        if ( pRemList == NULL )
        {
            CCTRACEE(_T("CScanManager::AddInfectionToExistingNonRelevantAnomaly() - Failed to get the remediation action list."));
            return false;
        }

        if ( ccEraser::Failed(eRes = pRemList->Add(spRem)) )
        {
            CCTRACEE(_T("CScanManager::AddInfectionToExistingNonRelevantAnomaly() - Failed to add the remediation action to the list. eResult = %d"), eRes);
            return false;
        }
    }
    
    // Now that we have a whole Anomaly, pass it on
    //  for processing.
    eRes = OnAnomalyDetected(ezAnomaly, NULL);
    
    return eRes == ccEraser::Continue;
}

bool 
CScanManager::AddInfectionToExistingSpecificAnomaly(IScanInfection* pInfection)
{
    ccEraser::eResult eRes;

    ANOMALYVIDINDEXMAP::iterator Iter = 
                     m_mapSpecificAnomalyIndex.find(pInfection->GetVirusID());

    if ( Iter == m_mapSpecificAnomalyIndex.end() )
    {
        return false;
    }

    CCTRACEI(_T("CScanManager::AddInfectionToExistingSpecificAnomaly() - VID %lu already has a specific anomaly. Will add this infection item to the existing anomaly."), pInfection->GetVirusID());

    // Get the existing anomaly
    ccEraser::IAnomalyPtr pAnomaly;

    if ( ccEraser::Failed( eRes = m_pAnomalyList->GetItem( (*Iter).second, 
                                                           pAnomaly) ) )
    {
        CCTRACEE(_T("CScanManager::AddInfectionToExistingSpecificAnomaly() - Failed to get existing specific anomaly at index %d. eResult = %d. Going to create a full anomaly."), (*Iter).second, eRes);
        return false;
    }

    CEZAnomaly EZAnomaly;

    if ( !EZAnomaly.Initialize(pAnomaly) )
    {
        CCTRACEE(_T("CScanManager::AddInfectionToExistingSpecificAnomaly() - Failed to initialize the anomaly data."));
        return false;
    }

    // Create the remediation action
    ccEraser::IRemediationActionPtr spRem;

    if ( !GenerateInfectionRemediationAction(pInfection, spRem) || 
         spRem == NULL )
    {
        CCTRACEE(_T("CScanManager::AddInfectionToExistingSpecificAnomaly() - Failed to generate the remediation action."));
        return false;
    }

    CEZRemediation EZRemToAdd(spRem);

    // Set the specific flag for this infection remediation action
    cc::IKeyValueCollectionPtr spRemProps = EZRemToAdd.GetProperties();

    if ( spRemProps != NULL )
    {
        spRemProps->SetValue(ccEraser::IRemediationAction::Specific, true);
    }

    // See if this anomaly has been processed already
    AnomalyAction eNextAction = ACTION_REPAIR;
    CommonUIInfectionStatus eStatus = InfectionStatus_Unhandled;
    EZAnomaly.GetAnomalyStatus(eNextAction, eStatus);

    // Check if we need to take action on this scan infection item before 
    // adding it to the anomaly (this is the case where the anomaly was 
    // already automatically dealt with)
    if ( eNextAction == ACTION_COMPLETE  )
    {
        // Set the repair mode for the remediation action of the scan 
        // infection item
        ccEraser::IRemediationAction::Handle_Threat_Ops operation;

        if(  InfectionStatus_Repaired == eStatus )
        {
            operation = ccEraser::IRemediationAction::RepairOnly;
        }
        else
        {
            operation = ccEraser::IRemediationAction::DeleteOnly;
        }

        if ( !EZRemToAdd.SetThreatOperation(operation) )
        {
            CCTRACEE(_T("CScanManager::AddInfectionToExistingSpecificAnomaly() - Failed to set the handle threat operation property"));
        }

        // Save undo information for this remediation action before performing 
        // remediation
        cc::IStream* pUndo = NULL;
        ATL::CAtlString strUndoFile;

        if ( EZRemToAdd.ShouldSaveRemediation() )
        {
            GetUndoData(EZRemToAdd, pUndo, strUndoFile);
        }

        // Do the remediation
        ccEraser::eResult eRemediationResult;

        if ( !PerformRemediationAction(&EZRemToAdd, eRemediationResult) )
        {
            CCTRACEE(_T("CScanManager::AddInfectionToExistingSpecificAnomaly() - Failed to remediate the infection. not adding to anomaly."));

            // Release the undo data if we have it
            ReleaseUndoData(pUndo, strUndoFile);

            return false;
        }

        // Add the remediation to quarantine if necessary
        if ( EZRemToAdd.ShouldSaveRemediation() )
        {
            UUID quarUUID = {0};

            if ( EZAnomaly.GetQuarantineItemUUID(quarUUID) )
            {
                CSymPtr<IQuarantineItem> pQuarItem;

                if ( m_Quarantine.LoadQuarantineItemFromUUID(quarUUID, 
                                                             &pQuarItem) && 
                     pQuarItem != NULL )
                {
                    CCTRACEI(_T("CScanManager::AddInfectionToExistingSpecificAnomaly() - Successfully loaded associated quarantine item."));

                    m_DisplayNames.
                             SetScanInfectionRemdiationActionText(EZRemToAdd);

                    if ( FAILED(pQuarItem->AddRemediationData(EZRemToAdd, 
                                                              pUndo)) )
                        CCTRACEE(_T("CScanManager::AddInfectionToExistingSpecificAnomaly() - Failed to set the quarantine item remediation undo data."));
                    else
                    {
                        CCTRACEI(_T("CScanManager::AddInfectionToExistingSpecificAnomaly() - Added the remediation to existing quarantine item."));
                        pQuarItem->SaveItem();
                    }
                }
                else
                {
                    CCTRACEE(_T("CScanManager::AddInfectionToExistingSpecificAnomaly() - Failed to load quarantine item from UUID."));
                }
            }
        }

        // Release the undo data if we have it
        ReleaseUndoData(pUndo, strUndoFile);

        // Mark if a reboot is required for this anomaly to be fully removed
        if ( eRemediationResult == ccEraser::RebootRequired )
        {
            EZAnomaly.SetRequiresReboot();
        }
    }

    // Add the remediation to the existing anomaly remediation action list
    ccEraser::IRemediationActionListPtr pRemList = EZAnomaly.GetRemediations();

    if ( pRemList == NULL )
    {
        CCTRACEE(_T("CScanManager::AddInfectionToExistingSpecificAnomaly() - Failed to get the remediation action list."));
        return false;
    }

    if ( ccEraser::Failed(eRes = pRemList->Add(spRem)) )
    {
        CCTRACEE(_T("CScanManager::AddInfectionToExistingSpecificAnomaly() - Failed to add the remediation action to the list. eResult = %d"), eRes);
        return false;
    }

    // Reset the text description for this anomaly
    m_DisplayNames.SetGenericText(EZAnomaly);

    return true;
}


// This will return false if all of the remediation actions successfully return
// that they are not present
bool CScanManager::CheckPresenceOfRemediationActions(CEZAnomaly* pEZAnomaly)
{
    if( !pEZAnomaly )
    {
        CCTRACEE(_T("CScanManager::CheckPresenceOfRemediationActions() - Input paramater bad."));
        return true;
    }

    if( g_DebugOutput.IsOutputEnabled(ccLib::CDebugOutput::eInformationDebug) )
    {
        cc::IStringPtr spStrName;
        ATL::CAtlString strName;
        if( pEZAnomaly->GetName(spStrName) && spStrName )
            strName = spStrName->GetStringA();

        size_t nCount = pEZAnomaly->GetRemediationCount();

        CCTRACEI(_T("CScanManager::CheckPresenceOfRemediationActions() - Begin for threat %s. With %d remediations."), strName.GetString(), nCount);
    }

    // Get the remediation list
    ccEraser::IRemediationActionListPtr pRemediateList = pEZAnomaly->GetRemediations();
    if( pRemediateList == NULL )
    {
        CCTRACEE(_T("CScanManager::CheckPresenceOfRemediationActions() - Failed to get remediation action list."));
        return true;
    }

    bool bARemediationIsPresent = false;

    size_t nCount = pEZAnomaly->GetRemediationCount();

    // Now we loop through each remediation action looking for IScanInfection items
    for(size_t nCurRem = 0; nCurRem < nCount; nCurRem++)
    {
        ccEraser::IRemediationActionPtr pAction = pEZAnomaly->GetRemediation(nCurRem);
        if( pAction == NULL )
        {
            CCTRACEE(_T("CScanManager::CheckPresenceOfRemediationActions() - Failed to get remediation action item %d."), nCurRem);
            continue;
        }

        CEZRemediation ezRem(pAction);

        // Processes come in suspend/terminate combinations so we only need to
        // check the terminate since that's what we display
        if( ezRem.GetType() == ccEraser::ProcessRemediationActionType &&
            ezRem.GetOperationType() == ccEraser::IRemediationAction::Suspend )
        {
            CCTRACEI(_T("CScanManager::CheckPresenceOfRemediationActions() - Not checking process suspend operation."));
            continue;
        }

        // Services come in Stop/Remove combinations we only need to check the
        // Remove since that's what we display
        if( ezRem.GetType() == ccEraser::ServiceRemediationActionType &&
            ezRem.GetOperationType() == ccEraser::IRemediationAction::Stop )
        {
            CCTRACEI(_T("CScanManager::CheckPresenceOfRemediationActions() - Not checking service stop operation."));
            continue;
        }

        bool bPresent = true;

        ccEraser::eResult eRes = pAction->IsPresent(bPresent);

        if( ccEraser::Succeeded(eRes) && !bPresent )
        {
            // Mark this remediation action as not present
            ezRem.SetRemediationPresent(false);

            if( g_DebugOutput.IsOutputEnabled(ccLib::CDebugOutput::eInformationDebug) )
            {
                cc::IStringPtr spStrDesc;
                ATL::CAtlString strDesc;
                if( ccEraser::Succeeded(pAction->GetDescription(spStrDesc)) && spStrDesc )
                    strDesc = spStrDesc->GetStringA();
                CCTRACEI(_T("CScanManager::CheckPresenceOfRemediationActions() - IsPresent() returned false for remediation action %s, marking not present in the user data"), strDesc.GetString());
            }
        }
        else
        {
            if( g_DebugOutput.IsOutputEnabled(ccLib::CDebugOutput::eWarningDebug) )
            {
                cc::IStringPtr spStrDesc;
                ATL::CAtlString strDesc;
                if( ccEraser::Succeeded(pAction->GetDescription(spStrDesc)) && spStrDesc )
                    strDesc = spStrDesc->GetStringA();

                if( ccEraser::Failed(eRes) )
                    CCTRACEW(_T("CScanManager::CheckPresenceOfRemediationActions() - IsPresent() for remediation action %s failed with error %d. Treating as present"), strDesc.GetString(), eRes);
                else
                    CCTRACEI(_T("CScanManager::CheckPresenceOfRemediationActions() - IsPresent() returned true for remediation action %s, marking present in the user data"), strDesc.GetString());
            }

            // At this point, we know we will return true, since we found 
            // a remediation action that is either present or its presence 
            // is unknown.  Don't actually return now, though, because we
            // want to finishi looping through all remediations and set to 
            // false any that are found to not be present
            bARemediationIsPresent = true;
        }
    }

    CCTRACEI(_T("CScanManager::CheckPresenceOfRemediationActions() - End."));

    return bARemediationIsPresent;
}

void CScanManager::MergeScanInfectionItems(CEZAnomaly* pEZAnomaly)
{
    // Get the remediation list
    ccEraser::IRemediationActionListPtr pRemediateList = pEZAnomaly->GetRemediations();
    if( pRemediateList == NULL )
    {
        CCTRACEE(_T("CScanManager::MergeScanInfectionItems() - Failed to get remediation action list."));
        return;
    }

    size_t nCount = pEZAnomaly->GetRemediationCount();

    // Now we loop through each remediation action looking for IScanInfection items
    for(size_t nCurRem = 0; nCurRem < nCount; nCurRem++)
    {
        ccEraser::IRemediationActionPtr pAction = pEZAnomaly->GetRemediation(nCurRem);
        if( pAction == NULL )
        {
            CCTRACEE(_T("CScanManager::MergeScanInfectionItems() - Failed to get remediation action item %d."), nCurRem);
            continue;
        }

        // Wrap with remediation helper class
        CEZRemediation ezRem(pAction);
        if(ezRem.IsScanInfectionType())
        {
            // Get the scan infection item
            IScanInfectionPtr pScanInfection = ezRem.GetScanInfection();

            // Merge this IScanInfection into our list
            if( pScanInfection != NULL )
            {
                // Block items with the Can't delete flag set unless they have
                // specific ccEraser repairs...
                if( !pEZAnomaly->IsSpecific() )
                {
                    IScanFileInfectionQIPtr pFileQIPtr = pScanInfection;
                    if( pFileQIPtr.m_p && !pFileQIPtr->CanDelete() )
                    {
                        CCTRACEI(_T("CScanManager::MergeScanInfectionItems() - This anomaly has infections with the can't delete flag set and it does not have a specific ccEraser repair, setting to review action."));
                        pEZAnomaly->SetAnomalyStatus(ACTION_REVIEW, InfectionStatus_CantDelete);
                    }
                }

                if( false == m_InfectionMerge.MergeInfection(pScanInfection) )
                {
                    CCTRACEW(_T("CScanManager::MergeScanInfectionItems() - The merge already reported that this item is detected..."));
                }
                else
                {
                    CCTRACEI(_T("CScanManager::MergeScanInfectionItems() - Merged scan infection item."));
                }
            }
            else
                CCTRACEE(_T("CScanManager::MergeScanInfectionItems() - Scan infection object is NULL."));
        }
    }
}

///////////////////////////////////////////////////////////////////////////////
//
// ICommonUISink implementation.

HRESULT CScanManager::OnCUIReady()
{
	if (m_hUIReadyEvent != NULL)
	{
		SetEvent(m_hUIReadyEvent);
	}

	return S_OK;
}

HRESULT CScanManager::OnCUIAbort()
{
	m_bAbortScan = true;

	return S_OK;
}

HRESULT CScanManager::OnCUIPause()
{
    // Reset the pause event
    ResetEvent(m_hUIResumeEvent);

	m_bPauseScan = true;

    // Save the time paused
    m_lCurTimePaused = time(NULL);

	return S_OK;
}

HRESULT CScanManager::OnCUIResume()
{
	m_bPauseScan = false;

    // Calculate total paused time
    m_lTotalTimePaused += (time(NULL) - m_lCurTimePaused);

	SetEvent(m_hUIResumeEvent);

	return S_OK;
}

unsigned __stdcall CScanManager::OnCUIHandleAnomalyProc(void* pArgument)
{
    // Initialize COM for this thread
    HRESULT hrComInit = CoInitialize(NULL);

    CScanManager* pThis = (CScanManager*) pArgument;

    unsigned ret = 0;
    bool bAbort = false;
    bool bFailure = false;

    try
    {
        // Disable Auto-Protect for this thread
        pThis->m_AutoProtectToggle.DisableAPForThisThread();

        // Walk through the anomaly list and take the action set in the user data "AnomalyAction" property
        // Call the repair progress callback if necessary

        if( pThis->m_pAnomalyList == NULL )
        {
            CCTRACEE(_T("CScanManager::OnCUIHandleAnomalyProc() - The internal anomaly list is not valid."));
            throw runtime_error("Throwing an exception after anomaly list failure.");
        }

        // Get the anomaly count
        size_t nAnomalyCount = 0;
        ccEraser::eResult eRes = pThis->m_pAnomalyList->GetCount(nAnomalyCount);

        if( ccEraser::Failed(eRes) )
        {
            CCTRACEE(_T("CScanManager::OnCUIHandleAnomalyProc() - GetCount for the anomaly list failed. eResult = %d"), eRes);
            throw runtime_error("Throwing an exception after get count failure.");
        }

        if( nAnomalyCount == 0 )
        {
            CCTRACEW(_T("CScanManager::OnCUIHandleAnomalyProc() - There are no anomalies in the list to handle."));
        }

        // Filter through each anomaly
        for( size_t nCur = 0; nCur < nAnomalyCount; nCur++ )
        {
            // Allow common UI to update progress and check for abort
            if( pThis->m_pCommonUI != NULL )
            {
                CCTRACEI(_T("CScanManager::OnCUIHandleAnomalyProc() - Calling UpdateRepairProgress with %d items"), nAnomalyCount-nCur);
                pThis->m_pCommonUI->UpdateRepairProgress(nAnomalyCount-nCur, bFailure, bAbort);
                if( bAbort )
                {
                    CCTRACEI(_T("CScanManager::OnCUIHandleAnomalyProc() - CommonUI has requested abort from the repair. Finished processing %d of %d items."), nCur, nAnomalyCount);
                    break;
                }
            }

            ccEraser::IAnomalyPtr pAnomaly;
            eRes = pThis->m_pAnomalyList->GetItem(nCur, pAnomaly);
            if( ccEraser::Succeeded(eRes) )
            {
                // Perform the handling for this item
                CEZAnomaly EZAnomaly;
                if( !EZAnomaly.Initialize(pAnomaly) )
                {
                    CCTRACEE(_T("CScanManager::OnCUIHandleAnomalyProc() - Failed to intialize anomaly data for %d from the list"), nCur);
                    continue;
                }

                if( !pThis->HandleAnomaly(&EZAnomaly) )
                {
                    bFailure = true;
                    CCTRACEW(_T("CScanManager::OnCUIHandleAnomalyProc() - Anomaly handling failed for item %d"), nCur);
                }
            }
            else
            {
                CCTRACEE(_T("CScanManager::OnCUIHandleAnomalyProc() - Failed to get anomaly %d from the list. eResult = %d"), nCur, eRes);
            }
        }
    }
    catch(exception& Ex)
    {
        CCTRACEE(_T("CScanManager::OnCUIHandleAnomalyProc() - %s"),Ex.what());
        ret = -1;
    }

    // Re-enable Auto-Protect for this thread
    pThis->m_AutoProtectToggle.EnableAPForThisThread();

    // Make sure common UI progress is called with 0 items so it knows to go to the next panel
    if( pThis->m_pCommonUI != NULL )
    {
        // If the abort was clicked then the CommonUI already knows to switch panels
        if( !bAbort )
        {
            CCTRACEI(_T("CScanManager::OnCUIHandleAnomalyProc() - Calling UpdateRepairProgress with 0 items"));
            pThis->m_pCommonUI->UpdateRepairProgress(0, bFailure, bAbort);
        }
    }

    // Uninitialize COM
    if( SUCCEEDED(hrComInit) )
        CoUninitialize();

    return ret;
}

HRESULT CScanManager::OnCUIHandleAnomalies()
{
    // Turn off automatic mode for the HandleAnomaly routine
    m_bAutomaticMode = false;

    // Reset the process termination and dependency items has been prompted\accepted flags
    m_bTerminateProcessPrompted = false;
    m_bTerminateProcessAccepted = false;

    // Kick off the repairs in another thread
    unsigned uID;
    HANDLE hThread =
        (HANDLE) _beginthreadex(NULL, 0, OnCUIHandleAnomalyProc, this, 0, &uID);    

    if( hThread == NULL )
    {
        CCTRACEE(_T("CScanManager::OnCUIHandleAnomalies() - Failed to start the repair thread."));
    }
    else
        CloseHandle(hThread);

    return S_OK;
}

HRESULT CScanManager::OnCUIGetCurrentFolder(LPSTR szFolder, long nLength)
{
    m_ScanMgrCounts.GetCurrentItemForDisplay(szFolder, nLength);
    return S_OK;
}

HRESULT CScanManager::OnCUIRemovedNonViralCount(long& nRemovedNonViralCount)
{
    nRemovedNonViralCount = m_ScanMgrCounts.GetNonViralItemsResolved();
    return S_OK;
}

HRESULT CScanManager::OnCUINonViralCount(long& nNonViralCount)
{
    nNonViralCount = m_ScanMgrCounts.GetItemCount(CScanManagerCounts::NonViral);
    return S_OK;
}

HRESULT CScanManager::OnCUIRemovedVirusCount(long& nRemovedVirusCount)
{
    nRemovedVirusCount = m_ScanMgrCounts.GetViralItemsResolved();
    return S_OK;
}

HRESULT CScanManager::OnCUIVirusCount(long& nVirusCount)
{
    nVirusCount = m_ScanMgrCounts.GetViralItemsDetected();
    return S_OK;
}

HRESULT CScanManager::OnCUIScanFileCount(long& nScanFileCount)
{
    nScanFileCount = m_ScanMgrCounts.GetItemCount(CScanManagerCounts::FilesScanned);
    return S_OK;
}

// This is the total number of threats quarantined and backed up in quarantine
HRESULT CScanManager::OnCUIGetQuarantinedCount(long& nQuarantineCount)
{
    nQuarantineCount = m_Quarantine.GetQuarantineCount();
    return S_OK;
}

HRESULT CScanManager::OnCUIAdditionalScan(bool& bAdditionalScan)
{
    bAdditionalScan = m_bAdditionalScan;
    return S_OK;
}

void CScanManager::ConvertItemsToOEM()
{
    vector<string>::iterator Iter;

    for(Iter = m_ItemsToScan.begin(); Iter != m_ItemsToScan.end(); Iter++)
    {
        ATL::CAtlString strItem = (*Iter).c_str();

        strItem.AnsiToOem();

        // Check to see if the item name was converted to OEM correctly.
        // Some characters (Hebrew characters in particulars) do not have OEM
        // equivalents. In this case CharToOem() replaces the ANSI character
        // with a bogus replacement character. In this case we will try to use
        // the file's short name instead.

        CFileAPI FileAPI;
        FileAPI.SwitchAPIToOEM();

        if (GetFileAttributes(strItem) == -1 && _tcslen((*Iter).c_str()) <= MAX_PATH )
        {
            CCTRACEW(_T("CScanManager::ConvertItemsToOEM() - Unable to get file attributes for %s in OEM mode. Attempting SFN."), (*Iter).c_str());
            FileAPI.SwitchAPIToANSI();

            char szShortName[MAX_PATH+1];
            if (GetShortPathName((*Iter).c_str(), szShortName, MAX_PATH) > 0)
            {
                strItem = szShortName;
                strItem.AnsiToOem();
                CCTRACEI(_T("CScanManager::ConvertItemsToOEM() - Using SFN OEM path = %s."), (LPCTSTR)strItem);

                FileAPI.SwitchAPIToOEM();
                if (GetFileAttributes(strItem) == -1)
                {
                    CCTRACEE(_T("CScanManager::ConvertItemsToOEM() - Still unable to get file attributes for %s in OEM mode."), (LPCTSTR)strItem);
                }
            }
        }

        (*Iter) = (LPCTSTR)strItem;
    }
}


SMRESULT CScanManager::ScanAndRepairWithProgress()
{
    CCTRACEI(_T("CScanManager::ScanAndRepairWithProgress() - Calling scan with progrss."));
    SMRESULT Result = SMRESULT_OK;

    // Create an event which we will use to signal the scan thread when
    // the UI is ready. The scan does not start until this event is signaled.
    m_hUIReadyEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
    if (m_hUIReadyEvent == NULL)
    {
        CCTRACEE(_T("CScanManager::ScanAndRepairWithProgress() - Failed to create the UI ready event."));
        return SMRESULT_ERROR_UNKNOWN;
    }

    // Create an event which we will use to resume the scan after it has
    // been paused. The event is signaled by OnCUIResume().
    m_hUIResumeEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
    if (m_hUIResumeEvent == NULL)
    {
        CCTRACEE(_T("CScanManager::ScanAndRepairWithProgress() - Failed to create the pause event."));

        if (m_hUIReadyEvent != NULL)
        {
            CloseHandle(m_hUIReadyEvent);
            m_hUIReadyEvent = NULL;
        }

        return SMRESULT_ERROR_UNKNOWN;
    }

    // Make sure this is a new common UI
    if (m_pCommonUI != NULL)
    {
        delete m_pCommonUI;
        m_pCommonUI = NULL;
    }

    // Create the UI thread
    m_pCommonUI = new CCommonUIThread;

    if( !m_pCommonUI )
    {
        // CommonUI creation failed, set result to this error
        CCTRACEE(_T("CScanManager::ScanAndRepairWithProgress() - Failed to create the common UI."));
        return SMRESULT_ERROR_COMMON_UI;
    }

    // Pass some essential information to the Common UI.
    m_pCommonUI->SetMemoryScanStatus(GetMemoryScanStatus());
    m_pCommonUI->SetScanCompressedFiles(
                                 m_Settings[SMSETTING_SCAN_COMPRESSED_FILES]);
    m_pCommonUI->SetRepairProgress(
                                 m_Settings[SMSETTING_SHOW_REPAIR_PROGRESS]);
    m_pCommonUI->SetScanNonViralThreats(
                                 m_Settings[SMSETTING_SCAN_NON_VIRAL_THREATS]);
    m_pCommonUI->SetParentWindow(m_hWndParent);
    
    bool bQuick = false;

    // 
    // If we're going to run the eraser scan, this is either a quick
    // scan or a full scan.
    //
    if ( m_Settings[SMSETTING_RUN_ERASER_SCAN] == true) {
       //
       // If not items were added to the scan item list, considering
       // this a quick scan.  Otherwise, considering it a full scan.  
       //
       if (0 == GetScanListItemCount()) 
       {
          bQuick = true;
       }
       else 
       {
          m_bFullScan = true;
       }
    }

    m_pCommonUI->SetQuickScan(bQuick);

    // Display the Common UI with the progress panel.
    CommonUIType UIType;
    if (m_UserInterface == SMUI_COMPLETE)
    {
        UIType = CommonUI_Complete;
    }
    else
    {
        UIType = CommonUI_ProgressOnly;
    }

    HRESULT hResult = m_pCommonUI->ShowCUIDlg(UIType, NULL, this, 0);

    if (FAILED(hResult))
    {
        CCTRACEE(_T("CScanManager::ScanAndRepairWithProgress() - ShowCUIDlg() failed."));

        // Indicate failure.
        Result = SMRESULT_ERROR_COMMON_UI;
    }
    else
    {
        // Kick off the scan in this thread.
        Result = Scan();

        CCTRACEI(_T("CScanManager::ScanAndRepairWithProgress() - Scan() returned %d."), Result);

        // Now wait for the UI thread to complete
        m_pCommonUI->WaitForExit(INFINITE);
    }

	// Clean-up.
	if (m_pCommonUI != NULL)
	{
		delete m_pCommonUI;
		m_pCommonUI = NULL;
	}

	if (m_hUIReadyEvent != NULL)
	{
		CloseHandle(m_hUIReadyEvent);
		m_hUIReadyEvent = NULL;
	}

	if (m_hUIResumeEvent != NULL)
	{
		CloseHandle(m_hUIResumeEvent);
		m_hUIResumeEvent = NULL;
	}

	return Result;
}

SMRESULT CScanManager::ScanAndRepairWithoutProgress()
{
    CCTRACEI(_T("CScanManager::ScanAndRepairWithoutProgress() - Calling scan without progrss."));

    // Perform the scan on this thread.
    SMRESULT Result = Scan();

    // Make sure we have a successful scan or an aborted scan before 
    // we show the CommonUI
    if ((Result == SMRESULT_OK) || (Result == SMRESULT_ABORT))
    {
        // Do we need to show UI?
        if (m_UserInterface != SMUI_NO_UI)
        {
            int nFound = GetTotalInfectionsFound();
            int nHandled = GetTotalInfectionsFixed();

            // We're done if no infections were found.
            if (nFound != 0)
            {
                // We're also done if the user interface is 
                // SMUI_NO_UI_UNLESS_INFECTIONS_REMAIN
                // and ALL infections have been dealt with.
                if (m_UserInterface == SMUI_NO_UI_UNLESS_INFECTIONS_REMAIN && 
                    nFound == nHandled )
                {
                    CCTRACEI(_T("CScanManager::ScanAndRepairWithoutProgress() - All infections were handled so we will not display the common UI."));
                }
                else
                {
                    // Infections were found so we need to display the 
                    // Common UI.

                    // Make sure this is a new common UI
                    if (m_pCommonUI != NULL)
                    {
                        delete m_pCommonUI;
                        m_pCommonUI = NULL;
                    }

                    m_pCommonUI = new CCommonUIThread;

                    if( !m_pCommonUI )
                    {
                        // CommonUI creation failed, set result to this error
                        CCTRACEE(_T("CScanManager::ScanAndRepairWithoutProgress() - Failed to create the common UI."));
                        Result = SMRESULT_ERROR_COMMON_UI;
                    }
                    else
                    {
                        //
                        // Now we can display the UI
                        //

                        // Pass some essential information to the Common UI.
                        m_pCommonUI->SetScanTime(m_dwTimeToScan);
                        m_pCommonUI->SetMemoryScanStatus(GetMemoryScanStatus());
                        m_pCommonUI->SetScanCompressedFiles(m_Settings[SMSETTING_SCAN_COMPRESSED_FILES]);
                        m_pCommonUI->SetRepairProgress(m_Settings[SMSETTING_SHOW_REPAIR_PROGRESS]);
                        m_pCommonUI->SetScanNonViralThreats(m_Settings[SMSETTING_SCAN_NON_VIRAL_THREATS]);
                        m_pCommonUI->SetParentWindow(m_hWndParent);

                        // Considering this a quick scan if we ran the eraser scan and there were no items
                        // added to the scan item list
                        bool bQuick = false;

                        if ( m_Settings[SMSETTING_RUN_ERASER_SCAN] == true && 
                            GetScanListItemCount() == 0 )
                        {
                            bQuick = true;
                        }

                        m_pCommonUI->SetQuickScan(bQuick);

                        if (m_Settings[SMSETTING_EMAIL] == true)
                        {
                            m_pCommonUI->SetEmailInfo(m_strEmailSubject.c_str(),
                                                      m_strEmailSender.c_str(),
                                                      m_strEmailRecipient.c_str());
                        }

                        HRESULT hResult = 
                           m_pCommonUI->ShowCUIDlg(CommonUI_RepairAndSummary,
                                                   m_pAnomalyList, this, 
                                                   m_lDamageControlItems);

                        if (FAILED(hResult))
                        {
                            CCTRACEE(_T("CScanManager::ScanAndRepairWithoutProgress() - ShowCUIDlg() failed."));

                            // Indicate failure.
                            Result = SMRESULT_ERROR_COMMON_UI;
                        }
                        else
                        {
                            // Wait for the thread to return
                            m_pCommonUI->WaitForExit(INFINITE);

                            // See if it failed
                            DWORD dwExit = SMRESULT_OK;
                            if( !GetExitCodeThread(m_pCommonUI->GetHandle(), 
                                                   &dwExit) )
                            {
                                CCTRACEE(_T("CScanManager::ScanAndRepairWithoutProgress() - Failed to get thread exit code."));
                            }
                            else if( dwExit != SMRESULT_OK )
                            {
                                CCTRACEE(_T("CScanManager::ScanAndRepairWithoutProgress() - ShowCUIDlg Failed with exit code %d."), dwExit);
                                Result = SMRESULT_ERROR_COMMON_UI;
                            }
                        }

                        // Clean-up common UI
                        if (m_pCommonUI != NULL)
                        {
                            delete m_pCommonUI;
                            m_pCommonUI = NULL;
                        }
                    }
                }
            }
	    }
	}

	return Result;
}

MemoryScanStatus CScanManager::GetMemoryScanStatus()
{
	COSInfo OsInfo;
	if (OsInfo.IsWinNT() == true)
	{
		return MemoryScanNotAvailable;
	}

	if (m_Settings[SMSETTING_SCAN_MEMORY] == false)
	{
		return MemoryNotScanned;
	}

	return MemoryScanned;
}

SMRESULT CScanManager::Scan()
{
    // If we're scanning with UI, wait till the UI is ready.
    if (m_pCommonUI != NULL)
    {
        //
        // Must use MsgWaitForMultipleObjects, instead of WaitForSingleObject,
        // in case any messages come in that need to be pumped before the
        // event is signaled.  If that happens, WaitForSingleObject would be
        // subject to possible deadlock.
        //
        DWORD dwWaitResult = WAIT_TIMEOUT;

        while (WAIT_OBJECT_0 != dwWaitResult)
        {
            dwWaitResult = MsgWaitForMultipleObjects( 1, &m_hUIReadyEvent, 
                                                      FALSE, INFINITE, 
                                                      QS_ALLINPUT );

            if ( WAIT_OBJECT_0 + 1 == dwWaitResult) 
            { 
                //
                // Woke up because there's a message to be processed.
                //
                MSG msg;

                while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
                {
                    //
                    // Any message that comes in shouldn't be a user a
                    // user input message, so need to call
                    // TranslateMessage before dispatching.
                    //
                    DispatchMessage(&msg);
                }
            }
        }
    }

    // See if we need to abort the scan before we start the scan!
    // This would only happen if we had problems displaying the Common UI
    if (m_bAbortScan == true)
    {
        CCTRACEW(_T("CScanManager::Scan() - Aborting the scan before it started"));
        return SMRESULT_ABORT;
    }

    // Keep track of the scan start time.
    long lStartTime = time(NULL);
    CCTRACEI(_T("CScanManager::Scan() - Start time: %ld"), lStartTime);

    // Perform the ccEraser scan if enabled
    ccEraser::eResult eRes = ccEraser::Success;
    if ( m_Settings[SMSETTING_RUN_ERASER_SCAN] )
    {
        // Need to temporarily turn on all of the enabled categories in the 
        // input filter for the full eraser scan
        CCategoryToggle catToggle(m_pInputFilter);
        
        // Always enable all viral categories
        catToggle.TurnOnTemporarily(ccEraser::IAnomaly::Viral);
        catToggle.TurnOnTemporarily(ccEraser::IAnomaly::Malicious);
        catToggle.TurnOnTemporarily(ccEraser::IAnomaly::ReservedMalicious);
        catToggle.TurnOnTemporarily(ccEraser::IAnomaly::Heuristic);

        // Enable\Disable non-viral categories according to the user settings
        for ( DWORD dwCur=m_dwFirstNonViralThreatCategory; 
              dwCur <= m_dwLastKnownThreatCategory; dwCur++ )
        {
            std::map<DWORD, bool>::iterator Iter = 
                                                 m_CategoryEnabled.find(dwCur);

            if( Iter != m_CategoryEnabled.end() && (*Iter).second == true && 
                m_Settings[SMSETTING_SCAN_NON_VIRAL_THREATS] == true )
            {
                catToggle.TurnOnTemporarily((ccEraser::IAnomaly::Anomaly_Category)dwCur);
            }
            else
            {
                catToggle.TurnOffTemporarily((ccEraser::IAnomaly::Anomaly_Category)dwCur);
            }
        }

        m_bEraserScanning = true;

        // Display our own custom text during the eraser scan
        m_ScanMgrCounts.UseInternalText(IDS_GSE_SCAN_TEXT, 10);

        ccEraser::IContextPtr pContext;
        CCTRACEI(_T("CScanManager::Scan() - Calling ccEraser Scan()"));
        eRes = m_pEraser->Scan(m_pEraserCallback, pContext);

        m_ScanMgrCounts.StopUsingInternalText();

        size_t nCompleteCount = 0;
        size_t nAnomCount = 0;

        if ( pContext != NULL )
        {
            nCompleteCount = pContext->GetAnomalyCompleteCount();
            nAnomCount = pContext->GetAnomalyCount();

            ccEraser::IAnomalyListPtr pAnomList;
            pAnomList.Attach(pContext->GetDetectedAnomalyList());

            if ( pAnomList )
            {
                size_t nCount = 0;

                if ( ccEraser::Succeeded(pAnomList->GetCount(nCount)) )
                {
                    CCTRACEI(_T("Eraser Scan() - returned %d anomalies in the list"), nCount);
                }
            }
        }
        else
        {
            CCTRACEE(_T("CScanManager::Scan() - The context object from the ccEraser scan is NULL."));
        }        

        // Also change the internal setting to indicate a GLP scan is not 
        // necessary when an infection is detected since we already performed 
        // the full eraser scan
        m_Settings[SMSETTING_SCAN_GENERIC_LOAD_POINTS] = false;

        m_bEraserScanning = false;
        CCTRACEI(_T("CScanManager::Scan() - Completed eraser scan. eResult = %d. Anomaly count = %d. Anomaly complete count = %d."), eRes, nAnomCount, nCompleteCount);
    }

    // If we have filename\VID items to scan and the scan was not aborted, scan
    // them now
    if ( !m_mapFileVidsToScan.empty() && !m_bAbortScan )
    {
        DoFileVidReverseScan();

        // Clear the list of file vid items
        m_mapFileVidsToScan.clear();
    }

    // Switch to the appropriate file APIs for the file scan
    //
    // We don't want to switch earlier than this since the Common UI/Internet 
    // Explorer loads the Script Blocking COM object out of the Common Files 
    // directory.  On some languages (such as Danish), this directory contains 
    // a high-ascii character that is different in OEM mode. If this happens 
    // while we're scanning in OEM mode, Internet Explorer will not be able to 
    // load the COM object since the path to the Script Blocking DLL in the 
    // registry is in ANSI.
    CFileAPI fileAPI;

    if (m_Settings[SMSETTING_OEM] == true)
    {
        fileAPI.SwitchAPIToOEM();
    }
    else
    {
        fileAPI.SwitchAPIToANSI();
    }

    // Convert the items to OEM if we're scanning in OEM mode.
    if (m_Settings[SMSETTING_OEM] == true)
    {
        ConvertItemsToOEM();
    }

    // Run the regular file scan
    SCANSTATUS Status = SCAN_OK;
    CSymPtr<IScanResults> pResults;

    if ( m_bAbortScan )
    {
        CCTRACEI(_T("CScanManager::Scan() - Scan was already aborted not running file scan."));
        Status = SCAN_ABORT;
    }
    else
    {
        CCTRACEI(_T("CScanManager::Scan() - Calling ccScan Scan()"));
        Status = m_pScanner->Scan(m_pScanItems, m_pScanProperties, 
                                  m_pScanSink, &pResults);
        CCTRACEI(_T("CScanManager::Scan() - ccScan Scan() returned 0x%X"), Status);
    }

    // Save the time it took to perform the scan.
    long lStopTime = time(NULL);

    CCTRACEI(_T("CScanManager::Scan() - Start time: %ld, Stop time: %ld, Paused time: %ld"), lStartTime, lStopTime, m_lTotalTimePaused );

    m_dwTimeToScan = (DWORD) ((lStopTime - lStartTime) - m_lTotalTimePaused);

    // Process all waiting compressed infection items
    OnPostProcessContainer(NULL, NULL);

    // Restore the original file APIs for this process after the scan
    fileAPI.RestoreOriginalAPI();

    // Do some clean-up if we're running with UI (i.e. in a separate thread).
    if (m_pCommonUI != NULL)
    {
        // Pass some essential information to the Common UI.
        m_pCommonUI->SetScanTime(m_dwTimeToScan);
        m_pCommonUI->SetScanComplete(m_pAnomalyList, m_lDamageControlItems);
    }

    // Log the counts we are tracking
    CCTRACEI(_T("ScanManager -  Files scanned: %d"), m_ScanMgrCounts.GetItemCount(CScanManagerCounts::FilesScanned));
    CCTRACEI(_T("ScanManager -  Total viral detections: %d. Repaired: %d. Quarantined: %d. Deleted %d."), 
             m_ScanMgrCounts.GetViralItemsDetected(),                        // Viral detected
             m_ScanMgrCounts.GetItemCount(CScanManagerCounts::Repaired),     // Repaired
             m_ScanMgrCounts.GetItemCount(CScanManagerCounts::Quarantined),  // Quarantined
             m_ScanMgrCounts.GetItemCount(CScanManagerCounts::ViralDeleted)  // Deleted
            );
    CCTRACEI(_T("ScanManager -  Total non-viral count: %d. Non-viral deleted: %d. Non-viral excluded %d."),
             m_ScanMgrCounts.GetItemCount(CScanManagerCounts::NonViral),         // Non-viral detected
             m_ScanMgrCounts.GetItemCount(CScanManagerCounts::NonViralDeleted),  // Deleted
             m_ScanMgrCounts.GetItemCount(CScanManagerCounts::NonViralExcluded)  // Excluded
            );
    
    // Make sure we have valid results
    if (pResults != NULL)
    {
        CCTRACEI(_T("ccScan -  Files scanned: %d"), 
                 pResults->GetTotalFilesScanned());
        CCTRACEI(_T("ccScan -  Total infections: %d"), 
                 pResults->GetTotalInfectionCount());

        // Save boot record counts from the scan results
        m_ScanMgrCounts.SetMbrBrScannedCounts(pResults->GetTotalMBRsScanned(), 
                                              pResults->GetTotalBootRecordsScanned());
    }
    else
    {
        CCTRACEW(_T("CScanManager::Scan() - Scan results objects are NULL"));
    }

    // Ignore decomposition errors.
    // There will always be some files we can't discompose...
    // (i.e. password protected zip files).
    if (Status == SCAN_ERROR_DECOMPOSER)
    {
        return SMRESULT_OK;
    }

    // Otherwise return the appropriate Scan Manager result.
    return NavScanToScanManager(Status);
}

//Check if the item is a non-viral threat
bool CScanManager::IsNonViral(IScanInfection* pInfection)
{
    if( pInfection )
    {
        // Check this infection item against all non-viral threat categories
        if( pInfection->IsCategoryType(SCAN_THREAT_TYPE_SECURITY_RISK) ||
            pInfection->IsCategoryType(SCAN_THREAT_TYPE_HACKTOOL) ||
            pInfection->IsCategoryType(SCAN_THREAT_TYPE_SPYWARE) ||
            pInfection->IsCategoryType(SCAN_THREAT_TYPE_TRACKWARE) ||
            pInfection->IsCategoryType(SCAN_THREAT_TYPE_DIALER) ||
            pInfection->IsCategoryType(SCAN_THREAT_TYPE_REMOTE_ACCESS) ||
            pInfection->IsCategoryType(SCAN_THREAT_TYPE_ADWARE) ||
            pInfection->IsCategoryType(SCAN_THREAT_TYPE_JOKE) )
            return true;
    }

	return false;
}

bool CScanManager::IsViral(IScanInfection* pInfection)
{
    if( pInfection )
    {
        // Check this infection item against all viral threat categories
        if( pInfection->IsCategoryType(SCAN_THREAT_TYPE_VIRUS) ||
            pInfection->IsCategoryType(SCAN_THREAT_TYPE_MALICIOUS) ||
            pInfection->IsCategoryType(SCAN_THREAT_TYPE_RESERVED_MALICIOUS) ||
            pInfection->IsCategoryType(SCAN_THREAT_TYPE_HEURISTIC) )
            return true;
    }

    return false;
}

SMRESULT CScanManager::GetScanner(const char* szAppID,
									 int iBloodhoundLevel,
									 long lFlags)
{
	SMRESULT Result = SMRESULT_OK;

    // Create the scan sink if necessary
    if( m_pScanSink == NULL )
    {
        m_pScanSink = new CScanSink;

        if( m_pScanSink != NULL )
        {
            // Take a ref count
            m_pScanSink->AddRef();
            m_pScanSink->Initialize(this);
        }
        else
        {
            CCTRACEE("CScanManager::GetScanner() - Failed to create the scan sink.");
            return SMRESULT_ERROR_MEMORY;
        }
    }

    // Create the scan properties if necessary
    if( m_pScanProperties == NULL )
    {
        m_pScanProperties = new CScanProperties;

        if( m_pScanProperties != NULL )
        {
            m_pScanProperties->AddRef();
            m_pScanProperties->Initialize(this);
        }
        else
        {
            CCTRACEE("CScanManager::GetScanner() - Failed to create the scan properties.");
            return SMRESULT_ERROR_MEMORY;
        }
    }

    // Create the scan items if necessary
    if( m_pScanItems == NULL )
    {
        m_pScanItems = new CScanItems;

        if( m_pScanItems != NULL )
        {
            m_pScanItems->AddRef();
            m_pScanItems->Initialize(this);
        }
        else
        {
            CCTRACEE("CScanManager::GetScanner() - Failed to create the scan items.");
            return SMRESULT_ERROR_MEMORY;
        }
    }

	// Load the scanner if necessary
	if( m_pScanner == NULL )
	{
        SYMRESULT symRes = SYM_OK;
	    if( SYM_FAILED(symRes = m_ccScanLoader.CreateObject(&m_pScanner)) || m_pScanner == NULL )
        {
		    CCTRACEE("CScanManager::GetScanner() - Error creating the scanner. SYMRESULT = 0x%X", symRes);
            return SMRESULT_ERROR_SCANNER;
        }

        // Always use the 32bit persistable VIDs
        lFlags |= IScanner::ISCANNER_USE_PERSISTABLE_VIDS;

		// Initialize the scanning engine.
		TCHAR szTempDir[MAX_PATH];
		GetTempPath(MAX_PATH, szTempDir);
		GetShortPathName(szTempDir, szTempDir, MAX_PATH);

        // If there was a ccSetting key specified to use test defs load from
        // there without checking authentication
        SCANSTATUS Status = SCAN_OK;
        if( 0 != _tcslen(m_szTestDefs) )
        {
            CCTRACEI("CScanManager::GetScanner() - Loading the scanner using the test defs at %s", m_szTestDefs);
            // Use the specified defs path
            lFlags |= IScanner::ISCANNER_USE_DEF_PATH;
            Status = m_pScanner->Initialize(m_szTestDefs, "", szTempDir,
				iBloodhoundLevel, lFlags);
        }
        else
        {
            // Enable definition authentication
            CCTRACEI("CScanManager::GetScanner() - Loading the scanner using defutils App ID %s", szAppID);
            lFlags |= IScanner::ISCANNER_AUTHENTICATE_DEFS;
		    Status = m_pScanner->Initialize(szAppID, "", szTempDir,
			iBloodhoundLevel, lFlags);
        }

        // Check if the definitions were reverted due to an authentication failure
        if( Status == SCAN_OK_OLD_DEFINITIONS || Status == SCAN_ERROR_DEFINITIONS_NOT_AUTHENTIC )
        {
			UINT uErrorID = 0;

            // Display the defs reverted error
            if( Status == SCAN_OK_OLD_DEFINITIONS )
            {
                uErrorID = IDS_ERR_DEFS_REVERTED;

                // Reset the return code to OK so clients use the scanner
                Status = SCAN_OK;
            }

            // Display the defs bad error
            else if( Status == SCAN_ERROR_DEFINITIONS_NOT_AUTHENTIC )
            {
                uErrorID = IDS_ERR_DEFS_NOT_AUTHENTIC;
            }

			// Format text with product name
			CString strText;
			strText.Format(uErrorID, m_strProductName);

            // Get a NAVError object
		    CComBSTR bstrText(strText);

		    CComBSTR bstrNAVErr(_T("NAVError.NAVCOMError"));
		    CComPtr <INAVCOMError> spNavError;

		    // Create, Populate, Log, and Display the error
		    if( SUCCEEDED( spNavError.CoCreateInstance(bstrNAVErr, NULL, CLSCTX_INPROC_SERVER) ) )
            {
                if( SUCCEEDED(spNavError->put_ModuleID(AV_MODULE_ID_SCANMANAGER)) &&
                    SUCCEEDED(spNavError->put_ErrorID(uErrorID)) &&
                    SUCCEEDED(spNavError->put_Message(bstrText)) )
                {
                    spNavError->LogAndDisplay(0);
                }
                else
                    CCTRACEE(_T("CScanManager::GetScanner() - Failed to fill out the NAV error object"));
            }
            else
                CCTRACEE(_T("CScanManager::GetScanner() - Failed to create the NAV error object"));
        }
		
		if (Status == SCAN_OK)
		{
			ConfiureDecomposerManager();
		}

        if (Status != SCAN_OK)
		{
			// If we are aborting due to an error in a component, update our result
			if (Result == SMRESULT_OK)
			Result = NavScanToScanManager(Status);

            // Release the scanner
            if (m_pScanner != NULL)
		    {
			    m_pScanner->Release();
			    m_pScanner = NULL;
		    }

			CCTRACEE("CScanManager::GetScanner() - Unable to initialize the scanner. Error = 0x%X", Status);
		}
        else
        {
            // Set the maximum decomposer extraction size.
	        m_pScanner->SetMaxDecomposerExtractSize(m_dwMaxDecomposerExtractSize);
        }
	}
    else
    {
        CCTRACEI("CScanManager::GetScanner() - IScanner is already valid.");
    }

	return Result;
}

SMRESULT CScanManager::ConfiureDecomposerManager()
{
	::ccLib::CExceptionInfo exceptionInfo;
	try
	{
		// We need to create our own ccDec object.
		if(SYM_FAILED(::cc::ccDecMgd_IDecomposerMgr2::CreateObject(GETMODULEMGR(), 
						&m_pDecomposerMgr)) || m_pDecomposerMgr == NULL)
			return SMRESULT_ERROR_SCANNER;   	    

#define DECOMPOSER_IMFS_SIZE 0x400000
		// Static list of decomposers.  Make sure Dec2ID engine is always first!
#define DECOMPOSER_COUNT	14

		int iEngineIDs[ DECOMPOSER_COUNT ] = {
				DEC_TYPE_ID,
				DEC_TYPE_ZIP,
				DEC_TYPE_OLESS,
				DEC_TYPE_GZIP,
				DEC_TYPE_CAB,
				DEC_TYPE_LHA,
				DEC_TYPE_RAR,
				DEC_TYPE_ARJ,
				DEC_TYPE_TNEF,
				DEC_TYPE_LZ,
				DEC_TYPE_AMG,
				DEC_TYPE_TAR,
				DEC_TYPE_RTF,
				DEC_TYPE_TEXT
		};

		// Fetch common client decomposer directory.
		::ccLib::CString strDecomposerDir;
		if( FALSE == ccSym::CInstalledApps::GetCCDecDirectory( strDecomposerDir ) )
		{
			CCTRACEE(_T("CScanManager::ConfiureDecomposerManager(): ccSym::CInstalledApps::GetCCDecDirectory() failed."));
			return SMRESULT_ERROR_SCANNER;
		}

		// Largest size of an in-memory file (512Kb)
		int dapiError = m_pDecomposerMgr->Initialize(iEngineIDs, DECOMPOSER_COUNT, "", 
			DECOMPOSER_IMFS_SIZE, strDecomposerDir, 1);
		if(dapiError != DAPI_OK)
		{
			CCTRACEE(_T("CScanManager::ConfiureDecomposerManager() : m_pDecomposerMgr->Initialize == 0x%08X"), dapiError);
			return SMRESULT_ERROR_SCANNER;            
		}

		// Initialize Unicode Mapping.
		m_pDecomposerMgr->InitializeUnicodeMapping();

		m_pDecomposerMgr->SetMaxExtractSize(m_dwMaxDecomposerExtractSize);
		m_pDecomposerMgr->SetOption(DEC_OPTION_TRUNCATE_TEMP_FILES, 0);
		m_pDecomposerMgr->SetOption(DEC_OPTION_EXTRACT_RTFHTML, 1);
		m_pDecomposerMgr->SetOption(DEC_OPTION_ENFORCE_CRC, 0);
		m_pDecomposerMgr->SetOption(DEC_OPTION_EXTRACT_OLE10NATIVE_ONLY, 0);
		m_pDecomposerMgr->SetOption(DEC_OPTION_USE_OLE10NATIVE_CHILD_NAME, 1);
		m_pDecomposerMgr->SetOption(DEC_OPTION_ENABLE_MIME_ENGINE, 1);
		m_pDecomposerMgr->SetOption(DEC_OPTION_ENABLE_BINHEX_ENGINE, 1);
		m_pDecomposerMgr->SetOption(DEC_OPTION_ENABLE_UUE_ENGINE, 1);
		m_pDecomposerMgr->SetOption(DEC_OPTION_NONMIMETHRESHOLD, 16384);					// Changed from 100,000, now matches UUE engine
		m_pDecomposerMgr->SetOption(DEC_OPTION_MAX_TEXT_SCAN_BYTES, 16384);					// Changed from 100,000, limits ALL text sub-engines to 16K
		m_pDecomposerMgr->SetOption(DEC_OPTION_MIME_FUZZY_MAIN_HEADER, 1 );
		m_pDecomposerMgr->SetOption(DEC_OPTION_MIME_IDENTIFICATION_STRENGTH, 1 );			// Added (default would be 0)
		m_pDecomposerMgr->SetOption(DEC_OPTION_ENHANCED_TEXT_ID, TEXT_OPTION_ABORT_ON_BINARY);
		return SMRESULT_OK;
	}
	CCCATCHMEM(exceptionInfo);

	if(exceptionInfo.IsException())
	{
		return SMRESULT_ERROR_MEMORY;
	}
	else
	{
		CCTRACEE(_T("CScanManager::ConfiureDecomposerManager(): Error creating decomposers."));
		return SMRESULT_ERROR_SCANNER;
	}
}

SMRESULT CScanManager::GetEraser()
{
    // Synchronize access around initializing eraser to be safe
    ccLib::CSingleLock Lock(&m_critScanSink, INFINITE, FALSE);

    SMRESULT Result = SMRESULT_OK;

    if( m_pEraser != NULL )
    {
        CCTRACEI(_T("CScanManager::GetEraser() - IEraser is already valid."));
        return Result;
    }

    ATL::CAtlString strEraserPath;
    CScanPtr <IScanDefinitionsInfo> spDefInfo;
    if ( SCAN_OK == m_pScanner->GetDefinitionsInfo ( &spDefInfo ))
    {
        if ( spDefInfo )
        {
            strEraserPath = spDefInfo->GetDefinitionsDirectory();

            // If our scanner object is in OEM mode the path will be in OEM, need
            // to convert it to ANSI
            if( m_Settings[SMSETTING_OEM] == true )
            {
                strEraserPath.OemToAnsi();
            }
        }
        else
            CCTRACEE(_T("CScanManager::GetEraser() - Error getting the definitions info object. Will not be able to load ccEraser."));
    }

    // Append the dll name
    strEraserPath += _T("\\");
    strEraserPath += cc::sz_ccEraser_dll;

    CCTRACEI(_T("CScanManager::GetEraser() - Loading IEraser from %s"), (LPCTSTR)strEraserPath);
    SYMRESULT symRes = SYM_OK;
    if( SYM_SUCCEEDED(symRes = m_ccEraserLoader.Initialize(strEraserPath)) )
    {
        if( SYM_SUCCEEDED(symRes = m_ccEraserLoader.CreateObject(ccEraser::IID_Eraser, ccEraser::IID_Eraser, reinterpret_cast<void**>(&m_pEraser))) )
        {
            CCTRACEI(_T("CScanManager::GetEraser() - Successfully loaded the IEraser object. Attempting initialization"));
            ccEraser::eResult resEraser = ccEraser::Success;

            // If the eraser version is 103.5.x.x make sure it is at least 103.5.3.x
            DWORD dwVerMost = 0;
            DWORD dwVerLeast = 0;
            m_pEraser->GetEngineVersion(dwVerMost, dwVerLeast);
            BOOL bIsInvalidEraserVersion = ((HIWORD(dwVerMost)) == 103 && (LOWORD(dwVerMost)) == 5 && (HIWORD(dwVerLeast)) < 3);
            if(!bIsInvalidEraserVersion)
            {
                // Create the eraser scan sink if necessary
                if( m_pEraserScanSink == NULL )
                    m_pEraserScanSink = new CEraserScanSink;

                // Create the eraser callbacks if necessary
                if( m_pEraserCallback == NULL )
                    m_pEraserCallback = new CEraserCallbacks;

                if( m_pEraserScanSink != NULL && m_pEraserCallback != NULL )
                {
                    // Pass the scan manager pointer into the sink and callback
                    m_pEraserScanSink->AddRef();
                    m_pEraserScanSink->Initialize(this);

                    m_pEraserCallback->AddRef();
                    m_pEraserCallback->Initialize(this);

                    resEraser = m_pEraser->Initialize(m_pScanner, m_pEraserScanSink, m_pScanProperties);
                    if( ccEraser::Succeeded(resEraser) )
                    {
                        // ccEraser was able initialize
                        CCTRACEI(_T("CScanManager::GetEraser() - Successfully loaded ccEraser. Version = %d.%d.%d.%d"), (HIWORD(dwVerMost)), (LOWORD(dwVerMost)), (HIWORD(dwVerLeast)), (LOWORD(dwVerLeast)));

                        if(ccEraser::RebootRequired == resEraser)
                        {
                            // ccEraser has been recently updated, and requires a reboot
                            CCTRACEI(_T("CScanManager::GetEraser() - ccEraser has been recently updated, and requires a reboot."));
                            Result = SMRESULT_ERROR_ERASER_REBOOT_REQUIRED;
                        }
                    }
                    else
                    {
                        CCTRACEE(_T("CScanManager::GetEraser() - Error initializing IEraser. ccEraser::eResult = %d"), resEraser);
                        Result = SMRESULT_ERROR_ERASER;
                    }
                }
            }
            else
            {
                // The eraser version is pre-103.5.3.x we will display the virus defs not authentic
                // error and refuse to scan with this version of eraser
                CCTRACEW(_T("CScanManager::GetEraser() - This version of eraser does not meet the minimum requirements. Version = %d.%d.%d.%d"), (HIWORD(dwVerMost)), (LOWORD(dwVerMost)), (HIWORD(dwVerLeast)), (LOWORD(dwVerLeast)));
                Result = SMRESULT_ERROR_DEF_AUTHENTICATION_FAILURE;

                HandleInvalidEraserVersion(dwVerMost, dwVerLeast);
            }
        }
        else
        {
            CCTRACEE(_T("CScanManager::GetEraser() - Error creating the IEraser object. SYMRESULT = 0x%X"), symRes);
            Result = SMRESULT_ERROR_ERASER;
        }
    }
    else
    {
        CCTRACEE(_T("CScanManager::GetEraser() - Error initializing the ccEraser loader. SYMRESULT = 0x%X"), symRes);
        Result = SMRESULT_ERROR_ERASER;
    }

    return Result;
}

void CScanManager::HandleInvalidEraserVersion(DWORD dwVerMost, DWORD dwVerLeast)
{
    // Format text with product name
    CString strText;
    strText.Format(IDS_ERR_DEFS_NOT_AUTHENTIC, m_strProductName);

    // Get a NAVError object
    CComBSTR bstrText(strText);

    CComBSTR bstrNAVErr(_T("NAVError.NAVCOMError"));
    CComPtr <INAVCOMError> spNavError;

    // Create, Populate, Log, and Display the error
    if( SUCCEEDED( spNavError.CoCreateInstance(bstrNAVErr, NULL, CLSCTX_INPROC_SERVER) ) )
    {
        if( SUCCEEDED(spNavError->put_ModuleID(AV_MODULE_ID_SCANMANAGER)) &&
            SUCCEEDED(spNavError->put_ErrorID(IDS_ERR_DEFS_NOT_AUTHENTIC)) &&
            SUCCEEDED(spNavError->put_Message(bstrText)) )
        {
            spNavError->LogAndDisplay(0);
        }
        else
            CCTRACEE(_T("CScanManager::HandleInvalidEraserVersion() - Failed to fill out the NAV error object"));
    }
    else
    {
        CCTRACEE(_T("CScanManager::HandleInvalidEraserVersion() - Failed to create the NAV error object"));
    }

    return;
}

SMRESULT CScanManager::SetEraserSettings()
{
    ccLib::CSingleLock Lock(&m_critScanSink, INFINITE, FALSE);

    SMRESULT Result = SMRESULT_OK;

    if( m_pEraser == NULL )
    {
        CCTRACEE(_T("CScanManager::SetEraserSettings() - Error m_pEraser is NULL."));
        return SMRESULT_ERROR_ERASER;
    }

    // Get the settings
    ccEraser::eResult eRes = ccEraser::Success;
    cc::IKeyValueCollectionPtr pSettings;
    eRes = m_pEraser->GetSettings(pSettings);
    if( ccEraser::Succeeded(eRes) )
    {
        // Set the scanning depth property
        if( !pSettings->SetValue(ccEraser::IEraser::Depth, m_dwScanDllDepth) )
            CCTRACEE(_T("CScanManager::SetEraserSettings() - Failed to set ccEraser::IEraser::Depth"));

        // Set the scan DLLs property
        if( !pSettings->SetValue(ccEraser::IEraser::ScanDlls, m_Settings[SMSETTING_SCAN_DLLS]) )
            CCTRACEE(_T("CScanManager::SetEraserSettings() - Failed to set ccEraser::IEraser::ScanDlls"));
    
        // Set OverrideDetectAnomaliesGLPScan to true.  Scan Manager does not need
        //  ccEraser to perform the GLP scan when calling DetectAnomalies.
        if( !pSettings->SetValue(ccEraser::IEraser::OverrideDetectAnomaliesGLPScan, true) )
            CCTRACEE(_T("CScanManager::SetEraserSettings() - Failed to set ccEraser::IEraser::OverrideDetectAnomaliesGLPScan"));
    

        // Set the filters up
        if( m_pInputFilter != NULL )
        {
            CCTRACEI(_T("CScanManager::SetEraserSettings() - The input filter is not null, releasing first."));
            m_pInputFilter.Release();
        }
        if( m_pOutputFilter != NULL )
        {
            CCTRACEI(_T("CScanManager::SetEraserSettings() - The output filter is not null, releasing first."));
            m_pOutputFilter.Release();
        }

        if( ccEraser::Succeeded(eRes = m_pEraser->CreateObject(ccEraser::FilterType, ccEraser::IID_Filter, reinterpret_cast<void**>(&m_pInputFilter))) &&
            ccEraser::Succeeded(eRes = m_pEraser->CreateObject(ccEraser::FilterType, ccEraser::IID_Filter, reinterpret_cast<void**>(&m_pOutputFilter))) )
        {
            SetEraserFilter();

            // Set the filter in the settings object as the input filter, we will not use an output filter
            if( !pSettings->SetValue(ccEraser::IEraser::InputFilter, m_pInputFilter) )
                CCTRACEE(_T("CScanManager::SetEraserSettings() - Failed to set ccEraser::IEraser::InputFilter"));
            if( !pSettings->SetValue(ccEraser::IEraser::OutputFilter, m_pOutputFilter) )
                CCTRACEE(_T("CScanManager::SetEraserSettings() - Failed to set ccEraser::IEraser::OutputFilter"));
        }
        else
        {
            CCTRACEE(_T("CScanManager::SetEraserSettings() - IEraser::GetSettings() failed to create the IFilter objects. eResult = %d"), eRes);
            Result = SMRESULT_ERROR_ERASER;
        }
    }
    else
    {
        CCTRACEE(_T("CScanManager::SetEraserSettings() - IEraser::GetSettings() failed. eResult = %d"), eRes);
        Result = SMRESULT_ERROR_ERASER;
    }

    return Result;
}

void CScanManager::SetEraserFilter()
{
    // Set the output filter according to the user enabled categories
    if( m_pOutputFilter == NULL )
    {
        CCTRACEE(_T("CScanManager::SetEraserFilter() - The output filter member is null."));
        return;
    }

    // Clear the output filter first just in case
    m_pOutputFilter->ClearAnomalyFilter();
    m_pOutputFilter->ClearDetectionFilter();

    // Always enable all viral categories
    m_pOutputFilter->AddAnomalyCategory(ccEraser::IAnomaly::Viral);
    m_pOutputFilter->AddAnomalyCategory(ccEraser::IAnomaly::Malicious);
    m_pOutputFilter->AddAnomalyCategory(ccEraser::IAnomaly::ReservedMalicious);
    m_pOutputFilter->AddAnomalyCategory(ccEraser::IAnomaly::Heuristic);

    // Disable generic load points for the output filter so we don't get GLP only items
    // or detections for categories that are disabled and were detected during the GLP
    m_pOutputFilter->RemoveAnomalyCategory(ccEraser::IAnomaly::GenericLoadPoint);

    // Disable client compliancy category for the output filter
    m_pOutputFilter->RemoveAnomalyCategory(ccEraser::IAnomaly::ClientCompliancy);

    // Enable\Disable non-viral categories according to the user settings
    for( DWORD dwCur=m_dwFirstNonViralThreatCategory; dwCur <= m_dwLastKnownThreatCategory; dwCur++ )
    {
        std::map<DWORD, bool>::iterator Iter = m_CategoryEnabled.find(dwCur);

        if( Iter != m_CategoryEnabled.end() && (*Iter).second == true && m_Settings[SMSETTING_SCAN_NON_VIRAL_THREATS] == true )
            m_pOutputFilter->AddAnomalyCategory((ccEraser::IAnomaly::Anomaly_Category)dwCur);
        else
            m_pOutputFilter->RemoveAnomalyCategory((ccEraser::IAnomaly::Anomaly_Category)dwCur);
    }

    //
    // Set the input filter to just GLP
    //
    if( m_pInputFilter == NULL )
    {
        CCTRACEE(_T("CScanManager::SetEraserFilter() - The filter member is null."));
        return;
    }

    // Clear the input filter
    m_pInputFilter->ClearAnomalyFilter();
    m_pInputFilter->ClearDetectionFilter();

    // Enable generic load point scanning
    m_pInputFilter->AddAnomalyCategory(ccEraser::IAnomaly::GenericLoadPoint);

    // Set client compliancy option (off by default)
    if( m_Settings[SMSETTING_SCAN_CLIENT_COMPLIANCY] )
        m_pInputFilter->AddAnomalyCategory(ccEraser::IAnomaly::ClientCompliancy);
    else
        m_pInputFilter->RemoveAnomalyCategory(ccEraser::IAnomaly::ClientCompliancy);

    // Setup the detection filter to the detection action we are aware of
    if( m_Settings[SMSETTING_DONT_SCAN_MEMORY] )
        m_pInputFilter->RemoveDetectionType(ccEraser::ProcessDetectionActionType);
    else
        m_pInputFilter->AddDetectionType(ccEraser::ProcessDetectionActionType);

    m_pInputFilter->AddDetectionType(ccEraser::RegistryDetectionActionType);
    m_pInputFilter->AddDetectionType(ccEraser::FileDetectionActionType);
    m_pInputFilter->AddDetectionType(ccEraser::BatchDetectionActionType);
    m_pInputFilter->AddDetectionType(ccEraser::INIDetectionActionType);
    m_pInputFilter->AddDetectionType(ccEraser::ServiceDetectionActionType);
    m_pInputFilter->AddDetectionType(ccEraser::StartupDetectionActionType);
    m_pInputFilter->AddDetectionType(ccEraser::COMDetectionActionType);
    m_pInputFilter->AddDetectionType(ccEraser::DirectoryDetectionActionType);
    m_pInputFilter->AddDetectionType(ccEraser::LSPDetectionActionType);
    m_pInputFilter->AddDetectionType(ccEraser::HostsDetectionActionType);
}

bool CScanManager::HandleAnomaly(CEZAnomaly* pEZAnomaly)
{
    if ( pEZAnomaly == NULL )
    {
        CCTRACEE(_T("CScanManager::HandleAnomaly() - Anomaly param is null returning false."));
        return false;
    }

    // Debug Log the anomaly details first
    cc::IStringPtr pStrName;
    pEZAnomaly->GetName(pStrName);

    cc::IStringPtr pStrID;
    pEZAnomaly->GetID(pStrID);

    CCTRACEI(_T("CScanManager::HandleAnomaly() - Processing anomaly. Name = %s ID = %s"), pStrName ? pStrName->GetStringA() : "No Name", pStrID ? pStrID->GetStringA() : "No ID");

    // Get the anomaly user data collection
    cc::IKeyValueCollectionPtr pUserData = pEZAnomaly->GetUserData();

    if ( pUserData == NULL )
    {
        CCTRACEE(_T("CScanManager::HandleAnomaly() - Failed to get the user data for anomaly"));
        return false;
    }

    // Setup an anomaly context
    CAnomalyContext context;
    context.m_pEZAnomaly = pEZAnomaly;

    // First pre-process this anomaly to determing what if any actions need to 
    // be taken
    //
    if ( PreProcessAnomaly(context) == false )
    {
        // There is no real processing necessary for this item
        return true;
    }

    // Assume viral
    bool bViral = true;

    if ( pEZAnomaly->IsNonViral() )
    {
        bViral = false;
    }

    // We only want to check for the presence of each memory item one time.
    // ProfileMemoryItems will return true if at least one memory item is 
    // active
    if ( pEZAnomaly->ProfileMemoryItems() == true )
    {
        // Only prompt once per run through the anomalies
        if ( !m_bTerminateProcessPrompted )
        {
            // Prompt the user now if we have a common UI
            if ( m_pCommonUI != NULL )
            {
                CCTRACEI(_T("CScanManager::HandleAnomaly() - Calling common UI to prompt for memory termination."));
                m_pCommonUI->ShowMemTerminationDlg(m_bTerminateProcessAccepted,
                                                   m_bAutomaticMode);
                CCTRACEI(_T("CScanManager::HandleAnomaly() - Common UI memory termination response = %d."), m_bTerminateProcessAccepted);
                m_bTerminateProcessPrompted = true;
            }
            else
            {
                CCTRACEI(_T("CScanManager::HandleAnomaly() - Detected a memory remediation but no CommonUI available to prompt the user. We will not terminate automatically."));
            }
        }

        // Now we see what the response was before attempting to process this 
        // anomaly
        if ( !m_bTerminateProcessAccepted )
        {
            // We should not terminate processes so bail out of this anomaly 
            // handling now
            CCTRACEI(_T("CScanManager::HandleAnomaly() - This anomaly has a process remediation and the user has not allowed us to terminate processes so we will skip this anomaly."));

            // Make sure there is a next action to take set for this anomaly
            if ( !pUserData->GetExists(ActionToTake) )
            {
                if ( bViral )
                {
                    // Viruses will be set to repair as the first operation
                    pEZAnomaly->SetAnomalyStatus(ACTION_REPAIR, 
                                                 InfectionStatus_Unhandled);
                }
                else
                {
                    // Get the recommended action for this non-viral threat
                    AnomalyAction action = ACTION_DELETE;
                    pEZAnomaly->GetRecommendedAction(action);
                    pEZAnomaly->SetAnomalyStatus(action, 
                                                 InfectionStatus_Unhandled);
                }
            }

            return true;
        }
    }

    //
    // Process this anomaly
    //
    ccEraser::eResult eRes = ccEraser::Success;

    // vector of remediation data for operations performed
    vector<CRemediationData> vRemData;

    // This will store the first remediation scan infection item that will be 
    // stored in quarantine if all other processing succeeds
    CSymPtr<IQuarantineItem> pQuarItem = NULL;
    IScanInfectionQIPtr pQuarItemScanInfection = NULL;
    bool bCreatedNewQuarItem = false;

    // If there is an existing quarantine item use it
    UUID uuid = {0};

    if ( context.m_bQuarantineBackup && 
         pEZAnomaly->GetQuarantineItemUUID(uuid) )
    {
        CCTRACEI(_T("CScanManager::HandleAnomaly() - This is a filename\\vid detected item, will attempt to load an existing qurantine item."));

        if ( !m_Quarantine.LoadQuarantineItemFromUUID(uuid, &pQuarItem) )
        {
            CCTRACEE(_T("CScanManager::HandleAnomaly() - Failed to load the existing qurantine item"));
        }
    }

    // This is set to false if we have failed the operation for this anomaly
    bool bSucceeded = true;

    // We want to know if the repair operation was actually a delete
    bool bRepairDelete = false;

    // Do we have undo data for any of the remediation actions?
    bool bHaveAnUndoForAtLeastOneRemediation = false;

    // Do we need a snap shot for the quarantine item?
    bool bTakeSnapShot = false;

    // For non-viral deletion failures, UI needs to know of the failure
    // happened on the quarantine attempt (as opposed to during the 
    // remediations).
    bool bNonViralDeleteFailedOnQuarantine = false;

    // Get the remediation list count
    size_t nCount = pEZAnomaly->GetRemediationCount();

    // Get the first action to attempt from the context
    std::vector<DWORD>::iterator IterCurAction = context.m_vdwActions.begin();
    DWORD dwAction = (*IterCurAction);

    // Now we loop through each remediation action and perform our magic
    for (size_t nCur = 0; nCur < nCount; nCur++)
    {
        ccEraser::IRemediationActionPtr pCurAction = 
                                             pEZAnomaly->GetRemediation(nCur);

        if ( pCurAction == NULL )
        {
            CCTRACEE(_T("CScanManager::HandleAnomaly() - Error getting remediation action item %d."), nCur);
            continue;
        }

        CEZRemediation ezRem(pCurAction);

        // Log this remediation action description
        cc::IStringPtr pStrRemDesc;

        if ( ccEraser::Succeeded(ezRem->GetDescription(pStrRemDesc)) )
        {
            CCTRACEI(_T("CScanManager::HandleAnomaly() - Remediation action %d. Description = %s"), nCur, pStrRemDesc->GetStringA() );
        }

        // Skip memory items completely that the ProfileMemoryItems did not 
        // mark as present
        if ( ezRem.IsProcessType() && ezRem.IsActiveInMemory() == false )
        {
            CCTRACEI(_T("CScanManager::HandleAnomaly() - This is a process or service stop remediation type that is not active in memory. Not performing action."));

            // If this is the case then we will manually mark the remediation 
            // as NothingToDo
            ezRem.SetRemediationResult(ccEraser::NothingToDo);
            continue;
        }

        // If we have already performed a successful remediation action for 
        // this item then skip it
        ccEraser::IRemediationAction::Action_State eCurrentState = 
                                                              ezRem.GetState();

        if ( eCurrentState == ccEraser::IRemediationAction::Remediated )
        {
            // Now check what the result of the remediation was
            ccEraser::eResult ePreviousRemediationResult = ccEraser::Fail;

            if ( ezRem.GetRemediationResult(ePreviousRemediationResult) && 
                 ccEraser::Succeeded(ePreviousRemediationResult) )
            {
                CCTRACEI(_T("CScanManager::HandleAnomaly() - We have already successfully remediated this item with result = %d. Skipping this time around."), ePreviousRemediationResult);
                continue;
            }
        }

        // Get the type for this remediation action
        ccEraser::eObjectType type = ezRem.GetType();

        if ( type == ccEraser::InfectionRemediationActionType )
        {
            // Set the repair mode for the remediation action of the scan 
            // infection item and set the user data to the correct action we 
            // want to perform for tracking
            if ( dwAction == ACTION_REPAIR )
            {
                ezRem.
                  SetThreatOperation(ccEraser::IRemediationAction::RepairOnly);

                IScanInfectionPtr spTempInf = ezRem.GetScanInfection();

                if ( spTempInf )
                {
                    spTempInf->SetUserData(InfectionStatus_Repaired);
                }
            }
            else
            {
                ezRem.
                  SetThreatOperation(ccEraser::IRemediationAction::DeleteOnly);

                IScanInfectionPtr spTempInf = ezRem.GetScanInfection();

                if ( spTempInf )
                {
                    if ( dwAction == ACTION_QUARANTINE )
                    {
                        spTempInf->SetUserData(InfectionStatus_Quarantined);
                    }
                    else
                    {
                        spTempInf->SetUserData(InfectionStatus_Deleted);
                    }
                }
            }

            // If we still need to create a new backup item in quarantine do it
            // now and then remediate the infection
            if ( context.m_bQuarantineBackup && pQuarItem == NULL )
            {
                CCTRACEI(_T("CScanManager::HandleAnomaly() - This is the first scan infection remediation action. Adding to quarantine..."));

                pQuarItemScanInfection = ezRem.GetScanInfection();

                if ( NULL == pQuarItemScanInfection.m_p )
                {
                    CCTRACEE(_T("CScanManager::HandleAnomaly() - Failed to get the scan infection object to add to quarantine."));
                    bSucceeded = false;
                    break;
                }

                std::string strFileName;
                m_DisplayNames.GetDisplayNameForFile(pQuarItemScanInfection, 
                                                     strFileName);
                
                CCTRACEI(_T("CScanManager::HandleAnomaly() - Adding file: %s to quarantine"), strFileName.c_str());

                bool bAdded = m_Quarantine.Add( pQuarItemScanInfection, 
                                                context.m_ulQuarItemStatus, 
                                                false, strFileName.c_str(), 
                                                &pQuarItem );
                
                if ( !bAdded || pQuarItem == NULL )
                {
                    // See if the scan infection file was wiped out from 
                    // underneath us somehow, possibly by another repair or 
                    // delete operation
                    if ( IsScanInfectionFileGone(pQuarItemScanInfection) )
                    {
                        CCTRACEW(_T("CScanManager::HandleAnomaly() - The scan infection file we wanted to add to quarantine is already gone...treating as a repair delete and letting the remediation attempt decide the true fate."));
                        pQuarItemScanInfection.Release();
                        bRepairDelete = true;
                    }
                    else
                    {
                        CCTRACEE(_T("CScanManager::HandleAnomaly() - Failed to add the scan infection item to quarantine. VID = %lu, Name = %s"), pQuarItemScanInfection->GetVirusID(), pQuarItemScanInfection->GetVirusName());

                        // Is there a second operation to attempt for this 
                        // anomaly before we truly fail?
                        IterCurAction++;

                        if ( IterCurAction != context.m_vdwActions.end() )
                        {
                            dwAction = (*IterCurAction);

                            // Modify the context object as required for the 
                            // new operation
                            if ( dwAction == ACTION_QUARANTINE )
                            {
                                CCTRACEW(_T("CScanManager::HandleAnomaly() - Repair operation failed, now attempting quarantine."));
                                context.m_bQuarantineBackup = true;
                                context.m_ulQuarItemStatus = 
                                                      QFILE_STATUS_QUARANTINED;
                            }
                            else if ( dwAction == ACTION_DELETE )
                            {
                                CCTRACEW(_T("CScanManager::HandleAnomaly() - Repair operation failed, now attempting delete."));
                                context.m_bQuarantineBackup = false;
                            }
                            else
                            {
                                CCTRACEE(_T("CScanManager::HandleAnomaly() - This is not a valid automatic second action."));
                                bSucceeded = false;
                                break;
                            }

                            // Try this remediation again
                            nCur--;
                            continue;
                        }

                        // Set the status on the infection item to the 
                        // appropriate failure state
                        CommonUIInfectionStatus eFailureStatus = 
                                                 InfectionStatus_Repair_Failed;

                        if ( dwAction == ACTION_REPAIR )
                        {
                            eFailureStatus = InfectionStatus_Repair_Failed;
                        }
                        else if ( dwAction == ACTION_QUARANTINE )
                        {
                            eFailureStatus = InfectionStatus_Quarantine_Failed;
                        }
                        else
                        {
                            eFailureStatus = InfectionStatus_Delete_Failed;

                            if (!bViral) 
                            {
                                bNonViralDeleteFailedOnQuarantine = true;
                            }
                        }

                        if ( pQuarItemScanInfection )
                        {
                            pQuarItemScanInfection->SetUserData(eFailureStatus);
                        }

                        bSucceeded = false;
                        break;
                    }
                }
                else
                {
                    bCreatedNewQuarItem = true;

                    // Take a system snapshot for non-viral anomalies that do 
                    // not already have a specific eraser repair script
                    if ( !bViral && !pEZAnomaly->IsSpecific() )
                    {
                        CCTRACEI(_T("CScanManager::HandleAnomaly() - This is a non-viral anomaly without a specific repair script. Taking snapshot."));
                        bTakeSnapShot = true;
                    }
                }
            }
        }

        // Temp storage for remediation and undo data
        CRemediationData remData;
        remData.pRemediation = ezRem;

        // Log process remediations before attempting
        ccEraser::IRemediationAction::Operation_Type opType = 
                                                      ezRem.GetOperationType();

        if ( (type == ccEraser::ProcessRemediationActionType && 
              opType == ccEraser::IRemediationAction::Suspend) ||
             (type == ccEraser::ServiceRemediationActionType && 
              opType == ccEraser::IRemediationAction::Stop) )
        {
            if ( m_pActivityLog != NULL )
            {
                m_pActivityLog->
                      LogProcessTermination(ezRem, 
                                            m_strProductName.GetString(), 
                                            pStrName ? pStrName->GetStringA() :
                                                       CResourceHelper::LoadString (IDS_UNKOWN_TEXT, g_hInstance).c_str());
            }
        }

        // If this is the item that was already manually added to quarantine 
        // mark it as such in the remediation data backup class so that we 
        // don't add it to the quarantine item if operations succeed, but we 
        // still have it for undo if operation fail
        if ( type == ccEraser::InfectionRemediationActionType && 
             pQuarItemScanInfection == ezRem.GetScanInfection() )
        {
            remData.bManuallyQuarantined = true;
        }

        // Save undo information for this remediation action before performing
        // remediation
        if ( GetUndoData(ezRem, remData.pUndo, remData.strFileName) )
        {
            // Set a flag so we know if we need to create an empty quarantine
            // item
            bHaveAnUndoForAtLeastOneRemediation = true;
        }

        // Do the remediation
        if ( !PerformRemediationAction(&ezRem, remData.eRemediationResult) )
        {
            // Remediation failed.
            //
            // If this is a scan infection item then fail the repair 
            // completely unless there is a next operation to attempt
            if ( type == ccEraser::InfectionRemediationActionType )
            {
                // Is there a second operation to attempt for this anomaly 
                // before we truly fail?
                IterCurAction++;

                if ( IterCurAction == context.m_vdwActions.end() )
                {
                    CCTRACEE(_T("CScanManager::HandleAnomaly() - Remediation for item %d failed. This is a scan infection remediation action so the entire anomaly will be failed!!!"), nCur);
                    bSucceeded = false;
                    break;
                }
                else
                {
                    // Attempt the next operation
                    dwAction = (*IterCurAction);

                    // Modify the context object as required for the new 
                    // operation
                    if( dwAction == ACTION_QUARANTINE )
                    {
                        // Quarantine is only valid for infections
                        CCTRACEW(_T("CScanManager::HandleAnomaly() - Repair operation failed, now attempting quarantine."));

                        context.m_bQuarantineBackup = true;
                        context.m_ulQuarItemStatus = QFILE_STATUS_QUARANTINED;

                        // If we already created a quarantine backup item 
                        // during the repair operation convert it to a 
                        // quarantine item
                        if ( bCreatedNewQuarItem && pQuarItem != NULL )
                        {
                            if ( SUCCEEDED(pQuarItem->SetFileStatus(QFILE_STATUS_QUARANTINED)) 
                                 && 
                                 SUCCEEDED(pQuarItem->SaveItem()) )
                            {
                                CCTRACEI(_T("CScanManager::HandleAnomaly() - Converted existing qurantine item from backup item to quarantine item."));
                            }
                            else
                            {
                                CCTRACEE(_T("CScanManager::HandleAnomaly() - Failed to convert existing qurantine item from backup item to quarantine item. Removing the backup item."));

                                // Remove this quarantine item and attempt a 
                                // fresh quarantine operation
                                if ( pQuarItemScanInfection != NULL )
                                {
                                    if ( m_Quarantine.Delete(pQuarItemScanInfection) )
                                    {
                                        pQuarItemScanInfection.Release();
                                        pQuarItem.Release();
                                        bCreatedNewQuarItem = false;
                                    }
                                }
                            }
                        }
                    }
                    else if (dwAction == ACTION_DELETE )
                    {
                        CCTRACEW(_T("CScanManager::HandleAnomaly() - Repair operation failed, now attempting delete."));

                        // If we created a quarantine backup item we will need 
                        // to delete it
                        if( bCreatedNewQuarItem && pQuarItem != NULL && 
                            pQuarItemScanInfection != NULL )
                        {
                            CCTRACEI(_T("CScanManager::HandleAnomaly() - Deleting the quarantine backup item we created."));
 
                            if ( m_Quarantine.Delete(pQuarItemScanInfection) )
                            {
                                pQuarItemScanInfection.Release();
                                pQuarItem.Release();
                                bCreatedNewQuarItem = false;
                            }
                        }

                        context.m_bQuarantineBackup = false;
                    }
                    else
                    {
                        CCTRACEE(_T("CScanManager::HandleAnomaly() - This is not a valid automatic second action."));
                        bSucceeded = false;
                        break;
                    }

                    // Re-attempting this remediation action with the next 
                    // action
                    nCur--;
                    continue;
                }
            }
            else
            {
                //
                // Failure was not on a scan infection item.
                //
                CCTRACEE(_T("CScanManager::HandleAnomaly() - Remediation for item %d of type %d failed with eResult = %d. Will continue processing anomaly"), nCur, type, remData.eRemediationResult);

                // See if we want to take the snapshot for this item since 
                // there was a failure
                if (!bViral)
                {
                    CCTRACEI(_T("CScanManager::HandleAnomaly() - VID = %lu is non-viral and a remediation action failed. Taking snapshot"), pEZAnomaly->GetVID());
                    bTakeSnapShot = true;
                }
                else if ( context.m_ulQuarItemStatus == 
                                                   QFILE_STATUS_QUARANTINED ) 
                              // Only take snapshot for quarantine viral items
                {
                    // Only take the snapshot if the No_Submit flag is not 
                    // present
                    IScanVirusInfoPtr spVirInfo;

                    if ( SCAN_OK == m_pScanner->GetVirusInfo(pEZAnomaly->GetVID(),
                                                             &spVirInfo) 
                        && 
                        spVirInfo != NULL )
                    {
                        if ( spVirInfo->IsSubmittable() )
                        {
                            CCTRACEI(_T("CScanManager::HandleAnomaly() - The No_Submit flag is not set for VID = %lu. Taking snapshot"), pEZAnomaly->GetVID());
                            bTakeSnapShot = true;
                        }
                        else
                        {
                            CCTRACEI(_T("CScanManager::HandleAnomaly() - The No_Submit flag is set for VID = %lu. Not taking snapshot"), pEZAnomaly->GetVID());
                        }
                    }
                    else
                    {
                        CCTRACEE(_T("CScanManager::HandleAnomaly() - Failed to get the virus info object for VID = %lu"), pEZAnomaly->GetVID());
                    }
                }
            }
        }

        // For infection types check if the repair was actually a delete
        if ( ezRem.IsScanInfectionType() )
        {
            IScanInfectionPtr pInfection = ezRem.GetScanInfection();

            if ( dwAction == ACTION_REPAIR && pInfection && 
                 pInfection->GetStatus() == IScanInfection::DELETED )
            {
                // Set the status to deleted since that's what really happened
                pInfection->SetUserData(InfectionStatus_Deleted);
                bRepairDelete = true;
            }
        }

        // Log process remediation success\failures
        if ( (type == ccEraser::ProcessRemediationActionType && 
              opType == ccEraser::IRemediationAction::Suspend) ||
             (type == ccEraser::ServiceRemediationActionType && 
              opType == ccEraser::IRemediationAction::Stop) )
        {
            if ( m_pActivityLog != NULL ) 
            {
                m_pActivityLog->LogProcessTermination(ezRem, 
                                                      m_strProductName.GetString(), 
                                                      pStrName ? pStrName->GetStringA() : 
                                                                 CResourceHelper::LoadString (IDS_UNKOWN_TEXT, g_hInstance).c_str());
			}
        }

        // Don't save the remediation action for the manually added quarantine
        // item or items that we don't care about (i.e: there was nothing to
        // do for the remediation action)
        if ( ezRem.ShouldSaveRemediation() )
        {
            vRemData.push_back(remData);
        }

        // Mark if a reboot is required for this anomaly to be fully removed
        if ( remData.eRemediationResult == ccEraser::RebootRequired )
        {
            if ( !pUserData->SetValue(AnomalyRequiresReboot, true) )
            {
                CCTRACEE(_T("CScanManager::HandleAnomaly() - Failed to set the anomaly reboot required flag in the user data."));
            }
        }
    }

    //
    // Undo operations on failure
    //
    if ( !bSucceeded )
    {
        CCTRACEW(_T("CScanManager::HandleAnomaly() - Anomaly processing failed."));

        // Remove the quarantine backup item if we created one
        if ( bCreatedNewQuarItem && pQuarItem != NULL)
        {
            if ( !m_Quarantine.Delete(pQuarItemScanInfection) )
            {
                CCTRACEE(_T("CScanManager::HandleAnomaly() - Failed to delete the quarantine item we created."));
            }

            unsigned long ulRef = pQuarItem.Release();
            CCTRACEI(_T("CScanManager::HandleAnomaly() - Quarantine item ref count after releasing is %d."), ulRef);
        }

        // Undo any successful remediation operations
        vector<CRemediationData>::iterator Iter;

        for ( Iter = vRemData.begin(); Iter != vRemData.end(); Iter++ )
        {
            if ( ccEraser::Succeeded((*Iter).eRemediationResult) && 
                 (*Iter).pRemediation != NULL && (*Iter).pUndo != NULL )
            {
                (*Iter).pUndo->SetPosition(0);
                CCTRACEI(_T("CScanManager::HandleAnomaly() - Attempting to undo remediation."));
                cc::IStringPtr pStrDesc;
                if ( ccEraser::Succeeded( (*Iter).pRemediation->GetDescription(pStrDesc) ) )
                    CCTRACEI(_T("CScanManager::HandleAnomaly() - Remediation description = %s."), pStrDesc->GetStringA());

                eRes = (*Iter).pRemediation->Undo((*Iter).pUndo);

                if ( ccEraser::Succeeded(eRes) )
                {
                    CCTRACEI(_T("CScanManager::HandleAnomaly() - Successful performed undo operation. Deleting the remediation result."));

                    // Blow away the remediation result since it no longer 
                    // applies
                    CEZRemediation ezRemReverted((*Iter).pRemediation);
                    cc::IKeyValueCollectionPtr spRemRevertedProps = 
                                                ezRemReverted.GetProperties();
                    if ( spRemRevertedProps )
                    {
                        spRemRevertedProps->Remove(ccEraser::IRemediationAction::RemediationResult);
                    }

                    // Now revert the status to unhandled for infection items 
                    // since we restored it
                    if ( ezRemReverted.IsScanInfectionType() )
                    {
                        IScanInfectionPtr spInfectionReverted = 
                                              ezRemReverted.GetScanInfection();

                        if ( spInfectionReverted )
                        {
                            // If this infection had a successful remediation 
                            // revert it
                            long lUserData = spInfectionReverted->GetUserData();
                            if ( !InfectionStatusFailure(static_cast<CommonUIInfectionStatus>(lUserData)) )
                            {
                                CCTRACEI(_T("CScanManager::HandleAnomaly() - Reverting status to unhandled for undone scan infection remediation action."));
                                spInfectionReverted->SetUserData(InfectionStatus_Unhandled);
                            }
                        }
                    }
                }
                else
                {
                    CCTRACEE(_T("CScanManager::HandleAnomaly() - Undo operation failed. eResult = %d"), eRes);
                }
            }
        }
    }
    else // SUCCEEDED!
    {
        // Do we need to create a quarantine item with no file infections?
        if ( pQuarItem == NULL && context.m_bQuarantineBackup == true && 
             vRemData.size() > 0 && bHaveAnUndoForAtLeastOneRemediation )
        {
            CCTRACEI(_T("CScanManager::HandleAnomaly() - Using the dummy file quarantine item implementation"));

            if ( !m_Quarantine.AddWithoutFile(context.m_ulQuarItemStatus, 
                                              &pQuarItem) || 
                 pQuarItem == NULL )
                CCTRACEE(_T("CScanManager::HandleAnomaly() - Failed to add the backup item to quarantine without a file."));
        }

        // Now add the extra quarantine item data if we have a quarantine item
        if ( pQuarItem != NULL )
        {
            // Set the file APIs to the same mode as the quarantine item scan 
            // infection for the quarantine operations
            CFileAPI fileAPI;
            
            IScanCompressedFileInfection2QIPtr pCompressed = 
                                                       pQuarItemScanInfection;
            IScanFileInfection2QIPtr pFile = pQuarItemScanInfection;

            if ( pFile.m_p != NULL )
            {
                if ( pFile->AreOEMFileApis() )
                {
                    fileAPI.SwitchAPIToOEM();
                }
                else
                {
                    fileAPI.SwitchAPIToANSI();
                }
            }
            else if ( pCompressed.m_p != NULL )
            {
                if ( pCompressed->AreOEMFileApis() )
                {
                    fileAPI.SwitchAPIToOEM();
                }
                else
                {
                    fileAPI.SwitchAPIToANSI();
                }
            }

            // Don't ever take snapshots for Email, IM, or Office scans
            if ( !m_Settings[SMSETTING_OEH_SILENT] && 
                 !m_Settings[SMSETTING_OEH] &&
                 !m_Settings[SMSETTING_IM] && 
                 !m_Settings[SMSETTING_EMAIL] && 
                 !m_Settings[SMSETTING_OFFICE] &&
                 bTakeSnapShot )
            {
                // Create the system snapshot
                CCTRACEI(_T("CScanManager::HandleAnomaly() - Taking the quarantine item system snapshot"));
                if ( !m_Quarantine.AddSnapShotToQuarantineItem(pQuarItem) )
                {
                    CCTRACEE(_T("CScanManager::HandleAnomaly() - Failed to take the quarantine item system snapshot"));
                }
                else
                {
                    CCTRACEI(_T("CScanManager::HandleAnomaly() - Finished taking the quarantine item system snapshot"));
                }
            }

            // Add the anomaly name
            if ( pStrName )
            {
                if ( FAILED(pQuarItem->SetAnomalyName(pStrName->GetStringA())) )
                {
                    CCTRACEE(_T("CScanManager::HandleAnomaly() - Failed to set the quarantine item anomaly name"));
                }
            }

            // Add the anomaly ID
            if ( pStrID )
            {
                if ( FAILED(pQuarItem->SetAnomalyID(pStrID->GetStringA())) )
                {
                    CCTRACEE(_T("CScanManager::HandleAnomaly() - Failed to set the quarantine item anomaly ID"));
                }
            }

            // Add the anomaly Categories
            if ( FAILED(pQuarItem->SetAnomalyCategories(pEZAnomaly->GetCategories())) )
            {
                CCTRACEE(_T("CScanManager::HandleAnomaly() - Failed to set the quarantine item anomaly categories"));
            }

            // Set the damage flag values for non-viral items
            if ( !bViral )
            {
                if ( FAILED(pQuarItem->SetAnomalyDamageFlag(ccEraser::IAnomaly::Stealth,
                                                            pEZAnomaly->GetDamageFlagValue(ccEraser::IAnomaly::Stealth))) )
                {
                    CCTRACEE(_T("CScanManager::HandleAnomaly() - Failed to set the quarantine item Stealth damage flag."));
                }
                if ( FAILED(pQuarItem->SetAnomalyDamageFlag(ccEraser::IAnomaly::Removal, 
                                                            pEZAnomaly->GetDamageFlagValue(ccEraser::IAnomaly::Removal))) )
                {
                    CCTRACEE(_T("CScanManager::HandleAnomaly() - Failed to set the quarantine item Removal damage flag."));
                }
                if ( FAILED(pQuarItem->SetAnomalyDamageFlag(ccEraser::IAnomaly::Performance, 
                                                            pEZAnomaly->GetDamageFlagValue(ccEraser::IAnomaly::Performance))) )
                {
                    CCTRACEE(_T("CScanManager::HandleAnomaly() - Failed to set the quarantine item Performance damage flag."));
                }
                if ( FAILED(pQuarItem->SetAnomalyDamageFlag(ccEraser::IAnomaly::Privacy, 
                                                            pEZAnomaly->GetDamageFlagValue(ccEraser::IAnomaly::Privacy))) )
                {
                    CCTRACEE(_T("CScanManager::HandleAnomaly() - Failed to set the quarantine item Privacy damage flag."));
                }

                // Also set dependency flag
                bool bHasKnownDependencies = 
                                        pEZAnomaly->KnownToHaveDependencies();

                if ( FAILED(pQuarItem->SetAnomalyDependencyFlag(bHasKnownDependencies ? 1 : 0)) )
                {
                    CCTRACEE(_T("CScanManager::HandleAnomaly() - Failed to set the quarantine item dependency flag."));
                }
            }

            // Add the remediation\undo information
            vector<CRemediationData>::iterator Iter;

            for ( Iter = vRemData.begin(); Iter != vRemData.end(); Iter++ )
            {
                // Don't add the manually quarantined remediation actions and 
                // make sure the remediation action pointer is valid
                if ( (*Iter).pRemediation != NULL && 
                     (*Iter).bManuallyQuarantined == false )
                {
                    if ( FAILED(pQuarItem->AddRemediationData((*Iter).pRemediation, 
                                                              (*Iter).pUndo)) )
                        CCTRACEE(_T("CScanManager::HandleAnomaly() - Failed to set the quarantine item remediation undo data"));
                }
            }

            // Save the data to the item
            if ( FAILED(pQuarItem->SaveItem()) )
            {
                CCTRACEE(_T("CScanManager::HandleAnomaly() - Failed to save the quarantine item."));
            }

            // Set the quarantine item UUID in the user data
            UUID uuid = {0};

            if ( SUCCEEDED(pQuarItem->GetUniqueID(uuid)) )
            {  
                pEZAnomaly->SetQuarantineItemUUID(uuid);
            }
            else
            {
                CCTRACEE(_T("CScanManager::HandleAnomaly() - Failed to get the quarantine item UUID."));
            }
        }

        // Save all remediation actions that need to be processed after reboot 
        // if we have reboot files
        if ( !m_vRebootFiles.empty() )
        {
            SaveRebootRemediationsToDisk(pEZAnomaly);
        }
    }

    //
    // Update status values
    //
    if ( bViral )
    {
        AnomalyAction eNextAction;
        CommonUIInfectionStatus eStatus;

        switch (dwAction)
        {
        case ACTION_REPAIR:
            if ( bSucceeded )
            {
                // Increment appropriate resolved count and set to repaired
                CScanManagerCounts::eSMCountType countType = 
                                                 CScanManagerCounts::Repaired;

                if ( pEZAnomaly->IsBootRecordAnomaly() )
                {
                    countType = CScanManagerCounts::BRRepaired;
                }
                else if ( pEZAnomaly->IsMBRAnomaly() )
                {
                    countType = CScanManagerCounts::MBRRepaired;
                }
                else if ( bRepairDelete )
                {
                    countType = CScanManagerCounts::ViralDeleted;
                }

                eNextAction = ACTION_COMPLETE;

                m_ScanMgrCounts.IncrementItemCount(countType);

                // If this was a repair-delete infection mark it as deleted.
                if ( bRepairDelete )
                {
                    eStatus = InfectionStatus_Deleted;
                }
                else
                {
                    eStatus = InfectionStatus_Repaired;
                }
            }
            else
            {
                // Set status to repair failed
                eStatus = InfectionStatus_Repair_Failed;
                eNextAction = ACTION_QUARANTINE;

                // Boot record items should be set to review
                // since quarantine and delete are not allowed
                if ( pEZAnomaly->IsBootRecordAnomaly() || 
                     pEZAnomaly->IsMBRAnomaly())
                {
                    eNextAction = ACTION_REVIEW;
                }
            }

            break;

        case ACTION_QUARANTINE:
            if ( bSucceeded )
            {
                // If we don't actually have a quarantine item then this item
                // may not have existed so treat it as deleted instead
                if ( pQuarItem == NULL )
                {
                    // Increment viral delete count
                    m_ScanMgrCounts.IncrementItemCount(CScanManagerCounts::ViralDeleted);

                    // Set status to delete
                    eStatus = InfectionStatus_Deleted;
                }
                else
                {
                    // Increment quarantine count
                    m_ScanMgrCounts.IncrementItemCount(CScanManagerCounts::Quarantined);

                    // Set status to quarantined
                    eStatus = InfectionStatus_Quarantined;
                }

                eNextAction = ACTION_COMPLETE;
            }
            else
            {
                // Set status to quarantine failed
                eNextAction = ACTION_DELETE;
                eStatus = InfectionStatus_Quarantine_Failed;
            }

            break;

        case ACTION_DELETE:
            if ( bSucceeded )
            {
                // Increment viral delete count
                m_ScanMgrCounts.IncrementItemCount(CScanManagerCounts::ViralDeleted);

                // Set status to deleted
                eNextAction = ACTION_COMPLETE;
                eStatus = InfectionStatus_Deleted;
            }
            else
            {
                // Set status to delete failed
                eNextAction = ACTION_REVIEW;
                eStatus = InfectionStatus_Delete_Failed;
            }

            break;

        }

        // Set the status
        pEZAnomaly->SetAnomalyStatus(eNextAction, eStatus);
    }
    else
    {
        AnomalyAction eNextAction;
        CommonUIInfectionStatus eStatus;

        // Non-viral, only delete operation will be available here
        switch (dwAction)
        {
        case ACTION_DELETE:
            if ( bSucceeded )
            {
                // Increment non-viral delete count
                m_ScanMgrCounts.IncrementItemCount(CScanManagerCounts::NonViralDeleted);

                // Set status to deleted
                eNextAction = ACTION_COMPLETE;
                eStatus = InfectionStatus_Deleted;
            }
            else
            {
                // Set next action to review
                eNextAction = ACTION_REVIEW;

                if (bNonViralDeleteFailedOnQuarantine) 
                {
                    // UI needs to treat special case a on non-viral
                    // deletion failing on quarantine differently from 
                    // the deletion failing during the actual remediation 
                    // attempt.  So even though the attempt was a delete,
                    // set the status to quarantine failed, rather than
                    // delete failed.
                    eStatus = InfectionStatus_Quarantine_Failed;
                } 
                else 
                {
                    // Set status to normal case of delete failed
                    eStatus = InfectionStatus_Delete_Failed;
                }
            }
            break;
        }

        // Set the status
        pEZAnomaly->SetAnomalyStatus(eNextAction, eStatus);
    }

    m_DisplayNames.UpdateAllRemediationStatusText(pEZAnomaly);

    return bSucceeded;
}


bool CScanManager::PreProcessAnomaly(CAnomalyContext& context)
{
    if ( context.m_pEZAnomaly == NULL )
    {
        CCTRACEE(_T("CScanManager::PreProcessAnomaly() - Anomaly context object is null."));
        return false;
    }

    // Default to viral, unless proven otherwise
    bool bViral = true;

    if ( context.m_pEZAnomaly->IsNonViral() )
    {
        bViral = false;
    }

    // Should we use the automatic options?
    bool bAutomatic = m_bAutomaticMode;

    // If this is during the CommonUI processing we need to get the action to 
    // take from the anomaly user data setting
    if ( !bAutomatic )
    {
        // Get the action set in the anomaly
        AnomalyAction eAction;
        CommonUIInfectionStatus eStatus;
        
        if ( context.m_pEZAnomaly->GetAnomalyStatus(eAction, eStatus) )
        {
            CCTRACEI(_T("CScanManager::PreProcessAnomaly() - Using the action set in the anomaly (CommonUI manual user handling). Action = %d"), eAction);

            //
            // Filter out all of the actions which don't require any processing
            //

            // Is this a review item
            if ( eAction == ACTION_REVIEW )
            {
                CCTRACEI(_T("CScanManager::PreProcessAnomaly() - This is a review item...doing nothing"));
                return false;
            }
            // Are we being told to ignore this anomaly?
            else if ( eAction == ACTION_IGNORE )
            {
                if ( !bViral )
                {
                    if ( context.m_pEZAnomaly->GetDamageFlagAverage() < 2 )
                    {
                        // This is a low ranked non-viral threat and it's being
                        // ignored, set it to resolved
                        CCTRACEI(_T("CScanManager::PreProcessAnomaly() - Low ranked non-viral threat with action to ignore, setting to resolved."));
                        context.m_pEZAnomaly->SetAnomalyStatus(
                                                    ACTION_COMPLETE,
                                                    InfectionStatus_Unhandled);
                        m_ScanMgrCounts.
                              IncrementItemCount(
                                     CScanManagerCounts::NonViralExcludedOnce);

                        return false;
                    }
                    else
                    {
                        // This is a high/med. ranked non-viral threat and it's
                        // being ignored, if the state is still unhandled then
                        // we set this to resolved
                        if ( eStatus == InfectionStatus_Unhandled )
                        {
                            CCTRACEI(_T("CScanManager::PreProcessAnomaly() - Medium or High ranked non-viral threat with action manually changed to ignore, setting to resolved."));
                            context.m_pEZAnomaly->SetAnomalyStatus(
                                                    ACTION_COMPLETE, 
                                                    InfectionStatus_Unhandled);
                            m_ScanMgrCounts.
                                 IncrementItemCount(
                                     CScanManagerCounts::NonViralExcludedOnce);
                            return false;
                        }
                    }
                }

                CCTRACEI(_T("CScanManager::PreProcessAnomaly() - Anomaly action is set to ignore for this anomaly. Not processing."));
                return false;
            }
            // Have we already completed all possible operations for this 
            // anomaly?
            else if ( eAction == ACTION_COMPLETE )
            {
                CCTRACEI(_T("CScanManager::PreProcessAnomaly() - Anomaly action is already set to complete for this anomaly."));
                return false;
            }
            // Are there no operations left for this anomaly?
            else if ( eAction == ACTION_NOOPERATIONSAVAILABLE )
            {
                CCTRACEI(_T("CScanManager::PreProcessAnomaly() - Anomaly action is set to none available for this anomaly."));
                return false;
            }
            else
            {
                // Just add the action to take
                CCTRACEI(_T("CScanManager::PreProcessAnomaly() - Using manual action of %d this anomaly."), eAction);
                context.m_vdwActions.push_back(static_cast<DWORD>(eAction));
            }
        }
        else
        {
            CCTRACEE(_T("CScanManager::PreProcessAnomaly() - Failed to get the action or status from the anomaly. Using automatic options."));
            bAutomatic = true;
        }
    }
    else
    {
        // Don't process "can't delete" items automatically
        if ( context.m_pEZAnomaly->CantDelete() )
        {
            CCTRACEI(_T("CScanManager::PreProcessAnomaly() - This is a can't delete flagged anomaly. Not processing automatically."));
            return false;
        }

        CCTRACEI(_T("CScanManager::PreProcessAnomaly() - Using automatic action(s) for this anomaly (during scan progress)."));
    }

    if ( bViral )
    {
        // Must check to see if ccEraser set the RepairOnly recommended
        // flag for any of this anomaly's remediations.
        //
        // Get the remediation list
        ccEraser::IRemediationActionListPtr pRemediateList = 
                                       context.m_pEZAnomaly->GetRemediations();

        if ( pRemediateList == NULL )
        {
            CCTRACEE(_T("CScanManager::PreProcessAnomaly() - Failed to get remediation action list."));
            return false;
        }

        size_t nCount = context.m_pEZAnomaly->GetRemediationCount();
  
        // Now we loop through each remediation action
        for (size_t nCurRem = 0; nCurRem < nCount; nCurRem++)
        {
            ccEraser::IRemediationActionPtr pAction = 
                                 context.m_pEZAnomaly->GetRemediation(nCurRem);

            if ( pAction == NULL )
            {
                CCTRACEE(_T("CScanManager::PreProcessAnomaly() - Failed to get remediation action item %d."), nCurRem);
                continue;
            }

            // Wrap with remediation helper class
            CEZRemediation ezRem(pAction);
            DWORD dwRecommendedOperation = -1;
            if (ezRem.GetRecommendedOperation(dwRecommendedOperation))
            {
                if(ccEraser::IRemediationAction::RepairOnly == dwRecommendedOperation) 
                {
                    context.m_pEZAnomaly->SetRepairOnly();
                }
                
                break;
            }
        }

        // For automatic items set the action according to the user's settings
        if ( bAutomatic )
        {
            // Set the action to take based on the viral response mode
            if ( m_Response == SCAN_MODE_AUTO_REPAIR )
            {
                context.m_vdwActions.push_back(ACTION_REPAIR);
            }
            else if ( m_Response == SCAN_MODE_AUTO_REPAIR_QUARANTINE )
            {
                context.m_vdwActions.push_back(ACTION_REPAIR);

                // Boot record and MBRinfections can only be repaired
                if ( !context.m_pEZAnomaly->IsBootRecordAnomaly() && 
                     !context.m_pEZAnomaly->IsMBRAnomaly() &&
                     !context.m_pEZAnomaly->IsRepairOnlyAnomaly() )
                {
                    context.m_vdwActions.push_back(ACTION_QUARANTINE);
                }
            }
            else if ( m_Response == SCAN_MODE_AUTO_REPAIR_DELETE )
            {
                context.m_vdwActions.push_back(ACTION_REPAIR);

                if ( !context.m_pEZAnomaly->IsRepairOnlyAnomaly() ) 
                {
                    context.m_vdwActions.push_back(ACTION_DELETE);
                }
            }
            else
            {
                // We do not want to perform any automatic actions so just set
                // he user data properties for CommonUI
                context.m_pEZAnomaly->
                                   SetAnomalyStatus(ACTION_REPAIR, 
                                                    InfectionStatus_Unhandled);
                CCTRACEI(_T("CScanManager::PreProcessAnomaly() - The automatic action to take for this viral anomaly is manual. Not handling now."));
                return false;
            }
        }

        // Do we need to make a backup of this viral anomaly?
        if ( context.m_vdwActions[0] == ACTION_REPAIR && 
             m_Settings[SMSETTING_BACKUP_REPAIRS] == true )
        {
            // Only allow backup for file infection types
            if ( context.m_pEZAnomaly->IsBootRecordAnomaly() || 
                 context.m_pEZAnomaly->IsMBRAnomaly() )
            {
                CCTRACEI(_T("CScanManager::PreProcessAnomaly() - This is a boot record anomaly so there will be no attempt to backup in quarantine."));
            }
            else
            {
                context.m_bQuarantineBackup = true;
                context.m_ulQuarItemStatus = QFILE_STATUS_BACKUP_FILE;
            }
        }
        else if ( context.m_vdwActions[0] == ACTION_QUARANTINE )
        {
            context.m_bQuarantineBackup = true;
            context.m_ulQuarItemStatus = QFILE_STATUS_QUARANTINED;
        }
    }
    else // Non-viral
    {
        // For automatic handling set the action according to the non-viral 
        // threat handling user settings
        if ( bAutomatic )
        {
            // Set the action to take based on the non-viral response mode
            if ( m_NonViralResponse == THREAT_SCAN_MODE_AUTO_DELETE )
            {
                CCTRACEI(_T("CScanManager::PreProcessAnomaly() - The automatic action to take for this non-viral anomaly is to delete."));
                context.m_vdwActions.push_back(ACTION_DELETE);
            }
            else if ( m_NonViralResponse == THREAT_SCAN_MODE_USE_DAMAGE_FLAG )
            {
                // Do the damage flags indicate auto-delete?
                if ( ShouldAutoDeleteBasedOnDamageFlags(context.m_pEZAnomaly) )
                {
                    CCTRACEI(_T("CScanManager::PreProcessAnomaly() - Damage flag logic is to automatically remove this threat."));
                    context.m_vdwActions.push_back(ACTION_DELETE);
                }
                // We will not take any automatic action on this item
                else
                {
                    // Bump up the number of damage control items that we have 
                    // skipped processing on
                    m_lDamageControlItems++;

                    // Get the recommended action
                    AnomalyAction action = ACTION_DELETE;
                    context.m_pEZAnomaly->GetRecommendedAction(action);
                    context.m_pEZAnomaly->
                          SetAnomalyStatus(action, InfectionStatus_Unhandled);

                    CCTRACEI(_T("CScanManager::PreProcessAnomaly() - Damage flag logic is to not automatically remove this threat. Setting recommended action to %d. Incremented damage control items."), action);

                    return false;
                }
            }
            else
            {
                // We do not want to perform any automatic actions so just set 
                // the user data properties for CommonUI

                // Get the recommended action
                AnomalyAction action = ACTION_DELETE;
                context.m_pEZAnomaly->GetRecommendedAction(action);
                context.m_pEZAnomaly->
                           SetAnomalyStatus(action, InfectionStatus_Unhandled);

                CCTRACEI(_T("CScanManager::PreProcessAnomaly() - The automatic action to take for this non-viral anomaly is manual. Not processing now. Set recommended to %d."), action);
                
                return false;
            }
        }

        if ( context.m_vdwActions[0] == ACTION_DELETE )
        {
            // Do we need to make a backup of this non-viral anomaly?
            if ( m_Settings[SMSETTING_BACKUP_DELETED_THREATS] )
            {
                AnomalyAction action;
                CommonUIInfectionStatus status = InfectionStatus_Unhandled;

                context.m_pEZAnomaly->GetAnomalyStatus(action, status);

                // 
                // Don't try to quarantine again if already failed before
                //
                if (InfectionStatus_Quarantine_Failed != status)
                {
                    context.m_bQuarantineBackup = true;
                    context.m_ulQuarItemStatus = QFILE_STATUS_THREAT_BACKUP;
                }
            }
        }
        else if ( context.m_vdwActions[0] == ACTION_EXCLUDE )
        {
            // Exclude now
            if( m_Exclusions.AddAnomaly(*(context.m_pEZAnomaly)) )
            {
                CCTRACEI(_T("CScanManager::PreProcessAnomaly() - Anomaly exclude operation succeeded."));

                // Set the anomaly status to excluded
                context.m_pEZAnomaly->
                                    SetAnomalyStatus(ACTION_COMPLETE, 
                                                     InfectionStatus_Excluded);
                m_ScanMgrCounts.IncrementItemCount(
                                         CScanManagerCounts::NonViralExcluded);

                // Update the status of the remediations to reflect the 
                // excluded state
                m_DisplayNames.UpdateAllRemediationStatusText(
                                                         context.m_pEZAnomaly);

                return false;
            }
            else
            {
                CCTRACEE(_T("CScanManager::PreProcessAnomaly() - Anomaly exclude operation failed."));

                // Set the anomaly status to exclude failed
                context.m_pEZAnomaly->
                              SetAnomalyStatus(ACTION_NOOPERATIONSAVAILABLE,
                                               InfectionStatus_Exclude_Failed);

                // Update the status of the remediations to reflect the exclude
                // failed state
                m_DisplayNames.
                         UpdateAllRemediationStatusText(context.m_pEZAnomaly);

                return false;
            }
        }
    }

    return true;
}

bool CScanManager::IsScanInfectionFileGone(IScanInfection* pInfection)
{
    if( NULL == pInfection )
    {
        CCTRACEE(_T("CScanManager::IsScanInfectionFileGone() - Scan infection object is null."));
        return false;
    }

    // Check to see if the file is actually gone already...
    ATL::CAtlString strFileName;
    IScanFileInfection2QIPtr pScanFile;
    IScanCompressedFileInfection2QIPtr pCompressed;
    CFileAPI fileAPI;
    if( pScanFile = pInfection )
    {
        // Make sure the current file APIs match the file mode for the path we are going to check
        strFileName = pScanFile->GetLongFileName();
        if( pScanFile->AreOEMFileApis() && fileAPI.IsOriginalANSI() )
        {
            CCTRACEI(_T("CScanManager::IsScanInfectionFileGone() - Switching file APIs to OEM in order to properly check for the path %s which was detected in OEM mode."), (LPCTSTR)strFileName);
            fileAPI.SwitchAPIToOEM();
        }
        else if( !pScanFile->AreOEMFileApis() && !fileAPI.IsOriginalANSI() )
        {
            CCTRACEI(_T("CScanManager::IsScanInfectionFileGone() - Switching file APIs to ANSI in order to properly check for the path %s which was detected in ANSI mode."), (LPCTSTR)strFileName);
            fileAPI.SwitchAPIToANSI();
        }
    }
    else if( pCompressed = pInfection )
    {
        int nComponentCount = pCompressed->GetComponentCount();
        strFileName = pCompressed->GetComponent(nComponentCount-1);

        // Make sure the current file APIs match the file mode for the path we are going to check
        if( pCompressed->AreOEMFileApis() && fileAPI.IsOriginalANSI() )
        {
            CCTRACEI(_T("CScanManager::IsScanInfectionFileGone() - Switching file APIs to OEM in order to properly check for the path %s which was detected in OEM mode."), (LPCTSTR)strFileName);
            fileAPI.SwitchAPIToOEM();
        }
        else if( !pCompressed->AreOEMFileApis() && !fileAPI.IsOriginalANSI() )
        {
            CCTRACEI(_T("CScanManager::IsScanInfectionFileGone() - Switching file APIs to ANSI in order to properly check for the path %s which was detected in ANSI mode."), (LPCTSTR)strFileName);
            fileAPI.SwitchAPIToANSI();
        }
    }
    else
    {
        CCTRACEE(_T("CScanManager::IsScanInfectionFileGone() - Scan infection item VID = %lu, Name = %s is not a file or compressed file infection."), pInfection->GetVirusID(), pInfection->GetVirusName());
        return false;
    }

    // Does this file exist?
    DWORD dwAttribs = INVALID_FILE_ATTRIBUTES;

    COSInfo osi;
    if( strFileName.GetLength() > MAX_PATH && osi.IsWinNT() )
    {
        CCTRACEI(_T("CScanManager::IsScanInfectionFileGone() - This is an SLFN path on NT, using GetFileAttributesW()"));
        vector<WCHAR> vwFileName;
        CFileAPI::ConvertLongFileToUnicodeBuffer(strFileName, vwFileName);
        dwAttribs = GetFileAttributesW(&vwFileName[0]);
    }
    else
        dwAttribs = GetFileAttributes(strFileName);

    if ( INVALID_FILE_ATTRIBUTES == dwAttribs &&
         ERROR_FILE_NOT_FOUND == GetLastError() )
    {
        CCTRACEW(_T("CScanManager::IsScanInfectionFileGone() - The path %s for this scan infection no longer exists."), strFileName);
        return true;
    }
    else if ( pCompressed && m_Quarantine.AddFailedOnExtraction() )
    {
        CCTRACEW(_T("CScanManager::IsScanInfectionFileGone() - The infected compressed file within the %s scan infection container no longer exists."), strFileName);
        return true;
    }

    return false;
}

bool CScanManager::GetUndoData(ccEraser::IRemediationAction* pAction, cc::IStream*& pStream, ATL::CAtlString& strUndoFileName)
{
    if( pAction == NULL )
    {
        CCTRACEE(_T("CScanManager::GetUndoData() - The remediation action is not valid."));
        return false;
    }

    ccEraser::eResult eRes = ccEraser::Success;
    bool bUndoable = false;

    eRes = pAction->SupportsUndo(bUndoable);
    if( ccEraser::Failed(eRes) )
    {
        CCTRACEE(_T("CScanManager::GetUndoData() - SupportsUndo() failed. eResult = %d"), eRes);
        return false;
    }
    else if( !bUndoable )
    {
        CCTRACEI(_T("CScanManager::GetUndoData() - The remediation action does not support undo."));
        return false;
    }
    else
    {
        ccSym::CFileStreamImplPtr pFileStream;
        pFileStream.Attach(ccSym::CFileStreamImpl::CreateFileStreamImpl());

        if (pFileStream == NULL)
        {
            CCTRACEE(_T("CScanManager::GetUndoData - File stream is NULL."));
            return false;
        }

        TCHAR szTempDir[MAX_PATH] = {0};
        if (GetTempPath(MAX_PATH, szTempDir) == 0)
            CCTRACEE(_T("CScanManager::GetUndoData - Failed to get the temp path."));

        GetTempFileName(szTempDir, _T("und"), 0, strUndoFileName.GetBuffer(MAX_PATH));
        strUndoFileName.ReleaseBuffer();

        if (FALSE == pFileStream->GetFile().Open(strUndoFileName, 
            GENERIC_READ | GENERIC_WRITE,
            0, 
            NULL, 
            CREATE_ALWAYS, 
            0,
            NULL))
        {
            CCTRACEE(_T("CScanManager::GetUndoData() - pFileStream->GetFile().Open() failed for file %s"), (LPCTSTR)strUndoFileName);
            DeleteFile(strUndoFileName);
            strUndoFileName.Empty();
            return false;
        }

        // Get the undo information
        eRes = pAction->GetUndoInformation(pFileStream);
        if( ccEraser::Failed(eRes) )
        {
            CCTRACEE(_T("CScanManager::GetUndoData() - Error getting undo information. eResult = %d"), eRes);
            pFileStream->GetFile().Close();
            
            if( !pFileStream->GetFile().Delete(strUndoFileName, FALSE) )
            {
                CCTRACEE(_T("CScanManager::GetUndoData() - Failed to delete backup undo file stream %s"), (LPCTSTR)strUndoFileName);
            }

            strUndoFileName.Empty();
            return false;
        }

        pStream = pFileStream.Detach();
    }

    return true;
}

void CScanManager::ReleaseUndoData(cc::IStream* pUndoStream, LPCTSTR pcszUndoFileName)
{
    // Release the undo stream if we have one
    if( pUndoStream != NULL )
    {
        pUndoStream->Release();
        pUndoStream = NULL;
    }

    // Delete the undo file if we have one
    if( _tcslen(pcszUndoFileName) != 0 )
    {
        if( DeleteFile(pcszUndoFileName) )
            CCTRACEI(_T("CScanManager::ReleaseUndoData() - Successfully deleted backup undo file %s"),pcszUndoFileName);
        else
            CCTRACEE(_T("CScanManager::ReleaseUndoData() - Failed to delete backup undo file %s"),pcszUndoFileName);
    }
}

bool CScanManager::PerformRemediationAction(CEZRemediation* pAction, 
                                            ccEraser::eResult& eRemResult)
{
    if ( pAction == NULL || (*pAction) == NULL )
    {
        CCTRACEE(_T("CScanManager::PerformRemediationAction() - The remediation action is not valid."));
        return false;
    }

    // Need to be in ANSI mode before calling into ccEraser
    CFileAPI fileAPI;
    fileAPI.SwitchAPIToANSI();

    CCTRACEI(_T("CScanManager::PerformRemediationAction() - Calling Remediate()"));
    // Perform the remediation action
    eRemResult = (*pAction)->Remediate();
    CCTRACEI(_T("CScanManager::PerformRemediationAction() - Finished calling Remediate()"));

    fileAPI.RestoreOriginalAPI();
    // ------------- Original - File Mode ----------------- //

    // Get the type
    ccEraser::eObjectType remType = pAction->GetType();

    // Get the scan infection if valid
    IScanInfectionPtr spInfection;
    long lUserData = InfectionStatus_Unhandled;

    if ( remType == ccEraser::InfectionRemediationActionType )
    {
        if ( spInfection = pAction->GetScanInfection() )
        {
            lUserData = spInfection->GetUserData();
        }
    }

    // Did the item not exist at all?
    if ( eRemResult == ccEraser::FileNotFound || 
         eRemResult == ccEraser::NothingToDo )
    {
        CCTRACEW(_T("CScanManager::PerformRemediationAction() - Remediation action did not exist. Nothing to remediate."));

        if ( spInfection )
        {
            spInfection->SetUserData(InfectionStatus_NotDetected);
        }

        // This is a success
        return true;
    }

    // At this point we want to mark that the remediation is present so it is 
    // displayed to the user regardless of whether it was previously marked 
    // present or not since the remediation attempt returned a valid success 
    // or error code we did try something during the remediation
    pAction->SetRemediationPresent(true);

    if ( ccEraser::Failed(eRemResult) )
    {
        // For infection items we want to see if the infection path or 
        // container was removed already
        if ( spInfection && IsScanInfectionFileGone(spInfection) )
        {
            CCTRACEW(_T("CScanManager::PerformRemediationAction() - The infected file could not be found. Treating as nothing to do."));

            // Update the status to not detected
            spInfection->SetUserData(InfectionStatus_NotDetected);

            // Set the remediation result to nothing to do
            cc::IKeyValueCollectionPtr pProps = pAction->GetProperties();

            if ( pProps )
            { 
                pProps->SetValue(ccEraser::IRemediationAction::RemediationResult, 
                                 static_cast<DWORD>(ccEraser::NothingToDo));
            }

            return true;
        }

        CCTRACEW(_T("CScanManager::PerformRemediationAction() - Remediation action failed. eResult = %d"), eRemResult);

        if ( spInfection )
        {
            // Set the appropriate user data value for this failure so we show 
            // proper status
            if ( eRemResult == ccEraser::AccessDenied )
            {
                spInfection->SetUserData(InfectionStatus_AccessDenied);
            }
            else if ( lUserData == InfectionStatus_Repaired )
            {
                spInfection->SetUserData(InfectionStatus_Repair_Failed);
            }
            else if ( lUserData == InfectionStatus_Quarantined )
            {
                spInfection->SetUserData(InfectionStatus_Quarantine_Failed);
            }
            else
            {
                spInfection->SetUserData(InfectionStatus_Delete_Failed);
            }
        }

        return false;
    }
    else if ( ccEraser::RebootRequired == eRemResult )
    {
        if ( spInfection )
        {
            // Set the user data value to reboot required so we show proper 
            // status
            spInfection->SetUserData(InfectionStatus_RebootRequired);
        }

        // If this is a file type save off the file path so we'll know if a 
        // future remediation action needs to be saved and performed after 
        // reboot
        if( remType == ccEraser::FileRemediationActionType ||
            remType == ccEraser::ServiceRemediationActionType || 
            remType == ccEraser::InfectionRemediationActionType || 
            remType == ccEraser::ProcessRemediationActionType )
        {
            cc::IStringPtr spStrPath;
            
            if ( pAction->GetDisplayString(spStrPath) )
            {
                CAtlString strPath = spStrPath->GetStringA();
                strPath.MakeLower();
                CCTRACEI(_T("CScanManager::PerformRemediationAction() - Adding %s to reboot paths."), (LPCTSTR)strPath);
                m_vRebootFiles.insert(strPath);
            }
        }
    }

    cc::IStringPtr pStrDesc;

    if ( ccEraser::Succeeded((*pAction)->GetDescription(pStrDesc)) )
    {
        CCTRACEI(_T("CScanManager::PerformRemediationAction() - Remediation action succeeded for %s. eResult = %d"), pStrDesc->GetStringA(), eRemResult);
    }
    else
    {
        CCTRACEI(_T("CScanManager::PerformRemediationAction() - Remediation action succeeded, but failed to get action description."));
    }

    return true;
}

void CScanManager::DoFileVidReverseScan()
{
    FILEVIDMAP::iterator Iter;

    for ( Iter = m_mapFileVidsToScan.begin(); 
          Iter != m_mapFileVidsToScan.end(); Iter++ )
    {
        // Do we need to pause or abort the scan?
        if ( SCAN_ABORT == HandlePauseAbort(_T("DoFileVidReverseScan")) )
        {
            return;
        }

        // Get the VID
        unsigned long ulVid = (*Iter).first;

        // Get the paths
        cc::IIndexValueCollectionPtr spPaths = (*Iter).second;

        if ( spPaths == NULL )
        {
            CCTRACEE(_T("CScanManager::DoFileVidReverseScan() - Failed to get the paths for vid = %lu to reverse scan."), ulVid);
            continue;
        }

        CCTRACEI(_T("CScanManager::DoFileVidReverseScan() - Reverse scanning %d paths for VID=%lu"), spPaths->GetCount(), ulVid);

        // If there is a specific anomaly for this VID then just add it to the 
        // regular scan list so the scan infection will be added to that 
        // anomaly
        ANOMALYVIDINDEXMAP::iterator IterSpecifics = 
                                         m_mapSpecificAnomalyIndex.find(ulVid);

        if ( IterSpecifics != m_mapSpecificAnomalyIndex.end() )
        {
            CCTRACEI(_T("CScanManager::DoFileVidReverseScan() - There is already a specific anomaly for Vid=%lu. Adding the paths for this VID to regular scan list."), ulVid);

            for ( size_t nIndex = 0; nIndex < spPaths->GetCount(); nIndex++ )
            {
                cc::IStringPtr pStrPath;

                if ( spPaths->GetValue(nIndex, 
                                       reinterpret_cast<ISymBase*&>(pStrPath)) )
                {
                    AddItemToScanList(pStrPath->GetStringA());
                }
            }

            continue;
        }

        // Check if any of these paths are already in a an anomaly we have 
        // detected
        for ( size_t nCurrentIndex = spPaths->GetCount()-1;; nCurrentIndex-- )
        {
            cc::IStringPtr pStrPath;

            if ( spPaths->GetValue(nCurrentIndex, 
                                   reinterpret_cast<ISymBase*&>(pStrPath)) )
            {
                if ( m_InfectionMerge.CheckFileVidItem(pStrPath->GetStringA(), 
                     ulVid) )
                {
                    CCTRACEW(_T("CScanManager::DoFileVidReverseScan() - The scan infection item for %s is already in an anomaly we have. We will not reverse scan this item."), pStrPath->GetStringA());
                    if ( !spPaths->Remove(nCurrentIndex) )
                    {
                        CCTRACEE(_T("CScanManager::DoFileVidReverseScan() - Failed to remove the path from the collection."));
                    }
                }
            }

            if ( nCurrentIndex == 0 )
            {
                break;
            }
        }

        // Check if all paths were already handled
        if ( spPaths->GetCount() == 0 )
        {
            CCTRACEI(_T("CScanManager::DoFileVidReverseScan() - All paths for Vid=%lu were already detected by anomalies we have, no reverse scan needed."), ulVid);
            continue;
        }

        // Get the settings
        ccEraser::eResult eRes = ccEraser::Success;
        cc::IKeyValueCollectionPtr pSettings;
        bool bOverrideDetectAnomaliesGLPScanSetting = false, bSetBackEraserSettings = false;
        eRes = m_pEraser->GetSettings(pSettings);
        if( ccEraser::Succeeded(eRes) )
        {
            if(pSettings->GetValue(ccEraser::IEraser::OverrideDetectAnomaliesGLPScan, bOverrideDetectAnomaliesGLPScanSetting))
            {
                bSetBackEraserSettings = true;
            }
            else
            {
                CCTRACEE(_T("CScanManager::DoFileVidReverseScan() - Failed to get ccEraser::IEraser::OverrideDetectAnomaliesGLPScan"));
            }
            
            // Set OverrideDetectAnomaliesGLPScan to false.
            if( !pSettings->SetValue(ccEraser::IEraser::OverrideDetectAnomaliesGLPScan, false) )
                CCTRACEE(_T("CScanManager::DoFileVidReverseScan() - Failed to set ccEraser::IEraser::OverrideDetectAnomaliesGLPScan"));
        }
        
        // Set up batch updating in the UI for the file paths
        m_ScanMgrCounts.UseBatchFolderUpdating(spPaths);

        // ------------- ANSI - File Mode for eraser scanning--------------- //
        CFileAPI fileAPI;
        fileAPI.SwitchAPIToANSI();

        // Detect any anomalies for this file\vid
        ccEraser::IAnomalyListPtr pAnomList;

        m_bEraserScanning = true;
        CCTRACEI(_T("CScanManager::DoFileVidReverseScan() - Calling ccEraser::DetectAnomalies()"));
        eRes = m_pEraser->DetectAnomalies(spPaths, ulVid, pAnomList);
        CCTRACEI(_T("CScanManager::DoFileVidReverseScan() - Finshed calling ccEraser::DetectAnomalies(). eRes == 0x%08X"), eRes);
        m_bEraserScanning = false;
        
        fileAPI.RestoreOriginalAPI();
        // ------------- Original - File Mode ----------------- //
        
        // Batch updating is done
        m_ScanMgrCounts.EndBatchUpdating();

        if(bSetBackEraserSettings)
        {
            // Set OverrideDetectAnomaliesGLPScan back
            if( !pSettings->SetValue(ccEraser::IEraser::OverrideDetectAnomaliesGLPScan, bOverrideDetectAnomaliesGLPScanSetting) )
                CCTRACEE(_T("CScanManager::DoFileVidReverseScan() - Failed to set ccEraser::IEraser::OverrideDetectAnomaliesGLPScan"));
        }

        size_t nCount = 0;
        if ( ccEraser::Succeeded(eRes) && NULL != pAnomList.m_p)
        {
            pAnomList->GetCount(nCount);
        }
        else
        {
            CCTRACEE(_T("CScanManager::DoFileVidReverseScan() - DetectAnomalies() failed or returned a NULL list. eResult = %d."), eRes);
        }

        if ( nCount == 0 )
        {
            CCTRACEI(_T("CScanManager::DoFileVidReverseScan() - Did not get a hit for VID %lu with %d files. Adding to regular scan list."), ulVid, spPaths->GetCount());
            for( size_t nIndex = 0; nIndex < spPaths->GetCount(); nIndex++ )
            {
                cc::IStringPtr pStrPath;

                if ( spPaths->GetValue(nCurrentIndex, 
                                       reinterpret_cast<ISymBase*&>(pStrPath)) )
                {
                    AddItemToScanList(pStrPath->GetStringA());
                }
            }
        }
        else if( nCount > 0 )
        {
            CCTRACEI(_T("CScanManager::DoFileVidReverseScan() - Detected %d anomalies reverse scanning for %d files with VID=%lu"), nCount, spPaths->GetCount(), ulVid);

            // If there are less files scanned than detections add a file 
            // scanned. This can happen if we get a specific anomaly back for 
            // an item where the file was already removed so it will not 
            // trigger a file count update. Since it looks funky to have less 
            // items scanned than detected we will just bump the files scanned
            // count
            if ( m_ScanMgrCounts.GetItemCount(CScanManagerCounts::FilesScanned)                                 < m_ScanMgrCounts.GetTotalItemsDetected() + 1 )
            {
                m_ScanMgrCounts.IncrementItemCount(CScanManagerCounts::FilesScanned);
            }

            // Filter through each anomaly
            for ( size_t nCur = 0; nCur < nCount; nCur++ )
            {
                ccEraser::eResult eRes = ccEraser::Success;
                ccEraser::IAnomalyPtr pAnomaly;
                eRes = pAnomList->GetItem(nCur, pAnomaly);

                if ( ccEraser::Succeeded(eRes) )
                {
                    CEZAnomaly EZAnomaly;

                    if ( !EZAnomaly.Initialize(pAnomaly) )
                    {
                        CCTRACEE(_T("CScanManager::DoFileVidReverseScan() - Failed to initialize the anomaly data for item %d."), nCur);
                        continue;
                    }

                    // Log the anomaly details if logging is enabled
                    if ( g_DebugOutput.IsOutputEnabled(ccLib::CDebugOutput::eInformationDebug) )
                    {
                        cc::IStringPtr spStrName;
                        cc::IStringPtr spStrID;
                        ATL::CAtlString strName;
                        ATL::CAtlString strID;

                        if ( EZAnomaly.GetName(spStrName) && spStrName )
                        {
                            strName = spStrName->GetStringA();
                        }

                        if ( EZAnomaly.GetID(spStrID) && spStrID )
                        {
                            strID = spStrID->GetStringA();
                        }

                        CCTRACEI("CScanManager::DoFileVidReverseScan() - File-VID detected anomaly. Name = %s ID = %s.", strName.GetString(), strID.GetString());
                    }

                    // Make sure this is not an excluded anomaly
                    if ( m_Exclusions.IsMatch(EZAnomaly) )
                    {
                        CCTRACEI(_T("CScanManager::DoFileVidReverseScan() - The anomaly matched an exclusion, not processing items."));
                        continue;
                    }

                    CScanManagerCounts::eSMCountType eCountType = 
                                                     CScanManagerCounts::Viral;

                    if ( EZAnomaly.IsViral() )
                    {
                        eCountType = CScanManagerCounts::Viral;
                    }
                    else if ( EZAnomaly.IsNonViral() )
                    {
                        eCountType = CScanManagerCounts::NonViral;
                    }
                    else
                    {
                        CCTRACEE(_T("CScanManager::DoFileVidReverseScan() - Anomaly %d from the file\\VID list is not classified as viral or non-viral, will not process."), nCur);
                        continue;
                    }

                    // Check for the presence of the anomalies remediation 
                    // actions
                    if ( !CheckPresenceOfRemediationActions(&EZAnomaly) )
                    {
                        CCTRACEW(_T("CScanManager::DoFileVidReverseScan() - None of the remediation actions for this anomaly were present. Skipping."));
                        continue;
                    }

                    // Add this to our list and increment the viral count
                    eRes = m_pAnomalyList->Add(EZAnomaly);
                    if ( ccEraser::Failed(eRes) )
                    {
                        CCTRACEE(_T("CScanManager::DoFileVidReverseScan() - Failed to add anomaly to the list. eResult = %d"), eRes);
                    }
                    else
                    {
                        m_ScanMgrCounts.IncrementItemCount(eCountType);
                    }

                    // Check for existing quarantine items referenced by any 
                    // of the reference file paths in this anomaly
                    UUID uuidQuarItem = {0};

                    if ( FindCorrespondingQuarantineItem(&EZAnomaly, ulVid, 
                                                         uuidQuarItem) )
                    {
                        // Set the UUID in the anomaly so when it is handled 
                        // we will use the existing quarantine item
                        EZAnomaly.SetQuarantineItemUUID(uuidQuarItem);
                    }

                    // If this is specific insert the VID into our map
                    if ( EZAnomaly.IsSpecific() )
                    {
                        size_t nIndex = 0;

                        m_pAnomalyList->GetCount(nIndex);
                        nIndex--;
                        DWORD dwVID = EZAnomaly.GetVID();

                        if ( dwVID != 0 )
                        {
                            m_mapSpecificAnomalyIndex[dwVID] = nIndex;
                            CCTRACEI(_T("CScanManager::DoFileVidReverseScan() - Added VID = %lu Index = %d to specific anomaly map."), dwVID, nIndex);
                        }
                        else
                        {
                            CCTRACEE(_T("CScanManager::DoFileVidReverseScan() - This is a specific anomaly, but the VID is 0. Not adding to the specific map."));
                        }
                    }

                    // Merge the IScanInfection items from this anomaly
                    MergeScanInfectionItems(&EZAnomaly);

                    // Set the common UI generic text
                    m_DisplayNames.SetGenericText(EZAnomaly);

                    // Perform any automatic handling for this item
                    HandleAnomaly(&EZAnomaly);
                }
                else
                {
                    CCTRACEE(_T("CScanManager::DoFileVidReverseScan() - Failed to get anomaly %d from the file\\VID detected list. eResult = %d"), nCur, eRes);
                }
            }
        }
    }
}

bool CScanManager::FindCorrespondingQuarantineItem(CEZAnomaly* pEZAnomaly, 
                                                   DWORD dwVID, UUID &uuid)
{
    if ( pEZAnomaly == NULL )
    {
        CCTRACEE(_T("CScanManager::FindCorrespondingQuarantineItem() - NULL anomaly paramanter."));
        return false;
    }

    // Get the remediation list
    ccEraser::IRemediationActionListPtr pRemediateList = 
                                                 pEZAnomaly->GetRemediations();

    if ( pRemediateList == NULL )
    {
        CCTRACEE(_T("CScanManager::FindCorrespondingQuarantineItem() - Failed to get remediation action list."));
        return false;
    }

    size_t nCount = pEZAnomaly->GetRemediationCount();

    // Now we loop through each remediation action
    for (size_t nCurRem = 0; nCurRem < nCount; nCurRem++)
    {
        ccEraser::IRemediationActionPtr pAction = 
                                           pEZAnomaly->GetRemediation(nCurRem);
        if ( pAction == NULL )
        {
            CCTRACEE(_T("CScanManager::FindCorrespondingQuarantineItem() - Failed to get remediation action item %d."), nCurRem);
            continue;
        }

        // Wrap with remediation helper class
        CEZRemediation ezRem(pAction);

        // Get the referenced file paths to see if this item is referencing a 
        // file that we are going to delete on reboot
        cc::IIndexValueCollectionPtr spFilePaths = 
                                                ezRem.GetReferencedFilePaths();

        if ( spFilePaths != NULL )
        {
            size_t nCount = spFilePaths->GetCount();

            for ( size_t nCur=0; nCur<nCount; nCur++ )
            {
                cc::IStringPtr spStrCurPath = NULL;

                if ( spFilePaths->GetValue( 
                                    nCur, 
                                    reinterpret_cast<ISymBase*&>(spStrCurPath))
                     && 
                     spStrCurPath )
                {
                    CSymPtr<IQuarantineItem> spQuarItem;

                    if ( m_Quarantine.LoadQuarantineItemFromFilenameVID(
                                                   spStrCurPath->GetStringA(), 
                                                   dwVID, &spQuarItem ) && 
                         spQuarItem )
                    {
                        CCTRACEI(_T("CScanManager::FindCorrespondingQuarantineItem() - Found a referenced file path quarantine item for path %s."),spStrCurPath->GetStringA());
                        if ( SUCCEEDED(spQuarItem->GetUniqueID(uuid)) )
                        {
                            return true;
                        }
                        else
                        {
                            CCTRACEE(_T("CScanManager::FindCorrespondingQuarantineItem() - Failed to get the quarantine item UUID."));
                            return false;
                        }
                    }
                }
            }
        }
    }

    return false;
}

SCANSTATUS 
CScanManager::DealWithBootScanInfection(IScanInfection* pInfection)
{
    // What type is this guy?
    IScanBootInfectionQIPtr pBR = pInfection;
    IScanMBRInfectionQIPtr pMBR = pInfection;

    // Create an anomaly for this item
    ccEraser::IAnomalyPtr pAnomaly;
    bool bGenerated = GenerateAnomalyFromInfection(pInfection, pAnomaly, 
                                                   InfectionStatus_Unhandled, 
                                                   ACTION_REPAIR, false, 
                                                   false);

    if ( !bGenerated || pAnomaly == NULL )
    {
        CCTRACEE(_T("CScanManager::DealWithBootScanInfection() - Failed to generate an anomaly"));
    }

    CEZAnomaly ezAnomaly(pAnomaly);

    if ( pMBR != NULL )
    {
        m_ScanMgrCounts.IncrementItemCount(CScanManagerCounts::MBRInfected);

        // Set the MBR type
        ezAnomaly.SetMemBootType(MBRInfectionType);

        // Attempt to handle now
        HandleAnomaly(&ezAnomaly);
    }
    else if ( pBR != NULL )
    {
        m_ScanMgrCounts.IncrementItemCount(CScanManagerCounts::BRInfected);

        // Set the Boot Record type
        ezAnomaly.SetMemBootType(BRInfectionType);

        // Attempt to handle now
        HandleAnomaly(&ezAnomaly);
    }
    else
    {
        CCTRACEE(_T("CScanManager::DealWithBootScanInfection() - This is not a valid memory or boot record infection"));
    }

    return SCAN_FALSE;
}

bool CScanManager::AreCategoriesDisabled(IScanInfection* pInfection)
{
    // Check the categories against the filter
    const unsigned long* pCategories = pInfection->GetCategories();
    unsigned long ulCount = pInfection->GetCategoryCount();

    if( pCategories == NULL )
    {
        CCTRACEE(_T("CScanManager::AreCategoriesDisabled() - Categories is NULL, will assume this is not a disabled category to be safe."));
        return false;
    }

    // Loop through each category
    for( unsigned long ulIndex = 0; ulIndex < ulCount; ulIndex++  )
    {
        // Find this in our map of category states
        std::map<DWORD, bool>::iterator Iter = m_CategoryEnabled.find(pCategories[ulIndex]);

        if( Iter != m_CategoryEnabled.end() && (*Iter).second == true )
        {
            // At least one of this scan infections categories is enabled
            return false;
        }
    }

    // This means none of the categories were in the filter
    CCTRACEI(_T("CScanManager::AreCategoriesDisabled() - None of the scan infection categories are enabled. Ignoring this item."));
    return true;
}

void CScanManager::DoSingleGenericLoadPointScan()
{
    // We must have an eraser object to do the scan
    if ( m_pEraser == NULL )
    {
        CCTRACEE(_T("CScanManager::DoSingleGenericLoadPointScan() - The eraser object is null"));
        return;
    }

    // Input filter should already be set to GLP only and output filter should 
    // only include the enabled categories.

    // Run the scan in ANSI mode
    CFileAPI fileAPI;
    fileAPI.SwitchAPIToANSI();

    // Scan
    ccEraser::IContextPtr pContext;
    CCTRACEI(_T("CScanManager::DoSingleGenericLoadPointScan() - Calling IEraser::Scan()."));

    if (!m_bFullScan) 
    {
        m_bAdditionalScan = true;
    }

    m_bEraserScanning = true;

    // Do we want only relevant risks from this scan?
    if(m_Settings[SMSETTING_ONLY_RELEVANT_RISKS])
    {
        CCTRACEI(_T("CScanManager::DoSingleGenericLoadPointScan - only capturing relevant detections."));
        m_bIgnoreOnAnomalyDetected = true;
    }

    m_ScanMgrCounts.UseInternalText(IDS_GSE_DELAYED_TEXT, 10);
    ccEraser::eResult eRes = m_pEraser->Scan(m_pEraserCallback, pContext);
    m_ScanMgrCounts.StopUsingInternalText();

    m_bIgnoreOnAnomalyDetected = false;
    m_bEraserScanning = false;

    CCTRACEI(_T("CScanManager::DoSingleGenericLoadPointScan() - Finished calling IEraser::Scan(). eResult = %d"), eRes);

    // Reset the file APIs
    fileAPI.RestoreOriginalAPI();

    if ( pContext )
    {
        size_t nAnomComplete = pContext->GetAnomalyCompleteCount();
        size_t nAnomCount = pContext->GetAnomalyCount();

        CCTRACEI(_T("CScanManager::DoSingleGenericLoadPointScan() - Anomaly Complete Count = %d. Anomaly Count = %d"), nAnomComplete, nAnomCount);
        
        ccEraser::IAnomalyListPtr spList;
        spList.Attach(pContext->GetDetectedAnomalyList());

        if ( spList )
        {
            size_t nCount = 0;
            eRes = spList->GetCount(nCount);

            if ( ccEraser::Succeeded(eRes) )
            {
                CCTRACEI(_T("CScanManager::DoSingleGenericLoadPointScan() - Anomaly List Count = %d."), nCount);
            }
            else
            {
                CCTRACEE(_T("CScanManager::DoSingleGenericLoadPointScan() - Get anomaly list count failed. eResult = %d."), eRes);
            }
            
            if(ccEraser::Succeeded(eRes) && m_Settings[SMSETTING_ONLY_RELEVANT_RISKS])
            {
                // Attach the non-relevant list
                m_pNonRelevantAnomalyList = spList ;

                // build the VID/index map for the non-relevant Anomalies
                for(size_t nIndex = 0; nIndex < nCount; nIndex++)
                {
                    ccEraser::IAnomalyPtr spAnomaly;
                    eRes = m_pNonRelevantAnomalyList->GetItem(nIndex, spAnomaly);
                    if(ccEraser::Succeeded(eRes))
                    {
                        // EZAnomaly helper
                        CEZAnomaly ezAnom;
                        if ( !ezAnom.Initialize(spAnomaly) )
                        {
                            CCTRACEE(_T("CScanManager::DoSingleGenericLoadPointScan() - Failed to wrap generated anomaly"));
                            continue;
                        }

                        DWORD dwVID = ezAnom.GetVID();
                        m_mapNonRelevantAnomalyIndex[dwVID] = nIndex;
                    }
                }
            }

        }
        else
        {
            CCTRACEE(_T("CScanManager::DoSingleGenericLoadPointScan() - The anomaly list is NULL."));
        }
    }
    else
    {
        CCTRACEE(_T("CScanManager::DoSingleGenericLoadPointScan() - The context object returned is NULL."));
    }

    //
    // Set the internal flag indicating not to run a GLP scan since we ust 
    // did
    //
    m_Settings[SMSETTING_SCAN_GENERIC_LOAD_POINTS] = false;
}

bool 
CScanManager::ShouldAutoDeleteBasedOnDamageFlags(CEZAnomaly* pEZAnomaly, 
                                                 bool bUseDependencyFlag /*= true*/)
{
    if ( pEZAnomaly == NULL )
    {
        CCTRACEE(_T("CScanManager::ShouldAutoDeleteBasedOnDamageFlags() - Anomaly is null"));
        return false;
    }

    // Need to use the damage flags to determine the action to take
    DWORD dwDamageAvg = pEZAnomaly->GetDamageFlagAverage();

    if ( bUseDependencyFlag )
    {
        bUseDependencyFlag = pEZAnomaly->KnownToHaveDependencies();
    }

    // If the damage average is 4 or higher and there are no dependencies we 
    // will automatically handle this threat
    if ( dwDamageAvg >= 4 && !bUseDependencyFlag )
    {
        CCTRACEI(_T("CScanManager::ShouldAutoDeleteBasedOnDamageFlags() - Damage flag logic is to automatically remove this threat."));
        return true;
    }

    return false;
}

void CScanManager::SaveRebootRemediationsToDisk(CEZAnomaly* pEZAnomaly)
{
    // If there are no reboot paths then there is nothing to do
    if ( m_vRebootFiles.empty() )
    {
        return;
    }

    if ( pEZAnomaly == NULL )
    {
        CCTRACEE(_T("CScanManager::SaveRebootRemediationsToDisk() - NULL anomaly paramanter."));
        return;
    }

    // Get the remediation list
    ccEraser::IRemediationActionListPtr pRemediateList = 
                                                 pEZAnomaly->GetRemediations();

    if ( pRemediateList == NULL )
    {
        CCTRACEE(_T("CScanManager::SaveRebootRemediationsToDisk() - Failed to get remediation action list."));
        return;
    }

    size_t nCount = pEZAnomaly->GetRemediationCount();

    // Now we loop through each remediation action looking for IScanInfection 
    // items
    for (size_t nCurRem = 0; nCurRem < nCount; nCurRem++)
    {
        ccEraser::IRemediationActionPtr pAction = 
                                           pEZAnomaly->GetRemediation(nCurRem);

        if ( pAction == NULL )
        {
            CCTRACEE(_T("CScanManager::SaveRebootRemediationsToDisk() - Failed to get remediation action item %d."), nCurRem);
            continue;
        }

        // Wrap with remediation helper class
        CEZRemediation ezRem(pAction);

        // If the remediation errored out with FileStillPresent and we have a 
        // reboot pending operation for one of the files it is dependent on 
        // then we will save it for processing after reboot. We do not save 
        // scan infection remediation actions, because the IScanInfection
        // item is not serialized so we will not be able to post process it
        ccEraser::eResult eRemResult;

        if ( ezRem.GetRemediationResult(eRemResult) &&
             ccEraser::FileStillPresent == eRemResult &&
             !ezRem.IsScanInfectionType() )
        {
            // Get the referenced file paths to see if this item is 
            // referencing a file that we are going to delete on reboot
            cc::IIndexValueCollectionPtr spFilePaths = 
                                               ezRem.GetReferencedFilePaths();

            if ( spFilePaths != NULL )
            {
                size_t nCount = spFilePaths->GetCount();

                for ( size_t nCur=0; nCur<nCount; nCur++ )
                {
                    cc::IStringPtr spStrCurPath = NULL;

                    if ( spFilePaths->GetValue(nCur, 
                                               reinterpret_cast<ISymBase*&>(spStrCurPath)) )
                    {
                        ATL::CAtlString strCurPath = spStrCurPath->GetStringA();
                        strCurPath.MakeLower();
                        std::set<ATL::CAtlString>::iterator Iter = 
                                               m_vRebootFiles.find(strCurPath);

                        if ( Iter != m_vRebootFiles.end() )
                        {
                            CCTRACEI(_T("CScanManager::SaveRebootRemediationsToDisk() - Found %s in reboot paths. Saving remediation for processing after reboot."), (LPCTSTR)strCurPath);

                            // If we have quarantine item associated with the 
                            // anomaly then we want to first set the 
                            // quarantine item UUID so it can be loaded and 
                            // added to after reboot
                            UUID uuid = {0};

                            if ( pEZAnomaly->GetQuarantineItemUUID(uuid) )
                            {
                                ezRem.SetQuarantineItemUUID(uuid);
                            }

                            if ( SaveRemediationAction(pAction) )
                            {
                                // Mark this remediation so it will be shown 
                                // as reboot required instead of a failure
                                CCTRACEI(_T("CScanManager::SaveRebootRemediationsToDisk() - Successfully saved remediation for processing after reboot."));
                                ezRem.SetManualRebootRequired();
                            }
                            else
                            {
                                CCTRACEE(_T("CScanManager::SaveRebootRemediationsToDisk() - Failed to save remediation for processing after reboot."));
                            }
                        }
                    }
                }
            }
        }
    }
}

bool CScanManager::SaveRemediationAction(ccEraser::IRemediationAction* pAction)
{
    CFileAPI fileAPI;
    fileAPI.SwitchAPIToANSI();

    if (pAction == NULL)
    {
        CCTRACEE(_T("CScanManager::SaveRemediationAction - remediation action is NULL."));
        return false;
    }

    CNAVInfo navInfo;
    ATL::CAtlString strRemFolder = navInfo.GetNAVUserDir();
    strRemFolder += _T("\\");
    strRemFolder += REMEDIATION_DATA_FOLDER;

    // Make sure the folder exists
    if( INVALID_FILE_ATTRIBUTES == GetFileAttributes(strRemFolder) )
    {
        CCTRACEI(_T("CScanManager::SaveRemediationAction - Creating remediation data folder = %s"), (LPCTSTR)strRemFolder);
        EnsurePathExists(strRemFolder);
    }

    ATL::CAtlString strRemFile;
    UINT uRet = GetTempFileName(strRemFolder, _T("rem"), 0, strRemFile.GetBuffer(MAX_PATH));
    strRemFile.ReleaseBuffer();
    if( 0 == uRet )
    {
        CCTRACEE(_T("CScanManager::SaveRemediationAction - Failed to get a temp path for remediation action."));
        return false;
    }

    // Delete the created temp file
    DeleteFile(strRemFile);

    ccSym::CFileStreamImplPtr pFileStream;
    pFileStream.Attach(ccSym::CFileStreamImpl::CreateFileStreamImpl());

    if (pFileStream == NULL)
    {
        CCTRACEE(_T("CScanManager::SaveRemediationAction - File stream is NULL."));
        return false;
    }

    CCTRACEI(_T("CScanManager::SaveRemediationAction - Creating remediation data file = %s"), (LPCTSTR)strRemFile);

    if (FALSE == pFileStream->GetFile().Open(strRemFile, 
        GENERIC_READ | GENERIC_WRITE,
        0, 
        NULL, 
        CREATE_ALWAYS, 
        0,
        NULL))
    {
        CCTRACEE(_T("CScanManager::SaveRemediationAction() - pFileStream->GetFile().Open() failed for file %s"), (LPCTSTR)strRemFile);
        return false;
    }

    // Get ISerialize interface from the remediation action
    cc::ISerializeQIPtr pSerialize(pAction);
    if (pSerialize == NULL)
    {
        CCTRACEE(_T("CScanManager::SaveRemediationAction() - pSerialize QI failed."));
        pFileStream->GetFile().Close();
        pFileStream->GetFile().Delete(strRemFile, FALSE);
        return false;
    }

    if (pSerialize->Save(pFileStream) == false)
    {
        CCTRACEE(_T("CScanManager::SaveRemediationAction() - pSerialize->Save() failed."));
        pFileStream->GetFile().Close();
        pFileStream->GetFile().Delete(strRemFile, FALSE);
        return false;
    }

    if( !pFileStream->GetFile().Close() )
    {
        CCTRACEE(_T("CScanManager::SaveRemediationAction() - Failed to close the file %s"), (LPCTSTR)strRemFile);
    }

    // Setup our run once key
    ATL::CRegKey regKeyRunOnce;
    if( ERROR_SUCCESS == regKeyRunOnce.Create(HKEY_CURRENT_USER, _T("Software\\Microsoft\\Windows\\CurrentVersion\\RunOnce")) )
    {
        ATL::CAtlString strNavRunOnce = navInfo.GetNAVDir();
        strNavRunOnce += _T("\\Navw32.exe ");
        strNavRunOnce += SWITCH_REMEDIATE_AFTER_REBOOT;
        CCTRACEI(_T("CScanManager::SaveRemediationAction() - Creating the run once post reboot key = %s"), (LPCTSTR)strNavRunOnce);
        if( ERROR_SUCCESS != regKeyRunOnce.SetStringValue(REMEDIATION_RUNONCE_KEY, strNavRunOnce) )
        {
            CCTRACEE(_T("CScanManager::SaveRemediationAction() - Failed to create the run once key"));
        }
    }
    else
    {
        CCTRACEE(_T("CScanManager::SaveRemediationAction() - Failed to open the run once key"));
    }

    return true;
}

void CScanManager::PerformPostRebootRemediations()
{
    CFileAPI fileAPI;
    fileAPI.SwitchAPIToANSI();

    // Initialize ccScan in ANSI mode
    m_Settings[SMSETTING_OEM] = false;

    // Use single threaded model for ccScan although we shouldn't be doing any
    // scanning, we never know what eraser might want to do...
    m_dwScanThreads = 0;

    // Load the scanner
    if( SMRESULT_OK != GetScanner(g_szNavAppIdNAVW32, m_iBloodhoundLevel, 0) )
    {
        CCTRACEE(_T("CScanManager::PerformPostRebootRemediations() - GetScanner failed."));
        return;
    }

    // Load eraser
    if( SMRESULT_OK != GetEraser() )
    {
        CCTRACEE(_T("CScanManager::PerformPostRebootRemediations() - GetEraser failed."));
        return;
    }

    // Load Quarantine server
    if( !m_Quarantine.Open(m_pScanner) )
    {
        CCTRACEE(_T("CScanManager::PerformPostRebootRemediations() - Quarantine load failed."));
    }

    // Get the remediation folder name
    CNAVInfo navInfo;
    ATL::CAtlString strRemFolder = navInfo.GetNAVUserDir();
    strRemFolder += _T("\\");
    strRemFolder += REMEDIATION_DATA_FOLDER;
    ATL::CAtlString strRemFolderAllFiles = strRemFolder + _T("\\*.*");
    ATL::CAtlString strCurRemFile;

    // Loop through all of the items in the remediation data folder
    BOOL bContinue = TRUE;
    WIN32_FIND_DATA wfd;
    HANDLE hFinder = FindFirstFile(strRemFolderAllFiles,&wfd);

    // Add all of the test files to Quarantine
    if(INVALID_HANDLE_VALUE == hFinder)
    {
        CCTRACEE(_T("CScanManager::PerformPostRebootRemediations - Unable to find first file."));
        return;
    }

    ccEraser::eResult eRes;
    BOOL bRebootRequired = FALSE;
    BOOL bFileStillPresent = FALSE;

    // Loop through each of the files.
    while(bContinue)
    {
        // If the file is not a directory then process it.
        if(!(FILE_ATTRIBUTE_DIRECTORY & wfd.dwFileAttributes))
        {
            strCurRemFile = strRemFolder;
            strCurRemFile += _T("\\");
            strCurRemFile += wfd.cFileName;

            CCTRACEI(_T("CScanManager::PerformPostRebootRemediations - Processing %s"), (LPCTSTR)strCurRemFile);
            eRes = ProcessRemediationFile(strCurRemFile);
            if(ccEraser::RebootRequired == eRes)
            {
                bRebootRequired = TRUE;
            }
            else if(ccEraser::FileStillPresent == eRes)
            {
                bFileStillPresent = TRUE;
            }

            // Delete the file
            if( !DeleteFile(strCurRemFile) )
            {
                CCTRACEE(_T("CScanManager::PerformPostRebootRemediations - Unable to delete the file after processing. file = %s"), (LPCTSTR)strCurRemFile);
            }
        }

        // Stop the loop the first time FindNextFile fails.
        bContinue = FindNextFile(hFinder,&wfd);
    }

    FindClose(hFinder);

    // Delete our directory
    if( !RemoveDirectory(strRemFolder) )
    {
        CCTRACEE(_T("CScanManager::PerformPostRebootRemediations - Unable to delete the directory after processing. Directory = %s"), (LPCTSTR)strRemFolder);
    }

    // Close quarantine
    m_Quarantine.Close();
    if(bFileStillPresent || bRebootRequired)
    {
        UINT uErrorId = NULL; 
        CString cszErrorText;
        
        // If there were any FileStillPresent errors, we want to 
        //  warn the user.
        if(bFileStillPresent)
        {
            uErrorId = IDS_POSTREBOOTREMEDIATION_ERROR_FILE_STILL_PRESENT;
            cszErrorText.Format(uErrorId, m_strProductName);

            // Get a NAVError object
            CComBSTR bstrText(cszErrorText);

            CComBSTR bstrNAVErr(_T("NAVError.NAVCOMError"));
            CComPtr <INAVCOMError> spNavError;


            // Create, Populate, Log, and Display the error
            if( SUCCEEDED( spNavError.CoCreateInstance(bstrNAVErr, NULL, CLSCTX_INPROC_SERVER) ) )
            {
                if( SUCCEEDED(spNavError->put_ModuleID(AV_MODULE_ID_SCANMANAGER)) &&
                    SUCCEEDED(spNavError->put_ErrorID(uErrorId)) &&
                    SUCCEEDED(spNavError->put_Message(bstrText)) )
                {
                    spNavError->Show(TRUE, TRUE, NULL);
                }
                else
                {
                    CCTRACEE(_T("CScanManager::OnError() - Failed to fill out the NAV error object"));
                }
            }
            else
            {
                CCTRACEE(_T("CScanManager::OnError() - Failed to create the NAV error object"));
            }
        }
        // If there weren't any FileStillPresent errors, but another
        //  reboot is required, then alert the user.
        else if(bRebootRequired)
        {
            uErrorId = IDS_POSTREBOOTREMEDIATION_ERROR_REBOOT_REQUIRED;
            cszErrorText.Format(uErrorId, m_strProductName);
            
            ::MessageBox(NULL, cszErrorText, m_strProductName, MB_OK | MB_ICONINFORMATION | MB_SYSTEMMODAL);
        }
    }

}

ccEraser::eResult CScanManager::ProcessRemediationFile(LPCTSTR pcszFileName)
{
    ccEraser::eResult eReturn = ccEraser::Success;

    // Get a file stream object
    ccSym::CFileStreamImplPtr pFileStream;
    pFileStream.Attach(ccSym::CFileStreamImpl::CreateFileStreamImpl());

    if (pFileStream == NULL)
    {
        CCTRACEE(_T("CScanManager::ProcessRemediationFile - File stream is NULL."));
        return ccEraser::Fail;
    }

    // Open the file
    if (FALSE == pFileStream->GetFile().Open(pcszFileName,
        GENERIC_READ | GENERIC_WRITE,
        0, 
        NULL, 
        OPEN_EXISTING, 
        0,
        NULL))
    {
        CCTRACEE(_T("CScanManager::ProcessRemediationFile() - pFileStream->GetFile().Open() failed for file = %s"), pcszFileName);
       return ccEraser::Fail;
    }

    ccEraser::eObjectType type;
    ISymBasePtr pBase;
    ccEraser::eResult eRes = m_pEraser->CreateObjectFromStream(pFileStream, &type, pBase);

    // Close the file
    pFileStream->GetFile().Close();

    if(ccEraser::Failed(eRes))
    {
        CCTRACEE(_T("CScanManager::ProcessRemediationFile() - Failed to create the remediation action from the file stream. eRes == %d"), eRes);
        return ccEraser::Fail;
    }

    ccEraser::IRemediationActionQIPtr pRemediationAction = pBase;
    if(pRemediationAction == NULL)
    {
        CCTRACEE(_T("CScanManager::ProcessRemediationFile() - Failed to QI for the remediation action."));
        return ccEraser::Fail;
    }

    cc::IStringPtr spStrDesc = NULL;
    if( ccEraser::Succeeded(pRemediationAction->GetDescription(spStrDesc)) )
        CCTRACEI(_T("CScanManager::ProcessRemediationFile() - Loaded remediation action. Description = %s"), spStrDesc->GetStringA());

    CEZRemediation ezRem(pRemediationAction);

    ATL::CAtlString strUndoFile;
    cc::IStream* pUndoStream = NULL;
    if( ezRem.ShouldSaveRemediation() )
    {
        GetUndoData(pRemediationAction, pUndoStream, strUndoFile);
    }
    BOOL bRebootRequired = FALSE;
    eRes = pRemediationAction->Remediate();

    if( ccEraser::FileNotFound == eRes || ccEraser::NothingToDo == eRes )
    {
        CCTRACEI(_T("CScanManager::ProcessRemediationFile() - Remediation not found or nothing to do."));
    }
    else if( ccEraser::Succeeded(eRes) )
    {
        CCTRACEI(_T("CScanManager::ProcessRemediationFile() - Remediation succeeded. eRes = %d"), eRes);
    }
    else if( ccEraser::RebootRequired == eRes )
    {
        CCTRACEI(_T("CScanManager::ProcessRemediationFile() - Another re-boot is necessary for this Remediation."));
        eReturn = eRes;

    }
    else if( ccEraser::FileStillPresent == eRes )
    {
        CCTRACEI(_T("CScanManager::ProcessRemediationFile() - Another re-boot is necessary for this Remediation."));
        eReturn = eRes;

    }
    else
    {
        CCTRACEW(_T("CScanManager::ProcessRemediationFile() - Remediation failed. eRes = %d"), eRes);
    }

    // If this remediation action has a quarantine item load it up and add to it
    if( ezRem.ShouldSaveRemediation() )
    {
        UUID quarUUID = {0};
        if( ezRem.GetQuarantineItemUUID(quarUUID) )
        {
            CSymPtr<IQuarantineItem> pQuarItem;
            if( m_Quarantine.LoadQuarantineItemFromUUID(quarUUID, &pQuarItem) && pQuarItem != NULL )
            {
                CCTRACEI(_T("CScanManager::ProcessRemediationFile() - Successfully loaded associated quarantine item."));
                if( SUCCEEDED(pQuarItem->AddRemediationData(pRemediationAction, pUndoStream)) )
                {
                    CCTRACEI(_T("CScanManager::ProcessRemediationFile() - Added remediation undo data to the quarantine item."));
                    pQuarItem->SaveItem();
                }
                else
                {
                    CCTRACEW(_T("CScanManager::ProcessRemediationFile() - Failed to add remediation undo data to the quarantine item."));
                }
            }
            else
            {
                CCTRACEE(_T("CScanManager::ProcessRemediationFile() - Failed to load quarantine item from UUID."));
            }
        }
    }

    // Free any undo data
    ReleaseUndoData(pUndoStream, strUndoFile);

    return eReturn;
}

bool CScanManager::GenerateAnomalyFromInfection(
                                          IScanInfection* pInfection, 
                                          ccEraser::IAnomaly*& pAnomaly,
                                          CommonUIInfectionStatus eStatus,
                                          AnomalyAction eNextAction,
                                          bool bCheckforSpecifics, /*= true*/
                                          bool bIncrementFindCounts /*= true*/)
{
    CCTRACEI(_T("CScanManager::GenerateAnomalyFromInfection() - Begin."));

    if ( pAnomaly != NULL )
    {
        pAnomaly->Release();
        pAnomaly = NULL;
    }

    if ( pInfection == NULL )
    {
        CCTRACEE(_T("CScanManager::GenerateAnomalyFromInfection() - Infection is null."));
        return false;
    }

    // See if a specific exists for this VID
    bool bSpecific = false;

    // Is this a compressed infection?
    IScanCompressedFileInfectionQIPtr pCompressed = pInfection;

    // Do we need to run a generic load point scan?
    if( (NULL == pCompressed.m_p) &&       // This is not a compressed infection

        bCheckforSpecifics && // Explicitly told to only wrap the 
                                // IScanInfection passed in (boot infections)
                                        
        !m_Settings[SMSETTING_RUN_ERASER_SCAN] && // We have not run the 
                                                    // eraser scan
                                                    
        m_Settings[SMSETTING_SCAN_GENERIC_LOAD_POINTS]  // GLP scanning is 
                                                        // explicitly 
                                                        // disabled 
                                                        // (email\IM scans)
                                                        // or we already 
                                                        // ran it
        )
    {
        DoSingleGenericLoadPointScan();

        // Check if this infection was found during the generic scan
        if ( m_InfectionMerge.CheckInfection(pInfection) )
        {
            // We've detected the original IScanInfection item that began 
            // this madness during the generic load point scan, should be 
            // safe to ignore the first anomaly list detected
            CCTRACEI(_T("CScanManager::GenerateAnomalyFromInfection() - The virus = %s Vid = %lu was already detected during the generic scan. The original IScanInfection can be ignored."), pInfection->GetVirusName(), pInfection->GetVirusID());
            return false;
        }
        // Check if the generic scan picked up a specific anomaly that 
        // this infection should be added to
        else if ( AddInfectionToExistingSpecificAnomaly(pInfection) )
        {
            CCTRACEI(_T("CScanManager::GenerateAnomalyFromInfection() - Infection %s vid = %lu was added to an existing specific anomaly after the GLP scan."), pInfection->GetVirusName(), pInfection->GetVirusID());
            return false;
        }
    }

    // Check if the infection is in the non-relevant list
    if ( m_Settings[SMSETTING_ONLY_RELEVANT_RISKS] && AddInfectionToExistingNonRelevantAnomaly(pInfection) )
    {
        CCTRACEI(_T("CScanManager::GenerateAnomalyFromInfection() - Infection %s vid = %lu was added to an existing specific non-relevant anomaly."), pInfection->GetVirusName(), pInfection->GetVirusID());
        return false;
    }

    ccEraser::eResult eRes = ccEraser::Success;
    ccEraser::IAnomalyListPtr pAnomList;
    
    // EZAnomaly helper
    CEZAnomaly ezAnom;

    if ( bCheckforSpecifics )
    {
        // Must run in ANSI mode for eraser
        CFileAPI fileAPI;
        fileAPI.SwitchAPIToANSI();

        if ( !m_bFullScan ) 
        {
            m_bAdditionalScan = true;
        } 

        m_bEraserScanning = true;
        CCTRACEI(_T("CScanManager::GenerateAnomalyFromInfection() - Calling IEraser::DetectAnomalies() to attempt to get a specific anomaly for THREAT = %s VID = %lu."), pInfection->GetVirusName(), pInfection->GetVirusID());
        eRes = m_pEraser->DetectAnomalies(NULL, pInfection->GetVirusID(), 
                                          pAnomList);
        CCTRACEI(_T("CScanManager::GenerateAnomalyFromInfection() - Completed calling IEraser::DetectAnomalies(). eResult = %d"), eRes);
        m_bEraserScanning = false;

        fileAPI.RestoreOriginalAPI();

        if ( ccEraser::Succeeded(eRes) && pAnomList != NULL)
        {
            // Get the specific anomaly from the list
            size_t nCount = 0;
            eRes = pAnomList->GetCount(nCount);

            if ( ccEraser::Succeeded(eRes) && nCount > 0 )
            {
                // There should only be one anomaly generated
                CCTRACEI(_T("CScanManager::GenerateAnomalyFromInfection() - IEraser::DetectAnomalies() found %d specific anomalies for THREAT = %s VID = %lu."), nCount, pInfection->GetVirusName(), pInfection->GetVirusID());
                _ASSERT(nCount == 1);
                bSpecific = true;
            }
            else
            {
                CCTRACEE(_T("CScanManager::GenerateAnomalyFromInfection() - Failed to get the specific anomaly list count. eResult = %d."), eRes);
            }
        }
        else
        {
            CCTRACEE(_T("CScanManager::GenerateAnomalyFromInfection() - DetectAnomalies() failed or returned a NULL list. eResult = %d."), eRes);
        }
    }
    else
    {
        CCTRACEI(_T("CScanManager::GenerateAnomalyFromInfection() - Explicitly not even checking eraser for specific anomalies."));
    }

    // If this is a specific anomaly get it from the list
    if ( !bSpecific || ccEraser::Failed(pAnomList->GetItem(0, pAnomaly)) || 
        pAnomaly == NULL )
    {
        // Need to build our own non-specific anomaly for this infection
        CCTRACEI(_T("CScanManager::GenerateAnomalyFromInfection() - Building our own non-specific anomaly for THREAT = %s VID = %lu."), pInfection->GetVirusName(), pInfection->GetVirusID());
        bSpecific = false;
        eRes = m_pEraser->CreateObject(ccEraser::AnomalyType, 
                                       ccEraser::IID_Anomaly, 
                                       reinterpret_cast<void**>(&pAnomaly));

        if ( ccEraser::Failed(eRes) || pAnomaly == NULL )
        {
            CCTRACEE(_T("CScanManager::GenerateAnomalyFromInfection() - Failed to create a new anomaly object. eResult = %d"), eRes);
            return false;
        }

        // Add the infection data to the anomaly
        SetAnomalyValuesFromInfection(pAnomaly, pInfection);

        // Create a new remediation action list for the anomaly
        ccEraser::IRemediationActionListPtr spRemediationList;
        eRes = m_pEraser->CreateObject(ccEraser::RemediationActionListType, 
                                       ccEraser::IID_RemediationActionList, 
                                       reinterpret_cast<void**>
                                                         (&spRemediationList));

        if ( ccEraser::Failed(eRes) || spRemediationList == NULL )
        {
            CCTRACEE(_T("CScanManager::GenerateAnomalyFromInfection() - Failed to create a new remediaton action list. eResult = %d"), eRes);
            return false;
        }

        // Add the remediation action list to the anomaly
        eRes = pAnomaly->SetRemediationActions(spRemediationList);

        if ( ccEraser::Failed(eRes) )
        {
            CCTRACEE(_T("CScanManager::GenerateAnomalyFromInfection() - Failed to add the new remediaton action list to the anomaly. eResult = %d"), eRes);
            return false;
        }

        // Create a remediation action from the infection
        ccEraser::IRemediationActionPtr spRem;

        if ( !GenerateInfectionRemediationAction(pInfection, spRem) || 
             spRem == NULL )
        {
            CCTRACEE(_T("CScanManager::GenerateAnomalyFromInfection() - Failed to generate the remediation action for the non-specific anomaly."));
            return false;
        }

        // Add the infection remediation action to this remediation action list
        if ( ccEraser::Failed(eRes = spRemediationList->Add(spRem)) )
        {
            CCTRACEE(_T("CScanManager::GenerateAnomalyFromInfection() - Failed to add the infection to the remediation action list for the non-specific anomaly. eResult = %d"), eRes);
            return false;
        }

        // Initialize the helper class
        if ( !ezAnom.Initialize(pAnomaly) )
        {
            CCTRACEE(_T("CScanManager::GenerateAnomalyFromInfection() - Failed to initialize anomaly helper with specific anomaly."));
            return false;
        }

        // Set the initial anomaly status
        ezAnom.SetAnomalyStatus(eNextAction, eStatus);

        // Merge the infection
        MergeScanInfectionItems(&ezAnom);

        // Set the single infection anomaly flag
        ezAnom.SetSingleInfectionAnomaly(true);
    }
    else  // Found a specific anomaly for this infections VID
    {
        // If this is a compressed infection we will replace the remediation 
        // action list with the compressed infection only
        if ( pCompressed )
        {
            CCTRACEI(_T("CScanManager::GenerateAnomalyFromInfection() - Replacing the specific remediations with the single compressed infection."));

            ccEraser::IRemediationActionListPtr spRemediationList;
            eRes = pAnomaly->GetRemediationActions(spRemediationList);
            if ( ccEraser::Failed(eRes) )
            {
                CCTRACEE(_T("CScanManager::GenerateAnomalyFromInfection() - Failed to get the remediaton action list. eResult = %d"), eRes);
                return false;
            }

            // Remove any remediations that are already in
            //  the list.
            size_t nCount = 0;
            eRes = spRemediationList->GetCount(nCount);
            if(nCount != 0)
            {
                for(size_t nIndex = 0; nIndex < nCount; nIndex++)
                {
                    eRes = spRemediationList->Remove(0);
                    if ( ccEraser::Failed(eRes) )
                    {
                        CCTRACEE(_T("CScanManager::GenerateAnomalyFromInfection() - Failed to remove the remediaton actions. eResult = %d"), eRes);
                    }
                }
            }

            // Create a remediation action from the infection
            ccEraser::IRemediationActionPtr spRem;

            if ( !GenerateInfectionRemediationAction(pInfection, spRem) || 
                 spRem == NULL )
            {
                CCTRACEE(_T("CScanManager::GenerateAnomalyFromInfection() - Failed to generate the remediation action for the specific compressed infection."));
                return false;
            }

            // Add the infection remediation action to this remediation action 
            // list
            if ( ccEraser::Failed(eRes = spRemediationList->Add(spRem)) )
            {
                CCTRACEE(_T("CScanManager::GenerateAnomalyFromInfection() - Failed to add the infection to the remediation action list for the specific compressed infection. eResult = %d"), eRes);
                return false;
            }
        }

        // Initialize the helper class with our anomaly
        if ( !ezAnom.Initialize(pAnomaly) )
        {
            CCTRACEE(_T("CScanManager::GenerateAnomalyFromInfection() - Failed to initialize anomaly helper with specific anomaly."));
            return false;
        }

        // Set the initial anomaly status
        ezAnom.SetAnomalyStatus(eNextAction, eStatus);

        // Merge all of the scan infection items for this anomaly
        MergeScanInfectionItems(&ezAnom);

        if ( pCompressed )
        {
            // Set the single infection anomaly flag
            ezAnom.SetSingleInfectionAnomaly(true);
        }
        else
        {
            // Do we need to manually add the IScanInfection that sparked this
            // anomaly detection?
            if ( m_InfectionMerge.CheckInfection(pInfection) == false )
            {
                CCTRACEI(_T("ScanManager::GenerateAnomalyFromInfection() - Manually adding the original infection to the specific anomaly."));

                // Grab the existing list of specific remediations for 
                // non-compressed specific anomalies
                ccEraser::IRemediationActionListPtr spRemediationList = 
                                                      ezAnom.GetRemediations();

                if ( !spRemediationList )
                {
                    CCTRACEE(_T("CScanManager::GenerateAnomalyFromInfection() - Failed to get the existing remediation action list from the specific anomaly."));
                    return false;
                }

                // Create a remediation action from the infection
                ccEraser::IRemediationActionPtr spRem;
                if ( !GenerateInfectionRemediationAction(pInfection, spRem) || 
                     spRem == NULL )
                {
                    CCTRACEE(_T("CScanManager::GenerateAnomalyFromInfection() - Failed to generate the remediation action for the specific infection."));
                    return false;
                }

                // Set the specific flag on this remediation action
                cc::IKeyValueCollectionPtr spRemProps;
                if ( ccEraser::Succeeded(eRes = 
                                           spRem->GetProperties(spRemProps)) && 
                    spRemProps != NULL )
                {
                    if ( !spRemProps->SetValue(
                                        ccEraser::IRemediationAction::Specific, 
                                        true) )
                        CCTRACEE(_T("CScanManager::GenerateAnomalyFromInfection() - Failed to set the specific property."));
                }
                else
                {
                    CCTRACEE(_T("CScanManager::GenerateAnomalyFromInfection() - Failed to get the remediation properties. eResult = %d"), eRes);
                }

                // Add the infection remediation action to this remediation 
                // action list
                if ( ccEraser::Failed(eRes = spRemediationList->Add(spRem)) )
                {
                    CCTRACEE(_T("CScanManager::GenerateAnomalyFromInfection() - Failed to add the infection to the remediation action list for the specific infection. eResult = %d"), eRes);
                    return false;
                }

                // Manually merge this infection now
                m_InfectionMerge.MergeInfection(pInfection);
            }
            else
            {
                CCTRACEI(_T("ScanManager::GenerateAnomalyFromInfection() - The original infection was already detected by the specific anomaly."));
            }

            // Check for the presence of the anomalies remediation actions
            if ( !CheckPresenceOfRemediationActions(&ezAnom) )
            {
                CCTRACEW(_T("CScanManager::GenerateAnomalyFromInfection() - None of the remediation actions for this anomaly were present. Skipping."));
                return false;
            }
        }
    }

    // Add this to our anomaly list
    eRes = m_pAnomalyList->Add(ezAnom);
    if ( ccEraser::Failed(eRes) )
    {
        CCTRACEE(_T("CScanManager::GenerateAnomalyFromInfection() - Failed to add the anomaly to our list. eResult = %d."), eRes);
        return false;
    }

    if ( bIncrementFindCounts )
    {
        // Update the found count
        if ( ezAnom.IsNonViral() )
        {
            m_ScanMgrCounts.IncrementItemCount(CScanManagerCounts::NonViral);
        }
        else
        {
            m_ScanMgrCounts.IncrementItemCount(CScanManagerCounts::Viral);
        }
    }

    // Add this item to our specific detected anomaly map if it is a specific non-compressed anomaly
    if ( bSpecific && !pCompressed )
    {
        size_t nIndex = 0;
        m_pAnomalyList->GetCount(nIndex);
        nIndex--;
        DWORD dwVID = ezAnom.GetVID();

        if ( dwVID != 0 )
        {
            m_mapSpecificAnomalyIndex[dwVID] = nIndex;
            CCTRACEI(_T("CScanManager::GenerateAnomalyFromInfection() - Added VID = %lu Index = %d to specific anomaly map."), dwVID, nIndex);
        }
        else
        {
            CCTRACEE(_T("CScanManager::GenerateAnomalyFromInfection() - This is a specific anomaly, but the VID is 0. Not adding to the specific map."));
        }
    }

    // Set the common UI generic text
    m_DisplayNames.SetGenericText(ezAnom);

    CCTRACEI(_T("CScanManager::GenerateAnomalyFromInfection() - End."));

    return true;
}

bool 
CScanManager::GenerateInfectionRemediationAction(
                                   IScanInfection* pInfection, 
                                   ccEraser::IRemediationAction*& pRemediation)
{
    if ( pInfection == NULL )
    {
        CCTRACEE(_T("CScanManager::GenerateInfectionRemediationAction() - Infection is null"));
        return false;
    }

    if ( pRemediation != NULL )
    {
        pRemediation->Release();
        pRemediation = NULL;
    }

    // Create a new remediation action
    ccEraser::eResult eRes = 
             m_pEraser->CreateObject(ccEraser::InfectionRemediationActionType, 
                                     ccEraser::IID_RemediationAction, 
                                     reinterpret_cast<void**>(&pRemediation));

    if ( ccEraser::Failed(eRes) || pRemediation == NULL )
    {
        CCTRACEE(_T("CScanManager::GenerateInfectionRemediationAction() - Failed to create a new infection remediaton action. eResult = %d"), eRes);
        return false;
    }

    CEZRemediation ezRem(pRemediation);

    // Add the infection to the remediation action
    if ( !ezRem.SetScanInfection(pInfection) )
    {
        CCTRACEE(_T("CScanManager::GenerateInfectionRemediationAction() - Failed to set the infection in the remediaton action."));
        return false;
    }

    // Set the operation type
    if ( !ezRem.SetOperationType(ccEraser::IRemediationAction::HandleThreat) )
    {
        CCTRACEE(_T("CScanManager::GenerateInfectionRemediationAction() - Failed to set the operation type for the infection remediaton action."));
        return false;
    }

    return true;
}

bool CScanManager::SetAnomalyValuesFromInfection( ccEraser::IAnomaly* pAnomaly, 
                                                  IScanInfection* pInfection )
{
    // Get the categories from the pInfection
    unsigned long nCategories = pInfection->GetCategoryCount();
    const unsigned long* pSrcCategories = pInfection->GetCategories();

    if (!pSrcCategories)
    {
        CCTRACEE(_T("CScanManager::SetAnomalyValuesFromInfection() - Failed to get the scan infection categories."));
        return false;
    }

    //
    // Add each category from the IScanInfection to the anomaly.
    //
    ccEraser::eResult result = ccEraser::Success;

    // Get the anomaly's properties
    cc::IKeyValueCollectionPtr pProps;
    if (ccEraser::Failed(result = pAnomaly->GetProperties(pProps)))
    {
        CCTRACEE(_T("CScanManager::SetAnomalyValuesFromInfection() - Failed to get the anomaly properties."));
        return false;
    }

    // Set the name
    cc::IStringPtr pStrThreatName;
    pStrThreatName.Attach(ccSym::CStringImpl::CreateStringImpl(pInfection->GetVirusName()));

    if (!pStrThreatName)
    {
        CCTRACEE(_T("CScanManager::SetAnomalyValuesFromInfection() - Failed to allocate the anomaly name string."));
        return false;
    }

    if ( !pProps->SetValue(ccEraser::IAnomaly::Name, pStrThreatName) )
    {
        CCTRACEE(_T("CScanManager::SetAnomalyValuesFromInfection() - Failed to set the anomaly name to %s."), pStrThreatName->GetStringA());
        return false;
    }

    // Set the VID
    if ( !pProps->SetValue(ccEraser::IAnomaly::VID, 
                           (DWORD)pInfection->GetVirusID()) )
    {
        CCTRACEE(_T("CScanManager::SetAnomalyValuesFromInfection() - Failed to set the anomaly VID."));
        return false;
    }

    // Create a collection to house the categories
    cc::IIndexValueCollectionPtr pCategories;
    pCategories.Attach(ccSym::CIndexValueCollectionImpl::CreateIndexValueCollectionImpl());

    if (!pCategories)
    {
        CCTRACEE(_T("CScanManager::SetAnomalyValuesFromInfection() - Failed to allocate the anomaly category index value collection."));
        return false;
    }

    // Set anomaly categories
    if (!pProps->SetValue(ccEraser::IAnomaly::Categories, pCategories))
    {
        CCTRACEE(_T("CScanManager::SetAnomalyValuesFromInfection() - Failed to set the anomaly categories."));
        return false;
    }

    // Add categories
    if ( pSrcCategories )
    {
        for(unsigned long i = 0; i < nCategories; i++ )
        {
            ccEraser::IAnomaly::Anomaly_Category iCategory = 
                                                     ccEraser::IAnomaly::Viral;

            switch( pSrcCategories[i] )
            {
            case SCAN_THREAT_TYPE_VIRUS:                
                iCategory = ccEraser::IAnomaly::Viral; 
                break;

            case SCAN_THREAT_TYPE_MALICIOUS:
                iCategory = ccEraser::IAnomaly::Malicious; 
                break;

            case SCAN_THREAT_TYPE_RESERVED_MALICIOUS:   
                iCategory = ccEraser::IAnomaly::ReservedMalicious; 
                break;

            case SCAN_THREAT_TYPE_HEURISTIC:            
                iCategory = ccEraser::IAnomaly::Heuristic; 
                break;

            case SCAN_THREAT_TYPE_SECURITY_RISK:        
                iCategory = ccEraser::IAnomaly::SecurityRisk; 
                break;

            case SCAN_THREAT_TYPE_HACKTOOL:
                iCategory = ccEraser::IAnomaly::Hacktool; 
                break;

            case SCAN_THREAT_TYPE_SPYWARE: 
                iCategory = ccEraser::IAnomaly::SpyWare; 
                break;
                
            case SCAN_THREAT_TYPE_TRACKWARE: 
                iCategory = ccEraser::IAnomaly::Trackware; 
                break;

            case SCAN_THREAT_TYPE_DIALER: 
                iCategory = ccEraser::IAnomaly::Dialer; 
                break;

            case SCAN_THREAT_TYPE_REMOTE_ACCESS:        
                iCategory = ccEraser::IAnomaly::RemoteAccess; 
                break;

            case SCAN_THREAT_TYPE_ADWARE:
                iCategory = ccEraser::IAnomaly::Adware; 
                break;

            case SCAN_THREAT_TYPE_JOKE: 
                iCategory = ccEraser::IAnomaly::Joke; 
                break;

            case SCAN_THREAT_TYPE_UNKNOWN:
            default:
                continue;
            }

            if (!pCategories->AddValue(DWORD(iCategory)))
            {
                CCTRACEE(_T("CScanManager::SetAnomalyValuesFromInfection() - Failed to add the %d category."), i);
                return false;
            }
        }
    }

    CCTRACEI(_T("CScanManager::SetAnomalyValuesFromInfection() - Setting the auto created flag to mark this as not-specific."));

    // Set the auto-created flag since this is not specific
    if ( !pProps->SetValue(ccEraser::IAnomaly::AutoCreated, true) )
    {
        CCTRACEE(_T("CScanManager::SetAnomalyValuesFromInfection() - Failed to set the auto created flag."));
    }

    // Generate a GUID for this anomaly
    GUID guid;
    HRESULT hr = S_OK;

    if (FAILED(hr = CoCreateGuid(&guid)))
    {
        CCTRACEE(_T("CScanManager::SetAnomalyValuesFromInfection() - CoCreateGuid() failed with 0x%X"), hr);
    }
    else
    {
        wchar_t wcharStr[MAX_PATH] = {0};

        if( 0 == StringFromGUID2(guid, wcharStr, MAX_PATH ) )
        {
            CCTRACEE(_T("CScanManager::SetAnomalyValuesFromInfection() - Failed to convert the GUID to a wide string."));
        }
        else
        {
            cc::IStringPtr pGUID;
            pGUID.Attach(ccSym::CStringImpl::CreateStringImpl(wcharStr));

            if (!pGUID)
            {
                CCTRACEE(_T("CScanManager::SetAnomalyValuesFromInfection() - Failed to create an IString for the GUID."));
            }
            else if (!pProps->SetValue(ccEraser::IAnomaly::ID, pGUID))
            {
                CCTRACEE(_T("CScanManager::SetAnomalyValuesFromInfection() - Failed to store the generated anomaly ID."));
            }
        }
    }

    return true;
}

void CScanManager::RemoveFromCompressedMap(IScanInfection* pInfection)
{
    if ( pInfection == NULL )
    {
        CCTRACEE(_T("CScanManager::RemoveFromCompressedMap() - infection is null."));
        return;
    }

    IScanCompressedFileInfection2QIPtr pCompressed = pInfection;

    if ( pCompressed.m_p != NULL )
    {
        // Get the container name
        CAtlString strContainerName = 
              pCompressed->GetComponent(pCompressed->GetComponentCount() - 1);
        strContainerName.MakeLower();

        // Get the vector of infections for this container
        COMPRESSED_ITEM_MAP::iterator Iter = 
                                  m_mapCompressedItems.find(strContainerName);

        if ( Iter != m_mapCompressedItems.end() )
        {
            // Find this compressed item in the vector and remove it
            for ( COMPRESSED_ITEM_VECTOR::iterator vecIter = 
                                                        (*Iter).second.begin();
                  vecIter != (*Iter).second.end(); vecIter++ )
            {
                if ( (*vecIter) == pCompressed )
                {
                    (*Iter).second.erase(vecIter);
                    CCTRACEI(_T("CScanManager::RemoveFromCompressedMap() - Removed the compressed item from the vector for container %s. Now %d items are in the vector"), (LPCTSTR)strContainerName, (*Iter).second.size());
                    break;
                }
            }
        }
        else
        {
            CCTRACEE(_T("CScanManager::RemoveFromCompressedMap() - Did not find container %s in the map."), (LPCTSTR)strContainerName);
        }
    }
    else
    {
        CCTRACEE(_T("CScanManager::RemoveFromCompressedMap() - Could not get a compressed infection to remove from the map."));
    }
}

void CScanManager::NotifyAP(IScanInfection* pInfection)
{
    // Only bother for non-viral items, the notification class will take care of
    // the rest
    if ( !IsNonViral(pInfection) )
    {
        return;
    }

    m_NotifyAPSpywareDetection.Notify(pInfection);
}

SCANSTATUS CScanManager::HandlePauseAbort(LPCTSTR pcszFuncName)
{
    // Do we need to pause the scan?
    if (m_bPauseScan == true)
    {
        CCTRACEI(_T("CScanManager::HandlePauseAbort() - %s Pausing the scan."),
                 pcszFuncName);

        //
        // Must use MsgWaitForMultipleObjects instead of WaitForSingleObject
        // in case any messages come in that need to be pumped before the
        // event is signaled.  If that happens, WaitForSingleObject would be
        // subject to possible deadlock.
        //
        DWORD dwWaitResult = WAIT_TIMEOUT;

        while (WAIT_OBJECT_0 != dwWaitResult)
        {
            dwWaitResult = MsgWaitForMultipleObjects( 1, &m_hUIResumeEvent, 
                                                      FALSE, INFINITE, 
                                                      QS_ALLINPUT );

            if ( WAIT_OBJECT_0 + 1 == dwWaitResult) 
            { 
                //
                // Woke up because there's a message to be processed.
                //
                MSG msg;

                while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
                {
                    //
                    // Any message that comes in shouldn't be a user a
                    // user input message, so need to call
                    // TranslateMessage before dispatching.
                    //
                    DispatchMessage(&msg);
                }
            }
        }

        CCTRACEI(_T("CScanManager::HandlePauseAbort() - %s Resuming the scan."),
                 pcszFuncName);
    }

    //  Do we need to abort the scan?
    if (m_bAbortScan == true)
    {
        CCTRACEI(_T("CScanManager::HandlePauseAbort() - %s scan aborted returning SCAN_ABORT."),
                 pcszFuncName);
        return SCAN_ABORT;
    }

    return SCAN_OK;
}

//
// Anomaly Context class
//
CScanManager::CAnomalyContext::CAnomalyContext()
{
    m_bQuarantineBackup = false;
    m_ulQuarItemStatus = QFILE_STATUS_BACKUP_FILE;
    m_pEZAnomaly = NULL;
}

void CScanManager::EnsurePathExists(LPCTSTR szFullPath)
{
    CString cszDirectoryName = szFullPath;
    INT iCurPos = NULL, iDirNameSize = cszDirectoryName.GetLength() + 1;
    CString cszTemp, cszNewPath = cszDirectoryName.Tokenize("\\/", iCurPos);
    LPTSTR szNewPath = cszNewPath.GetBuffer(iDirNameSize);
    cszTemp = cszDirectoryName.Tokenize("\\", iCurPos);
    while(!cszTemp.IsEmpty())
    {
        PathAppend(szNewPath, cszTemp);
        if(-1 == GetFileAttributes(szNewPath))
        {
            CreateDirectory(szNewPath, NULL);
            if(-1 == GetFileAttributes(szNewPath))
                break;
        }

        cszTemp = cszDirectoryName.Tokenize("\\/", iCurPos);
    }

    szNewPath = NULL;
    cszNewPath.ReleaseBuffer();

    return;
}
