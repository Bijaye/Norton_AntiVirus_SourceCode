// IEToolBandObj.cpp : Implementation of CIEToolBand
#include "StdAfx.h"
#define INITIIDS
#include "ScanTask.h"
#include "navtrust.h"       // For loading Sym COM objects

#include "NAVShellExt.h"
#include "IEToolBandObj.h"
#include "cltLicenseConstants.h"
#include "InstOptsNames.h"
#include "NAVSettingsHelperEx.h"
#include "SSOsinfo.h"
#include "InstOptsNames.h"
#include "NavUIHelp.h"
#include "AVRESBranding.h"

// PEP Headers
#define _V2AUTHORIZATION_SIMON
#define _SIMON_PEP_
#define _AV_COMPONENT_CLIENT
#include "ComponentPepHelper.h"
#include "PepMiddleMan.h"

#include "SuiteOwnerHelper.h"
#include "NAVOptHelperEx.h"

// ccLib delay loader
// This will allow the ccLib dll to delay load at run-time
#include "ccLibDllLink.h"
#include "ccSymDelayLoader.h"
ccSym::CDelayLoader g_DelayLoader;

#include "ccTrace.h"
#include "ccSymDebugOutput.h"
ccSym::CDebugOutput g_DebugOutput(_T("NavShExt"));
IMPLEMENT_CCTRACE(::g_DebugOutput);

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

const DWORD TOOLBAR_STYLE = WS_CHILD         /* Window styles: */ 
                          | WS_CLIPSIBLINGS
                          | WS_VISIBLE
                          | WS_TABSTOP       /* Note: WS_TABSTOP allows us to get tabbed to */
                          | TBSTYLE_TOOLTIPS /* Toolbar styles: */
                          | TBSTYLE_FLAT
                          | TBSTYLE_TRANSPARENT
//                          | TBSTYLE_LIST
//                          | TBSTYLE_WRAPABLE
                          | CCS_TOP          /* Common Control styles: */ 
                          | CCS_NODIVIDER
                          | CCS_NOPARENTALIGN
                          | CCS_NORESIZE;


const TBBUTTON TOOLBARBUTTONS[1] = { { 0               // iBitmap;
                                     , IDC_TOOLBAR     // idCommand;
                                     , TBSTATE_ENABLED // fsState;
                                     , TBSTYLE_BUTTON  // fsStyle;
                                     | TBSTYLE_DROPDOWN
                                     ,
                                     } };

/////////////////////////////////////////////////////////////////////////////
// CIEToolBand
CIEToolBand::CIEToolBand(void) : m_dwBandID(0UL)
                               , m_dwViewMode(0UL)
                               , m_bCanTouchThisSite(true)
                               , m_BrowserEvents(*this)
                               , m_hBitmap(HBITMAP(::LoadImage(_Module.GetResourceInstance(), MAKEINTRESOURCE(IDB_IETOOLBAR)
	                                                                                        , IMAGE_BITMAP
	                                                                                        , 0, 0
	                                                                                        , LR_LOADTRANSPARENT
	                                                                                        | LR_LOADMAP3DCOLORS)))
{
	CBrandingRes BrandRes;
	m_csProductName = BrandRes.ProductName();
}

void CIEToolBand::FinalRelease(void)
{
	m_BrowserEvents.Unadvise();
	::DeleteObject(HGDIOBJ(m_hBitmap));
}


/////////////////////////////////////////////////////////////////////////////
// CIEToolBand Message Processing
LRESULT CIEToolBand::onScan(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	HRESULT hr;
	
	if (FAILED(hr = scan()))
		ShowError(hr);

	return 0L;
}

LRESULT CIEToolBand::onLaunch(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	return navigate2NMAINPanel(IDS_SCAN);
}

LRESULT CIEToolBand::onStatus(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	return navigate2NMAINPanel(IDS_STATUS);
}

