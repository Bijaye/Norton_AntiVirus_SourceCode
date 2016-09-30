#pragma once
#include "savrt32.h"
#include "ccCriticalSection.h"
#include "ccMutex.h"

#define SAVRT_SUCCEEDED(x) ((x == SAVRT_DRIVER_OK) || (x == SAVRT_DRIVER_ALREADY_LOADED) || (x == SAVRT_DRIVER_FALSE))

// Wrapper for SAVRT32.H .
// This class handles loading SAVRT32.DLL dynamically, checking trust, and loading exported functions.
// Also adds thread safety and smart unloading of the handles used to communicate with SAVRT32.DLL .
//
class CAutoProtectWrapper
{
public:
    // The constructor won't actually talk to SAVRT drivers
    // until you call a function that requires it. If you just
    // need to call options then the drivers won't be loaded 
    // or anything. The only time you should need bUseAllDrivers=true
    // is if you are the AP service.
    //
    CAutoProtectWrapper(bool bUseAllDrivers = false);
    virtual ~CAutoProtectWrapper(void);

	// Unprotect the current thread
	bool DisableAPForThisThread();

	// Re-protect the current thread
	bool EnableAPForThisThread();

    // Are the AP drivers loaded and running OK?
    bool GetAPRunning ();

    // Driver functions exported from SAVRT, basically as-is...
    SAVRT_DRIVER_STATUS RegisterServiceCallback ( PSAVRTCALLBACK pCallback );
    SAVRT_DRIVER_STATUS UnloadDrivers ();
    SAVRT_DRIVER_STATUS LoadDrivers ();
    SAVRT_DRIVER_STATUS ReloadConfig ();
    SAVRT_DRIVER_STATUS ReloadAVAPI ();
    SAVRT_DRIVER_STATUS GetEnabledState ( PBOOL pbEnabled /*out*/);
    SAVRT_DRIVER_STATUS SetEnabledState ( BOOL bEnabled = TRUE );
    SAVRT_DRIVER_STATUS CancelSemiSynchronousScan (SAVRT_ROUS64 hCookie);
    SAVRT_DRIVER_STATUS SetDriversSystemStart ();
    SAVRT_DRIVER_STATUS SetDriversDemandStart ();

private:
	// Handle to savrt32.DLL.
	HINSTANCE m_hAPDLL;

    // Handle to the driver communication handle
    SAVRT_DRIVER_HANDLE m_hDriver;

    // Are all the exports OK?
    bool m_bExportsOK;

    // Reference count to keep track of protect/unprotect matching
    DWORD m_dwRefCount;

    // Critical section for providing thread safety for SAVRT32
    ccLib::CCriticalSection m_csApDrvComm;

    // No locked internal version. If bInit is true it will 
    // initialize if needed.
    //
    bool getAPRunning(bool bInitIfNeeded = true);

    bool m_bUseAllDrivers;

    // protected methods we don't want to expose!!
    SAVRT_DRIVER_STATUS terminateDriverCommunications ();
    SAVRT_DRIVER_STATUS initializeDriverCommunications ();

    pfn_SAVRT_REG_SVC_CB m_RegSvcCb;
    pfn_SAVRT_INIT_DRIVER_COMM m_InitDriverComm;
    pfn_SAVRT_PROTECT_PROCESS m_ProtectProcess;
    pfn_SAVRT_UNPROTECT_PROCESS m_UnprotectProcess;
    pfn_SAVRT_TERM_DRIVER_COMM m_TermDriverComm;

    pfn_SAVRT_EXCLUSION_OPEN m_APExclusionOpen;
    pfn_SAVRT_EXCLUSION_CLOSE m_APExclusionClose;
    pfn_SAVRT_EXCLUSION_GETNEXT m_APExclusionGetNext;
    pfn_SAVRT_EXCLUSION_MODIFY_ADD m_APExclusionModifyAdd;
    pfn_SAVRT_EXCLUSION_MODIFY_REMOVE m_APExclusionModifyRemove;
    pfn_SAVRT_EXCLUSION_MODIFY_EDIT m_APExclusionModifyEdit;
    pfn_SAVRT_EXCLUSION_SET_DEFAULT m_APExclusionSetDefault;
    pfn_SAVRT_EXCLUSION_IS_DIRTY m_APExclusionIsDirty;
    pfn_SAVRT_EXCLUSION_GO_TO_LIST_HEAD m_APExclusionGoToListHead;

    pfn_SAVRT_SET_ENABLED m_SetEnabled;
    pfn_SAVRT_GET_ENABLED m_GetEnabled;
    
