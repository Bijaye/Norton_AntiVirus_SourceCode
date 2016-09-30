////////////////////////
//
// PROPRIETARY / CONFIDENTIAL.
// Use of this product is subject to license terms.
// Copyright © 2006 Symantec Corporation.
// All rights reserved.
//
/////////////////////////////////////////////////////////////////////////////////////////////////

//*****************************************************************************
//*****************************************************************************
#pragma once

#include "ccWebWnd.h"
#include "MsHtmdid.h"
#include "ccResourceLoader.h"
#include "isVersion.h"


#pragma warning( disable : 4311 ) //'variable' : pointer truncation from 'type' to 'type'

extern const DWORD WM_HTML_GETOBJECT;

// Resource Loader
extern cc::CResourceLoader g_ResLoader;

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
enum eSymSymDHtmlEventMapEntryType
{ 
    SYM_DHTMLEVENTMAPENTRY_NAME, 
    SYM_DHTMLEVENTMAPENTRY_CLASS, 
    SYM_DHTMLEVENTMAPENTRY_TAG, 
    SYM_DHTMLEVENTMAPENTRY_ELEMENT, 
    SYM_DHTMLEVENTMAPENTRY_CONTROL, 
    SYM_DHTMLEVENTMAPENTRY_END 
};

struct SymDHtmlEventMapEntry;

//-----------------------------------------------------------------------------
// Dummy base classes just to force pointers-to-member that work with multiple inheritance
//-----------------------------------------------------------------------------
class CSymDHtmlSinkHandlerBase1
{
};

class CSymDHtmlSinkHandlerBase2
{
};

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
class CSymDHtmlSinkHandler :
    public CSymDHtmlSinkHandlerBase1,
    public CSymDHtmlSinkHandlerBase2
{
public:
    STDMETHOD(CSymDHtmlSinkHandlerQueryInterface)(REFIID riid, void ** ppvObject) = 0;

    virtual ULONG STDMETHODCALLTYPE AddRef(void) = 0;
    virtual ULONG STDMETHODCALLTYPE Release(void) = 0;

    STDMETHOD(GetTypeInfoCount)(UINT *pctinfo) = 0;
    STDMETHOD(GetTypeInfo)(UINT iTInfo, LCID lcid, ITypeInfo **ppTInfo) = 0;
    STDMETHOD(GetIDsOfNames)(REFIID riid, OLECHAR **rgszNames, UINT cNames, LCID lcid, DISPID *rgDispId) = 0;
    STDMETHOD(Invoke)(DISPID dispIdMember, REFIID riid, LCID lcid, WORD wFlags, DISPPARAMS *pDispParams, VARIANT *pVarResult, EXCEPINFO *pExcepInfo, UINT *puArgErr) = 0;

    virtual const SymDHtmlEventMapEntry* GetDHtmlEventMap() = 0;
};


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
struct SymDHtmlEventMapEntry
{
    eSymSymDHtmlEventMapEntryType nType;
    DISPID dispId;
    LPCTSTR szName;	// name or class based on nType
    void (__stdcall CSymDHtmlSinkHandler::*pfnEventFunc)();
};

typedef HRESULT (CSymDHtmlSinkHandler::*SYMDHEVTFUNC)(IHTMLElement *pElement);
typedef void (__stdcall CSymDHtmlSinkHandler::*SYMDHEVTFUNCCONTROL)();

#define DECLARE_SYM_DHTML_EVENT_MAP()\
private:\
protected:\
    virtual const SymDHtmlEventMapEntry* GetDHtmlEventMap();\

#define BEGIN_SYM_DHTML_EVENT_MAP(className)\
    const SymDHtmlEventMapEntry* className::GetDHtmlEventMap()\
{\
    typedef className theClass; \
    static const SymDHtmlEventMapEntry _dhtmlEventEntries[] = {

#define END_SYM_DHTML_EVENT_MAP()\
{ SYM_DHTMLEVENTMAPENTRY_END, 0, NULL, (SYMDHEVTFUNCCONTROL) NULL },\
    };\
    return _dhtmlEventEntries;\
}

#define BEGIN_SYM_DHTML_EVENT_MAP_INLINE( className )\
    const SymDHtmlEventMapEntry* GetDHtmlEventMap()\
{\
    typedef className theClass; \
    static const SymDHtmlEventMapEntry _dhtmlEventEntries[] = \
{\

#define END_SYM_DHTML_EVENT_MAP_INLINE()\
{ SYM_DHTMLEVENTMAPENTRY_END, 0, NULL, (SYMDHEVTFUNCCONTROL) NULL },\
};\
    return _dhtmlEventEntries;\
}\


#define SYM_DHTML_EVENT(dispid, elemName, memberFxn)\
{ SYM_DHTMLEVENTMAPENTRY_NAME, dispid, elemName, (SYMDHEVTFUNCCONTROL) (SYMDHEVTFUNC) theClass::memberFxn },\

#define SYM_DHTML_EVENT_CLASS(dispid, elemName, memberFxn)\
{ SYM_DHTMLEVENTMAPENTRY_CLASS, dispid, elemName, (SYMDHEVTFUNCCONTROL) (SYMDHEVTFUNC) theClass::memberFxn },\

#define SYM_DHTML_EVENT_TAG(dispid, elemName, memberFxn)\
{ SYM_DHTMLEVENTMAPENTRY_TAG, dispid, elemName, (SYMDHEVTFUNCCONTROL) (SYMDHEVTFUNC) theClass::memberFxn },\

#define SYM_DHTML_EVENT_ELEMENT(dispid, elemName, memberFxn)\
{ SYM_DHTMLEVENTMAPENTRY_ELEMENT, dispid, elemName, (SYMDHEVTFUNCCONTROL) (SYMDHEVTFUNC) theClass::memberFxn },\

#define SYM_DHTML_EVENT_AXCONTROL(dispid, controlName, memberFxn)\
{ SYM_DHTMLEVENTMAPENTRY_CONTROL, dispid, controlName, (SYMDHEVTFUNCCONTROL) (static_cast<void (__stdcall theClass::*)()>(theClass::memberFxn)) },\

// specific commonly used events
#define SYM_DHTML_EVENT_ONHELP(elemName, memberFxn)\
    SYM_DHTML_EVENT(DISPID_HTMLELEMENTEVENTS_ONHELP, elemName, memberFxn)\

#define SYM_DHTML_EVENT_ONCLICK(elemName, memberFxn)\
    SYM_DHTML_EVENT(DISPID_HTMLELEMENTEVENTS_ONCLICK, elemName, memberFxn)\

#define SYM_DHTML_EVENT_ONDBLCLICK(elemName, memberFxn)\
    SYM_DHTML_EVENT(DISPID_HTMLELEMENTEVENTS_ONDBLCLICK, elemName, memberFxn)\

#define SYM_DHTML_EVENT_ONKEYPRESS(elemName, memberFxn)\
    SYM_DHTML_EVENT(DISPID_HTMLELEMENTEVENTS_ONKEYPRESS, elemName, memberFxn)\

#define SYM_DHTML_EVENT_ONKEYDOWN(elemName, memberFxn)\
    SYM_DHTML_EVENT(DISPID_HTMLELEMENTEVENTS_ONKEYDOWN, elemName, memberFxn)\

#define SYM_DHTML_EVENT_ONKEYUP(elemName, memberFxn)\
    SYM_DHTML_EVENT(DISPID_HTMLELEMENTEVENTS_ONKEYUP, elemName, memberFxn)\

#define SYM_DHTML_EVENT_ONMOUSEOUT(elemName, memberFxn)\
    SYM_DHTML_EVENT(DISPID_HTMLELEMENTEVENTS_ONMOUSEOUT, elemName, memberFxn)\

#define SYM_DHTML_EVENT_ONMOUSEOVER(elemName, memberFxn)\
    SYM_DHTML_EVENT(DISPID_HTMLELEMENTEVENTS_ONMOUSEOVER, elemName, memberFxn)\

#define SYM_DHTML_EVENT_ONMOUSEMOVE(elemName, memberFxn)\
    SYM_DHTML_EVENT(DISPID_HTMLELEMENTEVENTS_ONMOUSEMOVE, elemName, memberFxn)\

#define SYM_DHTML_EVENT_ONMOUSEDOWN(elemName, memberFxn)\
    SYM_DHTML_EVENT(DISPID_HTMLELEMENTEVENTS_ONMOUSEDOWN, elemName, memberFxn)\

#define SYM_DHTML_EVENT_ONMOUSEUP(elemName, memberFxn)\
    SYM_DHTML_EVENT(DISPID_HTMLELEMENTEVENTS_ONMOUSEUP, elemName, memberFxn)\

#define SYM_DHTML_EVENT_ONSELECTSTART(elemName, memberFxn)\
    SYM_DHTML_EVENT(DISPID_HTMLELEMENTEVENTS_ONSELECTSTART, elemName, memberFxn)\

#define SYM_DHTML_EVENT_ONFILTERCHANGE(elemName, memberFxn)\
    SYM_DHTML_EVENT(DISPID_HTMLELEMENTEVENTS_ONFILTERCHANGE, elemName, memberFxn)\

#define SYM_DHTML_EVENT_ONDRAGSTART(elemName, memberFxn)\
    SYM_DHTML_EVENT(DISPID_HTMLELEMENTEVENTS_ONDRAGSTART, elemName, memberFxn)\

#define SYM_DHTML_EVENT_ONBEFOREUPDATE(elemName, memberFxn)\
    SYM_DHTML_EVENT(DISPID_HTMLELEMENTEVENTS_ONBEFOREUPDATE, elemName, memberFxn)\

#define SYM_DHTML_EVENT_ONAFTERUPDATE(elemName, memberFxn)\
    SYM_DHTML_EVENT(DISPID_HTMLELEMENTEVENTS_ONAFTERUPDATE, elemName, memberFxn)\

#define SYM_DHTML_EVENT_ONERRORUPDATE(elemName, memberFxn)\
    SYM_DHTML_EVENT(DISPID_HTMLELEMENTEVENTS_ONERRORUPDATE, elemName, memberFxn)\

#define SYM_DHTML_EVENT_ONROWEXIT(elemName, memberFxn)\
    SYM_DHTML_EVENT(DISPID_HTMLELEMENTEVENTS_ONROWEXIT, elemName, memberFxn)\

#define SYM_DHTML_EVENT_ONROWENTER(elemName, memberFxn)\
    SYM_DHTML_EVENT(DISPID_HTMLELEMENTEVENTS_ONROWENTER, elemName, memberFxn)\

#define SYM_DHTML_EVENT_ONDATASETCHANGED(elemName, memberFxn)\
    SYM_DHTML_EVENT(DISPID_HTMLELEMENTEVENTS_ONDATASETCHANGED, elemName, memberFxn)\

#define SYM_DHTML_EVENT_ONDATAAVAILABLE(elemName, memberFxn)\
    SYM_DHTML_EVENT(DISPID_HTMLELEMENTEVENTS_ONDATAAVAILABLE, elemName, memberFxn)\

#define SYM_DHTML_EVENT_ONDATASETCOMPLETE(elemName, memberFxn)\
    SYM_DHTML_EVENT(DISPID_HTMLELEMENTEVENTS_ONDATASETCOMPLETE, elemName, memberFxn)\

#define SYM_DHTML_EVENT_ONSELECT(elemName, memberFxn)\
    SYM_DHTML_EVENT(-600, elemName, memberFxn )\

#define SYM_DHTML_EVENT_MOUSEWHEEL(elemName, memberFxn)\
    SYM_DHTML_EVENT(DISPID_ONMOUSEWHEEL, elemName, memberFxn )\

// non-bubbled element events
#define SYM_DHTML_EVENT_ONBLUR(elemName, memberFxn)\
    SYM_DHTML_EVENT_ELEMENT(DISPID_EVMETH_ONBLUR, elemName, memberFxn)\

#define SYM_DHTML_EVENT_ONCHANGE(elemName, memberFxn)\
    SYM_DHTML_EVENT_ELEMENT(DISPID_EVMETH_ONCHANGE, elemName, memberFxn)\

#define SYM_DHTML_EVENT_ONFOCUS(elemName, memberFxn)\
    SYM_DHTML_EVENT_ELEMENT(DISPID_HTMLELEMENTEVENTS2_ONFOCUS, elemName, memberFxn)\

#define SYM_DHTML_EVENT_ONRESIZE(elemName, memberFxn)\
    SYM_DHTML_EVENT_ELEMENT(DISPID_EVMETH_ONRESIZE, elemName, memberFxn)\


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
interface ISymDHTMLUIHandler
{
public:

    STDMETHOD(ShowContextMenu)(DWORD dwID, POINT *ppt, IUnknown *pcmdtReserved, IDispatch *pdispReserved) = 0;
    STDMETHOD(GetHostInfo)(DOCHOSTUIINFO *pInfo) = 0;
    STDMETHOD(ShowUI)(DWORD dwID, IOleInPlaceActiveObject *pActiveObject, IOleCommandTarget *pCommandTarget, IOleInPlaceFrame *pFrame, IOleInPlaceUIWindow *pDoc) = 0;
    STDMETHOD(HideUI)(void) = 0;
    STDMETHOD(UpdateUI)(void) = 0;
    STDMETHOD(EnableModeless)(BOOL fEnable) = 0;
    STDMETHOD(OnDocWindowActivate)(BOOL fActivate) = 0;
    STDMETHOD(OnFrameWindowActivate)(BOOL fActivate) = 0;
    STDMETHOD(ResizeBorder)(LPCRECT prcBorder, IOleInPlaceUIWindow *pUIWindow, BOOL fRameWindow) = 0;
    STDMETHOD(TranslateAccelerator)(LPMSG lpMsg, const GUID *pguidCmdGroup, DWORD nCmdID) = 0;
    STDMETHOD(GetOptionKeyPath)(LPOLESTR *pchKey, DWORD dw) = 0;
    STDMETHOD(GetDropTarget)(IDropTarget *pDropTarget, IDropTarget **ppDropTarget) = 0;
    STDMETHOD(GetExternal)(IDispatch **ppDispatch) = 0;
    STDMETHOD(TranslateUrl)(DWORD dwTranslate, OLECHAR *pchURLIn, OLECHAR **ppchURLOut) = 0;
    STDMETHOD(FilterDataObject)(IDataObject *pDO, IDataObject **ppDORet) = 0;
};


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
class CSymDHtmlEventSink : 
    public CSymDHtmlSinkHandler
{
public:
    HRESULT ConnectToConnectionPoint(IUnknown *punkObj, REFIID riid, DWORD *pdwCookie);
    void DisconnectFromConnectionPoint(IUnknown *punkObj, REFIID riid, DWORD& dwCookie);
    STDMETHOD(CSymDHtmlSinkHandlerQueryInterface)(REFIID iid, LPVOID* ppvObj);
    STDMETHOD_(ULONG, AddRef)();
    STDMETHOD_(ULONG, Release)();
    STDMETHOD(GetTypeInfoCount)(UINT *pctinfo);
    STDMETHOD(GetTypeInfo)(UINT iTInfo, LCID lcid, ITypeInfo **ppTInfo);
    STDMETHOD(GetIDsOfNames)(REFIID riid, OLECHAR **rgszNames, UINT cNames, LCID lcid, DISPID *rgDispId);
    STDMETHOD(Invoke)(DISPID dispIdMember, REFIID riid, LCID lcid, WORD wFlags, DISPPARAMS *pDispParams, VARIANT *pVarResult, EXCEPINFO *pExcepInfo, UINT *puArgErr);
    virtual BOOL DHtmlEventHook(HRESULT *phr, DISPID dispIdMember, DISPPARAMS *pDispParams,VARIANT *pVarResult, EXCEPINFO *pExcepInfo, UINT *puArgErr);
    virtual const SymDHtmlEventMapEntry* GetDHtmlEventMap() = 0;
    virtual HRESULT GetDHtmlDocument(IHTMLDocument2 **pphtmlDoc) = 0;
    int FindDHtmlEventEntry(const SymDHtmlEventMapEntry *pEventMap, DISPID dispIdMember,IHTMLElement **ppsrcElement);
};


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
class CSymDHtmlControlSink : 
    public IDispatch
{
public:

    LPCTSTR m_szControlId;
    DWORD m_dwCookie;
    CComPtr<IUnknown> m_spunkObj;
    IID m_iid;
    GUID m_libid;
    WORD m_wMajor;
    WORD m_wMinor;
    CComPtr<ITypeInfo> m_spTypeInfo;
    CSymDHtmlSinkHandler *m_pHandler;
    DWORD_PTR m_dwThunkOffset;

    CSymDHtmlControlSink();
    CSymDHtmlControlSink(IUnknown *punkObj, CSymDHtmlSinkHandler *pHandler, LPCTSTR szControlId, DWORD_PTR dwThunkOffset = 0);
    ~CSymDHtmlControlSink();

    HRESULT ConnectToControl(IUnknown *punkObj);

    STDMETHOD_(ULONG, AddRef)();
    STDMETHOD_(ULONG, Release)();
    STDMETHOD(QueryInterface)(REFIID iid, LPVOID* ppvObj);

    STDMETHOD(GetTypeInfoCount)(UINT *pctinfo);
    STDMETHOD(GetTypeInfo)(UINT iTInfo, LCID lcid, ITypeInfo **ppTInfo);
    STDMETHOD(GetIDsOfNames)(REFIID riid, OLECHAR **rgszNames, UINT cNames, LCID lcid, DISPID *rgDispId);
    STDMETHOD(Invoke)(DISPID dispidMember, REFIID riid, LCID lcid, WORD wFlags,DISPPARAMS *pdispparams, VARIANT *pvarResult, EXCEPINFO *pExcepInfo, UINT *puArgErr);

    HRESULT InvokeFromFuncInfo(SYMDHEVTFUNCCONTROL pEvent, _ATL_FUNC_INFO& info, DISPPARAMS* pdispparams, VARIANT* pvarResult);
    HRESULT GetFuncInfoFromId(const IID& iid, DISPID dispidMember, LCID lcid, _ATL_FUNC_INFO& info);
    VARTYPE GetUserDefinedType(ITypeInfo *pTI, HREFTYPE hrt);
};


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
class CSymDHtmlElementEventSink : 
    public IDispatch
{
public:

    CSymDHtmlEventSink *m_pHandler;
    CComPtr<IUnknown> m_spunkElem;
    DWORD m_dwCookie;

    CSymDHtmlElementEventSink(CSymDHtmlEventSink *pHandler, IDispatch *pdisp);

    STDMETHOD_(ULONG, AddRef)();
    STDMETHOD_(ULONG, Release)();
    STDMETHOD(QueryInterface)(REFIID iid, LPVOID* ppvObj);

    STDMETHOD(GetTypeInfoCount)(UINT *pctinfo);
    STDMETHOD(GetTypeInfo)(UINT iTInfo, LCID lcid, ITypeInfo **ppTInfo);
    STDMETHOD(GetIDsOfNames)(REFIID riid, OLECHAR **rgszNames, UINT cNames, LCID lcid, DISPID *rgDispId);
    STDMETHOD(Invoke)(DISPID dispIdMember, REFIID riid, LCID lcid, WORD wFlags, DISPPARAMS *pdispparams, VARIANT *pVarResult, EXCEPINFO *pExcepInfo, UINT *puArgErr);

    HRESULT Advise(LPUNKNOWN pUnkObj);
    HRESULT UnAdvise(LPUNKNOWN pUnkObj);
};


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
class CSymDHtmlBrowserEventSink : 
    public IDispatch
{
public:

    CSymDHtmlBrowserEventSink();

    STDMETHOD_(ULONG, AddRef)();
    STDMETHOD_(ULONG, Release)();
    STDMETHOD(QueryInterface)(REFIID iid, LPVOID* ppvObj);

    STDMETHOD(GetTypeInfoCount)(UINT *pctinfo);
    STDMETHOD(GetTypeInfo)(UINT iTInfo, LCID lcid, ITypeInfo **ppTInfo);
    STDMETHOD(GetIDsOfNames)(REFIID riid, OLECHAR **rgszNames, UINT cNames, LCID lcid, DISPID *rgDispId);
    STDMETHOD(Invoke)(DISPID dispIdMember, REFIID riid, LCID lcid, WORD wFlags, DISPPARAMS *pdispparams, VARIANT *pVarResult, EXCEPINFO *pExcepInfo, UINT *puArgErr);

    HRESULT Advise( LPUNKNOWN pUnkObj );
    HRESULT UnAdvise( LPUNKNOWN pUnkObj );

protected:

    DWORD m_dwCookie;

    virtual void StatusTextChange(BSTR Text){};
    virtual void ProgressChange( long Progress, long ProgressMax){};
    virtual void CommandStateChange(long Command, VARIANT_BOOL Enable){};
    virtual void DownloadBegin(){};
    virtual void DownloadComplete(){};
    virtual void TitleChange(BSTR Text){};
    virtual void PropertyChange(BSTR szProperty){};
    virtual void BeforeNavigate(IDispatch* pDisp,VARIANT * URL, VARIANT * Flags, VARIANT * TargetFrameName, VARIANT * PostData, VARIANT * Headers, VARIANT_BOOL * Cancel ){};
    virtual void NewWindow(IDispatch* ppDisp, VARIANT_BOOL * Cancel){};
    virtual void NavigateComplete(IDispatch* pDisp, VARIANT * URL ){};
    virtual void DocumentComplete(IDispatch* pDisp, VARIANT * URL ){};
    virtual void OnQuit(){};
    virtual void OnVisible(VARIANT_BOOL Visible){};
    virtual void OnToolBar(VARIANT_BOOL ToolBar){};
    virtual void OnMenuBar(VARIANT_BOOL MenuBar){};
    virtual void OnStatusBar(VARIANT_BOOL StatusBar){};
    virtual void OnFullScreen(VARIANT_BOOL FullScreen){};
    virtual void OnTheaterMode(VARIANT_BOOL TheaterMode){};
};


class CDHTMLWndCtrlView;


