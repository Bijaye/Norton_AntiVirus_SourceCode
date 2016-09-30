/*
 * PROPRIETARY/CONFIDENTIAL.  use of this product is subject to license terms.
 * Copyright (c) 2005 Symantec Corporation.  All Rights Reserved.
 *
*/

// GeneralItemPage.cpp : implementation file
//

#include "stdafx.h"
#include "resource.h"
#include "GeneralItemPage.h"
#include "RangeStr.h"
#include "NumberString.h"
#include "FileNameParse.h"
#include "mmc.h"
#include "qscommon.h"
#include "qsfields.h"
#include "macros.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


// Local
DWORD GetQserverVersion( void );


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
        // GET SERVER VERSION
        m_dwQserverVersion = GetQserverVersion();
         
        // Get file name.
        m_sFileName = pItemData->m_sFileName;

        // Get file path
        m_sFilePath = pItemData->m_sFilePath;

        // Get file size info.
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
                       DATE_SHORTDATE,    // DATE_LONGDATE, jhill 4/27/00
                       &pItemData->m_stRecieved, 
                       NULL, szBuffer, MAX_PATH ); 
        m_sQuarantineDate = szBuffer;
        m_sQuarantineDate += _T(" ");
        GetTimeFormat( LOCALE_SYSTEM_DEFAULT,
                       0,
                       &pItemData->m_stRecieved,
                       NULL, szBuffer, MAX_PATH );
        m_sQuarantineDate += szBuffer;

        // Get submission date. 
        if( !pItemData->m_sSubmittedDateStr.IsEmpty() )
        {
            m_sSubmitDate = pItemData->m_sSubmittedDateStr;
            if( m_dwQserverVersion == 1 )
                m_sInformation.LoadString( IDS_RECOMMEND_SUBMITTED );
        }
        else
        {
            m_sSubmitDate.LoadString( IDS_NOT_SUBMITTED );
            if( m_dwQserverVersion == 1 )
                m_sInformation.LoadString( IDS_RECOMMEND_QUARANTINE );
        }
        
        //  jhill 5/4/00 IS THIS VER 2.0
        if( m_dwQserverVersion > 1 )
        {
            // m_GrpBox.ModifyStyle( WS_VISIBLE, 0, 0 );
            //GetDlgItem( IDC_STATIC_RECOMMEND_GRP )->ModifyStyle( WS_VISIBLE, 0, 0 );      // WS_DISABLED
            
            switch( pItemData->m_dwStatus )
            {
                case STATUS_QUARANTINED:
                    m_sInformation.LoadString( IDS_QUARANTINED_RECOMEND );
                    break;

                case STATUS_RELEASED:
                    m_sInformation.LoadString( IDS_RELEASED_RECOMMEND );
                    break;

                case STATUS_UNNEEDED:
                    m_sInformation.LoadString( IDS_UNNEEDED_RECOMMEND );
                    break;

                case STATUS_SUBMITTED:
                    m_sInformation.LoadString( IDS_SUBMITTED_RECOMMEND );
                    break;

                case STATUS_NEEDED:
                    m_sInformation.LoadString( IDS_NEEDED_RECOMMEND );
                    break;

                case STATUS_AVAILIABLE:
                    m_sInformation.LoadString( IDS_AVAILABLE_RECOMMEND );
                    break;

                case STATUS_DISTRIBUTE:
                    m_sInformation.LoadString( IDS_DISTRIBUTE_RECOMMEND );
                    break;

                case STATUS_DISTRIBUTED:
                    m_sInformation.LoadString( IDS_DISTRIBUTED_RECOMMEND );
                    break;

                case STATUS_ATTENTION:
                case STATUS_ERROR:
                    m_sInformation.LoadString( IDS_ERROR_RECOMMEND );
                    break;

                case STATUS_NOTINSTALLED:
                    m_sInformation.LoadString( IDS_NOTINSTALLED_RECOMMEND );
                    break;

                case STATUS_INSTALLED:
                    m_sInformation.LoadString( IDS_INSTALLED_RECOMMEND );
                    break;

                case STATUS_HELD:
                    m_sInformation.LoadString( IDS_HELD_RECOMMEND );
                    break;

                default:
                    m_sInformation.LoadString( IDS_RECOMMEND_NONE );
                    break;
            }
        }


        // Get status text.
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

	try
	{
    Release();
	}
	catch (...)
	{
	}
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


/*----------------------------------------------------------------------------
GetQserverVersion

  QsConsole writes the version of the currently selected
  Qserver here just before my ScopeExtData or AvisConExtData 
  objects are created.
  
	These registry entries are stored on the local console machine.
	
	  Written by: Jim Hill      1/3/00
----------------------------------------------------------------------------*/
DWORD GetQserverVersion( void )
{
    DWORD   dwQserverVersion = 1;
	CRegKey reg;
	LONG    lResult;
	
	
    //  GO RETRIEVE IT FROM LOCAL REGISTRY ENTRY
	lResult = reg.Open( HKEY_LOCAL_MACHINE, REGKEY_QSCONSOLE);
    if( ERROR_SUCCESS == lResult )
	{
		lResult = reg.QueryDWORDValue(REGVALUE_SELECTED_QSERVER_VERSION,dwQserverVersion);
		if(ERROR_SUCCESS != lResult )
		{
            dwQserverVersion = 1;
		}
		reg.Close();
	}
	
    return(dwQserverVersion);
}





BOOL CGeneralItemPage::OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult) 
{
    char szPath[512];
    CString sTopic;
    // INITIALIZE
    HWND hHelpWin = NULL;
    sTopic = _T(" ");

	AFX_MANAGE_STATE(AfxGetStaticModuleState());	 
    NMHDR* pNMHDR = (NMHDR*)lParam;

	if( pNMHDR->code != PSN_HELP)   //********* in "Prsht.h"
    return(S_FALSE);

     CWnd *pwnd = GetActiveWindow();
    sTopic.Format( _T("%s::/%s"), m_sHelpFile, QCONSOLE_HELPTOPIC_SAMPLE_PROP);
#ifdef _UNICODE
     int iRet = WideCharToMultiByte(CP_ACP,0,(LPCWSTR)sTopic.GetBuffer(0),-1,
                                   (LPSTR)szPath,sizeof(szPath),NULL,NULL);
     sTopic.ReleaseBuffer(-1);
     if( iRet == FALSE )
        return(FALSE);
#else
    vpstrncpy( szPath, sTopic.GetBuffer(0), sizeof(szPath) );
    sTopic.ReleaseBuffer(-1);
#endif

    hHelpWin =HtmlHelpA(
                    pwnd->m_hWnd,
                    szPath,                // (LPCTSTR)sTopic,    // "c:\\Help.chm::/Intro.htm"
                    HH_DISPLAY_TOPIC,         // 
                    NULL) ;
	return CPropertyPage::OnNotify(wParam, lParam, pResult);
}
