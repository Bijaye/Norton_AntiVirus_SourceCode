#include "stdafx.h"
#include "SavScanSettings.h"
#include "SymSaferRegistry.h"
#include "SymSaferStrings.h"
#include "pscan.h"


// ** CONSTANTS **
const SYSTEMTIME CSavScanSettings::SYSTEMTIME_NULL = {0,0,0,0,0,0,0,0};
const time_t CSavScanSettings::TIME_NULL = 0;


// ** FUNCTION MEMBERS **
// * Persistence and state
HRESULT CSavScanSettings::ReadFromRegistryKey( HKEY parentKeyHandle, LPCTSTR readParentKeyPath, LPCTSTR readScanKeyName )
// Reads the data for this scan in from the registry
{
    _bstr_t     stringConverter;
    TCHAR       stringBuffer[2*MAX_PATH]    = _T("");
    DWORD       stringBufferSize            = 0;
    DWORD       valueType                   = REG_DWORD;
    DWORD       returnValDW                 = ERROR_OUT_OF_PAPER;
    DWORD       tempDWORD                   = 0;
    HRESULT     returnValHR                 = E_FAIL;

    // Validate parameters
    if (parentKeyHandle == NULL)
        return E_INVALIDARG;
    if (readParentKeyPath == NULL)
        return E_POINTER;
    if (readScanKeyName == NULL)
        return E_POINTER;

    try
    {
        parentPath = readParentKeyPath;
        scanKeyName = readScanKeyName;
        stringConverter = readScanKeyName;
        CLSIDFromString(stringConverter, &id);

        returnValDW = RegOpenKeyEx(parentKeyHandle, scanKeyName.c_str(), NULL, KEY_READ, &keyHandle);
        if (returnValDW == ERROR_SUCCESS)
        {
            // What to scan
            ReadValueBool(keyHandle, _T(szReg_Val_ScanAllDrives), &scanAllDrives);
            ReadValueBool(keyHandle, _T(szReg_Val_FileTypes), &scanRegardlessOfExtension);
            ReadValueBool(keyHandle, _T(szReg_Val_ScanProcesses), &scanMemory);
            ReadValueBool(keyHandle, _T(szReg_Val_ScanLoadpoints), &scanLoadPoints);
            ReadValueBool(keyHandle, _T(szReg_Val_ScanERASERDefs), &scanSideEffects);
            ReadValueBool(keyHandle, _T(szReg_Val_ScanHeuristicCOH), &scanProcessHeuristic);
            ReadValueBool(keyHandle, _T(szReg_Val_ScanForGreyware), &scanGreyware, true);
            ReadValueBool(keyHandle, _T(szReg_Val_ExcludedByExtensions), &excludeByExtensions);
            ReadValueBool(keyHandle, _T(szReg_Val_ScanADS), &scanAlternateDataStreams);
            ReadValueBool(keyHandle, _T(szReg_Val_ScanZipFile), &scanContainers);
            ReadValueDWORD(keyHandle, _T(szReg_Val_ZipDepth), &containerMaxDepth);

            // Actions
            ReadValueDWORD(keyHandle, _T(szReg_Val_First_Action), &virusPrimaryAction);
            ReadValueDWORD(keyHandle, _T(szReg_Val_Second_Action), &virusSecondaryAction);
            ReadValueDWORD(keyHandle, _T(szReg_Val_Macro_First), &macroVirusPrimaryAction);
            ReadValueDWORD(keyHandle, _T(szReg_Val_Macro_Second), &macroVirusSecondaryAction);
            ReadValueDWORD(keyHandle, _T(szReg_Key_PVID_Template) _T("\\") _T(szReg_Val_First_Action), &riskPrimaryAction);
            ReadValueDWORD(keyHandle, _T(szReg_Key_PVID_Template) _T("\\") _T(szReg_Val_Second_Action), &riskSecondaryAction);
            ReadValueDWORD(keyHandle, _T(szReg_Val_ScanNotifyTerminateProcess), &stopServiceAction);
            ReadValueDWORD(keyHandle, _T(szReg_Val_ScanNotifyStopServices), &terminateProcessAction);

            // Scanning behavior
            ReadValueBool(keyHandle, _T(szReg_Val_BackupToQuarantine), &backupToQuarantine);
            ReadValueBool(keyHandle, _T(szReg_Val_ReverseLookup), &doReverseLookups);
            ReadValueBool(keyHandle, _T(szReg_Val_AuditScanResults), &auditScanResults);
            ReadValueBool(keyHandle, _T(szReg_Val_PreserveTimeStamp), &preserveLastAccessTime);

            // Scheduled scan settings
            ReadValueBool(keyHandle, _T(szReg_Val_Enabled), &scheduledEnabled);
            ReadValueDWORD(keyHandle, _T(szReg_Val_HowOften), &tempDWORD);
            switch (tempDWORD)
            {
            case S_DAILY:
                scheduledRunFrequency = ScheduledFrequency_Daily;
                break;
            case S_WEEKLY:
                scheduledRunFrequency = ScheduledFrequency_Weekly;
                break;
            case S_MONTHLY:
                scheduledRunFrequency = ScheduledFrequency_Monthly;
                break;
            default:
                scheduledRunFrequency = ScheduledFrequency_OnDemand;
                break;
            }
            ReadValueDWORD(keyHandle, _T(szReg_Val_MinOfDay), &scheduledMinuteOfDay);
            ReadValueDWORD(keyHandle, _T(szReg_Val_DayOfMonth), &scheduledDayOfMonth);
            ReadValueDWORD(keyHandle, _T(szReg_Val_DayOfWeek), &tempDWORD);
            scheduledDayOfWeek = static_cast<ScheduledDay>(tempDWORD);
            ReadValueBool(keyHandle, _T(szReg_Val_MissedEvent_Enabled), &scheduledRunMissedEvents);
            ReadValueBool(keyHandle, _T(szReg_Val_AllowMissedScansOutsideScanWindow), &scheduledAllowMissedScansOutsideWindow);
            ReadValueBool(keyHandle, _T(szReg_Val_RunLoggedOutUsersScheduledScans), &scheduledAllowRunWhenLoggedOut);
	        switch (scheduledRunFrequency)
	        {
            case ScheduledFrequency_Daily:
                ReadValueDWORD(keyHandle, _T(szReg_Val_TimeWindow_Daily), &scheduledMissedScanWindowTime);
                break;
            case ScheduledFrequency_Weekly:
                ReadValueDWORD(keyHandle, _T(szReg_Val_TimeWindow_Weekly), &scheduledMissedScanWindowTime);
    	        break;
            case ScheduledFrequency_Monthly:
                ReadValueDWORD(keyHandle, _T(szReg_Val_TimeWindow_Monthly), &scheduledMissedScanWindowTime);
		        break;
            default:
                scheduledMissedScanWindowTime = 0;
		        break;
	        }
            ReadValueBool(keyHandle, _T(szReg_Val_RunNow), &scheduledRunNow);
            ReadValueTime(keyHandle, _T("LastDelayedTime"), &scheduledWhenDelayed);
            ReadValueDWORD(keyHandle, _T("ScanDelay"), &scheduledDelayDuration);
            ReadValueDWORD(keyHandle, _T(szReg_Val_ScanDuration), &scheduledScanWindowSize);
            scheduledIsResumeable = (scheduledScanWindowSize != 0);

            // Miscellaneous
            ReadValueTime(keyHandle, _T(szReg_Val_Schedule_Created), &creationTime);
            ReadValueTime(keyHandle, _T(szReg_Val_Schedule_LastStart), &lastStartTime);
            ReadValueDWORD(keyHandle, _T(szReg_Val_SelectedScanType), &customScanType);
            ReadValueString(keyHandle, _T(szReg_Value_ScanTitle), &statusDialogTitle);
            
            ReadValueBool(keyHandle, _T(szReg_Val_CloseScan), &closeWhenDone);
            ReadValueBool(keyHandle, _T(szReg_Value_ScanLocked), &allowUserToStopScan);
            ReadValueBool(keyHandle, _T(szReg_Value_DisplayStatusDlg), &showScanStatusDialog);



            // Datastore format for user scans:
            // <SAV Main HCU key>\Custom Tasks
            //      <Scan GUID>
            //          <All Scan options EXCEPT description>
            //          SelectedScanType = Full | Quick | Custom | COH
            //      TaskPadScripted
            //          <Scan User Readable Name>
            //              (Default) = <Scan GUID>
            //              ActiveX ID = HARDCODED VALUE
            //              Description = <Scan Description>
            // RTVScan determines Scheduled by this test:  (Schedule\MinOfDay == NO_SCHEDULED_START_TIME) && (!Scheduled\RandomizeDayEnabled && !Schedule\RandomizeWeekEnabled && !Schedule\RandomizeMonthEnabled))
            //      See misc.cpp's GetTimeValues for loading time values
            //
            // If this is a StartupScan, it is also listed here:
            //      TaskPadStartup
            //          <Scan Name>
            //              (Default) = <Scan GUID>
            //              ActiveX ID = HARDCODED VALUE
            //              Description = <Scan Description>
            //
            // If this is a ScheduledScan, it is also listed here:
            //      TaskPadScheduled
            //          <Scan Name>
            //              (Default) = <Scan GUID>
            //              ActiveX ID = HARDCODED VALUE
            //              Description = <Scan Description>
            //
            // Datastore format for admin scheduled scans:
            // <SAV main HLM key>\LocalScans
            //      <Scan GUID>
            //          <All Scan options...>
            //          Schedule
            //              RTVScan determines Scheduled by this test:  (Schedule\MinOfDay == NO_SCHEDULED_START_TIME) && (!Scheduled\RandomizeDayEnabled && !Schedule\RandomizeWeekEnabled && !Schedule\RandomizeMonthEnabled))
            //              See misc.cpp's GetTimeValues for loading time values
            //              Note that Type != 0 is also used in the UI for this
            // Note: SSC has no description UI for admin scheduled scans, only names
            RegCloseKey(keyHandle);
        }
        returnValHR = HRESULT_FROM_WIN32(returnValDW);
    }
    catch (std::bad_alloc&)
    {
        returnValHR = E_OUTOFMEMORY;
    }
    
    return returnValHR;
}

