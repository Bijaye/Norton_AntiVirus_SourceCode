// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright (c) 2005 Symantec Corporation. All rights reserved.
/*===========================================================================*/

// mec.h : Include file for source module that use MAPI routines.

#ifndef _MEC_H_
#define _MEC_H_

#include <windows.h>
#include <tchar.h>

#include "UseMAPIIID.h"
#include "memexcp.h"
#include "helpers.h"

#include <mapix.h>
#include <mapihook.h>
#include <mapiwz.h>
#include <mapiform.h>
#include <crtdbg.h>
#include "SymSaferStrings.h"
#include "SavAssert.h"

#pragma warning(disable: 4091)

#include <exchext.h>

#pragma warning(default: 4091)

#ifdef EXTERN
	#define DO_INITIALIZERS
#else
	#define EXTERN extern
#endif

/*---------------------------------------------------------------------------*/

// The entry ID of each of the special folders created by Outlook (Calendar,
// Contacts, Journal, Notes, and Tasks) is stored in a named property on the
// Inbox folder of the message store.  For more information see knowledge
// base artical: Q171670

#define PR_OUTLOOK_CALENDAR_ENTRYID	PROP_TAG(PT_BINARY, 0x36D0)
#define PR_OUTLOOK_CONTACTS_ENTRYID	PROP_TAG(PT_BINARY, 0x36D1)
#define PR_OUTLOOK_JOURNAL_ENTRYID	PROP_TAG(PT_BINARY, 0x36D2)
#define PR_OUTLOOK_NOTES_ENTRYID	PROP_TAG(PT_BINARY, 0x36D3)
#define PR_OUTLOOK_TASKS_ENTRYID	PROP_TAG(PT_BINARY, 0x36D4)

// Create a new property for storing signature to prevent sacnning a message 
// more than once. 0x6321 is just a arbitrary number selected
#define PR_HASSIG PROP_TAG (PT_STRING8, 0x6321)

/*===========================================================================*/
/*---------------------------------------------------------------------------*/

// class MAPIAllocators contains pointers to the MAPI allocator functions.  This class is used by the various wrapper
// classes for MAPI (such as CMessage or CSession) and for the Smart MAPI pointers defined below.  In service providers,
// the allocators function pointers are passed in at initialization time and must be saved.  In client code, the allocator
// functions can be called directly.  Using class MAPIAllocators enables the wrapper class code to be the same for either
// environment (at the cost of passing the reference around).
//
// Member functions parameters are the same as the MAPIAllocation function parameters.  However, AllocateBuffer() and
// AllocateMore() have one additional OPTIONAL argument "bNoThrow".  If bNoThrow is true and memory can't be allocated,
// CMyMemoryException isn't thrown (instead the code must check the return values).  Otherwise, if the allocator fails,
// CMyMemoryException will be thrown if the allocation fails.
//
// Of special note are the member functions FreeProws() and FreePadrlist().  In client code the direct API can be called.
// In service providers no API is available.  The member functions FreeProws() and FreePadrlist() provides this functionality.
// The FreeProws() and FreePadrlist() function pointers are optional but should be provided in client code.  They will be zero in
// service provider code (as there are no APIs).


typedef void (__stdcall * LPFREEPROWS)(LPSRowSet prows);
typedef void (__stdcall * LPFREEPADRLIST)(LPADRLIST lpAdrList);


// The following definitions are from MAPIUTIL.H--they are used to define the prototypes to FreeProws() and FreePaddlist() and
// are shown here to document the above typedefs:
//
//	#define EXTERN_C    extern "C"
//	#define STDAPICALLTYPE          __export __stdcall
//	#define STDAPI_(type)           EXTERN_C type STDAPICALLTYPE
//	STDAPI_(void)           FreePadrlist(LPADRLIST lpAdrlist);
//	STDAPI_(void)           FreeProws(LPSRowSet lpRows);

class MAPIAllocators
{
public :
	MAPIAllocators() { Set(0, 0, 0, 0, 0); }

	SCODE AllocateBuffer(ULONG cbSize, LPVOID FAR *lppBuffer, bool bNoThrow = false);
	SCODE AllocateMore(ULONG cbSize, LPVOID lpObject, LPVOID FAR *lppBuffer, bool bNoThrow = false);
	ULONG FreeBuffer(LPVOID lpBuffer) { SAVASSERT(pfnFreeBuffer != 0); return pfnFreeBuffer(lpBuffer); }

	void FreeProws(LPSRowSet prows);
	void FreePadrlist(LPADRLIST lpAdrlist);

