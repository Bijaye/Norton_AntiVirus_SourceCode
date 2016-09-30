// ServiceUtil.h: interface for the CServiceUtil class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SERVICEUTIL_H__28B38A2B_45CA_4275_872E_20C2FA9F960F__INCLUDED_)
#define AFX_SERVICEUTIL_H__28B38A2B_45CA_4275_872E_20C2FA9F960F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CServiceUtil  
{
public:
	static bool StartService( LPCTSTR szServiceName );
	static bool StopService( LPCTSTR szServiceName );
	static bool ChangeServicePath( LPCTSTR szServiceName, LPCTSTR szServicePath );

private:
	static bool waitForState( SC_HANDLE hService, DWORD dwState );

	class CServiceHandle 
	{
	public:
		CServiceHandle(){ m_Handle = NULL;}
		CServiceHandle(SC_HANDLE handle){ m_Handle = handle; }
		~CServiceHandle(){ if( m_Handle ) CloseServiceHandle( m_Handle ); }
		bool IsValid() { return m_Handle != NULL; }
		operator SC_HANDLE() {return m_Handle;}
	private:
		SC_HANDLE m_Handle;
	};
};

#endif // !defined(AFX_SERVICEUTIL_H__28B38A2B_45CA_4275_872E_20C2FA9F960F__INCLUDED_)
