////////////////////////
//
// PROPRIETARY / CONFIDENTIAL.
// Use of this product is subject to license terms.
// Copyright © 2006 Symantec Corporation.
// All rights reserved.
//
/////////////////////////////////////////////////////////////////////////////////////////////////

// NAVOptionsStub.cpp : Defines the entry point for the application.
//

//#include <windows.h>  UNICODE_CHANGE : Replacing windows.h with atlbase.h and atlconv.h
#include <atlbase.h>
#include <atlconv.h>
#include <atlcom.h>
#include "OSInfo.h"
#include "Resource.h"

#define INITIIDS
#include "NAVTrust.h"
#include "NAVOptions.h"
#include "NAVOptions_i.c"
#include "ISVersion.h"

#include "ccLib.h"
#include "ccLibDllLink.h"
#include "ccLibDll.h"
#include "ccSymDelayLoader.h"
#include "ccCoInitialize.h"
#include "ccString.h"

// Element stuff for product info
#include "uiNISDataElementGuids.h"
#include "uiProviderInterface.h"
#include "uiElementInterface.h"
#include "uiNumberDataInterface.h"
#include "uiDateDataInterface.h"
#include "ISDataClientLoader.h"

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

static const WCHAR SWITCH_PAGE[] = L"/page:"; // lowercase
static const WCHAR SWITCH_HWND[] = L"/hwnd:"; // lowercase

EShowPageID g_pageID = ShowPageID_REAL_TIME_PROTECTION;
long g_lhWnd = 0;
bool ProcessSwitch(const wchar_t* szSwitch);

void MessageBox(HINSTANCE hInstance, UINT uiIDS)
{
	CString csTitle, csWarning;
	CString csTempStr;

	csTitle = CISVersion::GetProductName();
	csTempStr.LoadString(hInstance, uiIDS);
	csWarning.Format(csTempStr, csTitle);	

	::MessageBox(::GetDesktopWindow(), csWarning, csTitle, MB_OK);
}

bool IsCfgwizFinished()
{
	CCTRACEI(_T("NavOptionsStub:IsCfgwizFinished() Enter"));

#ifdef _DEBUG
	// allow connection to debugger in debug mode...
	::MessageBox(NULL, L"connect to debugger", L"debug me", 0);
#endif

	// Check business rules for displaying UI
    ui::IProviderPtr pProvider;
    ISShared::ISShared_IProvider providerFactory;
    SYMRESULT sr = providerFactory.CreateObject(GETMODULEMGR(), &pProvider);
    if(SYM_FAILED(sr))
    {
        CCTRCTXE1(_T("Failed to create provider = 0%x80"), sr);
        return false;
    }

    // Can show UI - CfgWiz and EULA
    bool bRet = false;
    ui::IElementPtr pElementCanShow;
    ui::INumberDataQIPtr pDataCanShow;
    if(SYM_SUCCEEDED(pProvider->GetElement(ISShared::CLSID_NIS_CanShowUI, pElementCanShow)))
    {
        ui::IDataPtr pData;
        if(SYM_SUCCEEDED(pElementCanShow->GetData(pData)))
        {
            pDataCanShow = pData;

            LONGLONG i64State = 0;
            if(SYM_SUCCEEDED(pDataCanShow->GetNumber(i64State)))
            {
                if(0 == i64State)
                {
		            CCTRCTXE0(_T("User has not accepted EULA. Abort"));
                    bRet = false;
                }
                else
                {
                    bRet = true;
                    CCTRCTXI0(_T("User has accepted EULA. Go go go."));
                }
            }            

        }
    }

    return bRet;
}

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE, wchar_t* szCmdLine, int iCmdShow)
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

    g_ResLoader.Initialize();

    // Check for Safe Mode
    if(GetSystemMetrics(SM_CLEANBOOT))
    {
        CCTRCTXE0(_T("Running in safe mode - Abort."));
        ::MessageBox(g_ResLoader.GetResourceInstance(), IDS_SAFEMODE);
        return 0;
    }

	HRESULT hr;
	BOOL bIsTrialValid = TRUE;
    ccLib::CCoInitialize coInit(ccLib::CCoInitialize::eSTAModel);

	if(IsCfgwizFinished() == false)			//Defect 1-49XIKE: NavStub.exe launches even when CfgWiz is not complete
	{
		CCTRACEE(_T("Config Wiz has not been completed. NavOptionsStub should not be launched"));
		::MessageBox(g_ResLoader.GetResourceInstance(), IDS_CFGWIZ_NOTCOMPLETE);
		return S_FALSE;
	}

	// Q: Does this user have adminstative rights?
    if (!g_OSInfo.IsAdminProcess())
    {
		::MessageBox(g_ResLoader.GetResourceInstance(), IDS_Wrn_NoneAdmin);
		return S_FALSE;
	}

    // Check for a valid digital signature on the COM Server before
    // loading it
    if( NAVToolbox::NAVTRUST_OK != NAVToolbox::IsSymantecComServer(CLSID_NAVOptions) )
    {
        // Invalid signature...bail out
        return int(E_ACCESSDENIED);
    }

	try
	{
		CComPtr<INAVOptions> spNAVOptions;
		if (SUCCEEDED(hr = spNAVOptions.CoCreateInstance(CLSID_NAVOptions))
			&& SUCCEEDED(hr = spNAVOptions->get_IsTrialValid(&bIsTrialValid)) && bIsTrialValid)
		{
            // If there is a command-line option to load a specific panel
            //
	        for(int iIndex = 1; iIndex < __argc; iIndex++)
	        {
		        // If the argument starts with a forward-slash it's a switch.
		        if(__wargv[iIndex][0] == L'/')
		        {
			        if(ProcessSwitch(__wargv[iIndex]) == false)
			        {
				        CCTRACEE(_T("Bad switch: %s"), __wargv[iIndex]);
			        }
		        }
            }

            if ( 0 == g_lhWnd )
                g_lhWnd = reinterpret_cast<long>(::GetDesktopWindow());

			// We allow only one instance of the options dialog.
			if (S_EXIST == (hr = spNAVOptions->ShowPage(g_lhWnd, g_pageID )))
				::MessageBox(g_ResLoader.GetResourceInstance(), IDS_Wrn_AnotherSession);
		}
	}
    catch(_com_error &ex)
    {
		hr = ex.Error();
        CCTRACEE(L"Failed to init COM\n");
    }

	return int(hr);
}

bool ProcessSwitch(const wchar_t* szSwitch)
{
    ccLib::CString strSwitch (szSwitch);
    strSwitch.MakeLower();
    int iPos = strSwitch.Find (SWITCH_PAGE);
    if(iPos>-1)
    {
	    // Get the panel ID
	    g_pageID = static_cast<EShowPageID>(_wtol(strSwitch.Mid(iPos+wcslen(SWITCH_PAGE))));
        return true;
    }

    iPos = strSwitch.Find (SWITCH_HWND);
    if(iPos>-1)
    {
	    // Get the hWnd
	    g_lhWnd = _wtol(strSwitch.Mid(iPos+wcslen(SWITCH_HWND)));
        return true;
    }

    return false;
}

