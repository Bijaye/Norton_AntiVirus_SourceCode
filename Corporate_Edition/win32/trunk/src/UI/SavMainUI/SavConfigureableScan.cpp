// SavConfigureableScan.cpp : Implementation of CSavConfigureableScan

#include "stdafx.h"
#include "SavConfigureableScan.h"
#include "Util.h"
#include "StrSafe.h"
#include "vpcommon.h"
#include "ConfigObj.h"
#include "UiUtil.h"
#include "ScanSelectFilesPage.h"
#include "ScanOptionsPage.h"
#include "ScanSchedulePage.h"
#include "ScanNamePage.h"


#ifndef _USE_32BIT_TIME_T
    #error "SAV requires 32-bit time_t's, please define _USE_32BIT_TIME_T in your project"
#endif
#define mtos(x) ((x) * 60L)             // Convert x minutes to seconds
#define htos(x) ((x) * 60L * 60L)       // Convert x hours to seconds
#define dtos(x) ((x) * 24L * 3600L)     // Convert x days to seconds


// IProtection_ConfigureableScan
HRESULT CSavConfigureableScan::get_ID( GUID* scanID )
{
    // Validate parameter
    if (scanID == NULL)
        return E_POINTER;
    // Validate caller security
    if (trustVerifier.IsCallerProcessTrusted() != S_OK)
        return E_ACCESSDENIED;

    *scanID = id;

    return S_OK;
}

HRESULT CSavConfigureableScan::get_Name( BSTR *nameBuffer )
{
    // Validate parameter
    if (nameBuffer == NULL)
        return E_POINTER;
    // Validate caller security
    if (trustVerifier.IsCallerProcessTrusted() != S_OK)
        return E_ACCESSDENIED;

    return userAssignedName.CopyTo(nameBuffer);
}

HRESULT CSavConfigureableScan::put_Name( BSTR newName )
{
	StringList 					existingScanNames;
	StringList::iterator		currExistingScanName;
	bool						foundScanName			= false;
	HKEY						keyHandle		= NULL;
	CString						statusDialogTitle;
	DWORD						returnValDW		= ERROR_OUT_OF_PAPER;
	HRESULT						returnValHR     = E_FAIL;
    HRESULT						returnValHRb    = E_FAIL;

	// Validate parameter
	if (newName == NULL)
		return E_POINTER;
	if (lstrcmpi(newName, _T("")) == 0)
		return E_INVALIDARG;
    // Validate caller security
    if (trustVerifier.IsCallerProcessTrusted() != S_OK)
        return E_ACCESSDENIED;

	// Is there a change?
	if (lstrcmpi(newName, userAssignedName) == 0)
	{
		// No, nothing to do
		return S_FALSE;
	}

	// Does the new name already exist?
	GetScanNames(&existingScanNames);
	returnValHR = E_FAIL;
	// Is this scan name already taken?
	for (currExistingScanName = existingScanNames.begin(); (currExistingScanName != existingScanNames.end()) && (!foundScanName); currExistingScanName++)
	{
		if (lstrcmpi(newName, currExistingScanName->c_str()) == 0)
			foundScanName = true;
	}
	if (foundScanName)
	{
		// Is this just a name case change of the existing scan name>
		if (!((lstrcmpi(newName, userAssignedName) == 0) && (lstrcmp(newName, userAssignedName) != 0)))
		{
			// No, name is a dupe, return failure
			return E_INVALIDARG;
		}
	}

	// Set the new name
    // The registry storage structure for the name is unusual.  The name is not stored under the scan's key.  Instead, it
    // and the description are stored as a key named for the userAssignedName under one of 3 other keys at the same level
    // as the scan's key:  TaskPadScripted, TaskPadStartup, and TaskPadScheduled.
    // Consequently, to change the name we must delete the old key, with the old name, and then write out the new key.
    returnValHR = S_OK;
    if (typeOfScan == StartType_Startup)
    {
        returnValHRb = DeleteTaskpadValues(_T(szReg_Value_Startup), userAssignedName, userAssignedDescription);
		if (returnValHRb == E_INVALIDARG)
			returnValHRb = S_OK;
        returnValHR = SUCCEEDED(returnValHR) ? returnValHRb : returnValHR;
        returnValHRb = WriteTaskpadValues(_T(szReg_Value_Startup), newName, userAssignedDescription);
        returnValHR = SUCCEEDED(returnValHR) ? returnValHRb : returnValHR;
    }
    else if (typeOfScan == StartType_Scheduled)
    {
        returnValHRb = DeleteTaskpadValues(_T(szReg_Value_Scheduled), userAssignedName, userAssignedDescription);
		if (returnValHRb == E_INVALIDARG)
			returnValHRb = S_OK;
        returnValHR = SUCCEEDED(returnValHR) ? returnValHRb : returnValHR;
        returnValHRb = WriteTaskpadValues(_T(szReg_Value_Scheduled), newName, userAssignedDescription);
        returnValHR = SUCCEEDED(returnValHR) ? returnValHRb : returnValHR;
    }
    else if (typeOfScan == StartType_OnDemand)
    {
        returnValHRb = DeleteTaskpadValues(_T(szReg_Value_Scripted), userAssignedName, userAssignedDescription);
		if (returnValHRb == E_INVALIDARG)
			returnValHRb = S_OK;
        returnValHR = SUCCEEDED(returnValHR) ? returnValHRb : returnValHR;
        returnValHRb = WriteTaskpadValues(_T(szReg_Value_Scripted), newName, userAssignedDescription);
        returnValHR = SUCCEEDED(returnValHR) ? returnValHRb : returnValHR;
    }

	// Finally, the status dialog title entry must be updated.
	// Failure here does not fail the put_Name operation.
	returnValDW = RegOpenKeyEx(parentKeyHandle, scanKeyName.c_str(), NULL, KEY_WRITE, &keyHandle);
	if (returnValDW == ERROR_SUCCESS)
	{
		statusDialogTitle.FormatMessage(IDS_SCAN_STATUSTITLETEMPLATE, static_cast<LPCTSTR>(newName));
		RegSetValueEx(keyHandle, _T(szReg_Value_ScanTitle), NULL, REG_SZ, (LPBYTE) (static_cast<LPCTSTR>(statusDialogTitle)), (statusDialogTitle.GetLength()+1)*sizeof(TCHAR));
		RegCloseKey(keyHandle);
		keyHandle = NULL;
	}
       
    try
    {
        if (SUCCEEDED(returnValHR))
            userAssignedName = newName;
    }
    catch (CMemoryException&)
    {
        returnValHR = E_OUTOFMEMORY;
    }
    return returnValHR;
}

