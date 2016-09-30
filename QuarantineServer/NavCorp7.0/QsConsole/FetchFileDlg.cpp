// FetchFileDlg.cpp : implementation file
//

#include "stdafx.h"
#include "qscon.h"
#include "QSConsoleData.h"
#include "FetchFileDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CFetchFileDlg dialog


CFetchFileDlg::CFetchFileDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CFetchFileDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CFetchFileDlg)
	m_sProgressText = _T("");
	//}}AFX_DATA_INIT 
    m_adwItemIDs = NULL;
    m_dwNumItems = 0;
    m_bCancelled = FALSE;
    m_dwFilesSoFar = 0;
}

CFetchFileDlg::~CFetchFileDlg()
{
    // 
    // Cleanup.
    // 
    if( m_adwItemIDs )
        {
        delete [] m_adwItemIDs;
        m_adwItemIDs = NULL;
        }

    for( DWORD i = 0; i < m_dwNumItems; i ++ )
        {
        delete m_aFileDataArray[i];
        }
    
}


///////////////////////////////////////////////////////////////////////////////
//
// Function name: CFetchFileDlg::DoDataExchange
//
// Description  : 
//
// Return type  : void 
//
// Argument     : CDataExchange* pDX
//
///////////////////////////////////////////////////////////////////////////////
// 3/16/99 - DBUCHES: Function created / header added 
///////////////////////////////////////////////////////////////////////////////
void CFetchFileDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CFetchFileDlg)
	DDX_Control(pDX, IDC_PROGRESS, m_progress);
	DDX_Text(pDX, IDC_PROGRESS_STATIC, m_sProgressText);
	//}}AFX_DATA_MAP
}



///////////////////////////////////////////////////////////////////////////////
//
// Function name: CFetchFileDlg::Initialize
//
// Description  : 
//
// Return type  : HRESULT 
//
// Argument     : DWORD * adwItemIDs
// Argument     : DWORD dwNumItems
// Argument     : LPUNKNOWN pCopyUnk
//
///////////////////////////////////////////////////////////////////////////////
// 3/16/99 - DBUCHES: Function created / header added 
///////////////////////////////////////////////////////////////////////////////
HRESULT CFetchFileDlg::Initialize( 
                    CSnapInItem* pRootNode,
                    CItemData** apItem, 
                    DWORD dwNumItems,
                    LPUNKNOWN pCopyUnk
                    )
{
    HRESULT hr = S_OK;
    
    // 
    // Save off input.
    // 
    try
        {
        m_pUnk = pCopyUnk;
        m_adwItemIDs = new DWORD[ dwNumItems ];
        for( DWORD i = 0; i < dwNumItems; i ++ )
            m_adwItemIDs[i] = apItem[i]->m_dwItemID;
        m_dwNumItems = dwNumItems;
        m_aFileDataArray.SetSize( dwNumItems );
        m_sProgressText.Format( IDS_PROGRESS_FORMAT, 0, dwNumItems );
        m_fetch.m_pIdentity = ((CQSConsoleData*)pRootNode)->m_pQServer.DCO_GetIdentity();
        }
    catch(...)
        {
        hr = E_FAIL;
        }
    
    return hr;
}


///////////////////////////////////////////////////////////////////////////////
//
// Function name: CFetchFileDlg::CopyCallback
//
// Description  : 
//
// Return type  : BOOL 
//
// Argument     :  DWORD dwMessage
// Argument     : DWORD dwFileID
// Argument     : LPCTSTR szTempFileName
// Argument     : DWORD dwUserData
//
///////////////////////////////////////////////////////////////////////////////
// 3/16/99 - DBUCHES: Function created / header added 
///////////////////////////////////////////////////////////////////////////////
BOOL CFetchFileDlg::CopyCallback( DWORD dwMessage,
                             DWORD dwFileID, 
                             LPCTSTR szTempFileName,
                             DWORD dwUserData )
{
    // 
    // Get pointer to data
    // 
    CFetchFileDlg* pThis = (CFetchFileDlg*) dwUserData;

    // 
    // Check for cancel.
    // 
    if( pThis->m_bCancelled )
        return FALSE;

    // 
    // Handle event.
    // 
    switch( dwMessage )
        {
        case COPY_STATUS_FILE_DONE:
            {
            // 
            // Add this file to our list of fetched data.
            // 
            try
                {
                CFetchedFileData *p = new CFetchedFileData( dwFileID, szTempFileName );
                pThis->m_aFileDataArray[ pThis->m_dwFilesSoFar++ ] = p;
                }
            catch(...)
                {
                // 
                // Out of memory.
                // 
                //pThis->PostMessage( WM_RDIALOG_ERROR );
                return FALSE;
                }
            }
            break;

        case COPY_STATUS_JOB_DONE:
            {
            // 
            // Communicate change to dialog.
            // 
            pThis->PostMessage( WM_FETCHDLG_END_JOB );
            }
            return TRUE;

        case COPY_STATUS_ERROR_DISK_SPACE:
            {
            //pThis->PostMessage( WM_RDIALOG_ERROR );
            }
            return FALSE;
        
        case COPY_STATUS_ERROR:
            {
            //pThis->PostMessage( WM_RDIALOG_ERROR );
            }
            return FALSE;
        }

    return TRUE;
}


///////////////////////////////////////////////////////////////////////////////
//
// Function name: CFetchFileDlg::OnEndJob
//
// Description  : 
//
// Return type  : LRESULT 
//
// Argument     :  WPARAM wParam
// Argument     : LPARAM lParam
//
///////////////////////////////////////////////////////////////////////////////
// 3/16/99 - DBUCHES: Function created / header added 
///////////////////////////////////////////////////////////////////////////////
LRESULT CFetchFileDlg::OnEndJob( WPARAM wParam, LPARAM lParam )
{
    EndDialog( IDOK );
    return 0;
}

BEGIN_MESSAGE_MAP(CFetchFileDlg, CDialog)
	//{{AFX_MSG_MAP(CFetchFileDlg)
        ON_MESSAGE( WM_FETCHDLG_END_JOB, OnEndJob )
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CFetchFileDlg message handlers


///////////////////////////////////////////////////////////////////////////////
//
// Function name: CFetchFileDlg::OnCancel
//
// Description  : Handles Cancel button click.
//
// Return type  : void 
//
//
///////////////////////////////////////////////////////////////////////////////
// 3/16/99 - DBUCHES: Function created / header added 
///////////////////////////////////////////////////////////////////////////////
void CFetchFileDlg::OnCancel()
{
    // 
    // Cancel running thread
    // 
    m_bCancelled = TRUE;

    m_fetch.Abort();

    CDialog::OnCancel();

}


///////////////////////////////////////////////////////////////////////////////
//
// Function name: CFetchFileDlg::OnInitDialog
//
// Description  : 
//
// Return type  : BOOL 
//
//
///////////////////////////////////////////////////////////////////////////////
// 3/16/99 - DBUCHES: Function created / header added 
///////////////////////////////////////////////////////////////////////////////
BOOL CFetchFileDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
    // 
    // Need to get temporary folder here.
    //
    CString sTempDir;
    GetTempPath( MAX_PATH, sTempDir.GetBuffer( MAX_PATH ) );
    sTempDir.ReleaseBuffer();

	// 
    // Set file fetch thread in motion.
    //
    m_fetch.GetFiles( m_adwItemIDs, 
                      m_dwNumItems, 
                      m_pUnk, 
                      sTempDir, 
                      CFetchFileDlg::CopyCallback, 
                      (DWORD) this );
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
