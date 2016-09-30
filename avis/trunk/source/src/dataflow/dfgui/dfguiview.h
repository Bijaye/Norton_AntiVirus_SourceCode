// DFGuiView.h : interface of the CDFGuiView class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_DFGUIVIEW_H__A244E3BE_5A46_11D2_A583_0004ACECC1E1__INCLUDED_)
#define AFX_DFGUIVIEW_H__A244E3BE_5A46_11D2_A583_0004ACECC1E1__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000


#define MSG_TRANSIT					WM_USER+500
#define TIMER_PROCESS_ID			WM_USER+501
#define TIMER_UPDATE_ID				WM_USER+502
#define TIMER_ARRIVAL_ID			WM_USER+503
#define TIMER_DATABASE_RESCAN_ID	WM_USER+504
#define TIMER_DEFINITION_IMPORT_ID	WM_USER+505
#define TIMER_STATUS_UPDATE_ID		WM_USER+506
#define TIMER_UNDEFERRER_ID		    WM_USER+507
#define TIMER_SAMPLEATTRIBUTE_ID	WM_USER+508
#define TIMER_THREAD_ID	            WM_USER+509
#define TIMER_UPDATELIST_ID	        WM_USER+510
#define MSG_TRANSIT1				WM_USER+511
#define MSG_IMMCLOSE				WM_USER+512
#define MSG_REQUEST_CLOSE			WM_USER+513


#define TIMEOUT_UPDATE_INTERVAL		60000      // (msec)



class CDFGuiView : public CListViewEx
{
protected: 
	CDFGuiView();
	DECLARE_DYNCREATE(CDFGuiView)
    
// Attributes
public:
 	CDFGuiDoc* GetDocument();
// Operations
public:
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDFGuiView)
	public:
	virtual void OnDraw(CDC* pDC);  // overridden to draw this view
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	protected:
	virtual void OnInitialUpdate(); // called first time after construct
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
	virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);
	//}}AFX_VIRTUAL

// Implementation
public:
	enum UserRequest {UNDEFINED, ENABLE, DISABLE};  
	enum SORT {DESCENDING, ASCENDING, NONE }; 
	enum ArrivalType {Init, Back, ToMax }; 
	enum TimerType { Both, Imported, Rescan }; 

    DFManager Manager;          // Manager's instance
	int killFlag;               // incation to kill the manager
    UserRequest request;        // user's request to
								// hold/im hold/resume/enable/disable samples      
	CImageList  m_image;        // image list to display bitmaps for samples in 
	                            // progress or in waiting status
	CBitmap  mybitmap;          // bitmap instance
	CBitmap  mybitmap4;         // bitmap instance
	CBitmap  mybitmap5;         // bitmap instance
	CBitmap  mybitmap6;         // bitmap instance
	CBitmap scanBitmap;
    int submitCount;            // submit count    
	int OnLauncherShow();       // show launcher's window 
	virtual ~CDFGuiView();    

	long	MaxSamples;
	long	SubmissionInterval;
	long	ArrivalInterval;
	long	DatabaseRescanInterval;
	long	DefImportInterval;
	long	StatusUpdateInterval;
	long	UndeferrerInterval;
	long	AttributeInterval;
	CString UNCPath;
	CString buildDefFilename;
	CString defBaseDir;
	CString defImporterDir;
	CString sampleDirPath;
	int     nSortedCol; 
    long    bSortOrder; 
	long    initFlag; 
    int     returnCode; 
    int		sendFlag;  
	long    enteredArrivalInterval;
	long    enteredDatabaseRescanInterval;
	std::ofstream *testFile;         
	long    logFlag; 
	long	sortFlag;

#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	int GetCurSel(void);                       // get the cursor's postion in the list control
    DFSample * FindSampleObject(int& nSel, POSITION& pos); // find sample object
	int FindSampleInList(DFSample *sample);    // find an object in the list control
	int UpdateSampleList();                    // update the list control
    int OnJobFromLauncher(WPARAM p1, LPARAM p2);// action on the message from Launcher  
 	int SetUserRequest(DFSample::UserRequest request); //set up user's request 
    void TermView();                            // terminate gui
	BOOL IsAtLeastOneSelectedNotOnHold();       // determine if there is at least one selection not on hold    
	BOOL IsAtLeastOneSelectedOnHold();          //determine if there is at least one selection on hold 
	BOOL AreAllSelectedNotOnHold();
	BOOL AreAllSelectedOnHold();
	BOOL AreAllSelectedNotInDefer(); 
	void OnSendMail(); 
	int CorrectArrivalInterval(ArrivalType type, TimerType timerType);
	void SaveConfigSettings();
	void OnHeaderClicked(NMHDR* pNMHDR, LRESULT* pResult); //sort columns
//	static int CALLBACK CompareFunc(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort);
    BOOL SortTextItems(int nCol, BOOL bAscending, int low, int high);
	void UpdateHeaderIcons();
	void SortList(); 
	BOOL SortNumericItems( int nCol, BOOL bAscending, int low /*= 0*/, int high /*= -1*/ );
	int  OnAfterThread(WPARAM p1, LPARAM p2);
	int  OnImmClose(WPARAM p1, LPARAM p2);

protected:
	//{{AFX_MSG(CDFGuiView)
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnClose();
	afx_msg void OnUpdateHold(CCmdUI* pCmdUI);
	afx_msg void OnUpdateStop(CCmdUI* pCmdUI);
	afx_msg void OnUpdateResume(CCmdUI* pCmdUI);
	afx_msg void OnHold();
	afx_msg void OnStop();
	afx_msg void OnResume();
	afx_msg void OnNcDestroy();
	afx_msg void OnDisable();
	afx_msg void OnUpdateDisable(CCmdUI* pCmdUI);
	afx_msg void OnUpdateEnable(CCmdUI* pCmdUI);
	afx_msg void OnEnable();
	afx_msg void OnUpdateHoldall(CCmdUI* pCmdUI);
	afx_msg void OnHoldall();
	afx_msg void OnResumeall();
	afx_msg void OnUpdateResumeall(CCmdUI* pCmdUI);
	afx_msg void OnConfigure();
	afx_msg void OnUpdateConfigure(CCmdUI* pCmdUI);
	afx_msg void OnStatistics();
	afx_msg void OnUpdateStatistics(CCmdUI* pCmdUI);
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	afx_msg void OnChangepriority();
	afx_msg void OnUpdateChangepriority(CCmdUI* pCmdUI);
	afx_msg void OnDefer();
	afx_msg void OnUpdateDefer(CCmdUI* pCmdUI);
	afx_msg void OnStopimport();
	afx_msg void OnUpdateStopimport(CCmdUI* pCmdUI);
	afx_msg void OnStopundeferrer();
	afx_msg void OnUpdateStopundeferrer(CCmdUI* pCmdUI);
	afx_msg void OnStopupdater();
	afx_msg void OnUpdateStopupdater(CCmdUI* pCmdUI);
	afx_msg void OnSampleattributes();
	afx_msg void OnUpdateSampleattributes(CCmdUI* pCmdUI);
	afx_msg void OnStopattrcollector();
	afx_msg void OnUpdateStopattrcollector(CCmdUI* pCmdUI);
	afx_msg void OnDestroy();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

#ifndef _DEBUG  // debug version in DFGuiView.cpp
inline CDFGuiDoc* CDFGuiView::GetDocument()
   { return (CDFGuiDoc*)m_pDocument; }
#endif

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DFGUIVIEW_H__A244E3BE_5A46_11D2_A583_0004ACECC1E1__INCLUDED_)
