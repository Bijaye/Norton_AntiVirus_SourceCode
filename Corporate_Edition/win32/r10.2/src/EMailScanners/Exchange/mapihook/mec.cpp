// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright (c) 2005 Symantec Corporation. All rights reserved.
/*===========================================================================*/

// mec.cpp contains code to load and unload MAPI32.DLL

#define EXTERN
#define INITGUID

#include "ldvpmec.h"
#include "mec.h"

// Dummy MAPI functions prevent calls through NULL pointers.

SCODE STDMETHODCALLTYPE DummyAllocateBuffer(ULONG, LPVOID FAR *)
{
	static TCHAR s_verboseName[] = _T("DummyAllocateBuffer");

	SAVASSERT(FALSE);
	DebugOut(SM_WARNING, s_verboseName);
	return E_FAIL;
}

SCODE STDMETHODCALLTYPE DummyAllocateMore(ULONG, LPVOID, LPVOID FAR *)
{
	static TCHAR s_verboseName[] = _T("DummyAllocateMore");

	SAVASSERT(FALSE);
	DebugOut(SM_WARNING, s_verboseName);
	return E_FAIL;
}

ULONG STDAPICALLTYPE DummyFreeBuffer(LPVOID)
{
	static TCHAR s_verboseName[] = _T("DummyFreeBuffer");

	SAVASSERT(FALSE);
	DebugOut(SM_WARNING, s_verboseName);
	return (ULONG) E_FAIL;
}

void __stdcall DummyFreeProws(LPSRowSet)
{
	static TCHAR s_verboseName[] = _T("DummyFreeProws");

	SAVASSERT(FALSE);
	DebugOut(SM_WARNING, s_verboseName);
}

void __stdcall DummyFreePaddrlist(LPADRLIST)
{
	static TCHAR s_verboseName[] = _T("DummyFreePaddrlist");

	SAVASSERT(FALSE);
	DebugOut(SM_WARNING, s_verboseName);
}

// UnloadMAPI() decrements the reference count and if zero unloads MAPI32.DLL and clears the function pointers.  

HRESULT UnloadMAPI(void)
{
	static TCHAR s_verboseName[] = _T("UnloadMAPI");

	if (!g_cMAPILoaded)
		{
		DebugOut(SM_WARNING, "%s: MAPI not loaded", s_verboseName);
		return E_FAIL;
		}

	if (--g_cMAPILoaded == 0)
		{
		g_pfnMAPIAllocateBuffer	= DummyAllocateBuffer;
		g_pfnMAPIAllocateMore = DummyAllocateMore;
		g_pfnMAPIFreeBuffer	= DummyFreeBuffer;
		g_pfnFreeProws = DummyFreeProws;
		g_pfnFreePaddrlist = DummyFreePaddrlist;

		if (g_hMAPIInstance)
			{
			FreeLibrary(g_hMAPIInstance);
			g_hMAPIInstance = 0;
			}

		DebugOut(SM_GENERAL, "%s: unload successful [%d]", s_verboseName, g_cMAPILoaded);
		}
	else
		DebugOut(SM_GENERAL, "%s: virtually unloaded [%d]", s_verboseName, g_cMAPILoaded);


	return S_OK;
}

// LoadMAPI() loads MAPI32.DLL and gets the proc addresses needed for CLIENT access.  This routine
// should not be used by Service Providers.  The .DLL is loaded only once.  Additional calls to
// LoadMAPI() simply increment the reference count.  Each call to LoadMAPI() must have corresponding
// calls to UnloadMAPI().

HRESULT LoadMAPI(void)
{
	static TCHAR s_verboseName[] = _T("LoadMAPI");
	static MAPIINIT_0 MAPIINIT = { MAPI_INIT_VERSION, MAPI_MULTITHREAD_NOTIFICATIONS | MAPI_NT_SERVICE };

//	DebugOut(SM_GENERAL, s_verboseName);

	if (g_cMAPILoaded++ == 0)
		{
		TCHAR szPath[MAX_PATH];
		ZeroMemory(szPath, sizeof(szPath));
		::GetSystemDirectory( szPath, MAX_PATH );
		_tcscat( szPath, _T("\\MAPI32.DLL"));
		g_hMAPIInstance = LoadLibrary(szPath);
		if (!g_hMAPIInstance)
			{
			DebugOut(SM_ERROR, "ERROR: %s: unable to load MAPI32.DLL", s_verboseName);
			UnloadMAPI();
			return E_NOINTERFACE;
			}

		g_pfnMAPIAllocateBuffer	= (LPMAPIALLOCATEBUFFER)	GetProcAddress(g_hMAPIInstance, _T("MAPIAllocateBuffer"));
		g_pfnMAPIAllocateMore	= (LPMAPIALLOCATEMORE)		GetProcAddress(g_hMAPIInstance, _T("MAPIAllocateMore"));
		g_pfnMAPIFreeBuffer		= (LPMAPIFREEBUFFER)		GetProcAddress(g_hMAPIInstance, _T("MAPIFreeBuffer"));
		g_pfnFreeProws			= (LPFREEPROWS)				GetProcAddress(g_hMAPIInstance, _T("FreeProws@4"));
		g_pfnFreePaddrlist		= (LPFREEPADRLIST)			GetProcAddress(g_hMAPIInstance, _T("FreePaddrlist@4"));

		if (!g_pfnMAPIAllocateBuffer	||
			!g_pfnMAPIAllocateMore		||
			!g_pfnMAPIFreeBuffer)	// If can't find FreeProws() or FreePaddrlist(), internal ones will be used
			{
			DebugOut(SM_ERROR, "ERROR: %s: unable to GetProcAddresses", s_verboseName);
			UnloadMAPI();
			return E_NOINTERFACE;
			}

		DebugOut(SM_GENERAL, "%s: load successful [%d]", s_verboseName, g_cMAPILoaded);
		}
	else
		DebugOut(SM_GENERAL, "%s: virtually loaded [%d]", s_verboseName, g_cMAPILoaded);


	return S_OK;
}

/* end source file */