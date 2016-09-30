// Rec2.h : Recomposer 2 Interface
// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright 2001, 2005 by Symantec Corporation.  All rights reserved.

#if !defined(REC2_H)
#define REC2_H

#include "DAPI.h"

#define MY_COPY_BUFFER_SIZE		32 * 1024

/////////////////////////////////////////////////////////////////////////////
// Forward Declarations

class CContainerItem;
class IRecEventSink;
class CRecEventSink;

/////////////////////////////////////////////////////////////////////////////
// DECRESULT Codes
//
// All Recomposer DECRESULT error values use the facility FACILITY_ITF
// and error codes in the range 0x0200 - 0xFFFF.  This is in accordance
// with Microsoft's recommendations in the Platform SDK.
//
// Once created, an error code's value must not change.
// Engine authors should resist adding new error codes whenever possible.
// Applications should properly handle unlisted error codes.

#define RECERR(code)  MAKE_DECRESULT(1, FACILITY_ITF, 0x0200 + code)

enum RecErr
{
	REC_OK						 = 0,
	RECERR_OUT_OF_MEMORY		 = RECERR(1),
	RECERR_NO_DELETE_ACCESS		 = RECERR(2),
	RECERR_NO_REPLACE_ACCESS	 = RECERR(3),
	RECERR_NO_MODIFY_ACCESS		 = RECERR(4),
	RECERR_DAPI_NOT_LOADED		 = RECERR(5),
	RECERR_GENERIC				 = RECERR(200)
};


enum RecAction
{
	RECACTION_NONE,						// No action
	RECACTION_DELETE,					// Delete item from container
	RECACTION_REPLACE,					// Replace item in container
	RECACTION_MODIFY,					// Modify item in container
	RECACTION_EXTRACT					// Extract item from container
};


/////////////////////////////////////////////////////////////////////////////
// Class CContainerItem
//

class CContainerItem
{
public:
	CContainerItem ();
	~CContainerItem ();

	void SetItemName (char *pName);
	char *GetItemName (void);
	void SetNewItemName (char *pNewName);
	char *GetNewItemName (void);
	void SetItemAction (int Action);
	int	GetItemAction (void);
	void SetItemData (UINT uData);
	UINT GetItemData (void);
	void SetNextItem (CContainerItem* pNext);
	CContainerItem* GetNextItem (void);
	DECRESULT GetItemResult (void);
	void SetItemResult (DECRESULT hr);

	// Performance enhancement - when TRUE, this item has already
	// been processed so no need to examine it again.
	bool			m_Processed;

protected:
	char			*m_pszItemName;
	char			*m_pszNewItemName;
	int				m_Action;
	UINT			m_Data;
	CContainerItem* m_pNextItem;

	// This member is used to track whether or not the
	// Recomposer successfully performed the requested
	// action on this item.  When set to REC_OK, the
	// action was completed successfully.  Any other value
	// indicates that the action has not been carried out.
	DECRESULT		 m_hr;
};


/////////////////////////////////////////////////////////////////////////////
// CContainerItem::GetItemName ()

inline char *CContainerItem::GetItemName (void)
{
	return (m_pszItemName);
}


/////////////////////////////////////////////////////////////////////////////
// CContainerItem::GetNewItemName ()

inline char *CContainerItem::GetNewItemName (void)
{
	return (m_pszNewItemName);
}


/////////////////////////////////////////////////////////////////////////////
// CContainerItem::SetItemAction ()

inline void CContainerItem::SetItemAction (int Action)
{
	m_Action = Action;
}


/////////////////////////////////////////////////////////////////////////////
// CContainerItem::GetItemAction ()

inline int CContainerItem::GetItemAction (void)
{
	return (m_Action);
}


/////////////////////////////////////////////////////////////////////////////
// CContainerItem::SetItemData ()

inline void CContainerItem::SetItemData (UINT uData)
{
	m_Data = uData;
}


/////////////////////////////////////////////////////////////////////////////
// CContainerItem::GetItemData ()

inline UINT CContainerItem::GetItemData (void)
{
	return (m_Data);
}


/////////////////////////////////////////////////////////////////////////////
// CContainerItem::SetItemResult ()

inline void CContainerItem::SetItemResult (DECRESULT hr)
{
	m_hr = hr;
}


/////////////////////////////////////////////////////////////////////////////
// CContainerItem::GetItemResult ()

inline DECRESULT CContainerItem::GetItemResult (void)
{
	return (m_hr);
}


/////////////////////////////////////////////////////////////////////////////
// CContainerItem::SetNextItem ()

