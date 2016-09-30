#include "Stdafx.h"
#include "NAVInfo.h"
#include <stdexcept>
#include "navtrust.h"
#include "autoprotectwrapper.h"

CAutoProtectWrapper::CAutoProtectWrapper(bool bUseAllDrivers /*false*/) :
	m_hAPDLL(NULL),
    m_APExclusionOpen(NULL),
    m_APExclusionClose(NULL),
    m_APExclusionGetNext(NULL),
    m_APExclusionModifyAdd(NULL),
    m_APExclusionModifyRemove(NULL),
    m_APExclusionModifyEdit(NULL),
    m_APExclusionSetDefault(NULL),
    m_APExclusionIsDirty(NULL),
    m_APExclusionGoToListHead(NULL),
    m_InitDriverComm(NULL),
    m_ProtectProcess(NULL),
    m_UnprotectProcess(NULL),
    m_TermDriverComm(NULL),
    m_SetEnabled(NULL),
    m_GetEnabled(NULL),
    m_InitDriverCommAllDrivers(NULL),
    m_AgentInit(NULL),
    m_AgentDeinit(NULL),
    m_ReloadCfg(NULL),
    m_ReloadAvapi(NULL),
    m_GetStatus(NULL),
    m_SvcThreads(NULL),
    m_RegSvcCb(NULL),
    m_CancelSemiSyncScan(NULL),
    m_DriversAreSystemLoad(NULL),
    m_CanSetSystemStart(NULL),
    m_SetDriversSystemStart(NULL),
    m_SetDriversDemandStart(NULL),
    m_LoadDrivers(NULL),
    m_UnloadDrivers(NULL),
    m_SetSavrtpelSystemStart(NULL),
    m_UnloadPel(NULL),
    m_hDriver(NULL),
    m_dwRefCount(0),
    m_bUseAllDrivers(bUseAllDrivers),
    m_bExportsOK(false)
{
	try
	{
        ccLib::CSingleLock lock (&m_csApDrvComm, INFINITE, FALSE );
        // Attempt to load savrt32.dll and initialize driver communication
		CNAVInfo NAVInfo;
		TCHAR szDLLPath[MAX_PATH];
		_stprintf(szDLLPath, _T("%s\\%s"), NAVInfo.GetNAVDir(), SAVRT_DLL_NAME);

		m_hAPDLL = LoadLibraryEx(szDLLPath, NULL, LOAD_WITH_ALTERED_SEARCH_PATH);
		DWORD dwError = GetLastError();
		if (m_hAPDLL == NULL)
        {
			CCTRACEE("CAutoProtectWrapper::CAutoProtectWrapper() - Error unable to load %s. GetLastError()=%d", szDLLPath, dwError);
            return;
        }

        // Verify the signature
        if( NAVToolbox::NAVTRUST_OK != NAVToolbox::IsSymantecSignedImage(szDLLPath) )
        {
            CCTRACEE ("CAutoProtectWrapper::CAutoProtectWrapper() - failed trust on %s", szDLLPath);
            return;
        }

        // Now get the function pointers
        m_APExclusionOpen = reinterpret_cast<pfn_SAVRT_EXCLUSION_OPEN>(::GetProcAddress(m_hAPDLL, reinterpret_cast<LPCTSTR>(SAVRT_EXCLUSION_OPEN)));
        m_APExclusionClose = reinterpret_cast<pfn_SAVRT_EXCLUSION_CLOSE>(::GetProcAddress(m_hAPDLL, reinterpret_cast<LPCTSTR>(SAVRT_EXCLUSION_CLOSE)));
        m_APExclusionGetNext = reinterpret_cast<pfn_SAVRT_EXCLUSION_GETNEXT>(::GetProcAddress(m_hAPDLL, reinterpret_cast<LPCTSTR>(SAVRT_EXCLUSION_GETNEXT)));
        m_APExclusionModifyAdd = reinterpret_cast<pfn_SAVRT_EXCLUSION_MODIFY_ADD>(::GetProcAddress(m_hAPDLL, reinterpret_cast<LPCTSTR>(SAVRT_EXCLUSION_MODIFY_ADD)));
        m_APExclusionModifyRemove = reinterpret_cast<pfn_SAVRT_EXCLUSION_MODIFY_REMOVE>(::GetProcAddress(m_hAPDLL, reinterpret_cast<LPCTSTR>(SAVRT_EXCLUSION_MODIFY_REMOVE)));
        m_APExclusionModifyEdit = reinterpret_cast<pfn_SAVRT_EXCLUSION_MODIFY_EDIT>(::GetProcAddress(m_hAPDLL, reinterpret_cast<LPCTSTR>(SAVRT_EXCLUSION_MODIFY_EDIT)));
        m_APExclusionSetDefault = reinterpret_cast<pfn_SAVRT_EXCLUSION_SET_DEFAULT>(::GetProcAddress(m_hAPDLL, reinterpret_cast<LPCTSTR>(SAVRT_EXCLUSION_SET_DEFAULT)));
        m_APExclusionIsDirty = reinterpret_cast<pfn_SAVRT_EXCLUSION_IS_DIRTY>(::GetProcAddress(m_hAPDLL, reinterpret_cast<LPCTSTR>(SAVRT_EXCLUSION_IS_DIRTY)));
        m_APExclusionGoToListHead = reinterpret_cast<pfn_SAVRT_EXCLUSION_GO_TO_LIST_HEAD>(::GetProcAddress(m_hAPDLL, reinterpret_cast<LPCTSTR>(SAVRT_EXCLUSION_GO_TO_LIST_HEAD)));

        m_InitDriverComm = reinterpret_cast<pfn_SAVRT_INIT_DRIVER_COMM>(::GetProcAddress(m_hAPDLL, reinterpret_cast<LPCSTR>(SAVRT_INIT_DRIVER_COMM)));
        m_TermDriverComm = reinterpret_cast<pfn_SAVRT_TERM_DRIVER_COMM>(::GetProcAddress(m_hAPDLL, reinterpret_cast<LPCSTR>(SAVRT_TERM_DRIVER_COMM)));
        m_ProtectProcess = reinterpret_cast<pfn_SAVRT_PROTECT_PROCESS>(::GetProcAddress(m_hAPDLL, reinterpret_cast<LPCSTR>(SAVRT_PROTECT_PROCESS)));
        m_UnprotectProcess = reinterpret_cast<pfn_SAVRT_UNPROTECT_PROCESS>(::GetProcAddress(m_hAPDLL, reinterpret_cast<LPCSTR>(SAVRT_UNPROTECT_PROCESS)));

        m_InitDriverCommAllDrivers = reinterpret_cast<pfn_SAVRT_INIT_DRIVER_COMM_ALL_DRIVERS>(::GetProcAddress(m_hAPDLL, reinterpret_cast<LPCSTR>(SAVRT_INIT_DRIVER_COMM_ALL_DRIVERS)));
        m_AgentInit = reinterpret_cast<pfn_SAVRT_AGENT_INIT>(::GetProcAddress(m_hAPDLL, reinterpret_cast<LPCSTR>(SAVRT_AGENT_INIT)));
        m_AgentDeinit = reinterpret_cast<pfn_SAVRT_AGENT_DEINIT>(::GetProcAddress(m_hAPDLL, reinterpret_cast<LPCSTR>(SAVRT_AGENT_DEINIT)));
        m_ReloadCfg = reinterpret_cast<pfn_SAVRT_RELOAD_CFG>(::GetProcAddress(m_hAPDLL, reinterpret_cast<LPCSTR>(SAVRT_RELOAD_CFG)));
        m_ReloadAvapi = reinterpret_cast<pfn_SAVRT_RELOAD_AVAPI>(::GetProcAddress(m_hAPDLL, reinterpret_cast<LPCSTR>(SAVRT_RELOAD_AVAPI)));
        m_SetEnabled = reinterpret_cast<pfn_SAVRT_SET_ENABLED>(::GetProcAddress(m_hAPDLL, reinterpret_cast<LPCSTR>(SAVRT_SET_ENABLED)));
        m_GetEnabled = reinterpret_cast<pfn_SAVRT_GET_ENABLED>(::GetProcAddress(m_hAPDLL, reinterpret_cast<LPCSTR>(SAVRT_GET_ENABLED)));
        m_GetStatus = reinterpret_cast<pfn_SAVRT_GET_STATUS>(::GetProcAddress(m_hAPDLL, reinterpret_cast<LPCSTR>(SAVRT_GET_STATUS)));
        m_SvcThreads = reinterpret_cast<pfn_SAVRT_SVC_THREADS>(::GetProcAddress(m_hAPDLL, reinterpret_cast<LPCSTR>(SAVRT_SVC_THREADS)));
        m_RegSvcCb = reinterpret_cast<pfn_SAVRT_REG_SVC_CB>(::GetProcAddress(m_hAPDLL, reinterpret_cast<LPCSTR>(SAVRT_REG_SVC_CB)));
        m_CancelSemiSyncScan = reinterpret_cast<pfn_SAVRT_CANCEL_SEMI_SYNCHRONOUS_SCAN>(::GetProcAddress(m_hAPDLL, reinterpret_cast<LPCSTR>(SAVRT_CANCEL_SEMI_SYNCHRONOUS_SCAN)));
        m_DriversAreSystemLoad = reinterpret_cast<pfn_SAVRT_DRIVERS_ARE_SYSTEM_LOAD>(::GetProcAddress(m_hAPDLL, reinterpret_cast<LPCSTR>(SAVRT_DRIVERS_ARE_SYSTEM_LOAD)));
        m_CanSetSystemStart = reinterpret_cast<pfn_SAVRT_CAN_SET_SYSTEM_START>(::GetProcAddress(m_hAPDLL, reinterpret_cast<LPCSTR>(SAVRT_CAN_SET_SYSTEM_START)));
        m_SetDriversSystemStart = reinterpret_cast<pfn_SAVRT_SET_DRIVERS_SYSTEM_START>(::GetProcAddress(m_hAPDLL, reinterpret_cast<LPCSTR>(SAVRT_SET_DRIVERS_SYSTEM_START)));
        m_SetDriversDemandStart = reinterpret_cast<pfn_SAVRT_SET_DRIVERS_DEMAND_START>(::GetProcAddress(m_hAPDLL, reinterpret_cast<LPCSTR>(SAVRT_SET_DRIVERS_DEMAND_START)));
        m_LoadDrivers = reinterpret_cast<pfn_SAVRT_LOAD_DRIVERS>(::GetProcAddress(m_hAPDLL, reinterpret_cast<LPCSTR>(SAVRT_LOAD_DRIVERS)));
        m_SetSavrtpelSystemStart = reinterpret_cast<pfn_SAVRT_SET_SAVRTPEL_SYSTEM_START>(::GetProcAddress(m_hAPDLL, reinterpret_cast<LPCSTR>(SAVRT_SET_SAVRTPEL_SYSTEM_START)));
        m_UnloadPel = reinterpret_cast<pfn_SAVRT_UNLOAD_PEL>(::GetProcAddress(m_hAPDLL, reinterpret_cast<LPCSTR>(SAVRT_UNLOAD_PEL)));

        if( m_APExclusionOpen && m_APExclusionClose && m_APExclusionGetNext &&
            m_APExclusionGoToListHead && m_APExclusionModifyAdd && m_APExclusionModifyRemove &&
            m_APExclusionModifyEdit && m_APExclusionSetDefault && m_APExclusionIsDirty && 
            m_InitDriverComm && m_ProtectProcess && m_UnprotectProcess && m_TermDriverComm &&
            m_InitDriverCommAllDrivers && m_AgentInit && m_AgentDeinit && m_ReloadCfg && m_ReloadAvapi &&
            m_SetEnabled && m_GetEnabled && m_GetStatus && m_SvcThreads && m_RegSvcCb && 
            m_CancelSemiSyncScan && m_DriversAreSystemLoad && m_CanSetSystemStart && m_SetDriversSystemStart &&
            m_SetDriversDemandStart && m_LoadDrivers && m_SetSavrtpelSystemStart && m_UnloadPel )
        {
            m_bExportsOK = true;
        }
        else
		{
            CCTRACEE("CAutoProtectWrapper::CAutoProtectWrapper() - missing export");            
		}

	}
	catch(exception& Ex)
	{
		CCTRACEE(Ex.what());
	}
}

