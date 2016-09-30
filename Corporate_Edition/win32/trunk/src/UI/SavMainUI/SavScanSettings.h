#ifndef SAVMAINUI_SAVSCAN
#define SAVMAINUI_SAVSCAN

#include "stdafx.h"
#include "ScanState.h"
#include <string>
#include <list>


// Represents the settings for a Symantec AntiVirus scan as read from the registry (persistent storage) at that time.
// Also provides a few minor utility functions, such as saving them back out and determining current status.
// This is intended as a "smart wrapper" over the persisted settings, although integrity checking is not done in favor of
// accurately reflecting the persisted settings.
//
// To use:
//  1.  Enumerate through one of the lists of scans
//  2.  Call ReadFromRegistryKey, specifying the path to the key enumerated and the scan key to read in
//  3.  Make changes if desired and call WriteToRegistryKey to save out changes.
//
// Notes:
// 1.  This code is written to be compileable in all SAV projects, which introduces 2
//     restrictions:
//          a.  Functions from RTVScan's MISC.CPP cannot be used
//          b.  Must be TCHAR aware and work in both MBCS and UNICODE
class CSavScanSettings
{
public:
    // ** DATA TYPES **
    typedef enum {ScheduledFrequency_OnDemand, ScheduledFrequency_Daily, ScheduledFrequency_Weekly, ScheduledFrequency_Monthly} ScheduledFrequency;
    typedef enum {ScheduledDay_Sunday, ScheduledDay_Monday, ScheduledDay_Tuesday, ScheduledDay_Wednesday, ScheduledDay_Thursday, ScheduledDay_Friday, ScheduledDay_Saturday} ScheduledDay;
    #ifdef _UNICODE
        typedef std::wstring tstring;
    #else
        typedef std::string tstring;
    #endif
    typedef std::list<tstring> TstringList;
    // Action information for PVID or category-based exceptions.  Both use DWORDs as IDs, so this work fine for either.
    struct IdActionInfo
    {
        DWORD   id;
        DWORD   primaryAction;
        DWORD   secondaryAction;
    };
    typedef std::list<IdActionInfo> IdActionInfoList;
    typedef std::list<CSavScanSettings> SavScanSettingsList;
    
    // ** CONSTANTS **
    static const SYSTEMTIME SYSTEMTIME_NULL;
    static const time_t TIME_NULL;
    
    // ** DATA MEMBERS **
    // What to scan
    bool                    scanAllDrives;              // szReg_Val_ScanAllDrives (same as ScanAllBootRecords)
    TstringList             directoriesToScan;          // Directories key
    bool                    scanRegardlessOfExtension;  // Same as scan all.  szReg_Val_FileTypes
    TstringList             extensionsToScan;           // If !scanRegardlessOfExtension, the list of extensions to scan.  szReg_Val_Extensions
    TstringList             filesToScan;                // szReg_Key_Files
    bool                    scanMemory;                 // szReg_Val_ScanProcesses
    bool                    scanLoadPoints;             // szReg_Val_ScanLoadpoints
    bool                    scanSideEffects;            // szReg_Val_ScanERASERDefs
    bool                    scanProcessHeuristic;       // COH Process scan
    bool                    scanGreyware;               // Normally always on, very unlikely this will ever be off
    bool                    excludeByExtensions;
    bool                    scanAlternateDataStreams;   // szReg_Val_ScanADS
    bool                    scanContainers;             // szReg_Val_ScanZipFile
    DWORD                   containerMaxDepth;          // szReg_Val_ZipDepth
    TstringList             excludeExtensions;          // szReg_Val_ExcludedExtensions
    TstringList             excludeDirectories;         // NoScanDir key
    TstringList             excludeFiles;               // FileExceptions key

    // Actions
    DWORD                   virusPrimaryAction;         // void CScanPropertiesAdapter::SetScanActionProperties(HKEY hScanOptionsKey, CSavScanPropertiesEx& oScanPropertiesEx)
    DWORD                   virusSecondaryAction;
    DWORD                   macroVirusPrimaryAction;
    DWORD                   macroVirusSecondaryAction;
    DWORD                   riskPrimaryAction;
    DWORD                   riskSecondaryAction;
    DWORD                   stopServiceAction;          // szReg_Val_ScanNotifyTerminateProcess.  SCAN_NOTIFY_ constant
    DWORD                   terminateProcessAction;     // szReg_Val_ScanNotifyStopServices.  SCAN_NOTIFY_ constant
    IdActionInfoList        actionsByPVID;              // PVID-specific action configurations (Expanded\PVID-<PVID ID>
    IdActionInfoList        actionsByThreatCategory;    // Threat category default actions (Expanded\\TCID-<Threat CatID>
    
    // Scanning behavior
    bool                    backupToQuarantine;
    bool                    doReverseLookups;
    bool                    auditScanResults;
    bool                    preserveLastAccessTime;
    
