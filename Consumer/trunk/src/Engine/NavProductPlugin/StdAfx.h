////////////////////////
//
// PROPRIETARY / CONFIDENTIAL.
// Use of this product is subject to license terms.
// Copyright © 2006 Symantec Corporation.
// All rights reserved.
//
/////////////////////////////////////////////////////////////////////////////////////////////////

// stdafx.h : include file for standard system include files,
//  or project specific include files that are used frequently, but
//      are changed infrequently
//

#if !defined(AFX_STDAFX_H__14D67024_897B_4539_9A79_67F8C4CE9EA1__INCLUDED_)
#define AFX_STDAFX_H__14D67024_897B_4539_9A79_67F8C4CE9EA1__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


// Insert your headers here
#define WIN32_LEAN_AND_MEAN		// Exclude rarely-used stuff from Windows headers

#define _WIN32_WINNT 0x0400

#include <windows.h>


// Warning 4786 apears often when using the C++ Library, so turn it off.
// ("identifier was truncated to '255' characters in the browser information")
#pragma warning(disable : 4786)

#define _WIN32_MSI	  110

#include <vector>
#include <string>
#include <atlstr.h>
#include <tchar.h>
#include <Msiquery.h>
#include "msi.h"

#include <atlbase.h>
extern CAtlBaseModule _Module;

#import "ccEventManager.tlb"

#include "ccLib.h"

// TODO: reference additional headers your program requires here

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_STDAFX_H__14D67024_897B_4539_9A79_67F8C4CE9EA1__INCLUDED_)
