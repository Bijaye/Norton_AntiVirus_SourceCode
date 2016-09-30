
#ifndef CSERVICEINSTALL_H
#define CSERVICEINSTALL_H

class CServiceInstall
{
public:
	CServiceInstall( LPTSTR szServiceName, LPTSTR szDisplay, LPTSTR szPath );
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
    TCHAR m_path[257];
};

#endif
