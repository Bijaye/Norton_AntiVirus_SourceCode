/////////////////////////////////////////////////////////////////////////////
// $Header:   S:/N32OPTS/VCS/stdafx.h_v   1.2   26 Jun 1998 17:39:08   mdunn  $
/////////////////////////////////////////////////////////////////////////////
//
//
// stdafx.h : include file for standard system include files,
//  or project specific include files that are used frequently, but
//      are changed infrequently
//
//
//
//
//
//
//
/////////////////////////////////////////////////////////////////////////////
// $Log:   S:/N32OPTS/VCS/stdafx.h_v  $
// 
//    Rev 1.2   26 Jun 1998 17:39:08   mdunn
// Now using navwhlp5.h for help IDs.
// 
//    Rev 1.1   20 Jun 1997 12:04:00   DBUCHES
// Added help support.
// 
//    Rev 1.0   14 May 1997 15:09:32   DBUCHES
// Initial revision.
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_STDAFX_H__0235E9C6_C1A7_11D0_8AD5_0060977341AC__INCLUDED_)
#define AFX_STDAFX_H__0235E9C6_C1A7_11D0_8AD5_0060977341AC__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000



#define VC_EXTRALEAN		// Exclude rarely-used stuff from Windows headers

#include <afxwin.h>         // MFC core and standard components
#include <afxext.h>         // MFC extensions

#ifndef _AFX_NO_OLE_SUPPORT
#include <afxole.h>         // MFC OLE classes
#include <afxodlgs.h>       // MFC OLE dialog classes
#include <afxdisp.h>        // MFC OLE automation classes
#endif // _AFX_NO_OLE_SUPPORT


#ifndef _AFX_NO_DB_SUPPORT
#include <afxdb.h>			// MFC ODBC database classes
#endif // _AFX_NO_DB_SUPPORT

#ifndef _AFX_NO_DAO_SUPPORT
#include <afxdao.h>			// MFC DAO database classes
#endif // _AFX_NO_DAO_SUPPORT

#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>			// MFC support for Windows Common Controls
#endif // _AFX_NO_AFXCMN_SUPPORT



//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#include "navwhlp5.h"

#endif // !defined(AFX_STDAFX_H__0235E9C6_C1A7_11D0_8AD5_0060977341AC__INCLUDED_)
