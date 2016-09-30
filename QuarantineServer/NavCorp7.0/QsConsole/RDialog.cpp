// RDialog.cpp : implementation file
//

#include "stdafx.h"
#include "qscon.h"
#include "QSConsoleData.h"
#include "resource.h"
#include "RDialog.h"
#include "shlobj.h"
#include "filenameparse.h"
#import "qserver.tlb" no_namespace
#include "qsfields.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CRDialog dialog



///////////////////////////////////////////////////////////////////////////////
//
// Function name : CRDialog::CRDialog
//
// Description   : 
//
// Return type   : 
//
// Argument      : CWnd* pParent /*=NULL*/
//
///////////////////////////////////////////////////////////////////////////////
// 2/23/99 - DBUCHES: Function created / header added 
///////////////////////////////////////////////////////////////////////////////
CRDialog::CRDialog(CWnd* pParent /*=NULL*/)
	: CDialog(CRDialog::IDD, pParent)
{
	//{{AFX_DATA_INIT(CRDialog)
	m_sProgressText = _T("");
	m_sDescr = _T("");
	//}}AFX_DATA_INIT

    m_apItems = NULL;
    m_dwNumItems = 0;
    m_bDone = FALSE;
    m_bCancelled = FALSE;
    m_dwFilesSoFar = 0;
    m_pAVScanner = NULL;
    m_pStream = NULL;
}


///////////////////////////////////////////////////////////////////////////////
//
// Function name : CRDialog::~CRDialog
//
// Description   : 
//
// Return type   : 
//
//
///////////////////////////////////////////////////////////////////////////////
// 2/23/99 - DBUCHES: Function created / header added 
///////////////////////////////////////////////////////////////////////////////
CRDialog::~CRDialog()
{
    if( m_apItems )
        delete [] m_apItems;
}


///////////////////////////////////////////////////////////////////////////////
//
// Function name : CRDialog::DoDataExchange
//
// Description   : 
//
// Return type   : void 
//
// Argument      : CDataExchange* pDX
//
///////////////////////////////////////////////////////////////////////////////
// 2/23/99 - DBUCHES: Function created / header added 
///////////////////////////////////////////////////////////////////////////////
void CRDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CRDialog)
	DDX_Control(pDX, IDC_ITEM_LIST, m_ListCtrl);
	DDX_Control(pDX, IDC_PROGRESS, m_progress);
	DDX_Text(pDX, IDC_PROGRESS_STATIC, m_sProgressText);
	DDX_Text(pDX, IDC_DESCR_STATIC, m_sDescr);
	//}}AFX_DATA_MAP
}



///////////////////////////////////////////////////////////////////////////////
//
// Function name : CRDialog::Initialize
//
// Description   : Sets up the copy dialog.
//
// Return type   : HRESULT 
//
// Argument      : CItemData** apItems
// Argument      : DWORD dwNumItems
// Argument      : LPCTSTR szDestFolder
// Argument      : LPUNKNOWN pUnk
// Argument      : BOOL bRepair
//
///////////////////////////////////////////////////////////////////////////////
// 3/9/99 - DBUCHES: Function created / header added 
///////////////////////////////////////////////////////////////////////////////
HRESULT CRDialog::Initialize( CItemData** apItems, DWORD dwNumItems, 
                              LPCTSTR szDestFolder, 
                              LPUNKNOWN pUnk, 
                              CSnapInItem* pRootNode,
                              CAVScanner* pScanner
                              )
{
    HRESULT hr = S_OK;

    // 
    // Save off initialization data.
    // 
    try
        {
       	// 
        // Only repair if we are given a scanner to repair with
        // 
		m_bRepair = pScanner != NULL;
        
		m_dwNumItems = dwNumItems;
        m_apItems = new CItemData*[ dwNumItems ];
        CopyMemory( m_apItems, apItems, sizeof( CItemData* ) * dwNumItems );
        m_sDestFolder = szDestFolder;
        m_pUnk = pUnk;
        m_fetch.m_pIdentity = ((CQSConsoleData*)pRootNode)->m_pQServer.DCO_GetIdentity();
        m_pAVScanner = pScanner;

        // 
        // Marshal interface for repair thread.
        //
        if( m_bRepair )
            hr = CoMarshalInterThreadInterfaceInStream( __uuidof( IQuarantineServer ), ((CQSConsoleData*)pRootNode)->m_pQServer, &m_pStream ); 
        }
    catch(...)
        {
        hr = E_FAIL;
        }

    return hr;
}


