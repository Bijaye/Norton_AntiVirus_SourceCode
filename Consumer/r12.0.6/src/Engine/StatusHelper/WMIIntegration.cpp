#include "StdAfx.h"
#include ".\wmiintegration.h"

#include "build.h"
#include "apwutil.h"
#include "ccSymCommonClientInfo.h"

CWMIIntegration::CWMIIntegration(void) : m_bIsPreXPSP2(true), m_bCfgWizFinished(FALSE)
{
    CCTRACEI ("CWMIIntegration - loading WSC");
    // We don't talk to WMI unless this is XP SP2+
    //
    ISShared::WSCHelper_Loader WSCLoader;
	CSymPtr<IWSCHelper> pWSCHelper;

    if(SYM_SUCCEEDED(WSCLoader.CreateObject(&pWSCHelper)))
	{
		CCTRACEI ("CWMIIntegration - WSC loaded");
        m_bIsPreXPSP2 = pWSCHelper->IsPreXPSP2();
    }
    else
        CCTRACEE ("CWMIIntegration() - no WSC");
}

CWMIIntegration::~CWMIIntegration(void)
{
}

bool CWMIIntegration::Subscribe ( long lEventID )
{
    if ( !m_bIsPreXPSP2 &&
        (AV::Event_ID_StatusAP == lEventID ||
         AV::Event_ID_StatusDefs == lEventID ||
         AV::Event_ID_StatusIWP == lEventID ))
         return true;

    return false;
}

// WMI cared about something so here's the status bag.
// lEventID = 0 when we do our initial update
//
void CWMIIntegration::OnStatusChange ( long lEventID /* 0 = INIT */,
                                       CEventData& edNewStatus /*contains the new data*/ )
{
    // We care about the initial event
    if ( 0 != lEventID && !Subscribe(lEventID))
        return;

    // If we've already determined CfgWiz is done, skip this.
    //
	if (!m_bCfgWizFinished )
    {
        if (!IsCfgWizFinished(m_bCfgWizFinished) || !m_bCfgWizFinished)
        {
            CCTRACEI("CWMIIntegration::OnStatusChange - Pre-config mode");
            return;
        }
        else
        {
            // Config Wiz is done. This is our Init.
            // Pre-load data
            ccSym::CCommonClientInfo::GetCCDirectory( m_sEnableUIPath );
            m_sEnableUIPath += _T("\\NMain.exe");

            m_sEnableUIParams = _T("/nosysworks /dat:");
            m_sEnableUIParams += g_NAVInfo.GetNAVDir();
            m_sEnableUIParams += _T("\\navui.nsi");
        }
    }

    ISShared::WSCHelper_Loader WSCLoader;
	CSymPtr<IWSCHelper> pWSCHelper;

	if(SYM_SUCCEEDED(WSCLoader.CreateObject(&pWSCHelper)))
	{
        long lAPStatus = AVStatus::statusNone;
        long lDefsStatus = AVStatus::statusNone;
        time_t timeDefs = 0;
        long lIWPStatus = AVStatus::statusNone;
        edNewStatus.GetData ( AVStatus::propAPStatus, lAPStatus );
        edNewStatus.GetData ( AVStatus::propDefsStatus, lDefsStatus );
        edNewStatus.GetData ( AVStatus::propDefsTime, (long) timeDefs );
        edNewStatus.GetData ( AVStatus::propIWPStatus, lIWPStatus );

        switch (lEventID)
        {
        case 0:
            // Update all
            WriteStatusToWMI_AP  ( pWSCHelper, lAPStatus );
            WriteStatusToWMI_Defs( pWSCHelper, lDefsStatus, timeDefs );
            WriteStatusToWMI_IWP ( pWSCHelper, lIWPStatus );
            break;

        case AV::Event_ID_StatusAP:
            WriteStatusToWMI_AP  ( pWSCHelper, lAPStatus );
            break;

        case AV::Event_ID_StatusDefs:
            WriteStatusToWMI_Defs( pWSCHelper, lDefsStatus, timeDefs );
            break;

        case AV::Event_ID_StatusIWP:
            WriteStatusToWMI_IWP ( pWSCHelper, lIWPStatus );
            break;

        default:
            break;
        }
    }
	else
	{
		CCTRACEE("Fail to load WSCHelper");
	}
}



void CWMIIntegration::WriteStatusToWMI_AP(IWSCHelper* pWSCHelper, long lAPStatus)
{
    CCTRACEI ( "CWMIIntegration::WriteStatusToWMI_AP() - start" );

	eEnabledState eOnAccessEnabled = (lAPStatus == AVStatus::statusEnabled)? eState_Enabled : eState_Disabled;
	eEnabledState eSignatureUptoDate = eState_Unchanged;
	HRESULT hrRet = pWSCHelper->UpdateSymantecAVStatus((LPGUID)&IID_ProductID_NAV_AV, 
		                                                eSignatureUptoDate,
		                                                eOnAccessEnabled);

    CCTRACEI ( "CStateAP::WriteStatusToWMI_AP() - end" );
}

void CWMIIntegration::WriteStatusToWMI_Defs(IWSCHelper* pWSCHelper, long lDefsStatus, time_t timeDefs)
{
    CCTRACEI ( "CWMIIntegration::WriteStatusToWMI_Defs() - start" );

	// Get the current date.
    bool bUpToDateDefs = false;
    
    if ( lDefsStatus == AVStatus::statusEnabled)
    {
	    ATL::CTime currentTime = CTime::GetCurrentTime();
        ATL::CTime atltimeDefs ( timeDefs );
	    ATL::CTimeSpan span = currentTime - atltimeDefs;

	    // 14_DAYS old ?
	    if( span.GetDays() <= 14 )
            bUpToDateDefs = true;
    }

    eEnabledState eSignatureUptoDate = bUpToDateDefs ? eState_Enabled : eState_Disabled;
    CCTRACEI("SignatureUptoDate: %s", bUpToDateDefs? "Yes" : "No");

    eEnabledState eOnAccessEnabled = eState_Unchanged;
    HRESULT hrRet = pWSCHelper->UpdateSymantecAVStatus((LPGUID)&IID_ProductID_NAV_AV, 
	                                                    eSignatureUptoDate,
	                                                    eOnAccessEnabled);

    CCTRACEI ( "CWMIIntegration::WriteStatusToWMI_Defs() - end" );
}

void CWMIIntegration::WriteStatusToWMI_IWP(IWSCHelper* pWSCHelper, long lIWPStatus)
{
    CCTRACEI ( "CWMIIntegration::WriteStatusToWMI_IWP() - start" );
    bool bOnAccessEnabled = (AVStatus::statusEnabled == lIWPStatus);
    
    // Are we yielding?
    //
    if(AVStatus::statusNotRunning == lIWPStatus )
    {                   
        CCTRACEI("IWP Is Yielding.  Calling uninstall FW.");

        pWSCHelper->UninstallFirewall((LPGUID)&IID_ProductID_NAV_FW, _T(""));
    } 
    else
    {
        CCTRACEI("Broadcast IWP is %s", bOnAccessEnabled? "On" : "Off");
    
        pWSCHelper->WriteFirewallStatus((LPGUID)&IID_ProductID_NAV_FW, 
                            _T("Symantec"),
							_T("2006"), 
							_T("Norton Internet Worm Protection"),
							m_sEnableUIPath, 
							m_sEnableUIParams,
							bOnAccessEnabled,
							_T("SymantecFirewall"));
    }
    CCTRACEI ( "CWMIIntegration::WriteStatusToWMI_IWP() - end" );
}

