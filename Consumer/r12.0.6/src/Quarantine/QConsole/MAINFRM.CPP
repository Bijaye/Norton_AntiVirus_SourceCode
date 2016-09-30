//////////////////////////////////////////////////////////////////////////////
// PROPRIETARY / CONFIDENTIAL.
// Use of this product is subject to license terms.
// Copyright © 2005 Symantec Corporation.
// All rights reserved.
//////////////////////////////////////////////////////////////////////////////
//
// MainFrm.cpp : implementation of the CMainFrame class
//
//////////////////////////////////////////////////////////////////////////////
// $Log:   S:/QCONSOLE/VCS/MainFrm.cpv  $
// 
//    Rev 1.22   03 Feb 1999 19:22:26   mdunn
// Tweaked toolbar button and rebar size calculations.  Added in a hack to get
// the toolbar to repaint so the buttons show up right (on Garret's machine).
// 
//    Rev 1.21   29 Jan 1999 19:21:44   mdunn
// Fixed value of BITMAP_HEIGHT.
// Now explicitly setting the height of the toolbar buttons so we can see
// the text again!
// 
//    Rev 1.20   11 Jun 1998 15:48:26   DBuches
// Added support for trial ware.
// 
//    Rev 1.19   04 Jun 1998 15:37:24   DBuches
// Changed to work with GMT times.
// 
//    Rev 1.18   26 May 1998 18:03:56   DBuches
// Fixed problem responding to the Arrange Items by... menu items
// 
//    Rev 1.17   22 May 1998 13:00:16   DBuches
// Don't delete un-deletable virus infected files.
// 
//    Rev 1.16   13 May 1998 15:55:14   DBuches
// Fixed total number of toolbar bitmaps being loaded by the menu manager.
// 
//    Rev 1.15   13 May 1998 14:46:32   DBuches
// Fixed total number of toolbar bitmaps being loaded by the menu manager.
// 
//    Rev 1.14   11 May 1998 08:24:28   DBuches
// Added new artwork.
// 
//    Rev 1.13   07 May 1998 18:18:24   DBuches
// Added code to remove LiveUpdate buttun when LiveUpdate is not present on the system.
// 
//    Rev 1.12   04 May 1998 20:49:40   DBuches
// Fixed divide by zero when exporting.
// 
//    Rev 1.11   18 Apr 1998 23:22:08   JTaylor
// Added another argument to AddFileToQuarantine.
// 
//    Rev 1.10   14 Apr 1998 00:43:10   DBuches
// Hooked up export dialog.
// 
//    Rev 1.9   06 Apr 1998 19:42:42   JTaylor
// Removed the DWORD arguments to READ/WRITE OptionsFile.
// 
//    Rev 1.8   03 Apr 1998 13:28:12   DBuches
// Added handlers for options and LiveUpdate.
// 
//    Rev 1.7   13 Mar 1998 15:39:56   DBuches
// Removed refresh button from toolbar.
// 
//    Rev 1.6   13 Mar 1998 15:20:50   DBuches
// Added text to toolbar.
// 
//    Rev 1.5   11 Mar 1998 15:14:00   DBuches
// Added delete code to Add menu item handler.
// 
//    Rev 1.4   06 Mar 1998 21:09:20   DBuches
// Added call to CMoveFileDlg.
// 
//    Rev 1.3   06 Mar 1998 17:17:42   DBuches
// Removed dead code.
// 
//    Rev 1.2   06 Mar 1998 11:22:12   DBuches
// Added coolbars.
// 
//    Rev 1.1   03 Mar 1998 17:03:58   DBuches
// Added handler for quarantine mode.
// 
//    Rev 1.0   27 Feb 1998 15:08:58   DBuches
// Initial revision.
//////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"

#define INITIIDS
#include "navtrust.h"
#include "defutilsinterface.h"
#include "ccSettingsInterface.h"
#include "AvEvents.h"

#include "platform.h"
#include "qconsole.h"
#include "qconsoleDoc.h"
#include "MainFrm.h"
#include "QTreeView.h"
#include "qconsoleView.h"
#include "const.h"
#include "MoveFileDlg.h"
#include "optionssheet.h"
#include "exportdlg.h"
#include "RangeStr.h"
#include "ProgDlg.h"
#include "xapi.h"
#include "shfolder.h"
#include "AVRESBranding.h"

#include "AVccModuleId.h"
#include "NAVError.h"

#include "QItemEZ.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

static BOOL IsLiveUpdateInstalled();


//////////////////////////////////////////////////////////////////////////////
// CSplitterWndEx


//////////////////////////////////////////////////////////////////////////////
// 2/25/98 DBUCHES - Function created / Header added.
//////////////////////////////////////////////////////////////////////////////
CSplitterWndEx::CSplitterWndEx() : m_nHiddenCol(-1),m_iSavedSize(-1)
{
}


//////////////////////////////////////////////////////////////////////////////
// Shows the hidden column
// 2/25/98 DBUCHES - Function created / Header added.
//////////////////////////////////////////////////////////////////////////////
void CSplitterWndEx::ShowColumn()
{
	ASSERT_VALID(this);
	ASSERT(m_nCols < m_nMaxCols);
	ASSERT(m_nHiddenCol != -1);

	int colNew = m_nHiddenCol;
	m_nHiddenCol = -1;
	int cxNew;
	if(m_iSavedSize == -1)
		cxNew = m_pColInfo[m_nCols].nCurSize;
	else
	{
		cxNew = m_iSavedSize;
		m_iSavedSize = -1;
	}

	m_nCols++;

	ASSERT(m_nCols == m_nMaxCols);
	int col;
	for(int row = 0; row < m_nRows; row++)
	{
		CWnd* pPaneShow = GetDlgItem(AFX_IDW_PANE_FIRST + row * 16 + m_nCols);
		ASSERT(pPaneShow != NULL);
		pPaneShow->ShowWindow(SW_SHOWNA);
		for(col = m_nCols - 2; col >= colNew; col--)
		{
			CWnd* pPane = GetPane(row, col);
			ASSERT(pPane != NULL);
			pPane->SetDlgCtrlID(IdFromRowCol(row, col + 1));
		}

		pPaneShow->SetDlgCtrlID(IdFromRowCol(row, colNew));
	}
	// new panes have been created -- recalculate layout
	for(col = colNew + 1; col < m_nCols; col++)
		m_pColInfo[col].nIdealSize = m_pColInfo[col - 1].nCurSize;

	m_pColInfo[colNew].nIdealSize = cxNew;
	RecalcLayout();
}


//////////////////////////////////////////////////////////////////////////////
// Hides a column.
// Argument: int colHide - column to hide.
// 2/25/98 DBUCHES - Function created / Header added.
//////////////////////////////////////////////////////////////////////////////
void CSplitterWndEx::HideColumn(int colHide)
{
	ASSERT_VALID(this);
	ASSERT(m_nCols > 1);
	ASSERT(colHide < m_nCols);
	ASSERT(m_nHiddenCol == -1);
	m_nHiddenCol = colHide;

	// if the column has an active window -- change it
	int rowActive, colActive;
	if(GetActivePane(&rowActive, &colActive) != NULL &&
		colActive == colHide)
	{
		if(++colActive >= m_nCols)
			colActive = 0;
		SetActivePane(rowActive, colActive);
	}

	// hide all column panes
	for(int row = 0; row < m_nRows; row++)
	{
		CWnd* pPaneHide = GetPane(row, colHide);
		ASSERT(pPaneHide != NULL);
		pPaneHide->ShowWindow(SW_HIDE);
		pPaneHide->SetDlgCtrlID(AFX_IDW_PANE_FIRST + row * 16 + m_nCols);

		for(int col = colHide + 1; col < m_nCols; col++)
		{
			CWnd* pPane = GetPane(row, col);
			ASSERT(pPane != NULL);
			pPane->SetDlgCtrlID(IdFromRowCol(row, col - 1));
		}
	}

	m_nCols--;
	m_pColInfo[m_nCols].nCurSize = m_pColInfo[colHide].nCurSize;
	RecalcLayout();
}


