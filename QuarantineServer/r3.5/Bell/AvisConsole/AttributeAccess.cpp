/*
 * PROPRIETARY/CONFIDENTIAL.  use of this product is subject to license terms.
 * Copyright (c) 2005 Symantec Corporation.  All Rights Reserved.
 *
*/


/*
    Implementation file:  AttributeAccess.cpp


    GetAllAttributes needs constructor on same thread as access occurs.

*/


#ifndef  _WIN32_DCOM
   #define _WIN32_DCOM
#endif
#include "stdafx.h"
#include "AvisConsole.h"

#include "AttributeAccess.h"
#include "GetAllAttributes.h"

//#include "PlatformAttributesPage.h"
//#include "SampleAttributesPage.h"
//#include "SampleActionsPage.h"




CAttributeAccess::CAttributeAccess( IDataObject* pDataObject, DWORD iExtDataObjectID )
{
    m_dwConstructorThreadID    = GetCurrentThreadId();  
    m_dwAccessThreadID         = 0;
    m_iExtDataObjectID         = iExtDataObjectID; 
    m_pDataObject              = pDataObject; 
//  m_pPlatformAttributesPage  = NULL; 
    m_pSampleAttributesPage    = NULL; 
    m_pSampleActionsPage       = NULL; 
    m_pQServerGetAllAttributes = NULL; 
    m_pGetAllAttributes        = NULL; 
    m_dwRef                    = 0;
    m_dwRegistered             = 0;
    m_dwObjSignature           = MY_OBJECT_SIGNATURE;
    m_dwGetAllAttributesInitializedOK = 0;

    fWidePrintString("CAttributeAccess Constructor called. Caller's ObjectID= %d", m_iExtDataObjectID);

    GetQServerAttributeData();

    return;    
}


CAttributeAccess::~CAttributeAccess()
{
    m_dwObjSignature = 0;
}


/*----------------------------------------------------------------------------
    CAttributeAccess::Release()

    Written by: Jim Hill
 ----------------------------------------------------------------------------*/
DWORD CAttributeAccess::Release()
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState());

    try
    {
        // IS A PAGE CALLING RELEASE WHEN IT NEVER CALLED INITIALIZE?
        // IF THE SIGNATURE IS NOT ==, THEN THE CAttributeAccess OBJECT HAS ALREADY BEEN DESTROYED.
        if( m_dwObjSignature != MY_OBJECT_SIGNATURE )
        {
            fWidePrintString("CAttributeAccess::Release called after Object Destroyed. ObjectID= %d  m_dwRef= %d  Pages Registered= %d", m_iExtDataObjectID, m_dwRef, m_dwRegistered);
            return(0);
        }
    }
    catch(...) 
    {
        return(0);
    } 


    fWidePrintString("CAttributeAccess::Release called.ObjectID= %d  m_dwRef= %d  Pages Registered= %d", m_iExtDataObjectID, m_dwRef, m_dwRegistered);
    InterlockedDecrement(&m_dwRef);

    // IF THERE'S AN EXTRA CALL AFTER THIS IS DELETED, AND AT LEAST 1 PAGE CALLED INITIALIZE, BAILOUT.
    if( m_dwRef < 0 && m_dwRegistered != 0 )
        return(0);

    if( m_dwRef == 0 || m_dwRegistered == 0 )
    {
	    fWidePrintString("CAttributeAccess::Release called. Deleting ObjectID= %d  m_dwRef= %d  Pages Registered= %d", m_iExtDataObjectID, m_dwRef, m_dwRegistered);

        // LOCK ANYONE ELSE OUT
        InterlockedDecrement(&m_dwRef);

        // TELL THE SAMPLE ATTRIBUTE OBJECT WE'RE NOT USING IT ANYMORE
        if( m_pQServerGetAllAttributes != NULL )
        {
            ((CQServerGetAllAttributes*)m_pQServerGetAllAttributes)->Release();
            m_pQServerGetAllAttributes = NULL;
            m_pGetAllAttributes        = NULL;
        }

        delete this;
        return 0;
    }
    return m_dwRef;
}


/*----------------------------------------------------------------------------
    CAttributeAccess::Initialize()

    Initialize the GetAllAttributes object
    Only the first page to call from OnInitDialog causes the 
         attributes to be read.

    Written by: Jim Hill
 ----------------------------------------------------------------------------*/
void* CAttributeAccess::Initialize()
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState());
    CString   sText, sErrorString;

    ++m_dwRef;
    ++m_dwRegistered;
    m_dwAccessThreadID = GetCurrentThreadId();
    fWidePrintString("CAttributeAccess Initialize called. Caller's ObjectID= %d", m_iExtDataObjectID);

    // CALLED FROM OnInitDialog() FROM ONE OF THE PROP PAGES
    // THIS IS ONLY CALL TO CREATE CQServerGetAllAttributes
    // ONLY CALLED ON THE THREAD FROM THE 1ST PAGE TO CALL

    HRESULT hr = E_FAIL;
    if( m_pGetAllAttributes != NULL )
    {
        if( m_dwGetAllAttributesInitializedOK )
        {
            if( !IsDataAvailable() )
                return(NULL);
        }
        else
        {
            hr = m_pGetAllAttributes->Initialize();
            m_dwGetAllAttributesInitializedOK = IsSuccessfullResult( hr );
            if( !m_dwGetAllAttributesInitializedOK )
            {
                sText.LoadString( IDS_ERROR_READING_FROM_QSERVER );
                CStringFormatErrorCode( sErrorString, hr );
                sText += sErrorString;
                ShowErrorMessage( sText, IDS_TITLE_ERROR_READING_FROM_QSERVER, 0, hr, g_ServerDisplayName );
            }
        }

    }

    return(m_pGetAllAttributes);
}