	void Set(LPALLOCATEBUFFER allocBuf, LPALLOCATEMORE allocMore, LPFREEBUFFER freeBuf, LPFREEPROWS freeRows = 0, LPFREEPADRLIST freeAdrList = 0)
		{ pfnAllocateBuffer = allocBuf; pfnAllocateMore = allocMore; pfnFreeBuffer = freeBuf; pfnFreeProws = freeRows; pfnFreePadrlist = freeAdrList; }

private :

	LPALLOCATEBUFFER pfnAllocateBuffer;
	LPALLOCATEMORE pfnAllocateMore;
	LPFREEBUFFER pfnFreeBuffer;
	LPFREEPROWS pfnFreeProws;
	LPFREEPADRLIST pfnFreePadrlist;
};

/*---------------------------------------------------------------------------*/

#ifdef DO_INITIALIZERS
	SCODE MAPIAllocators::AllocateBuffer(ULONG cbSize, LPVOID FAR *lppBuffer, bool bNoThrow)
	{
		SCODE
			rv;

		SAVASSERT(pfnAllocateBuffer != 0);

		rv = pfnAllocateBuffer(cbSize, lppBuffer);

		if (!bNoThrow && (rv != S_OK || *lppBuffer == 0))
			throw CMyMemoryException(cbSize);

		return rv;
	}

	SCODE MAPIAllocators::AllocateMore(ULONG cbSize, LPVOID lpObject, LPVOID FAR *lppBuffer, bool bNoThrow)
	{
		SCODE
			rv;

		SAVASSERT(pfnAllocateBuffer != 0);

		rv = pfnAllocateMore(cbSize, lpObject, lppBuffer);

		if (!bNoThrow && (rv != S_OK || *lppBuffer == 0))
			throw CMyMemoryException(cbSize);

		return rv;
	}

	void MAPIAllocators::FreeProws(LPSRowSet prows)
	{
		if (pfnFreeProws)
			pfnFreeProws(prows);
		else 
		{
			for (ULONG i = 0; i < prows->cRows; i++)
			{
				pfnFreeBuffer(prows->aRow[i].lpProps);
			}

			pfnFreeBuffer(prows);
		}
	}

	void MAPIAllocators::FreePadrlist(LPADRLIST lpAdrlist)
	{
		if (pfnFreePadrlist)
			pfnFreePadrlist(lpAdrlist);
		else 
		{
			for (ULONG i = 0; i < lpAdrlist->cEntries; i++)
			{
				pfnFreeBuffer(lpAdrlist->aEntries[i].rgPropVals);
			}

			pfnFreeBuffer(lpAdrlist);
		}
	}

#endif

/*---------------------------------------------------------------------------*/

template <class T> class SmartMAPIBufPtr
{
public:
	SmartMAPIBufPtr(MAPIAllocators& alloc, T* pT = 0) : m_allocators(alloc)		{ owner = (p = pT) != 0; }
	SmartMAPIBufPtr(SmartMAPIBufPtr<T>& srT) : m_allocators(srT.m_allocators)	{ owner = srT.owner; p = srT.Detach(); }
	~SmartMAPIBufPtr()							{ _cleanup(); }
	T* Detach(void)								{ owner = 0; return p; }
	void Release(void)							{ _cleanup(); Reset(); }
	void Reset(void)							{ owner = 0; p = 0; }
	operator T*()								{ return (T*) p; }
	T& operator*()								{ SAVASSERT(p != 0); return *p; }
	// operator&() is used for assignment as an OUT only parameter.  If the pointer is to be used as an IN/OUT
	// parameter, take the address of the 'p' member explicitly.  operator&() assumes that ownership is about to
	// be granted to this pointer.  Reset() or Detach() member functions can be called to relinquish ownership.
	T** operator&()								{ _cleanup(); p = 0; owner = true; return &p; }
	T* operator->()								{ SAVASSERT(p != 0); return p; }
	T* operator=(T* pT)							{ _cleanup(); owner = (p = pT) != 0; return p; }
	SmartMAPIBufPtr<T>& operator=(SmartMAPIBufPtr<T>& srT)	{ _cleanup(); owner = srT.owner; m_allocators = srT.m_allocators; p = srT.Detach(); return *this; }
	int operator!()								{ return (p == 0); }
	T* p;
private:
	void _cleanup(void)							{ if (owner && p) m_allocators.FreeBuffer(p); }
	int owner;
	MAPIAllocators& m_allocators;
};

/*---------------------------------------------------------------------------*/

