/*
 * PROPRIETARY/CONFIDENTIAL.  use of this product is subject to license terms.
 * Copyright (c) 2005 Symantec Corporation.  All Rights Reserved.
 *
*/

/*
	SubmitOrDeliver.cpp

    Handle actions from Toolbar buttons or Context menu in the result pane.
*/

#include "stdafx.h"
#include <comdef.h>
#include "resource.h"
#include "AvisConsole.h"
#include "GetAllAttributes.h"
#include "ConfirmationDialog.h"
#include "SubmitOrDeliver.h"
#include "filenameparse.h"
#include "vpstrutils.h"


// Handle to main console window.
extern HWND g_hMainWnd;


//static IQCResultItem*  m_pIQCResultItem;


#define EXCLUDE_DISABLED_ITEMS   0
/*----------------------------------------------------------------------------

    Written by: Jim Hill
 ----------------------------------------------------------------------------*/
HRESULT CAvisResultItems::Initialize()
{
    IQuarantineServerItem* pQServerItem = NULL;;
    HRESULT  hr = S_OK;
    int      iTotalAdded = 0;
	SEC_WINNT_AUTH_IDENTITY *pIdentity= NULL;  

    if( !m_pDataObject )
        return(E_FAIL);

    // IS THIS A MULTI-SELECT OBJECT
    // IF SO BUILD A LIST OF THE OBJECTS PASSED FOR THE CONFIRMATION DIALOG.
    CString   sItemStr, sSubItemStr, s, sText, sError, sCaption;
    sText.Empty();
    CComQIPtr<IQCResultItem> pQCResultItem( m_pDataObject );


    try
    {
        // GET QUARANTINE CONSOLE INTERFACE FROM DATA OBJECT.   
        //CComQIPtr<IQCResultItem> pQCResultItem( m_pDataObject );
        if( pQCResultItem == NULL )
        {
            hr = E_UNEXPECTED;
            throw; 
        }
        m_pQCResultItem = pQCResultItem;
        
        // GET THE TOTAL NUMBER OF ITEMS
        //ULONG  ulCount = 1;
        hr = pQCResultItem->GetItemIDs( &m_ulCount, &m_aItemIDs );
        if( FAILED( hr ) )
        {
            fWidePrintString("FAILURE: CAvisResultItems::Initialize  GetItemIDs(). ObjectID= %d  Error= 0x%08X", m_iExtDataObjectID, hr );
            _com_issue_error( hr );    // E_NOINTERFACE
        }
        
        fWidePrintString("CAvisResultItems::Initialize  m_pDataObject=0x%X  m_pQCResultItem=0x%X  ObjectID= %d  m_ulCount= %d", 
                        ((DWORD)m_pDataObject), ((DWORD)m_pQCResultItem), m_iExtDataObjectID, m_ulCount );
        
        // BUILD OUR LIST FOR THE CONFIRMATION DIALOG
        LIST_DATA_STRUCT   *pList;
        //m_lpItemList = new LIST_DATA_STRUCT;
        m_lpItemList = (LIST_DATA_STRUCT *) MemAlloc( sizeof(LIST_DATA_STRUCT) * (m_ulCount + 1) );
        if( m_lpItemList == NULL ) 
        {
            hr = E_OUTOFMEMORY;
            throw; 
        }
        pList = m_lpItemList;

        BOOL  bIsValid = FALSE;
        //int   iSamplesDropped = 0;
        VARIANT v;
        VariantInit( &v );
        VariantClear( &v );
        //IQuarantineServerItem* pQServerItem;
        for( ULONG i = 0; i < m_ulCount; i++ )
        {
		    // modified to store the connection identity tam 7-19-00
            // GET QSERVER ITEM
            hr = pQCResultItem->GetQserverItem( m_aItemIDs[i], (LPUNKNOWN*) &pQServerItem,(ULONG *)&pIdentity);  //pUnk
            if( FAILED( hr ) || pQServerItem == NULL )    //  pUnk
            {
                 fWidePrintString("FAILURE: CAvisResultItems::Initialize  GetQserverItem(). ObjectID= %d  Error= 0x%08X", 
                                   m_iExtDataObjectID, hr );
                 _com_issue_error( hr );
            }
        
            CQServerGetAllAttributes* pGetAllAttributes = 
                 new CQServerGetAllAttributes( m_pDataObject, m_iExtDataObjectID, pQServerItem, pIdentity );  //pUnk
            if( pGetAllAttributes == NULL ) 
            {
                hr = E_OUTOFMEMORY;
                throw; 
            }

            // DO WE HAVE THE SERVER INTERFACE?
            hr = pGetAllAttributes->IsErrorGettingQserverInterface();
            if( hr != S_OK )
            {
                pGetAllAttributes->Release();  // FREE THE OBJECT
                _com_issue_error( hr );
            }

            // INITIALIZE THE ATTRIBUTE OBJECT FOR THIS ITEM
            // Here's where "access denied" errors come back. RPC_E_ACCESS_DENIED  0x8001011B
            // Also error when Qserver disconnects.           RPC_E_DISCONNECTED   0x80010108
            hr = pGetAllAttributes->Initialize();
            if( hr != S_OK )            // 7/11 test
            {
                pGetAllAttributes->Release();  // FREE THE OBJECT
                _com_issue_error( hr );
            }

        
            if( m_dwActionType == SUBMIT_ACTION_TYPE )
            {
                // IS THIS SAMPLE VALID FOR MANUAL SUBMISSION
                bIsValid = pGetAllAttributes->IsValidSampleSubmission( sError, sCaption );
                if( !bIsValid )
                {
                    pList->iIsDisabled                = TRUE;
                    pGetAllAttributes->m_dwIsDisabled = TRUE;   // 6/29/99
                    ++m_iItemsDisabled;            // SKIP THIS ONE
#if EXCLUDE_DISABLED_ITEMS
                    pGetAllAttributes->Release();  // FREE THE OBJECT
                    continue;
#endif              // 6/29/99 Old method of dropping them from the list
                }
            }
            else   // DELIVERY
            {
                // IS THIS SAMPLE VALID FOR MANUAL DELIVERY
                bIsValid = pGetAllAttributes->IsValidDefDelivery( sError, sCaption );
                if( !bIsValid )
                {
                    pList->iIsDisabled                = TRUE;
                    pGetAllAttributes->m_dwIsDisabled = TRUE;
                    ++m_iItemsDisabled;            // SKIP THIS ONE
#if EXCLUDE_DISABLED_ITEMS
                    pGetAllAttributes->Release();  // FREE THE OBJECT
                    continue;
#endif              // 6/29/99 Old method of dropping them from the list
                }
            }
        
            // ADD THE ITEM, SUBITEM, AND LPARAM VALUES TO OUR LIST   
            pList->iItem      = i;
            pList->iIsSelected= 0;
            s = AVIS_X_PLATFORM_USER;     // AVIS_QSERVER_ITEM_INFO_USERNAME
            hr = pGetAllAttributes->GetValueByAttributeName( (LPCTSTR) s, &v );
            if( SUCCEEDED( hr ) && v.vt == VT_BSTR )
            {
                vpstrncpy( pList->sItemStr, v.bstrVal, sizeof(pList->sItemStr) );
                VariantClear( &v );
            }
        
            s = AVIS_X_SAMPLE_FILE;    // AVIS_QSERVER_ITEM_INFO_FILENAME;
            hr = pGetAllAttributes->GetValueByAttributeName( (LPCTSTR) s, &v );
            if( SUCCEEDED( hr ) && v.vt == VT_BSTR )
            {
                TCHAR szFullPath[MAX_PATH+1];
                vpstrncpy( szFullPath, v.bstrVal,sizeof(szFullPath) );
        
                // STRIP OFF FILE PATH.
                CFileNameParse::GetFileName( szFullPath, pList->sSubItemStr, MAX_PATH );  //v.bstrVal
                VariantClear( &v );
            }
        
            // SAVE PTR TO THE ATTTRIBUTE DATA OBJECT                   
            pList->lParam     = (LPARAM) pGetAllAttributes;     
            pGetAllAttributes = NULL;
            ++pList;
            ++iTotalAdded;
        
            // Free the IQuarantineServerItem
            if( pQServerItem != NULL )
            {
                pQServerItem->Release();
                pQServerItem = NULL;
            }

        }
        
        // DID WE ADD LESS THAN WHAT THE USER SELECTED IN THE RESULT PANE?
        if( iTotalAdded == 0 || m_iItemsDisabled > 0)
        {
            // SELECT ERROR TEXT
            int iCaptionID, iMsgID;
            CString s;
            if( m_dwActionType == SUBMIT_ACTION_TYPE )
            {
                iCaptionID = IDS_SAMPLE_SUBMIT_ERROR_CAPTION;
                iMsgID     = IDS_SAMPLE_SUBMIT_ERROR;
                s.LoadString( IDS_MANUAL_SUBMIT_CRITERIA );
            }
            else
            {
                iCaptionID = IDS_SAMPLE_DELIVERY_ERROR_CAPTION;
                iMsgID     = IDS_SAMPLE_DELIVERY_ERROR;
                s.LoadString( IDS_MANUAL_DELIVERY_CRITERIA );
            }
        
            CString sErrorMsg;
            sErrorMsg.LoadString( iMsgID );
            sErrorMsg += _T("\r\n");
            // SINGLE SELECT?
            if( iTotalAdded < 2 && !sError.IsEmpty() )
            {
                // This will use the ret value in sCaption for the MessageBox title 9/2/99
                if( m_dwActionType == SUBMIT_ACTION_TYPE )
                {
                    iCaptionID = IDS_SAMPLE_SUBMIT_ERROR_CAPTION2;
                    sErrorMsg.LoadString( IDS_SAMPLE_SUBMIT_ERROR2 );
                }
                else
                {
                    iCaptionID = IDS_SAMPLE_DELIVERY_ERROR_CAPTION2;
                    sErrorMsg.LoadString( IDS_SAMPLE_DELIVERY_ERROR2 );
                }
                sErrorMsg += _T("\r\n");
                sErrorMsg += _T("\r\n");
                sErrorMsg += sError;
                sErrorMsg += s;
            }
            else     // MULTIPLE SELECT
            {
                sErrorMsg += s;
                sCaption.LoadString( iCaptionID );
            }

            if( g_hMainWnd != NULL )
                MessageBox( g_hMainWnd, sErrorMsg, sCaption, MB_ICONASTERISK | MB_OK );
            else
                ShowErrorMessage(sErrorMsg, iCaptionID, MB_ICONASTERISK | MB_OK, 0, g_ServerDisplayName);
        }

    }
    catch( _com_error e )
    {
        sText.LoadString( IDS_COM_ERROR_READING_FROM_QSERVER2 );
        hr = e.Error();                // RPC_S_SERVER_UNAVAILABLE
        sText += e.ErrorMessage();
    }
    catch(...) 
    {
        sText.LoadString( IDS_ERROR_READING_FROM_QSERVER2 );
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
    }

    // IS THERE AN ERROR STRING TO DISPLAY?
    if( !sText.IsEmpty() )
    {
        ShowErrorMessage( sText, IDS_TITLE_ERROR_READING_FROM_QSERVER, 0, hr, g_ServerDisplayName );
    }


    // Free the IQuarantineServerItem. IS THIS STILL AROUND, AT THIS POINT?
    if( pQServerItem != NULL )
    {
        pQServerItem->Release();
        pQServerItem = NULL;
    }


    // DID WE ADD SOME
    if( iTotalAdded > 0)
    {
        m_iTotalObjects = iTotalAdded;
        m_dwIsDataAvailable = TRUE;
        //hr = S_OK;  //jhill 7/9 test
    }
    else
    {   // NONE WERE ADDED
        if( m_lpItemList != NULL )
             MemFree(m_lpItemList);
        m_lpItemList = NULL;
        return(NTE_NOT_FOUND);   // ERROR_NOT_FOUND
    }

    // DO WE NEED TO FREE THE LIST OF OBJECTS? 
    if( hr != S_OK )
    {
        FreeAttributeObjectList();
    }
    return( hr ) ;
}  // Initialize()


