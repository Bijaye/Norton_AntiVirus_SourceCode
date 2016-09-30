// GeneralItemPage.cpp : implementation file
//

#include "stdafx.h"
#include "resource.h"
#include "GeneralItemPage.h"
#include "RangeStr.h"
#include "NumberString.h"
#include "FileNameParse.h"
#include "mmc.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CGeneralItemPage property page

IMPLEMENT_DYNCREATE(CGeneralItemPage, CQPropPage)

CGeneralItemPage::CGeneralItemPage() : CQPropPage(CGeneralItemPage::IDD)
{
	//{{AFX_DATA_INIT(CGeneralItemPage)
	m_sFileName = _T("");
	m_sFilePath = _T("");
	m_sType = _T("");
	m_sSubmitDate = _T("");
	m_sQuarantineDate = _T("");
	m_sFileSize = _T("");
	m_sStatusText = _T("");
	m_sInformation = _T("");
	m_sSubmittedBy = _T("");
	//}}AFX_DATA_INIT
    m_dwRef = 1;
    m_lNotifyHandle = 0;
}

CGeneralItemPage::~CGeneralItemPage()
{
}

void CGeneralItemPage::DoDataExchange(CDataExchange* pDX)
{
	CQPropPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CGeneralItemPage)
	DDX_Control(pDX, IDC_ICON_BUTTON, m_iconBtn);
	DDX_Text(pDX, IDC_NAME_EDIT, m_sFileName);
	DDX_Text(pDX, IDC_LOCATION_EDIT, m_sFilePath);
	DDX_Text(pDX, IDC_TYPE_EDIT, m_sType);
	DDX_Text(pDX, IDC_SUBMITTED_EDIT, m_sSubmitDate);
	DDX_Text(pDX, IDC_QUARANTINE_EDIT, m_sQuarantineDate);
	DDX_Text(pDX, IDC_SIZE_EDIT, m_sFileSize);
	DDX_Text(pDX, IDC_STATUS_EDIT, m_sStatusText);
	DDX_Text(pDX, IDC_INFORMATION_EDIT, m_sInformation);
	DDX_Text(pDX, IDC_SUBMITTEDBY_EDIT, m_sSubmittedBy);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CGeneralItemPage, CQPropPage)
	//{{AFX_MSG_MAP(CGeneralItemPage)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CGeneralItemPage message handlers


