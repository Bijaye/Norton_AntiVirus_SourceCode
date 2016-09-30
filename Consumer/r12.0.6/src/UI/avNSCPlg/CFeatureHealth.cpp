// ----------------------------------------------------------------------------
// PROPRIETARY/CONFIDENTIAL.  Use of this product is subject to license terms.
// Copyright (C) 2005 Symantec Corporation.  All rights reserved.
// ----------------------------------------------------------------------------

#include "stdafx.h"

#include "CFeature.h"
#include "util.h"
#include "ATLComTime.h"



const long FULLSYSSCAN_DAYS_TILL_STALE = 90;
const long VIRUSDEFS_DAYS_TILL_STALE = 14;
const long VIRUSDEFS_DAYS_TILL_BAD = 30;

// overrides for GetHealth
 nsc::NSCRESULT CNAVLicenseFeature::GetHealth (const nsc::IContext* context_in, 
                                    const nsc::IHealth*& health_out) 
                                    const throw()
{
    CROSS_PROCESS_LOCK();

	CCTRCTXI0(_T("Start"));

    // check params
    if(health_out != NULL || context_in == NULL)
    {
        TRACEE(_T("Bad param"));
        return nsc::NSCERR_ILLEGALPARAM;
    }

    nsc::IHealthPtr spHealth = NULL;
	nsc::NSCRESULT result = CreateHealth (&spHealth, nsc::GOOD);
    if (NSC_FAILED(result))
    {
        TRACEE(_T("CreateHealth failed for NAVLicense"));
        return result;
    }

    health_out = spHealth;
	health_out->AddRef();

    return result;
}

 nsc::NSCRESULT CAutoProtectFeature::GetHealth (const nsc::IContext* context_in, 
                                    const nsc::IHealth*& health_out) 
                                    const throw()
{
    CROSS_PROCESS_LOCK();

	CCTRCTXI0(_T("Start"));

    // check params
    if (health_out != NULL || context_in == NULL)
    {
        TRACEE(_T("Bad param"));
        return nsc::NSCERR_ILLEGALPARAM;
    }

    nsc::NSCRESULT result = nsc::NSC_FAIL;
    long lStatus = AVStatus::statusError;

    nsc::IHealthPtr spHealth = NULL;

    // If we fail to get real status use the above defaults
    CEventData* pEventData = NULL;
    if (CFeature::getInternalStatus (context_in, pEventData) &&
        pEventData )
    {
        ccLib::CSingleLock lock ( &m_critStatus, INFINITE, FALSE );
        pEventData->GetData( AVStatus::propAPStatus, lStatus );
    }    

	switch(lStatus)
	{
		case AVStatus::statusEnabled:
            // Resolution for 1-4624M3.  Good health, no action required.
			CCTRCTXI1(_T("Creating GOOD health, Status=%d"), lStatus);
			result = CreateHealth (&spHealth, nsc::GOOD);
			CHECK_NSCRESULT(result);
			break;
		case AVStatus::statusDisabled:
		case AVStatus::statusNotRunning:
		case AVStatus::statusNotInstalled:			// NOT USED
		case AVStatus::statusError:
		default:
            // Defect #1-43I4XG resolution.  Change health from Fair to Poor.
			CCTRCTXI1(_T("Creating POOR health, Status=%d"), lStatus);
			result = CreateHealth (&spHealth, 
				                    nsc::POOR,
				                    IDS_FIX_ACTION_ENABLE_AP,
				                    IDS_FIX_ACTION_DESC_ENABLE_AP,
				                    CClientCallback::eCOOKIE_ENABLE_AP,
                                    guidAction_Enable_AP);
			CHECK_NSCRESULT(result);
			break;
	}
    if (NSC_FAILED(result))
    {
        TRACEE(_T("CreateHealth failed for AP"));
        return result;
    }

    health_out = spHealth;
	health_out->AddRef();

    return result;
}

 nsc::NSCRESULT CEmailScanningFeature::GetHealth (const nsc::IContext* context_in, 
                                    const nsc::IHealth*& health_out) 
                                    const throw()
{
    CROSS_PROCESS_LOCK();

	CCTRCTXI0(_T("Start"));

    // check params
    if (health_out != NULL || context_in == NULL)
    {
        TRACEE(_T("Bad param"));
        return nsc::NSCERR_ILLEGALPARAM;
    }

    nsc::NSCRESULT result = nsc::NSC_FAIL;
    long lStatus = AVStatus::statusError;
    nsc::IHealthPtr spHealth = NULL;

    // If we fail to get real status use the above defaults
    CEventData* pEventData = NULL;
    if (CFeature::getInternalStatus (context_in, pEventData) &&
        pEventData )
    {
        ccLib::CSingleLock lock ( &m_critStatus, INFINITE, FALSE );
        pEventData->GetData( AVStatus::propEmailStatus, lStatus );
    }    

	switch(lStatus)
	{
		case AVStatus::statusEnabled:
            // Resolution for 1-4624M3.  Good health, no action required.
			CCTRCTXI1(_T("Creating GOOD health, Status=%d"), lStatus);
			result = CreateHealth (&spHealth, nsc::GOOD);
			CHECK_NSCRESULT(result);
			break;
		case AVStatus::statusDisabled:
			CCTRCTXI1(_T("Creating FAIR health, Status=%d"), lStatus);
			result = CreateHealth (&spHealth, 
				nsc::FAIR,
				IDS_FIX_ACTION_ENABLE_EMAIL_SCANNING,
				IDS_FIX_ACTION_DESC_ENABLE_EMAIL_SCANNING,
				CClientCallback::eCOOKIE_ENABLE_EMAIL_SCANNING,
                guidAction_Enable_Email_Scanning);
			CHECK_NSCRESULT(result);
			break;
		case AVStatus::statusNotRunning:
		case AVStatus::statusNotInstalled:
		case AVStatus::statusError:
		default:
			CCTRCTXI1(_T("Creating FAIR health, no actions, Status=%d"), lStatus);
			result = CreateHealth (&spHealth, nsc::FAIR);
			CHECK_NSCRESULT(result);
			break;
	}

    if (NSC_FAILED(result))
    {
        TRACEE(_T("CreateHealth failed for email scanning"));
        return result;
    }

    health_out = spHealth;
	health_out->AddRef();

    return result;
}

 nsc::NSCRESULT CSpywareProtectionFeature::GetHealth (const nsc::IContext* context_in, 
                                    const nsc::IHealth*& health_out) 
                                    const throw()
{
    CROSS_PROCESS_LOCK();

	CCTRCTXI0(_T("Start"));

    // check params
    if (health_out != NULL || context_in == NULL)
    {
        TRACEE(_T("Bad param"));
        return nsc::NSCERR_ILLEGALPARAM;
    }

    nsc::NSCRESULT result = nsc::NSC_FAIL;
    long lSpywareOn = 0;
	long lSpywareInstalled = 0;
    nsc::IHealthPtr spHealth = NULL;

    // If we fail to get real status use the above defaults
    CEventData* pEventData = NULL;
    if (CFeature::getInternalStatus (context_in, pEventData) &&
        pEventData )
    {
        ccLib::CSingleLock lock ( &m_critStatus, INFINITE, FALSE );
		pEventData->GetData(AVStatus::propSpywareCat, lSpywareOn);
		pEventData->GetData(AVStatus::propSpywareInstalled, lSpywareInstalled);
    }

    // Spyware Feature status only determined by Spyware Cat
    // ignore ap, ap-threat and adware setting
	if(0 == lSpywareInstalled)
	{
		CCTRCTXI1(_T("Creating NSC_DOESNOTEXIST health, lSpywareInstalled=%d"), lSpywareInstalled);
		result = nsc::NSC_DOESNOTEXIST;
		CHECK_NSCRESULT(result);
	}
	else if (0 != lSpywareOn)
	{
        // Resolution for 1-4624M3.  Good health, no action required.
		CCTRCTXI2(_T("Creating GOOD health, lSpywareInstalled=%d, lSpywareOn=%d"), lSpywareInstalled, lSpywareOn);
		result = CreateHealth (&spHealth, nsc::GOOD);
		CHECK_NSCRESULT(result);
	}
	else
	{
		CCTRCTXI2(_T("Creating FAIR health, lSpywareInstalled=%d, lSpywareOn=%d"), lSpywareInstalled, lSpywareOn);
		result = CreateHealth (&spHealth, 
			nsc::FAIR,
			IDS_FIX_ACTION_ENABLE_SPYWARE_PROTECTION,
			IDS_FIX_ACTION_DESC_ENABLE_SPYWARE_PROTECTION,
			CClientCallback::eCOOKIE_ENABLE_SPYWARE_PROTECTION,
            guidAction_Enable_Spyware);
		CHECK_NSCRESULT(result);
	}

	if (NSC_FAILED(result))
    {
        TRACEE(_T("CreateHealth failed for Spyware Protection"));
        return result;
    }

    health_out = spHealth;
	health_out->AddRef();

    return result;
}

 nsc::NSCRESULT CIFPFeature::GetHealth (const nsc::IContext* context_in, 
                                    const nsc::IHealth*& health_out) 
                                    const throw()
{
    CROSS_PROCESS_LOCK();

	CCTRCTXI0(_T("Start"));

    // check params
    if (health_out != NULL || context_in == NULL)
    {
        TRACEE(_T("Bad param"));
        return nsc::NSCERR_ILLEGALPARAM;
    }

    nsc::NSCRESULT result = nsc::NSC_FAIL;
    long lStatus = AVStatus::statusError;
    nsc::IHealthPtr spHealth = NULL;

    // If we fail to get real status use the above defaults
    CEventData* pEventData = NULL;
    if (CFeature::getInternalStatus (context_in, pEventData) &&
        pEventData )
    {
        ccLib::CSingleLock lock ( &m_critStatus, INFINITE, FALSE );
        pEventData->GetData( AVStatus::propIWPStatus, lStatus );
    }    

    switch(lStatus)
	{
		case AVStatus::statusEnabled:
            // Resolution for 1-4624M3.  Good health, no action required.
			result = CreateHealth (&spHealth, nsc::GOOD);
			CHECK_NSCRESULT(result);
			break;
			
		case AVStatus::statusNotAvailable: // IWP not in layout.
		case AVStatus::statusNotRunning:   // Yielding
			result = nsc::NSC_DOESNOTEXIST;
			CHECK_NSCRESULT(result);
			break;

        // For 1-4BSV5H. Returning no fix action (for '!' icon).
        case AVStatus::statusError:
            {
                result = CreateHealth (&spHealth, nsc::POOR);
                CHECK_NSCRESULT(result);
            }
            break;

       // Resolution for 1-44IWSC.  Change health from FAIR to POOR.
		case AVStatus::statusDisabled:
		case AVStatus::statusNotInstalled:
		default:
			result = CreateHealth (&spHealth, 
				nsc::POOR,
				IDS_FIX_ACTION_ENABLE_IFP,
				IDS_FIX_ACTION_DESC_ENABLE_IFP,
				CClientCallback::eCOOKIE_ENABLE_IFP,
                guidAction_Enable_IFP);
			CHECK_NSCRESULT(result);
			break;
	}

    if (NSC_FAILED(result))
	{
        TRACEE(_T("CreateHealth failed for IFP"));
        return result;
    }
        
    health_out = spHealth;
	health_out->AddRef();

    return result;
}

 nsc::NSCRESULT CIMScanningFeature::GetHealth (const nsc::IContext* context_in, 
                                    const nsc::IHealth*& health_out) 
                                    const throw()
{
    CROSS_PROCESS_LOCK();

	CCTRCTXI0(_T("Start"));

    // check params
    if (health_out != NULL || context_in == NULL)
    {
        TRACEE(_T("Bad param"));
        return nsc::NSCERR_ILLEGALPARAM;
    }

	// CodeReview: Hmm... We need an error state to init with here.
	//  Otherwise, error states are weird.  Logic doesn't flow.
    nsc::NSCRESULT result = nsc::NSC_FAIL;
    long lMSNProtected = AVStatus::IM_Status_NotInstalled;
    long lAOLProtected = AVStatus::IM_Status_NotInstalled;
    long lYIMProtected = AVStatus::IM_Status_NotInstalled;
    long lIMInstalled;
    nsc::IHealthPtr spHealth = NULL;

    // If we fail to get real status use the above defaults
    CEventData* pEventData = NULL;
    if (CFeature::getInternalStatus (context_in, pEventData) &&
        pEventData )
    {
        ccLib::CSingleLock lock ( &m_critStatus, INFINITE, FALSE );
		pEventData->GetData(AVStatus::propIMMSN, lMSNProtected);
		pEventData->GetData(AVStatus::propIMYIM, lYIMProtected);
		pEventData->GetData(AVStatus::propIMAOL, lAOLProtected);
    } 
	
	lIMInstalled = (lMSNProtected != AVStatus::IM_Status_NotInstalled) ? 1 : 0 +
				   (lAOLProtected != AVStatus::IM_Status_NotInstalled) ? 1 : 0 +
				   (lYIMProtected != AVStatus::IM_Status_NotInstalled) ? 1 : 0;
		
	// If no IMs are installed --> GREEN
	// If all IMs are installed and protected --> GREEN
	// If 1 IM is installed, and unprotected --> FAIR (TURN ON)
	// Anything else --> FAIR (configure)
	
	// Protection ON for all IMs installed or no IMs installed.
    if (((lMSNProtected == AVStatus::IM_Status_Protected) || (lMSNProtected == AVStatus::IM_Status_NotInstalled)) && 
		((lAOLProtected == AVStatus::IM_Status_Protected) || (lAOLProtected == AVStatus::IM_Status_NotInstalled)) && 
		((lYIMProtected == AVStatus::IM_Status_Protected) || (lYIMProtected == AVStatus::IM_Status_NotInstalled)))
	{
        // Resolution for 1-4624M3.  Good health, no action required.
		result = CreateHealth (&spHealth, nsc::GOOD);
			CHECK_NSCRESULT(result);
	}
    
	// 1 IM installed, and its protection is OFF
	else if (1 == lIMInstalled)
    {
		result = CreateHealth (&spHealth, 
			nsc::FAIR,
			IDS_FIX_ACTION_ENABLE_IM_SCANNING,
			IDS_FIX_ACTION_DESC_ENABLE_IM_SCANNING,
			CClientCallback::eCOOKIE_ENABLE_IM_SCANNING,
            guidAction_Enable_IM);
			CHECK_NSCRESULT(result);
	}
    
	// Multiplied IM clients with at least 1 unprotected
    else 
	{
        // Resolution for 1-49MY43.  Changing health to fair, adding back configure action
		result = CreateHealth (&spHealth, 
			nsc::FAIR,
			IDS_ACTION_CONFIGURE_IM_SCANNING,
			IDS_ACTION_DESC_CONFIGURE_IM_SCANNING,
			CClientCallback::eCOOKIE_CONFIGURE_IM_SCANNING,
			guidAction_Configure_IM);
			CHECK_NSCRESULT(result);
	}
    
	health_out = spHealth;
	health_out->AddRef();

    return result;
}

 nsc::NSCRESULT CWormBlockingFeature::GetHealth (const nsc::IContext* context_in, 
                                    const nsc::IHealth*& health_out) 
                                    const throw()
{
    CROSS_PROCESS_LOCK();

	CCTRCTXI0(_T("Start"));

    // check params
    if (health_out != NULL || context_in == NULL)
    {
        TRACEE(_T("Bad param"));
        return nsc::NSCERR_ILLEGALPARAM;
    }

    nsc::NSCRESULT result = nsc::NSC_FAIL;
	long lOEHVal = 0;
	long lSMTPVal = 0;
    nsc::IHealthPtr spHealth = NULL;

    // If we fail to get real status use the above defaults
    CEventData* pEventData = NULL;
    if (CFeature::getInternalStatus (context_in, pEventData) &&
        pEventData )
    {
        ccLib::CSingleLock lock ( &m_critStatus, INFINITE, FALSE );
        pEventData->GetData( AVStatus::propEmailSMTP, lSMTPVal );
        pEventData->GetData( AVStatus::propEmailOEH, lOEHVal );
    }

    // outbound email scanning and OEH must be on for worm blocking
	if (lSMTPVal && lOEHVal)
	{
        // Resolution for 1-4624M3.  Good health, no action required.
		result = CreateHealth (&spHealth, nsc::GOOD);
		CHECK_NSCRESULT(result);
	}
	else
	{
		result = CreateHealth (&spHealth, 
				nsc::FAIR,
				IDS_FIX_ACTION_ENABLE_WORM_BLOCKING,
				IDS_FIX_ACTION_DESC_ENABLE_WORM_BLOCKING,
				CClientCallback::eCOOKIE_ENABLE_WORM_BLOCKING,
                guidAction_Enable_Worm_Blocking);
		CHECK_NSCRESULT(result);
	}

	if (NSC_FAILED(result))
    {
        TRACEE(_T("CreateHealth failed"));
        return result;
    }

    health_out = spHealth;
	health_out->AddRef();

    return result;
}

 nsc::NSCRESULT CFSSFeature::GetHealth (const nsc::IContext* context_in, 
                                    const nsc::IHealth*& health_out) 
                                    const throw()
{
    CROSS_PROCESS_LOCK();

	CCTRCTXI0(_T("Start"));

    // check params
    if (health_out != NULL || context_in == NULL)
    {
        TRACEE(_T("Bad param"));
        return nsc::NSCERR_ILLEGALPARAM;
    }

    nsc::NSCRESULT result = nsc::NSC_FAIL;
    long lStatus = AVStatus::statusError;
    nsc::IHealthPtr spHealth = NULL;

    long lVal = 0;
    long lAgeVal = 0; 
    DATE dateFSS = 0; 
    long lSize = sizeof (DATE); 

    // If we fail to get real status use the above defaults
    CEventData* pEventData = NULL;
    if (CFeature::getInternalStatus (context_in, pEventData) &&
        pEventData )
    {
        ccLib::CSingleLock lock ( &m_critStatus, INFINITE, FALSE );
        pEventData->GetData( AVStatus::propFSSStatus, lStatus );
		pEventData->GetData( AVStatus::propFSSDate, (BYTE*)&dateFSS, lSize);
    }    

	// Figure out how many days since FSS 
    TRACEI(_T("FSS date returned value: %d"), dateFSS);
    if (dateFSS != 0)
	{
		ATL::COleDateTime timeFSS(dateFSS);
		TRACEI(_T("Last Scan: %s"), (LPCTSTR)timeFSS.Format("%#c")); 

        ATL::COleDateTimeSpan timespan = ATL::COleDateTime::GetCurrentTime() - timeFSS; 
		lAgeVal = (long)timespan.GetDays(); 
	} 
    else 
    { 
		TRACEI(_T("Failed get AVStatus::szFSSDate in getFSSHealth"));
		lStatus = AVStatus::statusError;
	} 

    switch(lStatus)
	{
		case AVStatus::statusEnabled:
			if (lAgeVal < FULLSYSSCAN_DAYS_TILL_STALE)
            {
                // Resolution for 1-4624M3.  Good health, no action required.
				CCTRCTXI1(_T("Creating GOOD health, Status=%d"), lStatus);
				result = CreateHealth (&spHealth, nsc::GOOD);
				CHECK_NSCRESULT(result);
				
				break;
			}
			// else FAIR health

		case AVStatus::statusDisabled:
		case AVStatus::statusError:
		default:
			CCTRCTXI1(_T("Creating FAIR health, Status=%d"), lStatus);
			result = CreateHealth (&spHealth, 
				nsc::FAIR,
				IDS_FIX_ACTION_LAUNCH_FSS,
				IDS_FIX_ACTION_DESC_LAUNCH_FSS,
				CClientCallback::eCOOKIE_LAUNCH_FSS_AND_WAIT,
                guidAction_Launch_FSS);
			CHECK_NSCRESULT(result);
			break;
	}

    if (NSC_FAILED(result))
    {
        TRACEE(_T("CreateHealth failed"));
        return result;
    }

    health_out = spHealth;
	health_out->AddRef();

    return result;
}

