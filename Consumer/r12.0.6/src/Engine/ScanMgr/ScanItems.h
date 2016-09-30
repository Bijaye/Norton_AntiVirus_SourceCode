#pragma once

class CScanManager;

class CScanItems : 
    public IScanItems,
    public ISymBaseImpl<CSymThreadSafeRefCount>
{
public:
    SYM_INTERFACE_MAP_BEGIN()
        SYM_INTERFACE_ENTRY(IID_ScanItems, IScanItems)
    SYM_INTERFACE_MAP_END()

    CScanItems(void);
    virtual ~CScanItems(void);

    // Initialization
    void Initialize(CScanManager* pScanMgr);
    void DeInitialize();

    virtual int GetScanItemCount();
    virtual const char* GetScanItemPath(int iIndex);

private:
    CScanManager* m_pScanMgr;
};