HRESULT DoScreenRefresh(void)
{
    HRESULT  hr = S_OK;
    //CComPtr< IConsole > pc;

    //CComPtr< IConsole > pc = m_spConsole;
    //hr = pc->UpdateAllViews( pData, (long) pComponent, VIEW_HINT_REFRESH_END );



    //CComPtr< IDataObject > pData;
    //m_pRootNode->GetDataObject( &pData, CCT_RESULT );
    //m_pRootNode->GetComponentData()->m_spConsole->UpdateAllViews( pData, (long)m_pItemData, VIEW_HINT_REFRESH_ITEM );


    return( hr ) ;
}  


/*----------------------------------------------------------------------------
  CAvisResultItems::SubmitOrDeliverAllItems
  Written by: Jim Hill
----------------------------------------------------------------------------*/
HRESULT CAvisResultItems::SubmitOrDeliverAllItems()
{
    try
    {   
        DWORD nIDSubmitHeading   = 0;
        DWORD nIDDeliveryHeading = 0;

        HRESULT hr = Initialize();
        if( FAILED( hr ) )
        {
            // ERROR MESSAGE WAS DISPLAYED IN Initialize()
            return( hr );
        }
    
        if( m_lpItemList == NULL )
            return(E_FAIL);
    
        if( m_iItemsDisabled )                               // ARE SOME DISABLED?
        {
            if( m_iItemsDisabled == m_iTotalObjects )        // ARE THEY ALL DISABLED?
            {
                nIDSubmitHeading   = IDS_SAMPLECONFIRM_HEADING3;    // Grayed out items do not meet the submission criteria.
                nIDDeliveryHeading = IDS_DELIVERYCONFIRM_HEADING3;  // Grayed out items do not meet the delivery criteria.
            }
            else
            {
                nIDSubmitHeading   = IDS_SAMPLECONFIRM_HEADING2;    // Release selected samples for automatic analysis.\r\nGrayed out items do not meet the submission criteria.
                nIDDeliveryHeading = IDS_DELIVERYCONFIRM_HEADING2;  // Deliver selected definitions to these clients.\r\nGrayed out items do not meet the delivery criteria.
            }
        }
        else
        {
            nIDSubmitHeading   = IDS_SAMPLECONFIRM_HEADING;         // Release selected samples for automatic analysis.
            nIDDeliveryHeading = IDS_DELIVERYCONFIRM_HEADING;       // Deliver selected definitions to these clients.
        }


        CConfirmationDialog  dlg;
    
        if( m_dwActionType == SUBMIT_ACTION_TYPE )
        {
            dlg.Initialize(IDS_SAMPLECONFIRM_TITLE,nIDSubmitHeading,
                           IDS_SAMPLECONFIRM_COL1,IDS_SAMPLECONFIRM_COL2, 
                           IDH_SUBMIT_CONFIRM_LISTBOX, m_lpItemList, m_iTotalObjects,
                           g_ConfirmationDialogSubmitHelpIdArray );     
        }
        else   // DELIVER
        {
            dlg.Initialize(IDS_DELIVERYCONFIRM_TITLE,nIDDeliveryHeading,
                           IDS_DELIVERYCONFIRM_COL1,IDS_DELIVERYCONFIRM_COL2, 
                           IDH_DELIVER_CONFIRM_LISTBOX, m_lpItemList, m_iTotalObjects, 
                           g_ConfirmationDialogDeliveryHelpIdArray );     
        }
       
        int nResponse = dlg.DoModal();
    
		if(nResponse == IDOK && dlg.m_iTotalSelectedItems == 0 )
		{
            fWidePrintString("CAvisConExtData::SubmitOrDeliverAllItems  OK, but no items selected in list");
            // DO WE NEED TO FREE THE LIST OF OBJECTS? 
            FreeAttributeObjectList();
            if( m_aItemIDs != NULL )
                CoTaskMemFree( m_aItemIDs );
            m_aItemIDs = NULL;
                return S_OK;
        }
    
        // DO THE SELECTED LIST
        ProcessSelectedList( nResponse );

    
        fWidePrintString("CAvisResultItems::SubmitOrDeliverAllItems. CoTaskMemFree m_aItemIDs=0x%X  m_pQCResultItem=0x%X  ObjectID= %d", 
                          ((DWORD)m_aItemIDs), ((DWORD)m_pQCResultItem), m_iExtDataObjectID );
    
        if( m_aItemIDs != NULL )
            CoTaskMemFree( m_aItemIDs );
        m_aItemIDs = NULL;

        // FREE THE ITEM LIST      freed in FreeAttributeObjectList() 



        if( nResponse == IDOK )
        {
            CString sText;
            CString sCaption;
            if( m_dwActionType == SUBMIT_ACTION_TYPE )
            {                                                              
                if( m_iTotalSent > 1 )     
                    sText.Format( IDS_ITEMS_SUBMITTED_FMT, m_iTotalSent );    // _T("%d  samples were released for submission to the Analysis Center")
                else
                    sText.Format( IDS_ITEM_SUBMITTED_FMT2, m_iTotalSent );    // %d sample was released for submission to the Analysis Center
                sCaption.LoadString( IDS_ITEMS_SUBMITTED_TITLE );             // _T("Selected Samples Released");  
            }
            else
            {
                if( m_iTotalSent > 1 )     
                    sText.Format( IDS_ITEMS_DELIVERED_FMT, m_iTotalSent );    // _T("%d items were set for distribution to the user"
                else
                    sText.Format( IDS_ITEM_DELIVERED_FMT2, m_iTotalSent );    // %d item was set for distribution to the user
                sCaption.LoadString( IDS_ITEMS_DELIVERED_TITLE );             // _T("Selected items set for distribution");
            }
            if( g_hMainWnd != NULL )
                MessageBox( g_hMainWnd, sText, sCaption, MB_ICONASTERISK | MB_OK );
        }

    }
    catch(...)
    {
        fWidePrintString("CAvisConExtData::SubmitOrDeliverAllItems  WARNING: Exception caught.");
    }


    DoScreenRefresh();  // test 7/28/99


#if 0
        // TELL MMC TO REFRESH THE SCREEN. SEND SCANCODE FOR F5 KEY TO MAIN WINDOW
        BOOL CStringFormatErrorCode( CString& s, HRESULT hErrorCode );
        HWND g_hScopePaneWnd = NULL;
        DWORD dwLastError    = 0;
        CString s;
        if( g_hMainWnd )
        {
            SetLastError(0);
            g_hScopePaneWnd = FindWindowEx( g_hMainWnd, NULL, _T("SysTreeView32"), _T("") );  //NULL
            if( g_hScopePaneWnd == NULL)
            {
                dwLastError = GetLastError();
                CStringFormatErrorCode( s, dwLastError );
            }
            //SendMessage( g_hMainWnd, WM_KEYDOWN, 0x74, 0x003F0001 );
            //SendMessage( g_hMainWnd, WM_KEYUP,   0x74, 0xC03F0001 );

            dwLastError = InvalidateRect(  g_hMainWnd,  // handle of window with changed update region
                                           NULL,        // address of rectangle coordinates
                                           TRUE );      // erase-background flag

        }   

    //InvalidateRect(  g_hMainWnd,  // handle of window with changed update region
    //                 NULL,        // address of rectangle coordinates
    //                 TRUE );      // erase-background flag

#endif

    return S_OK;
}


