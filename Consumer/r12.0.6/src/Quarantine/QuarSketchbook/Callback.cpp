// /////////////////////////////////////////////////////////////////////////////////////////////////
// Symantec copyright header start
// //////////////////////
//
// PROPRIETARY / CONFIDENTIAL.
// Use of this product is subject to license terms.
// Copyright © 2004 Symantec Corporation.
// All rights reserved.
//
// /////////////////////////////////////////////////////////////////////////////////////////////////
// Symantec copyright header stop
// //////////////////////
#include "StdAfx.h"
#include ".\callback.h"
#include "ccSymStringConvert.h"
#include "sketchbook.h"

CCallback::CCallback(void)
{
	m_pDlg = NULL;
	m_bSkip = FALSE;
	m_dwDetectedCalled = 0;
	m_pAnomalyList = NULL;
}

CCallback::~CCallback(void)
{
}

ccEraser::eResult CCallback::PreDetection(ccEraser::IDetectionAction* pDetectAction, const ccEraser::IContext* pContext) throw()
{
	if(m_pDlg != NULL)
	{
		if(m_pDlg->m_bScanAbort)
		{
			m_pDlg->m_bScanAbort = FALSE;
			return ccEraser::Abort;
		}
	}

	if(m_bSkip)
	{
		m_bSkip = FALSE;
		return ccEraser::False;
	}

	return ccEraser::Continue;
}

ccEraser::eResult CCallback::PostDetection(ccEraser::IDetectionAction* pDetectAction, ccEraser::eResult eDetectionResult, const ccEraser::IContext* pContext) throw()
{
	if(m_pDlg != NULL)
	{
		cc::IStringPtr pDescription;
		ccEraser::eResult res = pDetectAction->GetDescription(pDescription);
		if(Failed(res))
		{
			m_pDlg->setResultWindow(eDetectionResult, _T("Scan Return code"));
		}
		else
		{
			m_pDlg->setResultWindow(eDetectionResult, ccSym::CStringConvert::GetStringT(pDescription));
		}

		if(m_pDlg->m_bScanAbort)
		{
			m_pDlg->m_bScanAbort = FALSE;
			return ccEraser::eResult::Abort;
		}
	}

	return ccEraser::Continue;
}

ccEraser::eResult CCallback::OnAnomalyDetected(ccEraser::IAnomaly* pAnomaly, const ccEraser::IContext* pContext) throw()
{
	m_dwDetectedCalled++;

	if(m_pAnomalyList == NULL)
	{
		if(m_pDlg)
		{
			ccEraser::eResult res = m_pDlg->m_pEraser->CreateObject(ccEraser::AnomalyListType, ccEraser::IID_AnomalyList, (void **) &m_pAnomalyList);
			m_pDlg->setResultWindow(res, _T("m_pDlg->m_pEraser->CreateObject(ccEraser::AnomalyListType, ccEraser::IID_AnomalyList, (void **) &m_pAnomalyList)"));
			if(Failed(res))
			{
				return ccEraser::Abort;
			}
		}
		else
		{
			ccEraser::eResult res = theApp.m_pEraser->CreateObject(ccEraser::AnomalyListType, ccEraser::IID_AnomalyList, (void **) &m_pAnomalyList);
			if(Failed(res))
			{
				return ccEraser::Abort;
			}
		}
	}

	ccEraser::IAnomalyListPtr pAnomalyList;
	pAnomalyList.Attach(pContext->GetDetectedAnomalyList());
	size_t nCount = 0;
	ccEraser::eResult res = pAnomalyList->GetCount(nCount);
	m_pDlg->setResultWindow(res, _T("pAnomalyList->GetCount(nCount)"));
	if(Failed(res))
	{
		return ccEraser::Abort;
	}

	ccEraser::IAnomalyPtr pAnomaly2;
	size_t i;
	for(i = 0; i < nCount; i++)
	{
		res = pAnomalyList->GetItem(i, pAnomaly2);
		m_pDlg->setResultWindow(res, _T("pAnomalyList->GetItem(i, pAnomaly2)"));
		if(Failed(res))
		{
			return ccEraser::Abort;
		}

		if(pAnomaly2 == pAnomaly)
		{
			break;
		}
	}

	if(i == nCount)
	{
		::AfxMessageBox(_T("i == nCount"));
		return ccEraser::Abort;
	}

	res = m_pAnomalyList->GetCount(nCount);
	m_pDlg->setResultWindow(res, _T("m_pAnomalyList->GetCount(nCount)"));
	if(Failed(res))
	{
		return ccEraser::Abort;
	}

	for(i = 0; i < nCount; i++)
	{
		res = m_pAnomalyList->GetItem(i, pAnomaly2);
		m_pDlg->setResultWindow(res, _T("m_pAnomalyList->GetItem(i, pAnomaly2)"));
		if(Failed(res))
		{
			return ccEraser::Abort;
		}

		if(pAnomaly2 == pAnomaly)
		{
			::AfxMessageBox(_T("pAnomaly2 == pAnomaly"));
			return ccEraser::Abort;
		}
	}

	m_pAnomalyList->Add(pAnomaly);

	if(m_pDlg != NULL)
	{
		PutScanStatus(pContext);
		m_pDlg->m_pAnomalyList = pAnomalyList;

		m_pDlg->SetAnomalyCtrl();

		if(m_pDlg->m_bScanAbort)
		{
			m_pDlg->m_bScanAbort = FALSE;
			return ccEraser::Abort;
		}
	}

	return ccEraser::Continue;
}

void CCallback::PutScanStatus(ccEraser::IContextPtr pContext)
{
	size_t AnomalyCount = pContext->GetAnomalyCount();
	size_t AnomalyCompleteCount = pContext->GetAnomalyCompleteCount();
	size_t DetectionCount = pContext->GetDetectionCount();
	size_t DetectionCompleteCount = pContext->GetDetectionCompleteCount();

	CString strStatus;

	strStatus.Format(_T("Total Anomaly to Scan: %u; Anomaly Scan Completed: %u; Total Dectection Count: %u; Detection Scan Complete: %u"),
		AnomalyCount, AnomalyCompleteCount, DetectionCount, DetectionCompleteCount);
	m_pDlg->m_editCtrlScan.SetWindowText(strStatus);
}