// stdafx.h : include file for standard system include files,
//  or project specific include files that are used frequently, but
//      are changed infrequently
//

#if !defined(AFX_STDAFX_H__876C58C5_74F5_11D2_BBAE_6EE175000000__INCLUDED_)
#define AFX_STDAFX_H__876C58C5_74F5_11D2_BBAE_6EE175000000__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


// Insert your headers here
#define WIN32_LEAN_AND_MEAN		// Exclude rarely-used stuff from Windows headers

#include <windows.h>


#ifdef UNICODE
	#define _tcout wcout
	#define _tostream wostream
#else
	#define _tcout cout
	#define _tostream ostream
#endif

#include <TCHAR.h>



// TODO: reference additional headers your program requires here

#pragma warning ( disable : 4786 )
#pragma warning ( disable : 4251 )


//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_STDAFX_H__876C58C5_74F5_11D2_BBAE_6EE175000000__INCLUDED_)
