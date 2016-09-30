// ItemData.cpp: implementation of the CItemData class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "resource.h"
#include "ItemData.h"
#include "safearray.h"
#include "qsfields.h"
#include "filenameparse.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

// 
// Macros
// 
#define INSERT_FIELD(fname,ftype,findex) VariantInit( &vInsert ); \
                                        vInsert.vt = VT_BSTR; \
                                        vInsert.bstrVal = ::SysAllocString( A2W( fname ) ); \
                                        index[0] = findex; \
                                        index[1] = 0; \
                                        SafeArrayPutElement( pArray, index, &vInsert ); \
                                        VariantClear( &vInsert ); \
                                        vInsert.vt = VT_UI4; \
                                        vInsert.ulVal = ftype; \
                                        index[0] = findex; \
                                        index[1] = 1; \
                                        SafeArrayPutElement( pArray, index, &vInsert );



//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////
//
// Function name : CItemData::CItemData
//
// Description   : Constructor
//
///////////////////////////////////////////////////////////////////////////////
// 1/27/99 - DBUCHES: Function created / header added 
///////////////////////////////////////////////////////////////////////////////
CItemData::CItemData() :
m_dwSubmittedBy( 0xFFFFFFFF ),
m_dwItemID( 0 ),
m_dwStatus( 0 ),
m_uImage( RESULT_IMAGE_UNKNOWN ),
m_ulFileSize( 0 ),
m_dwVirusID( 0 )
{
    ZeroMemory( &m_stRecieved, sizeof (SYSTEMTIME) );
    ZeroMemory( &m_stSubmitted, sizeof (SYSTEMTIME) );
}


///////////////////////////////////////////////////////////////////////////////
//
// Function name : CItemData::~CItemData
//
// Description   : Destructor
//
///////////////////////////////////////////////////////////////////////////////
// 1/27/99 - DBUCHES: Function created / header added 
///////////////////////////////////////////////////////////////////////////////
CItemData::~CItemData()
{

}


