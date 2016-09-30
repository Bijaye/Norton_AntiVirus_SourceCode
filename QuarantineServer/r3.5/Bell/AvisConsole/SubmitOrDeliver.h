/*
	SubmitOrDeliver.h


*/


#ifndef SUBMITORDELIVER_H_INCLUDED
    #define SUBMITORDELIVER_H_INCLUDED

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000




// ACTION TYPES FOR m_dwActionType AND Initialize( DWORD dwActionType );
#define SUBMIT_ACTION_TYPE     1
#define DELIVER_ACTION_TYPE    2


class CAvisResultItems  
{
public:

    CAvisResultItems( IDataObject* pDataObject, DWORD iExtDataObjectID, BOOL bAutoSubmit )
    {
        m_iExtDataObjectID = iExtDataObjectID;
    	fWidePrintString("CAvisResultItems constructor called. Caller's ObjectID= %d", m_iExtDataObjectID);
        m_pDataObject       = pDataObject;
        m_dwIsDataAvailable = 0;
        m_dwRef             = 0;
        //m_pGetAllAttributes = NULL;
        m_lpItemList        = NULL;
        m_ulCount           = 0;
        m_aItemIDs          = NULL; 
        m_pQCResultItem     = NULL;
        m_iTotalObjects     = 0;
        m_iTotalSent        = 0;
        m_iItemsDisabled    = 0;
        if( bAutoSubmit )
            m_dwActionType = SUBMIT_ACTION_TYPE;
        else
            m_dwActionType = DELIVER_ACTION_TYPE;

    //    m_pResultItem       = NULL;
    //    m_pQServerItem      = NULL;
    //    m_pUnk              = NULL;
    //    m_ulItems           = NULL;
    //    m_ulCount           = 0;
    //    m_dwRef             = 0;
    }

    ~CAvisResultItems()
    {
      	fWidePrintString("CAvisResultItems destructor called. Caller's ObjectID= %d", m_iExtDataObjectID);
    
    }

    //DWORD   IsDataAvailable() { return( m_dwIsDataAvailable ); }


    HRESULT Release();
    HRESULT ReleaseSeverInterface();
    HRESULT Initialize();
    HRESULT SubmitOrDeliverAllItems();
    HRESULT SubmitItem( LIST_DATA_STRUCT  *pItemList );
    HRESULT DeliverItem( LIST_DATA_STRUCT  *pItemList );
    HRESULT ProcessSelectedList( int nResponse );
    HRESULT FreeAttributeObjectList();
    HRESULT FreeResultItem();


//  DATA
public:
    int                       m_iExtDataObjectID;
	IDataObject*              m_pDataObject;
    int                       m_iTotalObjects;
    int                       m_iItemsDisabled;
    int                       m_iTotalSent;
    LIST_DATA_STRUCT*         m_lpItemList;
    ULONG                     m_ulCount;
    ULONG*                    m_aItemIDs;
    void*                     m_pQCResultItem;

    //CQServerGetAllAttributes* m_pGetAllAttributes; 


//    IQuarantineServerItem*  m_pQServerItem;
//    IUnknown*               m_pUnk; 
//    ULONG                   m_ulCount;
//    ULONG *                 m_ulItems;



private:
    LONG        m_dwRef;
    DWORD       m_dwIsDataAvailable;
    //BOOL        m_bAutoSubmit;
    DWORD       m_dwActionType;
};



#endif


