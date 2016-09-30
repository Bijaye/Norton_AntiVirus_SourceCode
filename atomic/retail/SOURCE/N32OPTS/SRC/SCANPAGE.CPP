/////////////////////////////////////////////////////////////////////////////
// $Header:   S:/N32OPTS/VCS/SCANPAGE.CPv   1.32   13 Jul 1998 12:17:50   DBuches  $
/////////////////////////////////////////////////////////////////////////////
//
// ScanPage.cpp : implementation file
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
// $Log:   S:/N32OPTS/VCS/SCANPAGE.CPv  $
// 
//    Rev 1.32   13 Jul 1998 12:17:50   DBuches
// Removed unused help topic.
// 
//    Rev 1.31   26 Jun 1998 17:39:18   mdunn
// Now using navwhlp5.h for help IDs.
// 
//    Rev 1.30   25 Jun 1998 17:18:28   jtaylor
// Enhanced the pages so the Quarantine checkboxes would not get re-enabled on non Quarantine systems.
//
//    Rev 1.29   24 Jun 1998 18:27:10   jtaylor
// Changed all the removals of Quarantine Checkboxes to dissables.
//
//    Rev 1.28   24 Jun 1998 17:54:46   jtaylor
// Removed more Quarantine Controls when Quarantine not installed.  Uncheck Qurantine checkboxes.  Removed unused first window parameter on AlignWindow calls.
//
//    Rev 1.27   24 Jun 1998 16:48:52   jtaylor
// Hid the Quarantine options from the scanner drop down box and the Quarantine check box on non Quarantine installs.
//
//    Rev 1.26   01 Jun 1998 16:35:18   DBuches
// Fixed problem with scan page drop down.
//
//    Rev 1.25   28 May 1998 16:36:38   DBuches
// Removed old reference to continue check box.
//
//    Rev 1.24   28 May 1998 14:39:24   DBuches
// Removed continue button.
//
//    Rev 1.23   21 May 1998 15:34:02   tcashin
// Added what's this help for the quarantine checkbox.
//
//    Rev 1.22   30 Apr 1998 01:56:00   tcashin
// Fixed a problem in the "How to respond" list box (#113767).
//
//    Rev 1.21   19 Apr 1998 15:05:38   tcashin
// Added Quarantine to "Buttons to display when prompted".
//
//    Rev 1.20   14 Apr 1998 17:30:06   tcashin
// Temporarily backed out changes for the quarantine button.
//
//    Rev 1.17   03 Apr 1998 13:24:46   MKEATIN
// Removed File Inoculation, Immediate Notify, and Sensor.
//
//    Rev 1.16   30 Mar 1998 20:49:44   tcashin
// Set new quarantine options.
//
//    Rev 1.15   19 Feb 1998 11:57:14   mdunn
// Ported NEC changes from ROSNEC.
//
//    Rev 1.1   23 Sep 1997 15:57:40   mdunn
// Now hiding the MBR checkbox & shifting the others up.
//
//    Rev 1.0   12 Sep 1997 17:44:50   RFULLER
// Initial revision
//
//    Rev 1.14   16 Jul 1997 16:54:46   DBUCHES
// Remove compression checkbox for DEC Alpha platform
//
//    Rev 1.13   25 Jun 1997 11:54:48   DBUCHES
// Hooked up more help topics.
//
//    Rev 1.12   24 Jun 1997 15:57:48   DBUCHES
// Enabled action buttons on custom response.
//
//    Rev 1.11   20 Jun 1997 12:03:58   DBUCHES
// Added help support.
//
//    Rev 1.10   16 Jun 1997 15:36:38   DBUCHES
// NT specific changes.
//
//    Rev 1.9   15 Jun 1997 20:06:36   DBUCHES
// Added support for saving Bloodhound options.
//
//    Rev 1.8   13 Jun 1997 11:55:02   DBUCHES
// Added Bloodhound dialog.
//
//    Rev 1.7   10 Jun 1997 15:20:34   DBUCHES
// Removed quarantine options.
//
//    Rev 1.6   04 Jun 1997 22:13:26   DBUCHES
// Added code to disable checkboxes.
//
//    Rev 1.5   27 May 1997 20:57:06   DBUCHES
// I touched 30 files this rev.  Needed for support of NT Alerting.
//
//    Rev 1.4   23 May 1997 15:57:30   DBUCHES
// Added setting of extra Quarantine bits.
//
//    Rev 1.3   21 May 1997 14:36:38   DBUCHES
// Added custom responst dialog.
//
//    Rev 1.2   18 May 1997 23:24:34   DBUCHES
// Now, use global copy of CIncludeDlg.
//
//    Rev 1.1   16 May 1997 18:54:32   DBUCHES
// Fixed problem with GetData() Member.  Went cut-paste crazy
// and paid the price...
//
//    Rev 1.0   14 May 1997 14:58:20   DBUCHES
// Initial revision.
/////////////////////////////////////////////////////////////////////////////
//