HRESULT CSavScanSettings::WriteToRegistryKey( LPCTSTR parentPath )
// Save this scan configuration out to a key named for the GUID of this scan under the specified key.
// Any existing data will be overwritten.
{
    return E_NOTIMPL;
}

HRESULT CSavScanSettings::WriteToRegistryKey()
// Same as above, but uses the parent path specified by in the last call to ReadFromRegistryKeyPath
// Returns S_OK on success, E_ABORT if no previous call to ReadFromRegistryKeyPath
{
    return E_NOTIMPL;
}

HRESULT CSavScanSettings::Clear()
// Clears all data, resets object to same state as if just instantiated
{
    //
    id                              = GUID_NULL;
    creationTime                    = TIME_NULL;
    lastStartTime                   = TIME_NULL;
    userAssignedName                = _T("");
    userAssignedDescription         = _T("");
    customScanType                  = 0;
    displayStatusDialog             = false;
    statusDialogTitle               = _T("");
    closeWhenDone                   = false;
    allowUserToStopScan             = false;
    showScanStatusDialog            = false;

    // Clear core members
    parentPath = _T("");
    scanKeyName = _T("");
    if (parentKeyHandle != NULL)
    {
        RegCloseKey(parentKeyHandle);
        parentKeyHandle = NULL;
    }
    if (keyHandle != NULL)
    {
        RegCloseKey(keyHandle);
        keyHandle = NULL;
    }
    return E_NOTIMPL;
}

