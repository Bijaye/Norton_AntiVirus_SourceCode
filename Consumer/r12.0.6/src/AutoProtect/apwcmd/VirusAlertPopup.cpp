// VirusAlertPopup.cpp: implementation of the CVirusAlertPopup class.
//
//////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "VirusAlertPopup.h"
#include "eventdata.h"
#include "AllNAVEvents.h"

#include "ResourceHelper.h"
#include "SymInterfaceHelpers.h"
#include "NAVHelpLauncher.h"               // For Help

#include "OSInfo.h"                        // For subscriptions
#include "Subscription.h" // For the stupid link format...

//////////////////////////////////////////////////////////////////////
// CVirusAlertPopup::CVirusAlertPopup()

CVirusAlertPopup::CVirusAlertPopup(void):CAlertUI()
{
}

//////////////////////////////////////////////////////////////////////
// CVirusAlertPopup::~CVirusAlertPopup()

CVirusAlertPopup::~CVirusAlertPopup()
{
}

//////////////////////////////////////////////////////////////////////
// CVirusAlertPopup::DoModal()

int CVirusAlertPopup::DoModal (long lIndex /*0*/)
{
    try
    {
        DATA alertdata = vecData.at (lIndex); // throws out_of_range exception if invalid.

		// Build strings.
        vecStrings vecsActions;
        std::wstring strResultDesc;
		composeStrings(alertdata, vecsActions, strResultDesc);

        // Top window title
        //
        m_pAlert->SetBriefDesc ( strResultDesc.c_str() );
        m_pAlert->SetAlertTitle ( _Module.GetResourceInstance(), IDS_VIRUS_ALERT_TITLE );
        m_pAlert->SetTitleBitmap ( g_hInstance, IDB_VIRUS_ALERT_BITMAP );

        // Expand the detail area by default
        //
        m_pAlert->SetShowDetail ( true );

        // ********************************************************************************
        //
        // Callbacks - these must go before table rows for the callbacks in the rows
        //
        dwHelpID = IDH_NAVW_AP_VIRUS_ALERT_HIGH_MOREINFO;

        if ( !sContainerName.empty ())
            dwHelpID = IDH_NAVW_AP_VIRUS_ALERT_HIGH_COMPRESSED_MOREINFO;
        //
        // ********************************************************************************


        // ********************************************************************************
        // Table rows
        //
        std::wstring strTemp;

        // clear the table in case we are being re-used.
        m_pAlert->ClearData(cc::IAlert::CLEAR_TABLE);

        CResourceHelper::LoadString ( IDS_OBJECT_NAME_STATIC, strTemp, _Module.GetResourceInstance() );
        m_pAlert->AddTableRowPath ( strTemp.c_str(), alertdata.sObjectName.c_str() );

        if ( !sContainerName.empty ())
        {
            CResourceHelper::LoadString ( IDS_CONTAINER_NAME_STATIC, strTemp, _Module.GetResourceInstance() );
            m_pAlert->AddTableRowPath ( strTemp.c_str(), sContainerName.c_str() );

            // Add the "Don't bug me about this container"
            //
            CResourceHelper::LoadString ( IDS_CONTAINER_IGNORE_CONTAINER, strTemp, _Module.GetResourceInstance() );
            m_pAlert->SetCheckBoxText ( strTemp.c_str(), false );
        }

        // Virus link callback
        //
        CResourceHelper::LoadString ( IDS_VIRUS_NAME_STATIC, strTemp, _Module.GetResourceInstance() );
        m_pAlert->AddTableRow ( strTemp.c_str(), alertdata.sVirusName.c_str(), this, alertdata.ulVirusID );

        // One row for each action
        CResourceHelper::LoadString ( IDS_ACTION_TAKEN_STATIC, strTemp, _Module.GetResourceInstance() );
        for ( size_t Index = 0; Index < alertdata.vecActions.size(); Index++)
        {
            m_pAlert->AddTableRow ( strTemp.c_str(), vecsActions.at(Index).c_str() );
        }
        //
        // ********************************************************************************

        // Put subscription info in the paragraph area 
        //
        // Expired?
        //
        bool bShowExpired = false;
        
        CSubscription Subscription;

        if ( Subscription.GetData().LicenseState != DJSMAR00_LicenseState_TRIAL &&
             Subscription.GetData().bCanRenewSub &&
             Subscription.GetData().bExpired )
        {
            bShowExpired = true;

			std::wstring sSubscriptionDesc, sRenewSub, sIgnore;

            if (COSInfo::IsAdminProcess())
			{
				CResourceHelper::LoadString ( IDS_SUBSCRIPTION_EXPIRED, sSubscriptionDesc, _Module.GetResourceInstance() );
				AddActionAccelerator ( IDS_ACTION_IGNORE_SUB );
				AddActionAccelerator ( IDS_ACTION_RENEW_SUB );
				m_pAlert->SetRecAction ( 0 );
			}
			else
			{
				CResourceHelper::LoadString ( IDS_SUBSCRIPTION_EXPIRED_NO_ADMIN_RIGHTS, sSubscriptionDesc, _Module.GetResourceInstance() );
			}

			m_pAlert->SetDetailedDesc ( sSubscriptionDesc.c_str ());
        }

        m_pAlert->SetThreatLevel ( cc::IAlert::THREAT_HIGH );

        // Show!
        //
        int iActionTaken = DisplayAlert(::GetDesktopWindow());

        if ( bShowExpired && 
             iActionTaken == 1) 
        {
            // Renew Subscription
            //
            CSubscription Subscription;
            Subscription.LaunchSubscriptionWizard ();
        }

        if ( !sContainerName.empty () && m_pAlert->GetCheckBoxState ())
            return RESULT_IGNORE_CONTAINER;
        else
            return RESULT_OK;
    }
    catch (...)
    {
        CCTRACEE ("CVirusAlertPopup::DoModal");
    }
    
    return RESULT_FAIL;
}


