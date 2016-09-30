/////////////////////////////////////////////////////////////////////////////
// $Header:   S:/N32OPTS/VCS/appage.cpv   1.28   01 Jul 1998 12:07:12   DBuches  $
/////////////////////////////////////////////////////////////////////////////
//
// APPAGE.CPP - Contains class impelmentaion for CAutoProtect class and
//              various helper classes
//
//
/////////////////////////////////////////////////////////////////////////////
// $Log:   S:/N32OPTS/VCS/appage.cpv  $
// 
//    Rev 1.28   01 Jul 1998 12:07:12   DBuches
// Added more help topics.
// 
//    Rev 1.27   26 Jun 1998 17:39:16   mdunn
// Now using navwhlp5.h for help IDs.
// 
//    Rev 1.26   25 Jun 1998 17:18:28   jtaylor
// Enhanced the pages so the Quarantine checkboxes would not get re-enabled on non Quarantine systems.
//
//    Rev 1.25   24 Jun 1998 18:27:10   jtaylor
// Changed all the removals of Quarantine Checkboxes to dissables.
//
//    Rev 1.24   24 Jun 1998 17:54:46   jtaylor
// Removed more Quarantine Controls when Quarantine not installed.  Uncheck Qurantine checkboxes.  Removed unused first window parameter on AlignWindow calls.
//
//    Rev 1.23   23 Apr 1998 15:22:06   MKEATIN
// Added the NT AP-Hueristics button.
//
//    Rev 1.22   19 Apr 1998 15:05:06   tcashin
// Added Quarantine to "Buttons to display when prompted".
//
//    Rev 1.21   15 Apr 1998 17:36:54   MKEATIN
// Added heuristics dialog to the AP pannel.
//
//    Rev 1.20   01 Apr 1998 16:41:34   DALLEE
// Added AP option for Quarantine if can't repair.
//
//    Rev 1.19   30 Mar 1998 21:22:28   tcashin
// Set new Quarantine options.
//
//    Rev 1.18   19 Feb 1998 11:57:10   mdunn
// Ported NEC changes from ROSNEC.
//
//    Rev 1.17   18 Nov 1997 20:48:34   jtaylor
// Removed an unused include file (n32behav.h).
//
//    Rev 1.16   18 Nov 1997 20:43:52   jtaylor
// Removed scanonly messagebox.
//
//    Rev 1.15   17 Nov 1997 15:29:36   mdunn
// Added support for the "Prompt if logged in" option.
//
//    Rev 1.14   10 Nov 1997 18:06:36   jtaylor
// Added a MessageBox to the first initialize of the panel when the NAV_IS_SCANONLY() macro from the behavior DLL returns true.
//
//    Rev 1.13   25 Jul 1997 12:11:24   DBUCHES
// Added Whats This for Load AP checkbox.
//
//    Rev 1.12   25 Jun 1997 11:54:34   DBUCHES
// Hooked up more help topics.
//
//    Rev 1.11   24 Jun 1997 15:57:44   DBUCHES
// Enabled action buttons on custom response.
//
//    Rev 1.10   20 Jun 1997 12:00:54   DBUCHES
// Added Help support.
//
//    Rev 1.9   17 Jun 1997 12:53:58   DBUCHES
// Removed old reference to quarantine functionality.
//
//    Rev 1.8   10 Jun 1997 15:20:30   DBUCHES
// Removed quarantine options.
//
//    Rev 1.7   04 Jun 1997 22:14:28   DBUCHES
// Added code to disable checkboxes.
//
//    Rev 1.6   30 May 1997 14:35:22   DBUCHES
// Update state of custom response button on InitDialog().
//
//    Rev 1.5   30 May 1997 14:30:34   DBUCHES
// Update state of custom response button on InitDialog().
//
//    Rev 1.4   27 May 1997 20:55:18   DBUCHES
// I touched 30 files this rev.  Needed for support of NT Alerting.
//
//    Rev 1.3   23 May 1997 15:57:28   DBUCHES
// Added setting of extra Quarantine bits.
//
//    Rev 1.2   21 May 1997 14:36:36   DBUCHES
// Added custom responst dialog.
//
//    Rev 1.1   18 May 1997 23:24:34   DBUCHES
// Now, use global copy of CIncludeDlg.
//
//    Rev 1.0   14 May 1997 14:58:10   DBUCHES
// Initial revision.
/////////////////////////////////////////////////////////////////////////////
// APPage.cpp : implementation file
//