LRESULT CIEToolBand::onQuarantine(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	TSTRING strPath = ::g_NAVInfo.GetNAVDir();

	// TODO: Change path
	strPath += _T("\\QCONSOLE.exe");
	::WinExec(strPath.c_str(), SW_SHOW);

	return 0L;
}

LRESULT CIEToolBand::onActivityLog(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	HRESULT hr;

    // Get the activity log object to display a report

    CComPtr <IAppLauncher> spAppLauncher;
    try
    {
        CComBSTR bstrNAVLnch ("Symantec.Norton.AntiVirus.AppLauncher");

        // Check the Navlnch dll for a valid symantec signature
        if( NAVToolbox::NAVTRUST_OK != NAVToolbox::IsSymantecComServer(_T("Symantec.Norton.AntiVirus.AppLauncher")) )
        {
            hr = E_ACCESSDENIED;
            throw false;
        }
		
        hr = spAppLauncher.CoCreateInstance ( bstrNAVLnch,
                                              NULL,
  		                                      CLSCTX_INPROC_SERVER );
        if ( FAILED (hr))
            throw false;

        hr = spAppLauncher->LaunchActivityLog ();
    }
    catch(...)
    {
    }

    if ( FAILED (hr))
        ShowError(hr);

	return 0L;
}

LRESULT CIEToolBand::onVirusDef(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	HRESULT hr;
	
	if (FAILED(hr = virusDef()))
		ShowError(hr);

	return 0L;
}

LRESULT CIEToolBand::onDropDown(int idCtrl, LPNMHDR pnmh, BOOL& bHandled)
{
	TPMPARAMS tpm = { sizeof(TPMPARAMS) };
	LPNMTOOLBAR pnmtb = reinterpret_cast<LPNMTOOLBAR>(pnmh);

	m_ToolBar.SendMessage(TB_GETRECT, static_cast<WPARAM>(pnmtb->iItem), reinterpret_cast<LPARAM>(&tpm.rcExclude));
	m_ToolBar.MapWindowPoints(HWND_DESKTOP, reinterpret_cast<LPPOINT>(&tpm.rcExclude), 2);

	HMENU hMenuLoaded = LoadMenu(_Module.GetResourceInstance(), MAKEINTRESOURCE(IDM_IETOOLBAR))
	    , hPopupMenu  = GetSubMenu(hMenuLoaded, 0);
	::TrackPopupMenuEx(hPopupMenu, TPM_LEFTALIGN
	                             | TPM_LEFTBUTTON
	                             | TPM_VERTICAL
	                             , tpm.rcExclude.left
	                             , tpm.rcExclude.bottom
	                             , *this, &tpm);
	::DestroyMenu(hMenuLoaded);

	return TBDDRET_DEFAULT;
}

