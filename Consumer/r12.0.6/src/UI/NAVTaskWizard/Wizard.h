// Wizard.h : header file
//

#if !defined(AFX_WIZARD_H__1F835761_588D_11D2_96F6_00C04FAC114C__INCLUDED_)
#define AFX_WIZARD_H__1F835761_588D_11D2_96F6_00C04FAC114C__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

/////////////////////////////////////////////////////////////////////////////
// CTaskWizard -- The scan task wizard.

class CTaskWizard : public CPropertySheet
{
	DECLARE_DYNAMIC(CTaskWizard)

// Construction
public:
    CTaskWizard (IScanTask* pScanTask, int iMode, UINT nIDCaption, CWnd* pParentWnd = NULL, UINT iSelectPage = 0 );

// Attributes
public:
    CTWIntroPage            m_IntroPg;
    CTWSelectItemsPage      m_SelectPg;
    CTWSaveTaskPage         m_SavePg;
    CTWSaveEditedTaskPage   m_SaveEditedPg;

    IScanTask*              m_pScanTask;

    enum modes 
    {
        modeCreate = 0,
        modeEdit,
        modeDuplicateName
    };

	CString m_csTitle;

// Operations
public:
	IScanTask* GetScanTask(void);
	bool EditingTask(void) const { return (m_iMode == modeEdit); }

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTaskWizard)
	public:
	virtual int DoModal();
	//}}AFX_VIRTUAL

// Implementation
public:
	bool IsEditing ();
	virtual ~CTaskWizard();


    // Generated message map functions
protected:
    int m_iMode;
	//{{AFX_MSG(CTaskWizard)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_WIZARD_H__1F835761_588D_11D2_96F6_00C04FAC114C__INCLUDED_)
