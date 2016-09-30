/*
 * PROPRIETARY/CONFIDENTIAL.  use of this product is subject to license terms.
 * Copyright (c) 2005 Symantec Corporation.  All Rights Reserved.
 *
*/


#include "stdafx.h"
#include "AvisConsole.h"
#ifndef  _WIN32_DCOM
   #define _WIN32_DCOM
#endif
#include "safearray.h"
#include "GetAllAttributes.h"
#include "SampleAttributesPage.h"
#include "filenameparse.h"
#include "AvisEventDetection.h" 
#include "vpstrutils.h"


//SAFEARRAY defined in afxdisp.h

typedef struct tag_xattributes_struct{
    LPCTSTR  lpszXHeader;
    DWORD    dwID;        // int
    DWORD    dwFormat;
} X_ATTRIBUTES_STRUCT;



// USE THIS TABLE TO FORMATING OF A KNOWN ATTRIBUTE
static X_ATTRIBUTES_STRUCT XAttributeTable[] =  
{     // X-Header                                 myID                                        Fromat
    { AVIS_X_PLATFORM_USER                      , AVIS_ID_X_PLATFORM_USER                     , 0 } ,
    { AVIS_X_PLATFORM_COMPUTER                  , AVIS_ID_X_PLATFORM_COMPUTER                 , 0 } ,
    { AVIS_X_PLATFORM_DOMAIN                    , AVIS_ID_X_PLATFORM_DOMAIN                   , 0 } ,
    { AVIS_X_PLATFORM_ADDRESS                   , AVIS_ID_X_PLATFORM_ADDRESS                  , 0 } ,
    { AVIS_X_PLATFORM_PROCESSOR                 , AVIS_ID_X_PLATFORM_PROCESSOR                , 0 } ,
    { AVIS_X_PLATFORM_DISTRIBUTOR               , AVIS_ID_X_PLATFORM_DISTRIBUTOR              , 0 } ,
    { AVIS_X_PLATFORM_HOST                      , AVIS_ID_X_PLATFORM_HOST                     , 0 } ,
    { AVIS_X_PLATFORM_SYSTEM                    , AVIS_ID_X_PLATFORM_SYSTEM                   , 0 } ,
    { AVIS_X_PLATFORM_LANGUAGE                  , AVIS_ID_X_PLATFORM_LANGUAGE                 , 0 } ,
    { AVIS_X_PLATFORM_OWNER                     , AVIS_ID_X_PLATFORM_OWNER                    , 0 } ,
    { AVIS_X_PLATFORM_SCANNER                   , AVIS_ID_X_PLATFORM_SCANNER                  , 0 } ,
    { AVIS_X_PLATFORM_CORRELATOR                , AVIS_ID_X_PLATFORM_CORRELATOR               , 0 } ,
    { AVIS_X_PLATFORM_GUID                      , AVIS_ID_X_PLATFORM_GUID                     , 0 } ,
                                                                                                
    { AVIS_X_DATE_ACCESSED                      , AVIS_ID_X_DATE_ACCESSED                     , 0 } ,
    { AVIS_X_DATE_ANALYZED                      , AVIS_ID_X_DATE_ANALYZED                     , 0 } ,
    { AVIS_X_DATE_BLESSED                       , AVIS_ID_X_DATE_BLESSED                      , 0 } ,
    { AVIS_X_DATE_CAPTURED                      , AVIS_ID_X_DATE_CAPTURED                     , 0 } ,
    { AVIS_X_DATE_CREATED                       , AVIS_ID_X_DATE_CREATED                      , 0 } ,
    { AVIS_X_DATE_DISTRIBUTED                   , AVIS_ID_X_DATE_DISTRIBUTED                  , 0 } ,
    { AVIS_X_DATE_FINISHED                      , AVIS_ID_X_DATE_FINISHED                     , 0 } ,
    { AVIS_X_DATE_FORWARDED                     , AVIS_ID_X_DATE_FORWARDED                    , 0 } ,
    { AVIS_X_DATE_INSTALLED                     , AVIS_ID_X_DATE_INSTALLED                    , 0 } ,
    { AVIS_X_DATE_MODIFIED                      , AVIS_ID_X_DATE_MODIFIED                     , 0 } ,
    { AVIS_X_DATE_PRODUCED                      , AVIS_ID_X_DATE_PRODUCED                     , 0 } ,
    { AVIS_X_DATE_PUBLISHED                     , AVIS_ID_X_DATE_PUBLISHED                    , 0 } ,
    { AVIS_X_DATE_SUBMITTED                     , AVIS_ID_X_DATE_SUBMITTED                    , 0 } ,
    { AVIS_X_DATE_ANALYSIS_FINISHED             , AVIS_ID_X_DATE_ANALYSIS_FINISHED            , 0 } ,
    { AVIS_X_DATE_SAMPLE_FINISHED               , AVIS_ID_X_DATE_SAMPLE_FINISHED              , 0 } ,
    { AVIS_X_DATE_QUARANTINEDATE                , AVIS_ID_X_DATE_QUARANTINEDATE               , 0 } ,
    { AVIS_X_DATE_QUARANTINE                    , AVIS_ID_X_DATE_QUARANTINE                   , 0 } ,
                                                                                                
    { AVIS_X_SCAN_VIRUS_IDENTIFIER              , AVIS_ID_X_SCAN_VIRUS_IDENTIFIER             , 0 } ,
    { AVIS_X_SCAN_SIGNATURE_SEQUENCE            , AVIS_ID_X_SCAN_SIGNATURE_SEQUENCE           , 0 } ,
    { AVIS_X_SCAN_SIGNATURE_VERSION             , AVIS_ID_X_SCAN_SIGNATURE_VERSION            , 0 } ,
    { AVIS_X_SCAN_SIGNATURES_SEQUENCE           , AVIS_ID_X_SCAN_SIGNATURES_SEQUENCE          , 0 } ,
    { AVIS_X_SCAN_SIGNATURES_VERSION            , AVIS_ID_X_SCAN_SIGNATURES_VERSION           , 0 } ,
    { AVIS_X_SCAN_VIRUS_NAME                    , AVIS_ID_X_SCAN_VIRUS_NAME                   , 0 } ,
    { AVIS_X_SCAN_RESULT                        , AVIS_ID_X_SCAN_RESULT                       , 0 } ,
                                                                                                
    { AVIS_X_SAMPLE_CHECKSUM                    , AVIS_ID_X_SAMPLE_CHECKSUM                   , 0 } ,
    { AVIS_X_CHECKSUM_METHOD                    , AVIS_ID_X_CHECKSUM_METHOD                   , 0 } ,
    { AVIS_X_SAMPLE_EXTENSION                   , AVIS_ID_X_SAMPLE_EXTENSION                  , 0 } ,
    { AVIS_X_SAMPLE_FILE                        , AVIS_ID_X_SAMPLE_FILE                       , 0 } ,
    { AVIS_X_SAMPLE_SIZE                        , AVIS_ID_X_SAMPLE_SIZE                       , 0 } ,
    { AVIS_X_SAMPLE_TYPE                        , AVIS_ID_X_SAMPLE_TYPE                       , 0 } ,
    { AVIS_X_SAMPLE_REASON                      , AVIS_ID_X_SAMPLE_REASON                     , 0 } ,
    { AVIS_X_SAMPLE_GEOMETRY                    , AVIS_ID_X_SAMPLE_GEOMETRY                   , 0 } ,
    { AVIS_X_SAMPLE_STATUS                      , AVIS_ID_X_SAMPLE_STATUS                     , AVIS_ATTRIBUTE_FORMAT_LOOKUP_SAMPLE_STATUS_TEXT } ,
    { AVIS_X_SAMPLE_SUBMISSION_ROUTE            , AVIS_ID_X_SAMPLE_SUBMISSION_ROUTE           , 0 } ,
    { AVIS_X_SAMPLE_UUID                        , AVIS_ID_X_SAMPLE_UUID                       , 0 } ,
    { AVIS_X_SAMPLE_FILEID                      , AVIS_ID_X_SAMPLE_FILEID                     , AVIS_ATTRIBUTE_FORMAT_HEX_STRING } ,
    { AVIS_X_SAMPLE_CHANGES                     , AVIS_ID_X_SAMPLE_CHANGES                    , 0 } ,
    { AVIS_X_SAMPLE_PRIORITY                    , AVIS_ID_X_SAMPLE_PRIORITY                   , 0 } ,
    { AVIS_X_SAMPLE_SIGNATURES_PRIORITY         , AVIS_ID_X_SAMPLE_SIGNATURES_PRIORITY        , 0 } ,
    { AVIS_X_SAMPLE_SECTOR                      , AVIS_ID_X_SAMPLE_SECTOR                     , 0 } ,
    { AVIS_X_SAMPLE_SERVICE                     , AVIS_ID_X_SAMPLE_SERVICE                    , 0 } ,
    { AVIS_X_SAMPLE_STRIP                       , AVIS_ID_X_SAMPLE_STRIP                      , 0 } ,
    { AVIS_X_SAMPLE_CATEGORY                    , AVIS_ID_X_SAMPLE_CATEGORY                   , 0 } ,
                                                                                                
    { AVIS_X_ANALYSIS_STATE                     , AVIS_ID_X_ANALYSIS_STATE                    , 0 } ,
    { AVIS_X_ANALYSIS_COOKIE                    , AVIS_ID_X_ANALYSIS_COOKIE                   , 0 } ,
    { AVIS_X_ANALYSIS_ISSUE                     , AVIS_ID_X_ANALYSIS_ISSUE                    , 0 } ,
    { AVIS_X_ANALYSIS_VIRUS_NAME                , AVIS_ID_X_ANALYSIS_VIRUS_NAME               , 0 } ,
    { AVIS_X_ANALYSIS_SERVICE                   , AVIS_ID_X_ANALYSIS_SERVICE                  , 0 } ,
    { AVIS_X_ANALYSIS_VIRUS_IDENTIFIER          , AVIS_ID_X_ANALYSIS_VIRUS_IDENTIFIER         , 0 } ,
                                                                                                
    { AVIS_X_SIGNATURES_SEQUENCE                , AVIS_ID_X_SIGNATURES_SEQUENCE               , 0 } ,
    { AVIS_X_SIGNATURES_PRIORITY                , AVIS_ID_X_SIGNATURES_PRIORITY               , 0 } ,
    { AVIS_X_SIGNATURES_NAME                    , AVIS_ID_X_SIGNATURES_NAME                   , 0 } ,
    { AVIS_X_SIGNATURES_SIZE                    , AVIS_ID_X_SIGNATURES_SIZE                   , 0 } ,
    { AVIS_X_SIGNATURES_VERSION                 , AVIS_ID_X_SIGNATURES_VERSION                , 0 } ,
    { AVIS_X_SIGNATURES_NAME_X_SAMPLE_CHECKSUM  , AVIS_ID_X_SIGNATURES_NAME_X_SAMPLE_CHECKSUM , 0 } ,
                                                                                                
    { AVIS_X_CONTENT_CHECKSUM                   , AVIS_ID_X_CONTENT_CHECKSUM                  , 0 } ,
    { AVIS_X_CONTENT_COMPRESSION                , AVIS_ID_X_CONTENT_COMPRESSION               , 0 } ,
    { AVIS_X_CONTENT_ENCODING                   , AVIS_ID_X_CONTENT_ENCODING                  , 0 } ,
    { AVIS_X_CONTENT_SCRAMBLING                 , AVIS_ID_X_CONTENT_SCRAMBLING                , 0 } ,
                                                                                                
    { AVIS_X_CUSTOMER_CONTACT_EMAIL             , AVIS_ID_X_CUSTOMER_CONTACT_EMAIL            , 0 } ,
    { AVIS_X_CUSTOMER_CONTACT_NAME              , AVIS_ID_X_CUSTOMER_CONTACT_NAME             , 0 } ,
    { AVIS_X_CUSTOMER_CONTACT_TELEPHONE         , AVIS_ID_X_CUSTOMER_CONTACT_TELEPHONE        , 0 } ,
    { AVIS_X_CUSTOMER_CREDENTIALS               , AVIS_ID_X_CUSTOMER_CREDENTIALS              , 0 } ,
    { AVIS_X_CUSTOMER_IDENTIFIER                , AVIS_ID_X_CUSTOMER_IDENTIFIER               , 0 } ,
    { AVIS_X_CUSTOMER_NAME                      , AVIS_ID_X_CUSTOMER_NAME                     , 0 } ,

    { AVIS_X_PLATFORM_INFO_DELIVER              , AVIS_ID_PLATFORM_INFO_DELIVER               , 0 } ,
    { AVIS_X_ERROR                              , AVIS_ID_X_ERROR                             , 0 } ,
    { AVIS_X_ATTENTION                          , AVIS_ID_X_ATTENTION                         , 0 } ,
    { AVIS_X_ALERT_STATUS                       , AVIS_ID_X_ALERT_STATUS                      , 0 } ,
    { AVIS_X_DATE_ALERT                         , AVIS_ID_X_DATE_ALERT                        , 0 } ,
    { AVIS_X_ALERT_RESULT                       , AVIS_ID_X_ALERT_RESULT                      , AVIS_ATTRIBUTE_FORMAT_EXCLUDE } ,  //     
    { AVIS_X_ALERT_STATUS_TIMER                 , AVIS_ID_X_ALERT_STATUS_TIMER                , AVIS_ATTRIBUTE_FORMAT_EXCLUDE } ,  // 
    

    { NULL, 0xFFFF, 0},
    { NULL,	-1, 0 }
};




