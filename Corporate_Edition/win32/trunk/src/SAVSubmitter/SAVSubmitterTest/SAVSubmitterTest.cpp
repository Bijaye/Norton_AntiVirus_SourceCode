// SAVSubmitterTest.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

//#include <iostream>

//#include "SubmissionEngineInterface.h"
#include "SAVSubmitterLoaderHelper.h"
#include "DummyQuarantineSample.h"
#include "DummyAnomaly.h"

#include "ccScanwInterface.h"
#include "ccScanwLoader.h"
#include "ccSymDebugOutput.h"
#include "ccTrace.h"

ccLib::CDebugOutput g_DebugOutput(_T("SAVSubmitterTest"));
IMPLEMENT_CCTRACE(g_DebugOutput);

// Needed to define globals that are used by CDummyAnomaly implementation
SYM_OBJECT_MAP_BEGIN()
SYM_OBJECT_MAP_END() 


static void Error(const TCHAR* tzMessage, HRESULT hResult)
{
	fprintf(stderr, "%s0x%08X", tzMessage, hResult);
//	std::wcerr << tzMessage;
//	std::wcerr << hResult;
//	std::wcerr << _T("\r\n");
}

static HRESULT NewDummyAnomaly(CDummyAnomaly::enumAnomalyType eAnomalyType, CDummyAnomaly*& pResult)
{
	HRESULT hResult = E_OUTOFMEMORY;

	if (pResult != NULL)
	{
		pResult->Release();
		pResult = NULL;
	}

	pResult = new CDummyAnomaly(eAnomalyType);
	if (pResult != NULL)
	{
		pResult->AddRef();
		hResult = pResult->Initialize();
		if (FAILED(hResult))
		{
			pResult->Release();
			pResult = NULL;
		}
	}
	else
	{
		hResult = E_OUTOFMEMORY;
	}
	return hResult;
}

static HRESULT DoQuarantineSubmission(ccScanw::IScannerw* pScanner, CDummyQuarantineSample::enumSampleType eSampleType)
{
	HRESULT hResult;
	// Loader's lifetime must be > than that of the ISAVSubmitter* that we use inside the block that follows
	SAVSubmission::savSubmission_ISAVSubmitter oSAVSubmitterLoader;

	{
		SAVSubmission::ISAVSubmitterPtr pSAVSubmitter;

		SYMRESULT sr = oSAVSubmitterLoader.CreateObject(&pSAVSubmitter);
		if (SYM_SUCCEEDED(sr))
		{
			CDummyQuarantineSample* pDummyManualQuarantineSample = new CDummyQuarantineSample(eSampleType);
			if (pDummyManualQuarantineSample != NULL)
			{
				pDummyManualQuarantineSample->AddRef();
				hResult = pSAVSubmitter->SubmitQuarantinedSample(pDummyManualQuarantineSample, pScanner);
				pDummyManualQuarantineSample->Release();
				if (!SUCCEEDED(hResult))
				{
					Error(_T("Error creating the quarantine submission: "), hResult);
				}
			}
			else
			{
				Error(_T("Error instantiating CDummyQuarantineSample"), E_OUTOFMEMORY);
				hResult = E_OUTOFMEMORY;
			}
		}
		else
		{
			Error(_T("Failed to load the ISAVSubmitter interface: "), sr);
			hResult = sr;
		}
	}
	return hResult;
}

static HRESULT DoDetectionSubmission(ccScanw::IScannerw* pScanner)
{
	HRESULT hResult;
	// Loader's lifetime must be > than that of the ISAVSubmitter* that we use inside the block that follows
	SAVSubmission::savSubmission_ISAVSubmitter oSAVSubmitterLoader;

	{
		SAVSubmission::ISAVSubmitterPtr pSAVSubmitter;

		SYMRESULT sr = oSAVSubmitterLoader.CreateObject(&pSAVSubmitter);
		if (SYM_SUCCEEDED(sr))
		{
			CDummyAnomaly* pDummyAnomaly = NULL;
			
			hResult = NewDummyAnomaly(CDummyAnomaly::eSimpleAVAnomaly, pDummyAnomaly);
			if (SUCCEEDED(hResult) && pDummyAnomaly != NULL)
			{
				hResult = pSAVSubmitter->SubmitAVDetection(pDummyAnomaly, pScanner);
				if (!SUCCEEDED(hResult))
				{
					Error(_T("Error creating the AV detection: "), hResult);
				}
				pDummyAnomaly->Release();
				pDummyAnomaly = NULL;
			}
			else
			{
				Error(_T("Error instantiating CDummyAnomaly"), hResult);
				hResult = E_OUTOFMEMORY;
			}
		}
		else
		{
			Error(_T("Failed to load the ISAVSubmitter interface: "), sr);
			hResult = sr;
		}
	}
	return hResult;
}

static void Usage(const TCHAR* tzToolName)
{
	_ftprintf(stderr, _T("Usage:\r\n\t%s (-avdetection | -quarantinemanual | -quarantinebloodhound)"), tzToolName);
	exit(1);
}

int _tmain(int argc, const TCHAR* argv[])
{
	if (argc != 2)
		Usage(argv[0]);

#define SUPPORT_REMOTE_DEBUGGING 1

#ifdef SUPPORT_REMOTE_DEBUGGING
	::MessageBox(NULL, _T("Attach remote debugger and press OK."), _T("Debugging"), MB_OK);
#endif

	int iiResult = 0;
	ccScanw::ccScan_IScannerw	scannerInterfaceLoader;
	ccScanw::IScannerwPtr		ptrScanner;
	SYMRESULT					result = scannerInterfaceLoader.CreateObject(ptrScanner);
	//SYMRESULT					result = ccScanw::mgdccScan_IScannerw::CreateObject(GETMODULEMGR(), ptrScanner);

	if (SYM_SUCCEEDED(result))
	{
		ccScanw::SCANWSTATUS scanStatus = ptrScanner->Initialize(L"SAV", L"C:\\temp\\", 2, 0);

		if (scanStatus == ccScanw::SCANW_OK)
		{
			HRESULT hResult;

			if (_tcsicmp(argv[1], _T("-avdetection")) == 0)
			{
				hResult = DoDetectionSubmission(ptrScanner);
			}
			else if (_tcsicmp(argv[1], _T("-quarantinemanual")) == 0)
			{
				hResult = DoQuarantineSubmission(ptrScanner, CDummyQuarantineSample::eManualSample);
			}
			else if (_tcsicmp(argv[1], _T("-quarantinebloodhound")) == 0)
			{
				hResult = DoQuarantineSubmission(ptrScanner, CDummyQuarantineSample::eViralBloodhoundSample);
			}
			else
			{
				assert(false);
				Usage(argv[0]);
				hResult = E_INVALIDARG;
			}
			if (FAILED(hResult))
			{
				Error(_T("Error submitting sample"), hResult);
				iiResult = 101;
			}
		}
		else
		{
			Error(_T("Error initializing ccScan interface"), scanStatus);
			return 2;
		}
	}
	else
	{
		Error(_T("Error retrieving ccScan interface"), result);
		return 1;
	}

	return 0;
}

