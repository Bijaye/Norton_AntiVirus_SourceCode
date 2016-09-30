//------------------------------------------------------------------------
// DecUnicode.cpp
//
// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright (c) Symantec, Corp. 2000, 2005. All rights reserved.
//------------------------------------------------------------------------

// Fix the defect in IBM's header files for V3.x of Visual Age for C++.
#include "fixibmps.h"

// Project headers
#define DAPI_CPP
#include "DecPlat.h"
#include "DAPI.h"

CUNIMO *g_pUNIMOListHead;
CUNIMO *g_pUNIMOListTail;

#if defined(_WINDOWS)
CRITICAL_SECTION	g_UNIMOcritsect;
#endif


DECSDKLINKAGE void STDCALL InitializeUnicodeMapping(void)
{
#if defined(_WINDOWS)
	InitializeCriticalSection(&g_UNIMOcritsect);
#endif

	g_pUNIMOListHead = NULL;
	g_pUNIMOListTail = NULL;
}


DECSDKLINKAGE void STDCALL EndUnicodeMapping(void)
{
#if defined(_WINDOWS)
	DeleteCriticalSection(&g_UNIMOcritsect);
#endif
	
	// The list should be empty at this point.
	dec_assert(!g_pUNIMOListHead);
	dec_assert(!g_pUNIMOListTail);
}


DECSDKLINKAGE CUNIMO * STDCALL NewUnicodeMappingObject(const wchar_t *pUnicodeName)
{
#if defined(_WINDOWS)
	CUNIMO	*pObject;

	if (!pUnicodeName)
		return (NULL);

	pObject = new CUNIMO;
	if (!pObject)
		return (NULL);

	// Initialize the new object
	if (!pObject->Init(pUnicodeName))
	{
		delete pObject;
		return (NULL);
	}

	// Enter a critical section here while we are looking at the linked-list
	// of UNIMO objects.
	EnterCriticalSection(&g_UNIMOcritsect);

	if (!g_pUNIMOListHead)
	{
		// No objects in the linked list yet, so add this as the first node.
		pObject->pPrev = NULL;
		g_pUNIMOListHead = pObject;
	}
	else
	{
		// Add this node to the end of the linked list.
		pObject->pPrev = g_pUNIMOListTail;
		g_pUNIMOListTail->pNext = pObject;
	}

	g_pUNIMOListTail = pObject;

	// Leave the critical section now that we are finished looking at
	// the linked-list of UNIMO objects.
	LeaveCriticalSection(&g_UNIMOcritsect);

	return (pObject);
#else
	// No Unicode support on any platform except Windows.
	return (NULL);
#endif
}


DECSDKLINKAGE CUNIMO * STDCALL GetUnicodeMappingObject(const char *pszMBCSName)
{
#if defined(_WINDOWS)
	CUNIMO	*pObject = NULL;
	int		iLen = 0;
	bool	bExtended = false;

	if (!pszMBCSName)
		return (NULL);

	// Enter a critical section here while we are looking at the linked-list
	// of UNIMO objects.
	EnterCriticalSection(&g_UNIMOcritsect);

	pObject = g_pUNIMOListHead;
	while (pObject)
	{
		if (!strnicmp(pszMBCSName, pObject->szMBCSName, UNICODE_MAPPINGOBJECT_LEN))
			break;

		pObject = pObject->pNext;
	}

	if (pObject)
	{
		iLen = strlen(pszMBCSName);
		
		// OK, located a matching mapping object.  Now check to see if
		// we need to create an extended mapping object.  We do this when
		// there is more data following the "uni://xxxxxxxx" portion of the
		// input string.        
		if (iLen > UNICODE_MAPPINGOBJECT_LEN)
		{
			bExtended = true;
		}

		else
		{
			// We found the object in the list. Bump reference count
			InterlockedIncrement(&pObject->lRefCount);		 
		}
	}

	// Leave the critical section now that we are finished looking at
	// the linked-list of UNIMO objects.
	LeaveCriticalSection(&g_UNIMOcritsect);

	// If we need an extended object, create it now
	if (bExtended)
	{
		wchar_t *pszUniName = NULL;
		wchar_t *pszExtendedUniName = NULL;
		CUNIMO	*pNewObject;

		pszUniName = (wchar_t *)malloc(UNICODE_MAX_PATH);
		pszExtendedUniName = (wchar_t *)malloc(UNICODE_MAX_PATH);
		if (!pszUniName || !pszExtendedUniName)
		{
			if (pszUniName)
				free(pszUniName);
			if (pszExtendedUniName)
				free(pszExtendedUniName);
			return (NULL);
		}

		// Yes, this is an extended Unicode namespace reference.
		// Construct the extended Unicode string.
		wcscpy(pszUniName, pObject->pszUniName);
		mbstowcs(pszExtendedUniName, &pszMBCSName[UNICODE_MAPPINGOBJECT_LEN], iLen - UNICODE_MAPPINGOBJECT_LEN + 1);
		wcscat(pszUniName, pszExtendedUniName);

		pNewObject = NewUnicodeMappingObject(pszUniName);

		if (pszUniName)
			free(pszUniName);
		if (pszExtendedUniName)
			free(pszExtendedUniName);

		// Return the newly created object
		return pNewObject;
	}

	return (pObject);
#else
	// No Unicode support on any platform except Windows.
	return (NULL);
#endif
}