/////////////////////////////////////////////////////////////////////////////
// CIEToolBand Implementations
HRESULT CIEToolBand::scan(void)
{
	HRESULT hr;

	TCHAR szPath[MAX_PATH*2];
	CComPtr<IShellView> spView;
	CComPtr<IDataObject> spDataObj;
    ccLib::CMessageLock msgLock (TRUE, TRUE);

	for (int trys = 0; CBrowserEvents::Navigate_Started == m_BrowserEvents.NavigateState(); trys++)
	{
		VARIANT_BOOL bBusy;

		// On some versions of Win9x when trying to navigate to an item that is
		// not on-line such as a removable drive, the browser gets confused
		// and forgets the current location. If the browser is not busy we'll
		// assume that the navigation is completed and try to scan the current
		// item.
		if (FAILED(hr = m_spBrowser->get_Busy(&bBusy)))
			return hr;

		if (!bBusy)
			break;

		// Q: Are we ready to display it yet.
		if (trys > 10)
			return messageBox(IDS_NAVIGATION);

        // Wait for it to get ready before we retry.
        // We must pump messages!
        //
        ccLib::CEvent eventNothing;
        eventNothing.Create ( NULL, FALSE, FALSE, NULL, FALSE);
        msgLock.Lock ( eventNothing, 1000 );
	}

	// If both TreeView as well as ListView are selected use the TreeView selection
	if (doesTreeViewHasFocus())
	{
		// TreeView selection
		switch(hr = GetTreeViewSelection(m_spBrowser, m_spShellBrowser, szPath))
		{
		case S_OK:
			break;
		case S_FALSE:
			// Means nothing is selected.
			return messageBox(IDS_NOITEMS);
		default:
			return hr;
		}

		// Do the actual work.
		switch(hr = Add2List(szPath))
		{
		case S_OK:
			return RunNAVW();
		case S_FALSE:
			// Means un-scannable selection.
			return messageBox(IDS_NOITEMS);
		default:
			return hr;
		}
	}

	if (!m_bCanTouchThisSite)
		// Means we may crash.
		return messageBox(IDS_NOITEMS);

	// Get the view object.
	if (FAILED(hr = m_spShellBrowser->QueryActiveShellView(&spView)))
		return hr;

	// Get the list of selected items.
	if (FAILED(hr = spView->GetItemObject(SVGIO_SELECTION, IID_IDataObject, (void**) &spDataObj)))
	{
		// Means nothing is selected.
		return messageBox(IDS_NOITEMS);
 	}

	// ListView selection
	if(FAILED(hr = GetSelectionList(spDataObj)))
		return hr;

	return RunNAVW();
}

bool CIEToolBand::doesTreeViewHasFocus(void)
{
	HRESULT hr;
	HWND hWnd, hFocus = GetFocus();

	// Q: Does the TreeView has the focus?
	if (FAILED(hr = m_spShellBrowser->GetControlWindow(FCW_TREE, &hWnd)))
	{
		// Microsoft did a terible job at implementing IShellBrowser::GetControlWindow().
		// This call may fail on IE5, and even IE6, for no good reason. This is why
		// we make one last attempt to see if we are focused on the TreeView control.
		TCHAR szClassName[128];

		// Q: Does the tree control has the focus.
		// Note: We make the unsafe assumption that there's only
		//       one tree control.
		::GetClassName(hFocus, szClassName, SIZEOF(szClassName));
		if (::_tcsicmp(szClassName, WC_TREEVIEW))
			// Means nothing is selected.
			return false;
	}
	else if (hFocus != hWnd) // Abort!
		return false;

	return true;
}

HRESULT CIEToolBand::messageBox(UINT uiIDS)
{
	CString csFormat;
	CString csMessage;

	csFormat.LoadString(uiIDS);
	csMessage = csFormat;

	switch(uiIDS)
	{
	case IDS_NAVIGATION:
	case IDS_NOITEMS:
		csMessage.Format(csFormat, m_csProductName);
		break;
	default:
		break;
	}

	MessageBox(csMessage, m_csProductName, MB_ICONEXCLAMATION | MB_OK);

	return S_FALSE;
}

HRESULT CIEToolBand::virusDef(void)
{
	try
	{
		StahlSoft::HRX hrx;
		CBrandingRes BrandRes;
		CString csURL(BrandRes.GetString(IDS_SARC_VIRUS_ENCYCLOPEDIA_URL));

		// Launch SARC URL
		hrx << ((HINSTANCE(32) >= ::ShellExecute(m_hWnd, NULL, csURL, NULL, NULL, SW_SHOWNORMAL)) ? E_FAIL : S_OK);
	}
	catch(_com_error& e)
	{
		return Error(IDS_Err_SARC_URL, IID_IDeskBand);
	}

	return S_OK;
}

