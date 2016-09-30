// MainFrm.cpp : implementation of the CMainFrame class
//

#include "stdafx.h"
#include "AVISDFRL.h"

#include "MainFrm.h"
#include "DFMsg.h"
#include "AVISDFRLView.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CMainFrame

IMPLEMENT_DYNCREATE(CMainFrame, CFrameWnd)

BEGIN_MESSAGE_MAP(CMainFrame, CFrameWnd)
	//{{AFX_MSG_MAP(CMainFrame)
	ON_WM_CREATE()
	ON_WM_CLOSE()
	//}}AFX_MSG_MAP
	// Global help commands
	ON_COMMAND(ID_HELP_FINDER, CFrameWnd::OnHelpFinder)
	ON_COMMAND(ID_HELP, CFrameWnd::OnHelp)
	ON_COMMAND(ID_CONTEXT_HELP, CFrameWnd::OnContextHelp)
	ON_COMMAND(ID_DEFAULT_HELP, CFrameWnd::OnHelpFinder)
    ON_MESSAGE(ALM_REQ_START_JOB, OnALMReqStartJob)
    ON_MESSAGE(ALM_REQ_IGNORE_JOB, OnALMReqIgnoreJob)
    ON_MESSAGE(ALM_REQ_RELOAD_RESOURCES, OnALMReqReloadResources)
    ON_MESSAGE(DFLAUNCHER_JOB_COMPLETE, OnDFLauncherJobComplete)
    ON_MESSAGE(DFLAUNCHER_DISABLE_MACHINE, OnDFLauncherDisableMachine)
END_MESSAGE_MAP()

static UINT indicators[] =
{
	ID_SEPARATOR,           // status line indicator
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
	
    CDFLauncherApp * theApp = (CDFLauncherApp *) AfxGetApp();
    HICON hMainFrameIcon;

    // You have to explicitely load the icon or else the system displays
    // a default windows icon
    hMainFrameIcon = theApp->LoadIcon(IDR_MAINFRAME);
    if (hMainFrameIcon)
    {
        SetIcon(hMainFrameIcon, TRUE);
    }
    // In debug mode the display of tooltips raises an assert which can be
    // ignored but it becomes annoying while testing so enable the diaplay
    // of tooltips only in the release mode.

	if (!m_wndToolBar.CreateEx(this, TBSTYLE_FLAT, WS_CHILD | WS_VISIBLE | CBRS_TOP |
#ifndef _DEBUG
        CBRS_TOOLTIPS | CBRS_FLYBY |
#endif
        CBRS_GRIPPER | CBRS_SIZE_DYNAMIC) ||
		!m_wndToolBar.LoadToolBar(IDR_MAINFRAME))
	{
		TRACE0("Failed to create toolbar\n");
		return -1;      // fail to create
	}

	if (!m_wndStatusBar.Create(this) ||
		!m_wndStatusBar.SetIndicators(indicators,
		  sizeof(indicators)/sizeof(UINT)))
	{
		TRACE0("Failed to create status bar\n");
		return -1;      // fail to create
	}

	int statIndex;
	CString statStr;

	statIndex = m_wndStatusBar.CommandToIndex(IDS_INDICATOR_UPTIME);
	if (statIndex != -1)
	{
		statStr = _T("0-00:00:00");
		m_wndStatusBar.SetPaneText(statIndex, statStr);
	}
    
	statIndex = m_wndStatusBar.CommandToIndex(IDS_INDICATOR_COUNTS);
	if (statIndex != -1)
	{
		statStr = _T("0/0/0 0 0");
		m_wndStatusBar.SetPaneText(statIndex, statStr);
	}
    
	m_wndToolBar.EnableDocking(CBRS_ALIGN_ANY);
	EnableDocking(CBRS_ALIGN_ANY);
	DockControlBar(&m_wndToolBar);

    CDFLauncherView * pDFLauncherView = new (CDFLauncherView);

	if (!pDFLauncherView ||
        !pDFLauncherView->Create(NULL, NULL, AFX_WS_DEFAULT_VIEW,
		    CRect(0, 0, 0, 0), this, AFX_IDW_PANE_FIRST, NULL))
	{
		TRACE0("Failed to create view window\n");
		return -1;
	}

    SetActiveView(pDFLauncherView);
	
	return 0;
}

BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
    if( !CFrameWnd::PreCreateWindow(cs) )
		return FALSE;

	cs.dwExStyle &= ~WS_EX_CLIENTEDGE;
	cs.lpszClass = AfxRegisterWndClass(0);
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

void CMainFrame::OnClose() 
{
    // On selecting the close window only hide the window and make the main
    // application window active. The application can be terminated only from
    // the main app window. The resources window should always be present to
    // receive and process the messages.

    ShowWindow(SW_HIDE);
    CDFLauncherApp * theApp = (CDFLauncherApp *) AfxGetApp();
    CWnd * pALMWnd = CWnd::FromHandle(theApp->m_hALMWnd);

    pALMWnd->ShowWindow(SW_NORMAL);
    pALMWnd->SetForegroundWindow();
}

// Route the messages to the view class.

LONG CMainFrame::OnALMReqStartJob(WPARAM w, LPARAM l)
{
    CDFLauncherView * pDFLauncherView = 
        (CDFLauncherView *) GetActiveView();

    if (pDFLauncherView)
        return pDFLauncherView->OnALMReqStartJob(w, l);
    else
        return 0;
}

LONG CMainFrame::OnALMReqIgnoreJob(WPARAM w, LPARAM l)
{
    CDFLauncherView * pDFLauncherView = 
        (CDFLauncherView *) GetActiveView();

    if (pDFLauncherView)
        return pDFLauncherView->OnALMReqIgnoreJob(w, l);
    else
        return 0;
}

LONG CMainFrame::OnALMReqReloadResources(WPARAM w, LPARAM l)
{
    CDFLauncherView * pDFLauncherView = 
        (CDFLauncherView *) GetActiveView();

    if (pDFLauncherView)
        return pDFLauncherView->OnALMReqReloadResources(w, l);
    else
        return 0;
}

LONG CMainFrame::OnDFLauncherJobComplete(WPARAM w, LPARAM l)
{
    CDFLauncherView * pDFLauncherView = 
        (CDFLauncherView *) GetActiveView();

    if (pDFLauncherView)
        return pDFLauncherView->OnDFLauncherJobComplete(w, l);
    else
        return 0;
}

LONG CMainFrame::OnDFLauncherDisableMachine(WPARAM w, LPARAM l)
{
    CDFLauncherView * pDFLauncherView = 
        (CDFLauncherView *) GetActiveView();

    if (pDFLauncherView)
        return pDFLauncherView->OnDFLauncherDisableMachine(w, l);
    else
        return 0;
}
