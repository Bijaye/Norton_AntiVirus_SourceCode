#pragma once
#include "SymIDSI.h"
#include "SymNetDriverAPI.h"
#include "ccModule.h"			// for loading dlls

class CFWManager
{
public:
    CFWManager(void);
    ~CFWManager(void);
    SYMRESULT ResetSymNetDrvSettings(PFACTORYRESETPARAMS pFRP);
    SYMRESULT CFWManager::SetFirewallSettings (bool bOn, bool bAllClients = false, bool bFirewallOnly = false);

protected:
	ccLib::CModule m_modSymNeti;
    bool m_bInit;   // Did we init successfully?
    
    // Functions
    SNRESETTOFACTORYDEFAULTS m_pSNResetToFactoryDefaults;
    SNSETFIREWALLSETTING m_pSNSetFirewallSetting;
    SNSETPRODUCTSTATE m_pSNSetProductState;
    SNSETFRAGMENTEDPKTACTION m_pSNSetFragmentedPktAction;
    SNUNINIT m_pSNUnInit;
    SNINIT m_pSNInit;
};
