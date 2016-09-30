#include "StdAfx.h"
#include ".\sideeffectspopup.h"

CSideEffectsPopup::CSideEffectsPopup(void):
    CAlertUI()
{
}

CSideEffectsPopup::~CSideEffectsPopup(void)
{
}

//////////////////////////////////////////////////////////////////////
// CSideEffectsPopup::DoModal()

int CSideEffectsPopup::DoModal()
{
    try
    {
        // Top window title
        //
		CString csDescFormat;
		CString csDesc;
		csDescFormat.LoadString(_Module.GetResourceInstance(), IDS_SIDEEFFECTS_BRIEF_DESC);
		csDesc.Format(csDescFormat, csProductName);

        m_pAlert->SetBriefDesc ( csDesc );
        m_pAlert->SetAlertTitle ( _Module.GetResourceInstance(), IDS_SIDEEFFECTS_ALERT_TITLE );

        // Expand the detail area by default
        //
        m_pAlert->SetShowDetail ( true );

        // ********************************************************************************
        //
        // Callbacks - these must go before table rows for the callbacks in the rows
        //
        dwHelpID = IDH_NAVW_AP_ALERT_SIDE_EFFECTS;

        //
        // ********************************************************************************


        // ********************************************************************************
        // Table rows
        //
        for ( long lIndex = 0; lIndex < (long)vecSideEffects.size (); lIndex++)
        {
            // Threat name - to support 9x we need to convert here
            std::vector<char> pszThreatName;
            pszThreatName.resize (vecSideEffects.at(lIndex).strThreatName.size()*sizeof(wchar_t));

            wcstombs( &pszThreatName[0],
                      vecSideEffects.at(lIndex).strThreatName.c_str(),
                      vecSideEffects.at(lIndex).strThreatName.size());

            m_pAlert->AddTableRow ( CResourceHelper::LoadString( IDS_THREAT_NAME, _Module.GetResourceInstance() ).c_str(),
                                    &pszThreatName[0],
                                    this,
                                    vecSideEffects.at(lIndex).ulVirusID );

        }

        //
        // ********************************************************************************

        // Actions - align this with the return codes!
        AddActionAccelerator ( IDS_SCAN_NOW );
        AddActionAccelerator ( IDS_IGNORE );
    	m_pAlert->SetRecAction ( 0 );

        // Show!
        //
        int iActionTaken = DisplayAlert();

        return iActionTaken;
    }
    catch (...)
    {
        CCTRACEE (_T("CSideEffectsPopup::DoModal"));
    }
    
    return RESULT_FAIL;
}
