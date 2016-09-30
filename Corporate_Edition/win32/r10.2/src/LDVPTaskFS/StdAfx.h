// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright (c) 2005 Symantec Corporation. All rights reserved.
// stdafx.h : include file for standard system include files,
//      or project specific include files that are used frequently,
//      but are changed infrequently

#define VC_EXTRALEAN		// Exclude rarely-used stuff from Windows headers

#include <afxctl.h>         // MFC support for OLE Controls
#include <afxcmn.h>

// Delete the two includes below if you do not wish to use the MFC
//  database classes
#ifndef _UNICODE
#include <afxdb.h>			// MFC database classes
#include <afxdao.h>			// MFC DAO database classes
#endif //_UNICODE
