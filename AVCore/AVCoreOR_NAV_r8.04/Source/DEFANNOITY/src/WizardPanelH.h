#if !defined(AFX_WIZARDPANELH_H__23B1F8C1_BF75_11D1_8767_00C04FB59E12__INCLUDED_)
#define AFX_WIZARDPANELH_H__23B1F8C1_BF75_11D1_8767_00C04FB59E12__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// WizardPanelH.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// WizardPanelH dialog

class WizardPanelH : public CPropertyPage
{
	DECLARE_DYNCREATE(WizardPanelH)

// Construction
public:
	WizardPanelH();
	~WizardPanelH();

// Dialog Data
	//{{AFX_DATA(WizardPanelH)
	enum { IDD = IDD_DIALOG14 };
		// NOTE - ClassWizard will add data members here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(WizardPanelH)
	public:
	virtual BOOL OnSetActive();
	virtual LRESULT OnWizardBack();
    virtual BOOL OnWizardFinish();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

public:
// Additional Functions
	inline void SetParentInfo(CPropertySheet *pParent, CString *pString = NULL, CFont *pFont = NULL)
	{	m_pParent = pParent; if(pString) m_csTitle = *pString;	};

// Implementation
protected:
	CFont m_Font;
	CPropertySheet *m_pParent;
	CString m_csTitle;

	// Generated message map functions
	//{{AFX_MSG(WizardPanelH)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_WIZARDPANELH_H__23B1F8C1_BF75_11D1_8767_00C04FB59E12__INCLUDED_)
