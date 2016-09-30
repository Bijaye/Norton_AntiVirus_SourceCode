
#ifndef CSERVICEINSTALL_H
#define CSERVICEINSTALL_H

#include <AVIScommon.h>

class AVISCOMMON_API CServiceInstall
{
public:
	CServiceInstall( LPTSTR szServiceName, LPTSTR szDisplay );
	~CServiceInstall( ) { }

	void Install(	DWORD dwType = SERVICE_WIN32_OWN_PROCESS,
					DWORD dwStart = SERVICE_DEMAND_START,
					LPCTSTR lpDepends = NULL, 
					LPCTSTR lpName = NULL,
					LPCTSTR lpPassword = NULL
					);
	void Remove( BOOL bForce = FALSE );

	BOOL IsInstalled( );
	DWORD ErrorPrinter( const TCHAR* pszFcn, DWORD dwErr = GetLastError() );

private:
	TCHAR m_service[257];
	TCHAR m_display[257];

};

#endif
