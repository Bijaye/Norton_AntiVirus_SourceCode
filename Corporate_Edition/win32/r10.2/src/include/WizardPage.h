// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright (c) 2005 Symantec Corporation. All rights reserved.
//----------------------------------------------------------------
//
//  File: WizardPage.h
//  Purpose: CWizardPage Definition file. 
//				Base class for Task wizard pages
//
//	Date: 1-20-97
//
//	Owner: Ken Knapton
//
//	***** Intel Corp. Confidential ******
//
//----------------------------------------------------------------

#if !defined(WIZARD_PAGE_INCLUDED)
#define WIZARD_PAGE_INCLUDED

#define SCAN_DLGS_DYNAMIC
#include "ScnDlgEx.h"

#include "ClientPropSheet.h"
#include "ConfigObj.h"
#include "MscFuncs.h"

//Forward define the Actions class
class CLDVPActions;

extern tCreateResultsView CreateResultsView;
extern tAddLogLineToResultsView AddLogLineToResultsView;

//----------------------------------------------------------------
//
// CWizardPage class
//
//----------------------------------------------------------------
class CWizardPage : public CPropertyPage
{
	DECLARE_DYNCREATE(CWizardPage)

// Construction
public:
	CWizardPage();
	CWizardPage( int iID );
	~CWizardPage();

	//This is used ONLY for the configuration
	//	pages, and ONLY when modal!!!
	CString		m_strTaskName;
	
	virtual void SizePage( int cx, int cy );
	virtual void OnMscButton();

// Dialog Data
	//{{AFX_DATA(CWizardPage)
		// NOTE - ClassWizard will add data members here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CWizardPage)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	void EnableAutoscan( CButton *ptrOnOff, CLDVPActions *pActions, int iControls[], int iCount );
	// Generated message map functions
	//{{AFX_MSG(CWizardPage)
	//}}AFX_MSG
	afx_msg void OnDestroying();

	//Assume an addref by the reciever!!!!!
	// NOTE: This is not COM, BUT THIS METHOD IS NOT EXPOSED, so I can
	//			follow my own rules.
	IConfig	*GetScanConfig(){ return ((CClientPropSheet*)GetParent())->GetScanConfig(); };
	//Method for getting an IVPStorage
	IVPStorage *GetStorage();

	virtual void PaintTitle( CDC* pDC );
	BOOL OnSetActive();

	BOOL InWizardMode(){ return ((CClientPropSheet*)GetParent())->IsWizard(); };

	DECLARE_MESSAGE_MAP()

private:
	CFont		m_titleFont;
};


#endif

/*------------------------------------------------------------------
*
* $History: $
*
*
  ------------------------------------------------------------------*/
