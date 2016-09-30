// MainFrm.cpp : implementation of the CMainFrame class
//

#include "stdafx.h"
#include "SymCorpUI.h"
#include "MainFrm.h"


// CMainFrame
IMPLEMENT_DYNCREATE(CMainFrame, CFrameWnd)

BEGIN_MESSAGE_MAP(CMainFrame, CFrameWnd)
	ON_WM_CREATE()
    ON_WM_CLOSE()
END_MESSAGE_MAP()


// CMainFrame construction/destruction
CMainFrame::CMainFrame()
{
    // Nothing for now
}

CMainFrame::~CMainFrame()
{
}

CHelpBar* CMainFrame::GetHelpBar()
{
    return &helpBar;
}

int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
    int xBorderWidth = 0;
    int yBorderWidth = 0;
    int captionHeight = 0;

    // Create controls
    lpCreateStruct->dwExStyle &= ~WS_EX_WINDOWEDGE;
	if (CFrameWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

    // Create/dock the toolbars
    EnableDocking(CBRS_ALIGN_LEFT | CBRS_ALIGN_TOP);
    navBar.Create(this, IDD_NAVBAR, CBRS_LEFT|CBRS_TOOLTIPS|CBRS_FLYBY, 2000);
    helpBar.Create(this, IDD_HELPBAR, CBRS_TOP | CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_FIXED, 2001);

    // Resize to width of NavBar + HelpBar and height of NavBar
    // Note sizes here include the non-client area
    xBorderWidth = GetSystemMetrics(SM_CXDLGFRAME);
    yBorderWidth = GetSystemMetrics(SM_CYDLGFRAME);
    captionHeight = GetSystemMetrics(SM_CYCAPTION);
    MoveWindow(0, 0, navBar.m_sizeDefault.cx + helpBar.m_sizeDefault.cx + 2*xBorderWidth, navBar.m_sizeDefault.cy + 2*yBorderWidth + captionHeight);
    CenterWindow(GetDesktopWindow());

    return ERROR_SUCCESS;
}

BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
    BOOL returnValBOOL = FALSE;
    // Remove the menu
    if(cs.hMenu!=NULL)
    {
        ::DestroyMenu(cs.hMenu);      // delete menu if loaded
        cs.hMenu = NULL;              // no menu for this window
    }
    // Make window non-sizeable and disable App Name - filename style caption
    // Actual app caption is in 1st substring of DocTemplate registration (IDR_MAINFRAME)
    cs.style &= ~(WS_MAXIMIZEBOX|WS_THICKFRAME|WS_SIZEBOX);
    returnValBOOL = CFrameWnd::PreCreateWindow(cs);
    // Must be done after CFrameWnd::PreCreateWindow, as adding WS_EX_CLIENTEDGE is hardcoded there
    cs.dwExStyle &= ~(WS_EX_CLIENTEDGE);
    return returnValBOOL;
}

// CMainFrame message handlers
void CMainFrame::OnClose()
{
    // Special handling since this is an SDI app residing in a DLL - MFC's
    // default CFrameWnd::OnClose will GPF due to using the document after
    // it's been deleted

    // Document object is a filler, just for MFC to be happy
	CWinApp* pApp = AfxGetApp();
	// hide the application's windows before closing all the documents
	pApp->HideApplication();

    // Post a message to quit the application when we're done with all this
	AfxPostQuitMessage(0);

    // Close all documents.  This will also destroy this object, as a result of
    // the document destroying all attached views, which in turn delete frames
	pApp->CloseAllDocuments(FALSE);
}

void CMainFrame::OnUpdateFrameTitle( BOOL addToTitle )
{
    CString windowCaption;

    windowCaption.LoadString(AFX_IDS_APP_TITLE);
    SetWindowText(windowCaption);
}


// CMainFrame diagnostics
#ifdef _DEBUG
    void CMainFrame::AssertValid() const
    {
	    CFrameWnd::AssertValid();
    }

    void CMainFrame::Dump(CDumpContext& dc) const
    {
	    CFrameWnd::Dump(dc);
    }
#endif //_DEBUG
