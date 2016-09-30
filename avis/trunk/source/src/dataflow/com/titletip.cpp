// TitleTip.cpp : implementation file
//

#include "stdafx.h"
#include "TitleTip.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CTitleTip

CTitleTip::CTitleTip()
{
        // Register the window class if it has not already been registered.
        WNDCLASS wndcls;
        HINSTANCE hInst = AfxGetInstanceHandle();
        if(!(::GetClassInfo(hInst, TITLETIP_CLASSNAME, &wndcls)))
        {
                // otherwise we need to register a new class
                wndcls.style = CS_SAVEBITS ;
                wndcls.lpfnWndProc = ::DefWindowProc;
                wndcls.cbClsExtra = wndcls.cbWndExtra = 0;
                wndcls.hInstance = hInst;
                wndcls.hIcon = NULL;
                wndcls.hCursor = LoadCursor( hInst, IDC_ARROW );
                wndcls.hbrBackground = (HBRUSH)(COLOR_INFOBK + 1); 
                wndcls.lpszMenuName = NULL;
                wndcls.lpszClassName = TITLETIP_CLASSNAME;
                if (!AfxRegisterClass(&wndcls))
                        AfxThrowResourceException();
        }
		m_created = FALSE;
}

CTitleTip::~CTitleTip()
{
}


BEGIN_MESSAGE_MAP(CTitleTip, CWnd)
	//{{AFX_MSG_MAP(CTitleTip)
	ON_WM_MOUSEMOVE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CTitleTip message handlers

BOOL CTitleTip::PreTranslateMessage(MSG* pMsg) 
{
        CWnd *pWnd;
        int hittest ;

        switch( pMsg->message )
        {
        case WM_LBUTTONDOWN:
        case WM_RBUTTONDOWN:
        case WM_MBUTTONDOWN:
		case WM_LBUTTONDBLCLK:
                POINTS pts = MAKEPOINTS( pMsg->lParam );
                POINT  point;
                point.x = pts.x;
                point.y = pts.y;
                ClientToScreen( &point );
                pWnd = WindowFromPoint( point );
                if( pWnd == this ) 
                        pWnd = m_pParentWnd;

                hittest = (int)pWnd->SendMessage(WM_NCHITTEST,
                                        0,MAKELONG(point.x,point.y));
                if (hittest == HTCLIENT) {
                        pWnd->ScreenToClient( &point );
                        pMsg->lParam = MAKELONG(point.x,point.y);
                } else {
                        switch (pMsg->message) {
                        case WM_LBUTTONDOWN: 
                                pMsg->message = WM_NCLBUTTONDOWN;
                                break;
                        case WM_RBUTTONDOWN: 
                                pMsg->message = WM_NCRBUTTONDOWN;
                                break;
                        case WM_MBUTTONDOWN: 
                                pMsg->message = WM_NCMBUTTONDOWN;
                                break;
                        case WM_LBUTTONDBLCLK: 
                                pMsg->message = WM_NCLBUTTONDBLCLK;
                                break;
                        }
                        pMsg->wParam = hittest;
                        pMsg->lParam = MAKELONG(point.x,point.y);
                }
                ReleaseCapture();
                ShowWindow( SW_HIDE );
                pWnd->PostMessage(pMsg->message,pMsg->wParam,pMsg->lParam);
                return TRUE;            
        case WM_KEYDOWN:
        case WM_SYSKEYDOWN:
                ReleaseCapture();
                ShowWindow( SW_HIDE );
                m_pParentWnd->PostMessage( pMsg->message, pMsg->wParam, pMsg->lParam );
                return TRUE;
        }

        if( GetFocus() == NULL )
        {
                ReleaseCapture();
                ShowWindow( SW_HIDE );
				m_oldRectTitle.SetRectEmpty();
                return TRUE;
        }

        return CWnd::PreTranslateMessage(pMsg);
}

