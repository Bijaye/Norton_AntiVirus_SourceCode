//****************************************************************************
// 
//****************************************************************************
#pragma once

#include "resource.h"
#include "ThunkImpl.h"

typedef CWinTraits < 0, 0 > CCommonWindowTraits;

const unsigned int TIMERID_CCEVTCHK  = 0x03;

const LPCTSTR SZ_SPP_APPWINDOWNAME = _T("_HPP_AppWnd_Class_");

namespace SPPWidget
{
    HICON LoadIconEx( HMODULE hModule, UINT nId, int cxDesired, int cyDesired, UINT uFlags );
};

//****************************************************************************
// 
//****************************************************************************
class CHPPAppWnd : public CWindowImpl< CHPPAppWnd, CWindow, CCommonWindowTraits >, 
                 public CMessageFilter, 
                 public CIdleHandler
{
public:
    
    //
    //  c'tor / d'tor
    //
    CHPPAppWnd();  // no body so clients don't forget to use proper c'tor
    virtual ~CHPPAppWnd();
    
    //
    //  window class name
    //
    DECLARE_WND_CLASS( SZ_SPP_APPWINDOWNAME )

    //
    // WTL message map
    //
    BEGIN_MSG_MAP( CHPPAppWnd )
		// Message handlers
        MESSAGE_HANDLER( WM_CREATE, OnCreate )
		MESSAGE_HANDLER( WM_DESTROY, OnDestroy )
        MESSAGE_HANDLER( HPPAPPWND::RM_CCEVTISDOWN, OnCCEvtIsDown )
        MESSAGE_HANDLER( WM_TIMER, OnTimer)
        MESSAGE_HANDLER( WM_QUERYENDSESSION, OnQueryEndSession )
    END_MSG_MAP()

    //
    //  virtual overrides
    //
    virtual BOOL PreTranslateMessage(MSG* pMsg);
    virtual BOOL OnIdle();
    virtual void OnFinalMessage( HWND hWnd );

    //
    // Message handlers
    //
    LRESULT OnCreate( UINT nMsg, WPARAM wParam, LPARAM lParam, BOOL & bHandled );
    LRESULT OnDestroy( UINT nMsg, WPARAM wParam, LPARAM lParam, BOOL & bHandled );
    LRESULT OnQueryEndSession( UINT nMsg, WPARAM wParam, LPARAM lParam, BOOL & bHandled );
    LRESULT OnTimer( UINT nMsg, WPARAM wParam, LPARAM lParam, BOOL & bHandled );
    LRESULT OnCCEvtIsDown( UINT nMsg, WPARAM wParam, LPARAM lParam, BOOL & bHandled );
  
protected:
    void OnTimerCheckEventManagerStatus();

public:
    HANDLE m_ccEvtMgrReconnectEvent;
    CSymQIPtr<ISymBase, &IID_SymBase> m_spUnknownApp;

};

