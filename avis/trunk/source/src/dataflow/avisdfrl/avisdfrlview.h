// AVISDFRLView.h : interface of the CDFLauncherView class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_DFLAUNCHERVIEW_H__BB9BB451_48D9_11D2_B96F_0004ACEC31AA__INCLUDED_)
#define AFX_DFLAUNCHERVIEW_H__BB9BB451_48D9_11D2_B96F_0004ACEC31AA__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#define TERMINATE_EXIT          10000
#define TERMINATE_TIMEOUT       10001

#define TIMEOUT_CHECK_INTERVAL  10000    // 10 seconds

#define TIMEOUT_CHECK_TIMERID       200
#define STATISTICS_WRITE_TIMERID    201
#define VIEW_REFRESH_TIMERID        202

#define PROCESS_EXIT_WAIT_TIME  500

#define SORT_NONE               0
#define SORT_ASCENDING          1
#define SORT_DESCENDING         2

#define SHOW_DIALOG         1
#define SAVE_TO_FILE        2

#include "listvwex.h"
#include "DFResource.h"
#include "ResourceSummary.h"
#include <afxmt.h>

class CDFLauncherView : public CListViewEx
{
public: // create from serialization only
	CDFLauncherView();
	DECLARE_DYNCREATE(CDFLauncherView)

// Attributes

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDFLauncherView)
	public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	virtual void OnInitialUpdate();
	protected:
	virtual void OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint);
	virtual BOOL OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult);
	//}}AFX_VIRTUAL
    void OnHeaderClicked(NMHDR* pNMHDR, LRESULT* pResult) ;
// Implementation
public:
#ifdef _DEBUG
	void LogMessage (DFJob *pDFJob, CString msg);
#endif
	BOOL m_ResourceModified;
	LONG OnDFLauncherJobComplete(WPARAM w, LPARAM l);
	LONG OnALMReqReloadResources(WPARAM w, LPARAM l);
	LONG OnALMReqIgnoreJob(WPARAM w, LPARAM l);
	LONG OnALMReqStartJob(WPARAM w, LPARAM l);
    LONG OnDFLauncherDisableMachine(WPARAM w, LPARAM l);
	void CleanUp();
    CDFResource * GetCurrentSelection(void);
	CDFResourceList m_DFResourceList;
	virtual ~CDFLauncherView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	//{{AFX_MSG(CDFLauncherView)
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnActionShowalmwindow();
	afx_msg void OnActionsAdd();
	afx_msg void OnUpdateActionsAdd(CCmdUI* pCmdUI);
	afx_msg void OnActionsCopy();
	afx_msg void OnUpdateActionsCopy(CCmdUI* pCmdUI);
	afx_msg void OnActionsDisable();
	afx_msg void OnUpdateActionsDisable(CCmdUI* pCmdUI);
	afx_msg void OnActionsEnable();
	afx_msg void OnUpdateActionsEnable(CCmdUI* pCmdUI);
	afx_msg void OnActionsModify();
	afx_msg void OnUpdateActionsModify(CCmdUI* pCmdUI);
	afx_msg void OnActionsRemove();
	afx_msg void OnUpdateActionsRemove(CCmdUI* pCmdUI);
	afx_msg void OnActionsRemoveall();
	afx_msg void OnUpdateActionsRemoveall(CCmdUI* pCmdUI);
	afx_msg void OnResourcesSave();
	afx_msg void OnResourcesLoad();
	afx_msg void OnUpdateResourcesLoad(CCmdUI* pCmdUI);
	afx_msg void OnUpdateResourcesSave(CCmdUI* pCmdUI);
	afx_msg void OnViewSummary();
	afx_msg void OnViewMachinestatistics();
	afx_msg void OnUpdateViewMachinestatistics(CCmdUI* pCmdUI);
	afx_msg void OnUpdateViewSummary(CCmdUI* pCmdUI);
	afx_msg void OnActionsDisableall();
	afx_msg void OnActionsEnableall();
	afx_msg void OnUpdateActionsDisableall(CCmdUI* pCmdUI);
	afx_msg void OnUpdateActionsEnableall(CCmdUI* pCmdUI);
	afx_msg void OnLogFlush();
	afx_msg void OnUpdateLogFlush(CCmdUI* pCmdUI);
	afx_msg void OnLogSettings();
	afx_msg void OnSetupSinglejobpermachine();
	afx_msg void OnUpdateSetupSinglejobpermachine(CCmdUI* pCmdUI);
	afx_msg void OnActionsDisablemachine();
	afx_msg void OnUpdateActionsDisablemachine(CCmdUI* pCmdUI);
	afx_msg void OnActionsEnablemachine();
	afx_msg void OnUpdateActionsEnablemachine(CCmdUI* pCmdUI);
	afx_msg void OnSetupErrorcountforautodisable();
	afx_msg void OnActionsEnablejob();
	afx_msg void OnUpdateActionsEnablejob(CCmdUI* pCmdUI);
	afx_msg void OnActionsDisablejob();
	afx_msg void OnUpdateActionsDisablejob(CCmdUI* pCmdUI);
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	afx_msg void OnSetupDisplayresourceprogresswindow();
	afx_msg void OnUpdateSetupDisplayresourceprogresswindow(CCmdUI* pCmdUI);
	afx_msg void OnViewCookieStatistics();
	afx_msg void OnUpdateViewCookieStatistics(CCmdUI* pCmdUI);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
private:
	COleDateTime m_FinalErrorTime;
	BOOL IsResourceTypePresent (CDFResource *pDFResource);
	void RemoveDuplicateResources();
	BOOL m_ShowAppWindow;
//	CString m_SampleDirUNCPath;
	UINT m_ViewRefreshInterval;
	CMutex m_Mutex;
	BOOL m_SingleJobPerMachine;
	void LoadAppSettings();
	void SaveAppSettings();
	CString m_LogFileName;
	LONG m_LogBufferSize;
	LONG m_LogFileSize;
	LONG m_MaxLogFileSize;
	void LogCompletionMessage(CDFResource * pDFResource, LONG exitCode);
	FILE * m_LogFile;
	ULONG m_AutoDisableCount;
	COleDateTime m_AVISDFStartTime;
	TCHAR m_ComputerName[ MAX_COMPUTERNAME_LENGTH + 1];
	CWinThread * m_StatThread;
	void ShowMachineStatistics(int showType);
	UINT m_StatisticsWriteInterval;
	CResourceSummary * m_pResourceSummaryDlg;
	static int CALLBACK CompareFunc(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort);
	int m_SortOrder;
	int m_SortedColumn;
	BOOL m_TerminateDLL;
	int m_resIndex;
	void ReadResourceList();
    CString m_ModulePath;
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DFLAUNCHERVIEW_H__BB9BB451_48D9_11D2_B96F_0004ACEC31AA__INCLUDED_)