#include "stdafx.h"
#include "n32opts.h"
#include "APPage.h"
#include "navwhlp5.h"
#include "sheet.h"
#include "n32behav.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


/////////////////////////////////////////////////////////////////////////////
// Set up help maps
/////////////////////////////////////////////////////////////////////////////
DWORD CAutoProtectPage::m_dwWhatsThisMap[] = {
    IDC_RUN_CHECK,          IDH_NAVW_WHATHIS_AP_RUN_CHKBOX,
    IDC_OPENED_CHECK,       IDH_NAVW_WHATHIS_AP_OPENED_CHKBOX,
    IDC_CREATED_CHECK,      IDH_NAVW_WHATHIS_AP_CREATED_CHKBOX,
    IDC_ALL_FILES_RADIO,    IDH_NAVW_WHATHIS_AP_WHAT_TO_SCAN_ALL_FILES_BTN,
    IDC_PROGRAM_RADIO,      IDH_NAVW_WHATHIS_AP_WHAT_TO_SCAN_PROGRAM_FILES_ONLY_BTN,
    IDC_SELECT_BUTTON,      IDH_NAVW_WHATHIS_AP_WHAT_TO_SCAN_PROGRAM_FILES_BTN,
    IDC_PROMPT_COMBO,       IDH_NAVW_WHATHIS_AP_WHEN_A_VIRUS_IS_FOUND_LSTBOX,
    IDC_REPAIR_CHECK,       IDH_NAVW_WHATHIS_AP_BTN_REPAIR,
    IDC_DELETE_CHECK,       IDH_NAVW_WHATHIS_AP_BTN_DELETE,
    IDC_CONTINUE_CHECK,     IDH_NAVW_WHATHIS_AP_BTN_CONTINUE,
    IDC_QUARANTINE_CHECK,   IDH_NAVW_WHATHIS_OPTIONS_SCANNER_QUARAN_CHKBX,
    IDC_SHOW_ICON_CHECK,    IDH_NAVW_WHATHIS_AP_SHOW_ICON_CHKBX,
    IDC_DISABLE_CHECK,      IDH_NAVW_WHATHIS_AP_CAN_BE_DISABLED_CHKBX,
    IDC_STOP_CHECK,         IDH_NAVW_WHATHIS_AP_BTN_STOP,
    IDC_EXCLUDE_CHECK,      IDH_NAVW_WHATHIS_AP_BTN_EXCLUDE,
    IDC_HEURISTIC_BUTTON,   IDH_NAVW_WHATHIS_SCANNER_SCANNER_BTN,
    IDC_ADVANCED_BUTTON,    IDH_NAVW_WHATHIS_AP_ADVANCED_BTN,
    IDC_FLOPPYACCESS_CHECK, IDH_NAVW_WHATHIS_AP_CHECK_FLOPPIES_FOR_BOOT_VIRUSES,
    IDC_FLOPPY_REBOOT_CHECK,IDH_NAVW_WHATHIS_AP_CHECK_FLOPPIES_WHEN_REBOOTING_COMPUTER,
    IDC_CUSTOM_BUTTON,      IDH_NAVW_WHATHIS_SCANNER_CUSTOMIZE_BTN,
    IDC_LOAD_CHECK,         IDH_NAVW_WHATHIS_STARTUP_LOAD_AUTO_PROTECT_CHKBOX,
    0,0
    };

DWORD CAutoProtectPage::m_dwHowToMap[] = { 0,0 };

