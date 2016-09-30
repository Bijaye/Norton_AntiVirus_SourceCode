/////////////////////////////////////////////////////////////////////////////
// $Header:   S:/QCONSOLE/VCS/filetypepage.cpv   1.2   21 May 1998 11:29:06   DBuches  $
/////////////////////////////////////////////////////////////////////////////
//
// FileTypePage.cpp : implementation file
//
//
/////////////////////////////////////////////////////////////////////////////
// $Log:   S:/QCONSOLE/VCS/filetypepage.cpv  $
// 
//    Rev 1.2   21 May 1998 11:29:06   DBuches
// Added help topic IDs.
// 
//    Rev 1.1   20 May 1998 17:11:18   DBuches
// 1st pass at help.
// 
//    Rev 1.0   03 Apr 1998 13:30:20   DBuches
// Initial revision.
/////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "qconsole.h"
#include "FileTypePage.h"
#include "quar32.h"
#include "qconhlp.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CFileTypePage property page

// Help maps
DWORD CFileTypePage::m_dwWhatsThisMap[] = {
    IDC_REPAIR_CHECK,       IDH_QUARAN_OPTIONS_REPAIR,
    IDC_DELETE_CHECK,       IDH_QUARAN_OPTIONS_DELETE, 
    IDC_RESTORE_CHECK,      IDH_QUARAN_OPTIONS_RESTORE,
    IDC_CHECK5,             IDH_QUARAN_OPTIONS_SUBMIT, 
    0,0};

DWORD CFileTypePage::m_dwHowToMap[] = {0,0};




/////////////////////////////////////////////////////////////////////////////

IMPLEMENT_DYNCREATE(CFileTypePage, CHelperPage)

CFileTypePage::CFileTypePage() : CHelperPage(CFileTypePage::IDD)
{
	//{{AFX_DATA_INIT(CFileTypePage)
	m_bDelete = FALSE;
	m_bRepair = FALSE;
	m_bRestore = FALSE;
	m_bSubmit = FALSE;
	//}}AFX_DATA_INIT

    // Use title field of header
    m_psp.dwFlags |= PSP_USETITLE | PSP_HASHELP;

    m_pHelpWhatsThisMap = m_dwWhatsThisMap;
    m_pHelpHowToMap = m_dwHowToMap;

    m_Images.Create( IDB_TREE_BITMAPS, 16, 1, RGB( 255, 0, 255 ) );
}

CFileTypePage::~CFileTypePage()
{
}

void CFileTypePage::DoDataExchange(CDataExchange* pDX)
{
	CHelperPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CFileTypePage)
	DDX_Check(pDX, IDC_DELETE_CHECK, m_bDelete);
	DDX_Check(pDX, IDC_REPAIR_CHECK, m_bRepair);
	DDX_Check(pDX, IDC_RESTORE_CHECK, m_bRestore);
	DDX_Check(pDX, IDC_CHECK5, m_bSubmit);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CFileTypePage, CHelperPage)
	//{{AFX_MSG_MAP(CFileTypePage)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CFileTypePage message handlers

void CFileTypePage::Initialize( IQuarantineOpts * pOpts, int iType )
{
    m_pOpts = pOpts; 
    m_iFileType = iType;

    m_psp.dwFlags |= PSP_USEHICON;

    switch( m_iFileType )
        {
        case FILE_TYPE_QUARANTINE:
            m_psp.pszTitle = MAKEINTRESOURCE( IDS_QUARANTINE_FILES );
            m_psp.hIcon = m_Images.ExtractIcon( 1 ); 
            m_dwHelpButtonHelpID = IDH_QUARAN_OPTIONS_QUARAN_ALLOW_HLPBTN;
            break;
        case FILE_TYPE_BACKUP:
            m_psp.pszTitle = MAKEINTRESOURCE( IDS_BACKUP_FILES );
            m_psp.hIcon = m_Images.ExtractIcon( 2 );
            m_dwHelpButtonHelpID = IDH_QUARAN_OPTIONS_BACKUP_FILES_HLPBTN;
            break;
        case FILE_TYPE_SUBMITTED:
            m_psp.pszTitle = MAKEINTRESOURCE( IDS_SUBMITTED_FILES );
            m_psp.hIcon = m_Images.ExtractIcon( 3 );
            m_dwHelpButtonHelpID = IDH_QUARAN_OPTIONS_SUBMIT_FILES_HLPBTN;
            break;
        }
}

