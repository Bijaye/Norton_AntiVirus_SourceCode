/////////////////////////////////////////////////////////////////////////////
// $Header:   S:/QCONSOLE/VCS/qconsoledoc.cpv   1.18   11 Jun 1998 15:48:28   DBuches  $
/////////////////////////////////////////////////////////////////////////////
//
// qconsoleDoc.cpp : implementation of the CQconsoleDoc class
//
//
/////////////////////////////////////////////////////////////////////////////
// $Log:   S:/QCONSOLE/VCS/qconsoledoc.cpv  $
// 
//    Rev 1.18   11 Jun 1998 15:48:28   DBuches
// Added support for trial ware.
// 
//    Rev 1.17   12 May 1998 15:20:52   DBuches
// Added code for NT security support.
// 
//    Rev 1.16   11 May 1998 15:45:32   DBuches
// Added corporate repair folder.
// 
//    Rev 1.15   27 Apr 1998 16:10:40   DBuches
// Hooked up corporate mode registry keys.
// 
//    Rev 1.14   10 Apr 1998 14:15:18   DBuches
// Removed unneccessary code.
// 
//    Rev 1.13   07 Apr 1998 16:49:40   DBuches
// 1st pass at startup scanning.
// 
//    Rev 1.12   06 Apr 1998 19:54:42   JTaylor
// Removed a redundant call to ReadOptionsFile.
// 
//    Rev 1.11   06 Apr 1998 19:42:42   JTaylor
// Removed the DWORD arguments to READ/WRITE OptionsFile.
// 
//    Rev 1.10   03 Apr 1998 13:28:34   DBuches
// 1st pass at options.
// 
//    Rev 1.9   31 Mar 1998 15:02:04   DBuches
// Switched to using MFC data maps for keeping track of quarantine items.
// 
//    Rev 1.8   16 Mar 1998 17:37:36   DALLEE
// Initialize m_hAVAPIContext to NULL.
// 
//    Rev 1.7   13 Mar 1998 22:28:16   JTaylor
// Added a call to IQuarantineDll::Initialize after the creation.
// 
//    Rev 1.6   13 Mar 1998 15:22:18   DBuches
// Added item classification stuff.
// 
//    Rev 1.5   11 Mar 1998 15:16:20   DBuches
// Added DataMap objects for fast access to item lists.
// 
//    Rev 1.4   06 Mar 1998 17:16:58   DBuches
// Added DeleteItemFromList().
// 
//    Rev 1.3   06 Mar 1998 11:21:04   DBuches
// Added more data members.
// 
//    Rev 1.2   03 Mar 1998 17:04:24   DBuches
// Checked in more work in progress.
// 
//    Rev 1.1   02 Mar 1998 15:26:12   DBuches
// Added AVAPI context to document data structure.
// 
//    Rev 1.0   27 Feb 1998 15:08:58   DBuches
// Initial revision.
/////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "qconsole.h"

#define INITGUID
#include "qconsoleDoc.h"
#include "const.h"
#include "defutils.h"
#include "navcb.h"


// bugbug - need to remove this
#include "quaradd.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif



/////////////////////////////////////////////////////////////////////////////
// CQconsoleDoc

IMPLEMENT_DYNCREATE(CQconsoleDoc, CDocument)

BEGIN_MESSAGE_MAP(CQconsoleDoc, CDocument)
	//{{AFX_MSG_MAP(CQconsoleDoc)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CQconsoleDoc construction/destruction



