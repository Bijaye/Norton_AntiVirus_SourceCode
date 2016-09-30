/////////////////////////////////////////////////////////////////////////////
// $Header:   S:/N32OPTS/VCS/ALERTPGE.CPv   1.14   16 Jul 1998 11:59:56   DBuches  $
/////////////////////////////////////////////////////////////////////////////
//
// ALERTPGE.CPP - Contains class impelmentaion for CAlertsPage class
//
//
//
//
//
//
//
//
//
//
/////////////////////////////////////////////////////////////////////////////
// $Log:   S:/N32OPTS/VCS/ALERTPGE.CPv  $
// 
//    Rev 1.14   16 Jul 1998 11:59:56   DBuches
// Added help to network browser dialog.
// 
//    Rev 1.13   24 Jun 1998 18:16:08   jtaylor
// Removed the firstWindow parameter to allignWnd.  It was causing an assertion.
// 
//    Rev 1.12   11 Jun 1998 01:39:40   TCASHIN
// Fixed the whatsthis IDs for forwarding alerts to NT for Elizabeth.
// 
//    Rev 1.11   27 Feb 1998 17:25:38   RPULINT
// Aligned checkbox->edit->Spin comination dymanically so they could look
// fine in all languages (for NAV95 only- these don;t show up in NAVNT)
// 
//    Rev 1.10   19 Feb 1998 11:57:10   mdunn
// Ported NEC changes from ROSNEC.
// 
//    Rev 1.9   25 Aug 1997 16:10:00   DBUCHES
// Fixed problem with NT alerts checkbox.
// 
//    Rev 1.8   08 Jul 1997 16:54:46   DBUCHES
// Use different dialog templat for NT platforms.
// 
//    Rev 1.7   25 Jun 1997 15:16:04   DBUCHES
// Converted buttons to owner draw buttons.
// 
//    Rev 1.6   25 Jun 1997 11:54:32   DBUCHES
// Hooked up more help topics.
// 
//    Rev 1.5   20 Jun 1997 12:00:42   DBUCHES
// Added Help support.
// 
//    Rev 1.4   13 Jun 1997 21:59:32   JBRENNA
// 1. Reworked the NLM option read/write to file. We now use the NAVALERT.DAT
//    for the NLM persistent storage.
// 2. Added "Forward to NAV NT Alert Service" alerting. This includes using
//    the GetForwardTarget and SetForwardTarget for NAVALERT.DAT accessing.
// 
//    Rev 1.3   10 Jun 1997 20:19:48   DBUCHES
// Added changes for alert password.
// 
//    Rev 1.2   27 May 1997 20:55:08   DBUCHES
// I touched 30 files this rev.  Needed for support of NT Alerting.
//
//    Rev 1.1   18 May 1997 23:26:32   DBUCHES
// Fixed browsing for NAV-NLM servers.
//
//    Rev 1.0   14 May 1997 14:58:06   DBUCHES
// Initial revision.
/////////////////////////////////////////////////////////////////////////////


#include "stdafx.h"
#include "n32opts.h"
#include "AlertPge.h"
#include "servenfy.h"
#include "netbrwsd.h"
#include "at_opt95.h"
#include "AlignWnd.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// Set up help maps
/////////////////////////////////////////////////////////////////////////////
DWORD CAlertsPage::m_dwWhatsThisMap[] = { 
    IDC_MESSAGE_EDIT,       IDH_NAVW_WHATHIS_DISPLAY_ALERT_MSG_TXTBOX,
    IDC_DISPLAY_CHECK,      IDH_NAVW_WHATHIS_DISPLAY_ALERT_MSG_CHKBOX,
    IDC_AUDIBLE_CHECK,      IDH_NAVW_WHATHIS_AUDIBLE_ALERT_CHKBOX,
    IDC_REMOVE_CHECK,       IDH_NAVW_WHATHIS_REMOVE_ALERT_DIALOG_AFTER_DLG,
    IDC_SECONDS_EDIT,       IDH_NAVW_WHATHIS_ALERT_SECONDS_CTRL,
    IDC_SECONDS_SPIN,       IDH_NAVW_WHATHIS_ALERT_SECONDS_CTRL,
    IDC_NLM_CHECK,          IDH_NAVW_ALERT_FORWARD_ALERT_TO_NAV_NLM_ALERT_SERVICE,
    IDC_NLM_COMBO,          IDH_NAVW_ALERT_NAV_IF_NLM_IS_PRESENT_COMBO,
    IDC_NTALERT_BROWSE,     IDH_NAVW_FORWARD_ALERT_TO_NAV_NT_ALERT_SERVICE_COMBOBX,
    IDC_NTALERT_CHECK,      IDH_NAVW_FORWARD_ALERT_TO_NAV_NT_ALERT_SERVICE_CHKBX,
    0,0 
    };