    // Scheduled scan settings
    bool                    scheduledEnabled;
    ScheduledFrequency      scheduledRunFrequency;
    DWORD                   scheduledMinuteOfDay;
    ScheduledDay            scheduledDayOfWeek;
    DWORD                   scheduledDayOfMonth;
    bool                    scheduledRunMissedEvents;
    bool                    scheduledAllowMissedScansOutsideWindow;
    bool                    scheduledAllowRunWhenLoggedOut;
    DWORD                   scheduledMissedScanWindowTime;              // Seconds
    bool                    scheduledRunNow;
    time_t                  scheduledWhenDelayed;
    DWORD                   scheduledDelayDuration;                     // Minutes
    // Scheduled scan settings - resumeable scans
    bool                    scheduledIsResumeable;
    DWORD                   scheduledScanWindowSize;                    // szReg_Val_ScanDuration
    // skipEvent unused, always 0 in reg

    // Miscellaneous
    GUID                    id;
    time_t                  creationTime;
    time_t                  lastStartTime;
    tstring                 userAssignedName;
    tstring                 userAssignedDescription;    // UI only.
    DWORD                   customScanType;             // szReg_Val_SelectedScanType, SCAN_TYPE constant.  UI only.
    bool                    displayStatusDialog;
    tstring                 statusDialogTitle;
    bool                    closeWhenDone;
    bool                    allowUserToStopScan;
    bool                    showScanStatusDialog;

    // STORAGE MIGRATION OPTIONS!!

    // ** FUNCTION MEMBERS **
    // * Persistence and state
    // Reads the data for this scan in from the registry
    HRESULT ReadFromRegistryKey( HKEY parentKeyHandle, LPCTSTR readParentKeyPath, LPCTSTR readScanKeyName );
    // Save this scan configuration out to a key named for the GUID of this scan under the specified key.
    // Any existing data will be overwritten.
    HRESULT WriteToRegistryKey( LPCTSTR parentPath );
    // Same as above, but uses the parent path specified by in the last call to ReadFromRegistryKeyPath
    // Returns S_OK on success, E_ABORT if no previous call to ReadFromRegistryKeyPath
    HRESULT WriteToRegistryKey();
    // Clears all data, resets object to same state as if just instantiated
    HRESULT Clear();
    // Adds this scan to the StartupScans list so it will be run at startup
    HRESULT MakeStartupScan( void );
    // Deletes this scan from persistent storage.
    HRESULT DeleteScan( void );

    // * Utility informative
    // Returns TRUE if this is a scheduled scan, else FALSE
    bool IsScheduled();
    // Sets *parentPath equal to the full path to the parent key
    HRESULT GetParentPath( std::wstring* parentPath );
    // Sets *fullPath equal to the full pathname to the key for this scan
    HRESULT GetFullPath( std::wstring* fullPath );
    // Returns a duplicate handle for the key for this scan.  The caller then owns this handle
    // and must close it.
    HKEY GetKeyHandle( void );
    // Sets *idAsString equal to the GUID ID for this scan converted to a string
    HRESULT GetIDString( std::wstring* idAsString );
    // Returns the current status of this scan
    DWORD GetCurrentStatus( void );
    // Returns TRUE if this scan is marked to run at StartUp, else FALSE
    HRESULT IsStartup( bool* isStartup );

    // * Static utility functions
    HRESULT GetCurrentUserStartupScans( SavScanSettingsList* scans );
    HRESULT GetAdminScans( SavScanSettingsList* scans );
    HRESULT GetCurrentUserScans( SavScanSettingsList* scans );
    // Later implement GetUserScans( SID* userSID, SavScanSettingsList* scans ) and GetUsrStartupScans as same

    // Constructor-destructor
    CSavScanSettings();
    ~CSavScanSettings();
private:
    // ** FUNCTION MEMBERS **
    // * Internals - caller should verify parameters
    HRESULT ReadValueBool( HKEY keyHandle, LPCTSTR valueName, bool* valueData, bool defaultValue = false );
    HRESULT ReadValueDWORD( HKEY keyHandle, LPCTSTR valueName, DWORD* resultValue, DWORD defaultValue = 0 );
    HRESULT ReadValueString( HKEY keyHandle, LPCTSTR valueName, tstring* resultValue, LPCTSTR defaultValue = NULL );
    HRESULT ReadValueTime( HKEY keyHandle, LPCTSTR valueName, time_t* resultValue, time_t defaultValue = TIME_NULL );
    
    // ** DATA MEMBERS **
    tstring                 parentPath;                 // Full pathname of the parent key where this scan's data is stored
    tstring                 scanKeyName;                // Name of the key within parentPat representing this scan
    HKEY                    keyHandle;
    HKEY                    parentKeyHandle;            // Handle to the parent key
};

#endif // SAVMAINUI_SAVSCAN