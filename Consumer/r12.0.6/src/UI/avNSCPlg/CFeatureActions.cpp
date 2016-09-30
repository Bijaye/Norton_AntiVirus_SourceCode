// ----------------------------------------------------------------------------
// PROPRIETARY/CONFIDENTIAL.  Use of this product is subject to license terms.
// Copyright (C) 2005 Symantec Corporation.  All rights reserved.
// ----------------------------------------------------------------------------

#include "stdafx.h"

#include "CFeature.h"
#include "util.h"
#include "CNAVActionsCollection.h"


// overrides for GetActions
nsc::NSCRESULT CAutoProtectFeature::GetActions(const nsc::IContext* context_in, 
                                const nsc::IActionsCollection*& actions_out)
                                const throw()
{
	CCTRCTXI0(_T("Start"));

    // check params
    if (actions_out != NULL || context_in == NULL)
    {
        TRACEE(_T("Bad param"));
        return nsc::NSCERR_ILLEGALPARAM;
    }

    nsc::NSCRESULT result = nsc::NSC_FAIL;

    // Is user an admin?
    //bool bAdmin = IsAdmin(context_in);

	CNAVActionsCollectionPtr spActions = new CNAVActionsCollection ();
    if (spActions == NULL)
    {
        TRACEE(_T("Error in CNAVActionsCollection allocation in AP."));
        return nsc::NSCERR_MEMORY;
    }

    // Check the status to ensure we do not show inappropriate actions.
    long lStatus = AVStatus::statusError;
    long lLicenseValid = 1;

    CEventData* pEventData = NULL;
    if (CFeature::getInternalStatus (context_in, pEventData) &&
        pEventData )
    {
        ccLib::CSingleLock lock ( &m_critStatus, INFINITE, FALSE );
        pEventData->GetData( AVStatus::propAPStatus, lStatus );
        pEventData->GetData( AVStatus::propLicValid, lLicenseValid );
    }

    // Resolution for 1-462YEI.  No AP actions with invalid license.
    if (lLicenseValid)
    {
        // Enable AP
        if(lStatus != AVStatus::statusEnabled)
        {
            result = spActions-> AddClientAction (
                IDS_ACTION_ENABLE_AP,
                IDS_ACTION_DESC_ENABLE_AP,
                CClientCallback::eCOOKIE_ENABLE_AP,
                guidAction_Enable_AP);
	        CHECK_NSCRESULT(result);
            if (NSC_FAILED(result))
            {
                return result;
            }
        }

	    // Disable AP
        if(lStatus != AVStatus::statusDisabled)
        {
            result = spActions-> AddClientAction (
                IDS_ACTION_DISABLE_AP,
                IDS_ACTION_DESC_DISABLE_AP,
                CClientCallback::eCOOKIE_DISABLE_AP,
                guidAction_Disable_AP);
	        CHECK_NSCRESULT(result);
            if (NSC_FAILED(result))
            {
                return result;
            }
        }
    
        // Configure AP
        //if (bAdmin)
        //{
            result = spActions->AddClientAction(
                IDS_ACTION_CONFIGURE_AP,
                IDS_ACTION_DESC_CONFIGURE_AP,
                CClientCallback::eCOOKIE_CONFIGURE_AUTO_PROTECT,
                guidAction_Configure_AP);
	        CHECK_NSCRESULT(result);
            if (NSC_FAILED(result))
            {
                return result;
            }
        //}
    }

    actions_out = spActions;
    actions_out->AddRef();

    return result;
};

