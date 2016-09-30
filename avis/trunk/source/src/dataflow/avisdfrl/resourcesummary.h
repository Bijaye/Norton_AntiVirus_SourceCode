#if !defined(AFX_RESOURCESUMMARY_H__4604A153_5A06_11D2_B974_0004ACEC31AA__INCLUDED_)
#define AFX_RESOURCESUMMARY_H__4604A153_5A06_11D2_B974_0004ACEC31AA__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// ResourceSummary.h : header file
//

#include "ListCtEx.h"
/////////////////////////////////////////////////////////////////////////////
// CResourceSummary dialog

class CResourceSummary : public CDialog
{
// Construction
public:
	CDWordArray m_TotalJobDoneArray;
	CUIntArray m_FreeCountArray;
	CUIntArray m_AvailableCountArray;
	CUIntArray m_TotalCountArray;
	CStringArray m_JobIDArray;
	CResourceSummary(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CResourceSummary)
	enum { IDD = IDD_DIALOG_RESOURCE_SUMMARY };
	CListCtrlEx	m_ResourceSummaryList;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CResourceSummary)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CResourceSummary)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:
	afx_msg LONG OnDFLauncherRefreshSummary(WPARAM w, LPARAM l);
    static int CALLBACK CompareFunc(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort);
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_RESOURCESUMMARY_H__4604A153_5A06_11D2_B974_0004ACEC31AA__INCLUDED_)
