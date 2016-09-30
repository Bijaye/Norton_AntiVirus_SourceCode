#include "StdAfx.h"
#include "erasercallbacks.h"
#include "ScanManager.h"

CEraserCallbacks::CEraserCallbacks(void)
{
}

CEraserCallbacks::~CEraserCallbacks(void)
{
}

// Initialization
void CEraserCallbacks::Initialize(CScanManager* pScanMgr)
{
    if( pScanMgr )
        m_pScanMgr = pScanMgr;
}

// De-Initialization
void CEraserCallbacks::DeInitialize()
{
    if( m_pScanMgr )
        m_pScanMgr = NULL;
}

// ICallback implementation.
ccEraser::eResult CEraserCallbacks::PreDetection(ccEraser::IDetectionAction* pDetectAction, const ccEraser::IContext* pContext)
{
    if( m_pScanMgr )
        return m_pScanMgr->PreDetection(pDetectAction, pContext);

    CCTRACEE(_T("CEraserCallbacks::PreDetection() - Scan manager invalid."));

    return ccEraser::Continue;
}

ccEraser::eResult CEraserCallbacks::PostDetection(ccEraser::IDetectionAction* pDetectAction, ccEraser::eResult eDetectionResult, const ccEraser::IContext* pContext)
{
    if( m_pScanMgr )
        return m_pScanMgr->PostDetection(pDetectAction, eDetectionResult, pContext);

    CCTRACEE(_T("CEraserCallbacks::PostDetection() - Scan manager invalid."));

    return ccEraser::Continue;
}

ccEraser::eResult CEraserCallbacks::OnAnomalyDetected(ccEraser::IAnomaly* pAnomaly, const ccEraser::IContext* pContext)
{
    if( m_pScanMgr )
        return m_pScanMgr->OnAnomalyDetected(pAnomaly, pContext);

    CCTRACEE(_T("CEraserCallbacks::OnAnomalyDetected() - Scan manager invalid."));

    return ccEraser::Continue;
}