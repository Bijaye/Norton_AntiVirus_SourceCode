#include "StdAfx.h"
#include ".\stateim.h"
#include "OptNames.h"   // Option names
#include "IMScanWrapper.h"

//extern CNAVOptSettingsEx g_NavOpts;

CStateIM::CStateIM(CWMIIntegration* pWMI, CNSCIntegration* pNSC):CState(pWMI,pNSC) 
{
    // Update for either Licensing changes.
    if ( makeEvent (SYM_REFRESH_NAV_LICENSE_STATUS_EVENT, true, m_eventLicensing))
        m_vecWaitEvents.push_back (m_eventLicensing);

    if ( makeEvent (SYM_REFRESH_IMSCANNER_STATUS_EVENT, true, m_eventIM))
        m_vecWaitEvents.push_back (m_eventIM);        

    m_lEventID = AV::Event_ID_StatusIMScan;
    m_lOldMSNProtected = AVStatus::IM_Status_NotInstalled;
    m_lOldAOLProtected = AVStatus::IM_Status_NotInstalled;
    m_lOldTOMProtected = AVStatus::IM_Status_NotInstalled;
    m_lOldYIMProtected = AVStatus::IM_Status_NotInstalled;
}

CStateIM::~CStateIM(void)
{
}

bool CStateIM::Save ()
{
    CCTRACEI( _T("CStateIM::save()"));
    // Only one save activity at a time since they all share the same data object
    ccLib::CSingleLock lockStatus (&m_critStatus, INFINITE, FALSE);

    DWORD dwMSN = 0;
    DWORD dwAOL = 0;
    DWORD dwTOM = 0;
    DWORD dwYIM = 0;

    // Look for any IM's that are installed but not protected
    //
    long lMSNProtected = AVStatus::IM_Status_NotInstalled;
    long lAOLProtected = AVStatus::IM_Status_NotInstalled;
    long lTOMProtected = AVStatus::IM_Status_NotInstalled;
    long lYIMProtected = AVStatus::IM_Status_NotInstalled;

    // Save data
    //
    m_edStatus.SetData ( AVStatus::propIMMSN, lMSNProtected );
    m_edStatus.SetData ( AVStatus::propIMAOL, lAOLProtected );
    m_edStatus.SetData ( AVStatus::propIMTOM, lTOMProtected ); 
    m_edStatus.SetData ( AVStatus::propIMYIM, lYIMProtected );

    // Is IM enabled?
    //
    try
    {
        CNAVOptSettingsEx NavOpts;

        // Try to load the file.
        //
	    if ( !NavOpts.Init() )
        {
            CCTRACEE("Failed to init options" );
            throw FALSE;
        }

        DWORD dwRetries = 0;

        // Read the IM settings.
        //
		NavOpts.GetValue(IMSCAN_FeatureEnabledScanMIM, dwMSN, 0 );
        NavOpts.GetValue(IMSCAN_FeatureEnabledScanAIM, dwAOL, 0 );
        NavOpts.GetValue(IMSCAN_FeatureEnabledScanYIM, dwYIM, 0 );
        NavOpts.GetValue(IMSCAN_FeatureEnabledScanTOL, dwTOM, 0 );

        try
        {
            CIMScanWrapper IMScan;

            if (IMScan.IsIMScannerInstalled (IMTYPE_MSN))
                lMSNProtected = (bool)dwMSN ? AVStatus::IM_Status_Protected : AVStatus::IM_Status_NotProtected;
            else
                lMSNProtected = AVStatus::IM_Status_NotInstalled;

            if (IMScan.IsIMScannerInstalled (IMTYPE_AOL))
                lAOLProtected = (bool)dwAOL ? AVStatus::IM_Status_Protected : AVStatus::IM_Status_NotProtected;
            else
                lAOLProtected = AVStatus::IM_Status_NotInstalled;

            if (IMScan.IsIMScannerInstalled (IMTYPE_YAHOO))
                lYIMProtected = (bool)dwYIM ? AVStatus::IM_Status_Protected : AVStatus::IM_Status_NotProtected;
            else
                lYIMProtected = AVStatus::IM_Status_NotInstalled;

            if (IMScan.IsIMScannerInstalled (IMTYPE_TOM))
                lTOMProtected = (bool)dwTOM ? AVStatus::IM_Status_Protected : AVStatus::IM_Status_NotProtected;
            else
                lTOMProtected = AVStatus::IM_Status_NotInstalled;

        }
        catch(...)
        {
            CCTRACEE ("Failed to load IMScan");
            assert(false);
            return false;
        }
    }
    catch (...)
    {
        CCTRACEE("StateIM: Unknown exception");
        assert(false);
        return false;
    }

    bool bReturn = false;

    // Save data, fire event if there was a change
    //
    if ( m_lOldMSNProtected != lMSNProtected ||
         m_lOldAOLProtected != lAOLProtected ||
         m_lOldTOMProtected != lTOMProtected ||
         m_lOldYIMProtected != lYIMProtected )
        bReturn = true;

    m_lOldMSNProtected = lMSNProtected;
    m_lOldAOLProtected = lAOLProtected;
    m_lOldTOMProtected = lTOMProtected;
    m_lOldYIMProtected = lYIMProtected;

    m_edStatus.SetData ( AVStatus::propIMMSN, m_lOldMSNProtected );
    m_edStatus.SetData ( AVStatus::propIMAOL, m_lOldAOLProtected );
    m_edStatus.SetData ( AVStatus::propIMTOM, m_lOldTOMProtected ); 
    m_edStatus.SetData ( AVStatus::propIMYIM, m_lOldYIMProtected );

	CCTRACEI("StateIM: Data saved");
    return bReturn;
}
