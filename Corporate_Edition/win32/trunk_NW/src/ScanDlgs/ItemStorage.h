// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright (c) 2005 Symantec Corporation. All rights reserved.
//----------------------------------------------------------------
//
//  File: ItemStorage.h
//  Purpose: Stores Long Items in a temp storage file
//
//	Date: 2-21-98
//
//	Owner: Randy Templeton
//
//	***** Intel Corp. Confidential ******
//
//----------------------------------------------------------------
#if (!defined ITEM_STORAGE_INCLUDED)
#define ITEM_STORAGE_INCLUDED

#include "ResultItem.h"


class CItemStorage : public CObject
{
private:
	CMapStringToString  m_map;
    CStdioFile          m_file;
    BOOL                m_bOpen;
    BOOL                m_bChanged;
    CString             m_sCurrentPath;
public:
	CItemStorage();						 
	~CItemStorage();									// releases the storage
    void Close();
    void Flush();
	BOOL AddToStorage(LPCSTR strLogLine);				// Add log line to storage
	BOOL LoadStorage(LPCSTR sKey, CStringList& stringList);	// Load the storage for the key
	CString GetKey(CResultItem& Item);					// get the key from the item
	CString GetKey(LPCSTR sComputer,DWORD id);			// get the key form computer and id
};

#endif