CAutoProtectWrapper::~CAutoProtectWrapper(void)
{
    ccLib::CSingleLock lock (&m_csApDrvComm, INFINITE, FALSE );

    if( m_dwRefCount != 0 )
    {
        CCTRACEE(_T("CAutoProtectWrapper::CAutoProtectWrapper() : There are still %d threads unprotected!"), m_dwRefCount);
    }

    terminateDriverCommunications();

	// Make sure Savrt32.DLL is unloaded.
	if (m_hAPDLL != NULL)
		FreeLibrary(m_hAPDLL);
}

SAVRT_DRIVER_STATUS CAutoProtectWrapper::terminateDriverCommunications ()
{
    ccLib::CSingleLock lock (&m_csApDrvComm, INFINITE, FALSE );

    if (!getAPRunning(false))
        return SAVRT_DRIVER_OK;

    SAVRT_DRIVER_STATUS status = m_TermDriverComm(m_hDriver);
    if( SAVRT_DRIVER_OK != status )
        CCTRACEE(_T("CAutoProtectWrapper::CAutoProtectWrapper() : SAVRT_TerminateDriverCommunications() failed. %d"), status);

    m_hDriver = NULL;

    return status;
}

SAVRT_DRIVER_STATUS CAutoProtectWrapper::initializeDriverCommunications ()
{
    ccLib::CSingleLock lock (&m_csApDrvComm, INFINITE, FALSE );
    //
    // *** DO NOT use SAVRT_InitializeDriverCommunicationsAllDrivers() in this case because
    // the SAVRT_PEL driver might not be running to accept communication. See defect 1-3J4ATO.
    //

    // Attempt to initialize driver communication
    SAVRT_DRIVER_STATUS status = SAVRT_DRIVER_ERROR;

    if ( m_bUseAllDrivers )
        status = m_InitDriverCommAllDrivers(SAVRT32_VERSION, &m_hDriver);
    else
        status = m_InitDriverComm(SAVRT32_VERSION, &m_hDriver);

    if( status != SAVRT_DRIVER_OK )
    {
        CCTRACEE("CAutoProtectWrapper::CAutoProtectWrapper() - Error Savrt_InitializeDriverCommunications() failed. %d %d", m_bUseAllDrivers, status);
    }

    return status;
}

