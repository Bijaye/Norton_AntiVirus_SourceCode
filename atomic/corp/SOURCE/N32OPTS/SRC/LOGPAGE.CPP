/////////////////////////////////////////////////////////////////////////////
// $Header:   S:/N32OPTS/VCS/logpage.cpv   1.24   08 Oct 1998 19:26:42   tcashin  $
/////////////////////////////////////////////////////////////////////////////
//
//
// LogPage.cpp : implementation file
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
// $Log:   S:/N32OPTS/VCS/logpage.cpv  $
// 
//    Rev 1.24   08 Oct 1998 19:26:42   tcashin
// Added Help ID for the access denied check box.
// 
//    Rev 1.23   06 Oct 1998 13:49:30   tcashin
// Added a new Access Denied check box option save in a .ini format
// file call navoptx.dat for extended options.
// 
//    Rev 1.22   10 Jul 1998 12:30:32   DBuches
// Hide Boot Record Inoc checkbox if running on NEC Win9x machines.
// 
//    Rev 1.21   24 Jun 1998 18:27:10   jtaylor
// Changed all the removals of Quarantine Checkboxes to dissables.
//
//    Rev 1.20   24 Jun 1998 17:54:46   jtaylor
// Removed more Quarantine Controls when Quarantine not installed.  Uncheck Qurantine checkboxes.  Removed unused first window parameter on AlignWindow calls.
//
//    Rev 1.19   28 May 1998 15:09:30   DBuches
// Removed virus list changes checkbox.
//
//    Rev 1.18   21 May 1998 15:33:00   tcashin
// Added what's this help for the quarantine checkbox.
//
//    Rev 1.17   23 Apr 1998 15:22:42   MKEATIN
// Re-Added Innoculation checkbox for SysInteg.
//
//    Rev 1.16   03 Apr 1998 13:24:40   MKEATIN
// Removed File Inoculation, Immediate Notify, and Sensor.
//
//    Rev 1.15   30 Mar 1998 15:07:16   tcashin
// Added a Quarantine check box to the activity log options panel.
//
//    Rev 1.14   25 Mar 1998 15:06:44   PVENKAT
// Moved changes from ROSWELL.  Disable "Innoculation" checkbox on OEM build on
// W95 platforms.  Uncheck before disbling it.
//
//
//    Rev 1.13   27 Feb 1998 17:30:36   RPULINT
// Aligned checkbox->edit->spin-static control combination dynamically so they
// could look fine in all languages
//
//    Rev 1.12   19 Feb 1998 11:57:12   mdunn
// Ported NEC changes from ROSNEC.
//
//    Rev 1.1   12 Oct 1997 19:03:46   mdunn
// Added context help to the log size edit box, log filename edit box, and
// log file browse button.
//
//    Rev 1.0   12 Sep 1997 17:44:48   RFULLER
// Initial revision
//
//    Rev 1.11   21 Aug 1997 19:50:10   DBUCHES
// Added validation for activity log file.
//
//    Rev 1.10   18 Aug 1997 11:49:46   DBUCHES
// Doh!  Refixed problem with 0Kb logfile size.
//
//    Rev 1.9   18 Jul 1997 17:20:42   DBUCHES
// We are now a Save dialog
//
//    Rev 1.8   16 Jul 1997 12:57:08   DBUCHES
// Special case for 0K log file size value.
//
//    Rev 1.7   09 Jul 1997 15:32:36   DBUCHES
// Changed title of FileOpen dialog.
//
//    Rev 1.6   25 Jun 1997 15:16:06   DBUCHES
// Converted buttons to owner draw buttons.
//
//    Rev 1.5   24 Jun 1997 16:01:28   DBUCHES
// Fixed problem updating controls.
//
//    Rev 1.4   20 Jun 1997 12:03:58   DBUCHES
// Added help support.
//
//    Rev 1.3   17 Jun 1997 13:13:40   DBUCHES
// Removed quarantine settings.
//
//    Rev 1.2   16 Jun 1997 15:36:16   DBUCHES
// Fixed problem with data validation.
//
//    Rev 1.1   27 May 1997 20:56:34   DBUCHES
// I touched 30 files this rev.  Needed for support of NT Alerting.
//
//    Rev 1.0   14 May 1997 14:58:18   DBUCHES
// Initial revision.
/////////////////////////////////////////////////////////////////////////////
//