/*----------------------------------------------------------------------------
  CAvisResultItems::ProcessSelectedList

  With multi-select, this will need to access more than one attribute list.
  Each item in pItemList contains the address of the coresponding attributes 
  list in lParam

  Written by: Jim Hill
----------------------------------------------------------------------------*/
HRESULT CAvisResultItems::ProcessSelectedList( int nResponse )
{
    LIST_DATA_STRUCT  *pItemList = NULL;
    int i;

    if( m_lpItemList == NULL )
        return S_OK;

    // PROCESS OUR LIST OF ATTRIBUTE OBJECTS
    pItemList = m_lpItemList;
    CQServerGetAllAttributes* pGetAllAttributes = NULL;
    for( i = 0 ; i < m_iTotalObjects; i++, pItemList++ )
	{
         pGetAllAttributes = (CQServerGetAllAttributes*) pItemList->lParam;
         if( pGetAllAttributes == NULL )
             continue;

         // MARK ITEM FOR SUBMISSION OR DELIVERY
         if( nResponse == IDOK && pGetAllAttributes != NULL && pItemList->iIsSelected )
         {
             if( m_dwActionType == SUBMIT_ACTION_TYPE )
                 SubmitItem( pItemList );
             else
                 DeliverItem( pItemList );
         }

	}


    // NOW FREE THE LIST OF OBJECTS 
    FreeAttributeObjectList();
#if 0
    // NOW FREE THE LIST OF OBJECTS 
    pItemList = m_lpItemList;
    for( i = 0 ; i < m_iTotalObjects; i++, pItemList++ )
	{
         // GET THE ATTRIBUTE OBJECT
         pGetAllAttributes = (CQServerGetAllAttributes*) pItemList->lParam;

         fWidePrintString("CAvisResultItems::ProcessSelectedList  Release pGetAllAttributes=0x%X  ObjectID= %d", 
                           ((DWORD)pGetAllAttributes), m_iExtDataObjectID );

         // FREE LIST OF ATTRIBUTE OBJECTS
         if( pGetAllAttributes != NULL )
             pGetAllAttributes->Release();
	}
#endif

    return S_OK;
}


