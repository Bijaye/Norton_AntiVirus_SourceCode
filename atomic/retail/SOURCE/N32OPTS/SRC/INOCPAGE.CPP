/////////////////////////////////////////////////////////////////////////////
// $Header:   S:/N32OPTS/VCS/INOCPAGE.CPv   1.21   28 Jul 1998 13:44:20   DBuches  $
/////////////////////////////////////////////////////////////////////////////
//
//
// InocPage.cpp : implementation file
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
// $Log:   S:/N32OPTS/VCS/INOCPAGE.CPv  $
// 
//    Rev 1.21   28 Jul 1998 13:44:20   DBuches
// Fixed more broken help stuff.
//
//    Rev 1.20   21 Jul 1998 14:22:52   mdunn
// Removed code from GetData() that wrote the inoc path to the registry -
// it belongs off in COMMON.LIB.
//
//    Rev 1.19   13 Jul 1998 12:24:10   DBuches
// Fixed help problems.
//
//    Rev 1.18   08 Jul 1998 11:58:00   DBuches
// Fixed another problem with enabling controls.
//
//    Rev 1.17   02 Jul 1998 15:21:10   mdunn
// Fixed breakages on help IDs for build 22
//
//    Rev 1.16   01 Jul 1998 12:07:10   DBuches
// Added more help topics.
//
//    Rev 1.15   10 Jun 1998 12:04:16   DBuches
// Needed to set initial gray states of controls.
//
//    Rev 1.14   28 May 1998 14:39:12   DBuches
// Fixed misc control enabling bugs.
//
//    Rev 1.13   26 May 1998 11:40:46   mdunn
// Fixed 112166: Now storing the inoc dir in the registry when options
// are saved.
//
//    Rev 1.12   21 May 1998 15:31:56   tcashin
// Fixed what's this help ID map to match the dialog.
//
//    Rev 1.11   10 Apr 1998 00:27:34   MKEATIN
// Made inoculation related changes.
//
//    Rev 1.10   19 Feb 1998 11:57:12   mdunn
// Ported NEC changes from ROSNEC.
//
//    Rev 1.9   18 Nov 1997 20:46:30   jtaylor
// Undid scanonly changes
//
//    Rev 1.7   20 Jun 1997 12:03:40   DBUCHES
// Added help support.
//
//    Rev 1.6   13 Jun 1997 21:52:06   JBRENNA
// Correct a comment.
//
//    Rev 1.5   10 Jun 1997 15:20:34   DBUCHES
// Removed quarantine options.
//
//    Rev 1.4   02 Jun 1997 14:20:34   DBUCHES
// Added extra validation for text entry fields.
//
//    Rev 1.3   27 May 1997 20:56:26   DBUCHES
// I touched 30 files this rev.  Needed for support of NT Alerting.
//
//    Rev 1.2   23 May 1997 15:57:30   DBUCHES
// Added setting of extra Quarantine bits.
//
//    Rev 1.1   16 May 1997 18:55:10   DBUCHES
// Fixed problem with GetData() Member.  Went cut-paste crazy
// and paid the price...
//
//    Rev 1.0   14 May 1997 14:58:16   DBUCHES
// Initial revision.
/////////////////////////////////////////////////////////////////////////////
//

#include "stdafx.h"
#include "n32opts.h"
#include "InocPage.h"
#include "xapi.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// Set up help maps
/////////////////////////////////////////////////////////////////////////////
DWORD CInocPage::m_dwWhatsThisMap[] = {
    IDC_BOOTRECORD_CHECK,       IDH_NAVW_WHATHIS_OPTIONS_INOC_INOC_BOOT_RECORDS_CHKBX,
    IDC_STATIC_INOC_PATH,       IDH_NAVW_WHATHIS_INOCULATION_PATH_TXTBOX,
    IDC_REPAIR_CHECK,           IDH_NAVW_WHATHIS_INOCULATION_BTN_REPAIR,
    IDC_INOCULATE_CHECK,        IDH_NAVW_WHATHIS_INOCULATION_BTN_INOCULATE,
    IDC_CONTINUE_CHECK,         IDH_NAVW_WHATHIS_INOCULATION_BTN_CONTINUE,
    IDC_STOP_CHECK,             IDH_NAVW_WHATHIS_INOCULATION_BTN_STOP,
    IDC_CHANGED_COMBO,          IDH_NAVW_WHATHIS_OPTIONS_INOC_BOOT_RECORD_CHANGED_COMBOBX,
    0,0
    };