#include "stdafx.h"
#include "n32opts.h"
#include "LogPage.h"
#include "xapi.h"
#include "AlignWnd.h"
#include "n32behav.h"
#include "navoptx.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// Set up help maps
/////////////////////////////////////////////////////////////////////////////
DWORD CLogPage::m_dwWhatsThisMap[] = {
    IDC_KNOWN_CHECK,        IDH_NAVW_WHATHIS_KNOWN_VIRUS_DETECTIONS_CHKBOX,
    IDC_INNOC_CHECK,        IDH_NAVW_WHATHIS_INOCULATION_ACTIVITIES_CHKBOX,
    IDC_VIRUS_LIKE_CHECK,   IDH_NAVW_WHATHIS_VIRUS_LIKE_ACTIVITIES_CHKBOX,
    IDC_COMPLETION_CHECK,   IDH_NAVW_WHATHIS_COMPLETION_OF_SCANS_CHKBOX,
    IDC_LIST_CHECK,         IDH_NAVW_WHATHIS_VIRUS_LIST_CHANGES,
    IDC_LIMIT_CHECK,        IDH_NAVW_WHATHIS_LIMIT_SIZE_OF_LOG_FILE_TO_CHKBOX,
    IDC_KB_EDIT,            IDH_NAVW_WHATHIS_KILOBYTES_CTRL,
    IDC_KB_SPIN,            IDH_NAVW_WHATHIS_KILOBYTES_CTRL,
    IDC_FILENAME_EDIT,      IDH_NAVW_WHATHIS_ACTIVITY_LOG_FILENAME_BROWSE_CTRL,
    IDC_FILEOPEN_BUTTON,    IDH_NAVW_WHATHIS_ACTIVITY_LOG_FILENAME_BROWSE_CTRL,
    IDC_QUARANTINE_CHECK,   IDH_NAVW_WHATHIS_OPTIONS_ACTIVITY_LOG_QUARAN_CHKBX,
    IDC_ACCESSDENIED_CHECK, IDH_NAVW_WHATHIS_DENIED_ACCESS_CHKBOX,
    0,0
    };
DWORD CLogPage::m_dwHowToMap[] = { 0,0 };


/////////////////////////////////////////////////////////////////////////////
// CLogPage property page

IMPLEMENT_DYNCREATE(CLogPage, CHelperPage)

CLogPage::CLogPage() : CHelperPage(IS_NT() ? IDD_ACTIVITY_LOG_PAGENT : CLogPage::IDD)
{
	//{{AFX_DATA_INIT(CLogPage)
	m_bComplete = FALSE;
	m_sFileName = _T("");
	m_bKnown = FALSE;
	m_bInnoc = FALSE;
	m_bLimit = FALSE;
	m_bList = FALSE;
	m_bVirusLike = FALSE;
	m_iSize = 0;
	m_bQuarantine = FALSE;
	m_bAccessDenied = FALSE;
	//}}AFX_DATA_INIT
    m_pHelpHowToMap = m_dwWhatsThisMap;
    m_pHelpWhatsThisMap = m_dwWhatsThisMap;
    m_dwHelpButtonHelpID = IDH_NAVW_ACTIVITY_LOG_DLG_SETTINGS_HELP_BTN;
    // Set up Password ID
    m_iPasswordFeature = PASSWORD_VIEWACTIVITYOPT;

}

CLogPage::~CLogPage()
{
}

void CLogPage::DoDataExchange(CDataExchange* pDX)
{
	CHelperPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CLogPage)
    DDX_Check(pDX, IDC_COMPLETION_CHECK, m_bComplete);
    DDX_Text(pDX, IDC_FILENAME_EDIT, m_sFileName);
	DDV_MaxChars(pDX, m_sFileName, MAX_PATH_SIZE);
    DDX_Check(pDX, IDC_KNOWN_CHECK, m_bKnown);
    DDX_Check(pDX, IDC_LIMIT_CHECK, m_bLimit);
	DDX_Text(pDX, IDC_KB_EDIT, m_iSize);
	DDV_MinMaxUInt(pDX, m_iSize, 1, 999);
	DDX_Control(pDX, IDC_FILEOPEN_BUTTON, m_BrowseBtn);
	DDX_Check(pDX, IDC_QUARANTINE_CHECK, m_bQuarantine);
	DDX_Check(pDX, IDC_ACCESSDENIED_CHECK, m_bAccessDenied);
	//}}AFX_DATA_MAP

    // These controls do not exist under NT.
    if(!IS_NT())
        {
        DDX_Check(pDX, IDC_VIRUS_LIKE_CHECK, m_bVirusLike);
        DDX_Check(pDX, IDC_INNOC_CHECK, m_bInnoc);
        }

}


