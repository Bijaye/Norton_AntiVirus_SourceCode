/////////////////////////////////////////////////////////////////////////////
// $Header:   S:/QCONSOLE/VCS/genprop.cpv   1.7   08 Jun 1998 14:54:54   DBuches  $
/////////////////////////////////////////////////////////////////////////////
//
// GenProp.cpp : implementation file
//
//
/////////////////////////////////////////////////////////////////////////////
// $Log:   S:/QCONSOLE/VCS/genprop.cpv  $
// 
//    Rev 1.7   08 Jun 1998 14:54:54   DBuches
// Added extra code to classify compressed items.
// 
//    Rev 1.6   04 Jun 1998 15:37:24   DBuches
// Changed to work with GMT times.
// 
//    Rev 1.5   20 May 1998 17:11:18   DBuches
// 1st pass at help.
// 
//    Rev 1.4   07 May 1998 15:07:46   DBuches
// Added help button.
// 
//    Rev 1.3   21 Apr 1998 17:17:54   DBuches
// Added recommendation text.
// 
//    Rev 1.2   13 Mar 1998 15:20:02   DBuches
// Display status text value.
// 
//    Rev 1.1   06 Mar 1998 11:20:24   DBuches
// Updated to use IQuarantineItem pointer to update data.
// 
//    Rev 1.0   27 Feb 1998 15:08:56   DBuches
// Initial revision.
/////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "platform.h"
#include "qconsole.h"
#include "GenProp.h"
#include "RangeStr.h"
#include "xapi.h"

// bugbug - should not need this
#include "quaradd.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CGenProp property page

IMPLEMENT_DYNCREATE(CGenProp, CItemPage)

CGenProp::CGenProp() : CItemPage(CGenProp::IDD)
{
	//{{AFX_DATA_INIT(CGenProp)
	m_sItemName = _T("");
	m_sTypeName = _T("");
	m_sFileSize = _T("");
	m_sLocation = _T("");
	m_sQuarantineDate = _T("");
	m_sSubDate = _T("");
	m_sStatus = _T("");
    m_sStatusInfo = _T("");
	//}}AFX_DATA_INIT

}

CGenProp::~CGenProp()
{
}

void CGenProp::DoDataExchange(CDataExchange* pDX)
{
	CItemPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CGenProp)
	DDX_Control(pDX, IDC_ICON_BUTTON, m_ImageButton);
	DDX_Text(pDX, IDC_NAME_EDIT, m_sItemName);
	DDX_Text(pDX, IDC_TYPE_EDIT, m_sTypeName);
	DDX_Text(pDX, IDC_SIZE_EDIT, m_sFileSize);
	DDX_Text(pDX, IDC_LOCATION_EDIT, m_sLocation);
	DDX_Text(pDX, IDC_QUARANTINE_EDIT, m_sQuarantineDate);
	DDX_Text(pDX, IDC_SUBMITTED_EDIT, m_sSubDate);
	DDX_Text(pDX, IDC_STATUS_EDIT, m_sStatus);
    DDX_Text(pDX, IDC_INFORMATION_EDIT, m_sStatusInfo);
    //}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CGenProp, CItemPage)
	//{{AFX_MSG_MAP(CGenProp)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CGenProp message handlers