BOOL CTitleTip::Create(CWnd* pParentWnd) 
{
	if (m_created == FALSE)
	{
        ASSERT_VALID(pParentWnd);

        DWORD dwStyle = WS_BORDER | WS_POPUP; 
        DWORD dwExStyle = WS_EX_TOOLWINDOW | WS_EX_TOPMOST;
        m_pParentWnd = pParentWnd;
		m_created = TRUE;
        return CreateEx( dwExStyle, TITLETIP_CLASSNAME, NULL, dwStyle, 0, 0, 0, 0, 
                NULL, NULL, NULL );
	}
	else
		return (FALSE);
}

void CTitleTip::OnMouseMove(UINT nFlags, CPoint point) 
{
         if (!m_rectTitle.PtInRect(point)) {
                  ReleaseCapture();
                  ShowWindow( SW_HIDE );
				  m_oldRectTitle.SetRectEmpty();

                  // Forward the message
                  ClientToScreen( &point );
                  CWnd *pWnd = WindowFromPoint( point );
                  if ( pWnd == this ) 
                          pWnd = m_pParentWnd;
                  int hittest = (int)pWnd->SendMessage(WM_NCHITTEST,
                                                0,MAKELONG(point.x,point.y));
                  if (hittest == HTCLIENT) {
                           pWnd->ScreenToClient( &point );
                           pWnd->PostMessage( WM_MOUSEMOVE, nFlags, 
                                                MAKELONG(point.x,point.y) );
                  } else {
                           pWnd->PostMessage( WM_NCMOUSEMOVE, hittest, 
                                                MAKELONG(point.x,point.y) );
                  }
         }
}

void CTitleTip::Hide ()
{
	if (GetCapture() == this)
		ReleaseCapture();
	if (m_created == TRUE)
    	ShowWindow( SW_HIDE );
}

// Show                 - Show the titletip if needed
// rectTitle            - The rectangle within which the original 
//                        title is constrained - in client coordinates
// lpszTitleText        - The text to be displayed
// xoffset              - Number of pixel that the text is offset from
//                        left border of the cell
void CTitleTip::Show (CRect rectTitle, LPCTSTR lpszTitleText, int xoffset)
{
        ASSERT( ::IsWindow( m_hWnd ) );
        ASSERT( !rectTitle.IsRectEmpty() );

        // If titletip is already displayed, don't do anything.
        if( IsWindowVisible() ) 
                return;

        // Do not display the titletip is app does not have focus
        if( GetFocus() == NULL )
                return;

		// Do not display if the app has called Hide and mouse reenters the same window
		if (rectTitle == m_oldRectTitle)
			return;

		m_oldRectTitle = rectTitle;

        // Define the rectangle outside which the titletip will be hidden.
        // We add a buffer of one pixel around the rectangle
        m_rectTitle.top = -1;
        m_rectTitle.left = -xoffset-1;
        m_rectTitle.right = rectTitle.Width()-xoffset;
        m_rectTitle.bottom = rectTitle.Height()+1;

        // Determine the width of the text
        m_pParentWnd->ClientToScreen( rectTitle );

        CClientDC dc(this);
        CString strTitle(lpszTitleText);
        CFont *pFont = m_pParentWnd->GetFont();         // use same font as ctrl
        CFont *pFontDC = dc.SelectObject( pFont );

        CRect rectDisplay = rectTitle;
        CSize size = dc.GetTextExtent( strTitle );
        rectDisplay.left += xoffset;
        rectDisplay.right = rectDisplay.left + size.cx + 3;

        // Do not display if the text fits within available space
        if( rectDisplay.right <= rectTitle.right-xoffset )
                return;

        // Show the titletip
        SetWindowPos( &wndTop, rectDisplay.left, rectDisplay.top, 
                        rectDisplay.Width(), rectDisplay.Height(), 
                        SWP_SHOWWINDOW|SWP_NOACTIVATE );

        dc.SetBkMode( TRANSPARENT );
        dc.TextOut( 0, 0, strTitle );
        dc.SelectObject( pFontDC );

        SetCapture();
}
