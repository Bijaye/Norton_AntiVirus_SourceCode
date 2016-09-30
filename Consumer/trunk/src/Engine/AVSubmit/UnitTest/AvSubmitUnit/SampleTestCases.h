////////////////////////
//
// PROPRIETARY / CONFIDENTIAL.
// Use of this product is subject to license terms.
// Copyright © 2006 Symantec Corporation.
// All rights reserved.
//
/////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "EraserBase.h"
#include "FilterUnitTest.h"
#include "Resource.h"

struct SampleBase : CEraserBase
{
protected: 
	SampleBase(const wchar_t* n, const wchar_t* d) : CEraserBase(n,d) {}

	bool Execute()
	{
		ABORT_ON_FAIL(LoadAll());
		QBackup::IQBackupSetPtr pSet;
		AVModule::IAVMapDwordDataPtr pData;
		ccEraser::IRemediationActionPtr pRem;
		ABORT_ON_FAIL(CreateQBackupSet(pSet.m_p, GetGivenType()));
		ABORT_ON_FAIL(CreateRemediations(pSet));
		ABORT_ON_FAIL(CreateAVMap(pSet, pData.m_p));
		ABORT_ON_FAIL(AutoSubmit(pData, GetFlags()));
		return true;
	}
	virtual bool SaveUndo() = 0;
	virtual bool CreateRemediations(QBackup::IQBackupSet* pSet) = 0;
	virtual DWORD GetFlags() const {return AVSubmit::IAVSubmissionManager::eAll;}
	virtual REFGUID GetGivenType() const {return avQBTypes::GT_AVANOMALYSET;}

	bool CreateRemediation(QBackup::IQBackupSet* pSet, LPCTSTR pszFile = NULL)
	{
		ccEraser::IRemediationActionPtr pRem, pRem2;
		ABORT_ON_FAIL(CreateFileRemediation(pRem.m_p, pszFile));
		ABORT_ON_FAIL(CreateRegistryRemediation(pRem2.m_p));
		ABORT_ON_FAIL(SaveRemediation(pSet, pRem, SaveUndo()));
		ABORT_ON_FAIL(SaveRemediation(pSet, pRem2, false));
		if(SaveUndo())
		{
			CCUNIT_TEST_CONDITION_RETURN(pRem->Remediate() == ccEraser::Success);
		}
		return true;
	}
	bool InitAnomaly(QBackup::IQBackupSet* pSet)
	{
		ccEraser::IAnomalyPtr pAnomaly;
		ABORT_ON_FAIL(CreateAnomaly(pAnomaly.m_p));
		ABORT_ON_FAIL(SaveAnomaly(pSet, pAnomaly));
		return true;
	}

	bool InitAnomalyEx(QBackup::IQBackupSet* pSet)
	{
		ccEraser::IAnomalyPtr pAnomaly;
		ccEraser::IRemediationActionListPtr pList;
		size_t nCount = 0;
		ABORT_ON_FAIL(GenerateAnomaly(pAnomaly.m_p));
		ABORT_ON_FAIL(SaveAnomaly(pSet, pAnomaly));

		pAnomaly->GetRemediationActions(pList.m_p);
		CCUNIT_TEST_CONDITION_RETURN(pList != NULL);
		pList->GetCount(nCount);
		CCUNIT_TEST_CONDITION_RETURN(nCount > 0);

		for(size_t i = 0; i<nCount; ++i)
		{
			ccEraser::IRemediationActionPtr pRem;
			pList->GetItem(i, pRem.m_p);
			CCUNIT_TEST_CONDITION_RETURN(pRem != NULL);
			ABORT_ON_FAIL(SaveRemediation(pSet, pRem, SaveUndo()));
			if(SaveUndo())
			{
				CCUNIT_TEST_CONDITION_RETURN(ccEraser::Succeeded(pRem->Remediate()));
			}

		}
		return true;
	}
};

struct SingleSample : SampleBase
{
	SingleSample (const wchar_t* n, const wchar_t* d) : SampleBase (n,d) {}
	virtual bool CreateRemediations(QBackup::IQBackupSet* pSet)
	{
		return CreateRemediation(pSet);
	}
};

struct SingleSampleNoUndo : SingleSample
{
	SingleSampleNoUndo() : SingleSample(L"AutoSubmit Sample, no undo", L"AutoSubmit remediation - No Undo Information") {}
	bool SaveUndo() {return false;}
};

struct SingleSampleUndo : SingleSample
{
	SingleSampleUndo() : SingleSample(L"AutoSubmit Sample with undo", L"AutoSubmit remediation with Undo Information") {}
	bool SaveUndo() {return true;}
};

