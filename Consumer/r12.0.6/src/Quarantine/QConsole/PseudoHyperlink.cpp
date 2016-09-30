//////////////////////////////////////////////////////////////////////////////
// PROPRIETARY / CONFIDENTIAL.
// Use of this product is subject to license terms.
// Copyright © 2005 Symantec Corporation.
// All rights reserved.
//////////////////////////////////////////////////////////////////////////////
//
// PseudoHyperlink.cpp : Implementation for CPseudoHyperlink class
// Makes a static text control into a pseudo hyperlink.
// Either launches a web page with specified URL or calls a callback.
//
/////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "qconsole.h"
#include "StartBrowser.h"
#include "PseudoHyperlink.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


/////////////////////////////////////////////////////////////////////////////
// CPseudoHyperlink

IMPLEMENT_DYNAMIC(CPseudoHyperlink, CStatic)

CPseudoHyperlink::CPseudoHyperlink()
{
    m_crLinkColor  = RGB(0, 0, 238);	// Blue
    m_crHoverColor = ::GetSysColor(COLOR_HIGHLIGHT);
    
	m_bHoveringOverLink = false; // Cursor not currently over control
	m_hLinkCursor   = NULL;	 // No cursor yet

    m_bUnderline    = TRUE;	 // Underline the link?
    m_bBold			= false;
	m_bAutoSize		= true;	 // Resize the window to fit text?
	m_bFocusDrawn	= false;
	
	m_brBackground.CreateSolidBrush(GetSysColor(COLOR_BTNFACE));

    m_sLinkText.Empty();
	m_sURL.Empty();
	m_pLinkData    = NULL;
	m_LinkType	   = LinkType_None;
	m_bLinkEffects = true;
}

CPseudoHyperlink::~CPseudoHyperlink()
{
	m_Font.DeleteObject();
	m_brBackground.DeleteObject();

	if(NULL != m_pLinkData)
	{
		delete m_pLinkData;
		m_pLinkData = NULL;
	}
}


BEGIN_MESSAGE_MAP(CPseudoHyperlink, CStatic)
	//{{AFX_MSG_MAP(CPseudoHyperlink)
    ON_CONTROL_REFLECT(STN_CLICKED, OnClicked)
    ON_WM_CTLCOLOR_REFLECT()
    ON_WM_SETCURSOR()
    ON_WM_MOUSEMOVE()
	ON_WM_KILLFOCUS()
	ON_WM_SETFOCUS()
	ON_WM_SHOWWINDOW()
    //}}AFX_MSG_MAP
END_MESSAGE_MAP()


// Overrides

BOOL CPseudoHyperlink::PreTranslateMessage(MSG* pMsg) 
{
	// Handle ENTER key
	if((WM_KEYDOWN == pMsg->message) 
		&& ((VK_RETURN == pMsg->wParam) || (VK_SPACE == pMsg->wParam)))
	{
		DoLinkAction();
		return TRUE;
	}

    return CStatic::PreTranslateMessage(pMsg);
}


void CPseudoHyperlink::PreSubclassWindow() 
{
    // Get mouse clicks via STN_CLICKED
	DWORD dwStyle = CWnd::GetStyle();
    ::SetWindowLongPtr(m_hWnd, GWL_STYLE, dwStyle | SS_NOTIFY);
    
    // If link text is empty, use window text
    if(m_sLinkText.IsEmpty())
        GetWindowText(m_sLinkText);
	SetWindowText(m_sLinkText);
	
    // Create the font
    LOGFONT logFont;
    GetFont()->GetLogFont(&logFont);
    logFont.lfUnderline = m_bUnderline;

	if(m_bBold)
		logFont.lfWeight = FW_BOLD;

    if(FALSE != m_Font.CreateFontIndirect(&logFont))
		SetFont(&m_Font);
	
    ResizeAndPositionWindow();	// Adjust size of window to fit text
    SetDefaultLinkCursor(); // Load a "hand" cursor
	
    CStatic::PreSubclassWindow();
}


// CPseudoHyperlink message handlers

