// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright (c) 2005 Symantec Corporation. All rights reserved.
///////////////////////////////////////////////////////////////////////////
//
// DefWatchService.h: interface for the CDefWatchService class.
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
#include "DefUpdateEvent.h"

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

public:
    HANDLE m_hServiceEvent;


private:
    static DWORD __stdcall WatchThread( LPVOID pData );
    void FireEvents();
    BOOL HaveDefsChanged();
    BOOL BuildEventHandlerList();

private:
    //
    // Data members
    //
    SERVICE_STATUS          m_ServiceStatus;
    SERVICE_STATUS_HANDLE   m_ServiceStatusHandle;

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

    //
    // Definition update event
    //
    CDefUpdateEvent         m_cDefUpdateEvent;
    HANDLE                  m_hDefUpdateSignal;
};

///////////////////////////////////////////////////////////////////////////

#endif // !defined(AFX_DefWatchSERVICE_H__3504E7FC_5FFD_11D2_8E24_B27635000000__INCLUDED_)
