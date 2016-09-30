// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright (c) 2005 Symantec Corporation. All rights reserved.
/*===========================================================================*/

#ifndef __MESSAGE_H__
#define __MESSAGE_H__

class CMessage
{
public:
	typedef HRESULT (*PFNEnumAttach)(CAttach *pAttach, void *pvData);

	CMessage(LPMESSAGE pIMessage, MAPIAllocators& alloc) : m_allocators(alloc), m_paProps(alloc) { m_pIMessage = pIMessage; }
	~CMessage() { }

	LPCTSTR SenderName(void);
	LPCTSTR SenderEmail(void);
	LPCTSTR UserName(void);
	LPCTSTR NormalizedSubject(void);
	int HasAttach(void);
	const LPSBinary EntryID(void);
	ULONG MessageFlags(void);
	const LPSBinary RecordKey(void);
	const LPSBinary SentRepresentingSearchKey(void);
	LPCTSTR HasSig(void);
	const LPSBinary SentRepresentingEntryID(void);
	LPCTSTR SentRepresentingEmailAddress(void);
	LPCTSTR SentRepresentingName(void);
	LPCTSTR SentRepresentingAddrType(void);
	HRESULT OpenAttach(CAttach **ppAttach, ULONG ulAttachNum, ULONG flags = MAPI_BEST_ACCESS);
	HRESULT EnumAttachments(PFNEnumAttach pfnEnumAttach, void *pvData);
	HRESULT DeleteAttach(ULONG ulAttachmentNum, ULONG ulUIParam = 0, LPMAPIPROGRESS lpProgress = 0, ULONG ulFlags = 0);
	HRESULT SaveChanges(ULONG ulFlags) { return m_pIMessage->SaveChanges(ulFlags); }
	HRESULT GetAttachmentTable(LPMAPITABLE* ppAttachmentTable, ULONG flags = fMapiUnicode);
	HRESULT OpenProperty(ULONG ulPropTag, LPCIID lpiid, ULONG ulInterfaceOptions, ULONG ulFlags, LPUNKNOWN *lppUnk, int bThrow = true);
	HRESULT SetProps(ULONG cValues, LPSPropValue lpPropArray, LPSPropProblemArray FAR * lppProblems, int bThrow = true);
	HRESULT ModifyRecipients(ULONG ulFlags, LPADRLIST lpMods);
	HRESULT SubmitMessage(ULONG ulFlags = 0);
	HRESULT EnumRecipients();
	HRESULT GetRecipientTable(LPMAPITABLE* ppRecipientsTable, ULONG flags);
	LPCTSTR Recipients() { return m_szRecipients; }

private:
	enum MessagePropIndexes { INDEX_PR_ENTRYID = 0, INDEX_PR_NORMALIZED_SUBJECT = 1, INDEX_PR_HASATTACH = 2, INDEX_PR_SENDER_NAME = 3, INDEX_PR_SENDER_EMAIL_ADDRESS = 4, INDEX_PR_RECORD_KEY = 5, 
		INDEX_PR_SENT_REPRESENTING_SEARCH_KEY = 6, INDEX_PR_SENT_REPRESENTING_ENTRYID = 7, INDEX_PR_SENT_REPRESENTING_EMAIL_ADDRESS = 8, INDEX_PR_SENT_REPRESENTING_NAME = 9,
		INDEX_PR_SENT_REPRESENTING_ADDRTYPE = 10, INDEX_PR_MESSAGE_FLAGS = 11, INDEX_PR_HASSIG = 12, 
		INDEX_PR_RECEIVED_BY_NAME = 13};
		// Changes to MessagePropIndexes need to be reflected in sptMessageProps in CMessage::GetMessageProps().

	MAPIAllocators& m_allocators;

	CComQIPtr<IMessage, &IID_IMessage> m_pIMessage;
	SmartMAPIBufPtr<SPropValue> m_paProps;
	TCHAR m_szRecipients[MAX_RECIPIENTS_LEN];

	void GetMessageProps(void);

    // copy constructor removed from interface
    CMessage(const CMessage &source);
 
    // assignment op removed from interface
    CMessage & operator = (const CMessage &source);

};

#endif //__MESSAGE_H__