#include "stdafx.h"
#include "n32opts.h"
#include "ScanPage.h"
#include "sheet.h"
#include "navwhlp5.h"
#include "alignwnd.h"
#include "n32behav.h" // Personality DLL

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


/////////////////////////////////////////////////////////////////////////////
// Set up help maps
/////////////////////////////////////////////////////////////////////////////
DWORD CScannerPage::m_dwWhatsThisMap[] = {
    IDC_MEMORY_CHECK,       IDH_NAVW_WHATHIS_SCANNER_WHAT_TO_SCAN_MEMORY,
    IDC_MBR_CHECK,          IDH_NAVW_WHATHIS_SCANNER_WHAT_TO_SCAN_MASTER_BOOT_RECORD,
    IDC_BOOT_CHECK,         IDH_NAVW_WHATHIS_SCANNER_WHAT_TO_SCAN_BOOT_RECORDS,
    IDC_ALL_FILES_RADIO,    IDH_NAVW_WHATHIS_SCANNER_WHAT_TO_SCAN_ALL_FILES_BTN,
    IDC_PROGRAM_RADIO,      IDH_NAVW_WHATHIS_SCANNER_WHAT_TO_SCAN_PROGRAM_FILES_ONLY_BTN,
    IDC_SELECT_BUTTON,      IDH_NAVW_WHATHIS_SCANNER_WHAT_TO_SCAN_PROGRAM_FILES_BTN,
    IDC_COMPRESSED_CHECK,   IDH_NAVW_WHATHIS_SCANNER_WHAT_TO_SCAN_WITHIN_COMPRESSED_FILES,
    IDC_REPAIR_CHECK,       IDH_NAVW_WHATHIS_SCAN_BTN_REPAIR,
    IDC_DELETE_CHECK,       IDH_NAVW_WHATHIS_SCAN_BTN_DELETE,
    IDC_EXCLUDE_CHECK,      IDH_NAVW_WHATHIS_SCAN_BTN_EXCLUDE,
    IDC_ADVANCED_BUTTON,    IDH_NAVW_WHATHIS_SCANNER_ADVANCED_BTN,
    IDC_SCANNER_BUTTON,     IDH_NAVW_WHATHIS_SCANNER_SCANNER_BTN,
    IDC_CUSTOM_BUTTON,      IDH_NAVW_WHATHIS_SCANNER_CUSTOMIZE_BTN,
    IDC_PROMPT_COMBO,       IDH_NAVW_WHATHIS_AP_WHEN_A_VIRUS_IS_FOUND_LSTBOX,
    IDC_QUARANTINE_CHECK,   IDH_NAVW_WHATHIS_OPTIONS_SCANNER_QUARAN_CHKBX,
    0,0
    };
DWORD CScannerPage::m_dwHowToMap[] = { 0,0 };

