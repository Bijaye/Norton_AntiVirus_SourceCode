// MainFrm.cpp : implementation of the CMainFrame class
//

#include "stdafx.h"
#include "DFGui.h"
#include "afxtempl.h"
#include "afxwin.h"
#include <iostream>
#include <fstream>
#include "dfpackthread.h"
#include "resource.h"
#include "dfevalstatus.h"
#include "dfevalstate.h"
#include "dfmsg.h" 
#include "DFJob.h"
#include "DFSample.h"
#include "dferror.h"  
#include "dfmatrix.h"
#include "dfdirinfo.h"
#include "DFManager.h"
#include "avisdfrlimp.h"
#include "listvwex.h"
#include "DFGui.h"
#include "DFGuiDoc.h"
#include "DFGuiView.h"
#include "MainFrm.h"
#include "dfdbrequests.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
//SQLHENV phenv;
//SQLHDBC phdbc;

/////////////////////////////////////////////////////////////////////////////
// CMainFrame

//JALAN: Add OnClose Handler and Call TermDFLauncher before the 
// window is actually closed.

IMPLEMENT_DYNCREATE(CMainFrame, CFrameWnd)

BEGIN_MESSAGE_MAP(CMainFrame, CFrameWnd)
	//{{AFX_MSG_MAP(CMainFrame)
	ON_WM_CREATE()
	ON_WM_CLOSE()
	ON_UPDATE_COMMAND_UI(ID_RESUMEALL, OnUpdateResumeall)
	ON_WM_HELPINFO()
	ON_COMMAND(ID_RESUMEALL, OnResumeall)
	//}}AFX_MSG_MAP
	ON_MESSAGE(DFLAUNCHER_POST_JOB_STATUS, OnJobFromLauncher)
	ON_MESSAGE(THREAD_ID, OnAfterThread)
	ON_MESSAGE(MSG_REQUEST_CLOSE, OnClose2)

END_MESSAGE_MAP()

static UINT indicators[] =
{
	ID_SEPARATOR,           // status line indicator
    IDS_INDICATOR_DATAFLOWSTATUS,
	IDS_SCHED_TASKS_IND1,
    IDS_INDICATOR_SAMPLECOUNT,

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
	
	if (!m_wndToolBar.Create(this) ||
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
    int paneIndex;
		
 	paneIndex = m_wndStatusBar.CommandToIndex(IDS_INDICATOR_DATAFLOWSTATUS);
	if (paneIndex != -1)
	{
		
		CString paneString((LPCSTR)IDS_STATUS_ENABLE);
		m_wndStatusBar.SetPaneText(paneIndex, paneString);
	}
 	paneIndex = m_wndStatusBar.CommandToIndex(IDS_SCHED_TASKS_IND1);
	if (paneIndex != -1)
	{
		
		CString str((LPCSTR)IDS_SCHED_TASKS_IND);
		CString paneString = str + CString(" I/I/I/I");
		m_wndStatusBar.SetPaneText(paneIndex, paneString);
	}
	
 	paneIndex = m_wndStatusBar.CommandToIndex(IDS_INDICATOR_SAMPLECOUNT);
	if (paneIndex != -1)
	{
		
		CString str((LPCSTR)IDS_INDICATOR_SAMPLE);
		CString paneString = str + _T("0/0/0");
		m_wndStatusBar.SetPaneText(paneIndex, paneString);
	}   
    

	// TODO: Remove this if you don't want tool tips or a resizeable toolbar
	m_wndToolBar.SetBarStyle(m_wndToolBar.GetBarStyle() |
		CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC);

	// TODO: Delete these three lines if you don't want the toolbar to
	//  be dockable
	m_wndToolBar.EnableDocking(CBRS_ALIGN_ANY);
	EnableDocking(CBRS_ALIGN_ANY);
	DockControlBar(&m_wndToolBar);
	return 0;
}

BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

    // Allow single instance of dfgui.exe

    CDFGuiApp *app = (CDFGuiApp *)AfxGetApp();
    cs.lpszClass = app->m_singleInstance.GetClassName();
	return CFrameWnd::PreCreateWindow(cs);
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

BOOL CMainFrame::PreTranslateMessage(MSG* pMsg) 
{
    BOOL rc;
	// TODO: Add your specialized code here and/or call the base class
	
	rc = CFrameWnd::PreTranslateMessage(pMsg);

#ifdef LAUNCHER
    if (!rc)
    {
        CDFGuiApp * testALMApp = (CDFGuiApp *) AfxGetApp();

	    if (testALMApp && testALMApp->DFLauncherWnd)
        {
// Return the value as returned by 	FilterDFLauncherMessage

            rc = FilterDFLauncherMessage(pMsg);
        }
    }

#endif
    return (rc);
}
/*----------------------------------------------------------------------------*/
/* Procedure name:      OnJobFromLauncher                                     */
/* Description:         On job acceptence                                     */
/*                                                                            */
/* Input:                                                                     */
/*                                                                            */
/* Output:              None                                                  */
/* Return:                                                                    */
/* Global References:   None                                                  */
/* Notes:                                                                     */
/*                                                                            */
/*                                                                            */
/*                                                                            */
/*                                                                            */
/*----------------------------------------------------------------------------*/
/* History:                                                                   */
/*----------------------------------------------------------------------------*/
int  CMainFrame::OnJobFromLauncher(WPARAM p1, LPARAM p2)
{
	DFJob *pJob = (DFJob*) p2;
    CDFGuiApp * pApp = (CDFGuiApp *) AfxGetApp();
	CDFGuiView *view =(CDFGuiView*) pApp->DFGuiWnd; 
	view->SendMessage(MSG_TRANSIT, 0, p2); 

	return 0;

}
    

void CMainFrame::OnClose() 
{
	// TODO: Add your message handler code here and/or call default
	CString title((LPCSTR) (IDS_AVIS_TITLE)); 
	CString message;
	CString message2 = CString((LPCSTR) (IDS_CONFIRM_MSG2)); 
	int rc = MessageBox (message2, title, MB_YESNO | MB_ICONQUESTION | MB_DEFBUTTON2);
	if (rc != IDYES)
        return;
    CDFGuiApp * pApp = (CDFGuiApp *) AfxGetApp();
	CDFGuiView *view =(CDFGuiView*) pApp->DFGuiWnd; 
	view->SendMessage(MSG_IMMCLOSE, 0, 0); 

//#ifndef DATABASE_INTERFACE
//	int retcode = DFDBRequests::DbClose();
//#endif     

//#ifdef LAUNCHER
//    TermDFLauncher();	
//#endif
//
//	CFrameWnd::OnClose();
}
void CMainFrame::OnClose2() 
{
#ifdef LAUNCHER
    TermDFLauncher();	
#endif
	CFrameWnd::OnClose();
}
void CMainFrame::OnResumeall() 
{
	// TODO: Add your command handler code here
	
}

void CMainFrame::OnUpdateResumeall(CCmdUI* pCmdUI) 
{
	// TODO: Add your command update UI handler code here
	
}

BOOL CMainFrame::OnHelpInfo(HELPINFO* pHelpInfo) 
{
	// TODO: Add your message handler code here and/or call default
	
	return CFrameWnd::OnHelpInfo(pHelpInfo);
}

int  CMainFrame::OnAfterThread(WPARAM p1, LPARAM p2)
{
	DFSample *pSample = (DFSample*) p1;
    CDFGuiApp * pApp = (CDFGuiApp *) AfxGetApp();
	CDFGuiView *view =(CDFGuiView*) pApp->DFGuiWnd; 
	view->SendMessage(MSG_TRANSIT1, p1, 0); 
	return 0;

}