LRESULT CIEToolBand::navigate2NMAINPanel(int nPanelResID)
{
	if(!IsCfgwizFinished())
	{
		LaunchCfgwiz();
		return S_OK;
	}

	TCHAR szPanelName[128];
	// Build the path to nmain
	TSTRING strPath = _T("\"");
	strPath += g_NAVInfo.GetSymantecCommonDir();
	strPath += _T("\\nmain.exe\"");

	// Build a command line for invoking NAVUI
	TSTRING strCmdLine = _T("/dat:");

	TCHAR szNisPlug[MAX_PATH] = {0};
	DWORD dwSize = sizeof(szNisPlug)/sizeof(TCHAR);

	if(SUCCEEDED(NAVUIHelp::GetPathToNisPlugin(szNisPlug, dwSize)))
	{
		strCmdLine += szNisPlug;
	}
	else
	{
		strCmdLine += ::g_NAVInfo.GetNAVDir();
		strCmdLine += _T("\\navui.nsi ");
	}

	DJSMAR_LicenseType licType = DJSMAR_LicenseType_Violated;
    
    STAHLSOFT_HRX_TRY(hr)
    {
        // Getting licensing and subscription properties needed to enable product features
        CPEPClientBase pepBase;
        hrx << pepBase.Init();
        pepBase.SetContextGuid(clt::pep::CLTGUID);
        pepBase.SetComponentGuid(V2LicensingAuthGuids::SZ_ANTIVIRUS_COMPONENT);
        pepBase.SetPolicyID(clt::pep::POLICY_ID_QUERY_LICENSE_RECORD);
        pepBase.SetCollectionDWORD(clt::pep::SZ_PROPERTY_DISPOSITION,clt::pep::DISPOSITION_NONE);
        pepBase.SetCollectionDWORD(SUBSCRIPTION_STATIC::SZ_SUBSCRIPTION_PROPERTY_DISPOSITION,SUBSCRIPTION_STATIC::DISPOSITION_NONE);

        hrx << pepBase.QueryPolicy();

        // Getting licensing properties
        hrx << pepBase.GetValueFromCollection(DRM::szDRMLicenseType, (DWORD &)licType, DJSMAR_LicenseType_Violated);
    }
    STAHLSOFT_HRX_CATCH_ALL_NO_RETURN(hr)

	if(licType == DJSMAR_LicenseType_Rental)
		strCmdLine += _T("/nosysworks ");

	::LoadString(_Module.GetResourceInstance(), nPanelResID, szPanelName, SIZEOF(szPanelName));

	strCmdLine += _T(" /goto:");
	strCmdLine += m_csProductName;
	strCmdLine += _T("\\");
	strCmdLine += szPanelName;

	HINSTANCE hInstance = ShellExecute(NULL, _T("open"), strPath.c_str(), strCmdLine.c_str(), ::g_NAVInfo.GetNAVDir(), SW_SHOWNORMAL);

	if( reinterpret_cast<int>(hInstance) <= 32 )
	{
		// Display an error if we can't launch the integrator
		CString csFormat;
		CString csError;

		csFormat.LoadString(_Module.GetResourceInstance(), IDS_Err_Cant_Launch_Nmain);
		csError.Format(csFormat, m_csProductName);

		MessageBox(csError, m_csProductName, MB_OK | MB_ICONERROR);
	}

	return 0L;
}

void CIEToolBand::ShowError(HRESULT hr)
{
	USES_CONVERSION;
    HRESULT _hr;
    
    CComBSTR sbDesc   = _T("Error in IEToolBar Object (HRESULT = ");
    CComPtr<IErrorInfo> spErrorInfo;
    
    // Check if ErrorInfo is available
	if (S_OK == (_hr = ::GetErrorInfo(0UL, &spErrorInfo)))
	{
//		if (SUCCEEDED(_hr = spErrorInfo->GetSource(&sbSource)))
//        {}
		_hr = spErrorInfo->GetDescription(&sbDesc);
	}
	else  // Format our own (cheesey) message
	{
		TCHAR szTmp[16];

		sbDesc += _ltot(hr, szTmp, 16);
		sbDesc += _T(")");
	}

	MessageBox(OLE2A(sbDesc), m_csProductName,MB_OK | MB_ICONERROR);
}