nsc::NSCRESULT CEmailScanningFeature::GetActions(const nsc::IContext* context_in, 
                                const nsc::IActionsCollection*& actions_out)
                                const throw()
{
	CCTRCTXI0(_T("Start"));

    // check params
    if (actions_out != NULL || context_in == NULL)
    {
        TRACEE(_T("Bad param"));
        return nsc::NSCERR_ILLEGALPARAM;
    }

    nsc::NSCRESULT result = nsc::NSC_FAIL;

    CNAVActionsCollectionPtr spActions = new CNAVActionsCollection ();
    if (spActions == NULL)
    {
        TRACEE(_T("Error in CNAVActionsCollection allocation in email."));
        return nsc::NSCERR_MEMORY;
    }

    // Check the status to ensure we do not show inappropriate actions.
    long lStatus = AVStatus::statusError;
	long lSMTPVal = 0;
	long lPOPVal = 0;
    long lLicenseValid = 1;

    CEventData* pEventData = NULL;
    if (CFeature::getInternalStatus (context_in, pEventData) &&
        pEventData )
    {
        ccLib::CSingleLock lock ( &m_critStatus, INFINITE, FALSE );
        pEventData->GetData( AVStatus::propLicValid, lLicenseValid );
        pEventData->GetData( AVStatus::propEmailStatus, lStatus );
        pEventData->GetData( AVStatus::propEmailSMTP, lSMTPVal );
        pEventData->GetData( AVStatus::propEmailPOP, lPOPVal );
    }    

    // Resolution for 1-462YEI.  No email actions with invalid license.
    if (lLicenseValid)
	{
		// Enable Email Scanning
		if(AVStatus::statusDisabled == lStatus)
		{
			result = spActions-> AddClientAction (
				IDS_ACTION_ENABLE_EMAIL_SCANNING,
				IDS_ACTION_DESC_ENABLE_EMAIL_SCANNING,
				CClientCallback::eCOOKIE_ENABLE_EMAIL_SCANNING,
				guidAction_Enable_Email_Scanning);
			CHECK_NSCRESULT(result);
			if (NSC_FAILED(result))
			{
				return result;
			}
		}

		// Disable Email Scanning
		if(AVStatus::statusEnabled == lStatus)
		{
			// can be in one of 3 states..
			// inbound ON, outbound OFF
			if (lPOPVal == 1 && lSMTPVal == 0)
			{
				// Enable outbound/smtp
				result = spActions-> AddClientAction (
					IDS_ACTION_ENABLE_EMAIL_OUTBOUND,
					IDS_ACTION_DESC_ENABLE_EMAIL_SCANNING,
					CClientCallback::eCOOKIE_ENABLE_EMAIL_SCANNING,
					guidAction_Disable_Email_Scanning);
				CHECK_NSCRESULT(result);
				if (NSC_FAILED(result))
				{
					return result;
				}

				// disable inbound/pop
				result = spActions-> AddClientAction (
					IDS_ACTION_DISABLE_EMAIL_INBOUND,
					IDS_ACTION_DESC_DISABLE_EMAIL_SCANNING,
					CClientCallback::eCOOKIE_DISABLE_EMAIL_SCANNING,
					guidAction_Disable_Email_Scanning);
				CHECK_NSCRESULT(result);
				if (NSC_FAILED(result))
				{
					return result;
				}
			}
			// inbound OFF, outbound ON
			else if (lPOPVal == 0 && lSMTPVal == 1)
			{
				// Enable inbound/pop
				result = spActions-> AddClientAction (
					IDS_ACTION_ENABLE_EMAIL_INBOUND,
					IDS_ACTION_DESC_ENABLE_EMAIL_SCANNING,
					CClientCallback::eCOOKIE_ENABLE_EMAIL_SCANNING,
					guidAction_Disable_Email_Scanning);
				CHECK_NSCRESULT(result);
				if (NSC_FAILED(result))
				{
					return result;
				}

				// disable outbound/smtp
				result = spActions-> AddClientAction (
					IDS_ACTION_DISABLE_EMAIL_OUTBOUND,
					IDS_ACTION_DESC_DISABLE_EMAIL_SCANNING,
					CClientCallback::eCOOKIE_DISABLE_EMAIL_SCANNING,
					guidAction_Disable_Email_Scanning);
				CHECK_NSCRESULT(result);
				if (NSC_FAILED(result))
				{
					return result;
				}
			}
			// both ON
			else 
			{
				result = spActions-> AddClientAction (
					IDS_ACTION_DISABLE_EMAIL_SCANNING,
					IDS_ACTION_DESC_DISABLE_EMAIL_SCANNING,
					CClientCallback::eCOOKIE_DISABLE_EMAIL_SCANNING,
					guidAction_Disable_Email_Scanning);
				CHECK_NSCRESULT(result);
				if (NSC_FAILED(result))
				{
					return result;
				}
			}
		}

		// Configure Email Scanning
		if((AVStatus::statusDisabled == lStatus) || (AVStatus::statusEnabled == lStatus))
		{
			result = spActions->AddClientAction(
				IDS_ACTION_CONFIGURE_EMAIL_SCANNING,
				IDS_ACTION_DESC_CONFIGURE_EMAIL_SCANNING,
				CClientCallback::eCOOKIE_CONFIGURE_EMAIL_SCANNING,
				guidAction_Configure_Email_Scanning);
			CHECK_NSCRESULT(result);
			if (NSC_FAILED(result))
			{
				return result;
			}
		}
	}

    actions_out = spActions;
    actions_out->AddRef();

    return result;
};

