// AVISMonitorView.h : interface of the CChildView class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_CHILDVIEW_H__66A8FF4D_D7CB_11D2_B98E_0004ACEC31AA__INCLUDED_)
#define AFX_CHILDVIEW_H__66A8FF4D_D7CB_11D2_B98E_0004ACEC31AA__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "listvwex.h"
#include "AVISModule.h"

#define MODULE_CHECK_TIMER          101
#define VIEW_REFRESH_TIMERID        102
#define MODULE_AUTO_START_TIMERID   103
#define MODULE_AUTO_STOP_TIMERID    104
#define MODULE_RESTART_TIMERID      105

#define SORT_NONE               0
#define SORT_ASCENDING          1
#define SORT_DESCENDING         2

#define TERMINATE_USER_STOP     1001
#define TERMINATE_INACTIVE      1002
#define TERMINATE_EXIT          1003
#define TERMINATE_AUTO_STOP     1004

/////////////////////////////////////////////////////////////////////////////
// CAVISMonitorView window

class CAVISMonitorView : public CListViewEx
{
// Construction
public:
	CAVISMonitorView();
	DECLARE_DYNCREATE(CAVISMonitorView)

// Attributes
public:

private:
    CAVISModuleList m_ModuleList;

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAVISMonitorView)
	public:
	virtual void OnInitialUpdate();
	protected:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	virtual void OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint);
	//}}AFX_VIRTUAL

// Implementation
public:
	void SendWMCloseMessage (CAVISModule * pAVISModule);
	void TerminateAllOngoingProcess();
	virtual ~CAVISMonitorView();

private:
	BOOL m_ModuleStartOnStartup;
	void SaveConfigFile();
	void ReadConfigFile();
	COleDateTime m_NextCheckTime;
	COleDateTime m_StartTime;
	void WriteModuleInfoToFile();
	void ModuleCheckStatus (CAVISModule *pAVISModule);
	void ModuleCheckExternalStatus (CAVISModule *pAVISModule);
	void StartModule (CAVISModule *pAVISModule);
//	BOOL m_ModuleAutoRestart;
	UINT m_ModuleTerminateErrorCount;
	void ReadModuleList(void);
	CString m_ModulePath;
	CAVISModule * GetCurrentSelection(void);
	UINT m_ViewRefreshInterval;
	UINT m_SortedColumn;
	UINT m_SortOrder;
	static int CALLBACK CompareFunc(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort);
	UINT m_ModuleCheckInterval;
	BOOL m_ModuleModified;
	TCHAR m_ComputerName[ MAX_COMPUTERNAME_LENGTH + 1];

	// Generated message map functions
protected:
	//{{AFX_MSG(CAVISMonitorView)
	afx_msg void OnActionsAdd();
	afx_msg void OnActionsRemove();
	afx_msg void OnUpdateActionsRemove(CCmdUI* pCmdUI);
	afx_msg void OnActionsStart();
	afx_msg void OnUpdateActionsStart(CCmdUI* pCmdUI);
	afx_msg void OnActionsStop();
	afx_msg void OnUpdateActionsStop(CCmdUI* pCmdUI);
	afx_msg void OnSetupOptions();
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnMonitorLoad();
	afx_msg void OnUpdateMonitorLoad(CCmdUI* pCmdUI);
	afx_msg void OnMonitorSave();
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	afx_msg void OnActionsEdit();
	afx_msg void OnUpdateActionsEdit(CCmdUI* pCmdUI);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CHILDVIEW_H__66A8FF4D_D7CB_11D2_B98E_0004ACEC31AA__INCLUDED_)
