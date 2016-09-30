#pragma once

#include "ThreatLevelLoader.h"

class CIWPThreatLevel
{
public:
    CIWPThreatLevel(void) throw();
    virtual ~CIWPThreatLevel(void) throw();

private:
    CIWPThreatLevel(const CIWPThreatLevel&) throw();
    CIWPThreatLevel& operator=(const CIWPThreatLevel&) throw();

public:
    enum IWP_THREAT_LEVEL
    {
        IWP_THREAT_LOW,
        IWP_THREAT_MEDIUM,
        IWP_THREAT_HIGH,
    };

    bool Initialize() throw();

    // Used to perfrom the Threat Level test fora  given application
    bool GetThreatLevel(LPCSTR szApplicationPath, IWP_THREAT_LEVEL& eLevel ) throw();

private:
    bool InitializeTestResultValues(cc::IValueCollection* pResultValues) throw();

    IWP_THREAT_LEVEL ComputeThreatLevel() throw();
    
protected:
    // The Threat Level loader and object
    ThreatLevel_Loader m_TLevelLoader;
    IThreatLevelPtr m_pTLevel;
};