extern int g_GetAllAttributesClassTotalCount = 0;



//_safearray_t< 2, VARIANT, Variant> m_xsa;



/*----------------------------------------------------------------------------
    GetFormatByAttributeText()

    Written by: Jim Hill
 ----------------------------------------------------------------------------*/
DWORD GetFormatByAttributeText(LPCTSTR lpszXAttributeName)  
{
    int   i;

    for( i = 0 ; ; i++ )
    {
        if( XAttributeTable[i].dwID == 0xFFFF || XAttributeTable[i].lpszXHeader == NULL )
             break;

        if( STRCMP( lpszXAttributeName, XAttributeTable[i].lpszXHeader) == 0)
            return(XAttributeTable[i].dwFormat);
    }

//#ifndef MY_SYSTEM
    // IS IT A BACKUP ATTRIBUTE? IF SO, THEN DON'T DISPLAY   4/4/00  
    if( STRNCMP( lpszXAttributeName, AVIS_WILDCARD_X_BACKUP, STRLEN( AVIS_WILDCARD_X_BACKUP ) ) == 0)
        return( AVIS_ATTRIBUTE_FORMAT_EXCLUDE );
//#endif



    return( AVIS_ATTRIBUTE_FORMAT_USE_DEFAULT );
}



/*----------------------------------------------------------------------------
    CQServerGetAllAttributes::CQServerGetAllAttributes
    Written by: Jim Hill
 ----------------------------------------------------------------------------*/
CQServerGetAllAttributes::CQServerGetAllAttributes( IDataObject* pDataObject, 
                                                    DWORD iExtDataObjectID,
                                                    IUnknown* pUnk ,
													SEC_WINNT_AUTH_IDENTITY *pIdentity) 
{
    HRESULT  hr = S_OK;

    ++g_GetAllAttributesClassTotalCount;
    m_iExtDataObjectID = iExtDataObjectID;
	fWidePrintString("CQServerGetAllAttributes constructor called. Caller's ObjectID= %d  TotalCount= %d", 
                                          m_iExtDataObjectID, g_GetAllAttributesClassTotalCount);
    m_pDataObject       = pDataObject;
    m_pUnk              = pUnk;
    m_dwIsMultiSelect   = 0;
    m_pResultItem       = NULL;
    m_pQServerItem      = NULL;
    m_ulItems           = NULL;
    m_ulCount           = 0;
    m_dwIsDataAvailable = 0;
    m_dwNumFields       = 0;
    VariantInit( &m_va );
    VariantClear( &m_va );
    m_dwRef             = 0;
    m_dwRegistered      = 0;
    m_dwObjSignature    = MY_OBJECT_SIGNATURE;
    m_iQserverInterfaceError = S_OK;
    m_dwIsDisabled      = 0;
    m_bOkToReSubmit     = TRUE;
	m_pIdentity			= pIdentity;// added to store the connection identity tam 7-19-00

    // EXCLUSION LIST          // AVIS_X_SCAN_SIGNATURES_SEQUENCE    AVIS_X_SCAN_SIGNATURES_VERSION
    m_sAttributeDeleteExcludedList.Format(_T("%s,%s,%s,%s,%s, %s,%s,%s,%s,%s, %s,%s,%s,%s,%s, %s,%s,%s,%s,%s, %s,%s,%s,%s,%s, %s,%s,%s,%s,%s, "),  
                AVIS_X_ALERT_STATUS_TIMER,     AVIS_X_SAMPLE_CHANGES,    AVIS_X_SAMPLE_PRIORITY,  AVIS_X_SIGNATURES_PRIORITY,   AVIS_X_SUBMISSION_COUNT, 
                AVIS_X_PLATFORM_USER,          AVIS_X_PLATFORM_COMPUTER, AVIS_X_PLATFORM_DOMAIN,  AVIS_X_PLATFORM_ADDRESS,      AVIS_X_PLATFORM_PROCESSOR,   
                AVIS_X_PLATFORM_HOST,          AVIS_X_PLATFORM_SYSTEM,   AVIS_X_PLATFORM_LANGUAGE,AVIS_X_PLATFORM_SCANNER,      AVIS_X_PLATFORM_GUID,        
                AVIS_X_PLATFORM_INFO_DELIVER,  AVIS_X_DATE_CREATED,      AVIS_X_DATE_QUARANTINE,  AVIS_X_SCAN_VIRUS_IDENTIFIER, AVIS_X_SAMPLE_FILEID,
                AVIS_X_SAMPLE_UUID,            AVIS_X_SCAN_VIRUS_NAME,   AVIS_X_SCAN_RESULT,      AVIS_X_SAMPLE_CHECKSUM,       AVIS_X_CHECKSUM_METHOD,  
                AVIS_X_SAMPLE_EXTENSION,       AVIS_X_SAMPLE_FILE,       AVIS_X_SAMPLE_SIZE,      AVIS_X_SAMPLE_TYPE,           AVIS_X_SAMPLE_REASON 
                );                                                                                                      

    m_sAttributeRestoreExcludedList.Format(_T("%s,%s,%s,%s,%s, "),  
                AVIS_X_ALERT_STATUS_TIMER,     AVIS_X_SAMPLE_CHANGES,    AVIS_X_SAMPLE_PRIORITY,  AVIS_X_SIGNATURES_PRIORITY,   AVIS_X_SUBMISSION_COUNT );



    // MULTI SELECT?
    if( m_pUnk != NULL )
    {
        m_dwIsMultiSelect = TRUE;
        hr = MarshallInterfaceMultiSelect();
        //if( IsSuccessfullResult( hr ) )

    }
    else
    {
        m_dwIsMultiSelect = FALSE;
        hr = MarshallInterfaceSingleSelect();
    }


}


/*----------------------------------------------------------------------------
    Written by: Jim Hill
 ----------------------------------------------------------------------------*/
HRESULT CQServerGetAllAttributes::Initialize()
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState());
    HRESULT  hr = 0;
	fWidePrintString("CQServerGetAllAttributes::Initialize called. Caller's ObjectID= %d  TotalCount= %d m_dwRef= %d", 
                                      m_iExtDataObjectID, g_GetAllAttributesClassTotalCount, m_dwRef);


    InterlockedIncrement( &m_dwRegistered );

    if( IsDataAvailable() )
        return(S_OK);
    else
    {
        InterlockedIncrement( &m_dwRef );
        hr = GetAllAttributes();
        if( IsDataAvailable() )
        {
			USES_CONVERSION;
            VARIANT v;
            VariantInit( &v );
            VariantClear( &v );
            TCHAR szFullPath[MAX_PATH*2];
            CString s = AVIS_X_SAMPLE_FILE;
            memset( szFullPath, 0, sizeof(szFullPath) );  
            memset( m_szFileName, 0, sizeof(m_szFileName) );  
            hr = GetValueByAttributeName( s, &v ); // X-Sample-File
            if( IsSuccessfullSampleResult( hr )  && v.vt == VT_BSTR )
            {
                vpstrncpy( szFullPath, OLE2T(v.bstrVal ), sizeof(szFullPath));

                // STRIP OFF PATH.
                CFileNameParse::GetFileName( szFullPath, m_szFileName, MAX_PATH );    // v.bstrVal
                // TEST
                //hr = GetValueByAttributeName( AVIS_X_SAMPLE_FILE, &v ); // X-Sample-File
				VariantClear(&v);
            }
            return(S_OK);
        }
        else
        {   // ERROR READING ATTRIBUTES. ADJUST REF COUNT. 7/14/99 JHILL
            m_dwRef        = 0;
            m_dwRegistered = 0;
        }
        if( hr == S_OK )
            hr = E_FAIL;
        return( hr );
    }
}


/*----------------------------------------------------------------------------
    CQServerGetAllAttributes::MarshallInterfaceMultiSelect()

    Called from AttributeAccess.cpp on Result Pane thread.

    Error messages equivalent to the ones in MarshallInterfaceSingleSelect()
    are displayed from CAvisResultItems::Initialize() in SubmitOrDeliver.cpp.

    Written by: Jim Hill
 ----------------------------------------------------------------------------*/
HRESULT CQServerGetAllAttributes::MarshallInterfaceMultiSelect()
{
    HRESULT hr = S_OK;

    fWidePrintString("CQServerGetAllAttributes::MarshallInterfaceMultiSelect called. Caller's ObjectID= %d  TotalCount= %d", 
                      m_iExtDataObjectID, g_GetAllAttributesClassTotalCount );

    try
    {

         // m_pUnk HAS QSERVER ITEM
         if(m_pUnk == NULL) {         // hr == RPC_E_WRONG_THREAD
             fWidePrintString("FAILURE: CQServerGetAllAttributes::MarshallInterfaceMultiSelect. GetQserverItem(). ObjectID= %d  TotalCount= %d", 
                               m_iExtDataObjectID, g_GetAllAttributesClassTotalCount );
             return(E_NOINTERFACE);       
         }

         fWidePrintString("CQServerGetAllAttributes::MarshallInterfaceMultiSelect. m_pUnk=0x%X  ObjectID= %d  TotalCount= %d", 
                           ((DWORD)m_pUnk), m_iExtDataObjectID, g_GetAllAttributesClassTotalCount );
         

         CComPtr< IQuarantineServerItem > pQServerItem;
         pQServerItem = (IQuarantineServerItem*)m_pUnk;
  
         // NEED TO MARSHAL THIS INTERFACE SO THE LOGIC IS CONSISTENT WITH SINGLE SELECT .
         hr = CoMarshalInterThreadInterfaceInStream( __uuidof( IQuarantineServerItem ), pQServerItem, &m_pServerStream ); 
         if( FAILED( hr ) )
         {
             fWidePrintString("FAILURE: CQServerGetAllAttributes CoMarshalInterThreadInterfaceInStream. MultiSelect. ObjectID= %d  TotalCount= %d  Error= 0x%08X", 
                               m_iExtDataObjectID, g_GetAllAttributesClassTotalCount, hr );
             return(E_FAIL);
         }
  
         // CALLER WILL RELEASE m_pResultItem AND m_pUnk WHEN WE'RE MULTI SELECT
         m_pResultItem = NULL;
         m_pUnk = NULL;

    }
    catch(...) 
    {
        fWidePrintString("FAILURE: CQServerGetAllAttributes::MarshallInterfaceMultiSelect called. GetQserverItem(). ObjectID= %d", m_iExtDataObjectID );
        return(E_FAIL);
    }

    return(hr);
}   // MarshallInterfaceMultiSelect   



/*----------------------------------------------------------------------------
    CQServerGetAllAttributes::MarshallInterfaceSingleSelect()

    Called from AttributeAccess.cpp on Result Pane thread.

    Written by: Jim Hill
 ----------------------------------------------------------------------------*/