////////////////////////////////////////////////////////////////////////////////
//
// Function name	: CQconsoleDoc::CQconsoleDoc
// Description	    : Constructor
//
////////////////////////////////////////////////////////////////////////////////
// 2/19/98 - DBUCHES function created / header added
////////////////////////////////////////////////////////////////////////////////
CQconsoleDoc::CQconsoleDoc()
{
  	// Get app object
    CWinApp* pApp = AfxGetApp();
    
    // Get list view style.
    m_dwListViewStyle = pApp->GetProfileInt( REGKEY_UI_SETTINGS, REGKEY_VALUE_LISTVIEW_MODE, LVS_REPORT );
    m_iViewMode = VIEW_MODE_BACKUP;

    // Get corporate mode switch from registry
    m_bCorporateMode = FALSE;
    HKEY hKey;
    DWORD dwValue = 0;
    DWORD dwType = REG_DWORD;
    DWORD dwSize = sizeof( DWORD );
    if( ERROR_SUCCESS == RegOpenKeyEx( HKEY_LOCAL_MACHINE, REGKEY_QUARANTINE, 0, KEY_READ, &hKey ) )
        {
        if( ERROR_SUCCESS == RegQueryValueEx( hKey, 
                            REGKEY_VALUE_CORPORATE,
                            0,
                            &dwType,
                            (LPBYTE) &dwValue,
                            &dwSize ) )
            {
            m_bCorporateMode = (BOOL) dwValue > 0;
            }

        // Get Repaired folder
        dwType = REG_SZ;
        dwSize = MAX_PATH;
        TCHAR szRepairPath[MAX_PATH];
        if( ERROR_SUCCESS == RegQueryValueEx( hKey, 
                            REGKEY_VALUE_REPAIR_PATH,
                            0,
                            &dwType,
                            (LPBYTE) &szRepairPath,
                            &dwSize ) )
            {
            m_sRepairFolder = szRepairPath;
            }

        // Cleanup
        RegCloseKey( hKey );
        }

    // Set defaults.
    m_hAVAPIContext = NULL;
    m_pQuarantine = NULL;
    m_iCurSortColumn = 0;
    m_pOpts = NULL;
    m_pQuarantine = NULL;
    m_bPasswordGood = FALSE;
    m_bStartupScanNeeded = FALSE;
    ZeroMemory( &m_stDefsDate, sizeof( SYSTEMTIME ) );

    // Q: What OS are we running on ?
    OSVERSIONINFO os;
    os.dwOSVersionInfoSize = sizeof( OSVERSIONINFO );
    GetVersionEx( &os );
    m_bNT = (BOOL)( os.dwPlatformId == VER_PLATFORM_WIN32_NT );

    // Get valid trialware state.
    m_bTrialValid = ((CQconsoleApp*)AfxGetApp())->IsTrialValid();

    // Set up our arrays
    ResetArrays();
}



////////////////////////////////////////////////////////////////////////////////
//
// Function name	: CQconsoleDoc::~CQconsoleDoc
// Description	    : Destructor
//
////////////////////////////////////////////////////////////////////////////////
// 2/26/98 - DBUCHES function created / header added
////////////////////////////////////////////////////////////////////////////////
CQconsoleDoc::~CQconsoleDoc()
{
    // Cleanup.
    ResetArrays();

    // Release objects
    if( m_pQuarantine )
        {
        m_pQuarantine->Release();
        m_pQuarantine = NULL;
        }

    if( m_pOpts )
        {
        m_pOpts->Release();
        m_pOpts = NULL;
        }

}


/////////////////////////////////////////////////////////////////////////////
// CQconsoleDoc diagnostics

#ifdef _DEBUG
void CQconsoleDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CQconsoleDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CQconsoleDoc commands



////////////////////////////////////////////////////////////////////////////////
//
// Function name	: CQconsoleDoc::OnCloseDocument
// Description	    : Saves any document-centric data to the registry
// Return type		: void 
//
////////////////////////////////////////////////////////////////////////////////
// 2/19/98 - DBUCHES function created / header added
////////////////////////////////////////////////////////////////////////////////
void CQconsoleDoc::OnCloseDocument() 
{
	// Get app object
    CWinApp* pApp = AfxGetApp();

    pApp->WriteProfileInt( REGKEY_UI_SETTINGS, REGKEY_VALUE_LISTVIEW_MODE, m_dwListViewStyle );
	
	CDocument::OnCloseDocument();
}