BEGIN_MESSAGE_MAP(CRDialog, CDialog)
	//{{AFX_MSG_MAP(CRDialog)
    ON_MESSAGE( WM_RDIALOG_JOB_DONE, OnJobDone )
    ON_MESSAGE( WM_RDIALOG_SET_PROGRESS, OnSetProgress )
    ON_MESSAGE( WM_RDIALOG_FETCH_FILES, OnFetchFiles )
    ON_MESSAGE( WM_RDIALOG_CANT_REPAIR, OnCantRepair )
    ON_MESSAGE( WM_RDIALOG_ERROR, OnThreadError )
    ON_MESSAGE( WM_RDIALOG_ERROR_NO_FILE, OnErrorNoFile )
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CRDialog message handlers




///////////////////////////////////////////////////////////////////////////////
//
// Function name : CRDialog::OnInitDialog
//
// Description   : 
//
// Return type   : BOOL 
//
//
///////////////////////////////////////////////////////////////////////////////
// 2/23/99 - DBUCHES: Function created / header added 
///////////////////////////////////////////////////////////////////////////////
BOOL CRDialog::OnInitDialog() 
{
	CDialog::OnInitDialog();

	// 
    // Set up controls
    // 
    m_progress.SetRange( 0, (short)m_dwNumItems );
    m_progress.SetPos( 0 );
    m_progress.SetStep(1);

    if( m_bRepair )
        {
        CString s;
        ((CStatic*)GetDlgItem( IDC_ICON_STATIC ) )->SetIcon( LoadIcon( AfxGetResourceHandle(), MAKEINTRESOURCE( IDI_REPAIR ) ) );
        m_sDescr.Format( IDS_REPAIR_DESCR, m_sDestFolder );
        s.LoadString( IDS_REPAIR_ITEMS );
        SetWindowText( s );
        }
    else
        {
        m_sDescr.Format( IDS_RESTORE_DESCR, m_sDestFolder );
        }

    // 
    // Use handle of image list for our listview.
    // 
    SHFILEINFO sfi;
    ZeroMemory( &sfi, sizeof(SHFILEINFO) );
    HIMAGELIST hSmallImageList = (HIMAGELIST)SHGetFileInfo( _T("Foo.EXE"), 
                   FILE_ATTRIBUTE_NORMAL,
                   &sfi, 
                   sizeof( SHFILEINFO ), 
                   SHGFI_SYSICONINDEX | SHGFI_ICON | SHGFI_SMALLICON | SHGFI_USEFILEATTRIBUTES 
                   );
    
    ListView_SetImageList( m_ListCtrl.GetSafeHwnd(), hSmallImageList, LVSIL_SMALL );
    
    // 
    // Set up column headers
    // 
    CString s;
    CRect rect;
    m_ListCtrl.GetClientRect( &rect );
    
    s.LoadString( IDS_RESTORE_FILENAME_HEADER );
    m_ListCtrl.InsertColumn( RDIALOG_COL_FILENAME, s, LVCFMT_LEFT, rect.Width() / 4 );

    s.LoadString( IDS_DESTINATION_DIR );
    m_ListCtrl.InsertColumn( RDIALOG_COL_DESTINATION, s, LVCFMT_LEFT, rect.Width() / 4 * 2 );

    s.LoadString( IDS_RESTORE_STATUS_HEADER );
    m_ListCtrl.InsertColumn( RDIALOG_COL_STATUS, s, LVCFMT_LEFT, rect.Width() / 4 );

    // 
    // Add column data.
    //
    s.LoadString( IDS_WORKING );
    for( DWORD i = 0; i < m_dwNumItems; i++ )
        {
        // 
        // Extract icon image for this item
        // 
        ZeroMemory( &sfi, sizeof(SHFILEINFO) );
        SHGetFileInfo( m_apItems[i]->m_sFileName, 
                       FILE_ATTRIBUTE_NORMAL,
                       &sfi, 
                       sizeof( SHFILEINFO ), 
                       SHGFI_SYSICONINDEX | SHGFI_ICON | SHGFI_SMALLICON | SHGFI_USEFILEATTRIBUTES 
                       );

        // 
        // Add file name.
        // 
        int iIndex = m_ListCtrl.InsertItem( 0, m_apItems[i]->m_sFileName, sfi.iIcon );

        // 
        // Set text to working.
        // 
        m_ListCtrl.SetItemText( iIndex, RDIALOG_COL_STATUS, s );

        // 
        // Store pointer to item.
        // 
        m_ListCtrl.SetItemData( iIndex, (DWORD) m_apItems[i] );
        }
    
    // 
    // Set progress text
    // 
    m_sProgressText.Format( IDS_PROGRESS_FORMAT, m_dwFilesSoFar, m_dwNumItems );

    // 
    // Blast data to screen.
    // 
    UpdateData( FALSE );

    // 
    // Start getting files
    // 
    PostMessage( WM_RDIALOG_FETCH_FILES );

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}