void CIEToolBand::canTouchThisSite(BSTR bstrURL)
{
	static WCHAR s_wcDontTouchMe[128];

	if (g_OSInfo.IsWinNT())
	{
		m_bCanTouchThisSite = true;
		return;
	}

	/*
	 * Under Win9x we get a trible crash if IShellView::GetItemObject(...)
	 * is called for Dial-up Networking folder. In order to avoid emberssing
	 * Microsoft we compare the URL at the end of every navigation to
	 * the name used for the DUN folder. If we got a match we won't touch
	 * it even with a 10' poll.
	 */

	// First time initalization
	if (!*s_wcDontTouchMe)
	{
		USES_CONVERSION;
		TCHAR szDontTouchMe[128];
		DWORD dwDontTouchMe = SIZEOF(szDontTouchMe);
		CRegKey DUN;

		// Find the name of the Dial-up Networking folder
		if (ERROR_SUCCESS == DUN.Open(HKEY_CLASSES_ROOT, _T("CLSID\\{992CFFA0-F557-101A-88EC-00DD010CCC48}"))
		 && ERROR_SUCCESS == DUN.QueryValue(szDontTouchMe, NULL, &dwDontTouchMe))
		 ::wcscpy(s_wcDontTouchMe, T2OLE(szDontTouchMe));
	}

	m_bCanTouchThisSite = (::wcscmp(s_wcDontTouchMe, bstrURL) != 0);
}

/////////////////////////////////////////////////////////////////////////////
// ISupportsErrorInfo
/////////////////////////////////////////////////////////////////////////////
// ISupportsErrorInfo::InterfaceSupportsErrorInfo()
STDMETHODIMP CIEToolBand::InterfaceSupportsErrorInfo(REFIID riid)
{
	return InlineIsEqualGUID(IID_IDeskBand, riid)
		|| InlineIsEqualGUID(IID_IInputObject, riid)
		|| InlineIsEqualGUID(IID_IPersistStream, riid) ? S_OK : S_FALSE;
}


/////////////////////////////////////////////////////////////////////////////
// IObjectWithSite
/////////////////////////////////////////////////////////////////////////////
// IObjectWithSite::SetSite()
STDMETHODIMP CIEToolBand::SetSite(IUnknown* punkSite)
{
	HRESULT hr;
	RECT  rc;
	HWND hwndParent = NULL;
	TCHAR szToolTip[32] = _T("");
	CComPtr<IOleWindow> spOleWindow;
	CComPtr<IServiceProvider> spSP;

	if (FAILED(hr = IObjectWithSiteImpl<CIEToolBand>::SetSite(punkSite)))
		return hr;

	if (!m_spUnkSite)
		return S_FALSE;

	// Find the IWebBrowser2 interface so we can trap the navigations
	if (FAILED(hr = m_spUnkSite.QueryInterface(&spSP))
	 || FAILED(hr = spSP->QueryService(IID_IWebBrowserApp, IID_IWebBrowser2, (void **)&m_spBrowser)))
		return hr;

	if (FAILED(hr = m_spUnkSite->QueryInterface(&spOleWindow))
	 || FAILED(hr = spOleWindow->GetWindow(&hwndParent)))
		return hr;

	if(!hwndParent)
		return Error(IDS_Err_NoParentWindow, IID_IObjectWithSite);

	::GetClientRect(hwndParent, &rc);

	// Create a window to recieve toolbar notification
	if (!Create(hwndParent, rc, _T("ToolBand"), WS_CHILD))
		return Error(IDS_Err_CreateToolBand, IID_IObjectWithSite);

	// Create a toolbar window
	HWND hToolBar = ::CreateToolbarEx( *this                  // Parent window
	                                 , TOOLBAR_STYLE          // Style
									 , IDC_TOOLBAR            // Control ID
									 , 1                      // Number of bitmaps
									 , NULL                   // HINSTANCE
									 , UINT(m_hBitmap)        // Bitmap resource ID
									 , TOOLBARBUTTONS         // Buttons info
									 , SIZEOF(TOOLBARBUTTONS) // Number of buttons
									 , 0                      // Button width
									 , 0                      // Button hight
									 , 0                      // Bitmap width
									 , 0                      // Bitmap hight
									 , sizeof(TBBUTTON) );
	if (!hToolBar)
		return Error(IDS_Err_CreateToolBar, IID_IObjectWithSite);

	// Finish toolbar setup
	m_ToolBar.Attach(hToolBar);
	m_ToolBar.SendMessage(TB_SETMAXTEXTROWS);
	 // Create the tooltip text
	::LoadString(_Module.GetResourceInstance(), IDS_TOOLTIP, szToolTip, SIZEOF(szToolTip));
	m_ToolBar.SendMessage(TB_ADDSTRING, 0, reinterpret_cast<LPARAM>(szToolTip));
	m_ToolBar.SendMessage(TB_SETEXTENDEDSTYLE, 0, TBSTYLE_EX_DRAWDDARROWS
	                                            | m_ToolBar.SendMessage(TB_GETEXTENDEDSTYLE));
	m_ToolBar.ShowWindow(SW_SHOW );

	// Get the Service Provider interface so we can get the browser window.
	m_spShellBrowser.Release();
	CComPtr<IServiceProvider> psp;
	if (SUCCEEDED(hr = m_spUnkSite->QueryInterface(&psp))
	 && SUCCEEDED(hr = psp->QueryService(SID_SShellBrowser, IID_IShellBrowser
	                                                      , reinterpret_cast<void**>(&m_spShellBrowser))))
		hr = m_BrowserEvents.Advise();

	return hr;
}

