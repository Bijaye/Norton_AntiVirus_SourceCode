#pragma once
#include "ccthread.h"
#include <set>

class CScanMonitorSink
{
public:
    virtual void ScanFinished ( HANDLE hScan ) = 0;
};

class CScanMonitor :
    public ccLib::CThread
{
public:
    CScanMonitor(CScanMonitorSink* pSink);
    virtual ~CScanMonitor(void);

    void AddScan (HANDLE hScan);
    bool IsEmpty (void);
    
    // CThread
    int Run (void);
    BOOL InitInstance(void);

protected:
    CScanMonitorSink* m_pSink;
    ccLib::CEvent m_eventNewWork;
    ccLib::CCriticalSection m_critEvents;
    std::set<HANDLE> m_setScans;
    typedef std::set<HANDLE>::iterator iterScans;

    CScanMonitor(void); // disallowed
};
