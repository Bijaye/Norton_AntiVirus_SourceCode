// Rec2.h : Recomposer 2 Interface
// Copyright 1999 by Symantec Corporation.  All rights reserved.


#if !defined(REC2_H)
#define REC2_H

// Include the pre-requisite DEC2.H header file if it has not
// already been included.
#include "Dec2.h"

/////////////////////////////////////////////////////////////////////////////
// Forward Declarations

class CContainerItem;
class IRecEventSink;
class CRecEventSink;


/////////////////////////////////////////////////////////////////////////////
// HRESULT Codes
//
// All Recomposer HRESULT error values use the facility FACILITY_ITF
// and error codes in the range 0x0200 - 0xFFFF.  This is in accordance
// with Microsoft's recommendations in the Platform SDK.
//
// Once created, an error code's value must not change.
// Engine authors should resist adding new error codes whenever possible.
// Applications should properly handle unlisted error codes.

#define RECERR(code)  MAKE_HRESULT(1, FACILITY_ITF, 0x0200 + code)

enum RecErr
{
    REC_OK                        = 0,
    RECERR_OUT_OF_MEMORY          = RECERR(1),
    RECERR_NO_DELETE_ACCESS       = RECERR(2),
    RECERR_NO_REPLACE_ACCESS      = RECERR(3),
    RECERR_NO_MODIFY_ACCESS       = RECERR(4),
    RECERR_GENERIC                = RECERR(200)
};


enum RecAction
{
    RECACTION_NONE,                     // No action
    RECACTION_DELETE,                   // Delete item from container
    RECACTION_REPLACE,                  // Replace item in container
    RECACTION_MODIFY,                   // Modify item in container
    RECACTION_EXTRACT                   // Extract item from container
};


/////////////////////////////////////////////////////////////////////////////
// Class CContainerItem
//

class CContainerItem
{
public:
    CContainerItem ();
    ~CContainerItem ();

    void SetItemName (LPTSTR pName);
    LPTSTR GetItemName (void);
    void SetNewItemName (LPTSTR pNewName);
    LPTSTR GetNewItemName (void);
    void SetItemAction (int Action);
    int  GetItemAction (void);
    void SetItemData (UINT uData);
    UINT GetItemData (void);
    void SetNextItem (CContainerItem* pNext);
    CContainerItem* GetNextItem (void);
    HRESULT GetItemResult (void);
    void SetItemResult (HRESULT hr);

    // Performance enhancement - when TRUE, this item has already
    // been processed so no need to examine it again.
    BOOL            m_Processed;

protected:
    LPTSTR          m_pszItemName;
    LPTSTR          m_pszNewItemName;
    int             m_Action;
    UINT            m_Data;
    CContainerItem* m_pNextItem;

    // This member is used to track whether or not the
    // Recomposer successfully performed the requested
    // action on this item.  When set to REC_OK, the
    // action was completed successfully.  Any other value
    // indicates that the action has not been carried out.
    HRESULT         m_hr;
};


/////////////////////////////////////////////////////////////////////////////
// Interface IRecEventSink

class IRecEventSink
{
public:
    // IRecEventSink methods:

    virtual HRESULT OnError(CContainerItem *pItem, HRESULT hr, BOOL *pbAbort) = 0;
    virtual HRESULT OnNew(IDecObject* pObject, CContainerItem *pItem, LPTSTR lpExtractedFile, BOOL *pbAbort) = 0;
	virtual HRESULT OnBusy() = 0;
    virtual HRESULT OnEngineError(IDecContainerObject* pObject, HRESULT hr, WORD* pwResult, LPTSTR pszNewDataFile) = 0;
};


/////////////////////////////////////////////////////////////////////////////
// Class CRecEventSink
//
// Standard implementation of a Recomposer event sink.

class CRecEventSink : 
    public IRecEventSink
{
public:
    virtual HRESULT OnError(CContainerItem *pItem, HRESULT hr, BOOL *pbAbort)
    {
        *pbAbort = FALSE;
        return REC_OK; 
    } 

    virtual HRESULT OnNew(IDecObject* pObject, CContainerItem *pItem, LPTSTR lpExtractedFile, BOOL *pbAbort)
    {
        *pbAbort = FALSE;
        return REC_OK;
    }

	virtual HRESULT OnBusy()
	{
		return DEC_OK;
	}

    virtual HRESULT OnEngineError(IDecContainerObject* pObject, HRESULT hr, WORD* pwResult, LPTSTR pszNewDataFile)
    {
        *pwResult = DECRESULT_UNCHANGED;
        *pszNewDataFile = '\0';
        return hr;
    }
};


/////////////////////////////////////////////////////////////////////////////
// Global Functions
#if defined(_WINDOWS)
#ifdef REC2_CPP
#define REC2IE __declspec(dllexport) 
#else
#define REC2IE __declspec(dllimport) 
#endif

REC2IE HRESULT Recompose (LPTSTR pContainerName, CContainerItem *pItemList, IRecEventSink *pSink, LPCTSTR szDir);
#endif // _WINDOWS

#if defined(UNIX)
EXTERN_C HRESULT Recompose (LPTSTR pContainerName, CContainerItem *pItemList, IRecEventSink *pSink, LPCTSTR szDir);
#endif

/////////////////////////////////////////////////////////////////////////////
// Library functions
LPTSTR BuildContainerString (IDecObject* pObject);
BOOL ParseContainerString (LPTSTR pPath, LPTSTR pBuffer, long lSize, DWORD dwDepth);
LPTSTR AppendText (LPTSTR pszBuffer, DWORD *pdwSize, LPTSTR pszText);
LPCTSTR GetRecToken (void);

#endif  // REC2_H
