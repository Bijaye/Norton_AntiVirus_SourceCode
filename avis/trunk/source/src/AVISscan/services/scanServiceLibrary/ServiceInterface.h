// ServiceInterface.h: interface for the ServiceInterface class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SERVICEINTERFACE_H__C762502C_DE0B_11D2_AD8D_00A0C9C71BBC__INCLUDED_)
#define AFX_SERVICEINTERFACE_H__C762502C_DE0B_11D2_AD8D_00A0C9C71BBC__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "cservice.h"
//#include "ScanBlessedService.h"
#include "Logger.h"

template<class T> class ServiceInterface : public CService  
{
public:
	ServiceInterface(LPTSTR name, LPTSTR display, DWORD type) :
					CService(name, display, type)
	{
		EntryExit	entryExit(Logger::LogAvisScanner, "ServiceInterface::ServiceInterface");

		m_pThis		= this;
	
		pause		= false;
		stop		= false;

		m_hPauseNotifyEvent	= NULL;
	}

	virtual ~ServiceInterface() {};

	void	Run(void)
	{
		EntryExit	entryExit(Logger::LogAvisScanner, "ServiceInterface::Run");

//PrintEvent(_T("Entering Run"), FALSE);

		SetStatus(SERVICE_RUNNING, 0, 0,
					SERVICE_ACCEPT_STOP | SERVICE_ACCEPT_PAUSE_CONTINUE);
//DebugBreak();

		service.Run();

		SetStatus(SERVICE_STOP_PENDING, 2, 5000);
//PrintEvent(_T("Leaving Run"), FALSE);
	}

	DECLARE_SERVICE(ServiceInterface, Scan)

protected:
	void	OnPause()
	{
		EntryExit	entryExit(Logger::LogAvisScanner, "ServiceInterface::OnPause");

		DWORD	dwCheckPt	= 1;
		SetStatus(SERVICE_PAUSE_PENDING, dwCheckPt, 6000);

		service.Pause();

		SetStatus(SERVICE_PAUSED);
	}

	void	OnContinue()
	{
		EntryExit	entryExit(Logger::LogAvisScanner, "ServiceInterface::OnContinue");

		DWORD	dwCheckPt	= 1;
		SetStatus(SERVICE_CONTINUE_PENDING, dwCheckPt, 6000);

		service.Continue();

		SetStatus(SERVICE_RUNNING);
	}

	void	OnStop()
	{
		EntryExit	entryExit(Logger::LogAvisScanner, "ServiceInterface::onStop");

		SetStatus(SERVICE_STOP_PENDING, 1, 6000);

		service.Stop();	// final status is sent when the run method returns
	}

	void	Init()
	{
		EntryExit	entryExit(Logger::LogAvisScanner, "ServiceInterface::Init");

		m_hPauseNotifyEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
		if (!m_hPauseNotifyEvent)
			ErrorHandler(_T("CreateEvent"));

		SERVICE_STATUS	sStatus;
		sStatus.dwServiceType		= SERVICE_WIN32_OWN_PROCESS;
		sStatus.dwCurrentState		= SERVICE_START_PENDING;
		sStatus.dwControlsAccepted	= SERVICE_CONTROL_SHUTDOWN;
		sStatus.dwWin32ExitCode		= NO_ERROR;
		sStatus.dwServiceSpecificExitCode	= 0;
		sStatus.dwCheckPoint		= 0;
		sStatus.dwWaitHint			= 5000;

//DebugBreak();
		service.Initialize(m_hServiceStatus, sStatus);
	}

	void	DeInit()
	{
		EntryExit	entryExit(Logger::LogAvisScanner, "ServiceInterface::DeInit");

		SERVICE_STATUS	sStatus;
		sStatus.dwServiceType		= SERVICE_WIN32_OWN_PROCESS;
		sStatus.dwCurrentState		= SERVICE_START_PENDING;
		sStatus.dwControlsAccepted	= SERVICE_CONTROL_SHUTDOWN;
		sStatus.dwWin32ExitCode		= NO_ERROR;
		sStatus.dwServiceSpecificExitCode	= 0;
		sStatus.dwCheckPoint		= 0;
		sStatus.dwWaitHint			= 5000;

		service.DeInitialize(m_hServiceStatus, sStatus);
	}

	HANDLE	m_hPauseNotifyEvent;

	bool	pause;
	bool	stop;

private:
	T		service;
};

#endif // !defined(AFX_SERVICEINTERFACE_H__C762502C_DE0B_11D2_AD8D_00A0C9C71BBC__INCLUDED_)
