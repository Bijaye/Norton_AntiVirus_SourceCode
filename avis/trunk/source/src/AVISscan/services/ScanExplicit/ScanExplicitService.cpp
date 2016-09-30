// ScanExplicitService.cpp: implementation of the ScanExplicitService class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ScanExplicitService.h"


TCHAR*	ScanService::shortName	= "ScanExplicit";
TCHAR*	ScanService::displayName= "Symantec Quarantine Scanner";
//TCHAR*	ScanService::displayName= "Scan with explicit signatures";

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

ScanExplicitService::ScanExplicitService()
{

}

ScanExplicitService::~ScanExplicitService()
{

}

void ScanExplicitService::TypeSpecificInitialization()
{
	serviceType = ServiceType::ScanExplicit;
	pipeName	= "\\\\.\\pipe\\ScanExplicit";
}
