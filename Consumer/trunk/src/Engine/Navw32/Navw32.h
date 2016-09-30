////////////////////////
//
// PROPRIETARY / CONFIDENTIAL.
// Use of this product is subject to license terms.
// Copyright © 2006 Symantec Corporation.
// All rights reserved.
//
/////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef NAVW32_H
#define NAVW32_H

#include <NavwInterface.h>
#include <AVInterfaceLoader.h>
#include <ScanUILoader.h>
#include <vector>
#include <AvProdSvcClientLoader.h>

class COverridableOption
{
public:
    enum eOptionState
    {
        eState_NotSet = 0,
        eState_Enabled,
        eState_EnabledIfGlobal,
        eState_Disabled,
        eState_DisabledIfGlobal
    };

    COverridableOption() : m_eState(eState_NotSet)
    {
    }

    eOptionState GetState(bool bGlobal)
    {
        switch(m_eState)
        {
            case eState_NotSet:
            case eState_Enabled:
            case eState_Disabled:
            {
                return m_eState;
            }
            case eState_EnabledIfGlobal:
            {
                if(bGlobal)
                    return eState_Enabled;
                break;
            }
            case eState_DisabledIfGlobal:
            {
                if(bGlobal)
                    return eState_Disabled;
                break;
            }
        }

        // Default to NotSet
        return eState_NotSet;
    }

    void SetState(eOptionState eState)
    {
        // Never downgrade to IfGlobal or NotSet...
        switch(eState)
        {
            case eState_NotSet:
            {
                // We can never be set to NotSet!
                break;
            }
            case eState_Enabled:
            case eState_Disabled:
            {
                // We can always go to enabled or disabled
                m_eState = eState;
                break;
            }
            case eState_EnabledIfGlobal:
            {
                // We can only switch between IfGlobal states or from NotSet
                if(m_eState == eState_NotSet || m_eState == eState_DisabledIfGlobal)
                    m_eState = eState_EnabledIfGlobal;
                break;
            }
            case eState_DisabledIfGlobal:
            {
                // We can only switch between IfGlobal states or from NotSet
                if(m_eState == eState_NotSet || m_eState == eState_EnabledIfGlobal)
                    m_eState = eState_DisabledIfGlobal;
                break;
            }
        }
    }

protected:
    eOptionState m_eState;
};

class CNavw32
{
public:
	// Constructor.
	CNavw32();

	// Destructor.
	~CNavw32();

	// Run the On-Demand Scanner.
	// Returns SMRESULT_OK if successful, otherwise the appropriate error.
    avScanUI::eScanResult Go(NAVWRESULT& NavwResult);

	// Formats and displays an error message using NAVErrorDisplay.
	void NAVErrorDisplay(UINT uStringID, const TCHAR* szItemName);

	void SetSubscriptionCOHOverride(bool bSubOverride);
	bool HasAdminPrivilege(bool bPromptForElevation);

private:
    // Add a target to the target list
    bool AddTarget(LPCWSTR pszTarget);

	// Process a list of files from a task list
	bool ProcessTaskFile(const TCHAR* pszTaskFilePath, AVModule::IAVMapStrData* pOptions);

	// Process a specific switch.
	bool ProcessSwitch(const wchar_t* szSwitch, AVModule::IAVMapStrData* pOptions);

	// Adds all drives except A: and B: to the scan item list.
	void FillWithDrives();

	// Adds files (no folders) from a specific directory to the scan list.
	bool FillWithFiles(const TCHAR* szDirectory);

	// Helper function used to determine if a disk is present.
	bool DiskExists(const TCHAR* szRootDir);

	// Should we log scan start/stops?
	bool ReadSettings(AVModule::IAVMapStrData* pOptions);

    // Read a response mode setting
    bool GetResponseMode(AVModule::IAVMapStrData* pOptions, LPCTSTR pszOption);

	// Helper function used to check if an item (file/folder/drive) exists.
	bool CheckItem(const wchar_t* szItem);

	// Checks to see if the specified item is a drive.
	bool IsItemADrive(const TCHAR* szItem);

	// Updates the last run time for task files.
	void UpdateTaskTimes();

	// Returns true if the item has wildcard characters (? or *).
	bool IsWildcardItem(const wchar_t* szItem);

	// Processes a wildcard item.
	bool ProcessWildcardItem(const wchar_t* szItem);
	
