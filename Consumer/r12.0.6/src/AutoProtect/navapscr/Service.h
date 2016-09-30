// Service.h: interface for the CService class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SERVICE_H__C6C535AB_B021_461E_8ADA_DA16FBA0FC06__INCLUDED_)
#define AFX_SERVICE_H__C6C535AB_B021_461E_8ADA_DA16FBA0FC06__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "SCSmartHandle.h"

class CService  
{
public:
	CService(LPCTSTR lpcszServiceName);
	virtual ~CService();

    // Try to start the service. Will retry if it fails.
    //
    //BOOL Start();

    // Try to stop the service. Will retry if it fails.
    //
	//BOOL Stop();

    // For setting services to AUTO, MANUAL, etc. startup type
    //
	BOOL ChangeStartType( DWORD dwStartType );

    // Gets the current state. Returns SERVICE_STATUS.dwCurrentState format.
    //
	DWORD GetCurrentState();

protected:
	DWORD   m_dwCurrentAccess;
    TCHAR   m_lpszServiceName[MAX_PATH];
    
    // Opens the service with the requested access rights. If we already
    // have the service opened with these rights or greater we just 
    // leave it open.
    //
	BOOL    openService ( DWORD dwDesiredAccess );
    
    // dwRetries are in 1/10th secs.
    //
    BOOL waitForService( DWORD dwState, DWORD dwRetries);

    // Smart handles to our service and the service manager so we
    // don't open/close them on every operation.
    //
    NAVToolbox::CSCSmartHandle   m_hService;
    NAVToolbox::CSCSmartHandle   m_hSvcMgr;
};

#endif // !defined(AFX_SERVICE_H__C6C535AB_B021_461E_8ADA_DA16FBA0FC06__INCLUDED_)
