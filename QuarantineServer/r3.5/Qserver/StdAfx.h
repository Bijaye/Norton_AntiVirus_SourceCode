/*
 * PROPRIETARY/CONFIDENTIAL.  use of this product is subject to license terms.
 * Copyright (c) 2005 Symantec Corporation.  All Rights Reserved.
 *
*/

// stdafx.h : include file for standard system include files,
//      or project specific include files that are used frequently,
//      but are changed infrequently

#if !defined(AFX_STDAFX_H__C60B0156_894B_11D2_8E5F_2A0712000000__INCLUDED_)
#define AFX_STDAFX_H__C60B0156_894B_11D2_8E5F_2A0712000000__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define STRICT
#ifndef _WIN32_WINNT
#define _WIN32_WINNT 0x0400
#endif
#define _ATL_APARTMENT_THREADED

#pragma warning(disable:4786)       

#include <afxwin.h>   // MFC core and standard components
#include <afxext.h>   // MFC extensions


#include <winsock2.h>
#include <atlbase.h>
#include "threadpool.h"
#include "eresource.h"
#include "map"
#include "qscommon.h"
#include "QSSesEvent.h"
using namespace std;

// 
// Log messages
// 
#include "qservermsg.h"


/////////////////////////////////////////////////////////////////////////////////////////
// 
// Map object for keeping list of data fields.
// 
typedef multimap< unsigned long, LPUNKNOWN > MAP;
typedef MAP::value_type VALUEPAIR;

#define DEBUGLEVEL_NONE		0x0000 // no debugging output
#define DEBUGLEVEL_LOW		0x0001 // output interface errors
#define DEBUGLEVEL_MED		0x0002 // output qserverinterface creation and deleation
#define DEBUGLEVEL_DETIAL	0x0004 // ouptut qserveritem creation and deleation and map additions.
#define DEBUGLEVEL_CONFIG_INTERFACE	0x0008 // ouptut CopyIteminterface operations.
#define DEBUGLEVEL_QSPAK	0x00010 // ouptut qspack operations with special attention to filesize.

#define MAX_WAIT_FOR_QF_MOVE	240 // maximum time to wait for a qfolder move in seconds.

//You may derive a class from CComModule and use it if you want to override
//something, but do not change the name of _Module

class CServiceModule : public CComModule
{
public:
    CServiceModule() :
        m_hStopping( NULL ),
        m_hSocketConfig( NULL ),
        m_hConfigEvent( NULL ),
        m_bListenIP( FALSE ),
        m_bListenSPX( FALSE ),
        m_iWinsockPort( 0 ),
        m_iSPXSocket( 0 ),
        m_uQserverByteCount( 0 ),
        m_ulItemCount(0),
		m_ulScanInterval (0),
		m_bQserver2ChanceAMSReg (0),
        m_bQserverFullEventLogged( FALSE ),
		m_dwTicktime(0),
		m_ulConfigITimeout(5),
		m_ulWinSockTrace(0),
		m_dwPurge(FALSE),
		m_dwQuotaError(FALSE),
		m_pQSSesInterface(NULL),
		m_dwMoveSamples(0),
		m_ulItemInterfaceTrace(0),
		m_ulSESAAgentPort(0)
        {
        InitializeCriticalSection( &m_csOpenMap );
        InitializeCriticalSection( &m_csInitSESAAgent);
        }
    ~CServiceModule()
        {
        DeleteCriticalSection( &m_csOpenMap );
        DeleteCriticalSection( &m_csInitSESAAgent );
        }

	HRESULT RegisterServer(BOOL bRegTypeLib, BOOL bService);
	BOOL RegisterIcePack();
	HRESULT UnregisterServer();
	BOOL StartQServer();
	BOOL StartIcePack();
	BOOL StopQServer();
	void Init(_ATL_OBJMAP_ENTRY* p, HINSTANCE h, UINT nServiceNameID, const GUID* plibid = NULL);
    void Start();
	void ServiceMain(DWORD dwArgc, LPTSTR* lpszArgv);
    void Handler(DWORD dwOpcode);
    void Run();
    BOOL IsInstalled();
	BOOL IsIcePackInstalled();
    BOOL Install();
    BOOL Uninstall();
	LONG Unlock();
    void SetServiceStatus(DWORD dwState);
    void SetupAsLocalServer();

    void GetQuarantineDir( LPTSTR pszDir );
    void SetQuarantineDir( LPCTSTR pszDir );
    int GetIPPort(){ return m_iWinsockPort; }
    int GetSPXPort(){ return m_iSPXSocket; }
    void SetIPPort( int iPort ) { m_iWinsockPort = iPort; }
    void SetSPXPort( int iPort ) { m_iSPXSocket = iPort; }
    BOOL GetListenIP(){ return m_bListenIP; }
    BOOL GetListenSPX(){ return m_bListenSPX; }
    void SetListenIP( BOOL bListen ) { m_bListenIP = bListen; }
    void SetListenSPX( BOOL bListen ) { m_bListenSPX = bListen; }
    ULONG GetDiskQuota() { return m_uDiskQuota; }
    void SetDiskQuota( ULONG u ) { m_uDiskQuota = u; }
    ULONG GetFileQuota() { return m_uFileQuota; }
    void SetFileQuota( ULONG u ) { m_uFileQuota = u; }