struct LoopingSingleSample : SingleSample
{
	LoopingSingleSample() : SingleSample(L"Autosubmit 1000 samples", L"AutoSubmit a lot of crap in a loop") {}
	bool SaveUndo() {return true;}
	bool Execute()
	{
		ABORT_ON_FAIL(LoadAll());
		for(int i = 0; i<1000; ++i)
		{
			QBackup::IQBackupSetPtr pSet;
			AVModule::IAVMapDwordDataPtr pData;
			ccEraser::IRemediationActionPtr pRem;
			ABORT_ON_FAIL(CreateQBackupSet(pSet.m_p, GetGivenType()));
			ABORT_ON_FAIL(CreateRemediations(pSet));
			ABORT_ON_FAIL(CreateAVMap(pSet, pData.m_p));
			ABORT_ON_FAIL(AutoSubmit(pData, GetFlags()));
		}
		//::MessageBox(0, _T("Done"), _T(__FUNCTION__), 0);
		return true;
	}
};

struct SingleDetection : SingleSample
{
	SingleDetection() : SingleSample(L"AutoSubmit Detection Only", L"AutoSubmit Detection with NO Undo Information") {}
	bool SaveUndo() {return false;}
	DWORD GetFlags() const {return AVSubmit::IAVSubmissionManager::eDetection;}
};


struct SingleDetectionUndo : SingleSample
{
	SingleDetectionUndo() : SingleSample(L"AutoSubmit Detection Only with undo", L"AutoSubmit Detection with Undo Information") {}
	bool SaveUndo() {return true;}
	DWORD GetFlags() const {return AVSubmit::IAVSubmissionManager::eDetection;}
};

struct MultiSample : SampleBase
{
	MultiSample (const wchar_t* n, const wchar_t* d) : SampleBase (n,d) {}
	virtual bool CreateRemediations(QBackup::IQBackupSet* pSet)
	{
		for(int i = 0; i< 2; ++i)
		{
			ABORT_ON_FAIL(CreateRemediation(pSet));
		}
		return true;
	}
};

struct MultiSampleNoUndo : MultiSample
{
	MultiSampleNoUndo() : MultiSample(L"AutoSubmit Composit Sample, no undo", L"AutoSubmit Sample with multiple remediations - No Undo Information") {}
	bool SaveUndo() {return false;}
};

struct MultiSampleUndo : MultiSample
{
	MultiSampleUndo() : MultiSample(L"AutoSubmit Composit Sample with undo", L"AutoSubmit multiple remediations with Undo Information") {}
	bool SaveUndo() {return true;}
};

struct CSFSample : SingleSample
{
	cc::IStringPtr pName;
	CSFSample() : SingleSample(L"AutoSubmit Clean Stealth File Sample", L"AutoSubmit Clean Stealth File ") {}
	bool SaveUndo() {return true;}
	virtual REFGUID GetGivenType() const {return avQBTypes::GT_CLEANSTEALTHBACKUPSET;}
	virtual bool CreateRemediations(QBackup::IQBackupSet* pSet)
	{
		ccEraser::IRemediationActionPtr pRem;
		cc::IKeyValueCollectionPtr pProps;
		ISymBasePtr pBase;
		ABORT_ON_FAIL(CreateFileRemediation(pRem.m_p));
		CCUNIT_TEST_CONDITION_RETURN(ccEraser::Succeeded(pRem->GetProperties(pProps.m_p)));
		CCUNIT_TEST_CONDITION_RETURN(pProps->GetValue(ccEraser::IRemediationAction::Path, pBase));
		cc::IStringQIPtr pFile = pBase;
		CCUNIT_TEST_CONDITION_RETURN(pFile != NULL);
		pName.Attach(ccSym::CStringImpl::CreateStringImpl(pFile->GetStringW()));

		pProps.Release();
		CCUNIT_TEST_CONDITION_RETURN(SYM_OK == pSet->GetProperties(pProps.m_p));
		CCUNIT_TEST_CONDITION_RETURN(pProps->SetValue(avQBTypes::avCSFProp_StealthFileName, pName));
		CCUNIT_TEST_CONDITION_RETURN(pSet->Flush() == SYM_OK);

		ABORT_ON_FAIL(SaveRemediation(pSet, pRem, SaveUndo()));
		return true;
	}
	virtual bool AutoSubmit(AVModule::IAVMapBase* _pMap, DWORD dwType)
	{
		AVModule::IAVMapDwordDataQIPtr pMap = _pMap;
		pMap->SetValue(AVModule::ThreatTracking::TrackingData_THREAT_TYPE, DWORD(AVModule::ThreatTracking::ThreatType_CleanStealthFile));
		return CEraserBase::AutoSubmit(pMap, dwType);
	}
};

