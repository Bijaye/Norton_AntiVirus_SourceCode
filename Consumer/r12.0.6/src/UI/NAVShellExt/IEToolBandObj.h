// IEToolBandObj.h : Declaration of the CIEToolBand

#ifndef __IETOOLBAND_H_
#define __IETOOLBAND_H_

#include <atlwin.h>
#include <exdispid.h>       // DISPID_NAVIGATECOMPLETE2
#include "Resource.h"       // main symbols
#include "BrowserEvents.h"
#include "SelectionList.h"
#include "NAVLnch.h"        // Launcher object


/////////////////////////////////////////////////////////////////////////////
// CIEToolBand
class ATL_NO_VTABLE CIEToolBand : public IDeskBand
                                , public IInputObject
                                , public IPersistStream
                                , public IObjectWithSiteImpl<CIEToolBand>
                                , public ISupportErrorInfo
                                , public CWindowImpl<CIEToolBand>
                                , public CComObjectRootEx<CComSingleThreadModel>
                                , public CSelectionList<CIEToolBand, &CLSID_IEToolBand, &IID_IDeskBand>
{
	DWORD m_dwBandID
	    , m_dwViewMode;
	bool m_bCanTouchThisSite;
	CWindow m_ToolBar;
	CComPtr<IShellBrowser> m_spShellBrowser;
	CComPtr<IWebBrowser2> m_spBrowser;
	CBrowserEvents m_BrowserEvents;
	friend class CBrowserEvents;
	HBITMAP m_hBitmap;

public:
	CIEToolBand(void);

DECLARE_REGISTRY_RESOURCEID(IDR_IETOOLBAND)

DECLARE_PROTECT_FINAL_CONSTRUCT()

BEGIN_COM_MAP(CIEToolBand)
	COM_INTERFACE_ENTRY(IDeskBand)
	COM_INTERFACE_ENTRY(IInputObject)
	COM_INTERFACE_ENTRY(IPersistStream)
	COM_INTERFACE_ENTRY(ISupportErrorInfo)
	COM_INTERFACE_ENTRY(IObjectWithSite)
END_COM_MAP()

BEGIN_MSG_MAP(CIEToolBand)
	COMMAND_ID_HANDLER(IDC_TOOLBAR,   onScan)
	COMMAND_ID_HANDLER(IDC_LAUNCH,    onLaunch)
	COMMAND_ID_HANDLER(IDC_STATUS,    onStatus)
	COMMAND_ID_HANDLER(IDC_QUARINTIN, onQuarantine)
	COMMAND_ID_HANDLER(IDC_VIRUSDEF,  onVirusDef)
	COMMAND_ID_HANDLER(IDC_LOG,       onActivityLog)
	NOTIFY_HANDLER(IDC_TOOLBAR, TBN_DROPDOWN, onDropDown)
END_MSG_MAP()

	void FinalRelease();

// ISupportsErrorInfo
	STDMETHOD(InterfaceSupportsErrorInfo)(REFIID riid);

// IObjectWithSite
	STDMETHOD(SetSite)(IUnknown* punkSite);

// IEToolBand
// IDeskBand
// *** IOleWindow methods ***
	STDMETHOD(GetWindow)(HWND * lphwnd);
	STDMETHOD(ContextSensitiveHelp)(BOOL fEnterMode);

// *** IDockingWindow methods ***
	STDMETHOD(ShowDW)(BOOL fShow);
	STDMETHOD(CloseDW)(DWORD dwReserved);
	STDMETHOD(ResizeBorderDW)(LPCRECT prcBorder, IUnknown* punkToolbarSite, BOOL fReserved);
// *** IDeskBand methods ***
	STDMETHOD(GetBandInfo)(DWORD dwBandID, DWORD dwViewMode, DESKBANDINFO* pdbi);

// IInputObject
	STDMETHOD(UIActivateIO)(BOOL fActivate, LPMSG lpMsg);
	STDMETHOD(HasFocusIO)(void);
	STDMETHOD(TranslateAcceleratorIO)(LPMSG lpMsg);

// IPersistStream
// *** IPersist ***
	STDMETHOD(GetClassID)(CLSID *pClassID);
// *** IPersistStream ***
	STDMETHOD(IsDirty)(void);
	STDMETHOD(Load)(IStream *pStm);
	STDMETHOD(Save)(IStream *pStm, BOOL fClearDirty);
	STDMETHOD(GetSizeMax)(ULARGE_INTEGER *pcbSize);

	void ShowError(HRESULT hr);

// CIEToolBand
private:
	LRESULT onScan(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
	bool doesTreeViewHasFocus(void);
	LRESULT onLaunch(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
	LRESULT onStatus(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
	LRESULT onQuarantine(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
	LRESULT onActivityLog(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
	LRESULT onVirusDef(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
	LRESULT onNotify(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT onDropDown(int idCtrl, LPNMHDR pnmh, BOOL& bHandled);
	void canTouchThisSite(BSTR bstrURL);
	void LaunchCfgwiz();
	bool IsCfgwizFinished();

	// Implementations
	HRESULT scan(void);
	HRESULT messageBox(UINT uiIDS);
	HRESULT virusDef(void);
	LRESULT navigate2NMAINPanel(int nPanelResID);

private:
	CString m_csProductName;
};

#endif //__IETOOLBAND_H_
