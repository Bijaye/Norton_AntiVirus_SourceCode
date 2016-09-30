#pragma once

#include "AlertHandlerFW.h"
#include "ccFirewallSettingsInterface.h"
#include "ccFirewallSettingsModuleLoader.h"

class CAlertHandlerTrojan : public CAlertHandlerFW
{
public:
    CAlertHandlerTrojan(void);
    virtual ~CAlertHandlerTrojan(void);

    IAlertData::ALERT_RESULT DisplayAlert(  SymNeti::CSNAlertEventEx* pSNEvent,
                                            LPTSTR szAppName,
                                            IAlertData::ALERT_TYPE eAlertType) throw ();

protected:
    // Our Trojan rules
    FirewallSettingsLoader m_FirewallSettingsLoader;
    ccFirewallSettings::IFirewallSettingsPtr m_pFirewallSettings;
    ccFirewallSettings::IRulePtr m_pRule;
    ccFirewallSettings::IRuleListPtr m_pRuleList;

};
