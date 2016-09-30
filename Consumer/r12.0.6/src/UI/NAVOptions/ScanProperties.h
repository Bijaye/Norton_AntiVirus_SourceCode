#pragma once

#include "ccScanInterface.h"

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

    // IScanProperties overrides.
    virtual int GetProperty(const char* szProperty, int iDefault);
    virtual const char* GetProperty(const char* szProperty, const char* szDefault);
};