DWORD CScanAdvDlg::m_dwWhatsThisMap[] = {
    IDC_NETWORK_CHECK,      IDH_NAVW_WHATHIS_ALLOW_NETWORK_SCANNING_CHKBOX,
    IDC_STOP_CHECK,         IDH_NAVW_WHATHIS_ALLOW_SCANNING_TO_BE_STOPPED_CHKBOX,
    IDC_START_FLOPPY_CHECK, IDH_NAVW_WHATHIS_ALL_FLOPPY_DRIVES_CHKBOX,
    IDC_START_HARD_CHECK,   IDH_NAVW_WHATHIS_ALL_HARD_DRIVES_CHKBOX,
    IDC_START_NETWORK_CHECK,IDH_NAVW_WHATHIS_ALL_NETWORK_DRIVES_CHKBOX,
    0,0
    };
DWORD CScanAdvDlg::m_dwHowToMap[] = { 0,0 };



/////////////////////////////////////////////////////////////////////////////
// CScannerPage property page

IMPLEMENT_DYNCREATE(CScannerPage, CPropertyPage)

CScannerPage::CScannerPage() : CHelperPage(IS_NT() ? IDD_SCANNER_PAGENT : CScannerPage::IDD)
{
	//{{AFX_DATA_INIT(CScannerPage)
	m_AllFiles = 0;
	m_bBootRecs = FALSE;
	m_bMBRCheck = FALSE;
	m_bMemory = FALSE;
	m_bContinue = FALSE;
	m_bDelete = FALSE;
	m_bExclude = FALSE;
    m_iAction = 0;
	m_bRepair = FALSE;
	m_bCompressed = FALSE;
	m_bQuarantine = FALSE;
	//}}AFX_DATA_INIT

    // Set up help maps.
    m_pHelpHowToMap = m_dwWhatsThisMap;
    m_pHelpWhatsThisMap = m_dwWhatsThisMap;
    m_dwHelpButtonHelpID = IDH_NAVW_SCANNER_SETTINGS_DLG_HELP_BTN;

    // Set up Password ID
    m_iPasswordFeature = PASSWORD_VIEWSCANOPT;

}

CScannerPage::~CScannerPage()
{
}

void CScannerPage::DoDataExchange(CDataExchange* pDX)
{
	CHelperPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CScannerPage)
	DDX_Radio(pDX, IDC_ALL_FILES_RADIO, m_AllFiles);
	DDX_Check(pDX, IDC_BOOT_CHECK, m_bBootRecs);
	DDX_Check(pDX, IDC_MBR_CHECK, m_bMBRCheck);
	DDX_Check(pDX, IDC_DELETE_CHECK, m_bDelete);
	DDX_Check(pDX, IDC_EXCLUDE_CHECK, m_bExclude);
	DDX_CBIndex(pDX, IDC_PROMPT_COMBO, m_iAction);
	DDX_Check(pDX, IDC_REPAIR_CHECK, m_bRepair);
	DDX_Check(pDX, IDC_COMPRESSED_CHECK, m_bCompressed);
	DDX_Check(pDX, IDC_QUARANTINE_CHECK, m_bQuarantine);
	//}}AFX_DATA_MAP

    // Memory scanning is un-supported under NT
    if(!IS_NT())
        DDX_Check(pDX, IDC_MEMORY_CHECK, m_bMemory);
}


BEGIN_MESSAGE_MAP(CScannerPage, CHelperPage)
	//{{AFX_MSG_MAP(CScannerPage)
	ON_BN_CLICKED(IDC_ADVANCED_BUTTON, OnAdvancedButton)
	ON_BN_CLICKED(IDC_SELECT_BUTTON, OnSelectButton)
	ON_BN_CLICKED(IDC_ALL_FILES_RADIO, OnAllFilesRadio)
	ON_CBN_SELCHANGE(IDC_PROMPT_COMBO, OnSelchangePromptCombo)
	ON_BN_CLICKED(IDC_CUSTOM_BUTTON, OnCustomButton)
	ON_BN_CLICKED(IDC_PROGRAM_RADIO, OnAllFilesRadio)
	ON_BN_CLICKED(IDC_SCANNER_BUTTON, OnScannerButton)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()



