////////////////////////
//
// PROPRIETARY / CONFIDENTIAL.
// Use of this product is subject to license terms.
// Copyright © 2006 Symantec Corporation.
// All rights reserved.
//
/////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "ccUnitTestCaseImpl.h"
#include "ccEraserInterface.h"
#include "QBackupInterfaces.h"
#include "QBackupLoader.h"
#include "DefUtilsInterface.h"
#include <ccLib/ccSymModuleLifetimeMgrHelper.h>
#include "AVSubmissionInterface.h"
#include "AVInterfaces.h"
#include "AVInterfaceLoader.h"
#include "avqbackuptypes.h"
#include "ccscanwinterface.h"

class CEraserBase : public ccUnit::CTestCaseImpl
{
protected:
	CEraserBase(const wchar_t* name, const wchar_t* descr);
	virtual ~CEraserBase(void);

	bool Initialize(ccUnit::ITestHost* pTestHost)
	{
		if(!__super::Initialize(pTestHost))
			return false;
		return LoadAll();
	}

	bool LoadAll()
	{
		ABORT_ON_FAIL(LoadEraser());
		ABORT_ON_FAIL(LoadQBackup());
		ABORT_ON_FAIL(LoadAVSubmit());
		return true;
	}

	virtual bool LoadEraser();
	virtual bool LoadQBackup();
	virtual bool LoadAVSubmit();

	virtual bool CreateQBackupSet(QBackup::IQBackupSet*&, REFGUID type);
	virtual bool CreateFileRemediation(ccEraser::IRemediationAction*&, LPCTSTR sFile = NULL);
	virtual bool CreateRegistryRemediation(ccEraser::IRemediationAction*&);
	virtual bool CreateBackupStream(QBackup::IQBackupSet*, QBackup::IQBackupItemStream*&, REFGUID gt);
	virtual bool CreateAnomaly(ccEraser::IAnomaly*&); //create empty with  hueristic data
	virtual bool GenerateAnomaly(ccEraser::IAnomaly*&); //generate from file
	virtual bool InitializeAnomaly(ccEraser::IAnomaly*); //init some stock props...

	virtual bool SaveRemediation(QBackup::IQBackupSet* pSet, ccEraser::IRemediationAction* pRem, bool bSaveUndo);
	virtual bool SaveAnomaly(QBackup::IQBackupSet* pSet, ccEraser::IAnomaly* pAnomaly);
	virtual bool CreateAVMap(QBackup::IQBackupSet*, AVModule::IAVMapDwordData*& pMap);
	virtual bool CreateAVStringMap(QBackup::IQBackupSet*, AVModule::IAVMapStrData*& pMap);
	virtual bool AutoSubmit(AVModule::IAVMapBase* pMap, DWORD dwType);
	virtual bool ManualSubmit(AVModule::IAVMapDwordData* pMap);

	bool CreateTempFile(cc::IString*& File);

	ccEraser::IEraser4Ptr m_pEraser;
	QBackup::IQBackupPtr m_pBackup;
	ccScanw::IScannerwPtr m_pScanner;
	AVSubmit::IAVSubmissionManagerPtr m_pSubmit;

	AVModule::IAVGlobalScannerPtr m_pGlobal;
};