///////////////////////////////////////////////////////////////////////////////
//
// Function name : CRDialog::OnCancel
//
// Description   : 
//
// Return type   : void 
//
//
///////////////////////////////////////////////////////////////////////////////
// 3/2/99 - DBUCHES: Function created / header added 
///////////////////////////////////////////////////////////////////////////////
void CRDialog::OnCancel()
{
    // 
    // Cancel running thread
    // 
    m_bCancelled = TRUE;

    if( !m_bDone )
        m_fetch.Abort();

    CDialog::OnCancel();

}

///////////////////////////////////////////////////////////////////////////////
//
// Function name : OnJobDone::OnFetchFiles
//
// Description   : 
//
// Return type   : LRESULT 
//
// Argument      : WPARAM wParam
// Argument      : LPARAM lParam
//
///////////////////////////////////////////////////////////////////////////////
// 2/23/99 - DBUCHES: Function created / header added 
///////////////////////////////////////////////////////////////////////////////
LRESULT CRDialog::OnJobDone( WPARAM wParam, LPARAM lParam )
{
    // 
    // Set control text
    // 
    CString s( (LPCTSTR) IDS_CLOSE );
    GetDlgItem( IDCANCEL )->SetWindowText( s );
   
    s.LoadString( IDS_ACTION_COMPLETED );
    GetDlgItem( IDC_PROGRESS_STATIC )->SetWindowText( s );

    m_bDone = TRUE;
    return 0;
}


///////////////////////////////////////////////////////////////////////////////
//
// Function name : CRDialog::OnFetchFiles
//
// Description   : 
//
// Return type   : LRESULT 
//
// Argument      : WPARAM wParam
// Argument      : LPARAM lParam
//
///////////////////////////////////////////////////////////////////////////////
// 3/2/99 - DBUCHES: Function created / header added 
///////////////////////////////////////////////////////////////////////////////
LRESULT CRDialog::OnFetchFiles( WPARAM wParam, LPARAM lParam )
{
    // 
    // Create array of file IDs
    //
    DWORD* pdwFileIDs = NULL;
    try
        {
        pdwFileIDs = new DWORD[ m_dwNumItems ];
        for( DWORD i = 0; i < m_dwNumItems; i++ )
            {
            pdwFileIDs[i] = m_apItems[i]->m_dwItemID;
            }

        CString sTemp;
        GetTempPath( MAX_PATH, sTemp.GetBuffer(MAX_PATH) );
        sTemp.ReleaseBuffer();

        if( FAILED( m_fetch.GetFiles( pdwFileIDs, m_dwNumItems, m_pUnk, sTemp, CRDialog::CopyCallback, (DWORD) this ) ) )
            throw;
        }
    catch(...)
        {
        // 
        // Display message box.
        // 
        OnThreadError( 0, 0 );
        }

    // 
    // Cleanup
    // 
    if( pdwFileIDs != NULL )
        delete [] pdwFileIDs;       
    
    return 0;
}


