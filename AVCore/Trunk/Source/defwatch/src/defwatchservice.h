///////////////////////////////////////////////////////////////////////////
//
// ProxyService.h: interface for the CProxyService class.
//
//
//
//
///////////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////////


#if !defined(AFX_DefWatchSERVICE_H__3504E7FC_5FFD_11D2_8E24_B27635000000__INCLUDED_)
#define AFX_DefWatchSERVICE_H__3504E7FC_5FFD_11D2_8E24_B27635000000__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "virusdefversion.h"

///////////////////////////////////////////////////////////////////////////
// This structure defines nodes in the linked list of event handlers.

class CEventHandler
{
public:    
    CLSID clsid;
    CEventHandler* next;
};

typedef CEventHandler* PEVENTHANDLER;



///////////////////////////////////////////////////////////////////////////
// CDefWatchService

class CDefWatchService
{
public:
    CDefWatchService();
    virtual ~CDefWatchService();


	BOOL Register( LPHANDLER_FUNCTION lpHandlerProc ); 
	BOOL StartService();
	BOOL SetServiceStatus( DWORD dwStatus, DWORD dwTimeOutHint = 0 );
	VOID StopService();
    BOOL IsWin9x() { return m_bIsWin9x; }

public:
	HANDLE m_hServiceEvent;


private:
    static DWORD __stdcall WatchThread( LPVOID pData );
	void FireEvents();
	BOOL GetVirusDefsDir();
    BOOL HaveDefsChanged();
    BOOL BuildEventHandlerList();

private:
    //
    // Data members
    //
    SERVICE_STATUS          m_ServiceStatus;
    SERVICE_STATUS_HANDLE   m_ServiceStatusHandle;

    // 
    // Location of virus defs
    // 
    TCHAR                   m_szVirusDefPath[ MAX_PATH ];

    // 
    // Handle to watch thread
    // 
    HANDLE                  m_hWatchThread;
    HANDLE                  m_hThreadSignal;
    DWORD                   m_dwThreadID;

    // 
    // List of event handler objects
    //
    PEVENTHANDLER           m_pListHead;

    // 
    // Def date information
    // 
    CVirusDefVersion        m_cDefVersion;

    BOOL                    m_bIsWin9x;
};

///////////////////////////////////////////////////////////////////////////

#endif // !defined(AFX_DefWatchSERVICE_H__3504E7FC_5FFD_11D2_8E24_B27635000000__INCLUDED_)
