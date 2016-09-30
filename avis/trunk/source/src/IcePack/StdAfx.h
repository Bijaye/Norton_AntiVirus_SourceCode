// stdafx.h : include file for standard system include files,
//  or project specific include files that are used frequently, but
//      are changed infrequently
//

#if !defined(AFX_STDAFX_H__CEC1FDFD_00B2_11D3_ADD3_00A0C9C71BBC__INCLUDED_)
#define AFX_STDAFX_H__CEC1FDFD_00B2_11D3_ADD3_00A0C9C71BBC__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

//#define WIN32_LEAN_AND_MEAN		// Exclude rarely-used stuff from Windows headers

#pragma warning ( disable : 4786 )
#pragma warning ( disable : 4251 )

#include <stdio.h>

#include <windows.h>

#include <TCHAR.h>

// TODO: reference additional headers your program requires here
#include <string>			// ATK added to remove some bogus error messages
#include <map>
#include <list>

#include <CMclGlobal.h>
#include <CMclSemaphore.h>
#include <CMclEvent.h>
#include <CMclAutoLock.h>

#include <atlbase.h>
#include <comdef.h>

#include "IcePackEventMsgs.h"

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_STDAFX_H__CEC1FDFD_00B2_11D3_ADD3_00A0C9C71BBC__INCLUDED_)
