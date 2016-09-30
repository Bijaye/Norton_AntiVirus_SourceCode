// NAVOptionsStub.cpp : Defines the entry point for the application.
//

#include <windows.h>
#include <atlbase.h>
#include <atlcom.h>
#include "OSInfo.h"
#include "Resource.h"

#define INITIIDS
#include "NAVTrust.h"
#include "NAVOptions.h"
#include "NAVOptions_i.c"
#include "ScriptSafeVerifyTrustCached.h"		// For scripting security
#include "AVRESBranding.h"

#include "ccLibStd.h"
#include "ccLibDllLink.h"
#include "ccLibDll.h"
#include "ccSymDelayLoader.h"

//Headers required for IsCfgwizFinished()
#include "Simon.h"
#include "InstOptsNames.h"
#define _INIT_COSVERSIONINFO
// PEP Headers
#define _V2AUTHORIZATION_SIMON
#include "PepMiddleMan.h"

#include "SuiteOwnerHelper.h"
#include "NAVOptHelperEx.h"
//End of Headers required for IsCfgwizFinished()

// ccLib delay loader
// This will allow the ccLib dll to delay load at run-time
ccSym::CDelayLoader g_DelayLoader;

// Debug output
#include "ccSymDebugOutput.h"
ccSym::CDebugOutput g_DebugOutput(_T("Navstub"));
IMPLEMENT_CCTRACE(::g_DebugOutput);

// Crash handler
#include "ccSymCrashHandler.h"
ccSym::CCrashHandler g_CrashHandler;

COSInfo g_OSInfo;
//Resource loading
#include "..\navoptionsstubres\resource.h"
#include "ccResourceLoader.h"
#define SYMC_RESOURCE_DLL _T("navstub.loc")
CAtlBaseModule _Module;
::cc::CResourceLoader g_ResLoader(&_Module, SYMC_RESOURCE_DLL);

void MessageBox(HINSTANCE hInstance, UINT uiIDS)
{
	CString csTitle, csWarning;
	CBrandingRes BrandRes;
	CString csTempStr;

	csTitle = BrandRes.ProductName();
	csTempStr.LoadString(hInstance, uiIDS);
	csWarning.Format(csTempStr, csTitle);	

	::MessageBox(::GetDesktopWindow(), csWarning, csTitle, MB_OK);
}

bool IsCfgwizFinished()
{
	CCTRACEI("NavOptionsStub:IsCfgwizFinished() Enter");

	bool bFinished = false;	
	CString csCfgWizDat;

	if(SuiteOwnerHelper::GetOwnerCfgWizDat(csCfgWizDat))
	{
		CNAVOptFileEx CfgWizOpts;
		if(CfgWizOpts.Init(csCfgWizDat, FALSE))
		{
			DWORD dwValue = 0;
			CfgWizOpts.GetValue(InstallToolBox::CFGWIZ_Finished, dwValue, 0);
			
			if(dwValue == 1)
			{
				bFinished = true;
			}
		}
	
	}
	CCTRACEI("NavOptionsStub::IsCfgwizFinished() Exit(%s)", bFinished ? "TRUE" : "FALSE");

	return bFinished;
}

int APIENTRY WinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPSTR     lpCmdLine,
                     int       nCmdShow)
{
    // Attach ccLib DLL TRACE output to this debug output
    ::ccLib::GetDebugOutput()->SetRedirectOutput(&g_DebugOutput);

    // Load debug options
    if (g_DebugOutput.LoadOptions() == FALSE)
    {
        CCTRACEE(_T("WinMain : g_DebugOutput.LoadOptions() == FALSE"));
    }

    // Enable the crash handler
    if (g_CrashHandler.LoadOptions() == FALSE)
    {
        CCTRACEE(_T("WinMain : g_CrashHandler.LoadOptions() == FALSE"));
    }

	HRESULT hr;
	BOOL bCanChange, bIsTrialValid;
	g_ResLoader.Initialize();

	if(IsCfgwizFinished() == false)			//Defect 1-49XIKE: NavStub.exe launches even when CfgWiz is not complete
	{
		CCTRACEE("Config Wiz has not been completed. NavOptionsStub should not be launched");
		::MessageBox(g_ResLoader.GetResourceInstance(), IDS_CFGWIZ_NOTCOMPLETE);
		return S_FALSE;
	}

	// Q: Does this user have adminstative rights?
    if (!g_OSInfo.IsAdminProcess())
    {
		::MessageBox(g_ResLoader.GetResourceInstance(), IDS_Wrn_NoneAdmin);
		return S_FALSE;
	}

	class CCOM  // COM environment setup / cleanup
	{
	public:
		HRESULT m_hr;

		CCOM(void) : m_hr(::CoInitialize(NULL)) {}
		~CCOM(void) { ::CoUninitialize(); }

		operator bool(void) { return SUCCEEDED(m_hr); }
	} com;
	CComPtr<INAVOptions> spNAVOptions;
    CComPtr<ISymScriptSafe> spSymScriptSafe;

    // Check for a valid digital signature on the COM Server before
    // loading it
    if( NAVToolbox::NAVTRUST_OK != NAVToolbox::IsSymantecComServer(CLSID_NAVOptions) )
    {
        // Invalid signature...bail out
        return int(E_ACCESSDENIED);
    }

	if (!com)  // Q: Did COM initialized correctly?
		return int(com.m_hr);

	if (SUCCEEDED(hr = spNAVOptions.CoCreateInstance(CLSID_NAVOptions))
	 && SUCCEEDED(hr = spNAVOptions->get_CanChange(&bCanChange)) && bCanChange
	 && SUCCEEDED(hr = spNAVOptions->get_IsTrialValid(&bIsTrialValid)) && bIsTrialValid)
	{
        // Set script safety
        if ( SUCCEEDED ( spNAVOptions.QueryInterface(&spSymScriptSafe)))
		{
			long dwAccess = ISYMSCRIPTSAFE_UNLOCK_KEYA;
			long dwKey = ISYMSCRIPTSAFE_UNLOCK_KEYB;
			spSymScriptSafe->SetAccess(dwAccess, dwKey);
		}

		// We allow only one instance of the options dialog.
		if (S_EXIST == (hr = spNAVOptions->Show(reinterpret_cast<long>(GetDesktopWindow())))
		 && g_OSInfo.IsTerminalServicesInstalled())
			::MessageBox(g_ResLoader.GetResourceInstance(), IDS_Wrn_AnotherSession);
	}

	return int(hr);
}