DWORD CAdvApDlg::m_dwWhatsThisMap[] = {
    IDC_LOWLEVEL_COMBO,     IDH_NAVW_WHATHIS_AP_VLA_LOW_LEVEL_FORMAT,
    IDC_STATIC_LL,          IDH_NAVW_WHATHIS_AP_VLA_LOW_LEVEL_FORMAT,
    IDC_BOOTRECORD_COMBO,   IDH_NAVW_WHATHIS_AP_VLA_WRITE_TO_HARD_DISK,
    IDC_STATIC_WH,          IDH_NAVW_WHATHIS_AP_VLA_WRITE_TO_HARD_DISK,
    IDC_FLOPPYBR_COMBO,     IDH_NAVW_WHATHIS_AP_VLA_WRITE_TO_FLOPPY_DISK,
    IDC_STATIC_WF,          IDH_NAVW_WHATHIS_AP_VLA_WRITE_TO_FLOPPY_DISK,
    IDC_PROGRAM_COMBO,      IDH_NAVW_WHATHIS_AP_VLA_WRITE_TO_PROGRAM_FILES,
    IDC_STATIC_WP,          IDH_NAVW_WHATHIS_AP_VLA_WRITE_TO_PROGRAM_FILES,
    IDC_DOS_COMBO,          IDH_NAVW_WHATHIS_AP_VLA_READ_ONLY_ATTRIBUTE,
    IDC_STATIC_DOS,         IDH_NAVW_WHATHIS_AP_VLA_READ_ONLY_ATTRIBUTE,
    IDC_FLOPPYACCESS_CHECK, IDH_NAVW_WHATHIS_AP_CHECK_FLOPPIES_FOR_BOOT_VIRUSES,
    IDC_FLOPPY_REBOOT_CHECK,IDH_NAVW_WHATHIS_AP_CHECK_FLOPPIES_WHEN_REBOOTING_COMPUTER,
    IDC_FLOPPY_BOTH_CHECK,  IDH_NAVW_WHATHIS_AP_WHEN_REBOOTING_CHECK_BOTH_DRIVES,
    0,0
    };
DWORD CAdvApDlg::m_dwHowToMap[] = { 0,0 };

DWORD CAdvApDlgNec::m_dwWhatsThisMap[] = {
    IDC_LOWLEVEL_COMBO,     IDH_NAVW_WHATHIS_AP_VLA_LOW_LEVEL_FORMAT,
    IDC_STATIC_LL,          IDH_NAVW_WHATHIS_AP_VLA_LOW_LEVEL_FORMAT,
    IDC_BOOTRECORD_COMBO,   IDH_NAVW_WHATHIS_AP_VLA_WRITE_TO_HARD_DISK,
    IDC_STATIC_WH,          IDH_NAVW_WHATHIS_AP_VLA_WRITE_TO_HARD_DISK,
    IDC_FLOPPYBR_COMBO,     IDH_NAVW_WHATHIS_AP_VLA_WRITE_TO_FLOPPY_DISK,
    IDC_STATIC_WF,          IDH_NAVW_WHATHIS_AP_VLA_WRITE_TO_FLOPPY_DISK,
    IDC_PROGRAM_COMBO,      IDH_NAVW_WHATHIS_AP_VLA_WRITE_TO_PROGRAM_FILES,
    IDC_STATIC_WP,          IDH_NAVW_WHATHIS_AP_VLA_WRITE_TO_PROGRAM_FILES,
    IDC_DOS_COMBO,          IDH_NAVW_WHATHIS_AP_VLA_READ_ONLY_ATTRIBUTE,
    IDC_STATIC_DOS,         IDH_NAVW_WHATHIS_AP_VLA_READ_ONLY_ATTRIBUTE,
    IDC_FLOPPYACCESS_CHECK, IDH_NAVW_WHATHIS_AP_CHECK_FLOPPIES_FOR_BOOT_VIRUSES,
    IDC_FLOPPY_REBOOT_CHECK,IDH_NAVW_WHATHIS_AP_CHECK_FLOPPIES_WHEN_REBOOTING_COMPUTER,
    IDC_FLOPPY_BOTH_CHECK,  IDH_NAVW_WHATHIS_AP_WHEN_REBOOTING_CHECK_BOTH_DRIVES,
    0,0
    };
DWORD CAdvApDlgNec::m_dwHowToMap[] = { 0,0 };

/////////////////////////////////////////////////////////////////////////////
// CAutoProtectPage property page

IMPLEMENT_DYNCREATE(CAutoProtectPage, CHelperPage)

