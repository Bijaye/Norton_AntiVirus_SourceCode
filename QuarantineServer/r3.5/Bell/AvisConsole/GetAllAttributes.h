/*
 * PROPRIETARY/CONFIDENTIAL.  use of this product is subject to license terms.
 * Copyright (c) 2005 Symantec Corporation.  All Rights Reserved.
 *
*/

#if !defined(AVIS_GET_ALL_ATTRIBUTES_INCLUDED)
    #define AVIS_GET_ALL_ATTRIBUTES_INCLUDED

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "qscon.h"
#include "qserver.h"
#include "AvisSampleFields.h"
#include "safearray.h"


// GLOBAL
extern int g_GetAllAttributesClassTotalCount;
#define MY_OBJECT_SIGNATURE   585
#define ENABLE_AUTO_SUBMISSION_VALUE   500
#define ENABLE_AUTO_DELIVERY_VALUE     500



DWORD GetFormatByAttributeText(LPCTSTR lpszXAttributeName);     // LPTSTR





class CQServerGetAllAttributes  
{
public:
	CQServerGetAllAttributes( IDataObject* pDataObject, DWORD iExtDataObjectID = 0, IUnknown* pUnk = NULL,SEC_WINNT_AUTH_IDENTITY *pIdentity=NULL );
    ~CQServerGetAllAttributes();
    HRESULT Initialize();

    LONG    Release();
    DWORD   GetObjectSig()    { return( m_dwObjSignature ); }
    BOOL    IsDataAvailable() { return( m_dwIsDataAvailable ); }
    DWORD   GetNumFields() { return( m_dwNumFields ); }
    HRESULT IsErrorGettingQserverInterface() { return( m_iQserverInterfaceError ); }    // != S_OK
    HRESULT DumpAllAttributes();
    HRESULT SetValueByAttributeName( LPCTSTR lpszFieldName, VARIANT * pValue );
    HRESULT GetValueByAttributeName( LPCTSTR lpszFieldName, VARIANT * pValue );
    HRESULT GetValueByPartialAttributeName( VARIANT * pFullFieldName, LPCTSTR lpszPartialName,
                                            VARIANT * pValue, DWORD* lpdwIndex );
    HRESULT GetValueOrCreateAttribute( LPCTSTR lpszFieldName, VARIANT* pValue, DWORD* lpdwAdded = NULL );
    HRESULT IncrementSampleChangeCounter();
    HRESULT CommitAndIncrementSampleChangeCounter();
    HRESULT GetFilenameAndIcon( LPTSTR lpszFilename, HICON* phIcon );
    void GetStatusText(CString& sStatusText, DWORD dwStatus );


    // SUBMIT AND DELIVER CRITERIA
    BOOL IsValidSampleSubmission(CString& sError, CString& sCaption);
    BOOL IsValidDefDelivery(CString& sError, CString& sCaption);
    HRESULT MarkSampleForSubmission(CString& sError, CString& sCaption, DWORD dwSubmissionPriority );
    HRESULT MarkSampleForAutoDefDelivery(CString& sError, CString& sCaption);

    // RESUBMIT
    BOOL RestoreAllSampleAttributes(); 
    BOOL Restore_DeleteAttribute(VARIANT * pFieldName, VARIANT * pValue ); 
    BOOL RestoreIsEligibleForReSubmission();                                
    BOOL RestoreIsAttributeDeleteable( CString& s );
    BOOL RestoreIsAttributeAlreadyDeleted( VARIANT * pValue );


private:
    // LOOKUP TEXT AND IDs FROM XAttributesList
    //DWORD GetResIDFromLocalID(DWORD dwID);  
    //BOOL  GetAttributeTextFromLocalID(CString& sText, DWORD dwID);  
    //BOOL  LoadDisplayStringFromLocalID(CString& sText, DWORD dwID);  
    //BOOL  LoadStringTextFromAttributeText(CString& sText, LPTSTR lpszXAttributeName);  
    //BOOL  IsAttributeExcludedByLocalID(DWORD dwID);  
    //BOOL  IsAttributeExcludedByAttributeText(LPTSTR lpszXAttributeName);  

    HRESULT GetAllAttributes();
    HRESULT GetServerInterface();
    HRESULT MarshallInterfaceSingleSelect();  //  DWORD dwItemIndex 
    HRESULT MarshallInterfaceMultiSelect();
    HRESULT DestroyAllData();
    HRESULT ReleaseSeverInterface();
    HRESULT CreateAllMissingAttributes();




//  DATA
public:
    TCHAR                   m_szFileName[MAX_PATH+1];   // DEBUG
    int                     m_iExtDataObjectID;
    DWORD                   m_dwIsDataAvailable;
    DWORD                   m_dwIsMultiSelect;
	IDataObject*            m_pDataObject;
    IQCResultItem*          m_pResultItem;
    IQuarantineServerItem*  m_pQServerItem;
    IUnknown*               m_pUnk; 
    LPSTREAM                m_pServerStream;
    SEC_WINNT_AUTH_IDENTITY *m_pIdentity;  // added to store the connection identity tam 7-13-00

    //BOOL                    m_bQserverInterfaceMarshalled;
    HRESULT                 m_iQserverInterfaceError;
    ULONG                   m_ulCount;
    ULONG *                 m_ulItems;
    DWORD                   m_dwIsDisabled;   // FOR SUBMIT OR DELIVER
    // SAFEARRRAY STUFF
    VARIANT                 m_va;
    DWORD                   m_dwNumFields;

    CString                 m_sAttributeDeleteExcludedList;
    CString                 m_sAttributeRestoreExcludedList;
    BOOL                    m_bOkToReSubmit;

private:
    LONG                    m_dwRef;
    LONG                    m_dwRegistered;
    DWORD                   m_dwObjSignature;


private:
    _safearray_t< 2, VARIANT, Variant> m_sa;  

};







#endif // AVIS_GET_ALL_ATTRIBUTES_INCLUDED