HRESULT CSavScanSettings::MakeStartupScan( void )
// Adds this scan to the StartupScans list so it will be run at startup
{
    return E_NOTIMPL;
}

HRESULT CSavScanSettings::DeleteScan( void )
// Deletes this scan from persistent storage.
{
    return E_NOTIMPL;
}


// * Utility informative
bool CSavScanSettings::IsScheduled()
// Returns TRUE if this is a scheduled scan, else FALSE
{
    return false;
}

HRESULT CSavScanSettings::GetParentPath( std::wstring* parentPath )
// Sets *parentPath equal to the full path to the parent key
{
    return E_NOTIMPL;
}

HRESULT CSavScanSettings::GetFullPath( std::wstring* fullPath )
// Sets *fullPath equal to the full pathname to the key for this scan
{
    return E_NOTIMPL;
}

HKEY CSavScanSettings::GetKeyHandle( void )
// Returns a duplicate handle for the key for this scan.  The caller then owns this handle
// and must close it.
{
    return keyHandle;
}

HRESULT CSavScanSettings::GetIDString( std::wstring* idAsString )
// Sets *idAsString equal to the GUID ID for this scan converted to a string
{
    return E_NOTIMPL;
}

DWORD CSavScanSettings::GetCurrentStatus( void )
// Returns the current status of this scan
{
    return S_UNKNOWN;
}

