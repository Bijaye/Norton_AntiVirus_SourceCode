// BrowserEvents.h : Declaration of the CBrowserEvents

#ifndef __BrowserEvents_h__
#define __BrowserEvents_h__

#include <exdispid.h>       // DISPID_NAVIGATECOMPLETE2

class CIEToolBand;  // Forword decleration
class CBrowserEvents;
typedef IDispEventSimpleImpl<1, CBrowserEvents, &DIID_DWebBrowserEvents2> TEventBeforeNavigate2;
typedef IDispEventSimpleImpl<2, CBrowserEvents, &DIID_DWebBrowserEvents2> TEventNavigateComplete2;

/////////////////////////////////////////////////////////////////////////////
// CBrowserEvents
class CBrowserEvents : public TEventBeforeNavigate2
                     , public TEventNavigateComplete2
{
public:
	typedef enum ENavigateState { Navigate_NA = -1
	                            , Navigate_Started
	                            , Navigate_Complete } ENavigateState;
private:
	ENavigateState m_bNavigateState;
	CIEToolBand& m_Outer;
	static _ATL_FUNC_INFO m_BeforeNavigate2Info;
	static _ATL_FUNC_INFO m_NavigateComplete2Info;

public:
	CBrowserEvents(CIEToolBand& Outer);

	void __stdcall onBeforeNavigate2(IDispatch *pIDispatch, VARIANT *pvURL       /* VT_BSTR */
	                                                      , VARIANT *pvFlags     /* VT_I4 */
	                                                      , VARIANT *pvTargetFrameName  /* VT_BSTR */
	                                                      , VARIANT *pvPostData  /* VT_VARIANT */
	                                                      , VARIANT *pvHeaders   /* VT_BSTR */
	                                                      , VARIANT *pvCancel    /* VT_BOOL */);
	void __stdcall onNavigateComplete2(IDispatch *pIDispatch, VARIANT *pvURL);
	HRESULT Unadvise(void);
	HRESULT Advise(void);
	ENavigateState NavigateState(void) { return m_bNavigateState; }

BEGIN_SINK_MAP(CBrowserEvents)
	SINK_ENTRY_INFO(1, DIID_DWebBrowserEvents2    // dipsinterface
	                 , DISPID_BEFORENAVIGATE2     // DISPID
	                 , onBeforeNavigate2          // Handler
	                 , &m_BeforeNavigate2Info)   // handler info
	SINK_ENTRY_INFO(2, DIID_DWebBrowserEvents2   // dipsinterface
	                 , DISPID_NAVIGATECOMPLETE2  // DISPID
	                 , onNavigateComplete2       // Handler
	                 , &m_NavigateComplete2Info) // handler info
END_SINK_MAP()
};
#endif __BrowserEvents_h__