HRESULT CSavConfigureableScan::get_ReadOnly( VARIANT_BOOL* isReadOnlyArg )
{
    // Validate parameter
    if (isReadOnlyArg == NULL)
        return E_POINTER;
    // Validate caller security
    if (trustVerifier.IsCallerProcessTrusted() != S_OK)
        return E_ACCESSDENIED;

    *isReadOnlyArg = isReadOnly;
    return S_OK;
}

HRESULT CSavConfigureableScan::get_StartType( StartType* typeOfScanArg )
{
    // Validate parameter
    if (typeOfScanArg == NULL)
        return E_POINTER;
    // Validate caller security
    if (trustVerifier.IsCallerProcessTrusted() != S_OK)
        return E_ACCESSDENIED;

    *typeOfScanArg = typeOfScan;
    return S_OK;
}

HRESULT CSavConfigureableScan::get_Description( BSTR* description )
{
    // Validate parameter
    if (description == NULL)
        return E_POINTER;
    // Validate caller security
    if (trustVerifier.IsCallerProcessTrusted() != S_OK)
        return E_ACCESSDENIED;

    return userAssignedDescription.CopyTo(description);
}

HRESULT CSavConfigureableScan::put_Description( BSTR description )
{
    HRESULT     returnValHR = E_FAIL;

    // Validate caller security
    if (trustVerifier.IsCallerProcessTrusted() != S_OK)
        return E_ACCESSDENIED;

    // The registry storage structure for the name is unusual.  See the notes for put_Name for more details.
	if (typeOfScan == StartType_Startup)
        returnValHR = WriteTaskpadValues(_T(szReg_Value_Startup), userAssignedName, description);
    else if (typeOfScan == StartType_Scheduled)
        returnValHR = WriteTaskpadValues(_T(szReg_Value_Scheduled), userAssignedName, description);
    else if (typeOfScan == StartType_OnDemand)
        returnValHR = WriteTaskpadValues(_T(szReg_Value_Scripted), userAssignedName, description);

    try
    {
        if (SUCCEEDED(returnValHR))
            userAssignedDescription = description;
    }
    catch (CMemoryException&)
    {
        returnValHR = E_OUTOFMEMORY;
    }
    return returnValHR;
}


HRESULT CSavConfigureableScan::get_LastRunTime( SYSTEMTIME* lastRunTimeArg )
{
    // Validate parameter
    if (lastRunTimeArg == NULL)
        return E_POINTER;
    // Validate caller security
    if (trustVerifier.IsCallerProcessTrusted() != S_OK)
        return E_ACCESSDENIED;

    *lastRunTimeArg = lastRunTime;
    return S_OK;
}

HRESULT CSavConfigureableScan::get_IsRunning( VARIANT_BOOL* isRunning )
{
    HKEY        keyHandle       = NULL;
    DWORD       scanStatus      = S_UNKNOWN;
    DWORD       returnValDW     = ERROR_OUT_OF_PAPER;

    // Validate parameter
    if (isRunning == NULL)
        return E_POINTER;
    // Validate caller security
    if (trustVerifier.IsCallerProcessTrusted() != S_OK)
        return E_ACCESSDENIED;

    // Read in the current status
    returnValDW = RegOpenKeyEx(parentKeyHandle, scanKeyName.c_str(), NULL, KEY_READ, &keyHandle);
    if (returnValDW == ERROR_SUCCESS)
    {
        RegQueryDword(keyHandle, _T(szReg_Value_ScanStatus), &scanStatus, S_UNKNOWN);
        RegCloseKey(keyHandle);
        keyHandle = NULL;
    }

    // NOTE:  ResultsView.cpp::ScanRunning does this differently
    switch (scanStatus)
    {
    case S_STARTING:
    case S_STARTED:
    case S_SCANNING_DIRS:
    case S_SCANNING_BOOT:
    case S_SCANNING_MEM:
    case S_SCANNING_FILES:
    case S_SCANNING_PROCS:
    case S_SCANNING_LOADPOINTS:
    case S_SCANNING_ERASER_DEFS:
    case S_ERASER_SCANS:
    case S_SCANNING_PROCS_AND_LOADPOINTS:
    case S_SCANNING_PROCS_AND_ERASER_DEFS:
    case S_SCANNING_LOADPOINTS_AND_ERASER_DEFS:
    case S_SCANNING_PROCS_AND_LOADPOINTS_AND_ERASER_DEFS:
        *isRunning = TRUE;
        break;
    default:
        *isRunning = FALSE;
    }
    return S_OK;
}

HRESULT CSavConfigureableScan::ShowConfigureUI( HWND parentWindowHandle )
{
    // Validate caller security
    if (trustVerifier.IsCallerProcessTrusted() != S_OK)
        return E_ACCESSDENIED;

    // Technically redundant since caller must do this to create their property page, but good practice
    AFX_MANAGE_STATE(AfxGetStaticModuleState());

    CControlParentSheet                 configWizard;
    CScanSelectFilesPage                filesPage;
	CScanOptionsPage					optionsPage;
	CScanSchedulePage					schedulePage;
	CScanNamePage						namePage;
    CWnd                                parentWindow;
	CString								wizardTitle;
    INT_PTR                             returnValIP     = IDCANCEL;
    HRESULT                             returnValHR     = E_FAIL;

    if (GetScanConfig() != NULL)
    {
		CConfigObj			config(GetScanConfig());

		// Initialize pages
		if (typeOfScan == StartType_Scheduled)
			schedulePage.SetScan(this);
		optionsPage.SetScan(this);
		namePage.SetScan(this);
		filesPage.SetScan(this);

		// Setup wizard
		parentWindow.Attach(parentWindowHandle);
        configWizard.SetParent(&parentWindow);
        configWizard.SetCaption(userAssignedName);
        configWizard.m_psh.dwFlags |= PSH_NOAPPLYNOW | PSH_PROPTITLE;
		if (typeOfScan == StartType_Scheduled)
			configWizard.AddPage(&schedulePage);
		configWizard.AddPage(&filesPage);
		configWizard.AddPage(&optionsPage);
		configWizard.AddPage(&namePage);
        returnValIP = configWizard.DoModal();
        parentWindow.Detach();
		ReadFromRegistry(NULL, NULL, false);
    }

    if (returnValIP == IDOK)
        returnValHR = S_OK;
    else
        returnValHR = S_FALSE;
    return returnValHR;
}