////////////////////////////////////////////////////////////////////////////
// Function name	: CQconsoleDoc::OnNewDocument
//
// Description	    : Handles creation of a new document
//
// Return type		: BOOL 
//
//
////////////////////////////////////////////////////////////////////////////
// 2/25/98 DBUCHES - Function created / Header added.
////////////////////////////////////////////////////////////////////////////
BOOL CQconsoleDoc::OnNewDocument() 
{
	// Create quarantine object
    HRESULT hr = CoCreateInstance( CLSID_QuarantineDLL,
                                   NULL,
                                   CLSCTX_INPROC_SERVER,
                                   IID_QuarantineDLL,
                                   (LPVOID*)&m_pQuarantine);
    
    if( FAILED( hr ) )
        {
        AfxMessageBox( IDS_ERROR_CREATING_QUARANTINE_OBJECT, MB_ICONSTOP | MB_OK );
        return FALSE;
        }

    // Initialize the IQuarantineDLL.
    hr = m_pQuarantine->Initialize();
    if( FAILED( hr ) )
        {
        AfxMessageBox( IDS_ERROR_CREATING_QUARANTINE_OBJECT, MB_ICONSTOP | MB_OK );
        return FALSE;
        }

    // Get location of quarantine folder
    m_pQuarantine->GetQuarantineFolder( m_sQuarantineFolder.GetBuffer(MAX_PATH), MAX_PATH );
    m_sQuarantineFolder.ReleaseBuffer();

    // Get scanning context from quarantine object
    if( FAILED( m_pQuarantine->GetScanningContext( &m_hAVAPIContext ) ) )
        {
        AfxMessageBox( IDS_ERROR_CREATING_QUARANTINE_OBJECT, MB_ICONSTOP | MB_OK );
        }

    // Create options object
    hr = CoCreateInstance( CLSID_QuarantineOpts,
                           NULL,
                           CLSCTX_INPROC_SERVER,
                           IID_QuarantineOpts,
                           (LPVOID*)&m_pOpts);
    if( FAILED( hr ) )
        {
        AfxMessageBox( IDS_ERROR_CREATING_QUARANTINE_OBJECT, MB_ICONSTOP | MB_OK );
        return FALSE;
        }

    // Compute path to options file
    TCHAR szFileName[MAX_PATH];
    TCHAR* p;
    GetModuleFileName( NULL, szFileName, MAX_PATH );
    
    // Search for our module name in string, and replace it with 
    // options file name.
    _tcsupr( szFileName );
    p = strstr( szFileName, _T("QCONSOLE.EXE") );
    if( p == NULL )
        {
        AfxMessageBox( IDS_ERROR_CREATING_QUARANTINE_OBJECT, MB_ICONSTOP | MB_OK );
        return FALSE;
        }
    
    // Add options file name to path
    *p = NULL;
    lstrcat( szFileName, _T("QUAROPTS.DAT") );

    // Initialize object.
    hr = m_pOpts->Initialize( szFileName );
    if( FAILED( hr ) )
        {
        AfxMessageBox( IDS_ERROR_READING_OPTIONS, MB_ICONSTOP | MB_OK );
        return FALSE;
        }

    // Get last defs date from registry.
    HKEY hKey;
    if( ERROR_SUCCESS == RegOpenKeyEx( HKEY_LOCAL_MACHINE,
                                       REGKEY_QUARANTINE,
                                       0,
                                       KEY_READ | KEY_WRITE,
                                       &hKey ) )
        {
        // Get defs being used by quarantine.            
        SYSTEMTIME stQuarDefs;
        if( FAILED( m_pQuarantine->GetCurrentDefinitionsDate( &stQuarDefs ) ) )
            {
            AfxMessageBox( IDS_ERROR_CREATING_QUARANTINE_OBJECT, MB_ICONSTOP | MB_OK );
            RegCloseKey( hKey );
            return FALSE;
            }
        
        // Get date of last defs used by quarantine console
        DWORD dwSize = sizeof( SYSTEMTIME );
        DWORD dwType = REG_BINARY;
        if( ERROR_SUCCESS == RegQueryValueEx( hKey,
                                               REGKEY_VALUE_DEFSDATE,
                                               0,
                                               &dwType,
                                               (LPBYTE) &m_stDefsDate,
                                               &dwSize ) )
            {

            // Compare the times.  If the defs being used by quarantine are newer
            // fire off a scan.
            CTime t1( m_stDefsDate );
            CTime t2( stQuarDefs );

            if( t2 > t1 )
                {
                m_bStartupScanNeeded = TRUE;
                }
            }

        // Save off new defs date.
        dwSize = sizeof( SYSTEMTIME );
        dwType = REG_BINARY;
        if( ERROR_SUCCESS != RegSetValueEx( hKey,
                                            REGKEY_VALUE_DEFSDATE,
                                            0,
                                            dwType,
                                            (LPBYTE) &stQuarDefs,
                                            dwSize ) )
            {
            AfxMessageBox( IDS_ERROR_CREATING_QUARANTINE_OBJECT, MB_ICONSTOP | MB_OK );
            RegCloseKey( hKey );
            return FALSE;
            }
        
        // Cleanup
        RegCloseKey( hKey );
        }

	return CDocument::OnNewDocument();
}



