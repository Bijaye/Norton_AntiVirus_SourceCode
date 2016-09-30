/////////////////////////////////////////////////////////////////////////////
// $Header:   S:/QCONSOLE/VCS/mainfrm.h_v   1.7   11 Jun 1998 15:48:26   DBuches  $
/////////////////////////////////////////////////////////////////////////////
//
// MainFrm.h : interface of the CMainFrame class
//
//
/////////////////////////////////////////////////////////////////////////////
// $Log:   S:/QCONSOLE/VCS/mainfrm.h_v  $
// 
//    Rev 1.7   11 Jun 1998 15:48:26   DBuches
// Added support for trial ware.
// 
//    Rev 1.6   07 May 1998 18:18:24   DBuches
// Added code to remove LiveUpdate buttun when LiveUpdate is not present on the system.
// 
//    Rev 1.5   14 Apr 1998 00:43:10   DBuches
// Hooked up export dialog.
// 
//    Rev 1.4   03 Apr 1998 13:28:14   DBuches
// Added handlers for options and LiveUpdate.
// 
//    Rev 1.3   13 Mar 1998 15:20:50   DBuches
// Added text to toolbar.
// 
//    Rev 1.2   06 Mar 1998 11:22:12   DBuches
// Added coolbars.
// 
//    Rev 1.1   03 Mar 1998 17:03:58   DBuches
// Added handler for quarantine mode.
// 
//    Rev 1.0   27 Feb 1998 15:10:26   DBuches
// Initial revision.
/////////////////////////////////////////////////////////////////////////////


#if !defined(AFX_MAINFRM_H__396DC35D_A5D8_11D1_A51E_0000C06F46D0__INCLUDED_)
#define AFX_MAINFRM_H__396DC35D_A5D8_11D1_A51E_0000C06F46D0__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "coolmenu.h"
#include "coolbar.h"
#include "iquaran.h"

/////////////////////////////////////////////////////////////////////////////
// Extra spiffy splitter class.  This splitter allows a us to hide a pane 
// of a static splitter. 

class CSplitterWndEx : public CSplitterWnd
{
protected:
    int m_nHiddenCol;
    int m_iSavedSize;
    
public:
    CSplitterWndEx();
    void ShowColumn();
    void HideColumn(int colHide);
    void SetSavedSize( int iSize ) { m_iSavedSize = iSize; }

    // ClassWizard generated virtual function overrides
    //{{AFX_VIRTUAL(CSplitterWndEx)
    //}}AFX_VIRTUAL
    
    // Generated message map functionsprotected:
    //{{AFX_MSG(CSplitterWndEx)
    // NOTE - the ClassWizard will add and remove member functions here.
    //}}AFX_MSG

    DECLARE_MESSAGE_MAP()
};


/////////////////
// CQuarantineCoolBar
//
class CQuarantineCoolBar : public CCoolBar {
protected:
    DECLARE_DYNAMIC(CQuarantineCoolBar)
    CCoolToolBar    m_wndToolBar;            // toolbar
    virtual BOOL   OnCreateBands();
//    BOOL IsLiveUpdateInstalled();

public:
    static int GetTextWidthHelper(CString& sText, CSize* pSize = NULL);
    



};


/////////////////////////////////////////////////////////////////////////////
// CMainFrame class


class CMainFrame : public CFrameWnd
{
protected: // create from serialization only
	CMainFrame();
	DECLARE_DYNCREATE(CMainFrame)


    CCoolMenuManager m_menuMgr;

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMainFrame)
	public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	protected:
	virtual BOOL OnCreateClient(LPCREATESTRUCT lpcs, CCreateContext* pContext);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CMainFrame();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:  // control bar embedded members
	CStatusBar  m_wndStatusBar;
	CToolBar    m_wndToolBar;
    CQuarantineCoolBar m_wndCoolBar;
    CSplitterWndEx m_wndSplitter;

    BOOL        m_bShowTree;

// Generated message map functions
protected:
	//{{AFX_MSG(CMainFrame)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnViewDetails();
	afx_msg void OnUpdateViewDetails(CCmdUI* pCmdUI);
	afx_msg void OnViewLargeicons();
	afx_msg void OnUpdateViewLargeicons(CCmdUI* pCmdUI);
	afx_msg void OnViewList();
	afx_msg void OnUpdateViewList(CCmdUI* pCmdUI);
	afx_msg void OnViewSmallicons();
	afx_msg void OnUpdateViewSmallicons(CCmdUI* pCmdUI);
	afx_msg void OnViewRefresh();
	afx_msg void OnViewLiveupdate();
	afx_msg void OnDestroy();
	afx_msg void OnViewTree();
    afx_msg void OnFileAdd();
	afx_msg void OnUpdateViewTree(CCmdUI* pCmdUI);
	afx_msg void OnViewShowAll();
    afx_msg void OnViewShowQuarantine();
	afx_msg void OnViewShowBackup();
	afx_msg void OnViewShowRepaired();
	afx_msg void OnViewShowSubmitted();
    afx_msg void OnUpdateViewShow(CCmdUI* pCmdUI);
    afx_msg void OnSortMenuItem( UINT nID );
    afx_msg void OnUpdateSortMenuItem(CCmdUI* pCmdUI);
	afx_msg void OnViewOptions();
	afx_msg void OnFileExport();
	afx_msg void OnUpdateFileAdd(CCmdUI* pCmdUI);
	afx_msg void OnUpdateViewOptions(CCmdUI* pCmdUI);
	afx_msg void OnUpdateFileExport(CCmdUI* pCmdUI);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:
    void Export( CString &sFileName, CMapPtrToPtr* pI, BOOL bComma = FALSE  );
    void WriteCommaDelimitedItem( CStdioFile &file, IQuarantineItem* pItem );
    void WriteStandardItem( CStdioFile &file, IQuarantineItem* pItem );
    void AddPadding( CString & sLine, int iNewLength );


};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MAINFRM_H__396DC35D_A5D8_11D1_A51E_0000C06F46D0__INCLUDED_)
