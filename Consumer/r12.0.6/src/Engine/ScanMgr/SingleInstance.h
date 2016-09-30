#pragma once

#include <map>

#include "SingleInstanceInterface.h"

class CNoNameMutex;
class CSmartNoNameMutexLock;
class CSingleInstanceData;
class CSingleInstanceDataSmartLock;

class CSingleInstance : 
    public ISymBaseImpl< CSymThreadSafeRefCount >, 
    public ISingleInstance
{
public:
    CSingleInstance(void);
    virtual ~CSingleInstance(void);

    SYM_INTERFACE_MAP_BEGIN()                
        SYM_INTERFACE_ENTRY( IID_SingleInstance, ISingleInstance)
    SYM_INTERFACE_MAP_END()                  

    // ISingleInstance interface
    HRESULT RegisterSingleInstance(LPCSTR lpName, bool &bAlreadyExists, DWORD &dwOwnerProcess, DWORD &dwOwnerThread);
    HRESULT AlreadyExists(LPCSTR lpName, bool &bAlreadyExists, DWORD &dwOwnerProcess, DWORD &dwOwnerThread);

protected:
    DWORD m_dwInstanceValue;

};

class CNoNameMutex
{
private:
    CNoNameMutex();

public:
    CNoNameMutex(PLONG plLocker, DWORD dwTimeout = 100000);
    ~CNoNameMutex();

    BOOL Lock();
    BOOL IsLocked(){ return m_bLocked; };
    void Unlock();

protected:
    PLONG m_plLocker;
    BOOL m_bLocked;
    DWORD m_dwTimeout;
};

typedef struct _SINGLE_INSTANCE_RECORD 
{
    DWORD dwValue;
    DWORD dwOwnerProcess;
    DWORD dwOwnerThread;
} SINGLE_INSTANCE_RECORD, *LPSINGLE_INSTANCE_RECORD;

class CSingleInstanceData
{
public:
    CSingleInstanceData();
    virtual ~CSingleInstanceData();

    BOOL Init();

    DWORD FindValue(DWORD dwValue, DWORD &dwOwnerProcess, DWORD &dwOwnerThread);
    DWORD AddValue(DWORD dwValue, DWORD dwOwnerProcess, DWORD dwOwnerThread);
    BOOL  RemoveValue(DWORD dwValue);
    BOOL  GetLock();
    void  ReleaseLock();

    static void Startup();
    static CSingleInstanceData * GetSingleInstanceData();
    static void Shutdown();

protected:
    static CSingleInstanceData *m_pSingleInstanceData;

    HANDLE m_hSharedFile;
    LPBYTE m_lpSharedMem;

    CNoNameMutex m_cSingleInstanceDataMutex;
    CNoNameMutex m_cMutexOwnerDataMutex;
};

class CSingleInstanceDataSmartLock
{
public:
    CSingleInstanceDataSmartLock()
    {
        m_bLocked = FALSE;

        Lock();
    }

    ~CSingleInstanceDataSmartLock()
    {
        Unlock();
        return;       
    };

    BOOL IsLocked()
    {
        return m_bLocked;
    };

    void Unlock()
    {
        if(m_bLocked)
        {
            CSingleInstanceData *pSingleInstanceData = CSingleInstanceData::GetSingleInstanceData();
            if(!pSingleInstanceData)
                return;

            pSingleInstanceData->ReleaseLock();
            m_bLocked = FALSE;
        }
        return;       
    };

protected:
    BOOL m_bLocked;

    BOOL Lock()
    {
        if(m_bLocked)
            return m_bLocked;

        CSingleInstanceData *pSingleInstanceData = CSingleInstanceData::GetSingleInstanceData();
        if(!pSingleInstanceData)
            return FALSE;

        m_bLocked = pSingleInstanceData->GetLock();
        return m_bLocked;
    };

};

class CSmartNoNameMutexLock
{
public:
    CSmartNoNameMutexLock(CNoNameMutex *pNoNameMutex)
    {
        m_pNoNameMutex = pNoNameMutex;
        if(m_pNoNameMutex)
            m_pNoNameMutex->Lock();
    }

    ~CSmartNoNameMutexLock()
    {
        Unlock();
    }

    BOOL IsLocked()
    {
        return m_pNoNameMutex->IsLocked();
    }

    void Unlock()
    {
        if(m_pNoNameMutex)
        {
            m_pNoNameMutex->Unlock();
        }
    }

protected:
    CNoNameMutex *m_pNoNameMutex;
};