//////////////////////////////////////////////////////////////////////
// CVirusAlertPopup::composeStrings()
void CVirusAlertPopup::composeStrings ( DATA& alertdata /*in*/,
                                        vecStrings& vecsActions /*out*/,
                                        std::wstring& strResultDesc /*out*/)
{
	UINT uResIdAction, uResIdFormat = 0;

    for ( size_t Index = 0; Index < alertdata.vecActions.size (); Index++)
    {
        // Make sure the entry exists
	    uResIdAction = IDS_VIR_ACT_UNDEFINED;
	    uResIdFormat = IDS_VIR_MSG_IS_INFECTED;

	    if( alertdata.lType == AV::Event_Threat_ObjectType_File )
	    {
		    switch ( alertdata.vecActions.at(Index) )
		    {
                case AV::Event_Action_Repaired:
			        uResIdAction = IDS_VIR_ACT_REPAIRED;
			        uResIdFormat = IDS_VIR_MSG_WAS_INFECTED;
			        break;

                case AV::Event_Action_RepairFailed:
			        uResIdAction = IDS_VIR_ACT_NOT_REPAIRED;
			        uResIdFormat = IDS_VIR_MSG_IS_INFECTED;
			        break;
            		
                case AV::Event_Action_Quarantined:
			        uResIdAction = IDS_VIR_ACT_QUARANTINED;
			        uResIdFormat = IDS_VIR_MSG_IS_INFECTED;
			        break;

		        case AV::Event_Action_QuarantinedFailed:
			        uResIdAction = IDS_VIR_ACT_NOT_QUARANTINED;
			        uResIdFormat = IDS_VIR_MSG_IS_INFECTED;
			        break;
            		
                case AV::Event_Action_Deleted:
			        uResIdAction = IDS_VIR_ACT_DELETED;
			        uResIdFormat = IDS_VIR_MSG_WAS_INFECTED;
			        break;

                case AV::Event_Action_DeleteFailed:
			        uResIdAction = IDS_VIR_ACT_NOT_DELETED;
			        uResIdFormat = IDS_VIR_MSG_IS_INFECTED;
			        break;
            		
                case AV::Event_Action_Access_Denied:
		        case AV::Event_Action_NoActionTaken: // this shouldn't happen for a virus
			        uResIdAction = IDS_VIR_ACT_ACCESS_DENIED;
			        uResIdFormat = IDS_VIR_MSG_IS_INFECTED;
			        break;

		        case AV::Event_Action_Auto_Deleted:
			        uResIdAction = IDS_VER_ACT_DESTROYED;
			        uResIdFormat = IDS_VIR_MSG_WAS_INFECTED;
			        break;

                case AV::Event_Action_Allowed: // e.g. unrepairable container items
			        uResIdAction = IDS_VIR_ACT_NONE;
			        uResIdFormat = IDS_VIR_MSG_IS_INFECTED;
                    break;

                default:
                    assert(false);
                    uResIdAction = IDS_VIR_ACT_NONE;
                    CCTRACEE("CVirusAlertPopup::composeStrings unhandled action %d", alertdata.vecActions.at(Index));
                    break;
		    }
	    }
	    else if( alertdata.lType == AV::Event_Threat_ObjectType_BootRecord )
	    {
		    switch ( alertdata.vecActions.at(Index) )
		    {
            case AV::Event_Action_Repaired:
			    uResIdAction = IDS_VIR_ACT_BOOTREC_REPAIRED;
			    uResIdFormat = IDS_VIR_MSG_WAS_INFECTED;
			    break;

            case AV::Event_Action_RepairFailed:
			    uResIdAction = IDS_VIR_ACT_BOOTREC_NOT_REPAIRED;
			    uResIdFormat = IDS_VIR_MSG_IS_INFECTED;
			    break;

		    case AV::Event_Action_NoActionTaken: // this shouldn't happen for a virus
			    uResIdAction = IDS_VIR_ACT_BOOTREC_ACCESS_DENIED;
			    uResIdFormat = IDS_VIR_MSG_IS_INFECTED;
			    break;

            // Any of these actions will translate to no action taken for the BR
            default:
                assert(false);
                CCTRACEE("CVirusAlertPopup::composeStrings unhandled action %d", alertdata.vecActions.at(Index));
                uResIdAction = IDS_VIR_ACT_NONE;
                break;
		    }
	    }
	    else if ( alertdata.lType == AV::Event_Threat_ObjectType_MasterBootRecord )
	    {
		    switch ( alertdata.vecActions.at(Index) )
		    {
                case AV::Event_Action_Repaired:
			        uResIdAction = IDS_VIR_ACT_MBR_REPAIRED;
			        uResIdFormat = IDS_VIR_MSG_WAS_INFECTED;
			        break;

                case AV::Event_Action_RepairFailed:
			        uResIdAction = IDS_VIR_ACT_MBR_NOT_REPAIRED;
			        uResIdFormat = IDS_VIR_MSG_IS_INFECTED;
			        break;

		        case AV::Event_Action_NoActionTaken: // this shouldn't happen for a virus
			        uResIdAction = IDS_VIR_ACT_MBR_ACCESS_DENIED;
			        uResIdFormat = IDS_VIR_MSG_IS_INFECTED;
			        break;

                // Any of these actions will translate to no action taken for the BR
                default:
                    assert(false);
                    CCTRACEE("CVirusAlertPopup::composeStrings unhandled action %d", alertdata.vecActions.at(Index));
                    uResIdAction = IDS_VIR_ACT_NONE;
                    break;
		    }
        }

		// We don't need to format uResIdAction string because it does not contain product name.
		vecsActions.push_back (CResourceHelper::LoadStringW( uResIdAction, _Module.GetResourceInstance() ));

		// Format uResIdFormat because it contains product name
		USES_CONVERSION;
		ATL::CStringW wcsBuffFormat;
		ATL::CStringW wcsBuff;
		wcsBuffFormat.LoadString(_Module.GetResourceInstance(), uResIdFormat);
		wcsBuff.Format(wcsBuffFormat, T2OLE(csProductName));

		// This will contain the final disposition of the file, infected or not infected
		strResultDesc = wcsBuff;
    }
}
