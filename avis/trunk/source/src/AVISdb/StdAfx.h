// stdafx.h : include file for standard system include files,
//  or project specific include files that are used frequently, but
//      are changed infrequently
//

#if !defined(AFX_STDAFX_H__C68959B1_526D_11D2_88F0_00A0C9DB9E9C__INCLUDED_)
#define AFX_STDAFX_H__C68959B1_526D_11D2_88F0_00A0C9DB9E9C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


#pragma warning ( disable : 4786 )
//#pragma warning ( disable : 4251 )

// Insert your headers here
#define WIN32_LEAN_AND_MEAN		// Exclude rarely-used stuff from Windows headers

#include <windows.h>

//#include <assert.h>
//#define ASSERT	assert

#include <string>			// ATK added to remove some bogus error messages
#include <sqlext.h>
#include <map>

// TODO: reference additional headers your program requires here

#pragma warning(default: 4251)	// try to limit the scope of this a little bit

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.


#endif // !defined(AFX_STDAFX_H__C68959B1_526D_11D2_88F0_00A0C9DB9E9C__INCLUDED_)