nsc::NSCRESULT CSpywareProtectionFeature::GetActions(const nsc::IContext* context_in, 
                                const nsc::IActionsCollection*& actions_out)
                                const throw()
{
	CCTRCTXI0(_T("Start"));

    // check params
    if (actions_out != NULL || context_in == NULL)
    {
        TRACEE(_T("Bad param"));
        return nsc::NSCERR_ILLEGALPARAM;
    }

    nsc::NSCRESULT result = nsc::NSC_FAIL;

    // Is user an admin?
    //bool bAdmin = IsAdmin(context_in);

    CNAVActionsCollectionPtr spActions = new CNAVActionsCollection ();
    if (spActions == NULL)
    {
        TRACEE(_T("Error in CNAVActionsCollection allocation in spyware."));
        return nsc::NSCERR_MEMORY;
    }

    // Check the status to ensure we do not show inappropriate actions.
    long lSpywareOn = 0;
    long lSpywareInstalled = 0;
    long lLicenseValid = 0;
    DWORD dwValue = 0;

    CEventData* pEventData = NULL;
    if (CFeature::getInternalStatus (context_in, pEventData) &&
        pEventData )
    {
        ccLib::CSingleLock lock ( &m_critStatus, INFINITE, FALSE );
		pEventData->GetData( AVStatus::propSpywareCat, lSpywareOn );
    
        // Resolution for 1-468SYR and 1-468SSD.  No spyware actions if spyware unavailable.
        pEventData->GetData( AVStatus::propSpywareInstalled, lSpywareInstalled );
        pEventData->GetData( AVStatus::propLicValid, lLicenseValid );
    }  

    // Resolution for 1-462YEI.  No spyware actions with invalid license.
    // Resolution for 1-468SYR and 1-468SSD.  No spyware actions if spyware unavailable.
    if (lLicenseValid && lSpywareInstalled)
    {
        // Spyware Feature status only determined by Spyware Cat
        // ignore ap, ap-threat and adware setting
        if(!lSpywareOn)
        {
            result = spActions-> AddClientAction (
                IDS_ACTION_ENABLE_SPYWARE_PROTECTION,
                IDS_ACTION_DESC_ENABLE_SPYWARE_PROTECTION,
                CClientCallback::eCOOKIE_ENABLE_SPYWARE_PROTECTION,
                guidAction_Enable_Spyware);
	        CHECK_NSCRESULT(result);
            if (NSC_FAILED(result))
            {
                return result;
            }
        }
        else 
        {
            result = spActions-> AddClientAction (
                IDS_ACTION_DISABLE_SPYWARE_PROTECTION,
                IDS_ACTION_DESC_DISABLE_SPYWARE_PROTECTION,
                CClientCallback::eCOOKIE_DISABLE_SPYWARE_PROTECTION,
                guidAction_Disable_Spyware);
	        CHECK_NSCRESULT(result);
            if (NSC_FAILED(result))
            {
                return result;
            }
        }

        //if (bAdmin)
        //{
            // Configure Spyware Protection
            result = spActions->AddClientAction(
                IDS_ACTION_CONFIGURE_SPYWARE_PROTECTION,
                IDS_ACTION_DESC_CONFIGURE_SPYWARE_PROTECTION,
                CClientCallback::eCOOKIE_CONFIGURE_SPYWARE_PROTECTION,
                guidAction_Configure_Spyware);
	        CHECK_NSCRESULT(result);
            if (NSC_FAILED(result))
            {
                return result;
            }
        //}
    }

    actions_out = spActions;
    actions_out->AddRef();

    return result;
};