CAutoProtectPage::CAutoProtectPage() :
CHelperPage(IS_NT() ? IDD_AUTOPORTECT_NT_PAGE : CAutoProtectPage::IDD)
{
	//{{AFX_DATA_INIT(CAutoProtectPage)
	m_AllFiles = 0;
	m_bContinue = FALSE;
	m_bCreated = FALSE;
	m_bDelete = FALSE;
	m_bDisable = FALSE;
	m_bExclude = FALSE;
	m_bLoad = FALSE;
	m_bOpened = FALSE;
	m_iPrompt = -1;
	m_bRepair = FALSE;
	m_bRun = FALSE;
	m_bShowIcon = FALSE;
	m_bStop = FALSE;
	m_bQuarantine = FALSE;
	//}}AFX_DATA_INIT
    // Set up help maps.
    m_pHelpHowToMap = m_dwWhatsThisMap;
    m_pHelpWhatsThisMap = m_dwWhatsThisMap;
    // Set up Password ID
    m_iPasswordFeature = PASSWORD_VIEWTSROPT;
    m_dwHelpButtonHelpID = IDH_NAVW_AP_HELP_BTN;

}

CAutoProtectPage::~CAutoProtectPage()
{
}

void CAutoProtectPage::DoDataExchange(CDataExchange* pDX)
{
	CHelperPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAutoProtectPage)
	DDX_Radio(pDX, IDC_ALL_FILES_RADIO, m_AllFiles);
	DDX_Check(pDX, IDC_CREATED_CHECK, m_bCreated);
	DDX_Check(pDX, IDC_DISABLE_CHECK, m_bDisable);
	DDX_Check(pDX, IDC_LOAD_CHECK, m_bLoad);
	DDX_Check(pDX, IDC_OPENED_CHECK, m_bOpened);
	DDX_CBIndex(pDX, IDC_PROMPT_COMBO, m_iPrompt);
    DDX_Check(pDX, IDC_SHOW_ICON_CHECK, m_bShowIcon);
	//}}AFX_DATA_MAP

    // These controls only exist for non-NT
    if(!IS_NT())
        {
	    DDX_Check(pDX, IDC_RUN_CHECK, m_bRun);
        DDX_Check(pDX, IDC_CONTINUE_CHECK, m_bContinue);
        DDX_Check(pDX, IDC_EXCLUDE_CHECK, m_bExclude);
        DDX_Check(pDX, IDC_REPAIR_CHECK, m_bRepair);
        DDX_Check(pDX, IDC_STOP_CHECK, m_bStop);
        DDX_Check(pDX, IDC_DELETE_CHECK, m_bDelete);
    	DDX_Check(pDX, IDC_QUARANTINE_CHECK, m_bQuarantine);
        }
    else
        {
        DDX_Check(pDX, IDC_FLOPPYACCESS_CHECK, m_bFloppyAccess);
        DDX_Check(pDX, IDC_FLOPPY_REBOOT_CHECK, m_bFloppyReboot);
        }

}


BEGIN_MESSAGE_MAP(CAutoProtectPage, CHelperPage)
	//{{AFX_MSG_MAP(CAutoProtectPage)
	ON_BN_CLICKED(IDC_ALL_FILES_RADIO, OnAllFilesRadio)
	ON_BN_CLICKED(IDC_HEURISTIC_BUTTON, OnHeuristicButton)
	ON_BN_CLICKED(IDC_ADVANCED_BUTTON, OnAdvancedButton)
	ON_BN_CLICKED(IDC_SELECT_BUTTON, OnSelectButton)
	ON_CBN_SELCHANGE(IDC_PROMPT_COMBO, OnSelchangePromptCombo)
    ON_BN_CLICKED(IDC_PROGRAM_RADIO, OnAllFilesRadio)
	ON_BN_CLICKED(IDC_CUSTOM_BUTTON, OnCustomButton)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CAutoProtectPage message handlers

void CAutoProtectPage::OnAllFilesRadio()
{
    UpdateData();

    GetDlgItem(IDC_SELECT_BUTTON)->EnableWindow(m_AllFiles);

}

void CAutoProtectPage::OnHeuristicButton()
{
    // Fire off Bloodhound config dialog.
    m_BloodhoundDlg.DoModal();
}

void CAutoProtectPage::OnAdvancedButton()
{
    if ( IS_NEC() )
        {
        m_AdvancedDlgNec.DoModal();
        }
    else
        {
        m_AdvancedDlg.DoModal();
        }
}