BEGIN_MESSAGE_MAP(CSplitterWndEx, CSplitterWnd)
	//{{AFX_MSG_MAP(CSplitterWndEx)
	// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAPEND_MESSAGE_MAP()
END_MESSAGE_MAP()


//////////////////////////////////////////////////////////////////////////////
// CQuarantineCoolBar
//
IMPLEMENT_DYNAMIC(CQuarantineCoolBar, CCoolBar)

////////////////

#define BITMAP_HEIGHT  20

//////////////////////////////////////////////////////////////////////////////
// This is the virtual function you have to override to add bands to the coolbar
// Return type: BOOL TRUE on success
// 3/5/98 - DBUCHES function created / header added
//////////////////////////////////////////////////////////////////////////////
BOOL CQuarantineCoolBar::OnCreateBands()
{
	// Create tool bar
	CCoolToolBar& tb = m_wndToolBar;
	if(!tb.Create(this,
		WS_CHILD|WS_VISIBLE|WS_CLIPSIBLINGS|WS_CLIPCHILDREN|
		CBRS_TOOLTIPS|CBRS_SIZE_DYNAMIC|CBRS_FLYBY) ||
		!tb.LoadToolBar(IDR_MAINFRAME)) 
	{
		TRACE0("Failed to create toolbar\n");
		return -1; // failed to create
	}
	tb.ModifyStyle(0, TBSTYLE_FLAT);

	// Set up button text
	CString sText;
	int     nWidestCX= 0;

	sText.LoadString(IDS_ADD_BTN_TEXT);
	m_wndToolBar.SetButtonText(0, sText);
	nWidestCX = max(nWidestCX,GetTextWidthHelper(sText));
	m_wndToolBar.SetButtonText(1, ""); // Seperator
	sText.LoadString(IDS_PROPERTIES_BTN_TEXT);
	m_wndToolBar.SetButtonText(2, sText);
	nWidestCX = max(nWidestCX,GetTextWidthHelper(sText));
	sText.LoadString(IDS_REPAIR_BTN_TEXT);
	m_wndToolBar.SetButtonText(3, sText);
	nWidestCX = max(nWidestCX,GetTextWidthHelper(sText));
	sText.LoadString(IDS_RESTORE_BTN_TEXT);
	m_wndToolBar.SetButtonText(4, sText);
	nWidestCX = max(nWidestCX,GetTextWidthHelper(sText));
	sText.LoadString(IDS_DELETE_BTN_TEXT);
	m_wndToolBar.SetButtonText(5, sText);
	nWidestCX = max(nWidestCX,GetTextWidthHelper(sText));
	sText.LoadString(IDS_SUBMIT_BTN_TEXT);
	m_wndToolBar.SetButtonText(6, sText);
	nWidestCX = max(nWidestCX,GetTextWidthHelper(sText));
	m_wndToolBar.SetButtonText(7, ""); // Seperator
	sText.LoadString(IDS_LU_BTN_TEXT);
	m_wndToolBar.SetButtonText(8, sText);
	nWidestCX = max(nWidestCX,GetTextWidthHelper(sText));

	// Remove live-update button if neccessary    
	if(!IsLiveUpdateInstalled()) 
	{
		tb.GetToolBarCtrl().DeleteButton(8);
		tb.GetToolBarCtrl().DeleteButton(7);
	}

	// Only one row of text per button
	m_wndToolBar.SendMessage(TB_SETMAXTEXTROWS, 1, 0L);

	// Compute height of text.
	CSize size;
	GetTextWidthHelper(sText, &size);

	// Set width and height of buttons
	int nBtnCX = nWidestCX + (GetSystemMetrics(SM_CXBORDER)*2) + 4;

	CSize sizeImage(BITMAP_HEIGHT, BITMAP_HEIGHT);
	CSize sizeMax(nBtnCX, BITMAP_HEIGHT + 2 + size.cy);

	m_wndToolBar.SetSizes(sizeMax, sizeImage);

	// bugbug- need hot image list
	//m_wndToolBar.SendMessage(TB_SETHOTIMAGELIST, 0, (LPARAM) m_imgListHot.m_hImageList);

	// Band 1: Add toolbar band
	CRebarBandInfo rbbi;

	// Get minimum size of bands
	CSize szHorz = tb.CalcDynamicLayout(-1, 0);         // get min horz size

	rbbi.fMask = RBBIM_STYLE|RBBIM_CHILD|RBBIM_CHILDSIZE|RBBIM_COLORS;
	rbbi.fStyle = RBBS_CHILDEDGE | 0x80;                 // 0x80 - allways show gripper
	rbbi.hwndChild = m_wndToolBar;
	// Note that per the MFC source, twice the 6 pixel padding should be added here.
	// 2/3/99 Mike Dunn - I'm putting it back to 6 because using 12
	// makes the rebar too tall.  Must be something flaky with the newest
	// common controls DLL.
	rbbi.cyMinChild = sizeMax.cy + 6;//2;
	rbbi.cxMinChild = 0;//szHorz.cx;
	rbbi.clrFore = GetSysColor(COLOR_BTNTEXT);
	rbbi.clrBack = GetSysColor(COLOR_BTNFACE);
	if(!InsertBand(-1, &rbbi))
		return -1;  

	// This call is here to force the toolbar to repaint and properly
	// resize the buttons.  The ADD button is always enabled, so disabling
	// it here means the update command UI mechanism will re-enable it,
	// thus forcing a repaint.  All this is to fix the button sizes on
	// Garret's machine. ;)
	m_wndToolBar.GetToolBarCtrl().EnableButton(ID_FILE_ADD, FALSE);

	return 0; // OK
}


//////////////////////////////////////////////////////////////////////////////
// Verifies that LiveUpdate is installed.  This code stolen from NavLU. 
// 5/7/98 DBUCHES - Function created / Header added.
//////////////////////////////////////////////////////////////////////////////
BOOL IsLiveUpdateInstalled() 
{
	LPCTSTR  szLuPathKey  = _T("SOFTWARE\\Symantec\\SharedUsage");
	LPCTSTR  szLuQueryKey = _T("LiveUpdate1");
	BOOL     bFoundPath   = FALSE;
	HKEY     hBaseKey;
	DWORD    dwValueType;
	TCHAR    lpszLuPathName[MAX_PATH];
	DWORD    dwPathSize = MAX_PATH;

	if(ERROR_SUCCESS == RegOpenKeyEx(HKEY_LOCAL_MACHINE, szLuPathKey, 0, KEY_READ, &hBaseKey))
	{
		if(ERROR_SUCCESS == RegQueryValueEx(hBaseKey, szLuQueryKey, 0, &dwValueType, (LPBYTE)lpszLuPathName, &dwPathSize))
		{
			if(dwValueType == REG_SZ && *lpszLuPathName)
			{
				// Make sure file exists.                
				lstrcat(lpszLuPathName, _T("\\S32LIVE1.DLL"));
				if(GetFileAttributes(lpszLuPathName) != 0xFFFFFFFF)
					bFoundPath = TRUE;
			}
		}
		RegCloseKey(hBaseKey);
	}

	return bFoundPath;
}


//////////////////////////////////////////////////////////////////////////////
// 11/25/97 DBUCHES - Function created / Header added.
//////////////////////////////////////////////////////////////////////////////
int CQuarantineCoolBar::GetTextWidthHelper(CString& sText, CSize* pSize)
{
	CDC*    pDC = CWnd::GetDesktopWindow()->GetDC();
	CFont   font;
	LOGFONT lf;
	SystemParametersInfo(SPI_GETICONTITLELOGFONT, sizeof(LOGFONT),&lf,0);
	font.CreateFontIndirect(&lf);
	CFont*  pOldFont = pDC->SelectObject(&font);
	CSize   size;
	GetTextExtentPoint32(pDC->GetSafeHdc(), sText, sText.GetLength(), &size);
	CWnd::GetDesktopWindow()->ReleaseDC(pDC);

	if(pSize)
		*pSize = size;

	return size.cx;
}