HRESULT CSavConfigureableScan::ShowLogUI( HWND parentWindow )
{
    // Validate caller security
    if (trustVerifier.IsCallerProcessTrusted() != S_OK)
        return E_ACCESSDENIED;

    return E_NOTIMPL;
}

HRESULT CSavConfigureableScan::RunScan( HWND parentWindow )
{
    _bstr_t         stringConverter;
	IVirusProtect*  ptrVirusProtect             = NULL;
    IScan*          pScan                       = NULL;
	DWORD		    dwErr                       = 0;
    TCHAR           scanName[50]                = _T("");

    // Validate caller security
    if (trustVerifier.IsCallerProcessTrusted() != S_OK)
        return E_ACCESSDENIED;

	CConfigObj config(GetScanConfig());
	config.SetOption(_T(szReg_Value_ScanStatus), (DWORD) S_NEVER_RUN);
    config.SetOption(_T(szReg_Value_Logger), (DWORD)LOGGER_Manual);
    config.SetOption(_T(szReg_Value_ScanTitle), (LPCTSTR) userAssignedName);

	if( SUCCEEDED(GetScanConfig()->CreateByIID(IID_IScan, (void**)&pScan)))
	{
		if (SUCCEEDED(pScan->Open(NULL, GetScanConfig())))
        {
            dwErr = pScan->StartScan(TRUE /*Launch Async*/);
	        if (dwErr != ERROR_SUCCESS)
		        AfxMessageBox(_T("ERROR!"));
        }
		pScan->Release();
		pScan = NULL;
	}
    return S_OK;
}

// ReadTaskpadValues_Scheduled
HRESULT CSavConfigureableScan::get_RepeatType( RepetitionType* type )
{
    // Validate parameter
    if (type == NULL)
        return E_POINTER;
    // Validate caller security
    if (trustVerifier.IsCallerProcessTrusted() != S_OK)
        return E_ACCESSDENIED;

    *type = scheduledRepeatType;
    return S_OK;
}

HRESULT CSavConfigureableScan::get_RepetitionDay( unsigned int* day )
{
    // Validate parameter
    if (day == NULL)
        return E_POINTER;
    // Validate caller security
    if (trustVerifier.IsCallerProcessTrusted() != S_OK)
        return E_ACCESSDENIED;

    *day = scheduledRepetitionDay;
    return S_OK;
}

HRESULT CSavConfigureableScan::get_StartTime( SYSTEMTIME* startTime )
// StartTime is the time only, no date, for when the scan starts
{
    // Validate parameter
    if (startTime == NULL)
        return E_POINTER;
    // Validate caller security
    if (trustVerifier.IsCallerProcessTrusted() != S_OK)
        return E_ACCESSDENIED;

    *startTime = scheduledStartTime;
    return S_OK;
}

HRESULT CSavConfigureableScan::get_Enabled( VARIANT_BOOL* isEnabled )
// Scheduled scans may be enabled or disabled - return which
{
    // Validate parameter
    if (isEnabled == NULL)
        return E_POINTER;
    // Validate caller security
    if (trustVerifier.IsCallerProcessTrusted() != S_OK)
        return E_ACCESSDENIED;

    *isEnabled = scheduledIsEnabled;
    return S_OK;
}

HRESULT CSavConfigureableScan::GetNextRunTime( SYSTEMTIME* nextRunTimeStart, VARIANT_BOOL* supportsWindows, unsigned int* scanWindowRunHours )
// Sets *nextRunTimeStart equal to the start of the next time this scan will run.
// If scan windows are supported, also sets *supportsWindows to true and *nextRunTime to the end of the window
{
    time_t      nextRunTime                 = {0};
    time_t      currTime                    = {0};
    SYSTEMTIME  nextRunTimeStartTemp        = {0};

    // Validate parameters
    if (nextRunTimeStart == NULL)
        return E_POINTER;
    if (supportsWindows == NULL)
        return E_POINTER;
    if (scanWindowRunHours == NULL)
        return E_POINTER;
    // Validate caller security
    if (trustVerifier.IsCallerProcessTrusted() != S_OK)
        return E_ACCESSDENIED;

	if (typeOfScan == StartType_Scheduled)
	{
	    currTime = time(NULL);
		nextRunTime = CalculateNextRunTime((scheduledStartTime.wHour*60) + scheduledStartTime.wMinute, scheduledRepetitionDay, scheduledRepetitionDay, currTime, scheduledRepeatType);
		CTime(nextRunTime).GetAsSystemTime(nextRunTimeStartTemp);
	}
	*nextRunTimeStart = nextRunTimeStartTemp;
    return S_OK;
}

HRESULT CSavConfigureableScan::ATLfilterIsScheduledScan( void* currObject, REFIID interfaceID, LPVOID* requestedInterfacePtr, DWORD userArgument )
// ATL Filter function - defers to IsSheduledScan.
{
    CSavConfigureableScan*      thisScanObject  = NULL;
    HRESULT                     returnValHR     = E_NOINTERFACE;

    if (currObject != NULL)
    {
        thisScanObject = reinterpret_cast<CSavConfigureableScan*>(currObject);
        if (thisScanObject->IsScheduledScan())
        {
            *requestedInterfacePtr = dynamic_cast<IProtection_ConfigureableScan_Scheduled*>(thisScanObject);
			thisScanObject->AddRef();
            returnValHR = S_OK;
        }
    }

    // Return result
    if (FAILED(returnValHR))
        *requestedInterfacePtr = NULL;
    return returnValHR;
}

bool CSavConfigureableScan::IsScheduledScan( void )
// If this is a scheduled scan, permits IProtection_ConfigureableScan_Scheduled to be exposed, otherwise does not.
{
    return (typeOfScan == StartType_Scheduled);
}