DWORD CAlertsPage::m_dwHowToMap[] = { 0,0 };

/////////////////////////////////////////////////////////////////////////////
// Local protos
static BOOL NLMServerEnumProc( LPSTR  lpszServerName,
                               LPVOID lpAddress,
                               LPVOID lpExtra);




/////////////////////////////////////////////////////////////////////////////
// CAlertsPage property page

IMPLEMENT_DYNCREATE(CAlertsPage, CHelperPage)

CAlertsPage::CAlertsPage() : CHelperPage(IS_NT() ? IDD_ALERTS_PAGENT : CAlertsPage::IDD)
{
	//{{AFX_DATA_INIT(CAlertsPage)
	m_bDisplayAlert = FALSE;
	m_sAlertText = _T("");
	m_sSecondsText = _T("");
	m_bRemoveAfter = FALSE;
	m_bAlertNLM = FALSE;
	m_bBeep = FALSE;
	m_bNtAlert = FALSE;
	m_sNtAlertServer = _T("");
	m_sNLMCombo = _T("");
	//}}AFX_DATA_INIT
    m_pHelpHowToMap = m_dwWhatsThisMap;
    m_pHelpWhatsThisMap = m_dwWhatsThisMap;
    m_dwHelpButtonHelpID = IDH_NAVW_ALERTS_DLG_HELP_BTN;
    // Set up Password ID
    m_iPasswordFeature = PASSWORD_VIEWALERTOPT;
}

CAlertsPage::~CAlertsPage()
{
}

void CAlertsPage::DoDataExchange(CDataExchange* pDX)
{
	CHelperPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAlertsPage)
	DDX_Control(pDX, IDC_NTALERT_BROWSE, m_BrowseBtn);
	DDX_Check(pDX, IDC_DISPLAY_CHECK, m_bDisplayAlert);
	DDX_Text(pDX, IDC_MESSAGE_EDIT, m_sAlertText);
    DDV_MaxChars(pDX, m_sAlertText, SIZE_ALERT_MSG);
	DDX_Check(pDX, IDC_NLM_CHECK, m_bAlertNLM);
	DDX_Check(pDX, IDC_AUDIBLE_CHECK, m_bBeep);
	DDX_Check(pDX, IDC_NTALERT_CHECK, m_bNtAlert);
	DDX_Text(pDX, IDC_NTALERT_EDIT, m_sNtAlertServer);
	DDX_CBString(pDX, IDC_NLM_COMBO, m_sNLMCombo);
	//}}AFX_DATA_MAP

    if(!IS_NT())
        {
        DDX_Control(pDX, IDC_SECONDS_SPIN, m_Spin);
        DDX_Check(pDX, IDC_REMOVE_CHECK, m_bRemoveAfter);
    	DDX_Text(pDX, IDC_SECONDS_EDIT, m_sSecondsText);
	    DDV_MaxChars(pDX, m_sSecondsText, 2);
        }
}


