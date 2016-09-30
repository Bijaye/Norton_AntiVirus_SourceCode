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


#include <TCHAR.h>
#include <atlbase.h>
#define _WTL_NO_CSTRING
#include <atlapp.h>

extern CAppModule _Module;

#include <atlstr.h>
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

//Wizard pages
#include "SetupPropertySheet.h"
#include "BasePropPage.h"
#include "DefaultWelcomePage.h"
#include "DefaultModifyPage.h"
#include "DefaultBrowsePage.h"
#include "DefaultVerifyRemovePage.h"
#include "DefaultFinishPage.h"
#include "DefaultActivationPage.h"
#include "DefaultScanPage.h"

#include "setupdata.h"
using namespace SetupData;

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
#define FILE_LINE __FILE__ "(" PPSTRIZE(__LINE__) ") : "
#define AUTO_FUNCNAME __FILE__ "(" PPSTRIZE(__LINE__) ") : " __FUNCTION__ ":  "

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_STDAFX_H__90ED1506_C54D_4BEC_AF59_D997526DF687__INCLUDED_)