/*----------------------------------------------------------------------------

    Written by: Jim Hill
 ----------------------------------------------------------------------------*/
HRESULT CAttributeAccess::IsErrorGettingQserverInterface() 
{
    HRESULT  hr = E_FAIL;

    // PASS ON TO THE ATTRIBUTE OBJECT
    if( m_pGetAllAttributes != NULL )
        hr = m_pGetAllAttributes->IsErrorGettingQserverInterface();

    return( hr );
}

/*----------------------------------------------------------------------------

    Written by: Jim Hill
 ----------------------------------------------------------------------------*/
BOOL CAttributeAccess::IsDataAvailable() 
{ 
    if( m_pGetAllAttributes == NULL)
        return(FALSE);
    return( m_pGetAllAttributes->IsDataAvailable() ); 
}

/*----------------------------------------------------------------------------

    Written by: Jim Hill
 ----------------------------------------------------------------------------*/
DWORD CAttributeAccess::GetNumFields()  
{ 
    if( m_pGetAllAttributes == NULL || !IsDataAvailable() )
        return(0);
    return( m_pGetAllAttributes->m_dwNumFields ); 
}

/*----------------------------------------------------------------------------

    Written by: Jim Hill
 ----------------------------------------------------------------------------*/
HRESULT CAttributeAccess::SetValueByAttributeName( LPCTSTR lpszFieldName, VARIANT * pValue )
{ 
    if( m_pGetAllAttributes == NULL || !IsDataAvailable() )
        return(E_FAIL);
    return( m_pGetAllAttributes->SetValueByAttributeName( lpszFieldName, pValue ) ); 
}

/*----------------------------------------------------------------------------

    Written by: Jim Hill
 ----------------------------------------------------------------------------*/
HRESULT CAttributeAccess::GetValueByAttributeName( LPCTSTR lpszFieldName, VARIANT * pValue )
{ 
    if( m_pGetAllAttributes == NULL || !IsDataAvailable() )
        return(E_FAIL);
    return( m_pGetAllAttributes->GetValueByAttributeName( lpszFieldName, pValue ) ); 
}

/*----------------------------------------------------------------------------

    Written by: Jim Hill
 ----------------------------------------------------------------------------*/
HRESULT CAttributeAccess::GetValueByPartialAttributeName( VARIANT * pFullFieldName, 
                                  LPCTSTR lpszPartialName, VARIANT * pValue, DWORD* lpdwIndex ) 
{ 
    if( m_pGetAllAttributes == NULL || !IsDataAvailable() )
        return(E_FAIL);
    return( m_pGetAllAttributes->GetValueByPartialAttributeName( pFullFieldName, lpszPartialName,
                                                                 pValue, lpdwIndex ) ); 
}






/*----------------------------------------------------------------------------
    CAttributeAccess::GetPropPage()

    Return a void* to the requested page.

    Written by: Jim Hill
 ----------------------------------------------------------------------------*/
void* CAttributeAccess::GetPropPage( DWORD dwID )
{
    void* ptr = NULL;

    switch( dwID )
    {
        //case GET_PLATFORMATTRIBUTES_PAGE:
        //    ptr = m_pPlatformAttributesPage;
        //    break;

        case GET_SAMPLEATTRIBUTES_PAGE:
            ptr = m_pSampleAttributesPage;
            break;

        case GET_SAMPLEACTIONS_PAGE:
            ptr = m_pSampleActionsPage;
            break;

        default:
            ptr = NULL;
            break;
    }
    return(ptr);
}


/*----------------------------------------------------------------------------
    CAttributeAccess::GetQServerAttributeData()

    Called from Constructor on the MMC Resultpane Thread.        
    This is only call to create CQServerGetAllAttributes.

    Data is not read untill Initialize() called from 
        OnInitDialog() in one of the prop pages.

    Written by: Jim Hill
 ----------------------------------------------------------------------------*/
HRESULT CAttributeAccess::GetQServerAttributeData()
{
    HRESULT  hr = S_OK;

    if(m_pDataObject == NULL)
    	return E_UNEXPECTED;

    CQServerGetAllAttributes* pGetAllAttributes = 
             new CQServerGetAllAttributes( m_pDataObject, m_iExtDataObjectID );

    m_pQServerGetAllAttributes = pGetAllAttributes;
    m_pGetAllAttributes        = pGetAllAttributes;

    return(hr);
}















