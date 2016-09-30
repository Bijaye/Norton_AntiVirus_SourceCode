// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright (c) 2005 Symantec Corporation. All rights reserved.
/*===========================================================================*/

#ifndef __MSGSTORE_H__
#define __MSGSTORE_H__

class CMsgStore
{
public:
	CMsgStore(LPMDB pIMsgStore, MAPIAllocators& alloc, bool bLogout = true) : m_allocators(alloc), m_bLogout(bLogout), m_paProps(alloc), m_pIMsgStore(){ m_pIMsgStore = pIMsgStore; }
	~CMsgStore() { if (m_bLogout) StoreLogoff(); }

	HRESULT StoreLogoff(ULONG *lpulFlags = 0);		// Default logoff flags: LOGOFF_ORDERLY
	LPCTSTR DisplayName(void);
	ULONG StoreSupportMask(void);
	const LPSBinary EntryID(void);
	const LPSBinary IPMSubtreeEntryID(void);
	const LPSBinary RecordKey(void);
	ULONG ValidFolderMask(void);
	HRESULT OpenFolder(CMAPIFolder **ppFolder, LPSBinary psbEntryID, ULONG flags = MAPI_BEST_ACCESS);
	HRESULT OpenOutboxFolder(CMAPIFolder **ppFolder, ULONG flags = MAPI_BEST_ACCESS);
	HRESULT OpenInboxFolder(CMAPIFolder **ppFolder, ULONG ulFlags = MAPI_BEST_ACCESS);
	HRESULT GetOutlookFolderEntryIDs(ULONG *pcValues, LPSPropValue *ppaProps);
	ULONG CompareEntryIDs(ULONG cb1, LPENTRYID pEntryID1, ULONG cb2, LPENTRYID pEntryID2);

protected:
	enum MsgStorePropIndexes { INDEX_PR_ENTRYID = 0, INDEX_PR_DISPLAY_NAME = 1, INDEX_PR_STORE_SUPPORT_MASK = 2, INDEX_PR_IPM_SUBTREE_ENTRYID = 3, INDEX_PR_RECORD_KEY = 4, INDEX_PR_VALID_FOLDER_MASK = 5 };
		// Changes to MsgStorePropIndexes need to be reflected in sptMsgStoreProps in CMsgStore::GetMsgStoreProps().

	const int m_bLogout;

	MAPIAllocators& m_allocators;
	CComQIPtr<IMsgStore, &IID_IMsgStore> m_pIMsgStore;
	SmartMAPIBufPtr<SPropValue> m_paProps;

	void GetMsgStoreProps(void);

    // copy constructor removed from interface
    CMsgStore(const CMsgStore &source);
 
    // assignment op removed from interface
    CMsgStore & operator = (const CMsgStore &source);

};

#endif //__MSGSTORE_H__