HBRUSH CPseudoHyperlink::CtlColor(CDC* pDC, UINT nCtlColor)
{
    ASSERT(CTLCOLOR_STATIC == nCtlColor);
	
    if(m_bHoveringOverLink)
        pDC->SetTextColor(m_crHoverColor);
    else
        pDC->SetTextColor(m_crLinkColor);
	
    // transparent text.
    pDC->SetBkMode(TRANSPARENT);

	return (HBRUSH)GetStockObject(NULL_BRUSH);
}


BOOL CPseudoHyperlink::OnSetCursor(CWnd* /*pWnd*/, UINT /*nHitTest*/, UINT /*message*/) 
{
	if(NULL == m_hLinkCursor)
	{
		SetDefaultLinkCursor();
		return FALSE;
	}

	// else
	::SetCursor(m_hLinkCursor);
	return TRUE;
}


void CPseudoHyperlink::OnMouseMove(UINT nFlags, CPoint point) 
{
    CStatic::OnMouseMove(nFlags, point);
	
    if(m_bHoveringOverLink) // Cursor is currently over control
    {
        CRect rect;
		CWnd::GetClientRect(rect);
		
        if(!rect.PtInRect(point) || !CWnd::IsWindowVisible())
        {
            m_bHoveringOverLink = false;
            ReleaseCapture();
            CWnd::RedrawWindow();
        }
    }
    else // Cursor has just moved over control
    {
        m_bHoveringOverLink = true;
        CWnd::RedrawWindow();
        CWnd::SetCapture();
    }
}


void CPseudoHyperlink::OnKillFocus(CWnd* pNewWnd)
{
	m_bFocusDrawn = false;
	ShowFocus();
}


void CPseudoHyperlink::OnSetFocus(CWnd* pOldWnd)
{
	m_bFocusDrawn = true;
	ShowFocus();
}


void CPseudoHyperlink::OnShowWindow(BOOL bShow, UINT nStatus)
{
	CStatic::OnShowWindow(bShow, nStatus);

	// if hidden and has focus rect, remove the focus rect
	if(!bShow && m_bFocusDrawn)
		ShowFocus();
}


void CPseudoHyperlink::OnClicked()
{	
	DoLinkAction();
}


/////////////////////////////////////////////////////////////////////////////
// CHyperLink implementation

// Move and resize the window so it's the same size as the hyperlink text.
// The hyperlink cursor will only be active directly over the text. 
// If the text is left justified the window is just resized, but if the text
// is centered or right justified, the window also has to be moved.
void CPseudoHyperlink::ResizeAndPositionWindow()
{
    if(!::IsWindow(m_hWnd) || !m_bAutoSize) 
        return;
	
    // Get current window position
    CRect rect;
	CWnd::GetWindowRect(rect);
	
    CWnd* pParent = CWnd::GetParent();
    if(NULL != pParent)
        pParent->ScreenToClient(rect);
	
    // Get size of window text
    CString sWndText;
    GetWindowText(sWndText);
	
    CDC* pDC = CWnd::GetDC();
    CFont* pOldFont = pDC->SelectObject(&m_Font);
    CSize Extent = pDC->GetTextExtent(sWndText);
	if(NULL != pOldFont)
		pDC->SelectObject(pOldFont);
    CWnd::ReleaseDC(pDC);
	
	// Get the text justification via the window style
    DWORD dwStyle = CWnd::GetStyle();
	
    // Recalculate window size and position based on the text justification
    if(dwStyle & SS_CENTERIMAGE) // text is centered vertically
        rect.DeflateRect(0, (rect.Height() - Extent.cy)/2);
    else
        rect.bottom = rect.top + Extent.cy;
	
	// horizontal spacing adjustment
    if(dwStyle & SS_CENTER)
        rect.DeflateRect((rect.Width() - Extent.cx)/2, 0);
    else if(dwStyle & SS_RIGHT)
        rect.left  = rect.right - Extent.cx;
    else // SS_LEFT = 0, can't test for it explicitly 
        rect.right = rect.left + Extent.cx;
	
    // Move the window
    if(FALSE == SetWindowPos(NULL, rect.left, rect.top, rect.Width(), rect.Height(), SWP_NOZORDER))
		CCTRACEE(_T("%s - !SetWindowPos()"), __FUNCTION__);
}