HRESULT CQServerGetAllAttributes::MarshallInterfaceSingleSelect()
{
    HRESULT hr = S_OK;
    CString sText;
    sText.Empty();

    fWidePrintString("CQServerGetAllAttributes::MarshallInterface called. Caller's ObjectID= %d  TotalCount= %d", 
                      m_iExtDataObjectID, g_GetAllAttributesClassTotalCount );

    try
    {

         // GET THE INTERFACE 
         hr = m_pDataObject->QueryInterface( __uuidof( IQCResultItem ), (LPVOID*) &m_pResultItem );  // m_pResultItem
         if( FAILED( hr ) )
         {    
             fWidePrintString("FAILURE: CQServerGetAllAttributes::MarshallInterfaceSingleSelect  QueryInterface(). ObjectID= %d  TotalCount= %d  Error= 0x%08X", 
                               m_iExtDataObjectID, g_GetAllAttributesClassTotalCount, hr );
             _com_issue_error( hr );      // E_NOINTERFACE
         }
      
         // HOW MANY ARE THERE
         m_pResultItem->GetItemIDs( &m_ulCount, &m_ulItems );
         
         // GET QSERVER ITEM
         hr = m_pResultItem->GetQserverItem( m_ulItems[0], &m_pUnk , (ULONG *)&m_pIdentity);
         if(m_pUnk == NULL) {         // hr == RPC_E_WRONG_THREAD
             fWidePrintString("FAILURE: CQServerGetAllAttributes::MarshallInterfaceSingleSelect. GetQserverItem(). ObjectID= %d  TotalCount= %d  Error= 0x%08X", 
                               m_iExtDataObjectID, g_GetAllAttributesClassTotalCount, hr );
             _com_issue_error( hr );
         }
         
         CComPtr< IQuarantineServerItem > pQServerItem;
         pQServerItem = (IQuarantineServerItem*)m_pUnk;
  
         // NEED TO MARSHAL THIS INTERFACE OVER TO THE PROPERTY PAGE, SINCE IT RUNS IN A SEPARATE THREAD.
         hr = CoMarshalInterThreadInterfaceInStream( __uuidof( IQuarantineServerItem ), pQServerItem, &m_pServerStream ); // m_pUnk  pResultItem
         if( FAILED( hr ) )
         {
             fWidePrintString("FAILURE: CQServerGetAllAttributes CoMarshalInterThreadInterfaceInStream. SingleSelect. ObjectID= %d  Error= 0x%08X", m_iExtDataObjectID, hr );
             _com_issue_error( hr );
         }


         // RELEASING RESULTITEM AND IUNKNOWN INTERFACE 
         // ON RESULT PANE THREAD INSTEAD OF PROP PAGE THREAD
         if( m_ulItems != NULL )
             CoTaskMemFree( m_ulItems );
         // RELEASE m_pResultItem IF WE'RE SINGLE SELECT
         if( m_pResultItem != NULL )
             m_pResultItem->Release();
         m_ulItems     = NULL;
         m_pResultItem = NULL;
         if( m_pUnk != NULL )      
             m_pUnk->Release();      
         m_pUnk = NULL;

    }
    catch( _com_error e )
    {
        sText.LoadString( IDS_COM_ERROR_READING_FROM_QSERVER );
        hr = e.Error();                // RPC_S_SERVER_UNAVAILABLE
        sText += e.ErrorMessage();
    }
    catch(...) 
    {
        sText.LoadString( IDS_ERROR_READING_FROM_QSERVER );
        if( hr == S_OK )
        {
            hr = E_FAIL;
        }
        else
        {
            CString sErrorString;
            CStringFormatErrorCode( sErrorString, hr );
            sText += sErrorString;
        }
        fWidePrintString("FAILURE: CQServerGetAllAttributes::MarshallInterface called. GetQserverItem(). ObjectID= %d", m_iExtDataObjectID );
        //return(E_FAIL);
    }
    m_iQserverInterfaceError = hr;

    // IS THERE AN ERROR STRING TO DISPLAY?
    if( !sText.IsEmpty() )
    {
        ShowErrorMessage( sText, IDS_TITLE_ERROR_READING_FROM_QSERVER, 0, hr, g_ServerDisplayName );
    }

    return(hr);
}   // MarshallInterfaceSingleSelect



 //CComPtr< IQCResultItem > pResultItem;
 //CComPtr< IDataObject > pDataObject;
 //pDataObject = m_pDataObject;
 //CComPtr< IQCResultItem > pResultItem;
//hr = CoMarshalInterThreadInterfaceInStream( __uuidof( IQCResultItem ), pResultItem, &m_pServerStream ); // m_pUnk  pResultItem
//hr = CoMarshalInterThreadInterfaceInStream( __uuidof( IDataObject ), pDataObject, &m_pServerStream ); // m_pUnk  pResultItem


/*----------------------------------------------------------------------------

    Written by: Jim Hill
 ----------------------------------------------------------------------------*/
CQServerGetAllAttributes::~CQServerGetAllAttributes()
{
    --g_GetAllAttributesClassTotalCount;
  	fWidePrintString("CQServerGetAllAttributes destructor called. Caller's ObjectID= %d  TotalCount= %d", 
                                       m_iExtDataObjectID, g_GetAllAttributesClassTotalCount );
    m_dwObjSignature = 0;
}


/*----------------------------------------------------------------------------

    Written by: Jim Hill
 ----------------------------------------------------------------------------*/
LONG CQServerGetAllAttributes::Release()
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState());

    // IF THE SIGNATURE IS NOT ==, THEN THE CQServerGetAllAttributes OBJECT HAS ALREADY BEEN DESTROYED.
    if( m_dwObjSignature != MY_OBJECT_SIGNATURE )
    {
        fWidePrintString("CQServerGetAllAttributes::Release called after Object Destroyed.");
        return(0);
    }

    fWidePrintString("CQServerGetAllAttributes::Release called. m_dwRef= %d  Registered= %d", m_dwRef, m_dwRegistered );
    InterlockedDecrement(&m_dwRef);

    // SYNCHRONIZE ENTRY, SO ONLY ONE CALLER SUCCEEDS 5/13/99 
    if( m_dwRef < 0 && m_dwRegistered != 0 )
        return(0);

    if( m_dwRef == 0 || m_dwRegistered == 0 )
    {
	    fWidePrintString("CQServerGetAllAttributes::Release called. Deleting ObjectID= %d  m_dwRef= %d Registered= %d", m_iExtDataObjectID, m_dwRef, m_dwRegistered );

        // LOCK ANYONE ELSE OUT
        InterlockedDecrement(&m_dwRef);

        // FREE THE MEMORY
        DestroyAllData();

        // RELEASE INTERFACE
        ReleaseSeverInterface();

        // DELETE CQServerGetAllAttributes 
        delete this;
        return 0;
    }
    return(m_dwRef);
}



/*----------------------------------------------------------------------------
    CQServerGetAllAttributes::ReleaseSeverInterface()

    Written by: Jim Hill
 ----------------------------------------------------------------------------*/
HRESULT CQServerGetAllAttributes::ReleaseSeverInterface()
{
    fWidePrintString("CQServerGetAllAttributes::ReleaseSeverInterface() called. ObjectID= %d m_dwRef= %d", m_iExtDataObjectID, m_dwRef );

    // IS THIS THE CASE WHERE OBJ WAS CREATED, BUT NOT INITIALIZED BECAUSE THE PROP PAGES WERE NOT OPENED?  7/26/99
    if( m_pQServerItem == NULL && m_pServerStream != NULL )   // jhill          
    {
        // UNMARSHAL SERVER INTERFACE.
        CoGetInterfaceAndReleaseStream( m_pServerStream, __uuidof( IQuarantineServerItem ), (LPVOID*)&m_pQServerItem );
        m_pServerStream = NULL;

    }
    if( m_pQServerItem != NULL )            
    {
        m_pQServerItem->Release();      
        m_pQServerItem = NULL;
        m_pUnk         = NULL;
    }

    // THESE WERE RELEASED FROM MARSHALL INTERFACE
    if( m_pUnk != NULL )            // SAME AS m_pQServerItem
    {
        m_pUnk->Release();      
    }

    if( m_ulItems != NULL )
        CoTaskMemFree( m_ulItems );

    // MULTI-SELECT CALLER WILL RELEASE
    if( m_pResultItem != NULL && !m_dwIsMultiSelect)
        m_pResultItem->Release();

    m_pQServerItem = NULL;
    m_pResultItem  = NULL;
    m_ulItems      = NULL;
    m_ulCount      = NULL;
    m_pUnk         = NULL;
    return(S_OK);
}


/*----------------------------------------------------------------------------

    Written by: Jim Hill
 ----------------------------------------------------------------------------*/
HRESULT CQServerGetAllAttributes::DestroyAllData()
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState());

    // FREE THE MEMORY
    if( IsDataAvailable() )
    {
		m_sa.Destroy();
        m_dwIsDataAvailable = 0;
    }

    return(S_OK);
}

/*----------------------------------------------------------------------------
    CQServerGetAllAttributes::CreateAllMissingAttributes()

    Make certain required attributes are included in the sample.
    If missing, create then with the default values.

    Written by: Jim Hill                                 
 ----------------------------------------------------------------------------*/
HRESULT CQServerGetAllAttributes::CreateAllMissingAttributes()
{
    HRESULT hr      = S_OK;
    DWORD   dwAdded = 0;
    VARIANT v;
    VariantInit( &v );
    VariantClear( &v );

    return(S_OK); // DISABLE 6/16/99

#if 0
    // UNMARSHALL THE INTERFACE IF WE DON'T HAVE IT YET
    if( m_pQServerItem == NULL )
    {
        hr = GetServerInterface();
        if( FAILED( hr ))
        {    
        	return(hr);
        }
    }

    // AVIS_SAMPLE_ACTIONS_SIGNATURES_PRIORITY 
    VariantClear( &v );
    v.vt    = VT_UI4;
    v.ulVal = 0;
    hr = GetValueOrCreateAttribute( AVIS_SAMPLE_ACTIONS_SIGNATURES_PRIORITY, &v, &dwAdded );

    // AVIS_SAMPLE_ACTIONS_SUBMISSION_PRIORITY 
    VariantClear( &v );
    v.vt    = VT_UI4;
    v.ulVal = 0;
    hr = GetValueOrCreateAttribute( AVIS_SAMPLE_ACTIONS_SUBMISSION_PRIORITY, &v, &dwAdded );

    // AVIS_SAMPLE_ACTIONS_DEF_SEQ_NEEDED 
    VariantClear( &v );
    v.vt    = VT_UI4;
    v.ulVal = 0;
    hr = GetValueOrCreateAttribute( AVIS_SAMPLE_ACTIONS_DEF_SEQ_NEEDED, &v, &dwAdded );

    
    // AVIS_QSERVER_ITEM_INFO_SUBMIT_QDATE 
    // VariantClear( &v );
    // v.vt    = VT_DATE;
    // v.date = 0;
    // hr = GetValueOrCreateAttribute( AVIS_QSERVER_ITEM_INFO_SUBMIT_QDATE, &v, &dwAdded );

    // AVIS_SAMPLE_ACTIONS_DATE_ANALYSIS_FINISHED 
    // VariantClear( &v );
    // v.vt    = VT_DATE;
    // v.ulVal = 0;
    // hr = GetValueOrCreateAttribute( AVIS_SAMPLE_ACTIONS_DATE_ANALYSIS_FINISHED, &v, &dwAdded );

    // AVIS_SAMPLE_ACTIONS_SAMPLE_FINISHED  
    // VariantClear( &v );
    // v.vt    = VT_DATE;
    // v.ulVal = 0;
    // hr = GetValueOrCreateAttribute( AVIS_SAMPLE_ACTIONS_SAMPLE_FINISHED, &v, &dwAdded );

    // DID WE ADD ANY?
    if( dwAdded )
    {
        // CALL m_pQServerItem->Commit and increment X-Sample-Changes
        hr = CommitAndIncrementSampleChangeCounter();
    }  
    return(S_OK);
#endif
}

/*----------------------------------------------------------------------------
    CQServerGetAllAttributes::CreateMissingAttributes()

    Create missing attributes before reading all the values, so they'll be in 
    the list. pValue has the default setting.

    Written by: Jim Hill                                 
 ----------------------------------------------------------------------------*/