/////////////////////////////////////////////////////////////////////////////
// IDeskBand
/////////////////////////////////////////////////////////////////////////////
// IDeskBand::GetWindow() *** IOleWindow method ***
STDMETHODIMP CIEToolBand::GetWindow(HWND *phWnd)
{
	if (!phWnd)
		return E_POINTER;

	*phWnd = m_ToolBar;

	return S_OK;
}

/////////////////////////////////////////////////////////////////////////////
// IDeskBand::ContextSensitiveHelp() *** IOleWindow method ***
STDMETHODIMP CIEToolBand::ContextSensitiveHelp(BOOL fEnterMode)
{
	return E_NOTIMPL;
}

/////////////////////////////////////////////////////////////////////////////
// IDeskBand::ShowDW() *** IDockingWindow methods ***
STDMETHODIMP CIEToolBand::ShowDW(BOOL fShow)
{
	if(m_ToolBar)
		m_ToolBar.ShowWindow(fShow ? SW_SHOW : SW_HIDE);

	return S_OK;
}

/////////////////////////////////////////////////////////////////////////////
// IDeskBand::CloseDW() *** IDockingWindow methods ***
STDMETHODIMP CIEToolBand::CloseDW(DWORD/*dwReserved*/)
{
	ShowDW(FALSE);

	if (m_ToolBar)
		m_ToolBar.Detach();

	if(IsWindow())
		DestroyWindow();
   
	return S_OK;
}

/////////////////////////////////////////////////////////////////////////////
// IDeskBand::ResizeBorderDW() *** IDockingWindow methods ***
STDMETHODIMP CIEToolBand::ResizeBorderDW(LPCRECT prcBorder, IUnknown* punkToolbarSite, BOOL fReserved)
{
	return E_NOTIMPL;
}

