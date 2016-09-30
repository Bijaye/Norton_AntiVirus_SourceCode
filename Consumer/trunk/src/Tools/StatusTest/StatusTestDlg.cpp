////////////////////////
//
// PROPRIETARY / CONFIDENTIAL.
// Use of this product is subject to license terms.
// Copyright © 2006 Symantec Corporation.
// All rights reserved.
//
/////////////////////////////////////////////////////////////////////////////////////////////////

// StatusTestDlg.cpp : implementation file
//

#include "stdafx.h"
#include "StatusTest.h"
#include "StatusTestDlg.h"

// CStatusTestDlg dialog
#include "OSInfo.h"
#include "AvEvents.h"
#include "NavEventFactoryLoader.h"
#include "AllNavEvents.h"
#include "StatusPropertyNames.h"
#include "NAVEventCommon.h"
#include "ccSymModuleLifetimeMgrHelper.h"

#include <xstring> // for wstring
#include <vector>
#include ".\statustestdlg.h"

#include "NSCLoader.h"
#include "avNSCPlg_GUIDs.h"     // Our GUIDs

CStatusTestDlg::CStatusTestDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CStatusTestDlg::IDD, pParent)
    , m_strLog(_T(""))
    , m_bALU(TRUE)
    , m_bAP(TRUE)
    , m_bFSS(TRUE)
    , m_bDefs(TRUE)
    , m_bEmail(TRUE)
    , m_bIWP(TRUE)
    , m_bLicensing(TRUE)
    , m_bIM(TRUE)
    , m_bSpyware(TRUE)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CStatusTestDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialog::DoDataExchange(pDX);
    DDX_Text(pDX, IDC_LOG, m_strLog);
    DDX_Check(pDX, IDC_ALU, m_bALU);
    DDX_Check(pDX, IDC_AP, m_bAP);
    DDX_Check(pDX, IDC_FSS, m_bFSS);
    DDX_Check(pDX, IDC_Defs, m_bDefs);
    DDX_Check(pDX, IDC_Email, m_bEmail);
    DDX_Check(pDX, IDC_IWP, m_bIWP);
    DDX_Check(pDX, IDC_LIC, m_bLicensing);
    DDX_Check(pDX, IDC_IM, m_bIM);
    DDX_Check(pDX, IDC_SPYWARE, m_bSpyware);
}

BEGIN_MESSAGE_MAP(CStatusTestDlg, CDialog)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
    ON_BN_CLICKED(IDC_GETSTATUS, OnBnClickedGetstatus)
    ON_BN_CLICKED(IDC_BUTTON1, OnRefreshAP)
    ON_BN_CLICKED(IDC_CLEAR, OnBnClickedClear)
END_MESSAGE_MAP()


// CStatusTestDlg message handlers