// Per Paul DiLascia's Jan 1998 MSJ article
// Loads a "hand" cursor from winhlp32.exe
void CPseudoHyperlink::SetDefaultLinkCursor()
{
    if(NULL == m_hLinkCursor) // No cursor handle - load our own
    {
        // Get the windows directory
        CString sWndDir;
        UINT iRet = GetWindowsDirectory(sWndDir.GetBuffer(MAX_PATH), MAX_PATH);
		sWndDir.ReleaseBuffer();
		if((0 == iRet) || (iRet > MAX_PATH) || sWndDir.IsEmpty())
		{
			CCTRACEE(_T("%s - Failed GetWindowsDirectory(). GetLastError=%d"), __FUNCTION__, GetLastError());
			return;
		}
		
		// Retrieves cursor #106 (hand pointer) from winhlp32.exe
        sWndDir += _T("\\winhlp32.exe");
        HMODULE hModule = LoadLibrary(sWndDir);
        if(NULL != hModule)
		{
            HCURSOR hHandCursor = ::LoadCursor(hModule, MAKEINTRESOURCE(106));
            if(NULL != hHandCursor)
                m_hLinkCursor = CopyCursor(hHandCursor);
        }
        if(FALSE == FreeLibrary(hModule))
			CCTRACEE(_T("%s - Failed FreeLibrary(). GetLastError=%d"), __FUNCTION__, GetLastError());
    }
}


void CPseudoHyperlink::ShowFocus()
{
	if(!m_bLinkEffects)
		return;

	CRect clientRect;
	CWnd *pWnd = CWnd::GetParent();
	if(NULL != pWnd)
	{
		CWnd::GetWindowRect(&clientRect);
		pWnd->ScreenToClient(&clientRect);
		clientRect.InflateRect(3, 3);
		pWnd->InvalidateRect(&clientRect);
		pWnd->UpdateWindow();
	}

	// draw focus rect
	if(m_bFocusDrawn)
	{
		CWnd::GetClientRect(&clientRect);
		clientRect.InflateRect(3, 3);
		CDC* pDC = CWnd::GetDC();
		pDC->DrawFocusRect(&clientRect);
		CWnd::ReleaseDC(pDC);
	}
}


void CPseudoHyperlink::RedrawControlRect()
{		
	CRect clientRect;
	CWnd::GetWindowRect(&clientRect);

	CWnd *pWnd = CWnd::GetParent();
	if(NULL != pWnd)
	{
		pWnd->ScreenToClient(&clientRect);
		pWnd->InvalidateRect(&clientRect);
		pWnd->UpdateWindow();
	}

	// Redraw focus.
	if(m_bFocusDrawn)
		ShowFocus();
}


void CPseudoHyperlink::SetLinkColors(COLORREF crLinkColor /* = -1 */, COLORREF crHoverColor /* = -1 */)
{ 
	if(-1 == crLinkColor)
		m_crLinkColor = RGB(0, 0, 238); // Blue
	else
	    m_crLinkColor = crLinkColor;

	if(-1 == crHoverColor)
		m_crHoverColor = ::GetSysColor(COLOR_HIGHLIGHT);
	else
		m_crHoverColor = crHoverColor;
	
    if(::IsWindow(m_hWnd))
        Invalidate(); 
}


COLORREF CPseudoHyperlink::GetLinkColor() const
{ 
    return m_crLinkColor; 
}


COLORREF CPseudoHyperlink::GetLinkHoverColor() const
{
    return m_crHoverColor;
}


void CPseudoHyperlink::SetLinkCursor(HCURSOR hCursor)
{ 
    m_hLinkCursor = hCursor;
    if(NULL == m_hLinkCursor)
        SetDefaultLinkCursor();
}


HCURSOR CPseudoHyperlink::GetLinkCursor() const
{
    return m_hLinkCursor;
}


void CPseudoHyperlink::SetUnderline(BOOL bUnderline /* = TRUE */)
{
    m_bUnderline = bUnderline;
	
    if(FALSE != ::IsWindow(m_hWnd))
    {
        LOGFONT logFont;
        if(0 != GetFont()->GetLogFont(&logFont))
		{
			logFont.lfUnderline = m_bUnderline;

			m_Font.DeleteObject();
			if(FALSE != m_Font.CreateFontIndirect(&logFont))
				SetFont(&m_Font);

			Invalidate();
		}
    }
}