bool CAutoProtectWrapper::DisableAPForThisThread()
{
    ccLib::CSingleLock lock (&m_csApDrvComm, INFINITE, FALSE );

    // If AP SAVRT driver isn't running, lie! See defects 1-3JNRUT, 1-3J4ATO
    if (!getAPRunning())
        return true;

    // Unprotect the current thread.
    SAVRT_DRIVER_STATUS status = m_UnprotectProcess(m_hDriver);
	if( SAVRT_DRIVER_OK == status )
    {
        m_dwRefCount++;
        CCTRACEI(_T("CAutoProtectWrapper::Register() - Success for thread ID: %d. Total threads: %d"), GetCurrentThreadId(), m_dwRefCount);
    }
    else
    {
        CCTRACEE("CAutoProtectWrapper::Register() - Error function failed. %d", status);
        return false;
    }

	return true;
}

bool CAutoProtectWrapper::EnableAPForThisThread()
{
    ccLib::CSingleLock lock (&m_csApDrvComm, INFINITE, FALSE );

    // If AP SAVRT driver isn't running, lie! See defects 1-3JNRUT, 1-3J4ATO
    if (!getAPRunning())
        return true;

    // Protect the current thread.
    SAVRT_DRIVER_STATUS status = m_ProtectProcess(m_hDriver);
	if( SAVRT_DRIVER_OK == status )
    {
        m_dwRefCount--;
        CCTRACEI(_T("CAutoProtectWrapper::Unregister() - Success for thread ID: %d. Total threads: %d"), GetCurrentThreadId(), m_dwRefCount);
    }
    else
    {
        CCTRACEE("CAutoProtectWrapper::Unregister() - Error function failed. %d", status);
        return false;
    }

	return true;
}