BEGIN_MESSAGE_MAP(CAlertsPage, CHelperPage)
	//{{AFX_MSG_MAP(CAlertsPage)
	ON_BN_CLICKED(IDC_DISPLAY_CHECK, OnDisplayCheck)
	ON_BN_CLICKED(IDC_REMOVE_CHECK, OnRemoveCheck)
	ON_BN_CLICKED(IDC_NLM_CHECK, OnNlmCheck)
	ON_CBN_DROPDOWN(IDC_NLM_COMBO, OnDropdownNlmCombo)
	ON_BN_CLICKED(IDC_NTALERT_CHECK, OnNtAlertCheck)
	ON_BN_CLICKED(IDC_NTALERT_BROWSE, OnNtAlertBrowse)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CAlertsPage message handlers

BOOL CAlertsPage::SetData(LPNAVOPTIONS pOptions)
{
	ALERTOPT *pAlert = &pOptions->alert;
	TCHAR    szServer[SYM_MAX_PATH];

    m_bBeep = pAlert->bBeep;
    m_bDisplayAlert = pAlert->bDispAlertMsg;
    m_bRemoveAfter = pAlert->bRemoveAfter;
    m_bAlertNLM = pAlert->bAlertNLM;

    pAlert->uSeconds = pAlert->bRemoveAfter ?
                       pAlert->uSeconds : 1;

    m_sSecondsText.Format(_T("%d"), pAlert->uSeconds );
    m_sAlertText = pAlert->szAlertMsg;

    // Get the NT Alert server and checkbox setting. Use reasonable defaults
    // when the get fails.
	if (GetForwardTarget (szServer, &m_bNtAlert))
	{
		m_sNtAlertServer = szServer;
	}
	else
	{
		m_sNtAlertServer = _T("");
		m_bNtAlert       = FALSE;
	}

    // Get the NLM server and checkbox setting. Use reasonable defaults
    // when the get fails.
    if (GetNLMTarget(szServer, &m_bAlertNLM))
    {
        CString sAll;
        sAll.LoadString (IDS_NLM_ALERT_ALL);

        // If the target is set for ALL, set our internal string to "".
        if (0 == sAll.CompareNoCase (szServer))
            m_sNLMCombo = _T("");
        else
            m_sNLMCombo = szServer;
    }
    else
    {
        m_sNLMCombo = _T("");
        m_bAlertNLM = FALSE;
    }

    return TRUE;
}

BOOL CAlertsPage::GetData(LPNAVOPTIONS pOptions)
{
    ALERTOPT  *pAlert = &pOptions->alert;

    pAlert->bBeep = m_bBeep;
    pAlert->bDispAlertMsg = m_bDisplayAlert;
    pAlert->bRemoveAfter = m_bRemoveAfter;
    pAlert->bAlertNLM = m_bAlertNLM;
    pAlert->uSeconds = (BYTE) _ttoi((LPCTSTR) m_sSecondsText);
    _tcscpy(pAlert->szAlertMsg, (LPCTSTR)m_sAlertText);

    //
    // If Remove after x seconds chkbox is checked and
    // seconds is 0, then we uncheck it, in both these
    // cases the alert is displayed indefinitely
    //
    if ( pAlert->bRemoveAfter && pAlert->uSeconds == 0)
        pAlert->bRemoveAfter = FALSE;

    return TRUE;
}