HRESULT CSavScanSettings::IsStartup( bool* isStartup )
// Returns TRUE if this scan is marked to run at StartUp, else FALSE
{
    HKEY            startupKeyHandle                = NULL;
    TCHAR           currScanKeyName[MAX_PATH]       = _T("");
    DWORD           currScanNo                      = 0;
    HKEY            startupSubkeyHandle             = NULL;
    tstring         scanIDrawString;
    TCHAR           scanIDformattedString[43]       = _T("");
    GUID            currStartupScanID               = GUID_NULL;
    DWORD           returnValDW                     = ERROR_OUT_OF_PAPER;
    HRESULT         returnValHR                     = E_FAIL;

    // Validate parameter
    if (isStartup == NULL)
        return E_POINTER;
    // Validate state
    if (parentKeyHandle == NULL)
        return E_ABORT;

    // Search for a subkey in the TaskPadStartup key of the parent where the default value is equal to the ID
    // of this scan.
    returnValDW = RegOpenKeyEx(HKEY_LOCAL_MACHINE, _T(szReg_Value_Scripted), 0, KEY_ENUMERATE_SUB_KEYS | KEY_READ, &startupKeyHandle);
    if (returnValDW == ERROR_SUCCESS)
    {
        *isStartup = false;
        while ((RegEnumKey(keyHandle, currScanNo, currScanKeyName, sizeof(currScanKeyName)/sizeof(currScanKeyName[0])) == ERROR_SUCCESS) && (!(*isStartup)))
        {
            // Initialize
            currStartupScanID           = GUID_NULL;
            scanIDrawString[0]          = NULL;
            scanIDformattedString[0]    = NULL;

            returnValDW = RegOpenKeyEx(keyHandle, currScanKeyName, NULL, KEY_READ, &startupSubkeyHandle);
            if (returnValDW == ERROR_SUCCESS)
            {
                ReadValueString(startupSubkeyHandle, _T(""), &scanIDrawString);
                if (scanIDrawString != _T(""))
                {
                    sssnprintf(scanIDformattedString, sizeof(scanIDformattedString), _T("{%s}"), scanIDrawString.c_str());
                    CLSIDFromString(scanIDformattedString, &currStartupScanID);
                    if (currStartupScanID == id)
                        *isStartup = true;
                }
                RegCloseKey(startupSubkeyHandle);
                startupSubkeyHandle = NULL;
            }
            currScanNo += 1;
        }
        RegCloseKey(startupKeyHandle);
        startupKeyHandle = NULL;
        returnValHR = S_OK;
    }
    else
    {
        returnValHR = HRESULT_FROM_WIN32(returnValDW);
    }
    return returnValHR;
}


// * Static utility functions
HRESULT CSavScanSettings::GetCurrentUserStartupScans( SavScanSettingsList* scans )
{
    return E_NOTIMPL;
}

