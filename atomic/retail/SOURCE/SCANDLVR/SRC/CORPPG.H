/////////////////////////////////////////////////////////////////////////////
// $Header:   S:/SCANDLVR/VCS/CorpPg.h_v   1.4   07 Jul 1998 18:20:52   sedward  $
/////////////////////////////////////////////////////////////////////////////
//
// CorpPg.h: interface for the CorpInfoPropertyPage class.
//
/////////////////////////////////////////////////////////////////////////////
// $Log:   S:/SCANDLVR/VCS/CorpPg.h_v  $
// 
//    Rev 1.4   07 Jul 1998 18:20:52   sedward
// Nuked 'ReleaseListCtrlMemory' (we're no longer allocating those, the control
// does it for us), plus some general cleanup.
//
//    Rev 1.3   08 Jun 1998 16:18:06   SEDWARD
// Added support for a list control that displays target platforms for defs.
//
//    Rev 1.2   26 Apr 1998 17:25:32   SEDWARD
// Added GetTemplateID().
//
//    Rev 1.1   20 Apr 1998 18:04:56   SEDWARD
// Added support for new UI.
/////////////////////////////////////////////////////////////////////////////


#if !defined(AFX_CORP_PG_H__8D28F8F6_C3FA_11D1_A7C2_0000E8D34392__INCLUDED_)
#define AFX_CORP_PG_H__8D28F8F6_C3FA_11D1_A7C2_0000E8D34392__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "WizPage.h"

/////////////////////////////////////////////////////////////////////////////
// CorpInfoPropertyPage dialog

class CorpInfoPropertyPage : public CWizardPropertyPage
{
    DECLARE_DYNCREATE(CorpInfoPropertyPage)

// Construction
public:
    CorpInfoPropertyPage();
    ~CorpInfoPropertyPage();

// Dialog Data
    //{{AFX_DATA(CorpInfoPropertyPage)
	enum { IDD = IDD_CORP_INFO };
	CListCtrl	m_listCtrlPlatforms;
	CString	m_szPlatGoldNumber;
	//}}AFX_DATA

    virtual UINT  GetTemplateID(void) { return  IDD; }


// Overrides
	// ClassWizard generate virtual function overrides
    //{{AFX_VIRTUAL(CorpInfoPropertyPage)
	public:
	virtual LRESULT OnWizardBack();
	virtual LRESULT OnWizardNext();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	virtual BOOL UpdateConfigData(void);
    void GetOsCheckboxes(DWORD*  dwOS);
    void SetOsCheckboxes(DWORD  dwOS);
	// Generated message map functions
    //{{AFX_MSG(CorpInfoPropertyPage)
	virtual BOOL OnInitDialog();
	afx_msg void OnClickListctrlPlatforms(NMHDR* pNMHDR, LRESULT* pResult);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

    CImageList      m_imageList;

    int     AddPlatformItemToListCtrl(CString&  szPlatform, CString&  szPlatformData);
    int     PopulateListCtrl(void);

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CORP_PG_H__8D28F8F6_C3FA_11D1_A7C2_0000E8D34392__INCLUDED_)