BOOL CStatusTestDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

    m_pEMSubscriber = new CEMSubscriber (this);
    m_pEMSubscriber->AddRef();

    // The list of subscribed events
    //
    m_vecEventIDs.push_back ( (long) AV::Event_ID_StatusALU );
    m_vecEventIDs.push_back ( (long) AV::Event_ID_StatusAP );
    m_vecEventIDs.push_back ( (long) AV::Event_ID_StatusFSS );
    m_vecEventIDs.push_back ( (long) AV::Event_ID_StatusDefs );
    m_vecEventIDs.push_back ( (long) AV::Event_ID_StatusIWP );
    m_vecEventIDs.push_back ( (long) AV::Event_ID_StatusEmail );
    m_vecEventIDs.push_back ( (long) AV::Event_ID_StatusLicensing );
    m_vecEventIDs.push_back ( (long) AV::Event_ID_StatusIMScan );
    m_vecEventIDs.push_back ( (long) AV::Event_ID_StatusSpyware );

    // Build the resulting list of unique subscribed events
    //
    for ( std::vector<long>::iterator iter = m_vecEventIDs.begin(); iter != m_vecEventIDs.end(); iter++)
    {
        ccEvtMgr::CSubscriberHelper::SubscriberInfo subinfo;
        subinfo.m_nEventType = *iter;
        subinfo.m_nPriority = 0;
	    subinfo.m_pEventStates = NULL;
	    subinfo.m_nEventStatesCount = 0;
        subinfo.m_nProviderId = 0;
        subinfo.m_bMonitorOnly = false;

        m_pEMSubscriber->m_vecSubscriberInfo.push_back (subinfo);
    }

    // Start listening for events
    if ( !m_pEMSubscriber->Connect())
    {
        CCTRACEE ("EMSubscriber - connect failed");
        m_strLog += "Connected to Event Manager: Failed\r\n";
    }
    else
    {
        m_strLog += "Connected to Event Manager: successful\r\n";
    }

    // For sending data

    if (SYM_FAILED (AV::NAVEventFactory_IAVEventFactory::CreateObject (GETMODULEMGR(), &m_pProvider)) ||
        m_pProvider.m_p == NULL )
    {
        CCTRACEE ( "Could not create IAvFactory object. - %d", ::GetLastError() );
    }

    UpdateData ( FALSE );
	return TRUE;  // return TRUE  unless you set the focus to a control
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CStatusTestDlg::OnPaint() 
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CStatusTestDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CStatusTestDlg::OnBnClickedGetstatus()
{
    UpdateData(TRUE);
    m_strLog += "*** Getting Status ***\r\n";

    if ( m_pProvider.m_p )
    {
        // We have to set the event ID before sending
        //
        CEventData edTemp;
        edTemp.SetData ( AV::Event_Base_propType, AV::Event_ID_StatusRequest);

        std::vector <long> vecFeatures;
        
        if ( m_bFSS )
            vecFeatures.push_back ( (long)AV::Event_ID_StatusFSS );

        if ( m_bAP )
            vecFeatures.push_back ( (long)AV::Event_ID_StatusAP );

        if ( m_bALU )
            vecFeatures.push_back ( (long)AV::Event_ID_StatusALU );
        if ( m_bEmail )
            vecFeatures.push_back ( (long)AV::Event_ID_StatusEmail );
        if ( m_bDefs )
            vecFeatures.push_back ( (long)AV::Event_ID_StatusDefs );
        if ( m_bIWP )
            vecFeatures.push_back ( (long)AV::Event_ID_StatusIWP );
        if ( m_bLicensing )
            vecFeatures.push_back ( (long)AV::Event_ID_StatusLicensing );
        if ( m_bIM )
            vecFeatures.push_back ( (long)AV::Event_ID_StatusIMScan );
        if ( m_bSpyware )
            vecFeatures.push_back ( (long)AV::Event_ID_StatusSpyware );

        edTemp.SetData ( AVStatus::propRequestedFeatures, (BYTE*)&vecFeatures[0], (long) vecFeatures.size()*sizeof(long));
        edTemp.SetData ( AV::Event_Base_propSessionID, COSInfo::GetSessionID());
        
        CEventData* pReturnEvent;
        SYMRESULT result = m_pProvider->SendAvEvent (edTemp, pReturnEvent);
        if ( SYM_SUCCEEDED (result))
        {
            if ( pReturnEvent )
            {
                CEventData edStatus;
                if (!pReturnEvent->GetNode ( AVStatus::propCurrentStatus, edStatus ))
                {
                    CCTRACEE ("No status bag");
                    m_strLog += "!!! No data in the return event. Is StatusHP running?\r\n";
                }
                else
                {
                    getProperties (edStatus);
                }
            }
            else
                m_strLog += "!!! No return event\r\n";
        }
        else
        {
            CCTRACEE ("Failed sending event %d", result);
            m_strLog += "Failed sending event\r\n";
        }
    }

    m_strLog += "*** Complete ***\r\n\r\n";
    UpdateData(FALSE);
}

void CStatusTestDlg::EMSubscriberOnEvent (const ccEvtMgr::CEventEx& Event,
                                ccEvtMgr::CSubscriberEx::EventAction& eAction)
{
    UpdateData (TRUE);
    // We want to alert on ALL NAV error events for this session, or
    // if no session data is available.
    //
    CNAVEventCommonQIPtr pNAVEvent (&Event);
    ASSERT (pNAVEvent != NULL);
    if ( pNAVEvent )
    {
        long lSessionID = 0;
        pNAVEvent->props.GetData(AV::Event_Base_propSessionID, lSessionID );
        
        if ( COSInfo::GetSessionID() == lSessionID )
        {
            CEventData edStatus;
            if (!pNAVEvent->props.GetNode ( AVStatus::propNewStatus, edStatus ))
            {
                CCTRACEE ("No status bag");
            }
            else
            {
                m_strLog += "*** Update ***\r\n";
                m_strLog += getEventString ( pNAVEvent->GetType () );
                m_strLog += "\r\n";
                getProperties (edStatus);
                m_strLog += "*** Update complete ***\r\n";
            }
        }
    }
    UpdateData (FALSE);
}

