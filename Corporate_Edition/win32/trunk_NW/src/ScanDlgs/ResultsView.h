//////////////////////////////////////////////////////////////////////////////////
// PROPRIETARY/CONFIDENTIAL.  Use of this product is subject to license terms.	//
// Copyright © 2004, 2005 Symantec Corporation.  All rights reserved.					//
//////////////////////////////////////////////////////////////////////////////////
//{{AFX_INCLUDES()
#include "ldvpresults.h"
#include "OneLine.h"	// Added by ClassView
//}}AFX_INCLUDES
#if !defined(AFX_RESULTSVIEW_H__B87C2D73_5D4B_11D1_AB11_00A0C90F8F6F__INCLUDED_)
#define AFX_RESULTSVIEW_H__B87C2D73_5D4B_11D1_AB11_00A0C90F8F6F__INCLUDED_

#if _MSC_VER >= 1000 
#pragma once
#endif // _MSC_VER >= 1000
// ResultsView.h : header file
//
#include "StaticPath.h"
#include "ItemStorage.h"
#include "wndsize.h"

typedef struct
{
	LPARAM Data;
	CString Description;
} LLSTUFF;



const int DATE_COMBOBOX_ID = 777; // id for date toolbar combobox
const int CM_SHOWSCAN = WM_USER + 6;
const int CM_RESIZECONTROLS = WM_USER + 7;
	// This message will force the window to resize dynamically sized controls.

// Custom toolbar to handle a ComboBox on a toolbar
class CCustomBar : public CToolBar  
{
public:
	CLDVPResults *m_pResults;
	CComboBox	 *m_pCombo;
	virtual BOOL OnCommand( WPARAM wParam, LPARAM lParam );
};



/////////////////////////////////////////////////////////////////////////////
// CResultsView dialog

class CResultsView : public CDialog
{
private:
	CFont			m_font;
	CCustomBar		m_toolBar;			// interits from CToolBar
	BOOL			m_bInitialized;
	CItemStorage	m_ItemStorage;		// infection storage
	BOOL			m_bDetails;			// is this a detail window?
	CResultsView*	m_pDetailView;
	CComboBox		m_datebox;
	HICON			m_hIcon;
	CPtrArray		m_MyAllocs;
    bool            m_bShowOnFirstThreat;
    bool            m_bDisplayDialogInViewThread;
    bool            m_bAutoRebootOnDialogClose;
    bool            m_bCloseDialogAfterRiskRepair;
    CWinThread     *m_pViewThread;
    CWindowResizer  m_oWndSizer;
    // Build a list of all items which need prompting.
	CStringList     m_strlistPromptItems;


// Construction
public:
	void UpdateView();
	BOOL IsHTMLHelpEnabled ( void );
	~CResultsView();
	CPtrArray m_arSelectedItems;

	CResultsView(CWnd* pParent = NULL);   // standard constructor
	DWORD Open(PRESULTSVIEW ResultsView);
	DWORD AddLogLine( LPARAM Data, char* Line );
	DWORD AddLogLine( LPARAM Data, char* Line, int cSize );
	DWORD AddProgress(PPROGRESSBLOCK Block);
	DWORD ShowScan(LPCSTR sComputer,long ScanID);
	void InitToolbar(DWORD type);
	static BOOL CALLBACK EnumWindowsProc( HWND hwnd,LPARAM lParam);
	BOOL InitScanner(LPTSTR lpszScanOptions);
    void DeInitScanner();
	BOOL SelectionContainsViralThreat( void );
    HANDLE GetViewThreadHandle () const;
    void ScanDone();
	BOOL InitCliscanScanner(void);
	void DeInitCliscanScanner(BOOL bInitScanEngine);

    bool GetCloseDialogAfterRiskRepair( void ) { return m_bCloseDialogAfterRiskRepair; };
    void SetCloseDialogAfterRiskRepair( bool bCloseDialogAfterRiskRepair ) { m_bCloseDialogAfterRiskRepair = bCloseDialogAfterRiskRepair; };

    bool GetAutoRebootOnDialogClose( void ) { return m_bAutoRebootOnDialogClose; };
    void SetAutoRebootOnDialogClose( bool bAutoRebootOnDialogClose ) { m_bAutoRebootOnDialogClose = bAutoRebootOnDialogClose; };

    IVBin2*             m_pVBin;
	IScanConfig*		m_pScanConfig;  // live scanning stuff
	IScan2*				m_pScan;
	IScanCallback*		m_pScanCallback;
    IVirusProtect*      m_pRoot;