////////////////////////////////////////////////////////////////////////////
// Function name	: CFileTypePage::OnInitDialog
//
// Description	    : Handles WM_INITDIALOG message
//
// Return type		: BOOL 
//
//
////////////////////////////////////////////////////////////////////////////
// 4/1/98 DBUCHES - Function created / Header added.
////////////////////////////////////////////////////////////////////////////
BOOL CFileTypePage::OnInitDialog() 
{
	CHelperPage::OnInitDialog();
	
//                              QFILEOPS_INDEX_QUARANTINED
//                              QFILEOPS_INDEX_SUBMITTED
//                              QFILEOPS_INDEX_BACKUP_FILE
	
    // Set up correct description text and control data
    CString s;
    int iIndex;
    switch( m_iFileType )
        {
        case FILE_TYPE_QUARANTINE:
            s.LoadString( IDS_OPTS_QUARANTINE_DESC );
            iIndex = QFILEOPS_INDEX_QUARANTINED;
            break;
        case FILE_TYPE_BACKUP:
            s.LoadString( IDS_OPTS_BACKUP_DESC );
            iIndex = QFILEOPS_INDEX_BACKUP_FILE;
            break;
        case FILE_TYPE_SUBMITTED:
            s.LoadString( IDS_OPTS_SUBMITTED_DESC );
            iIndex = QFILEOPS_INDEX_SUBMITTED;
            break;
        }
    GetDlgItem( IDC_DECRIPTION_STATIC )->SetWindowText( s );

    // Set up check states.
    DWORD dwFileOpts;
    m_pOpts->GetValidFileOperations( &dwFileOpts, iIndex );

    if( dwFileOpts & QUAR_OPTS_FILEOP_DELETE )
        {
        m_bDelete = TRUE;
        }
    if( dwFileOpts & QUAR_OPTS_FILEOP_REPAIR )
        {
        m_bRepair = TRUE;
        }
    if( dwFileOpts & QUAR_OPTS_FILEOP_PUT_BACK )
        {
        m_bRestore = TRUE; 
        }
    if( dwFileOpts & QUAR_OPTS_FILEOP_SUBMIT_TO_SARC )
        {
        m_bSubmit = TRUE;
        }

    // Move data to controls
    UpdateData( FALSE );
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}



////////////////////////////////////////////////////////////////////////////////
//
// Function name	: CFileTypePage::OnOK
// Description	    : Handles OK button click
// Return type		: void 
//
////////////////////////////////////////////////////////////////////////////////
// 4/2/98 - DBUCHES function created / header added
////////////////////////////////////////////////////////////////////////////////
void CFileTypePage::OnOK() 
{
	// Get Data from controls
    UpdateData();

    // Figure out which file type index to save
    int iIndex;
    switch( m_iFileType )
        {
        case FILE_TYPE_QUARANTINE:
            iIndex = QFILEOPS_INDEX_QUARANTINED;
            break;
        case FILE_TYPE_BACKUP:
            iIndex = QFILEOPS_INDEX_BACKUP_FILE;
            break;
        case FILE_TYPE_SUBMITTED:
            iIndex = QFILEOPS_INDEX_SUBMITTED;
            break;
        }

    // Save off flags
    DWORD dwFileOpts = 0;
    if( m_bDelete )
        {
        dwFileOpts |= QUAR_OPTS_FILEOP_DELETE;
        }
    if( m_bRepair )
        {
        dwFileOpts |= QUAR_OPTS_FILEOP_REPAIR;
        }
    if( m_bRestore )
        {
        dwFileOpts |= QUAR_OPTS_FILEOP_PUT_BACK;
        }
    if( m_bSubmit )
        {
        dwFileOpts |= QUAR_OPTS_FILEOP_SUBMIT_TO_SARC;
        }

    m_pOpts->SetValidFileOperations( dwFileOpts, iIndex );
	
	CHelperPage::OnOK();
}