HRESULT CQServerGetAllAttributes::GetValueOrCreateAttribute( LPCTSTR lpszFieldName, VARIANT* pValue, DWORD* lpdwAdded )
{
    HRESULT      hr = S_OK;
    DWORD   dwAdded = 0;
    VARIANT v;
    VariantInit( &v );
	USES_CONVERSION;
//    VariantClear( &v );// tam 5-25-00 bstr change
	BSTR bstrS = NULL;	// tam 5-25-00 bstr change
//    CString s = lpszFieldName; // tam 5-25-00 bstr change
	bstrS = SysAllocString (T2COLE(lpszFieldName));// tam 5-25-00 bstr change
    if( lpdwAdded != NULL )
        dwAdded = *lpdwAdded;

    // TRY TO READ FIRST DIRECTLY FROM THE SERVER
    hr = m_pQServerItem->GetValue( bstrS, &v );
    // S_FALSE INDICATES THE ATTRIBUTE IS MISSING
    if( hr == S_FALSE || hr ==2 )                              // 
    {
        hr = VariantCopyInd( pValue, &v );  // JUST RETURN THE VALUE
        // 6/16/99 NO LONGER CREATE MISSING ATTRIBUTES
        // hr = m_pQServerItem->SetValue( s.AllocSysString(), *pValue );
        //if( IsSuccessfullResult( hr ) )
        //{
        //    ++dwAdded;
        //    if( lpdwAdded != NULL )
        //        *lpdwAdded = dwAdded;
        //}
    }
    else if( IsSuccessfullResult( hr ) )
        hr = VariantCopyInd( pValue, &v );  // RETURN THE VALUE
	VariantClear (&v); // tam 5-25-00 bstr change

    return(hr);
}


/*----------------------------------------------------------------------------
    CQServerGetAllAttributes::CommitAndIncrementSampleChangeCounter

    Written by: Jim Hill                                 
 ----------------------------------------------------------------------------*/
HRESULT CQServerGetAllAttributes::CommitAndIncrementSampleChangeCounter()
{
    HRESULT hr = S_OK;

    if( m_pQServerItem == NULL )
    	return(E_UNEXPECTED);

    // INCREMENT THE CHANGE COUNTER
    IncrementSampleChangeCounter();

    // COMMIT CHANGES TO DISK
    hr = m_pQServerItem->Commit();

    return(hr);
}

/*----------------------------------------------------------------------------
    CQServerGetAllAttributes::IncrementSampleChangeCounter

    Get the change counter for this sample.
    Increment by one.
    Write out the new value.

    Written by: Jim Hill                                 
 ----------------------------------------------------------------------------*/
HRESULT CQServerGetAllAttributes::IncrementSampleChangeCounter()
{
    return(S_OK);

// DISABLE 6/1/99 QSERVER IS NOW INCREMENTING THIS VALUE WHEN COMMIT() IS CALLED.
#if 0
    HRESULT hr = S_OK;
    DWORD   dwValue = 0;
    CString s;
    VARIANT v;
    VariantInit( &v );
    VariantClear( &v );

    try
    {
        // GET CURRENT COUNTER FOR THIS VALUE
        hr = GetValueByAttributeName( AVIS_QSERVER_ITEM_INFO_CHANGES, &v );
        //if( FAILED(hr) )
        //    return(hr);
        if( IsSuccessfullSampleResult( hr ) )
        {
            if( !ConvertVariantToDword( &dwValue, &v) )
                return(E_FAIL);
        }

        // INCREMENT
        ++dwValue;

        // WRITE IT BACK  
        if( !InitializeVariantWithData( &dwValue, NULL, &v) )
            return(E_FAIL);

        // WRITE IT BACK TO THE SAMPLE DATA
        s  = AVIS_QSERVER_ITEM_INFO_CHANGES;
        hr = m_pQServerItem->SetValue( s.AllocSysString(), v )

        fWidePrintString("IncrementSampleChangeCounter() Sample change counter incremented. hr= 0x%X", hr );

    }
    catch(...)
    {
        return(hr);
    }

    return(hr);
#endif
}

/*----------------------------------------------------------------------------
    CQServerGetAllAttributes::SetValueByAttributeName

    Written by: Jim Hill                                 
 ----------------------------------------------------------------------------*/
HRESULT CQServerGetAllAttributes::SetValueByAttributeName( LPCTSTR lpszFieldName, VARIANT* pValue )
{
    HRESULT hr = S_OK;
	USES_CONVERSION;
    if(lpszFieldName == NULL || pValue == NULL || !IsDataAvailable())
    	return(E_UNEXPECTED);

    //fWidePrintString("CQServerGetAllAttributes::SetValueByAttributeName called. Caller's ObjectID= %d", m_iExtDataObjectID );

    VARIANT v;
    VariantInit( &v );
//    VariantClear( &v );

    // UPDATE IT REMOTELY
	BSTR bstrS;
    hr = E_FAIL;

    try
    {
        // UNMARSHALL THE INTERFACE IF WE DON'T HAVE IT YET
        if( m_pQServerItem == NULL )
        {
            hr = GetServerInterface();
            if( FAILED( hr ))
            {    
            	return(hr);
            }
        }
        if( m_pQServerItem != NULL )
        {
			bstrS = SysAllocString(T2COLE(lpszFieldName));
            hr = m_pQServerItem->SetValue( bstrS, *pValue );// tam 5-25-00 bstr change
            if( SUCCEEDED( hr ) )
            {
                // CALL m_pQServerItem->Commit and increment X-Sample-Changes
                hr = CommitAndIncrementSampleChangeCounter();
            }  
        
        }
        
        // UPDATE THE LOCAL COPY
        if( !IsDataAvailable() )
           	return(E_UNEXPECTED);
        for( DWORD i = 0; i < m_dwNumFields; i++)
        {
             v = m_sa( i, 0 );
             if( v.vt == VT_BSTR )
             {
                 if( STRCMP( lpszFieldName, OLE2T(v.bstrVal)) == 0 )
                 {
                     v = m_sa( i, 1 );
                     m_sa( i, 1 ) = *pValue;
                     break;
                 }
             }
        }
        // else // MUST BE MISSSING FROM THE ATTRIBUTE LIST
    }
    catch(...)
    {
        return(hr);
    }
	SysFreeString(bstrS);
    return(hr);
}



/*----------------------------------------------------------------------------
    CQServerGetAllAttributes::GetServerInterface()

    Called on Prop Page thread

    Written by: Jim Hill
 ----------------------------------------------------------------------------*/
HRESULT CQServerGetAllAttributes::GetServerInterface()
{
    HRESULT hr = S_OK;

    // DO WE ALREADY HAVE IT?
    if( m_pQServerItem != NULL )
        return(S_OK);

    if( m_pDataObject == NULL  )  
    	return(E_UNEXPECTED);

    if( IsErrorGettingQserverInterface() )
        return( m_iQserverInterfaceError );

    fWidePrintString("TIMING START CQServerGetAllAttributes::GetServerInterface called. Caller's ObjectID= %d", m_iExtDataObjectID );

    try
    {
        // UNMARSHAL SERVER INTERFACE.
        hr = CoGetInterfaceAndReleaseStream( m_pServerStream, __uuidof( IQuarantineServerItem ), (LPVOID*)&m_pQServerItem );
        m_pServerStream = NULL;  // jhill 7/26/99 
		// 
		// Need to set proxy blanket
		//
		if( FAILED( CoSetProxyBlanket( m_pQServerItem ,
			RPC_C_AUTHN_WINNT,
			RPC_C_AUTHZ_NONE,
			NULL,
			RPC_C_AUTHN_LEVEL_CONNECT,
			RPC_C_IMP_LEVEL_IMPERSONATE,
			(_COAUTHIDENTITY*) m_pIdentity,
			EOAC_NONE) ) )
		{
			hr = E_FAIL;
		}
    }
    catch(...) 
    {
        fWidePrintString("FAILURE: CQServerGetAllAttributes  CoGetInterfaceAndReleaseStream(). ObjectID= %d", m_iExtDataObjectID );
        return(E_FAIL);
    }

    fWidePrintString("TIMING STOP CQServerGetAllAttributes::GetServerInterface called. Caller's ObjectID= %d", m_iExtDataObjectID );
    return(hr); // DISABLE


#if 0
    try
    {
        // GET THE INTERFACE
        hr = m_pDataObject->QueryInterface( __uuidof( IQCResultItem ), (LPVOID*) &m_pResultItem );  //m_pDataObject
        if( FAILED( hr ))
        {    
            fWidePrintString("FAILURE: CQServerGetAllAttributes::GetServerInterface  QueryInterface(). ObjectID= %d  Error= 0x%08X", m_iExtDataObjectID, hr );
        	return(hr); // E_NOINTERFACE
        }
        
        // HOW MANY ARE THERE
        m_pResultItem->GetItemIDs( &m_ulCount, &m_ulItems );
        
        // GET QSERVER ITEM
        hr = m_pResultItem->GetQserverItem( m_ulItems[0], &m_pUnk );
        if(m_pUnk == NULL) {         // hr == RPC_E_WRONG_THREAD
            fWidePrintString("FAILURE: CQServerGetAllAttributes::GetServerInterface. GetQserverItem(). ObjectID= %d  Error= 0x%08X", m_iExtDataObjectID, hr );
            //ReleaseSeverInterface();
            return(hr);       // E_FAIL
        }
        m_pQServerItem = (IQuarantineServerItem*)m_pUnk;

    }
    catch(...) 
    {
        fWidePrintString("FAILURE: CQServerGetAllAttributes::GetServerInterface called. GetQserverItem(). ObjectID= %d", m_iExtDataObjectID );
        return(E_FAIL);
    }
    return(S_OK);
#endif
}


/*----------------------------------------------------------------------------

    Written by: Jim Hill
 ----------------------------------------------------------------------------*/
HRESULT CQServerGetAllAttributes::GetAllAttributes()
{
    HRESULT hr = S_OK;

    if(m_pDataObject == NULL)
    	return(E_UNEXPECTED);

    if( IsErrorGettingQserverInterface() )
        return( m_iQserverInterfaceError );

    try
    {
        // UNMARSHALL THE INTERFACE IF WE DON'T HAVE IT YET
        if( m_pQServerItem == NULL )
        {
            // GET THE INTERFACE
            hr = GetServerInterface();
            if( FAILED( hr ))
            {   
            	return(hr);
            }
        }

        fWidePrintString("TIMING START CQServerGetAllAttributes::GetAllAttributes called. Caller's ObjectID= %d", m_iExtDataObjectID );

        // CREATE ANY REQUIRED ATTRIBUTES THAT ARE CURRENTLY MISSING
        CreateAllMissingAttributes();

        // GET ALL THE ATTRIBUTES
        VariantInit( &m_va );
        VariantClear( &m_va );
        hr = m_pQServerItem->GetAllValues(&m_va);
        if( FAILED( hr ) || m_va.parray == NULL )
        {    
            // 6/15 add msg box. Here's where "access denied" errors happen.
            // There's already a msg box in SubmitOrDeliver.cpp

            fWidePrintString("TIMING GetAllAttributes::GetAllValues Failed. Caller's ObjectID= %d  hr=0x%X", m_iExtDataObjectID, hr );
            if( hr == S_OK )
                hr = E_FAIL;
        	return( hr );
        }
        
        // MAKE A LOCAL COPY    7/10 jhill
        VARIANT  m_vTempa;
        VariantInit( &m_vTempa );
        VariantClear( &m_vTempa );
        VariantCopyInd( &m_vTempa, &m_va );
		VariantClear(&m_va);
        m_sa.Attach( m_vTempa.parray );    

        // MAKE SURE THAT THE ARRAY HAS THE CORRECT SHAPE.  
        // m_sa.Attach( m_va.parray );    
        if( 2 != m_sa.GetDim() )
        {    
            throw (DWORD) E_INVALIDARG; 
        }
        m_dwIsDataAvailable = TRUE;
        
        m_dwNumFields = m_sa.GetLength(1); 
    }
    catch( DWORD dwError )
    {
        DestroyAllData();
        if( dwError != S_OK)
            hr = dwError;
    }
    catch(...) 
    {
        DestroyAllData();
        if( hr == S_OK)
            hr = E_FAIL;
    } 

    fWidePrintString("TIMING STOP CQServerGetAllAttributes::GetAllAttributes called. Caller's ObjectID= %d  hr=0x%X", m_iExtDataObjectID, hr );
    return(hr);
}

 
/*----------------------------------------------------------------------------
    CQServerGetAllAttributes::GetValueByAttributeName
    Written by: Jim Hill                                 IsSafeArrayEqual
 ----------------------------------------------------------------------------*/
HRESULT CQServerGetAllAttributes::GetValueByAttributeName( LPCTSTR lpszFieldName, VARIANT * pValue )
{
    HRESULT hr = S_OK;
	USES_CONVERSION;
    try
    {
        if(lpszFieldName == NULL || pValue == NULL || !IsDataAvailable())
        	return(E_UNEXPECTED);
        VariantClear( pValue );
        
        VARIANT v, v2;
        VariantInit( &v );
        VariantClear( &v );
        VariantInit( &v2 );
        VariantClear( &v2 );
        for( DWORD i = 0; i < m_dwNumFields; i++)
        {
             v = m_sa( i, 0 );
             if( v.vt == VT_BSTR )
             {
                 if( STRCMP( lpszFieldName, OLE2T(v.bstrVal)) == 0)
                 {
                     //VariantClear( &v );  // 7/11 This causes the X- header, in v, to be cleared.
                     v2 = m_sa( i, 1 );
                     hr = VariantCopyInd( pValue, &v2);
                     return(hr);
                 }
             }
        }
    } 
    catch(...) 
    {
        return(NTE_NOT_FOUND);   
    } 


    return(NTE_NOT_FOUND);   // ERROR_NOT_FOUND
}