//////////////////////////////////////////////////////////////////////////////
// CMainFrame

IMPLEMENT_DYNCREATE(CMainFrame, CFrameWnd)

BEGIN_MESSAGE_MAP(CMainFrame, CFrameWnd)
	//{{AFX_MSG_MAP(CMainFrame)
	ON_WM_CREATE()
	ON_COMMAND(ID_VIEW_REFRESH, OnViewRefresh)
	ON_COMMAND(ID_VIEW_LIVEUPDATE, OnViewLiveupdate)
	ON_WM_DESTROY()
	//	ON_COMMAND(ID_VIEW_TREE, OnViewTree)
	ON_COMMAND(ID_FILE_ADD, OnFileAdd)
	//	ON_UPDATE_COMMAND_UI(ID_VIEW_TREE, OnUpdateViewTree)
	ON_COMMAND(ID_VIEW_SHOW_EXPANDED_THREATS, OnViewShowExpandedThreats)
	ON_COMMAND(ID_VIEW_SHOW_VIRAL_THREATS, OnViewShowViralThreats)
	ON_COMMAND_RANGE(ID_VIEW_ARRANGEICONS_BYQUARANTINEDATE, ID_VIEW_ARRANGEICONS_BYTHREATNAME, OnSortMenuItem)
	ON_UPDATE_COMMAND_UI_RANGE(ID_VIEW_ARRANGEICONS_BYQUARANTINEDATE, ID_VIEW_ARRANGEICONS_BYTHREATNAME, OnUpdateSortMenuItem)
	ON_COMMAND(ID_VIEW_OPTIONS, OnViewOptions)
	ON_COMMAND(ID_FILE_EXPORT, OnFileExport)
	ON_UPDATE_COMMAND_UI(ID_FILE_ADD, OnUpdateFileAdd)
	ON_UPDATE_COMMAND_UI(ID_VIEW_OPTIONS, OnUpdateViewOptions)
	ON_UPDATE_COMMAND_UI(ID_VIEW_SHOW_EXPANDED_THREATS, OnUpdateViewShow)
	ON_UPDATE_COMMAND_UI(ID_VIEW_SHOW_VIRAL_THREATS, OnUpdateViewShow)
	ON_UPDATE_COMMAND_UI(ID_FILE_EXPORT, OnUpdateFileExport)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


static UINT indicators[] =
{
	ID_SEPARATOR,           // status line indicator
};


//////////////////////////////////////////////////////////////////////////////
// CMainFrame construction/destruction

//////////////////////////////////////////////////////////////////////////////
// 2/25/98 DBUCHES - Function created / Header added.
//////////////////////////////////////////////////////////////////////////////
CMainFrame::CMainFrame()
{
	// Should we show the tree?
	//m_bShowTree = AfxGetApp()->GetProfileInt(REGKEY_UI_SETTINGS,
	//                                          REGKEY_VALUE_SHOWTREE,
	//                                          1);
	m_bShowTree = TRUE;

	CBrandingRes BrandRes;
	m_sProductName = BrandRes.ProductName();
	m_sAppName = BrandRes.GetString(IDS_QUARANTINE_APP_NAME);

	m_bAutoMenuEnable  = FALSE;
}


//////////////////////////////////////////////////////////////////////////////
// 2/25/98 DBUCHES - Function created / Header added.
//////////////////////////////////////////////////////////////////////////////
CMainFrame::~CMainFrame()
{
}


//////////////////////////////////////////////////////////////////////////////
// Handles WM_CREATE message
// 2/25/98 DBUCHES - Function created / Header added.
//////////////////////////////////////////////////////////////////////////////
int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if(CFrameWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

	// Set title
	SetWindowText(m_sAppName);

	CString sTitle((LPCTSTR) IDS_TOOLBAR_TITLE);

	// Create cool bar
	if(!m_wndCoolBar.Create(this,
		WS_CHILD|WS_VISIBLE|WS_BORDER|WS_CLIPSIBLINGS|WS_CLIPCHILDREN|
		RBS_TOOLTIPS|RBS_BANDBORDERS|RBS_VARHEIGHT)) 
	{
		if(!m_wndToolBar.Create(this) ||
			!m_wndToolBar.LoadToolBar(IDR_MAINFRAME))
		{   
			TRACE0("Failed to create toolbar\n");
			return -1;      // fail to create
		}

		m_wndToolBar.SetWindowText(sTitle);
	}

	m_wndCoolBar.SetWindowText(sTitle);

	if(!m_wndStatusBar.Create(this) ||
		!m_wndStatusBar.SetIndicators(indicators,
		sizeof(indicators)/sizeof(UINT)))
	{
		TRACE0("Failed to create status bar\n");
		return -1;      // fail to create
	}

	// TODO: Remove this if you don't want tool tips or a resizeable toolbar
	m_wndCoolBar.SetBarStyle(m_wndCoolBar.GetBarStyle() |
		CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC);

	// Set About menu item name
	CString csAboutFormat;
	CString csMenuAbout;

	CMenu* pMenu = GetMenu();
	pMenu->GetMenuString(ID_APP_ABOUT, csAboutFormat, MF_BYCOMMAND);
	csMenuAbout.Format(csAboutFormat, m_sProductName);
	pMenu->ModifyMenu(ID_APP_ABOUT, MF_STRING|MF_BYCOMMAND, ID_APP_ABOUT, csMenuAbout);

	// Remove liveupdate menu item if it is not installed
	CMenu* pViewMenu = pMenu->GetSubMenu(2);
	if(!IsLiveUpdateInstalled())
	{
		// Remove "View | LiveUpdate" menu item
		pViewMenu->DeleteMenu(ID_VIEW_LIVEUPDATE, MF_BYCOMMAND); 
	}

	CQconsoleDoc* pDoc = (CQconsoleDoc*)GetActiveDocument();
	ASSERT(NULL != pDoc);
	if(!pDoc->GetExpandedThreatShowSetting())
	{
		pViewMenu->DeleteMenu(0, MF_BYPOSITION);

		pViewMenu->DeleteMenu(ID_VIEW_ARRANGEICONS_BYDEPENDENCIES, MF_BYCOMMAND);
		//pViewMenu->DeleteMenu(ID_VIEW_ARRANGEICONS_BYRISK_LEVEL, MF_BYCOMMAND);
	}
	//else // Default view is VIEW_MODE_EXPANDED_THREATS
	//{
	//	pViewMenu->EnableMenuItem(ID_VIEW_ARRANGEICONS_BYNAME, MF_BYCOMMAND | MF_GRAYED | MF_DISABLED);
	//	pViewMenu->EnableMenuItem(ID_VIEW_ARRANGEICONS_BYLOCATION, MF_BYCOMMAND | MF_GRAYED | MF_DISABLED);
	//}

	return 0;
}


//////////////////////////////////////////////////////////////////////////////
// Handles WM_PRECREATEWINDOW message
// 2/15/98 - DBUCHES function created / header added
//////////////////////////////////////////////////////////////////////////////
BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	// Remove document title
	cs.style &=~ (FWS_ADDTOTITLE);

	// Get previous window position.
	HKEY hKey;
	WINDOWPLACEMENT wp;

	CString sKeyName(REGKEY_QUARANTINE_UI);
	sKeyName += _T("\\");
	sKeyName += REGKEY_UI_SETTINGS;

	if(RegOpenKeyEx(HKEY_CURRENT_USER, sKeyName, 0, KEY_READ, &hKey) == ERROR_SUCCESS)
	{
		DWORD dwType = REG_BINARY;
		DWORD dwSize = sizeof(WINDOWPLACEMENT);

		if(RegQueryValueEx(hKey,
			REGKEY_VALUE_WP,
			0,
			&dwType,
			(LPBYTE)&wp,
			&dwSize) == ERROR_SUCCESS)
		{
			CRect rect(wp.rcNormalPosition);
			cs.x = wp.rcNormalPosition.left;
			cs.y = wp.rcNormalPosition.top;
			cs.cx = rect.Width();
			cs.cy = rect.Height();
			((CQconsoleApp*)AfxGetApp())->m_nShowCmd = wp.showCmd;
		}

		// Cleanup
		RegCloseKey(hKey);
	}

	return CFrameWnd::PreCreateWindow(cs);
}


