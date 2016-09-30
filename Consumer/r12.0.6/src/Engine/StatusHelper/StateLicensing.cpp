#include "StdAfx.h"
#include ".\statelicensing.h"

CStateLicensing::CStateLicensing(CWMIIntegration* pWMI, CNSCIntegration* pNSC):CState(pWMI,pNSC)
{
    // Update for either Licensing or Subscription.
    // We have to query the same stuff anyway so...
    if ( makeEvent (SYM_REFRESH_NAV_LICENSE_STATUS_EVENT, true, m_eventLicensing))
        m_vecWaitEvents.push_back (m_eventLicensing);

    if ( makeEvent (SYM_REFRESH_SUBSCRIPTION_STATUS_EVENT, true, m_eventSubscription))
        m_vecWaitEvents.push_back (m_eventSubscription);

    ZeroMemory ( &m_OldSubData, sizeof (CSubscription::LICENSE_SUB_DATA));

    m_lEventID = AV::Event_ID_StatusLicensing;           // What CC event ID is this for?
}

CStateLicensing::~CStateLicensing(void)
{
}

bool CStateLicensing::Save()
{
    // Only one save activity at a time since they all share the same data object
    ccLib::CSingleLock lockStatus (&m_critStatus, INFINITE, FALSE);

    // Default to off
	//
	DWORD dwStatus = AVStatus::statusError;
    CSubscription::LICENSE_SUB_DATA data;
    ZeroMemory ( &data, sizeof (CSubscription::LICENSE_SUB_DATA));

	try
	{
        /*
#ifdef _DEBUG    
        data.LicenseState = DJSMAR00_LicenseState_TRIAL;
        data.LicenseType = DJSMAR_LicenseType_Unlicensed;
        data.LicenseZone = DJSMAR_LicenseZone_Trial;
        data.VendorID = DJSMAR00_VendorID_XtreamLok;
        data.bLicenseValid = true;
        data.szEndDate = "03/25/2006";
        data.lDaysLeft = 15;
        data.bWarning = false;
        data.bExpired = false;
        data.bCanRenewSub = false;
        data.dwEndDate = 1234;
#else*/

        // Defect 1-43HHVI - NAVLicense snapshots the license info when you query it.
        // We need a fresh copy so we have to recreate the object here.
        CSubscription Subscription;
        data = Subscription.GetData ();

//#endif
    	dwStatus = AVStatus::statusEnabled;
	}
	catch (...)
	{
		dwStatus = AVStatus::statusError;
	}

    bool bReturn = false;
    // Re-use LICENSE_SUB_DATA instead????

    if ( m_OldSubData.LicenseType != data.LicenseType ||
         m_OldSubData.LicenseState != data.LicenseState ||
         m_OldSubData.LicenseZone != data.LicenseZone ||
         m_OldSubData.bLicenseValid != data.bLicenseValid ||
         m_OldSubData.VendorID != data.VendorID ||
         m_OldSubData.lDaysLeft != data.lDaysLeft ||
         m_OldSubData.bWarning != data.bWarning ||
         m_OldSubData.bCanRenewSub != data.bCanRenewSub ||
         m_OldSubData.dwEndDate != data.dwEndDate )
        bReturn = true;

    m_OldSubData.LicenseType = data.LicenseType;
    m_OldSubData.LicenseState = data.LicenseState;
    m_OldSubData.LicenseZone = data.LicenseZone;
    m_OldSubData.bLicenseValid = data.bLicenseValid;
    m_OldSubData.VendorID = data.VendorID;
    m_OldSubData.lDaysLeft = data.lDaysLeft;
    m_OldSubData.bWarning = data.bWarning;
    m_OldSubData.bCanRenewSub = data.bCanRenewSub;
    m_OldSubData.dwEndDate = data.dwEndDate;

    // Transfer the options values to the property bag.
	//
	m_edStatus.SetData ( AVStatus::propLicStatus, dwStatus );
	m_edStatus.SetData ( AVStatus::propLicType, data.LicenseType);
	m_edStatus.SetData ( AVStatus::propLicState, data.LicenseState );
	m_edStatus.SetData ( AVStatus::propLicZone, data.LicenseZone );
	m_edStatus.SetData ( AVStatus::propLicValid, data.bLicenseValid );
    m_edStatus.SetData ( AVStatus::propLicVendorID, data.VendorID );

    if ( data.szEndDate )
        m_edStatus.SetData ( AVStatus::propSubDateS, data.szEndDate );
    m_edStatus.SetData ( AVStatus::propSubDaysLeft, data.lDaysLeft );
    m_edStatus.SetData ( AVStatus::propSubExpired, (data.lDaysLeft <= 0) ? true:false );
    m_edStatus.SetData ( AVStatus::propSubWarning, data.bWarning );
    m_edStatus.SetData ( AVStatus::propSubCanRenew, data.bCanRenewSub );
    m_edStatus.SetData ( AVStatus::propSubDate, data.dwEndDate );

    return bReturn;
}