BOOL CAutoProtectPage::OnInitDialog()
{
	CHelperPage::OnInitDialog();

    // Update state of controls.
    OnAllFilesRadio();
	OnSelchangePromptCombo();

    // On NECs, we need to move the "Load AP on startup" checkbox to the right
    // a bit since the descriptive text above it wraps to a 2nd line on
    // Japanese Windows.

    if ( IS_NEC() )
        {
        CWnd* pGroup = GetDlgItem ( IDC_WHAT_TO_SCAN_GROUP );
        CWnd* pCheckbox = GetDlgItem ( IDC_LOAD_CHECK );
        CRect rectGroup;
        CRect rectCheckbox;

        ASSERT ( pGroup != NULL  &&  pCheckbox != NULL );

        pGroup -> GetWindowRect ( rectGroup );
        pCheckbox -> GetWindowRect ( rectCheckbox );

        // Move the checkbox to the same X coordinate as the groupbox.

        rectCheckbox.left = rectGroup.left;

        // SetWindowPos() needs client coordinates...
        ScreenToClient ( rectCheckbox );

        pCheckbox -> SetWindowPos ( NULL, rectCheckbox.left, rectCheckbox.top,
                                    0, 0, SWP_NOZORDER | SWP_NOSIZE );
        }

    // If this installation of NAV does not have Quarantine installed
    // then remove Qurantine from the controls.
    if( !IS_NT() && !NAV_IS_QUARANTINE() )
        {
        GetDlgItem(IDC_QUARANTINE_CHECK)->EnableWindow( FALSE );
        GetDlgItem(IDC_QUARANTINE_CHECK)->SendMessage( BM_SETCHECK, BST_UNCHECKED);
        }

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}


void CAutoProtectPage::OnSelectButton()
{
    CNavOptionsSheet* pParent = (CNavOptionsSheet*)GetParent();
	ASSERT(pParent);

	// Invoke global extension dialog
	pParent->m_IncludeDlg.DoModal();
}

void CAutoProtectPage::OnSelchangePromptCombo()
{
	UpdateData();
    BOOL bEnable = FALSE;

    // Only enable if on custom response item.
    if(m_iPrompt == 4)
        {
        bEnable = TRUE;
        }

    GetDlgItem(IDC_CUSTOM_BUTTON)->EnableWindow(bEnable);

    // Enable prompt checks if prompt is selected.
    if(!IS_NT())
        {
        if( m_iPrompt == 0 || m_iPrompt == 4)
            bEnable = TRUE;
        else
            bEnable = FALSE;

        GetDlgItem(IDC_REPAIR_CHECK)->EnableWindow(bEnable);
        GetDlgItem(IDC_DELETE_CHECK)->EnableWindow(bEnable);
        GetDlgItem(IDC_EXCLUDE_CHECK)->EnableWindow(bEnable);
        GetDlgItem(IDC_CONTINUE_CHECK)->EnableWindow(bEnable);
        GetDlgItem(IDC_STOP_CHECK)->EnableWindow(bEnable);

        // Only enable the Quarantine checkbox if Quarantine is installed.
        if( NAV_IS_QUARANTINE() )
            {
            GetDlgItem(IDC_QUARANTINE_CHECK)->EnableWindow(bEnable);
            }
        }
}

void CAutoProtectPage::OnCustomButton()
{
    m_CustDlg.DoModal();
}


