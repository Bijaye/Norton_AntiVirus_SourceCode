////////////////////////
//
// PROPRIETARY / CONFIDENTIAL.
// Use of this product is subject to license terms.
// Copyright © 2006 Symantec Corporation.
// All rights reserved.
//
/////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "SessionAppSysTrayIcon.h"
#include "SessionAppServerInterface.h"

#include <map>

typedef std::map<DWORD, HICON> MAP_DWORD_TO_HICON;
typedef std::map<DWORD, CString> MAP_DWORD_TO_CSTRING;

namespace SessionApp
{
    // Forward declaration
    class CServerImpl;

    extern const UINT RM_TRAY;
    extern const UINT RM_TASKBARCREATED;
    extern const UINT RM_SHOWTRAYICON;
    extern const UINT RM_EVTMGRISDOWN;
    extern const UINT RM_DOSTATECHANGE;
    extern const UINT RM_SHOWTRAYAPP;
    extern const UINT RM_HIDETRAYAPP;
    extern const UINT RM_SETTRAYICON;
    extern const UINT RM_SETTOOLTIPTEXT;
    extern const UINT RM_CHANGESTATE;
    extern const UINT RM_GETTRAYICONSTATE;

    extern const UINT TIMERID_CCEVTCHK;
    extern const UINT TIMERID_FLASHICON;


    typedef CWinTraits < 0, 0 > CCommonWindowTraits;

    namespace Flash
    {
        enum{ eInvalid, eBegin, eAppNotify, eAppState, eEnd = eAppState };
    };
    
    typedef struct tagSETICON_DATA
    {
        DWORD dwIconId;
        HMODULE hResModule;
        UINT iResIconId;
    } SETICON_DATA;
    typedef struct tagSETICON_DATA *LPSETICON_DATA;

    typedef struct tagSTATESTRING_DATA
    {
        DWORD dwState;
        LPCWSTR wszText;
    } STATESTRING_DATA;
    typedef struct tagSTATESTRING_DATA *LPSTATESTRING_DATA;

    typedef struct tagSTATECHANGE_DATA
    {
        DWORD dwState;
        DWORD dwDuration;
        LPCWSTR wszText;
    } STATECHANGE_DATA;
    typedef struct tagSTATECHANGE_DATA *LPSTATECHANGE_DATA;

    typedef struct tagGETTRAYICONSTATE_DATA
    {
        DWORD *pdwAppState;
        DWORD *pdwNotifyState;
        DWORD *pdwDuration;
        CString *pcszText;
    } GETTRAYICONSTATE_DATA;
    typedef struct tagGETTRAYICONSTATE_DATA *LPGETTRAYICONSTATE_DATA;

