#include "StdAfx.h"
#include "SingleInstance.h"
#include <atlsecurity.h>

const DWORD RECORDLIST_MAX_ENTRIES    = 256;
const DWORD RECORDLIST_ENTRY_MAX_SIZE = sizeof(SINGLE_INSTANCE_RECORD);
const DWORD SHAREDFILESIZE            = RECORDLIST_MAX_ENTRIES * RECORDLIST_ENTRY_MAX_SIZE;
const LPSTR SHAREDFILE_NAME           = _T("_SYMC_SCNMGR_SI_");

#pragma data_seg(".SINGLEINSTANCE_SHARED")
volatile LONG  g_lMutexOwnerLocker = 0;
volatile DWORD g_dwMutexOwner = 0;

volatile bool g_bInit = false;
volatile LONG  g_lValueListLocker = 0;
volatile DWORD g_dwNumRecordsInUse = 0;

#pragma data_seg()

#pragma comment(linker, "/section:.SINGLEINSTANCE_SHARED,rws")

CSingleInstanceData * CSingleInstanceData::m_pSingleInstanceData = NULL;

//****************************************************************************
//****************************************************************************
DWORD SimpleHash(LPCBYTE pbInput, DWORD dwSize)
{

    DWORD dwHash = 0;
    for(DWORD dwIndex = 0; dwIndex < dwSize; dwIndex++)
    {
        dwHash = pbInput[dwIndex] + (dwHash << 6) + (dwHash << 16) - dwHash;
    }

    return (dwHash & 0x7FFFFFFF);

}

//****************************************************************************
//  c'tor / d'tor
//****************************************************************************
CSingleInstance::CSingleInstance(void) :
m_dwInstanceValue(NULL)
{
    // does nothing
}

CSingleInstance::~CSingleInstance(void)
{
    // If there is a value associated with this instance,
    //  then release this instance lock on destruction.
    if(m_dwInstanceValue)
    {
        CSingleInstanceData *pSIData = CSingleInstanceData::GetSingleInstanceData();
        if(pSIData)
        {
            CCTRACEI("CSingleInstance::~CSingleInstance() - unlocking instance 0x%08X", m_dwInstanceValue);
            pSIData->RemoveValue(m_dwInstanceValue);
        }
    }

}

//****************************************************************************
//****************************************************************************
HRESULT CSingleInstance::RegisterSingleInstance(LPCSTR lpName, bool &bAlreadyExists, DWORD &dwOwnerProcess, DWORD &dwOwnerThread)
{
    HRESULT hrReturn = E_FAIL;

    CSingleInstanceData *pSIData = CSingleInstanceData::GetSingleInstanceData();
    if(!pSIData)
        return E_FAIL;

    // Get the hash
    DWORD dwHash = 0;
    if(lpName && lpName[0])
    {
        dwHash = SimpleHash((LPCBYTE)lpName, strlen(lpName));
    }

    CSingleInstanceDataSmartLock cLocker;
    if(!cLocker.IsLocked())
    {
        CCTRACEE("CSingleInstance::RegisterSingleInstance - unable to get single instance data lock.");
        return E_FAIL;
    }

    DWORD dwIndex = pSIData->FindValue(dwHash, dwOwnerProcess, dwOwnerThread);

    // If we didn't find the value in the instance data,
    // then try to add it.
    if(-1 == dwIndex)
    {
        m_dwInstanceValue = dwHash;

        dwIndex = pSIData->AddValue(m_dwInstanceValue, dwOwnerProcess, dwOwnerThread);
        if(-1 == dwIndex)
            hrReturn = E_FAIL;

        bAlreadyExists = false;
        hrReturn = S_OK;
        CCTRACEI("CSingleInstance::RegisterSingleInstance - instance 0x%08X now locked by %X::%X.", dwOwnerProcess, dwOwnerThread);
    }

    //  This value already exists in the instance data
    else
    {
        CCTRACEI("CSingleInstance::RegisterSingleInstance - instance 0x%08X already locked by %X::%X.", dwOwnerProcess, dwOwnerThread);
        bAlreadyExists = true;
        hrReturn = S_OK;
    }

    return hrReturn;
}