/*----------------------------------------------------------------------------
    CQServerGetAllAttributes::GetValueByPartialAttributeName
    Allows for finding a series of entries where the first x chars
    of the field name are used to do the match.

    "X-Sample-" will find all of the sample entries
    "X-Platform-" will find all of the platform entries

    It returns each as it is found and sets dwIndex to the value of i.
    When we're at the end of the list, it returns E_FAIL.

    Written by: Jim Hill                                 IsSafeArrayEqual
 ----------------------------------------------------------------------------*/
HRESULT CQServerGetAllAttributes::GetValueByPartialAttributeName( VARIANT * pFullFieldName, LPCTSTR lpszPartialName, 
                                                                  VARIANT * pValue, DWORD* lpdwIndex)
{
    HRESULT hr = NTE_NOT_FOUND;  // ERROR_NOT_FOUND
	USES_CONVERSION;

    if(lpszPartialName == NULL || pValue == NULL || lpdwIndex == NULL || pFullFieldName == NULL || !IsDataAvailable())
    	return(E_UNEXPECTED);
    VariantClear( pFullFieldName );
    VariantClear( pValue );

    VARIANT v, v2;
    VariantInit( &v );
    VariantClear( &v );
    VariantInit( &v2 );
    VariantClear( &v2 );

    DWORD dwStrLength = STRLEN(lpszPartialName);
    DWORD i = 0;
    if( lpdwIndex != NULL)
        i = *lpdwIndex;

    for( ; i < m_dwNumFields; i++)
    {
         v = m_sa( i, 0 );
         if( v.vt == VT_BSTR )
         {
             // DID WE FIND ONE?  NULL MEANS RETURN THEM ALL
             if( lpszPartialName == NULL || STRNCMP( lpszPartialName, OLE2T(v.bstrVal), dwStrLength) == 0)
             {
                 // RETURN THE ATTRIBUTE NAME     
                 hr = VariantCopyInd( pFullFieldName, &v);
                 if( SUCCEEDED( hr ) )
                 {    
                     // RETURN THE VALUE
                     v2 = m_sa( i, 1 );
                     hr = VariantCopyInd( pValue, &v2);
                 }
                 break;
             }
         }
    }

    // RETURN THE INDEX
    if( lpdwIndex != NULL)
        *lpdwIndex = i;

    return(hr);
}


/*----------------------------------------------------------------------------
    RestoreIsAttributeDeleteable
    Should the attribute be deleted as part of the sample restore?
    Written by: Jim Hill 
 ----------------------------------------------------------------------------*/
BOOL CQServerGetAllAttributes::RestoreIsAttributeDeleteable( CString& s )
{
    s.TrimLeft();
    if( !s.IsEmpty() )
    {
        // DOES IT START WITH "X-" ?
        if( s.Find( _T("X-") ) != 0 )
            return( FALSE );

        // IS IT A BACKUP ATTRIBUTE "X-Backup"
        if( s.Find( AVIS_WILDCARD_X_BACKUP ) == 0 )
            return( FALSE );    

        // IS IT IN THE EXCLUSION LIST?
        return( m_sAttributeDeleteExcludedList.Find( s ) == -1 );

    }
    return( FALSE );
}


#ifdef MY_SYSTEM
    //#define INCLUDE_RESUBMIT_ANY  1
#endif

/*----------------------------------------------------------------------------
    RestoreIsEligibleForReSubmission()
    Written by: Jim Hill 
 ----------------------------------------------------------------------------*/
BOOL CQServerGetAllAttributes::RestoreIsEligibleForReSubmission()
{
    HRESULT hr       = S_OK;
    VARIANT v, v2;
    VariantInit( &v );
    VariantInit( &v2 );
    DWORD  dwIndex = 0;

//#ifndef INCLUDE_RESUBMIT_CODE   
//    return(FALSE);  // DISABLE
//#endif

#ifdef INCLUDE_RESUBMIT_ANY    
    // test Force to resubmit any sample
    return( TRUE );
#endif


    try
    {
        // DO WE HAVE THE INTERFACE AND DATA FOR THIS SAMPLE?
        if( !IsDataAvailable() )
            return(FALSE);

        // GET SAMPLE STATUS       
        hr = GetValueByAttributeName( AVIS_X_SAMPLE_STATUS, &v );   
        if( IsSuccessfullResult( hr ) && v.vt == VT_UI4)      
        {
            if( v.ulVal == AVIS_SAMPLE_STATUS_ERROR || v.ulVal == AVIS_SAMPLE_STATUS_ATTENTION )
            {
                VariantClear( &v );
                // IS THERE AT LEAST ONE BACKUP ATTRIBUTE?
                hr = GetValueByPartialAttributeName( &v, AVIS_WILDCARD_X_BACKUP, &v2, &dwIndex );
                if( IsSuccessfullResult( hr ) && v.vt != VT_EMPTY)      
				{
					VariantClear(&v);
                    return( TRUE );
				}
            }
        }
    }
    catch(...)
    {
    }
    return(FALSE);
}


/*----------------------------------------------------------------------------
    Restore_DeleteAttribute

    Written by: Jim Hill 
 ----------------------------------------------------------------------------*/
BOOL CQServerGetAllAttributes::Restore_DeleteAttribute(VARIANT * pFieldName, VARIANT * pValue )
{
    HRESULT hr = 0;
//    CString sFieldName;
	CString s;
    VARIANT vValue;

    if( pFieldName->vt != VT_BSTR )
        return(FALSE);

//    sFieldName = pFieldName->bstrVal;

    VariantInit( &vValue );
    hr = VariantCopyInd( &vValue, pValue);
    if( SUCCEEDED( hr ) )
    {          
        switch( vValue.vt )
        {
            case VT_UI4 :  // Number 
                if( vValue.ulVal == 0xFFFFFFF )   // SKIP IF ALREADY DELETED
                    return( FALSE );
                vValue.ulVal = 0xFFFFFFF;
                break;
            case VT_DATE : // Date  
                if( vValue.date == 0.0 )          // SKIP IF ALREADY DELETED
                    return( FALSE );
                vValue.date = 0.0;
                break;
            case VT_BSTR : // String 
                s = _T(" ");
                if( vValue.bstrVal == s )         // SKIP IF ALREADY DELETED
                    return( FALSE );
                vValue.bstrVal = s.AllocSysString();
                break;
            default :
                return( FALSE );
        }

        // WRITE IT OUT
        hr = m_pQServerItem->SetValue( pFieldName->bstrVal, vValue );// tam 5-25-00 bstr change
        if( SUCCEEDED( hr ) )
        {
            // RETURN THE NEW VALUE
            hr = VariantCopyInd( pValue, &vValue);
            return( TRUE );
        }
		VariantClear(&vValue);
    }
    return(FALSE);
}



/*----------------------------------------------------------------------------
    RestoreIsAttributeAlreadyDeleted
    Was this attribute already deleted?

    Written by: Jim Hill 
 ----------------------------------------------------------------------------*/
BOOL CQServerGetAllAttributes::RestoreIsAttributeAlreadyDeleted( VARIANT * pValue )
{
    return( IsAttributeDeleted( pValue ) );

    // CString s;
    // switch (pValue->vt)
    // {
    //     case VT_UI4 :
    //         if( pValue->ulVal == 0xFFFFFFFF )
    //             return( TRUE ); 
    //         break;
    //     case VT_DATE :
    //         if( pValue->date == 0.0 )
    //             return ( TRUE ); 
    //         break;
    //     case VT_BSTR :
    //         s = pValue->bstrVal;
    //         if( !s.IsEmpty() && s == _T(" ") )
    //             return ( TRUE ); 
    //         break;
    //     default :
    //         /* DO NOTHING */
    //         break;
    // }
    // // Mysterious attribute type, so presumably not deleted.
    // return( FALSE );
}


/*----------------------------------------------------------------------------
    RestoreAllSampleAttributes

    Written by: Jim Hill 
 ----------------------------------------------------------------------------*/
BOOL CQServerGetAllAttributes::RestoreAllSampleAttributes()
{
    HRESULT hr = NTE_NOT_FOUND;  // ERROR_NOT_FOUND
    VARIANT v, v2, vValue;
    VariantInit( &v );
    VariantInit( &v2 );
    VariantInit( &vValue );
    CString s, sSearchName;

    try
    {
        // HAVE WE ALREADY RESUBMITTED?
        if( !m_bOkToReSubmit )
            return(TRUE);

        // ARE WE ELIGIBLE TO RESUBMIT?
        if( !RestoreIsEligibleForReSubmission() )
            return(FALSE);

        // DELETE ALL RELEVANT ATTRIBUTES
        for( int i = 0; i < m_dwNumFields; i++)
        {
            v = m_sa( i, 0 );
            if( v.vt == VT_BSTR )
            {
                sSearchName  = v.bstrVal;
                sSearchName += _T(",");
                if( RestoreIsAttributeDeleteable( sSearchName ) )
                {
                    // PSEUDO DELETE THIS ATTRIBUTE
                    v2 = m_sa( i, 1 );
                    hr = VariantCopyInd( &vValue, &v2);
                    if( Restore_DeleteAttribute( &v, &vValue ) )
                    {
                         // // UPDATE THE LOCAL COPY
                         // m_sa( i, 1 ) = vValue;
                    }
                }
            }
            VariantClear( &vValue );
        }
                
        // COMMIT CHANGES TO DISK
        // hr = m_pQServerItem->Commit();

        // DO THE RESTORE 
        for( i = 0; i < m_dwNumFields; i++)
        {
            v = m_sa( i, 0 );
            if( v.vt == VT_BSTR )
            {
                s = v.bstrVal;
                s.TrimLeft();

                // SKIP IF NOT A BACKUP ATTRIBUTE "X-Backup"
                if( s.Find( AVIS_WILDCARD_X_BACKUP ) != 0 )
                    continue;

                // GET THE VALUE FROM THE BACKUP ATTRIBUTE
                v2 = m_sa( i, 1 );
                
                // WAS THIS ATTRIBUTE ALREADY DELETED? THEN DON'T USE IT FOR RESTORE.
                if( RestoreIsAttributeAlreadyDeleted( &v2 ) )
                    continue;

                // REMOVE "Backup-" FROM THE FIELD NAME
                s.Delete( 1, STRLEN( _T("Backup-") ) ); 

                sSearchName  = s;
                sSearchName += _T(",");
                
                // IS IT IN THE EXCLUSION LIST?
                if( m_sAttributeRestoreExcludedList.Find( sSearchName ) == -1 )
                {
                    // GET THE VALUE FROM THE BACKUP ATTRIBUTE
                    hr = VariantCopyInd( &vValue, &v2);
                    if( ! SUCCEEDED( hr ) )
                        continue;
    
                    // WRITE IT OUT
					BSTR bstrS = s.AllocSysString(); // tam 5-25 bstr update
                    hr = m_pQServerItem->SetValue( bstrS, vValue );// tam 5-25-00 bstr change
                    //hr = SetValueByAttributeName( s.AllocSysString(), &vValue ); // CALLS COMMIT
					SysFreeString(bstrS);
                }
            }
            VariantClear( &vValue );
        }
 
        Sleep(100);

        // SPECIAL CASE
        VariantClear( &vValue );
        hr = GetValueByAttributeName( AVIS_X_SAMPLE_PRIORITY, &vValue );
        if( IsSuccessfullResult( hr ) && v.vt == VT_UI4)      
        {
            if( vValue.ulVal == 0 )              // SINCE THIS IS A RESUBMIT, MAKE SURE THIS IS NOT 0
            {
                vValue.ulVal = ENABLE_AUTO_SUBMISSION_VALUE;
                hr = SetValueByAttributeName( AVIS_X_SAMPLE_PRIORITY, &vValue ); // CALLS COMMIT
                if( IsSuccessfullResult( hr ) )
				{
					VariantClear(&v);
                    return(TRUE);
				}
            }
        }

        // COMMIT CHANGES TO DISK
        hr = m_pQServerItem->Commit();
        Sleep(100);
        m_bOkToReSubmit = FALSE;

		VariantClear(&v);
    }
    catch(...)
    {
        fWidePrintString( "ERROR: CQServerGetAllAttributes::RestoreAllSampleAttributes Exception" );
        return(FALSE);
    }
    return(TRUE);
}






