/////////////////////////////////////////////////////////////////////////////
// $Header:   S:/N32OPTS/VCS/GENPAGE.CPv   1.11   02 Jul 1998 15:21:08   mdunn  $
/////////////////////////////////////////////////////////////////////////////
//
// GENPAGE.CPP - contains class implememtaion for CGeneralPage
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
// $Log:   S:/N32OPTS/VCS/GENPAGE.CPv  $
// 
//    Rev 1.11   02 Jul 1998 15:21:08   mdunn
// Fixed breakages on help IDs for build 22
// 
//    Rev 1.10   24 Jun 1998 16:00:42   jtaylor
// Added dynamic change to General page for .vir file backups.
// 
//    Rev 1.9   10 Apr 1998 00:27:12   MKEATIN
// Made inoculation related changes.
// 
//    Rev 1.8   03 Apr 1998 13:24:04   MKEATIN
// Removed File Inoculation, Immediate Notify, and Sensor.
// 
//    Rev 1.7   25 Jun 1997 19:57:28   DBUCHES
// Removed Scan high memory checkbox.
// 
//    Rev 1.6   20 Jun 1997 12:01:06   DBUCHES
// Added Help support.
// 
//    Rev 1.5   10 Jun 1997 15:20:32   DBUCHES
// Removed quarantine options.
// 
//    Rev 1.4   02 Jun 1997 14:20:34   DBUCHES
// Added extra validation for text entry fields.
// 
//    Rev 1.3   27 May 1997 20:56:04   DBUCHES
// I touched 30 files this rev.  Needed for support of NT Alerting.
//
//    Rev 1.2   23 May 1997 15:57:28   DBUCHES
// Added setting of extra Quarantine bits.
//
//    Rev 1.1   21 May 1997 14:37:26   DBUCHES
// Removed Browse button.
//
//    Rev 1.0   14 May 1997 14:58:14   DBUCHES
// Initial revision.
/////////////////////////////////////////////////////////////////////////////
// GenPage.cpp : implementation file
//

#include "stdafx.h"
#include "n32opts.h"
#include "GenPage.h"
#include "xapi.h"
#include "n32behav.h" // Personality DLL

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


/////////////////////////////////////////////////////////////////////////////
// Set up help maps
/////////////////////////////////////////////////////////////////////////////
DWORD CGeneralPage::m_dwWhatsThisMap[] = { 
    IDC_BACKUP_CHECK,       IDH_NAVW_WHATHIS_GENERAL_BACKUP_FILE_WHEN_REPAIRING_CHKBOX,
//&^    IDC_EXTENSION_EDIT,     IDH_NAVW_WHATHIS_GENERAL_BACKUP_EXTENSION_TXTBOX,
//&^    IDC_HIMEM_CHECK,        IDH_NAVW_WHATHIS_GENERAL_BACKUP_SCAN_HIGH_MEMORY_WHEN_SCANNING_MEMORY_CHKBOX,
    0,0 
    };
DWORD CGeneralPage::m_dwHowToMap[] = { 0,0 };


/////////////////////////////////////////////////////////////////////////////
// CGeneralPage property page

IMPLEMENT_DYNCREATE(CGeneralPage, CHelperPage)

CGeneralPage::CGeneralPage() : CHelperPage(CGeneralPage::IDD)
{
	//{{AFX_DATA_INIT(CGeneralPage)
	m_bBackup = FALSE;
	m_sExtension = _T("");
	m_bHimem = FALSE;
	m_sQuarentineDir = _T("");
	//}}AFX_DATA_INIT
    m_pHelpHowToMap = m_dwWhatsThisMap;
    m_pHelpWhatsThisMap = m_dwWhatsThisMap;
    m_dwHelpButtonHelpID = IDH_NAVW_GENERAL_DLG_HELP_BTN;

    // Set up Password ID
    m_iPasswordFeature = PASSWORD_VIEWGENOPT;
}

CGeneralPage::~CGeneralPage()
{
}

void CGeneralPage::DoDataExchange(CDataExchange* pDX)
{
	CHelperPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CGeneralPage)
	DDX_Check(pDX, IDC_BACKUP_CHECK, m_bBackup);
	DDX_Text(pDX, IDC_EXTENSION_EDIT, m_sExtension);
	DDV_MaxChars(pDX, m_sExtension, 3);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CGeneralPage, CHelperPage)
	//{{AFX_MSG_MAP(CGeneralPage)
	ON_BN_CLICKED(IDC_BACKUP_CHECK, OnBackupCheck)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CGeneralPage message handlers

BOOL CGeneralPage::OnInitDialog()
{
	CHelperPage::OnInitDialog();

    // If Quarantine is not enabled in this install then change the
    // backup text to the non-Quarantine version.
    if( !NAV_IS_QUARANTINE() )
        {
        CString szVirText;
        szVirText.LoadString( IDS_VIR_BACKUP );
        GetDlgItem( IDC_BACKUP_CHECK )->SetWindowText( szVirText );
        }
    
    OnBackupCheck();

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CGeneralPage::OnBackupCheck()
{
	GetDlgItem(IDC_EXTENSION_EDIT)->EnableWindow(IsDlgButtonChecked(IDC_BACKUP_CHECK));
}


BOOL CGeneralPage::GetData(LPNAVOPTIONS pOptions)
{
    pOptions->general.bBackupRep = m_bBackup;
    pOptions->general.bScanHighMemory = m_bHimem;

    m_sExtension.AnsiToOem();
  //  m_sQuarentineDir.AnsiToOem();

    _tcscpy(pOptions->general.szBackExt, (LPCTSTR) m_sExtension);
//    _tcscpy(pOptions->quarantineset.szQuarantineDir, (LPCTSTR) m_sQuarentineDir);


    return TRUE;
}

BOOL CGeneralPage::SetData(LPNAVOPTIONS pOptions)
{
    m_bBackup = pOptions->general.bBackupRep;
    m_bHimem = pOptions->general.bScanHighMemory;
    m_sExtension = pOptions->general.szBackExt;
//    m_sQuarentineDir = pOptions->quarantineset.szQuarantineDir;

    m_sExtension.OemToAnsi();
//    m_sQuarentineDir.OemToAnsi();

    return TRUE;
}



BOOL CGeneralPage::IsQuarantineTextValid()
{
    // ---------------------------------
    // Validate the quarantine path
    // ---------------------------------

    // Convert to upper case                                                     
    m_sQuarentineDir.MakeUpper();

    // Make sure length is valid
    if(m_sQuarentineDir.GetLength() == 0)
        return FALSE;

    // 1st character cannot be a period
    if( m_sQuarentineDir.Find( '.' ) == 0 )
        return FALSE;

    // Need to make sure this is a mapped drive (eg. C:\QUARANTINE )
    if( m_sQuarentineDir.GetAt(1) != ':' ||
        m_sQuarentineDir.GetAt(2) != '\\' )
        return FALSE;

    return NameValidatePathEx(m_sQuarentineDir.GetBuffer(0), FALSE );
}
