
#include "stdafx.h"
#include "ServiceInterface.h"
#include "CServiceInstall.h"

#include "ServiceTemplates.h"

#include "ScanLatestService.h"

IMPLEMENT_SERVICE(ServiceInterface<ScanLatestService>, Scan)

int main()
{
	return ServiceMain<ScanLatestService>();
}