///////////////////////////////////////////////////////////////////////////////
//
// Function name : CItemData::Initialize
//
// Description   : Initialize this class
//
// Return type   : HRESULT 
//
// Argument      : SAFEARRAY* pArray
//
///////////////////////////////////////////////////////////////////////////////
// 1/27/99 - DBUCHES: Function created / header added 
///////////////////////////////////////////////////////////////////////////////
HRESULT CItemData::Initialize( SAFEARRAY* pArray )
{
    HRESULT hr = S_OK;
    VARIANT v;
    LONG lIndex;
    SYSTEMTIME st;

    // 
    // Check input.
    // 
    if( pArray == NULL )
        return E_INVALIDARG;

    // 
    // Initialize our data members.
    // 
    try
        {
        // 
        // Get string data items
        // 
        lIndex = ITEM_DATA_FILE_NAME;
        SafeArrayGetElement( pArray, &lIndex, &v );
        if( v.vt != VT_EMPTY )
            {
            // 
            // Strip off file path.
            //
            CFileNameParse::GetFileName( v.bstrVal, m_sFileName.GetBuffer( MAX_PATH ), MAX_PATH );
            m_sFileName.ReleaseBuffer();

            CFileNameParse::GetFilePath( v.bstrVal, m_sFilePath.GetBuffer( MAX_PATH ), MAX_PATH );
            m_sFilePath.ReleaseBuffer();
            }

        lIndex = ITEM_DATA_USER_NAME;
        SafeArrayGetElement( pArray, &lIndex, &v );
        if( v.vt != VT_EMPTY )
            m_sUserName = v.bstrVal;

        lIndex = ITEM_DATA_DOMAIN_NAME;
        SafeArrayGetElement( pArray, &lIndex, &v );
        if( v.vt != VT_EMPTY )
            m_sDomainName = v.bstrVal;

        if( m_sDomainName.IsEmpty() )
            {
            m_sDomainName.LoadString( IDS_NA );
            }

        lIndex = ITEM_DATA_MACHINE_NAME;
        SafeArrayGetElement( pArray, &lIndex, &v );
        if( v.vt != VT_EMPTY )
            m_sMachineName = v.bstrVal;

        lIndex = ITEM_DATA_OS;
        SafeArrayGetElement( pArray, &lIndex, &v );
        if( v.vt != VT_EMPTY )
            m_sPlatform = v.bstrVal;

        lIndex = ITEM_DATA_PRODUCT;
        SafeArrayGetElement( pArray, &lIndex, &v );
        if( v.vt != VT_EMPTY )
            m_sProduct = v.bstrVal;

        lIndex = ITEM_DATA_ADDRESS;
        SafeArrayGetElement( pArray, &lIndex, &v );
        if( v.vt != VT_EMPTY )
            {
            // 
            // Need to seperate addresses.
            // 
            m_sAddress = v.bstrVal;
            }

        lIndex = ITEM_DATA_VIRUS_NAME;
        SafeArrayGetElement( pArray, &lIndex, &v );
        if( v.vt != VT_EMPTY )
            m_sVirusName = v.bstrVal;
        else
            {
            m_sVirusName.LoadString( IDS_UNKNOWN );
            }

        lIndex = ITEM_DATA_GUID;
        SafeArrayGetElement( pArray, &lIndex, &v );
        if( v.vt != VT_EMPTY )
            {
            CLSIDFromString( v.bstrVal, &m_ItemGUID );
            }
        
        // 
        // Get Date attributes
        // 
        lIndex = ITEM_DATA_RECIEVED_TIME;
        SafeArrayGetElement( pArray, &lIndex, &v );
        if( v.vt == VT_DATE )
            {
            VariantTimeToSystemTime( v.date, &st );
            SystemTimeToTzSpecificLocalTime( NULL,
                                             &st,
                                             &m_stRecieved );
            m_sRecievedDateStr = GetDateString( &m_stRecieved );
            }

        lIndex = ITEM_DATA_SUBMITTED_TIME;
        SafeArrayGetElement( pArray, &lIndex, &v );
        if( v.vt == VT_DATE )
            {
            VariantTimeToSystemTime( v.date, &st );
            SystemTimeToTzSpecificLocalTime( NULL,
                                             &st,
                                             &m_stSubmitted );
            m_sSubmittedDateStr = GetDateString( &m_stSubmitted );
            }
        else
            {
            // 
            // No submission field means not submitted.
            // 
            m_sSubmittedDateStr.LoadString( IDS_NOT_SUBMITTED );
            }

        // 
        // Get DWORD attributes.
        // 
        lIndex = ITEM_DATA_STATUS;
        SafeArrayGetElement( pArray, &lIndex, &v );
        if( v.vt != VT_EMPTY )
            m_dwStatus = v.ulVal;
        SetStatusText();

        lIndex = ITEM_DATA_SUBMITTED_BY;
        SafeArrayGetElement( pArray, &lIndex, &v );
        if( v.vt != VT_EMPTY )
            m_dwSubmittedBy = v.ulVal;
        SetSubmittedText();

        lIndex = ITEM_DATA_FILE_ID;
        SafeArrayGetElement( pArray, &lIndex, &v );
        if( v.vt != VT_EMPTY )
            m_dwItemID = v.ulVal;

        lIndex = ITEM_DATA_FILE_SIZE;
        SafeArrayGetElement( pArray, &lIndex, &v );
        if( v.vt != VT_EMPTY )
            m_ulFileSize = v.ulVal;

        lIndex = ITEM_DATA_VIRUS_ID;
        SafeArrayGetElement( pArray, &lIndex, &v );
        if( v.vt != VT_EMPTY )
            m_dwVirusID = v.ulVal;

        // 
        // Get correct image index.
        // 
        ComputeImageIndex();
        }
    catch(...)
        {
        hr = E_UNEXPECTED;
        }

    // 
    // All done.
    // 
    return hr;   
}