///////////////////////////////////////////////////////////////////////////////
//
// Function name : CRDialog::OnSetProgress
//
// Description   : 
//
// Return type   : LRESULT 
//
// Argument      : WPARAM wParam
// Argument      : LPARAM lParam
//
///////////////////////////////////////////////////////////////////////////////
// 3/2/99 - DBUCHES: Function created / header added 
///////////////////////////////////////////////////////////////////////////////
LRESULT CRDialog::OnCantRepair( WPARAM wParam, LPARAM lParam )
{
    // 
    // Set progress text
    // 
    CString s;
    s.Format( IDS_PROGRESS_FORMAT, ++m_dwFilesSoFar, m_dwNumItems );
    GetDlgItem( IDC_PROGRESS_STATIC )->SetWindowText( s );
    m_progress.StepIt();

    // 
    // Update item text.
    // 
    LVFINDINFO findInfo = { 0 };
    findInfo.flags = LVFI_PARAM | LVFI_WRAP;
    findInfo.lParam = wParam;
    int iIndex = m_ListCtrl.FindItem( &findInfo );
    if( iIndex == -1 )
        return 0;

    s.LoadString( IDS_NOACTION );
    m_ListCtrl.SetItemText( iIndex, RDIALOG_COL_STATUS, s );
    s.LoadString( lParam == 0 ? IDS_NO_VIRUS_FOUND : IDS_CANT_REPAIR );
    m_ListCtrl.SetItemText( iIndex, RDIALOG_COL_DESTINATION, s );

    return 0;
}




///////////////////////////////////////////////////////////////////////////////
//
// Function name : CRDialog::OnSetProgress
//
// Description   : 
//
// Return type   : LRESULT 
//
// Argument      : WPARAM wParam
// Argument      : LPARAM lParam
//
///////////////////////////////////////////////////////////////////////////////
// 3/2/99 - DBUCHES: Function created / header added 
///////////////////////////////////////////////////////////////////////////////
LRESULT CRDialog::OnSetProgress( WPARAM wParam, LPARAM lParam )
{
    // 
    // Set progress text
    // 
    CString s;
    s.Format( IDS_PROGRESS_FORMAT, ++m_dwFilesSoFar, m_dwNumItems );
    GetDlgItem( IDC_PROGRESS_STATIC )->SetWindowText( s );
    m_progress.StepIt();

    // 
    // Update item text.
    // 
    LVFINDINFO findInfo = { 0 };
    findInfo.flags = LVFI_PARAM | LVFI_WRAP;
    findInfo.lParam = wParam;
    int iIndex = m_ListCtrl.FindItem( &findInfo );
    if( iIndex == -1 )
        return 0;

    if( m_bRepair )
        s.LoadString( IDS_REPAIRED );
    else
        s.LoadString( IDS_RESTORED );

    m_ListCtrl.SetItemText( iIndex, RDIALOG_COL_STATUS, s );

    // 
    // Set destination text.
    // 
    LPTSTR pszDest = (LPTSTR) lParam;
    if( pszDest )
        {
        m_ListCtrl.SetItemText( iIndex, RDIALOG_COL_DESTINATION, pszDest );
        delete [] pszDest;
        }

    return 0;
}



///////////////////////////////////////////////////////////////////////////////
//
// Function name : CRDialog::OnThreadError
//
// Description   : Handles errors in restore thread.
//
// Return type   : LRESULT 
//
// Argument      : WPARAM wParam
// Argument      : LPARAM lParam
//
///////////////////////////////////////////////////////////////////////////////
// 3/3/99 - DBUCHES: Function created / header added 
///////////////////////////////////////////////////////////////////////////////
LRESULT CRDialog::OnThreadError( WPARAM wParam, LPARAM lParam )
{
    // 
    // Blast error.
    // 
    CString sText, sTitle;
    sTitle.LoadString( IDS_ERROR_TITLE );
    sText.LoadString( IDS_ERROR_FETCHING_FILES );
    MessageBox( sText, sTitle, MB_ICONEXCLAMATION | MB_OK );

    return 0;
}