// Private APIs, used by ConfigureableScanCollection
HRESULT CSavConfigureableScan::ReadFromRegistry( HKEY readParentKeyHandle, LPCTSTR readScanKeyName, bool readIsReadOnly )
// Reads data for this object in from the registry.
// To refresh data instead, call with readParentKeyHandle NULL and readScanKeyName NULL
{
    HKEY            keyHandle                               = NULL;
    DWORD           currKeyNo                               = 0;
    TCHAR           keyName[50]                             = _T("");
    HKEY            startupSubkeyHandle                     = NULL;
    std::wstring    startupScanGuidString;
    TCHAR           startupScanGuidStringFormatted[50]      = _T("");
    GUID            startupScanGuid                         = GUID_NULL;
    time_t          lastRunTimeTT                           = NULL;
    DWORD           startMinute                             = 0;
    HKEY            scheduleKey                             = NULL;
    CComBSTR        assignedNameTemp;
    CComBSTR        assignedDescriptionTemp;
    DWORD           repeatTypeDW                            = 0;
    DWORD           repeateDayDW                            = 0;
    DWORD           returnValDW                             = ERROR_OUT_OF_PAPER;
    HRESULT         returnValHR                             = E_FAIL;

    // Validate parameters
    if (id == GUID_NULL)
	{
		if ((readScanKeyName == NULL) || (readParentKeyHandle == NULL))
			return E_INVALIDARG;
	}
	else
	{
		if ((readScanKeyName != NULL) || (readParentKeyHandle != NULL))
			return E_INVALIDARG;
	}

	// Are we initializing this object from scratch, or updating it's data?
	if (id == GUID_NULL)
	{
		// Initializing it from scratch
		// Make sure the name is valid.  The key name is the scan ID - if it isn't a GUID, it isn't valid
		StringCchPrintf(keyName, sizeof(keyName)/sizeof(keyName[0]), _T("{%s}"), readScanKeyName);
		if (FAILED(CLSIDFromString(keyName, &id)))
			return E_INVALIDARG;
		if (GetScanConfig() == NULL)
			return E_FAIL;
		// Duplicate the parent handle
		if (!DuplicateHandle(GetCurrentProcess(), readParentKeyHandle, GetCurrentProcess(), (LPHANDLE) &parentKeyHandle, NULL, FALSE, DUPLICATE_SAME_ACCESS))
			return HRESULT_FROM_WIN32(GetLastError());
		scanKeyName = readScanKeyName;
		// The location of where the scan data is stored determines RO or not, so enumerator must tell us
		isReadOnly = readIsReadOnly;
	}
	else
	{
		// Just updating data from the registry
	}
	// Open the key for this scan
	returnValDW = RegOpenKeyEx(parentKeyHandle, scanKeyName.c_str(), NULL, KEY_READ, &keyHandle);
	if (returnValDW != ERROR_SUCCESS)
		return HRESULT_FROM_WIN32(returnValDW);

    userAssignedName.Empty();
    userAssignedDescription.Empty();
    typeOfScan                  = StartType_OnDemand;
    lastRunTime                 = SYSTEMTIME_NULL;
    scheduledRepeatType         = RepetitionType_Daily;
    scheduledRepetitionDay      = 0;
    scheduledStartTime          = SYSTEMTIME_NULL;
    scheduledIsEnabled          = FALSE;

    // Determine the scan type.  Is this a startup scan?
    // If this is a scheduled scan, then the following key relative to the parent will also exist:
    //      TaskPadStartup
    //          <Scan Name>
    //              (Default) = <Scan GUID>
    //              ActiveX ID = HARDCODED VALUE
    //              Description = <Scan Description>
    // The name and description are stored under either TaskPadScript, TaskPadScheduled, or TaskPadStartup.
    // WHICH of these they are stored under corresponds with the StartType of this scan - on demand, scheduled, or startup
    // Read in the name/description for manual scans
    returnValHR = ReadTaskpadValues(_T(szReg_Value_Scripted), &assignedNameTemp, &assignedDescriptionTemp);
    if (returnValHR == S_OK)
    {
        typeOfScan = StartType_OnDemand;
        userAssignedName = assignedNameTemp;
        userAssignedDescription = assignedDescriptionTemp;
    }
    returnValHR = ReadTaskpadValues(_T(szReg_Value_Startup), &assignedNameTemp, &assignedDescriptionTemp);
    if (returnValHR == S_OK)
    {
        typeOfScan = StartType_Startup;
        userAssignedName = assignedNameTemp;
        userAssignedDescription = assignedDescriptionTemp;
    }
    returnValHR = ReadTaskpadValues(_T(szReg_Value_Scheduled), &assignedNameTemp, &assignedDescriptionTemp);
    if (returnValHR == S_OK)
    {
        typeOfScan = StartType_Scheduled;
        userAssignedName = assignedNameTemp;
        userAssignedDescription = assignedDescriptionTemp;
    }

    // Read in the last start time and schedule information
    returnValDW = RegOpenKey(keyHandle, _T(szReg_Key_ScheduleKey), &scheduleKey);
    if (returnValDW == ERROR_SUCCESS)
    {
		// Ensure we return NULL for lastRunTime if not set
        RegQueryDword(scheduleKey, _T(szReg_Val_Schedule_LastStart), reinterpret_cast<DWORD*>(&lastRunTimeTT), NULL);
		if (lastRunTimeTT != NULL)
	        CTime(lastRunTimeTT).GetAsSystemTime(lastRunTime);
        RegQueryDword(scheduleKey, _T(szReg_Val_MinOfDay), &startMinute, NO_SCHEDULED_START_TIME);

        // IProtection_ConfigureableScan_Scheduled Specific
        RegQueryDword(scheduleKey, _T(szReg_Val_HowOften), &repeatTypeDW, S_NONE);
        switch (repeatTypeDW)
        {
        case S_DAILY:
            scheduledRepeatType = RepetitionType_Daily;
            RegQueryDword(scheduleKey, _T(szReg_Val_DayOfWeek), static_cast<DWORD*>(&repeateDayDW), 0);
            break;
        case S_WEEKLY:
            scheduledRepeatType = RepetitionType_Weekly;
            RegQueryDword(scheduleKey, _T(szReg_Val_DayOfWeek), static_cast<DWORD*>(&repeateDayDW), 0);
            break;
        case S_MONTHLY:
            scheduledRepeatType = RepetitionType_Monthly;
            RegQueryDword(scheduleKey, _T(szReg_Val_DayOfMonth), static_cast<DWORD*>(&repeateDayDW), 0);
            break;
        }
        scheduledRepetitionDay = repeateDayDW;
        scheduledStartTime.wHour = static_cast<WORD>(startMinute / 60);
        scheduledStartTime.wMinute = static_cast<WORD>(startMinute % 60);
        RegQueryVARIANTBOOL(scheduleKey, _T(szReg_Val_Enabled), &scheduledIsEnabled, FALSE);

        RegCloseKey(scheduleKey);
        scheduleKey = NULL;
    }
    // Use the same method as RTVScan uses to determine scheduled/custom
    if ((typeOfScan != StartType_Startup) && (startMinute == NO_SCHEDULED_START_TIME))
        typeOfScan = StartType_OnDemand;

    RegCloseKey(keyHandle);
    keyHandle = NULL;
    return S_OK;
}