/*----------------------------------------------------------------------------
    CAvisResultItems::FreeAttributeObjectList()

    Written by: Jim Hill
 ----------------------------------------------------------------------------*/
HRESULT CAvisResultItems::FreeAttributeObjectList()
{   
    LIST_DATA_STRUCT  *pItemList = NULL;
    int i;

    if( m_lpItemList == NULL )
        return S_OK;

    // NOW FREE THE LIST OF OBJECTS 
    pItemList = m_lpItemList;
    CQServerGetAllAttributes* pGetAllAttributes = NULL;
    for( i = 0 ; i < m_iTotalObjects; i++, pItemList++ )
	{
         // GET THE ATTRIBUTE OBJECT
         pGetAllAttributes = (CQServerGetAllAttributes*) pItemList->lParam;

         fWidePrintString("CAvisResultItems::ProcessSelectedList  Release pGetAllAttributes=0x%X  ObjectID= %d", 
                           ((DWORD)pGetAllAttributes), m_iExtDataObjectID );

         // FREE LIST OF ATTRIBUTE OBJECTS
         if( pGetAllAttributes != NULL )
             pGetAllAttributes->Release();
	}

    // FREE THE LIST ITSELF
    if( m_lpItemList != NULL )
    {
        MemFree( m_lpItemList );
        m_lpItemList = NULL;
	}

    return S_OK;
}


