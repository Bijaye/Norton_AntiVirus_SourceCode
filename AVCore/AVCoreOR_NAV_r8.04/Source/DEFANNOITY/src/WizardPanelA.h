#if !defined(AFX_WIZARDPANELA_H__ACC4C600_BE90_11D1_8767_00C04FB59E12__INCLUDED_)
#define AFX_WIZARDPANELA_H__ACC4C600_BE90_11D1_8767_00C04FB59E12__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// WizardPanelA.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CWizardPanelA dialog

class CWizardPanelA : public CPropertyPage
{
	DECLARE_DYNCREATE(CWizardPanelA)

// Construction
public:
	CWizardPanelA();
	~CWizardPanelA();

// Dialog Data
	//{{AFX_DATA(CWizardPanelA)
	enum { IDD = IDD_DIALOG7 };
	int		m_iButton;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CWizardPanelA)
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
	//{{AFX_MSG(CWizardPanelA)
	virtual BOOL OnInitDialog();
	afx_msg void OnYes();
	afx_msg void OnNo();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_WIZARDPANELA_H__ACC4C600_BE90_11D1_8767_00C04FB59E12__INCLUDED_)
