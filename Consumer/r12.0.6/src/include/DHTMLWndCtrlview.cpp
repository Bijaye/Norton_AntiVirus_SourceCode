//*****************************************************************************
//*****************************************************************************
#include "stdafx.h"
#include "resource.h"
#include "ccWebWnd.h"
#include <ExDispid.h>

#include "DHTMLWndCtrlView.h"


//~|~|~|~|~|~|~|~|~|~|~|~|~|~|~|~|~|~|~|~|~|~|~|~|~|~|~|~|~|~|~|~|~|~|~|~|~|~|~
// CSymDHtmlControlSink

const DWORD WM_HTML_GETOBJECT = ::RegisterWindowMessage( _T("WM_HTML_GETOBJECT") );


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
CSymDHtmlControlSink::CSymDHtmlControlSink()
{
    m_dwCookie = 0;
    m_pHandler = NULL;
    m_dwThunkOffset = 0;
    memset(&m_iid, 0x00, sizeof(IID));
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
CSymDHtmlControlSink::CSymDHtmlControlSink(IUnknown *punkObj, CSymDHtmlSinkHandler *pHandler,LPCTSTR szControlId, DWORD_PTR dwThunkOffset /*= 0*/)
{
    m_dwCookie = 0;
    m_pHandler = pHandler;
    m_szControlId = szControlId;
    m_dwThunkOffset = dwThunkOffset;

    HRESULT hr = S_OK;
    hr = ConnectToControl(punkObj);
    ATLASSERT( SUCCEEDED(hr) );
}

//------------------    -----------------------------------------------------------
//-----------------------------------------------------------------------------
CSymDHtmlControlSink::~CSymDHtmlControlSink()
{
    if( m_dwCookie )
    {
        AtlUnadvise(m_spunkObj, m_iid, m_dwCookie);
    }
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
HRESULT CSymDHtmlControlSink::ConnectToControl(IUnknown *punkObj)
{ 
    m_spunkObj = punkObj;
    HRESULT hr = AtlGetObjectSourceInterface(punkObj, &m_libid, &m_iid, &m_wMajor, &m_wMinor);
    if (FAILED(hr))
        return hr;

    CComPtr<ITypeLib> spTypeLib;

    hr = LoadRegTypeLib(m_libid, m_wMajor, m_wMinor, LOCALE_USER_DEFAULT, &spTypeLib);
    if (FAILED(hr))
        return hr;

    hr = spTypeLib->GetTypeInfoOfGuid(m_iid, &m_spTypeInfo);
    if (FAILED(hr))
        return hr;

    return AtlAdvise(punkObj, this, m_iid, &m_dwCookie);
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
STDMETHODIMP_(ULONG) CSymDHtmlControlSink::AddRef()
{
    return 1;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
STDMETHODIMP_(ULONG) CSymDHtmlControlSink::Release()
{
    return 1;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
STDMETHODIMP CSymDHtmlControlSink::QueryInterface(REFIID iid, LPVOID* ppvObj)
{
    if (!ppvObj)
        return E_POINTER;

    *ppvObj = NULL;

    if (IsEqualIID(iid, __uuidof(IUnknown)) || 
        IsEqualIID(iid, __uuidof(IDispatch)) || 
        IsEqualIID(iid, m_iid))
    {
        *ppvObj = this;
        return S_OK;
    }
    return E_NOINTERFACE;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
STDMETHODIMP CSymDHtmlControlSink::GetTypeInfoCount(UINT *pctinfo)
{
    *pctinfo = 0;
    ATLTRACENOTIMPL(_T("CSymDHtmlControlSink::GetTypeInfoCount"));
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
STDMETHODIMP CSymDHtmlControlSink::GetTypeInfo(UINT /*iTInfo*/, LCID /*lcid*/, ITypeInfo **ppTInfo)
{
    *ppTInfo = NULL;
    ATLTRACENOTIMPL(_T("CSymDHtmlControlSink::GetTypeInfo"));
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
STDMETHODIMP CSymDHtmlControlSink::GetIDsOfNames(REFIID /*riid*/, OLECHAR** /*rgszNames*/, UINT /*cNames*/, LCID /*lcid*/, DISPID* /*rgDispId*/)
{
    ATLTRACENOTIMPL(_T("CSymDHtmlControlSink::GetIDsOfNames"));
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
STDMETHODIMP CSymDHtmlControlSink::Invoke(DISPID dispidMember, REFIID /*riid*/, LCID lcid, WORD /*wFlags*/, DISPPARAMS *pdispparams, VARIANT *pvarResult, EXCEPINFO* /*pExcepInfo*/, UINT * /*puArgErr*/)
{
    _ATL_FUNC_INFO info;

    BOOL fFound = FALSE;
    SYMDHEVTFUNCCONTROL pEvent = NULL;

    //if( -606 != dispidMember )
    ATLTRACE( "dispidMember %d\n", dispidMember );

    const SymDHtmlEventMapEntry *pEventMap = m_pHandler->GetDHtmlEventMap();
    for (int i=0; pEventMap[i].nType != SYM_DHTMLEVENTMAPENTRY_END; i++)
    {
        if (pEventMap[i].nType==SYM_DHTMLEVENTMAPENTRY_CONTROL &&
            pEventMap[i].dispId == dispidMember &&
            !_tcscmp(pEventMap[i].szName, m_szControlId))
        {
            // found the entry
            pEvent = pEventMap[i].pfnEventFunc;
            fFound = TRUE;
            break;
        }
    }
    if (!fFound)
        return DISP_E_MEMBERNOTFOUND;

    HRESULT hr = GetFuncInfoFromId(m_iid, dispidMember, lcid, info);
    if (FAILED(hr))
    {
        return S_OK;
    }

    return InvokeFromFuncInfo(pEvent, info, pdispparams, pvarResult);
}

//-----------------------------------------------------------------------------
//Helper for invoking the event
//-----------------------------------------------------------------------------
HRESULT CSymDHtmlControlSink::InvokeFromFuncInfo(SYMDHEVTFUNCCONTROL pEvent, _ATL_FUNC_INFO& info, DISPPARAMS* pdispparams, VARIANT* pvarResult)
{
    USES_ATL_SAFE_ALLOCA;
    VARIANTARG** pVarArgs = info.nParams ? (VARIANTARG**)_ATL_SAFE_ALLOCA(sizeof(VARIANTARG*)*info.nParams,_ATL_SAFE_ALLOCA_DEF_THRESHOLD) : 0;

    for (int i=0; i<info.nParams; i++)
        pVarArgs[i] = &pdispparams->rgvarg[info.nParams - i - 1];

    CComStdCallThunk<CSymDHtmlSinkHandler> thunk;

    if (m_pHandler)
        thunk.Init(pEvent, reinterpret_cast< CSymDHtmlSinkHandler* >((DWORD_PTR) m_pHandler - m_dwThunkOffset));

    CComVariant tmpResult;
    if (pvarResult == NULL)
        pvarResult = &tmpResult;

    HRESULT hr = DispCallFunc( &thunk, 0, info.cc, info.vtReturn, info.nParams, info.pVarTypes, pVarArgs, pvarResult );

    ATLASSERT(SUCCEEDED(hr));
    return hr;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
HRESULT CSymDHtmlControlSink::GetFuncInfoFromId( const IID& iid, DISPID dispidMember, LCID lcid, _ATL_FUNC_INFO& info)
{
    if (!m_spTypeInfo)
        return E_FAIL;

    return AtlGetFuncInfoFromId(m_spTypeInfo, iid, dispidMember, lcid, info);
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
VARTYPE CSymDHtmlControlSink::GetUserDefinedType(ITypeInfo *pTI, HREFTYPE hrt)
{
    return AtlGetUserDefinedType(pTI, hrt);
}


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
CSymDHtmlElementEventSink::CSymDHtmlElementEventSink(CSymDHtmlEventSink *pHandler, IDispatch *pdisp)
{
    m_pHandler = pHandler;
    pdisp->QueryInterface(__uuidof(IUnknown), (void **) &m_spunkElem);
    m_dwCookie = 0;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
STDMETHODIMP_(ULONG) CSymDHtmlElementEventSink::AddRef()
{
    return 1;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
STDMETHODIMP_(ULONG) CSymDHtmlElementEventSink::Release()
{
    return 1;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
STDMETHODIMP CSymDHtmlElementEventSink::QueryInterface(REFIID iid, LPVOID* ppvObj)
{
    if (!ppvObj)
        return E_POINTER;

    *ppvObj = NULL;
    if (IsEqualIID(iid, __uuidof(IUnknown)) || IsEqualIID(iid, __uuidof(IDispatch)))
    {
        *ppvObj = this;
        return S_OK;
    }
    return E_NOINTERFACE;
}


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
STDMETHODIMP CSymDHtmlElementEventSink::GetTypeInfoCount(UINT *pctinfo)
{
    *pctinfo = 0;
    ATLTRACENOTIMPL(_T("CSymDHtmlElementEventSink::GetTypeInfoCount"));
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
STDMETHODIMP CSymDHtmlElementEventSink::GetTypeInfo(UINT /*iTInfo*/, LCID /*lcid*/, ITypeInfo **ppTInfo)
{
    *ppTInfo = NULL;
    ATLTRACENOTIMPL(_T("CSymDHtmlElementEventSink::GetTypeInfo"));
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
STDMETHODIMP CSymDHtmlElementEventSink::GetIDsOfNames(REFIID /*riid*/, OLECHAR ** /*rgszNames*/, UINT /*cNames*/, LCID /*lcid*/, DISPID * /*rgDispId*/)
{
    ATLTRACENOTIMPL(_T("CSymDHtmlElementEventSink::GetIDsOfNames"));
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
STDMETHODIMP CSymDHtmlElementEventSink::Invoke(DISPID dispIdMember, REFIID /*riid*/, LCID /*lcid*/, WORD /*wFlags*/,
                                               DISPPARAMS * /*pdispparams*/, VARIANT *pVarResult, EXCEPINFO * /*pExcepInfo*/, UINT * /*puArgErr*/)
{
    HRESULT hr = S_OK;

    if (pVarResult)
        VariantInit(pVarResult);

    CComPtr<IHTMLElement> sphtmlElem;
    hr = m_spunkElem->QueryInterface(&sphtmlElem);

    if (!sphtmlElem)
        return hr;

    CComBSTR bstrId;
    hr = sphtmlElem->get_id(&bstrId);
    if (FAILED(hr))
        return hr;

    CComPtr<IHTMLDocument2> sphtmlDoc;
    hr = m_pHandler->GetDHtmlDocument(&sphtmlDoc);
    if (FAILED(hr))
        return hr;

    CComPtr<IHTMLWindow2> sphtmlWnd;
    hr = sphtmlDoc->get_parentWindow(&sphtmlWnd);

    if (FAILED(hr))
        return hr;

    CComPtr<IHTMLEventObj> sphtmlEvent;
    hr = sphtmlWnd->get_event(&sphtmlEvent);

    if (FAILED(hr))
        return hr;

    CComPtr<IHTMLElement> spsrcElem;
    hr = sphtmlEvent->get_srcElement(&spsrcElem);
    if (FAILED(hr))
        return hr;

    const SymDHtmlEventMapEntry *pEventMap = m_pHandler->GetDHtmlEventMap();
    for (int i=0; pEventMap[i].nType != SYM_DHTMLEVENTMAPENTRY_END; i++)
    {
        if (pEventMap[i].nType==SYM_DHTMLEVENTMAPENTRY_ELEMENT && pEventMap[i].dispId == dispIdMember)
        {
            if (pEventMap[i].szName && !wcscmp(CComBSTR(pEventMap[i].szName), bstrId))
            {
                // found the entry
                hr = (m_pHandler->*((SYMDHEVTFUNC) (m_pHandler->GetDHtmlEventMap()[i].pfnEventFunc)))(spsrcElem);

                if (pVarResult)
                {
                    pVarResult->vt = VT_BOOL;
                    pVarResult->boolVal = (hr==S_OK) ? ATL_VARIANT_TRUE : ATL_VARIANT_FALSE;
                }
                return S_OK;
            }
        }
    }

    return DISP_E_MEMBERNOTFOUND;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
HRESULT CSymDHtmlElementEventSink::Advise( LPUNKNOWN pUnkObj )
{
    return AtlAdvise((LPUNKNOWN)pUnkObj, (LPDISPATCH)this, __uuidof(HTMLElementEvents), &m_dwCookie);
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
HRESULT CSymDHtmlElementEventSink::UnAdvise( LPUNKNOWN pUnkObj)
{
    return AtlUnadvise((LPUNKNOWN)pUnkObj, __uuidof(HTMLElementEvents), m_dwCookie);
}


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
CSymDHtmlBrowserEventSink::CSymDHtmlBrowserEventSink() :
m_dwCookie(0)
{
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
STDMETHODIMP_(ULONG) CSymDHtmlBrowserEventSink::AddRef()
{
    return 1;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
STDMETHODIMP_(ULONG) CSymDHtmlBrowserEventSink::Release()
{
    return 1;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
STDMETHODIMP CSymDHtmlBrowserEventSink::QueryInterface(REFIID iid, LPVOID* ppvObj)
{
    if (!ppvObj)
        return E_POINTER;

    *ppvObj = NULL;
    if (IsEqualIID(iid, __uuidof(IUnknown)) || IsEqualIID(iid, __uuidof(IDispatch)))
    {
        *ppvObj = this;
        return S_OK;
    }
    return E_NOINTERFACE;
}


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
STDMETHODIMP CSymDHtmlBrowserEventSink::GetTypeInfoCount(UINT *pctinfo)
{
    *pctinfo = 0;
    ATLTRACENOTIMPL(_T("CSymDHtmlElementEventSink::GetTypeInfoCount"));
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
STDMETHODIMP CSymDHtmlBrowserEventSink::GetTypeInfo(UINT /*iTInfo*/, LCID /*lcid*/, ITypeInfo **ppTInfo)
{
    *ppTInfo = NULL;
    ATLTRACENOTIMPL(_T("CSymDHtmlElementEventSink::GetTypeInfo"));
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
STDMETHODIMP CSymDHtmlBrowserEventSink::GetIDsOfNames(REFIID /*riid*/, OLECHAR ** /*rgszNames*/, UINT /*cNames*/, LCID /*lcid*/, DISPID * /*rgDispId*/)
{
    ATLTRACENOTIMPL(_T("CSymDHtmlElementEventSink::GetIDsOfNames"));
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
STDMETHODIMP CSymDHtmlBrowserEventSink::Invoke(DISPID dispIdMember, REFIID riid, LCID lcid, WORD wFlags,
                                               DISPPARAMS * pDispParams, VARIANT *pVarResult, EXCEPINFO * pExcepInfo, UINT * puArgErr)
{
    HRESULT hr = S_OK;

    switch (dispIdMember)
    {
    case DISPID_BEFORENAVIGATE2:
        if( 7 == pDispParams->cArgs )
            BeforeNavigate( pDispParams->rgvarg[6].pdispVal, 
            &pDispParams->rgvarg[5],
            &pDispParams->rgvarg[4],
            &pDispParams->rgvarg[3],
            &pDispParams->rgvarg[2],
            &pDispParams->rgvarg[1], 
            pDispParams->rgvarg[0].pboolVal );
        break;

    case DISPID_NAVIGATECOMPLETE2:
        if( 2 == pDispParams->cArgs )
            NavigateComplete( pDispParams->rgvarg[1].pdispVal, &pDispParams->rgvarg[0] );
        break;

    case DISPID_STATUSTEXTCHANGE:
        StatusTextChange( pDispParams->rgvarg[1].bstrVal );
        break;

    case DISPID_PROGRESSCHANGE:
        if( 2 == pDispParams->cArgs )
            ProgressChange( pDispParams->rgvarg[1].lVal, pDispParams->rgvarg[0].lVal );
        break;

    case DISPID_DOCUMENTCOMPLETE:
        if( 2 == pDispParams->cArgs )
            DocumentComplete( pDispParams->rgvarg[1].pdispVal, &pDispParams->rgvarg[0] );
        break;

    case DISPID_DOWNLOADBEGIN:
        DownloadBegin();
        break;

    case DISPID_DOWNLOADCOMPLETE:
        DownloadComplete();
        break;

    case DISPID_COMMANDSTATECHANGE:
        if( 2 == pDispParams->cArgs )
            CommandStateChange( pDispParams->rgvarg[1].lVal, pDispParams->rgvarg[0].boolVal );
        break;

    case DISPID_NEWWINDOW2:
        if( 1 == pDispParams->cArgs )
            NewWindow( pDispParams->rgvarg[1].pdispVal, pDispParams->rgvarg[0].pboolVal );
        break;

    case DISPID_TITLECHANGE:
        if( 1 == pDispParams->cArgs )
            TitleChange( pDispParams->rgvarg[0].bstrVal );
        break;

    case DISPID_PROPERTYCHANGE:
        if( 1 == pDispParams->cArgs )
            PropertyChange( pDispParams->rgvarg[0].bstrVal );
        break;

    case DISPID_ONQUIT:
        OnQuit(); 
        break;

    case DISPID_ONVISIBLE:
        if( 1 == pDispParams->cArgs )
            OnVisible( pDispParams->rgvarg[0].boolVal );
        break;

    default:
        ATLTRACE2( atlTraceCOM, 2, _T("Invoke with unknown ID# %d\n"), dispIdMember );
        break;
    }

    return DISP_E_MEMBERNOTFOUND;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
HRESULT CSymDHtmlBrowserEventSink::Advise( LPUNKNOWN pUnkObj )
{
    return AtlAdvise( pUnkObj, (LPDISPATCH)this, __uuidof(DWebBrowserEvents2), &m_dwCookie);
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
HRESULT CSymDHtmlBrowserEventSink::UnAdvise( LPUNKNOWN pUnkObj )
{
    return AtlUnadvise( pUnkObj, __uuidof(DWebBrowserEvents2), m_dwCookie);
}



/////////////////////////////////////////////////////////////////////////////
// CSymDHtmlEventSink


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
HRESULT CSymDHtmlEventSink::ConnectToConnectionPoint(IUnknown *punkObj, REFIID riid, DWORD *pdwCookie)
{
    return AtlAdvise(punkObj, (IDispatch *) this, riid, pdwCookie);
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void CSymDHtmlEventSink::DisconnectFromConnectionPoint(IUnknown *punkObj, REFIID riid, DWORD& dwCookie)
{
    AtlUnadvise(punkObj, riid, dwCookie);
    dwCookie = 0;  // reset the cookie!
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
STDMETHODIMP CSymDHtmlEventSink::CSymDHtmlSinkHandlerQueryInterface(REFIID iid, LPVOID* ppvObj)
{
    if (!ppvObj)
        return E_POINTER;

    *ppvObj = NULL;
    if (IsEqualIID(iid, __uuidof(IDispatch)) || IsEqualIID(iid, __uuidof(IUnknown)))
    {
        *ppvObj = (IDispatch *) this;
        AddRef();
        return S_OK;
    }

    return E_NOINTERFACE;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
STDMETHODIMP_(ULONG) CSymDHtmlEventSink::AddRef()
{
    return 1;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
STDMETHODIMP_(ULONG) CSymDHtmlEventSink::Release()
{
    return 1;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
STDMETHODIMP CSymDHtmlEventSink::GetTypeInfoCount( UINT *pctinfo )
{
    *pctinfo = 0;
    return E_NOTIMPL;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
STDMETHODIMP CSymDHtmlEventSink::GetTypeInfo(UINT /*iTInfo*/, LCID /*lcid*/, ITypeInfo **ppTInfo)
{
    *ppTInfo = NULL;
    return E_NOTIMPL;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
STDMETHODIMP CSymDHtmlEventSink::GetIDsOfNames(REFIID /*riid*/, OLECHAR ** /*rgszNames*/, UINT /*cNames*/, LCID /*lcid*/, DISPID * /*rgDispId*/)
{
    return E_NOTIMPL;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
STDMETHODIMP CSymDHtmlEventSink::Invoke(DISPID dispIdMember, REFIID /*riid*/, LCID /*lcid*/, WORD /*wFlags*/, DISPPARAMS *pDispParams, VARIANT *pVarResult, EXCEPINFO *pExcepInfo, UINT *puArgErr)
{
    CComPtr< IHTMLElement > psrcElement;
    HRESULT hr = S_OK;

    VariantInit(pVarResult);

    if (DHtmlEventHook(&hr, dispIdMember, pDispParams, pVarResult, pExcepInfo, puArgErr))
        return hr;

    const SymDHtmlEventMapEntry *pMap = GetDHtmlEventMap();

    int nIndex = FindDHtmlEventEntry(pMap, dispIdMember, &psrcElement );

    if (nIndex<0)
        return DISP_E_MEMBERNOTFOUND;

    // now call it
    if (pMap)
    {
        // Added this check to prevent disabled button calls...
        bool bDisabled = false;
        CComQIPtr< IHTMLButtonElement > spButton( psrcElement );

        if( spButton )
        {
            VARIANT_BOOL var;
            spButton->get_disabled( &var );

            if( VARIANT_TRUE == var )
                bDisabled = true;
        }

        if( ! bDisabled )
        {
            hr = (this->*((SYMDHEVTFUNC) (GetDHtmlEventMap()[nIndex].pfnEventFunc)))(psrcElement);
            if (GetDHtmlEventMap()[nIndex].nType != SYM_DHTMLEVENTMAPENTRY_CONTROL && pVarResult)
            {
                pVarResult->vt = VT_BOOL;
                pVarResult->boolVal = (hr==S_OK) ? ATL_VARIANT_TRUE : ATL_VARIANT_FALSE;
            }
        }
    }

    return hr;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
BOOL CSymDHtmlEventSink::DHtmlEventHook(HRESULT * /*phr*/, DISPID /*dispIdMember*/, DISPPARAMS * /*pDispParams*/,VARIANT * /*pVarResult*/, EXCEPINFO * /*pExcepInfo*/, UINT * /*puArgErr*/)
{
    // stub base implementation
    return FALSE;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
int CSymDHtmlEventSink::FindDHtmlEventEntry(const SymDHtmlEventMapEntry *pEventMap, DISPID dispIdMember,IHTMLElement **ppsrcElement)
{
    HRESULT hr = DISP_E_MEMBERNOTFOUND;
    CComPtr<IHTMLWindow2> sphtmlWnd;
    CComPtr<IHTMLEventObj> sphtmlEvent;
    CComPtr<IHTMLElement> sphtmlElement;
    CComPtr<IHTMLDocument2> sphtmlDoc;
    CComBSTR bstrName;
    CComBSTR bstrClass;
    CComBSTR bstrTagName;

    int i;
    int nIndexFound = -1;

    if(ppsrcElement == NULL)
        return E_POINTER;

    *ppsrcElement = NULL;

    if (!pEventMap)
        goto Error;

    // get the html document
    hr = GetDHtmlDocument(&sphtmlDoc);
    if (sphtmlDoc == NULL)
        goto Error;

    // get the element that generated the event
    sphtmlDoc->get_parentWindow(&sphtmlWnd);
    if ((sphtmlWnd==NULL) || FAILED(sphtmlWnd->get_event(&sphtmlEvent)) || (sphtmlEvent==NULL))
    {
        hr = DISP_E_MEMBERNOTFOUND;
        goto Error;
    }

    sphtmlEvent->get_srcElement(&sphtmlElement);
    *ppsrcElement = sphtmlElement;

    if (sphtmlElement)
        sphtmlElement.p->AddRef();

#ifdef _DEBUG

    if( -606 != dispIdMember )
    {
        if (!bstrName && sphtmlElement)
            sphtmlElement->get_id(&bstrName);

        USES_CONVERSION;
        CString strTest = OLE2T(bstrName);

        if( strTest == "s" )
            DebugBreak();
    }
#endif // _DEBUG

    // look for the dispid in the map
    for (i=0; pEventMap[i].nType != SYM_DHTMLEVENTMAPENTRY_END; i++)
    {
        if (pEventMap[i].dispId == dispIdMember)
        {
            if (pEventMap[i].nType == SYM_DHTMLEVENTMAPENTRY_NAME)
            {
                if (!bstrName && sphtmlElement)
                    sphtmlElement->get_id(&bstrName);

                //ATLTRACE( "Element name = %S, id = %d\n", bstrName, dispIdMember );

                if (bstrName && pEventMap[i].szName && !wcscmp(bstrName, CComBSTR(pEventMap[i].szName)) ||
                    (!bstrName && !sphtmlElement))
                {
                    nIndexFound = i;
                    break;
                }
            }
            else if (pEventMap[i].nType == SYM_DHTMLEVENTMAPENTRY_CLASS)
            {
                if (!bstrClass && sphtmlElement)
                    sphtmlElement->get_className(&bstrClass);

                if (bstrClass && !wcscmp(bstrClass, CComBSTR(pEventMap[i].szName)))
                {
                    nIndexFound = i;
                    break;
                }
            }
            else if (pEventMap[i].nType == SYM_DHTMLEVENTMAPENTRY_TAG)
            {
                if (!bstrTagName && sphtmlElement)
                    sphtmlElement->get_tagName(&bstrTagName);

                if (bstrTagName && !_wcsicmp(bstrTagName, CComBSTR(pEventMap[i].szName)))
                {
                    nIndexFound = i;
                    break;
                }
            }
        }
    }
Error:
    if (nIndexFound==-1 && *ppsrcElement)
    {
        (*ppsrcElement)->Release();
        *ppsrcElement = NULL;
    }

    return nIndexFound;
}