nsc::NSCRESULT CVirusDefsFeature::GetHealth (const nsc::IContext* context_in, 
                                    const nsc::IHealth*& health_out) 
                                    const throw()
{
    CROSS_PROCESS_LOCK();

	CCTRCTXI0(_T("Start"));

    // check params
    if (health_out != NULL || context_in == NULL)
    {
        TRACEE(_T("Bad param"));
        return nsc::NSCERR_ILLEGALPARAM;
    }

    nsc::NSCRESULT result = nsc::NSC_FAIL;
    long lStatus = AVStatus::statusError;
    nsc::IHealthPtr spHealth = NULL;
	long lVal = 0;
	long lAgeVal = 0;
    time_t timeDefTime = 0;			// Time in secs / secs / hours / days 

    // If we fail to get real status use the above defaults
    CEventData* pEventData = NULL;
    if (CFeature::getInternalStatus (context_in, pEventData) &&
        pEventData )
    {
        ccLib::CSingleLock lock ( &m_critStatus, INFINITE, FALSE );
        pEventData->GetData( AVStatus::propDefsStatus, lStatus );
        pEventData->GetData( AVStatus::propDefsTime, (time_t)timeDefTime);
    } 
    TRACEI(_T("VD time returned: %d"), timeDefTime); 

    // Get def date 
    if (timeDefTime != 0) 
	{
		ATL::CTime timeDefs (timeDefTime); 
		ATL::CTimeSpan timespan = ATL::CTime::GetCurrentTime() - timeDefs; 
		lAgeVal = (long)timespan.GetDays (); 
		TRACEI(_T("VD age %d days, (%s)"), lAgeVal, timeDefs.Format("%#c")); 
	}
	else
	{ 
		TRACEI(_T("Could not find Defs time!")); 
		lStatus = AVStatus::statusError;
	} 

	switch(lStatus)
	{
		case AVStatus::statusEnabled:
			if (lAgeVal < VIRUSDEFS_DAYS_TILL_STALE)
			{
                // Resolution for 1-4624M3.  Good health, no action required.
				CCTRCTXI2(_T("Creating GOOD health, Status=%d, DefAge=%d"), lStatus, lAgeVal);
				result = CreateHealth (&spHealth, nsc::GOOD);
				CHECK_NSCRESULT(result);

			}
			else if (lAgeVal < VIRUSDEFS_DAYS_TILL_BAD)
			{
				CCTRCTXI2(_T("Creating FAIR health, Status=%d, DefAge=%d"), lStatus, lAgeVal);
				result = CreateHealth (&spHealth, 
					nsc::FAIR,
					IDS_FIX_ACTION_LAUNCH_VIRUSDEFS,
					IDS_FIX_ACTION_DESC_LAUNCH_VIRUSDEFS,
					CClientCallback::eCOOKIE_LAUNCH_RUN_LU_AND_WAIT,
                    guidAction_Launch_LiveUpdate);
				CHECK_NSCRESULT(result);
			}
            else
            {
				CCTRCTXI2(_T("Creating POOR health, Status=%d, DefAge=%d"), lStatus, lAgeVal);
				result = CreateHealth (&spHealth, 
					nsc::POOR,
					IDS_FIX_ACTION_LAUNCH_VIRUSDEFS,
					IDS_FIX_ACTION_DESC_LAUNCH_VIRUSDEFS,
					CClientCallback::eCOOKIE_LAUNCH_RUN_LU_AND_WAIT,
                    guidAction_Launch_LiveUpdate);
					CHECK_NSCRESULT(result);
            }
			break;
		case AVStatus::statusDisabled:
		case AVStatus::statusError:
		default:
			CCTRCTXI1(_T("Creating POOR health, Status=%d"), lStatus);
			result = CreateHealth (&spHealth, 
				nsc::POOR,
				IDS_FIX_ACTION_LAUNCH_VIRUSDEFS,
				IDS_FIX_ACTION_DESC_LAUNCH_VIRUSDEFS,
				CClientCallback::eCOOKIE_LAUNCH_RUN_LU_AND_WAIT,
                guidAction_Launch_LiveUpdate);
			CHECK_NSCRESULT(result);
			break;
	}

    if (NSC_FAILED(result))
    {
        TRACEE(_T("CreateHealth failed"));
        return result;
    }

    health_out = spHealth;
	health_out->AddRef();

    return result;
}
