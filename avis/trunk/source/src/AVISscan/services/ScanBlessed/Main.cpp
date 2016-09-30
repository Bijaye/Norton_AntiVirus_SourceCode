
#include "stdafx.h"
#include "ServiceInterface.h"
#include "CServiceInstall.h"

#include "ServiceTemplates.h"
#include "ScanBlessedService.h"

IMPLEMENT_SERVICE(ServiceInterface<ScanBlessedService>, Scan)

int main()
{
	return ServiceMain<ScanBlessedService>();
/*
	TCHAR*	pszCmdLine	= GetCommandLine();
	CharLowerBuff(pszCmdLine, lstrlen(pszCmdLine));
	if (_tcsstr(pszCmdLine, _T("-install")))
	{
		CServiceInstall	si( ScanBlessedService::ShortName(),
							ScanBlessedService::DisplayName());
		si.Install();
	}
	else if (_tcsstr(pszCmdLine, _T("-remove")))
	{
		CServiceInstall	si( ScanBlessedService::ShortName(),
							ScanBlessedService::DisplayName());
		si.Remove();
	}
	else if (_tcsstr(pszCmdLine, _T("-run")))
	{
		ScanBlessedService	service;
		SERVICE_STATUS		sStatus;

		service.Service(false);
		service.Initialize(0, sStatus);
		service.Run();
		service.DeInitialize(0, sStatus);
	}
	else	// must be running as a service
	{
		ServiceInterface	service(ScanBlessedService::ShortName(),
									ScanBlessedService::DisplayName(),
									SERVICE_WIN32_OWN_PROCESS);

		BEGIN_SERVICE_MAP
			SERVICE_MAP_ENTRY(ServiceInterface, ScanBlessed)
		END_SERVICE_MAP
	}

	return 0;
*/
}
