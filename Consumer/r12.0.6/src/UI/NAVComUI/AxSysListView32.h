//******************************************************************************
//
// Note:  In NAV 2006, the NAVComUI project is changing from having one
//        instance of this class to having two.  The functionality of the
//        two is different enough so that from a design perspective, 
//        ideally there should be an abstract base class, with two derived
//        classes handling things like setting the columns and populating
//        the list view.  However, given the time constraints approaching
//        the code complete date, and my lack of experience with COM, the
//        decision is being made to leave both implementations within the
//        same single class.  A project splitting them out would be a 
//        worthwhile endeavor at a later date.
//
//        -- MMW  04/26/2006
//        
//******************************************************************************
#pragma once

#include "resource.h"       // main symbols
#include "..\NAVComUIRes\ResResource.h"
#include <atlctl.h>
#include "NAVComUI.h"
#include "_IAxSysListView32Events_CP.h"
#include <commctrl.h>
#include "ccEraserInterface.h"
#include "NAVComUI.h"
#include "ccString.h"
#include "EZEraserObjects.h"
#include "ThreatCatInfo.h"

#include "ColBasedCCListView.h"

typedef CContainedWindowT<CListViewCtrl> CContainedListViewCtrl;

static bool g_bThreatNameAscending = false;
static bool g_bThreatTypeAscending = false;
static bool g_bThreatImpactAscending = false;
static bool g_bThreatWarningAscending = false;
static bool g_bThreatStatusAscending = false;


static TCHAR REPAIR_ACTION[MAX_PATH];
static TCHAR QUARANTINE_ACTION[MAX_PATH];
static TCHAR REPAIR_THEN_QUARANTINE_ACTION[MAX_PATH];
static TCHAR REMOVE_ACTION[MAX_PATH];
static TCHAR REPAIR_THEN_REMOVE_ACTION[MAX_PATH];
static TCHAR EXCLUDE_ACTION[MAX_PATH];
static TCHAR IGNORE_ACTION[MAX_PATH];
static TCHAR COMPLETE_ACTION[MAX_PATH];
static TCHAR NO_OPERATIONS_ACTION[MAX_PATH];
static TCHAR REVIEW_ACTION[MAX_PATH];
static TCHAR REPAIR_ACTION_RECOMMENDED[MAX_PATH];
static TCHAR QUARANTINE_ACTION_RECOMMENDED[MAX_PATH];
static TCHAR REPAIR_THEN_QUARANTINE_ACTION_RECOMMENDED[MAX_PATH];
static TCHAR REMOVE_ACTION_RECOMMENDED[MAX_PATH];
static TCHAR REPAIR_THEN_REMOVE_ACTION_RECOMMENDED[MAX_PATH];
static TCHAR EXCLUDE_ACTION_RECOMMENDED[MAX_PATH];
static TCHAR IGNORE_ACTION_RECOMMENDED[MAX_PATH];
static TCHAR COMPLETE_ACTION_RECOMMENDED[MAX_PATH];
static TCHAR NO_OPERATIONS_ACTION_RECOMMENDED[MAX_PATH];
static TCHAR REVIEW_ACTION_RECOMMENDED[MAX_PATH];

static const int RECOMMEND_CONSTANT = ACTION_REVIEW + 1;

