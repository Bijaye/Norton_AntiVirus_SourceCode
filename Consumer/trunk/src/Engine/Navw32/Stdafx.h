////////////////////////
//
// PROPRIETARY / CONFIDENTIAL.
// Use of this product is subject to license terms.
// Copyright © 2006 Symantec Corporation.
// All rights reserved.
//
/////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef STDAFX_H
#define STDAFX_H

#ifndef _WIN32_WINNT
#define _WIN32_WINNT 0x0400
#endif

#include <atlbase.h>
#include <atlstr.h>
#include <tchar.h>
#include <crtdbg.h>

#include <navtstring.h>

#include <StahlSoft.h>
#include <ccLib.h>
#include <ccOSInfo.h>

#pragma warning (disable: 4786)

#undef STRIZE
#define STRIZE(x) #x
#undef PPSTRIZE
#define PPSTRIZE(x) STRIZE(x)
#undef FILE_LINE
#define FILE_LINE __FILE__ "(" PPSTRIZE(__LINE__) ") : "
#define TODO(msg) message(FILE_LINE "TODO:  " msg)

extern HINSTANCE g_hInstance;

// Define the tag used to mark the object code for Norton AntiVirus
// as Symantec, Inc. property.

// This string must satisfy the following requirements:
//
// 1) It uniquely identifies the product.
// 2) It changes for every release.
// 3) It doesn't consist of human readable text.
// 4) It must be possible to search for and find it in the object code.

// To meet these requirements, the string was generated using the GUIDGEN.EXE
// program from Microsoft Visual Studio.

// To use this string:
//
// 1) Reference the NAVCODETAG symbol in a static/global variable initialization
//    expression, specified such that your compiler won't optimize it out.
// 2) Place the variable definition in a source file that doesn't contain
//    any proprietary information.
// 3) Compile your application.
// 4) Open your application's object file in a hex editor, and verify
//    that the string is there by searching for it.

// NAV 11.5 - Added 115 to the end of the exisiting tag GUID.
// NAV 12.0 - Changing the 115 to 120

#define NAVCODETAG "FBCE73DC4D854397A870F53079A0B923120"

#endif