    pfn_SAVRT_INIT_DRIVER_COMM_ALL_DRIVERS m_InitDriverCommAllDrivers;
    pfn_SAVRT_AGENT_INIT m_AgentInit;
    pfn_SAVRT_AGENT_DEINIT m_AgentDeinit;
    pfn_SAVRT_RELOAD_CFG m_ReloadCfg;
    pfn_SAVRT_RELOAD_AVAPI m_ReloadAvapi;
    pfn_SAVRT_GET_STATUS m_GetStatus;
    pfn_SAVRT_SVC_THREADS m_SvcThreads;
    pfn_SAVRT_CANCEL_SEMI_SYNCHRONOUS_SCAN m_CancelSemiSyncScan;
    pfn_SAVRT_DRIVERS_ARE_SYSTEM_LOAD m_DriversAreSystemLoad;
    pfn_SAVRT_CAN_SET_SYSTEM_START m_CanSetSystemStart;
    pfn_SAVRT_SET_DRIVERS_SYSTEM_START m_SetDriversSystemStart;
    pfn_SAVRT_SET_DRIVERS_DEMAND_START m_SetDriversDemandStart;
    pfn_SAVRT_LOAD_DRIVERS m_LoadDrivers;
    pfn_SAVRT_UNLOAD_DRIVERS m_UnloadDrivers;
    pfn_SAVRT_SET_SAVRTPEL_SYSTEM_START m_SetSavrtpelSystemStart;
    pfn_SAVRT_UNLOAD_PEL m_UnloadPel;
};

// Our wrapper for options. This class is NOT thread safe because I don't think that the 
// SAVRT32 functions are thread aware.
//
#include "apoptnames.h"

class CAutoProtectOptions
{
public:
    CAutoProtectOptions(void);
    virtual ~CAutoProtectOptions(void);

    // Options - these aren't thread safe since it looks like the SAVRT32 calls aren't thread aware.
    SAVRT_OPTS_STATUS Load();
    SAVRT_OPTS_STATUS Save();
    SAVRT_OPTS_STATUS DefaultAll();
    SAVRT_OPTS_STATUS RestoreDefaultsAndLoad();
    SAVRT_OPTS_STATUS AreOptionsDirty();

    SAVRT_OPTS_STATUS GetDwordValue ( LPCSTR szValueName, LPDWORD pdwValue, DWORD dwDefault );
    SAVRT_OPTS_STATUS GetBinaryValue( LPCSTR szValueName, LPBYTE pBuffer, DWORD dwBufferSize );
    SAVRT_OPTS_STATUS GetStringValue( LPCSTR szValueName, LPSTR szOut, DWORD dwLength, LPSTR szDefault );

    SAVRT_OPTS_STATUS SetStringValue ( LPCSTR szValueName, LPCSTR szData );
    SAVRT_OPTS_STATUS SetDwordValue  ( LPCSTR szValueName, DWORD dwValue );
    SAVRT_OPTS_STATUS SetBinaryValue ( LPCSTR szValueName, LPBYTE pBuffer, DWORD dwDataLength );
    SAVRT_OPTS_STATUS SetDefaultValue( LPCSTR szValueName );

    SAVRT_OPTS_STATUS GetValuesInfo  ( PSAVRT_OPTS_VALUE_INFO* ppOpts, unsigned int* puiCount );
    SAVRT_OPTS_STATUS FreeValuesInfo ( PSAVRT_OPTS_VALUE_INFO pOpts );

protected:
	// Handle to savrt32.DLL.
	HINSTANCE m_hAPDLL;

    // Are all the exports OK?
    bool m_bExportsOK;

    // Critical section for providing thread safety for SAVRT32 options
    ccLib::CCriticalSection m_critOptions;

    // Mutex for providing load/save safety for mulitple processes. 
    // See defect 1-4RE385 and "NavOptHelperEx.h" the old navopts.dat helper.
    ccLib::CMutex m_mutexSAVRTOptions;

    // protected methods we don't want to expose!!
    pfn_SAVRT_OPTIONS_LOAD m_LoadAPOptions;
    pfn_SAVRT_OPTIONS_RESTOREDEFAULTSANDLOAD m_RestoreAPDefaultsAndLoad;
    pfn_SAVRT_OPTIONS_SAVE m_SaveAPOptions;
    pfn_SAVRT_OPTIONS_AREOPTIONSDIRTY m_AreAPOptionsDirty;
    pfn_SAVRT_OPTIONS_SETSTRINGVALUE m_SetAPStringValue;
    pfn_SAVRT_OPTIONS_SETDWORDVALUE m_SetAPDwordValue;
    pfn_SAVRT_OPTIONS_SETBINARYVALUE m_SetAPBinaryValue;
    pfn_SAVRT_OPTIONS_GETSTRINGVALUE m_GetAPStringValue;
    pfn_SAVRT_OPTIONS_GETDWORDVALUE m_GetAPDwordValue;
    pfn_SAVRT_OPTIONS_GETBINARYVALUE m_GetAPBinaryValue;
    pfn_SAVRT_OPTIONS_GETVALUESINFO m_GetAPValuesInfo;
    pfn_SAVRT_OPTIONS_FREEVALUESINFO m_FreeAPValuesInfo;
    pfn_SAVRT_OPTIONS_SETDEFAULTVALUE m_SetAPDefaultValue;
    pfn_SAVRT_OPTIONS_DEFAULTALL m_APDefaultAll;
};