//****************************************************************************
//****************************************************************************
HRESULT CSingleInstance::AlreadyExists(LPCSTR lpName, bool &bAlreadyExists, DWORD &dwOwnerProcess, DWORD &dwOwnerThread)
{
    HRESULT hrReturn = E_FAIL;

    CSingleInstanceData *pSIData = CSingleInstanceData::GetSingleInstanceData();
    if(!pSIData)
        return E_FAIL;

    // Get the hash
    DWORD dwHash = 0;
    if(lpName && lpName[0])
    {
        dwHash = SimpleHash((LPCBYTE)lpName, strlen(lpName));
    }

    CSingleInstanceDataSmartLock cLocker;
    if(!cLocker.IsLocked())
        return E_FAIL;

    DWORD dwIndex = pSIData->FindValue(dwHash, dwOwnerProcess, dwOwnerThread);

    // If we found it in the instance data, then it already exists
    if(-1 != dwIndex)
    {
        bAlreadyExists = true;
        hrReturn = S_OK;
        
        CCTRACEI("CSingleInstance::AlreadyExists - instance 0x%08X already locked by %X::%X.", dwOwnerProcess, dwOwnerThread);
    }
    else
    {
        bAlreadyExists = false;
        hrReturn = S_OK;
        
        CCTRACEI("CSingleInstance::AlreadyExists - instance 0x%08X not currently locked.");
    }

    return hrReturn;
}

//****************************************************************************
// c'tor / d'tor
//****************************************************************************
CSingleInstanceData::CSingleInstanceData() :
m_cSingleInstanceDataMutex(&((LONG)g_lValueListLocker)),
m_cMutexOwnerDataMutex(&((LONG)g_lMutexOwnerLocker)),
m_hSharedFile(NULL),
m_lpSharedMem(NULL)
{
    // nothing to do here
}

CSingleInstanceData::~CSingleInstanceData()
{
    if(m_lpSharedMem)
    {
        UnmapViewOfFile((LPVOID)m_lpSharedMem);
    }

    if(INVALID_HANDLE_VALUE != m_hSharedFile)
    {
        CloseHandle(m_hSharedFile);
    }
}

//****************************************************************************
//****************************************************************************
void CSingleInstanceData::Startup()
{
    // This function should usually just be called once per process, 
    //   from DllMain(DLL_PROCESS_ATTACH)

    // if there is already an instance of this singleton, then bail out
    if(m_pSingleInstanceData)
        return;

    // create a new instance of the singleton
    CSingleInstanceData * pSingleInstanceData = new CSingleInstanceData;
    if(!pSingleInstanceData)
        return;

    if(!pSingleInstanceData->Init())
    {
        delete pSingleInstanceData;
        return;
    }

    m_pSingleInstanceData = pSingleInstanceData;
    return;
}

//****************************************************************************
//****************************************************************************
void CSingleInstanceData::Shutdown()
{
    // This function should usually just be called once per process, 
    //   from DllMain(DLL_PROCESS_DETACH)

    if(m_pSingleInstanceData)
    {
        delete m_pSingleInstanceData;
        m_pSingleInstanceData = NULL;        
    }

    return;
}

