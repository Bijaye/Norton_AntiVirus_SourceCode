// MainFrm.h : interface of the CMainFrame class
//


#pragma once
#include "stdafx.h"
#include "NavigationBar.h"
#include "HelpBar.h"

class CMainFrame : public CFrameWnd
{
protected: // create from serialization only
	CMainFrame();
	DECLARE_DYNCREATE(CMainFrame)

// Attributes
public:

// Operations
public:

// Overrides
public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);

// Implementation
public:
	virtual ~CMainFrame();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

    CHelpBar* GetHelpBar();
// Generated message map functions
protected:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
    afx_msg void OnClose();
    virtual void OnUpdateFrameTitle( BOOL addToTitle );

	DECLARE_MESSAGE_MAP()

    CNavigationBar  navBar;
    CHelpBar        helpBar;
    CBrush          backgroundBrush;
};


