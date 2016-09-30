// IEToolBandHelper.cpp : Implementation of CIEToolBandHelper
#include "StdAfx.h"
#define INITIIDS
#include "ScanTask.h"
#include "navtrust.h"       // For loading Sym COM objects
#include "IEToolBandHelper.h"
#include "IEContextMenuHelper.h"
// I don't believe this is needed anymore - Javed 6/5/2005
//#include "DJSMAR00_Static.h"
#include "InstOptsNames.h"
#include "NAVSettingsHelperEx.h"
#include "SSOsinfo.h"
//#include "InstOptsNames.h"
#include "NavUIHelp.h"
#include "NAVUIHTM_Resource.h"
#include "TraceHR.h"
// PEP Headers
#define _V2AUTHORIZATION_SIMON
#define _SIMON_PEP_
#define _AV_COMPONENT_CLIENT
#include "ComponentPepHelper.h"
#include "PepMiddleMan.h"
#include "SuiteOwnerHelper.h"
#include "NAVOptHelperEx.h"
#include "navlnch.h"
#include "AVRESBranding.h"
//#include "PathValidation.h"

// ccLib delay loader
// This will allow the ccLib dll to delay load at run-time
#include "ccLibDllLink.h"
#include "ccSymDelayLoader.h"
ccSym::CDelayLoader g_DelayLoader;

#include "ccTrace.h"
#include "ccSymDebugOutput.h"
ccSym::CDebugOutput g_DebugOutput(_T("NavShCom"));
IMPLEMENT_CCTRACE(::g_DebugOutput);


// CIEToolBandHelper
// onActivityLog() : Launches Activity Log UI
STDMETHODIMP CIEToolBandHelper::onActivityLog(void)
{
	TRACEHR (hrx);
	if( S_OK != (hrx = IsConfigured() ))
		return hrx;

#ifdef IMPERSONATE_CLIENT
	hrx = ::CoImpersonateClient();
	if(FAILED(hrx))	{
		ErrorMsg();
		return hrx;
	}
#endif

    CComPtr <IAppLauncher> spAppLauncher;
    try
    {
        CComBSTR bstrNAVLnch ("Symantec.Norton.AntiVirus.AppLauncher");

        // Check the Navlnch dll for a valid symantec signature
        if( NAVToolbox::NAVTRUST_OK != NAVToolbox::IsSymantecComServer(_T("Symantec.Norton.AntiVirus.AppLauncher")) )
        {
            hrx << E_ACCESSDENIED;
        }
		
        hrx << spAppLauncher.CoCreateInstance ( bstrNAVLnch,NULL,CLSCTX_INPROC_SERVER );
        if ( FAILED (hrx))
            throw false;

        hrx << spAppLauncher->LaunchActivityLog ();
    }
	catch(_com_error& e)
	{
		hrx = e;
	}
	catch(...)
	{
        hrx = E_UNEXPECTED;
		CCTRACEE("*** Error");
	}
#ifdef IMPERSONATE_CLIENT
	CoRevertToSelf();
#endif
	return hrx;
}
// onVirusDef() : Launches SARC URL
STDMETHODIMP CIEToolBandHelper::onVirusDef(void)
{
	TRACEHR (hrx);
	if( S_OK != (hrx = IsConfigured() ))
		return hrx;
#ifdef IMPERSONATE_CLIENT
	hrx = ::CoImpersonateClient();
	if(FAILED(hrx))
	{
		ErrorMsg();
		return hrx;
	}
#endif

	try
	{
		CBrandingRes BrandRes;
		CString csURL(BrandRes.GetString(IDS_SARC_VIRUS_ENCYCLOPEDIA_URL));

		// Launch SARC URL
		hrx << ((HINSTANCE(32) >= ::ShellExecute(NULL, NULL, csURL, NULL, NULL, SW_SHOWNORMAL)) ? E_FAIL : S_OK);
	}
	catch(_com_error& e)
	{
#ifdef IMPERSONATE_CLIENT
	CoRevertToSelf();
#endif
		hrx = Error(IDS_Err_SARC_URL, IID_IDeskBand);
	}

#ifdef IMPERSONATE_CLIENT
	CoRevertToSelf();
#endif

	return hrx;
}