inline void CContainerItem::SetNextItem (CContainerItem* pNext)
{
	m_pNextItem = pNext;
}


/////////////////////////////////////////////////////////////////////////////
// CContainerItem::GetNextItem ()

inline CContainerItem* CContainerItem::GetNextItem (void)
{
	return (m_pNextItem);
}


/////////////////////////////////////////////////////////////////////////////
// Interface IRecEventSink

class IRecEventSink
{
public:
	// IRecEventSink methods:
	virtual DECRESULT OnError(CContainerItem *pItem, DECRESULT hr, bool *pbAbort) = 0;
	virtual DECRESULT OnObjectBegin(IDecObject* pObject, CContainerItem *pItem, char *lpExtractedFile, bool *pbAbort) = 0;
	virtual DECRESULT OnBusy() = 0;
	virtual DECRESULT OnEngineError(IDecContainerObject* pObject, DECRESULT hr, WORD* pwResult, char *pszNewDataFile) = 0;
	virtual DECRESULT OnKeyNeeded(IDecContainerObject *pObject, unsigned char *pbyKey, int nKeyBufferLen, int *piKeyLen, bool *pbContinue, DWORD *lpdwParam) = 0;

	virtual DECRESULT GetFileSystem(IDecFiles **ppfs) = 0;
	virtual DECRESULT SetFileSystem(IDecFiles *pfs) = 0;
};


/////////////////////////////////////////////////////////////////////////////
// Class CRecEventSink
//
// Standard implementation of a Recomposer event sink.

class CRecEventSink : 
	public IRecEventSink
{
public:
	virtual DECRESULT OnError(CContainerItem *pItem, DECRESULT hr, bool *pbAbort)
	{
		*pbAbort = FALSE;
		return REC_OK; 
	} 

	virtual DECRESULT OnObjectBegin(IDecObject* pObject, CContainerItem *pItem, char *lpExtractedFile, bool *pbAbort)
	{
		*pbAbort = FALSE;
		return REC_OK;
	}

	virtual DECRESULT OnBusy()
	{
		return DEC_OK;
	}

	virtual DECRESULT OnEngineError(IDecContainerObject* pObject, DECRESULT hr, WORD* pwResult, char *pszNewDataFile)
	{
		*pwResult = DECRESULT_UNCHANGED;
		*pszNewDataFile = '\0';
		return hr;
	}

	virtual DECRESULT OnKeyNeeded(IDecContainerObject *pObject, unsigned char *pbyKey, int nKeyBufferLen, int *piKeyLen, bool *pbContinue, DWORD *lpdwParam)
	{
		*pbContinue = FALSE;
		return DECERR_CHILD_DECRYPT;
	}

	virtual DECRESULT GetFileSystem(IDecFiles **ppfs)
	{
		*ppfs = m_pfs;
		return REC_OK;
	}

	virtual DECRESULT SetFileSystem(IDecFiles *pfs)
	{
		m_pfs = pfs;
		return REC_OK;
	}



private:
	IDecFiles *m_pfs;
};


/////////////////////////////////////////////////////////////////////////////
// Global Functions
#if defined(_WINDOWS)
  #ifdef REC2_CPP
	#define RECLINKAGE __declspec(dllexport) 
  #else
	#define RECLINKAGE __declspec(dllimport) 
  #endif
#else
  #ifdef __cplusplus
	#define RECLINKAGE extern "C"
  #else
	#define RECLINKAGE
  #endif
#endif

RECLINKAGE DECRESULT Recompose (char *pszConfigFile, char *pContainerName, CContainerItem *pItemList, IRecEventSink *pSink, const char *szDir);

/////////////////////////////////////////////////////////////////////////////
// Library functions
#if defined(_WINDOWS)
  #define RECLIBLINKAGE
#else
  #ifdef __cplusplus
	#define RECLIBLINKAGE extern "C"
  #else
	#define RECLIBLINKAGE
  #endif
#endif

RECLIBLINKAGE char *BuildContainerString (IDecObject* pObject);
RECLIBLINKAGE bool ParseContainerString (char *pPath, char *pBuffer, long lSize, DWORD dwDepth);
RECLIBLINKAGE char *AppendText (char *pszBuffer, DWORD *pdwSize, char *pszText);
RECLIBLINKAGE const char *GetRecToken (void);

extern char szRecToken[6]; // = _T(RECOMPOSER_TOKEN);

//************************************************************************
//
// const char *GetRecToken (void)
//
// Purpose:
//   Returns a pointer to the token used to separate container items.
//
//************************************************************************
inline const char *GetRecToken (void)
{
	return (&szRecToken[0]);
}

#endif	// REC2_H
