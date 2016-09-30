// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright (c) 2005 Symantec Corporation. All rights reserved.
/*===========================================================================*/

// helpers.cpp.  Contains a variety of general helper routines.
//
// Mostly, these have been borrowed from other libraries.  They are included here
// because I don't want to incure the overhead of the rest of the libraries (or source).

#include <windows.h>
#include <tchar.h>
#include <crtdbg.h>

#include "helpers.h"

IUnknown* AtlComPtrAssign(IUnknown** pp, IUnknown* lp)
{
	if (lp != 0) lp->AddRef();
	if (*pp)(*pp)->Release();
	
	*pp = lp;
	return lp;
}

IUnknown* AtlComQIPtrAssign(IUnknown** pp, IUnknown* lp, REFIID riid)
{
	IUnknown* pTemp = *pp;
	lp->QueryInterface(riid, (void**)pp);
	if (pTemp) pTemp->Release();
	return *pp;
}

/* end source file */