BEGIN_MESSAGE_MAP(CLogPage, CHelperPage)
	//{{AFX_MSG_MAP(CLogPage)
	ON_BN_CLICKED(IDC_FILEOPEN_BUTTON, OnFileopenButton)
	ON_BN_CLICKED(IDC_LIMIT_CHECK, OnLimitCheck)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CLogPage message handlers

BOOL CLogPage::OnInitDialog()
{
	CHelperPage::OnInitDialog();

    // Set icon for browse button.
    m_BrowseBtn.SetIconID(IDI_BROWSE);

	// Align these controls
	// This will help make the UI look alligned in English
	// as well as international languages
	AlignWndResize (GetDlgItem(IDC_LIMIT_CHECK)->m_hWnd);
    AlignWnd (GetDlgItem(IDC_KB_EDIT)->m_hWnd,
              GetDlgItem(IDC_LIMIT_CHECK)->m_hWnd,
              gAlignWndFollowX,
              NULL, 1 /*padding*/);
    AlignWnd (GetDlgItem(IDC_KB_SPIN)->m_hWnd,
              GetDlgItem(IDC_KB_EDIT)->m_hWnd,
              gAlignWndFollowX,
              NULL, -5 /*padding*/);
    AlignWnd (GetDlgItem(IDC_STATIC_KB)->m_hWnd,
              GetDlgItem(IDC_KB_SPIN)->m_hWnd,
              gAlignWndFollowX,
              NULL, 2 /*padding*/);

    // Set up spin control
    CSpinButtonCtrl * pSpin =(CSpinButtonCtrl*)GetDlgItem(IDC_KB_SPIN);
    pSpin->SetRange(1, 999);

    // Enable controls
    OnLimitCheck();

    if (!IS_NT() && NAV_IS_SCANONLY())
    {
        // On Scanner Only version, No Innoculation so remove default check.
        GetDlgItem ( IDC_INNOC_CHECK ) -> SendMessage ( BM_SETCHECK,
                                                        BST_UNCHECKED,
                                                        0L ) ;
        GetDlgItem ( IDC_INNOC_CHECK ) -> EnableWindow ( FALSE ) ;
    }

    // If this installation of NAV does not have Quarantine installed
    // then remove Qurantine from the controls.
    if( !NAV_IS_QUARANTINE() )
        {
        GetDlgItem(IDC_QUARANTINE_CHECK)->EnableWindow( FALSE );
        GetDlgItem(IDC_QUARANTINE_CHECK)->SendMessage( BM_SETCHECK, BST_UNCHECKED);
        }


    // Hide Boot Record Inoc button on NEC machines.
    if( !IS_NT() && IS_NEC() )
        {
        GetDlgItem( IDC_INNOC_CHECK )->ShowWindow( SW_HIDE );
        }

    return TRUE;      // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CLogPage::OnFileopenButton()
{
    // Get filename from user.
    CString sFilter;
    sFilter.LoadString(IDS_LOG_FILTER);
    CFileDialog fileDlg(TRUE,
                        NULL,
                        m_sFileName,
                        OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
                        sFilter);

    CString sTitle;
    sTitle.LoadString(IDS_LOG_FILE_OPEN_TITLE);
    fileDlg.m_ofn.lpstrTitle = sTitle.GetBuffer(0);

    // Get the filename.
    if(fileDlg.DoModal() == IDOK)
        {
        UpdateData();
        m_sFileName = fileDlg.GetPathName();
        UpdateData(FALSE);
        }

}



BOOL CLogPage::SetData(LPNAVOPTIONS pOptions)
{   
    LPACTOPT pActOpts = &pOptions->activity;

    m_bComplete     = pActOpts->bStartEnd;
    m_sFileName     = pActOpts->szLogFile;
    m_bInnoc        = pActOpts->bInocChange;
    m_bKnown        = pActOpts->bKnown;
    m_bList         = pActOpts->bVirusList;
    m_bLimit        = pActOpts->bKeepUpTo;
    m_bVirusLike    = pActOpts->bVirusLike;
    m_bQuarantine   = pActOpts->bQuarantine;
    m_iSize         = pActOpts->uKiloBytes;

    // Special case: If limit is 0, set default to 50K
    if( m_bLimit == 0 )
        m_iSize = 50;

    // -----------------------------------------------------------------
    // In-line WorkAround Kludge Alert!
    //
    // We can't change NAVOPTS.DAT. Instead, update the extended options 
    // file, NAVOPTX.DAT, for the access denied option.
    // 
    // Make sure this option gets added to NAVOPTS.DAT in the next rev!
    // -----------------------------------------------------------------

    TCHAR   szNavPath[SYM_MAX_PATH*4] = {0};

    GetPath(szNavPath, sizeof(szNavPath) / sizeof(szNavPath[0]));
    _tcscat(szNavPath, NAVOPTX_FILE);

    int i = GetPrivateProfileInt(NAVOPTX_SECT_ACTLOG,
                             NAVOPTX_ACTLOG_ACCESSDENIED,
                             0,
                             szNavPath);

    if ( i == 0 )
        m_bAccessDenied = FALSE;
    else
        m_bAccessDenied = TRUE;

    // -----------------------------------------------------------------
    // End In-line Kludge Alert!
    // -----------------------------------------------------------------

    return TRUE;

}

BOOL CLogPage::GetData(LPNAVOPTIONS pOptions)
{
    LPACTOPT pActOpts = &pOptions->activity;

    pActOpts->bStartEnd    = m_bComplete;
    pActOpts->bInocChange  = m_bInnoc;
    pActOpts->bKnown       = m_bKnown;
    pActOpts->bVirusList   = m_bList;
    pActOpts->bKeepUpTo    = m_bLimit;
    pActOpts->bVirusLike   = m_bVirusLike;
    pActOpts->bQuarantine  = m_bQuarantine;

    pActOpts->uKiloBytes   = m_iSize;
    _tcsncpy(pActOpts->szLogFile, (LPCTSTR)m_sFileName, MAX_PATH_SIZE);

    // -----------------------------------------------------------------
    // In-line Kludge Alert!
    //
    // We can't change NAVOPTS.DAT. Instead, read the extended options 
    // file, NAVOPTX.DAT, for the access denied option. 
    // 
    // Make sure this option gets added to NAVOPTS.DAT in the next rev!
    // -----------------------------------------------------------------

    TCHAR   szOptString[4] = {0};
    TCHAR   szNavPath[SYM_MAX_PATH*4] = {0};

    GetPath(szNavPath, sizeof(szNavPath) / sizeof(szNavPath[0]));

    _tcscat (szNavPath, NAVOPTX_FILE);

    SPRINTF(szOptString,"%d", m_bAccessDenied);

    WritePrivateProfileString (NAVOPTX_SECT_ACTLOG,
                               NAVOPTX_ACTLOG_ACCESSDENIED,
                               szOptString,
                               szNavPath);

    // -----------------------------------------------------------------
    // End In-line Kludge Alert!
    // -----------------------------------------------------------------


    return TRUE;

}



void CLogPage::OnLimitCheck()
{
	BOOL bEnable = IsDlgButtonChecked(IDC_LIMIT_CHECK);

    GetDlgItem( IDC_KB_EDIT )->EnableWindow(bEnable);
    GetDlgItem( IDC_KB_SPIN )->EnableWindow(bEnable);

}


BOOL CLogPage::OnKillActive()
{

    // First get data from controls
    UpdateData();

    // Next, strip leading and trailing spaces.
    m_sFileName.TrimLeft();
    m_sFileName.TrimRight();

    UpdateData(FALSE);

    // Finally, validate the file path
    if( NameValidatePathEx(m_sFileName.GetBuffer(0), FALSE) )
        {
        // Looks good, call base class method
        return CHelperPage::OnKillActive();

        }

    // Inform the user that he has wandered from the
    // one true path to enlightenment...
    CString sTitle, sText;
    sTitle.LoadString(IDS_ERROR_TITLE);
    sText.LoadString(IDS_ERROR_BAD_LOGFILE);
    MessageBox( sText, sTitle , MB_OK | MB_ICONEXCLAMATION);

    GetDlgItem(IDC_FILENAME_EDIT)->SetFocus();
    ((CEdit*)GetDlgItem(IDC_FILENAME_EDIT))->SetSel(0, -1);

    return FALSE;
}

BOOL CLogPage::GetPath(TCHAR * szPath, int iSize)
{
    auto HKEY   hKey = 0;
    auto BOOL   bRet = FALSE;

    MEMSET (szPath, 0, iSize);

    if (ERROR_SUCCESS == RegOpenKeyEx(HKEY_LOCAL_MACHINE, TEXT("SOFTWARE\\Symantec\\InstalledApps"), 0, KEY_READ, &hKey))
    {
#ifdef NAVWNT
        if (ERROR_SUCCESS == RegQueryValueEx(hKey, TEXT("NAVNT"), 0, 0, (LPBYTE)szPath, (LPDWORD)&iSize))
#else
        if (ERROR_SUCCESS == RegQueryValueEx(hKey, TEXT("NAV95"), 0, 0, (LPBYTE)szPath, (LPDWORD)&iSize))
#endif
        {
            bRet = TRUE;
            RegCloseKey(hKey);
        }
    }

    return ( bRet );

}
