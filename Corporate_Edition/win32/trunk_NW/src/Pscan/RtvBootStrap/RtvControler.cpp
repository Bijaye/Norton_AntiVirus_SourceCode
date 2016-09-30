// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright (c) 2005 Symantec Corporation. All rights reserved.
// RtvControler.cpp - CNAV inplementation

#include "RtvControler.h"
// for IScanDialogs - used to start/stop NAV on 9X

#ifdef WIN95
#include "scaninf.h"
#include "scaninf_i.c"

CNAV::CEnvironment::CEnvironment(void) : m_hr(E_FAIL)
{
}

CNAV::CEnvironment::~CEnvironment(void)
{
	if (SUCCEEDED(m_hr))
		CoUninitialize();
}

CNAV::CEnvironment::operator bool(void)
{
	return SUCCEEDED(m_hr)
	    || SUCCEEDED(m_hr = CoInitialize(NULL));
}

BOOL CNAV::Running(void)
{
	CRegKey Main;
	DWORD dwRunning;

	//Find out if the service is running by looking at the registry key
	if (ERROR_SUCCESS != Main.Open(HKEY_LOCAL_MACHINE, szReg_Key_Main))
		return FALSE;

	//Read the ServiceRunning value
	return ERROR_SUCCESS == Main.QueryValue(dwRunning, szReg_Val_ServiceRunning)
	    && dwRunning != KEYVAL_NOTRUNNING;
}

BOOL CNAV::Stop(void)
{
	if (!m_env)
		return FALSE;

	CComPtr<IScanDialogs> spScanDlgs;

	return SUCCEEDED(spScanDlgs.CoCreateInstance(CLSID_ScanDialogs))
	    && SUCCEEDED(spScanDlgs->UnLoadServices());
}

BOOL CNAV::Start(void)
{
	if (!m_env)
		return FALSE;

	CComPtr<IScanDialogs> spScanDlgs;

	return SUCCEEDED(spScanDlgs.CoCreateInstance(CLSID_ScanDialogs))
		// SERVICE_HUNG error is being returned although there doesn't appear to be a problem.
		//     This error is apparently begin returned because SERVICE_START_PENDING status
		//        is not changing to SERVICE_RUNNING in the ScanDialog code.
		//        However subsequent checks here have returned SERVICE_RUNNING status.
		//        In addition, Control Panel - Services displays the service as running.
	    && SUCCEEDED(spScanDlgs->LoadServices());
}
#else WIN95
CNAV::CEnvironment::CEnvironment(void) : m_schSCManager(NULL), m_schService(NULL)
{
}

CNAV::CEnvironment::~CEnvironment(void)
{
	if (m_schSCManager)
	{
		CloseServiceHandle(m_schSCManager);

		if (m_schService)
			CloseServiceHandle(m_schService);
	}
}

CNAV::CEnvironment::operator bool(void)
{
	return (NULL != m_schSCManager
	     || NULL != (m_schSCManager = OpenSCManager( NULL                      // machine (NULL == local)
	                                               , NULL                      // database (NULL == default)
	                                               , SC_MANAGER_ALL_ACCESS ))) // access required
	    && (NULL != m_schService
	     || NULL != (m_schService = OpenService( m_schSCManager
	                                           , _T(SERVICE_NAME)
	                                           , SERVICE_ALL_ACCESS) ));
}

BOOL CNAV::Running(void)
{
	SERVICE_STATUS	status;

	return (m_env == 1
	    && QueryServiceStatus(m_env, &status) == 1
	    && (status.dwCurrentState == SERVICE_RUNNING) == 1);
}

BOOL CNAV::Stop(void)
{
	BOOL            ret;
	SERVICE_STATUS	status;

	if (m_env == 0)
		return TRUE;

	ret = ControlService(m_env,SERVICE_CONTROL_STOP,&status);
	if (!ret)
		ret = (ERROR_SERVICE_NOT_ACTIVE == GetLastError());
	else {
		while (status.dwCurrentState == SERVICE_STOP_PENDING) {
			Sleep(100);
			if (!QueryServiceStatus(m_env,&status)) {
				ret = FALSE;
				break;
				}
			}
		}

	return ret;
}

BOOL CNAV::Start(void)
{
	if ( m_env == 0)
		return FALSE;

	return StartService(m_env,0,NULL)
	     ? TRUE
	     : GetLastError() == ERROR_SERVICE_ALREADY_RUNNING;
}
#endif WIN95
