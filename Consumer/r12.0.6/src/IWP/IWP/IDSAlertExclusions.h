#pragma once

#include "SMHelper.h"

class CIDSAlertExclusions
{
public:
    CIDSAlertExclusions(void);
    ~CIDSAlertExclusions(void);

    bool IsExcluded (DWORD dwIDSSignature);
    bool AddExclusion (DWORD dwIDSSignature);
    bool RemoveExclusion (DWORD dwIDSSignature);
    bool RemoveAll (void);

protected:  
    CSMHelper m_exclusions;
};