void CStatusTestDlg::EMSubscriberOnShutdown ()
{
}

void CStatusTestDlg::EndDialog(int nResult)
{
    if ( m_pEMSubscriber )
    {    
        m_pEMSubscriber->Disconnect();
        delete m_pEMSubscriber;
        m_pEMSubscriber = NULL;
    }

    return CDialog::EndDialog (nResult);
}

void CStatusTestDlg::getProperties(CEventData& edStatus)
{
    // Add new status items here.
    //
    std::wstring strTemp;
    long lTemp = 0;

    if ( edStatus.GetData ( AVStatus::propALUStatus, lTemp ))
    {
        m_strLog+= "ALU status: ";
        m_strLog+= getStatusString (lTemp);
        m_strLog+= "\r\n";
    }

    if ( edStatus.GetData ( AVStatus::propAPStatus, lTemp ))
    {
        m_strLog+= "AP status: ";
        m_strLog+= getStatusString (lTemp);
        m_strLog+= "\r\n";
    }

    if ( edStatus.GetData ( AVStatus::propAPSpywareStatus, lTemp ))
    {
        m_strLog+= "AP spyware status: ";
        m_strLog+= getStatusString (lTemp);
        m_strLog+= "\r\n";
    }

    // Defs
    //
    if ( edStatus.GetData ( AVStatus::propDefsStatus, lTemp ))
    {
        m_strLog+= "Defs status: ";
        m_strLog+= getStatusString (lTemp);
        m_strLog+= "\r\n";
    }

    if ( edStatus.GetData ( AVStatus::propDefsDate, strTemp ))
    {
        m_strLog+= "Defs date: ";
        m_strLog+= strTemp.c_str();
        m_strLog+= "\r\n";
    }

    if ( edStatus.GetData ( AVStatus::propDefsUpdateType, lTemp ))
    {
        m_strLog+= "Defs update: ";
        switch ( lTemp )
        {
        case AVStatus::Defs_Update_Type_None:
            m_strLog += "Initial";
            break;
        case AVStatus::Defs_Update_Type_ALU:
            m_strLog += "ALU";
            break;
        case AVStatus::Defs_Update_Type_LU:
            m_strLog += "LiveUpdate";
            break;
        case AVStatus::Defs_Update_Type_IU:
            m_strLog += "Intelligent Updater/other";
            break;
        default:
            m_strLog += "Unknown";
        }

        m_strLog+= "\r\n";
    }

    // FSS
    if ( edStatus.GetData ( AVStatus::propFSSStatus, lTemp ))
    {
        m_strLog+= "FSS status: ";
        m_strLog+= getStatusString (lTemp);
        m_strLog+= "\r\n";
    }

    if ( edStatus.GetData ( AVStatus::propFSSDateS, strTemp ))
    {
        m_strLog+= "FSS Date (S): ";
        m_strLog+= strTemp.c_str();
        m_strLog+= "\r\n";
    }

    // IWP
    if ( edStatus.GetData ( AVStatus::propIWPStatus, lTemp ))
    {
        m_strLog+= "IWP status: ";
        m_strLog+= getStatusString (lTemp);
        m_strLog+= "\r\n";
    }

    // Email
    if ( edStatus.GetData ( AVStatus::propEmailStatus, lTemp ))
    {
        m_strLog+= "Email status: ";
        m_strLog+= getStatusString (lTemp);
        m_strLog+= "\r\n";
    }

    if ( edStatus.GetData ( AVStatus::propEmailSMTP, lTemp ))
    {
        m_strLog+= "Email SMTP: ";
        char szTemp [10] = {0};
        itoa ( lTemp, szTemp, 10);
        m_strLog+= szTemp;
        m_strLog+= "\r\n";
    }

    if ( edStatus.GetData ( AVStatus::propEmailPOP, lTemp ))
    {
        m_strLog+= "Email POP: ";
        char szTemp [10] = {0};
        itoa ( lTemp, szTemp, 10);
        m_strLog+= szTemp;
        m_strLog+= "\r\n";
    }

    if ( edStatus.GetData ( AVStatus::propEmailOEH, lTemp ))
    {
        m_strLog+= "Email OEH: ";
        char szTemp [10] = {0};
        itoa ( lTemp, szTemp, 10);
        m_strLog+= szTemp;
        m_strLog+= "\r\n";
    }

    // Licensing
    if ( edStatus.GetData ( AVStatus::propLicStatus, lTemp ))
    {
        m_strLog+= "Licensing status: ";
        m_strLog+= getStatusString (lTemp);
        m_strLog+= "\r\n";
    }

    if ( edStatus.GetData ( AVStatus::propLicType, lTemp ))
    {
        m_strLog+= "Licensing type: ";
        char szTemp [10] = {0};
        itoa ( lTemp, szTemp, 10);
        m_strLog+= szTemp;
        m_strLog+= "\r\n";
    }

    if ( edStatus.GetData ( AVStatus::propLicState, lTemp ))
    {
        m_strLog+= "Licensing state: ";
        char szTemp [10] = {0};
        itoa ( lTemp, szTemp, 10);
        m_strLog+= szTemp;
        m_strLog+= "\r\n";
    }

    if ( edStatus.GetData ( AVStatus::propLicZone, lTemp ))
    {
        m_strLog+= "Licensing zone: ";
        char szTemp [10] = {0};
        itoa ( lTemp, szTemp, 10);
        m_strLog+= szTemp;
        m_strLog+= "\r\n";
    }

    if ( edStatus.GetData ( AVStatus::propLicValid, lTemp ))
    {
        m_strLog+= "Licensing valid: ";
        char szTemp [10] = {0};
        itoa ( lTemp, szTemp, 10);
        m_strLog+= szTemp;
        m_strLog+= "\r\n";
    }

    if ( edStatus.GetData ( AVStatus::propLicVendorID, lTemp ))
    {
        m_strLog+= "Licensing vendor ID: ";
        char szTemp [100] = {0};
        itoa ( lTemp, szTemp, 10);
        m_strLog+= szTemp;
        m_strLog+= "\r\n";
    }

    // Subscriptions
    if ( edStatus.GetData ( AVStatus::propSubWarning, lTemp ))
    {
        m_strLog+= "Subscription warning: ";
        char szTemp [10] = {0};
        itoa ( lTemp, szTemp, 10);
        m_strLog+= szTemp;
        m_strLog+= "\r\n";
    }

    if ( edStatus.GetData ( AVStatus::propSubExpired, lTemp ))
    {
        m_strLog+= "Subscription expired: ";
        char szTemp [10] = {0};
        itoa ( lTemp, szTemp, 10);
        m_strLog+= szTemp;
        m_strLog+= "\r\n";
    }

    if ( edStatus.GetData ( AVStatus::propSubDaysLeft, lTemp ))
    {
        m_strLog+= "Subscription days left: ";
        char szTemp [10] = {0};
        itoa ( lTemp, szTemp, 10);
        m_strLog+= szTemp;
        m_strLog+= "\r\n";
    }

    if ( edStatus.GetData ( AVStatus::propSubDateS, strTemp ))
    {
        m_strLog+= "Subcription Date (S): ";
        m_strLog+= strTemp.c_str();
        m_strLog+= "\r\n";
    }

    // IM scanner
    if ( edStatus.GetData ( AVStatus::propIMAOL, lTemp ))
    {
        m_strLog+= "IM AOL: ";
        char szTemp [10] = {0};
        itoa ( lTemp, szTemp, 10);
        m_strLog+= szTemp;
        m_strLog+= "\r\n";
    }

    if ( edStatus.GetData ( AVStatus::propIMYIM, lTemp ))
    {
        m_strLog+= "IM YIM: ";
        char szTemp [10] = {0};
        itoa ( lTemp, szTemp, 10);
        m_strLog+= szTemp;
        m_strLog+= "\r\n";
    }

    if ( edStatus.GetData ( AVStatus::propIMTOM, lTemp ))
    {
        m_strLog+= "IM TOM: ";
        char szTemp [10] = {0};
        itoa ( lTemp, szTemp, 10);
        m_strLog+= szTemp;
        m_strLog+= "\r\n";
    }

    if ( edStatus.GetData ( AVStatus::propIMMSN, lTemp ))
    {
        m_strLog+= "IM MSN: ";
        char szTemp [10] = {0};
        itoa ( lTemp, szTemp, 10);
        m_strLog+= szTemp;
        m_strLog+= "\r\n";
    }

    //Spyware
    if ( edStatus.GetData ( AVStatus::propSpywareCat, lTemp ))
    {
        m_strLog+= "Spyware category: ";
        char szTemp [10] = {0};
        itoa ( lTemp, szTemp, 10);
        m_strLog+= szTemp;
        m_strLog+= "\r\n";
    }

    if ( edStatus.GetData ( AVStatus::propSpywareInstalled, lTemp ))
    {
        m_strLog+= "Spyware installed: ";
        char szTemp [10] = {0};
        itoa ( lTemp, szTemp, 10);
        m_strLog+= szTemp;
        m_strLog+= "\r\n";
    }
}