class SmartSRowSetPtr
{
public:
	SmartSRowSetPtr(MAPIAllocators& alloc, LPSRowSet pT = 0) : m_allocators(alloc)	{ owner = (p = pT) != 0; }
	SmartSRowSetPtr(SmartSRowSetPtr& srT) : m_allocators(srT.m_allocators)			{ owner = srT.owner; p = srT.Detach(); }
	~SmartSRowSetPtr()							{ _cleanup(); }
	LPSRowSet Detach(void)						{ owner = 0; return p; }
	void Release(void)							{ _cleanup(); Reset(); }
	void Reset(void)							{ owner = 0; p = 0; }
	operator LPSRowSet()						{ return (LPSRowSet) p; }
	SRowSet& operator*()						{ SAVASSERT(p != 0); return *p; }
	// operator&() is used for assignment as an OUT only parameter.  If the pointer is to be used as an IN/OUT
	// parameter, take the address of the 'p' member explicitly.  operator&() assumes that ownership is about to
	// be granted to this pointer.  Reset() or Detach() member functions can be called to relinquish ownership.
	LPSRowSet* operator&()						{ _cleanup(); p = 0; owner = true; return &p; }
	LPSRowSet operator->()						{ SAVASSERT(p != 0); return p; }
	LPSRowSet operator=(LPSRowSet pT)			{ _cleanup(); owner = (p = pT) != 0; return p; }
	SmartSRowSetPtr& operator=(SmartSRowSetPtr& srT)
												{ _cleanup(); owner = srT.owner; m_allocators = srT.m_allocators; p = srT.Detach(); return *this; }
	int operator!()								{ return (p == 0); }
	LPSRowSet p;
private:
	void _cleanup(void)							{ if (owner && p) m_allocators.FreeProws(p); }
	int owner;
	MAPIAllocators& m_allocators;
};

/*---------------------------------------------------------------------------*/

class SmartADRLISTPtr
{
public:
	SmartADRLISTPtr(MAPIAllocators& alloc, LPADRLIST pT = 0) : m_allocators(alloc)	{ owner = (p = pT) != 0; }
	SmartADRLISTPtr(SmartADRLISTPtr& srT) : m_allocators(srT.m_allocators)			{ owner = srT.owner; p = srT.Detach(); }
	~SmartADRLISTPtr()							{ _cleanup(); }
	LPADRLIST Detach(void)						{ owner = 0; return p; }
	void Release(void)							{ _cleanup(); Reset(); }
	void Reset(void)							{ owner = 0; p = 0; }
	operator LPADRLIST()						{ return (LPADRLIST) p; }
	ADRLIST& operator*()						{ SAVASSERT(p != 0); return *p; }
	// operator&() is used for assignment as an OUT only parameter.  If the pointer is to be used as an IN/OUT
	// parameter, take the address of the 'p' member explicitly.  operator&() assumes that ownership is about to
	// be granted to this pointer.  Reset() or Detach() member functions can be called to relinquish ownership.
	LPADRLIST* operator&()						{ _cleanup(); p = 0; owner = true; return &p; }
	LPADRLIST operator->()						{ SAVASSERT(p != 0); return p; }
	LPADRLIST operator=(LPADRLIST pT)			{ _cleanup(); owner = (p = pT) != 0; return p; }
	SmartADRLISTPtr& operator=(SmartADRLISTPtr& srT)
												{ _cleanup(); owner = srT.owner; m_allocators = srT.m_allocators; p = srT.Detach(); return *this; }
	int operator!()								{ return (p == 0); }
	LPADRLIST p;
private:
	void _cleanup(void)							{ if (owner && p) m_allocators.FreePadrlist(p); }
	int owner;
	MAPIAllocators& m_allocators;
};


/*===========================================================================*/
/*---------------------------------------------------------------------------*/

// The CMAPIException class/macros/functions are used for stuctured
// exception handling for MAPI interface routines.

class CMAPIException
{
public :
	HRESULT	hr;
	char *file;
	int line, bMAPIError;

	// data from MAPIERROR

	ULONG	ulVersion;
	TCHAR	szError[128];
	TCHAR	szComponent[80];
	ULONG	ulLowLevelError;
	ULONG	ulContext;

	CMAPIException(HRESULT hErr = E_FAIL, LPMAPIERROR pErr = 0, char *fname = __FILE__, int lineno = __LINE__);
};

void verboseMAPIException(LPCTSTR pszVerboseName, CMAPIException& e);

EXTERN TCHAR
	VERBOSE_MAPIFullError[]
	#ifdef DO_INITIALIZERS
		= _T("ERROR: %s: MAPI Exception: HR = %x, error %s, component %s, context %x, file %s, line %d")
	#endif
	,
	VERBOSE_MAPIPartError[]
	#ifdef DO_INITIALIZERS
		= _T("ERROR: %s: MAPI Exception: HR = %x, file %s, line %d")
	#endif
	;