////////////////////////////////////////////////////////////////////////////////
//
// Function name	: CQconsoleDoc::ResetArrays
// Description	    : Sets up our item arrays.
// Return type		: void 
//
////////////////////////////////////////////////////////////////////////////////
// 2/26/98 - DBUCHES function created / header added
////////////////////////////////////////////////////////////////////////////////
void CQconsoleDoc::ResetArrays()
{
    // Set up allocation sizes for arrays
    POSITION pos;
    IQuarantineItem* p1;
    IQuarantineItem* p2;

    // Release all elements.
    for( pos = m_aMapAllItems.GetStartPosition(); pos != NULL; )
        {
        m_aMapAllItems.GetNextAssoc(pos, p1, p2);
        p2->Release();
        }
    
    // Remove references from lists
    m_aMapAllItems.RemoveAll();
    m_aMapCurrentSubset.RemoveAll();


    // Reset counters.
    m_iTotalItems = 0;    
    m_iQuarantineItems = 0;
    m_iBackupItems = 0;   
    m_iSubmittedItems = 0;
}




////////////////////////////////////////////////////////////////////////////////
//
// Function name	: CQconsoleDoc::RebuildItemArrays
// Description	    : This routine will enumerate all items in Quarantine, and
//                    add them to the appropriate lists.
// Return type		: void 
//
////////////////////////////////////////////////////////////////////////////////
// 2/26/98 - DBUCHES function created / header added
////////////////////////////////////////////////////////////////////////////////
void CQconsoleDoc::RebuildItemArrays()
{
    // Remove contents of arrays.
    ResetArrays();    

    // Set up enumeration.
    IEnumQuarantineItems* pEnum;
    if( FAILED( m_pQuarantine->Enum( &pEnum ) ) )
        {
        ASSERT(FALSE);
        return;
        }

    // Fetch items and insert them into the proper array
    ULONG uFetched;
    DWORD dwStatus;
    IQuarantineItem* pItem;
    while( pEnum->Next( 1, &pItem, &uFetched ) == NOERROR )
        {
        // Add this item to our list of items.
        m_aMapAllItems.SetAt( pItem, pItem );   

        // Bump count
        m_iTotalItems ++;

        // Classify this item
        if( FAILED( pItem->GetFileStatus( &dwStatus ) ) )
            continue;

        switch( dwStatus )
            {
            case QFILE_STATUS_QUARANTINED: 
                m_iQuarantineItems++; 
                break;
            case QFILE_STATUS_SUBMITTED:   
                m_iSubmittedItems++;   
                break;
            case QFILE_STATUS_BACKUP_FILE: 
                m_iBackupItems++;
                break;
            }

        }

    // Release enumeration object.
    pEnum->Release();
}



