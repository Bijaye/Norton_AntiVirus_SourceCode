// stdafx.h : include file for standard system include files,
//  or project specific include files that are used frequently, but
//      are changed infrequently
//

#if !defined(AFX_STDAFX_H__2BC75339_E169_11D2_8A97_00203529AC86__INCLUDED_)
#define AFX_STDAFX_H__2BC75339_E169_11D2_8A97_00203529AC86__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define VC_EXTRALEAN		// Exclude rarely-used stuff from Windows headers

#include <afxwin.h>         // MFC core and standard components
#include <afxext.h>         // MFC extensions
#include <afxdtctl.h>		// MFC support for Internet Explorer 4 Common Controls
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>			// MFC support for Windows Common Controls
#endif // _AFX_NO_AFXCMN_SUPPORT

// give all source files access to configuration profile methods
#include "AVISProfile.h"

// give all source files access to the application class
#include "AVISACSignatureComm.h"

// give all source files access to the configuration macros
#include "config.h"


//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_STDAFX_H__2BC75339_E169_11D2_8A97_00203529AC86__INCLUDED_)
