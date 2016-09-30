// stdafx.h : include file for standard system include files,
//  or project specific include files that are used frequently, but
//      are changed infrequently
//

#if !defined(AFX_STDAFX_H__9D885809_7C94_47F2_A9AE_ABB38583A4F4__INCLUDED_)
#define AFX_STDAFX_H__9D885809_7C94_47F2_A9AE_ABB38583A4F4__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define VC_EXTRALEAN		// Exclude rarely-used stuff from Windows headers

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

#include "ccLib.h"

// This is an optimization/convenience macro for switches where the
// default case should never be hit.
#ifdef DEFAULT_UNREACHABLE
#undef DEFAULT_UNREACHABLE
#endif
#ifdef _DEBUG
    #define DEFAULT_UNREACHABLE  default: ASSERT(0); break
#elif _MSC_VER >= 1200
    #define DEFAULT_UNREACHABLE  default: __assume(0); break
#else
    #define DEFAULT_UNREACHABLE  default: break
#endif   

// Convenience macro for inline loading of temp strings from the string table,
// for use in message boxes and such.
// Debug version is in globals.cpp
#ifndef _DEBUG
#define _S(id) (CString((LPCTSTR)id))
#else
CString _S(UINT uID);
#endif

extern BOOL    g_bUseHiColorBmps;

#include <atlbase.h>

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_STDAFX_H__9D885809_7C94_47F2_A9AE_ABB38583A4F4__INCLUDED_)
