// stdafx.h : include file for standard system include files,
//  or project specific include files that are used frequently, but
//      are changed infrequently
//

//  *** Note ***
//  When using pre-compiled headers, for some curious reason, you cannot
//  put ifdefs around #include "stdafx.h". The precompiler will complain
//  about the endif. So I've put ifndef NLM _inside_ stdafx.h.
//
#ifndef NLM

#if !defined(AFX_STDAFX_H__6CEB6CB0_D696_11D2_ADDB_00104BF3F36A__INCLUDED_)
#define AFX_STDAFX_H__6CEB6CB0_D696_11D2_ADDB_00104BF3F36A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


// Insert your headers here
#define WIN32_LEAN_AND_MEAN		// Exclude rarely-used stuff from Windows headers

#include <windows.h>

// TODO: reference additional headers your program requires here

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_STDAFX_H__6CEB6CB0_D696_11D2_ADDB_00104BF3F36A__INCLUDED_)

#endif // !NLM
