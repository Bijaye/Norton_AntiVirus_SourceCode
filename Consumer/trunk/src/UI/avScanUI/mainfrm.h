////////////////////////
//
// PROPRIETARY / CONFIDENTIAL.
// Use of this product is subject to license terms.
// Copyright © 2006 Symantec Corporation.
// All rights reserved.
//
/////////////////////////////////////////////////////////////////////////////////////////////////

/*****************************************************************************/
/*****************************************************************************/
#pragma once

#include <SymInterface.h>
#include <SymHTMLLoader.h>
#include <SymHTMLMouseEvent.h>
#include "FrameDocument.h"
#include <SymInterface.h>
#include <ccSymModuleLifetimeMgrHelper.h>

typedef CWinTraits<WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN, 0> CTestTraits;
#define WMU_SWITCHTOTAB (WM_USER + 12)

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
class CMainFrame : 
    public CFrameWindowImpl<CMainFrame, CWindow, CTestTraits>, 
    public CMessageFilter
{

public:
    DECLARE_FRAME_WND_CLASS(L"Sym_Common_Scan_Window", IDR_MAINFRAME)

    typedef CFrameWindowImpl<CMainFrame, CWindow, CTestTraits> theBase;

    virtual ~CMainFrame()
    {
        CCTRCTXI0(L"dtor");
        TRACE_REF_COUNT(m_spSymHTMLWindow);
        TRACE_REF_COUNT(m_spFrameHTMLdoc);
    }

    //-------------------------------------------------------------------------
    virtual BOOL PreTranslateMessage(MSG* pMsg)
    {
        if(theBase::PreTranslateMessage(pMsg))
            return TRUE;

        // Forward these messages to our frame window
        if(m_spSymHTMLWindow && S_OK == m_spSymHTMLWindow->PretranslateMessage(pMsg))
            return TRUE;

        return FALSE;
    }

    BEGIN_MSG_MAP(CMainFrame)
        MESSAGE_HANDLER(WM_SIZE, OnSize)
        MESSAGE_HANDLER(WM_CREATE, OnCreate)
        MESSAGE_HANDLER(WM_GETMINMAXINFO, OnGetMinMaxInfo)
        MESSAGE_HANDLER(WM_SYSCOMMAND, OnSysCommand)
        MESSAGE_HANDLER(WM_SETFOCUS, OnSetFocus)
        MESSAGE_HANDLER(WMU_SWITCHTOTAB, OnSwitchTab)
        CHAIN_MSG_MAP(theBase)
    END_MSG_MAP()

    void SetMainDocument(CFrameDocument* pFrameDoc)
    {
        m_spFrameHTMLdoc = pFrameDoc;
        if(m_spSymHTMLWindow)
            m_spSymHTMLWindow->Render(m_spFrameHTMLdoc);
    }

    void ReleaseMainDocument()
    {
        m_spFrameHTMLdoc.Release();
    }

    //-------------------------------------------------------------------------
    LRESULT OnSize(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
    {
        if(m_spSymHTMLWindow)
        {
            CRect rect;
            GetClientRect(rect);
            m_spSymHTMLWindow->Resize(rect.Width(), rect.Height());
        }

        return 0;
    }

    LRESULT OnSetFocus(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
    {
        bHandled = TRUE;
        m_spSymHTMLWindow->SetFocus();
        return 0;
    }

    //-------------------------------------------------------------------------
    LRESULT OnGetMinMaxInfo(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
    {
        LPMINMAXINFO lpMMI = reinterpret_cast<LPMINMAXINFO>(lParam);

        lpMMI->ptMinTrackSize.x = m_nMinWidth;
        lpMMI->ptMinTrackSize.y = m_nMinHeight;
        return 0;
    }

    //-------------------------------------------------------------------------
    LRESULT OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
    {
        symhtml::loader::ISymHTMLWindow::CreateObject(GETMODULEMGR(), &m_spSymHTMLWindow);
        if(m_spSymHTMLWindow)
        {
            CRect rect;
            GetWindowRect(rect);

            HRESULT hr = m_spSymHTMLWindow->Create(m_hWnd, rect);
            if(FAILED(hr))
            {
                CCTRACEE(CCTRCTX _T("Unable to create SymHTML window."));
                ATLASSERT(FALSE);
                return -1;
            }

            ATLASSERT(m_spFrameHTMLdoc != NULL);

            // Render the main document
            m_spSymHTMLWindow->Render(m_spFrameHTMLdoc);
       }

        // register object for message filtering and idle updates
        CMessageLoop* pLoop = _Module.GetMessageLoop();
        ATLASSERT(pLoop != NULL);
        pLoop->AddMessageFilter(this);

        // Because we may be using SymTheme, we must send a WM_NCCALCSIZE
        // to see what the client size would be using the max width/height.
        // Once we know what the client size would be with the skin, we can adjust the window size...
        CRect rcWnd(0, 0, FRAME_MIN_WIDTH, FRAME_MIN_HEIGHT);
        SendMessage(WM_NCCALCSIZE, FALSE, (LPARAM)&rcWnd);

        int nXAdd = FRAME_MIN_WIDTH - rcWnd.Width();
        int nYAdd = FRAME_MIN_HEIGHT - rcWnd.Height();

        m_nMinWidth = FRAME_MIN_WIDTH + nXAdd;
        m_nMinHeight = FRAME_MIN_HEIGHT + nYAdd;

        CCTRACEI(CCTRCTX L"Adjusting window rect to: cx=%d, cy=%d, nonclient_cx=%d, nonclinet_cy=%d, min-width=%d, min-height=%d",
                 m_nMinWidth, m_nMinHeight, nXAdd, nYAdd, FRAME_MIN_WIDTH, FRAME_MIN_HEIGHT);

        SetWindowPos(0, 0, 0, m_nMinWidth, m_nMinHeight, SWP_NOMOVE | SWP_NOZORDER);

        // Center our window on the screen/parent
        CenterWindow();

        return 0;
    }

    LRESULT OnSysCommand(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
    {
        bHandled = FALSE;
        switch(wParam)
        {
            case SC_CLOSE:
            {
                if(m_spFrameHTMLdoc)
                    return m_spFrameHTMLdoc->OnSysClose(uMsg, wParam, lParam, bHandled);
                break;
            }
            case SC_MINIMIZE:
            {
                if(m_spFrameHTMLdoc)
                    return m_spFrameHTMLdoc->OnSysMinimize(uMsg, wParam, lParam, bHandled);
                break;
            }
            case SC_RESTORE:
            {
                if(m_spFrameHTMLdoc)
                    return m_spFrameHTMLdoc->OnSysRestore(uMsg, wParam, lParam, bHandled);
                break;
            }
        }

        return -1;
    }

    LRESULT OnSwitchTab(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
    {
        if(!m_sNewTab.IsEmpty() && m_spFrameHTMLdoc)
        {
            HRESULT hr = m_spFrameHTMLdoc->SwitchToTab(m_sNewTab);
            LOG_FAILURE(L"Failed to switch tabs", hr);
            m_sNewTab.Empty();
        }

        return 0;
    }

    void SwitchToTab(LPCSTR pszNewTab)
    {
        m_sNewTab = pszNewTab;
        PostMessage(WMU_SWITCHTOTAB);
    }

protected:
    symhtml::ISymHTMLWindowPtr m_spSymHTMLWindow;
    CSymPtr<CFrameDocument> m_spFrameHTMLdoc;
    CStringA m_sNewTab;
    int m_nMinWidth;
    int m_nMinHeight;
};
