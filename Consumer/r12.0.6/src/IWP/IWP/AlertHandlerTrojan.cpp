#include "StdAfx.h"
#include ".\alerthandlertrojan.h"
#include "TrayNotifyUI.h"
#include "ResourceHelper.h"
#include "resource.h"

using namespace ccEvtMgr;
using namespace SymNeti;
using namespace cc;
using namespace ccFirewallSettings;

CAlertHandlerTrojan::CAlertHandlerTrojan(void)
{
    if ( SYM_FAILED (m_FirewallSettingsLoader.CreateObject ( &m_pFirewallSettings)) ||
         !m_pFirewallSettings.m_p)
    {
        CCTRACEE ("CAlertHandlerTrojan - failed loading firewall settings");
        m_pFirewallSettings.m_p = NULL;
    }
    else
    {
        if (!Succeeded (m_pFirewallSettings->GetTrojanRules ( m_pRuleList.m_p )))
        {
            CCTRACEE ( "CAlertHandlerTrojan - failed getting Trojan rules" );
        }
    }
}

CAlertHandlerTrojan::~CAlertHandlerTrojan(void)
{
}

// Handles Trojan events
//
IAlertData::ALERT_RESULT CAlertHandlerTrojan::DisplayAlert( SymNeti::CSNAlertEventEx* pSNEvent,
                                                            LPTSTR szAppName,
                                                            IAlertData::ALERT_TYPE eAlertType)
{
	if ( !pSNEvent || !szAppName )
        return IAlertData::NO_ACTION;

    m_pSNEvent = pSNEvent;
    m_eAlertType = eAlertType;
    m_strAppName = szAppName;

    commonInit();

    char szRuleSummary[256] = {0};      // Trojan Horse rule summary, used as key for exclusions

    // Display the tray alert first - scope for memory
    {
        CTrayNotifyUI trayalert;
        std::string strTrayAlertText;
        CResourceHelper::LoadString ( IDS_IDS_DESC, strTrayAlertText, g_hInstance );

        if ( !trayalert.Start (strTrayAlertText.c_str()))
            return IAlertData::NO_ACTION;

        // If the user doesn't want "More Info" then we are done.
        if (!trayalert.m_bUserWantsMoreInfo)
            return IAlertData::NO_ACTION;
    }

	// Get our tlevel keys and the name of the rule that caused this event
	char szRuleDetails[256] = {0};
	char szRuleName[256] = {0};
	BOOL bListenEvent = FALSE;
    
	pSNEvent->GetAlertInfo(CSNAlertEventEx::eRuleName, szRuleName, 255);
	pSNEvent->GetAlertInfo(CSNAlertEventEx::eRuleDetails, szRuleDetails, 255);
	pSNEvent->GetAlertInfo(CSNAlertEventEx::eIsListenEvent, &bListenEvent, sizeof(bListenEvent));

	// Rules with summary UserDefined are rules created by the user
	// Rules with summary NoAlert are default system rules that normally do not alert
	// All others are assumed to be trojan rules
	if (( strcmpi(szRuleSummary, "UserDefined") == 0) ||
        ( strcmpi(szRuleSummary, "NoAlert") == 0))
    {
        // Should never get here.
        return IAlertData::NO_ACTION;
    }

   
    // Brief description
    CResourceHelper::LoadString ( IDS_IDS_DESC, m_strDescription, g_hInstance );

    // More description
    std::string strRuleKey;
    CResourceHelper::LoadString ( IDS_SECURITY_RULE, strRuleKey, g_hInstance );
    m_alert.GetAlert()->AddTableRow ( strRuleKey.c_str(), szRuleName);

	// Title
    //
    m_alert.GetAlert()->SetAlertTitle ( g_hInstance, IDS_SECURITYALERT );

    // No actions to take
    // Display a "Don't show this alert again" box.
    m_alert.GetAlert()->SetCheckBoxText ( g_hInstance, IDS_SECDONTASKAGAINCHECK, FALSE );

    m_alert.dwHelpID = NAV_CSH_IWP_TROJAN_ALERT;

    displayCommonAlert ();

    // If "Do not show again" was checked, then save off the setting to not show this alert again
    //
    if ( m_alert.GetAlert()->GetCheckBoxState () && m_pRuleList.m_p && m_pFirewallSettings.m_p )
    {
        // Get rule info
        //
        size_t nCount = 0;
//		cc::IStringPtr pStrDesc;
		char szTempName [256] = {0};
        DWORD dwSizeTempName = 256;
        if ( Succeeded (m_pRuleList->GetItemCount ((long&)nCount)))
        {
            for ( long lIndex = 0; lIndex < nCount; lIndex++)
            {
                ccFirewallSettings::IRulePtr pRule;
                if ( Succeeded (m_pRuleList->GetItem (lIndex, pRule.m_p)))
                {
                    // Reset the buffer
                    //
                    DWORD dwSize = dwSizeTempName;
                    ZeroMemory ( szTempName, dwSizeTempName );

                    pRule->GetDescription (szTempName, dwSize);
//                    pRule->GetDescription (pStrDesc);
//                    if ( pStrDesc->GetLength() > 0 && 0 == stricmp ( szRuleName, pStrDesc->GetStringA() ))
                    if ( strlen(szTempName) > 0 && 0 == stricmp ( szRuleName, szTempName ))
                    {
                        // Match
                        IRule::LoggingType eLogging;
                        pRule->GetLogging (eLogging);
                        if ( eLogging > IRule::LoggingType::Dialog)
                            eLogging = (IRule::LoggingType)(eLogging - IRule::LoggingType::Dialog); // add exclusion means remove "dialog"
                        pRule->SetLogging (eLogging);
                        m_pFirewallSettings->SetTrojanRules (m_pRuleList);
                        break;
                    }
                }
            }
        }
        else
            CCTRACEE ("CAlertHandlerTrojan::DisplayAlert - failed getting rule info");
    }

    return IAlertData::NO_ACTION;
}