BOOL CPseudoHyperlink::GetUnderline() const
{ 
    return m_bUnderline; 
}


void CPseudoHyperlink::SetAutoSize(bool bAutoSize /* = true */)
{
    m_bAutoSize = bAutoSize;
	
    if(FALSE != ::IsWindow(m_hWnd))
        ResizeAndPositionWindow();
}


bool CPseudoHyperlink::GetAutoSize() const
{ 
    return m_bAutoSize; 
}


void CPseudoHyperlink::SetLinkText(CString sLinkText)
{
    m_sLinkText = sLinkText;

	if(FALSE != ::IsWindow(m_hWnd))
	{
		SetWindowText(m_sLinkText);
        ResizeAndPositionWindow();
    }
}

CString CPseudoHyperlink::GetLinkText() const
{
    return m_sLinkText;
}


void CPseudoHyperlink::SetURL(CString sURL)
{
    m_sURL = sURL;
}

CString CPseudoHyperlink::GetURL() const
{ 
    return m_sURL;   
}


void CPseudoHyperlink::TurnOffLinkEffects()
{
	m_bLinkEffects = false;
	m_LinkType = LinkType_None;
	SetLinkColors(RGB(0, 0, 0), RGB(0, 0, 0));
	SetLinkCursor(LoadCursor(NULL, IDC_ARROW));

	m_bHoveringOverLink = false;
	m_bFocusDrawn = false;

	DWORD dwStyle = GetStyle();
	::SetWindowLongPtr(m_hWnd, GWL_STYLE, dwStyle & ~WS_TABSTOP);

	m_bBold = false;
	SetUnderline(FALSE);
}


void CPseudoHyperlink::SetLink(CString sDisplayText)
{
	m_LinkType = LinkType_None;
	SetLinkText(sDisplayText);
	TurnOffLinkEffects();
}


void CPseudoHyperlink::SetLink(CString sDisplayText, CString sURL)
{
	m_LinkType = LinkType_URL;
	SetLinkText(sDisplayText);
	SetURL(sURL);

	RedrawControlRect();
}


void CPseudoHyperlink::SetLink(CString sDisplayText,
							   fnPseudoHyperlinkCallback pfnCallBack,
							   void *pUserData, void *pParam)
{
	SetLinkText(sDisplayText);

	StoreLinkData(pfnCallBack, pUserData, pParam);
	m_LinkType = LinkType_Callback;

	RedrawControlRect();
}


void CPseudoHyperlink::StoreLinkData(fnPseudoHyperlinkCallback pfnCallBack,
									 void *pUserData, void *pParam)
{
	if(NULL != m_pLinkData)
	{
		delete m_pLinkData;
		m_pLinkData = NULL;
	}
	m_pLinkData = new LinkData;
	m_pLinkData->m_pfnCallback = pfnCallBack;
	m_pLinkData->m_pData = pUserData;
	m_pLinkData->m_pParam = pParam;
}


void CPseudoHyperlink::DoLinkAction()
{
	if((LinkType_URL == m_LinkType) && !m_sURL.IsEmpty())
	{
		::SendMessage(GetParent()->m_hWnd, WM_NEXTDLGCTL, (WPARAM)m_hWnd, TRUE);

		// Launch link in browser
		NAVToolbox::CStartBrowser browser;
		if(!browser.ShowURL(m_sURL))
		{
			MessageBeep(MB_ICONEXCLAMATION);     // Error
			CCTRACEE("%s - CStartBrowser::ShowURL failed", __FUNCTION__);
		}
	}
	else if(LinkType_Callback == m_LinkType)
	{
		::SendMessage(GetParent()->m_hWnd, WM_NEXTDLGCTL, (WPARAM)m_hWnd, TRUE);

		m_pLinkData->m_pfnCallback(m_pLinkData->m_pData,m_pLinkData->m_pParam);
	}

	// else LinkType is LinkType_None, so do nothing
}