//navigate2NMAINPanel: Launches Nmain.exe integrator UI
STDMETHODIMP CIEToolBandHelper::navigate2NMAINPanel(BSTR bstrPanelName)
{
//	MessageBox(NULL,"navigate2NMAINPanel()","Break",MB_OK);

	TRACEHR (hrx);
	CCTRACEI(_T("CIEToolBandHelper::navigate2NMAINPanel(%s)"),CW2A(bstrPanelName));

#ifdef IMPERSONATE_CLIENT
	hrx = ::CoImpersonateClient();
	if(FAILED(hrx))	{
		ErrorMsg();
		return hrx;
	}
#endif
	if( S_OK != (hrx = IsConfigured() ))
		return hrx;

	TCHAR szNAV[128];
	// Build the path to nmain
	TSTRING strPath = _T("\"");
	TCHAR szNisPlug[MAX_PATH] = {0};
	try
	{
		strPath += g_NAVInfo.GetSymantecCommonDir();
		strPath += _T("\\nmain.exe\"");

		// Build a command line for invoking NAVUI
		TSTRING strCmdLine = _T("/dat:");
		DWORD dwSize = sizeof(szNisPlug)/sizeof(TCHAR);
		if(SUCCEEDED(NAVUIHelp::GetPathToNisPlugin(szNisPlug, dwSize))) {
			strCmdLine += szNisPlug;
		}
		else {
			strCmdLine += ::g_NAVInfo.GetNAVDir();
			strCmdLine += _T("\\navui.nsi ");
		}

		DJSMAR_LicenseType licType = DJSMAR_LicenseType_Violated;
		{
			STAHLSOFT_HRX_TRY(hr)
			{
				// Getting licensing and subscription properties needed to enable product features
				CPEPClientBase pepBase;
				hr << pepBase.Init();
				pepBase.SetContextGuid(clt::pep::CLTGUID);
				pepBase.SetComponentGuid(V2LicensingAuthGuids::SZ_ANTIVIRUS_COMPONENT);
				pepBase.SetPolicyID(clt::pep::POLICY_ID_QUERY_LICENSE_RECORD);
				pepBase.SetCollectionDWORD(clt::pep::SZ_PROPERTY_DISPOSITION,clt::pep::DISPOSITION_NONE);
				pepBase.SetCollectionDWORD(SUBSCRIPTION_STATIC::SZ_SUBSCRIPTION_PROPERTY_DISPOSITION,SUBSCRIPTION_STATIC::DISPOSITION_NONE);

				hr << pepBase.QueryPolicy();

				// Getting licensing properties
				hr << pepBase.GetValueFromCollection(DRM::szDRMLicenseType, (DWORD &)licType, DJSMAR_LicenseType_Violated);
			}
			STAHLSOFT_HRX_CATCH_ALL_NO_RETURN(hr)
		}
		if(licType == DJSMAR_LicenseType_Rental)
			strCmdLine += _T("/nosysworks ");

		CBrandingRes BrandRes;		
		CString csPanelSelection = CW2A(bstrPanelName, CP_ACP);
		CString csScanMsg, csStatusMsg, csPannelName = _T("Status");
		GetResourceString(IDS_SCAN,csScanMsg);
		GetResourceString(IDS_STATUS,csStatusMsg);

		if( !csPanelSelection.Compare(csScanMsg) )
		{
			csPannelName = BrandRes.GetString(IDS_SCAN_CATEGORYTITLE);
		}
		else if( !csPanelSelection.Compare(csStatusMsg) )
		{
			csPannelName = BrandRes.GetString(IDS_STATUS_CATEGORYTITLE);
		}

		strCmdLine += _T(" /goto:");
		strCmdLine += m_csProductName;
		strCmdLine += _T("\\");
		strCmdLine += csPannelName;

		HINSTANCE hInstance = ShellExecute(NULL, _T("open"), strPath.c_str(), strCmdLine.c_str(), ::g_NAVInfo.GetNAVDir(), SW_SHOWNORMAL);

		if( reinterpret_cast<int>(hInstance) <= 32 ) {
			// Display an error if we can't launch the integrator
			CString csError;
			hrx = ErrorFromResource(IDS_Err_Cant_Launch_Nmain,IID_IDeskBand,csError);
			MessageBox(NULL,csError, CHelper::m_csProductName, MB_OK | MB_ICONERROR);
		}
	}
	catch(_com_error& e) {
		hrx = e;
	}
	catch(...) {
        hrx = E_UNEXPECTED;
		CCTRACEE("*** Error");
	}
	return hrx;
}

STDMETHODIMP CIEToolBandHelper::onQuarantine(void)
{
	TRACEHR (hrx);

#ifdef IMPERSONATE_CLIENT
	hrx = ::CoImpersonateClient();
	if(FAILED(hrx))	{
		ErrorMsg();
		return hrx;
	}
#endif
	if( S_OK != (hrx = IsConfigured() ))
		return hrx;

	TSTRING strPath = ::g_NAVInfo.GetNAVDir();

	// TODO: Change path
	strPath += _T("\\QCONSOLE.exe");
	::WinExec(strPath.c_str(), SW_SHOW);

	return hrx;
}

#ifdef IMPERSONATE_CLIENT
void	ErrorMsg()
{
	if( IDYES == MessageBox(NULL,"Failed Impersonation, break?","ERROR",MB_YESNO))	{
		DebugBreak();
	}
}
#endif