// If the client wants to know if AP is running then we need to at least try to connect.
//
bool CAutoProtectWrapper::GetAPRunning ()
{
    ccLib::CSingleLock lock (&m_csApDrvComm, INFINITE, FALSE );
    return getAPRunning(true);
}

// No Lock!!!!
bool CAutoProtectWrapper::getAPRunning (bool bInitIfNeeded /*true*/)
{
    // Validate the exports
    if( !m_bExportsOK)
    {
        CCTRACEE("CAutoProtectWrapper::getAPRunning() - bad exports");
        return false;
    }

    // Validate the driver handle
    if( !m_hDriver)
    {
        if ( SAVRT_DRIVER_OK != initializeDriverCommunications())
        {
            CCTRACEE("CAutoProtectWrapper::getAPRunning() - Error: invalid driver handle");
            return false;
        }
    }

    return true;
}


SAVRT_DRIVER_STATUS CAutoProtectWrapper::RegisterServiceCallback ( PSAVRTCALLBACK pCallback )
{
    ccLib::CSingleLock lock (&m_csApDrvComm, INFINITE, FALSE );
    if (!getAPRunning())
        return SAVRT_DRIVER_ERROR;

    return m_RegSvcCb ( m_hDriver, pCallback );
}

SAVRT_DRIVER_STATUS CAutoProtectWrapper::UnloadDrivers ()
{
    ccLib::CSingleLock lock (&m_csApDrvComm, INFINITE, FALSE );
    if (!getAPRunning())
        return SAVRT_DRIVER_ERROR;

    return m_UnloadDrivers ();
}

