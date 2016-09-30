/*
 * PROPRIETARY/CONFIDENTIAL.  use of this product is subject to license terms.
 * Copyright (c) 2005 Symantec Corporation.  All Rights Reserved.
 *
*/

// ItemData.cpp: implementation of the CItemData class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "resource.h"
#include "ItemData.h"
#include "safearray.h"
#include "qsfields.h"
#include "filenameparse.h"
#include "time.h"
#include "AvisEventDetection.h"
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif
TIME_ZONE_INFORMATION	CItemData::m_tzi= {0};
DWORD					CItemData::m_tzRc = 0;
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
m_dwVirusID( 0 ),
m_bError(FALSE)
{
	TIME_ZONE_INFORMATION tzitemp = {0};
    ZeroMemory( &m_stRecieved, sizeof (SYSTEMTIME) );
    ZeroMemory( &m_stSubmitted, sizeof (SYSTEMTIME) );

	// Added 12-2-99 Terrym for addational Bell attributes.
    ZeroMemory( &m_stCompleted, sizeof (SYSTEMTIME) );
	m_ulAgeMinute = 0;
	m_ulDefNeeded = 0; 
	m_dwfStateFlag = ITEM_DATA_STATE_UNKNOWN;

	if (memcmp (&m_tzi,&tzitemp,sizeof(TIME_ZONE_INFORMATION))==0)
	{
			m_tzRc = GetTimeZoneInformation(&m_tzi);
	}
		

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
    SYSTEMTIME st = {0};


	USES_CONVERSION;
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

		// Added 12-2-99 Terrym for addational Bell attributes.
        lIndex = ITEM_DATA_ERROR;
        SafeArrayGetElement( pArray, &lIndex, &v );
        if( v.vt != VT_EMPTY )
            m_sError = OLE2T(v.bstrVal);
		VariantClear(&v);

		// Added 12-2-99 Terrym for addational Bell attributes.
        lIndex = ITEM_DATA_STATE;
        SafeArrayGetElement( pArray, &lIndex, &v );
        if( v.vt != VT_EMPTY )
            m_sState = OLE2T(v.bstrVal);
		VariantClear(&v);

		// Added 12-2-99 Terrym for addational Bell attributes.
        lIndex = ITEM_DATA_DEFSEQNEEDED;
        SafeArrayGetElement( pArray, &lIndex, &v );
        if( v.vt != VT_EMPTY )
		{
			DWORD dwSeq = 0;
            m_sDefSeqNeeded = OLE2T(v.bstrVal);
			if (atol(T2A(m_sDefSeqNeeded.GetBuffer(0))) == 0)
				m_sDefSeqNeeded.LoadString(IDS_BLANK);
		}
//		else
//		{
//			m_sDefSeqNeeded.LoadString(IDS_NONE);
//		}

		VariantClear(&v);

        lIndex = ITEM_DATA_FILE_NAME;
        SafeArrayGetElement( pArray, &lIndex, &v );
        if( v.vt != VT_EMPTY )
            {
			USES_CONVERSION;
            // 
            // Strip off file path.
            //
            CFileNameParse::GetFileName( OLE2T(v.bstrVal), m_sFileName.GetBuffer( MAX_PATH ), MAX_PATH );
            m_sFileName.ReleaseBuffer();

            CFileNameParse::GetFilePath( OLE2T(v.bstrVal), m_sFilePath.GetBuffer( MAX_PATH ), MAX_PATH );
            m_sFilePath.ReleaseBuffer();
            }
		VariantClear(&v);

        lIndex = ITEM_DATA_USER_NAME;
        SafeArrayGetElement( pArray, &lIndex, &v );
        if( v.vt != VT_EMPTY )
            m_sUserName = OLE2T(v.bstrVal);
		VariantClear(&v);

        lIndex = ITEM_DATA_DOMAIN_NAME;
        SafeArrayGetElement( pArray, &lIndex, &v );
        if( v.vt != VT_EMPTY )
            m_sDomainName = OLE2T(v.bstrVal);

        if( m_sDomainName.IsEmpty() )
            {
            m_sDomainName.LoadString( IDS_NA );
            }
		VariantClear(&v);

        lIndex = ITEM_DATA_MACHINE_NAME;
        SafeArrayGetElement( pArray, &lIndex, &v );
        if( v.vt != VT_EMPTY )
            m_sMachineName = OLE2T(v.bstrVal);
		VariantClear(&v);

        lIndex = ITEM_DATA_OS;
        SafeArrayGetElement( pArray, &lIndex, &v );
        if( v.vt != VT_EMPTY )
            m_sPlatform = OLE2T(v.bstrVal);
		VariantClear(&v);

        lIndex = ITEM_DATA_PRODUCT;
        SafeArrayGetElement( pArray, &lIndex, &v );
        if( v.vt != VT_EMPTY )
            m_sProduct = OLE2T(v.bstrVal);
		VariantClear(&v);

        lIndex = ITEM_DATA_ADDRESS;
        SafeArrayGetElement( pArray, &lIndex, &v );
        if( v.vt != VT_EMPTY )
            {
            // 
            // Need to seperate addresses.
            // 
            m_sAddress = OLE2T(v.bstrVal);
            }
		VariantClear(&v);

        lIndex = ITEM_DATA_VIRUS_NAME;
        SafeArrayGetElement( pArray, &lIndex, &v );
        if( v.vt != VT_EMPTY )
            m_sVirusName = OLE2T(v.bstrVal);
        else
            {
            m_sVirusName.LoadString( IDS_UNKNOWN );
            }
		VariantClear(&v);

        lIndex = ITEM_DATA_SAMPLE_RESULT;
        SafeArrayGetElement( pArray, &lIndex, &v );
        if( v.vt != VT_EMPTY )
            m_sSampleResult = OLE2T(v.bstrVal);
        else
            {
            //m_sVirusName.LoadString( IDS_UNKNOWN );  // jhill fixed 2/19/00
            m_sSampleResult.LoadString( IDS_UNKNOWN );
            }
		VariantClear(&v);
        
		lIndex = ITEM_DATA_SCAN_RESULT;
        SafeArrayGetElement( pArray, &lIndex, &v );
        if( v.vt != VT_EMPTY )
            m_sScanResult = OLE2T(v.bstrVal);
        else
            {
            m_sScanResult.LoadString( IDS_UNKNOWN );
            }
		VariantClear(&v);

		lIndex = ITEM_DATA_ALERT_STATUS;
        SafeArrayGetElement( pArray, &lIndex, &v );
        m_dwAlertStatus = 0;
        if( v.vt != VT_EMPTY )
		{
			if(v.vt == VT_BSTR)
				m_sAlertStatus = OLE2T(v.bstrVal);
			else if (v.vt == VT_UI4)
				m_dwAlertStatus = v.ulVal;
		}
		VariantClear(&v);

        lIndex = ITEM_DATA_GUID;
        SafeArrayGetElement( pArray, &lIndex, &v );
        if( v.vt != VT_EMPTY )
            {
            CLSIDFromString( v.bstrVal, &m_ItemGUID );
            }
		VariantClear(&v);
        
        // 
        // Get Date attributes
        // 
        lIndex = ITEM_DATA_RECIEVED_TIME;
        SafeArrayGetElement( pArray, &lIndex, &v );
        if( v.vt == VT_DATE )
            {
			BOOL bRc = 0;
			DWORD dwRc = 0;
			SYSTEMTIME stTemp = {0};
			GetSystemTime (&stTemp);

			bRc = VariantTimeToSystemTime( v.date, &st );
            SystemTimeToTzSpecificLocalTime( NULL,
                                             &st,
                                             &m_stRecieved );
			// added by terrym 10-11-00  as part of Win9x support.  SystemTimeToTz... not supported on win9x
//			memcpy (&m_stRecieved, &st, sizeof(SYSTEMTIME));
//			ConvertTimeFromUTC2LT(&m_stRecieved);				

			m_sRecievedDateStr = GetDateString( &m_stRecieved );

            CTime ctQTime(st), ctCurrentTime(stTemp);

			if (ctQTime > ctCurrentTime)
			{
				m_sAge = "--";
				m_cstAge=0;
			}
			else
				{
				m_cstAge = ctCurrentTime - ctQTime;

				m_sAge = m_cstAge.Format(IDS_SAMPLE_AGE_FORMAT );
				}

            }
		VariantClear(&v);

        lIndex = ITEM_DATA_SUBMITTED_TIME;
        SafeArrayGetElement( pArray, &lIndex, &v );
        if( v.vt == VT_DATE )
            {
            VariantTimeToSystemTime( v.date, &st );
            SystemTimeToTzSpecificLocalTime( NULL,
                                             &st,
                                             &m_stSubmitted );
//			memcpy (&m_stSubmitted , &st, sizeof(SYSTEMTIME));
			// added by terrym 10-11-00  as part of Win9x support.  SystemTimeToTz... not supported on win9x
//			ConvertTimeFromUTC2LT(&m_stSubmitted );				

            m_sSubmittedDateStr = GetDateString( &m_stSubmitted );
            }
        else
            {
            // 
            // No submission field means not submitted.
            // 
            m_sSubmittedDateStr.LoadString( IDS_NOT_SUBMITTED );
            }
		VariantClear(&v);
#if 0 //3-15-00 we had used this to determine if the sample has been proccessed, but Analyzed time
		// is supposed to be used.  Leaving it here incase there is information present that we will
		// use later.
		// Added 12-2-99 Terrym for addational Bell attributes.
        lIndex = ITEM_DATA_COMPLETED_TIME;
        SafeArrayGetElement( pArray, &lIndex, &v );
        if( v.vt == VT_DATE )
            {
            VariantTimeToSystemTime( v.date, &st );
            SystemTimeToTzSpecificLocalTime( NULL,
                                             &st,
                                             &m_stCompleted);
            m_sCompletedDateStr = GetDateString( &m_stCompleted);
			m_sComplete.LoadString(IDS_BOOL_COMPLETE);
            }
        else
            {
            // 
            // No completed field means sample is still being proccessed.
            // 
            m_sCompletedDateStr.LoadString( IDS_NOT_COMPLETED );
			m_sComplete.LoadString(IDS_BOOL_NOTCOMPLETE);
            }
		VariantClear(&v);
#endif
		// Added 3-15-00 Terrym for addational Bell attributes.
        lIndex = ITEM_DATA_ANALYZED_TIME;
        SafeArrayGetElement( pArray, &lIndex, &v );
        if( v.vt == VT_BSTR )
            {
            m_sCompletedDateStr = v.bstrVal;
			m_sComplete.LoadString(IDS_BOOL_COMPLETE);
            }
        else
            {
            // 
            // No completed field means sample is still being proccessed.
            // 
            m_sCompletedDateStr.LoadString( IDS_NOT_COMPLETED );
			m_sComplete.LoadString(IDS_BOOL_NOTCOMPLETE);
            }
		VariantClear(&v);

        // 
        // Get DWORD attributes.
        // 
        lIndex = ITEM_DATA_STATUS;
        SafeArrayGetElement( pArray, &lIndex, &v );
        if( v.vt != VT_EMPTY )
		{
            m_dwStatus = v.ulVal;
		}
		m_sStatus.Format(_T("%d"),m_dwStatus);
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

 
/*		// Added 12-2-99 Terrym for addational Bell attributes.
        lIndex = ITEM_DATA_DEFSEQNEEDED;
        SafeArrayGetElement( pArray, &lIndex, &v );
        if( v.vt != VT_EMPTY )
		{
            m_ulDefNeeded = v.ulVal;
			m_sDefSeqNeeded.Format(_T("%d"), m_ulDefNeeded);
		}
*/        
		SetError(IsSampleErrorCondition());

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
// Function name : CItemData::Initialize 
//
// Description   : Initialize this class
//
// Return type   : HRESULT 
//
// Argument      : CItemData* ItemData
//
///////////////////////////////////////////////////////////////////////////////
// 12/14/99 - TMARLES: Function created / header added 
///////////////////////////////////////////////////////////////////////////////
HRESULT CItemData::Initialize( CItemData* pItemData)
{
    HRESULT hr = S_OK;

    // 
    // Check input.
    // 
    if( pItemData == NULL )
        return E_INVALIDARG;

    // 
    // Initialize our data members.
    // 
    try
        {
        // 
        // Get string data items
        // 
		if (!pItemData->m_sError.IsEmpty())
			m_sError = pItemData->m_sError;

		if (!pItemData->m_sState.IsEmpty())
			m_sState = pItemData->m_sState;
		
		if (!pItemData->m_sDefSeqNeeded.IsEmpty())
			m_sDefSeqNeeded  = pItemData->m_sDefSeqNeeded ;

		if (!pItemData->m_sFileName.IsEmpty())
			m_sFileName = pItemData->m_sFileName;

		if (!pItemData->m_sFilePath.IsEmpty())
			m_sFilePath = pItemData->m_sFilePath;

		if (!pItemData->m_sUserName.IsEmpty())
			m_sUserName = pItemData->m_sUserName;

		if (!pItemData->m_sDomainName.IsEmpty())
			m_sDomainName = pItemData->m_sDomainName ;

        if( m_sDomainName.IsEmpty() )
            {
            m_sDomainName.LoadString( IDS_NA );
            }

		if (!pItemData->m_sMachineName.IsEmpty())
			m_sMachineName = pItemData->m_sMachineName;

		if (!pItemData->m_sPlatform.IsEmpty())
			m_sPlatform = pItemData->m_sPlatform;

		if (!pItemData->m_sProduct.IsEmpty())
			m_sProduct = pItemData->m_sProduct;

		if (!pItemData->m_sAddress.IsEmpty())
			m_sAddress = pItemData->m_sAddress;

		if (!pItemData->m_sVirusName.IsEmpty())
			m_sVirusName = pItemData->m_sVirusName;
        else
            {
            m_sVirusName.LoadString( IDS_UNKNOWN );
            }

		if (!pItemData->m_sSampleResult.IsEmpty())
			m_sSampleResult = pItemData->m_sSampleResult;

		if (!pItemData->m_sScanResult.IsEmpty())
			m_sScanResult = pItemData->m_sScanResult;

		if (!pItemData->m_sStatus.IsEmpty())
			m_sStatus= pItemData->m_sStatus;
		
        m_ItemGUID = pItemData->m_ItemGUID;
		
        
        // 
        // Get Date attributes
        // 

        m_stRecieved = pItemData->m_stRecieved;
		m_sRecievedDateStr = pItemData->m_sRecievedDateStr;
		m_sAge = pItemData->m_sAge;

		m_cstAge = pItemData->m_cstAge;

        m_sSubmittedDateStr = pItemData->m_sSubmittedDateStr;
		
		m_stCompleted = pItemData->m_stCompleted;
        m_sCompletedDateStr = pItemData->m_sCompletedDateStr;
        m_sComplete = pItemData->m_sComplete;

        // 
        // Get DWORD attributes.
        // 

        m_dwStatus = pItemData->m_dwStatus;
		SetStatusText();

        m_dwSubmittedBy = pItemData->m_dwSubmittedBy;
		SetSubmittedText();

        m_dwItemID = pItemData->m_dwItemID;

        m_ulFileSize = pItemData->m_ulFileSize;

        m_dwVirusID = pItemData->m_dwVirusID;

		m_sAlertStatus = pItemData->m_sAlertStatus;

		m_dwAlertStatus = pItemData->m_dwAlertStatus;

		SetError(IsSampleErrorCondition());
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
		// Added 12-2-99 Terrym for addational Bell attributes.
        INSERT_FIELD( QSERVER_ITEM_INFO_COMPLETED_TIME, VT_DATE, ITEM_DATA_COMPLETED_TIME )
        INSERT_FIELD( QSERVER_ITEM_INFO_SAMPLE_STATE, VT_BSTR, ITEM_DATA_STATE )
        INSERT_FIELD( QSERVER_ITEM_INFO_ERROR, VT_BSTR, ITEM_DATA_ERROR )
        INSERT_FIELD( QSERVER_ITEM_INFO_MIN_DEF_SEQ, VT_BSTR, ITEM_DATA_DEFSEQNEEDED )
        INSERT_FIELD( QSERVER_ITEM_INFO_SAMPLE_RESULT, VT_BSTR, ITEM_DATA_SAMPLE_RESULT)
        INSERT_FIELD( QSERVER_ITEM_INFO_SCAN_RESULT, VT_BSTR, ITEM_DATA_SCAN_RESULT )
        INSERT_FIELD( QSERVER_ITEM_INFO_ALERT_STATUS, VT_BSTR, ITEM_DATA_ALERT_STATUS ) //tam 2-27-00
		// Added 3-15-00 Terrym for addational Bell attributes.
        INSERT_FIELD( QSERVER_ITEM_INFO_ANALYZED_TIME, VT_BSTR, ITEM_DATA_ANALYZED_TIME)
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
	DWORD dwRc, dwError;
    // 
    // Format the time
    // 
    dwRc = GetDateFormat( LOCALE_SYSTEM_DEFAULT, DATE_SHORTDATE, pst, NULL, szBuffer, 32 );
	if (dwRc == 0)
		dwError = GetLastError();
    s += szBuffer;
    s += " ";
    dwRc = GetTimeFormat( LOCALE_SYSTEM_DEFAULT, 0, pst, NULL, szBuffer, 32 );
	if (dwRc == 0)
		dwError = GetLastError();
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
    HIMAGELIST hSmallImageList =NULL;
		
	// 
    // Get icon index.
    // 
    if (m_bError)
	{
		m_uImage = 1;
		return;
	}

	SHFILEINFO sfi;
    ZeroMemory( &sfi, sizeof(SHFILEINFO) );
    hSmallImageList = (HIMAGELIST)SHGetFileInfo( m_sFileName, 
                   FILE_ATTRIBUTE_NORMAL,
                   &sfi, 
                   sizeof( SHFILEINFO ), 
                   SHGFI_SYSICONINDEX  | SHGFI_LARGEICON | SHGFI_USEFILEATTRIBUTES 
                   );

    // 
    // Need to add one to account for main icon and one for the error icon.
    // 
    m_uImage = sfi.iIcon + 2;

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

        case STATUS_DISTRIBUTE:            // 12/31/99 jhill added  
            id = IDS_STATUS_DISTRIBUTE;    // "Distribute"
            break;

        case STATUS_DISTRIBUTED:
            id = IDS_STATUS_DISTRIBUTED;   // "Distributed" 1/4/00 jhill
            break;

        case STATUS_INSTALLED:             // 7/1/99 jhill added
            id = IDS_STATUS_INSTALLED;     // "Installed"
            break;

        case STATUS_ATTENTION:             // 7/11/99 jhill added
            id = IDS_STATUS_ATTENTION;     // "Attention"
            break;

        case STATUS_ERROR:                 // 7/11/99 jhill added
            id = IDS_STATUS_ERROR;         // "Error"
            break;

        case STATUS_NOTINSTALLED:          // 12/31/99 jhill added  
            id = IDS_STATUS_NOTINSTALLED;  // "Not installed"
            break;

        case STATUS_RESTART:               // 12/31/99 jhill added  
            id = IDS_STATUS_RESTART;       // "Restart"
            break;

        case STATUS_LEGACY:                // 12/31/99 jhill added  
            id = IDS_STATUS_LEGACY;        // "Legacy"
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

BOOL CItemData::operator==( const CItemData& ciData)
{
	BOOL rc = FALSE;
	rc = Compare(ciData);
	return rc;
}

BOOL CItemData::Compare( const CItemData& itemData)
{
	BOOL rc = FALSE;

	if (m_bError != itemData.m_bError )
		return 1;

	if (m_sError != itemData.m_sError)
		return 1;

	if (m_sState!= itemData.m_sState)
		return 1;

	if (m_sDefSeqNeeded != itemData.m_sDefSeqNeeded)
		return 1;

	if (m_sFileName != itemData.m_sFileName)
		return 1;

	if (m_sFilePath != itemData.m_sFilePath)
		return 1;

	if (m_sUserName!= itemData.m_sUserName)
		return 1;

	if (m_sDomainName!= itemData.m_sDomainName)
		return 1;

	if (m_sMachineName!= itemData.m_sMachineName)
		return 1;

	if (m_sPlatform!= itemData.m_sPlatform)
		return 1;

	if (m_sProduct!= itemData.m_sProduct)
		return 1;

	if (m_sAddress!= itemData.m_sAddress)
		return 1;

	if (m_sVirusName!= itemData.m_sVirusName)
		return 1;

	if (m_ItemGUID != itemData.m_ItemGUID )
		return 1;

	if ((m_cstAge.GetTotalMinutes() - itemData.m_cstAge.GetTotalMinutes()) >= ITEM_AGE_UPDATE_INTERVAL)
		return 1;

	if (m_sSubmittedDateStr!= itemData.m_sSubmittedDateStr)
		return 1;

	if (m_sCompletedDateStr!= itemData.m_sCompletedDateStr||
		m_sComplete != itemData.m_sComplete)
		return 1;

	if (m_dwStatus!= itemData.m_dwStatus||
		m_dwSubmittedBy!= itemData.m_dwSubmittedBy||
		m_dwItemID!= itemData.m_dwItemID||
		m_ulFileSize!= itemData.m_ulFileSize||
		m_dwVirusID!= itemData.m_dwVirusID)
		return 1;

	return rc;
}




/*----------------------------------------------------------------------------
    GetStatusToken
    This is not translated text. It is the raw token strings.
    Looks up the text for X-Sample-Status 
 ----------------------------------------------------------------------------*/
BOOL GetStatusToken(CString& sStatusToken, CString& sStatusID )
{
    DWORD dwStatus = 0;
    TCHAR*  endptr=NULL;
    
    dwStatus = STRTOUL( (LPCTSTR) sStatusID, &endptr, 10 );

    switch( dwStatus )
        {
        case STATUS_QUARANTINED:
            sStatusToken = TOKEN_QUARANTINED;       // _T("quarantined");
            break;

        case STATUS_SUBMITTED:
            sStatusToken = TOKEN_SUBMITTED;         // _T("submitted");
            break;

        case STATUS_HELD:
            sStatusToken = TOKEN_HELD;              // _T("held");
            break;

        case STATUS_RELEASED:
            sStatusToken = TOKEN_RELEASED;          // _T("released");
            break;

        case STATUS_UNNEEDED:
            sStatusToken = TOKEN_UNNEEDED;          // _T("unneeded");
            break;

        case STATUS_NEEDED:
            sStatusToken = TOKEN_NEEDED;            // _T("needed");
            break;

        case STATUS_AVAILIABLE:
            sStatusToken = TOKEN_AVAILABLE;         // _T("available");
            break;

        case STATUS_DISTRIBUTE:                  
            sStatusToken = TOKEN_DISTRIBUTE;        // _T("distribute");   
            break;

        case STATUS_DISTRIBUTED:
            sStatusToken = TOKEN_DISTRIBUTED;       // _T("distributed");  
            break;

        case STATUS_INSTALLED:                 
            sStatusToken = TOKEN_INSTALLED;         // _T("installed");          
            break;

        case STATUS_ATTENTION:                 
            sStatusToken = TOKEN_ATTENTION;         // _T("attention");            
            break;

        case STATUS_ERROR:                     
            sStatusToken = TOKEN_ERROR;             // _T("error");                    
            break;

        case STATUS_NOTINSTALLED:               
            sStatusToken = TOKEN_NOTINSTALLED;      // _T("notinstalled"); 
            break;

        case STATUS_RESTART:                     
            sStatusToken = TOKEN_RESTART;           // _T("restart");      
            break;

        case STATUS_LEGACY:                      
            sStatusToken = TOKEN_LEGACY;            // _T("legacy");

        default:
            sStatusToken = TOKEN_UNKNOWN;           // _T("unknown");
            return(FALSE);
        }

    return(TRUE);
}

/*----------------------------------------------------------------------------
    FindStringInCommaDelimitedList()

    Written by: Jim Hill
 ----------------------------------------------------------------------------*/
int FindStringInCommaDelimitedList( CString& sStringList, CString& sSubString )
{
    int  index =  0;
    CString sList         = _T("");
    CString sSearchString = _T("");
    CString s             = _T("");

    // INITIALIZE
    s = sStringList;
    s.TrimLeft();
    s.TrimRight();
    sSubString.TrimLeft();
    sSubString.TrimRight();
    sList.Format( _T(",%s,"), s  );
    sSearchString.Format( _T(",%s,"), sSubString );

    // SEARCH
    index = sList.Find( sSearchString );

    // // MAKE THE INDEX 0 BASED AGAIN, BECAUSE IT'S POINTING AT THE LEAD COMMA
    // if( index > 0 )
    //     --index;

    return( index );
}


/*----------------------------------------------------------------------------
    IsSampleErrorCondition()
    Added to QsConsole code

    This provides a method for QsConsole to change the icon of a sample
    if there are any errors. This is to aid the user in visually identifying 
    those samples needing manual attention.

    This requires the following Sample Attributes:
    X-Sample-Status       // DWORD stored as a string
    X-Sample-Result       // string

    The possible error conditions are:
    1. Sample Intervention Needed
       X-Sample-Status attribute is one of the following:
       "held,available,attention,error,notinstalled"

    2. Local Quarantine Intervention Needed
       X-Sample-Status attribute is one of the following: "unneeded,installed"
       And X-Sample-Result (X-Scan-Result?tm) does NOT contain "repaired"

    3. Status too long In-State

    Written by: Jim Hill
 ----------------------------------------------------------------------------*/
BOOL CItemData::IsSampleErrorCondition( )
{
    CString s               = _T("");
    CString sTemp           = _T("");
    CString sTemp2          = _T("");
    CString sTemp3          = _T("");
    CString sSampleStatusID = _T("");
    CString sSampleResult   = _T("");
    CString sSampleAlertStatusTokenIDs       = _T("");
    CString sQuarantineAlertStatusTokenIDs   = _T("");
    CString sSampleAlertStatusTimerTokenList = _T("");
    CString sAlertStatus                     = _T("");
    CString sState		                     = _T("");

    // INITIALIZE
    sSampleAlertStatusTokenIDs.Format(_T("%d,%d,%d,%d,%d"),STATUS_HELD,STATUS_AVAILIABLE,STATUS_ATTENTION,STATUS_ERROR,STATUS_NOTINSTALLED);  
    sQuarantineAlertStatusTokenIDs.Format(_T("%d,%d"),STATUS_UNNEEDED,STATUS_INSTALLED);  

    try
    {
		sSampleStatusID = m_sStatus;
    	sSampleResult   = m_sScanResult;
        sAlertStatus    = m_sAlertStatus;
		sState			= m_sState;

        // IS THIS A SAMPLE INTERVENTION ALERT CONDITION?  X-Sample-Status is a Dword stored as a string.
        if( FindStringInCommaDelimitedList( sSampleAlertStatusTokenIDs, sSampleStatusID ) >= 0 )             // is it in the list
		{
            return(TRUE);
		}

        // IS THIS A LOCAL QUARANTINE INTERVENTION ALERT CONDITION?
        if( FindStringInCommaDelimitedList( sQuarantineAlertStatusTokenIDs, sSampleStatusID ) >= 0 )         // is it in the list
        {
            sTemp = QUARANTINE_ALERT_RESULT_TOKENS;
			sTemp2 = QUARANTINE_SAMPLE_STATE_TOKENS;
			sTemp3 = QUARANTINE_ALERT_STATUS_TOKENS;
            if( (FindStringInCommaDelimitedList( sTemp, sSampleResult ) == -1) &&  // condition != "repaired"  &&
				!(FindStringInCommaDelimitedList( sTemp2, sState ) >= 0 &&         // !((State == "infected) && (Alert Status == uneeded))
					FindStringInCommaDelimitedList( sTemp3, sAlertStatus) >= 0	))  
			{
				return(TRUE);
			}
        }

        // IS THIS A TIME IN STATE CONDITION?   QSITEMINFO_X_ALERT_STATUS    "X-Alert-Status"
        CString sSampleStatus = _T("");
        TCHAR*  endptr        = NULL;
        int iStatusID         = STRTOUL( (LPCTSTR)sSampleStatusID, &endptr, 10 );
        if( iStatusID >= 0 )
        {
            CString sStatusID;
            sStatusID.Format( _T("%d"), iStatusID );
            if( GetStatusToken( s, sStatusID ) )
                sSampleStatus = s;
        }
        if( sSampleStatus.IsEmpty() )
            return(FALSE);

        // CHECK FOR TIME IN STATE ERRORS. INCLUDES ONLY ONES WITH TIMERS
        sTemp = SAMPLE_ALERT_STATUS_TIMER_TOKEN_LIST;   
        if( FindStringInCommaDelimitedList( sTemp, sSampleStatus ) >= 0 )        // is it in the list
        {
            if( sSampleStatus == sAlertStatus )
                return(TRUE);
        }

    }
    catch(...) 
    {
    }
    return(FALSE);
}


#if 0
// jhill 2/22/00
        if(!m_sStatus.IsEmpty())
			sSampleStatusID         = ","+m_sStatus;
		if (!m_sScanResult.IsEmpty())
			sSampleResult           = m_sScanResult;

        // IS THIS A SAMPLE INTERVENTION ALERT CONDITION?  X-Sample-Status is a Dword stored as a string.
        if( sSampleAlertStatusTokenIDs.Find(sSampleStatusID) >= 0 )             // is it in the list
		{
            return(TRUE);
		}
        // IS THIS A LOCAL QUARANTINE INTERVENTION ALERT CONDITION?
        if( sQuarantineAlertStatusTokenIDs.Find(sSampleStatusID) >= 0 )             // is it in the list
        {
            sTemp = QUARANTINE_ALERT_RESULT_TOKENS;
            if( sTemp.Find(sSampleResult) == -1 )        // condition != "repaired"
			{
				return(TRUE);
			}
        }
#endif


#if 0 // not used any more
///////////////////////////////////////////////////////////////////////////////
//
// Function name : CItemData::ConvertTimeFromUTC2LT
//
// Description   : Converts a UTC time to a local time based on the time zone info 
//
// Return type   : none
//
//
///////////////////////////////////////////////////////////////////////////////
// 10-11-00: Terrym Function created / header added 
///////////////////////////////////////////////////////////////////////////////

void CItemData::ConvertTimeFromUTC2LT(LPSYSTEMTIME lptime)
{
	TIME_ZONE_INFORMATION tzitemp = {0};
	if (memcmp (&m_tzi,&tzitemp,sizeof(TIME_ZONE_INFORMATION))==0)
	{
			m_tzRc = GetTimeZoneInformation(&m_tzi);
	}

	lptime->wHour -= m_tzi.Bias/60;
	switch (m_tzRc)
	{
	case TIME_ZONE_ID_UNKNOWN:
	case TIME_ZONE_ID_STANDARD:
		lptime->wHour -= m_tzi.StandardBias/60;
		break;
	case TIME_ZONE_ID_DAYLIGHT:
		lptime->wHour -= m_tzi.DaylightBias/60;
		break;
	}

}

#endif