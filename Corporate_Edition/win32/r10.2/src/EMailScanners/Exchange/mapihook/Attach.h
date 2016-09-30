// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright (c) 2005 Symantec Corporation. All rights reserved.
/*===========================================================================*/

#ifndef __ATTACH_H__
#define __ATTACH_H__

class CAttach
{
public:
	CAttach(LPATTACH pIAttach, MAPIAllocators& alloc) : m_allocators(alloc), m_paProps(alloc), m_ulAttachmentNumber(-1) { m_pIAttach = pIAttach; }
	~CAttach() { }

	LPCTSTR DisplayName(void);
	ULONG AttachNum(void);
	void SetAttachNum(ULONG ulAttachmentNumber) { m_ulAttachmentNumber = ulAttachmentNumber; }
	ULONG AttachMethod(void);
	ULONG AccessLevel(void);
	const LPSBinary AttachEncoding(void);
	const LPSBinary AttachTag(void);
	LPCTSTR AttachExtension(void);
	LPCTSTR AttachFilename(void);
	LPCTSTR AttachLongFilename(void);
	LPCTSTR AttachPathname(void);
	LPCTSTR AttachLongPathname(void);
	ULONG AttachSize(void);
	const LPSBinary RecordKey(void);
	HRESULT OpenProperty(ULONG ulPropTag, LPCIID lpiid, ULONG ulInterfaceOptions, ULONG ulFlags, LPUNKNOWN *lppUnk, int bThrow = true);
	HRESULT CopyToTemporaryFile(LPTSTR pszFilename);
	HRESULT SaveChanges(ULONG ulFlags) { return m_pIAttach->SaveChanges(ulFlags); }
	HRESULT CopyFromFile(LPTSTR pszFile);
	ULONG RenderingPosition(void);
	HRESULT SetProps(ULONG cValues, LPSPropValue lpPropArray, LPSPropProblemArray FAR * lppProblems, int bThrow = true);

private:
	enum AttachPropIndexes { INDEX_PR_ATTACH_NUM = 0, INDEX_PR_DISPLAY_NAME = 1, INDEX_PR_ATTACH_METHOD = 2,
		INDEX_PR_ACCESS_LEVEL = 3, INDEX_PR_ATTACH_ENCODING = 4, INDEX_PR_ATTACH_TAG = 5,
		INDEX_PR_ATTACH_EXTENSION = 6, INDEX_PR_ATTACH_FILENAME = 7, INDEX_PR_ATTACH_LONG_FILENAME = 8,
		INDEX_PR_ATTACH_PATHNAME = 9, INDEX_PR_ATTACH_LONG_PATHNAME = 10, INDEX_PR_ATTACH_SIZE = 11,
		INDEX_PR_RECORD_KEY = 12, INDEX_PR_RENDERING_POSITION = 13 };
		// Changes to AttachPropIndexes need to be reflected in sptAttachProps in CAttach::GetAttachProps().

	MAPIAllocators& m_allocators;

	CComQIPtr<IAttach, &IID_IAttachment> m_pIAttach;
	SmartMAPIBufPtr<SPropValue> m_paProps;

	void GetAttachProps(void);

    // copy constructor removed from interface
    CAttach(const CAttach &source);
 
    // assignment op removed from interface
    CAttach & operator = (const CAttach &source);

	// attachment number as-provided by the attachment-table
	ULONG m_ulAttachmentNumber;
};

#endif //__ATTACH_H__