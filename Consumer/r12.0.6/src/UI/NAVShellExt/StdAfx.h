// stdafx.h : include file for standard system include files,
//      or project specific include files that are used frequently,
//      but are changed infrequently

#if !defined(AFX_STDAFX_H__AC31C20B_45D0_4C59_8C59_624E15E9E019__INCLUDED_)
#define AFX_STDAFX_H__AC31C20B_45D0_4C59_8C59_624E15E9E019__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define STRICT
#ifndef _WIN32_WINNT
#define _WIN32_WINNT 0x0400
#endif
#define _ATL_APARTMENT_THREADED

#include <atlbase.h>
//You may derive a class from CComModule and use it if you want to override
//something, but do not change the name of _Module
extern CComModule _Module;
#include <atlcom.h>
//#include <atlapp.h>
//#include <atlmisc.h>

#include <shlobj.h>
#include <comdef.h>
// STL headers
#include <vector>

#define SIZEOF(x) (sizeof(x) / sizeof(x[0]))

#include "OSInfo.h"
#include "NAVInfo.h"
#include "StahlSoft.h"

#include "ccLibStd.h"

typedef ::std::basic_string<TCHAR> TSTRING;
extern COSInfo  g_OSInfo;
extern CNAVInfo g_NAVInfo;

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_STDAFX_H__AC31C20B_45D0_4C59_8C59_624E15E9E019__INCLUDED)
