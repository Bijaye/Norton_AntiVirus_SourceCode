#if !defined(AFX_WIZARDPANELBPRE_H__54D4B940_BEAA_11D1_8767_00C04FB59E12__INCLUDED_)
#define AFX_WIZARDPANELBPRE_H__54D4B940_BEAA_11D1_8767_00C04FB59E12__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// WizardPanelBPre.h : header file
//

#include "HyperLink.h"

/////////////////////////////////////////////////////////////////////////////
// CWizardPanelBPre dialog
class CWizardPanelBPre : public CPropertyPage
{
	DECLARE_DYNCREATE(CWizardPanelBPre)

// Construction
public:
	CWizardPanelBPre();
	~CWizardPanelBPre();

// Dialog Data
	//{{AFX_DATA(CWizardPanelBPre)
	enum { IDD = IDD_DIALOG11PRE };
	CHyperLink	m_ddxInterLink;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CWizardPanelBPre)
	public:
	virtual LRESULT OnWizardNext();
	virtual LRESULT OnWizardBack();
	virtual BOOL OnSetActive();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

public:
	afx_msg LONG OnFixUpWizardButtons ( UINT, LONG );
// Additional Functions
	inline void SetParentInfo(CPropertySheet *pParent, CString *pString = NULL, CFont *pFont = NULL)
	{	m_pParent = pParent; if(pString) m_csTitle = *pString;	};

// Implementation
protected:
	CFont m_Font;
	CPropertySheet *m_pParent;
	CString m_csTitle;

	// Generated message map functions
	//{{AFX_MSG(CWizardPanelBPre)
	virtual BOOL OnInitDialog();
	afx_msg void OnWorldwide();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_WIZARDPANELBPRE_H__54D4B940_BEAA_11D1_8767_00C04FB59E12__INCLUDED_)