/////////////////////////////////////////////////////////////////////////////
// IDeskBand::GetBandInfo()
STDMETHODIMP CIEToolBand::GetBandInfo(DWORD dwBandID, DWORD dwViewMode, DESKBANDINFO* pdbi)
{
	USES_CONVERSION;

	if(!pdbi)
		return E_INVALIDARG;

	m_dwBandID = dwBandID;
	m_dwViewMode = dwViewMode;

	if(pdbi->dwMask & DBIM_MINSIZE)
	{
		pdbi->ptMinSize.x = 40;
		pdbi->ptMinSize.y = 22;
	}

	if(pdbi->dwMask & DBIM_MAXSIZE)
	{
		pdbi->ptMaxSize.x = -1;
		pdbi->ptMaxSize.y = 22;
	}

	if(pdbi->dwMask & DBIM_INTEGRAL)
	{
		pdbi->ptIntegral.x = 1;
		pdbi->ptIntegral.y = 1;
	}

	if(pdbi->dwMask & DBIM_ACTUAL)
	{
		pdbi->ptActual.x = -1;
		pdbi->ptActual.y = 22;
	}

	if(pdbi->dwMask & DBIM_TITLE)
		lstrcpyW(pdbi->wszTitle, A2W(m_csProductName));

	if(pdbi->dwMask & DBIM_MODEFLAGS)
		pdbi->dwModeFlags = DBIMF_NORMAL | DBIMF_VARIABLEHEIGHT;

	if(pdbi->dwMask & DBIM_BKCOLOR)  // Use the default background color by removing this flag.
		pdbi->dwMask &= ~DBIM_BKCOLOR;

	pdbi->crBkgnd = 0UL;

	return S_OK;
}

/////////////////////////////////////////////////////////////////////////////
// IInputObject
/////////////////////////////////////////////////////////////////////////////
// IInputObject::UIActivateIO()
STDMETHODIMP CIEToolBand::UIActivateIO(BOOL fActivate, LPMSG lpMsg)
{
	return S_FALSE;
}

/////////////////////////////////////////////////////////////////////////////
// IInputObject::HasFocusIO()
STDMETHODIMP CIEToolBand::HasFocusIO(void)
{
	return ::GetFocus() == m_ToolBar ? S_OK : S_FALSE;
}

/////////////////////////////////////////////////////////////////////////////
// IInputObject::TranslateAcceleratorIO()
STDMETHODIMP CIEToolBand::TranslateAcceleratorIO(LPMSG lpMsg)
{
	return S_FALSE;
}

/////////////////////////////////////////////////////////////////////////////
// IPersistStream
/////////////////////////////////////////////////////////////////////////////
// IPersistStream::GetClassID() *** IPersist ***
STDMETHODIMP CIEToolBand::GetClassID(CLSID *pClassID)
{
	if (!pClassID)
		return E_POINTER;

	*pClassID = GetObjectCLSID();

	return S_OK;
}

/////////////////////////////////////////////////////////////////////////////
// IPersistStream::IsDirty()
STDMETHODIMP CIEToolBand::IsDirty(void)
{
	return S_FALSE;
}

/////////////////////////////////////////////////////////////////////////////
// IPersistStream::Load()
STDMETHODIMP CIEToolBand::Load(IStream *pStm)
{
	return S_OK;
}

/////////////////////////////////////////////////////////////////////////////
// IPersistStream::Save()
STDMETHODIMP CIEToolBand::Save(IStream *pStm, BOOL fClearDirty)
{
	return S_OK;
}

/////////////////////////////////////////////////////////////////////////////
// IPersistStream::GetSizeMax()
STDMETHODIMP CIEToolBand::GetSizeMax(ULARGE_INTEGER *pcbSize)
{
	return E_NOTIMPL;
}

bool CIEToolBand::IsCfgwizFinished()
{
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

	return bFinished;
}

void CIEToolBand::LaunchCfgwiz()
{	
	CString csApp, csParam;
	if(SuiteOwnerHelper::GetOwnerCfgWizCmdLine(csApp, csParam))
	{
		CString csCmdLine;
		csCmdLine.Format(_T("%s %s"), csApp, csParam);

		// Launch ConfigWiz
		STARTUPINFO si = {0};
		si.cb = sizeof( STARTUPINFO );
		PROCESS_INFORMATION pi = {0};

		CreateProcess(NULL, csCmdLine.GetBuffer(), NULL, NULL, FALSE, DETACHED_PROCESS, NULL, NULL, &si, &pi);

		CloseHandle(pi.hThread);
		CloseHandle(pi.hProcess);
	}
}