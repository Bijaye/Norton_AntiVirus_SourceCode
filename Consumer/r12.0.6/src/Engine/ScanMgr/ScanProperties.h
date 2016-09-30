#pragma once

class CScanManager;

class CScanProperties : 
    public IScanProperties,
    public ISymBaseImpl<CSymThreadSafeRefCount>
{
public:

    SYM_INTERFACE_MAP_BEGIN()
        SYM_INTERFACE_ENTRY(IID_ScanProperties, IScanProperties)
    SYM_INTERFACE_MAP_END()

    CScanProperties(void);
    ~CScanProperties(void);

    // Initialization
    void Initialize(CScanManager* pScanMgr);
    void DeInitialize();

    // IScanProperties overrides.
    virtual int GetProperty(const char* szProperty, int iDefault);
    virtual const char* GetProperty(const char* szProperty, const char* szDefault);

protected:
    CScanManager* m_pScanMgr;
};