nsc::NSCRESULT CIFPFeature::GetActions(const nsc::IContext* context_in, 
                                const nsc::IActionsCollection*& actions_out)
                                const throw()
{
	CCTRCTXI0(_T("Start"));

    // check params
    if (actions_out != NULL || context_in == NULL)
    {
        TRACEE(_T("Bad param"));
        return nsc::NSCERR_ILLEGALPARAM;
    }

    nsc::NSCRESULT result = nsc::NSC_FAIL;

    // Is user an admin?
    //bool bAdmin = IsAdmin(context_in);

	CNAVActionsCollectionPtr spActions = new CNAVActionsCollection ();
    if (spActions == NULL)
    {
        TRACEE(_T("Error in CNAVActionsCollection allocation in IFP."));
        return nsc::NSCERR_MEMORY;
    }

    // Check the status to ensure we do not show inappropriate actions.
    long lStatus = AVStatus::statusError;
    long lLicenseValid = 1;

    CEventData* pEventData = NULL;
    if (CFeature::getInternalStatus (context_in, pEventData) &&
        pEventData )
    {
        ccLib::CSingleLock lock ( &m_critStatus, INFINITE, FALSE );
        pEventData->GetData( AVStatus::propIWPStatus, lStatus );
        pEventData->GetData( AVStatus::propLicValid, lLicenseValid );
    }    

    // Resolution for 1-462YEI.  No IWP actions with invalid license.
    // Resolution for 1-42WXIB.  No IWP actions if IWP not in layout.
    if (lLicenseValid && (lStatus != AVStatus::statusNotAvailable))
    {
        // Enable Inbound Firewall Protection
        if(lStatus != AVStatus::statusEnabled)
        {
            result = spActions-> AddClientAction (
                IDS_ACTION_ENABLE_IFP,
                IDS_ACTION_DESC_ENABLE_IFP,
                CClientCallback::eCOOKIE_ENABLE_IFP,
                guidAction_Enable_IFP);
	        CHECK_NSCRESULT(result);
            if (NSC_FAILED(result))
            {
                return result;
            }
        }

	    // Disable Inbound Firewall Protection
        if(lStatus != AVStatus::statusDisabled)
        {
            result = spActions-> AddClientAction (
                IDS_ACTION_DISABLE_IFP,
                IDS_ACTION_DESC_DISABLE_IFP,
                CClientCallback::eCOOKIE_DISABLE_IFP,
                guidAction_Disable_IFP);
	        CHECK_NSCRESULT(result);
            if (NSC_FAILED(result))
            {
                return result;
            }
        }

        //if (bAdmin)
        //{
            // Configure Inbound Firewall Protection
            result = spActions->AddClientAction(
                IDS_ACTION_CONFIGURE_IFP,
                IDS_ACTION_DESC_CONFIGURE_IFP,
                CClientCallback::eCOOKIE_CONFIGURE_IFP,
                guidAction_Configure_IFP);
	        CHECK_NSCRESULT(result);
            if (NSC_FAILED(result))
            {
                return result;
            }
        //}
    }

    actions_out = spActions;
    actions_out->AddRef();

    return result;
};