SAVRT_DRIVER_STATUS CAutoProtectWrapper::LoadDrivers ()
{
    ccLib::CSingleLock lock (&m_csApDrvComm, INFINITE, FALSE );

    // Validate the exports
    if( !m_bExportsOK)
    {
        CCTRACEE("CAutoProtectWrapper::getAPRunning() - bad exports");
        return SAVRT_DRIVER_ERROR;
    }

    // Validate the driver handle
    if( !m_hDriver)
    {
        // If we have a driver handle that means we are loaded and comm is init'ed
        //
        SAVRT_DRIVER_STATUS status = m_LoadDrivers();

        if ( SAVRT_DRIVER_OK != status &&
             SAVRT_DRIVER_ALREADY_LOADED != status)
        {
            CCTRACEE ( "CAutoProtectWrapper::loadDrivers() - error %d", status );
            return status;
        }
    }

    return SAVRT_DRIVER_OK;
}

SAVRT_DRIVER_STATUS CAutoProtectWrapper::SetEnabledState ( BOOL bEnabled /*TRUE*/)
{
    ccLib::CSingleLock lock (&m_csApDrvComm, INFINITE, FALSE );
    if (!getAPRunning())
        return SAVRT_DRIVER_ERROR;

    return m_SetEnabled (m_hDriver, bEnabled);
}

SAVRT_DRIVER_STATUS CAutoProtectWrapper::ReloadConfig ()
{
    ccLib::CSingleLock lock (&m_csApDrvComm, INFINITE, FALSE );
    if (!getAPRunning())
        return SAVRT_DRIVER_ERROR;

    return m_ReloadCfg (m_hDriver);
}

SAVRT_DRIVER_STATUS CAutoProtectWrapper::ReloadAVAPI ()
{
    ccLib::CSingleLock lock (&m_csApDrvComm, INFINITE, FALSE );
    if (!getAPRunning())
        return SAVRT_DRIVER_ERROR;

    return m_ReloadAvapi (m_hDriver);
}

SAVRT_DRIVER_STATUS CAutoProtectWrapper::GetEnabledState ( PBOOL pbEnabled /*out*/)
{
    ccLib::CSingleLock lock (&m_csApDrvComm, INFINITE, FALSE );
    if (!getAPRunning())
        return SAVRT_DRIVER_ERROR;

    return m_GetEnabled(m_hDriver, pbEnabled);
}

SAVRT_DRIVER_STATUS CAutoProtectWrapper::CancelSemiSynchronousScan (SAVRT_ROUS64 hCookie)
{
    ccLib::CSingleLock lock (&m_csApDrvComm, INFINITE, FALSE );
    if (!getAPRunning())
        return SAVRT_DRIVER_ERROR;

    return m_CancelSemiSyncScan(m_hDriver, hCookie);
}

SAVRT_DRIVER_STATUS CAutoProtectWrapper::SetDriversSystemStart ()
{
    ccLib::CSingleLock lock (&m_csApDrvComm, INFINITE, FALSE );

    // Validate the exports
    if( !m_bExportsOK)
    {
        CCTRACEE("CAutoProtectWrapper::SetDriversSystemStart() - bad exports");
        return SAVRT_DRIVER_ERROR;
    }

    SAVRT_DRIVER_STATUS status = m_SetDriversSystemStart();
    if ( SAVRT_SUCCEEDED ( status ))
        CCTRACEI ( "CAutoProtectWrapper::SetDriversSystemStart - Success - %d", status );
    else
        CCTRACEE ( "CAutoProtectWrapper::SetDriversSystemStart - Failed - %d", status );

    return status;
}

SAVRT_DRIVER_STATUS CAutoProtectWrapper::SetDriversDemandStart ()
{
    ccLib::CSingleLock lock (&m_csApDrvComm, INFINITE, FALSE );

    // Validate the exports
    if( !m_bExportsOK)
    {
        CCTRACEE("CAutoProtectWrapper::SetDriversDemandStart() - bad exports");
        return SAVRT_DRIVER_ERROR;
    }

    SAVRT_DRIVER_STATUS status = m_SetDriversDemandStart();
    if ( SAVRT_SUCCEEDED ( status ))
        CCTRACEI ( "CAutoProtectWrapper::SetDriversDemandStart - Success - %d", status );
    else
        CCTRACEE ( "CAutoProtectWrapper::SetDriversDemandStart - Failed - %d", status );

    return status;
}


