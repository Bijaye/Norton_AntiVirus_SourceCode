#if !defined(AFX_WIZARDPANELJ_H__54D4B940_BEAA_11D1_8767_00C04FB59E12__INCLUDED_)
#define AFX_WIZARDPANELJ_H__54D4B940_BEAA_11D1_8767_00C04FB59E12__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// WizardPanelJ.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CWizardPanelJ dialog

class CWizardPanelJ : public CPropertyPage
{
	DECLARE_DYNCREATE(CWizardPanelJ)

// Construction
public:
	CWizardPanelJ();
	~CWizardPanelJ();

// Dialog Data
	//{{AFX_DATA(CWizardPanelJ)
	enum { IDD = IDD_DIALOG16 };
	int		m_iButton;
//	CString	m_strConfirmation;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CWizardPanelJ)
	public:
	virtual LRESULT OnWizardNext();
	virtual LRESULT OnWizardBack();
    virtual BOOL OnWizardFinish();
	virtual BOOL OnSetActive();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

public:
// Additional Functions
	inline void SetParentInfo(CPropertySheet *pParent, CString *pString = NULL, CFont *pFont = NULL)
	{	m_pParent = pParent; if(pString) m_csTitle = *pString;	};
	void MySetWizardButtons();

// Implementation
protected:
	CFont m_Font;
	CPropertySheet *m_pParent;
	CString m_csTitle;

	// Generated message map functions
	//{{AFX_MSG(CWizardPanelJ)
	virtual BOOL OnInitDialog();
	afx_msg void OnInternet();
	afx_msg void OnSymantec();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_WIZARDPANELJ_H__54D4B940_BEAA_11D1_8767_00C04FB59E12__INCLUDED_)