const char* CStatusTestDlg::getStatusString (DWORD dwStatus)
{
    switch (dwStatus)
    {
    case AVStatus::statusNone:
        return "None";// Uninitialized - ERROR  (Objects should never return this)
    case AVStatus::statusError:
        return "Error";
    case AVStatus::statusNotInstalled:
        return "Not installed";
    case AVStatus::statusNotRunning:
        return "Not running";
    case AVStatus::statusEnabled:
        return "Enabled";
    case AVStatus::statusDisabled:
        return "Disabled";
    case AVStatus::statusNotAvailable:
        return "Not available";
    case AVStatus::statusRental:
        return "Rental";
    }
    return "Unknown";
}

const char* CStatusTestDlg::getEventString (long lEventID )
{
    switch (lEventID)
    {
    case AV::Event_ID_StatusALU:
        return "ALU";
    case AV::Event_ID_StatusAP:
        return "AP";
    case AV::Event_ID_StatusEmail:
        return "Email";
    case AV::Event_ID_StatusFSS:
        return "FSS";
    case AV::Event_ID_StatusLicensing:
        return "Licensing";
    case AV::Event_ID_StatusDefs:
        return "Defs";
    case AV::Event_ID_StatusIWP:
        return "IWP";
    case AV::Event_ID_StatusIMScan:
        return "IM";
    case AV::Event_ID_StatusSpyware:
        return "Spyware";
    default:
        return "Unknown";
    }
}
void CStatusTestDlg::OnRefreshAP()
{
    // Load NSCServerAPI.dll
    //
    CCTRACEI ( "CStatusTestDlg::OnRefreshAP - start " );

    NSC_IConsoleServerLoader NSCLoader;
    nsc::IConsoleServerPtr pNSCServer;

    try
    {
        if ( SYM_SUCCEEDED ( NSCLoader.CreateObject ( &pNSCServer )) && pNSCServer)
        {
            CCTRACEI ( "CStatusTestDlg::OnRefreshAP - NSCAPI loaded, calling Update" );
            nsc::NSCRESULT result = nsc::NSC_FAIL;

            result = pNSCServer->UpdateWrapper ( CLSID_NAV_Feature_AutoProtect, 0 /*not currently defined*/);
            if ( NSC_FAILED (result))
                CCTRACEE ("CStatusTestDlg OnRefreshAP failed %d", result);
            else
                CCTRACEI ( "CStatusTestDlg::OnRefreshAP - success" );

        }
        else
        {
            CCTRACEE ( "CStatusTestDlg::OnRefreshAP - failed to load NSC API" );
        }
    }
    catch(...)
    {
        CCTRACEE ( "CStatusTestDlg::OnRefreshAP - caught exception" );
        assert(false);
    }

}

void CStatusTestDlg::OnBnClickedClear()
{
    m_strLog.Empty();
    UpdateData (FALSE);
}