nsc::NSCRESULT CIMScanningFeature::GetActions(const nsc::IContext* context_in, 
                                const nsc::IActionsCollection*& actions_out)
                                const throw()
{
	CCTRCTXI0(_T("Start"));

    // check params
    if (actions_out != NULL || context_in == NULL)
    {
        TRACEE(_T("Bad param"));
        return nsc::NSCERR_ILLEGALPARAM;
    }

    nsc::NSCRESULT result = nsc::NSC_FAIL;

    // Is user an admin?
    //bool bAdmin = IsAdmin(context_in);

    CNAVActionsCollectionPtr spActions = new CNAVActionsCollection ();
    if (spActions == NULL)
    {
        TRACEE(_T("Error in CNAVActionsCollection allocation in IM."));
        return nsc::NSCERR_MEMORY;
    }

    // Check the status to ensure we do not show inappropriate actions.
    long lLicenseValid = 1;
    long lMSNProtected = 0;
    long lAOLProtected = 0;
    long lYIMProtected = 0;
    long lIMValue = 0;

    CEventData* pEventData = NULL;
    if (CFeature::getInternalStatus (context_in, pEventData) &&
        pEventData )
    {
        ccLib::CSingleLock lock ( &m_critStatus, INFINITE, FALSE );
        pEventData->GetData( AVStatus::propLicValid, lLicenseValid );
        pEventData->GetData( AVStatus::propIMMSN, lMSNProtected);
        pEventData->GetData( AVStatus::propIMAOL, lAOLProtected);
        pEventData->GetData( AVStatus::propIMYIM, lYIMProtected);
    } 

    lIMValue = lMSNProtected + lYIMProtected + lAOLProtected;

    // Resolution for 1-462YEI.  No IM scanning action with invalid license.
    // Resolution for 1-44CQ5Y.  Enable/disable for 1 IM installed, configure for more than 1 IM. 
    if (lLicenseValid)                  // TODO:  Add " && (lIMValue > 0)" ?
    {
        if (lIMValue == 1)              // 1 IM client, protected
        {
            result = spActions-> AddClientAction (
                IDS_ACTION_DISABLE_IM_SCANNING,
                IDS_ACTION_DESC_DISABLE_IM_SCANNING,
                CClientCallback::eCOOKIE_DISABLE_IM_SCANNING,
                guidAction_Disable_IM);
	        CHECK_NSCRESULT(result);
            if (NSC_FAILED(result))
            {
                return result;
            }
        }
        else if (lIMValue == 2)         // Either 1 client is installed and not protected
                                        // or 2 clients are installed and protected.
        {
            if ((lMSNProtected == AVStatus::IM_Status_NotProtected) ||
                (lYIMProtected == AVStatus::IM_Status_NotProtected) ||
                (lAOLProtected == AVStatus::IM_Status_NotProtected))
            {
                result = spActions-> AddClientAction (
                    IDS_ACTION_ENABLE_IM_SCANNING,
                    IDS_ACTION_DESC_ENABLE_IM_SCANNING,
                    CClientCallback::eCOOKIE_ENABLE_IM_SCANNING,
                    guidAction_Enable_IM);
	            CHECK_NSCRESULT(result);
                if (NSC_FAILED(result))
                {
                    return result;
                }
            }
        }

        // always add Config for Admin users
        //if (bAdmin)
        //{
            result = spActions->AddClientAction(
                IDS_ACTION_CONFIGURE_IM_SCANNING,
                IDS_ACTION_DESC_CONFIGURE_IM_SCANNING,
                CClientCallback::eCOOKIE_CONFIGURE_IM_SCANNING,
                guidAction_Configure_IM);
	        CHECK_NSCRESULT(result);
            if (NSC_FAILED(result))
            {
                return result;
            }
        //}
    }

    actions_out = spActions;
    actions_out->AddRef();

    return result;
};