	BOOL m_bPaused;
	BOOL m_Modeless;
	char m_tmp[512];
	HWND m_Parent;
	HWND m_hWndMain;
	GETFILESTATEFORVIEW m_GetFileStateForView;
	TAKEACTION  m_TakeAction;
	TAKEACTION2 m_TakeAction2;
	GETCOPYOFFILE m_GetCopyOfFile;
	VIEWCLOSED m_ViewClosed;
    EXCLUDEITEM m_pfnExcludeItem;
    GETVBINDATA m_GetVBinData;
	DWORD m_Flags;
	DWORD m_Type;
	DWORD m_dwScanID;
	DWORD m_bStopTheScan;
	CString m_Title;
	PROGRESSBLOCK m_Progress;
	DWORD m_ProgCount;
	UINT m_Timer;
	void *m_Context;
	CStatusBar m_Status;
	BOOL m_AviRunning;
	BOOL m_bHasData;
	DWORD m_time;
	CString	m_strFilterSettingsKey;
	DWORD m_dwScanDelay;
	DWORD m_dwDelayCount;
	DWORD m_dwMaxDelay;
	DWORD m_dwAllow4Hour;
	DWORD m_dwMaxPause;
	BOOL m_bMaxedOut;
	BOOL m_bMaxedOutUI;
	BOOL m_bViewOpenedRemotely;
    DWORD m_dwMinY;
    DWORD m_dwMinX;
	BOOL m_bRemovingRisks; // Is this dialog in the process of removing risks?

// Dialog Data
	//{{AFX_DATA(CResultsView)
	enum { IDD = IDD_RESULTS_VIEW };
	CProgressCtrl	m_ctlProgress;
	CEdit	m_filename;
	CStaticPath	m_filePath;
    CStaticPath m_GenericWarning;
	CAnimateCtrl	m_Avi;
	CLDVPResults	m_oLDVPResultsCtrl;
    CButton m_oTerminateProcess;
    CButton m_oClose;
    CStatic m_oWarningIcon;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CResultsView)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual void PostNcDestroy();
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
	//}}AFX_VIRTUAL

// Implementation


	
protected:
	void OnHTMLHelp ( );
	DWORD FreeSelectedItems();
	DWORD GetSelectedItems();
	BOOL IsScanDeliverEnabled();
    void SetDisplayingResultsViewRegValue (bool bDisplaying);

	void OnStop();
	void OnPlay();
	afx_msg  LONG OnShowScan( WPARAM wparam, LPARAM lparam );
	BOOL ShowWindow(int nCmdShow );
	afx_msg  LONG OnResizeControls( WPARAM wUnused, LPARAM lUnused );

    void SetNotifications();
	void GetOverallStatus(bool *pbReboot,
						  bool *pbRemoveRisk,
						  bool *pbTookAction,
						  bool *pbAnalyzing);

    static void ViewThread (CResultsView *pThis);

    static void ActOnPromptItems( CResultsView* pThis );
    void GetItemsNeedingPrompt( );
	void TerminateProcesses();
	void RebootSystem();

	void ExpandResultsCtrlVertically();
	void ExcludeCheckedItems();

	// Generated message map functions
	//{{AFX_MSG(CResultsView)
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnDestroy();
	virtual BOOL OnInitDialog();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnReport();
	afx_msg void OnDetails();
	afx_msg void OnFilter();
	afx_msg void OnActions();
	afx_msg void OnExport();
	afx_msg void OnHelp();
	afx_msg void OnConfigHistory();
	afx_msg void OnClickClose();
	afx_msg void OnDestroyingItemLdvpresults(long lParam);
	afx_msg void OnPause();
	afx_msg void OnClose();
	afx_msg void OnGetMinMaxInfo( MINMAXINFO* lpMMI );
	afx_msg void OnActionRequestedLdvpresults(long lAction);
	afx_msg void OnShowDetailsLdvpresults(long ID, LPCTSTR ComputerName, BOOL bInProgress, long lParam);
	afx_msg void OnSelectionChangedLdvpresults(BOOL IsOldLogLine);
	afx_msg void OnPaint();
	afx_msg void OnScandeliver();
	afx_msg void OnRepair();
	afx_msg void OnRestore();
	afx_msg void OnDelete();
	afx_msg void OnAddToQuarantine();
	afx_msg void OnPurgeQuarantine();
	afx_msg void OnUndo();
	afx_msg void OnQuarantineInfectedItem();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnFilterEventsLdvpresults();
	afx_msg void OnScanDeliverLdvpresults();
	afx_msg void OnAddToQuarantineLdvpresults();
	afx_msg void OnStarttheScanLdvpresults();
	afx_msg void OnPausetheScanLdvpresults();
	afx_msg void OnStoptheScanLdvpresults();
    afx_msg void OnTerminateProcess();
    afx_msg void OnRebootSystem();
    afx_msg void OnButtonClose();
	DECLARE_EVENTSINK_MAP()
	//}}AFX_MSG
	afx_msg BOOL OnTTT(UINT,NMHDR *NotifyMessageHeader,LRESULT *);
	DECLARE_MESSAGE_MAP()
public:
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_RESULTSVIEW_H__B87C2D73_5D4B_11D1_AB11_00A0C90F8F6F__INCLUDED_)