///////////////////////////////////////////////////////////////////////////////
//
// Function name: CRDialog::RepairFile
//
// Description  : 
//
// Return type  : BOOL 
//
// Argument     : CItemData* pItem
// Argument     : LPCTSTR szTempFileName
//
///////////////////////////////////////////////////////////////////////////////
// 3/10/99 - DBUCHES: Function created / header added 
///////////////////////////////////////////////////////////////////////////////
BOOL CRDialog::RepairFile( CItemData* pItem, LPCTSTR szTempFileName, BOOL *pbRepaired )
{
USES_CONVERSION;    
    
    HRESULT hr;
    DWORD dwVirusID = 0;
    LPVOID pVirus = NULL;

    try
        {
        *pbRepaired = FALSE;

        // 
        // If this is the first time thru here, we need to unmarshal our interface.
        // 
        if( m_pQServer == NULL )
            {
            hr = CoGetInterfaceAndReleaseStream( m_pStream, __uuidof( IQuarantineServer ), (LPVOID*) &m_pQServer );
            if( FAILED( hr ) )
                {
                ASSERT(FALSE);
                return FALSE;
                }
    
            // 
            // Need to set proxy blanket
            // 
            if( FAILED( CoSetProxyBlanket( m_pQServer,
                               RPC_C_AUTHN_WINNT,
                               RPC_C_AUTHZ_NONE,
                               NULL,
                               RPC_C_AUTHN_LEVEL_CONNECT,
                               RPC_C_IMP_LEVEL_IMPERSONATE,
                               (_COAUTHIDENTITY*) m_fetch.m_pIdentity,
                               EOAC_NONE) ) )
                {
                ASSERT(FALSE);
                return FALSE;
                }
            }

        // 
        // First, try scanning the file for viruses.
        //
        hr = m_pAVScanner->ScanFile( szTempFileName, &pVirus );
        if( FAILED( hr ) )
            {
            ASSERT( FALSE );
            PostMessage( WM_RDIALOG_CANT_REPAIR, (WPARAM) pItem );
            return FALSE;
            }

        // 
        // If there is no virus, don't do anything.
        // 
        if( pVirus == NULL )
            {
            PostMessage( WM_RDIALOG_CANT_REPAIR, (WPARAM) pItem );
            return TRUE;
            }

        // 
        // Save off the virus name and ID.
        // 
        
        m_pAVScanner->GetVirusID( pVirus, dwVirusID );
        
        // 
        // Only save the item if the virus has changed.
        // 
        if( dwVirusID != pItem->m_dwVirusID )
            {
            m_pAVScanner->GetVirusName( pVirus, pItem->m_sVirusName.GetBuffer(MAX_PATH), MAX_PATH );
            pItem->m_sVirusName.ReleaseBuffer();
            pItem->m_dwVirusID = dwVirusID;

            // 
            // Now, we need to update the virus ID on the quarantine server.
            // 
            CComPtr< IQuarantineServerItem > pQSItem;
            if( FAILED( m_pQServer->GetQuarantineItem( pItem->m_dwItemID, &pQSItem ) ) )
                return FALSE;

            // 
            // Need to set proxy blanket
            // 
            if( FAILED( CoSetProxyBlanket( pQSItem,
                               RPC_C_AUTHN_WINNT,
                               RPC_C_AUTHZ_NONE,
                               NULL,
                               RPC_C_AUTHN_LEVEL_CONNECT,
                               RPC_C_IMP_LEVEL_IMPERSONATE,
                               (_COAUTHIDENTITY*) m_fetch.m_pIdentity,
                               EOAC_NONE) ) )
                {
                ASSERT(FALSE);
                return FALSE;
                }

            // 
            // Set the item data.
            //
            VARIANT v;
            v.vt = VT_BSTR;
            v.bstrVal = ::SysAllocStringLen( pItem->m_sVirusName, pItem->m_sVirusName.GetLength() );  
            hr = pQSItem->SetValue( A2BSTR( QSERVER_ITEM_INFO_VIRUSNAME ), v );
            if( FAILED( hr ) )
                {
                ASSERT(FALSE);
                return FALSE;
                }
            
            VariantClear( &v );
            v.vt = VT_UI4;
            v.ulVal = dwVirusID;
            hr = pQSItem->SetValue( A2BSTR( QSERVER_ITEM_INFO_VIRUSID ), v );
            if( FAILED( hr ) )
                {
                ASSERT(FALSE);
                return FALSE;
                }

            // 
            // Commit changes
            // 
            hr = pQSItem->Commit();
            if( FAILED( hr ) )
                {
                ASSERT(FALSE);
                return FALSE;
                }
            }

        // 
        // Try repairing the file.
        // 
        hr = m_pAVScanner->RepairFile( szTempFileName );
        if( FAILED( hr ) )
            {
            // 
            // bugbug - need error reporting
            // 
            ASSERT( FALSE );
            return FALSE;
            }

        // 
        // Could not repair file, nothing to do here.
        // 
        if( hr == S_FALSE )
            {
            PostMessage( WM_RDIALOG_CANT_REPAIR, (WPARAM) pItem, (LPARAM) TRUE );
            return TRUE;
            }

        // 
        // Looks like repair succeeded.
        // 
        *pbRepaired = TRUE;
        m_pAVScanner->FreeVirus( pVirus );
        pVirus = NULL;
        return TRUE;
        }
    catch( _com_error e )
        {
        // 
        // Bugbug - need errror log here
        // 
        ASSERT(FALSE);
        }
    catch(...)
        {
        // 
        // Bugbug - need errror log here
        // 
        ASSERT(FALSE);
        }

    // 
    // Cleanup
    // 
    if( pVirus )
        m_pAVScanner->FreeVirus( pVirus );

    return FALSE;
}


