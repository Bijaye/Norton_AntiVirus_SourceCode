////////////////////////
//
// PROPRIETARY / CONFIDENTIAL.
// Use of this product is subject to license terms.
// Copyright © 2006 Symantec Corporation.
// All rights reserved.
//
/////////////////////////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////
// Stuff related to the DefAlert program that notifies the user when the
// defs on the machine are too old, even if he never goes into NAV.

#include <windows.h>
#include <tchar.h>
#include <commctrl.h>
#include <winreg.h>


#ifndef DEFALERT_H_INCLUDED
#define DEFALERT_H_INCLUDED


#define SYM_DEFALERT_KEY        _T("Software\\Symantec\\Norton AntiVirus\\DefAlert")
#define SYM_DEFALERT_NEXTCHECK_VALUE    _T("NextCheck")
#define SYM_DEFALERT_VALUE      _T("NAV DefAlert")

// **WARNING**  The SYM_DEFALERT_VALUE *must* be less than 32 chars long.  The
// OS uses a 32-char buffer while enumerating the Run key, so if any strings
// have a name of >= 32 chars, they will skipped entirely!


#endif  // ndef DEFALERT_H_INCLUDED