//****************************************************************************
//****************************************************************************
BOOL CSingleInstanceData::Init()
{
    // For our null dacl
    ATL::CDacl nulldacl;
    ATL::CSecurityDesc secdesc;
    ATL::CSecurityAttributes secatt;

    // Set our null dacl
    //
    if ( nulldacl.IsEmpty())
    {
        nulldacl.SetNull();
        secdesc.SetDacl (nulldacl);
        secatt.Set (secdesc); // This throws assertions in debug mode, I know.
    }
    
    // Create a mapping to our shared memory object
    m_hSharedFile = CreateFileMapping(INVALID_HANDLE_VALUE, &secatt, PAGE_READWRITE, 0, SHAREDFILESIZE, SHAREDFILE_NAME);
    if(NULL == m_hSharedFile)
    {
        CCTRACEE("CSingleInstanceData::Init - CreateFileMapping() failed. dwErr: 0x%08X", GetLastError());
        return FALSE;
    }

    // Create a view of the shared memory object
    m_lpSharedMem = (LPBYTE)MapViewOfFile(m_hSharedFile, FILE_MAP_WRITE, 0, 0, 0);
    if(!m_lpSharedMem)
    {
        CCTRACEE("CSingleInstanceData::Init - MapViewOfFile() failed. dwErr: 0x%08X", GetLastError());
        CloseHandle(m_hSharedFile);
        m_hSharedFile = INVALID_HANDLE_VALUE;
        return FALSE;
    }

    // Windows is supposed to initialize the new shared memory object to 0s,
    //  so there is no initialization to do on the object.

    return TRUE;
}

//****************************************************************************
//****************************************************************************
CSingleInstanceData* CSingleInstanceData::GetSingleInstanceData()
{
    return m_pSingleInstanceData;
}


//****************************************************************************
//****************************************************************************
DWORD CSingleInstanceData::FindValue(DWORD dwValue, DWORD &dwOwnerProcess, DWORD &dwOwnerThread)
{
    if(!m_lpSharedMem)
        return -1;

    DWORD dwIndex = NULL;
    BOOL bFound = FALSE;
    LPBYTE pbIndex = m_lpSharedMem;

    // Enumerate the values in this table, and see
    // if there are any matches.
    for(dwIndex = 0; dwIndex < g_dwNumRecordsInUse; dwIndex++)
    {
        LPSINGLE_INSTANCE_RECORD pRec = (LPSINGLE_INSTANCE_RECORD)pbIndex;
        if(pRec->dwValue == dwValue)
        {
            dwOwnerProcess = pRec->dwOwnerProcess;
            dwOwnerThread = pRec->dwOwnerThread;
            bFound = TRUE;
            break;
        }

        pbIndex += sizeof(SINGLE_INSTANCE_RECORD);
    }

    if(bFound)
        return dwIndex;

    return -1;
}

//****************************************************************************
//****************************************************************************
DWORD CSingleInstanceData::AddValue(DWORD dwValue, DWORD dwOwnerProcess, DWORD dwOwnerThread)
{
    if(!m_lpSharedMem)
        return -1;

    // If the table is already full, then fail the call
    if(g_dwNumRecordsInUse >= RECORDLIST_MAX_ENTRIES)
    {
        CCTRACEE(_T("CSingleInstanceData::AddValue - There are no more available records in the single instance table."));
        return -1;
    }

    // Get a pointer to the last record in the table
    LPBYTE pbEntry = m_lpSharedMem + (g_dwNumRecordsInUse * RECORDLIST_ENTRY_MAX_SIZE);

    // It is expected that all records are init'd to zero, if not, something is fishy here.
    if(0 != *pbEntry)
    {
        CCTRACEE(_T("CSingleInstanceData::AddValue - This record should have been 0'd, but it isn't."));
        return -1;
    }

    //  Place the value into the table
    LPSINGLE_INSTANCE_RECORD pRec = (LPSINGLE_INSTANCE_RECORD)pbEntry;
    pRec->dwValue = dwValue;
    pRec->dwOwnerProcess = dwOwnerProcess;
    pRec->dwOwnerThread = dwOwnerThread;

    // Increment the record count
    g_dwNumRecordsInUse++;

    return g_dwNumRecordsInUse - 1;
}

