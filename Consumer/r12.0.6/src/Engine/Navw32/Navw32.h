
#ifndef NAVW32_H
#define NAVW32_H

#include "ScanMgrInterface.h"
#include "NavwInterface.h"
#include "AutoProtectWrapper.h"
#include "AVScanManagerLoader.h"

#include <vector>
using std::vector;

class CNavw32
{
public:
	// Constructor.
	CNavw32();

	// Destructor.
	~CNavw32();

	// Run the On-Demand Scanner.
	// Returns SMRESULT_OK if successfull, otherwise the appropriate error.
	SMRESULT Go(NAVWRESULT &NavwResult);

    // Process remediations after a reboot
    void ProcessRebootRemediations();

	// Formats and displays an error message using NAVErrorDisplay.
	void NAVErrorDisplay(UINT uStringID, const char* szItemName);

private:
	// Process a list of files from a task list
	bool ProcessTaskFile(const char* pszTaskFilePath);

	// Process a specific switch.
	bool ProcessSwitch(const char* szSwitch);

	// Adds all drives except A: and B: to the scan item list.
	void FillWithDrives();

	// Adds files (no folders) from a specific directory to the scan list.
	bool FillWithFiles(const char* szDirectory);

	// Helper function used to determine if a disk is present.
	bool DiskExists(const char* szRootDir);

	// Should we log scan start/stops?
	bool ReadSettings();

	// Helper function used to check if an item (file/folder/drive) exists.
	bool CheckItem(const char* szItem);

	// Checks to see if the specified item is a drive.
	bool IsItemADrive(const char* szItem);

	// Updates the last run time for task files.
	void UpdateTaskTimes();

	// Returns true if the item has wildcard characters (? or *).
	bool IsWildcardItem(const char* szItem);

	// Processes a wildcard item.
	bool ProcessWildcardItem(const char* szItem);
	
	// Extracts the directory from a wildcard item.
	bool ExtractWildcardDir(const char* szItem, char* szDirectory);

	// Searches the command-line arguments for temporary task files.
	// These task files are then deleted.
	void DeleteTempTaskFiles();

    // Single Instance Check
    bool IsAlreadyRunning(LPCSTR szName, bool bSwitch);

private:

    // Scan manager loader.
    AV::AVScanManager_IScanManagerFactory m_ScanManagerLoader;

	// Scan manager.
	IScanManager* m_pScanManager;

	// Non-temporary tasks files we're processing.
	vector<string> m_TaskFiles;

	// True only if user did not specify a exclusive scan option.
	bool m_bShouldAddItems;

	// A flag to know if we are performing an Office AV Scan
	bool m_bIsOfficeScan;

    // A flag to know if we should log scan starts and stops
    bool m_bLogScanStartStop;

    // A flag to know if we should be performing a side-effect scan
    bool m_bEnableEraserScan;

	// Auto-Protect class
	CAutoProtectWrapper m_APWrapper;

	// Name of the Current Scan for logging and UI
    ATL::CAtlString m_strScanName;

    // SingleInstance interface
    AV::AVScanManager_ISingleInstanceFactory m_SingleInstanceFactory;
    ISingleInstancePtr m_spSingleInstance;

};

#endif

