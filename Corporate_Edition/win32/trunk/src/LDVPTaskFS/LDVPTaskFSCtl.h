// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright (c) 2005 Symantec Corporation. All rights reserved.
//----------------------------------------------------------------
//
//  File: LDVPTaskFSCtl.h
//  Purpose: LDVPTaskFS OCX control definition file
//
//	Date: 1-20-97
//
//	Owner: Ken Knapton
//
//	***** Intel Corp. Confidential ******
//
//----------------------------------------------------------------

#if !defined(TASKS_CTRL_INCLUDED)
#define TASKS_CTRL_INCLUDED

#include "ConfigObj.h"
#include "TaskBase.h"

//----------------------------------------------------------------
//
// CLDVPTaskFSCtrl class
//
//----------------------------------------------------------------
class CLDVPTaskFSCtrl : public CTaskBase
{
	DECLARE_DYNCREATE(CLDVPTaskFSCtrl)

	void ScanStarting();
	void ScanEnding();

// Constructor
public:
	CLDVPTaskFSCtrl();

	//Virtual
	void NotifyNameDescription( const CString &strName, const CString &strDescription );
	CString GetTaskName(){ return m_taskName; };
	CString GetTaskDescriptiveName(){ return m_taskDescriptiveName; };
	CString GetTaskDescription(){ return m_taskDescription; };
	void NotifyEndTask();
	BOOL Recording(){ return m_recording; };
	void SetRecording( BOOL bRecording ){ m_recording = bRecording; };
	BOOL RecordingSchedule(){ return m_recordingSchedule; };
	BOOL SkipFirst(){ return m_bSkipFirst; };
	void BeginRecordingSession(){ FireBeginRecording(); };

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CLDVPTaskFSCtrl)
	public:
	virtual void OnDraw(CDC* pdc, const CRect& rcBounds, const CRect& rcInvalid);
	virtual void DoPropExchange(CPropExchange* pPX);
	virtual void OnResetState();
	//}}AFX_VIRTUAL

// Implementation
protected:
	DWORD GetHiddenDrives();
	~CLDVPTaskFSCtrl();

	DECLARE_OLECREATE_EX(CLDVPTaskFSCtrl)    // Class factory and guid
	DECLARE_OLETYPELIB(CLDVPTaskFSCtrl)      // GetTypeInfo
	DECLARE_PROPPAGEIDS(CLDVPTaskFSCtrl)     // Property page IDs
	DECLARE_OLECTLTYPE(CLDVPTaskFSCtrl)		// Type name and misc status

// Message maps
	//{{AFX_MSG(CLDVPTaskFSCtrl)
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnSetFocus(CWnd* pOldWnd);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

// Dispatch maps
	//{{AFX_DISPATCH(CLDVPTaskFSCtrl)
	BOOL m_recordingSchedule;
	afx_msg void OnRecordingScheduleChanged();
	BOOL m_bSkipFirst;
	afx_msg void OnSkipFirstChanged();
	CString m_helpFilename;
	afx_msg void OnHelpFilenameChanged();
	BOOL m_recording;
	afx_msg void OnRecordingChanged();
	CString m_taskName;
	afx_msg void OnTaskNameChanged();
	CString m_taskDescription;
	afx_msg void OnTaskDescriptionChanged();
	CString m_taskDescriptiveName;
	afx_msg void OnTaskDescriptiveNameChanged();
	afx_msg BOOL Deinitialize();
	afx_msg BOOL EndTask();
	afx_msg BOOL StartTask(long dwID);
	afx_msg BOOL Initialize();
	afx_msg BOOL DoModalSchedule(LPCTSTR lpstrSubKey, short iKey);
	afx_msg BOOL DoModalConfig(long dwPagemask);
	afx_msg BOOL DeleteTask();
	//}}AFX_DISPATCH
	DECLARE_DISPATCH_MAP()

// Event maps
	//{{AFX_EVENT(CLDVPTaskFSCtrl)
	void FireCreateGroup(LPCTSTR szGroup, BOOL bScanningTasks, long lImageID)
		{FireEvent(eventidCreateGroup,EVENT_PARAM(VTS_BSTR  VTS_BOOL  VTS_I4), szGroup, bScanningTasks, lImageID);}
	void FireCreateTask(LPCTSTR szGroupname, LPCTSTR szTaskname, LPCTSTR szTaskDescription, long dwTaskID, long dwImageID)
		{FireEvent(eventidCreateTask,EVENT_PARAM(VTS_BSTR  VTS_BSTR  VTS_BSTR  VTS_I4  VTS_I4), szGroupname, szTaskname, szTaskDescription, dwTaskID, dwImageID);}
	void FireTaskEnding()
		{FireEvent(eventidTaskEnding,EVENT_PARAM(VTS_NONE));}
	void FireTaskStarting()
		{FireEvent(eventidTaskStarting,EVENT_PARAM(VTS_NONE));}
	void FireScanStarting()
		{FireEvent(eventidScanStarting,EVENT_PARAM(VTS_NONE));}
	void FireScanEnding()
		{FireEvent(eventidScanEnding,EVENT_PARAM(VTS_NONE));}
	void FireSaveScan(LPCTSTR szDescriptiveName, LPCTSTR szDescription, LPCTSTR szTaskName)
		{FireEvent(eventidSaveScan,EVENT_PARAM(VTS_BSTR  VTS_BSTR  VTS_BSTR), szDescriptiveName, szDescription, szTaskName);}
	void FireBeginRecording()
		{FireEvent(eventidBeginRecording,EVENT_PARAM(VTS_NONE));}
	//}}AFX_EVENT
	DECLARE_EVENT_MAP()

//	CTaskpadExt* m_pTaskpad;
	
// Dispatch and event IDs
public:
	enum {
	//{{AFX_DISP_ID(CLDVPTaskFSCtrl)
	dispidRecordingSchedule = 1L,
	dispidSkipFirst = 2L,
	dispidHelpFilename = 3L,
	dispidRecording = 4L,
	dispidTaskName = 5L,
	dispidTaskDescription = 6L,
	dispidTaskDescriptiveName = 7L,
	dispidDeinitialize = 8L,
	dispidEndTask = 9L,
	dispidStartTask = 10L,
	dispidInitialize = 11L,
	dispidDoModalSchedule = 12L,
	dispidDoModalConfig = 13L,
	dispidDeleteTask = 14L,
	eventidCreateGroup = 1L,
	eventidCreateTask = 2L,
	eventidTaskEnding = 3L,
	eventidTaskStarting = 4L,
	eventidScanStarting = 5L,
	eventidScanEnding = 6L,
	eventidSaveScan = 7L,
	eventidBeginRecording = 8L,
	//}}AFX_DISP_ID
	};
};

#endif

/*------------------------------------------------------------------
*
* $History: $
*
*
  ------------------------------------------------------------------*/
