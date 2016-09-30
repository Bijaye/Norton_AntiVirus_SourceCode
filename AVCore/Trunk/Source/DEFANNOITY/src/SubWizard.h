#if !defined(AFX_SUBWIZARD_H__ACC4C601_BE90_11D1_8767_00C04FB59E12__INCLUDED_)
#define AFX_SUBWIZARD_H__ACC4C601_BE90_11D1_8767_00C04FB59E12__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "WizardPanelA.h"
#include "WizardPanelBPre.h"
#include "WizardPanelB.h"
#include "WizardPanelC.h"
#include "WizardPanelD.h"
#include "WizardPanelE.h"
#include "WizardPanelF.h"
#include "WizardPanelG.h"
#include "WizardPanelH.h"
#include "WizardPanelI.h"
#include "WizardPanelJ.h"
// SubWizard.h : header file
//

enum SUB_STATE { SUB_FREEWARN = 0, SUB_FREEEXP, SUB_PAIDWARN, SUB_PAIDEXP };

void AdjustHeaderFont(CWnd *pHeaderWnd, CFont *pHeaderFont);

/////////////////////////////////////////////////////////////////////////////
// CSubWizard

class CSubWizard : public CPropertySheet
{
	DECLARE_DYNAMIC(CSubWizard)

// Construction
public:
	CSubWizard(UINT nIDCaption, CWnd* pParentWnd = NULL, UINT iSelectPage = 0);
	CSubWizard(LPCTSTR pszCaption, CWnd* pParentWnd = NULL, UINT iSelectPage = 0);

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSubWizard)
	public:
	virtual BOOL OnInitDialog();
	//}}AFX_VIRTUAL

// Implementation
public:
	void SetState(SUB_STATE s);
	inline SUB_STATE GetState() { return state; };
	inline void SetLastPanel(CPropertyPage *pPage)
	{	last_panel = GetPageIndex(pPage); };
	inline void GotoLastPanel()
	{	SetActivePage(last_panel); };
	inline void SetRemaining(int iRem) {	m_iRemaining = iRem; };
	inline int GetRemaining() { return m_iRemaining; };
    inline void SetConfirmationNumberUI ( BOOL bRequireConfirmationNumber ) { m_bConfirmationNumber = bRequireConfirmationNumber; };
	inline void SetResult(SUB_RESULT iRes) {	m_result = iRes; };
	inline SUB_RESULT GetResult() { return m_result; };

protected:
	BOOL m_bConfirmationNumber;
	int   m_iRemaining;
	SUB_RESULT m_result;
	CWizardPanelA panelA;
	CWizardPanelBPre panelBPre;
	CWizardPanelB panelB;
	CWizardPanelC panelC;
	CWizardPanelD panelD;
	CWizardPanelE panelE;
	CWizardPanelF panelF;
	WizardPanelG  panelG;
	WizardPanelH  panelH;
	WizardPanelI  panelI;
	CWizardPanelJ panelJ;
	SUB_STATE state;
	int last_panel;

	// Generated message map functions
	//{{AFX_MSG(CSubWizard)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SUBWIZARD_H__ACC4C601_BE90_11D1_8767_00C04FB59E12__INCLUDED_)