DWORD CInocPage::m_dwHowToMap[] = { 0,0 };


/////////////////////////////////////////////////////////////////////////////
// CInocPage property page

IMPLEMENT_DYNCREATE(CInocPage, CHelperPage)

CInocPage::CInocPage() : CHelperPage(CInocPage::IDD)
{
	//{{AFX_DATA_INIT(CInocPage)
	m_bBootRecord = FALSE;
	m_iChanged = -1;
	m_sFilePath = _T("");
	m_bInnoc = FALSE;
	m_bRepair = FALSE;
	m_bStop = FALSE;
	m_bContinue = FALSE;
	//}}AFX_DATA_INIT
    m_pHelpHowToMap = m_dwWhatsThisMap;
    m_pHelpWhatsThisMap = m_dwWhatsThisMap;
    m_dwHelpButtonHelpID = IDH_NAVW_INOCULATION_DLG_HELP_BTN;
    // Set up Password ID
    m_iPasswordFeature = PASSWORD_VIEWINOCOPT;

}

CInocPage::~CInocPage()
{
}

void CInocPage::DoDataExchange(CDataExchange* pDX)
{
	CHelperPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CInocPage)
	DDX_Check(pDX, IDC_BOOTRECORD_CHECK, m_bBootRecord);
	DDX_CBIndex(pDX, IDC_CHANGED_COMBO, m_iChanged);
	DDX_Text(pDX, IDC_INOC_EDIT, m_sFilePath);
	DDV_MaxChars(pDX, m_sFilePath, MAX_PATH_SIZE);
	DDX_Check(pDX, IDC_INOCULATE_CHECK, m_bInnoc);
	DDX_Check(pDX, IDC_REPAIR_CHECK, m_bRepair);
	DDX_Check(pDX, IDC_STOP_CHECK, m_bStop);
	DDX_Check(pDX, IDC_CONTINUE_CHECK, m_bContinue);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CInocPage, CHelperPage)
	//{{AFX_MSG_MAP(CInocPage)
	ON_CBN_SELCHANGE(IDC_CHANGED_COMBO, OnSelchangeChangedCombo)
	ON_BN_CLICKED(IDC_BOOTRECORD_CHECK, OnBootrecordCheck)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CInocPage message handlers

BOOL CInocPage::SetData(LPNAVOPTIONS pOptions)
{
    LPINOCOPT pInocOpts = &pOptions->inoc;

    m_bBootRecord   = pInocOpts->bUseSystemInoc;
    m_bRepair       = pInocOpts->bDispRepair;
    m_bInnoc        = pInocOpts->bDispInoc;
    m_bContinue     = pInocOpts->bDispCont;
    m_bStop         = pInocOpts->bDispStop;
    m_sFilePath     = pInocOpts->szNetInocPath;

    switch(pInocOpts->uActionInocChg)
        {
        case INOC_CHANGE_PROMPT: m_iChanged = 0; break;
        case INOC_CHANGE_REPORT: m_iChanged = 1; break;
        }

    return TRUE;
}

