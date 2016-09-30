#if !defined(AFX_CHIDDENWNDMMCTHREAD_H__FB2E3B2C_69EA_4044_A809_900A04700707__INCLUDED_)
#define AFX_CHIDDENWNDMMCTHREAD_H__FB2E3B2C_69EA_4044_A809_900A04700707__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// HiddenWndMmcThread.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CHiddenWndMmcThread window
class CHiddenWndMmcThread : public CWnd
{
// Construction
public:
	CHiddenWndMmcThread();


// Attributes
public:

    //CQSConsoleData    *m_pRootNode;    
	//CQSConsoleComponent* m_pCQSConsoleComponent;
    // CServerConfigData  Object
//    CHiddenWndMmcThread*  m_pHiddenWndMmcThread;
//    CHiddenWndMmcThread*  m_pWnd;
    CServerConfigData*    m_pSConfigData;
    DWORD                 m_dwEntryThreadID;
    DWORD                 m_dwRegWriteThreadID;

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CHiddenWndMmcThread)
	protected:
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CHiddenWndMmcThread();

	// Generated message map functions
protected:
	//{{AFX_MSG(CHiddenWndMmcThread)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CHIDDENWNDMMCTHREAD_H__FB2E3B2C_69EA_4044_A809_900A04700707__INCLUDED_)