/*----------------------------------------------------------------------------
    From QConsole
    Looks up the text for X-Sample-Status 
 ----------------------------------------------------------------------------*/
void CQServerGetAllAttributes::GetStatusText(CString& sStatusText, DWORD dwStatus )
{
    int id;
    
    switch( dwStatus )
        {
        case AVIS_SAMPLE_STATUS_QUARANTINED:
            id = IDS_STATUS_QUARANTINED;
            break;

        case AVIS_SAMPLE_STATUS_SUBMITTED:
            id = IDS_STATUS_SUBMITTED;
            break;

        case AVIS_SAMPLE_STATUS_HELD:
            id = IDS_STATUS_HELD;
            break;

        case AVIS_SAMPLE_STATUS_RELEASED:
            id = IDS_STATUS_RELEASED;
            break;

        case AVIS_SAMPLE_STATUS_UNNEEDED:
            id = IDS_STATUS_UNNEEDED;
            break;

        case AVIS_SAMPLE_STATUS_NEEDED:
            id = IDS_STATUS_NEEDED;
            break;

        case AVIS_SAMPLE_STATUS_AVAILABLE:
            id = IDS_STATUS_AVAILABLE;
            break;

        case AVIS_SAMPLE_STATUS_DISTRIBUTE:    // 12/31/99 jhill added  
            id = IDS_STATUS_DISTRIBUTE;        // "Distribute"
            break;

        case AVIS_SAMPLE_STATUS_DISTRIBUTED:
            id = IDS_STATUS_DISTRIBUTED;       // "Distributed"
            break;

        case AVIS_SAMPLE_STATUS_INSTALLED:     // 7/1/99 jhill added
            id = IDS_STATUS_INSTALLED;         // "Installed"       
            break;

        case AVIS_SAMPLE_STATUS_ATTENTION:     // 7/11/99 jhill added
            id = IDS_STATUS_ATTENTION;         // "Attention"        
            break;

        case AVIS_SAMPLE_STATUS_ERROR:         // 7/11/99 jhill added
            id = IDS_STATUS_ERROR;             // "Error"            
            break;

        case AVIS_SAMPLE_STATUS_NOTINSTALLED:  // 12/31/99 jhill added  
            id = IDS_STATUS_NOTINSTALLED;      // "Not installed"
            break;

        case AVIS_SAMPLE_STATUS_RESTART:       // 12/31/99 jhill added  
            id = IDS_STATUS_RESTART;           // "Restart"
            break;

        case AVIS_SAMPLE_STATUS_LEGACY:        // 12/31/99 jhill added  
            id = IDS_STATUS_LEGACY;            // "Legacy"
            break;


        case AVIS_SAMPLE_STATUS_RISK:			// 8-4-03 Tmarles
            id = IDS_STATUS_RISK;				// "Risk"       
            break;
        case AVIS_SAMPLE_STATUS_HACKTOOL:		// 8-4-03 Tmarles
            id = IDS_STATUS_HACKTOOL;		
            break;
        case AVIS_SAMPLE_STATUS_SPYWARE:			// 8-4-03 Tmarles
            id = IDS_STATUS_SPYWARE;			
            break;
        case AVIS_SAMPLE_STATUS_TRACKWARE:			// 8-4-03 Tmarles
            id = IDS_STATUS_TRACKWARE;			
            break;
        case AVIS_SAMPLE_STATUS_DIALER:			// 8-4-03 Tmarles
            id = IDS_STATUS_DIALER;			
            break;
        case AVIS_SAMPLE_STATUS_REMOTE:			// 8-4-03 Tmarles
            id = IDS_STATUS_REMOTE;			
            break;
        case AVIS_SAMPLE_STATUS_ADWARE:			// 8-4-03 Tmarles
            id = IDS_STATUS_ADWARE;			
            break;
        case AVIS_SAMPLE_STATUS_PRANK:			// 8-4-03 Tmarles
            id = IDS_STATUS_PRANK;			
            break;


        default:
            id = IDS_STATUS_UNKNOWN;
        }

    // 12/11/99
    // sStatusText.LoadString( id );
    // In Utilplus.cpp
    LookupResourceString(sStatusText, id );

}



/*----------------------------------------------------------------------------
    CQServerGetAllAttributes::IsValidSampleSubmission()

    Is this sample valid for auto submission to analysis center?

    Written by: Jim Hill
 ----------------------------------------------------------------------------*/
BOOL CQServerGetAllAttributes::IsValidSampleSubmission(CString& sError, CString& sCaption)
{
    HRESULT  hr      = 0;
    DWORD    dwValue = 0;
    BOOL     bSucceeded = TRUE;
    CString  s, sDate;
    VARIANT v;
    VariantInit( &v );
    VariantClear( &v );
    sError.Empty();
    sCaption.Empty();
 

    // X-Sample-Priority needs to be 0
    hr = GetValueByAttributeName( AVIS_SAMPLE_ACTIONS_SUBMISSION_PRIORITY, &v );  // 
    // Attrib missing is same as 0. But should not happen with this one
    if( hr != S_OK && hr != NTE_NOT_FOUND )  //ERROR_NOT_FOUND
    {
        bSucceeded = FALSE;         // return(FALSE);
        s.Format( IDS_UNABLE_TO_READ_ATTRIBUTE_FMT, AVIS_SAMPLE_ACTIONS_SUBMISSION_PRIORITY );
        sError += s;
        sCaption.Format( IDS_UNABLE_TO_READ_ATTRIBUTE_CAPTION );
    }
    if( hr == S_OK && ConvertVariantToDword( &dwValue, &v ) && bSucceeded )
    {
        if( dwValue != 0 )
        {
            bSucceeded = FALSE;      
            sCaption.Format( IDS_MANUAL_SUBMIT_ALREADY_QUEUED );  // 9/2/99
        }
        s.Format( _T("%s == %d\r\n"), AVIS_SAMPLE_ACTIONS_SUBMISSION_PRIORITY, dwValue );
        sError += s;
    }
    else if( hr == NTE_NOT_FOUND )
    {
        s.Format( _T("%s == 0\r\n"), AVIS_SAMPLE_ACTIONS_SUBMISSION_PRIORITY );
        sError += s;
    }

    //----------------------------------------------------------------------------------------
    // X-Date-Submitted needs to be 0  
    VariantClear( &v );
    hr = GetValueByAttributeName( AVIS_X_DATE_SUBMITTED, &v );              //AVIS_QSERVER_ITEM_INFO_SUBMIT_QDATE
    // Attrib missing is same as 0
    if( hr != S_OK && hr != NTE_NOT_FOUND )    // ERROR_NOT_FOUND
    {
         bSucceeded = FALSE;          // return(FALSE);
         s.Format( IDS_UNABLE_TO_READ_ATTRIBUTE_FMT, AVIS_X_DATE_SUBMITTED );
         sError += s;
         sCaption.Format( IDS_UNABLE_TO_READ_ATTRIBUTE_CAPTION );
    }
    if( hr == S_OK && !IsVariantDateValid( &v ) )
    {
         bSucceeded = FALSE;          // return(FALSE);
         s.Format( IDS_ATTRIBUTE_INVALID_DATE, AVIS_X_DATE_SUBMITTED );
         sError += s;
         if( sCaption.IsEmpty() )
             sCaption.Format( IDS_MANUAL_SUBMIT_ALREADY_SUBMITTED );  // 9/2/99
    }
    if( hr == NTE_NOT_FOUND )
    {
         s.Format(_T("%s == 0\r\n"), AVIS_X_DATE_SUBMITTED);
         sError += s;
    }
    else if( hr == S_OK && ConvertVariantToString( sDate, &v, AVIS_ATTRIBUTE_FORMAT_USE_DEFAULT ) )
    {
         s.Format(_T("%s == %s\r\n"), AVIS_X_DATE_SUBMITTED, sDate);
         sError += s;
    }


    //----------------------------------------------------------------------------------------
//  Removed 8/21/00 NOT NEEDED
//     // X-Date-Analysis-Finished needs to be 0
//     VariantClear( &v );
//     hr = GetValueByAttributeName( AVIS_SAMPLE_ACTIONS_DATE_ANALYSIS_FINISHED, &v );  //AVIS_X_DATE_ANALYSIS_FINISHED
//     // Attrib missing is same as 0
//     if( hr != S_OK && hr != NTE_NOT_FOUND )     //  ERROR_NOT_FOUND
//     {
//          bSucceeded = FALSE;          // return(FALSE);
//          s.Format( IDS_UNABLE_TO_READ_ATTRIBUTE_FMT, AVIS_SAMPLE_ACTIONS_DATE_ANALYSIS_FINISHED );
//          sError += s;
//          sCaption.Format( IDS_UNABLE_TO_READ_ATTRIBUTE_CAPTION );
//     }
//     if( hr == S_OK && !IsVariantDateValid( &v ) )
//     {
//          bSucceeded = FALSE;          // return(FALSE);
//          s.Format( IDS_ATTRIBUTE_INVALID_DATE, AVIS_SAMPLE_ACTIONS_DATE_ANALYSIS_FINISHED );
//          sError += s;
//          if( sCaption.IsEmpty() )
//              sCaption.Format( IDS_MANUAL_SUBMIT_ALREADY_FINISHED );  // 9/2/99
//     }
//     if( hr == NTE_NOT_FOUND )
//     {
//          s.Format(_T("%s == 0\r\n"), AVIS_SAMPLE_ACTIONS_DATE_ANALYSIS_FINISHED);
//          sError += s;
//     }
//     else if( hr == S_OK && ConvertVariantToString( sDate, &v, AVIS_ATTRIBUTE_FORMAT_USE_DEFAULT ) )
//     {
//          s.Format(_T("%s == %s\r\n"), AVIS_SAMPLE_ACTIONS_DATE_ANALYSIS_FINISHED, sDate);
//          sError += s;
//     }
	VariantClear(&v);

    return(bSucceeded);
}

/*----------------------------------------------------------------------------
    CQServerGetAllAttributes::IsValidDefDelivery()

    Is this sample valid for auto delivery of defs?

    Written by: Jim Hill
 ----------------------------------------------------------------------------*/