BOOL CAutoProtectPage::SetData(LPNAVOPTIONS pOptions)
{

    TSROPT *pApOpts = &pOptions->tsr;

    // Set up Ap options
    m_AllFiles  = !pApOpts->bScanAll;
    m_bContinue =  pApOpts->bDispContinue;
    m_bCreated  =  pApOpts->bScanCreate;
    m_bDelete   =  pApOpts->bDispDelete;
    m_bDisable  =  pApOpts->bUnloadable;
    m_bExclude  =  pApOpts->bDispExclude;
    m_bLoad     =  pOptions->startup.bLoadVxD;
    m_bOpened   =  pApOpts->bScanOpen;
    m_bRepair   =  pApOpts->bDispRepair;
    m_bRun      =  pApOpts->bScanRun;
    m_bShowIcon =  !pApOpts->bHideIcon;
    m_bStop     =  pApOpts->bDispStop;
    m_bQuarantine = pApOpts->bDispQuarantine;

    // Set action combo box up.
    if(IS_NT())
        {
       switch (pApOpts->uActionKnown)
            {
            case TSR_DENY: m_iPrompt = 0; break;
            case TSR_PROMPT: m_iPrompt = 1; break;
            case TSR_REPAIR: m_iPrompt = 2; break;
            case TSR_DELETE: m_iPrompt = 3; break;
            case TSR_CUSTOM_RESPONSE:m_iPrompt = 4;break;
            case TSR_QUARANTINEQUIET: m_iPrompt = 5; break;
            case TSR_QUAR_CANT_REP: m_iPrompt = 6; break;
            }
        }
    else
        {
        switch (pApOpts->uActionKnown)
            {
            case TSR_PROMPT: m_iPrompt = 0; break;
            case TSR_DENY: m_iPrompt = 1; break;
            case TSR_REPAIR: m_iPrompt = 2; break;
            case TSR_DELETE: m_iPrompt = 3; break;
            case TSR_CUSTOM_RESPONSE: m_iPrompt = 4; break;
            case TSR_HALTCPU: m_iPrompt = 5; break;
            case TSR_QUARANTINEQUIET: m_iPrompt = 6; break;
            case TSR_QUAR_CANT_REP: m_iPrompt = 7; break;
            }
        }


    // Set up floppy checks for NT
    if(IS_NT())
        {
        m_bFloppyAccess = pApOpts->bChkFlopAccess;
        m_bFloppyReboot = pApOpts->bChkFlopOnBoot;
        }
    else
        {
        // Set up advanced dialogs.

        if ( IS_NEC() )
            {
            m_AdvancedDlgNec.m_iDos =            pApOpts->uROAttrib;
            m_AdvancedDlgNec.m_iProgram =        pApOpts->uWriteExe;
            m_AdvancedDlgNec.m_bFloppyReboot =   pApOpts->bChkFlopOnBoot;
            m_AdvancedDlgNec.m_bFloppyAccess =   pApOpts->bChkFlopAccess;
            }
        else
            {
            m_AdvancedDlg.m_iBootRecord =     pApOpts->uWriteHard;
            m_AdvancedDlg.m_iDos =            pApOpts->uROAttrib;
            m_AdvancedDlg.m_iFloppyBR =       pApOpts->uWriteFlop;
            m_AdvancedDlg.m_iLowLevel =       pApOpts->uFormat;
            m_AdvancedDlg.m_iProgram =        pApOpts->uWriteExe;
            m_AdvancedDlg.m_bFloppyBoth =     pApOpts->bChkBothFlops;
            m_AdvancedDlg.m_bFloppyReboot =   pApOpts->bChkFlopOnBoot;
            m_AdvancedDlg.m_bFloppyAccess =   pApOpts->bChkFlopAccess;
            }
        }

    // Set up heuristic configuration dialog.
    m_BloodhoundDlg.m_bEnabled = pApOpts->bEnableHeuristicScan;
    m_BloodhoundDlg.m_iProtectionLevel = (int)pApOpts->uHeuristicLevel;


    // Set AP Flag.
    m_CustDlg.m_bAP = TRUE;

    // Set up custom response actions.
    m_CustDlg.m_iFile = pOptions->customAP.uchFile;
    m_CustDlg.m_iMacro = pOptions->customAP.uchMacro;
    m_CustDlg.m_iBoot = pOptions->customAP.uchBoot;

//    m_bQuarantine = pOptions->quarantineset.bQuarantineAP;


    return TRUE;
}


