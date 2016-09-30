// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright (c) 2005 Symantec Corporation. All rights reserved.
//----------------------------------------------------------------
//
//  File: Taskpad.h
//  Purpose: CTaskPad Definition file
//
//	Date: 2-3-97
//
//	Owner: Ken Knapton
//
//	***** Intel Corp. Confidential ******
//
//----------------------------------------------------------------

#ifndef __TASKPAD_H
#define __TASKPAD_H

#include "TaskpadExt.h"

class CDefaultView;
class CLDVPTaskFS;

#define ID_TASK_CUSTOM 999
#define UWM_SELECTBYPOINT	WM_USER + 5
#define GROUP_TASK_TOKEN	_T("----")

#define TASK_GROUP_CONFIG_SNAP_IN	0x1000

class CTaskpad;

void AddTaskToMenu( CTaskpad *ptrTaskPad, CString strGroup, DWORD dwID, CString strTaskName );

//----------------------------------------------------------------
//
// CTaskPad class
//
//----------------------------------------------------------------
class CTaskpad : public CTaskpadExt
{
private:
	void GetConfigurationSnapIns();


// Construction
public:
	void ModifyCurrentTask( CString strTask, CString strDescription, BOOL bFireTask = TRUE );
	void GetSelTask( CString &strTaskname, CString &strGroup );
	void SelectTask( DWORD dwID, CString strCategory );
	void SelectConfigurationSnapInTask( WORD wID, WORD wGroupID );
	int m_ySize;
	CTaskpad();

// Operations
public:
	CTaskGroup *CreateGroup(LPCTSTR szGroup, BOOL bScanningTasks, long lImageID = 0 );
	CSubTask *CreateTask(LPCTSTR GroupName, LPCTSTR TaskName, LPCTSTR TaskDescript, long TaskID, GUID guid, long lImageID = 17, TaskType taskType = TASK_EXTERNAL );
    // Creates as task with an icon supplied from an external file, as specified
    CSubTask *CreateTask(LPCTSTR GroupName, LPCTSTR TaskName, LPCTSTR TaskDescript, long TaskID, GUID guid, CString iconFilename, UINT iconResourceID, TaskType taskType = TASK_EXTERNAL );    
	void GetExternalTasks();
	void GetCustomTaskInfo( CString strCategory, CString strTaskname, CString &strDescription, CString &strFilename, GUID &guid, BOOL bDeleteAfterGet = FALSE );
	BOOL SaveCustomTaskInfo( CString strCategory, CString &strTaskname, CString strDescription, CString strFilename, GUID guid );
	void FillLBScanTasks( CListBox *ptrList );
	BOOL GetScheduleInfo( CString &strTaskname, CString &strDescription, CString strTask, GUID ocxGUID );

	void InitGroups();
	void VerifyGroups();
	void ExpandScanningTasks();
	// 8-23-00 terrym the taskpad strings and the menu strings need to be different now due to translation issues.
	// the two strings are combine into one resource.  The first part is the menu string.  The second string is the
	// taskpad string.  The strings are seperated by a token '----' 
	// this function will parse out the raw resource string into the menu and taskpad components.
	void ConvertRawMenuString (CString sRaw, CString& sMenu, CString& sTaskPad)
				{
					int iTokenStart = sRaw.Find(GROUP_TASK_TOKEN);
					if (iTokenStart != -1)
					{
						sMenu = sRaw.Left(iTokenStart);
						sTaskPad = &sRaw.GetBuffer(0)[(iTokenStart+sizeof(GROUP_TASK_TOKEN))-1];
					}
					else
					{
						sMenu = sRaw;
						sTaskPad = sRaw;
					}
				}
	BOOL SelectItem( HTREEITEM item );

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTaskpad)
	//}}AFX_VIRTUAL

// Implementation
public:
	LRESULT SelectTaskByPoint( WPARAM wParam, LPARAM lParam );
	virtual ~CTaskpad();
	
	virtual	BOOL AddDefaultTasks();
	virtual BOOL ShowChildren( HTREEITEM item, BOOL bShow );
	virtual BOOL DrawTaskWindow( CDC* pDC );
	virtual BOOL SizeTaskWindow();
	void	RemoveScheduleInfo( const CString &strFilename );

	// Generated message map functions
protected:
	//{{AFX_MSG(CTaskpad)
	afx_msg void OnGetdispinfo(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDestroy();
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnItemexpanding(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void Ondblclk(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnSelChanged(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnBegindrag(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	//}}AFX_MSG

	virtual void DestroyTree( HTREEITEM = TVI_ROOT );

	HTREEITEM		m_displayedParent;
	CFont			m_normalFont;
	CFont			m_underlineFont;
	HTREEITEM		m_oldItem;
	CImageList		m_images;
	BOOL            m_bCanUpdateRegKey;

	DECLARE_MESSAGE_MAP()
private:
	BOOL StartSelectedTask( HTREEITEM item );
	BOOL		m_bDragging;
	HCURSOR		cursorNo,
				cursorOld;
	TV_ITEM		m_draggingItem;
	GUID		m_recordingGUID;
	DWORD		m_recordingTaskID;
	BOOL		m_bScanningComponentInstalled;

	void LoadTaskGroup( CString strGroupName, CString strRegName  );
	void LoadCustomTask( CSubTask *ptrTask );
	void HandleInternalTask( CSubTask *ptrTask, BOOL bDoubleClick );
	void FireTask( BOOL bDoubleClick );
	CTaskGroup *CreateGroupWithMenu( UINT iID, int iImageNum );

};

#endif /* ndef __TASKPAD_H */