    //****************************************************************************
    // 
    //****************************************************************************
    class CAppWindow : public CWindowImpl< CAppWindow, CWindow, CCommonWindowTraits >, 
        public CTrayIconImpl<CAppWindow>,
        public CMessageFilter, 
        public CIdleHandler
    {
    public:

        //
        //  c'tor / d'tor
        //
        CAppWindow();  // no body so clients don't forget to use proper c'tor
        virtual ~CAppWindow();

        //
        //  window class name
        //
        DECLARE_WND_CLASS( SZ_AppWND_APPWINDOWNAME )

        //
        // WTL message map
        //
        BEGIN_MSG_MAP( CAppWindow )
            MESSAGE_HANDLER( WM_CREATE, OnCreate )
            MESSAGE_HANDLER( WM_DESTROY, OnDestroy )
            MESSAGE_HANDLER( SessionApp::RM_TASKBARCREATED, OnTaskbarCreated )
            MESSAGE_HANDLER( SessionApp::RM_EVTMGRISDOWN, OnEvtMgrIsDown )
            MESSAGE_HANDLER( SessionApp::RM_DOSTATECHANGE, OnDoStateChange )
            MESSAGE_HANDLER( SessionApp::RM_SHOWTRAYAPP, OnShowTrayApp )
            MESSAGE_HANDLER( SessionApp::RM_HIDETRAYAPP, OnHideTrayApp )
            MESSAGE_HANDLER( SessionApp::RM_SETTRAYICON, OnSetIcon )
            MESSAGE_HANDLER( SessionApp::RM_SETTOOLTIPTEXT, OnSetTooltipText )
            MESSAGE_HANDLER( SessionApp::RM_CHANGESTATE, OnChangeState )
            MESSAGE_HANDLER( SessionApp::RM_GETTRAYICONSTATE, OnGetCurrentIconState )
            MESSAGE_HANDLER( WM_TIMER, OnTimer)
            MESSAGE_HANDLER( WM_QUERYENDSESSION, OnQueryEndSession )
            COMMAND_CODE_HANDLER( 0, OnMenuCommand)
            CHAIN_MSG_MAP(CTrayIconImpl<CAppWindow>)
        END_MSG_MAP()

        //
        //  CWindowImpl virtual overrides
        //
        virtual BOOL PreTranslateMessage(MSG* pMsg);
        virtual BOOL OnIdle();
        virtual void OnFinalMessage( HWND hWnd );

        //
        //  CTrayIconImpl virtual overrides
        //
        virtual void PrepareMenu(HMENU &hMenu);
        virtual void OnTrayItemClick();
        virtual void OnTrayItemDblClick();

        //
        // Message handlers
        //
        LRESULT OnCreate( UINT nMsg, WPARAM wParam, LPARAM lParam, BOOL & bHandled );
        LRESULT OnDestroy( UINT nMsg, WPARAM wParam, LPARAM lParam, BOOL & bHandled );
        LRESULT OnQueryEndSession( UINT nMsg, WPARAM wParam, LPARAM lParam, BOOL & bHandled );
        LRESULT OnTimer( UINT nMsg, WPARAM wParam, LPARAM lParam, BOOL & bHandled );
        LRESULT OnTaskbarCreated( UINT nMsg, WPARAM wParam, LPARAM lParam, BOOL & bHandled );
        LRESULT OnEvtMgrIsDown( UINT nMsg, WPARAM wParam, LPARAM lParam, BOOL & bHandled );
        LRESULT OnDoStateChange( UINT nMsg, WPARAM wParam, LPARAM lParam, BOOL & bHandled );

        LRESULT OnShowTrayApp( UINT nMsg, WPARAM wParam, LPARAM lParam, BOOL & bHandled );
        LRESULT OnHideTrayApp( UINT nMsg, WPARAM wParam, LPARAM lParam, BOOL & bHandled );

        LRESULT OnSetIcon( UINT nMsg, WPARAM wParam, LPARAM lParam, BOOL & bHandled );
        LRESULT OnSetTooltipText( UINT nMsg, WPARAM wParam, LPARAM lParam, BOOL & bHandled );
        LRESULT OnChangeState( UINT nMsg, WPARAM wParam, LPARAM lParam, BOOL & bHandled );
        LRESULT OnGetCurrentIconState( UINT nMsg, WPARAM wParam, LPARAM lParam, BOOL & bHandled );

        //
        // Command handlers
        //
        LRESULT OnMenuCommand(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);

    public:
        HRESULT GetExitCode(){ return m_hrExitCode; };
        BOOL ShowTrayApp(DWORD dwState = SessionApp::AppState::eInvalid);
        BOOL HideTrayApp();

        BOOL SetIcon(DWORD dwState, HMODULE hModule, UINT iIconId);
        BOOL SetTooltipText(DWORD dwState, HMODULE hModule, UINT iToolTipText);
        BOOL SetTooltipText(DWORD dwState, LPCWSTR wszToolTipText);
        BOOL ChangeState(DWORD dwState, DWORD dwDuration, LPCWSTR wszToolTipText);
        BOOL GetCurrentIconState(DWORD& dwAppState, DWORD& dwNotifyState, DWORD& dwDuration, CString& cszToolTipText);

    protected:
        void SetExitCode(HRESULT hrExitCode){ m_hrExitCode = hrExitCode; };

        void OnTimerCheckEventManagerStatus();
        void OnTimerFlashIcon();
        void SetFlashingState(DWORD dwState, DWORD dwTimeout, LPCWSTR wszToolTipText);

    protected:
        MAP_DWORD_TO_HICON m_mapIconMap;
        MAP_DWORD_TO_CSTRING m_mapToolTipText;

        HRESULT m_hrExitCode;

        CToolTipCtrl m_wndBallon;
        
        DWORD m_dwCurrentAppState;
        DWORD m_dwCurrentNotifyState;
        DWORD m_dwFlashState;
        DWORD m_dwFlashIconTimeout;

        UINT m_iDefaultMenuItem;
        UINT m_iMenu;
        
        CString m_cszTempToolTipText;

        HMODULE m_hModuleMenu;
        
    public:
        static const LPCTSTR SZ_AppWND_APPWINDOWNAME;
        CServerImpl* m_pSessionAppServer;
        
    };

}; // App
