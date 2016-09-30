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
#pragma once
#include "cceraserinterface.h"
#include "resource.h"
#include "SketchbookDlg.h"

class CCallback :
	public ccEraser::ICallback,
	public ISymBaseImpl<CSymThreadSafeRefCount>
{
public:
	CCallback(void);
	virtual ~CCallback(void);

	CSketchbookDlg * m_pDlg;
	BOOL m_bSkip;
	DWORD m_dwDetectedCalled;

	ccEraser::IAnomalyListPtr m_pAnomalyList;

	SYM_INTERFACE_MAP_BEGIN()
        SYM_INTERFACE_ENTRY( ccEraser::IID_Callback, ccEraser::ICallback )
    SYM_INTERFACE_MAP_END()

	virtual ccEraser::eResult PreDetection(ccEraser::IDetectionAction* pDetectAction, const ccEraser::IContext* pContext) throw();
	virtual ccEraser::eResult PostDetection(ccEraser::IDetectionAction* pDetectAction, ccEraser::eResult eDetectionResult, const ccEraser::IContext* pContext) throw();
    virtual ccEraser::eResult OnAnomalyDetected(ccEraser::IAnomaly* pAnomaly, const ccEraser::IContext* pContext) throw();

	void PutScanStatus(ccEraser::IContextPtr pContext);

private:
	    // Disallowed
        CCallback(const CCallback &) throw();
        CCallback& operator=(const CCallback&) throw();

	
};