//////////////////////////////////////////////////////////////////////////////
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


//////////////////////////////////////////////////////////////////////////////
// CMainFrame message handlers

//////////////////////////////////////////////////////////////////////////////
// Rebuilds item lists and tells all views to refresh.
// 2/15/98 - DBUCHES function created / header added
//////////////////////////////////////////////////////////////////////////////
void CMainFrame::OnViewRefresh() 
{
	// Get pointer to active documents
	CQconsoleDoc* pDoc = (CQconsoleDoc*)GetActiveDocument();

	// Rebuild item lists
	pDoc->RebuildItemArrays();

	// Tell all views what happened.
	pDoc->UpdateAllViews(NULL, LISTVIEW_REFRESH, NULL);
}


//////////////////////////////////////////////////////////////////////////////
// Handle the LiveUpdate menu item
// 2/20/98 - DBUCHES function created / header added
//////////////////////////////////////////////////////////////////////////////
void CMainFrame::OnViewLiveupdate() 
{
	CString sDir;
	SHGetFolderPath(GetSafeHwnd(), 
		CSIDL_PROGRAM_FILES, 
		NULL, 
		SHGFP_TYPE_DEFAULT,  
		sDir.GetBuffer(MAX_PATH * sizeof(TCHAR)));
	sDir.ReleaseBuffer();

	sDir += _T("\\Symantec\\LiveUpdate\\Luall.exe");	
	GetShortPathName(sDir.GetBuffer(),sDir.GetBuffer(MAX_PATH * sizeof(TCHAR)),MAX_PATH * sizeof(TCHAR));
	sDir.ReleaseBuffer();

	// Spawn Live update process
	WinExec(sDir, SW_SHOWNORMAL);
}


//////////////////////////////////////////////////////////////////////////////
// Override of CFrameWnd::OnCreateClient
// 2/20/98 - DBUCHES function created / header added
//////////////////////////////////////////////////////////////////////////////
BOOL CMainFrame::OnCreateClient(LPCREATESTRUCT lpcs, CCreateContext* pContext) 
{
	// Create splitter window
	if(m_wndSplitter.CreateStatic(this, 1, 2) == FALSE)
		return FALSE;

	// Get window dimensions
	CRect rect;
	GetClientRect(&rect);

	// Get width of tree view    
	int iTreeViewWidth = AfxGetApp()->GetProfileInt(REGKEY_UI_SETTINGS,
		REGKEY_VALUE_TREEWIDTH,
		rect.Width() / 4);
	// Create our views.
	if(FALSE == m_wndSplitter.CreateView(0, 
		0,
		RUNTIME_CLASS(CQTreeView), 
		CSize(iTreeViewWidth, 100), 
		pContext))
		return FALSE;

	if(FALSE == m_wndSplitter.CreateView(0, 
		1, 
		RUNTIME_CLASS(CQconsoleView), 
		CSize(100,100), 
		pContext))
		return FALSE;

	// Set active view
	m_wndSplitter.SetActivePane(0, 1, NULL);

	//   // Hide the tree pane if neccessary.
	//if(m_bShowTree == FALSE)
	//{
	//	m_wndSplitter.SetSavedSize(iTreeViewWidth);
	//	m_wndSplitter.HideColumn(0);
	//}

	return TRUE;
}


//////////////////////////////////////////////////////////////////////////////
// Save off position of main window
// 2/20/98 - DBUCHES function created / header added
//////////////////////////////////////////////////////////////////////////////
void CMainFrame::OnDestroy() 
{
	HKEY hKey;

	// Don't save position if we are minimized.
	if(IsIconic())
		return;

	// Open our key
	CString sKeyName(REGKEY_QUARANTINE_UI);
	sKeyName += _T("\\");
	sKeyName += REGKEY_UI_SETTINGS;
	if(RegOpenKeyEx(HKEY_CURRENT_USER, sKeyName, 0, KEY_WRITE, &hKey) == ERROR_SUCCESS)
	{
		// Save off window placement
		WINDOWPLACEMENT wp;
		wp.length = sizeof(WINDOWPLACEMENT);
		GetWindowPlacement(&wp);

		RegSetValueEx(hKey,
			REGKEY_VALUE_WP,
			0,
			REG_BINARY,
			(LPBYTE)&wp,
			sizeof(WINDOWPLACEMENT));
		// Cleanup
		RegCloseKey(hKey);
	}

	//// Save state of tree
	//AfxGetApp()->WriteProfileInt(REGKEY_UI_SETTINGS,
	//                              REGKEY_VALUE_SHOWTREE,
	//                              m_bShowTree);

	// If the tree pane is visible, save the size
	if(m_bShowTree)
	{
		CRect rect;
		m_wndSplitter.GetPane(0,0)->GetClientRect(&rect);
		AfxGetApp()->WriteProfileInt(REGKEY_UI_SETTINGS,
			REGKEY_VALUE_TREEWIDTH,
			rect.Width());
	}

	// Call base class to finish the job.
	CFrameWnd::OnDestroy();
}


//////////////////////////////////////////////////////////////////////////////
// Handles View/Tree menu item
// 2/25/98 DBUCHES - Function created / Header added.
//////////////////////////////////////////////////////////////////////////////
//void CMainFrame::OnViewTree() 
//{
//	// Invert state
//	m_bShowTree = !m_bShowTree;
//
//	if(m_bShowTree)
//		m_wndSplitter.ShowColumn();
//	else
//	{
//		// Save the width of the tree before we hide it.
//		CRect rect;
//		m_wndSplitter.GetPane(0,0)->GetClientRect(&rect);
//		AfxGetApp()->WriteProfileInt(REGKEY_UI_SETTINGS,
//                                      REGKEY_VALUE_TREEWIDTH,
//                                      rect.Width());
//        
//		m_wndSplitter.HideColumn(0);
//	}
//}


//////////////////////////////////////////////////////////////////////////////
// Handles CMDUI updateing for View/Tree menu item
// 2/25/98 DBUCHES - Function created / Header added.
//////////////////////////////////////////////////////////////////////////////
//void CMainFrame::OnUpdateViewTree(CCmdUI* pCmdUI) 
//{
//    pCmdUI->SetCheck(m_bShowTree);	
//}


//////////////////////////////////////////////////////////////////////////////
// Handles Show/Security Risks menu item
// 3/28/05 LKurahashi Function added
//////////////////////////////////////////////////////////////////////////////
void CMainFrame::OnViewShowExpandedThreats()
{
	// Get pointer to document object
	CQconsoleDoc* pDoc = (CQconsoleDoc*)GetActiveDocument();
	ASSERT(NULL != pDoc);

	// Change mode
	pDoc->SetViewMode(VIEW_MODE_EXPANDED_THREATS);
	pDoc->UpdateAllViews(NULL, SET_VIEW_FILTER, NULL);
}


//////////////////////////////////////////////////////////////////////////////
// Handles Show/Viral menu item
// 3/28/05 LKurahashi Function added
//////////////////////////////////////////////////////////////////////////////
void CMainFrame::OnViewShowViralThreats()
{
	// Get pointer to document object
	CQconsoleDoc* pDoc = (CQconsoleDoc*)GetActiveDocument();
	ASSERT(NULL != pDoc);

	// Change mode
	pDoc->SetViewMode(VIEW_MODE_VIRAL_THREATS);
	pDoc->UpdateAllViews(NULL, SET_VIEW_FILTER, NULL);
}


