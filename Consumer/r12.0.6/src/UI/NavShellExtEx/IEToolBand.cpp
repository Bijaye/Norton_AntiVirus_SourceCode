// IEToolBandObj.cpp : Implementation of CIEToolBand
#include "StdAfx.h"
#define INITIIDS
#include "IEToolBand.h"
#include "NavShellExtEx.h"
#include "Util.h"
#include "atlstr.h"

#define WINBASE_DECLARE_GET_SYSTEM_WOW64_DIRECTORY
#include "Windows.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#define _WIN32_DCOM 

using namespace UTIL;

#include "ssosinfo.h"
extern StahlSoft::COSVersionInfo g_OSInfo;

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
							   , m_hBitmap(HBITMAP(::LoadImage(_AtlBaseModule.GetResourceInstance(), MAKEINTRESOURCE(IDB_IETOOLBAR)
	                                                                                        , IMAGE_BITMAP
	                                                                                        , 0, 0
	                                                                                        , LR_LOADTRANSPARENT
	                                                                                        | LR_LOADMAP3DCOLORS)))
{
}

void CIEToolBand::FinalRelease(void)
{
	m_BrowserEvents.Unadvise();
	::DeleteObject(HGDIOBJ(m_hBitmap));
}


bool CIEToolBand::InitHelper()
{
	g_Log.LogT("CIEToolBand::InitHelper() Enter");

	if( !IsTrusted() ) {
		g_Log.LogT(("CIEToolBand::InitHelper() m_TrustState = %s"),
			( m_TrustState == UNDEFINED) ? "UNDEFINED" : "FAILED" );
		messageBox(IDS_Err_VerifyTrustFailed);
		return false;
	}

	bool bRet = true;

	if( !m_spIEToolBandHelper)
	{
		HRESULT hr = m_spIEToolBandHelper.CoCreateInstance(CLSID_IEToolBandHelper,NULL,CLSCTX_LOCAL_SERVER);
		if(!SUCCEEDED(hr))
		{
			g_Log.LogT(("CIEToolBand::InitHelper() : CoCreateInstance(CLSID_IEToolBandHelper) Failed 0x%08X\n"), hr);
			ShowError(hr);
			bRet = false;
		}
	}
	return bRet;
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
	if( !InitHelper() ) return E_FAIL;
	HRESULT hr = m_spIEToolBandHelper->onQuarantine();
	if ( FAILED (hr))
       ShowError(hr);
	return 0L;
}

LRESULT CIEToolBand::onActivityLog(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	if( !InitHelper() ) return E_FAIL;
	HRESULT hr = m_spIEToolBandHelper->onActivityLog();
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

	HMENU hMenuLoaded = LoadMenu(_AtlBaseModule.GetResourceInstance(), MAKEINTRESOURCE(IDM_IETOOLBAR))
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
    CMessageLock msgLock (TRUE, TRUE);

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
		HANDLE	hWaitEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
		if (hWaitEvent == NULL)
		{
	    	return E_FAIL;
		}        
        msgLock.Lock ( hWaitEvent, 1000 );
		CloseHandle(hWaitEvent);
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
			g_Log.LogT(("ERROR!!! CIEToolBand::scan(): GetTreeViewSelection() Failed with 0x%08X"),hr);
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
			g_Log.LogT(("ERROR!!! CIEToolBand::scan(): Add2List() Failed with 0x%08X"),hr);
			return hr;
		}
	}

	if (!m_bCanTouchThisSite)
		// Means we may crash.
		return messageBox(IDS_NOITEMS);

	// Get the view object.
	if (FAILED(hr = m_spShellBrowser->QueryActiveShellView(&spView)))
	{
		g_Log.LogT(("ERROR!!! CIEToolBand::scan(): m_spShellBrowser->QueryActiveShellView Failed with 0x%08X"),hr);
		return hr;
	}

	// Get the list of selected items.
	if (FAILED(hr = spView->GetItemObject(SVGIO_SELECTION, IID_IDataObject, (void**) &spDataObj)))
	{
		// Means nothing is selected.
		return messageBox(IDS_NOITEMS);
 	}

	// ListView selection
	if(FAILED(hr = GetSelectionList(spDataObj)))
	{
		g_Log.LogT(("ERROR!!! CIEToolBand::scan(): GetSelectionList() Failed with 0x%08X"),hr);
		return hr;
	}

	return RunNAVW();
}

HRESULT CIEToolBand::virusDef(void)
{
	if( !InitHelper() ) {
		return 0L;
	}
	HRESULT hr = m_spIEToolBandHelper->onVirusDef();	
	if( FAILED(hr)) {
		ShowError(hr);
	}
	return 0L ;
}

LRESULT CIEToolBand::navigate2NMAINPanel(int nPanelResID)
{
	if( !InitHelper() ) {
		return 0L;
	}
	TCHAR szPanelName[128];
	::LoadString(_AtlBaseModule.GetResourceInstance(), nPanelResID, szPanelName, SIZEOF(szPanelName));
	CComBSTR bstr = szPanelName;
	HRESULT hr = m_spIEToolBandHelper->navigate2NMAINPanel(bstr);	
	if( FAILED(hr)) {
		ShowError(hr);
	}
	return 0L;
}


void CIEToolBand::canTouchThisSite(BSTR bstrURL)
{
	static WCHAR s_wcDontTouchMe[128];

	if (g_OSInfo.IsWinNT())		// @TBD
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
	g_Log.LogT("CIEToolBand::SetSite()");

	HRESULT hr = S_OK;
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
	::LoadString(_AtlBaseModule.GetResourceInstance(), IDS_TOOLTIP, szToolTip, SIZEOF(szToolTip));
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
	VerifyTrust();
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

HRESULT	CIEToolBand::_Error(int iId, const IID& guid)
{
	return Error(iId,guid);
}

HRESULT	CIEToolBand::_Error(CString csError, const IID& guid)
{
	return Error(csError,guid);
}