/////////////////////////////////////////////////////////////////////////////
// CScannerPage message handlers

void CScannerPage::OnAdvancedButton()
{
    // Fire off advanced dialog.
    m_AdvDlg.DoModal();
}

void CScannerPage::OnSelectButton()
{
    CNavOptionsSheet* pParent = (CNavOptionsSheet*)GetParent();
	ASSERT(pParent);

	// Invoke global extension dialog
	pParent->m_IncludeDlg.DoModal();
}

void CScannerPage::OnAllFilesRadio()
{
    UpdateData();

    GetDlgItem(IDC_SELECT_BUTTON)->EnableWindow(m_AllFiles);
}

BOOL CScannerPage::OnInitDialog()
{
	CHelperPage::OnInitDialog();

	// TODO: Add extra initialization here

    // Update state of select button.
    OnAllFilesRadio();

    // Remove extra combo items for NT
    if(IS_NT())
        {
        CComboBox* pCombo = (CComboBox*)GetDlgItem(IDC_PROMPT_COMBO);

        // Remove 8th element from combo box.  There is
        // no ShutDown Cpu option for NT machines.
        pCombo->DeleteString(7);
        }

    // Set state of combo
    OnSelchangePromptCombo();

    // If this installation of NAV does not have Quarantine installed
    // then remove Qurantine from the controls.
    if( !NAV_IS_QUARANTINE() )
        {
        GetDlgItem(IDC_QUARANTINE_CHECK)->EnableWindow( FALSE );
        GetDlgItem(IDC_QUARANTINE_CHECK)->SendMessage( BM_SETCHECK, BST_UNCHECKED);
        CComboBox* pCombo = (CComboBox*)GetDlgItem(IDC_PROMPT_COMBO);

        // Remove the two Quarantine Specific items from the list.
        pCombo->DeleteString(6);
        pCombo->DeleteString(5);
        }


    // Hide compressed check for DEC Alpha platform
#ifdef _M_ALPHA
    GetDlgItem(IDC_COMPRESSED_CHECK)->ShowWindow(SW_HIDE);
#endif

    if ( IS_NEC() )
        {
        HWND hMBRCheck, hBRCheck, hZipsCheck;

        // Hide the MBR checkbox, and move the BR and zips checkboxes
        // up to fill the space.

        hMBRCheck = GetDlgItem ( IDC_MBR_CHECK ) -> GetSafeHwnd();
        hBRCheck = GetDlgItem ( IDC_BOOT_CHECK ) -> GetSafeHwnd();
        hZipsCheck = GetDlgItem ( IDC_COMPRESSED_CHECK ) -> GetSafeHwnd();

        AlignWnd ( hZipsCheck, hBRCheck, gAlignWndBottom, NULL, 0 );
        AlignWnd ( hBRCheck, hMBRCheck, gAlignWndBottom, NULL, 0 );

        GetDlgItem ( IDC_MBR_CHECK ) -> ShowWindow ( SW_HIDE );
        }

    return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CScannerPage::OnSelchangePromptCombo()
{
	UpdateData();
    BOOL bEnable = FALSE;

    // Only enable if on custom response item.
    if( m_iAction == 4 )
        {
        GetDlgItem(IDC_CUSTOM_BUTTON)->EnableWindow(TRUE);
        }
    else
        GetDlgItem(IDC_CUSTOM_BUTTON)->EnableWindow(FALSE);

    // Enable prompt checks if prompt is selected.
    if( m_iAction == 0 || m_iAction == 4 )
        {
        bEnable = TRUE;
        }

    GetDlgItem(IDC_REPAIR_CHECK)->EnableWindow(bEnable);
    GetDlgItem(IDC_DELETE_CHECK)->EnableWindow(bEnable);
    GetDlgItem(IDC_EXCLUDE_CHECK)->EnableWindow(bEnable);

    // Only enable the Quarantine checkbox if Quarantine is installed.
    if( NAV_IS_QUARANTINE() )
        {
        GetDlgItem(IDC_QUARANTINE_CHECK)->EnableWindow(bEnable);
        }

}


void CScannerPage::OnCustomButton()
{
    // Fire off custom response config dialog.
    m_CustDlg.DoModal();
}


void CScannerPage::OnScannerButton()
{
    // Fire off Bloodhound config dialog.
    m_BloodhoundDlg.DoModal();
}


BOOL CScannerPage::SetData(LPNAVOPTIONS pOptions)
{

    LPSCANOPT pScanOpts = &pOptions->scanner;

    // Set options for scanner page
    m_AllFiles =  (int) !pScanOpts->bScanAll;
    m_bBootRecs = (BOOL) pScanOpts->bBootRecs;
    m_bMBRCheck = (BOOL) pScanOpts->bMasterBootRec;
    m_bMemory =   (BOOL) pScanOpts->bMemory;
    m_bCompressed = pOptions->general.bScanZipFiles;
    m_bRepair =   (BOOL) pScanOpts->bDispRepair;
    m_bDelete =   (BOOL) pScanOpts->bDispDelete;
    m_bContinue = (BOOL) pScanOpts->bDispCont;
    m_bExclude =  (BOOL) pScanOpts->bDispExcl;
    m_bQuarantine = (BOOL) pScanOpts->bDispQuarantine;


    switch( pScanOpts->uAction )
        {
        case SCAN_PROMPT:           m_iAction = 0; break;
        case SCAN_REPORT:           m_iAction = 1; break;
        case SCAN_REPAUTO:          m_iAction = 2; break;
        case SCAN_DELAUTO:          m_iAction = 3; break;
        case SCAN_CUSTOM_RESPONSE:  m_iAction = 4; break;
        case SCAN_QUARAUTO:         m_iAction = 5; break;
        case SCAN_QUAR_CANT_REP:    m_iAction = 6; break;
        case SCAN_HALTCPU:          m_iAction = 7; break;
        }

    // Set options for advanced dialog
    m_AdvDlg.m_bShowNetwork =   (BOOL) pScanOpts->bAllowNetScan;
    m_AdvDlg.m_bAllowStop =     (BOOL) pScanOpts->bAllowScanStop;
    m_AdvDlg.m_bStartFloppy =   (BOOL) pScanOpts->bPresFlop;
    m_AdvDlg.m_bStartHard =     (BOOL) pScanOpts->bPresHard;
    m_AdvDlg.m_bStartNet =      (BOOL) pScanOpts->bPresNet;

    // Set up custom response actions.
    m_CustDlg.m_iFile = pOptions->customScan.uchFile;
    m_CustDlg.m_iMacro = pOptions->customScan.uchMacro;
    m_CustDlg.m_iBoot = pOptions->customScan.uchBoot;
    m_CustDlg.m_bAP = FALSE;

    // Set up heuristic configuration dialog.
    m_BloodhoundDlg.m_bEnabled = pScanOpts->bEnableHeuristicScan;
    m_BloodhoundDlg.m_iProtectionLevel = (int)pScanOpts->uHeuristicLevel;

    return TRUE;
}

BOOL CScannerPage::GetData(LPNAVOPTIONS pOptions)
{
    LPSCANOPT pScanOpts = &pOptions->scanner;

    // Set options for scanner page
    pScanOpts->bScanAll = !m_AllFiles;
    pScanOpts->bBootRecs = m_bBootRecs;
    pScanOpts->bMasterBootRec = m_bMBRCheck;
    pScanOpts->bMemory = m_bMemory;
    pScanOpts->bDispRepair = m_bRepair;
    pScanOpts->bDispDelete =m_bDelete;
    pScanOpts->bDispCont = m_bContinue;
    pScanOpts->bDispExcl = m_bExclude;
    pScanOpts->bDispQuarantine = m_bQuarantine;
    pOptions->general.bScanZipFiles = m_bCompressed;

    switch( m_iAction )
        {
        case 0: pScanOpts->uAction = SCAN_PROMPT;           break;
        case 1: pScanOpts->uAction = SCAN_REPORT;           break;
        case 2: pScanOpts->uAction = SCAN_REPAUTO;          break;
        case 3: pScanOpts->uAction = SCAN_DELAUTO;          break;
        case 4: pScanOpts->uAction = SCAN_CUSTOM_RESPONSE;  break;
        case 5: pScanOpts->uAction = SCAN_QUARAUTO;         break;
        case 6: pScanOpts->uAction = SCAN_QUAR_CANT_REP;    break;
        case 7: pScanOpts->uAction = SCAN_HALTCPU;          break;
        }

    // Set options for advanced dialog
    pScanOpts->bAllowNetScan = m_AdvDlg.m_bShowNetwork;
    pScanOpts->bAllowScanStop = m_AdvDlg.m_bAllowStop;
    pScanOpts->bPresFlop = m_AdvDlg.m_bStartFloppy;
    pScanOpts->bPresHard = m_AdvDlg.m_bStartHard;
    pScanOpts->bPresNet = m_AdvDlg.m_bStartNet;

    // Get custom response actions.
    pOptions->customScan.bSelected = pScanOpts->uAction == SCAN_CUSTOM_RESPONSE ? TRUE : FALSE;
    pOptions->customScan.uchFile = (BYTE) m_CustDlg.m_iFile;
    pOptions->customScan.uchMacro = (BYTE) m_CustDlg.m_iMacro;
    pOptions->customScan.uchBoot = (BYTE) m_CustDlg.m_iBoot;


    // Save off heuristic settings
    pScanOpts->bEnableHeuristicScan = (BYTE) m_BloodhoundDlg.m_bEnabled;
    pScanOpts->uHeuristicLevel = (BYTE) m_BloodhoundDlg.m_iProtectionLevel;


    return TRUE;
}




/////////////////////////////////////////////////////////////////////////////
// CScanAdvDlg dialog

CScanAdvDlg::CScanAdvDlg(CWnd* pParent /*=NULL*/)
	: CHelperDialog(CScanAdvDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CScanAdvDlg)
	m_bShowNetwork = FALSE;
	m_bStartFloppy = FALSE;
	m_bStartHard = FALSE;
	m_bStartNet = FALSE;
	m_bAllowStop = FALSE;
	//}}AFX_DATA_INIT
    m_pHelpHowToMap = m_dwWhatsThisMap;
    m_pHelpWhatsThisMap = m_dwWhatsThisMap;
    m_dwHelpButtonHelpID = IDH_NAVW_SCANNER_ADVANCED_SETTINGS_DLG_HELP_BTN;
}