static TCHAR * pszActions[RECOMMEND_CONSTANT * 2 ] = 
{
    REPAIR_ACTION, 
    QUARANTINE_ACTION,
    REPAIR_THEN_QUARANTINE_ACTION,
    REMOVE_ACTION,
    REPAIR_THEN_REMOVE_ACTION,
    EXCLUDE_ACTION,
    IGNORE_ACTION,
    COMPLETE_ACTION,
    NO_OPERATIONS_ACTION,
    REVIEW_ACTION,
    REPAIR_ACTION_RECOMMENDED, 
    QUARANTINE_ACTION_RECOMMENDED,
    REPAIR_THEN_QUARANTINE_ACTION_RECOMMENDED,
    REMOVE_ACTION_RECOMMENDED,
    REPAIR_THEN_REMOVE_ACTION_RECOMMENDED,
    EXCLUDE_ACTION_RECOMMENDED,
    IGNORE_ACTION_RECOMMENDED,
    COMPLETE_ACTION_RECOMMENDED,
    NO_OPERATIONS_ACTION_RECOMMENDED,
    REVIEW_ACTION_RECOMMENDED
};

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
class ATL_NO_VTABLE CAxSysListView32 : 
    public CComObjectRootEx<CComSingleThreadModel>,
    public CStockPropImpl<CAxSysListView32, IAxSysListView32>,
    public IPersistStreamInitImpl<CAxSysListView32>,
    public IOleControlImpl<CAxSysListView32>,
    public IOleObjectImpl<CAxSysListView32>,
    public IOleInPlaceActiveObjectImpl<CAxSysListView32>,
    public IViewObjectExImpl<CAxSysListView32>,
    public IOleInPlaceObjectWindowlessImpl<CAxSysListView32>,
    public IConnectionPointContainerImpl<CAxSysListView32>,
    public CProxy_IAxSysListView32Events<CAxSysListView32>, 
    public IProvideClassInfo2Impl<&CLSID_AxSysListView32, 
                                  &__uuidof(_IAxSysListView32Events), 
                                  &LIBID_NAVCOMUILib>,
    public IPropertyNotifySinkCP<CAxSysListView32>,
    public IQuickActivateImpl<CAxSysListView32>,
    public CComCoClass<CAxSysListView32, &CLSID_AxSysListView32>,
    public CComControl<CAxSysListView32>,
    public IObjectWithSiteImpl<CAxSysListView32>,
    public IPersistPropertyBagImpl<CAxSysListView32>,
    public IObjectSafetyImpl<CAxSysListView32, 
                             INTERFACESAFE_FOR_UNTRUSTED_CALLER |
                             INTERFACESAFE_FOR_UNTRUSTED_DATA>
 {
public:
    CColBasedCCListView m_ctlSysListView32;

#pragma warning(push)
#pragma warning(disable: 4355) // 'this' : used in base member initializer list

    CAxSysListView32();
    ~CAxSysListView32();

#pragma warning(pop)

DECLARE_OLEMISC_STATUS( OLEMISC_RECOMPOSEONRESIZE | 
                        OLEMISC_CANTLINKINSIDE | 
                        OLEMISC_INSIDEOUT | 
                        OLEMISC_ACTIVATEWHENVISIBLE |  
                        OLEMISC_SETCLIENTSITEFIRST )

DECLARE_REGISTRY_RESOURCEID(IDR_AXSYSLISTVIEW32)

BEGIN_COM_MAP(CAxSysListView32)
    COM_INTERFACE_ENTRY(IAxSysListView32)
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

BEGIN_PROP_MAP(CAxSysListView32)
    PROP_DATA_ENTRY("_cx", m_sizeExtent.cx, VT_UI4)
    PROP_DATA_ENTRY("_cy", m_sizeExtent.cy, VT_UI4)
    PROP_ENTRY("Enabled", DISPID_ENABLED, CLSID_NULL)
END_PROP_MAP()

BEGIN_CONNECTION_POINT_MAP(CAxSysListView32)
    CONNECTION_POINT_ENTRY(IID_IPropertyNotifySink)
    CONNECTION_POINT_ENTRY(__uuidof(_IAxSysListView32Events))
END_CONNECTION_POINT_MAP()

BEGIN_MSG_MAP(CAxSysListView32)
    MESSAGE_HANDLER(WM_CREATE, OnCreate)
    MESSAGE_HANDLER(WM_SETFOCUS, OnSetFocus)
    MESSAGE_HANDLER(WM_PAINT, OnPaint)
    MESSAGE_HANDLER(WM_ERASEBKGND, OnEraseBkgnd)
    NOTIFY_CODE_HANDLER(HDN_BEGINTRACK, OnHeaderBeginTrack)
    NOTIFY_CODE_HANDLER(HDN_DIVIDERDBLCLICK, OnDividerDblClick)
    NOTIFY_CODE_HANDLER(LVN_GETDISPINFO, OnGetDispInfo)
    NOTIFY_CODE_HANDLER(LVN_COLUMNCLICK, OnColumnClick)
//    NOTIFY_CODE_HANDLER(LVN_KEYDOWN, OnKeyDown)
    NOTIFY_CODE_HANDLER(LVN_ITEMCHANGED, OnItemChanged)
    NOTIFY_CODE_HANDLER(CColBasedCCListView::LVN_COMBOITEMACTIVATE, 
                        OnLvComboItemActivate)
    NOTIFY_CODE_HANDLER(CColBasedCCListView::LVN_HYPERLINKACTIVATE, 
                        OnLvHyperlinkActivate)
    NOTIFY_CODE_HANDLER(CColBasedCCListView::LVN_COMBOITEMCHANGE, 
                        OnLvComboItemChange)
   // NOTIFY_CODE_HANDLER(NM_CLICK, OnClick)
    REFLECT_NOTIFICATIONS()
    CHAIN_MSG_MAP(CComControl<CAxSysListView32>)
    ALT_MSG_MAP(1)
END_MSG_MAP()

// Handler prototypes:
//  LRESULT MessageHandler(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
//  LRESULT CommandHandler(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
//  LRESULT NotifyHandler(int idCtrl, LPNMHDR pnmh, BOOL& bHandled);

private:
    // 
    // See note at top:  Should be one parent class with an abstract
    //                   InitColumns method which would be 
    //                   implemented in the child classes.
    //                   
    BOOL InitMRColumns();

    // Sort methods
    static bool Compare_ByThreatName(ccEraser::IAnomaly* pAnom1, 
                                     ccEraser::IAnomaly* pAnom2);
    static bool Compare_ByThreatType(ccEraser::IAnomaly* pAnom1, 
                                     ccEraser::IAnomaly* pAnom2);
    static bool Compare_ByThreatImpact(ccEraser::IAnomaly* pAnom1, 
                                       ccEraser::IAnomaly* pAnom2);
    static bool Compare_ByThreatWarning(ccEraser::IAnomaly* pAnom1, 
                                        ccEraser::IAnomaly* pAnom2);
    static bool Compare_ByThreatStatus(ccEraser::IAnomaly* pAnom1, 
                                       ccEraser::IAnomaly* pAnom2);
    static bool Compare_ByAction(ccEraser::IAnomaly* pAnom1, 
                                 ccEraser::IAnomaly* pAnom2);

public:
    STDMETHOD(SetObjectRects)(LPCRECT prcPos,LPCRECT prcClip);
    STDMETHOD(Initialize)(BOOL bEmailScan);
    STDMETHOD(InitColumns)(BOOL bManualRemoval);
    STDMETHOD(put_Filter)(LONG newVal);
    STDMETHOD(put_AnomalyList)(VARIANT* newVal);
    STDMETHOD(get_Anomaly)(VARIANT* pVal);
    STDMETHOD(Refresh)(void);

    BOOL PreTranslateAccelerator(LPMSG pMsg, HRESULT& hRet);

    LRESULT OnCreate(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, 
                     BOOL& /*bHandled*/);
    LRESULT OnSetFocus(UINT uMsg, WPARAM wParam, LPARAM lParam, 
                       BOOL& bHandled);
    LRESULT OnPaint(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
    LRESULT OnEraseBkgnd(UINT uMsg, WPARAM wParam, LPARAM lParam, 
                         BOOL& bHandled);
    LRESULT OnColumnClick(int nCtrlID, LPNMHDR pNMHdr, BOOL& bHandled);
//    LRESULT OnClick(int nCtrlID, LPNMHDR pNMHdr, BOOL& bHandled);
    LRESULT OnItemChanged(int nCtrlID, LPNMHDR pNMHdr, BOOL& bHandled);
//    LRESULT OnKeyDown(int nCtrlID, LPNMHDR pNMHdr, BOOL& bHandled);
    LRESULT OnLvComboItemActivate(int nCtrlID, LPNMHDR pNMHdr, BOOL& bHandled);
    LRESULT OnLvHyperlinkActivate(int nCtrlID, LPNMHDR pNMHdr, BOOL& bHandled);
    LRESULT OnLvComboItemChange(int nCtrlID, LPNMHDR pNMHdr, BOOL& bHandled);
    LRESULT OnHeaderBeginTrack(int idCtrl, LPNMHDR pnmh, BOOL& bHandled);
    LRESULT OnDividerDblClick(int idCtrl, LPNMHDR pnmh, BOOL& bHandled);

  
    // 
    // See note at top:  Should be one parent class with an abstract
    //                   OnGetDispInfo method which would be implemented 
    //                   in the child classes.
    //                   
    LRESULT OnGetDispInfo(int nCtrlID, LPNMHDR pNMHdr, BOOL& bHandled);
private:
    LRESULT OnMRGetDispInfo(int nCtrlID, LPNMHDR pNMHdr, BOOL& bHandled);

public:

    DECLARE_PROTECT_FINAL_CONSTRUCT()

// IViewObjectEx
    DECLARE_VIEW_STATUS(VIEWSTATUS_SOLIDBKGND | VIEWSTATUS_OPAQUE)


    ccEraser::IAnomalyListPtr m_spAnomalyList;

// IAxSysListView32
    BOOL m_bEnabled;
    void OnEnabledChanged();

    HRESULT FinalConstruct();
    void FinalRelease();

    int m_iThreatNameColumn;
    int m_iThreatTypeColumn;
    int m_iThreatImpactColumn;
    int m_iThreatWarningColumn;
    int m_iRepairActionColumn;

    //
    // m_bThreatNameAscending is only necessary becuase this class is not 
    // sub-classed as it should be.  (See note at top.)  Changes to the 
    // ascending/descending order of the threat name that *should* be 
    // global across both tabs or the repair/remove window, should *NOT* 
    // also apply globally to the manual removal window.  Thus the 
    // g_bThreatNameAscending global alone is insufficent for keeping 
    // proper track of the changes.
    //
    // m_bThreatNameAscending used only in the manual removal window.
    // 
    bool m_bThreatNameAscending;
    
    IAxSysListView32_Filter m_eFilterType;

protected:
    CEZAnomaly m_EZAnomaly;
    std::vector<ccEraser::IAnomalyPtr> m_vListRepaired;
    std::vector<ccEraser::IAnomalyPtr> m_vListRemaining;
    TCHAR m_szDisplay[MAX_PATH];
    CThreatCatInfo m_ThreatInfo;

private:
    void GetWarning(const CEZAnomaly & ezAnom, CString & strWarning,
                    const bool bPastTense);
    static void GetWarning(const CEZAnomaly & ezAnom, CString & strWarning);

    CFont m_fontFaked;

    BOOL m_bEmailScan;
    BOOL m_bManualRemoval;

    BOOL m_bRepairedScrollBarAdjusted;
    BOOL m_bRemainingScrollBarAdjusted;
};

OBJECT_ENTRY_AUTO(__uuidof(AxSysListView32), CAxSysListView32)
