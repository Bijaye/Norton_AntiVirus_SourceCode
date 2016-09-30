// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright (c) 2005 Symantec Corporation. All rights reserved.
//----------------------------------------------------------------
//
//  File: MainFrm.h
//  Purpose: CMainFrame Definition file
//
//	Date: 2-3-97
//
//	Owner: Ken Knapton
//
//	***** Intel Corp. Confidential ******
//
//----------------------------------------------------------------

#include "Splitter.h"
#include "ldvpaboutdlg.h"
#include "UnloadDlg.h"

#define WM_STOPSCANNING			WM_USER+100
#define TASK_FOCUS				WM_USER+101
#define UWM_TASKPAD_RESIZED		WM_USER+102
#define UWM_POST_INITIALIZE		WM_USER+103
#define WM_SET_NUM_QUARANTINE_ITEMS  WM_USER+104

BOOL CheckRecording( DWORD dwRecordType = 0 );

//----------------------------------------------------------------
//
// CMainFrame class
//
//----------------------------------------------------------------
class CMainFrame : public CFrameWnd
{

protected: // create from serialization only
	DWORD m_dwNumQuarantineItems;
	BOOL m_bStartupInProgress;
	CMainFrame();
	DECLARE_DYNCREATE(CMainFrame)

    static void WatchDefVersion( CMainFrame* pThis );

private:
	DWORD           m_dwHHCookie;
	BOOL			m_bScanning;
	BOOL			m_bFirstTime;
	CLDVPAboutDlg	m_AboutDlg;
	CString			m_strTitle;
    HKEY            m_hMainKey;

	void StopCurrentScan();
	void EnableMenu( BOOL bEnable );
	void DelSelTask();
	LRESULT OnPostInitialize( WPARAM, LPARAM );

    BOOL CheckForStartUpScans();
	int GetServicesState();
	HRESULT UnloadServices();
	HRESULT LoadServices();
    BOOL GetDefsDate(LPWORD lpwYear, LPWORD lpwMonth, LPWORD lpwDay);
    HRESULT CreateScanConfig(const CLSID &clsidScanner, DWORD RootID,char *SubKey, IScanConfig*& pConfig);
    BOOL IsStartUpScanInProgress(LPSTR FullKey);
	LPARAM OnUpdateDisplayedDefVersion( WPARAM wparam, LPARAM lparam );
    LPARAM OnSetNumQuarantineItems( WPARAM wparam, LPARAM lparam );

public:
	CSplitter	*m_wndSplitter;
	CStatusBar  m_wndStatusBar;
	CToolBar    m_wndToolBar;

	void OnEndTask();
	void SwitchToView( CRuntimeClass *pNewView );
	void ScanStarting();
	void ScanEnding();
	BOOL IsScanning(){ return m_bScanning; };
	BOOL GetFirstTime(){ BOOL bRet = m_bFirstTime; m_bFirstTime = FALSE; return bRet; };

	virtual BOOL OnCommand( WPARAM wparam, LPARAM lparam );
	LRESULT OnTaskFocus( WPARAM wparam, LPARAM lparam );
	LRESULT OnResizeTaskpad( WPARAM wparam, LPARAM lparam );

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMainFrame)
	public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	virtual void WinHelpInternal(DWORD dwData, UINT nCmd = HELP_CONTEXT);
	protected:
	virtual BOOL OnCreateClient(LPCREATESTRUCT lpcs, CCreateContext* pContext);
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CMainFrame();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif


// Generated message map functions
public:
	DWORD CountQuarantineItems(DWORD dwQuarantineFlags, BOOL bServiceNotUpDialog = FALSE);
	static UINT SetNumQuarantineItems(LPVOID pVoid);
	BOOL SetStartupFlag(BOOL bStartup);
	BOOL GetStartupFlag();
	//{{AFX_MSG(CMainFrame)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnDestroy();
	afx_msg void OnRecordNew();
	afx_msg void OnUpdateTaskDelete(CCmdUI* pCmdUI);
	afx_msg void OnTaskDelete();
	afx_msg LRESULT OnStopScanning(WPARAM wparam,LPARAM lparam);
	afx_msg void OnNewScheduled();
	afx_msg void OnContents();
	afx_msg void OnVirusEncyclopedia();
	afx_msg void OnUpdateRecordStartup(CCmdUI* pCmdUI);
	afx_msg void OnRecordStartup();
	afx_msg void OnUpdateRecordNew(CCmdUI* pCmdUI);
	afx_msg void OnUpdateNewScheduled(CCmdUI* pCmdUI);
	afx_msg void OnUpdatePatternfile();
	afx_msg void OnScheduleUpdates();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnGetMinMaxInfo( MINMAXINFO* lpMMI );
	afx_msg void OnSizing(UINT fwSide, LPRECT pRect);
	afx_msg void OnConfigHistory();
	afx_msg void OnUpdateUpdatePatternfile(CCmdUI* pCmdUI);
	afx_msg void OnUpdateScheduleUpdates(CCmdUI* pCmdUI);
	afx_msg void OnInitMenu(CMenu* pMenu);
	afx_msg void OnViruslist();
	afx_msg void OnExitMenuLoop(BOOL bIsTrackPopupMenu);
	afx_msg void OnCut();
	afx_msg void OnPaste();
	afx_msg void OnLoadServices();
	afx_msg void OnUpdateLoadServices(CCmdUI* pCmdUI);
    afx_msg void OnClose();
	//}}AFX_MSG
//	afx_msg void OnRegister();

	DECLARE_MESSAGE_MAP()

};
