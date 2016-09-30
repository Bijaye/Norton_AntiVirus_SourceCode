#pragma once
#include "ccModule.h"
#include "IMScan.h"     // For IMScanner functions

class CIMScanWrapper
{
public:
    CIMScanWrapper(void) throw (bool);
    virtual ~CIMScanWrapper(void);

    bool IsIMScannerInstalled (IMTYPE imtype);

private:
    ccLib::CModule m_modIMScan;
    pfnISIMCLIENTINSTALLED m_pfnIsIMScannerInstalled;

};
