// TWSelectItemsPage.h : header file
//

#if !defined(AFX_TWSELECTITEMSPAGE_H__8AD9EAA7_5854_11D2_96F6_00C04FAC114C__INCLUDED_)
#define AFX_TWSELECTITEMSPAGE_H__8AD9EAA7_5854_11D2_96F6_00C04FAC114C__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "picture.h"
#include "wizardpg.h"
#include "xlistctrl.h"

/////////////////////////////////////////////////////////////////////////////
// CTWSelectItemsPage dialog -- The scan task wizard page where the user
// selects the items to put in the task.

class CTWSelectItemsPage : public CBaseWizPage
{
	DECLARE_DYNCREATE(CTWSelectItemsPage)

// Construction
public:
	CTWSelectItemsPage();
	~CTWSelectItemsPage();

// Dialog Data
	//{{AFX_DATA(CTWSelectItemsPage)
	//enum { IDD = IDD_TW_SELECTITEMS };
	CXListCtrl	m_ItemList;
	CPicture	m_Picture;
	//}}AFX_DATA

    //CScanTask* m_pTask;
    BOOL       m_bEditingTask;

// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CTWSelectItemsPage)
	public:
	virtual BOOL OnSetActive();
	virtual LRESULT OnWizardNext();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
    CImageList m_ImgList;

    //void FillTaskList();
    int  AddScanItem ( CScanItem& Item );
    void RemoveScanItem ( int nItemIndex );
    void SetScanItemData ( int nItemIndex, const CScanItem& Item );
    void UpdateControls(void);
    void AddScanItems(void);

    int  m_nNextDriveIndex;
    int  m_nNextFolderIndex;
    int  m_nNextFileIndex;

	// Generated message map functions
	//{{AFX_MSG(CTWSelectItemsPage)
	virtual BOOL OnInitDialog();
	afx_msg void OnDestroy();
	afx_msg void OnItemchangedItemlist(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnScanDrive();
	afx_msg void OnScanFolder();
	afx_msg void OnScanFile();
	afx_msg void OnRemove();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TWSELECTITEMSPAGE_H__8AD9EAA7_5854_11D2_96F6_00C04FAC114C__INCLUDED_)