BOOL CAutoProtectPage::GetData(LPNAVOPTIONS pOptions)
{
    TSROPT *pApOpts = &pOptions->tsr;

    // Save  Ap options
    pApOpts->bScanAll           = !m_AllFiles;
    pApOpts->bDispContinue      = m_bContinue;
    pApOpts->bScanCreate        = m_bCreated;
    pApOpts->bDispDelete        = m_bDelete;
    pApOpts->bUnloadable        = m_bDisable;
    pApOpts->bDispExclude       = m_bExclude;
    pOptions->startup.bLoadVxD  = m_bLoad;
    pApOpts->bScanOpen          = m_bOpened;
    pApOpts->bDispRepair        = m_bRepair;
    pApOpts->bScanRun           = m_bRun;
    pApOpts->bHideIcon          = !m_bShowIcon;
    pApOpts->bDispStop          = m_bStop;
    pApOpts->bDispQuarantine    = m_bQuarantine;

    // Save action combo
    if(IS_NT())
        {
        switch ( m_iPrompt )
            {
            case 0: pApOpts->uActionKnown = TSR_DENY; break;
            case 1: pApOpts->uActionKnown = TSR_PROMPT; break;
            case 2: pApOpts->uActionKnown = TSR_REPAIR; break;
            case 3: pApOpts->uActionKnown = TSR_DELETE; break;
            case 4: pApOpts->uActionKnown = TSR_CUSTOM_RESPONSE; break;
            case 5: pApOpts->uActionKnown = TSR_QUARANTINEQUIET; break;
            case 6: pApOpts->uActionKnown = TSR_QUAR_CANT_REP; break;
            }
        }
    else
        {
        switch ( m_iPrompt )
            {
            case 0: pApOpts->uActionKnown = TSR_PROMPT; break;
            case 1: pApOpts->uActionKnown = TSR_DENY; break;
            case 2: pApOpts->uActionKnown = TSR_REPAIR; break;
            case 3: pApOpts->uActionKnown = TSR_DELETE; break;
            case 4: pApOpts->uActionKnown = TSR_CUSTOM_RESPONSE; break;
            case 5: pApOpts->uActionKnown = TSR_HALTCPU; break;
            case 6: pApOpts->uActionKnown = TSR_QUARANTINEQUIET; break;
            case 7: pApOpts->uActionKnown = TSR_QUAR_CANT_REP; break;
            }
        }


    // Set up floppy checks for NT
    if(IS_NT())
        {
        pApOpts->bChkFlopAccess = m_bFloppyAccess;
        pApOpts->bChkFlopOnBoot = m_bFloppyReboot;
        }
    else
        {
        // Set up advanced dialogs.

        if ( IS_NEC() )
            {
            // Options that are not on the NEC dialog are not touched.

            pApOpts->uROAttrib      =   m_AdvancedDlgNec.m_iDos;
            pApOpts->uWriteExe      =   m_AdvancedDlgNec.m_iProgram;
            pApOpts->bChkFlopOnBoot =   m_AdvancedDlgNec.m_bFloppyReboot;
            pApOpts->bChkFlopAccess =   m_AdvancedDlgNec.m_bFloppyAccess;
            }
        else
            {
            pApOpts->uWriteHard     =   m_AdvancedDlg.m_iBootRecord;
            pApOpts->uROAttrib      =   m_AdvancedDlg.m_iDos;
            pApOpts->uWriteFlop     =   m_AdvancedDlg.m_iFloppyBR;
            pApOpts->uFormat        =   m_AdvancedDlg.m_iLowLevel;
            pApOpts->uWriteExe      =   m_AdvancedDlg.m_iProgram;
            pApOpts->bChkBothFlops  =   m_AdvancedDlg.m_bFloppyBoth;
            pApOpts->bChkFlopOnBoot =   m_AdvancedDlg.m_bFloppyReboot;
            pApOpts->bChkFlopAccess =   m_AdvancedDlg.m_bFloppyAccess;
            }
        }

    // Save off heuristic settings
    pApOpts->bEnableHeuristicScan = (BYTE) m_BloodhoundDlg.m_bEnabled;
    pApOpts->uHeuristicLevel = (BYTE) m_BloodhoundDlg.m_iProtectionLevel;

    // Get up custom response actions.
    pOptions->customAP.bSelected = pApOpts->uActionKnown == TSR_CUSTOM_RESPONSE ? TRUE : FALSE;
    pOptions->customAP.uchFile = (BYTE) m_CustDlg.m_iFile;
    pOptions->customAP.uchMacro = (BYTE) m_CustDlg.m_iMacro;
    pOptions->customAP.uchBoot = (BYTE) m_CustDlg.m_iBoot;

//    pOptions->quarantineset.bQuarantineAP = m_bQuarantine;

   return TRUE;

}


