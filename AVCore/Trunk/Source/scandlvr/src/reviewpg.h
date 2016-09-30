/////////////////////////////////////////////////////////////////////////////
// $Header:   S:/SCANDLVR/VCS/Reviewpg.h_v   1.6   26 Jun 1998 15:08:56   SEDWARD  $
/////////////////////////////////////////////////////////////////////////////
//
// ReviewPg.h: interface for the CReviewPropertyPage class.
//
/////////////////////////////////////////////////////////////////////////////
// $Log:   S:/SCANDLVR/VCS/Reviewpg.h_v  $
// 
//    Rev 1.6   26 Jun 1998 15:08:56   SEDWARD
// Added 'OnSetActive' so we can refresh the summary text.
//
//    Rev 1.5   08 Jun 1998 23:26:28   SEDWARD
// Started working on 'SetSummaryText'.
//
//    Rev 1.4   27 Apr 1998 02:09:10   SEDWARD
// Added OnInitDialog().
//
//    Rev 1.2   24 Apr 1998 18:18:14   SEDWARD
// Added OnWizardNext()
//
//    Rev 1.1   20 Apr 1998 18:05:18   SEDWARD
// Added 'm_szSummary'.
/////////////////////////////////////////////////////////////////////////////


#if !defined(AFX_REVIEW_H__8D28F8F8_C3FA_11D1_A7C2_0000E8D34392__INCLUDED_)
#define AFX_REVIEW_H__8D28F8F8_C3FA_11D1_A7C2_0000E8D34392__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "WizPage.h"
#include "ScanDeliverDLL.h"


// define statements
#define FOUR_SPACE_TAB      _T("    ")
#define NEWLINE             _T("\n")


/////////////////////////////////////////////////////////////////////////////
// CReviewPropertyPage dialog

class CReviewPropertyPage : public CWizardPropertyPage
{
    DECLARE_DYNCREATE(CReviewPropertyPage)

// Construction
public:
    CReviewPropertyPage(CScanDeliverDLL*  pDLL);
    CReviewPropertyPage();
    ~CReviewPropertyPage();

// Dialog Data
    //{{AFX_DATA(CReviewPropertyPage)
	enum { IDD = IDD_REVIEW };
	CString	m_szSummary;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
    //{{AFX_VIRTUAL(CReviewPropertyPage)
	public:
	virtual LRESULT OnWizardNext();
	virtual BOOL OnSetActive();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
    //{{AFX_MSG(CReviewPropertyPage)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

    void    SetSummaryText(void);

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_REVIEW_H__8D28F8F8_C3FA_11D1_A7C2_0000E8D34392__INCLUDED_)