void CScanAdvDlg::DoDataExchange(CDataExchange* pDX)
{
	CHelperDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CScanAdvDlg)
	DDX_Check(pDX, IDC_NETWORK_CHECK, m_bShowNetwork);
	DDX_Check(pDX, IDC_START_FLOPPY_CHECK, m_bStartFloppy);
	DDX_Check(pDX, IDC_START_HARD_CHECK, m_bStartHard);
	DDX_Check(pDX, IDC_START_NETWORK_CHECK, m_bStartNet);
	DDX_Check(pDX, IDC_STOP_CHECK, m_bAllowStop);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CScanAdvDlg, CHelperDialog)
	//{{AFX_MSG_MAP(CScanAdvDlg)
	ON_BN_CLICKED(IDC_NETWORK_CHECK, OnNetworkCheck)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CScanAdvDlg message handlers


BOOL CScanAdvDlg::OnInitDialog()
{
	CHelperDialog::OnInitDialog();

	// Enable Preselect network drive control.
    GetDlgItem(IDC_START_NETWORK_CHECK)->EnableWindow(m_bShowNetwork);

    return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CScanAdvDlg::OnNetworkCheck()
{
    // Only enable if this control is checked
    BOOL bEnabled = (BOOL) (IsDlgButtonChecked(IDC_NETWORK_CHECK) != 0);

    // Enable the control.
    GetDlgItem(IDC_START_NETWORK_CHECK)->EnableWindow(bEnabled);
}




