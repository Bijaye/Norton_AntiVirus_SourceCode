// Dec2LIB.cpp : Decomposer 2 Library (for Decomposer use only)
// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright 2000, 2005 by Symantec Corporation.  All rights reserved.

// Fix the defect in IBM's header files for V3.x of Visual Age for C++.
#include "fixibmps.h"

// character conversion stuff
#include "asc_char.h"
#include "asc_ctype.h"
#include "bstricmp.h"

// Project headers
#define DEC2_CPP
#include "DecPlat.h"
#include "Dec2.h"
#include "Dec2EX.h"
#include "Dec2LIB.h"
#include "Decstdio.h"
#include "dectmpf.h"

// Pull in the code for binary_stricmp
#include "../CharUtil/bstricmp.c"

// C Library headers
#include <time.h>

#include "SymSaferStrings.h"

static char AtoZString[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";


/////////////////////////////////////////////////////////////////////////////
// CDecObj Methods


CDecObj::CDecObj(IDecContainerObject* pParentObject /* = NULL */) 
:	m_pParent(pParentObject),

#if !defined(OS400) //*** Start AS400 Attribute API Removal *** 
	m_eAttributeState(eUninitialized),
	m_eAttributeSupport(eBaseSupport),
#endif //*** End AS400 Attribute API Removal *** 

	m_pEngineInterface(NULL),
	m_pszName(NULL),
	m_pszName2(NULL),
	m_dwNameType(DEC_NAMETYPE_INTERNAL),  // Assume internal file system compatibility  
	m_dwNameCharset(DEC_CHARSET_CP1252),  // Assume ANSI codepage
	m_bCanDelete(false),
	m_bCanModify(false),
	m_bCanReplace(false),
	m_bCanRename(false),
	m_dwMalformedType(DEC_TYPE_UNKNOWN),
	m_dwContainerType(DEC_TYPE_UNKNOWN),
	m_dwItemType(DEC_ITEMTYPE_NORMAL),
	m_dwAttrs(0),
	m_dwAttrType(DEC_ATTRTYPE_DOS),
	m_dwYear(1980),
	m_dwMonth(1),
	m_dwDay(1),
	m_dwHour(0),
	m_dwMinute(0),
	m_dwSecond(0),
	m_dwMillisecond(0),
	m_dwlUncompressedSize(DEC_CHILD_SIZE_UNKNOWN),
	m_dwlCompressedSize(DEC_CHILD_SIZE_UNKNOWN),
	m_pCachedFile(NULL),
	m_pbyCachedBuffer(NULL),
	m_sizeCachedBufferSize(0),
	m_lCachedFileSize(0),
	m_pDecObjValString(NULL)

{	 
	*m_szDataFile = '\0';
	*m_szRenameName = '\0';
	*m_szMIMEContentType = '\0';

	int i;
	for (i = 0; i < MAX_PROPS; ++i)
	{
		m_Properties[i].name[0] = 0;
		m_Properties[i].value = 0;
		m_Properties[i].strValue = NULL;
	}

#if !defined(OS400) //*** Start AS400 Attribute API Removal *** 
	m_AttributeCollection.initializeMemory(true);
	m_AttributeCollection.reset();
#endif //*** End AS400 Attribute API Removal *** 

}


CDecObj::~CDecObj()
{
	if (m_pszName)
	{
		free(m_pszName);
		m_pszName = NULL;
	}
	
	if (m_pszName2)
	{
		free(m_pszName2);
		m_pszName2 = NULL;
	}

	CloseDataFile();

	int i;
	for (i = 0; i < MAX_PROPS; ++i)
	{
		delete [] m_Properties[i].strValue;
	}


#if !defined(OS400) //*** Start AS400 Attribute API Removal *** 
	for ( i = m_AttributeCollection.getSize(); i > 0; i-- )
	{
		IDecAttributeCollection* ptr = m_AttributeCollection[i-1];
		if ( ptr != NULL )
		{
			delete ptr;
		}
	}
#endif //*** End AS400 Attribute API Removal *** 


	if (m_pDecObjValString)
	{
		delete [] m_pDecObjValString;
	}


}


DECRESULT CDecObj::GetName(char *pszName, DWORD *pdwNameSize, DWORD *pdwCharset)
{
	DWORD	dwNameSize;
	DWORD	dwBufferSize;

	// Get the size of the caller's buffer.
	if (pdwNameSize)
		dwBufferSize = *pdwNameSize;
	else
		dwBufferSize = MAX_PATH;

	if (m_pszName && (*m_pszName != '\0'))
	{
		// Get the length of our stored name.
		dwNameSize = strlen(m_pszName) + 1;

		if (pszName)
		{
			if(dwBufferSize >= dwNameSize)
			{
				strcpy(pszName, m_pszName);
			}
			else
			{
				// The buffer isn't large enough to hold the string.
				// Return a NULL-string for the name.
				*pszName = '\0';
			}
		}
	}
	else
	{
		// There is no name.  Return a NULL-string for the name if 
		// the client provided an output name buffer.
		dwNameSize = 0;
		if (pszName && dwBufferSize > 0)
			*pszName = 0;
	}

	if (pdwNameSize)
		*pdwNameSize = dwNameSize;

	if (pdwCharset)
		*pdwCharset = m_dwNameCharset;

	return DEC_OK;
}


DECRESULT CDecObj::GetSecondaryName(char *pszName, DWORD *pdwNameSize, DWORD *pdwCharset)
{
	DWORD	dwNameSize;
	DWORD	dwBufferSize;

	// Get the size of the caller's buffer.
	if (pdwNameSize)
		dwBufferSize = *pdwNameSize;
	else
		dwBufferSize = MAX_PATH;

	if (m_pszName2 && (*m_pszName2 != '\0'))
	{
		// Get the length of our stored name.
		dwNameSize = strlen(m_pszName2) + 1;

		if (pszName)
		{
			if(dwBufferSize >= dwNameSize)
			{
				strcpy(pszName, m_pszName2);
			}
			else
			{
				// The buffer isn't large enough to hold the string.
				// Return a NULL-string for the name.
				*pszName = '\0';
			}
		}
	}
	else
	{
		// There is no name.  Return a NULL-string for the name if 
		// the client provided an output name buffer.
		dwNameSize = 0;
		if (pszName && dwBufferSize > 0)
			*pszName = '\0';
	}

	if (pdwNameSize)
		*pdwNameSize = dwNameSize;

	if (pdwCharset)
		*pdwCharset = m_dwNameCharset;

	return DEC_OK;
}


DECRESULT CDecObj::GetNameType(DWORD *dwNameType)
{
   if (dwNameType)
	  *dwNameType = m_dwNameType;

   return DEC_OK;
}


DECRESULT CDecObj::SetName(const char *szName)
{
	char	*ptr;
	size_t	len;

	if (szName == NULL)
		return DECERR_INVALID_PARAMS;

	len = strlen(szName) + 1;
	ptr = (char *)malloc(len);
	if (!ptr)
		return DECERR_OUT_OF_MEMORY;

	strcpy(ptr, szName);
	if (m_pszName)
		free(m_pszName);
	m_pszName = ptr;
	return DEC_OK;
}


DECRESULT CDecObj::SetSecondaryName(const char *szName)
{
	char	*ptr;
	size_t	len;

	if (szName == NULL)
		return DECERR_INVALID_PARAMS;

	len = strlen(szName) + 1;
	ptr = (char *)malloc(len);
	if (!ptr)
		return DECERR_OUT_OF_MEMORY;

	strcpy(ptr, szName);
	if (m_pszName2)
		free(m_pszName2);
	m_pszName2 = ptr;
	return DEC_OK;
}


DECRESULT CDecObj::SetNameType(DWORD dwNameType)
{
	m_dwNameType = dwNameType;
	return DEC_OK;
}


DECRESULT CDecObj::SetNameCharacterSet(DWORD dwCharset)
{
	if (dwCharset > DEC_CHARSET_LAST)
		return DECERR_INVALID_PARAMS;

	m_dwNameCharset = dwCharset;
	return DEC_OK;
}


DECRESULT CDecObj::GetParent(IDecContainerObject **ppParentObject)
{
	if (ppParentObject == NULL)
		return DECERR_INVALID_PARAMS;

	*ppParentObject = m_pParent;
	return DEC_OK;
}


DECRESULT CDecObj::SetParent(IDecContainerObject *pParentObject)
{
	m_pParent = pParentObject;
	return DEC_OK;
}


DECRESULT CDecObj::GetDepth(DWORD *pdwDepth)
{
	if (pdwDepth == NULL)
		return DECERR_INVALID_PARAMS;

	*pdwDepth = 0;

	IDecContainerObject* pParent;
	GetParent(&pParent);

	while (pParent != NULL)
	{
		(*pdwDepth)++;
		pParent->GetParent(&pParent);
	}

	return DEC_OK;
}


DECRESULT CDecObj::GetDataFile(char *pszDataFile, DWORD *pdwNameSize)
{
	DWORD	dwNameSize;
	DWORD	dwBufferSize;

	// Get the length of our stored name.
	dwNameSize = strlen(m_szDataFile) + 1;

	// Get the size of the caller's buffer.
	if (pdwNameSize)
		dwBufferSize = *pdwNameSize;
	else
		dwBufferSize = MAX_PATH;

	if (pszDataFile && dwBufferSize >= dwNameSize)
	{
		strcpy(pszDataFile, m_szDataFile);
	}
	else
	{
		// The buffer isn't large enough to hold the string.
		// Return a NULL-string for the name.
		*pszDataFile = '\0';
	}

	if (pdwNameSize)
		*pdwNameSize = dwNameSize;

	return DEC_OK;
}


DECRESULT CDecObj::SetDataFile(const char *szDataFile)
{

	if (szDataFile == NULL)
		return DECERR_INVALID_PARAMS;

	if (strlen(szDataFile) >= sizeof(m_szDataFile))
		return DECERR_INVALID_PARAMS;

	strcpy(m_szDataFile, szDataFile);


#if !defined(OS400) //*** Start AS400 Attribute API Removal *** 
	// Is there any attributes tied to the file yet?
	// If not, add one...
 	if ( m_AttributeCollection.getPosition() == 0 )
	{
		CGenericAttributeCollection* attributes = new CGenericAttributeCollection();
		if ( attributes != NULL )
		{
			CDecTextGeneric* nameAttr = new CDecTextGeneric(CDecAttributeKey::kPathname,
															m_szDataFile,strlen(m_szDataFile),IDecAttribute::Alias);
			if ( nameAttr != NULL )
			{
				attributes->AddAttribute(nameAttr);
				AddAttributeCollection(attributes);
			}
		}
	}
#endif //*** End AS400 Attribute API Removal *** 

	return DEC_OK;
}


DECRESULT CDecObj::GetCanDelete(bool *pbCanDelete)
{
	if (pbCanDelete == NULL)
		return DECERR_INVALID_PARAMS;

	*pbCanDelete = m_bCanDelete;
	return DEC_OK;
}


DECRESULT CDecObj::SetCanDelete(bool bCanDelete)
{
	m_bCanDelete = bCanDelete;
	return DEC_OK;
}


DECRESULT CDecObj::GetCanDeleteAncestor(bool *pbCanDelete, DWORD *pdwDepth)
{
	if (pbCanDelete == NULL || pdwDepth == NULL)
		return DECERR_INVALID_PARAMS;

	IDecContainerObject* pCurrent = NULL;
	IDecContainerObject* pLastUnDeletable = NULL;
	IDecContainerObject* pTarget = NULL;

	// Find the highest ancestor (lowest depth value) that can't be deleted.
	GetParent(&pCurrent);
	while (pCurrent != NULL)
	{
		bool bCanDelete;
		pCurrent->GetCanDelete(&bCanDelete);
		if (!bCanDelete)
			pLastUnDeletable = pCurrent;
		pCurrent->GetParent(&pCurrent);
	}

	// If there are no "can't delete" obstacles,
	// then our immediate parent (if any) is the delete target,
	// else it's the parent (if any) of the obstacle.
	if (pLastUnDeletable == NULL)
		GetParent(&pTarget);
	else
		pLastUnDeletable->GetParent(&pTarget);

	// Make sure we got a valid target.
	if (pTarget == NULL)
	{
		*pbCanDelete = false;
		*pdwDepth = 0;
	}
	else
	{
		*pbCanDelete = true;
		pTarget->GetDepth(pdwDepth);
	}

	return DEC_OK;
}


DECRESULT CDecObj::GetCanModify(bool *pbCanModify)
{
	if (pbCanModify == NULL)
		return DECERR_INVALID_PARAMS;

	*pbCanModify = m_bCanModify;
	return DEC_OK;
}


DECRESULT CDecObj::SetCanModify(bool bCanModify)
{
	m_bCanModify = bCanModify;
	return DEC_OK;
}


DECRESULT CDecObj::GetCanReplace(bool *pbCanReplace)
{
	if (pbCanReplace == NULL)
		return DECERR_INVALID_PARAMS;

	*pbCanReplace = m_bCanReplace;
	return DEC_OK;
}


DECRESULT CDecObj::SetCanReplace(bool bCanReplace)
{
	m_bCanReplace = bCanReplace;
	return DEC_OK;
}


DECRESULT CDecObj::GetCanRename(bool *pbCanRename)
{
	if (pbCanRename == NULL)
		return DECERR_INVALID_PARAMS;

	*pbCanRename = m_bCanRename;
	return DEC_OK;
}


DECRESULT CDecObj::SetCanRename(bool bCanRename)
{
	m_bCanRename = bCanRename;
	return DEC_OK;
}


DECRESULT CDecObj::GetContainerType(DWORD *pdwType)
{
	if (pdwType == NULL)
		return DECERR_INVALID_PARAMS;

	*pdwType = m_dwContainerType;
	return DEC_OK;
}


DECRESULT CDecObj::SetContainerType(DWORD dwType)
{
	m_dwContainerType = dwType;
	return DEC_OK;
}


DECRESULT CDecObj::GetItemType(DWORD *pdwType)
{
	if (pdwType == NULL)
		return DECERR_INVALID_PARAMS;

	*pdwType = m_dwItemType;
	return DEC_OK;
}


DECRESULT CDecObj::SetItemType(DWORD dwType)
{
	m_dwItemType = dwType;
	return DEC_OK;
}


DECRESULT CDecObj::GetMalformed(DWORD *pdwType)
{
	if (pdwType)
		*pdwType = m_dwMalformedType;
	return DEC_OK;
}


DECRESULT CDecObj::SetMalformed(DWORD dwType)
{
	m_dwMalformedType = dwType;
	return DEC_OK;
}


DECRESULT CDecObj::GetEngineInterface(void **ppInterface)
{
	if (ppInterface)
	{
		*ppInterface = m_pEngineInterface;
		return DEC_OK;
	}

	return DECERR_INVALID_PARAMS;
}


DECRESULT CDecObj::SetEngineInterface(void *pInterface)
{
	m_pEngineInterface = pInterface;
	return DEC_OK;
}


DECRESULT CDecObj::GetUncompressedSize(DWORDLONG *pdwlSize)
{
	if (pdwlSize == NULL)
		return DECERR_INVALID_PARAMS;

	*pdwlSize = m_dwlUncompressedSize;
	if (m_dwlUncompressedSize == DEC_CHILD_SIZE_UNKNOWN)
		return DECERR_CHILD_SIZE_UNKNOWN;
	return DEC_OK;
}


DECRESULT CDecObj::SetCompressedSize(DWORDLONG dwlSize)
{
	m_dwlCompressedSize = dwlSize;
	return DEC_OK;
}


DECRESULT CDecObj::GetCompressedSize(DWORDLONG *pdwlSize)
{
	if (pdwlSize == NULL)
		return DECERR_INVALID_PARAMS;

	*pdwlSize = m_dwlCompressedSize;
	if (m_dwlCompressedSize == DEC_CHILD_SIZE_UNKNOWN)
		return DECERR_CHILD_SIZE_UNKNOWN;
	return DEC_OK;
}


DECRESULT CDecObj::SetUncompressedSize(DWORDLONG dwlSize)
{
	m_dwlUncompressedSize = dwlSize;
	return DEC_OK;
}

int CDecObj::FindProperty(const char *szName)
{
	int i;

	for (i = 0; i < MAX_PROPS; ++i)
	{
		if (strcmp(m_Properties[i].name, szName) == 0)
		{
			break;
		}
	}

	return i;
}

int CDecObj::FindAvailableProperty(const char *szName)
{
	int i = FindProperty(szName);
	if (i >= MAX_PROPS)
	{
		for (i = 0; i < MAX_PROPS; ++i)
		{
			if (m_Properties[i].name[0] == 0)
			{
				strcpy(m_Properties[i].name, szName);
				break;
			}
		}
	}

	return i;
}

DECRESULT CDecObj::GetProperty(const char *szName, DWORD *pValue)
{
	if (pValue == NULL)
	{
		dec_assert(0);
		return DECERR_INVALID_PARAMS;
	}

	return GetProperty(szName, *pValue);

}

DECRESULT CDecObj::GetProperty(const char *szName, DWORD &value)
{
	DECRESULT result = DECERR_GENERIC;

	if (szName == NULL)
	{
		dec_assert(0);
		return DECERR_INVALID_PARAMS;
	}

	if (strlen(szName) >= MAX_PROP_NAME_LEN)
	{
		dec_assert(0);
		return DECERR_INVALID_PARAMS;
	}

	value = 0;

	int i = FindProperty(szName);
	if (i < MAX_PROPS)
	{
		value = m_Properties[i].value;
		result = DEC_OK;
	}

	return result;
}

DECRESULT CDecObj::GetProperty(const char *szName, const char *&szValue)
{
	DECRESULT result = DECERR_GENERIC;

	if (szName == NULL)
	{
		dec_assert(0);
		return DECERR_INVALID_PARAMS;
	}

	if (strlen(szName) >= MAX_PROP_NAME_LEN)
	{
		dec_assert(0);
		return DECERR_INVALID_PARAMS;
	}

	szValue = NULL;

	int i = FindProperty(szName);
	if (i < MAX_PROPS)
	{
		szValue = m_Properties[i].strValue;
		result = DEC_OK;
	}

	return result;
}


DECRESULT CDecObj::SetProperty(const char *szName, DWORD value)
{
	DECRESULT result = DECERR_GENERIC;
	
	if (szName == NULL)
	{
		dec_assert(0);
		return DECERR_INVALID_PARAMS;
	}

	if (strlen(szName) >= MAX_PROP_NAME_LEN)
	{
		dec_assert(0);
		return DECERR_INVALID_PARAMS;
	}

	int found = FindAvailableProperty(szName);
	if (found < MAX_PROPS)
	{
		m_Properties[found].value = value;
		result = DEC_OK;
	}

	return result;
}

DECRESULT CDecObj::SetProperty(const char *szName, const char *szValue)
{
	DECRESULT result = DECERR_GENERIC;
	
	if (szName == NULL)
	{
		dec_assert(0);
		return DECERR_INVALID_PARAMS;
	}

	if (strlen(szName) >= MAX_PROP_NAME_LEN)
	{
		dec_assert(0);
		return DECERR_INVALID_PARAMS;
	}

	int found = FindAvailableProperty(szName);
	if (found < MAX_PROPS)
	{
		delete [] m_Properties[found].strValue;
		m_Properties[found].strValue = new char [strlen(szValue)+1];

		strcpy(m_Properties[found].strValue, szValue);
		result = DEC_OK;
	}

	return result;
}

DECRESULT CDecObj::RemoveProperty(const char *szName)
{
	if (szName == NULL)
	{
		dec_assert(0);
		return DECERR_INVALID_PARAMS;
	}

	if (strlen(szName) >= MAX_PROP_NAME_LEN)
	{
		dec_assert(0);
		return DECERR_INVALID_PARAMS;
	}

	int i = FindProperty(szName);
	if (i < MAX_PROPS)
	{
		m_Properties[i].name[0] = 0;
		m_Properties[i].value = 0;

		delete [] m_Properties[i].strValue;
		m_Properties[i].strValue = NULL;
	}

	return DEC_OK;
}


// ===================================================================
// GetNormalizedName()
//
// This function maps a DEC_CHARSET_xxx value to either ANSI or OEM
// and performs a conversion on the input string based on which
// character set (ANSI or OEM) the user wants.
//
// Input:
//     pszInput   - pointer to input string
//     pszOutput  - pointer to output buffer (assumed to be large enough
//                  to hold the given input)
//     bANSI      - If true, output string will be ANSI
//                  If false, output string will be OEM
//
// Output:
//     pszOutput  - filled with the (possibly) converted string
//
// ===================================================================
DECRESULT CDecObj::GetNormalizedName (char *pszInput, char *pszOutput, DWORD dwOutputSize, bool bANSI)
{
#if defined(UNIX) || defined(SYM_NLM)
	// *** DEBUG ***
	// This function should be ported at some point...
	// For now, no translation.
	strcpy (pszOutput, pszInput);
	return DEC_OK;
#else
	DWORD dwCharset;

	if (!pszInput || !pszOutput)
		return DECERR_INVALID_PARAMS;

	// Get the character set used for the filename associated
	// with this object.
	GetName(NULL, NULL, &dwCharset);

	if (bANSI)
	{
		// OK, caller wants the output string to be ANSI.
		switch (dwCharset)
		{
			case DEC_CHARSET_CP850:
				// Input string is assumed to be OEM.
				// Translation is needed.
				OemToCharBuff (pszInput,
							  pszOutput,
							  dwOutputSize);
				break;

			default:
				// No translation needed.
				strcpy (pszOutput, pszInput);
				break;
		}
	}
	else
	{
		// OK, caller wants the output string to be OEM.
		switch (dwCharset)
		{
			case DEC_CHARSET_US_ASCII:
			case DEC_CHARSET_CP850:
				// Input string is assumed to already be OEM.
				// No translation needed.
				strcpy (pszOutput, pszInput);
				break;

			default:
				// Input string is assumed to be ANSI.
				// Translation is needed.
				CharToOemBuff (pszInput,
							  pszOutput,
							  dwOutputSize);
				break;
		}
	}

	return DEC_OK;
#endif	// UNIX
}


DECRESULT CDecObj::GetRenameItemName(char **ppszName)
{
	if (ppszName == NULL)
		return DECERR_INVALID_PARAMS;

	*ppszName = &m_szRenameName[0];
	return DEC_OK;
}


#if !defined(OS400) //*** Start AS400 Attribute API Removal *** 
IDecObject::AttributeState CDecObj::GetAttributeState() const
{
	return m_eAttributeState;  
}

void  CDecObj::SetAttributesSupported()
{
	m_eAttributeSupport = eEngineSupport;
}

IDecObject::AttributeSupport CDecObj::GetAttributeSupport() const
{
	return m_eAttributeSupport;
}

void CDecObj::FinalizeAttributes()
{
	m_eAttributeState = eInitialized;
}

IDecAttributeCollection* CDecObj::GetAttributeCollectionAt(size_t index) const
{
	CDecObj* nonConst = (CDecObj*)this;

	if ( index < m_AttributeCollection.getPosition() )
	{
		 return nonConst->m_AttributeCollection[index];
	}
	else
	{
		return NULL;
	}
}

size_t CDecObj::GetNumberOfAttributeCollections() const
{
	return m_AttributeCollection.getPosition();
}

void CDecObj::AddAttributeCollection(IDecAttributeCollection* pAttrCollection)
{
	if ( pAttrCollection != NULL )
	{
		m_eAttributeState = ePartiallyInitialzed;
		m_AttributeCollection[m_AttributeCollection.getPosition()] = pAttrCollection;
		m_AttributeCollection++;
	}
}
#endif //*** End AS400 Attribute API Removal *** 

DECRESULT CDecObj::SetRenameItemName(const char *pszName, bool bSetParent)
{
	DECRESULT hr = DEC_OK;
	IDecContainerObject *pParentObject = NULL;

	if (pszName == NULL)
	{
		*m_szRenameName = '\0';

		if (bSetParent)
		{
			// Copy the new name (if any) into this object's parent object
			// so that the parent engine can apply the new name.
			hr = GetParent(&pParentObject);
			if (hr == DEC_OK && pParentObject)
				hr = pParentObject->SetRenameItemName(pszName, false);
		}
		return DEC_OK;
	}

	if (strlen(pszName) >= sizeof(m_szRenameName))
		return DECERR_INVALID_PARAMS;

	if (bSetParent)
	{
		// Copy the new name (if any) into this object's parent object
		// so that the parent engine can apply the new name.
		hr = GetParent(&pParentObject);
		if (hr == DEC_OK && pParentObject)
			hr = pParentObject->SetRenameItemName(pszName, false);
	}

	strcpy(m_szRenameName, pszName);
	return (hr);
}


DECRESULT CDecObj::GetMIMEItemContentType(char **ppszType)
{
	if (ppszType == NULL)
		return DECERR_INVALID_PARAMS;

	*ppszType = &m_szMIMEContentType[0];
	return DEC_OK;
}


DECRESULT CDecObj::SetMIMEItemContentType(const char *pszType, bool bSetParent)
{
	DECRESULT hr = DEC_OK;
	IDecContainerObject *pParentObject = NULL;

	if (pszType == NULL)
	{
		*m_szMIMEContentType = '\0';

		if (bSetParent)
		{
			// Copy the type (if any) into this object's parent object
			// so that the parent engine can apply it.
			hr = GetParent(&pParentObject);
			if (hr == DEC_OK && pParentObject)
				hr = pParentObject->SetMIMEItemContentType(pszType, false);
		}
		return DEC_OK;
	}

	if (strlen(pszType) >= sizeof(m_szMIMEContentType))
		return DECERR_INVALID_PARAMS;

	if (bSetParent)
	{
		// Copy the type (if any) into this object's parent object
		// so that the parent engine can apply it.
		hr = GetParent(&pParentObject);
		if (hr == DEC_OK && pParentObject)
			hr = pParentObject->SetMIMEItemContentType(pszType, false);
	}

	strcpy(m_szMIMEContentType, pszType);
	return (hr);
}


DECRESULT CDecObj::GetAttributes(DWORD *pdwAttrs, DWORD *pdwAttrType)
{
	if (pdwAttrs)
		*pdwAttrs = m_dwAttrs;
	if (pdwAttrType)
		*pdwAttrType = m_dwAttrType;
	return DEC_OK;
}


DECRESULT CDecObj::SetAttributes(DWORD dwAttrs, DWORD dwAttrType)
{
	m_dwAttrs = dwAttrs;
	m_dwAttrType = dwAttrType;
	return DEC_OK;
}

DECRESULT CDecObj::GetTime(DWORD *pdwHour, DWORD *pdwMinute, DWORD *pdwSecond, DWORD *pdwMillisecond)
{
	if (pdwHour)
		*pdwHour = m_dwHour;
	if (pdwMinute)
		*pdwMinute = m_dwMinute;
	if (pdwSecond)
		*pdwSecond = m_dwSecond;
	if (pdwMillisecond)
		*pdwMillisecond = m_dwMillisecond;
	return DEC_OK;
}


DECRESULT CDecObj::SetTime(DWORD dwHour, DWORD dwMinute, DWORD dwSecond, DWORD dwMillisecond)
{
	if (dwHour > 24 ||
		dwMinute > 59 ||
		dwSecond > 59 ||
		dwMillisecond > 999)
	{
		return DECERR_INVALID_PARAMS;
	}

	m_dwHour = dwHour;
	m_dwMinute = dwMinute;
	m_dwSecond = dwSecond;
	m_dwMillisecond = dwMillisecond;
	return DEC_OK;
}


DECRESULT CDecObj::GetDate(DWORD *pdwYear, DWORD *pdwMonth, DWORD *pdwDay)
{
	if (pdwYear)
		*pdwYear = m_dwYear;
	if (pdwMonth)
		*pdwMonth = m_dwMonth;
	if (pdwDay)
		*pdwDay = m_dwDay;
	return DEC_OK;
}


DECRESULT CDecObj::SetDate(DWORD dwYear, DWORD dwMonth, DWORD dwDay)
{
	if (dwMonth < 1 ||
		dwMonth > 12 ||
		dwDay < 1 ||
		dwDay > 31)
	{
		return DECERR_INVALID_PARAMS;
	}

	m_dwYear = dwYear;
	m_dwMonth = dwMonth;
	m_dwDay = dwDay;
	return DEC_OK;
}


FILE *CDecObj::OpenDataFile(const char *pszFilename, 
							const char *pszMode, 
							BYTE **ppbyBuffer, 
							size_t *pSizeOfBuffer, 
							long *plFileSize)
{
	//
	// Validate required parameters
	//
	dec_assert(pszFilename);
	dec_assert(pszMode);

	//
	// If we've already opened the file, just return the open file pointer
	// and any requested parameters.
	//
	if (m_pCachedFile)
	{
		if (ppbyBuffer)
			*ppbyBuffer = m_pbyCachedBuffer;

		if (pSizeOfBuffer)
			*pSizeOfBuffer = m_sizeCachedBufferSize;        
		
		if (plFileSize)
			*plFileSize = m_lCachedFileSize;        

		//
		// Seek back to the start of the file
		//
		if (0 == dec_fseek(m_pCachedFile, 0, SEEK_SET))
			return m_pCachedFile;

		//
		// Error state
		//
		return NULL;
	}
	
	else
	{
		//
		// Open the file
		//
		m_pCachedFile = dec_fopen(pszFilename, pszMode);
		if (m_pCachedFile)
		{
			//
			// Get the file size
			//
			if (0 == dec_fseek(m_pCachedFile, 0, SEEK_END))
			{
				size_t nSizeOfBuffer;

				//
				// Memory optimization - if the file size is less than SIZE_INITIAL_BUFFER_READ,
				// then we don't need to malloc anything larger than the file size.
				//
				m_lCachedFileSize = dec_ftell(m_pCachedFile);
				if (0 != dec_fseek(m_pCachedFile, 0, SEEK_SET))
					goto bailOut;

				nSizeOfBuffer = (m_lCachedFileSize < SIZE_INITIAL_BUFFER_READ) ? m_lCachedFileSize : SIZE_INITIAL_BUFFER_READ;
				if (nSizeOfBuffer < 128)
					nSizeOfBuffer = 128;  // This is here just to avoid doing a 0-byte read below.

				dec_assert(nSizeOfBuffer <= SIZE_INITIAL_BUFFER_READ);

				//
				// CLB - 3/14/02
				// Adding 1 byte to the allocation.  This is for the case of zero
				// byte files. Calling malloc(0) results in OS specific behavior
				// (POSIX doesn't define what should happen here).  It works fine
				// except for AS400 and AIX.
				//
				m_pbyCachedBuffer = (BYTE *)malloc(nSizeOfBuffer + 1);
				if (m_pbyCachedBuffer)
				{
					//
					// Read an initial buffer of data
					//
					m_sizeCachedBufferSize = dec_fread(m_pbyCachedBuffer, 
													  sizeof(BYTE), 
													  nSizeOfBuffer, 
													  m_pCachedFile);
					//
					// Reset the file pointer to the start of the file.
					//
					if (0 == dec_fseek(m_pCachedFile, 0, SEEK_SET))
					{
						//
						// Make sure the read succeeded...
						//
						if (nSizeOfBuffer != m_sizeCachedBufferSize)
						{
							//
							// If an error occurred, then bail out...
							//
							if (0 != dec_ferror(m_pCachedFile))
							{
								goto bailOut;
							}
						}

						//
						// Save the requested out params and return the open file pointer
						//
						if (ppbyBuffer)
							*ppbyBuffer = m_pbyCachedBuffer;

						if (pSizeOfBuffer)
							*pSizeOfBuffer = m_sizeCachedBufferSize;        

						if (plFileSize)
							*plFileSize = m_lCachedFileSize;        

						return m_pCachedFile;
					}
				}
			}
		}
	}

bailOut:
	//
	// If we get here, then something bad happened.  Clean up
	// anything we've done so far.  No need to check return of 
	// CloseDataFile since we're already in an error state here.
	//
	CloseDataFile();

	if (ppbyBuffer)
		*ppbyBuffer = NULL;

	if (pSizeOfBuffer)
		*pSizeOfBuffer = 0;        

	if (plFileSize)
		*plFileSize = 0;        

	return NULL;
}


int CDecObj::CloseDataFile()
{
	int nReturn = 0;

	//
	// Cleanup
	//
	if (m_pCachedFile)
	{
		nReturn = dec_fclose(m_pCachedFile);
		m_pCachedFile = NULL;
	}

	if (m_pbyCachedBuffer)
	{
		free(m_pbyCachedBuffer);
		m_pbyCachedBuffer = NULL;
	}

	m_sizeCachedBufferSize = 0;	  
	m_lCachedFileSize = 0;

	return nReturn;
}

#if !defined(OS400) //*** Start AS400 Attribute API Removal *** 
const char* CDecObj::GetAttrStateEnum(DWORD type)
{
	switch (type)
	{
		case eUninitialized			: return "Uninitialized";
		case eInitialized			: return "Initialized";
		case ePartiallyInitialzed	: return "Partially Initialzed";
		default						: return "(invalid)";
	}
};

const char* CDecObj::GetAttrSupportEnum(DWORD type)
{
	switch (type)
	{
		case eBaseSupport	: return "Base Only";
		case eEngineSupport	: return "Full Support";
		default				: return "(invalid)";
	}
};
#endif //*** End AS400 Attribute API Removal *** 

const char* CDecObj::ToString(unsigned int dwLevel)
{
	delete [] m_pDecObjValString;	// delete any previously allocated string

	size_t allocationAmount = 1500;	// initial size of our output, without strings
	allocationAmount += ( m_pszName == NULL ? 0 : strlen(m_pszName)+1 );
	allocationAmount += ( m_pszName2 == NULL ? 0 : strlen(m_pszName2)+1 );
	allocationAmount += ( m_szDataFile == NULL ? 0 : strlen(m_szDataFile)+1 );
	allocationAmount += ( m_szRenameName == NULL ? 0 : strlen(m_szRenameName)+1 );
	allocationAmount += ( m_szMIMEContentType == NULL ? 0 : strlen(m_szMIMEContentType)+1 );

	m_pDecObjValString = new char[allocationAmount];

	if (m_pDecObjValString)
	{
        *m_pDecObjValString='\0';

		switch (dwLevel & 0x03)
		{
			case 3: 		// case fall-through is on-purpose; denotes level of output
				sssnprintfappend(m_pDecObjValString, allocationAmount, "m_dwYear:\t%d\t", m_dwYear);
				sssnprintfappend(m_pDecObjValString, allocationAmount, "m_dwMonth:\t%d\t", m_dwMonth);
				sssnprintfappend(m_pDecObjValString, allocationAmount, "m_dwDay:\t%d\t", m_dwDay);
				sssnprintfappend(m_pDecObjValString, allocationAmount, "m_dwHour:\t%d\t", m_dwHour);
				sssnprintfappend(m_pDecObjValString, allocationAmount, "m_dwMinute:\t%d\t", m_dwMinute);
				sssnprintfappend(m_pDecObjValString, allocationAmount, "m_dwSecond:\t%d\t", m_dwSecond);
				sssnprintfappend(m_pDecObjValString, allocationAmount, "m_dwMillisecond:\t%d\t", m_dwMillisecond);
				sssnprintfappend(m_pDecObjValString, allocationAmount, "m_dwAttrs:\t%d\t", m_dwAttrs);
				sssnprintfappend(m_pDecObjValString, allocationAmount, "m_dwAttrType:\t%d\t", m_dwAttrType);
				sssnprintfappend(m_pDecObjValString, allocationAmount, "m_pbyCachedBuffer:\t%08X\t", m_pbyCachedBuffer);
				sssnprintfappend(m_pDecObjValString, allocationAmount, "m_sizeCachedBufferSize:\t%d\t", m_sizeCachedBufferSize);
				sssnprintfappend(m_pDecObjValString, allocationAmount, "m_lCachedFileSize:\t%d\t", m_lCachedFileSize);
				sssnprintfappend(m_pDecObjValString, allocationAmount, "m_pEngineInterface:\t%08X\t", m_pEngineInterface);

			case 2:
				sssnprintfappend(m_pDecObjValString, allocationAmount, "m_bCanDelete:\t%s\t", ( m_bCanDelete ? "True" : "False"));
				sssnprintfappend(m_pDecObjValString, allocationAmount, "m_bCanModify:\t%s\t", ( m_bCanModify ? "True" : "False"));
				sssnprintfappend(m_pDecObjValString, allocationAmount, "m_bCanReplace:\t%s\t", ( m_bCanReplace ? "True" : "False"));
				sssnprintfappend(m_pDecObjValString, allocationAmount, "m_bCanRename:\t%s\t", ( m_bCanRename ? "True" : "False"));
				sssnprintfappend(m_pDecObjValString, allocationAmount, "m_dwNameCharset:\t%d\t", m_dwNameCharset);
				sssnprintfappend(m_pDecObjValString, allocationAmount, "m_dwNameType:\t%d\t", m_dwNameType);
#if !defined(OS400) //*** Start AS400 Attribute API Removal *** 
				sssnprintfappend(m_pDecObjValString, allocationAmount, "m_eAttributeState:\t%s\t", GetAttrStateEnum(m_eAttributeState));
				sssnprintfappend(m_pDecObjValString, allocationAmount, "m_eAttributeSupport:\t%s\t", GetAttrSupportEnum(m_eAttributeSupport));
				sssnprintfappend(m_pDecObjValString, allocationAmount, "m_AttributeCollection.size:\t%d\t", m_AttributeCollection.getSize());
#endif //*** End AS400 Attribute API Removal *** 

			case 1:	
				sssnprintfappend(m_pDecObjValString, allocationAmount, "m_pszName:\t%s\t", (m_pszName == NULL ? "(null)" : m_pszName));
				sssnprintfappend(m_pDecObjValString, allocationAmount, "m_pszName2:\t%s\t", (m_pszName2 == NULL ? "(null)" : m_pszName2));
				sssnprintfappend(m_pDecObjValString, allocationAmount, "m_szDataFile:\t%s\t", (m_szDataFile == NULL ? "(null)" : m_szDataFile));
				sssnprintfappend(m_pDecObjValString, allocationAmount, "m_szRenameName:\t%s\t", (m_szRenameName == NULL ? "(null)" : m_szRenameName));
				sssnprintfappend(m_pDecObjValString, allocationAmount, "m_szMIMEContentType:\t%s\t", (m_szMIMEContentType == NULL ? "(null)" : m_szMIMEContentType));

				sssnprintfappend(m_pDecObjValString, allocationAmount, "m_dwMalformedType:\t%d\t", m_dwMalformedType);
				sssnprintfappend(m_pDecObjValString, allocationAmount, "m_dwContainerType:\t%d\t", m_dwContainerType);
				sssnprintfappend(m_pDecObjValString, allocationAmount, "m_dwItemType:\t%d\t", m_dwItemType);
				sssnprintfappend(m_pDecObjValString, allocationAmount, "m_dwlUncompressedSize:\t%d\t", m_dwlUncompressedSize);
				sssnprintfappend(m_pDecObjValString, allocationAmount, "m_dwlCompressedSize:\t%d\t", m_dwlCompressedSize);
				sssnprintfappend(m_pDecObjValString, allocationAmount, "m_pParent:\t%08X\t", m_pParent);
				sssnprintfappend(m_pDecObjValString, allocationAmount, "m_pCachedFile:\t%08X\t", m_pCachedFile);

			default:
				break;
		}
	}

	return m_pDecObjValString;
}

/////////////////////////////////////////////////////////////////////////////
// CDecEngine Methods

CDecEngine::CDecEngine()
{
	*m_szTempDir = '\0';

#ifdef WIN32
	// Increase the number of files that may
	// be open simultaneously at the stdio level
	// to the maximum possible.  The default is
	// 512, which was reached in our stress tests.
	_setmaxstdio(2048);
#endif
}


CDecEngine::~CDecEngine()
{
}


DECRESULT CDecEngine::SetTempDir(const char *szDir)
{
	DECRESULT hr = DEC_OK;

	// If caller wants us to use the system temp dir...
	if (szDir == NULL)
	{
		// If can't get the Windows temp dir, use the current dir.
		if (Support_GetTempPath(sizeof(m_szTempDir), m_szTempDir) == 0)
			*m_szTempDir = '\0';
	}
	else
	{
		// Else use the dir provided.
		strcpy(m_szTempDir, szDir);
	}

	return hr;
}


DECRESULT CDecEngine::GetTempDir(char *pszDir)
{
	if (pszDir == NULL)
		return DECERR_INVALID_PARAMS;

	strcpy(pszDir, m_szTempDir);
	return DEC_OK;
}


DECRESULT CDecEngine::CreateTempFile(const char *szExtension, char *pszFile, IDecomposerEx *pDecomposer)
{
	// Create a temporary file with the specified extension.
	// The extension must have a period, e.g. ".EXE", unless it is empty.
	if (pszFile == NULL)
		return DECERR_INVALID_PARAMS;

#ifdef DEC_REUSE_TEMP_FILES
	return (dec_get_tempname(m_szTempDir, szExtension, pszFile) == 0) ? 
		DEC_OK : DECERR_TEMPFILE_CREATE;
#else
	// If we get a name collision, retry up to 5 times.
	for (int i = 0; i < 5; i++)
	{
		// Get the next temporary filename and add the specified extension.
		GetNextTempFileName(pszFile, pDecomposer);

		if (szExtension)
		{
			char	szExt[MAX_PATH];
			size_t	ulen1;
			size_t	ulen2;

			// Make a temporary copy of the extension so that we can modify it.
			memset (szExt, 0, sizeof (szExt));
			strncpy (szExt, szExtension, MAX_PATH - 1);

			// Modify the extension such that it is ALWAYS entirely low-ASCII.
			MakeValidLowASCIIWindowsFilename (szExt);

			// Limit the extension to no more than 12 bytes
			ulen1 = strlen(szExt);
#if defined (SYM_NLM)
			if (ulen1 > 4)
			{
				szExt[4] = '\0';
			}
			else if ( ulen1 == 1 )
			{
				szExt[0] = '\0';
			}
#else
			if (ulen1 > 12)
				szExt[12] = '\0';
#endif

			// Compute the maximum number of bytes that we can append to pszFile.
			ulen1 = strlen(pszFile);
			if (ulen1 > MAX_PATH)
				ulen2 = 0;
			else
				ulen2 = MAX_PATH - ulen1;

			// Concatenate no more than we can hold in pszFile.
			strncat(pszFile, szExt, ulen2);
		}

		// Try to create the file.
		// If successful, return immediately.
		bool bFileExists;
		DECRESULT hr = CreateTempFile(pszFile, &bFileExists);
		if (SUCCEEDED(hr))
			return DEC_OK;

		// If error other than "name already exists",
		// give up immediately.
		if (!bFileExists)
			return DECERR_TEMPFILE_CREATE;

#if defined(_DEBUG) && !defined(UNIX)
		char szTemp[MAX_PATH];
		sssnprintf(szTemp, sizeof(szTemp), "Name collision: %s\r\n", pszFile);
		OutputDebugString(szTemp);
#endif
	}

	return DECERR_TEMPFILE_CREATE;
#endif
}


DECRESULT CDecEngine::CreateTempFile(const char *szFile, bool *pbFileExists)
{
	FILE *pfile;

	if (szFile == NULL || pbFileExists == NULL)
		return DECERR_INVALID_PARAMS;

	// Create the specified file, but fail if the name already exists.
	if (dec_access(szFile, 0) == 0)
	{
		*pbFileExists = true;
		return DECERR_TEMPFILE_CREATE;
	}

	pfile = dec_fopen(szFile, "w+b");
	if (!pfile)
	{
		*pbFileExists = false;
		return DECERR_TEMPFILE_CREATE;
	}

	*pbFileExists = true;
	
	if(EOF == dec_fclose(pfile))
	{
		return DECERR_TEMPFILE_CLOSE;
	}

	pfile = NULL;
	return DEC_OK;
}


DECRESULT CDecEngine::GetNextTempFileName(char *pszName, IDecomposerEx *pDecomposer)
{
	const char *pszLastBackslash;
	char	szBetween[2];

	if (pszName == NULL)
		return DECERR_INVALID_PARAMS;

	// If the directory is non-empty and doesn't end in a backslash,
	// we need to add one.
	pszLastBackslash = strrchr(m_szTempDir, '/');
	if (!pszLastBackslash)
		pszLastBackslash = strrchr(m_szTempDir, '\\');

	szBetween[0] = '\0';
	if (m_szTempDir[0] && (pszLastBackslash == NULL || pszLastBackslash[1]))
#if defined(UNIX)
		strcpy(szBetween, "/");
#else
		strcpy(szBetween, "\\");
#endif

	// Build a string of the form "tempdir\xxxxxxxx" or just "xxxxxxxx".
	sprintf(pszName, "%s%s%08X",
			m_szTempDir,
			szBetween,
			pDecomposer->GetUniqueValue());

	return DEC_OK;
}


DECRESULT CDecEngine::SetMaxExtractSize(DWORD dwMaxSize)
{
	// If an engine does not implement this function, then this is
	// the default behavior.
	return DEC_OK;
}


DECRESULT CDecEngine::AbortProcess(bool bAbort)
{
	// If an engine does not implement this function, then this is
	// the default behavior.
	return DEC_OK;
}


DECRESULT CDecEngine::SetIOCallback(IDecIOCB *pIOCallback)
{
	// If an engine does not implement this function, then this is
	// the default behavior.
	return DEC_OK;
}


DECRESULT CDecEngine::FilterObject(IDecContainerObject *pObject, DWORD types[], bool *pbContinue)
{
	return (FilterObjectByType(pObject, types, pbContinue));
}


DECRESULT CDecEngine::FilterObjectByType(IDecContainerObject* pObject, DWORD types[], bool* pbContinue)
{
	if (pObject == NULL || types == NULL || pbContinue == NULL)
		return DECERR_INVALID_PARAMS;

	// If the object has a type property...
	DWORD dwType;
	if (SUCCEEDED(pObject->GetProperty(DEC_PROPERTY_GUESSTYPE, dwType)))
	{
		// Assume the engine will not need to process the object.
		*pbContinue = false;

		// If the object's type is in the engine's support type list,
		// then the engine will need to process the object.
		for (int i = 0; types[i] != 0; i++)
		{
			if (dwType == types[i])
			{
				*pbContinue = true;
				break;
			}
		}
	}
	else
	{
		// The engine will need to process the object.
		*pbContinue = true;
	}

	return DEC_OK;
}


DECRESULT CDecEngine::FindExtension(const char *szName, const char **ppszExtension)
{
	if (szName == NULL || ppszExtension == NULL)
		return DECERR_INVALID_PARAMS;

	// Get a pointer to the rightmost dot
	// that does not have any backslashes to the right of it,
	// or NULL if there is no such dot.
	const char *pszExtension = strrchr(szName, '.');
	if (pszExtension != NULL)
		if (strchr(pszExtension, '\\') != NULL || strchr(pszExtension, '/') != NULL)
			pszExtension = NULL;

	*ppszExtension = pszExtension;

	return (pszExtension) ? DEC_OK : DECERR_GENERIC;
}


DECRESULT CDecEngine::FindNativeExtension(const char *szName, DWORD dwNameType, char *pszDst)
{
	size_t	nLen = 0;

	// *** NOTE ***
	// This function assumes the input buffer pointed to by pszDst
	// is at least MAX_PATH in size.
	// ************
	// Leave now if parameters are not valid
	if (szName == NULL || pszDst == NULL)
		return DECERR_INVALID_PARAMS;
   
	// initialize OUT-parameter
	pszDst[0] = 0;
   
	// used during string walking code
	const char chDot = (dwNameType == DEC_NAMETYPE_EXTERNAL) ? ASC_CHR_PERIOD : '.';
	const char chFwd = (dwNameType == DEC_NAMETYPE_EXTERNAL) ? ASC_CHR_FSLASH : '/';
	const char chBck = (dwNameType == DEC_NAMETYPE_EXTERNAL) ? ASC_CHR_BSLASH : '\\';
   
	// find the dot, then ensure no fwd or bck seperators are after it.
	char *pszDot = strrchr(szName, chDot);
	if (pszDot != NULL && strchr(pszDot, chFwd) == NULL && strchr(pszDot, chBck) == NULL)
	{
		// extension is copied regardless
		nLen = strlen(pszDot);
		if (nLen >= MAX_PATH)
			nLen = MAX_PATH - 1;
		strncpy(pszDst, pszDot, nLen);
		pszDst[nLen] = 0;
	 
		// if external, then ASCII is assumed as the encoding and we use
		// character conversion where appropriate to get the native. Note: the
		// code below degenerates to virtually nothing quickly on ASCII platforms
		// with even half-witted optimizing compilers
		if (dwNameType == DEC_NAMETYPE_EXTERNAL)
		{
			static const char szExternal[] =
			{
				// upper case characters
				ASC_CHR_A, ASC_CHR_B, ASC_CHR_C, ASC_CHR_D, ASC_CHR_E, ASC_CHR_F, ASC_CHR_G, 
				ASC_CHR_H, ASC_CHR_I, ASC_CHR_J, ASC_CHR_K, ASC_CHR_L, ASC_CHR_M, ASC_CHR_N, 
				ASC_CHR_O, ASC_CHR_P, ASC_CHR_Q, ASC_CHR_R, ASC_CHR_S, ASC_CHR_T, ASC_CHR_U,
				ASC_CHR_V, ASC_CHR_W, ASC_CHR_X, ASC_CHR_Y, ASC_CHR_Z,
			
				// lower case characters
				ASC_CHR_a, ASC_CHR_b, ASC_CHR_c, ASC_CHR_d, ASC_CHR_e, ASC_CHR_f, ASC_CHR_g,
				ASC_CHR_h, ASC_CHR_i, ASC_CHR_j, ASC_CHR_k, ASC_CHR_l, ASC_CHR_m, ASC_CHR_n,
				ASC_CHR_o, ASC_CHR_p, ASC_CHR_q, ASC_CHR_r, ASC_CHR_s, ASC_CHR_t, ASC_CHR_u,
				ASC_CHR_v, ASC_CHR_w, ASC_CHR_x, ASC_CHR_y, ASC_CHR_z,
			
				// numbers
				ASC_CHR_0, ASC_CHR_1, ASC_CHR_2, ASC_CHR_3, ASC_CHR_4, ASC_CHR_5,
				ASC_CHR_6, ASC_CHR_7, ASC_CHR_8, ASC_CHR_9,
			
				// null terminator
				0
			};
		
			static const char szNative[] =
			{
				// upper case characters
				'A', 'B', 'C', 'D', 'E', 'F', 'G',
				'H', 'I', 'J', 'K', 'L', 'M', 'N',
				'O', 'P', 'Q', 'R', 'S', 'T', 'U',
				'V', 'W', 'X', 'Y', 'Z',
			
				// lower case characters
				'a', 'b', 'c', 'd', 'e', 'f', 'g', 
				'h', 'i', 'j', 'k', 'l', 'm', 'n',
				'o', 'p', 'q', 'r', 's', 't', 'u',
				'v', 'w', 'x', 'y', 'z',
			
				// numbers
				'0', '1', '2', '3', '4', '5',
				'6','7', '8', '9',
			
				// null terminator
				0
			};
		
			// hard-assign dot
			*pszDst++ = '.';

			// convert each char byte-per-byte
			char *pszNext = Support_CharNext(pszDst);
			while (*pszDst != 0)
			{
				// find the char in the external map
				const char *pszChar = strchr(szExternal, *pszDst);
				*pszDst = (pszChar != NULL) ? szNative[ (pszChar - szExternal) / sizeof(char) ] : '_';
			
				// advance to next 
				pszDst = pszNext;
				pszNext = Support_CharNext(pszNext);
			}
		}
	}

	return DEC_OK;
}


// NOTE: The input filename is assumed to be ANSI!
DECRESULT CDecEngine::MakeValidLowASCIIWindowsFilename (char *pszFilename)
{
/*
	BYTE	*pStr;
	BYTE	ch;

	if (!pszFilename)
		return (DECERR_GENERIC);

	// Set pointer to start of input filename string.
	pStr = (BYTE *)pszFilename;
	for (;;)
	{
		ch = *pStr;
		if (ch == 0)
			break;

		if (ch < 32)
			goto do_replace;

		if (ch == '<' ||
			ch == '>' ||
			ch == ':' ||
			ch == '"' ||
			ch == '/' ||
			ch == '\\' ||
			ch == '|')
			goto do_replace;

		if (ch >= 128)
			goto do_replace;

		goto skip_replace;

do_replace:
		*pStr = '_';

skip_replace:
		pStr++;
	}

	return (DEC_OK);
*/

   static const char *szValid = "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
								"abcdefghijklmnopqrstuvwxyz"
								"0123456789"
								"~_.";
   DECRESULT hr = DEC_OK;

   if (pszFilename != NULL)
   {
	  char *pszTmp = pszFilename;
	  while (*pszTmp != 0)
	  {
		  if (strchr(szValid, *pszTmp) == NULL)
			  *pszTmp = '_';
		  pszTmp++;
	  }
   }
   else
   {
	  // invalid filename reference
	  hr = DECERR_INVALID_PARAMS;
   }

   return hr;
}
