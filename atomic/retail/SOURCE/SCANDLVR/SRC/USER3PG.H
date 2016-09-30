/////////////////////////////////////////////////////////////////////////////
// $Header:   S:/SCANDLVR/VCS/User3Pg.h_v   1.4   01 Jun 1998 17:06:16   SEDWARD  $
/////////////////////////////////////////////////////////////////////////////
//
// User3Pg.h: interface for the CUserInfo3PropertyPage class.
//
/////////////////////////////////////////////////////////////////////////////
// $Log:   S:/SCANDLVR/VCS/User3Pg.h_v  $
// 
//    Rev 1.4   01 Jun 1998 17:06:16   SEDWARD
// Added support for the country drop-down listbox.
//
//    Rev 1.3   19 May 1998 19:44:58   SEDWARD
// Added code so a message appears if the user selects the custom SMTP server
// checkbox but leaves the corresponding field blank.
//
//    Rev 1.2   26 Apr 1998 17:25:42   SEDWARD
// Added GetTemplateID().
//
//    Rev 1.1   24 Apr 1998 18:23:00   SEDWARD
// Added support for "strip content" radio buttons and custom SMTP server.
//
//    Rev 1.0   20 Apr 1998 16:26:12   SEDWARD
// Initial revision.
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_USER3PG_H__3392A3A2_D62B_11D1_A7F1_0000E8D34392__INCLUDED_)
#define AFX_USER3PG_H__3392A3A2_D62B_11D1_A7F1_0000E8D34392__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "WizPage.h"



/////////////////////////////////////////////////////////////////////////////
// CUserInfo3PropertyPage dialog

class CUserInfo3PropertyPage : public CWizardPropertyPage
{
    DECLARE_DYNCREATE(CUserInfo3PropertyPage)

// Construction
public:
    CUserInfo3PropertyPage();
    ~CUserInfo3PropertyPage();

// Dialog Data
    //{{AFX_DATA(CUserInfo3PropertyPage)
    enum { IDD = IDD_USER_INFO3 };
    CString m_szSmtpServer;
    BOOL    m_checkboxCustomSmtpServer;
    CString m_szSymptoms;
    CString m_szCountryOfInfection;
    //}}AFX_DATA

    virtual UINT  GetTemplateID(void) { return  IDD; }


// Overrides
    // ClassWizard generate virtual function overrides
    //{{AFX_VIRTUAL(CUserInfo3PropertyPage)
    public:
    virtual LRESULT OnWizardBack();
    virtual LRESULT OnWizardNext();
    protected:
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
    //}}AFX_VIRTUAL

// Implementation
protected:
    virtual BOOL UpdateConfigData(void);
    // Generated message map functions
    //{{AFX_MSG(CUserInfo3PropertyPage)
    virtual BOOL OnInitDialog();
	afx_msg void OnCheckCustomServer();
	//}}AFX_MSG
    DECLARE_MESSAGE_MAP()

    BOOL    GetStripFileContentRadioButtonValue(void);
    void    InitializeStripFileContentRadioButtons(void);
    void    GetSmtpServerFromUI(void);
    void    InitializeSmtpServerControls(void);
    BOOL    RequiredFieldsFilled(void);

    void    UpdateInfectionCountryCode(void);

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_USER3PG_H__3392A3A2_D62B_11D1_A7F1_0000E8D34392__INCLUDED_)