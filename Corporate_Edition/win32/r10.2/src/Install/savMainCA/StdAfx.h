// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright (c) 2005 Symantec Corporation. All rights reserved.
// stdafx.h : include file for standard system include files,
//  or project specific include files that are used frequently, but
//      are changed infrequently
//

#if !defined(AFX_STDAFX_H__B6E26EBF_27D5_4BAF_ABB5_5578E3826DC2__INCLUDED_)
#define AFX_STDAFX_H__B6E26EBF_27D5_4BAF_ABB5_5578E3826DC2__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <afxwin.h>         // MFC core and standard components
#include <afxext.h>         // MFC extensions

#ifndef _AFX_NO_OLE_SUPPORT
#include <afxole.h>         // MFC OLE classes
#include <afxodlgs.h>       // MFC OLE dialog classes
#include <afxdisp.h>        // MFC Automation classes
#endif // _AFX_NO_OLE_SUPPORT


#ifndef _AFX_NO_DB_SUPPORT
#include <afxdb.h>			// MFC ODBC database classes
#endif // _AFX_NO_DB_SUPPORT

#ifndef _AFX_NO_DAO_SUPPORT
#include <afxdao.h>			// MFC DAO database classes
#endif // _AFX_NO_DAO_SUPPORT

#include <afxdtctl.h>		// MFC support for Internet Explorer 4 Common Controls
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>			// MFC support for Windows Common Controls
#endif // _AFX_NO_AFXCMN_SUPPORT

#include <windows.h>
#define _WIN32_MSI 200
#include "..\..\..\..\..\..\tools\platform.sdk\include\msi.h"
#include "..\..\..\..\..\..\tools\platform.sdk\include\msiquery.h"

#include "DefUtils.h"
#include "vpstrutils.h"

// Shared #defines

#define DEFAULT_DOMAIN_PASSWORD		"symantec"
#define MBUFFER						512

#ifdef UNICODE
#define tstring std::wstring
#else
#define tstring std::string
#endif

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_STDAFX_H__B6E26EBF_27D5_4BAF_ABB5_5578E3826DC2__INCLUDED_)
