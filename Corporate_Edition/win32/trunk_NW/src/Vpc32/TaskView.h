// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright (c) 2005 Symantec Corporation. All rights reserved.
//----------------------------------------------------------------
//
//  File: TaskView.h
//  Purpose: CTaskView Definition file
//
//	Date: 2-3-97
//
//	Owner: Ken Knapton
//
//	***** Intel Corp. Confidential ******
//
//----------------------------------------------------------------
#if !defined (TASK_VIEW_INCLUDED)
#define TASK_VIEW_INCLUDED

#include "Taskpad.h"
#include "LDVPTaskfs.h"

/////////////////////////////////////////////////////////////////////////////
// CTaskView view

class CTaskView : public CView
{
public:
	void DelTask();

protected:
	CTaskView();           // protected constructor used by dynamic creation
	DECLARE_DYNCREATE(CTaskView)

private:
	CLDVPTaskFS *m_ptrTaskOCX;
	CTaskpad	*m_ptrTaskPad;
	BOOL		m_bShowedPleaseWait;

	CString GenerateUniqueKey();

// Attributes
protected:
	afx_msg void OnCreateTask(LPCTSTR GroupName, LPCTSTR TaskName, LPCTSTR TaskDescript, long TaskID, long lImageID);
	afx_msg void OnCreateGroup(LPCTSTR szGroup, BOOL bScanningTasks, long lImageID);
	afx_msg void OnTaskEnding();
	afx_msg void OnTaskStarting();
	afx_msg void OnScanStarting();
	afx_msg void OnScanEnding();
	afx_msg void OnNotifyEndTask();
	afx_msg void OnSaveScan(LPCTSTR szDescriptiveName, LPCTSTR szDescription, LPCTSTR szTaskname);
	afx_msg void OnBeginRecording();
	DECLARE_EVENTSINK_MAP();

// Operations
public:
	BOOL		m_bAlreadySetFocus;

	void SetTaskPadPtr( CTaskpad *ptrTaskpad ){ m_ptrTaskPad = ptrTaskpad; };
	void Initialize();
	void StartTask( DWORD dwID, CString strTaskName, const CString &strTaskDescriptiveName, const CString &strDescription,  BOOL bDoubleClick = FALSE );
	void EndTask();
	void SelectTask( DWORD dwID, CString strCategory );
	void SelectTask( WORD dwID, WORD byteGroupID );

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTaskView)
	public:
	virtual void OnInitialUpdate();
	virtual BOOL Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, CCreateContext* pContext = NULL);
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual void OnDraw(CDC* pDC);      // overridden to draw this view
	//}}AFX_VIRTUAL

// Implementation
protected:
	virtual ~CTaskView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	// Generated message map functions
protected:
	//{{AFX_MSG(CTaskView)
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnDestroy();
	afx_msg void OnSetFocus(CWnd* pOldWnd);
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

#endif