///////////////////////////////////////////////////////////////////////////////
//
// Function name : CRDialog::CopyCallback
//
// Description   : 
//
// Return type   : BOOL 
//
// Argument      : DWORD dwFileID
// Argument      : DWORD dwPercentComplete
// Argument      : DWORD dwStatus
// Argument      : DWORD dwUserData
//
///////////////////////////////////////////////////////////////////////////////
// 3/1/99 - DBUCHES: Function created / header added 
///////////////////////////////////////////////////////////////////////////////
BOOL CRDialog::CopyCallback( DWORD dwMessage,
                             DWORD dwFileID, 
                             LPCTSTR szTempFileName,
                             DWORD dwUserData )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

    // 
    // Get pointer to dialog.
    // 
    CRDialog* pThis = (CRDialog*) dwUserData;
    BOOL    bRepaired = TRUE;
    

    // 
    // Check for cancel.
    // 
    if( pThis->m_bCancelled )
        return FALSE;

    switch( dwMessage )
        {
        case COPY_STATUS_FILE_DONE:
            {
            // 
            // Find the item in question.
            // 
            CItemData* p = pThis->GetItemByID( dwFileID );
            if( p == NULL )
                {
                pThis->PostMessage( WM_COMMAND, IDCANCEL, 0L );
                ASSERT( FALSE );
                return FALSE;
                }

            // 
            // Q: Are we repairing this file?
            // 
            if( pThis->m_bRepair )
                {
                // 
                // Attempt repair.
                //
                if( pThis->RepairFile( p, szTempFileName, &bRepaired ) == FALSE )
                    return FALSE;

                // 
                // If the file has not been repaired, return just return.
                // 
                if( bRepaired == FALSE )
                    {
                    return TRUE;
                    }

                }

            // 
            // Move file to it's final destination.
            //
            CString sSavePath;
            if( FALSE == pThis->GetSaveLocation( p->m_sFileName, pThis->m_sDestFolder, sSavePath) )
                {
                // 
                // Signal an abort.
                // 
                pThis->PostMessage( WM_COMMAND, IDCANCEL, 0L );
                return FALSE;
                }

            // 
            // Move it.
            // 
            if( FALSE == CopyFile( szTempFileName, sSavePath, FALSE ) )
                {
                pThis->m_pAVScanner->DeleteFile( szTempFileName );
                pThis->PostMessage( WM_RDIALOG_ERROR );
                return FALSE;
                }
            else
                {
                pThis->m_pAVScanner->DeleteFile( szTempFileName );
                }

            // 
            // Get name of destination folder.
            // 
            LPTSTR pszSaveLocation = new TCHAR[ MAX_PATH ];
            CFileNameParse::GetFilePath( sSavePath, pszSaveLocation, MAX_PATH ); 

            // 
            // Update progress.
            // 
            pThis->PostMessage( WM_RDIALOG_SET_PROGRESS, (WPARAM) p, (LPARAM) pszSaveLocation );
            }
            return TRUE;

        case COPY_STATUS_JOB_DONE:
            {
            // 
            // Communicate change to dialog.
            // 
            pThis->PostMessage( WM_RDIALOG_JOB_DONE );
            }
            return TRUE;

        case COPY_STATUS_ERROR_DISK_SPACE:
            {
            pThis->PostMessage( WM_RDIALOG_ERROR );
            }
            return FALSE;
        
        case COPY_STATUS_ERROR:
            {
            pThis->PostMessage( WM_RDIALOG_ERROR );
            }
            return FALSE;

        case COPY_STATUS_FILE_DOES_NOT_EXIST:
            {
            pThis->PostMessage( WM_RDIALOG_ERROR_NO_FILE, dwFileID );
            }
            return TRUE;
        }

    // 
    // All good.
    // 
    return TRUE;
}