////////////////////////////////////////////////////////////////////////////////
//
// Function name	: CQconsoleDoc::GetItemList
// Description	    : 
// Return type		: BOOL 
// Argument         : int iMode
// Argument         : CPtrList * pList
//
////////////////////////////////////////////////////////////////////////////////
// 2/26/98 - DBUCHES function created / header added
////////////////////////////////////////////////////////////////////////////////
CMapPtrToPtr* CQconsoleDoc::GetItemList( int iMode /* = -1 */ )
{
    // Return the correct list    
    if( iMode == -1 )
        iMode = m_iViewMode;

    switch( iMode )
        {
        case VIEW_MODE_ALL:
            return &m_aMapAllItems;

        case VIEW_MODE_QUARANTINE:    
        case VIEW_MODE_BACKUP:
        case VIEW_MODE_SUBMITTED:
            BuildSubsetList( iMode );
            return &m_aMapCurrentSubset;
            break;
        
        default:
            ASSERT(FALSE);
            return NULL;
        }
    
    return NULL;
}



////////////////////////////////////////////////////////////////////////////////
//
// Function name	: CQconsoleDoc::BuildSubsetList
// Description	    : Builds the current subset list.
// Return type		: void 
// Argument         : int iMode
//
////////////////////////////////////////////////////////////////////////////////
// 3/12/98 - DBUCHES function created / header added
////////////////////////////////////////////////////////////////////////////////
void CQconsoleDoc::BuildSubsetList( int iMode )
{
    BOOL bAdd;
    POSITION pos;
    DWORD dwStatus;
    IQuarantineItem* p;
    IQuarantineItem* p1;

    // Clean up list and map.
    m_aMapCurrentSubset.RemoveAll();

    // Classify all elements
    for( pos = m_aMapAllItems.GetStartPosition(); pos != NULL; )
        {
        m_aMapAllItems.GetNextAssoc(pos, p, p1);
        ASSERT( p );
        
        if( FAILED( p->GetFileStatus( &dwStatus ) ) )
            return;

        bAdd = FALSE;
        switch( dwStatus )
            {
            case QFILE_STATUS_QUARANTINED: 
                if( iMode == VIEW_MODE_QUARANTINE )
                    bAdd = TRUE;
                break;
            case QFILE_STATUS_SUBMITTED:
                if( iMode == VIEW_MODE_SUBMITTED )
                    bAdd = TRUE;
                break;
            case QFILE_STATUS_BACKUP_FILE: 
                if( iMode == VIEW_MODE_BACKUP )
                    bAdd = TRUE;
                break;
            }

        if( bAdd )
            {
            // Add this item to our list of items.
            m_aMapCurrentSubset.SetAt( p, p );
            }
        }
}



////////////////////////////////////////////////////////////////////////////////
//
// Function name	: CQconsoleDoc::RecalcItemTypes
// Description	    : Recomputes the item type counts
// Return type		: void 
//
////////////////////////////////////////////////////////////////////////////////
// 3/12/98 - DBUCHES function created / header added
////////////////////////////////////////////////////////////////////////////////
void CQconsoleDoc::RecalcItemTypes()
{
    POSITION pos;
    DWORD dwStatus;
    IQuarantineItem* pItem;
    IQuarantineItem* p;
    
    // Reset counters.
    m_iQuarantineItems = 0;
    m_iSubmittedItems = 0;
    m_iBackupItems = 0;

    // Classify all elements
    for( pos = m_aMapAllItems.GetStartPosition(); pos != NULL; )
        {
        m_aMapAllItems.GetNextAssoc( pos, p, pItem);
        ASSERT( pItem );

        // Classify this item
        if( FAILED( pItem->GetFileStatus( &dwStatus ) ) )
            continue;

        // adjust counters
        switch( dwStatus )
            {
            case QFILE_STATUS_QUARANTINED: 
                m_iQuarantineItems++; 
                break;
            case QFILE_STATUS_SUBMITTED:   
                m_iSubmittedItems++;   
                break;
            case QFILE_STATUS_BACKUP_FILE: 
                m_iBackupItems++;
                break;
            }

        }
}