HRESULT CSavScanSettings::GetAdminScans( SavScanSettingsList* scans )
{
    TCHAR               currScanKeyName[MAX_PATH]       = _T("");
    DWORD               currScanNo                      = 0;
    HKEY                keyHandle                       = NULL;
    CSavScanSettings    newScan;
    DWORD               returnValDW                     = ERROR_OUT_OF_PAPER;
    HRESULT             returnValHR                     = E_FAIL;

    // Validate parameter
    if (scans == NULL)
        return E_POINTER;

    returnValDW = RegOpenKeyEx(HKEY_LOCAL_MACHINE, _T(szReg_Key_Main) _T(szReg_Key_LocalScans), 0, KEY_ENUMERATE_SUB_KEYS | KEY_READ, &keyHandle);
    if (returnValDW == ERROR_SUCCESS)
    {
        scans->clear();

        while (RegEnumKey(keyHandle, currScanNo, currScanKeyName, sizeof(currScanKeyName)/sizeof(currScanKeyName[0])) == ERROR_SUCCESS )
        {
            newScan.Clear();
            returnValHR = newScan.ReadFromRegistryKey(keyHandle, _T(""), currScanKeyName);
            if (SUCCEEDED(returnValHR))
                scans->push_back(newScan);
            currScanNo += 1;
        }
        returnValHR = S_OK;
        RegCloseKey(keyHandle);
        keyHandle = NULL;
    }
    else
    {
        returnValHR = HRESULT_FROM_WIN32(returnValDW);
    }

    return returnValHR;
}

HRESULT CSavScanSettings::GetCurrentUserScans( SavScanSettingsList* scans )
{
    return E_NOTIMPL;
}

    // Constructor-destructor
CSavScanSettings::CSavScanSettings() : scanAllDrives(false),
    scanRegardlessOfExtension(false),
    scanMemory(false),
    scanLoadPoints(false),
    scanSideEffects(false),
    scanProcessHeuristic(false),
    scanGreyware(false),
    excludeByExtensions(false),
    scanAlternateDataStreams(false),
    scanContainers(false),
    containerMaxDepth(0),
    virusPrimaryAction(AC_NOTHING),
    virusSecondaryAction(AC_NOTHING),
    macroVirusPrimaryAction(AC_NOTHING),
    macroVirusSecondaryAction(AC_NOTHING),
    riskPrimaryAction(AC_NOTHING),
    riskSecondaryAction(AC_NOTHING),
    stopServiceAction(SCAN_NOTIFY_PROMPT),
    terminateProcessAction(SCAN_NOTIFY_PROMPT),
    backupToQuarantine(false),
    doReverseLookups(false),
    auditScanResults(false),
    scheduledEnabled(false),
    scheduledRunFrequency(ScheduledFrequency_OnDemand),
    scheduledMinuteOfDay(0),
    scheduledDayOfWeek(ScheduledDay_Sunday),
    scheduledDayOfMonth(1),
    scheduledRunMissedEvents(false),
    scheduledAllowMissedScansOutsideWindow(false),
    scheduledAllowRunWhenLoggedOut(false),
    scheduledMissedScanWindowTime(0),
    scheduledRunNow(false),
    scheduledIsResumeable(false),
    scheduledScanWindowSize(0),
    id(GUID_NULL),
    creationTime(TIME_NULL),
    lastStartTime(TIME_NULL),
    customScanType(SCAN_TYPE_CUSTOM),
    displayStatusDialog(false),
    keyHandle(NULL),
    parentKeyHandle(NULL)
{
    // No code needed
}

CSavScanSettings::~CSavScanSettings()
{
    Clear();
}