HRESULT CSavConfigureableScan::ReadTaskpadValues( LPCTSTR taskpadKeyName, BSTR* userAssignedName, BSTR* userAssignedDescription )
// Read the name and description for this scan from the specified Taskpad subkey.
// Returns S_FALSE if there is no subkey for this scan in the specified taskpad subkey, S_OK if it exists and sets
// *userAssignedName and *userAssignedDescription, else the error code of the failure
{
    HKEY            taskpadKeyHandle                    = NULL;
    DWORD           currKeyNo                           = 0;
    TCHAR           currTaskpadKeyName[MAX_PATH]        = _T("");
    GUID            currTaskpadKeyGuid                  = GUID_NULL;
    TCHAR           currTaskpadKeyGuidFormatted[50]     = _T("");
    std::wstring    currTaskpadKeyGuidString;
    HKEY            currTaskpadKeyHandle                = NULL;
    DWORD           returnValDW                         = ERROR_OUT_OF_PAPER;
    HRESULT         returnValHR                         = E_FAIL;

    // Open the startup key
    returnValDW = RegOpenKeyEx(parentKeyHandle, taskpadKeyName, NULL, KEY_READ, &taskpadKeyHandle);
    if (returnValDW == ERROR_SUCCESS)
    {
        returnValHR = S_FALSE;
        // Enumerate each key
        returnValDW = RegEnumKey(taskpadKeyHandle, currKeyNo, currTaskpadKeyName, sizeof(currTaskpadKeyName)/sizeof(currTaskpadKeyName[0]));
        while ((returnValHR == S_FALSE) && (returnValDW == ERROR_SUCCESS))
        {
            // currTaskpadKeyName is the user's assigned name
            currTaskpadKeyGuid                     = GUID_NULL;
            currTaskpadKeyGuidFormatted[0]         = NULL;
            currTaskpadKeyGuidString.clear();

            // Open each key
            returnValDW = RegOpenKeyEx(taskpadKeyHandle, currTaskpadKeyName, NULL, KEY_READ, &currTaskpadKeyHandle);
            if (returnValDW == ERROR_SUCCESS)
            {
                // Key name is the user assigned description.  Default value is the ID of the scan
                // Is this key for the current scan?
                RegQueryString(currTaskpadKeyHandle, NULL, &currTaskpadKeyGuidString, NULL);
                StringCchPrintf(currTaskpadKeyGuidFormatted, sizeof(currTaskpadKeyGuidFormatted)/sizeof(currTaskpadKeyGuidFormatted[0]), _T("{%s}"), currTaskpadKeyGuidString.c_str());
                CLSIDFromString(currTaskpadKeyGuidFormatted, &currTaskpadKeyGuid);
                if (currTaskpadKeyGuid == id)
                {
                    // Yes.  Return the name and description and we're done.
                    returnValHR = S_OK;
                    try
                    {
                        *userAssignedName = CComBSTR(currTaskpadKeyName).Detach();
                    }
                    catch (CMemoryException*)
                    {
                        returnValHR = E_OUTOFMEMORY;
                    }
                    RegQueryString(currTaskpadKeyHandle, _T(szReg_Val_Description), userAssignedDescription, NULL);
                }

                RegCloseKey(currTaskpadKeyHandle);
                currTaskpadKeyHandle = NULL;
            }

            currKeyNo += 1;
            returnValDW = RegEnumKey(taskpadKeyHandle, currKeyNo, currTaskpadKeyName, sizeof(currTaskpadKeyName)/sizeof(currTaskpadKeyName[0]));
        }
        RegCloseKey(taskpadKeyHandle);
        taskpadKeyHandle = NULL;
    }

    return returnValHR;
}

HRESULT CSavConfigureableScan::DeleteTaskpadValues( LPCTSTR taskpadKeyName, LPCTSTR userAssignedName, LPCTSTR userAssignedDescription )
// Deletes the TaskPad values and key for this scan from the specified Taskpad key
{
    HKEY    taskpadKeyHandle        = NULL;
    DWORD   returnValDW             = ERROR_OUT_OF_PAPER;

	// Validate parameters
	if (taskpadKeyName == NULL)
		return E_INVALIDARG;
	if (userAssignedName == NULL)
		return E_INVALIDARG;

    returnValDW = RegOpenKeyEx(parentKeyHandle, taskpadKeyName, NULL, KEY_WRITE, &taskpadKeyHandle);
    if (returnValDW == ERROR_SUCCESS)
    {
        returnValDW = SHDeleteKey(taskpadKeyHandle, userAssignedName);
        RegCloseKey(taskpadKeyHandle);
        taskpadKeyHandle = NULL;
    }
    return HRESULT_FROM_WIN32(returnValDW);
}

HRESULT CSavConfigureableScan::WriteTaskpadValues( LPCTSTR taskpadKeyName, LPCTSTR userAssignedName, LPCTSTR userAssignedDescription )
// Writes out the Taskpad values for the scan to the specified Taskpad key
{
    HKEY    taskpadKeyHandle        = NULL;
    HKEY    taskpadSubkeyHandle     = NULL;
    TCHAR   scanName[50]            = _T("");
	size_t	stringLength			= 0;
    DWORD   returnValDW             = ERROR_OUT_OF_PAPER;

	// Validate parameters
	if (taskpadKeyName == NULL)
		return E_INVALIDARG;
	if (userAssignedName == NULL)
		return E_INVALIDARG;

	// Initialize    
    StringFromGUID2(id, scanName, sizeof(scanName)/sizeof(scanName[0]));
    scanName[0] = NULL;
    scanName[37] = NULL;

    returnValDW = RegCreateKeyEx(parentKeyHandle, taskpadKeyName, NULL, NULL, NULL, KEY_WRITE, NULL, &taskpadKeyHandle, NULL);
    if (returnValDW == ERROR_SUCCESS)
    {
        returnValDW = RegCreateKeyEx(taskpadKeyHandle, userAssignedName, NULL, NULL, NULL, KEY_WRITE, NULL, &taskpadSubkeyHandle, NULL);
        if (returnValDW == ERROR_SUCCESS)
        {
			StringCchLength(userAssignedDescription, STRSAFE_MAX_CCH, &stringLength);

			returnValDW = RegSetValueEx(taskpadSubkeyHandle, NULL, NULL, REG_SZ, reinterpret_cast<LPBYTE>(&(scanName[1])), 37*2);
            returnValDW = RegSetValueEx(taskpadSubkeyHandle, _T(szReg_Val_OCXGUID), NULL, REG_SZ, reinterpret_cast<LPBYTE>(_T("64b4a5ae-0799-11d1-812a-00a0c95c0756")), 37*2);
            returnValDW = RegSetValueEx(taskpadSubkeyHandle, _T(szReg_Val_Description), NULL, REG_SZ, (LPBYTE) userAssignedDescription, static_cast<DWORD>(stringLength));
            RegCloseKey(taskpadSubkeyHandle);
            taskpadSubkeyHandle = NULL;
        }
        RegCloseKey(taskpadKeyHandle);
        taskpadKeyHandle = NULL;
    }
    return HRESULT_FROM_WIN32(returnValDW);
}