//****************************************************************************
//****************************************************************************
BOOL CSingleInstanceData::RemoveValue(DWORD dwValue)
{
    if(!m_lpSharedMem)
        return FALSE;

    DWORD dwProcess, dwThread;
    DWORD dwIndex = FindValue(dwValue, dwProcess, dwThread);

    //  We should have found a value, if not, something is fishy.
    if(-1 == dwIndex)
    {
        CCTRACEE(_T("CSingleInstanceData::RemoveValue - An attempt was made to remove a value that didn't exist."));
        return FALSE;
    }

    // Get a pointer to the entry of interest so it can be 0'd out.
    LPBYTE pbEntry = m_lpSharedMem + (dwIndex * RECORDLIST_ENTRY_MAX_SIZE);
    ZeroMemory(pbEntry, RECORDLIST_ENTRY_MAX_SIZE);
    g_dwNumRecordsInUse--;

    // If the the entry was not at the end of the table,
    //  then move the item currently at the end of the table to 
    //  the empty entry.
    if(dwIndex != g_dwNumRecordsInUse)
    {
        LPBYTE pbLastEntry = m_lpSharedMem + (g_dwNumRecordsInUse * RECORDLIST_ENTRY_MAX_SIZE);
        memcpy(pbEntry, pbLastEntry, RECORDLIST_ENTRY_MAX_SIZE);
        ZeroMemory(pbLastEntry, RECORDLIST_ENTRY_MAX_SIZE);
    }

    return TRUE;
}


//****************************************************************************
//****************************************************************************
BOOL CSingleInstanceData::GetLock()
{
    // Get a lock on the single instance data
    m_cSingleInstanceDataMutex.Lock();
    if(!m_cSingleInstanceDataMutex.IsLocked())
        return FALSE;

    // Get a lock on the mutex owner data, and update to current process
    CSmartNoNameMutexLock cMutexOwnerData(&m_cMutexOwnerDataMutex);
    if(cMutexOwnerData.IsLocked())
    {
        g_dwMutexOwner = GetCurrentProcessId();
        cMutexOwnerData.Unlock();
    }

    return m_cSingleInstanceDataMutex.IsLocked();
}

//****************************************************************************
//****************************************************************************
void CSingleInstanceData::ReleaseLock()
{
    if(!m_cSingleInstanceDataMutex.IsLocked())
        return;

    // Get a lock on the mutex owner data, and clear the data
    CSmartNoNameMutexLock cMutexOwnerData(&m_cMutexOwnerDataMutex);
    if(cMutexOwnerData.IsLocked())
    {
        g_dwMutexOwner = NULL;
        cMutexOwnerData.Unlock();
    }

    // Unlock the single instance data
    m_cSingleInstanceDataMutex.Unlock();
    if(m_cSingleInstanceDataMutex.IsLocked())
    {
        CCTRACEE(_T("CSingleInstanceData::ReleaseLock - Unable to release lock on Single Instance Data"));
    }

    return;
}

//****************************************************************************
//****************************************************************************
CNoNameMutex::CNoNameMutex(PLONG plLocker, DWORD dwTimeout)
{
    m_plLocker = plLocker;
    m_dwTimeout = dwTimeout;
    m_bLocked = FALSE;
}

CNoNameMutex::~CNoNameMutex()
{

}

//****************************************************************************
//****************************************************************************
BOOL CNoNameMutex::Lock()
{
    DWORD dwCountdownTimer = m_dwTimeout;
    while (dwCountdownTimer)
    {   
        // Try to switch the lock value from 0 to 1,
        //  if it is changing from anything other than 0,
        //  then it is still locked.
        if(0 == ::InterlockedExchange(m_plLocker, 1))
        {
            m_bLocked = TRUE;
            break;
        }

        // Decrement timeout
        dwCountdownTimer--;

        // take a nap
        Sleep(0);
    }

    return m_bLocked;
}

//****************************************************************************
//****************************************************************************
void CNoNameMutex::Unlock()
{
    if(!m_bLocked)
        return;

    // Release the locking mutex
    ::InterlockedExchange(m_plLocker, 0);
    m_bLocked = FALSE;

    return;
}