///////////////////////////////////////////////////////////////////////////////
//
// Function name : CItemData::GetDataMemberArray
//
// Description   : This routine will construct the appropriate SAFEARRAY 
//                 structure to be used with IQuarantineServer::EnumItemsVariant.
//
// Return type   : SAFEARRAY* Pointer to array on success, NULL on failure
//
//
///////////////////////////////////////////////////////////////////////////////
// 1/27/99 - DBUCHES: Function created / header added 
///////////////////////////////////////////////////////////////////////////////
SAFEARRAY* CItemData::GetDataMemberArray()
{
USES_CONVERSION;
    // 
    // Allocate a safe array.
    // 
    SAFEARRAYBOUND bounds[2];
    bounds[0].lLbound = 0;
    bounds[0].cElements = ITEM_DATA_NUM_ELEMENTS;
    bounds[1].lLbound = 0;
    bounds[1].cElements = 2;
    SAFEARRAY* pArray = SafeArrayCreate( VT_VARIANT, 2, bounds );
    LONG      index[2];
    if( pArray == NULL )
        return NULL;

    // 
    // Use template class ror ease.
    // 
    
    try
        {
        // 
        // Insert items into list.
        // 
        VARIANT vInsert;
        INSERT_FIELD( QSERVER_ITEM_INFO_FILENAME, VT_BSTR, ITEM_DATA_FILE_NAME )
        INSERT_FIELD( QSERVER_ITEM_INFO_MACHINENAME, VT_BSTR, ITEM_DATA_MACHINE_NAME )
        INSERT_FIELD( QSERVER_ITEM_INFO_USERNAME, VT_BSTR, ITEM_DATA_USER_NAME )
        INSERT_FIELD( QSERVER_ITEM_INFO_DOMAINNAME, VT_BSTR, ITEM_DATA_DOMAIN_NAME )
        INSERT_FIELD( QSERVER_ITEM_INFO_QUARANTINE_QDATE, VT_DATE, ITEM_DATA_RECIEVED_TIME )
        INSERT_FIELD( QSERVER_ITEM_INFO_SUBMIT_QDATE, VT_DATE, ITEM_DATA_SUBMITTED_TIME )
        INSERT_FIELD( QSERVER_ITEM_INFO_STATUS, VT_UI4, ITEM_DATA_STATUS )
        INSERT_FIELD( QSERVER_ITEM_INFO_SUBMISSION_ROUTE, VT_UI4, ITEM_DATA_SUBMITTED_BY )
        INSERT_FIELD( QSERVER_ITEM_INFO_FILEID, VT_UI4, ITEM_DATA_FILE_ID )
        INSERT_FIELD( QSERVER_ITEM_INFO_FILESIZE, VT_UI4, ITEM_DATA_FILE_SIZE )
        INSERT_FIELD( QSERVER_ITEM_INFO_VIRUSNAME, VT_BSTR, ITEM_DATA_VIRUS_NAME )
        INSERT_FIELD( QSERVER_ITEM_INFO_VIRUSID, VT_UI4, ITEM_DATA_VIRUS_ID )
        INSERT_FIELD( QSERVER_ITEM_INFO_UUID, VT_BSTR, ITEM_DATA_GUID )
        INSERT_FIELD( QSERVER_ITEM_INFO_SYSTEM, VT_BSTR, ITEM_DATA_OS )
        INSERT_FIELD( QSERVER_ITEM_INFO_SCANNER, VT_BSTR, ITEM_DATA_PRODUCT )
        INSERT_FIELD( QSERVER_ITEM_INFO_MACHINE_ADDRESS, VT_BSTR, ITEM_DATA_ADDRESS )
        }
    catch(...)
        {
        // 
        // Catch all errors.
        // 
        SafeArrayDestroy( pArray );
        return NULL;
        }

    // 
    // All done.
    // 
    return pArray;
}


