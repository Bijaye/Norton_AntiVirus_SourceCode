// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright (c) 2005 Symantec Corporation. All rights reserved.
// PScanTest.cpp : Test application for RTVScan

#include "stdafx.h"
#include <iostream>
#include <crtdbg.h>
#include <string>

#pragma warning (disable: 4786) // identifier was truncated to '255' characters in the debug information

#define INITIIDS
#include "defutilsinterface.h"
#include "ccSettingsManagerHelper.h"
#include "ccSymDebugOutput.h"
#include "ccSettingsManagerHelper.h"
#include "ccCoInitialize.h"
#include "TrustUtil.h"

// Instantiate CC's debug object and trace support - must do so or CC won't build
ccSym::CDebugOutput		g_DebugOutput(_T("SavMainUI"));
IMPLEMENT_CCTRACE(g_DebugOutput)

// Declares any objects available from this process. Really the below is to
// get a g_DLLObjectCount instance created.
SYM_OBJECT_MAP_BEGIN()
SYM_OBJECT_MAP_END()

// Function definition for a few core LDVP object functions.  Designed to be included in another CPP file, alas.
//#include "ldvpcom.c"

// Import SAV GUIDs
#include "vpcommon.h"
IMP_VPCOMMON_IIDS


// Prototypes
void DoTestOpState( void );
void DoTestTrust( int argc, char* argv[] );

int main( int argc, char* argv[] )
{
	CoInitialize(NULL);

	DoTestOpState();

	DoTestTrust(argc, argv);

	CoUninitialize();
    return 0;
}

void DoTestTrust(int argc, char* argv[] )
{
	CTrustVerifier trustUtil;
	HRESULT returnValHR = E_FAIL;

	returnValHR = trustUtil.Initialize(CTrustVerifier::VerifyMode_CommonClient);
	if (SUCCEEDED(returnValHR))
	{
		if (argc == 2)
		{
			returnValHR = trustUtil.IsFileTrusted(argv[1]);
			if (SUCCEEDED(returnValHR))
				printf("File %s is trusted.\n", argv[1]);
			else
				printf("File %s is NOT trusted.\n", argv[1]);
		}
		trustUtil.Shutdown();
	}
	else
	{
		printf("TrustUtil::Initialize failed, 0x%08x.\n", returnValHR);
	}
}

void DoTestOpState( void )
{
    GUID				clsid_cliproxy			= _CLSID_CliProxy;
    GUID				iid_ivirusprotect		= _IID_IVirusProtect;
    GUID				iid_isavinfo			= _IID_ISavInfo;
    IVirusProtect*		rootObject				= NULL;
	ISavInfo*			opStateObject			= NULL;
	SavInfo				stateInfo				= {0};
	HRESULT				returnValHR				= E_FAIL;

    returnValHR = CoCreateInstance(clsid_cliproxy, NULL, CLSCTX_INPROC_SERVER, iid_ivirusprotect, reinterpret_cast<void **>(&rootObject));
    if (SUCCEEDED(returnValHR))
    {
        returnValHR = rootObject->CreateByIID(iid_isavinfo, reinterpret_cast<void**>(&opStateObject));
		if (SUCCEEDED(returnValHR))
		{
			SavInfo info = {0};

			info.structureSize = sizeof(info);
			returnValHR = opStateObject->GetState(&info);
			if (SUCCEEDED(returnValHR))
				printf("OpState retrieval succeeded.\n");
			else
				printf("OpState retrieval FAILED, error 0x%08x.\n", returnValHR);
			printf("Info:  Service is running %s (no other data good if OFF)\n", (info.serviceRunning ? "ON" : "OFF"));
			printf("Info:  FileSystem AP %s\n", (info.fileSystemAutoprotectEnabled ? "ON" : "OFF"));
			printf("Info:  SymProtect %s\n", (info.symProtectedEnabled ? "ON" : "OFF"));
			printf("Info:  definitionsPatternRevision = %d.\n", info.definitionsPatternRevision);
			printf("Info:  definitionsPatternVersion = %d.\n", info.definitionsPatternVersion);
			printf("\n");
			printf("NOTE that this app is unsigned, and therefore you must have ccVerifyTrust disabled and be using the debug ccvrTrst.dll for Filesystem AP results.\n");
			opStateObject->Release();
		}
		else
		{
			printf("Error 0x%08x creating opstate object from root object.\n", returnValHR);
		}
		rootObject->Release();
	}
	else
	{
		printf("Error 0x%08x creating root object.\n", returnValHR);
	}
}
