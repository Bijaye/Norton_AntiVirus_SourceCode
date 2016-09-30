////////////////////////////////////////////////////////////////////////////////////
// $Header:   S:/REMIND/VCS/STDAFX.H_v   1.1   15 Dec 1997 17:34:04   jtaylor  $
////////////////////////////////////////////////////////////////////////////////////
//
// stdafx.h : include file for standard system include files,
//  or project specific include files that are used frequently, but
//      are changed infrequently
//
////////////////////////////////////////////////////////////////////////////////////
// $Log:   S:/REMIND/VCS/STDAFX.H_v  $
// 
//    Rev 1.1   15 Dec 1997 17:34:04   jtaylor
// fixed the logout problem, updated the timer logic.
//
////////////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_STDAFX_H__85687ECA_6DD3_11D1_8F2D_444553540000__INCLUDED_)
#define AFX_STDAFX_H__85687ECA_6DD3_11D1_8F2D_444553540000__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#define VC_EXTRALEAN		// Exclude rarely-used stuff from Windows headers

#include <afxwin.h>         // MFC core and standard components
#include <afxext.h>         // MFC extensions
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>			// MFC support for Windows Common Controls
#endif // _AFX_NO_AFXCMN_SUPPORT


//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_STDAFX_H__85687ECA_6DD3_11D1_8F2D_444553540000__INCLUDED_)