///////////////////////////////////////////////////////////////////////////////
//
// Function name : CItemData::GetDateString
//
// Description   : Returns a formatted string containing the date in question.
//
// Return type   : CString 
//
//
///////////////////////////////////////////////////////////////////////////////
// 1/27/99 - DBUCHES: Function created / header added 
///////////////////////////////////////////////////////////////////////////////
CString CItemData::GetDateString( SYSTEMTIME * pst )
{
    CString s;
    TCHAR szBuffer[ 32 ];

    // 
    // Format the time
    // 
    GetDateFormat( LOCALE_SYSTEM_DEFAULT, DATE_SHORTDATE, pst, NULL, szBuffer, 32 );
    s += szBuffer;
    s += " ";
    GetTimeFormat( LOCALE_SYSTEM_DEFAULT, 0, pst, NULL, szBuffer, 32 );
    s += szBuffer;
    return s;
}



///////////////////////////////////////////////////////////////////////////////
//
// Function name : CItemData::ComputeImageIndex
//
// Description   : Populates the m_uImage field with the correct image based on
//                 the filename extension.
//
// Return type   : void 
//
//
///////////////////////////////////////////////////////////////////////////////
// 2/1/99 - DBUCHES: Function created / header added 
///////////////////////////////////////////////////////////////////////////////
void CItemData::ComputeImageIndex()
{
    // 
    // Get icon index.
    // 
    SHFILEINFO sfi;
    ZeroMemory( &sfi, sizeof(SHFILEINFO) );
    SHGetFileInfo( m_sFileName, 
                   FILE_ATTRIBUTE_NORMAL,
                   &sfi, 
                   sizeof( SHFILEINFO ), 
                   SHGFI_SYSICONINDEX | SHGFI_ICON | SHGFI_LARGEICON | SHGFI_USEFILEATTRIBUTES 
                   );

    // 
    // Need to add one to account for main icon 
    // 
    m_uImage = sfi.iIcon + 1;
}


///////////////////////////////////////////////////////////////////////////////
//
// Function name: CItemData::SetStatus
//
// Description  : 
//
// Return type  : void 
//
// Argument     : DWORD dwStatus
//
///////////////////////////////////////////////////////////////////////////////
// 3/29/99 - DBUCHES: Function created / header added 
///////////////////////////////////////////////////////////////////////////////
void CItemData::SetStatusText( )
{
    int id;
    
    switch( m_dwStatus )
        {
        case STATUS_QUARANTINED:
            id = IDS_QUARANTINED;
            break;

        case STATUS_SUBMITTED:
            id = IDS_SUBMITTED;
            break;

        case STATUS_HELD:
            id = IDS_STATUS_HELD;
            break;

        case STATUS_RELEASED:
            id = IDS_STATUS_RELEASED;
            break;

        case STATUS_UNNEEDED:
            id = IDS_STATUS_UNNEEDED;
            break;

        case STATUS_NEEDED:
            id = IDS_STATUS_NEEDED;
            break;

        case STATUS_AVAILIABLE:
            id = IDS_STATUS_AVAILIABLE;
            break;

        case STATUS_DISTRIBUTED:
            id = IDS_STATUS_DISTRIBUTED;
            break;

        case STATUS_INSTALLED:       // 7/1/99 jhill added
            id = IDS_STATUS_INSTALLED;
            break;

        case STATUS_ATTENTION:       // 7/11/99 jhill added
            id = IDS_STATUS_ATTENTION;
            break;

        case STATUS_ERROR:           // 7/11/99 jhill added
            id = IDS_STATUS_ERROR;
            break;

        default:
            id = IDS_UNKNOWN;
        }

    m_sStatusText.LoadString( id );
}



///////////////////////////////////////////////////////////////////////////////
//
// Function name:   CItemData::SetSubmitted
//
// Description  : 
//
// Return type  : void  
//
// Argument     : DWORD dwSubmit
//
///////////////////////////////////////////////////////////////////////////////
// 3/29/99 - DBUCHES: Function created / header added 
///////////////////////////////////////////////////////////////////////////////
void CItemData::SetSubmittedText()
{
    int id;
    switch( m_dwSubmittedBy )
        {
        case SUBMISSION_SCANANDDELIVER:
            id = IDS_SCANANDDELIVER;
            break;

        case SUBMISSION_AVIS:
            id = IDS_AVIS;
            break;

        default:
            id = IDS_NOT_SUBMITTED;
        }

    m_sSubmittedBy.LoadString( id );
}