//////////////////////////////////////////////////////////////////////////////
// Handles CMDUI stuff for Show popup menu items
// 2/25/98 DBUCHES - Function created / Header added.
//////////////////////////////////////////////////////////////////////////////
void CMainFrame::OnUpdateViewShow(CCmdUI* pCmdUI) 
{
	// Get pointer to document object
	CQconsoleDoc* pDoc = (CQconsoleDoc*) GetActiveDocument();
	ASSERT(NULL != pDoc);

	BOOL bCheck = FALSE;

	// Get the current mode, and match it to the control
	int iCurrentMode = pDoc->GetViewMode();

	switch(iCurrentMode)
	{
	case VIEW_MODE_EXPANDED_THREATS:
		bCheck = pCmdUI->m_nID == ID_VIEW_SHOW_EXPANDED_THREATS ? TRUE : FALSE;
		break;
	case VIEW_MODE_VIRAL_THREATS:
		bCheck = pCmdUI->m_nID == ID_VIEW_SHOW_VIRAL_THREATS ? TRUE : FALSE;
		break;
	}

	// Set checked state
	pCmdUI->SetRadio(bCheck);
}


//////////////////////////////////////////////////////////////////////////////
// Handles the File/Add menu item
// 3/2/98 DBUCHES - Function created / Header added.
// 4/18/98 JTAYLOR - Added another argument to AddFileToQuarantine.
//////////////////////////////////////////////////////////////////////////////
void CMainFrame::OnFileAdd()
{
	CString sFilter((LPCTSTR) IDS_ADD_FILTER_STRING);
	CMoveFileDlg dlg(TRUE,                 // Open 
		NULL,                  // No default extension
		NULL,                  // Initial file name
		OFN_FILEMUSTEXIST |    // Flags
		OFN_HIDEREADONLY |
		OFN_PATHMUSTEXIST,
		sFilter,               // Filter string
		this);                // Parent window


	// Fire off dialog.
	if(dlg.DoModal() != IDOK)
		return;

	// Get pointer to document
	CQconsoleDoc* pDoc = (CQconsoleDoc*)GetActiveDocument();
	ASSERT(NULL != pDoc);
	
	bool bShowExpandedThreats = pDoc->GetExpandedThreatShowSetting();
	DWORD dwView = VIEW_MODE_EXPANDED_THREATS;
	DWORD dwItemStatus = QFILE_STATUS_THREAT_BACKUP;
	if(!bShowExpandedThreats)
	{
		dwView = VIEW_MODE_VIRAL_THREATS;
		dwItemStatus = QFILE_STATUS_QUARANTINED;
	}

    // Is the file compressed?
    DWORD dwQFileType = QFILE_TYPE_NORMAL;
    if(pDoc->IsFileCompressed(dlg.GetPathName()))
    {
        dwQFileType = QFILE_TYPE_COMPRESSED;
    }

	// Add this file to quarantine.
	CSymPtr<IQuarantineItem> pItem;
	HRESULT hr = pDoc->m_pQuarantine->CreateNewQuarantineItemFromFile(dlg.GetPathName().GetBuffer(0),
		dwQFileType,  // Type
		dwItemStatus, // Status
		&pItem);

	if(SUCCEEDED(hr) && (pItem != NULL))
	{
		pItem->TakeSystemSnapshot();

		if(QItemEZ::THREAT_VIRAL == QItemEZ::IsViral(pItem))
		{
			pItem->SetFileStatus(QFILE_STATUS_QUARANTINED);   
			pItem->SaveItem();

			dwView = VIEW_MODE_VIRAL_THREATS;
		}

		pItem.Release();
	}

	// Check for failure
	if(FAILED(hr))
	{
		// Let the user know we could not add the file to quarantine
		CString sMsg;
		sMsg.LoadString(IDS_ERROR_ADDING_TO_QUARANTINE);
		DisplayNAVError(IDS_ERROR_ADDING_TO_QUARANTINE, sMsg);
		return;
	}

	// We need to do a safe delete here, to keep infected files from 
	// ending up in NU's protected recycle bin.
	if(dlg.m_bDelete && hr != S_UNDELETABLE_VIRUS_FOUND)
	{
		// Remove the file's Read only attributes
		if(FILE_ATTRIBUTE_READONLY & GetFileAttributes(dlg.GetPathName()))
			SetFileAttributes(dlg.GetPathName(), FILE_ATTRIBUTE_NORMAL);

		TRY
		{
			// Open the file exclusivly
			CFile file(dlg.GetPathName(), CFile::modeReadWrite | CFile::shareExclusive);

			// Set to 0 bytes
			file.SetLength(0);

			// Close file
			file.Close();

			// Finally, delete file.
			CFile::Remove(dlg.GetPathName());
		}
		CATCH(CFileException, e)
		{
			// Inform the user that the original file could not be deleted
			CString sMsg;
			sMsg.Format(IDS_ERROR_DELETING_FILE, dlg.GetPathName());
			DisplayNAVError(IDS_ERROR_DELETING_FILE, sMsg);
			return;
		}
		END_CATCH;
	}

	// Tell everyone that a new item has been added.
	pDoc->RebuildItemArrays();
	pDoc->UpdateAllViews(NULL, LISTVIEW_REFRESH, NULL);

	switch(dwView)
	{
	case VIEW_MODE_VIRAL_THREATS:
		OnViewShowViralThreats();
		break;

	case VIEW_MODE_EXPANDED_THREATS:
		OnViewShowExpandedThreats();
		break;

	default:
		break;
	}
}


//////////////////////////////////////////////////////////////////////////////
// Handles all Arrange Icon menu items
// 3/5/98 - DBUCHES function created / header added
//////////////////////////////////////////////////////////////////////////////
void CMainFrame::OnSortMenuItem(UINT nID)
{
	// Get active document
	CQconsoleDoc* pDoc = (CQconsoleDoc*)GetActiveDocument();    
	ASSERT(NULL != pDoc);

	// Save off the column we are sorting by
	int iColumn;
	switch(nID)
	{
	//case ID_VIEW_ARRANGEICONS_BYNAME:           
	//	iColumn = pDoc->MapColumnIDToIndex(COLUMN_FILE_NAME);
	//	break; 
	//case ID_VIEW_ARRANGEICONS_BYLOCATION:       
	//	iColumn = pDoc->MapColumnIDToIndex(COLUMN_LOCATION);
	//	break;  
	case ID_VIEW_ARRANGEICONS_BYQUARANTINEDATE: 
		iColumn = pDoc->MapColumnIDToIndex(COLUMN_QDATE);
		break;
	case ID_VIEW_ARRANGEICONS_BYDEPENDENCIES:
		iColumn = pDoc->MapColumnIDToIndex(COLUMN_DEPENDENCIES);
		break;     
	case ID_VIEW_ARRANGEICONS_BYRISK_LEVEL:
		iColumn = pDoc->MapColumnIDToIndex(COLUMN_RISK_LEVEL);
		break;     
	case ID_VIEW_ARRANGEICONS_BYSUBMITTEDDATE:  
		iColumn = pDoc->MapColumnIDToIndex(COLUMN_SDATE);
		break;
	case ID_VIEW_ARRANGEICONS_BYSTATUS:
		iColumn = pDoc->MapColumnIDToIndex(COLUMN_STATUS);
		break;
	case ID_VIEW_ARRANGEICONS_BYTHREATNAME:      
		iColumn = pDoc->MapColumnIDToIndex(COLUMN_THREAT_NAME);
		break;
	case ID_VIEW_ARRANGEICONS_BYTYPE:      
		iColumn = pDoc->MapColumnIDToIndex(COLUMN_CATEGORY);
		break;
	default:
		// Should never get here.
		ASSERT(FALSE);
		return;
	}
	// Save off old sort col.
	int iOldCol = pDoc->GetSortCol();

	// Save off sort column.
	pDoc->SetSortCol(iColumn);

	// Tell all views what happened.
	pDoc->UpdateAllViews(NULL, LISTVIEW_SORTITEMS, (CObject*)iOldCol);
}