/////////////////////////////////////////////////////////////////////////////
// CAdvApDlg dialog
CAdvApDlg::CAdvApDlg(CWnd* pParent /*=NULL*/)
	: CHelperDialog(CAdvApDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CAdvApDlg)
	m_iBootRecord = -1;
	m_iDos = -1;
	m_iFloppyBR = -1;
	m_iLowLevel = -1;
	m_iProgram = -1;
	m_bFloppyBoth = FALSE;
	m_bFloppyReboot = FALSE;
	m_bFloppyAccess = FALSE;
	//}}AFX_DATA_INIT
    m_pHelpHowToMap = m_dwWhatsThisMap;
    m_pHelpWhatsThisMap = m_dwWhatsThisMap;
    m_dwHelpButtonHelpID = IDH_NAVW_AP_ADVANCED_SETTINGS_DLG_HELP_BTN;

}


void CAdvApDlg::DoDataExchange(CDataExchange* pDX)
{
	CHelperDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAdvApDlg)
	DDX_CBIndex(pDX, IDC_BOOTRECORD_COMBO, m_iBootRecord);
	DDX_CBIndex(pDX, IDC_DOS_COMBO, m_iDos);
	DDX_CBIndex(pDX, IDC_FLOPPYBR_COMBO, m_iFloppyBR);
	DDX_CBIndex(pDX, IDC_LOWLEVEL_COMBO, m_iLowLevel);
	DDX_CBIndex(pDX, IDC_PROGRAM_COMBO, m_iProgram);
	DDX_Check(pDX, IDC_FLOPPY_BOTH_CHECK, m_bFloppyBoth);
	DDX_Check(pDX, IDC_FLOPPY_REBOOT_CHECK, m_bFloppyReboot);
	DDX_Check(pDX, IDC_FLOPPYACCESS_CHECK, m_bFloppyAccess);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CAdvApDlg, CHelperDialog)
	//{{AFX_MSG_MAP(CAdvApDlg)
	ON_BN_CLICKED(IDC_FLOPPY_REBOOT_CHECK, OnFloppyRebootCheck)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CAdvApDlg message handlers



BOOL CAdvApDlg::OnInitDialog()
{
	CHelperDialog::OnInitDialog();

    // Initialize state of both floppy reboot check.
    OnFloppyRebootCheck();

    return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CAdvApDlg::OnFloppyRebootCheck()
{
	if(IsDlgButtonChecked(IDC_FLOPPY_REBOOT_CHECK))
        {
        GetDlgItem(IDC_FLOPPY_BOTH_CHECK)->EnableWindow(TRUE);
        }
    else
        {
        GetDlgItem(IDC_FLOPPY_BOTH_CHECK)->EnableWindow(FALSE);
        }
}

/////////////////////////////////////////////////////////////////////////////
// CAdvApDlgNec dialog


CAdvApDlgNec::CAdvApDlgNec(CWnd* pParent /*=NULL*/)
	: CHelperDialog(CAdvApDlgNec::IDD, pParent)
{
	//{{AFX_DATA_INIT(CAdvApDlgNec)
	m_iDos = -1;
	m_iProgram = -1;
	m_bFloppyReboot = FALSE;
	m_bFloppyAccess = FALSE;
	//}}AFX_DATA_INIT
    m_pHelpHowToMap = m_dwWhatsThisMap;
    m_pHelpWhatsThisMap = m_dwWhatsThisMap;
    m_dwHelpButtonHelpID = IDH_NAVW_AP_ADVANCED_SETTINGS_DLG_HELP_BTN;
}


void CAdvApDlgNec::DoDataExchange(CDataExchange* pDX)
{
	CHelperDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAdvApDlgNec)
	DDX_CBIndex(pDX, IDC_DOS_COMBO, m_iDos);
	DDX_CBIndex(pDX, IDC_PROGRAM_COMBO, m_iProgram);
	DDX_Check(pDX, IDC_FLOPPY_REBOOT_CHECK, m_bFloppyReboot);
	DDX_Check(pDX, IDC_FLOPPYACCESS_CHECK, m_bFloppyAccess);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CAdvApDlgNec, CHelperDialog)
	//{{AFX_MSG_MAP(CAdvApDlgNec)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CAdvApDlgNec message handlers

BOOL CAdvApDlgNec::OnInitDialog()
{
	CHelperDialog::OnInitDialog();

    return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