///////////////////////////////////////////////////////////////////////////////
//
// Function name : CRDialog::GetItemByID
//
// Description   : Search list of items for a particular file ID.
//
// Return type   : CItemData* 
//
// Argument      : DWORD dwID
//
///////////////////////////////////////////////////////////////////////////////
// 3/2/99 - DBUCHES: Function created / header added 
///////////////////////////////////////////////////////////////////////////////
CItemData* CRDialog::GetItemByID( DWORD dwID )
{
    for( DWORD i = 0; i < m_dwNumItems; i++ )
        {
        if( m_apItems[i]->m_dwItemID == dwID )
            return m_apItems[i];
        }

    return NULL;
}

////////////////////////////////////////////////////////////////////////////
// Function name    : CRDialog::GetSaveLocation
//
// Description      : Prompts user for save location
//
// Return type      : BOOL 
//
// Argument         : CString& sFilePath
// Argument         : CString& sOriginalFileName
// Argument         : CString& sFileName
//
////////////////////////////////////////////////////////////////////////////
// 5/13/98 DBUCHES - Function created / Header added.
////////////////////////////////////////////////////////////////////////////
BOOL CRDialog::GetSaveLocation( LPCTSTR szFileName, 
                                LPCTSTR szSaveFolder, 
                                CString& sFullSavePath )
{
    CString sFilter((LPCTSTR) IDS_SAVE_AS_FILTER);
    CString sTitle, sText;
    DWORD dwAttr;

    // 
    // Construct full save path.
    //
    sFullSavePath = CFileNameParse::AppendFile( szSaveFolder, szFileName );

    // 
    // Make sure original directory actually exists.  
    // 
    dwAttr = GetFileAttributes( szSaveFolder );
    
    // 
    // If it does, check to see if we can save the file here.
    // 
    if( dwAttr != 0xFFFFFFFF )
        {
        // 
        // Check to see if we need to prompt user.
        // 
        dwAttr = GetFileAttributes( sFullSavePath );
        if( dwAttr == 0xFFFFFFFF )
            {
            // 
            // File does not exist yet so we should be good to go.
            // 
            return TRUE;
            }
        }

    // 
    // Make sure original directory actually exists.  
    // 
    dwAttr = GetFileAttributes( szSaveFolder );
    if( dwAttr == 0xFFFFFFFF || !(dwAttr & FILE_ATTRIBUTE_DIRECTORY) )
        
        {
        // 
        // Ask user where file should be saved
        // 
        CFileDialog dlg( FALSE,                 // Save
                         NULL,                  // No default extension
                         szFileName,            // Initial file name
                         OFN_FILEMUSTEXIST |    // Flags
                         OFN_HIDEREADONLY |
                         OFN_PATHMUSTEXIST,
                         sFilter,               // Filter string
                         this );                // Parent window

        // Fire off dialog
        if( dlg.DoModal() == IDCANCEL )
            {
            return FALSE;
            }

        // Save off file name and path
        sFullSavePath = dlg.GetPathName();
        }

    // If the file already exists, ask the user if he wants to
    // overwrite the file.
    dwAttr = GetFileAttributes( sFullSavePath );
    while( dwAttr != 0xFFFFFFFF )
        {
        // Prompt the user for overwrite
        sTitle.LoadString( IDS_RESTORE_OVERWRITE_TITLE );
        sText.Format( IDS_RESTORE_OVERWRITE_FORMAT, sFullSavePath );
        if( MessageBox( sText, sTitle, MB_ICONWARNING | MB_YESNO ) != IDYES )
            {
            CFileDialog dlg( FALSE,                 // Save
                             NULL,                  // No default extension
                             sFullSavePath,         // Initial file name
                             OFN_FILEMUSTEXIST |    // Flags
                             OFN_HIDEREADONLY |
                             OFN_PATHMUSTEXIST,
                             sFilter,               // Filter string
                             this );                // Parent window

            // Fire off dialog
            if( dlg.DoModal() == IDCANCEL )
                {
                return FALSE;
                }

            // Save off file name and path
            sFullSavePath = dlg.GetPathName();
            }
        else
            {
            // User wants to overwrite this file.
            break;
            }

        // Get file attributes of this file
        dwAttr = GetFileAttributes( sFullSavePath );
        }

    return TRUE;

}