//////////////////////////////////////////////////////////////////////////////
// Handles CMDUI chores for the sort menu item items.
// 3/5/98 - DBUCHES function created / header added
//////////////////////////////////////////////////////////////////////////////
void CMainFrame::OnUpdateSortMenuItem(CCmdUI* pCmdUI)
{
	// Get active document
	CQconsoleDoc* pDoc = (CQconsoleDoc*)GetActiveDocument();    
	ASSERT(NULL != pDoc);

	BOOL bCheck = FALSE;

	int iColumnID = pDoc->MapColumnIndexToID(pDoc->GetSortCol());
	switch(iColumnID)
	{
	//case COLUMN_FILE_NAME:
	//	bCheck = pCmdUI->m_nID == ID_VIEW_ARRANGEICONS_BYNAME ? TRUE : FALSE;
	//	break;
	//case COLUMN_LOCATION:
	//	bCheck = pCmdUI->m_nID == ID_VIEW_ARRANGEICONS_BYLOCATION ? TRUE : FALSE;
	//	break;
	case COLUMN_QDATE:
		bCheck = pCmdUI->m_nID == ID_VIEW_ARRANGEICONS_BYQUARANTINEDATE ? TRUE : FALSE;
		break;
	case COLUMN_DEPENDENCIES:
		bCheck = pCmdUI->m_nID == ID_VIEW_ARRANGEICONS_BYDEPENDENCIES ? TRUE : FALSE;
		break;
	case COLUMN_RISK_LEVEL:
		bCheck = pCmdUI->m_nID == ID_VIEW_ARRANGEICONS_BYRISK_LEVEL ? TRUE : FALSE;
		break;
	case COLUMN_SDATE:
		bCheck = pCmdUI->m_nID == ID_VIEW_ARRANGEICONS_BYSUBMITTEDDATE ? TRUE : FALSE;
		break;
	case COLUMN_STATUS:
		bCheck = pCmdUI->m_nID == ID_VIEW_ARRANGEICONS_BYSTATUS ? TRUE : FALSE;
		break;
	case COLUMN_THREAT_NAME:
		bCheck = pCmdUI->m_nID == ID_VIEW_ARRANGEICONS_BYTHREATNAME ? TRUE : FALSE;
		break;
	case COLUMN_CATEGORY:
		bCheck = pCmdUI->m_nID == ID_VIEW_ARRANGEICONS_BYTYPE ? TRUE : FALSE;
		break;
	}

	pCmdUI->SetRadio(bCheck);
}


//////////////////////////////////////////////////////////////////////////////
// Handles Options menu item.
// 3/31/98 DBUCHES - Function created / Header added.
//////////////////////////////////////////////////////////////////////////////
void CMainFrame::OnViewOptions() 
{
	// Get active document
	CQconsoleDoc* pDoc = (CQconsoleDoc*)GetActiveDocument();    
	ASSERT(NULL != pDoc);

	// Get options object
	IQuarantineOpts* pOpts = pDoc->m_pOpts;

	// Create dialog
	CString sOptionsTitle;
	sOptionsTitle.Format(IDS_OPTIONS_TITLE, m_sAppName);
	COptionsSheet dlg(sOptionsTitle, pOpts);

	// Fire off property sheet
	if(IDOK == dlg.DoModal())
	{
		// Save off options
		pOpts->WriteQuarOptsFile();
	}
}


//////////////////////////////////////////////////////////////////////////////
// Handles File/Export menu item.
// 4/13/98 DBUCHES - Function created / Header added.
//////////////////////////////////////////////////////////////////////////////
void CMainFrame::OnFileExport() 
{
	// Get pointer to document
	CQconsoleDoc* pDoc = (CQconsoleDoc*) GetActiveDocument();
	ASSERT(NULL != pDoc);

	// Fire off export dialog
	CString sFilter((LPCTSTR) IDS_EXPORT_FILTER_STRING);
	CString sDefFileName((LPCTSTR) IDS_EXPORT_FILENAME);
	CExportDlg dlg(FALSE,                   // Save 
		NULL,                    // No default extension
		sDefFileName,			 // Initial file name
		OFN_OVERWRITEPROMPT | OFN_HIDEREADONLY | OFN_PATHMUSTEXIST,
		sFilter,                 // Filter string
		this);

	dlg.m_iViewMode = pDoc->GetViewMode();

	if(dlg.DoModal() != IDOK)
		return;

	// Get the list we want to export.
	CMapPtrToPtr *pItems;
	int iItemCount;

	if(dlg.m_iExport)
	{
		// Current view
		pItems = pDoc->GetItemList();

		// How many items in this view?
		switch(pDoc->GetViewMode())
		{
		case VIEW_MODE_EXPANDED_THREATS:
			iItemCount = pDoc->GetExpandedThreatItemCount();
			break;
		case VIEW_MODE_VIRAL_THREATS:
			iItemCount = pDoc->GetViralItemCount();
			break;
		}
	}
	else
	{
		// All items
		pItems = pDoc->GetItemList(VIEW_MODE_ALL);

		//// Get total number of items
		iItemCount = pDoc->GetItemCount();
	}

	// Only export if there are items to display
	if(iItemCount == 0)
	{
		CString sMessage((LPCTSTR)IDS_ERROR_NO_ITEMS_IN_VIEW);
		MessageBox(sMessage, m_sAppName,  MB_ICONINFORMATION | MB_OK);
		return;
	}

	// Export to the proper format
	if(dlg.m_iFormat)
		Export(dlg.GetPathName(), pItems, TRUE); // Comma delimited
	else
		Export(dlg.GetPathName(), pItems); // Standard
}


const TCHAR LINE[] = _T("------------------------------------------------------------------------------\n\n");

//////////////////////////////////////////////////////////////////////////////
// ExportDelimited
// 4/13/98 DBUCHES - Function created / Header added.
//////////////////////////////////////////////////////////////////////////////
void CMainFrame::Export(CString &sFileName, CMapPtrToPtr* pI, BOOL bComma /* = FALSE */)
{
	CTypedPtrMap<CMapPtrToPtr, IQuarantineItem*, IQuarantineItem*>    *pItems;
	pItems = reinterpret_cast<CTypedPtrMap<CMapPtrToPtr, IQuarantineItem*, IQuarantineItem*>*> (pI);
	ASSERT(pItems);

	// Create progress dialog
	CProgressDlg dlg;
	int iProgress = 0;
	dlg.Create();
	dlg.SetRange(0, pItems->GetCount());

	// Create the file.
	try
	{
		CStdioFile file(sFileName, CFile::modeCreate | CFile::modeWrite | CFile::shareExclusive);

		// If we are in comma delimited form, fields of the 1st line
		// in the file should contain field descriptions.
		if(bComma)
		{
			CString s;
			s.LoadString(IDS_EXPORT_DELIMIT_DESC);
			s += _T('\n');
			file.WriteString(s);
		}
		else
		{
			// Write standard format header.
			CString sDate;
			TCHAR szDate[128];
			TCHAR szTime[128];
			SYSTEMTIME tm;
			GetLocalTime(&tm);

			GetDateFormat(NULL, DATE_LONGDATE, &tm, NULL, szDate, 128);
			GetTimeFormat(NULL, 0, &tm, NULL, szTime, 128);

			sDate += szDate;
			sDate += _T(" ");
			sDate += szTime;

			CString s;
			s.Format(IDS_EXPORT_HEADER_FORMAT, m_sAppName, sDate);
			s += _T('\n');
			file.WriteString(s);
			file.WriteString(LINE);
			s.LoadString(IDS_EXPORT_ITEM_HEADER);
			s += _T('\n');
			file.WriteString(s);
			file.WriteString(LINE);
		}

		POSITION pos;
		IQuarantineItem* p1;
		IQuarantineItem* p2;

		// Export each element
		for(pos = pItems->GetStartPosition(); pos != NULL && !dlg.CheckCancelButton();)
		{
			pItems->GetNextAssoc(pos, p1, p2);

			if(bComma)
				WriteCommaDelimitedItem(file, p2);
			else
				WriteStandardItem(file, p2);
		}

		// Bump progress dialog
		dlg.SetPos(++iProgress);
	}
	catch(CException* e)
	{
		// Tell the user that something bad happened
		CString sMessage((LPCTSTR)IDS_ERROR_EXPORTING_FILE);
		MessageBox(sMessage, m_sAppName, MB_ICONSTOP | MB_OK);
		e->Delete();
	}

	// Check for cancel operation.  If canceled, delete file we created.
	if(dlg.CheckCancelButton())
		::DeleteFile(sFileName);

	// Remove progress window
	dlg.DestroyWindow();
}


