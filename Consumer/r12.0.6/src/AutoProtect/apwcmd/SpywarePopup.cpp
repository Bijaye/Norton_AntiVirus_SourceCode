#include "StdAfx.h"
#include ".\spywarepopup.h"
#include "ThreatCatInfo.h"
#include "apwntres.h"
#include "Subscription.h"

CSpywarePopup::CSpywarePopup(void):
    CAlertUI()
{
}

CSpywarePopup::~CSpywarePopup(void)
{
}

//////////////////////////////////////////////////////////////////////
// CSpywarePopup::DoModal()

int CSpywarePopup::DoModal()
{
    // Top window title
    //
    m_pAlert->SetBriefDesc ( _Module.GetResourceInstance(), IDS_SPYWARE_BRIEF_DESC );
    m_pAlert->SetAlertTitle ( _Module.GetResourceInstance(), IDS_SPYWARE_ALERT_TITLE );

    // Expand the detail area by default
    //
    m_pAlert->SetShowDetail ( true );

    // ********************************************************************************
    //
    // Callbacks - these must go before table rows for the callbacks in the rows
    //
    dwHelpID = IDH_NAVW_AP_ALERT_SPYWARE;

    //
    // ********************************************************************************


    // ********************************************************************************
    // Table rows
    //
    
    // Threat name
    m_pAlert->AddTableRow ( CResourceHelper::LoadStringW ( IDS_THREAT_NAME, _Module.GetResourceInstance() ).c_str(),
                            strThreatName.c_str(),
                            this,
                            ulVirusID );

    // Threat categories
    CThreatCatInfo ThreatInfo;
    TCHAR szCategoryList[512] = {0};
    ThreatInfo.GetCategoryText ( strThreatCats.c_str(), szCategoryList, 512 );
    
    m_pAlert->AddTableRow ( (LPCSTR)CResourceHelper::LoadString ( IDS_THREAT_CATEGORY, _Module.GetResourceInstance() ).c_str(),
                            szCategoryList);

    //
    // ********************************************************************************

    // Actions - align this with the return codes! +1
    AddActionAccelerator ( IDS_SCAN_NOW );
    AddActionAccelerator ( IDS_EXCLUDE );
    AddActionAccelerator ( IDS_IGNORE_TIME );
	m_pAlert->SetRecAction ( 0 );

    // Show!
    //
    int iActionTaken = DisplayAlert();

    if( iActionTaken == RESULT_SCAN )
    {
        // Good
        BOOL bAgreedToEULA;
        CSubscription Subscription;
        if ( SUCCEEDED (Subscription.HasUserAgreedToEULA (&bAgreedToEULA))
             && !bAgreedToEULA)
        {
            if( IDYES == ::MessageBox(NULL, CResourceHelper::LoadString(IDS_LAUNCH_CFGWIZ, _Module.GetResourceInstance()).c_str(), 
                                    csProductName, MB_YESNO) )
            {
                Subscription.LaunchSubscriptionWizard();
            }
        }
    }

    return iActionTaken;
}
