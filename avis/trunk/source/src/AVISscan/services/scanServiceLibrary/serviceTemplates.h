

#ifndef SERVICETEMPLATES_H
#define SERVICETEMPLATES_H

//#include <iostream>

#include "CServiceInstall.h"
#include "ServiceInterface.h"

#include "TraceSinkFile.h"
#include "Logger.h"
#include "SystemException.h"

using namespace std;

template <class T> int ServiceMain(void)
{
	EntryExit	entryExit(Logger::LogAvisScanner, "ServiceMain template");

	SystemException::Init();

using namespace std;

	TCHAR*	pszCmdLine	= GetCommandLine();
	CharLowerBuff(pszCmdLine, lstrlen(pszCmdLine));
	if (_tcsstr(pszCmdLine, _T("-install")))
	{
		CServiceInstall	si( T::ShortName(), T::DisplayName());

		si.Install(SERVICE_WIN32_OWN_PROCESS, SERVICE_AUTO_START,
					"RPCSS\0\0");
	}
	else if (_tcsstr(pszCmdLine, _T("-remove")))
	{
		CServiceInstall	si( T::ShortName(), T::DisplayName());
		si.Remove();
	}
	else if (_tcsstr(pszCmdLine, _T("-run")))
	{
		T				service;
		SERVICE_STATUS	sStatus;

		service.Service(false);
		if (service.Initialize(0, sStatus))
		{
			service.Run();
			service.DeInitialize(0, sStatus);
		}
	}
	else	// must be running as a service, or by somebody that does not know the args
	{
//		cerr << "Usage: " << T::ShortName() << " [-install | -remove | -run]" << endl;
//		cerr << "-install\tInstall this app as a service" << endl;
//		cerr << "-remove\t\tRemove this app as a service" << endl;
//		cerr << "-run\t\tRun this app, but not as a service" << endl;
//		cerr << "no args\t\tOnly works if called from the service control manager" << endl;


		ServiceInterface<T>	service(T::ShortName(), T::DisplayName(),
									SERVICE_WIN32_OWN_PROCESS);

		BEGIN_SERVICE_MAP
			SERVICE_MAP_ENTRY(ServiceInterface<T>, Scan)
		END_SERVICE_MAP
	}

	return 0;
}

#endif
