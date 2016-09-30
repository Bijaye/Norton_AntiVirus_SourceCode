//******************************************************************************
//******************************************************************************
#pragma once
#include "resource.h"       // main symbols
#include "..\NAVComUIRes\ResResource.h"
#include <atlctl.h>
#include "NAVComUI.h"
#include "_IAxSysListView32OAAEvents_CP.h"
#include <commctrl.h>
#include "ccEraserInterface.h"
#include "EZEraserObjects.h"

typedef CContainedWindowT<CListViewCtrl> CContainedListViewCtrl;

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
class ATL_NO_VTABLE CAxSysListView32OAA : 
	public CComObjectRootEx<CComSingleThreadModel>,
	public CStockPropImpl<CAxSysListView32OAA, IAxSysListView32OAA>,
	public IPersistStreamInitImpl<CAxSysListView32OAA>,
	public IOleControlImpl<CAxSysListView32OAA>,
	public IOleObjectImpl<CAxSysListView32OAA>,
	public IOleInPlaceActiveObjectImpl<CAxSysListView32OAA>,
	public IViewObjectExImpl<CAxSysListView32OAA>,
	public IOleInPlaceObjectWindowlessImpl<CAxSysListView32OAA>,
	public IConnectionPointContainerImpl<CAxSysListView32OAA>,
	public CProxy_IAxSysListView32OAAEvents<CAxSysListView32OAA>, 
	public IQuickActivateImpl<CAxSysListView32OAA>,
	public CComControl<CAxSysListView32OAA>,
    public IPropertyNotifySinkCP<CAxSysListView32OAA>,
    public IProvideClassInfo2Impl<&CLSID_AxSysListView32OAA, &__uuidof(_IAxSysListView32OAAEvents), &LIBID_NAVCOMUILib>,
    public CComCoClass<CAxSysListView32OAA, &CLSID_AxSysListView32OAA>,
    public IObjectWithSiteImpl<CAxSysListView32OAA>,
    public IPersistPropertyBagImpl<CAxSysListView32OAA>,
    public IObjectSafetyImpl<CAxSysListView32OAA, INTERFACESAFE_FOR_UNTRUSTED_CALLER|INTERFACESAFE_FOR_UNTRUSTED_DATA>
{
public:
	CContainedListViewCtrl m_ctlSysListView32;

#pragma warning(push)
#pragma warning(disable: 4355) // 'this' : used in base member initializer list

	CAxSysListView32OAA();
    ~CAxSysListView32OAA();

#pragma warning(pop)

DECLARE_OLEMISC_STATUS(OLEMISC_RECOMPOSEONRESIZE | 
	OLEMISC_CANTLINKINSIDE | 
	OLEMISC_INSIDEOUT | 
	OLEMISC_ACTIVATEWHENVISIBLE | 
	OLEMISC_SETCLIENTSITEFIRST
)

DECLARE_REGISTRY_RESOURCEID(IDR_AXSYSLISTVIEW32OAA)

BEGIN_COM_MAP(CAxSysListView32OAA)
	COM_INTERFACE_ENTRY(IAxSysListView32OAA)
    COM_INTERFACE_ENTRY(IDispatch)
    COM_INTERFACE_ENTRY(IViewObjectEx)
    COM_INTERFACE_ENTRY(IViewObject2)
    COM_INTERFACE_ENTRY(IViewObject)
    COM_INTERFACE_ENTRY(IOleInPlaceObjectWindowless)
    COM_INTERFACE_ENTRY(IOleInPlaceObject)
    COM_INTERFACE_ENTRY2(IOleWindow, IOleInPlaceObjectWindowless)
    COM_INTERFACE_ENTRY(IOleInPlaceActiveObject)
    COM_INTERFACE_ENTRY(IOleControl)
    COM_INTERFACE_ENTRY(IOleObject)
    COM_INTERFACE_ENTRY(IPersistStreamInit)
    COM_INTERFACE_ENTRY2(IPersist, IPersistStreamInit)
    COM_INTERFACE_ENTRY(IConnectionPointContainer)
    COM_INTERFACE_ENTRY(IProvideClassInfo)
    COM_INTERFACE_ENTRY(IProvideClassInfo2)
    COM_INTERFACE_ENTRY(IObjectWithSite)
    COM_INTERFACE_ENTRY(IObjectSafety)
    COM_INTERFACE_ENTRY(IQuickActivate)
    COM_INTERFACE_ENTRY(IPersistPropertyBag)
END_COM_MAP()

BEGIN_PROP_MAP(CAxSysListView32OAA)
	PROP_DATA_ENTRY(_T("_cx"), m_sizeExtent.cx, VT_UI4)
	PROP_DATA_ENTRY(_T("_cy"), m_sizeExtent.cy, VT_UI4)
    PROP_ENTRY(_T("ControlMode"), 3, CAxSysListView32OAA::GetObjectCLSID())
END_PROP_MAP()

BEGIN_CONNECTION_POINT_MAP(CAxSysListView32OAA)
    CONNECTION_POINT_ENTRY(IID_IPropertyNotifySink)
	CONNECTION_POINT_ENTRY(__uuidof(_IAxSysListView32OAAEvents))
END_CONNECTION_POINT_MAP()

BEGIN_MSG_MAP(CAxSysListView32OAA)
	MESSAGE_HANDLER(WM_CREATE, OnCreate)
	MESSAGE_HANDLER(WM_SETFOCUS, OnSetFocus)
    MESSAGE_HANDLER(WM_PAINT, OnPaint)
    MESSAGE_HANDLER(WM_ERASEBKGND, OnEraseBkgnd)
    NOTIFY_CODE_HANDLER(LVN_ITEMCHANGED, OnItemChanged)
    NOTIFY_CODE_HANDLER(LVN_GETDISPINFO, OnGetDispInfo)
	CHAIN_MSG_MAP(CComControl<CAxSysListView32OAA>)
    ALT_MSG_MAP(1)
END_MSG_MAP()

    CImageList m_imgListSystem;
    long m_lControlMode;
    CEZAnomaly m_EZAnomaly;
    int m_iDescriptionColumn;
    TCHAR m_szDisplay[MAX_PATH];

    void InitListViewColumns();
    void InitializeTopControl();
	BOOL PreTranslateAccelerator(LPMSG pMsg, HRESULT& hRet);

	LRESULT OnSetFocus(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnCreate(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
    LRESULT OnGetDispInfo( int nCtrlID, LPNMHDR pNMHdr, BOOL& bHandled );
    LRESULT OnPaint(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
    LRESULT OnEraseBkgnd(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
    LRESULT OnCustomDraw(int nCtrlID, LPNMHDR pNMHdr, BOOL& bHandled);
    LRESULT OnItemChanged(int nCtrlID, LPNMHDR pNMHdr, BOOL& bHandled);

	STDMETHOD(SetObjectRects)(LPCRECT prcPos,LPCRECT prcClip);
    STDMETHOD(put_Anomaly)(VARIANT* newVal);
    STDMETHOD(get_AffectedType)(ULONG* pVal);
    STDMETHOD(put_AffectedType)(ULONG newVal);
    STDMETHOD(get_ControlMode)(ULONG* pVal);
    STDMETHOD(put_ControlMode)(ULONG newVal);

// IViewObjectEx
	DECLARE_VIEW_STATUS(VIEWSTATUS_SOLIDBKGND | VIEWSTATUS_OPAQUE)

// IAxSysListView32OAA

	DECLARE_PROTECT_FINAL_CONSTRUCT()

	HRESULT FinalConstruct();
	void FinalRelease();

    CRemediationStats* m_pStats;
    typedef std::pair<CString,AffectedTypes> AFFECTEDTYPESPAIR;
    std::vector<AFFECTEDTYPESPAIR> m_vTopItems;
    std::vector<CRemediationStats::REMEDIATIONPAIR> m_vBottomItems;
    AffectedTypes m_BottomMode;
};

OBJECT_ENTRY_AUTO(__uuidof(AxSysListView32OAA), CAxSysListView32OAA)