// *****************************************************************************************
//
//                      Options
//
// *****************************************************************************************
CAutoProtectOptions::CAutoProtectOptions(void):
	m_hAPDLL(NULL),
    m_LoadAPOptions(NULL),
    m_RestoreAPDefaultsAndLoad(NULL),
    m_SaveAPOptions(NULL),
    m_AreAPOptionsDirty(NULL),
    m_SetAPStringValue(NULL),
    m_SetAPDwordValue(NULL),
    m_SetAPBinaryValue(NULL),
    m_GetAPStringValue(NULL),
    m_GetAPDwordValue(NULL),
    m_GetAPBinaryValue(NULL),
    m_GetAPValuesInfo(NULL),
    m_FreeAPValuesInfo(NULL),
    m_SetAPDefaultValue(NULL),
    m_APDefaultAll(NULL),    
    m_bExportsOK(false)
{
	try
	{
        // Attempt to load savrt32.dll and initialize driver communication
		CNAVInfo NAVInfo;
		TCHAR szDLLPath[MAX_PATH];
		_stprintf(szDLLPath, _T("%s\\%s"), NAVInfo.GetNAVDir(), SAVRT_DLL_NAME);

		m_hAPDLL = LoadLibraryEx(szDLLPath, NULL, LOAD_WITH_ALTERED_SEARCH_PATH);

		if (m_hAPDLL == NULL)
        {
			CCTRACEE("CAutoProtectOptions::CAutoProtectOptions() - Error unable to load.");
            return;
        }

        // Verify the signature
        if( NAVToolbox::NAVTRUST_OK != NAVToolbox::IsSymantecSignedImage(szDLLPath) )
        {
            CCTRACEE ("CAutoProtectOptions::CAutoProtectOptions() - failed trust");
            return;
        }

        // Now get the function pointers
        m_LoadAPOptions = reinterpret_cast<pfn_SAVRT_OPTIONS_LOAD>(::GetProcAddress(m_hAPDLL, reinterpret_cast<LPCTSTR>(SAVRT_OPTIONS_LOAD)));
		m_RestoreAPDefaultsAndLoad = reinterpret_cast<pfn_SAVRT_OPTIONS_RESTOREDEFAULTSANDLOAD>(::GetProcAddress(m_hAPDLL, reinterpret_cast<LPCTSTR>(SAVRT_OPTIONS_RESTOREDEFAULTSANDLOAD)));
        m_SaveAPOptions = reinterpret_cast<pfn_SAVRT_OPTIONS_SAVE>(::GetProcAddress(m_hAPDLL, reinterpret_cast<LPCTSTR>(SAVRT_OPTIONS_SAVE)));
        m_AreAPOptionsDirty = reinterpret_cast<pfn_SAVRT_OPTIONS_AREOPTIONSDIRTY>(::GetProcAddress(m_hAPDLL, reinterpret_cast<LPCTSTR>(SAVRT_OPTIONS_AREOPTIONSDIRTY)));
        m_SetAPStringValue = reinterpret_cast<pfn_SAVRT_OPTIONS_SETSTRINGVALUE>(::GetProcAddress(m_hAPDLL, reinterpret_cast<LPCTSTR>(SAVRT_OPTIONS_SETSTRINGVALUE)));
        m_SetAPDwordValue = reinterpret_cast<pfn_SAVRT_OPTIONS_SETDWORDVALUE>(::GetProcAddress(m_hAPDLL, reinterpret_cast<LPCTSTR>(SAVRT_OPTIONS_SETDWORDVALUE)));
        m_SetAPBinaryValue = reinterpret_cast<pfn_SAVRT_OPTIONS_SETBINARYVALUE>(::GetProcAddress(m_hAPDLL, reinterpret_cast<LPCTSTR>(SAVRT_OPTIONS_SETBINARYVALUE)));
        m_GetAPStringValue = reinterpret_cast<pfn_SAVRT_OPTIONS_GETSTRINGVALUE>(::GetProcAddress(m_hAPDLL, reinterpret_cast<LPCTSTR>(SAVRT_OPTIONS_GETSTRINGVALUE)));
        m_GetAPDwordValue = reinterpret_cast<pfn_SAVRT_OPTIONS_GETDWORDVALUE>(::GetProcAddress(m_hAPDLL, reinterpret_cast<LPCTSTR>(SAVRT_OPTIONS_GETDWORDVALUE)));
        m_GetAPBinaryValue = reinterpret_cast<pfn_SAVRT_OPTIONS_GETBINARYVALUE>(::GetProcAddress(m_hAPDLL, reinterpret_cast<LPCTSTR>(SAVRT_OPTIONS_GETBINARYVALUE)));
        m_GetAPValuesInfo = reinterpret_cast<pfn_SAVRT_OPTIONS_GETVALUESINFO>(::GetProcAddress(m_hAPDLL, reinterpret_cast<LPCTSTR>(SAVRT_OPTIONS_GETVALUESINFO)));
        m_FreeAPValuesInfo = reinterpret_cast<pfn_SAVRT_OPTIONS_FREEVALUESINFO>(::GetProcAddress(m_hAPDLL, reinterpret_cast<LPCTSTR>(SAVRT_OPTIONS_FREEVALUESINFO)));
        m_SetAPDefaultValue = reinterpret_cast<pfn_SAVRT_OPTIONS_SETDEFAULTVALUE>(::GetProcAddress(m_hAPDLL, reinterpret_cast<LPCTSTR>(SAVRT_OPTIONS_SETDEFAULTVALUE)));
        m_APDefaultAll = reinterpret_cast<pfn_SAVRT_OPTIONS_DEFAULTALL>(::GetProcAddress(m_hAPDLL, reinterpret_cast<LPCTSTR>(SAVRT_OPTIONS_DEFAULTALL)));
       

        if( m_LoadAPOptions && m_SaveAPOptions && m_AreAPOptionsDirty && m_SetAPStringValue &&
            m_SetAPDwordValue && m_SetAPBinaryValue && m_GetAPStringValue && m_GetAPDwordValue &&
            m_GetAPBinaryValue && m_GetAPValuesInfo && m_FreeAPValuesInfo && m_SetAPDefaultValue &&
            m_APDefaultAll )
        {
            // Build our mutex but don't lock it.
            // 1-4RE385
	        // Setup the descriptor.
	        SECURITY_ATTRIBUTES sa		= {0}; 
	        SECURITY_DESCRIPTOR sdEvent = {0};
        	
            InitializeSecurityDescriptor(&sdEvent, SECURITY_DESCRIPTOR_REVISION);

	        // Add a null DACL to the security descriptor. 
	        SetSecurityDescriptorDacl(&sdEvent, TRUE, (PACL) NULL, FALSE);
	        sa.nLength = sizeof(sa); 
	        sa.lpSecurityDescriptor = &sdEvent;
	        sa.bInheritHandle = TRUE;

            if ( !m_mutexSAVRTOptions.Create ( &sa, FALSE, "SYM_NAV_SAVRT_OPTIONS_MUTEX_GLOBAL", TRUE))
            {
                CCTRCTXI0 ( "Failed to create mutex, trying open" );
                if ( !m_mutexSAVRTOptions.Open ( SYNCHRONIZE, FALSE, "SYM_NAV_SAVRT_OPTIONS_MUTEX_GLOBAL", TRUE))
                {
                    CCTRCTXE0 ( "Failed to open mutex" );
                    return;
                }
            }

            m_bExportsOK = true;
        }
        else
            CCTRACEE("CAutoProtectOptions::CAutoProtectOptions() - missing export");            

	}
	catch(exception& Ex)
	{
		CCTRACEE(Ex.what());
	}
}