	// Extracts the directory from a wildcard item.
	bool ExtractWildcardDir(const wchar_t* szItem, wchar_t* szDirectory);

	// Searches the command-line arguments for temporary task files.
	// These task files are then deleted.
	void DeleteTempTaskFiles();

    // Single Instance Check
    bool IsAlreadyRunning(LPCSTR szName, bool bSwitch);

    // Something to do check
    bool ScanHasSomethingToDo(AVModule::IAVMapStrData* pTargets, AVModule::IAVMapStrData* pOptions);

private:
    // UI Loader
    avScanUI::sui_IScanUI m_suiLoader;

    // AVComponent loaders
    AVModule::AVLoader_IAVStandAlone m_avSafeModeLoader;
    AVModule::IAVStandAlonePtr m_spStandAlone;

    AVModule::AVLoader_IAVGlobalScanner m_avGSLoader;
    AVModule::AVLoader_IAVContextScanner m_avCtxLoader;
    AVModule::AVLoader_IAVMapDwordData m_avMapDwordDataLoader;

    // AVComponent objects
    AVModule::IAVScanBasePtr m_spScanner;

	// Non-temporary tasks files we're processing.
    std::vector<std::wstring> m_TaskFiles;

    // True if we are using the single instance global scan, false is context scan
    bool m_bGlobal;

	// A flag to know if we are performing an Office AV Scan
	bool m_bIsOfficeScan;

    // A flag to know if we are performing an IM scan
    bool m_bIsIMScan;

    // A flag to know if we should log scan starts and stops
    bool m_bLogScanStartStop;

    // Is another instance already running
    bool m_bAlreadyRunning;

    // Do we need to tell the scanui to "ignore once" low risk threats?
    bool m_bIgnoreOnceLow;

    // Are we scanning any network drives?
    bool m_bScanningNetwork;

    // Special overridable options
    COverridableOption m_oOptEraser;
    COverridableOption m_oOptStealth;

    // UI Mode
    avScanUI::eUIMode m_eUIMode;

	// Name of the Current Scan for logging and UI
    ATL::CAtlString m_strScanName;

    // SingleInstance interface
    avScanUI::sui_ISingleInstance m_loaderSingleInstance;
    avScanUI::ISingleInstancePtr m_spSingleInstance;

	bool m_bSubscriptionCOHOverride;
	// Administrator Privilieges 
	DWORD m_dwHasAdminPrivilege;
};

class CSuppressAvProdSvcRebootDialogSwitch
{
public: // c'tor / d'tor
    CSuppressAvProdSvcRebootDialogSwitch() : m_bInitialized(FALSE) { Lock(); return; };
    ~CSuppressAvProdSvcRebootDialogSwitch() { Unlock(); return; };

public: // methods
    // Suppress the AvProdService's reboot dialog
    void Lock()
    {
        AvProdSvcClientLoader.CreateObject(m_spAvProdServiceClient);
        if(!m_spAvProdServiceClient)
        {
            CCTRACEE( CCTRCTX _T("Unable to create AvProdSvcClient object."));
            return;
        }

        // initialize the AvProdServiceClient object
        HRESULT hr = m_spAvProdServiceClient->Initialize();
        if(FAILED(hr))
        {
            CCTRACEE( CCTRCTX _T("Unable to initialize AvProdSvcClient. hr = 0x%08X"), hr);
            return;
        }
        
        // remember that we successfully init'd
        m_bInitialized = TRUE;
        
        // call into AvProdSvc and disable reboot dialog
        m_spAvProdServiceClient->DisableRebootDialog();
    }
    
    // Restore the AvProdService's reboot dialog
    void Unlock()
    {
        // make sure we got init'd properly
        if(!m_spAvProdServiceClient || !m_bInitialized)
        {
            CCTRACEE( CCTRCTX _T("AvProdSvcClient has not been initialized!"));
            return;
        }

        // restore the reboot dialog, do not allow pending operations, do not force reboot dialog
        m_spAvProdServiceClient->EnableRebootDialog(NULL, false, false);
    }

protected: // members
    AvProdSvc::AvProdSvcClient_Loader AvProdSvcClientLoader;
    AvProdSvc::IAvProdSvcClientPtr m_spAvProdServiceClient;
    BOOL m_bInitialized;
};

#endif