/*----------------------------------------------------------------------------
    CAvisResultItems::FreeResultItem()

    Not using. 7/13/99  IQCResultItem* freed by QsConsole
    Written by: Jim Hill
 ----------------------------------------------------------------------------*/
HRESULT CAvisResultItems::FreeResultItem()
{   
    int iGoAhead = 0;
    HRESULT  hr = S_OK;
    //CComQIPtr<IQCResultItem> pQCResultItem( m_pDataObject );
    IQCResultItem* pQCResultItem;

    // 7/13 Test disable
    return( hr );

    if( m_pQCResultItem != NULL )
    {
        pQCResultItem = (IQCResultItem*) m_pQCResultItem;
        hr = pQCResultItem->Release();
        m_pQCResultItem = NULL;
    }
    return( hr );
}


/*----------------------------------------------------------------------------
    CAvisResultItems::SubmitItem

    Set the attributes for auto submission 

    Written by: Jim Hill
 ----------------------------------------------------------------------------*/
HRESULT CAvisResultItems::SubmitItem( LIST_DATA_STRUCT  *pItemList )
{   
    HRESULT  hr = S_OK;
    CString sError, sCaption;
//    CString s;
//    VARIANT v;
//    VariantInit( &v );
//    VariantClear( &v );

    if( pItemList == NULL )
        return(E_FAIL);

    CQServerGetAllAttributes* pGetAllAttributes = (CQServerGetAllAttributes*) pItemList->lParam;
    if( pGetAllAttributes == NULL )
        return(E_FAIL);

//     // CHANGE ATTRIBUTES TO CAUSE AVIS TO AUTOMATICALLY SUBMIT THESE FILES TO THE ANALYSIS CENTER
//     s = AVIS_SAMPLE_ACTIONS_SUBMISSION_PRIORITY;
//     v.vt    = VT_UI4;
//     v.ulVal = 500;
//     hr = pGetAllAttributes->SetValueByAttributeName( (LPCTSTR) s, &v );

    hr = pGetAllAttributes->MarkSampleForSubmission( sError, sCaption, 0 );
    if( IsSuccessfullResult( hr ) )
    {
        ++m_iTotalSent;
        fWidePrintString("CAvisResultItems::SubmitItem  Selected item: %s    %s   ObjectID= %d", 
                            (LPCTSTR) pItemList->sItemStr,(LPCTSTR) pItemList->sSubItemStr, m_iExtDataObjectID );
    }
    else
    {
        fWidePrintString("FAILED: CAvisResultItems::SubmitItem Failed to submit selected item: %s    %s   ObjectID= %d", 
                            (LPCTSTR) pItemList->sItemStr,(LPCTSTR) pItemList->sSubItemStr, m_iExtDataObjectID );
    }

    return S_OK;
}