///////////////////////////////////////////////////////////////////////////////
//
// Function name: CRDialog::OnErrorNoFile
//
// Description  : Display a dialog telling the user that one of the files
//                being copied from the server does not exist.
//
// Return type  : LRESULT 
//
// Argument     : WPARAM wParam
// Argument     : LPARAM lParam
//
///////////////////////////////////////////////////////////////////////////////
// 3/24/99 - DBUCHES: Function created / header added 
///////////////////////////////////////////////////////////////////////////////
LRESULT CRDialog::OnErrorNoFile( WPARAM wParam, LPARAM lParam )
{
    CString sTitle((LPCTSTR) IDS_ERROR_TITLE );
    CString sText;

    // 
    // Locate the item in question.
    // 
    CItemData* pItem = GetItemByID( wParam );
    if( pItem == NULL )
        {
        // 
        // Should never get here.
        // 
        ASSERT(FALSE);
        return 0;
        }

    // 
    // Set progress text
    // 
    CString s;
    s.Format( IDS_PROGRESS_FORMAT, ++m_dwFilesSoFar, m_dwNumItems );
    GetDlgItem( IDC_PROGRESS_STATIC )->SetWindowText( s );
    m_progress.StepIt();


    // 
    // Format string and tell user about the bad news.
    // 
    sText.Format( IDS_ERROR_FILE_GONE_FMT, pItem->m_sFileName );
    MessageBox( sText, sTitle, MB_ICONEXCLAMATION | MB_OK );

    // 
    // Remove the file from the list view.
    // 
    LVFINDINFO findInfo = { 0 };
    findInfo.flags = LVFI_PARAM | LVFI_WRAP;
    findInfo.lParam = (long)pItem;
    int iIndex = m_ListCtrl.FindItem( &findInfo );
    if( iIndex == -1 )
        return 0;
    
    s.LoadString( IDS_NOACTION );
    m_ListCtrl.SetItemText( iIndex, RDIALOG_COL_STATUS, s );
    s.LoadString( IDS_FILE_DOES_NOT_EXIST );
    m_ListCtrl.SetItemText( iIndex, RDIALOG_COL_DESTINATION, s );

    return 0;

}



