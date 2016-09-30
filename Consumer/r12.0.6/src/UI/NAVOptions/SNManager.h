#pragma once
#include "SymNetDriverAPI.h"
#include "ccModule.h"			// for loading dlls

class CSNManager
{
public:
    CSNManager(void);
    ~CSNManager(void);
    HRESULT SetAutoBlockEnabled(bool bOn);
    HRESULT GetAutoBlockEnabled(bool &bOn);

protected:
	ccLib::CModule m_modSymNeti;
    bool m_bInit;   // Did we init successfully?
    
    // Functions
    SNGETFIREWALLSETTING m_pSNGetFirewallSetting;
    SNSETFIREWALLSETTING m_pSNSetFirewallSetting;
    SNUNINIT m_pSNUnInit;
    SNINIT m_pSNInit;
};
