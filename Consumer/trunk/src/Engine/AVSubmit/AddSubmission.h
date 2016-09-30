////////////////////////
//
// PROPRIETARY / CONFIDENTIAL.
// Use of this product is subject to license terms.
// Copyright © 2006 Symantec Corporation.
// All rights reserved.
//
/////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "AVInterfaces.h"
#include "SubmissionEngineInterface.h"

namespace AVSubmit {

class CAddSubmission : public ccLib::CTaskScheduler::CTask 
{
public:
	typedef std::auto_ptr<CAddSubmission> AutoPtr;

	CAddSubmission(void);
	virtual ~CAddSubmission(void);

	bool Initialize(AVModule::IAVMapBase* pData, cc::IString* pProduct, cc::IString* pVer, cc::IString* pActor);
	void SetType(DWORD dwType) {m_dwType = dwType;}
	void SetManual() {m_bManual = true;}
	HRESULT Process();

protected:
	enum SubmissionType
	{
		eUnknown,
		eSample,
		eDetection,
		eManual,
	};
	virtual void Execute(HANDLE hAbort) throw();
	HRESULT AddSubmissions();
	HRESULT GetThreatInfo(cc::IKeyValueCollection*);
	HRESULT CreateSubmission(cc::IKeyValueCollection*, SubmissionType);
	HRESULT AddEmail();
	HRESULT AddAPSamples(AVModule::IAVArrayData* pData);
	inline HRESULT CheckInit();
	inline cc::IKeyValueCollectionPtr CreateKVC() throw(_com_error);
	inline bool CheckType(AVModule::IAVMapDwordData* pData, DWORD dwIndex, DWORD dwType);

protected:
	AVModule::IAVMapDwordDataQIPtr m_pData;
	AVModule::IAVMapStrDataQIPtr m_pStrData;

	struct SubmissionRec
	{
		clfs::ISubmissionPtr pSubmission;
		SubmissionType eType;
		SubmissionRec() : eType(eUnknown) {}
		SubmissionRec(clfs::ISubmission* pSub, SubmissionType t) : pSubmission(pSub), eType(t) {}
	};

	typedef std::deque<SubmissionRec> SubmissionList;
	SubmissionList m_submissions;
	cc::IStringPtr m_pProduct;
	cc::IStringPtr m_pVersion;
	cc::IStringPtr m_pActor;
	bool m_bManual;
	DWORD m_dwType;

private:

	CAddSubmission(const CAddSubmission&);
	const CAddSubmission& operator = (const CAddSubmission&);

};

}