const TCHAR QUOTE[] = _T("\"");
const TCHAR SEP[]   = _T("\",\"");

const int BEGIN  = 0;
const int MIDDLE = 27;
const int END    = 56;

//////////////////////////////////////////////////////////////////////////////
// 4/13/98 DBUCHES - Function created / Header added.
//////////////////////////////////////////////////////////////////////////////
void CMainFrame::WriteCommaDelimitedItem(CStdioFile &file, IQuarantineItem* pItem)
{
	CString sLine = QUOTE;
	CString sTemp;
	TCHAR   sBuffer[MAX_PATH];

	CQconsoleDoc* pDoc = (CQconsoleDoc*) GetActiveDocument();

	// Save off item name
	pDoc->GetANSIFileNameOnly(pItem, sTemp);
	sLine += sTemp;
	sLine += SEP;

	// Save off item location
	pDoc->GetANSIFilePathOnly(pItem, sTemp);
	sLine += sTemp;
	sLine += SEP;

	// Get size.
	DWORD dwFileSize;
	pItem->GetOriginalFilesize(&dwFileSize);
	CRangedString rs(dwFileSize);
	sLine += rs;
	sLine += SEP;

	// Get date strings
	TCHAR szDate[128];
	TCHAR szTime[128];

	SYSTEMTIME tm;
	pItem->GetDateQuarantined(&tm);

	// Convert to local time
	FILETIME   ft, lft;
	SystemTimeToFileTime(&tm, &ft);
	FileTimeToLocalFileTime(&ft, &lft);
	FileTimeToSystemTime(&lft, &tm);

	GetDateFormat(NULL, DATE_LONGDATE, &tm, NULL, szDate, 128);
	GetTimeFormat(NULL, 0, &tm, NULL, szTime, 128);

	sLine += szDate;
	sLine += _T(" ");
	sLine += szTime;
	sLine += SEP;

	pItem->GetDateSubmittedToSARC(&tm);

	// Check to make sure this was actually submitted.
	if(tm.wYear == 0)
	{
		sLine += SEP;
	}
	else
	{
		// Convert to local time
		SystemTimeToFileTime(&tm, &ft);
		FileTimeToLocalFileTime(&ft, &lft);
		FileTimeToSystemTime(&lft, &tm);

		GetDateFormat(NULL, DATE_SHORTDATE, &tm, NULL, szDate, 128);
		GetTimeFormat(NULL, 0, &tm, NULL, szTime, 128);

		sLine += szDate;
		sLine += _T(" ");
		sLine += szTime;
		sLine += SEP;
	}

	// Get status text
	DWORD dwStatus;
	int iStrID;
	pItem->GetFileStatus(&dwStatus);

	if(dwStatus & QFILE_STATUS_QUARANTINED)
		iStrID = IDS_STATUS_QUARANTINED;
	else if(dwStatus & QFILE_STATUS_BACKUP_FILE)
		iStrID = IDS_STATUS_BACKUP;
	else if(dwStatus & QFILE_STATUS_THREAT_BACKUP)
		iStrID = IDS_STATUS_THREAT_BACKUP;
	//else if(dwStatus & QFILE_STATUS_SUBMITTED)
	//	iStrID = IDS_STATUS_SUBMITTED;

	sTemp.LoadString(iStrID);
	sLine += sTemp;
	sLine += SEP;

	// Get user name
	DWORD dwBufferSize = MAX_PATH;
	pItem->GetOriginalOwnerName(sBuffer, &dwBufferSize);
	sLine += sBuffer;
	sLine += SEP;

	// Get Machine name
	dwBufferSize = MAX_PATH;
	pItem->GetOriginalMachineName(sBuffer, &dwBufferSize);
	sLine += sBuffer;
	sLine += SEP;

	// Get domain name
	dwBufferSize = MAX_PATH;
	pItem->GetOriginalMachineDomain(sBuffer, &dwBufferSize);

	// If no domain name, display not applicable text
	if(lstrlen(sBuffer) == 0)
	{
		sTemp.LoadString(IDS_NA);
		sLine += sTemp;
		sLine += SEP;
	}
	else
	{
		sLine += sBuffer;
		sLine += SEP;
	}

	DWORD dwVirID;
	if(SUCCEEDED(pItem->GetVirusID(&dwVirID)))
	{
		CSymPtr<IScanVirusInfo> pScanVirusInfo;

		// Get the Virus info for this VID
		_tcscpy(sBuffer, _T(""));
		if(SUCCEEDED(pDoc->m_pQuarantine->GetVirusInformation(dwVirID, &pScanVirusInfo)))
		{
			unsigned long ulSize = MAX_PATH;
			pScanVirusInfo->GetName(sBuffer, &ulSize);
			sLine += sBuffer;
		}
	}
	// If there is no virus name check if it's from WarChild or OEH
	else
	{
		// Is this file from WarChild?
		DWORD dwFileType;
		pItem -> GetFileType(&dwFileType);

		// Default to unknown
		//
		CString cszDefault, cszFileName;
		cszDefault.LoadString(IDS_UNKNOWN_THREAT_NAME);
		QItemEZ::GetANSIFileNameOnly(pItem, cszFileName);

		sTemp.FormatMessage(cszDefault, cszFileName);

		if(dwFileType & QFILE_TYPE_WARCHILD)
			sTemp.LoadString(IDS_WARCHILD_SOURCE);

		if(dwFileType & QFILE_TYPE_OEH)
			sTemp.LoadString(IDS_OEH_SOURCE);

		sLine += sTemp;
	}

	// Add last quote
	sLine += QUOTE;
	sLine += _T('\n');

	// Write line to file.
	file.WriteString(sLine);
}