BOOL CQServerGetAllAttributes::IsValidDefDelivery(CString& sError, CString& sCaption)
{
    HRESULT  hr = 0;
    DWORD    dwValue = 0;
    BOOL     bSucceeded = TRUE;
    CString  s, sDate;
    VARIANT v;
    VariantInit( &v );
    VariantClear( &v );
    sError.Empty();
    sCaption.Empty();

    // X-Signatures-Priority needs to be 0, to be eligible for automatic delivery
    hr = GetValueByAttributeName( AVIS_SAMPLE_ACTIONS_SIGNATURES_PRIORITY, &v );
    // Attrib missing is same as 0
    if( hr != S_OK && hr != NTE_NOT_FOUND )  // ERROR_NOT_FOUND
    {
         bSucceeded = FALSE;         // return(FALSE);
         s.Format( IDS_UNABLE_TO_READ_ATTRIBUTE_FMT, AVIS_SAMPLE_ACTIONS_SIGNATURES_PRIORITY );
         sError += s;
    }
    if( hr == S_OK )   
    {
         if( ConvertVariantToDword( &dwValue, &v ) && dwValue != 0 )
         {
             bSucceeded = FALSE;      // return(FALSE);
             sCaption.Format( IDS_MANUAL_DELIVER_ALREADY_QUEUED );  // 9/2/99 
         }
         s.Format(_T("%s == %d\r\n"), AVIS_SAMPLE_ACTIONS_SIGNATURES_PRIORITY, dwValue);
         sError += s;
    }
    else if( hr == NTE_NOT_FOUND )
    {
        s.Format( _T("%s == 0\r\n"), AVIS_SAMPLE_ACTIONS_SIGNATURES_PRIORITY );
        sError += s;
    }


    //----------------------------------------------------------------------------------------
    // X-Signatures-Sequence > 0, to require new defs
    VariantClear( &v );
    v.vt    = VT_UI4;
    v.ulVal = 0;        // DEFAULT DATA
    // CREATE IT IF MISSING
    hr = GetValueOrCreateAttribute( AVIS_SAMPLE_ACTIONS_DEF_SEQ_NEEDED, &v ); // 6/15 don't create
    //hr = GetValueByAttributeName( AVIS_SAMPLE_ACTIONS_DEF_SEQ_NEEDED, &v );
    // It must be > 0
    // Attrib missing is same as 0
    if( IsSuccessfullResult( hr )  )  // || hr == NTE_NOT_FOUND
    {
         if( ConvertVariantToDword( &dwValue, &v ) )
         {
             if( dwValue == 0 )
             {
                 bSucceeded = FALSE;      // return(FALSE);
                 s.Format(_T("%s == %d\r\n"), AVIS_SAMPLE_ACTIONS_DEF_SEQ_NEEDED, dwValue);
                 sError += s;
                 sCaption.Format( IDS_MANUAL_DELIVER_NO_SIG_SEQ );  // 9/2/99 
             }
         }
         else
         {
             bSucceeded = FALSE;      // return(FALSE);
             s.Format(_T("%s == %d\r\n"), AVIS_SAMPLE_ACTIONS_DEF_SEQ_NEEDED, dwValue);
             sError += s;
             sCaption.Format( IDS_MANUAL_DELIVER_NO_SIG_SEQ );  // 9/2/99 
         }
    }

    //----------------------------------------------------------------------------------------
//  Removed 8/21/00 NOT NEEDED
//    // X-Date-Sample-Finished needs to be 0 - not yet disinfected 
//    VariantClear( &v );
//    hr = GetValueByAttributeName( AVIS_SAMPLE_ACTIONS_SAMPLE_FINISHED, &v );
//    // Attrib missing is same as 0
//    if( hr != S_OK && hr != NTE_NOT_FOUND )  // ERROR_NOT_FOUND
//    {
//         bSucceeded = FALSE;         // return(FALSE);
//         s.Format( IDS_UNABLE_TO_READ_ATTRIBUTE_FMT, AVIS_SAMPLE_ACTIONS_SAMPLE_FINISHED );
//         sError += s;
//    }
//    if( hr == S_OK && !IsVariantDateValid( &v ) )
//    {
//         bSucceeded = FALSE;          // return(FALSE);
//         s.Format( IDS_ATTRIBUTE_INVALID_DATE, AVIS_SAMPLE_ACTIONS_SAMPLE_FINISHED );
//         sError += s;
//         sCaption.Format( IDS_MANUAL_DELIVER_ALREADY_FINISHED );  // 9/2/99 
//    }
//    if( hr == NTE_NOT_FOUND )
//    {
//         s.Format(_T("%s == 0\r\n"), AVIS_SAMPLE_ACTIONS_SAMPLE_FINISHED);
//         sError += s;
//    }
//    else if( hr == S_OK && ConvertVariantToString( sDate, &v, AVIS_ATTRIBUTE_FORMAT_USE_DEFAULT ) )
//    {
//         s.Format(_T("%s == %s\r\n"), AVIS_SAMPLE_ACTIONS_SAMPLE_FINISHED, sDate);
//         sError += s;
//    }

	VariantClear(&v);
    return(bSucceeded);  
}


/*----------------------------------------------------------------------------
    CQServerGetAllAttributes::GetFilenameAndIcon()
    Return the Icon and the Filename, minus the path.
    Written by: Jim Hill
 ----------------------------------------------------------------------------*/
HRESULT CQServerGetAllAttributes::GetFilenameAndIcon( LPTSTR lpszFilename, HICON* phIcon )
{
    HRESULT  hr = S_OK;
    CString  s;
    TCHAR szFullPath[MAX_PATH*2];
	USES_CONVERSION;

    // GET THE FILENAME
    VARIANT v;
    VariantInit( &v );
    VariantClear( &v );
    memset( szFullPath, 0, sizeof(szFullPath) );
    s = AVIS_X_SAMPLE_FILE;    
    hr = GetValueByAttributeName( (LPCTSTR) s, &v );
    if( SUCCEEDED( hr ) && v.vt == VT_BSTR )
    {
        vpstrncpy( szFullPath, OLE2T(v.bstrVal),sizeof(szFullPath) );

        // STRIP OFF FILE PATH.
        if( lpszFilename != NULL )
        {
            CFileNameParse::GetFileName( szFullPath, lpszFilename, MAX_PATH );
            //CFileNameParse::GetFileName( v.bstrVal, lpszFilename, MAX_PATH );

            //fWidePrintString( "Filename: %s  %s", lpszFilename, szFullPath );
        }
    }
    else
        return( hr );

    // LOOK UP ICON
    SHFILEINFO sfi;
    DWORD dwRet = 0;
    memset( &sfi, 0, sizeof(SHFILEINFO) );
    dwRet = SHGetFileInfo( szFullPath, 
                   FILE_ATTRIBUTE_NORMAL,
                   &sfi, 
                   sizeof( SHFILEINFO ), 
                   SHGFI_ICON | SHGFI_LARGEICON | SHGFI_USEFILEATTRIBUTES 
                   );

    if( phIcon != NULL )
        *phIcon = sfi.hIcon;

	VariantClear(&v);
    return hr;
}

/*----------------------------------------------------------------------------
    HRESULT CQServerGetAllAttributes::MarkSampleForSubmission()

    Mark the sample for auto submission to the analysis center

    Written by: Jim Hill
 ----------------------------------------------------------------------------*/
HRESULT CQServerGetAllAttributes::MarkSampleForSubmission(CString& sError, CString& sCaption, DWORD dwSubmissionPriority )
{
    HRESULT  hr = S_OK;
    CString s;
    VARIANT v;
    VariantInit( &v );
    VariantClear( &v );

    // IF 0, THEN CHECK FOR VALID AND USE THE DEFAULT VALUE
    if( dwSubmissionPriority == 0 )
    {
        if( !IsValidSampleSubmission( sError, sCaption ) )
            return(E_FAIL);
        dwSubmissionPriority = ENABLE_AUTO_SUBMISSION_VALUE;
    }

    // CHANGE ATTRIBUTES TO CAUSE AVIS TO AUTOMATICALLY SUBMIT THESE FILES TO THE ANALYSIS CENTER
    s = AVIS_SAMPLE_ACTIONS_SUBMISSION_PRIORITY;
    v.vt    = VT_UI4;
    v.ulVal = dwSubmissionPriority;
    hr = SetValueByAttributeName( (LPCTSTR) s, &v );

    return(hr);
}

/*----------------------------------------------------------------------------
    CQServerGetAllAttributes::MarkSampleForAutoDefDelivery()

    Mark the sample for auto delivery of the def set

    Written by: Jim Hill
 ----------------------------------------------------------------------------*/
HRESULT CQServerGetAllAttributes::MarkSampleForAutoDefDelivery(CString& sError, CString& sCaption)
{
    HRESULT  hr = S_OK;
    CString s;
    VARIANT v;
    VariantInit( &v );
    VariantClear( &v );

    if( !IsValidDefDelivery( sError, sCaption ) )
        return(E_FAIL);

    // CHANGE ATTRIBUTES TO CAUSE AVIS TO AUTOMATICALLY DISTRIBUTE NEW DEFS
    s = AVIS_SAMPLE_ACTIONS_SIGNATURES_PRIORITY;
    v.vt    = VT_UI4;
    v.ulVal = ENABLE_AUTO_DELIVERY_VALUE;
    hr = SetValueByAttributeName( (LPCTSTR) s, &v );

    return(hr);
}




/*----------------------------------------------------------------------------

    Written by: Jim Hill
 ----------------------------------------------------------------------------*/
HRESULT CQServerGetAllAttributes::DumpAllAttributes()
{
    HRESULT hr = S_OK;
    VARIANT v;
    VariantInit( &v );
    VariantClear( &v );
	USES_CONVERSION;

    if(!IsDataAvailable())
    	return(E_UNEXPECTED);

    hr = GetValueByAttributeName( AVIS_X_SAMPLE_FILE, &v );
    if( SUCCEEDED( hr ) && v.vt == VT_BSTR )
        fWidePrintString("CQServerGetAllAttributes::GetValueByAttributeName  Value= %s   ExtObjectID= %d ", v.bstrVal, m_iExtDataObjectID);

    TCHAR   szBuff[259],szTemp[259];
    VARIANT item, value;
    VariantInit( &item );
    VariantClear( &item );
    VariantInit( &value );
    VariantClear( &value );

    for(DWORD i = 0; i < m_dwNumFields; i++)
    {
        szBuff[0] = szTemp[0] = 0;
        item  = m_sa( i, 0 );
        if( item.vt == VT_BSTR )
        {
            wsprintf(szBuff, _T("%s    "), item.bstrVal);
        }
        value = m_sa( i, 1 );
        if( value.vt == VT_BSTR )
            vpstrnappend(szBuff, OLE2T(value.bstrVal), sizeof (szBuff));
        else if( value.vt == VT_UI4 )
        {
            wsprintf( szTemp, _T("0x%08x"), value.ulVal );
            vpstrnappend(szBuff, szTemp, sizeof (szBuff));
        }
        else if( value.vt == VT_DATE ) 
        {
            TCHAR szDate[32];
            TCHAR szTime[32];
            SYSTEMTIME st;                                  
            szDate[0] = szTime[0] = 0;

            VariantTimeToSystemTime( value.date, &st );
            GetDateFormat( LOCALE_SYSTEM_DEFAULT, DATE_SHORTDATE, &st, NULL, szDate, 32 ); // NULL
            GetTimeFormat( LOCALE_SYSTEM_DEFAULT, 0, &st, NULL, szTime, 32 );              // NULL
            wsprintf( szTemp, _T("%s - %s"), szDate, szTime );
            vpstrnappend(szBuff, szTemp, sizeof(szBuff));
        }

        fWidePrintString("Attribute: %s", szBuff);
    }



	VariantClear(&v);
    return(S_OK);
}








#if 0
typedef struct tag_xattributes_struct{
    LPCTSTR  lpszXHeader;
    int      dwID;
    DWORD    dwXlatedDisplayResourceID;
    DWORD    dwExclude;
} X_ATTRIBUTES_STRUCT;