CAutoProtectOptions::~CAutoProtectOptions(void)
{
	// Make sure Savrt32.DLL is unloaded.
	if (m_hAPDLL != NULL)
		FreeLibrary(m_hAPDLL);
}

SAVRT_OPTS_STATUS CAutoProtectOptions::Load()
{
    if (m_bExportsOK)
    {
        ccLib::CSingleLock lock (&m_critOptions, INFINITE, FALSE );

        SAVRT_OPTS_STATUS statusReturn = SAVRT_OPTS_ERROR;
        CCTRCTXI0 ( "Locking SAVRT options" );

        m_mutexSAVRTOptions.Lock(INFINITE,FALSE);
        statusReturn = m_LoadAPOptions ();
        m_mutexSAVRTOptions.Unlock();

        CCTRCTXI0 ( "Releasing lock on SAVRT options" );
        return statusReturn;
    }

    return SAVRT_OPTS_ERROR;
}

SAVRT_OPTS_STATUS CAutoProtectOptions::GetDwordValue ( LPCSTR szValueName, LPDWORD pdwValue, DWORD dwDefault )
{
    if (m_bExportsOK)
    {
        ccLib::CSingleLock lock (&m_critOptions, INFINITE, FALSE );
        return m_GetAPDwordValue (szValueName, pdwValue, dwDefault);
    }
    return SAVRT_OPTS_ERROR;
}

SAVRT_OPTS_STATUS CAutoProtectOptions::GetBinaryValue( LPCSTR szValueName, LPBYTE pBuffer, DWORD dwBufferSize )
{
    if (m_bExportsOK)
    {
        ccLib::CSingleLock lock (&m_critOptions, INFINITE, FALSE );
        return m_GetAPBinaryValue (szValueName, pBuffer, dwBufferSize);
    }
    return SAVRT_OPTS_ERROR;
}

SAVRT_OPTS_STATUS CAutoProtectOptions::GetStringValue( LPCSTR szValueName, LPSTR szOut, DWORD dwLength, LPSTR szDefault )
{
    if (m_bExportsOK)
    {
        ccLib::CSingleLock lock (&m_critOptions, INFINITE, FALSE );
        return m_GetAPStringValue (szValueName, szOut, dwLength, szDefault);
    }
    return SAVRT_OPTS_ERROR;
}