BOOL CInocPage::GetData(LPNAVOPTIONS pOptions)
{
    LPINOCOPT pInocOpts = &pOptions->inoc;

    pInocOpts->bUseSystemInoc    = m_bBootRecord;
    pInocOpts->bDispRepair       = m_bRepair;
    pInocOpts->bDispInoc         = m_bInnoc;
    pInocOpts->bDispCont         = m_bContinue;
    pInocOpts->bDispStop         = m_bStop;

    _tcsncpy(pInocOpts->szNetInocPath, (LPCTSTR)m_sFilePath, MAX_PATH_SIZE);

    switch(m_iChanged)
        {
        case 0: pInocOpts->uActionInocChg = INOC_CHANGE_PROMPT; break;
        case 1: pInocOpts->uActionInocChg = INOC_CHANGE_REPORT; break;
        }

    return TRUE;
}


BOOL CInocPage::OnInitDialog()
{
	CHelperPage::OnInitDialog();

    if ( IS_NEC() )
        {
        // The NEC version does not have Sysinteg, so hide the "inoc boot
        // records and system files" checkbox.

        GetDlgItem ( IDC_BOOTRECORD_CHECK ) -> ShowWindow ( SW_HIDE );
        }


    // Set state of controls.
    OnBootrecordCheck();
    OnSelchangeChangedCombo();

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}


BOOL CInocPage::OnKillActive()
{
    UpdateData();

    // Make sure inoculate text is valid.
    if(IsInocDirValid())
        {
        // Everyting is cool.. .return
        return CHelperPage::OnKillActive();
        }

    // Let the user know something bad happened.
    CString sTitle, sText;
    sTitle.LoadString(IDS_ERROR_TITLE);
    sText.LoadString(IDS_ERROR_BAD_INOC_DIR);
    MessageBox( sText, sTitle , MB_OK | MB_ICONEXCLAMATION);

    // Set focus back to offending control.
    ((CEdit*)GetDlgItem(IDC_INOC_EDIT))->SetFocus();
    ((CEdit*)GetDlgItem(IDC_INOC_EDIT))->SetSel(0, -1);
    return FALSE;

}


BOOL CInocPage::IsInocDirValid()
{
    // ---------------------------------
    // Validate the inoculation path
    // ---------------------------------

    // Convert to upper case
    m_sFilePath.MakeUpper();

    // Make sure length is valid
    if(m_sFilePath.GetLength() == 0)
        return FALSE;

    // Make sure first character is a '\'
    if( m_sFilePath.GetAt(0) != _T('\\') )
        return FALSE;

    // Perform additional validation.
    if( m_sFilePath.Find( '.' ) == 0 ||     // 1st character cannot be a period
        m_sFilePath.Find( ':' ) != -1 ||    // Drive letters are not allowed
        m_sFilePath.Find( '/' ) != -1)      // neither are foreward slashes
        return FALSE;

    return NameValidatePathEx(m_sFilePath.GetBuffer(0), FALSE );

}

void CInocPage::OnSelchangeChangedCombo()
{
	UpdateData();
    BOOL bEnable = FALSE;

    // Enable prompt checks if prompt is selected.
    if( m_iChanged == 0 && m_bBootRecord )
        {
        bEnable = TRUE;
        }

    GetDlgItem(IDC_REPAIR_CHECK)->EnableWindow(bEnable);
    GetDlgItem(IDC_CONTINUE_CHECK)->EnableWindow(bEnable);
    GetDlgItem(IDC_STOP_CHECK)->EnableWindow(bEnable);
    GetDlgItem(IDC_INOCULATE_CHECK)->EnableWindow(bEnable);

}

void CInocPage::OnBootrecordCheck()
{
	UpdateData();

    GetDlgItem(IDC_REPAIR_CHECK)->EnableWindow( m_bBootRecord );
    GetDlgItem(IDC_CONTINUE_CHECK)->EnableWindow( m_bBootRecord );
    GetDlgItem(IDC_STOP_CHECK)->EnableWindow( m_bBootRecord );
    GetDlgItem(IDC_INOCULATE_CHECK)->EnableWindow( m_bBootRecord );
    GetDlgItem(IDC_CHANGED_COMBO)->EnableWindow( m_bBootRecord );

}
