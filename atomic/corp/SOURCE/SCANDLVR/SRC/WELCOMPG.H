/////////////////////////////////////////////////////////////////////////////
// $Header:   S:/SCANDLVR/VCS/Welcompg.h_v   1.2   26 Apr 1998 17:30:02   SEDWARD  $
/////////////////////////////////////////////////////////////////////////////
//
// WelcomPg.h: interface for the CWelcomePropertyPage class.
//
/////////////////////////////////////////////////////////////////////////////
// $Log:   S:/SCANDLVR/VCS/Welcompg.h_v  $
// 
//    Rev 1.2   26 Apr 1998 17:30:02   SEDWARD
// Added GetTemplateID(), now derive from CWizardPropertyPage.
//
//    Rev 1.1   20 Apr 1998 20:58:18   SEDWARD
// Added OnKillActive() and OnSetActive().
//
//    Rev 1.0   20 Apr 1998 18:10:42   SEDWARD
// Initial revision.
/////////////////////////////////////////////////////////////////////////////


#if !defined(AFX_WELCOMPG_H__3392A3A3_D62B_11D1_A7F1_0000E8D34392__INCLUDED_)
#define AFX_WELCOMPG_H__3392A3A3_D62B_11D1_A7F1_0000E8D34392__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "WizPage.h"


/////////////////////////////////////////////////////////////////////////////
// CWelcomePropertyPage dialog

class CWelcomePropertyPage : public CWizardPropertyPage
{
    DECLARE_DYNCREATE(CWelcomePropertyPage)

// Construction
public:
    CWelcomePropertyPage();
    ~CWelcomePropertyPage();

// Dialog Data
    //{{AFX_DATA(CWelcomePropertyPage)
    enum { IDD = IDD_WELCOME };
        // NOTE - ClassWizard will add data members here.
        //    DO NOT EDIT what you see in these blocks of generated code !
    //}}AFX_DATA

    virtual UINT  GetTemplateID(void) { return  IDD; }

// Overrides
    // ClassWizard generate virtual function overrides
    //{{AFX_VIRTUAL(CWelcomePropertyPage)
	public:
	virtual BOOL OnKillActive();
	virtual BOOL OnSetActive();
	protected:
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
    // Generated message map functions
    //{{AFX_MSG(CWelcomePropertyPage)
        // NOTE: the ClassWizard will add member functions here
    //}}AFX_MSG
    DECLARE_MESSAGE_MAP()

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_WELCOMPG_H__3392A3A3_D62B_11D1_A7F1_0000E8D34392__INCLUDED_)