#ifdef DO_INITIALIZERS
	CMAPIException::CMAPIException(HRESULT hErr, LPMAPIERROR pMAPIError, char *fname, int lineno)
	{
		hr = hErr;
		file = fname;
		line = lineno;
		bMAPIError = false;
		*szError = *szComponent = 0;

		if (pMAPIError)
		{
			ulVersion = pMAPIError->ulVersion;
			ulLowLevelError = pMAPIError->ulLowLevelError;
			ulContext = pMAPIError->ulContext;
			if (pMAPIError->lpszError)
				ssStrnCpy(szError, pMAPIError->lpszError, sizeof (szError));
			if (pMAPIError->lpszComponent)
				ssStrnCpy(szComponent, pMAPIError->lpszComponent, sizeof (szComponent));

			bMAPIError = true;
		}
	}

	void verboseMAPIException(LPCTSTR pszVerboseName, CMAPIException& e)
	{
		if (e.bMAPIError)
			DebugOut(SM_ERROR, VERBOSE_MAPIFullError, pszVerboseName, e.hr, e.szError, e.szComponent, e.ulContext, e.file, e.line);
		else
			DebugOut(SM_ERROR, VERBOSE_MAPIPartError, pszVerboseName, e.hr, e.file, e.line);
	}
#endif

// THROW_CMAPIException() macro is used to throw a CMAPIException object with associated GetLastError information
//
#define THROW_CMAPIException(alloc, hErr, pInterface)						\
{																			\
	LPMAPIERROR lpMAPIError = 0;											\
																			\
	if (S_OK != pInterface->GetLastError(hErr, fMapiUnicode, &lpMAPIError))	\
		lpMAPIError = 0;													\
																			\
	CMAPIException e(hErr, lpMAPIError, __FILE__, __LINE__);				\
																			\
	if (lpMAPIError)														\
		alloc.FreeBuffer(lpMAPIError);										\
																			\
	throw e;																\
}
	
// THROW_CMAPIExceptionHR() macro is used to throw CMAPIException when no extended error information will be available
// (interface->GetLastError() doesn't exist).

#define THROW_CMAPIExceptionHR(hErr) { throw CMAPIException(hErr, 0, __FILE__, __LINE__); }

EXTERN SBinary g_NullSBinary
	#ifdef DO_INITIALIZERS
		= { 0, (LPBYTE) _T("") }
	#endif
	;

/*---------------------------------------------------------------------------*/

SCODE STDMETHODCALLTYPE DummyAllocateBuffer(ULONG, LPVOID FAR *);
SCODE STDMETHODCALLTYPE DummyAllocateMore(ULONG, LPVOID, LPVOID FAR *);
ULONG STDAPICALLTYPE DummyFreeBuffer(LPVOID);
void __stdcall DummyFreeProws(LPSRowSet);
void __stdcall DummyFreePaddrlist(LPADRLIST);

/*---------------------------------------------------------------------------*/

HRESULT UnloadMAPI(void);
HRESULT LoadMAPI(void);

/*---------------------------------------------------------------------------*/

EXTERN HINSTANCE g_hMAPIInstance;
EXTERN DWORD g_cMAPILoaded;

EXTERN LPMAPIALLOCATEBUFFER g_pfnMAPIAllocateBuffer
	#ifdef DO_INITIALIZERS
		= DummyAllocateBuffer
	#endif
	;
EXTERN LPMAPIALLOCATEMORE g_pfnMAPIAllocateMore
	#ifdef DO_INITIALIZERS
		= DummyAllocateMore
	#endif
	;
EXTERN LPMAPIFREEBUFFER g_pfnMAPIFreeBuffer
	#ifdef DO_INITIALIZERS
		= DummyFreeBuffer
	#endif
	;
EXTERN LPFREEPROWS g_pfnFreeProws
	#ifdef DO_INITIALIZERS
		= DummyFreeProws
	#endif
	;
EXTERN LPFREEPADRLIST g_pfnFreePaddrlist
	#ifdef DO_INITIALIZERS
		= DummyFreePaddrlist
	#endif
	;

#ifdef DO_INITIALIZERS
	#undef DO_INITIALIZERS
#else
	#undef EXTERN
#endif

/*---------------------------------------------------------------------------*/

inline int IsValidProp(LPSPropValue pProps, ULONG propIndex, ULONG propID)
{
	return pProps[propIndex].ulPropTag == propID;
}

inline int IsValidPropInRow(LPSRowSet pRows, ULONG rowIndex, ULONG propIndex, ULONG propID)
{
	return	pRows &&
			rowIndex < pRows->cRows &&
			propIndex < pRows->aRow[rowIndex].cValues &&
			pRows->aRow[rowIndex].lpProps[propIndex].ulPropTag == propID;
}

/*---------------------------------------------------------------------------*/

#include <edkmdb.h>

#include "Attach.h"
#include "Message.h"
#include "MAPIFolder.h"
#include "MsgStore.h"

#endif  // _MEC_H_