SAVRT_OPTS_STATUS CAutoProtectOptions::SetStringValue( LPCSTR szValueName, LPCSTR szData )
{
    if (m_bExportsOK)
    {
        ccLib::CSingleLock lock (&m_critOptions, INFINITE, FALSE );
        return m_SetAPStringValue (szValueName, szData);
    }
    return SAVRT_OPTS_ERROR;
}

SAVRT_OPTS_STATUS CAutoProtectOptions::SetDwordValue( LPCSTR szValueName, DWORD dwValue )
{
    if (m_bExportsOK)
    {
        ccLib::CSingleLock lock (&m_critOptions, INFINITE, FALSE );
        return m_SetAPDwordValue (szValueName, dwValue);
    }
    return SAVRT_OPTS_ERROR;
}

SAVRT_OPTS_STATUS CAutoProtectOptions::SetBinaryValue( LPCSTR szValueName, LPBYTE pBuffer, DWORD dwDataLength )
{
    if (m_bExportsOK)
    {
        ccLib::CSingleLock lock (&m_critOptions, INFINITE, FALSE );
        return m_SetAPBinaryValue (szValueName, pBuffer, dwDataLength);
    }
    return SAVRT_OPTS_ERROR;
}

SAVRT_OPTS_STATUS CAutoProtectOptions::Save()
{
    if (m_bExportsOK)
    {
        ccLib::CSingleLock lock (&m_critOptions, INFINITE, FALSE );

        SAVRT_OPTS_STATUS statusReturn = SAVRT_OPTS_ERROR;
        CCTRCTXI0 ( "Locking SAVRT options" );

        m_mutexSAVRTOptions.Lock(INFINITE,FALSE);
        statusReturn = m_SaveAPOptions ();
        m_mutexSAVRTOptions.Unlock();

        CCTRCTXI0 ( "Releasing lock on SAVRT options" );
        return statusReturn;
    }
    return SAVRT_OPTS_ERROR;
}

SAVRT_OPTS_STATUS CAutoProtectOptions::GetValuesInfo (PSAVRT_OPTS_VALUE_INFO* ppOpts, unsigned int* puiCount)
{
    if (m_bExportsOK)
    {
        ccLib::CSingleLock lock (&m_critOptions, INFINITE, FALSE );
        return m_GetAPValuesInfo ( ppOpts, puiCount );
    }

    return SAVRT_OPTS_ERROR;
}

SAVRT_OPTS_STATUS CAutoProtectOptions::FreeValuesInfo (PSAVRT_OPTS_VALUE_INFO pOpts)
{
    if (m_bExportsOK)
    {
        ccLib::CSingleLock lock (&m_critOptions, INFINITE, FALSE );
        return m_FreeAPValuesInfo ( pOpts );
    }

    return SAVRT_OPTS_ERROR;
}

SAVRT_OPTS_STATUS CAutoProtectOptions::RestoreDefaultsAndLoad ()
{
    if (m_bExportsOK)
    {
        ccLib::CSingleLock lock (&m_critOptions, INFINITE, FALSE );
        return m_RestoreAPDefaultsAndLoad ();
    }

    return SAVRT_OPTS_ERROR;
}

SAVRT_OPTS_STATUS CAutoProtectOptions::AreOptionsDirty()
{
    if (m_bExportsOK)
    {
        ccLib::CSingleLock lock (&m_critOptions, INFINITE, FALSE );
        return m_AreAPOptionsDirty ();
    }

    return SAVRT_OPTS_ERROR;
}

SAVRT_OPTS_STATUS CAutoProtectOptions::SetDefaultValue( LPCSTR szValueName )
{
    if (m_bExportsOK)
    {
        ccLib::CSingleLock lock (&m_critOptions, INFINITE, FALSE );
        return m_SetAPDefaultValue (szValueName);
    }

    return SAVRT_OPTS_ERROR;
}

SAVRT_OPTS_STATUS CAutoProtectOptions::DefaultAll()
{
    if (m_bExportsOK)
    {
        ccLib::CSingleLock lock (&m_critOptions, INFINITE, FALSE );

        SAVRT_OPTS_STATUS statusReturn = SAVRT_OPTS_ERROR;
        CCTRCTXI0 ( "Locking SAVRT options" );

        m_mutexSAVRTOptions.Lock(INFINITE,FALSE);
        statusReturn = m_APDefaultAll ();
        m_mutexSAVRTOptions.Unlock();

        CCTRCTXI0 ( "Releasing lock on SAVRT options" );
        return statusReturn;
    }

    return SAVRT_OPTS_ERROR;
}
