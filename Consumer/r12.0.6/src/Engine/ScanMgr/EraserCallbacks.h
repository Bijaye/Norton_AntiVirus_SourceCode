#pragma once

class CScanManager;

class CEraserCallbacks : 
    public ccEraser::ICallback,
    public ISymBaseImpl<CSymThreadSafeRefCount>
{
public:
    SYM_INTERFACE_MAP_BEGIN()
        SYM_INTERFACE_ENTRY(ccEraser::IID_Callback, ccEraser::ICallback)
    SYM_INTERFACE_MAP_END()

    CEraserCallbacks(void);
    virtual ~CEraserCallbacks(void);

    // Initialization
    void Initialize(CScanManager* pScanMgr);
    void DeInitialize();

    // ICallback implementation.
    virtual ccEraser::eResult PreDetection(ccEraser::IDetectionAction* pDetectAction, const ccEraser::IContext* pContext) throw();
    virtual ccEraser::eResult PostDetection(ccEraser::IDetectionAction* pDetectAction, ccEraser::eResult eDetectionResult, const ccEraser::IContext* pContext) throw();
    virtual ccEraser::eResult OnAnomalyDetected(ccEraser::IAnomaly* pAnomaly, const ccEraser::IContext* pContext) throw();

private:
    CScanManager* m_pScanMgr;
};