// Delete this scan object from persistent storage
HRESULT CSavConfigureableScan::Delete( void )
{
    HKEY        writeKeyHandle  = NULL;
    DWORD       returnValDW     = ERROR_OUT_OF_PAPER;
    HRESULT     returnValHR     = E_FAIL;

    // Validate state
    if (scanKeyName.compare(_T("")) == 0)
        return E_ABORT;
    // Validate access rights
    if (isReadOnly)
        return E_ACCESSDENIED;

    // Delete the scan data key and associated TaskPad listing
    returnValDW = RegOpenKeyEx(parentKeyHandle, NULL, NULL, KEY_WRITE, &writeKeyHandle);
    if (returnValDW == ERROR_SUCCESS)
    {
        returnValDW = SHDeleteKey(parentKeyHandle, scanKeyName.c_str());
        returnValHR = HRESULT_FROM_WIN32(returnValDW);
        switch (typeOfScan)
        {
        case StartType_OnDemand:
			DeleteTaskpadValues(_T(szReg_Value_Scripted), userAssignedName, userAssignedDescription);
            break;
        case StartType_Startup:
			DeleteTaskpadValues(_T(szReg_Value_Startup), userAssignedName, userAssignedDescription);
            break;
        case StartType_Scheduled:
			DeleteTaskpadValues(_T(szReg_Value_Scheduled), userAssignedName, userAssignedDescription);
            break;
        }
		// Reset this object
		Clear();
    }
    else
    {
        returnValHR = HRESULT_FROM_WIN32(returnValDW);
    }
    return returnValHR;
}

IScanConfig* CSavConfigureableScan::GetScanConfig( void )
// Returns an IConfig configuration object configured for this scan's datastore in the registry.
// This object is owned by this scan - do NOT call Release on it!
{
	IScanConfig*    newScanConfig               = NULL;
    _bstr_t         stringConverter;
    TCHAR           scanName[50]                = _T("");

	if (scanConfig == NULL)
	{
		if( SUCCEEDED(CoCreateLDVPObject(CLSID_CliProxy, IID_IScanConfig, (void**)&newScanConfig)))
		{
			StringFromGUID2(id, scanName, sizeof(scanName)/sizeof(scanName[0]));
			scanName[0] = NULL;
			scanName[37] = NULL;
			stringConverter = &scanName[1];
			if (!isReadOnly)
    			newScanConfig->Open(NULL, HKEY_VP_USER_SCANS, (char*) stringConverter);
			else
    			newScanConfig->Open(NULL, HKEY_VP_ADMIN_SCANS, (char*) stringConverter);
			newScanConfig->ChangeCloseMode(FALSE);
			scanConfig = newScanConfig;
		}
	}
	return scanConfig;
}

HRESULT CSavConfigureableScan::WriteToRegistry( void )
// Saves data to registry
{
    // All we support in this object is description!
    return E_NOTIMPL;
}

time_t CSavConfigureableScan::CalculateNextRunTime( int minuteOfDay, int dayOfWeek, int dayOfMonth, time_t baseTime, RepetitionType scanType )
{
    time_t      currTime;
    tm          currTimeTM;
    time_t      nextRunTime;
    tm          nextRunTM;
	int			scheduledMinuteOfSpan		= 0;
	int			currMinuteOfSpan			= 0;

    currTime = time(NULL);
    localtime_s(&currTimeTM, &currTime);
    switch (scanType)
    {
        case RepetitionType_Daily:
            // Has the run time already gone by today?
            if (minuteOfDay > (currTimeTM.tm_hour*60 + currTimeTM.tm_min))
            {
                // No, next run time is today at the specified hour and minute
                nextRunTM = currTimeTM;
                nextRunTM.tm_hour = minuteOfDay / 60;
                nextRunTM.tm_min = minuteOfDay % 60;
                nextRunTM.tm_sec	= 0;
            }
            else
            {
                // Yes.  Schedule for today
                nextRunTM = currTimeTM;
                nextRunTM.tm_hour = minuteOfDay / 60;
                nextRunTM.tm_min = minuteOfDay % 60;
                nextRunTM.tm_sec = 0;
				// Advance to tomorrow
                nextRunTime = mktime(&nextRunTM);
                nextRunTime += 60*60*24;
                localtime_s(&nextRunTM, &nextRunTime);
            }
            break;
        case RepetitionType_Weekly:
			scheduledMinuteOfSpan	= (dayOfWeek*60*24) + minuteOfDay;
			currMinuteOfSpan		= (currTimeTM.tm_wday*60*24) + currTimeTM.tm_hour*60 + currTimeTM.tm_min;

            // Has the run time already gone by this week?
            if (scheduledMinuteOfSpan > currMinuteOfSpan)
            {
                // No.  Schedule for today a the specified.
                nextRunTM			= currTimeTM;
                nextRunTM.tm_hour	= minuteOfDay / 60;
                nextRunTM.tm_min	= minuteOfDay % 60;
                nextRunTM.tm_sec	= 0;
				// Advance the day to the specified day of week this week
                nextRunTime = mktime(&nextRunTM);
				nextRunTime += 60*60*24*(dayOfWeek - currTimeTM.tm_wday);
                localtime_s(&nextRunTM, &nextRunTime);
            }
            else
            {
                // Yes.  Schedule for today at the specified time
                nextRunTM			= currTimeTM;
                nextRunTM.tm_hour	= minuteOfDay / 60;
                nextRunTM.tm_min	= minuteOfDay % 60;
                nextRunTM.tm_sec	= 0;
                nextRunTime = mktime(&nextRunTM);
				// Advance the day to the specified day of week next week
                nextRunTime += 60*60*24*((7-currTimeTM.tm_wday) + dayOfWeek);
                localtime_s(&nextRunTM, &nextRunTime);
            }
            break;
        case RepetitionType_Monthly:
			scheduledMinuteOfSpan	= (dayOfMonth*60*24) + minuteOfDay;
			currMinuteOfSpan		= (currTimeTM.tm_mday*60*24) + currTimeTM.tm_hour*60 + currTimeTM.tm_min;

            // Has the run time already gone by this month?
            if (scheduledMinuteOfSpan > currMinuteOfSpan)
            {
                // No.  Schedule for today at the specified time
                nextRunTM			= currTimeTM;
                nextRunTM.tm_hour	= minuteOfDay / 60;
                nextRunTM.tm_min	= minuteOfDay % 60;
                nextRunTM.tm_sec	= 0;
				// Advance the day to later this month
                nextRunTime = mktime(&nextRunTM);
				nextRunTime += 60*60*24*(dayOfMonth - currTimeTM.tm_mday);
                localtime_s(&nextRunTM, &nextRunTime);
			}
            else
            {
                // Yes.  Schedule for today at the specified time
                nextRunTM			= currTimeTM;
                nextRunTM.tm_hour	= minuteOfDay / 60;
                nextRunTM.tm_min	= minuteOfDay % 60;
                nextRunTM.tm_sec	= 0;
				// Advance the day to next month on the specified day
                nextRunTime = mktime(&nextRunTM);
                nextRunTime += 60*60*24*((GetMonthDays(currTimeTM.tm_mon, currTimeTM.tm_year) - currTimeTM.tm_mday) + dayOfMonth);
                localtime_s(&nextRunTM, &nextRunTime);
            }
            break;
        default:
            break;
    }

    nextRunTime = mktime(&nextRunTM);
    if ((time_t) -1 == nextRunTime)
        nextRunTime = 0;

    return nextRunTime;
}