HRESULT CSavScanSettings::ReadValueDWORD( HKEY keyHandle, LPCTSTR valueName, DWORD* resultValue, DWORD defaultValue )
{
    DWORD       tempValueData   = 0;
    DWORD       valueDataSize   = sizeof(tempValueData);
    DWORD       valueType       = REG_SZ;
    DWORD       returnValDW     = ERROR_OUT_OF_PAPER;
    HRESULT     returnValHR     = E_FAIL;

    // Validate parameters    
    if (keyHandle == NULL)
        return E_INVALIDARG;
    if (resultValue == NULL)
        return E_POINTER;

    returnValDW = SymSaferRegQueryValueEx(keyHandle, valueName, NULL, &valueType, (LPBYTE) &tempValueData, &valueDataSize);
    if (returnValDW == ERROR_SUCCESS)
    {
        if (valueType == REG_DWORD)
        {
            *resultValue = tempValueData;
            returnValHR = S_OK;
        }
        else
        {
            *resultValue = defaultValue;
            returnValHR = HRESULT_FROM_WIN32(ERROR_DATATYPE_MISMATCH);
        }
    }
    else
    {
        *resultValue = defaultValue;
        if (returnValDW == ERROR_FILE_NOT_FOUND)
            returnValHR = S_FALSE;
        else
            returnValHR = HRESULT_FROM_WIN32(returnValDW);
    }

    return returnValHR;
}

HRESULT CSavScanSettings::ReadValueBool( HKEY keyHandle, LPCTSTR valueName, bool* valueData, bool defaultValue )
// Same as RegQueryDword, but for bool's
{
    DWORD       valueDataDW     = FALSE;
    DWORD       defaultValueDW  = FALSE;
    HRESULT     returnValHR     = E_FAIL;
    
    // Validate parameters
    if (valueData == NULL)
        return E_POINTER;

    if (defaultValue)
        defaultValueDW = TRUE;
    returnValHR = ReadValueDWORD(keyHandle, valueName, &valueDataDW, defaultValueDW);
    if (valueDataDW)
        *valueData = true;
    return returnValHR;
}

HRESULT CSavScanSettings::ReadValueString( HKEY keyHandle, LPCTSTR valueName, tstring* resultValue, LPCTSTR defaultValue )
{
    LPTSTR      valueDataBuffer = NULL;
    DWORD       valueDataSize   = 0;
    DWORD       valueType       = REG_SZ;
    DWORD       returnValDW     = ERROR_OUT_OF_PAPER;
    HRESULT     returnValHR     = E_FAIL;

    // Validate parameters    
    if (keyHandle == NULL)
        return E_INVALIDARG;
    if (resultValue == NULL)
        return E_POINTER;

    // Read in the string
    returnValDW = SymSaferRegQueryValueEx(keyHandle, valueName, NULL, &valueType, NULL, &valueDataSize);
    if (returnValDW == ERROR_SUCCESS)
    {
        valueDataBuffer = new(std::nothrow) TCHAR[valueDataSize+1];
        if (valueDataBuffer != NULL)
        {
            if (valueType == REG_SZ)
            {
                returnValDW = SymSaferRegQueryValueEx(keyHandle, valueName, NULL, &valueType, reinterpret_cast<LPBYTE>(valueDataBuffer), &valueDataSize);
                if (returnValDW == ERROR_SUCCESS)
                {
                    try
                    {
                        *resultValue = (LPCTSTR) valueDataBuffer;
                    }
                    catch (std::bad_alloc)
                    {
                        returnValHR = E_OUTOFMEMORY;
                    }
                    returnValHR = S_OK;
                }
            }
            else
            {
                returnValHR = HRESULT_FROM_WIN32(ERROR_DATATYPE_MISMATCH);
            }
            delete [] valueDataBuffer;
            valueDataBuffer = NULL;
        }
        else
        {
            returnValHR = E_OUTOFMEMORY;
        }
    }
    else
    {
        if (returnValDW == ERROR_FILE_NOT_FOUND)
            returnValHR = S_FALSE;
        else
            returnValHR = HRESULT_FROM_WIN32(returnValDW);
    }
    
    // If for any reason we failed, set *resultValue to the default
    if ((returnValHR != S_OK) && (returnValHR != E_OUTOFMEMORY))
    {
        try
        {
            *resultValue = defaultValue;
        }
        catch (std::bad_alloc)
        {
            returnValHR = E_OUTOFMEMORY;
        }
    }

    return returnValHR;
}

