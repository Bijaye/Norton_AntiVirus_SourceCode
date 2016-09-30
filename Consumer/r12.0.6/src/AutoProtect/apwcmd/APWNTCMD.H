// Copyright 1997 Symantec Corporation
//***************************************************************************
//
// $Header:   S:/APWCMD/VCS/ApwNtCmd.h_v   1.2   29 Jun 1998 22:21:02   RStanev  $
//
// Description:
//   Contains the NT specific class definition for ApwNtCmd.
//
//***************************************************************************
// $Log:   S:/APWCMD/VCS/ApwNtCmd.h_v  $
// 
//    Rev 1.2   29 Jun 1998 22:21:02   RStanev
// Added new functionality to support asynchronous processing of tasks
// triggered by driver notifications. This is a workaround for a problem
// with RPC in Win9x.
//
//    Rev 1.1   01 Jun 1997 12:39:32   DALLEE
// Changed naming scheme to be more readable - "APW" in classes and
// functions is now "Apw".
//
//    Rev 1.0   31 May 1997 19:56:38   JBRENNA
// Initial revision.
//
//***************************************************************************

#ifndef __APWNTCMD_H
#define __APWNTCMD_H

#ifndef __APWCMD_H
#include "ApwCmd.h"                     // For CAPWCmd class.
#endif

#include "NavSettingsHelperEx.h"
#include "SyncQueue.h"
#include "apwutil.h"
#include "EventData.h"

// Forward declarations
class CEventHandler;
class CEventQueue;
class CDispatchQueue;
class CDefAuthenticationAlertQueue;

//************************************************************************
// class CApwNtCmd
//
// NT version of the CAPWCmd class.

class CApwNtCmd : public CApwCmd
{
// Operations
public:
    virtual BOOL     Init( HWND hParent );
    virtual BOOL     DeInit();
    virtual BOOL     Options( HWND hParent );
    virtual BOOL     StartDriver();
    virtual BOOL     StopDriver();
    virtual BOOL     WaitForStartupPending();
    virtual BOOL     EnableDriver();
    virtual BOOL     DisableDriver();
    virtual AP_STATE GetStatusOfDriver();
    virtual BOOL     ShutdownScan( HWND hParent );
    virtual VOID     ConfirmApwComTasksReceived();
    virtual BOOL     FeatureEnabled( VOID );
    virtual BOOL     IsAggregated( VOID );

	void QueueEvent( CEventData*& pEventData );
	void OnServiceStopped();
    void DisplayDefAuthenticationFailure(DWORD dwFailureCode);

    // Pointer to Dispatch queue object
	CDispatchQueue*	    m_pDispatchQueue;

    // Pointer to Event Queue object
    CEventQueue*        m_pEventQueue;

// Implementation
public:
    CApwNtCmd();
	~CApwNtCmd();

// Private Functions
private:
	BOOL registerHandler();
	BOOL unRegisterHandler();
	BOOL isServiceRunning();
    void InitCCsettingsOptions(); // this uses a local copy of the settings

	static unsigned __stdcall maintenenceThreadProc( void* pData );

// Private Data.
private:
    // Pointer to event handler object.
	CEventHandler*		m_pHandler;

    CDefAuthenticationAlertQueue* m_pDefAuthQueue;

	// Handle to thread stop event
	HANDLE				m_hStopEvent;

	// Thread ID.
	HANDLE				m_hThread;

	// NAVAP running mutex name.
	TCHAR				m_szNAVAPrunningMutex[32];

    // Critical section for driver status routine
    ccLib::CCriticalSection m_csGetStatus;

    // Licensing valid?
    BOOL                m_bFeatureEnabled;

    // Aggregated product?
    BOOL                m_bIsAggregated;
};

#endif // __APWNTCMD_H