DWORD CSavConfigureableScan::GetMonthDays( int monthNo, int year )
// Returns the number of days in the specified month
{
    DWORD	daysInMonth		= 0;
	bool	isLeapYear		= false;

	isLeapYear = ((((year % 4 == 0 ) && (year % 100 != 0)) || (year % 400 == 0)) ? true : false);

    switch (monthNo)
    {
        case 0:
        case 2:
        case 4:
        case 6:
        case 7:
        case 9:
        case 11:
            daysInMonth = 31;
            break;
        case 1:
            daysInMonth = isLeapYear ? 29 : 28;
            break;
        case 3:
        case 5:
        case 8:
        case 10:
            daysInMonth = 30;
            break;
        default:
            break;
    }

    return daysInMonth;
}

HRESULT CSavConfigureableScan::GetWhatToScan( WhatToScan* toScan )
// Returns what will be scanned - quick scan, entire system, or user specified
{
	DWORD		scanTypeDW			= SCAN_TYPE_CUSTOM;

	// Validate parameter
	if (toScan == NULL)
		return E_POINTER;

	// Read the scan type and convert to a WhatToScan
	CConfigObj config(GetScanConfig());
	scanTypeDW = config.GetOption(_T(szReg_Val_SelectedScanType), SCAN_TYPE_CUSTOM);
	switch (scanTypeDW)
	{
	case SCAN_TYPE_QUICK:
		*toScan = WhatToScan_QuickScanAreas;
		break;
	case SCAN_TYPE_FULL:
		*toScan = WhatToScan_EntireSystem;
		break;
	case SCAN_TYPE_CUSTOM:
		*toScan = WhatToScan_UserSpecified;
		break;
	}
	return S_OK;
}

HRESULT CSavConfigureableScan::SetWhatToScan( WhatToScan newToScan )
{
	BOOL		scanEraserDefs		= FALSE;

	// Save the WhatToScan as a legacy DWORD value
	CConfigObj config(GetScanConfig());
	switch (newToScan)
	{
	case WhatToScan_QuickScanAreas:
		config.SetOption(_T(szReg_Val_SelectedScanType), SCAN_TYPE_QUICK);
		//Get the scan options for memory, loadpoint and eraser definition scanning.
		//Honor user configured changes for only the selectable options.
		scanEraserDefs	= config.GetOption(_T(szReg_Val_ScanERASERDefs), REG_DEFAULT_ScanERASERDefs_QuickScan);
		config.SetOption(_T(szReg_Val_ScanProcesses),  REG_DEFAULT_ScanProcesses_QuickScan);
		config.SetOption(_T(szReg_Val_ScanLoadpoints), REG_DEFAULT_ScanLoadpoints_QuickScan);
		if (scanEraserDefs)
			config.SetOption(_T(szReg_Val_ScanERASERDefs), REG_DEFAULT_ScanERASERDefs_QuickScan);
		// Don't scan all physical drives
		config.SetOption(_T(szReg_Val_ScanAllDrives),  0);
		break;
	case WhatToScan_EntireSystem:
		config.SetOption(_T(szReg_Val_SelectedScanType), SCAN_TYPE_FULL);
		config.SetOption(_T(szReg_Val_SelectedScanType), SCAN_TYPE_FULL);
		config.SetOption(_T(szReg_Val_ScanProcesses), REG_DEFAULT_ScanProcesses_FullScan);
		config.SetOption(_T(szReg_Val_ScanLoadpoints), REG_DEFAULT_ScanLoadpoints_FullScan);
		config.SetOption(_T(szReg_Val_ScanERASERDefs), REG_DEFAULT_ScanERASERDefs_FullScan);
		// Scan all physical drives
		config.SetOption(_T(szReg_Val_ScanAllDrives),  1);
		break;
	case WhatToScan_UserSpecified:
		config.SetOption(_T(szReg_Val_SelectedScanType), SCAN_TYPE_CUSTOM);
		break;
	}
	return S_OK;
}

// Following are only for use during add wizard and so are not exposed via COM
HRESULT CSavConfigureableScan::put_StartType( StartType newStartType )
{
	// Due to the bizzare datatore structure...this is how it's done
	// See comments on put_name for more information
	typeOfScan = newStartType;
	return put_Name(userAssignedName);
}

HRESULT CSavConfigureableScan::put_ID( GUID newID )
{
	id = newID;
	return S_OK;
}