HRESULT CSavScanSettings::ReadValueTime( HKEY keyHandle, LPCTSTR valueName, time_t* resultValue, time_t defaultValue )
{
    DWORD       valueRead       = 0;
    HRESULT     returnValHR     = E_FAIL;

    // Validate parameters
    if (keyHandle == NULL)
        return E_INVALIDARG;
    if (valueName == NULL)
        return E_INVALIDARG;
    if (resultValue == NULL)
        return E_POINTER;

    returnValHR = ReadValueDWORD(keyHandle, valueName, &valueRead, static_cast<DWORD>(defaultValue));
    *resultValue = static_cast<time_t>(valueRead);
    return returnValHR;
}


/*
HRESULT CSavScanSettings::ReadFromRegistry( HKEY parentKeyHandle, LPCTSTR parentPath, LPCTSTR scanKeyName, bool isReadOnly )
// Reads the data for this scan in from the registry
{
    _bstr_t     stringConverter;
    TCHAR       stringBuffer[2*MAX_PATH]    = _T("");
    DWORD       stringBufferSize            = 0;
    DWORD       valueType                   = REG_DWORD;
    DWORD       returnValDW                 = ERROR_OUT_OF_PAPER;
    HRESULT     returnValHR                 = E_FAIL;

    // Validate parameters
    if (parentKeyHandle == NULL)
        return E_INVALIDARG;
    if (scanKeyName == NULL)
        return E_POINTER;
    if (parentPath == NULL)
        return E_POINTER;

    keyPath = parentPath;
    stringConverter = scanKeyName;
    CLSIDFromString(stringConverter, &scanID);
    readOnly = isReadOnly;

    returnValDW = RegOpenKeyEx(parentKeyHandle, scanKeyName, NULL, KEY_READ, &keyHandle);
    if (returnValDW == ERROR_SUCCESS)
    {
        stringBufferSize = sizeof(stringBuffer)/sizeof(stringBuffer[0]);
        returnValDW = SymSaferRegQueryValueExW(keyHandle, _T(szReg_Value_ScanTitle), NULL, &valueType, stringBuffer, &stringBufferSize);

        // Datastore format for user scans:
        // <SAV Main HCU key>\Custom Tasks
        //      <Scan GUID>
        //          <All Scan options EXCEPT description>
        //          SelectedScanType = Full | Quick | Custom | COH
        //      TaskPadScripted
        //          <Scan User Readable Name>
        //              (Default) = <Scan GUID>
        //              ActiveX ID = HARDCODED VALUE
        //              Description = <Scan Description>
        // RTVScan determines Scheduled by this test:  (Schedule\MinOfDay == NO_SCHEDULED_START_TIME) && (!Scheduled\RandomizeDayEnabled && !Schedule\RandomizeWeekEnabled && !Schedule\RandomizeMonthEnabled))
        //      See misc.cpp's GetTimeValues for loading time values
        //
        // If this is a StartupScan, it is also listed here:
        //      TaskPadStartup
        //          <Scan Name>
        //              (Default) = <Scan GUID>
        //              ActiveX ID = HARDCODED VALUE
        //              Description = <Scan Description>
        //
        // If this is a ScheduledScan, it is also listed here:
        //      TaskPadScheduled
        //          <Scan Name>
        //              (Default) = <Scan GUID>
        //              ActiveX ID = HARDCODED VALUE
        //              Description = <Scan Description>
        //
        // Datastore format for admin scheduled scans:
        // <SAV main HLM key>\LocalScans
        //      <Scan GUID>
        //          <All Scan options...>
        //          Schedule
        //              RTVScan determines Scheduled by this test:  (Schedule\MinOfDay == NO_SCHEDULED_START_TIME) && (!Scheduled\RandomizeDayEnabled && !Schedule\RandomizeWeekEnabled && !Schedule\RandomizeMonthEnabled))
        //              See misc.cpp's GetTimeValues for loading time values
        //              Note that Type != 0 is also used in the UI for this
        // Note: SSC has no description UI for admin scheduled scans, only names
        RegCloseKey(keyHandle);
    }
    return HRESULT_FROM_WIN32(returnValDW);
}
*/