//////////////////////////////////////////////////////////////////////////////
// 4/13/98 DBUCHES - Function created / Header added.
//////////////////////////////////////////////////////////////////////////////
void CMainFrame::WriteStandardItem(CStdioFile &file, IQuarantineItem* pItem)
{
	CString sLine;
	CString sTemp;
	TCHAR   sBuffer[ MAX_PATH + 2 ];

	CQconsoleDoc* pDoc = (CQconsoleDoc*) GetActiveDocument();

	// Save off item name
	pDoc->GetANSIFileNameOnly(pItem, sTemp);
	sTemp += _T("\n");
	file.WriteString(sTemp);

	// Save off item location
	pDoc->GetANSIFilePathOnly(pItem, sTemp);
	sTemp += _T("\n");
	file.WriteString(sTemp);

	// Get status text
	DWORD dwStatus;
	int iStrID;
	pItem->GetFileStatus(&dwStatus);

	if(dwStatus & QFILE_STATUS_QUARANTINED)
		iStrID = IDS_STATUS_QUARANTINED;
	else if(dwStatus & QFILE_STATUS_BACKUP_FILE)
		iStrID = IDS_STATUS_BACKUP;
	else if(dwStatus & QFILE_STATUS_THREAT_BACKUP)
		iStrID = IDS_STATUS_THREAT_BACKUP;
	//else if(dwStatus & QFILE_STATUS_SUBMITTED)
	//	iStrID = IDS_STATUS_SUBMITTED;

	sLine.LoadString(iStrID);
	AddPadding(sLine, MIDDLE);

	// Get size.
	DWORD dwFileSize;
	pItem->GetOriginalFilesize(&dwFileSize);
	CRangedString rs(dwFileSize);
	sLine += rs;
	AddPadding(sLine, END);

	DWORD dwVirID;
	if(SUCCEEDED(pItem->GetVirusID(&dwVirID)))
	{
		// Get the Virus info for this VID
		CSymPtr<IScanVirusInfo> pScanVirusInfo;

		_tcscpy(sBuffer, _T(""));
		if(SUCCEEDED(pDoc->m_pQuarantine->GetVirusInformation(dwVirID, &pScanVirusInfo)))
		{
			unsigned long ulSize = MAX_PATH;
			pScanVirusInfo->GetName(sBuffer, &ulSize);
			sLine += sBuffer;
		}
	}
	// If there is no virus name check if it's from WarChild or OEH
	else
	{
		// Default to unknown
		CString cszDefault, cszFileName;
		cszDefault.LoadString(IDS_UNKNOWN_THREAT_NAME);
		QItemEZ::GetANSIFileNameOnly(pItem, cszFileName);

		sTemp.FormatMessage(cszDefault, cszFileName);

		// Is this file from WarChild?
		DWORD dwFileType;
		pItem -> GetFileType(&dwFileType);

		if(dwFileType & QFILE_TYPE_WARCHILD)
			sTemp.LoadString(IDS_WARCHILD_SOURCE);

		if(dwFileType & QFILE_TYPE_OEH)
			sTemp.LoadString(IDS_OEH_SOURCE);

		sLine += sTemp;
	}

	sLine += _T('\n');
	file.WriteString(sLine);


	// Get user name
	DWORD dwBufferSize = MAX_PATH;
	pItem->GetOriginalOwnerName(sBuffer, &dwBufferSize);
	sLine = sBuffer;
	AddPadding(sLine, MIDDLE);

	// Get Machine name
	dwBufferSize = MAX_PATH;
	pItem->GetOriginalMachineName(sBuffer, &dwBufferSize);
	sLine += sBuffer;
	AddPadding(sLine, END);

	// Get domain name
	dwBufferSize = MAX_PATH;
	pItem->GetOriginalMachineDomain(sBuffer, &dwBufferSize);

	// If no domain name, display not applicable text
	if(lstrlen(sBuffer) == 0)
	{
		sTemp.LoadString(IDS_NA);
		sLine += sTemp;
	}
	else
	{
		sLine += sBuffer;
	}

	sLine += _T('\n');
	file.WriteString(sLine);

	// Get date strings
	TCHAR szDate[128];
	TCHAR szTime[128];
	SYSTEMTIME tm;
	pItem->GetDateQuarantined(&tm);

	// Convert to local time
	FILETIME   ft, lft;
	SystemTimeToFileTime(&tm, &ft);
	FileTimeToLocalFileTime(&ft, &lft);
	FileTimeToSystemTime(&lft, &tm);

	GetDateFormat(NULL, DATE_LONGDATE, &tm, NULL, szDate, 128);
	GetTimeFormat(NULL, 0, &tm, NULL, szTime, 128);

	sLine = szDate;
	sLine += _T(" ");
	sLine += szTime;
	sLine += _T('\n');
	file.WriteString(sLine);

	pItem->GetDateSubmittedToSARC(&tm);

	// Check to make sure this was actually submitted.
	if(tm.wYear == 0)
	{
		sTemp.LoadString(IDS_NOT_SUBMITTED);
		sLine = sTemp;
	}
	else
	{
		// Convert to local time
		SystemTimeToFileTime(&tm, &ft);
		FileTimeToLocalFileTime(&ft, &lft);
		FileTimeToSystemTime(&lft, &tm);

		GetDateFormat(NULL, DATE_SHORTDATE, &tm, NULL, szDate, 128);
		GetTimeFormat(NULL, 0, &tm, NULL, szTime, 128);

		sLine += szDate;
		sLine += _T(" ");
		sLine += szTime;
	}

	sLine += _T("\n\n");
	file.WriteString(sLine);
	file.WriteString(LINE);
}


//////////////////////////////////////////////////////////////////////////////
// Addes space padding to quarantine export line.
// 4/14/98 DBUCHES - Function created / Header added.
//////////////////////////////////////////////////////////////////////////////
void CMainFrame::AddPadding(CString & sLine, int iNewLength)
{
	TCHAR szPadding[80] = {0};
	FillMemory(szPadding, 79, _T(' '));

	int iSpacesNeeded = iNewLength - sLine.GetLength();
	if(iSpacesNeeded > 0)
		szPadding[iSpacesNeeded] = NULL;
	else
		szPadding[1] = NULL; // Not enough space so just add one

	sLine += szPadding;
}


//////////////////////////////////////////////////////////////////////////////
// 6/11/98 DBUCHES, created - header added.
//////////////////////////////////////////////////////////////////////////////
void CMainFrame::OnUpdateFileAdd(CCmdUI* pCmdUI) 
{
	// Get pointer to document
	CQconsoleDoc* pDoc = (CQconsoleDoc*) GetActiveDocument();
	ASSERT(NULL != pDoc);

	pCmdUI->Enable(pDoc->IsQConsoleEnabled());
}


void CMainFrame::OnUpdateViewOptions(CCmdUI* pCmdUI) 
{
	// Get pointer to document
	CQconsoleDoc* pDoc = (CQconsoleDoc*) GetActiveDocument();
	ASSERT(NULL != pDoc);

	pCmdUI->Enable(pDoc->IsQConsoleEnabled());
}


void CMainFrame::OnUpdateFileExport(CCmdUI* pCmdUI) 
{
	// Get pointer to document
	CQconsoleDoc* pDoc = (CQconsoleDoc*) GetActiveDocument();
	ASSERT(NULL != pDoc);

	pCmdUI->Enable(pDoc->IsQConsoleEnabled());
}


// Only for strings in NAVError resource
void CMainFrame::DisplayNAVError(long lErrorID, UINT uMessageID)
{
	STAHLSOFT_HRX_TRY(hr)
	{
		CComBSTR bstrNAVErr(_T("NAVError.NAVCOMError"));
		CComPtr<INAVCOMError> spNavError;
		hrx << spNavError.CoCreateInstance(bstrNAVErr, NULL, CLSCTX_INPROC_SERVER);

		// Populate the error
		hrx << spNavError->put_ModuleID(AV_MODULE_ID_QUARANTINE);
		hrx << spNavError->put_ErrorID(lErrorID);
		hrx << spNavError->put_ErrorResourceID(uMessageID);

		if(NULL == AfxGetMainWnd())
		{
			hrx << spNavError->Show(TRUE, FALSE, 0);
		}
		else
		{
			hrx << spNavError->Show(TRUE, FALSE, reinterpret_cast<unsigned long>(m_hWnd));
		}
	}
	STAHLSOFT_HRX_CATCH_ALL_NO_RETURN(hr)
}


void CMainFrame::DisplayNAVError(long lErrorID, CString sMsg)
{
	STAHLSOFT_HRX_TRY(hr)
	{
		CComBSTR bstrNAVErr(_T("NAVError.NAVCOMError"));
		CComPtr<INAVCOMError> spNavError;
		hrx << spNavError.CoCreateInstance(bstrNAVErr, NULL, CLSCTX_INPROC_SERVER);

		// Populate the error
		hrx << spNavError->put_ModuleID(AV_MODULE_ID_QUARANTINE);
		hrx << spNavError->put_ErrorID(lErrorID);

		CComBSTR bstrErrorMessage(sMsg);
		hrx << spNavError->put_Message(bstrErrorMessage);

		if(NULL == AfxGetMainWnd())
		{
			hrx << spNavError->Show(TRUE, FALSE, 0);
		}
		else
		{
			hrx << spNavError->Show(TRUE, FALSE, reinterpret_cast<unsigned long>(m_hWnd));
		}
	}
	STAHLSOFT_HRX_CATCH_ALL_NO_RETURN(hr)
}
