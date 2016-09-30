// MainFrm.h : interface of the CMainFrame class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_MAINFRM_H__BB9BB44D_48D9_11D2_B96F_0004ACEC31AA__INCLUDED_)
#define AFX_MAINFRM_H__BB9BB44D_48D9_11D2_B96F_0004ACEC31AA__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

class CMainFrame : public CFrameWnd
{
public: // create from serialization only
	CMainFrame();
	DECLARE_DYNCREATE(CMainFrame)

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMainFrame)
	public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CMainFrame();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif
	CStatusBar  m_wndStatusBar;

protected:  // control bar embedded members

    // Use the extended toolbar class CToolBarEx to display the toolbar in
    // the new style as used by MS Office etc.
	CToolBar    m_wndToolBar;

// Generated message map functions
protected:
	//{{AFX_MSG(CMainFrame)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnClose();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
private:
	afx_msg LONG OnDFLauncherJobComplete(WPARAM w, LPARAM l);
	afx_msg LONG OnALMReqStartJob (WPARAM w, LPARAM l);
	afx_msg LONG OnALMReqReloadResources(WPARAM w, LPARAM l);
	afx_msg LONG OnALMReqIgnoreJob(WPARAM w, LPARAM l);
	afx_msg LONG OnDFLauncherDisableMachine(WPARAM w, LPARAM l);
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MAINFRM_H__BB9BB44D_48D9_11D2_B96F_0004ACEC31AA__INCLUDED_)
