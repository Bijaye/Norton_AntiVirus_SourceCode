//------------------------------------------------------------------------
// DecUnicode.h
//
// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright (c) Symantec, Corp. 2001, 2005. All rights reserved.
//------------------------------------------------------------------------

#ifndef DECUNICODE_H
#define DECUNICODE_H

#define NAMESPACE_UNICODE			"uni://"
#define NAMESPACE_UNICODE_U			"UNI://"
#define NAMESPACE_UNICODE_LEN		6
#define UNICODE_MAPPINGOBJECT_LEN	(NAMESPACE_UNICODE_LEN + 8)
#define UNICODE_MAX_PATH			(64 * 1024)


class CUNIMO
{
public:
	CUNIMO()
	{
		szMBCSName[0] = '\0';
		pszUniName = NULL;
		pPrev = NULL;
		pNext = NULL;
		lRefCount = 1;
	}

	~CUNIMO()
	{
		// Make sure no references remain
		dec_assert(0 == lRefCount);

		if (pszUniName)
		{
			free(pszUniName);
			pszUniName = NULL;
		}		
	}

	inline bool Init(const wchar_t *pwszUnicodeName)
	{
#if defined(_WINDOWS)
		size_t sizeNameLen = (wcslen(pwszUnicodeName) * sizeof(wchar_t)) + sizeof(wchar_t);
		dec_assert(sizeNameLen <= UNICODE_MAX_PATH);

		pszUniName = (wchar_t *)malloc(sizeNameLen);
		if (!pszUniName)
		{
			dec_assert(0);
			return false;
		}

		// Generate a unique name for this mapping object.
		// Using the address of the object should work fine since that
		// by definition MUST be unique.
		sssnprintf(szMBCSName, sizeof(szMBCSName), NAMESPACE_UNICODE "%08x", this);

		// Copy the input Unicode string into the object
		wcscpy(pszUniName, pwszUnicodeName);

		// Since new objects are always added to the end of the linked-list,
		// set the object's pNext pointer to NULL.
		pNext = NULL;
		return true;
#else
		return false;
#endif
	}

	char	szMBCSName[_MAX_PATH];
	wchar_t *pszUniName;
	CUNIMO	*pPrev;
	CUNIMO	*pNext;
	long	lRefCount;
};


DECSDKLINKAGE void STDCALL InitializeUnicodeMapping(void);
DECSDKLINKAGE void STDCALL EndUnicodeMapping(void);
DECSDKLINKAGE CUNIMO * STDCALL NewUnicodeMappingObject(const wchar_t *pUnicodeName);
DECSDKLINKAGE CUNIMO * STDCALL GetUnicodeMappingObject(const char *pszMBCSName);
DECSDKLINKAGE CUNIMO * STDCALL GetUnicodeMappingObjectW(const wchar_t *pszUnicodeName);
DECSDKLINKAGE void STDCALL DeleteUnicodeMappingObject(CUNIMO *pObjectToDelete);

#endif // DECUNICODE_H
