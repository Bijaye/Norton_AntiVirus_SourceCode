// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright (c) 2005 Symantec Corporation. All rights reserved.
//----------------------------------------------------------------
//
//  File: TaskpadExt.h
//  Purpose: CTaskPadExt Definition file
//
//	Date: 2-3-97
//
//	Owner: Ken Knapton
//
//	***** Intel Corp. Confidential ******
//
//----------------------------------------------------------------

#ifndef __TASKPADEXT_H
#define __TASKPADEXT_H

class CTaskpadExt;

enum TaskType { TASK_INTERNAL, TASK_EXTERNAL, TASK_HELP, TASK_RECORDABLE };

//----------------------------------------------------------------
//
// CTaskpadObject class
//
//----------------------------------------------------------------
class CTaskpadObject : public CObject
{
	DECLARE_DYNAMIC( CTaskpadObject );

public:
	CTaskpadObject();
	CTaskpadObject( const CTaskpadObject& object );
	~CTaskpadObject();

	virtual CTaskpadObject& operator=( const CTaskpadObject& object );
	
	HBITMAP			m_hbitmap;
	CString			m_text;
	HTREEITEM		m_handle;
	int				m_image;
	CTaskpadExt*	m_pTaskpad;

protected:
	virtual void copy( const CTaskpadObject& );
};

//----------------------------------------------------------------
//
// CTaskGroup class
//
//----------------------------------------------------------------
class CTaskGroup : public CTaskpadObject
{
	DECLARE_DYNAMIC( CTaskGroup );
private:
	BOOL bCanDragDropChildren;

public:
	CTaskGroup();
	CTaskGroup( const CTaskGroup& );
	~CTaskGroup();

	BOOL	m_bScanTaskGroup;

	virtual CTaskGroup& operator=( const CTaskGroup& group );
	void SetCanDragDropChildren( BOOL bState ){ bCanDragDropChildren = bState; };
	BOOL CanDragDropChildren(){ return bCanDragDropChildren; };
};

//----------------------------------------------------------------
//
// CSubTask class
//
//----------------------------------------------------------------
class CSubTask : public CTaskpadObject
{

	DECLARE_DYNAMIC( CSubTask );

public:
	CRect m_textRect;
	CSubTask();
	CSubTask( const CSubTask& );
	~CSubTask();

	virtual CSubTask& operator=( const CSubTask& task );
	virtual BOOL DrawTask( UINT x, UINT* y, UINT cxMax, 
		CDC* pDC, CFont* pHeaderFont, 
		CFont* pDescriptionFont, CImageList * ptrList );

	DWORD		m_identifier;
	CString		m_description;
	CString		m_strGroup;
	TaskType	m_dwTaskType;
	GUID		m_guid;
	DWORD		m_recordType;
	CString		m_strInternalName;

protected:
	virtual void copy( const CSubTask& );
};

//----------------------------------------------------------------
//
// CTaskpadExt class
//
//----------------------------------------------------------------
class CTaskpadExt : public CTreeCtrl
{
private:
		WORD	m_wGroupID;

// Construction
public:
	CTaskpadExt();
	CMapWordToOb	m_GroupIds;

// Attributes
public:

// Operations
public:
	virtual CTaskGroup *AddTaskGroup( CTaskGroup* pGroup, BOOL bRootItem  = FALSE  );
	virtual CSubTask* AddSubTask( CTaskGroup*, CSubTask* );

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTaskpadExt)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CTaskpadExt();

	// Generated message map functions
protected:
	//{{AFX_MSG(CTaskpadExt)
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};


#endif /* __TASKPADEXT_H */
