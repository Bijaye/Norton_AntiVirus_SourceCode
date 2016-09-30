// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright (c) 2005 Symantec Corporation. All rights reserved.
//----------------------------------------------------------------
//
//  File: TaskBase.h
//  Purpose: Task base class 
//
//	Date: 2-2-98
//
//	Owner: Ken Knapton
//
//	***** Intel Corp. Confidential ******
//
//----------------------------------------------------------------

#if !defined(AFX_TASKBASE_H__DEA6DCDB_9C0F_11D1_8208_00A0C95C0756__INCLUDED_)
#define AFX_TASKBASE_H__DEA6DCDB_9C0F_11D1_8208_00A0C95C0756__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// TaskBase.h : header file
//

//Defines for Type to be sent in to OPen
#define TYPE_STANDARD	0
#define TYPE_REALTIME	1
#define TYPE_IDLE		2

#define UWM_SCAN_STARTING		5021
#define UWM_SCAN_ENDING			5022
#define UWM_END_SCAN			5023
#define UWM_DESTROYSCAN			5024

//Forward define for CWizard
class CWizard;

/////////////////////////////////////////////////////////////////////////////
// CTaskBase window
class CTaskBase : public COleControl
{
protected:

	DWORD	m_dwActiveTask;
	CWizard *m_ptrWizard;
	HWND	m_hWndWizard;
	BOOL	m_bCanSkipFirst;
	BOOL	m_bModalConfigDisplayed;
	BOOL	m_bUserTask;
// Construction
public:
	CTaskBase(){};


	BOOL IsUserTask(){ return m_bUserTask; };

	//Virtual methods
	virtual CString GetTaskName() = 0;
	virtual void NotifyNameDescription( const CString &strName, const CString &strDescription ) = 0;
	virtual CString GetTaskDescriptiveName() = 0;
	virtual CString GetTaskDescription() = 0;
	virtual void NotifyEndTask() = 0;
	virtual BOOL Recording() = 0;
	virtual void SetRecording( BOOL bRecording ) = 0;
	virtual BOOL RecordingSchedule() =0;
	virtual BOOL SkipFirst() = 0;
	virtual void BeginRecordingSession() = 0;

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTaskBase)
	//}}AFX_VIRTUAL

// Implementation
public:

	// Generated message map functions
protected:
	//{{AFX_MSG(CTaskBase)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG

};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TASKBASE_H__DEA6DCDB_9C0F_11D1_8208_00A0C95C0756__INCLUDED_)