DECSDKLINKAGE CUNIMO * STDCALL GetUnicodeMappingObjectW(const wchar_t *pszUnicodeName)
{
#if defined(_WINDOWS)
	CUNIMO	*pObject;

	if (!pszUnicodeName)
		return (NULL);

	// Enter a critical section here while we are looking at the linked-list
	// of UNIMO objects.
	EnterCriticalSection(&g_UNIMOcritsect);

	pObject = g_pUNIMOListHead;
	while (pObject)
	{
		if (!wcscmp(pszUnicodeName, pObject->pszUniName))
			break;

		pObject = pObject->pNext;
	}

	if(pObject)
	{
		// We found the object in the list. Bump reference count
		InterlockedIncrement(&pObject->lRefCount);
	}

	// Leave the critical section now that we are finished looking at
	// the linked-list of UNIMO objects.
	LeaveCriticalSection(&g_UNIMOcritsect);

	return (pObject);
#else
	// No Unicode support on any platform except Windows.
	return (NULL);
#endif
}


DECSDKLINKAGE void STDCALL DeleteUnicodeMappingObject(CUNIMO *pObjectToDelete)
{
#if defined(_WINDOWS)
	CUNIMO	*pObject;
	LONG	lRefCount;

	if (!pObjectToDelete)
		return;

	// Enter a critical section here while we are looking at the linked-list
	// of UNIMO objects.
	EnterCriticalSection(&g_UNIMOcritsect);

	// Decrement reference count
	lRefCount = InterlockedDecrement(&pObjectToDelete->lRefCount);

	// If reference count is zero, delete the object
	if(0 == lRefCount)
	{
		// Get a pointer to the previous object.
		pObject = pObjectToDelete->pPrev;

		// Unhook this object from the linked-list by setting the previous
		// object's pNext to this object's pNext.
		if (!pObject)
		{
			// Deleting the first object in the linked-list is a special case.
			// Set the head pointer to this object's pNext.
			g_pUNIMOListHead = pObjectToDelete->pNext;
			if (g_pUNIMOListHead)
				g_pUNIMOListHead->pPrev = NULL;
		}
		else
		{
			// This is not the first object in the linked-list that we are
			// deleting.  Set the previous object's pNext to point to this
			// object's pNext.
			pObject->pNext = pObjectToDelete->pNext;
		}

		// Are we deleting the last object in the linked-list?
		pObject = pObjectToDelete->pNext;
		if (pObject)
		{
			// No, this is not the last object in the linked-list.
			// Point the next object's pPrev to this object's pPrev.
			pObject->pPrev = pObjectToDelete->pPrev;
		}
		else
		{
			// Yes, we are deleting the last object in the linked-list.
			// Set the tail pointer to this object's pPrev.
			g_pUNIMOListTail = pObjectToDelete->pPrev;
		}

		delete pObjectToDelete;
	}

	// Leave the critical section now that we are finished looking at
	// the linked-list of UNIMO objects.
	LeaveCriticalSection(&g_UNIMOcritsect);

#endif
}
