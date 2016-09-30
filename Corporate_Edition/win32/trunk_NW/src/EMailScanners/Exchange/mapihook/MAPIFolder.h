// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright (c) 2005 Symantec Corporation. All rights reserved.
/*===========================================================================*/

#ifndef __MAPIFOLDER_H__
#define __MAPIFOLDER_H__

class CMAPIFolder
{
public:
	CMAPIFolder(LPMAPIFOLDER pIMAPIFolder, MAPIAllocators& alloc) : m_allocators(alloc), m_paProps(alloc) { m_pIMAPIFolder = pIMAPIFolder; }
	~CMAPIFolder() { }

	LPCTSTR DisplayName(void);
	const LPSBinary EntryID(void);
	const LPSBinary RecordKey(void);

	HRESULT OpenFolder(CMAPIFolder **ppFolder, LPSBinary psbEntryID, ULONG flags = MAPI_BEST_ACCESS);
	HRESULT OpenMessage(CMessage **ppMessage, LPSBinary psbEntryID, ULONG flags = MAPI_BEST_ACCESS);
	HRESULT GetHierarchyTable(LPMAPITABLE* ppHierarchyTable, ULONG flags = fMapiUnicode);
	HRESULT GetContentsTable(LPMAPITABLE* ppContentsTable, ULONG flags = fMapiUnicode);
	HRESULT CreateMessage(CMessage **ppMessage, ULONG flags = 0);
	HRESULT GetProps(LPSPropTagArray pPropTagArray, ULONG *pcValues, LPSPropValue *ppPropArray);

private:
	enum MAPIFolderPropIndexes { INDEX_PR_ENTRYID = 0, INDEX_PR_DISPLAY_NAME = 1, INDEX_PR_RECORD_KEY = 2 };
		// Changes to MAPIFolderPropIndexes need to be reflected in sptMAPIFolderProps in CMAPIFolder::GetMAPIFolderProps().

	MAPIAllocators& m_allocators;
	CComQIPtr<IMAPIFolder, &IID_IMAPIFolder> m_pIMAPIFolder;
	SmartMAPIBufPtr<SPropValue>	m_paProps;

	void GetMAPIFolderProps(void);

    // copy constructor removed from interface
    CMAPIFolder(const CMAPIFolder &source);
 
    // assignment op removed from interface
    CMAPIFolder & operator = (const CMAPIFolder &source);

};

#endif //__MAPIFOLDER_H__