struct COHSample : SingleSample
{
	COHSample() : SingleSample(L"AutoSubmit COH Sample", L"AutoSubmit COH Sample") {}
	bool SaveUndo() {return true;}
	virtual bool CreateRemediations(QBackup::IQBackupSet* pSet)
	{
		ABORT_ON_FAIL(CreateRemediation(pSet));
		ABORT_ON_FAIL(InitAnomaly(pSet));
		return true;
	}
	virtual bool AutoSubmit(AVModule::IAVMapBase* _pMap, DWORD dwType)
	{
		AVModule::IAVMapDwordDataQIPtr pMap = _pMap;
		pMap->SetValue(AVModule::ThreatTracking::TrackingData_VID, DWORD(0x4ade));
		pMap->SetValue(AVModule::ThreatTracking::TrackingData_NAME, L"Bloodhound.COH.Spyware");
		return CEraserBase::AutoSubmit(pMap, dwType);
	}
};

struct OLESample : SingleSample
{
	OLESample() : SingleSample(L"AutoSubmit OLE Sample", L"AutoSubmit OLE Sample") {}
	bool SaveUndo() {return true;}
	virtual bool CreateRemediations(QBackup::IQBackupSet* pSet)
	{
		ccLib::CString sPath;
		CCUNIT_TEST_CONDITION_RETURN(CFilterUnitTest::BinResToFile(_T("TestFilterInput.doc"), sPath, IDR_WORDDOC));
		ABORT_ON_FAIL(CreateRemediation(pSet, sPath));
		return true;
	}
};

struct ManualSample : SingleSample
{
	ManualSample() : SingleSample(L"ManualSubmissionSample", L"Simulate Manual Submission") {}
	virtual bool SaveUndo() {return true;}
	virtual bool CreateRemediations(QBackup::IQBackupSet* pSet)
	{
		return InitAnomalyEx(pSet);
	}

	virtual bool AutoSubmit(AVModule::IAVMapBase* _pMap, DWORD dwType)
	{
		AVModule::IAVMapDwordDataQIPtr pMap = _pMap;
		pMap->RemoveValue(AVModule::ThreatTracking::TrackingData_VID);
		pMap->RemoveValue(AVModule::ThreatTracking::TrackingData_NAME);
		//switch to  manual submission...
		return CEraserBase::ManualSubmit(pMap);
	}
};

struct AnomalyOnlySample : SingleSample
{
	AnomalyOnlySample() : SingleSample(L"AnomalyOnlySample", L"Simulate unremediated anomaly only sample") {}
	virtual bool SaveUndo() {return true;}
	virtual bool CreateRemediations(QBackup::IQBackupSet* pSet)
	{
		ccEraser::IAnomalyPtr pAnomaly;
		ABORT_ON_FAIL(GenerateAnomaly(pAnomaly.m_p));
		ABORT_ON_FAIL(SaveAnomaly(pSet, pAnomaly));
		return true;
	}
	virtual bool AutoSubmit(AVModule::IAVMapBase* _pMap, DWORD dwType)
	{
		AVModule::IAVMapDwordDataQIPtr pMap = _pMap;
		pMap->SetValue(AVModule::ThreatTracking::TrackingData_VID, DWORD(0x4adc));
		pMap->SetValue(AVModule::ThreatTracking::TrackingData_NAME, L"Possible Hueristic Trojan Worm (mikeo unittest)");
		return CEraserBase::AutoSubmit(pMap, dwType);
	}
};

struct OEHTest : CEraserBase
{
	OEHTest() : CEraserBase(L"OEHTest", L"Simulate OEH Detection") {}
	bool Execute()
	{
		ABORT_ON_FAIL(LoadAll());
		AVModule::IAVMapStrDataPtr pData;
		cc::IStringPtr pSample;
		ABORT_ON_FAIL(CreateTempFile(pSample.m_p));
		ABORT_ON_FAIL(CreateAVStringMap(NULL, pData.m_p));
		ABORT_ON_FAIL(LoadOEHMap(pData, pSample));
		ABORT_ON_FAIL(AutoSubmit(pData, GetFlags()));
		return true;
	}
	virtual DWORD GetFlags() const {return AVSubmit::IAVSubmissionManager::eAll;}
	virtual bool LoadOEHMap(AVModule::IAVMapStrData* pData, cc::IString* pSample)
	{
		CCUNIT_TEST_CONDITION_RETURN(pData->SetValue(AVModule::EmailScanner::AV_EMAILOEH_WORMPATH, pSample->GetStringW())==S_OK);
		return true;
	}
};