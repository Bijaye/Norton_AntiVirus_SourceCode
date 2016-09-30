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

#include <atlbase.h>
#include "threadpool.h"
#include "eresource.h"
#include "safearray.h"
#include "map"
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
        m_ulItemCount(0)
        {
        InitializeCriticalSection( &m_csOpenMap );
        }
    ~CServiceModule()
        {
        DeleteCriticalSection( &m_csOpenMap );
        }

	HRESULT RegisterServer(BOOL bRegTypeLib, BOOL bService);
	HRESULT UnregisterServer();
	void Init(_ATL_OBJMAP_ENTRY* p, HINSTANCE h, UINT nServiceNameID, const GUID* plibid = NULL);
    void Start();
	void ServiceMain(DWORD dwArgc, LPTSTR* lpszArgv);
    void Handler(DWORD dwOpcode);
    void Run();
    BOOL IsInstalled();
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

    BOOL AddOpenItem( DWORD dwItemID, LPUNKNOWN pUnkItem );
    void RemoveOpenItem( DWORD dwItemID, LPUNKNOWN pUnkItem );
    BOOL IsItemOpen( DWORD dwItemID );
    ULONG GetItemCount(){ return m_ulItemCount; }

    void LogEvent( DWORD dwMsgID,
				   WORD wType = EVENTLOG_ERROR_TYPE, 					   
				   LPSTR lpszParam1 = NULL,
				   LPSTR lpszParam2 = NULL,
				   LPSTR lpszParam3 = NULL,
				   LPSTR lpszParam4 = NULL );


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
    TCHAR           m_szEventLogServiceName[256];
    ULONG           m_ulItemCount;

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
    

private:
    MAP                 m_OpenItemMap;
    CRITICAL_SECTION    m_csOpenMap;
};

extern CServiceModule _Module;
#include <atlcom.h>

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_STDAFX_H__C60B0156_894B_11D2_8E5F_2A0712000000__INCLUDED)
