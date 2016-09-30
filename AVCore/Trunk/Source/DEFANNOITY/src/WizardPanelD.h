#if !defined(AFX_WIZARDPANELD_H__54D4B942_BEAA_11D1_8767_00C04FB59E12__INCLUDED_)
#define AFX_WIZARDPANELD_H__54D4B942_BEAA_11D1_8767_00C04FB59E12__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// WizardPanelD.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CWizardPanelD dialog

class CWizardPanelD : public CPropertyPage
{
	DECLARE_DYNCREATE(CWizardPanelD)

// Construction
public:
	CWizardPanelD();
	~CWizardPanelD();

// Dialog Data
	//{{AFX_DATA(CWizardPanelD)
	enum { IDD = IDD_DIALOG8 };
	int		m_iButton;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CWizardPanelD)
	public:
	virtual LRESULT OnWizardNext();
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
	//{{AFX_MSG(CWizardPanelD)
	virtual BOOL OnInitDialog();
	afx_msg void OnYes();
	afx_msg void OnNo();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_WIZARDPANELD_H__54D4B942_BEAA_11D1_8767_00C04FB59E12__INCLUDED_)