/*----------------------------------------------------------------------------
  CAvisResultItems::DeliverItem

  Set the attributes for auto delivery

  Written by: Jim Hill
----------------------------------------------------------------------------*/
HRESULT CAvisResultItems::DeliverItem( LIST_DATA_STRUCT  *pItemList )
{
    HRESULT  hr = S_OK;
    CString sError, sCaption;
//    CString s;
//    VARIANT v;
//    VariantInit( &v );
//    VariantClear( &v );


    if( pItemList == NULL )
        return(E_FAIL);

    CQServerGetAllAttributes* pGetAllAttributes = (CQServerGetAllAttributes*) pItemList->lParam;
    if( pGetAllAttributes == NULL )
        return(E_FAIL);

//     // CHANGE ATTRIBUTES TO CAUSE AVIS TO AUTOMATICALLY DISTRIBUTE NEW DEFS
//     s = AVIS_SAMPLE_ACTIONS_SIGNATURES_PRIORITY;
//     v.vt    = VT_UI4;
//     v.ulVal = 500;
//     hr = pGetAllAttributes->SetValueByAttributeName( (LPCTSTR) s, &v );

    hr = pGetAllAttributes->MarkSampleForAutoDefDelivery( sError, sCaption );
    if( IsSuccessfullResult( hr ) )
    {
        ++m_iTotalSent;  // jhill 7/12/00
        fWidePrintString("CAvisResultItems::DeliverItem  Selected item: %s    %s   ObjectID= %d",
                            (LPCTSTR) pItemList->sItemStr,(LPCTSTR) pItemList->sSubItemStr, m_iExtDataObjectID);
    }
    else
    {
        fWidePrintString("FAILED: CAvisResultItems::DeliverItem Failed to deliver selected item: %s    %s   ObjectID= %d", 
                            (LPCTSTR) pItemList->sItemStr,(LPCTSTR) pItemList->sSubItemStr, m_iExtDataObjectID );
    }

    return S_OK;
}



/*----------------------------------------------------------------------------
  CAvisResultItems::Release
  Written by: Jim Hill
----------------------------------------------------------------------------*/
HRESULT CAvisResultItems::Release()
{
#if 0
    // 6/21/99 causes an exception in MMC
    // DELETE RESULT ITEM
    IQCResultItem* pResultItem = (IQCResultItem *) m_pQCResultItem;
    if( pResultItem != NULL )
        pResultItem->Release();
    m_pQCResultItem = NULL;

    // THIS HAS SAME EFFECT
    if( m_pDataObject )
        m_pDataObject->Release();
    m_pDataObject = NULL;
#endif

    delete this;
    return S_OK;
}