// DDX_DHtml helper macros
#define DDX_DHtml_ElementValue(dx, name, var) DDX_DHtml_ElementText(dx, name, DISPID_A_VALUE, var)
#define DDX_DHtml_ElementInnerText(dx, name, var) DDX_DHtml_ElementText(dx, name, DISPID_IHTMLELEMENT_INNERTEXT, var)
#define DDX_DHtml_ElementInnerHtml(dx, name, var) DDX_DHtml_ElementText(dx, name, DISPID_IHTMLELEMENT_INNERHTML, var)
#define DDX_DHtml_Anchor_Href(dx, name, var) DDX_DHtml_ElementText(dx, name, DISPID_IHTMLANCHORELEMENT_HREF, var)
#define DDX_DHtml_Anchor_Target(dx, name, var) DDX_DHtml_ElementText(dx, name, DISPID_IHTMLANCHORELEMENT_TARGET, var)
#define DDX_DHtml_Img_Src(dx, name, var) DDX_DHtml_ElementText(dx, name, DISPID_IHTMLIMGELEMENT_SRC, var)
#define DDX_DHtml_Frame_Src(dx, name, var) DDX_DHtml_ElementText(dx, name, DISPID_IHTMLFRAMEBASE_SRC, var)
#define DDX_DHtml_IFrame_Src(dx, name, var) DDX_DHtml_ElementText(dx, name, DISPID_IHTMLFRAMEBASE_SRC, var)


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
template < class TBase, class TWinBase = CWindowImpl< TBase > >
class CSymDHTMLWindow : 
    public CSymDHtmlEventSink, // <--- DONT CHANGE THE ORDER OF THIS CLASS INHERITANCE. CSymDHtmlEventSink should always be first...
    public TWinBase, 
    public CMessageFilter,
    public CSymDHtmlBrowserEventSink,
    public CWinDataExchange< TWinBase >
{
public:

    CSymDHTMLWindow();
    CSymDHTMLWindow( LPCTSTR szHtmlResID, HWND hParentWnd = NULL );
    ~CSymDHTMLWindow();

    HRESULT GetDHtmlDocument( IHTMLDocument2** pphtmlDoc );

    virtual BOOL CanAccessExternal();
    virtual void OnBeforeNavigate(LPDISPATCH pDisp, LPCTSTR szUrl);
    virtual void OnNavigateComplete(LPDISPATCH pDisp, LPCTSTR szUrl);
    virtual void OnDocumentComplete(LPDISPATCH pDisp, LPCTSTR szUrl);

    virtual BOOL OnInitDialog();

    BOOL LoadFromResource(LPCTSTR lpszResource);
    BOOL LoadFromResource(UINT nRes);
    void Navigate( LPCTSTR lpszURL, DWORD dwFlags= 0, LPCTSTR lpszTargetFrameName = NULL, LPCTSTR lpszHeaders = NULL, LPVOID lpvPostData = NULL, DWORD dwPostDataLen = 0 );
    void SetResourceDLL( LPCTSTR szResourceDLL ){ m_strResourceDLL = szResourceDLL; }
    void DestroyModeless();

    HRESULT SetImgSrc( LPCTSTR szId, LPCTSTR szSrc );
    HRESULT SetElementClassName( LPCTSTR szId, LPCTSTR szClass );
    HRESULT GetElementClassName( LPCTSTR szId, BSTR* bstrClassName );
    HRESULT DisplayElement( LPCTSTR szElementName, BOOL bShow );
    HRESULT GetMouseWheelDelta( long* pDelta );
    HRESULT SetActiveElement( LPCTSTR szID );
    HRESULT GetActiveElement( IHTMLElement** pElement );

    // Methods used for SELECT objects...
    HRESULT SelectAddString( LPCTSTR szId, LPCTSTR szText, LPCTSTR szValue, BOOL bSelected = FALSE );
    HRESULT SelectAddString( IHTMLSelectElement* pSelect, LPCTSTR szText, LPCTSTR szValue, BOOL bSelected = FALSE );
    HRESULT SelectGetText( LPCTSTR szId, BSTR* bstrText, long lIndex = -1 );
    HRESULT SelectGetText( IHTMLSelectElement* pSelect, BSTR* bstrText, long lIndex = -1 );
    HRESULT SelectGetValue( LPCTSTR szId, BSTR* bstrValue, long lIndex = -1 );
    HRESULT SelectGetValue( IHTMLSelectElement* pSelect, BSTR* bstrValue, long lIndex = -1 );

    HRESULT IsElementDisplayed( LPCTSTR szElementName, BOOL& bVisible );

    // DDX functions
    void DDX_DHtml_ElementText(BOOL bSaveAndValidate, LPCTSTR szId, DISPID dispid, CString& value);
    void DDX_DHtml_ElementText(BOOL bSaveAndValidate, LPCTSTR szId, DISPID dispid, short& value);
    void DDX_DHtml_ElementText(BOOL bSaveAndValidate, LPCTSTR szId, DISPID dispid, int& value);
    void DDX_DHtml_ElementText(BOOL bSaveAndValidate, LPCTSTR szId, DISPID dispid, long& value);
    void DDX_DHtml_ElementText(BOOL bSaveAndValidate, LPCTSTR szId, DISPID dispid, DWORD& value);
    void DDX_DHtml_ElementText(BOOL bSaveAndValidate, LPCTSTR szId, DISPID dispid, float& value);
    void DDX_DHtml_ElementText(BOOL bSaveAndValidate, LPCTSTR szId, DISPID dispid, double& value);
    void DDX_DHtml_CheckBox(BOOL bSaveAndValidate, LPCTSTR szId, int& value);
    void DDX_DHtml_Radio(BOOL bSaveAndValidate, LPCTSTR szId, long& value);
    void DDX_DHtml_SelectValue(BOOL bSaveAndValidate, LPCTSTR szId, CString& value);
    void DDX_DHtml_SelectString(BOOL bSaveAndValidate, LPCTSTR szId, CString& value);
    void DDX_DHtml_SelectIndex(BOOL bSaveAndValidate, LPCTSTR szId, long& value);
    void DDX_DHtml_AxControl(BOOL bSaveAndValidate, LPCTSTR szId, DISPID dispid, VARIANT& var);
    void DDX_DHtml_AxControl(BOOL bSaveAndValidate, LPCTSTR szId, LPCTSTR szPropName, VARIANT& var);

    // host handler
    void SetHostFlags(DWORD dwFlags);
    void SetExternalDispatch(IDispatch *pdispExternal);
    virtual BOOL IsExternalDispatchSafe();

    STDMETHOD(ShowContextMenu)(DWORD dwID, POINT *ppt, IUnknown *pcmdtReserved, IDispatch *pdispReserved);
    STDMETHOD(GetHostInfo)(DOCHOSTUIINFO *pInfo);
    STDMETHOD(ShowUI)(DWORD dwID, IOleInPlaceActiveObject *pActiveObject, IOleCommandTarget *pCommandTarget, IOleInPlaceFrame *pFrame, IOleInPlaceUIWindow *pDoc);
    STDMETHOD(HideUI)(void);
    STDMETHOD(UpdateUI)(void);
    STDMETHOD(EnableModeless)(BOOL fEnable);
    STDMETHOD(OnDocWindowActivate)(BOOL fActivate);
    STDMETHOD(OnFrameWindowActivate)(BOOL fActivate);
    STDMETHOD(ResizeBorder)(LPCRECT prcBorder, IOleInPlaceUIWindow *pUIWindow, BOOL fRameWindow);
    STDMETHOD(TranslateAccelerator)(LPMSG lpMsg, const GUID *pguidCmdGroup, DWORD nCmdID);
    STDMETHOD(GetOptionKeyPath)(LPOLESTR *pchKey, DWORD dw);
    STDMETHOD(GetDropTarget)(IDropTarget *pDropTarget, IDropTarget **ppDropTarget);
    STDMETHOD(GetExternal)(IDispatch **ppDispatch);
    STDMETHOD(TranslateUrl)(DWORD dwTranslate, OLECHAR *pchURLIn, OLECHAR **ppchURLOut);
    STDMETHOD(FilterDataObject)(IDataObject *pDO, IDataObject **ppDORet);

    HRESULT GetElement(LPCTSTR szElementId, IDispatch **ppdisp, BOOL *pbCollection = NULL);
    HRESULT GetElement(LPCTSTR szElementId, IHTMLElement **pphtmlElement);

    template <class Q>
        HRESULT GetElementInterface(LPCTSTR szElementId, Q** ppvObj)
    {
        return GetElementInterface(szElementId, __uuidof(Q), (void **) ppvObj);
    }

    HRESULT GetElementInterface(LPCTSTR szElementId, REFIID riid, void **ppvObj);
    BSTR GetElementText(LPCTSTR szElementId);
    void SetElementText(LPCTSTR szElementId, BSTR bstrText);
    void SetElementText(LPCTSTR szElementId, LPCTSTR szText);
    void SetElementText(IUnknown *punkElem, BSTR bstrText);
    BSTR GetElementHtml(LPCTSTR szElementId);
    void SetElementHtml(LPCTSTR szElementId, BSTR bstrText);
    void SetElementHtml(IUnknown *punkElem, BSTR bstrText);
    VARIANT GetElementProperty(LPCTSTR szElementId, DISPID dispid);
    void SetElementProperty(LPCTSTR szElementId, DISPID dispid, VARIANT *pVar);
    HRESULT GetControlDispatch(LPCTSTR szId, IDispatch **ppdisp);
    VARIANT GetControlProperty(IDispatch *pdispControl, DISPID dispid);
    VARIANT GetControlProperty(LPCTSTR szId, DISPID dispid);
    VARIANT GetControlProperty(LPCTSTR szId, LPCTSTR szPropName);
    void SetControlProperty(IDispatch *pdispControl, DISPID dispid, VARIANT *pVar);
    void SetControlProperty(LPCTSTR szElementId, DISPID dispid, VARIANT *pVar);
    void SetControlProperty(LPCTSTR szElementId, LPCTSTR szPropName, VARIANT *pVar);
    HRESULT GetEvent(IHTMLEventObj **ppEventObj);

    HRESULT SetElementWidth(LPCTSTR szElementId, LPCTSTR szWidth );
    HRESULT SetElementWidth(LPCTSTR szElementId, BSTR bstrWidth );
    HRESULT SetElementHeight(LPCTSTR szElementId, LPCTSTR szWidth );
    HRESULT SetElementHeight(LPCTSTR szElementId, BSTR bstrWidth );

    HRESULT SetElementButtonEnabled( LPCTSTR szElementId, BOOL bEnabled = TRUE );
    HRESULT GetElementButtonEnabled( LPCTSTR szElementId, BOOL& bEnabled );

    HRESULT GetElementRect( LPCTSTR szElementId, RECT& rc );
    HRESULT GetElementOffsetLeft( LPCTSTR szElementId, long& lLeft );
    HRESULT GetElementOffsetTop( LPCTSTR szElementId, long& lTop );
    HRESULT GetElementOffsetWidth( LPCTSTR szElementId, long& lWidth );
    HRESULT GetElementOffsetHeight( LPCTSTR szElementId, long& lHeight );

    HRESULT GetStyleSheetRule( LPCTSTR szGetStyleName, IHTMLRuleStyle** lpStyle );

    //Implementation
public:

    virtual BOOL PreTranslateMessage(MSG* pMsg);
    HRESULT OnDocumentReadyStateChange(IHTMLElement *phtmlElem);

    BEGIN_MSG_MAP( CSymDHTMLWindow )
        MESSAGE_HANDLER( WM_FORWARDMSG+OCM__BASE, OnForwardMessage )
        MESSAGE_HANDLER( WM_FORWARDMSG, OnForwardMessage )
        MESSAGE_HANDLER( WM_CREATE, OnCreate )
        MESSAGE_HANDLER( WM_INITDIALOG, OnInitDialog )
        MESSAGE_HANDLER( WM_DESTROY, OnDestroy )
        MESSAGE_HANDLER( WM_SIZE, OnSize )
        MESSAGE_HANDLER( WM_SETFOCUS, OnSetFocus )
    END_MSG_MAP()

protected:

    //-----------------------------------------------------------------------------
    //-----------------------------------------------------------------------------
    BEGIN_SYM_DHTML_EVENT_MAP_INLINE( CSymDHTMLWindow )
        SYM_DHTML_EVENT( DISPID_EVMETH_ONREADYSTATECHANGE, NULL, OnDocumentReadyStateChange )
    END_SYM_DHTML_EVENT_MAP_INLINE()

    HRESULT CreateCCWebWindow();

    LRESULT OnSetFocus(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
    LRESULT OnKillFocus(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
    LRESULT OnSize(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
    LRESULT OnDestroy(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
    LRESULT OnCreate(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
    LRESULT OnInitDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
    LRESULT OnForwardMessage(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
    

    void Initialize();

    // 
    // CSymDHtmlBrowserEventSink funcs....
    //
    virtual void BeforeNavigate(IDispatch* pDisp,VARIANT * URL, VARIANT * Flags, VARIANT * TargetFrameName, VARIANT * PostData, VARIANT * Headers, VARIANT_BOOL * Cancel);
    virtual void NavigateComplete(IDispatch* pDisp, VARIANT * URL);
    virtual void DocumentComplete(IDispatch* pDisp, VARIANT * URL);

    LRESULT OnDestroyModeless(WPARAM, LPARAM);

    // DDX Functions
    void DDX_DHtml_ElementText(LPCTSTR szId, DISPID dispid, CString& value, BOOL bSave);
    void DDX_DHtml_ElementText(LPCTSTR szId, DISPID dispid, short& value, BOOL bSave);
    void DDX_DHtml_ElementText(LPCTSTR szId, DISPID dispid, int& value, BOOL bSave);
    void DDX_DHtml_ElementText(LPCTSTR szId, DISPID dispid, long& value, BOOL bSave);
    void DDX_DHtml_ElementText(LPCTSTR szId, DISPID dispid, DWORD& value, BOOL bSave);
    void DDX_DHtml_ElementText(LPCTSTR szId, DISPID dispid, float& value, BOOL bSave);
    void DDX_DHtml_ElementText(LPCTSTR szId, DISPID dispid, double& value, BOOL bSave);

    /////////////////////////////////////////////////////////////////////////////
    // Data exchange for special controls
    void DDX_DHtml_CheckBox(LPCTSTR szId, int& value, BOOL bSave);
    void DDX_DHtml_Radio(LPCTSTR szId, long& value, BOOL bSave);
    void DDX_DHtml_SelectValue(LPCTSTR szId, CString& value, BOOL bSave);
    void DDX_DHtml_SelectString(LPCTSTR szId, CString& value, BOOL bSave);
    void DDX_DHtml_SelectIndex(LPCTSTR szId, long& value, BOOL bSave);
    void DDX_DHtml_AxControl(LPCTSTR szId, DISPID dispid, VARIANT& var, BOOL bSave);
    void DDX_DHtml_AxControl(LPCTSTR szId, LPCTSTR szPropName, VARIANT& var, BOOL bSave);
    void OnDDXError(LPCTSTR szId, UINT nIDPrompt, BOOL bSave);

    void __cdecl DDX_DHtml_ElementTextWithFormat(LPCTSTR szId,DISPID dispid, LPCTSTR lpszFormat, UINT nIDPrompt, BOOL bSave, ...);
    void DDX_DHtml_ElementTextFloatFormat(LPCTSTR szId,DISPID dispid, void* pData, double value, int nSizeGcvt, BOOL bSave);
    BOOL _SymSimpleScanf(LPCTSTR lpszText,LPCTSTR lpszFormat, va_list pData);
    BOOL _SymSimpleFloatParse(LPCTSTR lpszText, double& d);
    long Select_FindString(IHTMLSelectElement *pSelect, BSTR bstr, BOOL fExact);
    void SetFocusToElement(LPCTSTR szId);

    HRESULT ConnectDHtmlEvents(IUnknown *punkDoc);
    void DisconnectDHtmlEvents();
    HRESULT ConnectDHtmlElementEvents(DWORD_PTR dwThunkOffset = 0);
    BOOL FindSinkForObject(LPCTSTR szName);
    BOOL IsSinkedElement(IDispatch *pdispElem);
    void DisconnectDHtmlElementEvents();

    void GetCurrentUrl(CString& szUrl);

protected:

    BOOL m_bUseHtmlTitle;

    LPCTSTR m_szHtmlResID;

    CString m_strCurrentUrl;
    CString m_strResourceDLL;

    BOOL m_bAttachedControl;
    CWindow m_wndBrowser;

    //static UINT WM_DESTROYMODELESS;

    // host handler
    DWORD m_dwHostFlags;
    CComPtr< IDispatch > m_spExternalDisp;

    CWindow m_wndCCWeb;
    CWindow m_wndInternetExplorer;

    // event sink
    DWORD m_dwDHtmlEventSinkCookie;
    CSimpleArray< CSymDHtmlControlSink *> m_ControlSinks;
    CSimpleArray< CSymDHtmlElementEventSink *> m_SinkedElements;

    CComPtr< IccWebWindow >     m_spCCWebWindow;
    CComPtr< IWebBrowser2 >     m_pBrowserApp;
    CComPtr< IHTMLDocument2 >   m_spHTMLDocument2;
    CComPtr< IHTMLWindow2 >     m_spHTMLWindow2;
    CComPtr< IServiceProvider > m_spHTMLServiceProvider;

    StahlSoft::CSmartHandle m_sDocCompleteEvent;

	static CString m_strProductName;
	static CString m_strSarcThreatInfoUrl;
};

template < class TBase, class TWinBase >
CString CSymDHTMLWindow<TBase,TWinBase>::m_strProductName;

template < class TBase, class TWinBase >
CString CSymDHTMLWindow<TBase,TWinBase>::m_strSarcThreatInfoUrl;

//******************************************************************************
//******************************************************************************
//
//               I M P L I M E N T A T I O N    B E L O W
//
//******************************************************************************
//******************************************************************************


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
static BOOL CALLBACK FindInternetExplorerWindow( HWND hwnd, LPARAM lParam )
{
    BOOL bResult = TRUE;

    TCHAR szClassName[MAX_PATH];
    GetClassName( hwnd, szClassName, MAX_PATH );

    if( 0 == _tcscmp( szClassName, _T("Internet Explorer_Server")) )
    {
        HWND* pWnd = (HWND*)lParam;
        *pWnd = hwnd;

        bResult = FALSE;
    }

    return bResult;
}



//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
template < class TBase, class TWinBase >
CSymDHTMLWindow<TBase,TWinBase>::CSymDHTMLWindow()
{
    Initialize();
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
template < class TBase, class TWinBase >
CSymDHTMLWindow<TBase,TWinBase>::CSymDHTMLWindow( LPCTSTR szHtmlResID, HWND hParentWnd /*= NULL*/ ) :
m_szHtmlResID( szHtmlResID )
{
    Initialize();
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
template < class TBase, class TWinBase >
CSymDHTMLWindow<TBase,TWinBase>::~CSymDHTMLWindow()
{
    m_wndCCWeb.Detach();
    DisconnectDHtmlEvents();

    m_spHTMLDocument2 = NULL;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
template < class TBase, class TWinBase >
HRESULT CSymDHTMLWindow<TBase,TWinBase>::GetDHtmlDocument( IHTMLDocument2 **pphtmlDoc )
{
    HRESULT hResult = E_NOINTERFACE;

    if( ! pphtmlDoc )
    {
        ATLASSERT(FALSE);
        hResult = E_POINTER;
    }
    else
    {
        *pphtmlDoc = NULL;

        if( m_spHTMLDocument2 )
        {
            *pphtmlDoc = m_spHTMLDocument2;
            (*pphtmlDoc)->AddRef();

            hResult = S_OK;
        }
    }

    return hResult;
}


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
template < class TBase, class TWinBase >
BOOL CSymDHTMLWindow<TBase,TWinBase>::CanAccessExternal()
{
    BOOL bResult = FALSE;

    // if the dispatch we have is safe, we allow access
    if( IsExternalDispatchSafe() )
    {
        bResult = TRUE;
    }
    else
    {
        // the external dispatch is not safe, so we check whether the current 
        // zone allows for scripting of objects that are not safe for scripting
        if( !m_spHTMLDocument2 )
        {
            CComPtr<IInternetHostSecurityManager> spSecMan;
            m_spHTMLDocument2->QueryInterface(IID_IInternetHostSecurityManager, (void **) &spSecMan);

            if( spSecMan )
            {
                HRESULT hr = S_OK;
                hr = spSecMan->ProcessUrlAction(URLACTION_ACTIVEX_OVERRIDE_OBJECT_SAFETY, NULL, 0, NULL, 0, 0, PUAF_DEFAULT);

                if( S_OK == hr )
                {
                    bResult = TRUE;
                }
            }
        }
    }

    return bResult;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
template < class TBase, class TWinBase >
void CSymDHTMLWindow<TBase,TWinBase>::OnNavigateComplete(LPDISPATCH pDisp, LPCTSTR /*szUrl*/)
{
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
template < class TBase, class TWinBase >
void CSymDHTMLWindow<TBase,TWinBase>::OnDocumentComplete(LPDISPATCH pDisp, LPCTSTR /*szUrl*/)
{
    SetEvent(m_sDocCompleteEvent);
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
template < class TBase, class TWinBase >
BOOL CSymDHTMLWindow<TBase,TWinBase>::OnInitDialog()
{
	// Set title window
	SetWindowText(m_strProductName);

    return TRUE;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
template < class TBase, class TWinBase >
void CSymDHTMLWindow<TBase,TWinBase>::OnBeforeNavigate(LPDISPATCH pDisp, LPCTSTR /*szUrl*/)
{
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
template < class TBase, class TWinBase >
void CSymDHTMLWindow<TBase,TWinBase>::DisconnectDHtmlEvents()
{
    CComPtr<IHTMLDocument2> sphtmlDoc;
    GetDHtmlDocument( &sphtmlDoc );

    if( sphtmlDoc )
    {
        DisconnectDHtmlElementEvents();
        DisconnectFromConnectionPoint( sphtmlDoc, __uuidof(HTMLDocumentEvents2), m_dwDHtmlEventSinkCookie );
    }
}


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
template < class TBase, class TWinBase >
void CSymDHTMLWindow<TBase,TWinBase>::GetCurrentUrl(CString& szUrl)
{
    szUrl = m_strCurrentUrl;
}


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
template < class TBase, class TWinBase >
BOOL CSymDHTMLWindow<TBase,TWinBase>::LoadFromResource( LPCTSTR lpszResource )
{

	g_ResLoader.Initialize();
    HINSTANCE hInstance = g_ResLoader.GetResourceInstance();
    ATLASSERT( hInstance );

    CString strResourceURL;
    BOOL bRetVal = TRUE;

    TCHAR szModule[_MAX_PATH];
    int ret = GetModuleFileName(hInstance, szModule, _MAX_PATH);

    if (ret == 0 || ret == _MAX_PATH)
    {
        bRetVal = FALSE;
    }
    else
    {
        strResourceURL.Format(_T("res://%s/%s"), szModule, lpszResource);
        Navigate(strResourceURL, 0, 0, 0);
    }

    return bRetVal;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
template < class TBase, class TWinBase >
BOOL CSymDHTMLWindow<TBase,TWinBase>::LoadFromResource(UINT nRes)
{
	g_ResLoader.Initialize();
    HINSTANCE hInstance = g_ResLoader.GetResourceInstance();
    ATLASSERT( hInstance );

    CString strResourceURL;
    BOOL bRetVal = TRUE;
    TCHAR szModule[_MAX_PATH];

    if( GetModuleFileName(hInstance, szModule, _MAX_PATH) )
    {
        strResourceURL.Format(_T("res://%s/%d"), szModule, nRes);
        Navigate(strResourceURL, 0, 0, 0);
    }
    else
    {
        bRetVal = FALSE;
    }

    return bRetVal;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
template < class TBase, class TWinBase >
void CSymDHTMLWindow<TBase,TWinBase>::Navigate(LPCTSTR lpszURL, DWORD dwFlags /*= 0*/, 
                                               LPCTSTR lpszTargetFrameName /*= NULL*/, LPCTSTR lpszHeaders /*= NULL*/,
                                               LPVOID lpvPostData /*= NULL*/, DWORD dwPostDataLen /*= 0*/)
{
    CComBSTR bstrURL = lpszURL;

    _variant_t vFlags( dwFlags );
    _variant_t vHeaders( lpszHeaders );
    _variant_t vTragetFrameName( lpszTargetFrameName );

    m_pBrowserApp->Navigate( bstrURL, &vFlags, &vTragetFrameName, NULL, &vHeaders );
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
template < class TBase, class TWinBase >
void CSymDHTMLWindow<TBase,TWinBase>::DestroyModeless()
{
    //    if( !CSymDHTMLWindow<TBase,TWinBase>::WM_DESTROYMODELESS )
    //        CSymDHTMLWindow<TBase,TWinBase>::WM_DESTROYMODELESS = RegisterWindowMessage(_T("DHtmlDialogDestroy"));
    //
    //    PostMessage(CSymDHTMLWindow<TBase,TWinBase>::WM_DESTROYMODELESS, 0, 0);
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
template < class TBase, class TWinBase >
LRESULT CSymDHTMLWindow<TBase,TWinBase>::OnDestroy(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
    DisconnectDHtmlEvents();
    m_spHTMLDocument2 = NULL;

    // now tell the browser control we're shutting down
    if( m_pBrowserApp )
    {
        HRESULT hr = 0;
        hr = CSymDHtmlBrowserEventSink::UnAdvise( m_pBrowserApp );
        ATLASSERT( SUCCEEDED(hr) );

        m_pBrowserApp = NULL;
    }

    CMessageLoop* pLoop = _Module.GetMessageLoop();
    ATLASSERT(pLoop != NULL);
    pLoop->RemoveMessageFilter(this);

    return 0;
}


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
template < class TBase, class TWinBase >
void CSymDHTMLWindow<TBase,TWinBase>::Initialize()
{
    SetHostFlags( DOCHOSTUIFLAG_NO3DBORDER | DOCHOSTUIFLAG_SCROLL_NO );
    m_bUseHtmlTitle = FALSE;
    m_bAttachedControl = FALSE;
    m_pBrowserApp = NULL;
    m_dwDHtmlEventSinkCookie = 0;
    m_sDocCompleteEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
template < class TBase, class TWinBase >
void CSymDHTMLWindow<TBase,TWinBase>::BeforeNavigate( IDispatch* pDisp,VARIANT * URL, VARIANT * /*Flags*/, VARIANT * /*TargetFrameName*/, VARIANT * /*PostData*/, VARIANT * /*Headers*/, VARIANT_BOOL * /*Cancel*/)
{
    CString str(V_BSTR(URL));
    OnBeforeNavigate(pDisp, str);
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
template < class TBase, class TWinBase >
void CSymDHTMLWindow<TBase,TWinBase>::NavigateComplete(IDispatch* pDisp, VARIANT * URL)
{
    _variant_t vURL = URL;
    vURL.ChangeType( VT_BSTR );

    HRESULT hr = S_OK;

    m_strCurrentUrl = vURL;

    CComPtr< IDispatch > spDispDoc;
    m_pBrowserApp->get_Document( &spDispDoc );

    if( spDispDoc )
    {
        m_spHTMLDocument2 = NULL;
        spDispDoc->QueryInterface(IID_IHTMLDocument2, (void **) &m_spHTMLDocument2);
    }

    CWindow wndParent;
    wndParent = GetParent();

//    if( m_bUseHtmlTitle )
//    {
//        _bstr_t bstrTitle;
//        hr = m_spHTMLDocument2->get_title( bstrTitle.GetAddress() );
//
//        if( SUCCEEDED(hr) )
//            wndParent.SetWindowText( bstrTitle );
//    }

    OnNavigateComplete( pDisp, m_strCurrentUrl );
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
template < class TBase, class TWinBase >
void CSymDHTMLWindow<TBase,TWinBase>::DocumentComplete(IDispatch* pDisp, VARIANT * URL)
{
    _variant_t vURL = URL;
    vURL.ChangeType( VT_BSTR );

    CString strURL = vURL;

    // ccWebWnd set the page to about:blank on close - don't need connect events
    // for about:blank...  [CBROWN]
    if( ! (0 == strURL.CompareNoCase(_T("about:blank")))) 
    {
        if( pDisp == m_pBrowserApp )
        {
            ConnectDHtmlElementEvents((((DWORD_PTR)static_cast< CSymDHtmlSinkHandler* >(this)) - (DWORD_PTR)this));
            OnDocumentComplete(pDisp, strURL);
        }
    }
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
template < class TBase, class TWinBase >
HRESULT CSymDHTMLWindow<TBase,TWinBase>::OnDocumentReadyStateChange(IHTMLElement * /*phtmlElem*/)
{
    CComPtr<IHTMLDocument2> sphtmlDoc;
    GetDHtmlDocument(&sphtmlDoc);

    if( sphtmlDoc )
    {
        CComBSTR bstrState;
        sphtmlDoc->get_readyState(&bstrState);

        if( bstrState )
        {
            if( bstrState==TEXT("complete") )
            {
                ConnectDHtmlElementEvents((((DWORD_PTR)static_cast< CSymDHtmlSinkHandler* >(this)) - (DWORD_PTR)this));
            }
            else if (bstrState==TEXT("loading"))
            {
                DisconnectDHtmlElementEvents();
            }
        }
    }
    return S_OK;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
template < class TBase, class TWinBase >
LRESULT CSymDHTMLWindow<TBase,TWinBase>::OnDestroyModeless(WPARAM, LPARAM)
{
    DestroyWindow();
    return 0;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
template < class TBase, class TWinBase >
void CSymDHTMLWindow<TBase,TWinBase>::DDX_DHtml_ElementText(BOOL bSaveAndValidate, LPCTSTR szId, DISPID dispid, CString& value)
{
    DDX_DHtml_ElementText(szId, dispid, value, bSaveAndValidate);
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
template < class TBase, class TWinBase >
void CSymDHTMLWindow<TBase,TWinBase>::DDX_DHtml_ElementText(BOOL bSaveAndValidate, LPCTSTR szId, DISPID dispid, short& value)
{
    DDX_DHtml_ElementText(szId, dispid, value, bSaveAndValidate);
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
template < class TBase, class TWinBase >
void CSymDHTMLWindow<TBase,TWinBase>::DDX_DHtml_ElementText(BOOL bSaveAndValidate, LPCTSTR szId, DISPID dispid, int& value)
{
    DDX_DHtml_ElementText(szId, dispid, value, bSaveAndValidate);
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
template < class TBase, class TWinBase >
void CSymDHTMLWindow<TBase,TWinBase>::DDX_DHtml_ElementText(BOOL bSaveAndValidate, LPCTSTR szId, DISPID dispid, long& value)
{
    DDX_DHtml_ElementText(szId, dispid, value, bSaveAndValidate);
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
template < class TBase, class TWinBase >
void CSymDHTMLWindow<TBase,TWinBase>::DDX_DHtml_ElementText(BOOL bSaveAndValidate, LPCTSTR szId, DISPID dispid, DWORD& value)
{
    DDX_DHtml_ElementText(szId, dispid, value, bSaveAndValidate);
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
template < class TBase, class TWinBase >
void CSymDHTMLWindow<TBase,TWinBase>::DDX_DHtml_ElementText(BOOL bSaveAndValidate, LPCTSTR szId, DISPID dispid, float& value)
{
    DDX_DHtml_ElementText(szId, dispid, value, bSaveAndValidate);
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
template < class TBase, class TWinBase >
void CSymDHTMLWindow<TBase,TWinBase>::DDX_DHtml_ElementText(BOOL bSaveAndValidate, LPCTSTR szId, DISPID dispid, double& value)
{
    DDX_DHtml_ElementText(szId, dispid, value, bSaveAndValidate);
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
template < class TBase, class TWinBase >
void CSymDHTMLWindow<TBase,TWinBase>::DDX_DHtml_CheckBox(BOOL bSaveAndValidate, LPCTSTR szId, int& value)
{
    DDX_DHtml_CheckBox(szId, value, bSaveAndValidate);
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
template < class TBase, class TWinBase >
void CSymDHTMLWindow<TBase,TWinBase>::DDX_DHtml_Radio(BOOL bSaveAndValidate, LPCTSTR szId, long& value)
{
    DDX_DHtml_Radio(szId, value, bSaveAndValidate);
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
template < class TBase, class TWinBase >
void CSymDHTMLWindow<TBase,TWinBase>::DDX_DHtml_SelectValue(BOOL bSaveAndValidate, LPCTSTR szId, CString& value)
{
    DDX_DHtml_SelectValue(szId, value, bSaveAndValidate);
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
template < class TBase, class TWinBase >
void CSymDHTMLWindow<TBase,TWinBase>::DDX_DHtml_SelectString(BOOL bSaveAndValidate, LPCTSTR szId, CString& value)
{
    DDX_DHtml_SelectString(szId, value, bSaveAndValidate);
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
template < class TBase, class TWinBase >
void CSymDHTMLWindow<TBase,TWinBase>::DDX_DHtml_SelectIndex(BOOL bSaveAndValidate, LPCTSTR szId, long& value)
{
    DDX_DHtml_SelectIndex(szId, value, bSaveAndValidate);
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
template < class TBase, class TWinBase >
void CSymDHTMLWindow<TBase,TWinBase>::DDX_DHtml_AxControl(BOOL bSaveAndValidate, LPCTSTR szId, DISPID dispid, VARIANT& var)
{
    DDX_DHtml_AxControl(szId, dispid, var, bSaveAndValidate);
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
template < class TBase, class TWinBase >
void CSymDHTMLWindow<TBase,TWinBase>::DDX_DHtml_AxControl(BOOL bSaveAndValidate, LPCTSTR szId, LPCTSTR szPropName, VARIANT& var)
{
    DDX_DHtml_AxControl(szId, szPropName, var, bSaveAndValidate);
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
template < class TBase, class TWinBase >
void CSymDHTMLWindow<TBase,TWinBase>::DDX_DHtml_ElementText(LPCTSTR szId, DISPID dispid, CString& value, BOOL bSave)
{
    CComPtr<IHTMLDocument2> sphtmlDoc;
    GetDHtmlDocument(&sphtmlDoc);
    if (sphtmlDoc == NULL)
        return;

    CComVariant var;

    if( bSave )
    {
        var = GetElementProperty(szId, dispid);
        var.ChangeType(VT_BSTR);
        value = var.bstrVal;
    }
    else
    {
        var = (LPCTSTR)value;
        SetElementProperty(szId, dispid, &var);
    }
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
template < class TBase, class TWinBase >
void CSymDHTMLWindow<TBase,TWinBase>::DDX_DHtml_ElementText(LPCTSTR szId, DISPID dispid, short& value, BOOL bSave)
{
    if( bSave )
    {
        DDX_DHtml_ElementTextWithFormat(szId, dispid, _T("%sd"), -1, bSave, &value);
    }
    else
    {
        DDX_DHtml_ElementTextWithFormat(szId, dispid, _T("%hd"), -1, bSave, value);
    }
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
template < class TBase, class TWinBase >
void CSymDHTMLWindow<TBase,TWinBase>::DDX_DHtml_ElementText(LPCTSTR szId, DISPID dispid, int& value, BOOL bSave)
{
    if( bSave )
    {
        DDX_DHtml_ElementTextWithFormat(szId, dispid, _T("%d"), -1, bSave, &value);
    }
    else
    {
        DDX_DHtml_ElementTextWithFormat(szId, dispid, _T("%d"), -1, bSave, value);
    }
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
template < class TBase, class TWinBase >
void CSymDHTMLWindow<TBase,TWinBase>::DDX_DHtml_ElementText(LPCTSTR szId, DISPID dispid, long& value, BOOL bSave)
{
    if( bSave )
    {
        DDX_DHtml_ElementTextWithFormat(szId, dispid, _T("%ld"), -1, bSave, &value);
    }
    else
    {
        DDX_DHtml_ElementTextWithFormat(szId, dispid, _T("%ld"), -1, bSave, value);
    }
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
template < class TBase, class TWinBase >
void CSymDHTMLWindow<TBase,TWinBase>::DDX_DHtml_ElementText(LPCTSTR szId, DISPID dispid, DWORD& value, BOOL bSave)
{
    if( bSave )
    {
        DDX_DHtml_ElementTextWithFormat(szId, dispid, _T("%lu"), -1, bSave, &value);
    }
    else
    {
        DDX_DHtml_ElementTextWithFormat(szId, dispid, _T("%lu"), -1, bSave, value);
    }
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
template < class TBase, class TWinBase >
void CSymDHTMLWindow<TBase,TWinBase>::DDX_DHtml_ElementText(LPCTSTR szId, DISPID dispid, float& value, BOOL bSave)
{
    DDX_DHtml_ElementTextFloatFormat(szId, dispid, &value, value, FLT_DIG, bSave);
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
template < class TBase, class TWinBase >
void CSymDHTMLWindow<TBase,TWinBase>::DDX_DHtml_ElementText(LPCTSTR szId, DISPID dispid, double& value, BOOL bSave)
{
    DDX_DHtml_ElementTextFloatFormat(szId, dispid, &value, value, DBL_DIG, bSave);
}

//-----------------------------------------------------------------------------
// Data exchange for special controls
//-----------------------------------------------------------------------------
template < class TBase, class TWinBase >
void CSymDHTMLWindow<TBase,TWinBase>::DDX_DHtml_CheckBox(LPCTSTR szId, int& value, BOOL bSave)
{
    CComVariant var;

    CComPtr<IHTMLDocument2> sphtmlDoc;
    GetDHtmlDocument(&sphtmlDoc);
    if (sphtmlDoc == NULL)
        return;

    CComPtr<IHTMLOptionButtonElement> spOptionButton;
    HRESULT hr = S_OK;

    hr = GetElementInterface(szId, __uuidof(IHTMLOptionButtonElement), (void **) &spOptionButton);
    if (spOptionButton == NULL)
        goto Error;

    if( bSave )
    {
        VARIANT_BOOL bIndeterminate;
        VARIANT_BOOL bChecked;
        hr = spOptionButton->get_checked(&bChecked);
        if (FAILED(hr))
            goto Error;
        hr = spOptionButton->get_indeterminate(&bIndeterminate);
        if (FAILED(hr))
            goto Error;
        value = ((bChecked != ATL_VARIANT_FALSE) ? BST_CHECKED : BST_UNCHECKED) | 
            ((bIndeterminate != ATL_VARIANT_FALSE) ? BST_INDETERMINATE : 0);
    }
    else
    {
        hr = spOptionButton->put_checked((value & BST_CHECKED) ? ATL_VARIANT_TRUE : ATL_VARIANT_FALSE);
        if (FAILED(hr))
            goto Error;
        hr = spOptionButton->put_indeterminate((value & BST_INDETERMINATE) ? ATL_VARIANT_TRUE : ATL_VARIANT_FALSE);
        if (FAILED(hr))
            goto Error;
    }
Error:
    if (FAILED(hr))
    {
        ATLTRACE("Failed DDX_DHtml_CheckBox\n");
    }
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
template < class TBase, class TWinBase >
void CSymDHTMLWindow<TBase,TWinBase>::DDX_DHtml_Radio(LPCTSTR szId, long& value, BOOL bSave)
{
    CComVariant var;

    CComPtr<IHTMLDocument2> sphtmlDoc;
    GetDHtmlDocument(&sphtmlDoc);
    if (sphtmlDoc == NULL)
        return;

    CComPtr<IHTMLOptionButtonElement> spOptionButton;
    CComPtr<IDispatch> spdispCollection;
    CComPtr<IHTMLElementCollection> spElementColl;
    BOOL bIsCollection = FALSE;
    CComPtr<IDispatch> spdispElem;

    HRESULT hr = S_OK;
    CComVariant varIndex;
    CComVariant varEmpty;
    varIndex.vt = VT_I4;
    varIndex.lVal = 0;

    // get the radio buttons in the group
    hr = GetElement(szId, &spdispCollection, &bIsCollection);
    if (spdispCollection && bIsCollection)
    {
        hr = spdispCollection->QueryInterface(__uuidof(IHTMLElementCollection), (void **) &spElementColl);
        if (spElementColl == NULL)
        {
            // Error: Collection didn't support IHTMLElementCollection!
            ATLASSERT(FALSE);
            goto Error;
        }

        if( bSave )
            value = -1;     // value if none found

        long lCount = 0;
        spElementColl->get_length(&lCount);

        for (long lCntr = 0; lCntr < lCount; lCntr++)
        {				
            spdispElem = NULL;
            spOptionButton = NULL;
            varIndex.lVal = lCntr;
            hr = spElementColl->item(varIndex, varEmpty, &spdispElem);
            if (spdispElem == NULL)
                break;
            hr = spdispElem->QueryInterface(__uuidof(IHTMLOptionButtonElement), (void **) &spOptionButton);

            if( bSave )
            {
                VARIANT_BOOL bChecked;
                hr = spOptionButton->get_checked(&bChecked);
                if (FAILED(hr))
                    goto Error;
                if (bChecked != ATL_VARIANT_FALSE)
                {
                    value = varIndex.lVal;
                    break;
                }
            }
            else
            {
                if (varIndex.lVal == value)
                {
                    spOptionButton->put_checked(ATL_VARIANT_TRUE);
                    break;
                }
            }
        };
    }
Error:
    if (FAILED(hr))
    {
        ATLTRACE("Warning: Failed DDX_DHtml_Radio\n");
    }
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
template < class TBase, class TWinBase >
void CSymDHTMLWindow<TBase,TWinBase>::DDX_DHtml_SelectValue(LPCTSTR szId, CString& value, BOOL bSave)
{
    CComPtr<IHTMLDocument2> sphtmlDoc;
    GetDHtmlDocument(&sphtmlDoc);
    if (sphtmlDoc == NULL)
        return;

    CComBSTR bstrText;
    CComPtr<IHTMLSelectElement> spSelect;
    HRESULT hr = GetElementInterface(szId, __uuidof(IHTMLSelectElement), (void **) &spSelect);
    if (FAILED(hr))
        goto Error;

    if( bSave )
    {
        spSelect->get_value(&bstrText);
        if (bstrText)
            value = bstrText;
    }
    else
    {
        bstrText.Attach(value.AllocSysString());
        spSelect->put_value(bstrText);
    }

Error:
    return;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
template < class TBase, class TWinBase >
void CSymDHTMLWindow<TBase,TWinBase>::DDX_DHtml_SelectString(LPCTSTR szId, CString& value, BOOL bSave)
{
    CComPtr<IHTMLDocument2> sphtmlDoc;
    GetDHtmlDocument(&sphtmlDoc);
    if (sphtmlDoc == NULL)
        return;

    CComVariant varEmpty, varIndex;

    CComPtr<IHTMLSelectElement> spSelect;
    CComPtr<IDispatch> spdispOption;
    CComPtr<IHTMLOptionElement> spOption;
    CComBSTR bstrText;
    HRESULT hr = S_OK;
    long lIndex=-1;

    hr = GetElementInterface(szId, __uuidof(IHTMLSelectElement), (void **) &spSelect);
    if (spSelect == NULL)
        return;

    if( bSave )
    {
        // get the selected item
        value.Empty();
        spSelect->get_selectedIndex(&lIndex);
        if (lIndex >= 0)
        {
            varIndex = lIndex;

            spSelect->item(varIndex, varEmpty, &spdispOption);
            if (spdispOption)
            {
                spdispOption->QueryInterface(__uuidof(IHTMLOptionElement), (void **) &spOption);
                if (spOption)
                {
                    spOption->get_text(&bstrText);
                    if (bstrText)
                        value = bstrText;
                }
            }
        }
    }
    else
    {
        bstrText.Attach(value.AllocSysString());
        lIndex = Select_FindString(spSelect, bstrText, FALSE);
        spSelect->put_selectedIndex(lIndex);
    }
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
template < class TBase, class TWinBase >
void CSymDHTMLWindow<TBase,TWinBase>::DDX_DHtml_SelectIndex(LPCTSTR szId, long& value, BOOL bSave)
{
    CComPtr<IHTMLDocument2> sphtmlDoc;
    GetDHtmlDocument(&sphtmlDoc);
    if (sphtmlDoc == NULL)
        return;

    CComPtr<IHTMLSelectElement> spSelect;
    HRESULT hr;

    hr = GetElementInterface(szId, __uuidof(IHTMLSelectElement), (void **) &spSelect);
    if( !spSelect )
        return;

    if( bSave )
    {
        spSelect->get_selectedIndex(&value);
    }
    else
    {
        spSelect->put_selectedIndex(value);
    }
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
template < class TBase, class TWinBase >
void CSymDHTMLWindow<TBase,TWinBase>::DDX_DHtml_AxControl(LPCTSTR szId, DISPID dispid, VARIANT& var, BOOL bSave)
{
    if( bSave )
    {
        var = GetControlProperty(szId, dispid);
    }
    else
    {
        SetControlProperty(szId, dispid, &var);
    }
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
template < class TBase, class TWinBase >
void CSymDHTMLWindow<TBase,TWinBase>::DDX_DHtml_AxControl(LPCTSTR szId, LPCTSTR szPropName, VARIANT& var, BOOL bSave)
{
    if( bSave )
    {
        var = GetControlProperty(szId, szPropName);
    }
    else
    {
        SetControlProperty(szId, szPropName, &var);
    }

}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
template < class TBase, class TWinBase >
void CSymDHTMLWindow<TBase,TWinBase>::OnDDXError(LPCTSTR szId, UINT nIDPrompt, BOOL /*bSave*/)
{
    //MessageBox(nIDPrompt);
    // default implementation just sets the focus back to the offending element
    SetFocusToElement(szId);
    CComPtr<IHTMLEventObj> sphtmlEvent;
    GetEvent(&sphtmlEvent);

    if (sphtmlEvent)
        sphtmlEvent->put_cancelBubble(ATL_VARIANT_TRUE);

}

//-----------------------------------------------------------------------------
// only supports windows output formats - no floating point
//-----------------------------------------------------------------------------
template < class TBase, class TWinBase >
void __cdecl CSymDHTMLWindow<TBase,TWinBase>::DDX_DHtml_ElementTextWithFormat(LPCTSTR szId, DISPID dispid, LPCTSTR lpszFormat, UINT nIDPrompt, BOOL bSave, ...)
{
    va_list pData;
    va_start(pData, bSave);

    CString value;

    if( bSave )
    {
        DDX_DHtml_ElementText(szId, dispid, value, bSave);
        // the following works for %d, %u, %ld, %lu
        if (!_SymSimpleScanf(value, lpszFormat, pData))
        {
            // set the focus to the offending element
            SetFocusToElement(szId);

            OnDDXError(szId, nIDPrompt, bSave);
            va_end(pData);
            return;
        }
    }
    else
    {
        CString strTemp ;
        strTemp.FormatV(lpszFormat,pData);
        // does not support floating point numbers - see dlgfloat.cpp
        DDX_DHtml_ElementText(szId, dispid, strTemp, bSave);
    }
    va_end(pData);
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
template < class TBase, class TWinBase >
void CSymDHTMLWindow<TBase,TWinBase>::DDX_DHtml_ElementTextFloatFormat(LPCTSTR szId, DISPID dispid, void* pData, double value, int nSizeGcvt, BOOL bSave)
{
    ATLASSERT(pData != NULL);

    CString strValue;
    if( bSave )
    {
        DDX_DHtml_ElementText(szId, dispid, strValue, bSave);
        double d;
        if (!_SymSimpleFloatParse(strValue, d))
        {
            OnDDXError(szId, AFX_IDP_PARSE_REAL, bSave);
        }
        if (nSizeGcvt == FLT_DIG)
            *((float*)pData) = (float)d;
        else
            *((double*)pData) = d;
    }
    else
    {
        CString szBuffer;
        szBuffer.Format(_T("%.*g"), nSizeGcvt, value);
        DDX_DHtml_ElementText(szId, dispid, szBuffer, bSave);
    }
}


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
template < class TBase, class TWinBase >
BOOL CSymDHTMLWindow<TBase,TWinBase>::_SymSimpleScanf(LPCTSTR lpszText,LPCTSTR lpszFormat, va_list pData)
{
    ATLASSERT(lpszText != NULL);
    ATLASSERT(lpszFormat != NULL);

    ATLASSERT(*lpszFormat == '%');
    lpszFormat++;        // skip '%'

    BOOL bLong = FALSE;
    BOOL bShort = FALSE;
    if (*lpszFormat == 'l')
    {
        bLong = TRUE;
        lpszFormat++;
    }
    else if (*lpszFormat == 's')
    {
        bShort = TRUE;
        lpszFormat++;
    }

    ATLASSERT(*lpszFormat == 'd' || *lpszFormat == 'u');
    ATLASSERT(lpszFormat[1] == '\0');

    while (*lpszText == ' ' || *lpszText == '\t')
        lpszText++;
    TCHAR chFirst = lpszText[0];
    long l, l2;
    if (*lpszFormat == 'd')
    {
        // signed
        l = _tcstol(lpszText, (LPTSTR*)&lpszText, 10);
        l2 = (int)l;
    }
    else
    {
        // unsigned
        if (*lpszText == '-')
            return FALSE;
        l = (long)_tcstoul(lpszText, (LPTSTR*)&lpszText, 10);
        l2 = (unsigned int)l;
    }
    if (l == 0 && chFirst != '0')
        return FALSE;   // could not convert

    while (*lpszText == ' ' || *lpszText == '\t')
        lpszText++;
    if (*lpszText != '\0')
        return FALSE;   // not terminated properly

    if (bShort)
    {
        if ((short)l != l)
            return FALSE;   // too big for short
        *va_arg(pData, short*) = (short)l;
    }
    else
    {
        ATLASSERT(sizeof(long) == sizeof(int));
        ATLASSERT(l == l2);
        *va_arg(pData, long*) = l;
    }

    // all ok
    return TRUE;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
template < class TBase, class TWinBase >
BOOL CSymDHTMLWindow<TBase,TWinBase>::_SymSimpleFloatParse(LPCTSTR lpszText, double& d)
{
    ATLASSERT(lpszText != NULL);
    while (*lpszText == ' ' || *lpszText == '\t')
        lpszText++;

    TCHAR chFirst = lpszText[0];
    d = _tcstod(lpszText, (LPTSTR*)&lpszText);

    if (d == 0.0 && chFirst != '0')
        return FALSE;   // could not convert

    while (*lpszText == ' ' || *lpszText == '\t')
        lpszText++;

    if (*lpszText != '\0')
        return FALSE;   // not terminated properly

    return TRUE;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
template < class TBase, class TWinBase >
long CSymDHTMLWindow<TBase,TWinBase>::Select_FindString(IHTMLSelectElement *pSelect, BSTR bstr, BOOL /*fExact*/)
{
    long lIndexFound = -1;
    CComVariant varIndex, varEmpty;
    CComPtr<IDispatch> spdispOption;
    CComPtr<IHTMLOptionElement> spOption;
    long lCount = 0;

    HRESULT hr = pSelect->get_length(&lCount);
    if (FAILED(hr))
        goto Error;

    varIndex.vt = VT_I4;

    // loop through the items searching for the string
    for (varIndex.lVal=0; varIndex.lVal<lCount; varIndex.lVal++)
    {
        pSelect->item(varIndex, varEmpty, &spdispOption);
        if (spdispOption)
        {
            hr = spdispOption->QueryInterface(__uuidof(IHTMLOptionElement), (void **) &spOption);

            if( SUCCEEDED(hr) && spOption )
            {
                CComBSTR bstrText;
                hr = spOption->get_text(&bstrText);

                if( SUCCEEDED(hr) && (bstrText && !wcscmp(bstrText, bstr)) )
                {
                    // we found it
                    lIndexFound = varIndex.lVal;
                    break;
                }
                spOption = NULL;
            }
            spdispOption = NULL;
        }
    }

Error:
    return lIndexFound;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
template < class TBase, class TWinBase >
void CSymDHTMLWindow<TBase,TWinBase>::SetFocusToElement(LPCTSTR szId)
{
    // check if the element is a control element
    CComPtr<IHTMLControlElement> sphtmlCtrlElem;
    HRESULT hr = 0;
    hr = GetElementInterface(szId, &sphtmlCtrlElem);

    if( SUCCEEDED(hr) && sphtmlCtrlElem )
    {
        sphtmlCtrlElem->focus();
        return;
    }

    // check if the element is an anchor element
    CComPtr<IHTMLAnchorElement> sphtmlAnchorElem;
    hr = GetElementInterface(szId, &sphtmlAnchorElem);

    if( SUCCEEDED(hr) && sphtmlAnchorElem )
    {
        sphtmlAnchorElem->focus();
        return;
    }

    // otherwise all we can do is scroll the element into view
    CComPtr<IHTMLElement> sphtmlElem;
    hr = GetElementInterface(szId, &sphtmlElem);

    if( SUCCEEDED(hr) && sphtmlElem )
    {
        CComVariant var;
        var.vt = VT_BOOL;
        var.boolVal = ATL_VARIANT_TRUE;
        sphtmlElem->scrollIntoView(var);
    }
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
template < class TBase, class TWinBase >
HRESULT CSymDHTMLWindow<TBase,TWinBase>::ConnectDHtmlEvents(IUnknown *punkDoc)
{
    return ConnectToConnectionPoint(punkDoc, __uuidof(HTMLDocumentEvents2), &m_dwDHtmlEventSinkCookie);
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
template < class TBase, class TWinBase >
HRESULT CSymDHTMLWindow<TBase,TWinBase>::ConnectDHtmlElementEvents(DWORD_PTR dwThunkOffset /*= 0*/)
{
    HRESULT hr = S_OK;
    const SymDHtmlEventMapEntry* pEventMap = GetDHtmlEventMap();
    if (!pEventMap)
        return hr;

    for( int i=0; pEventMap[i].nType != SYM_DHTMLEVENTMAPENTRY_END; i++ )
    {
        if (pEventMap[i].nType==SYM_DHTMLEVENTMAPENTRY_ELEMENT)
        {
            // an element name must be specified when using element events
            ATLASSERT(pEventMap[i].szName);

            // connect to the element's event sink
            CComPtr<IDispatch> spdispElement;
            hr = GetElement(pEventMap[i].szName, &spdispElement);

            if( SUCCEEDED(hr) && spdispElement )
            {
                if( !IsSinkedElement(spdispElement) )
                {
                    CSymDHtmlElementEventSink *pSink = NULL;
                    ATLTRY(pSink = new CSymDHtmlElementEventSink(this, spdispElement));
                    if (pSink == NULL)
                        return E_OUTOFMEMORY;

                    hr = AtlAdvise(spdispElement, pSink, __uuidof(IDispatch), &pSink->m_dwCookie);

                    if (SUCCEEDED(hr))
                    {
                        m_SinkedElements.Add(pSink);
                    }
                    else
                    {
                        delete pSink;
                    }
#ifdef _DEBUG
                    //                    if (FAILED(hr))
                    //                        ATLTRACE(traceHtml, 0, "Warning: Failed to connect to ConnectionPoint!\n");
#endif
                }
            }
        }
        else if (pEventMap[i].nType==SYM_DHTMLEVENTMAPENTRY_CONTROL)
        {
            // check if we already have a sink connected to this control
            if (!FindSinkForObject(pEventMap[i].szName))
            {
                // create a new sink and connect it to the element's event sink
                CComPtr<IDispatch> spdispElement;
                GetElement(pEventMap[i].szName, &spdispElement);

                if( spdispElement )
                {
                    CComPtr<IHTMLObjectElement> sphtmlObj;
                    spdispElement->QueryInterface(__uuidof(IHTMLObjectElement), (void **) &sphtmlObj);

                    if( sphtmlObj )
                    {
                        CComPtr<IDispatch> spdispControl;
                        sphtmlObj->get_object(&spdispControl);

                        if( spdispControl )
                        {
                            // create a new control sink to connect to the control's events
                            CSymDHtmlControlSink *pSink = NULL; 
                            ATLTRY(pSink = new CSymDHtmlControlSink(spdispControl, this, pEventMap[i].szName, dwThunkOffset));

                            if (pSink == NULL)
                                return E_OUTOFMEMORY;

                            m_ControlSinks.Add(pSink);
                        }
                    }
                }
            }
        }
    }

    return hr;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
template < class TBase, class TWinBase >
BOOL CSymDHTMLWindow<TBase,TWinBase>::FindSinkForObject(LPCTSTR szName)
{
    int nLength = m_ControlSinks.GetSize();
    for (int i=0; i<nLength; i++)
    {
        if (!_tcscmp(szName, m_ControlSinks[i]->m_szControlId))
            return TRUE;
    }
    return FALSE;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
template < class TBase, class TWinBase >
BOOL CSymDHTMLWindow<TBase,TWinBase>::IsSinkedElement(IDispatch *pdispElem)
{
    CComPtr<IUnknown> spunk;
    pdispElem->QueryInterface(__uuidof(IUnknown), (void **) &spunk);

    if (!spunk)
        return FALSE;

    for (int i=0; i<m_SinkedElements.GetSize(); i++)
    {
        if (spunk == m_SinkedElements[i]->m_spunkElem)
            return TRUE;
    }

    return FALSE;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
template < class TBase, class TWinBase >
void CSymDHTMLWindow<TBase,TWinBase>::DisconnectDHtmlElementEvents()
{
    const SymDHtmlEventMapEntry* pEventMap = GetDHtmlEventMap();

    if (!pEventMap)
        return;

    int i;

    // disconnect from element events
    for (i=0; i<m_SinkedElements.GetSize(); i++)
    {
        CSymDHtmlElementEventSink *pSink = m_SinkedElements[i];
        AtlUnadvise(pSink->m_spunkElem, __uuidof(IDispatch), pSink->m_dwCookie);
        delete pSink;
    }
    m_SinkedElements.RemoveAll();

    // disconnect from control events
    for (i=0; i<m_ControlSinks.GetSize(); i++)
    {
        DisconnectFromConnectionPoint(m_ControlSinks[i]->m_spunkObj, m_ControlSinks[i]->m_iid, m_ControlSinks[i]->m_dwCookie);
        delete m_ControlSinks[i];
    }
    m_ControlSinks.RemoveAll();
    return;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
template < class TBase, class TWinBase >
HRESULT CSymDHTMLWindow<TBase,TWinBase>::GetElement(LPCTSTR szElementId, IDispatch **ppdisp, BOOL *pbCollection /*= NULL*/)
{
    CComPtr<IHTMLElementCollection> sphtmlAll;
    CComPtr<IHTMLElementCollection> sphtmlColl;
    CComPtr<IDispatch> spdispElem;
    CComVariant varName;
    CComVariant varIndex;
    HRESULT hr = S_OK;
    CComPtr<IHTMLDocument2> sphtmlDoc;

    *ppdisp = NULL;

    if (pbCollection)
        *pbCollection = FALSE;

    hr = GetDHtmlDocument(&sphtmlDoc);
    if (sphtmlDoc == NULL)
        return hr;

    if(szElementId == NULL)
        return E_INVALIDARG;

    varName.vt = VT_BSTR;
    varName.bstrVal = T2BSTR_EX(szElementId);

#ifndef _UNICODE	
    if (!varName.bstrVal)
    {
        hr = E_OUTOFMEMORY;
        goto Error;
    }
#endif

    hr = sphtmlDoc->get_all(&sphtmlAll);
    if (sphtmlAll == NULL)
        goto Error;
    hr = sphtmlAll->item(varName, varIndex, &spdispElem);
    if (spdispElem == NULL)
    {
        hr = E_NOINTERFACE;
        goto Error;
    }

    spdispElem->QueryInterface(__uuidof(IHTMLElementCollection), (void **) &sphtmlColl);
    if (sphtmlColl)
    {
        if (pbCollection)
            *pbCollection = TRUE;
#ifdef _DEBUG
        else
        {
            //ATLTRACE(traceHtml, 0, "Warning: duplicate IDs or NAMEs.\n");
            ATLASSERT(FALSE);
        }
#endif

    }
Error:
    if (SUCCEEDED(hr))
    {
        *ppdisp = spdispElem;
        if (spdispElem)
            (*ppdisp)->AddRef();
    }
    return hr;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
template < class TBase, class TWinBase >
HRESULT CSymDHTMLWindow<TBase,TWinBase>::GetElement(LPCTSTR szElementId, IHTMLElement **pphtmlElement)
{
    return GetElementInterface(szElementId, __uuidof(IHTMLElement), (void **) pphtmlElement);
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
template < class TBase, class TWinBase >
HRESULT CSymDHTMLWindow<TBase,TWinBase>::GetElementInterface(LPCTSTR szElementId, REFIID riid, void **ppvObj)
{
    HRESULT hr = E_NOINTERFACE;
    *ppvObj = NULL;
    CComPtr<IDispatch> spdispElem;

    hr = GetElement(szElementId, &spdispElem);

    if (spdispElem)
        hr = spdispElem->QueryInterface(riid, ppvObj);

    return hr;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
template < class TBase, class TWinBase >
BSTR CSymDHTMLWindow<TBase,TWinBase>::GetElementText(LPCTSTR szElementId)
{
    BSTR bstrText = NULL;
    CComPtr<IHTMLElement> sphtmlElem;
    GetElement(szElementId, &sphtmlElem);
    if (sphtmlElem)
        sphtmlElem->get_innerText(&bstrText);
    return bstrText;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
template < class TBase, class TWinBase >
void CSymDHTMLWindow<TBase,TWinBase>::SetElementText(LPCTSTR szElementId, BSTR bstrText)
{
    CComPtr<IHTMLElement> sphtmlElem;
    GetElement(szElementId, &sphtmlElem);

    if (sphtmlElem)
        sphtmlElem->put_innerText(bstrText);
}


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
template < class TBase, class TWinBase >
void CSymDHTMLWindow<TBase,TWinBase>::SetElementText(LPCTSTR szElementId, LPCTSTR szText)
{
    CComPtr<IHTMLElement> sphtmlElem;
    GetElement(szElementId, &sphtmlElem);

    _bstr_t bstrTemp( szText );

    if (sphtmlElem)
        sphtmlElem->put_innerText( bstrTemp );
}


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
template < class TBase, class TWinBase >
void CSymDHTMLWindow<TBase,TWinBase>::SetElementText(IUnknown *punkElem, BSTR bstrText)
{
    CComPtr<IHTMLElement> sphtmlElem;
    punkElem->QueryInterface(__uuidof(IHTMLElement), (void **) &sphtmlElem);

    if( sphtmlElem )
        sphtmlElem->put_innerText(bstrText);
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
template < class TBase, class TWinBase >
BSTR CSymDHTMLWindow<TBase,TWinBase>::GetElementHtml(LPCTSTR szElementId)
{
    BSTR bstrText = NULL;
    CComPtr<IHTMLElement> sphtmlElem;
    GetElement(szElementId, &sphtmlElem);

    if (sphtmlElem)
        sphtmlElem->get_innerHTML(&bstrText);

    return bstrText;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
template < class TBase, class TWinBase >
void CSymDHTMLWindow<TBase,TWinBase>::SetElementHtml(LPCTSTR szElementId, BSTR bstrText)
{
    CComPtr<IHTMLElement> sphtmlElem;
    GetElement(szElementId, &sphtmlElem);

    if (sphtmlElem)
        sphtmlElem->put_innerHTML(bstrText);
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
template < class TBase, class TWinBase >
void CSymDHTMLWindow<TBase,TWinBase>::SetElementHtml(IUnknown *punkElem, BSTR bstrText)
{
    CComPtr<IHTMLElement> sphtmlElem;
    punkElem->QueryInterface(__uuidof(IHTMLElement), (void **) &sphtmlElem);

    if( sphtmlElem )
        sphtmlElem->put_innerHTML(bstrText);
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
template < class TBase, class TWinBase >
VARIANT CSymDHTMLWindow<TBase,TWinBase>::GetElementProperty(LPCTSTR szElementId, DISPID dispid)
{
    VARIANT varRet;
    CComPtr<IDispatch> spdispElem;
    varRet.vt = VT_EMPTY;
    GetElement(szElementId, &spdispElem);

    if( spdispElem )
    {
        DISPPARAMS dispparamsNoArgs = { NULL, NULL, 0, 0 };

        spdispElem->Invoke(dispid, IID_NULL, LOCALE_USER_DEFAULT, 
            DISPATCH_PROPERTYGET, &dispparamsNoArgs, &varRet, NULL, NULL);
    }

    return varRet;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
template < class TBase, class TWinBase >
void CSymDHTMLWindow<TBase,TWinBase>::SetElementProperty(LPCTSTR szElementId, DISPID dispid, VARIANT *pVar)
{
    CComPtr<IDispatch> spdispElem;

    GetElement(szElementId, &spdispElem);

    if (spdispElem)
    {
        DISPPARAMS dispparams = {NULL, NULL, 1, 1};
        dispparams.rgvarg = pVar;
        DISPID dispidPut = DISPID_PROPERTYPUT;
        dispparams.rgdispidNamedArgs = &dispidPut;

        spdispElem->Invoke( dispid, IID_NULL, LOCALE_USER_DEFAULT, DISPATCH_PROPERTYPUT, &dispparams, NULL, NULL, NULL);
    }
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
template < class TBase, class TWinBase >
HRESULT CSymDHTMLWindow<TBase,TWinBase>::GetControlDispatch(LPCTSTR szId, IDispatch **ppdisp)
{
    HRESULT hr = S_OK;
    CComPtr<IDispatch> spdispElem;

    hr = GetElement(szId, &spdispElem);

    if (spdispElem)
    {
        CComPtr<IHTMLObjectElement> sphtmlObj;

        hr = spdispElem.QueryInterface(&sphtmlObj);
        if (sphtmlObj)
        {
            spdispElem.Release();
            hr = sphtmlObj->get_object(ppdisp);
        }
    }

    return hr;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
template < class TBase, class TWinBase >
VARIANT CSymDHTMLWindow<TBase,TWinBase>::GetControlProperty(IDispatch *pdispControl, DISPID dispid)
{
    VARIANT varRet;
    varRet.vt = VT_EMPTY;

    if( pdispControl )
    {
        DISPPARAMS dispparamsNoArgs = { NULL, NULL, 0, 0 };
        pdispControl->Invoke( dispid, IID_NULL, LOCALE_USER_DEFAULT, 
            DISPATCH_PROPERTYGET, &dispparamsNoArgs, &varRet, NULL, NULL);
    }

    return varRet;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
template < class TBase, class TWinBase >
VARIANT CSymDHTMLWindow<TBase,TWinBase>::GetControlProperty(LPCTSTR szId, DISPID dispid)
{
    CComPtr<IDispatch> spdispElem;

    GetControlDispatch(szId, &spdispElem);
    return GetControlProperty(spdispElem, dispid);
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
template < class TBase, class TWinBase >
VARIANT CSymDHTMLWindow<TBase,TWinBase>::GetControlProperty(LPCTSTR szId, LPCTSTR szPropName)
{
    CComVariant varEmpty;
    CComPtr<IDispatch> spdispElem;

    GetControlDispatch(szId, &spdispElem);
    if (!spdispElem)
        return varEmpty;

    DISPID dispid;
    USES_CONVERSION;
    LPOLESTR pPropName = (LPOLESTR)T2COLE(szPropName);
    HRESULT hr = spdispElem->GetIDsOfNames(IID_NULL, &pPropName, 1, LOCALE_USER_DEFAULT, &dispid);

    if (SUCCEEDED(hr))
        return GetControlProperty(spdispElem, dispid);

    return varEmpty;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
template < class TBase, class TWinBase >
void CSymDHTMLWindow<TBase,TWinBase>::SetControlProperty(IDispatch *pdispControl, DISPID dispid, VARIANT *pVar)
{
    if (pdispControl != NULL)
    {
        DISPPARAMS dispparams = {NULL, NULL, 1, 1};
        dispparams.rgvarg = pVar;
        DISPID dispidPut = DISPID_PROPERTYPUT;
        dispparams.rgdispidNamedArgs = &dispidPut;

        pdispControl->Invoke(dispid, IID_NULL,
            LOCALE_USER_DEFAULT, DISPATCH_PROPERTYPUT,
            &dispparams, NULL, NULL, NULL);
    }
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
template < class TBase, class TWinBase >
void CSymDHTMLWindow<TBase,TWinBase>::SetControlProperty(LPCTSTR szElementId, DISPID dispid, VARIANT *pVar)
{
    CComPtr<IDispatch> spdispElem;
    GetControlDispatch(szElementId, &spdispElem);

    SetControlProperty(spdispElem, dispid, pVar);
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
template < class TBase, class TWinBase >
void CSymDHTMLWindow<TBase,TWinBase>::SetControlProperty(LPCTSTR szElementId, LPCTSTR szPropName, VARIANT *pVar)
{
    CComPtr<IDispatch> spdispElem;
    GetControlDispatch(szElementId, &spdispElem);

    if (!spdispElem)
        return;

    DISPID dispid;
    USES_CONVERSION;
    LPOLESTR pPropName = (LPOLESTR)T2COLE(szPropName);
    HRESULT hr = spdispElem->GetIDsOfNames(IID_NULL, &pPropName, 1, LOCALE_USER_DEFAULT, &dispid);

    if (SUCCEEDED(hr))
        SetControlProperty(spdispElem, dispid, pVar);
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
template < class TBase, class TWinBase >
HRESULT CSymDHTMLWindow<TBase,TWinBase>::GetEvent(IHTMLEventObj **ppEventObj)
{

    CComPtr<IHTMLWindow2> sphtmlWnd;
    CComPtr<IHTMLDocument2> sphtmlDoc;

    *ppEventObj = NULL;

    HRESULT hr = GetDHtmlDocument(&sphtmlDoc);
    if (sphtmlDoc == NULL)
        return hr;

    hr = sphtmlDoc->get_parentWindow(&sphtmlWnd);

    if (FAILED(hr))
        goto Error;

    hr = sphtmlWnd->get_event(ppEventObj);

Error:
    return hr;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
template < class TBase, class TWinBase >
void CSymDHTMLWindow<TBase,TWinBase>::SetHostFlags(DWORD dwFlags)
{
    m_dwHostFlags = dwFlags;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
template < class TBase, class TWinBase >
void CSymDHTMLWindow<TBase,TWinBase>::SetExternalDispatch(IDispatch *pdispExternal)
{
    m_spExternalDisp = pdispExternal;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
template < class TBase, class TWinBase >
BOOL CSymDHTMLWindow<TBase,TWinBase>::IsExternalDispatchSafe()
{
    return FALSE;
}


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
template < class TBase, class TWinBase >
STDMETHODIMP CSymDHTMLWindow<TBase,TWinBase>::ShowContextMenu(DWORD /*dwID*/, POINT * /*ppt*/, 
                                                              IUnknown * /*pcmdtReserved*/, IDispatch * /*pdispReserved*/)
{
    return S_FALSE;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
template < class TBase, class TWinBase >
STDMETHODIMP CSymDHTMLWindow<TBase,TWinBase>::GetHostInfo(DOCHOSTUIINFO *pInfo)
{
    pInfo->dwFlags = m_dwHostFlags;
    return S_OK;
}


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
template < class TBase, class TWinBase >
STDMETHODIMP CSymDHTMLWindow<TBase,TWinBase>::ShowUI(DWORD /*dwID*/, IOleInPlaceActiveObject * /*pActiveObject*/, IOleCommandTarget * /*pCommandTarget*/, 
                                                     IOleInPlaceFrame * /*pFrame*/, IOleInPlaceUIWindow * /*pDoc*/)
{
    return S_FALSE;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
template < class TBase, class TWinBase >
STDMETHODIMP CSymDHTMLWindow<TBase,TWinBase>::HideUI(void)
{
    return E_NOTIMPL;
}


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
template < class TBase, class TWinBase >
STDMETHODIMP CSymDHTMLWindow<TBase,TWinBase>::UpdateUI(void)
{
    return E_NOTIMPL;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
template < class TBase, class TWinBase >
STDMETHODIMP CSymDHTMLWindow<TBase,TWinBase>::EnableModeless(BOOL /*fEnable*/)
{
    return E_NOTIMPL;
}


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
template < class TBase, class TWinBase >
STDMETHODIMP CSymDHTMLWindow<TBase,TWinBase>::OnDocWindowActivate(BOOL /*fActivate*/)
{
    return E_NOTIMPL;
}


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
template < class TBase, class TWinBase >
STDMETHODIMP CSymDHTMLWindow<TBase,TWinBase>::OnFrameWindowActivate(BOOL /*fActivate*/)
{
    return E_NOTIMPL;
}


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
template < class TBase, class TWinBase >
STDMETHODIMP CSymDHTMLWindow<TBase,TWinBase>::ResizeBorder(LPCRECT /*prcBorder*/, IOleInPlaceUIWindow * /*pUIWindow*/, BOOL /*fRameWindow*/)
{
    return E_NOTIMPL;
}


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
template < class TBase, class TWinBase >
STDMETHODIMP CSymDHTMLWindow<TBase,TWinBase>::TranslateAccelerator(LPMSG /*lpMsg*/, const GUID * /*pguidCmdGroup*/, DWORD /*nCmdID*/)
{
    return S_FALSE;
}


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
template < class TBase, class TWinBase >
STDMETHODIMP CSymDHTMLWindow<TBase,TWinBase>::GetOptionKeyPath(LPOLESTR * /*pchKey*/, DWORD /*dw*/)
{
    return E_NOTIMPL;
}


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
template < class TBase, class TWinBase >
STDMETHODIMP CSymDHTMLWindow<TBase,TWinBase>::GetDropTarget(IDropTarget * /*pDropTarget*/, IDropTarget **ppDropTarget)
{
    *ppDropTarget = NULL;
    return E_NOTIMPL;
}


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
template < class TBase, class TWinBase >
STDMETHODIMP CSymDHTMLWindow<TBase,TWinBase>::GetExternal(IDispatch **ppDispatch)
{
    if(ppDispatch == NULL)
        return E_POINTER;

    *ppDispatch = NULL;
    if (m_spExternalDisp.p && CanAccessExternal())
    {
        m_spExternalDisp.p->AddRef();
        *ppDispatch = m_spExternalDisp.p;
        return S_OK;
    }
    return E_NOTIMPL;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
template < class TBase, class TWinBase >
STDMETHODIMP CSymDHTMLWindow<TBase,TWinBase>::TranslateUrl(DWORD /*dwTranslate*/, OLECHAR * /*pchURLIn*/, OLECHAR **ppchURLOut)
{
    if(ppchURLOut == NULL)
        return E_POINTER;

    *ppchURLOut = NULL;
    return S_FALSE;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
template < class TBase, class TWinBase >
STDMETHODIMP CSymDHTMLWindow<TBase,TWinBase>::FilterDataObject(IDataObject * /*pDO*/, IDataObject **ppDORet)
{
    if(ppDORet == NULL)
        return E_POINTER;

    *ppDORet = NULL;
    return S_FALSE;
}


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
template < class TBase, class TWinBase >
HRESULT CSymDHTMLWindow<TBase,TWinBase>::DisplayElement( LPCTSTR szElementName, BOOL bShow )
{
    HRESULT hr = 0;
    CComPtr< IHTMLElement > spElement;
    hr = GetElementInterface( szElementName, IID_IHTMLElement, (void**)&spElement );

    if( SUCCEEDED(hr) )
    {
        CComPtr< IHTMLStyle > spStyle;
        hr = spElement->get_style( &spStyle );

        if( SUCCEEDED(hr) )
        {
            CComBSTR bstrDisplay( bShow ? "inline" : "none" );
            hr = spStyle->put_display( bstrDisplay );
        }
    }

    return hr;
}


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
template < class TBase, class TWinBase >
HRESULT CSymDHTMLWindow<TBase,TWinBase>::SelectAddString( LPCTSTR szId, LPCTSTR szText, LPCTSTR szValue, BOOL bSelected )
{
    HRESULT hr = 0;

    CComPtr< IHTMLSelectElement > spSelect;
    hr = GetElementInterface( szId, &spSelect);

    if( SUCCEEDED(hr) )
        hr = SelectAddString( spSelect.p, szText, szValue, bSelected );

    return hr;
}


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
template < class TBase, class TWinBase >
HRESULT CSymDHTMLWindow<TBase,TWinBase>::SelectAddString( IHTMLSelectElement* pSelect, LPCTSTR szText, LPCTSTR szValue, BOOL bSelected )
{
    HRESULT hr = 0;

    CComPtr< IHTMLElement > spNewElement;
    hr = m_spHTMLDocument2->createElement( CComBSTR(_T("OPTION")), &spNewElement );

    if( SUCCEEDED(hr) )
    {
        CComQIPtr< IHTMLOptionElement > spOption( spNewElement );

        if( spOption )
        {
            hr = spOption->put_text( CComBSTR(szText) );

            if( SUCCEEDED(hr) )
            {
                CComVariant varBefore = FALSE;
                hr = pSelect->add( spNewElement, varBefore );

                if( SUCCEEDED(hr) )
                {
                    hr = spOption->put_value( CComBSTR(szValue) );
                    ATLASSERT( SUCCEEDED(hr) );

                    if( bSelected )
                    {
                        hr = spOption->put_selected( VARIANT_TRUE );
                        ATLASSERT( SUCCEEDED(hr) );
                    }
                }
            }
        }
    }

    return hr;
}


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
template < class TBase, class TWinBase >
HRESULT CSymDHTMLWindow<TBase,TWinBase>::SelectGetText( LPCTSTR szId, BSTR* bstrText, long lIndex /*= -1*/ )
{
    CComPtr< IHTMLSelectElement > spSelect;

    HRESULT hr = S_OK;
    hr = GetElementInterface( szId, &spSelect );

    if( SUCCEEDED(hr) )
        hr = SelectGetText( spSelect.p, bstrText, lIndex );

    return hr;
}


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
template < class TBase, class TWinBase >
HRESULT CSymDHTMLWindow<TBase,TWinBase>::SelectGetText( IHTMLSelectElement* pSelect, BSTR* bstrText, long lIndex /*= -1*/ )
{
    HRESULT hr = S_OK;
    long lCurSel = lIndex;

    if( -1 == lIndex )
        hr = pSelect->get_selectedIndex( &lCurSel );

    if( SUCCEEDED(hr) )
    {
        CComPtr< IDispatch > spDisp;

        CComVariant index = lCurSel;
        hr = pSelect->item( index, index, &spDisp );
        ATLASSERT( SUCCEEDED(hr) );

        if( SUCCEEDED(hr) )
        {
            CComQIPtr< IHTMLOptionElement > spOption( spDisp );
            ATLASSERT( spOption );

            if( spOption )
                hr = spOption->get_text( bstrText);
        }            
    }

    return hr;
}


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
template < class TBase, class TWinBase >
HRESULT CSymDHTMLWindow<TBase,TWinBase>::SelectGetValue( LPCTSTR szId, BSTR* bstrValue, long lIndex /*= -1*/ )
{
    CComPtr< IHTMLSelectElement > spSelect;

    HRESULT hr = 0;
    hr = GetElementInterface( szId, &spSelect );

    if( SUCCEEDED(hr) )
        hr = SelectGetValue( spSelect.p, bstrValue, lIndex );

    return hr;
}


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
template < class TBase, class TWinBase >
HRESULT CSymDHTMLWindow<TBase,TWinBase>::SelectGetValue( IHTMLSelectElement* pSelect, BSTR* bstrValue, long lIndex /*= -1*/ )
{
    HRESULT hr = S_OK;
    long lCurSel = lIndex;

    if( -1 == lIndex )
        hr = pSelect->get_selectedIndex( &lCurSel );

    if( SUCCEEDED(hr) )
    {
        CComPtr< IDispatch > spDisp;

        CComVariant index = lCurSel;
        hr = pSelect->item( index, index, &spDisp );
        ATLASSERT( SUCCEEDED(hr) );

        if( SUCCEEDED(hr) )
        {
            CComQIPtr< IHTMLOptionElement > spOption( spDisp );
            ATLASSERT( spOption );

            if( spOption )
                hr = spOption->get_value( bstrValue );
        }            
    }

    return hr;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
template < class TBase, class TWinBase >
HRESULT CSymDHTMLWindow<TBase,TWinBase>::GetMouseWheelDelta( long* pDelta )
{
    HRESULT hr = S_OK;

    CComPtr< IHTMLEventObj > spEvent;
    hr = m_spHTMLWindow2->get_event( &spEvent );

    if( SUCCEEDED(hr) )
    {
        CComPtr< IHTMLEventObj4 > spEvent4;
        hr = spEvent->QueryInterface( __uuidof(IHTMLEventObj4), (void**)&spEvent4 );

        if( SUCCEEDED(hr) )
            hr = spEvent4->get_wheelDelta( pDelta );
    }

    return hr;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
template < class TBase, class TWinBase >
HRESULT CSymDHTMLWindow<TBase,TWinBase>::SetActiveElement( LPCTSTR szID )
{
    CComPtr< IHTMLElement3 > spElement;

    HRESULT hr = S_OK;
    hr = GetElementInterface( szID, &spElement );

    if( SUCCEEDED(hr) )
        hr = spElement->setActive();

    return hr;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
template < class TBase, class TWinBase >
HRESULT CSymDHTMLWindow<TBase,TWinBase>::GetActiveElement( IHTMLElement** pElement )
{
    HRESULT hr = S_OK;
    hr = m_spHTMLDocument2->get_activeElement( pElement );

    return hr;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
template < class TBase, class TWinBase >
HRESULT CSymDHTMLWindow<TBase,TWinBase>::IsElementDisplayed( LPCTSTR szElementName, BOOL& bVisible )
{
    HRESULT hr = 0;
    CComPtr< IHTMLElement > spElement;
    hr = GetElementInterface( szElementName, IID_IHTMLElement, (void**)&spElement );

    if( SUCCEEDED(hr) )
    {
        CComPtr< IHTMLStyle > spStyle;
        hr = spElement->get_style( &spStyle );

        if( SUCCEEDED(hr) )
        {
            CComBSTR bstrDisplay;
            hr = spStyle->get_display( &bstrDisplay );

            if( SUCCEEDED(hr) && bstrDisplay )
            {
                bVisible = !(0 == wcscmp( bstrDisplay, L"none" ));
            }
            else
            {
                bVisible = TRUE;
            }
        }
    }

    return hr;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
template < class TBase, class TWinBase >
HRESULT CSymDHTMLWindow<TBase,TWinBase>::SetImgSrc( LPCTSTR szId, LPCTSTR szSrc )
{
    HRESULT hr = 0;

    CComPtr< IHTMLImgElement > spImgElement;
    hr = GetElementInterface( szId, __uuidof(IHTMLImgElement), (void**)&spImgElement );

    if( SUCCEEDED(hr) )
    {
		g_ResLoader.Initialize();
        HINSTANCE hInst = g_ResLoader.GetResourceInstance();

        TCHAR szPath[MAX_PATH];
        GetModuleFileName( hInst, szPath, MAX_PATH );

        CString strPanelURL;

        if( m_strResourceDLL.IsEmpty() )
        {
            strPanelURL.Format( _T("res://%s//%s"), szPath, szSrc );
        }
        else
        {
            PathRemoveFileSpec( szPath );
            strPanelURL.Format( _T("res://%s\\%s/%s"), szPath, m_strResourceDLL, szSrc );
        }

        hr = spImgElement->put_src( CComBSTR(strPanelURL) );
    }

    return hr;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
template < class TBase, class TWinBase >
HRESULT CSymDHTMLWindow<TBase,TWinBase>::SetElementClassName( LPCTSTR szId, LPCTSTR szClass )
{
    HRESULT hr = 0;

    CComPtr< IHTMLElement > spElement;
    hr = GetElement( szId, &spElement );
    ATLASSERT( SUCCEEDED(hr) );

    if( SUCCEEDED(hr) )
    {
        hr = spElement->put_className( CComBSTR(szClass) );
        ATLASSERT( SUCCEEDED(hr) );
    }

    return hr;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
template < class TBase, class TWinBase >
HRESULT CSymDHTMLWindow<TBase,TWinBase>::GetElementClassName( LPCTSTR szId, BSTR* bstrClassName )
{
    HRESULT hr = 0;

    CComPtr< IHTMLElement > spElement;
    hr = GetElement( szId, &spElement );
    ATLASSERT( SUCCEEDED(hr) );

    if( SUCCEEDED(hr) )
    {
        hr = spElement->get_className( bstrClassName );
        ATLASSERT( SUCCEEDED(hr) );
    }

    return hr;
}


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
template < class TBase, class TWinBase >
HRESULT CSymDHTMLWindow<TBase,TWinBase>::CreateCCWebWindow()
{
    HRESULT hr = S_OK;
    hr = m_spCCWebWindow.CoCreateInstance( CLSID_ccWebWindow );

    if( SUCCEEDED(hr) )
    {
		g_ResLoader.Initialize();
        HINSTANCE hInstResource = g_ResLoader.GetResourceInstance();

        // Format our initial URL.
        TCHAR szResourcePath[MAX_PATH];
        GetModuleFileName( hInstResource, szResourcePath, _countof(szResourcePath) );

        CString strURL;
        strURL.Format( _T("res://%s/%s"), szResourcePath, m_szHtmlResID );

        _bstr_t bstrURIorHTML = strURL;


        const DWORD dwStyle = WS_CHILD;
        hr = m_spCCWebWindow->CreateChildControl( (long)m_hWnd, 102, bstrURIorHTML, dwStyle, NULL, FALSE, (long*)& m_wndCCWeb.m_hWnd );

        if( SUCCEEDED(hr) )
        {
            EnumChildWindows( m_hWnd, FindInternetExplorerWindow, (LPARAM)&m_wndBrowser.m_hWnd );

            if( ::IsWindow(m_wndBrowser) )
            {
                DWORD_PTR dwpRes = NULL;
                LRESULT lRes =  ::SendMessageTimeout( m_wndBrowser, WM_HTML_GETOBJECT, NULL, NULL, SMTO_ABORTIFHUNG, 1000, &dwpRes );

                if( dwpRes )
                {
                    CComPtr<IUnknown> spUnk;
                    hr = ObjectFromLresult( dwpRes, __uuidof(IUnknown), NULL, (void**)&spUnk );

                    if( S_OK == hr )
                    {
                        hr = spUnk.QueryInterface( &m_spHTMLServiceProvider );

                        if( SUCCEEDED(hr) )
                        {
                            hr = m_spHTMLServiceProvider->QueryService( IID_IWebBrowserApp, IID_IWebBrowser2, (void**)&m_pBrowserApp );

                            if( SUCCEEDED(hr) )
                            {
                                hr = m_spHTMLServiceProvider->QueryService( IID_IHTMLWindow2, IID_IHTMLWindow2, (void**)&m_spHTMLWindow2 );
                                ATLASSERT( SUCCEEDED(hr) );

                                CComPtr< IDispatch > spDispath;
                                hr = m_pBrowserApp->get_Document( &spDispath );

                                if( SUCCEEDED(hr) && spDispath )
                                {
                                    spDispath->QueryInterface( __uuidof(IHTMLDocument2), (void**)&m_spHTMLDocument2 );
                                    hr = ConnectDHtmlEvents( spUnk );
                                    ATLASSERT( SUCCEEDED(hr) );

                                    if( SUCCEEDED(hr) )
                                    {
                                        if( SUCCEEDED(hr) )
                                        {
                                            // Connect the Web browser events....
                                            hr = CSymDHtmlBrowserEventSink::Advise( m_pBrowserApp );
                                            ATLASSERT( SUCCEEDED(hr) );

                                            bstrURIorHTML = strURL;
                                            Navigate( bstrURIorHTML );
                                        }
                                    }
                                }
                            }
                        }
                    }

                    // Temp hack here to get the ccWebWindow to tab through it's controls...
                    m_wndBrowser.SendMessage( WM_MOUSEACTIVATE, 0,0 );
                    m_wndCCWeb.ShowWindow( SW_NORMAL );
                }
            }
        }
    }

	// Get product name and SARC threat info URL from AVRES.DLL
	if(m_strProductName.IsEmpty())
	{
		m_strProductName = CISVersion::GetProductName();
		m_strSarcThreatInfoUrl = BrandRes.GetString(IDS_SARC_THREAT_INFO_URL);
	}

    return hr;   
}



//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
template < class TBase, class TWinBase >
LRESULT CSymDHTMLWindow<TBase,TWinBase>::OnCreate(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled)
{
    HRESULT hr = 0;
    hr = CreateCCWebWindow();
    ATLASSERT( SUCCEEDED(hr) );

    if( FAILED(hr) )
    {
        //CCTRACEE(_T("Unable to create ccWebWindow") );
        return -1;
    }

    CWindow wndParent;
    wndParent = GetParent();

    CRect rectClient;
    wndParent.GetClientRect( rectClient );
    SetWindowPos( 0, rectClient, SWP_NOMOVE|SWP_NOZORDER );

    // Wait for the document to complete before continuing
    StahlSoft::WaitForSingleObjectWithMessageLoop(m_sDocCompleteEvent, INFINITE);

    CMessageLoop* pLoop = _Module.GetMessageLoop();
    ATLASSERT(pLoop != NULL);
    pLoop->AddMessageFilter(this);

    bHandled = FALSE;
    return SUCCEEDED(hr);   
}


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
template < class TBase, class TWinBase >
HRESULT CSymDHTMLWindow<TBase,TWinBase>::OnInitDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled)
{
    HRESULT hr = 0;
    hr = CreateCCWebWindow();
    ATLASSERT( SUCCEEDED(hr) );

    OnInitDialog();

    bHandled = FALSE;
    return 0;   
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
template < class TBase, class TWinBase >
LRESULT CSymDHTMLWindow<TBase,TWinBase>::OnForwardMessage(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
    HWND hWndCtl = ::GetFocus();

    if( IsChild (hWndCtl) )
    {
        // find a direct child of the dialog from the window that has focus
        while(::GetParent(hWndCtl) != m_hWnd)
            hWndCtl = ::GetParent(hWndCtl);

        // give control a chance to translate this message
        if(::SendMessage(hWndCtl, WM_FORWARDMSG, 0, lParam) != 0)
            return TRUE;
    }

    return m_wndCCWeb.SendMessage( WM_FORWARDMSG, wParam, lParam );   ;
}


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
template < class TBase, class TWinBase >
LRESULT CSymDHTMLWindow<TBase,TWinBase>::OnSetFocus(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
    m_wndBrowser.SetFocus();
    return 1;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
template < class TBase, class TWinBase >
LRESULT CSymDHTMLWindow<TBase,TWinBase>::OnSize(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM lParam, BOOL& bHandled)
{
    if( ::IsWindow(m_wndCCWeb) )
    {
        CRect rectClient;
        GetClientRect( rectClient );
        m_wndCCWeb.SetWindowPos( 0, rectClient, SWP_NOZORDER|SWP_NOMOVE );
    }

    bHandled = FALSE;
    return 0;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
template < class TBase, class TWinBase >
BOOL CSymDHTMLWindow<TBase,TWinBase>::PreTranslateMessage(MSG* pMsg)
{
    if((pMsg->message < WM_KEYFIRST || pMsg->message > WM_KEYLAST) &&
        (pMsg->message < WM_MOUSEFIRST || pMsg->message > WM_MOUSELAST))
        return FALSE;

    HWND hWndCtl = ::GetFocus();

    if( IsChild(hWndCtl) )
    {
        // find a direct child of the dialog from the window that has focus
        while(::GetParent(hWndCtl) != m_hWnd)
            hWndCtl = ::GetParent(hWndCtl);

        // give control a chance to translate this message
        if(::SendMessage(hWndCtl, WM_FORWARDMSG, 0, (LPARAM)pMsg) != 0)
            return TRUE;
    }

    return FALSE;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
template < class TBase, class TWinBase >
HRESULT CSymDHTMLWindow<TBase,TWinBase>::SetElementWidth(LPCTSTR szElementId, LPCTSTR szWidth )
{
    _bstr_t bstrWidth( szWidth );
    return SetElementWidth( szElementId, (BSTR)bstrWidth );
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
template < class TBase, class TWinBase >
HRESULT CSymDHTMLWindow<TBase,TWinBase>::SetElementWidth(LPCTSTR szElementId, BSTR bstrWidth )
{
    CComPtr< IHTMLElement > spElement;

    HRESULT hr = S_OK;
    hr = GetElementInterface( szElementId, &spElement );

    if( SUCCEEDED(hr) )
    {
        CComPtr< IHTMLStyle > spStyle;
        hr = spElement->get_style( &spStyle );

        if( SUCCEEDED(hr)  )
        {
            _variant_t var = bstrWidth;
            hr = spStyle->put_width( var );
        }
    }

    return hr;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
template < class TBase, class TWinBase >
HRESULT CSymDHTMLWindow<TBase,TWinBase>::SetElementHeight(LPCTSTR szElementId, LPCTSTR szHeight )
{
    _bstr_t bstrHeight( szHeight );
    return SetElementHeight( szElementId, bstrHeight );
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
template < class TBase, class TWinBase >
HRESULT CSymDHTMLWindow<TBase,TWinBase>::SetElementHeight(LPCTSTR szElementId, BSTR bstrHeight )
{
    CComPtr< IHTMLElement > spElement;

    HRESULT hr = S_OK;
    hr = GetElementInterface( szElementId, &spElement );

    if( SUCCEEDED(hr) )
    {
        CComPtr< IHTMLStyle > spStyle;
        hr = spElement->get_style( &spStyle );

        if( SUCCEEDED(hr)  )
        {
            _variant_t var = bstrHeight;
            hr = spStyle->put_Height( var );
        }
    }

    return hr;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
template < class TBase, class TWinBase >
HRESULT CSymDHTMLWindow<TBase,TWinBase>::SetElementButtonEnabled( LPCTSTR szElementId, BOOL bEnabled /*= TRUE*/ )
{
    CComPtr< IHTMLButtonElement > spElement;

    HRESULT hr = S_OK;
    hr = GetElementInterface( szElementId, &spElement );

    if( SUCCEEDED(hr) )
    {
        hr = spElement->put_disabled( bEnabled ? VARIANT_FALSE : VARIANT_TRUE );
        ATLASSERT( SUCCEEDED(hr) );
    }

    return hr;
}


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
template < class TBase, class TWinBase >
HRESULT CSymDHTMLWindow<TBase,TWinBase>::GetElementOffsetWidth( LPCTSTR szElementId, long& lWidth )
{
    CComPtr< IHTMLElement > spElement;

    HRESULT hr = S_OK;
    hr = GetElementInterface( szElementId, &spElement );

    if( SUCCEEDED(hr) )
    {
        hr = spElement->get_offsetWidth( &lWidth );
        ATLASSERT( SUCCEEDED(hr) );
    }

    return hr;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
template < class TBase, class TWinBase >
HRESULT CSymDHTMLWindow<TBase,TWinBase>::GetElementOffsetHeight( LPCTSTR szElementId, long& lHeight )
{
    CComPtr< IHTMLElement > spElement;

    HRESULT hr = S_OK;
    hr = GetElementInterface( szElementId, &spElement );

    if( SUCCEEDED(hr) )
    {
        hr = spElement->get_offsetHeight( &lHeight );
        ATLASSERT( SUCCEEDED(hr) );
    }

    return hr;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
template < class TBase, class TWinBase >
HRESULT CSymDHTMLWindow<TBase,TWinBase>::GetElementOffsetLeft( LPCTSTR szElementId, long& lLeft )
{
    CComPtr< IHTMLElement > spElement;

    HRESULT hr = S_OK;
    hr = GetElementInterface( szElementId, &spElement );

    if( SUCCEEDED(hr) )
    {
        hr = spElement->get_offsetLeft( &lLeft );
        ATLASSERT( SUCCEEDED(hr) );
    }

    return hr;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
template < class TBase, class TWinBase >
HRESULT CSymDHTMLWindow<TBase,TWinBase>::GetElementOffsetTop( LPCTSTR szElementId, long& lTop )
{
    CComPtr< IHTMLElement > spElement;

    HRESULT hr = S_OK;
    hr = GetElementInterface( szElementId, &spElement );

    if( SUCCEEDED(hr) )
    {
        hr = spElement->get_offsetHeight( &lTop );
        ATLASSERT( SUCCEEDED(hr) );
    }

    return hr;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
template < class TBase, class TWinBase >
HRESULT CSymDHTMLWindow<TBase,TWinBase>::GetElementRect( LPCTSTR szElementId, RECT& rc )
{
    CComPtr< IHTMLElement > spElement;

    HRESULT hr = S_OK;
    hr = GetElementInterface( szElementId, &spElement );

    if( SUCCEEDED(hr) )
    {
        hr = spElement->get_offsetLeft( &rc.left );
        ATLASSERT( SUCCEEDED(hr) );

        hr = spElement->get_offsetTop( &rc.top );
        ATLASSERT( SUCCEEDED(hr) );

        long lWidth = 0;
        hr = spElement->get_offsetWidth( &lWidth );
        ATLASSERT( SUCCEEDED(hr) );

        rc.right = rc.left+lWidth;

        long lHeight = 0;
        hr = spElement->get_offsetHeight( &lHeight );
        ATLASSERT( SUCCEEDED(hr) );

        rc.bottom = rc.top+lHeight;
    }

    return hr;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
template < class TBase, class TWinBase >
HRESULT CSymDHTMLWindow<TBase,TWinBase>::GetStyleSheetRule( LPCTSTR szGetStyleName, IHTMLRuleStyle** lpStyle )
{
    HRESULT hr = E_NOINTERFACE;

    CComPtr< IHTMLStyleSheetsCollection > spStyleSheetsCollection;
    hr = m_spHTMLDocument2->get_styleSheets( &spStyleSheetsCollection );

    if( SUCCEEDED(hr) )
    {
        long lLength = 0;
        hr = spStyleSheetsCollection->get_length( &lLength );

        if( SUCCEEDED(hr) && lLength )
        {
            bool bFoundStyle = false;

            for( long lStyleSheetsCollection = 0; lStyleSheetsCollection < lLength && !bFoundStyle; lStyleSheetsCollection++ )
            {
                _variant_t varIndex = lStyleSheetsCollection;
                _variant_t varSheet;

                hr = spStyleSheetsCollection->item( &varIndex, &varSheet );

                if( SUCCEEDED(hr) )
                {
                    CComQIPtr< IHTMLStyleSheet > spStyleSheet( varSheet );

                    if( spStyleSheet )
                    {
                        CComPtr< IHTMLStyleSheetRulesCollection > spRulesCollection;
                        hr = spStyleSheet->get_rules( &spRulesCollection );

                        if( SUCCEEDED(hr) )
                        {
                            long lLength = 0;
                            hr = spRulesCollection->get_length( &lLength );

                            if( SUCCEEDED(hr) )
                            {
                                for( long x = 0; x < lLength; x++ )
                                {
                                    CComPtr< IHTMLStyleSheetRule > spStyleSheetRule;
                                    hr = spRulesCollection->item( x, &spStyleSheetRule );

                                    if( SUCCEEDED(hr) )
                                    {
                                        _bstr_t bstrSelectorText;
                                        hr = spStyleSheetRule->get_selectorText( &bstrSelectorText.GetBSTR() );

                                        if( SUCCEEDED(hr) )
                                        {
                                            if( 0 == _tcsicmp(szGetStyleName, bstrSelectorText) )
                                            {
                                                hr = spStyleSheetRule->get_style( lpStyle );
                                                bFoundStyle = true;
                                                break;
                                            }
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }// END_FOR
        }
        else
        {
            hr = E_FAIL;
        }
    }

    return hr;
}


