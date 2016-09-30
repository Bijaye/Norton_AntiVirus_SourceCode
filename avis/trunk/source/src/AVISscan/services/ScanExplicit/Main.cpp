
#include "stdafx.h"
#include "ServiceInterface.h"
#include "CServiceInstall.h"

#include "ServiceTemplates.h"

#include "ScanExplicitService.h"

IMPLEMENT_SERVICE(ServiceInterface<ScanExplicitService>, Scan)

int main()
{
	return ServiceMain<ScanExplicitService>();
}