// Configures this object for a new user scan (admin scans cannot be created at this time, likely never)
HRESULT CSavConfigureableScan::CreateNew( void )
{
	TCHAR							scanKeyNameActual[50]		= _T("");
	StringList 						existingScanNames;
	std::wstring::size_type			currNewScanNameNo			= 0;
	CString							newScanName;
	StringList::iterator			currExistingScanName;
	bool							foundNewScanName			= false;
	DWORD							returnValDW					= ERROR_OUT_OF_PAPER;
	HRESULT							returnValHR					= E_FAIL;

    AFX_MANAGE_STATE(AfxGetStaticModuleState());

	// Generate a unique internal ID
	CoCreateGuid(&id);
	returnValDW = RegCreateKeyEx(HKEY_CURRENT_USER, _T(szReg_Key_Main) _T("\\") _T(szReg_Key_CustomTasks), NULL, NULL, NULL, KEY_WRITE, NULL, &parentKeyHandle, NULL);
	if (returnValDW == ERROR_SUCCESS)
	{
		StringFromGUID2(id, scanKeyNameActual, sizeof(scanKeyNameActual)/sizeof(scanKeyNameActual[0]));
		scanKeyNameActual[0] = NULL;
		scanKeyNameActual[37] = NULL;
		scanKeyName = &scanKeyNameActual[1];

		// Configure scan defaults
		isReadOnly = false;
		typeOfScan = StartType_OnDemand;
		scanConfig = GetScanConfig();
		if (scanConfig != NULL)
		{
			CConfigObj config(scanConfig);
			SetWhatToScan(WhatToScan_UserSpecified);
			config.SetOption(_T(szReg_Value_DisplayStatusDlg), 1);
			config.SetOption(_T(szReg_Value_ScanStatus), S_NEVER_RUN);
			returnValHR = S_OK;
		}
		else
		{
			returnValHR = E_FAIL;
		}

		// Finally, create a new scan name.  This is required to determine the type of scan due to the
		// registry datastore format (see put_Name for more info)
		// Create a name for the new scan, based on a template with a number
		if (SUCCEEDED(returnValHR))
		{
			GetScanNames(&existingScanNames);
			returnValHR = E_FAIL;
			for (currNewScanNameNo = 1; (currNewScanNameNo < (existingScanNames.size() + 1)) && FAILED(returnValHR); currNewScanNameNo++)
			{
				newScanName.FormatMessage(IDS_SCAN_DEFAULTNAMETEMPLATE, currNewScanNameNo);
				foundNewScanName = false;
				// Is this scan name already taken?
				for (currExistingScanName = existingScanNames.begin(); (currExistingScanName != existingScanNames.end()) && (!foundNewScanName); currExistingScanName++)
				{
					if (lstrcmpi(newScanName, currExistingScanName->c_str()) == 0)
						foundNewScanName = true;
				}
				if (!foundNewScanName)
					returnValHR = S_OK;
			}
			if (SUCCEEDED(returnValHR))
				returnValHR = put_Name(CComBSTR(newScanName));
		}
	}
	else
	{
		returnValHR = HRESULT_FROM_WIN32(returnValDW);
	}

	return returnValHR;
}

HRESULT CSavConfigureableScan::GetScanNames( StringList* scanNames )
// Sets *scanNames equal to a list of the names of all existing scans
{
	HKEY	scanListKeyHandle	= NULL;
	DWORD	returnValDW		= ERROR_OUT_OF_PAPER;

	// Validate parameter
	if (scanNames == NULL)
		return E_POINTER;
	
	returnValDW = RegOpenKeyEx(HKEY_LOCAL_MACHINE, _T(szReg_Key_Main) _T("\\") _T(szReg_Key_LocalScans), NULL, KEY_READ, &scanListKeyHandle);
	if (returnValDW == ERROR_SUCCESS)
	{
		GetKeySubkeys(scanListKeyHandle, _T(szReg_Value_Scripted), scanNames);
		GetKeySubkeys(scanListKeyHandle, _T(szReg_Value_Scheduled), scanNames);
		GetKeySubkeys(scanListKeyHandle, _T(szReg_Value_Startup), scanNames);
		RegCloseKey(scanListKeyHandle);
		scanListKeyHandle = NULL;
	}

	returnValDW = RegOpenKeyEx(HKEY_CURRENT_USER, _T(szReg_Key_Main) _T("\\") _T(szReg_Key_CustomTasks), NULL, KEY_READ, &scanListKeyHandle);
	if (returnValDW == ERROR_SUCCESS)
	{
		GetKeySubkeys(scanListKeyHandle, _T(szReg_Value_Scripted), scanNames);
		GetKeySubkeys(scanListKeyHandle, _T(szReg_Value_Scheduled), scanNames);
		GetKeySubkeys(scanListKeyHandle, _T(szReg_Value_Startup), scanNames);
		RegCloseKey(scanListKeyHandle);
		scanListKeyHandle = NULL;
	}

	return S_OK;
}

void CSavConfigureableScan::Clear( void )
// Clears all fields and releases all objects - for use in Delete and at shutdown
{
	// Release resources
	if (parentKeyHandle != NULL)
	{
		RegCloseKey(parentKeyHandle);
		parentKeyHandle = NULL;
	}
	scanKeyName.clear();
	if (scanConfig != NULL)
	{
		scanConfig->Release();
		scanConfig = NULL;
	}

	// Clear all fields
	id						= GUID_NULL;
	userAssignedName.Empty();
	userAssignedDescription.Empty();
	isReadOnly				= false;
	typeOfScan				= StartType_OnDemand;
	lastRunTime				= SYSTEMTIME_NULL;
	scheduledRepeatType		= RepetitionType_Daily;
	scheduledRepetitionDay	= 0;
	scheduledStartTime		= SYSTEMTIME_NULL;
	scheduledIsEnabled		= false;
}

// Constructor-destructor
CSavConfigureableScan::CSavConfigureableScan() : parentKeyHandle(NULL),
	scanConfig(NULL),
    id(GUID_NULL),
    isReadOnly(FALSE),
    typeOfScan(StartType_OnDemand),
    lastRunTime(SYSTEMTIME_NULL),
    scheduledRepeatType(RepetitionType_Daily), 
    scheduledRepetitionDay(0),
    scheduledStartTime(SYSTEMTIME_NULL),
    scheduledIsEnabled(FALSE)
{
    // Nothing needed
}

HRESULT CSavConfigureableScan::FinalConstruct()
{
    return trustVerifier.Initialize(CTrustVerifier::VerifyMode_CommonClient);
}

void CSavConfigureableScan::FinalRelease()
{
	Clear();
    trustVerifier.Shutdown();
}
