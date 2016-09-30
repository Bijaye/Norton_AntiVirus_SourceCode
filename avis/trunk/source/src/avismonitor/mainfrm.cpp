// MainFrm.cpp : implementation of the CMainFrame class
//

#include "stdafx.h"
#include "AVISMonitor.h"

#include "MainFrm.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CMainFrame

IMPLEMENT_DYNAMIC(CMainFrame, CFrameWnd)

BEGIN_MESSAGE_MAP(CMainFrame, CFrameWnd)
	//{{AFX_MSG_MAP(CMainFrame)
	ON_WM_CREATE()
	ON_WM_SETFOCUS()
	ON_WM_CLOSE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

static UINT indicators[] =
{
	ID_SEPARATOR,           // status line indicator
    IDS_INDICATOR_NEXT_CHECK_TIME,
    IDS_INDICATOR_UPTIME,
    IDS_INDICATOR_COUNTS,
};

/////////////////////////////////////////////////////////////////////////////
// CMainFrame construction/destruction

CMainFrame::CMainFrame()
{
	// TODO: add member initialization code here
	
}

CMainFrame::~CMainFrame()
{
}

int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CFrameWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

    // create a view to occupy the client area of the frame
    m_pWndView = new (CAVISMonitorView);

	if (!m_pWndView->Create(NULL, NULL, AFX_WS_DEFAULT_VIEW,
		CRect(0, 0, 0, 0), this, AFX_IDW_PANE_FIRST, NULL))
	{
		TRACE0("Failed to create view window\n");
		return -1;
	}

    SetActiveView(m_pWndView);
	
	if (!m_wndToolBar.CreateEx(this, TBSTYLE_FLAT, WS_CHILD | WS_VISIBLE | CBRS_TOP |
        CBRS_TOOLTIPS | CBRS_FLYBY |
        CBRS_GRIPPER | CBRS_SIZE_DYNAMIC) ||
		!m_wndToolBar.LoadToolBar(IDR_MAINFRAME))
	{
		TRACE0("Failed to create toolbar\n");
		return -1;      // fail to create
	}
    /*
	if (!m_wndDlgBar.Create(this, IDR_MAINFRAME, 
		CBRS_ALIGN_TOP, AFX_IDW_DIALOGBAR))
	{
		TRACE0("Failed to create dialogbar\n");
		return -1;		// fail to create
	}

	if (!m_wndReBar.Create(this) ||
		!m_wndReBar.AddBar(&m_wndToolBar) ||
		!m_wndReBar.AddBar(&m_wndDlgBar))
	{
		TRACE0("Failed to create rebar\n");
		return -1;      // fail to create
	}
    */

	if (!m_wndStatusBar.Create(this) ||
		!m_wndStatusBar.SetIndicators(indicators,
		  sizeof(indicators)/sizeof(UINT)))
	{
		TRACE0("Failed to create status bar\n");
		return -1;      // fail to create
	}

	int statIndex;
	CString statStr;

    // Create status bar indicators to show next check time, uptime and counts
	statIndex = m_wndStatusBar.CommandToIndex(IDS_INDICATOR_NEXT_CHECK_TIME);
	if (statIndex != -1)
	{
		statStr = _T("Check After 00:00:00");
		m_wndStatusBar.SetPaneText(statIndex, statStr);
	}
    
	statIndex = m_wndStatusBar.CommandToIndex(IDS_INDICATOR_UPTIME);
	if (statIndex != -1)
	{
		statStr = _T("System Up Time 0-00:00:00");
		m_wndStatusBar.SetPaneText(statIndex, statStr);
	}
    
	statIndex = m_wndStatusBar.CommandToIndex(IDS_INDICATOR_COUNTS);
	if (statIndex != -1)
	{
        /*
		statStr = _T("Total 0, Active 0, Error 0");
        */
		statStr = _T("Total 0, Active 0");
		m_wndStatusBar.SetPaneText(statIndex, statStr);
	}
    
    /*
	// TODO: Remove this if you don't want tool tips
	m_wndToolBar.SetBarStyle(m_wndToolBar.GetBarStyle() |
		CBRS_TOOLTIPS | CBRS_FLYBY);
    */

	m_wndToolBar.EnableDocking(CBRS_ALIGN_ANY);
	EnableDocking(CBRS_ALIGN_ANY);
	DockControlBar(&m_wndToolBar);

	return 0;
}

BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
    CAVISMonitorApp *app = (CAVISMonitorApp *)AfxGetApp();
    cs.lpszClass = app->m_singleInstance.GetClassName();

	if( !CFrameWnd::PreCreateWindow(cs) )
		return FALSE;
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs
	cs.dwExStyle &= ~WS_EX_CLIENTEDGE;

	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
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

/////////////////////////////////////////////////////////////////////////////
// CMainFrame message handlers
void CMainFrame::OnSetFocus(CWnd* /*pOldWnd*/)
{
	// forward focus to the view window
	m_pWndView->SetFocus();
}

BOOL CMainFrame::OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo)
{
	// let the view have first crack at the command
	//if (m_pWndView->OnCmdMsg(nID, nCode, pExtra, pHandlerInfo))
	//	return TRUE;

	// otherwise, do default handling
	return CFrameWnd::OnCmdMsg(nID, nCode, pExtra, pHandlerInfo);
}


void CMainFrame::OnClose() 
{
	// TODO: Add your message handler code here and/or call default
    int rc;

    // Display a message and get the confirmation from the user before closing the application
    rc = AfxMessageBox ("This action will terminate all ongoing modules and exit the application.\n\nAre you sure ?", MB_YESNO | MB_ICONQUESTION | MB_DEFBUTTON2);

    if (rc == IDYES)
    {
        m_pWndView->TerminateAllOngoingProcess();
	    
	    CFrameWnd::OnClose();
    }
}
