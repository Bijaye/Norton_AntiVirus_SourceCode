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

#if !defined(AFX_STDAFX_H__90ED1506_C54D_4BEC_AF59_D997526DF687__INCLUDED_)
#define AFX_STDAFX_H__90ED1506_C54D_4BEC_AF59_D997526DF687__INCLUDED_

// Change these values to use different versions
#define WINVER		0x0500
#define _WIN32_WINNT	0x0501
#define _WIN32_IE	0x0501
#define _RICHEDIT_VER	0x0200
#define _ANIMATION
#define _SHOWINSTALLNAMES
#define _WIN32_MSI 200
#define _USE_CC_CRASH_HANDLER

#define SYMHTML_MODULE_PATHPROVIDER // tell SymHTML to load DLL from working folder
#define SYMTHEME_MODULE_PATHPROVIDER // tell SymTheme to load DLL from working folder

#include <TCHAR.h>
#include <atlstr.h>
#include <atlbase.h>
#define _WTL_NO_CSTRING
#include <atlapp.h>

extern CAppModule _Module;

#include <atlmisc.h>
#include <atlwin.h>

#include <atlframe.h>
#include <atlctrls.h>
#include <atldlgs.h>
#include <atlctrlw.h>
#include <vector>
#include <map>
#include <msi.h>
#include <msiquery.h>

//MSI ToolBox
#include "MsiDebugLog.h"
#include <stahlsoft.h>

#include "shfolder.h"
#include "shlobj.h"

#include "setupdata.h"
using namespace SetupData;

#include "cltLicenseConstants.h"

//Wizard pages
#include "SetupPropertySheet.h"
#include "BasePropPage.h"
#include "DefaultWelcomePage.h"
#include "DefaultModifyPage.h"
#include "DefaultBrowsePage.h"
#include "DefaultOptionPage.h"
#include "DefaultVerifyRemovePage.h"
#include "DefaultFinishPage.h"
#include "DefaultActivationPage.h"
#include "DefaultScanPage.h"
#include "DefaultStartInstallPage.h"
#include "NAVSubscriptionWarningPage.h"
#include "ICFRestorePage.h"

// these must be changed by each product
#include "navsetupinfo.h"
#include "NAVBrowsePage.h"

#include "ccLib.h"
#include "ccTrace.h"

//Used for pragma message outputs.
#undef STRIZE
#define STRIZE(x) #x
#undef PPSTRIZE
#define PPSTRIZE(x) STRIZE(x)
#undef FILE_LINE
#define FILE_LINE _T(__FILE__) _T("(") _T(PPSTRIZE(__LINE__)) _T(") : ")
#define AUTO_FUNCNAME _T(__FILE__) _T("(") _T(PPSTRIZE(__LINE__)) _T(") : ") _T(__FUNCTION__) _T(":  ")

#define PRINTFUNCTION _T(__FUNCTION__)

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_STDAFX_H__90ED1506_C54D_4BEC_AF59_D997526DF687__INCLUDED_)
