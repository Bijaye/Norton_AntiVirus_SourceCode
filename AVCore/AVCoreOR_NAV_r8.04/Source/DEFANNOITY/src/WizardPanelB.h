#if !defined(AFX_WIZARDPANELB_H__54D4B940_BEAA_11D1_8767_00C04FB59E12__INCLUDED_)
#define AFX_WIZARDPANELB_H__54D4B940_BEAA_11D1_8767_00C04FB59E12__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// WizardPanelB.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CWizardPanelB dialog

class CWizardPanelB : public CPropertyPage
{
	DECLARE_DYNCREATE(CWizardPanelB)

// Construction
public:
	CWizardPanelB();
	~CWizardPanelB();

// Dialog Data
	//{{AFX_DATA(CWizardPanelB)
	enum { IDD = IDD_DIALOG11 };
	int		m_iButton;
	CString	m_strConfirmation;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CWizardPanelB)
	public:
	virtual LRESULT OnWizardNext();
	virtual LRESULT OnWizardBack();
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
	//{{AFX_MSG(CWizardPanelB)
	virtual BOOL OnInitDialog();
	afx_msg void OnNumberIs();
	afx_msg void OnNoNumber();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_WIZARDPANELB_H__54D4B940_BEAA_11D1_8767_00C04FB59E12__INCLUDED_)