nsc::NSCRESULT CWormBlockingFeature::GetActions(const nsc::IContext* context_in, 
                                const nsc::IActionsCollection*& actions_out)
                                const throw()
{
	CCTRCTXI0(_T("Start"));

    // check params
    if (actions_out != NULL || context_in == NULL)
    {
        TRACEE(_T("Bad param"));
        return nsc::NSCERR_ILLEGALPARAM;
    }

    nsc::NSCRESULT result = nsc::NSC_FAIL;

    // Is user an admin?
    //bool bAdmin = IsAdmin(context_in);

    CNAVActionsCollectionPtr spActions = new CNAVActionsCollection ();
    if (spActions == NULL)
    {
        TRACEE(_T("Error in CNAVActionsCollection allocation in worm blocking."));
        return nsc::NSCERR_MEMORY;
    }

    // Check the status to ensure we do not show inappropriate actions.
	long lOEHVal = 1;
    long lLicenseValid = 1;
    long lSMTPVal = 1;


    CEventData* pEventData = NULL;
    if (CFeature::getInternalStatus (context_in, pEventData) &&
        pEventData )
    {
        ccLib::CSingleLock lock ( &m_critStatus, INFINITE, FALSE );
        pEventData->GetData( AVStatus::propEmailOEH, lOEHVal );
        pEventData->GetData( AVStatus::propLicValid, lLicenseValid );
        pEventData->GetData( AVStatus::propEmailSMTP, lSMTPVal );
    }    

    // Resolution for 1-462YEI.  No worm blocking action with invalid license.
    if (lLicenseValid)
    {
        // Enable Worm Blocking
        if(lSMTPVal && !lOEHVal)
	    {
            result = spActions-> AddClientAction (
                IDS_ACTION_ENABLE_WORM_BLOCKING,
                IDS_ACTION_DESC_ENABLE_WORM_BLOCKING,
                CClientCallback::eCOOKIE_ENABLE_WORM_BLOCKING,
                guidAction_Enable_Worm_Blocking);
	        CHECK_NSCRESULT(result);
            if (NSC_FAILED(result))
            {
                return result;
            }
        }

	    // Disable Worm Blocking
        if(lSMTPVal && lOEHVal)
	    {
            result = spActions-> AddClientAction (
                IDS_ACTION_DISABLE_WORM_BLOCKING,
                IDS_ACTION_DESC_DISABLE_WORM_BLOCKING,
                CClientCallback::eCOOKIE_DISABLE_WORM_BLOCKING,
                guidAction_Disable_Worm_Blocking);
	        CHECK_NSCRESULT(result);
            if (NSC_FAILED(result))
            {
                return result;
            }
        }

        //if (bAdmin)
        //{
            // Configure Worm Blocking
            result = spActions->AddClientAction(
                IDS_ACTION_CONFIGURE_WORM_BLOCKING,
                IDS_ACTION_DESC_CONFIGURE_WORM_BLOCKING,
                CClientCallback::eCOOKIE_CONFIGURE_WORM_BLOCKING,
                guidAction_Configure_Worm_Blocking);
	        CHECK_NSCRESULT(result);
            if (NSC_FAILED(result))
            {
                return result;
            }
        //}
    }

    actions_out = spActions;
    actions_out->AddRef();

    return result;
};


nsc::NSCRESULT CFSSFeature::GetActions(const nsc::IContext* context_in, 
                                const nsc::IActionsCollection*& actions_out)
                                const throw()
{
	CCTRCTXI0(_T("Start"));

    nsc::NSCRESULT result = nsc::NSC_FAIL;

	CNAVActionsCollectionPtr spActions = new CNAVActionsCollection ();
    if (spActions == NULL)
    {
        TRACEE(_T("Error in CNAVActionsCollection allocation in FSS."));
        return nsc::NSCERR_MEMORY;
    }

    // Check the product license.
    long lLicenseValid = 1;

    CEventData* pEventData = NULL;
    if (CFeature::getInternalStatus (context_in, pEventData) &&
        pEventData )
    {
        ccLib::CSingleLock lock ( &m_critStatus, INFINITE, FALSE );
        pEventData->GetData( AVStatus::propLicValid, lLicenseValid );
    }    

    // Resolution for 1-462YEI.  No FSS action with invalid license.
    if (lLicenseValid)
    {
        // Launch Full System Scan
        result = spActions-> AddClientAction (
            IDS_ACTION_LAUNCH_FSS,
            IDS_ACTION_DESC_LAUNCH_FSS,
            CClientCallback::eCOOKIE_LAUNCH_FSS_AND_WAIT,
            guidAction_Launch_FSS);
	    CHECK_NSCRESULT(result);
        if (NSC_FAILED(result))
        {
            return result;
        }
    }

    actions_out = spActions;
    actions_out->AddRef();

    return result;
};