BOOL CAlertsPage::OnInitDialog()
{
	CHelperPage::OnInitDialog();

    // Set icon for browse button.
    m_BrowseBtn.SetIconID(IDI_NTALERT_BROWSE);

	// Set up controls.
    if(!IS_NT())
        m_Spin.SetRange(1, 99);

    // Set enable state of controls.
    OnDisplayCheck();
    if(!IS_NT())
        OnRemoveCheck();
	OnNtAlertCheck();

	// Align these controls - present only in NAV95
	// This will help make the UI look alligned in English 
	// as well as international languages
    if(!IS_NT())
	{
		AlignWndResize (GetDlgItem(IDC_REMOVE_CHECK)->m_hWnd);
		AlignWnd (GetDlgItem(IDC_SECONDS_EDIT)->m_hWnd,
				  GetDlgItem(IDC_REMOVE_CHECK)->m_hWnd,
				  gAlignWndFollowX, 
				  NULL, 1 /*padding*/);
		AlignWnd (GetDlgItem(IDC_SECONDS_SPIN)->m_hWnd,
				  GetDlgItem(IDC_SECONDS_EDIT)->m_hWnd,
				  gAlignWndFollowX, 
				  NULL, -5 /*padding*/);
		AlignWnd (GetDlgItem(IDC_STATIC_SECONDS)->m_hWnd,
				  GetDlgItem(IDC_SECONDS_SPIN)->m_hWnd,
				  gAlignWndFollowX, 
				  NULL, 2 /*padding*/);
	}

    // Set up NLM Combo box.
	InitNLMCombo();
    OnNlmCheck();


	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CAlertsPage::OnDisplayCheck()
{
	BOOL bEnable = IsDlgButtonChecked(IDC_DISPLAY_CHECK);

    GetDlgItem(IDC_MESSAGE_EDIT)->EnableWindow(bEnable);

}

void CAlertsPage::OnRemoveCheck()
{
    BOOL bEnable = IsDlgButtonChecked(IDC_REMOVE_CHECK);

    GetDlgItem(IDC_SECONDS_EDIT)->EnableWindow(bEnable);
    m_Spin.EnableWindow(bEnable);

}

void CAlertsPage::OnNlmCheck()
{
    BOOL bEnable = IsDlgButtonChecked(IDC_NLM_CHECK);

    GetDlgItem(IDC_NLM_COMBO)->EnableWindow(bEnable);

}

void CAlertsPage::InitNLMCombo()
{
	CComboBox * pCombo = (CComboBox*)GetDlgItem(IDC_NLM_COMBO);
	ASSERT(pCombo);

	// If no server configured, set "All Servers."
	if (m_sNLMCombo.IsEmpty())
		{
		pCombo->SetCurSel(0);
		return;
		}

	// If the server is not already in the list, add it to the combo box.
    if (CB_ERR == pCombo->FindString (-1, m_sNLMCombo))
    {
	    int iIndex = pCombo->AddString(m_sNLMCombo);
        pCombo->SetCurSel(iIndex);
    }
}

void CAlertsPage::OnDropdownNlmCombo()
{
	HCURSOR hWait, hArrow;

	// Hourglass for long operation.
    hWait = LoadCursor (NULL, IDC_WAIT);
    hArrow = SetCursor (hWait);

	// Save off current selection.
	CComboBox * pCombo = (CComboBox*)GetDlgItem(IDC_NLM_COMBO);
	ASSERT(pCombo);

	int iIndex = pCombo->GetCurSel();
	CString sCurrentServer;
	pCombo->GetLBText(iIndex, sCurrentServer);

	// Build list of servers
    EnumNAVNLMServers( NLMServerEnumProc,   // Our callback function
                       FALSE,               // We don't want addresses
                       (LPNETRESOURCE) NULL,// Always start at NULL/beginning
                       (LPVOID) pCombo);

	// Select the current server.
	iIndex = pCombo->FindString(-1, sCurrentServer);
	if(iIndex!= CB_ERR)
		{
		pCombo->SetCurSel(iIndex);
		}

	// Restore cursor.
	SetCursor(hArrow);
}

void CAlertsPage::OnOK()
{
	CComboBox *pCombo = (CComboBox*)GetDlgItem(IDC_NLM_COMBO);

	// Transfer data from controls to member variables.
    UpdateData();

	// Set the NT Alert Server.
	SetForwardTarget (m_sNtAlertServer, m_bNtAlert);

    // The NLM target for alerting all NLMs is special. Load
    // that string from our resources.
    ASSERT (pCombo);
    if (0 == pCombo->GetCurSel ())
    {
        CString sAll;
        sAll.LoadString (IDS_NLM_ALERT_ALL);
        SetNLMTarget (sAll, m_bAlertNLM);
    }
    else
    {
        SetNLMTarget(m_sNLMCombo, m_bAlertNLM);
    }

    CHelperPage::OnOK();
}

//************************************************************************
// BOOL NLMServerEnumProc (
//
// Purpose:
//   This is the callback for the enumeration function.
//   Adds all NAV NLM servers found to the listbox.
//
// Parameters:
//   LPSTR  lpszServerName - [in] - Server to be added
//   LPVOID lpAddress      - [in] - Optional address of server.
//   LPVOID lpExtra        - [in] - extra data (passthrough)
//
// Returns:
//   BOOL - TRUE  - Continue enumerating
//        - FALSE - STOP enumerating
//
// See Also:
//************************************************************************
// 05-18-96 - DBUCHES - Stolen and modified :)
// 12-12-96 - DSACKIN - Function Created.
//************************************************************************

static BOOL NLMServerEnumProc (
    LPSTR  lpszServerName,
    LPVOID lpAddress,
    LPVOID lpExtra)
{
    CComboBox* pCombo = (CComboBox*) lpExtra;
	auto    LPSTR   lpszTempServerName;

    ASSERT (pCombo);

    lpszTempServerName = lpszServerName;

                                        // Get rid of those backslashes!
    while ('\\' == *lpszTempServerName)
        lpszTempServerName++;

                                        // Only add server names if they aren't
                                        // already in the list
    if ( pCombo->FindString( -1, lpszTempServerName ) == CB_ERR )
        {
                                        // OK.  Its not there, we must add it.
        pCombo->AddString( lpszTempServerName );
        }

                                        // Always Keep going until we are done
    return ( TRUE );
}

void CAlertsPage::OnNtAlertCheck() 
{
    BOOL bEnable = IsDlgButtonChecked(IDC_NTALERT_CHECK);

    GetDlgItem(IDC_NTALERT_EDIT)->EnableWindow(bEnable);
    GetDlgItem(IDC_NTALERT_BROWSE)->EnableWindow(bEnable);
}

void CAlertsPage::OnNtAlertBrowse() 
{
    const   DWORD    dwResTypeExclude = (RESOURCE_TYPE_NETWORK_NETWARE);
    const   DWORD    dwDisplayResType = (RESOURCE_TYPE_ALL & (~dwResTypeExclude));
    auto    int      result           = TRUE;
    auto    DWORD    dwResourceType;

    // if we're in "modify" mode, initialize the net browser's field
    // with the record's current address string
    auto    CNetBrowsDlg    BrowseDlg(BROWSER_TYPE_FORWARD, this);

	if (!m_sNtAlertServer.IsEmpty())
	{
		BrowseDlg.SetInitialBrowseItemName(m_sNtAlertServer);
    }
    BrowseDlg.SetDisplayItemFlags (dwDisplayResType);
    
    BrowseDlg.SetHelpFileInfo( IDH_NAVNT_NETWORK_BROWSER_DLG_HELP_BTN, IS_NT() ? _T("NAVWNT.HLP") : _T("NAVW32.HLP") );
                                                   
    // browse...
    result = BrowseDlg.DoModal();
    if (IDOK == result)
    {
        // Move data from controls to member variables.
    	UpdateData();
    
        m_sNtAlertServer = BrowseDlg.GetSelectedItemName();
        dwResourceType   = BrowseDlg.GetSelectedItemType();

        switch(dwResourceType)
        {
            case  RESOURCE_TYPE_SERVER:
                // remove the two leading backslashes before writing to
                // the data file
                m_sNtAlertServer = 
					m_sNtAlertServer.Right(m_sNtAlertServer.GetLength() - 2);
                break;

            default:
                break;
        }

		// Update the displayed server name.
		UpdateData (FALSE);
    }
}

BOOL CAlertsPage::OnKillActive()
{
    BOOL   bUpdateControls = FALSE;

    UpdateData();

    m_sNtAlertServer.TrimLeft();
    m_sNtAlertServer.TrimRight();

    // remove the two leading backslashes before writing to
    // the data file.
    if (0 == m_sNtAlertServer.Left(2).Compare (_T("\\\\")))
    {
        bUpdateControls  = TRUE;
        m_sNtAlertServer = 
            m_sNtAlertServer.Right(m_sNtAlertServer.GetLength() - 2);
    }

	// Transfer data from member variables to controls.
    UpdateData (FALSE);

    return CHelperPage::OnKillActive();
}
