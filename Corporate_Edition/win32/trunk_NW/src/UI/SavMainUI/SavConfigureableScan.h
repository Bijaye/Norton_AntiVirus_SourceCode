// SavConfigureableScan.h : Declaration of the CSavConfigureableScan

#pragma once
#include "resource.h"
#define PROTECTIONPROVIDER_HELPERTYPES_NONAMESPACE
#include "ProtectionProvider.h"
#include "SavMainUI.h"
#include "TrustUtil.h"


// Represents a scan object in the configureable scan collection.  More specifically, this represents a snapshot
// of a scan on disk - it is not intended to be a fully live stated object.
// To use:
// 1.  Create a CSaveConfigureableScan via CSavConfigureableScan::CreateInstance
// 2.  Cast your result pointer to a CSavConfigureableScan* - call this the internalObject pointer.
// 3.  Call either ReadFromRegistry or CreateNew against the internalObject pointer
// 4.  Now call members as needed to retrieve information and manipulate the scan.
class ATL_NO_VTABLE CSavConfigureableScan :
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<CSavConfigureableScan, &CLSID_SavConfigureableScan>,
	public IDispatchImpl<IProtection_ConfigureableScan, &IID_IProtection_ConfigureableScan, &LIBID_ProtectionProviderLib, /*wMajor =*/ 1, /*wMinor =*/ 0>,
	public IDispatchImpl<IProtection_ConfigureableScan_Scheduled, &IID_IProtection_ConfigureableScan_Scheduled, &LIBID_ProtectionProviderLib, /*wMajor =*/ 1, /*wMinor =*/ 0>
{
public:
    // Install COM Registration
    DECLARE_REGISTRY_RESOURCEID(IDR_SAVCONFIGUREABLESCAN)

    // ATL Support
    BEGIN_COM_MAP(CSavConfigureableScan)
	    COM_INTERFACE_ENTRY(IProtection_ConfigureableScan)
        COM_INTERFACE_ENTRY_FUNC(IID_IProtection_ConfigureableScan_Scheduled, NULL, CSavConfigureableScan::ATLfilterIsScheduledScan)
	    COM_INTERFACE_ENTRY2(IDispatch, IProtection_ConfigureableScan)
    END_COM_MAP()

    // IProtection_ConfigureableScan
    STDMETHOD(get_ID)( GUID* scanID );
    STDMETHOD(get_Name)( BSTR *nameBuffer );
    STDMETHOD(put_Name)( BSTR newName );
    STDMETHOD(get_ReadOnly)( VARIANT_BOOL* isReadOnlyArg );
    STDMETHOD(get_StartType)( StartType* typeOfScanArg );
    STDMETHOD(get_Description)( BSTR* description );
    STDMETHOD(put_Description)( BSTR description );
    STDMETHOD(get_LastRunTime)( SYSTEMTIME* lastRunTimeArg );
    STDMETHOD(get_IsRunning)( VARIANT_BOOL* isRunning );
    STDMETHOD(ShowConfigureUI)( HWND parentWindow );
    STDMETHOD(ShowLogUI)( HWND parentWindow );
    STDMETHOD(RunScan)( HWND parentWindow );

    // IProtection_ConfigureableScan_Scheduled
    STDMETHOD(get_RepeatType)( RepetitionType* type );
    STDMETHOD(get_RepetitionDay)( unsigned int* day );
    STDMETHOD(get_StartTime)( SYSTEMTIME* startTime );
    // Scheduled scans may be enabled or disabled - return which
    STDMETHOD(get_Enabled)( VARIANT_BOOL* isEnabled );
    // Sets *nextRunTimeStart equal to the start of the next time this scan will run.
    // If scan windows are supported, also sets *supportsWindows to true and *nextRunTime to the end of the window
    STDMETHOD(GetNextRunTime)( SYSTEMTIME* nextRunTimeStart, VARIANT_BOOL* supportsWindows, unsigned int* scanWindowRunHours );

    // ATL Filter function.  If this is a scheduled scan, permits IProtection_ConfigureableScan_Scheduled to be exposed, otherwise does not.
    static HRESULT WINAPI ATLfilterIsScheduledScan( void* currObject, REFIID interfaceID, LPVOID* requestedInterfacePtr, DWORD userArgument );
    // Returns TRUE if this is a scheduled scan object, else FALSE
    bool IsScheduledScan( void );

    // Private APIs, used by ConfigureableScanCollection
    // *** DATA TYPES **
    // Differentiates where this scan resides in the registry since this is needed to use the proper key for the LDVP
    // config objects
	typedef enum {WhatToScan_QuickScanAreas, WhatToScan_EntireSystem, WhatToScan_UserSpecified} WhatToScan;

    // Reads data for this object in from the registry.
	// To refresh data instead, call with readParentKeyHandle NULL and readScanKeyName NULL
    HRESULT ReadFromRegistry( HKEY parentKeyHandle, LPCTSTR scanKeyName, bool readIsReadOnly );
    // Saves data to registry
    HRESULT WriteToRegistry( void );
    // Swiped from MISC.CPP CalculateFirstRunTime and updated
    time_t CalculateNextRunTime( int minuteOfDay, int dayOfWeek, int dayOfMonth, time_t baseTime, RepetitionType scanType );
    // Read the name and description for this scan from the specified Taskpad subkey.
    // Returns S_FALSE if there is no subkey for this scan in the specified taskpad subkey, S_OK if it exists and sets
    // *userAssignedName and *userAssignedDescription, else the error code of the failure
    HRESULT ReadTaskpadValues( LPCTSTR taskpadKeyName, BSTR* userAssignedName, BSTR* userAssignedDescription );
    // Deletes the TaskPad values for this scan from the specified Taskpad key
	HRESULT DeleteTaskpadValues( LPCTSTR taskpadKeyName, LPCTSTR userAssignedName, LPCTSTR userAssignedDescription );
    // Writes out the Taskpad values for the scan to the specified Taskpad key
	HRESULT WriteTaskpadValues( LPCTSTR taskpadKeyName, LPCTSTR userAssignedName, LPCTSTR userAssignedDescription );
    // Delete this scan object from persistent storage
    HRESULT Delete( void );
    // Creates an IConfig configuration object configured for this scan's datastore in the registry
	// This object is owned by this scan - do NOT call Release on it!
    IScanConfig* GetScanConfig( void );
	// Returns the number of days in the specified month
	DWORD GetMonthDays( int monthNo, int year );
	HRESULT GetWhatToScan( WhatToScan* toScan );
	HRESULT SetWhatToScan( WhatToScan newToScan );
	// Following are only for use during add wizard and so are not exposed via COM
	HRESULT put_StartType( StartType newStartType );
	HRESULT put_ID( GUID newID );
	// Configures this object for a new user scan (admin scans cannot be created at this time, likely never)
	HRESULT CreateNew( void );
	// Sets *scanNames equal to a list of the names of all existing scans
	HRESULT GetScanNames( StringList* scanNames );
	// Clears all fields and releases all objects - for use in Delete and at shutdown
	void Clear( void );

    // Constructor-destructor
	CSavConfigureableScan();
	DECLARE_PROTECT_FINAL_CONSTRUCT()
	HRESULT FinalConstruct();
	void FinalRelease();
private:
    // Scan storage
    CTrustVerifier  trustVerifier;
    HKEY            parentKeyHandle;
    std::wstring    scanKeyName;
	IScanConfig*	scanConfig;

    // IProtection_ConfigureableScan
    GUID            id;
    CComBSTR        userAssignedName;
    CComBSTR        userAssignedDescription;
    VARIANT_BOOL    isReadOnly;
    StartType       typeOfScan;
    SYSTEMTIME      lastRunTime;

    // IProtection_ConfigureableScan_Scheduled Specific
    RepetitionType  scheduledRepeatType;
    unsigned int    scheduledRepetitionDay;
    SYSTEMTIME      scheduledStartTime;
    VARIANT_BOOL    scheduledIsEnabled;
};

OBJECT_ENTRY_NON_CREATEABLE_EX_AUTO(__uuidof(SavConfigureableScan), CSavConfigureableScan)