nsc::NSCRESULT CVirusDefsFeature::GetActions(const nsc::IContext* context_in, 
                                const nsc::IActionsCollection*& actions_out)
                                const throw()
{
	CCTRCTXI0(_T("Start"));

    nsc::NSCRESULT result = nsc::NSC_FAIL;

	CNAVActionsCollectionPtr spActions = new CNAVActionsCollection ();
    if (spActions == NULL)
    {
        TRACEE(_T("Error in CNAVActionsCollection allocation in VirusDefs."));
        return nsc::NSCERR_MEMORY;
    }

    // Check the product license.
    long lLicenseValid = 1;

    CEventData* pEventData = NULL;
    if (CFeature::getInternalStatus (context_in, pEventData) &&
        pEventData )
    {
        ccLib::CSingleLock lock ( &m_critStatus, INFINITE, FALSE );
        pEventData->GetData( AVStatus::propLicValid, lLicenseValid );
    }    

    // Resolution for 1-462YEI.  No virus definition action with invalid license.
    if (lLicenseValid)
    {
        // Launch Virus Definitions
        result = spActions-> AddClientAction (
            IDS_ACTION_LAUNCH_VIRUSDEFS,
            IDS_ACTION_DESC_LAUNCH_VIRUSDEFS,
            CClientCallback::eCOOKIE_LAUNCH_RUN_LU_AND_WAIT,
            guidAction_Launch_LiveUpdate);
	    CHECK_NSCRESULT(result);
        if (NSC_FAILED(result))
        {
            return result;
        }
    }

    actions_out = spActions;
    actions_out->AddRef();

    return result;
};


nsc::NSCRESULT CScanNowFeature::GetActions(const nsc::IContext* context_in, 
                                const nsc::IActionsCollection*& actions_out)
                                const throw()
{
	CCTRCTXI0(_T("Start"));

    nsc::NSCRESULT result = nsc::NSC_FAIL;

	CNAVActionsCollectionPtr spActions = new CNAVActionsCollection ();
    if (spActions == NULL)
    {
        TRACEE(_T("Error in CNAVActionsCollection allocation in scan now."));
        return nsc::NSCERR_MEMORY;
    }

    // Check the product license.
    long lLicenseValid = 1;

    CEventData* pEventData = NULL;
    if (CFeature::getInternalStatus (context_in, pEventData) &&
        pEventData )
    {
        ccLib::CSingleLock lock ( &m_critStatus, INFINITE, FALSE );
        pEventData->GetData( AVStatus::propLicValid, lLicenseValid );
    }    

    // Resolution for 1-462YEI.  No scan now action with invalid license.
    if (lLicenseValid)
    {
        // Launch Full System Scan
        result = spActions-> AddClientAction (
            IDS_ACTION_SCAN_NOW,
            IDS_ACTION_DESC_SCAN_NOW,
            CClientCallback::eCOOKIE_LAUNCH_FSS_AND_WAIT,
            guidAction_Launch_FSS);
	    CHECK_NSCRESULT(result);
        if (NSC_FAILED(result))
        {
            return result;
        }
    }

    actions_out = spActions;
    actions_out->AddRef();

    return result;
};

nsc::NSCRESULT CQuarantineFeature::GetActions(const nsc::IContext* context_in, 
                                const nsc::IActionsCollection*& actions_out)
                                const throw()
{
	CCTRCTXI0(_T("Start"));

    nsc::NSCRESULT result = nsc::NSC_FAIL;

	CNAVActionsCollectionPtr spActions = new CNAVActionsCollection ();
    if (spActions == NULL)
    {
        TRACEE(_T("Error in CNAVActionsCollection allocation in quarantine."));
        return nsc::NSCERR_MEMORY;
    }

    // launch quarantine
    result = spActions-> AddClientAction (
        IDS_ACTION_LAUNCH_QUARANTINE,
        IDS_ACTION_DESC_LAUNCH_QUARANTINE,
        CClientCallback::eCOOKIE_LAUNCH_QUARANTINE,
        guidAction_Launch_Quarantine);
	CHECK_NSCRESULT(result);
    if (NSC_FAILED(result))
    {
        return result;
    }

    actions_out = spActions;
    actions_out->AddRef();

    return result;
};
