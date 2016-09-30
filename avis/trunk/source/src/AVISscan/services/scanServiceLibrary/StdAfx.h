// stdafx.h : include file for standard system include files,
//  or project specific include files that are used frequently, but
//      are changed infrequently
//

#if !defined(AFX_STDAFX_H__470F1CAF_DC08_11D2_AD8A_00A0C9C71BBC__INCLUDED_)
#define AFX_STDAFX_H__470F1CAF_DC08_11D2_AD8A_00A0C9C71BBC__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define WIN32_LEAN_AND_MEAN		// Exclude rarely-used stuff from Windows headers


// TODO: reference additional headers your program requires here
#include <stdio.h>

#include <windows.h>

#pragma warning(disable: 4786)		// remove warning that std templates expanding
									// past 256 debugging limit (truncated to 256)

#include <string>			// ATK added to remove some bogus error messages
#include <sqlext.h>
#include <map>

#include <CMclGlobal.h>
#include <CMclSemaphore.h>
#include <CMclEvent.h>
#include <CMclAutoLock.h>

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_STDAFX_H__470F1CAF_DC08_11D2_AD8A_00A0C9C71BBC__INCLUDED_)
