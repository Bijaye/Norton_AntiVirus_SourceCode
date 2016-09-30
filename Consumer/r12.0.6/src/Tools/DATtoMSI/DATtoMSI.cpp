// DATtoMSI.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#define INITIIDS
#include "ccSettingsInterface.h"
#include "ccVerifyTrustInterface.h"
#include "ccLibStaticLink.h"
#include "ccLibStd.h"

#include "ccSymDebugOutput.h"
ccSym::CDebugOutput g_DebugOutput(_T("navinst"));
IMPLEMENT_CCTRACE(::g_DebugOutput);

#include "NAVOptMigrate.h"

int _tmain(int argc, _TCHAR* argv[])
{
	if(3 > argc)
	{
		printf("USAGE: DATtoMSI.exe <path to datfile> <path to MSI> <component ID>\n");
		return -1;
	}

	CNAVOptMigrate migrate;
	if(migrate.MigrateNAVOptFile(argv[1], TRUE, argv[2], argv[3]))
	{
		printf("DATtoMSI.exe: Migrate Suceeded");
		return 0;
	}
	else
	{
		printf("DATtoMSI.exe: Migrate Failed");
		return -1;
	}
	
	return -1;
}