    ULONG GetQuotaError() { return m_dwQuotaError; }
    void SetQuotaError( void) {m_dwQuotaError = TRUE; }
    void ClearQuotaError( void) {m_dwQuotaError = FALSE; }

    BOOL AddOpenItem( DWORD dwItemID, LPUNKNOWN pUnkItem );
    void RemoveOpenItem( DWORD dwItemID, LPUNKNOWN pUnkItem );
    BOOL IsItemOpen( DWORD dwItemID );
    ULONG GetItemCount(){ return m_ulItemCount; }
    ULONG GetScanInterval(){ return m_ulScanInterval; }
    void SetScanInterval( ULONG u ) { m_ulScanInterval = u; }

    ULONG GetConfigITimeout(){ return m_ulConfigITimeout; }
    void SetConfigITimeout( ULONG u ) { m_ulConfigITimeout = u; }

    ULONG GetPurge(){ return m_dwPurge; }
    void SetPurge( ULONG u ) { m_dwPurge = u; }

	void LogEvent( DWORD dwMsgID,
				   WORD wType = EVENTLOG_ERROR_TYPE, 					   
				   LPTSTR lpszParam1 = NULL,
				   LPTSTR lpszParam2 = NULL,
				   LPTSTR lpszParam3 = NULL,
				   LPTSTR lpszParam4 = NULL );
	LPCTSTR GetAMSServerName(void) {
		USES_CONVERSION;
		return (m_szAmsServerName);}
	void SetAMSServerName(CString& s){
									if (s.IsEmpty()) return;
									_tcscpy(m_szAmsServerName, s);}
	
	LPCTSTR GetQSEventAppName(void) {
		USES_CONVERSION;
		return (m_szQsEventAppName);}

	void SetSampleMove (DWORD dwMove){m_dwMoveSamples = dwMove;}
	DWORD GetSampleMove(void){return m_dwMoveSamples;}
	BOOL IsSESAAgentPresent();

//Implementation
private:
	static void WINAPI _ServiceMain(DWORD dwArgc, LPTSTR* lpszArgv);
    static void WINAPI _Handler(DWORD dwOpcode);
    BOOL InitializeQserver();
    BOOL ShutdownQserver();
    void CountQserverFileBytes();


// data members
public:
    SERVICE_STATUS_HANDLE m_hServiceStatus;
    SERVICE_STATUS  m_status;
	DWORD           dwThreadID;
	BOOL            m_bService;
    CThreadPool*    m_pThreadPool;
    HANDLE          m_hStopping;
    HANDLE          m_hSocketConfig;
    HANDLE          m_hConfigEvent;
    LONG            m_iConfigCount;
    ULONG           m_uQserverByteCount;
    TCHAR           m_szServiceName[256];
    TCHAR           m_szIcePackServiceName[256];
    TCHAR           m_szEventLogServiceName[256];
    ULONG           m_ulItemCount;
    BOOL            m_bQserverFullEventLogged;
	CTime			m_tTimeOfAMSReg;
	BOOL			m_bQserver2ChanceAMSReg;
	DWORD			m_dwTicktime;
	ULONG			m_ulConfigITimeout;  // config interface time out in mins.
	ULONG			m_ulWinSockTrace;	// flag to indicate to log all of the WinSock activity.
	ULONG			m_ulItemInterfaceTrace;	// flag to indicate to log all of Qserver item interface activity.
	QSSesInterface* m_pQSSesInterface;  // added 4-3-01 tam pointer to naboo event interface.
    HANDLE          m_hQFolderMoveEvent;  // added 6-20-01  This is used to shutdown interfaces during a qfolder move

	CString			m_sWinINetVersion;
	CString			m_sWinINetEncryptionBit;
	CString			m_sCountryCode;

// configuration information
private:
    CEResource      m_configER;
    TCHAR           m_szQuarantineFolder[ MAX_PATH ];
    int             m_iWinsockPort;
    int             m_iSPXSocket;
    BOOL            m_bListenIP;
    BOOL            m_bListenSPX;
    ULONG           m_uDiskQuota;
    ULONG           m_uFileQuota;
	ULONG			m_ulScanInterval;
	ULONG			m_ulSESAAgentPort;
	TCHAR			m_szAmsServerName[MAX_PATH];
   	TCHAR			m_szQsEventAppName[MAX_PATH];
	DWORD			m_dwPurge; // tm 05/08/00
	DWORD			m_dwQuotaError; // tm 05/08/00
	DWORD			m_dwMoveSamples; // tm 06/20/01
	DWORD			m_dwfGenSESAEvents; // tm 11-11-03 1 = generate, 0 = disable

private:
    MAP                 m_OpenItemMap;
    CRITICAL_SECTION    m_csOpenMap;
	CRITICAL_SECTION	m_csInitSESAAgent;
};

extern CServiceModule _Module;
#include <atlcom.h>

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_STDAFX_H__C60B0156_894B_11D2_8E5F_2A0712000000__INCLUDED)
