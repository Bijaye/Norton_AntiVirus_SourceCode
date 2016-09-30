/*
 * PROPRIETARY/CONFIDENTIAL.  use of this product is subject to license terms.
 * Copyright (c) 2005 Symantec Corporation.  All Rights Reserved.
 *
*/

#if !defined(AVIS_ATTRIBUTE_ACCESS_INCLUDED)
    #define AVIS_ATTRIBUTE_ACCESS_INCLUDED

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

// #include "qscon.h"


// FORWARD DECLARATIONS
//class CPlatformAttributesPage;
class CSampleAttributesPage; 
class CSampleActionsPage;
class CSampleErrorPage;  
class CQServerGetAllAttributes;



class CAttributeAccess  
{
public:
    CAttributeAccess( IDataObject* pDataObject, DWORD iExtDataObjectID );
    ~CAttributeAccess();
    void*   Initialize();
    DWORD   Release();
    DWORD   GetObjectSig()    { return( m_dwObjSignature ); }
    void*   GetPropPage( DWORD dwID );


    // INDIRECT ACCESS TO ATTRIBUTE OBJECT
    BOOL    IsDataAvailable(); 
    DWORD   GetNumFields();
    HRESULT IsErrorGettingQserverInterface(); 
    HRESULT SetValueByAttributeName( LPCTSTR lpszFieldName, VARIANT * pValue );
    HRESULT GetValueByAttributeName( LPCTSTR lpszFieldName, VARIANT * pValue );
    HRESULT GetValueByPartialAttributeName( VARIANT * pFullFieldName, LPCTSTR lpszPartialName,
                                            VARIANT * pValue, DWORD* lpdwIndex );


private:
    HRESULT GetQServerAttributeData();


//  DATA
public:
    CSampleAttributesPage*     m_pSampleAttributesPage;
    CSampleActionsPage*        m_pSampleActionsPage;
    CSampleErrorPage*          m_pSampleErrorPage;


    DWORD                      m_dwConstructorThreadID;
    DWORD                      m_dwAccessThreadID;
    int                        m_iExtDataObjectID;
	IDataObject*               m_pDataObject;
    void*                      m_pQServerGetAllAttributes;
    CQServerGetAllAttributes*  m_pGetAllAttributes;
    DWORD                      m_dwGetAllAttributesInitializedOK;

private:
    LONG                       m_dwRef;
    LONG                       m_dwRegistered;
    DWORD                      m_dwObjSignature;



};


// For GetPropPage( DWORD dwID );
#define GET_PLATFORMATTRIBUTES_PAGE   1
#define GET_SAMPLEATTRIBUTES_PAGE     2
#define GET_SAMPLEACTIONS_PAGE        3




#endif  