///////////////////////////////////////////////////////////////////////////////
//
// Function name : CGeneralItemPage::Initialize
//
// Description   : Initializes all field values for this page.
//
// Return type   : HRESULT 
//
// Argument      : IUnknown* pUnk
//
///////////////////////////////////////////////////////////////////////////////
// 2/9/99 - DBUCHES: Function created / header added 
///////////////////////////////////////////////////////////////////////////////
HRESULT CGeneralItemPage::Initialize( CItemData* pItemData )
{
    HRESULT hr = S_OK;
    CString s;
    try
        {
        // 
        // Get file name.
        // 
        m_sFileName = pItemData->m_sFileName;

        // 
        // Get file path
        // 
        m_sFilePath = pItemData->m_sFilePath;

        // 
        // Get file size info.
        // 
        CRangedString rstr;
        CNumberString numStr( pItemData->m_ulFileSize );
        rstr.SetRangedString( pItemData->m_ulFileSize );
        if( pItemData->m_ulFileSize < 1024 )
            m_sFileSize = rstr;
        else
            {
            m_sFileSize.Format( IDS_BYTES, rstr, (LPCTSTR) numStr );
            }
                    
        // 
        // Get type string.
        // 
        SHFILEINFO sfi;
        ZeroMemory( &sfi, sizeof(SHFILEINFO) );
        SHGetFileInfo( m_sFileName, 
                       FILE_ATTRIBUTE_NORMAL,
                       &sfi, 
                       sizeof( SHFILEINFO ), 
                       SHGFI_ICON | SHGFI_LARGEICON | SHGFI_TYPENAME | SHGFI_USEFILEATTRIBUTES 
                       );

        // 
        // Save off icon.
        // 
        m_iconBtn.SetIcon( sfi.hIcon );

        // 
        // Get file type name.  If name is blank, just get the extension
        //  
        if( lstrlen( sfi.szTypeName ) )
            m_sType = sfi.szTypeName;
        else
            {
            // 
            // File type is unknown, so build from extension.
            // 
            CFileNameParse::GetFileExtension( m_sFileName, s.GetBuffer( MAX_PATH ), MAX_PATH );
            s.ReleaseBuffer();

            if( s.GetLength() )
                {
                s.MakeUpper();
                m_sType.Format( IDS_FILE_TYPE, s );
                }
            else
                {
                // 
                // No extension, too bad.
                // 
                m_sType.LoadString( IDS_FILE );
                }
            }

        // 
        // Get quarantine date.
        //
        TCHAR szBuffer[ MAX_PATH ];
        GetDateFormat( LOCALE_SYSTEM_DEFAULT,
                       DATE_LONGDATE, 
                       &pItemData->m_stRecieved, 
                       NULL, szBuffer, MAX_PATH ); 
        m_sQuarantineDate = szBuffer;
        m_sQuarantineDate += _T(" ");
        GetTimeFormat( LOCALE_SYSTEM_DEFAULT,
                       0,
                       &pItemData->m_stRecieved,
                       NULL, szBuffer, MAX_PATH );
        m_sQuarantineDate += szBuffer;
                        
        // 
        // Get submission date. 
        // 
        if( pItemData->m_stSubmitted.wYear )
            {
            GetDateFormat( LOCALE_SYSTEM_DEFAULT,
                           DATE_LONGDATE, 
                           &pItemData->m_stSubmitted, 
                           NULL, szBuffer, MAX_PATH ); 
            m_sSubmitDate = szBuffer;
            m_sSubmitDate += _T(" ");
            GetTimeFormat( LOCALE_SYSTEM_DEFAULT,
                           0,
                           &pItemData->m_stSubmitted,
                           NULL, szBuffer, MAX_PATH );
            m_sSubmitDate += szBuffer;
            m_sSubmittedBy = pItemData->m_sSubmittedBy;
            m_sInformation.LoadString( IDS_RECOMMEND_SUBMITTED );
            }
        else
            {
            m_sSubmitDate.LoadString( IDS_NOT_SUBMITTED );
            m_sSubmittedBy.LoadString( IDS_NA );
            m_sInformation.LoadString( IDS_RECOMMEND_QUARANTINE );
            }

        // 
        // Get status text.
        // 
        m_sStatusText = pItemData->m_sStatusText;
        }
    catch(...)
        {
        hr = E_UNEXPECTED;
        }

    return hr;
}


///////////////////////////////////////////////////////////////////////////////
//
// Function name : CGeneralItemPage::PostNcDestroy
//
// Description   : Need to destroy ourselves here.
//
// Return type   : void 
//
//
///////////////////////////////////////////////////////////////////////////////
// 2/9/99 - DBUCHES: Function created / header added 
///////////////////////////////////////////////////////////////////////////////
void CGeneralItemPage::PostNcDestroy() 
{
	// TODO: Add your specialized code here and/or call the base class
	
	CQPropPage::PostNcDestroy();

    // 
    // Signal that that we are going away, and that all pages should be destroyed.
    // 
    MMCPropertyChangeNotify( m_lNotifyHandle, m_lParam );
    MMCFreeNotifyHandle(m_lNotifyHandle);

    Release();
}



///////////////////////////////////////////////////////////////////////////////
//
// Function name: CGeneralItemPage::OnInitDialog
//
// Description  : Bump reference count of this dialog.
//
// Return type  : BOOL 
//
//
///////////////////////////////////////////////////////////////////////////////
// 4/2/99 - DBUCHES: Function created / header added 
///////////////////////////////////////////////////////////////////////////////
BOOL CGeneralItemPage::OnInitDialog() 
{
	CQPropPage::OnInitDialog();
	
	m_dwRef ++;
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