// USE THIS TABLE TO TURN OFF DISPLAY OF A KNOWN ATTRIBUTE
static X_ATTRIBUTES_STRUCT XAttributeTable[] =  
{     // X-Header                                  myID                                            DisplayResourceID in RC file                     Exclude
    { AVIS_X_PLATFORM_USER           , AVIS_ID_X_PLATFORM_USER             	 , IDS_DISPLAY_ITEMINFO_USERNAME                  , 0 },	 
    { AVIS_X_PLATFORM_COMPUTER       , AVIS_ID_X_PLATFORM_COMPUTER         	 , IDS_DISPLAY_ITEMINFO_MACHINENAME               , 0 },	
    { AVIS_X_PLATFORM_DOMAIN         , AVIS_ID_X_PLATFORM_DOMAIN           	 , IDS_DISPLAY_ITEMINFO_DOMAINNAME                , 0 },	
    { AVIS_X_PLATFORM_ADDRESS        , AVIS_ID_X_PLATFORM_ADDRESS          	 , IDS_DISPLAY_ITEMINFO_MACHINE_ADDRESS           , 0 },	
    { AVIS_X_PLATFORM_PROCESSOR      , AVIS_ID_X_PLATFORM_PROCESSOR        	 , IDS_DISPLAY_ITEMINFO_PROCESSOR                 , 0 },	
    { AVIS_X_PLATFORM_DISTRIBUTOR    , AVIS_ID_X_PLATFORM_DISTRIBUTOR      	 , IDS_DISPLAY_ITEMINFO_DISTRIBUTOR               , 0 },	
    { AVIS_X_PLATFORM_HOST           , AVIS_ID_X_PLATFORM_HOST             	 , IDS_DISPLAY_ITEMINFO_HOST                      , 0 },	
    { AVIS_X_PLATFORM_SYSTEM         , AVIS_ID_X_PLATFORM_SYSTEM           	 , IDS_DISPLAY_ITEMINFO_SYSTEM                    , 0 },	
    { AVIS_X_PLATFORM_LANGUAGE       , AVIS_ID_X_PLATFORM_LANGUAGE         	 , IDS_DISPLAY_ITEMINFO_LANGUAGE                  , 0 },	
    { AVIS_X_PLATFORM_OWNER          , AVIS_ID_X_PLATFORM_OWNER            	 , IDS_DISPLAY_ITEMINFO_OWNER                     , 0 },	
    { AVIS_X_PLATFORM_SCANNER        , AVIS_ID_X_PLATFORM_SCANNER          	 , IDS_DISPLAY_ITEMINFO_SCANNER                   , 0 },	
    { AVIS_X_PLATFORM_CORRELATOR     , AVIS_ID_X_PLATFORM_CORRELATOR       	 , IDS_DISPLAY_ITEMINFO_CORRELATOR                , 0 },	
    { AVIS_X_DATE_CREATED 	         , AVIS_ID_X_DATE_CREATED 	             , IDS_DISPLAY_ITEMINFO_FILE_CREATED_TIME         , 0 },	
    { AVIS_X_DATE_MODIFIED	         , AVIS_ID_X_DATE_MODIFIED	             , IDS_DISPLAY_ITEMINFO_FILE_MODIFIED_TIME        , 0 },	
    { AVIS_QSERVER_ITEM_INFO_FILE_ACCESSED_TIME	 , AVISID_QSERVER_ITEM_INFO_FILE_ACCESSED_TIME	 , IDS_DISPLAY_ITEMINFO_FILE_ACCESSED_TIME        , 0 },	
    { AVIS_QSERVER_ITEM_INFO_QUARANTINE_QDATE  	 , AVISID_QSERVER_ITEM_INFO_QUARANTINE_QDATE  	 , IDS_DISPLAY_ITEMINFO_QUARANTINE_QDATE          , 0 },	
    { AVIS_QSERVER_ITEM_INFO_SUBMIT_QDATE      	 , AVISID_QSERVER_ITEM_INFO_SUBMIT_QDATE      	 , IDS_DISPLAY_ITEMINFO_SUBMIT_QDATE              , 0 },	
    { AVIS_QSERVER_ITEM_INFO_VIRUSID             , AVISID_QSERVER_ITEM_INFO_VIRUSID              , IDS_DISPLAY_ITEMINFO_VIRUSID                   , 0 },
    { AVIS_QSERVER_ITEM_INFO_DEF_SEQUENCE      	 , AVISID_QSERVER_ITEM_INFO_DEF_SEQUENCE      	 , IDS_DISPLAY_ITEMINFO_DEF_SEQUENCE              , 0 },	
    { AVIS_QSERVER_ITEM_INFO_DEF_VERSION       	 , AVISID_QSERVER_ITEM_INFO_DEF_VERSION       	 , IDS_DISPLAY_ITEMINFO_DEF_VERSION               , 0 },	
    { AVIS_QSERVER_ITEM_INFO_VIRUSNAME         	 , AVISID_QSERVER_ITEM_INFO_VIRUSNAME         	 , IDS_DISPLAY_ITEMINFO_VIRUSNAME                 , 0 },	
    { AVIS_QSERVER_ITEM_INFO_SCAN_RESULT       	 , AVISID_QSERVER_ITEM_INFO_SCAN_RESULT       	 , IDS_DISPLAY_ITEMINFO_SCAN_RESULT               , 0 },	
    { AVIS_QSERVER_ITEM_INFO_CHECKSUM          	 , AVISID_QSERVER_ITEM_INFO_CHECKSUM          	 , IDS_DISPLAY_ITEMINFO_CHECKSUM                  , 0 },	
    { AVIS_QSERVER_ITEM_INFO_INFO_FILE_EXT     	 , AVISID_QSERVER_ITEM_INFO_INFO_FILE_EXT     	 , IDS_DISPLAY_ITEMINFO_INFO_FILE_EXT             , 0 },	
    { AVIS_QSERVER_ITEM_INFO_FILENAME          	 , AVISID_QSERVER_ITEM_INFO_FILENAME          	 , IDS_DISPLAY_ITEMINFO_FILENAME                  , 0 },	
    { AVIS_QSERVER_ITEM_INFO_FILESIZE          	 , AVISID_QSERVER_ITEM_INFO_FILESIZE          	 , IDS_DISPLAY_ITEMINFO_FILESIZE                  , 0 },	
    { AVIS_QSERVER_ITEM_INFO_SAMPLE_TYPE       	 , AVISID_QSERVER_ITEM_INFO_SAMPLE_TYPE       	 , IDS_DISPLAY_ITEMINFO_SAMPLE_TYPE               , 0 },	
    { AVIS_QSERVER_ITEM_INFO_SAMPLE_REASON     	 , AVISID_QSERVER_ITEM_INFO_SAMPLE_REASON     	 , IDS_DISPLAY_ITEMINFO_SAMPLE_REASON             , 0 },	
    { AVIS_QSERVER_ITEM_INFO_DISK_GEOMETRY     	 , AVISID_QSERVER_ITEM_INFO_DISK_GEOMETRY     	 , IDS_DISPLAY_ITEMINFO_DISK_GEOMETRY             , 0 },	
    { AVIS_QSERVER_ITEM_INFO_STATUS            	 , AVISID_QSERVER_ITEM_INFO_STATUS            	 , IDS_DISPLAY_ITEMINFO_STATUS                    , 0 },	
    { AVIS_QSERVER_ITEM_INFO_SUBMISSION_ROUTE  	 , AVISID_QSERVER_ITEM_INFO_SUBMISSION_ROUTE  	 , IDS_DISPLAY_ITEMINFO_SUBMISSION_ROUTE          , 0 },	
//  { AVIS_QSERVER_ITEM_INFO_GUID              	 , AVISID_QSERVER_ITEM_INFO_GUID              	 , IDS_DISPLAY_ITEMINFO_GUID                      , 0 },	
    { AVIS_QSERVER_ITEM_CHECKSUM_METHOD        	 , AVISID_QSERVER_ITEM_CHECKSUM_METHOD        	 , IDS_DISPLAY_ITEMCHECKSUM_METHOD                , 0 },	
    { AVIS_QSERVER_ITEM_INFO_UUID  	             , AVISID_QSERVER_ITEM_INFO_UUID  	             , IDS_DISPLAY_ITEMINFO_UUID  	                  , 1 },	
    { AVIS_QSERVER_ITEM_INFO_FILEID	             , AVISID_QSERVER_ITEM_INFO_FILEID	             , IDS_DISPLAY_ITEMINFO_FILEID	                  , 1 },
    { AVIS_QSERVER_ITEM_INFO_CHANGES             , AVISID_QSERVER_ITEM_INFO_CHANGES              , IDS_DISPLAY_ITEMINFO_CHANGES                   , 0 },

    { AVIS_SAMPLE_ACTIONS_SUBMISSION_PRIORITY    , AVISID_SAMPLE_ACTIONS_SUBMISSION_PRIORITY     , IDS_DISPLAY_SAMPLE_ACTIONS_SUBMISSION_PRIORITY , 0 },	
    { AVIS_SAMPLE_ACTIONS_AUTO_DELIVERY_DEFS     , AVISID_SAMPLE_ACTIONS_AUTO_DELIVERY_DEFS      , IDS_DISPLAY_SAMPLE_ACTIONS_AUTO_DELIVERY_DEFS  , 0 },	
    { AVIS_SAMPLE_ACTIONS_ANALYSIS_STATE         , AVISID_SAMPLE_ACTIONS_ANALYSIS_STATE          , IDS_DISPLAY_SAMPLE_ACTIONS_ANALYSIS_STATE      , 0 },	
    { AVIS_SAMPLE_ACTIONS_GATEWAY_COOKIE         , AVISID_SAMPLE_ACTIONS_GATEWAY_COOKIE          , IDS_DISPLAY_SAMPLE_ACTIONS_GATEWAY_COOKIE      , 0 },	
    { AVIS_SAMPLE_ACTIONS_ANALYSIS_ISSUE         , AVISID_SAMPLE_ACTIONS_ANALYSIS_ISSUE          , IDS_DISPLAY_SAMPLE_ACTIONS_ANALYSIS_ISSUE      , 0 },	
    { AVIS_SAMPLE_ACTIONS_VIRUS_NAME             , AVISID_SAMPLE_ACTIONS_VIRUS_NAME              , IDS_DISPLAY_SAMPLE_ACTIONS_VIRUS_NAME          , 0 },	
    { AVIS_SAMPLE_ACTIONS_DEF_SEQ_NEEDED         , AVISID_SAMPLE_ACTIONS_DEF_SEQ_NEEDED          , IDS_DISPLAY_SAMPLE_ACTIONS_DEF_SEQ_NEEDED      , 0 },	

    { NULL,	-1, 0, 0 }
};
#endif


/*----------------------------------------------------------------------------
    GetAttributeTextFromLocalID()

    Written by: Jim Hill
 ----------------------------------------------------------------------------*/
//BOOL CQServerGetAllAttributes::GetAttributeTextFromLocalID(CString& sText, DWORD dwID)  
//{
//    sText.Empty();
//    for( int i = 0 ; ; i++ )
//    {
//        if( XAttributeTable[i].dwID == -1 || XAttributeTable[i].lpszXHeader == NULL )
//             break;
//
//        if( XAttributeTable[i].dwID == dwID)
//        {    
//            sText = XAttributeTable[i].lpszXHeader;
//            return(sText.IsEmpty());
//        }
//    }
//    return(FALSE);
//}

/*----------------------------------------------------------------------------
    LoadDisplayStringFromLocalID()

    Written by: Jim Hill
 ----------------------------------------------------------------------------*/
//BOOL CQServerGetAllAttributes::LoadDisplayStringFromLocalID(CString& sText, DWORD dwID)  
//{
//    sText.Empty();
//    for( int i = 0 ; ; i++ )
//    {
//        if( XAttributeTable[i].dwID == -1 || XAttributeTable[i].lpszXHeader == NULL )
//             break;
//
//        if( XAttributeTable[i].dwID == dwID) 
//            return(sText.LoadString(XAttributeTable[i].dwXlatedDisplayResourceID));
//    }
//    return(FALSE);
//}


/*----------------------------------------------------------------------------
    LoadDisplayTextFromAttributeText()

    Written by: Jim Hill                                       LPCTSTR
 ----------------------------------------------------------------------------*/
//BOOL CQServerGetAllAttributes::LoadStringTextFromAttributeText(CString& sText, LPTSTR lpszXAttributeName)  
//{
//    sText.Empty();
//    for( int i = 0 ; ; i++ )
//    {
//        if( XAttributeTable[i].dwID == -1 || XAttributeTable[i].lpszXHeader == NULL )
//             break;
//
//        if( STRCMP( lpszXAttributeName, XAttributeTable[i].lpszXHeader) == 0)
//        {    
//            return(sText.LoadString(XAttributeTable[i].dwXlatedDisplayResourceID));
//        }
//    }
//    return(FALSE);
//}

/*----------------------------------------------------------------------------
    IsAttributeExcludedByAttributeText()

    Written by: Jim Hill
 ----------------------------------------------------------------------------*/
//BOOL CQServerGetAllAttributes::IsAttributeExcludedByAttributeText(LPTSTR lpszXAttributeName)  
//{
//    for( int i = 0 ; ; i++ )
//    {
//        if( XAttributeTable[i].dwID == -1 || XAttributeTable[i].lpszXHeader == NULL )
//             break;
//
//        if( STRCMP( lpszXAttributeName, XAttributeTable[i].lpszXHeader) == 0)
//            return(XAttributeTable[i].dwExclude);
//    }
//    return(FALSE);
//}


/*----------------------------------------------------------------------------
    IsAttributeExcludedByLocalID()

    Written by: Jim Hill
 ----------------------------------------------------------------------------*/
//BOOL CQServerGetAllAttributes::IsAttributeExcludedByLocalID(DWORD dwLocalID)  
//{
//    for( int i = 0 ; ; i++ )
//    {
//        if( XAttributeTable[i].dwID == -1 || XAttributeTable[i].lpszXHeader == NULL )
//             break;
//
//        if( XAttributeTable[i].dwID == dwLocalID)
//            return(XAttributeTable[i].dwExclude);
//    }
//    return(FALSE);
//}

/*----------------------------------------------------------------------------
    GetResIDFromLocalID()

    Written by: Jim Hill                 XAttributeTable[]
 ----------------------------------------------------------------------------*/
//DWORD CQServerGetAllAttributes::GetResIDFromLocalID(DWORD dwID)  
//{
//    for( int i = 0 ; ; i++ )
//    {
//        if( XAttributeTable[i].dwID == -1 || XAttributeTable[i].lpszXHeader == NULL )
//             break;
//
//        if( XAttributeTable[i].dwID == dwID)
//            return(XAttributeTable[i].dwXlatedDisplayResourceID);
//    }
//    return(0);
//}




