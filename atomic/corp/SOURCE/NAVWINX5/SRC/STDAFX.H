// Copyright 1992 - 1994 Symantec, Peter Norton Group
// ************************************************************************
//
// $Header:   S:/NAVWINX5/VCS/STDAFX.H_v   1.0   01 Jun 1998 16:08:40   DBuches  $
//
// Description:
//
// See Also:
//
// ************************************************************************
// $Log:   S:/NAVWINX5/VCS/STDAFX.H_v  $
// 
//    Rev 1.0   01 Jun 1998 16:08:40   DBuches
// Initial revision.
// 
//    Rev 1.0   01 Mar 1998 18:49:10   pagbabi
// Initial revision.
// ************************************************************************

// stdafx.h : include file for standard system include files,
//      or project specific include files that are used frequently,
//      but are changed infrequently

#if !defined(AFX_STDAFX_H__73546D82_AD84_11D1_9251_0060979C3468__INCLUDED_)
#define AFX_STDAFX_H__73546D82_AD84_11D1_9251_0060979C3468__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#define STRICT

#include <afxwin.h>
#include <afxdisp.h>

#define _WIN32_WINNT 0x0400
#define _ATL_APARTMENT_THREADED


#include <atlbase.h>
//You may derive a class from CComModule and use it if you want to override
//something, but do not change the name of _Module
extern CComModule _Module;
#include <atlcom.h>

#include "mmcbase.h"

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_STDAFX_H__73546D82_AD84_11D1_9251_0060979C3468__INCLUDED)