////////////////////////////////////////////////////////////////////////////
// Function name	: CQconsoleDoc::DeleteItemFromLists
//
// Description	    : Removes a quarantine item from the master list, as well
//                    as from any classification lists
//
// Return type		: BOOL - TRUE on success
//
// Argument         : IQuarantineItem* pItem - item to remove
//
////////////////////////////////////////////////////////////////////////////
// 3/6/98 DBUCHES - Function created / Header added.
////////////////////////////////////////////////////////////////////////////
BOOL CQconsoleDoc::DeleteItemFromLists( IQuarantineItem* pItem, BOOL bAll /* = TRUE */ )
{
    //POSITION pos;
    DWORD dwStatus;
    
    // Remove from current subset list    
    m_aMapCurrentSubset.RemoveKey( pItem );
    
    // Remove from main list.
    if( bAll )
        {
        // Remove element.
        m_aMapAllItems.RemoveKey( pItem );

        // Adjust counters
        m_iTotalItems --;
        if( SUCCEEDED( pItem->GetFileStatus( &dwStatus ) ) )
            {
            switch( dwStatus )
                {
                case QFILE_STATUS_QUARANTINED: 
                    m_iQuarantineItems--; 
                    break;
                case QFILE_STATUS_SUBMITTED:   
                    m_iSubmittedItems--;   
                    break;
                case QFILE_STATUS_BACKUP_FILE: 
                    m_iBackupItems--;
                    break;
                }
            }

        // Release object
        pItem->Release();
        }

    // Success!
    return TRUE;
}


////////////////////////////////////////////////////////////////////////////
// Function name	: CQconsoleDoc::InitVirusEngine
//
// Description	    : Sets up our virus scanning engine
//
// Return type		: BOOL 
//
//
////////////////////////////////////////////////////////////////////////////
// 3/2/98 DBUCHES - Function created / Header added.
////////////////////////////////////////////////////////////////////////////
BOOL CQconsoleDoc::InitVirusEngine()
{
#if 0    
    // Get virus definitions dir.
    CDefUtils defUtils;

    // Test code - need to get real APPID for hawkings
    if( defUtils.InitWindowsApp( _T( "NAVNT_50_NAVW" ) ) == FALSE )
        {
        ASSERT( FALSE );
        return FALSE;
        }

    // Get virus def location
    if( defUtils.GetCurrentDefs( m_sDefDir.GetBuffer(MAX_PATH), MAX_PATH ) == FALSE )
        {
        m_sDefDir.ReleaseBuffer();    
        ASSERT( FALSE );
        return FALSE;
        }
    m_sDefDir.ReleaseBuffer();    

    // Set up the stupid callbacks
    NAVSetDefsPath( m_sDefDir );

    // Build full paths to data files.
    CString sDatFile( m_sDefDir ), sInfFile( m_sDefDir );

    sDatFile += _T( "\\VIRSCAN1.DAT" );
    sInfFile += _T( "\\VIRSCAN.INF" );

    // Initialize our scanning engine.
    m_hAVAPIContext = VLScanInit(1, &gstNAVCallBacks, 0);
    if( m_hAVAPIContext == NULL ) 
        {
        ASSERT( FALSE );
        return FALSE;
        }

    // Register the virus scan database
    if (VLRegisterVirusDB( m_hAVAPIContext,
                           sDatFile.GetBuffer(0),
                           sInfFile.GetBuffer(0),
                           "") != VS_OK )
        {
        VLScanClose( m_hAVAPIContext );
        m_hAVAPIContext = NULL;
        return FALSE;
        }
#endif

    // All good.
    return TRUE;    
}