////////////////////////////////////////////////////////////////////////////
// Function name	: CGenProp::OnInitDialog
//
// Description	    : Handles WM_INITDIALOG message
//
// Return type		: BOOL 
//
//
////////////////////////////////////////////////////////////////////////////
// 3/3/98 DBUCHES - Function created / Header added.
////////////////////////////////////////////////////////////////////////////
BOOL CGenProp::OnInitDialog() 
{
	CItemPage::OnInitDialog();
	
	// Set up controls.
    
    // Build real file name
    CString s;
    CString & sQuarantineFolder = m_pDoc->GetQuarantineFolder();
    s = sQuarantineFolder;
    s += _T( '\\' );

    // Get the filename and add it to our path
    char szRealFileName[MAX_PATH];
    m_pItem->GetCurrentFilename( szRealFileName, MAX_PATH );
    s += szRealFileName;
    
    // Get file type name.
    SHFILEINFO sfi;
    ZeroMemory( &sfi, sizeof(SHFILEINFO) );
    SHGetFileInfo( s, 
                   0,
                   &sfi, 
                   sizeof( SHFILEINFO ), 
                   SHGFI_TYPENAME 
                   );

    // Get file type name.  If name is blank, just get the extension
    if( lstrlen( sfi.szTypeName ) )
        m_sTypeName = sfi.szTypeName;
    else
        {
        // Search for last period.  
        int iExtIndex = s.ReverseFind( _T( '.' ) );
        
        // Q: did we find a period?  If not, don't display anything
        if( iExtIndex == -1)
            m_sTypeName.LoadString( IDS_FILE );
        else
            {
            s.MakeUpper();
            LPTSTR p = s.GetBuffer(0);
            m_sTypeName.Format( IDS_FILE_TYPE_FORMAT, &p[iExtIndex + 1] );
            }
        }

    // Get original file name
    LPTSTR p = m_sItemName.GetBuffer( MAX_PATH );
    m_pItem->GetOriginalAnsiFilename( p, MAX_PATH );
    NameReturnFile( p, p );
    m_sItemName.ReleaseBuffer();

    // Get original location
    p = m_sLocation.GetBuffer( MAX_PATH );
    m_pItem->GetOriginalAnsiFilename( p, MAX_PATH );
    NameReturnPath( p, p );
    m_sLocation.ReleaseBuffer();

    // Get size.
    DWORD dwFileSize;
    m_pItem->GetOriginalFilesize( &dwFileSize );
    CRangedString rs( dwFileSize );
    m_sFileSize = rs;

    // Get date strings
    TCHAR szDate[128];
    TCHAR szTime[128];
    SYSTEMTIME tm;
    m_pItem->GetDateQuarantined( &tm );

    // Convert to local time
    FILETIME   ft, lft;
    SystemTimeToFileTime( &tm, &ft );
    FileTimeToLocalFileTime( &ft, &lft );
    FileTimeToSystemTime( &lft, &tm );
    
    GetDateFormat( NULL, 
                   DATE_LONGDATE,
                   &tm,
                   NULL,
                   szDate,
                   128 );

    GetTimeFormat( NULL,
                   0,
                   &tm,
                   NULL,
                   szTime,
                   128 );

    m_sQuarantineDate = szDate;
    m_sQuarantineDate += _T(" ");
    m_sQuarantineDate += szTime;

    m_pItem->GetDateSubmittedToSARC( &tm );

    // Check to make sure this was actually submitted.
    if( tm.wYear == 0 )
        {
        m_sSubDate.LoadString( IDS_NOT_SUBMITTED );
        }
    else
        {
        // Convert to local time
        SystemTimeToFileTime( &tm, &ft );
        FileTimeToLocalFileTime( &ft, &lft );
        FileTimeToSystemTime( &lft, &tm );

        GetDateFormat( NULL, 
                       DATE_LONGDATE,
                       &tm,
                       NULL,
                       szDate,
                       128 );

        GetTimeFormat( NULL,
                       0,
                       &tm,
                       NULL,
                       szTime,
                       128 );

        m_sSubDate = szDate;
        m_sSubDate += _T(" ");
        m_sSubDate += szTime;
        }

    // Get status text
    DWORD dwStatus;
    int iStrID, iStrInfoID;
   
    if( SUCCEEDED( m_pItem->GetFileStatus( &dwStatus ) ) )
        {
        switch( dwStatus )
            {
            case QFILE_STATUS_QUARANTINED: 
                iStrID = IDS_STATUS_QUARANTINED; 
                iStrInfoID = IDS_RECOMMEND_QUARANTINE;
                break;
            case QFILE_STATUS_SUBMITTED:   
                iStrID = IDS_STATUS_SUBMITTED;
                iStrInfoID = IDS_RECOMMEND_SUBMITTED;
                break;
            case QFILE_STATUS_BACKUP_FILE: 
                iStrID = IDS_STATUS_BACKUP;
                iStrInfoID = IDS_RECOMMEND_BACKUP;
                break;
            }

    // Check for compressed file
    if( FileIsCompressed( m_sItemName ) )
        {
        iStrInfoID = IDS_RECOMMEND_COMPRESSED_FILE;
        }

    m_sStatus.LoadString( iStrID );
    m_sStatusInfo.LoadString( iStrInfoID );
    }

    // Blast data to controls
    UpdateData( FALSE );
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

//*************************************************************************
// CGenProp::FileIsCompressed()
//
// CGenProp::FileIsCompressed(
//      LPCTSTR szFileName )
//
// Description:
//
// Returns: BOOL 
//
//*************************************************************************
// 6/8/98 DBUCHES, created - header added.
//*************************************************************************

BOOL CGenProp::FileIsCompressed(LPCTSTR szFileName)
{
    const   char            szCompressedExtList[] = "ZIP.LHA.LZH.ARJ.CAB.MIM.MME.UUE";
    auto    char            szExt[MAX_PATH];

    NameReturnExtensionEx( szFileName, szExt, MAX_PATH );

    strupr( szExt );

    if( ( strlen( szExt ) == 3 ) &&
        ( ( strstr(szCompressedExtList, szExt) ) ||
          ( szExt[2] == '_' ) ) )          // LZ extentions: ".??_"
        {
        return TRUE;
        }

    return FALSE;
}
